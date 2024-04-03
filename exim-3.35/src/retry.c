/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Functions concerned with retrying unsuccessful deliveries. */


#include "exim.h"



/*************************************************
*     Set status of a host+address item          *
*************************************************/

/* This function is passed a host_item which contains a host name and an
IP address string. Its job is to set the status of the address if it is not
already set (indicated by hstatus_unknown). The possible values are:

   hstatus_usable    the address is not listed in the unusable tree, and does
                     not have a retry record, OR the time is past the next
                     try time, OR the message has been on the queue for more
                     than the maximum retry time for a failing host

   hstatus_unusable  the address is listed in the unusable tree, or does have
                     a retry record, and the time is not yet at the next retry
                     time.

   hstatus_unusable_expired  as above, but also the retry time has expired
                     for this address.

The reason a delivery is permitted when a message has been around for a very
long time is to allow the ultimate address timeout to operate after a delivery
failure. Otherwise some messages may stick around without being tried for too
long.

If a host retry record is retrieved from the hints database, the time of last
trying is filled into the last_try field of the host block. If a host is
generally usable, a check is made to see if there is a retry delay on this
specific message at this host.

If a non-standard port is being used, it is added to the retry key.

Arguments:
  domain              the address domain
  host                pointer to a host item
  portstring          "" for standard port, ":xxxx" for a non-standard port
  include_ip_address  TRUE to include the address in the key - this is
                        usual, but sometimes is not wanted
  retry_host_key      where to put a pointer to the key for the host-specific
                        retry record, if one is read and the host is usable
  retry_message_key   where to put a pointer to the key for the message+host
                        retry record, if one is read and the host is usable

Returns:    TRUE if the host has expired but is usable because
             its retry time has come
*/

