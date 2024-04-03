/*
 * <intr_stat.c>
 *
 * Copyright (C) 2001 Laurent Pelecq
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
 * $Id: intr_stat.c,v 1.1.1.1 2006-09-14 22:54:14 jmaggard Exp $
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "noflushd.h"
#include "bug.h"
#include "util.h"
#include "intr_stat.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <time.h>

#define UNKNOWN_INTERRUPT	"unknown"

#define LIST_DELIMITERS		",;"

/* FIXME I'd rather use args like "idle"/"active" to a single script to limit
 * footprint on disk caches. Even better something like:
 * /etc/noflushd/notify /dev/hda spinup, or .../notify irq idle.
 */
#define CMD_IF_IDLE		"/etc/noflushd/do_idle"
#define CMD_IF_ACTIVE		"/etc/noflushd/do_active"

typedef struct irq_stat_s_t *irq_stat_t;

struct irq_stat_s_t {
	int nr;				/* Irq line */
	long unsigned int count;	/* Stat counter */
	int active;			/* True if it should be monitored */
	char *name;			/* Name as in /proc/interrupts */
	irq_stat_t next;
};

static struct {
	irq_stat_t irq_head;	/* Linked list of irq line structs */
	int timeout;		/* Idle timeout for inactivity */
	int is_idle;		/* Inactivity flag */
	unsigned int nb_watch;	/* Number of irq lines watched */
	time_t stamp;		/* last modification */
} global;

/* Query for irq line n in irq list. Returns NULL on failure, irq entry
 * on success. */
static irq_stat_t get_irq(irq_stat_t head, int n)
{
	while (head) {
		if (head->nr == n)
			break;
		head = head->next;
	}
	return head;
}

/* Allocate new entry for irq line n. Returns new list head on success,
 * NULL on failure.
 */
static irq_stat_t alloc_irq(irq_stat_t head, int n)
{
	irq_stat_t irq;

	/* Sanity check. Don't do anything if irq already present. */
	irq = get_irq(head, n);
	if (irq) {
		DEBUG("Double alloc of irq #%d", n);
		return NULL;
	}

	irq = malloc(sizeof(*irq));
	if (!irq) {
		ERR("No mem for irq stat");
		return NULL;
	}

	irq->nr = n;
	irq->count = 0;
	irq->active = 0;
	irq->name = NULL;
	irq->next = head;

	return irq;
}
		

static irq_stat_t add_interrupt(irq_stat_t head, int n,
		                unsigned long int count, const char *name)
{
	irq_stat_t irq;
	
	if (!(irq = alloc_irq(head, n)))
		return NULL;
	
	irq->count = count;
	irq->name = strdup(name);
	if (!irq->name)
		irq->name = UNKNOWN_INTERRUPT;

	return irq;
}

static int parse_interrupts(time_t now)
{
	FILE *stat;
	char *line, *entry, *tmp;
	irq_stat_t irq;
	int ret = 1;
	unsigned int lineno = 0;

	stat = fopen("/proc/interrupts", "r");
	if (!stat)
		return 0;

	while ((line=get_line(stat))) {
		int n, nr;
		unsigned long int count;

		lineno++;
		
		entry = get_entry(line, 0);
		if (!entry || !isdigit(*entry)) {
			release_entry(entry);
			goto next_line;
		}

		nr = atoi(entry);

		release_entry(entry);

		n=1;
		count=0;
		
		while ((entry=get_entry(line, n++))) {
			if (!isdigit(*entry)) {
				break;
			}

			count += atol(entry);
			release_entry(entry);
		}

		if (!entry) {
			ERR("Weird line #%u in /proc/interrupts", lineno);
			ret = 0;
			goto err;
		}

		/* Argh!  The layout of /proc/interrupts differs across
		 * architectures.  Some contain the IRQ type in the
		 * next-to-last field, some don't.
		 */
		if ((tmp = get_remains(line, n))) {
			release_entry(entry);
			entry = tmp;
		}

		irq = get_irq(global.irq_head, nr);
		if (irq) {
			if (irq->active && irq->count != count) {
				DEBUG("Updated irq #%d %12lu %s",
						nr, count, entry);
				global.stamp = now;
			}
			irq->count = count;
		} else
			if ((irq = add_interrupt(global.irq_head, nr,
			                         count, entry)))
				 global.irq_head = irq;
		
		release_remains(entry);
next_line:
		release_line(line);
	}
err:
	fclose(stat);
	return ret;
}

/* Initialize interrupts monitoring */
int intr_stat_init(void)
{
	time_t now = time(NULL);
	global.is_idle = 0;
	global.timeout = 0;
	global.nb_watch = 0;
	global.stamp = now;
	global.irq_head = NULL;
	return parse_interrupts(now);
}

void intr_set_timeout(int timeout)
{
	global.timeout = timeout;
}

/* Release allocated resources */
void intr_stat_release(void)
{
	irq_stat_t irq = global.irq_head;
	
	while (irq) {
		irq_stat_t tmp = irq->next;
		if (irq->name && strcmp(irq->name, UNKNOWN_INTERRUPT))
			free(irq->name);
		free(irq);
		irq = tmp;
	}
}

/* Register a given interrupt to monitor it */
int intr_stat_register(int n)
{
	irq_stat_t irq = get_irq(global.irq_head, n);
	if (!irq) {
		ERR("No irq line #%d found", n);
		return 0;
	}
	irq->active = 1;
	global.nb_watch++;
	DEBUG("Monitoring interrupt #%d \"%s\"", n,
	     irq->name ? irq->name : UNKNOWN_INTERRUPT);
	return 1;
}

/* Register interrupts given as a comma separated list of interrupt
   ids */
int intr_stat_register_byids(const char *ids)
{
	char *buffer = strdup(ids);
	char *persist = NULL;
	const char *p = NULL;
	int status = 1;

	if (!buffer)
		return 0;

	for (p = strtok_r(buffer, LIST_DELIMITERS, &persist);
	     p; p = strtok_r(NULL, LIST_DELIMITERS, &persist)) {
		if (isdigit(*p)) {
			status = intr_stat_register(atoi(p));
		} else {
			irq_stat_t irq = global.irq_head;
			while (irq) {
				if (!strcmp(irq->name, p)) {
					status = intr_stat_register(irq->nr);
					break;
				}
				irq = irq->next;
			}
					
		}
	}
	free(buffer);
	return status;
}

/* Return 1 if not idle */
int intr_stat_check_idleness(void)
{
	int is_idle;
	char *cmd;
	time_t now = time(NULL);
	
	/* No irqs watched -> we're always idle */
	if (!global.nb_watch)
		return 1;
	
	if (!parse_interrupts(now))
		BUG("Failed updating irq stats");
		
	is_idle = (now - global.stamp > global.timeout);
	
	if (global.is_idle == is_idle)
		return is_idle;
		
	cmd = (is_idle) ? CMD_IF_IDLE : CMD_IF_ACTIVE;

	DEBUG("state is %s", (is_idle) ? "idle" : "active");
	
	if (access(cmd, X_OK) == 0 && system(cmd) < 0) {
		ERR("cannot execute %s", cmd);
		return global.is_idle;	/* Will retry next time */
	}
	
	global.is_idle = is_idle;
	
	return is_idle;
}
