/*-*- Mode: C; c-basic-offset: 8; indent-tabs-mode: nil -*-*/

#ifndef foospecialhfoo
#define foospecialhfoo

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

#define SPECIAL_DEFAULT_TARGET "default.target"

/* Shutdown targets */
#define SPECIAL_UMOUNT_TARGET "umount.target"
/* This is not really intended to be started by directly. This is
 * mostly so that other targets (reboot/halt/poweroff) can depend on
 * it to bring all services down that want to be brought down on
 * system shutdown. */
#define SPECIAL_SHUTDOWN_TARGET "shutdown.target"
#define SPECIAL_HALT_TARGET "halt.target"
#define SPECIAL_POWEROFF_TARGET "poweroff.target"
#define SPECIAL_REBOOT_TARGET "reboot.target"
#define SPECIAL_KEXEC_TARGET "kexec.target"
#define SPECIAL_EXIT_TARGET "exit.target"
#define SPECIAL_SUSPEND_TARGET "suspend.target"
#define SPECIAL_HIBERNATE_TARGET "hibernate.target"

/* Special boot targets */
#define SPECIAL_RESCUE_TARGET "rescue.target"
#define SPECIAL_EMERGENCY_TARGET "emergency.target"

/* Early boot targets */
#define SPECIAL_SYSINIT_TARGET "sysinit.target"
#define SPECIAL_SOCKETS_TARGET "sockets.target"
#define SPECIAL_LOCAL_FS_TARGET "local-fs.target"         /* LSB's $local_fs */
#define SPECIAL_LOCAL_FS_PRE_TARGET "local-fs-pre.target"
#define SPECIAL_REMOTE_FS_TARGET "remote-fs.target"       /* LSB's $remote_fs */
#define SPECIAL_REMOTE_FS_PRE_TARGET "remote-fs-pre.target"
#define SPECIAL_SWAP_TARGET "swap.target"
#define SPECIAL_BASIC_TARGET "basic.target"

/* LSB compatibility */
#define SPECIAL_NETWORK_TARGET "network.target"           /* LSB's $network */
#define SPECIAL_NSS_LOOKUP_TARGET "nss-lookup.target"     /* LSB's $named */
#define SPECIAL_RPCBIND_TARGET "rpcbind.target"           /* LSB's $portmap */
#define SPECIAL_SYSLOG_TARGET "syslog.target"             /* LSB's $syslog */
#define SPECIAL_TIME_SYNC_TARGET "time-sync.target"       /* LSB's $time */
#define SPECIAL_DISPLAY_MANAGER_SERVICE "display-manager.service"       /* Debian's $x-display-manager */
#define SPECIAL_MAIL_TRANSFER_AGENT_TARGET "mail-transfer-agent.target" /* Debian's $mail-{transport|transfer-agent */
#define SPECIAL_HTTP_DAEMON_TARGET "http-daemon.target"

/* Magic early boot services */
#define SPECIAL_FSCK_SERVICE "fsck@.service"
#define SPECIAL_QUOTACHECK_SERVICE "quotacheck.service"
#define SPECIAL_QUOTAON_SERVICE "quotaon.service"
#define SPECIAL_REMOUNT_FS_SERVICE "systemd-remount-fs.service"

/* Services systemd relies on */
#define SPECIAL_DBUS_SERVICE "dbus.service"
#define SPECIAL_DBUS_SOCKET "dbus.socket"
#define SPECIAL_JOURNALD_SOCKET "systemd-journald.socket"
#define SPECIAL_JOURNALD_SERVICE "systemd-journald.service"

/* Magic init signals */
#define SPECIAL_KBREQUEST_TARGET "kbrequest.target"
#define SPECIAL_SIGPWR_TARGET "sigpwr.target"
#define SPECIAL_CTRL_ALT_DEL_TARGET "ctrl-alt-del.target"

/* For SysV compatibility. Usually an alias for a saner target. On
 * SysV-free systems this doesn't exist. */
#define SPECIAL_RUNLEVEL2_TARGET "runlevel2.target"
#define SPECIAL_RUNLEVEL3_TARGET "runlevel3.target"
#define SPECIAL_RUNLEVEL4_TARGET "runlevel4.target"
#define SPECIAL_RUNLEVEL5_TARGET "runlevel5.target"

#endif