BOOL
retry_check_address(char *domain, host_item *host, char *portstring,
  BOOL include_ip_address, char **retry_host_key, char **retry_message_key)
{
BOOL yield = FALSE;
time_t now = time(NULL);
char *host_key, *message_key;
open_db dbblock;
open_db *dbm_file;
tree_node *node;
dbdata_retry *host_retry_record, *message_retry_record;

*retry_host_key = *retry_message_key = NULL;

DEBUG(9) debug_printf("checking status of %s\n", host->name);

/* Do nothing if status already set; otherwise initialize status as usable. */

if (host->status != hstatus_unknown) return FALSE;
host->status = hstatus_usable;

/* Generate the host key for the unusable tree and the retry database. Ensure
host names are lower cased (that's what %S does). */

host_key = include_ip_address?
  string_sprintf("T:%S:%s%s", host->name, host->address, portstring) :
  string_sprintf("T:%S%s", host->name, portstring);

/* Generate the message-specific key */

message_key = string_sprintf("%s:%s", host_key, message_id);

/* Search the tree of unusable IP addresses. This is filled in when deliveries
fail, because the retry database itself is not updated until the end of all
deliveries (so as to do it all in one go). The tree records addresses that have
become unusable during this delivery process (i.e. those that will get put into
the retry database when it is updated). */

node = tree_search(tree_unusable, host_key);
if (node != NULL)
  {
  DEBUG(9) debug_printf("found in tree of unusables\n");
  host->status = (node->data.val > 255)?
    hstatus_unusable_expired : hstatus_unusable;
  host->why = node->data.val & 255;
  return FALSE;
  }

/* Open the retry database, giving up if there isn't one. Otherwise, search for
the retry records, and then close the database again. */

if ((dbm_file = dbfn_open("retry", O_RDONLY, &dbblock, FALSE)) == NULL)
  return FALSE;
host_retry_record = dbfn_read(dbm_file, host_key);
message_retry_record = dbfn_read(dbm_file, message_key);
dbfn_close(dbm_file);

/* Ignore the data if it is too old - too long since it was written */

if (host_retry_record == NULL)
  {
  DEBUG(9) debug_printf("no host retry record\n");
  }
else if (now - host_retry_record->time_stamp > retry_data_expire)
  {
  host_retry_record = NULL;
  DEBUG(9) debug_printf("host retry record too old\n");
  }

if (message_retry_record == NULL)
  {
  DEBUG(9) debug_printf("no message retry record\n");
  }
else if (now - message_retry_record->time_stamp > retry_data_expire)
  {
  message_retry_record = NULL;
  DEBUG(9) debug_printf("message retry record too old\n");
  }

/* If there's a host-specific retry record, check for reaching the retry
time (or forcing). If not, and the host is not expired, check for the message
having been around for longer than the maximum retry time for this host or
address. Allow the delivery if it has. Otherwise set the appropriate unusable
flag and return FALSE. Otherwise arrange to return TRUE if this is an expired
host. */

if (host_retry_record != NULL)
  {
  time_t now = time(NULL);

  *retry_host_key = host_key;

  if (now < host_retry_record->next_try && !deliver_force)
    {
    /* Check for ultimate address timeout if host not expired */

    if (!host_retry_record->expired)
      {
      BOOL address_timeout = FALSE;

      retry_config *retry =
        retry_find_config(host_key+2, domain,
          host_retry_record->basic_errno,
          host_retry_record->more_errno);

      DEBUG(9) debug_printf("retry time not reached: checking ultimate "
        "address timeout\n");

      if (retry != NULL)
        {
        retry_rule *last_rule;
        for (last_rule = retry->rules;
             last_rule->next != NULL;
             last_rule = last_rule->next);
        if (now - received_time > last_rule->timeout) address_timeout = TRUE;
        }
      else address_timeout = TRUE;    /* No rule => timed out */

      if (address_timeout)
        {
        DEBUG(5) debug_printf("on queue longer than maximum retry for "
          "address - allowing delivery\n");
        return FALSE;
        }
      }

    /* We have not hit the ultimate address timeout; host is unusable. */

    host->status = (host_retry_record->expired)?
      hstatus_unusable_expired : hstatus_unusable;
    host->why = hwhy_retry;
    host->last_try = host_retry_record->last_try;
    return FALSE;
    }

  /* Host is usable; set return TRUE if expired. */

  yield = host_retry_record->expired;
  }

/* It's OK to try the host. If there's a message-specific retry record, check
for reaching its retry time (or forcing). If not, mark the host unusable. */

if (message_retry_record != NULL)
  {
  *retry_message_key = message_key;
  if (time(NULL) < message_retry_record->next_try && !deliver_force)
    {
    host->status = hstatus_unusable;
    host->why = hwhy_retry;
    return FALSE;
    }
  }

return yield;
}




/*************************************************
*           Add a retry item to an address       *
*************************************************/

/* Retry items are chained onto an address when it is deferred either by
a director, router, or transport, or if it succeeds or fails and there was a
previous retry item that now needs to be deleted. Sometimes there can be both
kinds of item: for example, if routing was deferred but then succeeded, and
delivery then deferred. In that case there is a delete item for the routing
retry, and an updating item for the delivery.

These items are used at the end of a delivery attempt to update the retry
database. The key name is built from a type letter and the domain, optionally
with "<local_part>@" on the front - this is used for local destinations - or
passed in externally for remote destinations, which might be more complex (SMTP
destinations consist of the domain + the IP address).

Arguments:
  addr    the address block
  type    the type letter: one of 'D', 'R', or 'T'
  use_at  if TRUE, "addr->local_part@" is included after "type:"
  key     if NULL, addr->domain is used, otherwise key
  flags   delete, host, and message flags, copied into the block

Returns:  nothing
*/

void
retry_add_item(address_item *addr, char *type, BOOL use_at, char *key,
  int flags)
{
retry_item *rti = store_get(sizeof(retry_item));
rti->next = addr->retries;
addr->retries = rti;
rti->destination = string_sprintf("%s:%s%s%s", type,
  use_at? addr->local_part : "",
  use_at? "@" : "",
  (key != NULL)? key : addr->domain);
rti->basic_errno = addr->basic_errno;
rti->more_errno = addr->more_errno;
rti->message = addr->message;
rti->flags = flags;

DEBUG(7) debug_printf("added retry item for %s: errno=%d %d flags=%d\n",
  rti->destination, rti->basic_errno, rti->more_errno, flags);
}



