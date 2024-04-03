/*
 * <disk_info.c>
 *
 * Copyright (C) 2000 Daniel Kobras
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * $Id: timeout.c,v 1.1.1.1 2006-09-14 22:54:14 jmaggard Exp $
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "timeout.h"
#include "bug.h"

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

struct tolist_s {
	tolist_t	next;
	tolist_t	prev;
	int             timeout;
};

static tolist_t default_to_head=NULL;

static tolist_t append_timeout(tolist_t head, int timeout)
{
	tolist_t entry = malloc(sizeof(struct tolist_s));
	
	if (!entry)
		return NULL;

	entry->timeout = timeout;
	
	if (!head) {
		entry->next = entry->prev = entry;
	} else {
		entry->next = head;
		entry->prev = head->prev;
		head->prev->next = entry;
		head->prev = entry;
	}
	
	return entry;
}

static void free_timeouts(tolist_t head)
{
	tolist_t tmp, t;

	if (!head)
		return;
	
	t = head;
	
	do {
		tmp = t;
		t = t->next;
		free(tmp);

	} while (t != head);
}
			
void timeout_advance_default(void)
{
	if (default_to_head)
		default_to_head=default_to_head->next;
}

/* If timeout was previously 'skip', this routine returns the new timeout
 * value automatically as long as 'skip' corresponds to timeout==0!
 */
#if NFD_TO_SKIP != 0
#error Bummer. NFD_TO_SKIP changed and broke timeout_advance.
#endif
int timeout_advance(tolist_t *head)
{
	int to_old, to_new;
	
	if (!(*head))
		return default_to_head->timeout-default_to_head->prev->timeout;
	
	to_old = (*head)->timeout;
	if (to_old == NFD_TO_USE_DEFAULT)
		to_old = default_to_head->prev->timeout;

	*head = (*head)->next;
	
	to_new = (*head)->timeout;
	if (to_new == NFD_TO_USE_DEFAULT)
		to_new = default_to_head->timeout;

	return to_new - to_old;
}

int timeout_get(tolist_t head)
{
	if (head && head->timeout != NFD_TO_USE_DEFAULT)
		return head->timeout;
	
	/* Return default timeout */

	if (!default_to_head)
		return -1;	/* Error: no default timeout! */
	
	return default_to_head->timeout;
}

/* Take a comma separated list of timeout values and store them in
 * a tolist.
 */
tolist_t timeout_parse(tolist_t head, char *args, int is_default)
{
	char *cur, *tmp;
	int timeout;

	tmp = strdup(args);
	
	do {
		cur = strrchr(tmp, ',');
		if (cur) 
			*cur++ = '\0'; 
		else
			cur = tmp;
		
		if (!strcmp(cur, "default"))
			timeout = NFD_TO_USE_DEFAULT;
		else if (!strcmp(cur, "skip"))
			timeout = NFD_TO_SKIP;
		else 
			timeout = atoi(cur);
		
		if (timeout > 60*timeout && timeout != NFD_TO_USE_DEFAULT) {
			ERR("Illegal timeout value %d", timeout);
			goto err;
		}
		if (is_default && timeout == NFD_TO_USE_DEFAULT) {
			ERR("Illegal default timeout %d", timeout);
			goto err;
		}
		if (NFD_TO_IS_REGULAR(timeout)) 
			timeout *= 60;
		
		head = append_timeout(head, timeout);

	} while (cur != tmp);

	if (is_default)
		default_to_head=head;

	free(tmp);
	return head;
err:
	free_timeouts(head);
	free(tmp);
	return NULL;
}

