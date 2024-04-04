
#ifndef __LISTENER_H__
#define __LISTENER_H__

enum listener_event_type
{
  /* The player has been started, stopped or seeked */
  LISTENER_PLAYER    = (1 << 0),
  /* The current playlist has been modified */
  LISTENER_PLAYLIST  = (1 << 1),
  /* The volume has been changed */
  LISTENER_VOLUME    = (1 << 2),
  /* A speaker has been enabled or disabled */
  LISTENER_SPEAKER   = (1 << 3),
  /* Options like repeat, random has been changed */
  LISTENER_OPTIONS   = (1 << 4),
  /* The library has been modified */
  LISTENER_DATABASE  = (1 << 5),
};

typedef void (*notify)(enum listener_event_type type);

/*
 * Registers the given callback function to the given event types.
 * This function is not thread safe. Listeners must be added once at startup.
 *
 * @param notify_cb Callback function
 * @param events Event mask, one or more of LISTENER_*
 * @return 0 on success, -1 on failure
 */
int
listener_add(notify notify_cb, short events);

/*
 * Removes the given callback function
 * This function is not thread safe. Listeners must be removed once at shutdown.
 *
 * @param notify_cb Callback function
 * @return 0 on success, -1 if the callback was not registered
 */
int
listener_remove(notify notify_cb);

/*
 * Calls the callback function of the registered listeners listening for the
 * given type of event.
 *
 * @param type The event type, on of the LISTENER_* values
 *
 */
void
listener_notify(enum listener_event_type type);

#endif /* !__LISTENER_H__ */