/*************************************************
*        Find retry configuration data           *
*************************************************/

/* Search the in-store retry information for the first retry item that applies
to a given destination. Remote Internet destinations are given as
domain:address. It doesn't seem necessary to go to that level of detail in
retry rules, so we chop off the address and match on the domain part only.
Local addresses are given as user@domain. The alternative destination is the
domain name, which is always just a domain. This function is used for real
only in this module, but it is global so it can be tested by means of the -brt
option.

Arguments:
  destination  destination for which retry info is wanted
  alternate    alternative destination, always just a domain
  basic_errno  specific error predicate on the retry rule, or zero
  more_errno   additional data for errno predicate (for errno_quota only)

Returns:       pointer to retry rule, or NULL
*/

retry_config *
retry_find_config(char *destination, char *alternate, int basic_errno,
  int more_errno)
{
int domain = 0;
int replace = ':';
int sep = UCHAR_MAX + 1;
char *colon = strchr(destination, ':');
retry_config *yield;

/* If there's a colon in the destination, temporarily replace it with
a zero to terminate the string there. If there's no colon there should be
an @; set up the domain offset. */

if (colon == NULL)
  {
  char *domain_offset = strrchr(destination, '@');
  if (domain_offset != NULL) domain = domain_offset - destination + 1;
  colon = destination + (int)strlen(destination);
  replace = 0;
  }
*colon = 0;

/* Scan the configured retry items. */

for (yield = retries; yield != NULL; yield = yield->next)
  {
  char *dlist = yield->destination;

  /* If a specific error is set for this item, check that we are
  handling that specific error, and if so, check any additional
  error information if required. Special code is required for quota
  errors, as these can either be system quota errors, or Exim's own
  quota imposition, which has a different error number. Full partitions
  are also treated in the same way as quota errors. */

  if (yield->basic_errno != 0)
    {
    if (yield->basic_errno == ERRNO_EXIMQUOTA)
      {
      if ((basic_errno != ERRNO_EXIMQUOTA && basic_errno != errno_quota &&
           basic_errno != ENOSPC) ||
          (yield->more_errno != 0 && yield->more_errno > more_errno))
        continue;
      }
    else
      {
      if (yield->basic_errno != basic_errno ||
         (yield->more_errno != 0 && yield->more_errno != more_errno))
       continue;
      }
    }

  /* If the destination contains an @ we are handling a local delivery;
  otherwise it's a remote host name. Either way, we can persuade the
  match_address function to handle it. */

  if (match_address_list(destination, domain, TRUE, &dlist, -1, &sep, FALSE))
    break;

  /* If the match failed, have a go at matching on the alternative destination,
  which is normally the domain as opposed to the host for a remote delivery. */

  if (alternate != NULL &&
      match_address_list(alternate, 0, TRUE, &dlist, -1, &sep, FALSE)) break;
  }

*colon = replace;
return yield;
}




/*************************************************
*              Update retry database             *
*************************************************/

/* Update the retry data for any directing/routing/transporting that was
deferred, or delete it for those that succeeded after a previous defer. This is
done all in one go to minimize opening/closing/locking of the database file.

Note that, because SMTP delivery involves a list of destinations to try, there
may be defer-type retry information for some of them even when the message was
successfully delivered. Likewise if it eventually failed.

This function may move addresses from the defer to the failed queue if the
ultimate retry time has expired.

Arguments:
  addr_defer    queue of deferred addresses
  addr_failed   queue of failed addresses
  addr_succeed  queue of successful addresses

Returns:        nothing
*/

