#ifndef ISCSI_TARGET_AUDIT_LOG_H
#define ISCSI_TARGET_AUDIT_LOG_H

#include <target/iscsi/iscsi_target_core.h>

#define AUDIT_USER_AUTH		1100
#define AUDIT_USER_END		1106

/*
 * Call audit API(audit_log_*) to send logs for iscsi login/logout
 * conn, login: used to get the login param like initiator/target name
 * type: here must be AUDIT_USER_AUTH or AUDIT_USER_END
 */
extern void iscsi_login_audit_log(struct iscsi_conn *conn, int type, int ret);

#endif
