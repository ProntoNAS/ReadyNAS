#include <linux/audit.h>
#include <linux/printk.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/string.h>
#include "iscsi_target_audit_log.h"
#include "iscsi_target_parameters.h"

#define AUDIT_DEFAULT_ID 4294967295
#define AUDIT_LIO_MSG " msg='op=LIO:%s acct=\"%s\" exe=\"lio-iscsi\""
#define AUDIT_LIO_MSG1 " hostname=%s addr=%s terminal=%s res=%s'"

static void send_audit_log(const char *initiator, const char *ip,
	const char *target, int type, int result)
{
	struct audit_buffer *ab;
	struct task_struct *tsk = current;
	pid_t pid = task_pid_nr(tsk);
	uid_t uid = from_kuid(&init_user_ns, task_uid(tsk));
	uid_t loginuid = AUDIT_DEFAULT_ID;
	unsigned int sessionid = AUDIT_DEFAULT_ID;
	const char *res = NULL;
	const char *op = NULL;

	if (!ip)
		return;

	if (result)
		res = "failed";
	else
		res = "success";

	if (type == AUDIT_USER_AUTH)
		op = "authentication";
	else
		op = "session_close";

	ab = audit_log_start(NULL, GFP_KERNEL, type);
	if (ab) {
		audit_log_format(ab, " pid=%u uid=%u auid=%u ses=%u", pid, uid,
				 loginuid, sessionid);
		audit_log_format(ab, AUDIT_LIO_MSG AUDIT_LIO_MSG1,
				op, initiator ? initiator : "--",
				ip, ip, target ? target : "--", res);
		audit_log_end(ab);
	}
}

static char *get_login_param(struct iscsi_login *login, const char *param)
{
	struct iscsi_login_req *login_req;
	char *tmpbuf, *start = NULL, *end = NULL, *key, *value;
	char *buf = NULL;
	u32 payload_length;
	int len = 0, len1 = 0;

	if (!login ||
		!login->req_buf ||
		!param) {
		pr_err("Invalid input.\n");
		goto out;
	}

	pr_debug("get_login_param: param=[%s]\n", param);

	len = strlen(param);
	if (len < 1) {
		pr_err("param is not valid\n");
		goto out;
	}

	login_req = (struct iscsi_login_req *) login->req;
	payload_length = ntoh24(login_req->dlength);

	tmpbuf = kzalloc(payload_length + 1, GFP_KERNEL);
	if (!tmpbuf)
		goto out;

	/* double check the req_buf */
	if (login->req_buf)
		memcpy(tmpbuf, login->req_buf, payload_length);
	else
		goto out;

	tmpbuf[payload_length] = '\0';
	start = tmpbuf;
	end = (start + payload_length);

	while (start < end) {
		if (iscsi_extract_key_value(start, &key, &value) < 0)
			goto out;

		if (!strncmp(key, param, len)) {
			len1 = strlen(value);
			buf = kzalloc(len1 + 1, GFP_KERNEL);
			if (!buf) {
				pr_err("Allocate memory for buf failed.\n");
				goto out;
			}
			memcpy(buf, value, len1);
			buf[len1] = '\0';
			break;
		}

		start += strlen(key) + strlen(value) + 2;
	}

out:
	kfree(tmpbuf);
	return buf;
}

void iscsi_login_audit_log(struct iscsi_conn *conn, int type, int ret)
{
	struct iscsi_portal_group *tpg = NULL;
	struct iscsi_login *login = conn->login;
	struct iscsi_session *sess = NULL;
	struct iscsi_param *param = NULL;
	const char *target = NULL;
	char *sockaddr = NULL;
	char *initiator = NULL;
	char *buf = NULL, *buf_target = NULL;
	int i = 0;

	if (audit_enabled == 0) {
		pr_debug("Audit disabled, don't send logs.\n");
		return;
	}

	if (conn == NULL ||
		conn->sess == NULL ||
		conn->sess->sess_ops == NULL) {
		pr_err("Invaild iscsi conn.\n");
		return;
	}

	sess = conn->sess;

	/* ignore Discovery session */
	if (sess->sess_ops->SessionType == 1)
		return;

	tpg = conn->tpg;
	if (tpg && tpg->tpg_tiqn)
		target = tpg->tpg_tiqn->tiqn;

	/* when login, it may be null string, get it from login param */
	if (!target || target[0] == '\0') {
		buf_target = get_login_param(login, "TargetName");
		if (buf_target)
			target = buf_target;
	}

	initiator = sess->sess_ops->InitiatorName;
	if (initiator[0] == '\0') {
		if (conn->param_list) {
			param = iscsi_find_param_from_key(INITIATORNAME,
					conn->param_list);
			if (param)
				initiator = param->value;
		} else {
			buf = get_login_param(login, "InitiatorName");
			if (buf)
				initiator = buf;
		}

	}

	sockaddr = kasprintf(GFP_KERNEL, "%pISpc", &conn->login_sockaddr);
	/* sockaddr should be sth like "ip:port", remove port */
	if (sockaddr) {
		for (i = strlen(sockaddr) - 1; i > 0; i--) {
			if (sockaddr[i] == ':') {
				sockaddr[i] = '\0';
				break;
			}
		}
	}

	send_audit_log(initiator, sockaddr, target, type, ret);

	kfree(sockaddr);
	kfree(buf);
	kfree(buf_target);
}