void
retry_update(address_item **addr_defer, address_item **addr_failed,
  address_item **addr_succeed)
{
open_db dbblock;
open_db *dbm_file = NULL;
time_t now = time(NULL);
int i;

DEBUG(2) debug_printf("Processing retry items\n");

/* Three-times loop to handle succeeded, failed, and deferred addresses.
Deferred addresses must be handled after failed ones, because some may be moved
to the failed chain if they have timed out. */

for (i = 0; i < 3; i++)
  {
  address_item *endaddr, *addr;
  address_item *last_first = NULL;
  address_item **paddr = (i==0)? addr_succeed :
    (i==1)? addr_failed : addr_defer;
  address_item **saved_paddr = NULL;

  DEBUG(5) debug_printf("%s addresses:\n", (i == 0)? "Succeeded" : (i == 1)?
    "Failed" : "Deferred");

  /* Loop for each address on the chain. For deferred addresses, the whole
  address times out unless one of its retry addresses has a retry rule that
  hasn't yet timed out. Deferred addresses should not be requesting deletion
  of retry items, but just in case they do by accident, treat that case
  as "not timed out".

  As well as handling the addresses themselves, we must also process any
  retry items for any parent addresses - these are typically "delete" items,
  because the parent must have succeeded in order to generate the child. */

  while ((endaddr = *paddr) != NULL)
    {
    BOOL timed_out = FALSE;
    retry_item *rti;

    for (addr = endaddr; addr != NULL; addr = addr->parent)
      {
      int update_count = 0;
      int timedout_count = 0;

      DEBUG(5) debug_printf("%s%s\n", addr->orig, (addr->retries == NULL)?
        ": no retry items" : "");

      /* Loop for each retry item. */

      for (rti = addr->retries; rti != NULL; rti = rti->next)
        {
        char *message;
        int message_length, message_space, failing_interval, next_try;
        retry_rule *rule, *final_rule;
        retry_config *retry;
        dbdata_retry *retry_record;

        /* Open the retry database if it is not already open; failure to open
        the file is logged, but otherwise ignored - deferred addresses will
        get retried at the next opportunity. Not opening earlier than this saves
        opening if no addresses have retry items - common when none have yet
        reached their retry next try time. */

        if (dbm_file == NULL)
          dbm_file = dbfn_open("retry", O_RDWR|O_CREAT, &dbblock, TRUE);
        if (dbm_file == NULL) return;

        /* If there are no deferred addresses, that is, if this message is
        completing, and the retry item is for a message-specific SMTP error,
        force it to be deleted, because there's no point in keeping data for
        no-longer-existing messages. This situation can occur when a domain has
        two hosts and a message-specific error occurs for the first of them,
        but the address gets delivered to the second one. This optimization
        doesn't succeed in cleaning out all the dead entries, but it helps. */

        if (*addr_defer == NULL && (rti->flags & rf_message) != 0)
          rti->flags |= rf_delete;

        /* Handle the case of a request to delete the retry info for this
        destination. */

        if ((rti->flags & rf_delete) != 0)
          {
          (void)dbfn_delete(dbm_file, rti->destination);
          DEBUG(5) debug_printf("deleted retry information for %s\n",
            rti->destination);
          continue;
          }

        /* Count the number of non-delete retry items. This is so that we
        can compare it to the count of timed_out ones, to check whether
        all are timed out. */

        update_count++;

        /* Get the retry information for this destination and error code, if
        any. If this item is for a remote host with ip address, then pass
        the domain name as an alternative to search for. If no retry
        information is found, we can't generate a retry time, so there is
        no point updating the database. This retry item is timed out. */

        if ((retry = retry_find_config(rti->destination+2,
             ((rti->flags & rf_host) != 0)? addr->domain : NULL,
             rti->basic_errno, rti->more_errno)) == NULL)
          {
          DEBUG(5) debug_printf("No configured retry item for %s%s%s\n",
            rti->destination,
            ((rti->flags & rf_host) != 0)? " or " : "",
            ((rti->flags & rf_host) != 0)? addr->domain : "");
          if (addr == endaddr) timedout_count++;
          continue;
          }

        DEBUG(5)
          {
          if ((rti->flags & rf_host) != 0)
            debug_printf("retry for %s (%s) = %s\n", rti->destination,
              addr->domain, retry->destination);
          else
            debug_printf("retry for %s = %s\n", rti->destination,
              retry->destination);
          }

        /* Set up the message for the database retry record. Because DBM
        records have a maximum data length, we enforce a limit. There isn't
        much point in keeping a huge message here, anyway. */

        message = (rti->basic_errno > 0)? strerror(rti->basic_errno) :
          (rti->message == NULL)?
          "unknown error" : string_printing(rti->message);
        message_length = strlen(message);
        if (message_length > 100) message_length = 100;

        /* Read a retry record from the database or construct a new one.
        Ignore an old one if it is too old since it was last updated. */

        retry_record = dbfn_read(dbm_file, rti->destination);
        if (retry_record != NULL &&
            now - retry_record->time_stamp > retry_data_expire)
          retry_record = NULL;

        if (retry_record == NULL)
          {
          retry_record = store_get(sizeof(dbdata_retry) + message_length);
          message_space = message_length;
          retry_record->first_failed = now;
          retry_record->last_try = now;
          retry_record->next_try = now;
          retry_record->expired = FALSE;
          retry_record->text[0] = 0;      /* just in case */
          }
        else message_space = (int)strlen(retry_record->text);

        /* Compute how long this destination has been failing */

        failing_interval = now - retry_record->first_failed;

        /* Search for the current retry rule. The cutoff time of the
        last rule is handled differently to the others. The rule continues
        to operate for ever (the global maximum interval will eventually
        limit the gaps) but its cutoff time determines when an individual
        destination times out. If there are no retry rules, the destination
        always times out, but we can't compute a retry time. */

        final_rule = NULL;
        for (rule = retry->rules; rule != NULL; rule = rule->next)
          {
          if (failing_interval <= rule->timeout) break;
          final_rule = rule;
          }

        /* If there's an un-timed out rule, the destination has not
        yet timed out, so the address as a whole has not timed out (but we are
        interested in this only for the end address). Make sure the expired
        flag is false (can be forced via fixdb from outside, but ensure it is
        consistent with the rules whenever we go through here). */

        if (rule != NULL)
          {
          retry_record->expired = FALSE;
          }

        /* Otherwise, set the retry timeout expired, and set the final rule
        as the one from which to compute the next retry time. Subsequent
        messages will fail immediately until the retry time is reached (unless
        there are other, still active, retries). */

        else
          {
          rule = final_rule;
          retry_record->expired = TRUE;
          if (addr == endaddr) timedout_count++;
          }

        /* There is a special case to consider when some messages get through
        to a destination and others don't. This can happen locally when a
        large message pushes a user over quota, and it can happen remotely
        when a machine is on a dodgy Internet connection. The messages that
        get through wipe the retry information, causing those that don't to
        stay on the queue longer than the final retry time. In order to
        avoid this, we check, using the time of arrival of the message, to
        see if it has been on the queue for more than the final cutoff time,
        and if so, cause this retry item to time out, and the retry time to
        be set to "now" so that any subsequent messages in the same condition
        also get tried. We search for the last rule onwards from the one that
        is in use. If there are no retry rules for the item, rule will be null
        and timedout_count will already have been updated.

        This implements "timeout this rule if EITHER the host (or routing or
        directing) has been failing for more than the maximum time, OR if the
        message has been on the queue for more than the maximum time." */

        if (received_time < retry_record->first_failed &&
            addr == endaddr && !retry_record->expired && rule != NULL)
          {
          retry_rule *last_rule;
          for (last_rule = rule;
               last_rule->next != NULL;
               last_rule = last_rule->next);
          if (now - received_time > last_rule->timeout)
            {
            DEBUG(5) debug_printf("on queue longer than maximum retry\n");
            timedout_count++;
            rule = NULL;
            }
          }

        /* Compute the next try time from the rule, subject to the global
        maximum, and update the retry database. If rule == NULL it means
        there were no rules at all (and the timeout will be set expired),
        or we have a message that is older than the final timeout. In this
        case set the next retry time to now, so that one delivery attempt
        happens for subsequent messages. */

        if (rule == NULL) next_try = now; else
          {
          if (rule->rule == 'F') next_try = now + rule->p1;
          else  /* assume rule = 'G' */
            {
            int last_predicted_gap =
              retry_record->next_try - retry_record->last_try;
            int last_actual_gap = now - retry_record->last_try;
            int lastgap = (last_predicted_gap < last_actual_gap)?
              last_predicted_gap : last_actual_gap;
            next_try = now + ((lastgap < rule->p1)? rule->p1 :
               (lastgap * rule->p2)/1000);
            }
          }

        /* Impose a global retry max */

        if (next_try - now > retry_interval_max)
          next_try = now + retry_interval_max;

        /* If the new message length is greater than the previous one, we
        have to copy the record first. */

        if (message_length > message_space)
          {
          dbdata_retry *newr = store_get(sizeof(dbdata_retry) + message_length);
          memcpy(newr, retry_record, sizeof(dbdata_retry));
          retry_record = newr;
          }

        /* Set up the retry record; message_length may be less than the string
        length for very long error strings. */

        retry_record->last_try = now;
        retry_record->next_try = next_try;
        retry_record->basic_errno = rti->basic_errno;
        retry_record->more_errno = rti->more_errno;
        strncpy(retry_record->text, message, message_length);
        retry_record->text[message_length] = 0;

        DEBUG(5)
          {
          debug_printf("Writing retry data for %s\n", rti->destination);
          debug_printf("  first failed=%d last try=%d next try=%d expired=%d\n",
            retry_record->first_failed, retry_record->last_try,
            retry_record->next_try, retry_record->expired);
          debug_printf("  error %d %d: %s\n", retry_record->basic_errno,
            retry_record->more_errno, retry_record->text);
          }

        (void)dbfn_write(dbm_file, rti->destination, retry_record,
          sizeof(dbdata_retry) + message_length);
        }                            /* Loop for each retry item */

      /* If all the non-delete retry items are timed out, the address is
      timed out. */

      if (update_count > 0 && update_count == timedout_count)
        {
        DEBUG(5) debug_printf("timed out: all retries expired\n");
        timed_out = TRUE;
        }
      }                              /* Loop for address & parents */

    /* If this is a deferred address, and retry processing was requested by
    means of one or more retry items, and no actions were skipped because a
    retry time had not been reached, and no retry item is still active,
    move the address to the failed queue, and restart this loop without
    updating paddr.

    If there were several addresses batched in the same remote delivery, only
    the original top one will have host retry items attached to it, but we want
    to handle all the same. Each will have a pointer back to its "top" address,
    and they will now precede the item with the retries because addresses are
    inverted when added to these final queues. We have saved information about
    them in passing (below) so they can all be cut out at once. */

    if (i == 2)   /* Handling defers */
      {
      if (endaddr->retries != NULL && !testflag(endaddr, af_retry_skipped) &&
          timed_out)
        {
        if (last_first == endaddr) paddr = saved_paddr;
        addr = *paddr;
        *paddr = endaddr->next;

        endaddr->next = *addr_failed;
        *addr_failed = addr;

        for (;; addr = addr->next)
          {
          setflag(addr, af_retry_timedout);
          addr->message = (addr->message == NULL)? "retry timeout exceeded" :
            string_sprintf("%s: retry timeout exceeded", addr->message);
          log_write(0, LOG_MAIN, "** %s%s%s%s: retry timeout exceeded",
            addr->orig,
           (addr->parent == NULL)? "" : " <",
           (addr->parent == NULL)? "" : addr->parent->orig,
           (addr->parent == NULL)? "" : ">");

          if (addr == endaddr) break;
          }

        continue;                       /* Restart from changed *paddr */
        }

      /* This address is to remain on the defer chain. If it has a "first"
      pointer, save the pointer to it in case we want to fail the set of
      addresses when we get to the first one. */

      if (endaddr->first != last_first)
        {
        last_first = endaddr->first;
        saved_paddr = paddr;
        }
      }

    /* All cases (succeed, fail, defer left on queue) */

    paddr = &(endaddr->next);         /* Advance to next address */
    }                                 /* Loop for all addresses  */
  }                                   /* Loop for succeed, fail, defer */

/* Close and unlock the database */

if (dbm_file != NULL) dbfn_close(dbm_file);

DEBUG(2) debug_printf("end of retry processing\n");
}

/* End of retry.c */
