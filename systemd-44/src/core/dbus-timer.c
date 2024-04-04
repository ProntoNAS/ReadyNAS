/*-*- Mode: C; c-basic-offset: 8; indent-tabs-mode: nil -*-*/

/***
  This file is part of systemd.

  Copyright 2010 Lennart Poettering

  systemd is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  systemd is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with systemd; If not, see <http://www.gnu.org/licenses/>.
***/

#include <errno.h>

#include "dbus-unit.h"
#include "dbus-timer.h"
#include "dbus-execute.h"
#include "dbus-common.h"

#define BUS_TIMER_INTERFACE                                             \
        " <interface name=\"org.freedesktop.systemd1.Timer\">\n"        \
        "  <property name=\"Unit\" type=\"s\" access=\"read\"/>\n"      \
        "  <property name=\"Timers\" type=\"a(stt)\" access=\"read\"/>\n" \
        "  <property name=\"NextElapseUSec\" type=\"t\" access=\"read\"/>\n" \
        "  <property name=\"Result\" type=\"s\" access=\"read\"/>\n"    \
        " </interface>\n"

#define INTROSPECTION                                                   \
        DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE                       \
        "<node>\n"                                                      \
        BUS_UNIT_INTERFACE                                              \
        BUS_TIMER_INTERFACE                                             \
        BUS_PROPERTIES_INTERFACE                                        \
        BUS_PEER_INTERFACE                                              \
        BUS_INTROSPECTABLE_INTERFACE                                    \
        "</node>\n"

#define INTERFACES_LIST                              \
        BUS_UNIT_INTERFACES_LIST                     \
        "org.freedesktop.systemd1.Timer\0"

const char bus_timer_interface[] _introspect_("Timer") = BUS_TIMER_INTERFACE;

const char bus_timer_invalidating_properties[] =
        "Timers\0"
        "NextElapseUSec\0"
        "Result\0";

static int bus_timer_append_timers(DBusMessageIter *i, const char *property, void *data) {
        Timer *p = data;
        DBusMessageIter sub, sub2;
        TimerValue *k;

        assert(i);
        assert(property);
        assert(p);

        if (!dbus_message_iter_open_container(i, DBUS_TYPE_ARRAY, "(stt)", &sub))
                return -ENOMEM;

        LIST_FOREACH(value, k, p->values) {
                char *buf;
                const char *t;
                size_t l;
                bool b;

                t = timer_base_to_string(k->base);
                assert(endswith(t, "Sec"));

                /* s/Sec/USec/ */
                l = strlen(t);
                if (!(buf = new(char, l+2)))
                        return -ENOMEM;

                memcpy(buf, t, l-3);
                memcpy(buf+l-3, "USec", 5);

                b = dbus_message_iter_open_container(&sub, DBUS_TYPE_STRUCT, NULL, &sub2) &&
                        dbus_message_iter_append_basic(&sub2, DBUS_TYPE_STRING, &buf) &&
                        dbus_message_iter_append_basic(&sub2, DBUS_TYPE_UINT64, &k->value) &&
                        dbus_message_iter_append_basic(&sub2, DBUS_TYPE_UINT64, &k->next_elapse) &&
                        dbus_message_iter_close_container(&sub, &sub2);

                free(buf);
                if (!b)
                        return -ENOMEM;
        }

        if (!dbus_message_iter_close_container(i, &sub))
                return -ENOMEM;

        return 0;
}

static int bus_timer_append_unit(DBusMessageIter *i, const char *property, void *data) {
        Unit *u = data;
        Timer *timer = TIMER(u);
        const char *t;

        assert(i);
        assert(property);
        assert(u);

        t = UNIT_DEREF(timer->unit) ? UNIT_DEREF(timer->unit)->id : "";

        return dbus_message_iter_append_basic(i, DBUS_TYPE_STRING, &t) ? 0 : -ENOMEM;
}

static DEFINE_BUS_PROPERTY_APPEND_ENUM(bus_timer_append_timer_result, timer_result, TimerResult);

static const BusProperty bus_timer_properties[] = {
        { "Unit",           bus_timer_append_unit,        "s", 0 },
        { "Timers",         bus_timer_append_timers, "a(stt)", 0 },
        { "NextElapseUSec", bus_property_append_usec,     "t", offsetof(Timer, next_elapse) },
        { "Result",         bus_timer_append_timer_result,"s", offsetof(Timer, result)      },
        { NULL, }
};

DBusHandlerResult bus_timer_message_handler(Unit *u, DBusConnection *c, DBusMessage *message) {
        Timer *t = TIMER(u);
        const BusBoundProperties bps[] = {
                { "org.freedesktop.systemd1.Unit",  bus_unit_properties,  u },
                { "org.freedesktop.systemd1.Timer", bus_timer_properties, t },
                { NULL, }
        };

        return bus_default_message_handler(c, message, INTROSPECTION, INTERFACES_LIST, bps);
}
