/*
 * Errors generated from
 * [MS-ERREF] https://msdn.microsoft.com/en-us/library/cc231199.aspx
 */
#include <Python.h>
#include "python/py3compat.h"
#include "includes.h"

static inline PyObject *ndr_PyLong_FromUnsignedLongLong(unsigned long long v)
{
	if (v > LONG_MAX) {
		return PyLong_FromUnsignedLongLong(v);
	} else {
		return PyInt_FromLong(v);
	}
}

static struct PyModuleDef moduledef = {
	PyModuleDef_HEAD_INIT,
	.m_name = "werror",
	.m_doc = "WERROR defines",
	.m_size = -1,
};

MODULE_INIT_FUNC(werror)
{
	PyObject *m;

	m = PyModule_Create(&moduledef);
	if (m == NULL)
		return NULL;

	PyModule_AddObject(m, "WERR_SUCCESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SUCCESS)));
	PyModule_AddObject(m, "WERR_NERR_SUCCESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_SUCCESS)));
	PyModule_AddObject(m, "WERR_INVALID_FUNCTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_FUNCTION)));
	PyModule_AddObject(m, "WERR_FILE_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FILE_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_PATH_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PATH_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_TOO_MANY_OPEN_FILES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TOO_MANY_OPEN_FILES)));
	PyModule_AddObject(m, "WERR_ACCESS_DENIED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ACCESS_DENIED)));
	PyModule_AddObject(m, "WERR_INVALID_HANDLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_HANDLE)));
	PyModule_AddObject(m, "WERR_ARENA_TRASHED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ARENA_TRASHED)));
	PyModule_AddObject(m, "WERR_NOT_ENOUGH_MEMORY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_ENOUGH_MEMORY)));
	PyModule_AddObject(m, "WERR_INVALID_BLOCK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_BLOCK)));
	PyModule_AddObject(m, "WERR_BAD_ENVIRONMENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_ENVIRONMENT)));
	PyModule_AddObject(m, "WERR_BAD_FORMAT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_FORMAT)));
	PyModule_AddObject(m, "WERR_INVALID_ACCESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_ACCESS)));
	PyModule_AddObject(m, "WERR_INVALID_DATA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_DATA)));
	PyModule_AddObject(m, "WERR_OUTOFMEMORY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_OUTOFMEMORY)));
	PyModule_AddObject(m, "WERR_INVALID_DRIVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_DRIVE)));
	PyModule_AddObject(m, "WERR_CURRENT_DIRECTORY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CURRENT_DIRECTORY)));
	PyModule_AddObject(m, "WERR_NOT_SAME_DEVICE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_SAME_DEVICE)));
	PyModule_AddObject(m, "WERR_NO_MORE_FILES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_MORE_FILES)));
	PyModule_AddObject(m, "WERR_WRITE_PROTECT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WRITE_PROTECT)));
	PyModule_AddObject(m, "WERR_BAD_UNIT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_UNIT)));
	PyModule_AddObject(m, "WERR_NOT_READY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_READY)));
	PyModule_AddObject(m, "WERR_BAD_COMMAND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_COMMAND)));
	PyModule_AddObject(m, "WERR_CRC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CRC)));
	PyModule_AddObject(m, "WERR_BAD_LENGTH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_LENGTH)));
	PyModule_AddObject(m, "WERR_SEEK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SEEK)));
	PyModule_AddObject(m, "WERR_NOT_DOS_DISK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_DOS_DISK)));
	PyModule_AddObject(m, "WERR_SECTOR_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SECTOR_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_OUT_OF_PAPER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_OUT_OF_PAPER)));
	PyModule_AddObject(m, "WERR_WRITE_FAULT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WRITE_FAULT)));
	PyModule_AddObject(m, "WERR_READ_FAULT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_READ_FAULT)));
	PyModule_AddObject(m, "WERR_GEN_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_GEN_FAILURE)));
	PyModule_AddObject(m, "WERR_SHARING_VIOLATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SHARING_VIOLATION)));
	PyModule_AddObject(m, "WERR_LOCK_VIOLATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOCK_VIOLATION)));
	PyModule_AddObject(m, "WERR_WRONG_DISK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WRONG_DISK)));
	PyModule_AddObject(m, "WERR_SHARING_BUFFER_EXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SHARING_BUFFER_EXCEEDED)));
	PyModule_AddObject(m, "WERR_HANDLE_EOF",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_HANDLE_EOF)));
	PyModule_AddObject(m, "WERR_HANDLE_DISK_FULL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_HANDLE_DISK_FULL)));
	PyModule_AddObject(m, "WERR_NOT_SUPPORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_SUPPORTED)));
	PyModule_AddObject(m, "WERR_REM_NOT_LIST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REM_NOT_LIST)));
	PyModule_AddObject(m, "WERR_DUP_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DUP_NAME)));
	PyModule_AddObject(m, "WERR_BAD_NETPATH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_NETPATH)));
	PyModule_AddObject(m, "WERR_NETWORK_BUSY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NETWORK_BUSY)));
	PyModule_AddObject(m, "WERR_DEV_NOT_EXIST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DEV_NOT_EXIST)));
	PyModule_AddObject(m, "WERR_TOO_MANY_CMDS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TOO_MANY_CMDS)));
	PyModule_AddObject(m, "WERR_ADAP_HDW_ERR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ADAP_HDW_ERR)));
	PyModule_AddObject(m, "WERR_BAD_NET_RESP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_NET_RESP)));
	PyModule_AddObject(m, "WERR_UNEXP_NET_ERR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNEXP_NET_ERR)));
	PyModule_AddObject(m, "WERR_BAD_REM_ADAP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_REM_ADAP)));
	PyModule_AddObject(m, "WERR_PRINTQ_FULL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PRINTQ_FULL)));
	PyModule_AddObject(m, "WERR_NO_SPOOL_SPACE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_SPOOL_SPACE)));
	PyModule_AddObject(m, "WERR_PRINT_CANCELLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PRINT_CANCELLED)));
	PyModule_AddObject(m, "WERR_NETNAME_DELETED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NETNAME_DELETED)));
	PyModule_AddObject(m, "WERR_NETWORK_ACCESS_DENIED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NETWORK_ACCESS_DENIED)));
	PyModule_AddObject(m, "WERR_BAD_DEV_TYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_DEV_TYPE)));
	PyModule_AddObject(m, "WERR_BAD_NET_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_NET_NAME)));
	PyModule_AddObject(m, "WERR_TOO_MANY_NAMES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TOO_MANY_NAMES)));
	PyModule_AddObject(m, "WERR_TOO_MANY_SESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TOO_MANY_SESS)));
	PyModule_AddObject(m, "WERR_SHARING_PAUSED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SHARING_PAUSED)));
	PyModule_AddObject(m, "WERR_REQ_NOT_ACCEP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REQ_NOT_ACCEP)));
	PyModule_AddObject(m, "WERR_REDIR_PAUSED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REDIR_PAUSED)));
	PyModule_AddObject(m, "WERR_FILE_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FILE_EXISTS)));
	PyModule_AddObject(m, "WERR_CANNOT_MAKE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANNOT_MAKE)));
	PyModule_AddObject(m, "WERR_FAIL_I24",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FAIL_I24)));
	PyModule_AddObject(m, "WERR_OUT_OF_STRUCTURES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_OUT_OF_STRUCTURES)));
	PyModule_AddObject(m, "WERR_ALREADY_ASSIGNED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ALREADY_ASSIGNED)));
	PyModule_AddObject(m, "WERR_INVALID_PASSWORD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_PASSWORD)));
	PyModule_AddObject(m, "WERR_INVALID_PARAMETER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_PARAMETER)));
	PyModule_AddObject(m, "WERR_NET_WRITE_FAULT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NET_WRITE_FAULT)));
	PyModule_AddObject(m, "WERR_NO_PROC_SLOTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_PROC_SLOTS)));
	PyModule_AddObject(m, "WERR_TOO_MANY_SEMAPHORES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TOO_MANY_SEMAPHORES)));
	PyModule_AddObject(m, "WERR_EXCL_SEM_ALREADY_OWNED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EXCL_SEM_ALREADY_OWNED)));
	PyModule_AddObject(m, "WERR_SEM_IS_SET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SEM_IS_SET)));
	PyModule_AddObject(m, "WERR_TOO_MANY_SEM_REQUESTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TOO_MANY_SEM_REQUESTS)));
	PyModule_AddObject(m, "WERR_INVALID_AT_INTERRUPT_TIME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_AT_INTERRUPT_TIME)));
	PyModule_AddObject(m, "WERR_SEM_OWNER_DIED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SEM_OWNER_DIED)));
	PyModule_AddObject(m, "WERR_SEM_USER_LIMIT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SEM_USER_LIMIT)));
	PyModule_AddObject(m, "WERR_DISK_CHANGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DISK_CHANGE)));
	PyModule_AddObject(m, "WERR_DRIVE_LOCKED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DRIVE_LOCKED)));
	PyModule_AddObject(m, "WERR_BROKEN_PIPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BROKEN_PIPE)));
	PyModule_AddObject(m, "WERR_OPEN_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_OPEN_FAILED)));
	PyModule_AddObject(m, "WERR_BUFFER_OVERFLOW",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BUFFER_OVERFLOW)));
	PyModule_AddObject(m, "WERR_DISK_FULL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DISK_FULL)));
	PyModule_AddObject(m, "WERR_NO_MORE_SEARCH_HANDLES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_MORE_SEARCH_HANDLES)));
	PyModule_AddObject(m, "WERR_INVALID_TARGET_HANDLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_TARGET_HANDLE)));
	PyModule_AddObject(m, "WERR_INVALID_CATEGORY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_CATEGORY)));
	PyModule_AddObject(m, "WERR_INVALID_VERIFY_SWITCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_VERIFY_SWITCH)));
	PyModule_AddObject(m, "WERR_BAD_DRIVER_LEVEL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_DRIVER_LEVEL)));
	PyModule_AddObject(m, "WERR_CALL_NOT_IMPLEMENTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CALL_NOT_IMPLEMENTED)));
	PyModule_AddObject(m, "WERR_SEM_TIMEOUT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SEM_TIMEOUT)));
	PyModule_AddObject(m, "WERR_INSUFFICIENT_BUFFER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSUFFICIENT_BUFFER)));
	PyModule_AddObject(m, "WERR_INVALID_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_NAME)));
	PyModule_AddObject(m, "WERR_INVALID_LEVEL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_LEVEL)));
	PyModule_AddObject(m, "WERR_NO_VOLUME_LABEL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_VOLUME_LABEL)));
	PyModule_AddObject(m, "WERR_MOD_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MOD_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_PROC_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PROC_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_WAIT_NO_CHILDREN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WAIT_NO_CHILDREN)));
	PyModule_AddObject(m, "WERR_CHILD_NOT_COMPLETE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CHILD_NOT_COMPLETE)));
	PyModule_AddObject(m, "WERR_DIRECT_ACCESS_HANDLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DIRECT_ACCESS_HANDLE)));
	PyModule_AddObject(m, "WERR_NEGATIVE_SEEK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NEGATIVE_SEEK)));
	PyModule_AddObject(m, "WERR_SEEK_ON_DEVICE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SEEK_ON_DEVICE)));
	PyModule_AddObject(m, "WERR_IS_JOIN_TARGET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IS_JOIN_TARGET)));
	PyModule_AddObject(m, "WERR_IS_JOINED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IS_JOINED)));
	PyModule_AddObject(m, "WERR_IS_SUBSTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IS_SUBSTED)));
	PyModule_AddObject(m, "WERR_NOT_JOINED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_JOINED)));
	PyModule_AddObject(m, "WERR_NOT_SUBSTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_SUBSTED)));
	PyModule_AddObject(m, "WERR_JOIN_TO_JOIN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_JOIN_TO_JOIN)));
	PyModule_AddObject(m, "WERR_SUBST_TO_SUBST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SUBST_TO_SUBST)));
	PyModule_AddObject(m, "WERR_JOIN_TO_SUBST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_JOIN_TO_SUBST)));
	PyModule_AddObject(m, "WERR_SUBST_TO_JOIN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SUBST_TO_JOIN)));
	PyModule_AddObject(m, "WERR_BUSY_DRIVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BUSY_DRIVE)));
	PyModule_AddObject(m, "WERR_SAME_DRIVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SAME_DRIVE)));
	PyModule_AddObject(m, "WERR_DIR_NOT_ROOT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DIR_NOT_ROOT)));
	PyModule_AddObject(m, "WERR_DIR_NOT_EMPTY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DIR_NOT_EMPTY)));
	PyModule_AddObject(m, "WERR_IS_SUBST_PATH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IS_SUBST_PATH)));
	PyModule_AddObject(m, "WERR_IS_JOIN_PATH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IS_JOIN_PATH)));
	PyModule_AddObject(m, "WERR_PATH_BUSY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PATH_BUSY)));
	PyModule_AddObject(m, "WERR_IS_SUBST_TARGET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IS_SUBST_TARGET)));
	PyModule_AddObject(m, "WERR_SYSTEM_TRACE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SYSTEM_TRACE)));
	PyModule_AddObject(m, "WERR_INVALID_EVENT_COUNT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_EVENT_COUNT)));
	PyModule_AddObject(m, "WERR_TOO_MANY_MUXWAITERS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TOO_MANY_MUXWAITERS)));
	PyModule_AddObject(m, "WERR_INVALID_LIST_FORMAT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_LIST_FORMAT)));
	PyModule_AddObject(m, "WERR_LABEL_TOO_LONG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LABEL_TOO_LONG)));
	PyModule_AddObject(m, "WERR_TOO_MANY_TCBS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TOO_MANY_TCBS)));
	PyModule_AddObject(m, "WERR_SIGNAL_REFUSED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SIGNAL_REFUSED)));
	PyModule_AddObject(m, "WERR_DISCARDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DISCARDED)));
	PyModule_AddObject(m, "WERR_NOT_LOCKED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_LOCKED)));
	PyModule_AddObject(m, "WERR_BAD_THREADID_ADDR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_THREADID_ADDR)));
	PyModule_AddObject(m, "WERR_BAD_ARGUMENTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_ARGUMENTS)));
	PyModule_AddObject(m, "WERR_BAD_PATHNAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_PATHNAME)));
	PyModule_AddObject(m, "WERR_SIGNAL_PENDING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SIGNAL_PENDING)));
	PyModule_AddObject(m, "WERR_MAX_THRDS_REACHED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MAX_THRDS_REACHED)));
	PyModule_AddObject(m, "WERR_LOCK_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOCK_FAILED)));
	PyModule_AddObject(m, "WERR_BUSY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BUSY)));
	PyModule_AddObject(m, "WERR_CANCEL_VIOLATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANCEL_VIOLATION)));
	PyModule_AddObject(m, "WERR_ATOMIC_LOCKS_NOT_SUPPORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ATOMIC_LOCKS_NOT_SUPPORTED)));
	PyModule_AddObject(m, "WERR_INVALID_SEGMENT_NUMBER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_SEGMENT_NUMBER)));
	PyModule_AddObject(m, "WERR_INVALID_ORDINAL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_ORDINAL)));
	PyModule_AddObject(m, "WERR_ALREADY_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ALREADY_EXISTS)));
	PyModule_AddObject(m, "WERR_INVALID_FLAG_NUMBER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_FLAG_NUMBER)));
	PyModule_AddObject(m, "WERR_SEM_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SEM_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_INVALID_STARTING_CODESEG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_STARTING_CODESEG)));
	PyModule_AddObject(m, "WERR_INVALID_STACKSEG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_STACKSEG)));
	PyModule_AddObject(m, "WERR_INVALID_MODULETYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_MODULETYPE)));
	PyModule_AddObject(m, "WERR_INVALID_EXE_SIGNATURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_EXE_SIGNATURE)));
	PyModule_AddObject(m, "WERR_EXE_MARKED_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EXE_MARKED_INVALID)));
	PyModule_AddObject(m, "WERR_BAD_EXE_FORMAT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_EXE_FORMAT)));
	PyModule_AddObject(m, "WERR_ITERATED_DATA_EXCEEDS_64K",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ITERATED_DATA_EXCEEDS_64K)));
	PyModule_AddObject(m, "WERR_INVALID_MINALLOCSIZE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_MINALLOCSIZE)));
	PyModule_AddObject(m, "WERR_DYNLINK_FROM_INVALID_RING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DYNLINK_FROM_INVALID_RING)));
	PyModule_AddObject(m, "WERR_IOPL_NOT_ENABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IOPL_NOT_ENABLED)));
	PyModule_AddObject(m, "WERR_INVALID_SEGDPL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_SEGDPL)));
	PyModule_AddObject(m, "WERR_AUTODATASEG_EXCEEDS_64K",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_AUTODATASEG_EXCEEDS_64K)));
	PyModule_AddObject(m, "WERR_RING2SEG_MUST_BE_MOVABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RING2SEG_MUST_BE_MOVABLE)));
	PyModule_AddObject(m, "WERR_RELOC_CHAIN_XEEDS_SEGLIM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RELOC_CHAIN_XEEDS_SEGLIM)));
	PyModule_AddObject(m, "WERR_INFLOOP_IN_RELOC_CHAIN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INFLOOP_IN_RELOC_CHAIN)));
	PyModule_AddObject(m, "WERR_ENVVAR_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ENVVAR_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_NO_SIGNAL_SENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_SIGNAL_SENT)));
	PyModule_AddObject(m, "WERR_FILENAME_EXCED_RANGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FILENAME_EXCED_RANGE)));
	PyModule_AddObject(m, "WERR_RING2_STACK_IN_USE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RING2_STACK_IN_USE)));
	PyModule_AddObject(m, "WERR_META_EXPANSION_TOO_LONG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_META_EXPANSION_TOO_LONG)));
	PyModule_AddObject(m, "WERR_INVALID_SIGNAL_NUMBER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_SIGNAL_NUMBER)));
	PyModule_AddObject(m, "WERR_THREAD_1_INACTIVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_THREAD_1_INACTIVE)));
	PyModule_AddObject(m, "WERR_LOCKED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOCKED)));
	PyModule_AddObject(m, "WERR_TOO_MANY_MODULES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TOO_MANY_MODULES)));
	PyModule_AddObject(m, "WERR_NESTING_NOT_ALLOWED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NESTING_NOT_ALLOWED)));
	PyModule_AddObject(m, "WERR_EXE_MACHINE_TYPE_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EXE_MACHINE_TYPE_MISMATCH)));
	PyModule_AddObject(m, "WERR_EXE_CANNOT_MODIFY_SIGNED_BINARY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EXE_CANNOT_MODIFY_SIGNED_BINARY)));
	PyModule_AddObject(m, "WERR_EXE_CANNOT_MODIFY_STRONG_SIGNED_BINARY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EXE_CANNOT_MODIFY_STRONG_SIGNED_BINARY)));
	PyModule_AddObject(m, "WERR_FILE_CHECKED_OUT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FILE_CHECKED_OUT)));
	PyModule_AddObject(m, "WERR_CHECKOUT_REQUIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CHECKOUT_REQUIRED)));
	PyModule_AddObject(m, "WERR_BAD_FILE_TYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_FILE_TYPE)));
	PyModule_AddObject(m, "WERR_FILE_TOO_LARGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FILE_TOO_LARGE)));
	PyModule_AddObject(m, "WERR_FORMS_AUTH_REQUIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FORMS_AUTH_REQUIRED)));
	PyModule_AddObject(m, "WERR_VIRUS_INFECTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_VIRUS_INFECTED)));
	PyModule_AddObject(m, "WERR_VIRUS_DELETED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_VIRUS_DELETED)));
	PyModule_AddObject(m, "WERR_PIPE_LOCAL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PIPE_LOCAL)));
	PyModule_AddObject(m, "WERR_BAD_PIPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_PIPE)));
	PyModule_AddObject(m, "WERR_PIPE_BUSY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PIPE_BUSY)));
	PyModule_AddObject(m, "WERR_NO_DATA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_DATA)));
	PyModule_AddObject(m, "WERR_PIPE_NOT_CONNECTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PIPE_NOT_CONNECTED)));
	PyModule_AddObject(m, "WERR_MORE_DATA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MORE_DATA)));
	PyModule_AddObject(m, "WERR_VC_DISCONNECTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_VC_DISCONNECTED)));
	PyModule_AddObject(m, "WERR_INVALID_EA_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_EA_NAME)));
	PyModule_AddObject(m, "WERR_EA_LIST_INCONSISTENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EA_LIST_INCONSISTENT)));
	PyModule_AddObject(m, "WERR_WAIT_TIMEOUT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WAIT_TIMEOUT)));
	PyModule_AddObject(m, "WERR_NO_MORE_ITEMS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_MORE_ITEMS)));
	PyModule_AddObject(m, "WERR_CANNOT_COPY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANNOT_COPY)));
	PyModule_AddObject(m, "WERR_DIRECTORY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DIRECTORY)));
	PyModule_AddObject(m, "WERR_EAS_DIDNT_FIT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EAS_DIDNT_FIT)));
	PyModule_AddObject(m, "WERR_EA_FILE_CORRUPT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EA_FILE_CORRUPT)));
	PyModule_AddObject(m, "WERR_EA_TABLE_FULL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EA_TABLE_FULL)));
	PyModule_AddObject(m, "WERR_INVALID_EA_HANDLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_EA_HANDLE)));
	PyModule_AddObject(m, "WERR_EAS_NOT_SUPPORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EAS_NOT_SUPPORTED)));
	PyModule_AddObject(m, "WERR_NOT_OWNER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_OWNER)));
	PyModule_AddObject(m, "WERR_TOO_MANY_POSTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TOO_MANY_POSTS)));
	PyModule_AddObject(m, "WERR_PARTIAL_COPY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PARTIAL_COPY)));
	PyModule_AddObject(m, "WERR_OPLOCK_NOT_GRANTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_OPLOCK_NOT_GRANTED)));
	PyModule_AddObject(m, "WERR_INVALID_OPLOCK_PROTOCOL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_OPLOCK_PROTOCOL)));
	PyModule_AddObject(m, "WERR_DISK_TOO_FRAGMENTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DISK_TOO_FRAGMENTED)));
	PyModule_AddObject(m, "WERR_DELETE_PENDING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DELETE_PENDING)));
	PyModule_AddObject(m, "WERR_MR_MID_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MR_MID_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_SCOPE_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SCOPE_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_FAIL_NOACTION_REBOOT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FAIL_NOACTION_REBOOT)));
	PyModule_AddObject(m, "WERR_FAIL_SHUTDOWN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FAIL_SHUTDOWN)));
	PyModule_AddObject(m, "WERR_FAIL_RESTART",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FAIL_RESTART)));
	PyModule_AddObject(m, "WERR_MAX_SESSIONS_REACHED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MAX_SESSIONS_REACHED)));
	PyModule_AddObject(m, "WERR_THREAD_MODE_ALREADY_BACKGROUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_THREAD_MODE_ALREADY_BACKGROUND)));
	PyModule_AddObject(m, "WERR_THREAD_MODE_NOT_BACKGROUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_THREAD_MODE_NOT_BACKGROUND)));
	PyModule_AddObject(m, "WERR_PROCESS_MODE_ALREADY_BACKGROUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PROCESS_MODE_ALREADY_BACKGROUND)));
	PyModule_AddObject(m, "WERR_PROCESS_MODE_NOT_BACKGROUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PROCESS_MODE_NOT_BACKGROUND)));
	PyModule_AddObject(m, "WERR_INVALID_ADDRESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_ADDRESS)));
	PyModule_AddObject(m, "WERR_USER_PROFILE_LOAD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_USER_PROFILE_LOAD)));
	PyModule_AddObject(m, "WERR_ARITHMETIC_OVERFLOW",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ARITHMETIC_OVERFLOW)));
	PyModule_AddObject(m, "WERR_PIPE_CONNECTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PIPE_CONNECTED)));
	PyModule_AddObject(m, "WERR_PIPE_LISTENING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PIPE_LISTENING)));
	PyModule_AddObject(m, "WERR_VERIFIER_STOP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_VERIFIER_STOP)));
	PyModule_AddObject(m, "WERR_ABIOS_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ABIOS_ERROR)));
	PyModule_AddObject(m, "WERR_WX86_WARNING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WX86_WARNING)));
	PyModule_AddObject(m, "WERR_WX86_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WX86_ERROR)));
	PyModule_AddObject(m, "WERR_TIMER_NOT_CANCELED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TIMER_NOT_CANCELED)));
	PyModule_AddObject(m, "WERR_UNWIND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNWIND)));
	PyModule_AddObject(m, "WERR_BAD_STACK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_STACK)));
	PyModule_AddObject(m, "WERR_INVALID_UNWIND_TARGET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_UNWIND_TARGET)));
	PyModule_AddObject(m, "WERR_INVALID_PORT_ATTRIBUTES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_PORT_ATTRIBUTES)));
	PyModule_AddObject(m, "WERR_PORT_MESSAGE_TOO_LONG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PORT_MESSAGE_TOO_LONG)));
	PyModule_AddObject(m, "WERR_INVALID_QUOTA_LOWER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_QUOTA_LOWER)));
	PyModule_AddObject(m, "WERR_DEVICE_ALREADY_ATTACHED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DEVICE_ALREADY_ATTACHED)));
	PyModule_AddObject(m, "WERR_INSTRUCTION_MISALIGNMENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSTRUCTION_MISALIGNMENT)));
	PyModule_AddObject(m, "WERR_PROFILING_NOT_STARTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PROFILING_NOT_STARTED)));
	PyModule_AddObject(m, "WERR_PROFILING_NOT_STOPPED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PROFILING_NOT_STOPPED)));
	PyModule_AddObject(m, "WERR_COULD_NOT_INTERPRET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_COULD_NOT_INTERPRET)));
	PyModule_AddObject(m, "WERR_PROFILING_AT_LIMIT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PROFILING_AT_LIMIT)));
	PyModule_AddObject(m, "WERR_CANT_WAIT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANT_WAIT)));
	PyModule_AddObject(m, "WERR_CANT_TERMINATE_SELF",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANT_TERMINATE_SELF)));
	PyModule_AddObject(m, "WERR_UNEXPECTED_MM_CREATE_ERR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNEXPECTED_MM_CREATE_ERR)));
	PyModule_AddObject(m, "WERR_UNEXPECTED_MM_MAP_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNEXPECTED_MM_MAP_ERROR)));
	PyModule_AddObject(m, "WERR_UNEXPECTED_MM_EXTEND_ERR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNEXPECTED_MM_EXTEND_ERR)));
	PyModule_AddObject(m, "WERR_BAD_FUNCTION_TABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_FUNCTION_TABLE)));
	PyModule_AddObject(m, "WERR_NO_GUID_TRANSLATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_GUID_TRANSLATION)));
	PyModule_AddObject(m, "WERR_INVALID_LDT_SIZE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_LDT_SIZE)));
	PyModule_AddObject(m, "WERR_INVALID_LDT_OFFSET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_LDT_OFFSET)));
	PyModule_AddObject(m, "WERR_INVALID_LDT_DESCRIPTOR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_LDT_DESCRIPTOR)));
	PyModule_AddObject(m, "WERR_TOO_MANY_THREADS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TOO_MANY_THREADS)));
	PyModule_AddObject(m, "WERR_THREAD_NOT_IN_PROCESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_THREAD_NOT_IN_PROCESS)));
	PyModule_AddObject(m, "WERR_PAGEFILE_QUOTA_EXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PAGEFILE_QUOTA_EXCEEDED)));
	PyModule_AddObject(m, "WERR_LOGON_SERVER_CONFLICT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOGON_SERVER_CONFLICT)));
	PyModule_AddObject(m, "WERR_SYNCHRONIZATION_REQUIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SYNCHRONIZATION_REQUIRED)));
	PyModule_AddObject(m, "WERR_NET_OPEN_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NET_OPEN_FAILED)));
	PyModule_AddObject(m, "WERR_IO_PRIVILEGE_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IO_PRIVILEGE_FAILED)));
	PyModule_AddObject(m, "WERR_CONTROL_C_EXIT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CONTROL_C_EXIT)));
	PyModule_AddObject(m, "WERR_MISSING_SYSTEMFILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MISSING_SYSTEMFILE)));
	PyModule_AddObject(m, "WERR_UNHANDLED_EXCEPTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNHANDLED_EXCEPTION)));
	PyModule_AddObject(m, "WERR_APP_INIT_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_APP_INIT_FAILURE)));
	PyModule_AddObject(m, "WERR_PAGEFILE_CREATE_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PAGEFILE_CREATE_FAILED)));
	PyModule_AddObject(m, "WERR_INVALID_IMAGE_HASH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_IMAGE_HASH)));
	PyModule_AddObject(m, "WERR_NO_PAGEFILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_PAGEFILE)));
	PyModule_AddObject(m, "WERR_ILLEGAL_FLOAT_CONTEXT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ILLEGAL_FLOAT_CONTEXT)));
	PyModule_AddObject(m, "WERR_NO_EVENT_PAIR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_EVENT_PAIR)));
	PyModule_AddObject(m, "WERR_DOMAIN_CTRLR_CONFIG_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DOMAIN_CTRLR_CONFIG_ERROR)));
	PyModule_AddObject(m, "WERR_ILLEGAL_CHARACTER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ILLEGAL_CHARACTER)));
	PyModule_AddObject(m, "WERR_UNDEFINED_CHARACTER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNDEFINED_CHARACTER)));
	PyModule_AddObject(m, "WERR_FLOPPY_VOLUME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FLOPPY_VOLUME)));
	PyModule_AddObject(m, "WERR_BIOS_FAILED_TO_CONNECT_INTERRUPT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BIOS_FAILED_TO_CONNECT_INTERRUPT)));
	PyModule_AddObject(m, "WERR_BACKUP_CONTROLLER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BACKUP_CONTROLLER)));
	PyModule_AddObject(m, "WERR_MUTANT_LIMIT_EXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MUTANT_LIMIT_EXCEEDED)));
	PyModule_AddObject(m, "WERR_FS_DRIVER_REQUIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FS_DRIVER_REQUIRED)));
	PyModule_AddObject(m, "WERR_CANNOT_LOAD_REGISTRY_FILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANNOT_LOAD_REGISTRY_FILE)));
	PyModule_AddObject(m, "WERR_DEBUG_ATTACH_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DEBUG_ATTACH_FAILED)));
	PyModule_AddObject(m, "WERR_SYSTEM_PROCESS_TERMINATED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SYSTEM_PROCESS_TERMINATED)));
	PyModule_AddObject(m, "WERR_DATA_NOT_ACCEPTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DATA_NOT_ACCEPTED)));
	PyModule_AddObject(m, "WERR_VDM_HARD_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_VDM_HARD_ERROR)));
	PyModule_AddObject(m, "WERR_DRIVER_CANCEL_TIMEOUT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DRIVER_CANCEL_TIMEOUT)));
	PyModule_AddObject(m, "WERR_REPLY_MESSAGE_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REPLY_MESSAGE_MISMATCH)));
	PyModule_AddObject(m, "WERR_LOST_WRITEBEHIND_DATA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOST_WRITEBEHIND_DATA)));
	PyModule_AddObject(m, "WERR_CLIENT_SERVER_PARAMETERS_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLIENT_SERVER_PARAMETERS_INVALID)));
	PyModule_AddObject(m, "WERR_NOT_TINY_STREAM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_TINY_STREAM)));
	PyModule_AddObject(m, "WERR_STACK_OVERFLOW_READ",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_STACK_OVERFLOW_READ)));
	PyModule_AddObject(m, "WERR_CONVERT_TO_LARGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CONVERT_TO_LARGE)));
	PyModule_AddObject(m, "WERR_FOUND_OUT_OF_SCOPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FOUND_OUT_OF_SCOPE)));
	PyModule_AddObject(m, "WERR_ALLOCATE_BUCKET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ALLOCATE_BUCKET)));
	PyModule_AddObject(m, "WERR_MARSHALL_OVERFLOW",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MARSHALL_OVERFLOW)));
	PyModule_AddObject(m, "WERR_INVALID_VARIANT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_VARIANT)));
	PyModule_AddObject(m, "WERR_BAD_COMPRESSION_BUFFER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_COMPRESSION_BUFFER)));
	PyModule_AddObject(m, "WERR_AUDIT_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_AUDIT_FAILED)));
	PyModule_AddObject(m, "WERR_TIMER_RESOLUTION_NOT_SET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TIMER_RESOLUTION_NOT_SET)));
	PyModule_AddObject(m, "WERR_INSUFFICIENT_LOGON_INFO",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSUFFICIENT_LOGON_INFO)));
	PyModule_AddObject(m, "WERR_BAD_DLL_ENTRYPOINT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_DLL_ENTRYPOINT)));
	PyModule_AddObject(m, "WERR_BAD_SERVICE_ENTRYPOINT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_SERVICE_ENTRYPOINT)));
	PyModule_AddObject(m, "WERR_IP_ADDRESS_CONFLICT1",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IP_ADDRESS_CONFLICT1)));
	PyModule_AddObject(m, "WERR_IP_ADDRESS_CONFLICT2",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IP_ADDRESS_CONFLICT2)));
	PyModule_AddObject(m, "WERR_REGISTRY_QUOTA_LIMIT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REGISTRY_QUOTA_LIMIT)));
	PyModule_AddObject(m, "WERR_NO_CALLBACK_ACTIVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_CALLBACK_ACTIVE)));
	PyModule_AddObject(m, "WERR_PWD_TOO_SHORT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PWD_TOO_SHORT)));
	PyModule_AddObject(m, "WERR_PWD_TOO_RECENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PWD_TOO_RECENT)));
	PyModule_AddObject(m, "WERR_PWD_HISTORY_CONFLICT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PWD_HISTORY_CONFLICT)));
	PyModule_AddObject(m, "WERR_UNSUPPORTED_COMPRESSION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNSUPPORTED_COMPRESSION)));
	PyModule_AddObject(m, "WERR_INVALID_HW_PROFILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_HW_PROFILE)));
	PyModule_AddObject(m, "WERR_INVALID_PLUGPLAY_DEVICE_PATH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_PLUGPLAY_DEVICE_PATH)));
	PyModule_AddObject(m, "WERR_QUOTA_LIST_INCONSISTENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_QUOTA_LIST_INCONSISTENT)));
	PyModule_AddObject(m, "WERR_EVALUATION_EXPIRATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVALUATION_EXPIRATION)));
	PyModule_AddObject(m, "WERR_ILLEGAL_DLL_RELOCATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ILLEGAL_DLL_RELOCATION)));
	PyModule_AddObject(m, "WERR_DLL_INIT_FAILED_LOGOFF",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DLL_INIT_FAILED_LOGOFF)));
	PyModule_AddObject(m, "WERR_VALIDATE_CONTINUE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_VALIDATE_CONTINUE)));
	PyModule_AddObject(m, "WERR_NO_MORE_MATCHES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_MORE_MATCHES)));
	PyModule_AddObject(m, "WERR_RANGE_LIST_CONFLICT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RANGE_LIST_CONFLICT)));
	PyModule_AddObject(m, "WERR_SERVER_SID_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERVER_SID_MISMATCH)));
	PyModule_AddObject(m, "WERR_CANT_ENABLE_DENY_ONLY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANT_ENABLE_DENY_ONLY)));
	PyModule_AddObject(m, "WERR_FLOAT_MULTIPLE_FAULTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FLOAT_MULTIPLE_FAULTS)));
	PyModule_AddObject(m, "WERR_FLOAT_MULTIPLE_TRAPS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FLOAT_MULTIPLE_TRAPS)));
	PyModule_AddObject(m, "WERR_NOINTERFACE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOINTERFACE)));
	PyModule_AddObject(m, "WERR_DRIVER_FAILED_SLEEP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DRIVER_FAILED_SLEEP)));
	PyModule_AddObject(m, "WERR_CORRUPT_SYSTEM_FILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CORRUPT_SYSTEM_FILE)));
	PyModule_AddObject(m, "WERR_COMMITMENT_MINIMUM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_COMMITMENT_MINIMUM)));
	PyModule_AddObject(m, "WERR_PNP_RESTART_ENUMERATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PNP_RESTART_ENUMERATION)));
	PyModule_AddObject(m, "WERR_SYSTEM_IMAGE_BAD_SIGNATURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SYSTEM_IMAGE_BAD_SIGNATURE)));
	PyModule_AddObject(m, "WERR_PNP_REBOOT_REQUIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PNP_REBOOT_REQUIRED)));
	PyModule_AddObject(m, "WERR_INSUFFICIENT_POWER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSUFFICIENT_POWER)));
	PyModule_AddObject(m, "WERR_SYSTEM_SHUTDOWN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SYSTEM_SHUTDOWN)));
	PyModule_AddObject(m, "WERR_PORT_NOT_SET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PORT_NOT_SET)));
	PyModule_AddObject(m, "WERR_DS_VERSION_CHECK_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_VERSION_CHECK_FAILURE)));
	PyModule_AddObject(m, "WERR_RANGE_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RANGE_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_NOT_SAFE_MODE_DRIVER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_SAFE_MODE_DRIVER)));
	PyModule_AddObject(m, "WERR_FAILED_DRIVER_ENTRY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FAILED_DRIVER_ENTRY)));
	PyModule_AddObject(m, "WERR_DEVICE_ENUMERATION_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DEVICE_ENUMERATION_ERROR)));
	PyModule_AddObject(m, "WERR_MOUNT_POINT_NOT_RESOLVED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MOUNT_POINT_NOT_RESOLVED)));
	PyModule_AddObject(m, "WERR_INVALID_DEVICE_OBJECT_PARAMETER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_DEVICE_OBJECT_PARAMETER)));
	PyModule_AddObject(m, "WERR_MCA_OCCURED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MCA_OCCURED)));
	PyModule_AddObject(m, "WERR_DRIVER_DATABASE_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DRIVER_DATABASE_ERROR)));
	PyModule_AddObject(m, "WERR_SYSTEM_HIVE_TOO_LARGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SYSTEM_HIVE_TOO_LARGE)));
	PyModule_AddObject(m, "WERR_DRIVER_FAILED_PRIOR_UNLOAD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DRIVER_FAILED_PRIOR_UNLOAD)));
	PyModule_AddObject(m, "WERR_VOLSNAP_PREPARE_HIBERNATE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_VOLSNAP_PREPARE_HIBERNATE)));
	PyModule_AddObject(m, "WERR_HIBERNATION_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_HIBERNATION_FAILURE)));
	PyModule_AddObject(m, "WERR_FILE_SYSTEM_LIMITATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FILE_SYSTEM_LIMITATION)));
	PyModule_AddObject(m, "WERR_ASSERTION_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ASSERTION_FAILURE)));
	PyModule_AddObject(m, "WERR_ACPI_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ACPI_ERROR)));
	PyModule_AddObject(m, "WERR_WOW_ASSERTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WOW_ASSERTION)));
	PyModule_AddObject(m, "WERR_PNP_BAD_MPS_TABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PNP_BAD_MPS_TABLE)));
	PyModule_AddObject(m, "WERR_PNP_TRANSLATION_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PNP_TRANSLATION_FAILED)));
	PyModule_AddObject(m, "WERR_PNP_IRQ_TRANSLATION_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PNP_IRQ_TRANSLATION_FAILED)));
	PyModule_AddObject(m, "WERR_PNP_INVALID_ID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PNP_INVALID_ID)));
	PyModule_AddObject(m, "WERR_WAKE_SYSTEM_DEBUGGER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WAKE_SYSTEM_DEBUGGER)));
	PyModule_AddObject(m, "WERR_HANDLES_CLOSED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_HANDLES_CLOSED)));
	PyModule_AddObject(m, "WERR_EXTRANEOUS_INFORMATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EXTRANEOUS_INFORMATION)));
	PyModule_AddObject(m, "WERR_RXACT_COMMIT_NECESSARY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RXACT_COMMIT_NECESSARY)));
	PyModule_AddObject(m, "WERR_MEDIA_CHECK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MEDIA_CHECK)));
	PyModule_AddObject(m, "WERR_GUID_SUBSTITUTION_MADE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_GUID_SUBSTITUTION_MADE)));
	PyModule_AddObject(m, "WERR_STOPPED_ON_SYMLINK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_STOPPED_ON_SYMLINK)));
	PyModule_AddObject(m, "WERR_LONGJUMP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LONGJUMP)));
	PyModule_AddObject(m, "WERR_PLUGPLAY_QUERY_VETOED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PLUGPLAY_QUERY_VETOED)));
	PyModule_AddObject(m, "WERR_UNWIND_CONSOLIDATE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNWIND_CONSOLIDATE)));
	PyModule_AddObject(m, "WERR_REGISTRY_HIVE_RECOVERED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REGISTRY_HIVE_RECOVERED)));
	PyModule_AddObject(m, "WERR_DLL_MIGHT_BE_INSECURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DLL_MIGHT_BE_INSECURE)));
	PyModule_AddObject(m, "WERR_DLL_MIGHT_BE_INCOMPATIBLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DLL_MIGHT_BE_INCOMPATIBLE)));
	PyModule_AddObject(m, "WERR_DBG_EXCEPTION_NOT_HANDLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DBG_EXCEPTION_NOT_HANDLED)));
	PyModule_AddObject(m, "WERR_DBG_REPLY_LATER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DBG_REPLY_LATER)));
	PyModule_AddObject(m, "WERR_DBG_UNABLE_TO_PROVIDE_HANDLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DBG_UNABLE_TO_PROVIDE_HANDLE)));
	PyModule_AddObject(m, "WERR_DBG_TERMINATE_THREAD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DBG_TERMINATE_THREAD)));
	PyModule_AddObject(m, "WERR_DBG_TERMINATE_PROCESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DBG_TERMINATE_PROCESS)));
	PyModule_AddObject(m, "WERR_DBG_CONTROL_C",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DBG_CONTROL_C)));
	PyModule_AddObject(m, "WERR_DBG_PRINTEXCEPTION_C",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DBG_PRINTEXCEPTION_C)));
	PyModule_AddObject(m, "WERR_DBG_RIPEXCEPTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DBG_RIPEXCEPTION)));
	PyModule_AddObject(m, "WERR_DBG_CONTROL_BREAK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DBG_CONTROL_BREAK)));
	PyModule_AddObject(m, "WERR_DBG_COMMAND_EXCEPTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DBG_COMMAND_EXCEPTION)));
	PyModule_AddObject(m, "WERR_OBJECT_NAME_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_OBJECT_NAME_EXISTS)));
	PyModule_AddObject(m, "WERR_THREAD_WAS_SUSPENDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_THREAD_WAS_SUSPENDED)));
	PyModule_AddObject(m, "WERR_IMAGE_NOT_AT_BASE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IMAGE_NOT_AT_BASE)));
	PyModule_AddObject(m, "WERR_RXACT_STATE_CREATED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RXACT_STATE_CREATED)));
	PyModule_AddObject(m, "WERR_SEGMENT_NOTIFICATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SEGMENT_NOTIFICATION)));
	PyModule_AddObject(m, "WERR_BAD_CURRENT_DIRECTORY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_CURRENT_DIRECTORY)));
	PyModule_AddObject(m, "WERR_FT_READ_RECOVERY_FROM_BACKUP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FT_READ_RECOVERY_FROM_BACKUP)));
	PyModule_AddObject(m, "WERR_FT_WRITE_RECOVERY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FT_WRITE_RECOVERY)));
	PyModule_AddObject(m, "WERR_IMAGE_MACHINE_TYPE_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IMAGE_MACHINE_TYPE_MISMATCH)));
	PyModule_AddObject(m, "WERR_RECEIVE_PARTIAL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RECEIVE_PARTIAL)));
	PyModule_AddObject(m, "WERR_RECEIVE_EXPEDITED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RECEIVE_EXPEDITED)));
	PyModule_AddObject(m, "WERR_RECEIVE_PARTIAL_EXPEDITED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RECEIVE_PARTIAL_EXPEDITED)));
	PyModule_AddObject(m, "WERR_EVENT_DONE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVENT_DONE)));
	PyModule_AddObject(m, "WERR_EVENT_PENDING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVENT_PENDING)));
	PyModule_AddObject(m, "WERR_CHECKING_FILE_SYSTEM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CHECKING_FILE_SYSTEM)));
	PyModule_AddObject(m, "WERR_FATAL_APP_EXIT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FATAL_APP_EXIT)));
	PyModule_AddObject(m, "WERR_PREDEFINED_HANDLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PREDEFINED_HANDLE)));
	PyModule_AddObject(m, "WERR_WAS_UNLOCKED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WAS_UNLOCKED)));
	PyModule_AddObject(m, "WERR_WAS_LOCKED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WAS_LOCKED)));
	PyModule_AddObject(m, "WERR_ALREADY_WIN32",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ALREADY_WIN32)));
	PyModule_AddObject(m, "WERR_IMAGE_MACHINE_TYPE_MISMATCH_EXE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IMAGE_MACHINE_TYPE_MISMATCH_EXE)));
	PyModule_AddObject(m, "WERR_NO_YIELD_PERFORMED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_YIELD_PERFORMED)));
	PyModule_AddObject(m, "WERR_TIMER_RESUME_IGNORED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TIMER_RESUME_IGNORED)));
	PyModule_AddObject(m, "WERR_ARBITRATION_UNHANDLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ARBITRATION_UNHANDLED)));
	PyModule_AddObject(m, "WERR_CARDBUS_NOT_SUPPORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CARDBUS_NOT_SUPPORTED)));
	PyModule_AddObject(m, "WERR_MP_PROCESSOR_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MP_PROCESSOR_MISMATCH)));
	PyModule_AddObject(m, "WERR_HIBERNATED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_HIBERNATED)));
	PyModule_AddObject(m, "WERR_RESUME_HIBERNATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RESUME_HIBERNATION)));
	PyModule_AddObject(m, "WERR_FIRMWARE_UPDATED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FIRMWARE_UPDATED)));
	PyModule_AddObject(m, "WERR_DRIVERS_LEAKING_LOCKED_PAGES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DRIVERS_LEAKING_LOCKED_PAGES)));
	PyModule_AddObject(m, "WERR_WAKE_SYSTEM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WAKE_SYSTEM)));
	PyModule_AddObject(m, "WERR_ABANDONED_WAIT_0",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ABANDONED_WAIT_0)));
	PyModule_AddObject(m, "WERR_ELEVATION_REQUIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ELEVATION_REQUIRED)));
	PyModule_AddObject(m, "WERR_REPARSE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REPARSE)));
	PyModule_AddObject(m, "WERR_OPLOCK_BREAK_IN_PROGRESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_OPLOCK_BREAK_IN_PROGRESS)));
	PyModule_AddObject(m, "WERR_VOLUME_MOUNTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_VOLUME_MOUNTED)));
	PyModule_AddObject(m, "WERR_RXACT_COMMITTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RXACT_COMMITTED)));
	PyModule_AddObject(m, "WERR_NOTIFY_CLEANUP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOTIFY_CLEANUP)));
	PyModule_AddObject(m, "WERR_PRIMARY_TRANSPORT_CONNECT_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PRIMARY_TRANSPORT_CONNECT_FAILED)));
	PyModule_AddObject(m, "WERR_PAGE_FAULT_TRANSITION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PAGE_FAULT_TRANSITION)));
	PyModule_AddObject(m, "WERR_PAGE_FAULT_DEMAND_ZERO",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PAGE_FAULT_DEMAND_ZERO)));
	PyModule_AddObject(m, "WERR_PAGE_FAULT_COPY_ON_WRITE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PAGE_FAULT_COPY_ON_WRITE)));
	PyModule_AddObject(m, "WERR_PAGE_FAULT_GUARD_PAGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PAGE_FAULT_GUARD_PAGE)));
	PyModule_AddObject(m, "WERR_PAGE_FAULT_PAGING_FILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PAGE_FAULT_PAGING_FILE)));
	PyModule_AddObject(m, "WERR_CACHE_PAGE_LOCKED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CACHE_PAGE_LOCKED)));
	PyModule_AddObject(m, "WERR_CRASH_DUMP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CRASH_DUMP)));
	PyModule_AddObject(m, "WERR_BUFFER_ALL_ZEROS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BUFFER_ALL_ZEROS)));
	PyModule_AddObject(m, "WERR_REPARSE_OBJECT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REPARSE_OBJECT)));
	PyModule_AddObject(m, "WERR_RESOURCE_REQUIREMENTS_CHANGED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RESOURCE_REQUIREMENTS_CHANGED)));
	PyModule_AddObject(m, "WERR_TRANSLATION_COMPLETE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRANSLATION_COMPLETE)));
	PyModule_AddObject(m, "WERR_NOTHING_TO_TERMINATE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOTHING_TO_TERMINATE)));
	PyModule_AddObject(m, "WERR_PROCESS_NOT_IN_JOB",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PROCESS_NOT_IN_JOB)));
	PyModule_AddObject(m, "WERR_PROCESS_IN_JOB",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PROCESS_IN_JOB)));
	PyModule_AddObject(m, "WERR_VOLSNAP_HIBERNATE_READY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_VOLSNAP_HIBERNATE_READY)));
	PyModule_AddObject(m, "WERR_FSFILTER_OP_COMPLETED_SUCCESSFULLY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FSFILTER_OP_COMPLETED_SUCCESSFULLY)));
	PyModule_AddObject(m, "WERR_INTERRUPT_VECTOR_ALREADY_CONNECTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INTERRUPT_VECTOR_ALREADY_CONNECTED)));
	PyModule_AddObject(m, "WERR_INTERRUPT_STILL_CONNECTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INTERRUPT_STILL_CONNECTED)));
	PyModule_AddObject(m, "WERR_WAIT_FOR_OPLOCK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WAIT_FOR_OPLOCK)));
	PyModule_AddObject(m, "WERR_DBG_EXCEPTION_HANDLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DBG_EXCEPTION_HANDLED)));
	PyModule_AddObject(m, "WERR_DBG_CONTINUE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DBG_CONTINUE)));
	PyModule_AddObject(m, "WERR_CALLBACK_POP_STACK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CALLBACK_POP_STACK)));
	PyModule_AddObject(m, "WERR_COMPRESSION_DISABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_COMPRESSION_DISABLED)));
	PyModule_AddObject(m, "WERR_CANTFETCHBACKWARDS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANTFETCHBACKWARDS)));
	PyModule_AddObject(m, "WERR_CANTSCROLLBACKWARDS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANTSCROLLBACKWARDS)));
	PyModule_AddObject(m, "WERR_ROWSNOTRELEASED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ROWSNOTRELEASED)));
	PyModule_AddObject(m, "WERR_BAD_ACCESSOR_FLAGS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_ACCESSOR_FLAGS)));
	PyModule_AddObject(m, "WERR_ERRORS_ENCOUNTERED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ERRORS_ENCOUNTERED)));
	PyModule_AddObject(m, "WERR_NOT_CAPABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_CAPABLE)));
	PyModule_AddObject(m, "WERR_REQUEST_OUT_OF_SEQUENCE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REQUEST_OUT_OF_SEQUENCE)));
	PyModule_AddObject(m, "WERR_VERSION_PARSE_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_VERSION_PARSE_ERROR)));
	PyModule_AddObject(m, "WERR_BADSTARTPOSITION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BADSTARTPOSITION)));
	PyModule_AddObject(m, "WERR_MEMORY_HARDWARE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MEMORY_HARDWARE)));
	PyModule_AddObject(m, "WERR_DISK_REPAIR_DISABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DISK_REPAIR_DISABLED)));
	PyModule_AddObject(m, "WERR_INSUFFICIENT_RESOURCE_FOR_SPECIFIED_SHARED_SECTION_SIZE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSUFFICIENT_RESOURCE_FOR_SPECIFIED_SHARED_SECTION_SIZE)));
	PyModule_AddObject(m, "WERR_SYSTEM_POWERSTATE_TRANSITION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SYSTEM_POWERSTATE_TRANSITION)));
	PyModule_AddObject(m, "WERR_SYSTEM_POWERSTATE_COMPLEX_TRANSITION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SYSTEM_POWERSTATE_COMPLEX_TRANSITION)));
	PyModule_AddObject(m, "WERR_MCA_EXCEPTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MCA_EXCEPTION)));
	PyModule_AddObject(m, "WERR_ACCESS_AUDIT_BY_POLICY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ACCESS_AUDIT_BY_POLICY)));
	PyModule_AddObject(m, "WERR_ACCESS_DISABLED_NO_SAFER_UI_BY_POLICY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ACCESS_DISABLED_NO_SAFER_UI_BY_POLICY)));
	PyModule_AddObject(m, "WERR_ABANDON_HIBERFILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ABANDON_HIBERFILE)));
	PyModule_AddObject(m, "WERR_LOST_WRITEBEHIND_DATA_NETWORK_DISCONNECTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOST_WRITEBEHIND_DATA_NETWORK_DISCONNECTED)));
	PyModule_AddObject(m, "WERR_LOST_WRITEBEHIND_DATA_NETWORK_SERVER_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOST_WRITEBEHIND_DATA_NETWORK_SERVER_ERROR)));
	PyModule_AddObject(m, "WERR_LOST_WRITEBEHIND_DATA_LOCAL_DISK_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOST_WRITEBEHIND_DATA_LOCAL_DISK_ERROR)));
	PyModule_AddObject(m, "WERR_EA_ACCESS_DENIED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EA_ACCESS_DENIED)));
	PyModule_AddObject(m, "WERR_OPERATION_ABORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_OPERATION_ABORTED)));
	PyModule_AddObject(m, "WERR_IO_INCOMPLETE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IO_INCOMPLETE)));
	PyModule_AddObject(m, "WERR_IO_PENDING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IO_PENDING)));
	PyModule_AddObject(m, "WERR_NOACCESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOACCESS)));
	PyModule_AddObject(m, "WERR_SWAPERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SWAPERROR)));
	PyModule_AddObject(m, "WERR_STACK_OVERFLOW",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_STACK_OVERFLOW)));
	PyModule_AddObject(m, "WERR_INVALID_MESSAGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_MESSAGE)));
	PyModule_AddObject(m, "WERR_CAN_NOT_COMPLETE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CAN_NOT_COMPLETE)));
	PyModule_AddObject(m, "WERR_INVALID_FLAGS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_FLAGS)));
	PyModule_AddObject(m, "WERR_UNRECOGNIZED_VOLUME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNRECOGNIZED_VOLUME)));
	PyModule_AddObject(m, "WERR_FILE_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FILE_INVALID)));
	PyModule_AddObject(m, "WERR_FULLSCREEN_MODE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FULLSCREEN_MODE)));
	PyModule_AddObject(m, "WERR_NO_TOKEN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_TOKEN)));
	PyModule_AddObject(m, "WERR_BADDB",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BADDB)));
	PyModule_AddObject(m, "WERR_BADKEY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BADKEY)));
	PyModule_AddObject(m, "WERR_CANTOPEN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANTOPEN)));
	PyModule_AddObject(m, "WERR_CANTREAD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANTREAD)));
	PyModule_AddObject(m, "WERR_CANTWRITE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANTWRITE)));
	PyModule_AddObject(m, "WERR_REGISTRY_RECOVERED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REGISTRY_RECOVERED)));
	PyModule_AddObject(m, "WERR_REGISTRY_CORRUPT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REGISTRY_CORRUPT)));
	PyModule_AddObject(m, "WERR_REGISTRY_IO_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REGISTRY_IO_FAILED)));
	PyModule_AddObject(m, "WERR_NOT_REGISTRY_FILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_REGISTRY_FILE)));
	PyModule_AddObject(m, "WERR_KEY_DELETED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_KEY_DELETED)));
	PyModule_AddObject(m, "WERR_NO_LOG_SPACE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_LOG_SPACE)));
	PyModule_AddObject(m, "WERR_KEY_HAS_CHILDREN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_KEY_HAS_CHILDREN)));
	PyModule_AddObject(m, "WERR_CHILD_MUST_BE_VOLATILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CHILD_MUST_BE_VOLATILE)));
	PyModule_AddObject(m, "WERR_NOTIFY_ENUM_DIR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOTIFY_ENUM_DIR)));
	PyModule_AddObject(m, "WERR_DEPENDENT_SERVICES_RUNNING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DEPENDENT_SERVICES_RUNNING)));
	PyModule_AddObject(m, "WERR_INVALID_SERVICE_CONTROL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_SERVICE_CONTROL)));
	PyModule_AddObject(m, "WERR_SERVICE_REQUEST_TIMEOUT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERVICE_REQUEST_TIMEOUT)));
	PyModule_AddObject(m, "WERR_SERVICE_NO_THREAD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERVICE_NO_THREAD)));
	PyModule_AddObject(m, "WERR_SERVICE_DATABASE_LOCKED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERVICE_DATABASE_LOCKED)));
	PyModule_AddObject(m, "WERR_SERVICE_ALREADY_RUNNING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERVICE_ALREADY_RUNNING)));
	PyModule_AddObject(m, "WERR_INVALID_SERVICE_ACCOUNT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_SERVICE_ACCOUNT)));
	PyModule_AddObject(m, "WERR_SERVICE_DISABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERVICE_DISABLED)));
	PyModule_AddObject(m, "WERR_CIRCULAR_DEPENDENCY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CIRCULAR_DEPENDENCY)));
	PyModule_AddObject(m, "WERR_SERVICE_DOES_NOT_EXIST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERVICE_DOES_NOT_EXIST)));
	PyModule_AddObject(m, "WERR_SERVICE_CANNOT_ACCEPT_CTRL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERVICE_CANNOT_ACCEPT_CTRL)));
	PyModule_AddObject(m, "WERR_SERVICE_NOT_ACTIVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERVICE_NOT_ACTIVE)));
	PyModule_AddObject(m, "WERR_FAILED_SERVICE_CONTROLLER_CONNECT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FAILED_SERVICE_CONTROLLER_CONNECT)));
	PyModule_AddObject(m, "WERR_EXCEPTION_IN_SERVICE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EXCEPTION_IN_SERVICE)));
	PyModule_AddObject(m, "WERR_DATABASE_DOES_NOT_EXIST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DATABASE_DOES_NOT_EXIST)));
	PyModule_AddObject(m, "WERR_SERVICE_SPECIFIC_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERVICE_SPECIFIC_ERROR)));
	PyModule_AddObject(m, "WERR_PROCESS_ABORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PROCESS_ABORTED)));
	PyModule_AddObject(m, "WERR_SERVICE_DEPENDENCY_FAIL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERVICE_DEPENDENCY_FAIL)));
	PyModule_AddObject(m, "WERR_SERVICE_LOGON_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERVICE_LOGON_FAILED)));
	PyModule_AddObject(m, "WERR_SERVICE_START_HANG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERVICE_START_HANG)));
	PyModule_AddObject(m, "WERR_INVALID_SERVICE_LOCK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_SERVICE_LOCK)));
	PyModule_AddObject(m, "WERR_SERVICE_MARKED_FOR_DELETE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERVICE_MARKED_FOR_DELETE)));
	PyModule_AddObject(m, "WERR_SERVICE_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERVICE_EXISTS)));
	PyModule_AddObject(m, "WERR_ALREADY_RUNNING_LKG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ALREADY_RUNNING_LKG)));
	PyModule_AddObject(m, "WERR_SERVICE_DEPENDENCY_DELETED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERVICE_DEPENDENCY_DELETED)));
	PyModule_AddObject(m, "WERR_BOOT_ALREADY_ACCEPTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BOOT_ALREADY_ACCEPTED)));
	PyModule_AddObject(m, "WERR_SERVICE_NEVER_STARTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERVICE_NEVER_STARTED)));
	PyModule_AddObject(m, "WERR_DUPLICATE_SERVICE_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DUPLICATE_SERVICE_NAME)));
	PyModule_AddObject(m, "WERR_DIFFERENT_SERVICE_ACCOUNT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DIFFERENT_SERVICE_ACCOUNT)));
	PyModule_AddObject(m, "WERR_CANNOT_DETECT_DRIVER_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANNOT_DETECT_DRIVER_FAILURE)));
	PyModule_AddObject(m, "WERR_CANNOT_DETECT_PROCESS_ABORT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANNOT_DETECT_PROCESS_ABORT)));
	PyModule_AddObject(m, "WERR_NO_RECOVERY_PROGRAM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_RECOVERY_PROGRAM)));
	PyModule_AddObject(m, "WERR_SERVICE_NOT_IN_EXE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERVICE_NOT_IN_EXE)));
	PyModule_AddObject(m, "WERR_NOT_SAFEBOOT_SERVICE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_SAFEBOOT_SERVICE)));
	PyModule_AddObject(m, "WERR_END_OF_MEDIA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_END_OF_MEDIA)));
	PyModule_AddObject(m, "WERR_FILEMARK_DETECTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FILEMARK_DETECTED)));
	PyModule_AddObject(m, "WERR_BEGINNING_OF_MEDIA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BEGINNING_OF_MEDIA)));
	PyModule_AddObject(m, "WERR_SETMARK_DETECTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SETMARK_DETECTED)));
	PyModule_AddObject(m, "WERR_NO_DATA_DETECTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_DATA_DETECTED)));
	PyModule_AddObject(m, "WERR_PARTITION_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PARTITION_FAILURE)));
	PyModule_AddObject(m, "WERR_INVALID_BLOCK_LENGTH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_BLOCK_LENGTH)));
	PyModule_AddObject(m, "WERR_DEVICE_NOT_PARTITIONED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DEVICE_NOT_PARTITIONED)));
	PyModule_AddObject(m, "WERR_UNABLE_TO_LOCK_MEDIA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNABLE_TO_LOCK_MEDIA)));
	PyModule_AddObject(m, "WERR_UNABLE_TO_UNLOAD_MEDIA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNABLE_TO_UNLOAD_MEDIA)));
	PyModule_AddObject(m, "WERR_MEDIA_CHANGED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MEDIA_CHANGED)));
	PyModule_AddObject(m, "WERR_BUS_RESET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BUS_RESET)));
	PyModule_AddObject(m, "WERR_NO_MEDIA_IN_DRIVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_MEDIA_IN_DRIVE)));
	PyModule_AddObject(m, "WERR_NO_UNICODE_TRANSLATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_UNICODE_TRANSLATION)));
	PyModule_AddObject(m, "WERR_DLL_INIT_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DLL_INIT_FAILED)));
	PyModule_AddObject(m, "WERR_SHUTDOWN_IN_PROGRESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SHUTDOWN_IN_PROGRESS)));
	PyModule_AddObject(m, "WERR_NO_SHUTDOWN_IN_PROGRESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_SHUTDOWN_IN_PROGRESS)));
	PyModule_AddObject(m, "WERR_IO_DEVICE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IO_DEVICE)));
	PyModule_AddObject(m, "WERR_SERIAL_NO_DEVICE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERIAL_NO_DEVICE)));
	PyModule_AddObject(m, "WERR_IRQ_BUSY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IRQ_BUSY)));
	PyModule_AddObject(m, "WERR_MORE_WRITES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MORE_WRITES)));
	PyModule_AddObject(m, "WERR_COUNTER_TIMEOUT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_COUNTER_TIMEOUT)));
	PyModule_AddObject(m, "WERR_FLOPPY_ID_MARK_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FLOPPY_ID_MARK_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_FLOPPY_WRONG_CYLINDER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FLOPPY_WRONG_CYLINDER)));
	PyModule_AddObject(m, "WERR_FLOPPY_UNKNOWN_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FLOPPY_UNKNOWN_ERROR)));
	PyModule_AddObject(m, "WERR_FLOPPY_BAD_REGISTERS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FLOPPY_BAD_REGISTERS)));
	PyModule_AddObject(m, "WERR_DISK_RECALIBRATE_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DISK_RECALIBRATE_FAILED)));
	PyModule_AddObject(m, "WERR_DISK_OPERATION_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DISK_OPERATION_FAILED)));
	PyModule_AddObject(m, "WERR_DISK_RESET_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DISK_RESET_FAILED)));
	PyModule_AddObject(m, "WERR_EOM_OVERFLOW",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EOM_OVERFLOW)));
	PyModule_AddObject(m, "WERR_NOT_ENOUGH_SERVER_MEMORY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_ENOUGH_SERVER_MEMORY)));
	PyModule_AddObject(m, "WERR_POSSIBLE_DEADLOCK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_POSSIBLE_DEADLOCK)));
	PyModule_AddObject(m, "WERR_MAPPED_ALIGNMENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MAPPED_ALIGNMENT)));
	PyModule_AddObject(m, "WERR_SET_POWER_STATE_VETOED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SET_POWER_STATE_VETOED)));
	PyModule_AddObject(m, "WERR_SET_POWER_STATE_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SET_POWER_STATE_FAILED)));
	PyModule_AddObject(m, "WERR_TOO_MANY_LINKS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TOO_MANY_LINKS)));
	PyModule_AddObject(m, "WERR_OLD_WIN_VERSION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_OLD_WIN_VERSION)));
	PyModule_AddObject(m, "WERR_APP_WRONG_OS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_APP_WRONG_OS)));
	PyModule_AddObject(m, "WERR_SINGLE_INSTANCE_APP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SINGLE_INSTANCE_APP)));
	PyModule_AddObject(m, "WERR_RMODE_APP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RMODE_APP)));
	PyModule_AddObject(m, "WERR_INVALID_DLL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_DLL)));
	PyModule_AddObject(m, "WERR_NO_ASSOCIATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_ASSOCIATION)));
	PyModule_AddObject(m, "WERR_DDE_FAIL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DDE_FAIL)));
	PyModule_AddObject(m, "WERR_DLL_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DLL_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_NO_MORE_USER_HANDLES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_MORE_USER_HANDLES)));
	PyModule_AddObject(m, "WERR_MESSAGE_SYNC_ONLY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MESSAGE_SYNC_ONLY)));
	PyModule_AddObject(m, "WERR_SOURCE_ELEMENT_EMPTY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SOURCE_ELEMENT_EMPTY)));
	PyModule_AddObject(m, "WERR_DESTINATION_ELEMENT_FULL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DESTINATION_ELEMENT_FULL)));
	PyModule_AddObject(m, "WERR_ILLEGAL_ELEMENT_ADDRESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ILLEGAL_ELEMENT_ADDRESS)));
	PyModule_AddObject(m, "WERR_MAGAZINE_NOT_PRESENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MAGAZINE_NOT_PRESENT)));
	PyModule_AddObject(m, "WERR_DEVICE_REINITIALIZATION_NEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DEVICE_REINITIALIZATION_NEEDED)));
	PyModule_AddObject(m, "WERR_DEVICE_REQUIRES_CLEANING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DEVICE_REQUIRES_CLEANING)));
	PyModule_AddObject(m, "WERR_DEVICE_DOOR_OPEN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DEVICE_DOOR_OPEN)));
	PyModule_AddObject(m, "WERR_DEVICE_NOT_CONNECTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DEVICE_NOT_CONNECTED)));
	PyModule_AddObject(m, "WERR_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_NO_MATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_MATCH)));
	PyModule_AddObject(m, "WERR_SET_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SET_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_POINT_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_POINT_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_NO_TRACKING_SERVICE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_TRACKING_SERVICE)));
	PyModule_AddObject(m, "WERR_NO_VOLUME_ID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_VOLUME_ID)));
	PyModule_AddObject(m, "WERR_UNABLE_TO_REMOVE_REPLACED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNABLE_TO_REMOVE_REPLACED)));
	PyModule_AddObject(m, "WERR_UNABLE_TO_MOVE_REPLACEMENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNABLE_TO_MOVE_REPLACEMENT)));
	PyModule_AddObject(m, "WERR_UNABLE_TO_MOVE_REPLACEMENT_2",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNABLE_TO_MOVE_REPLACEMENT_2)));
	PyModule_AddObject(m, "WERR_JOURNAL_DELETE_IN_PROGRESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_JOURNAL_DELETE_IN_PROGRESS)));
	PyModule_AddObject(m, "WERR_JOURNAL_NOT_ACTIVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_JOURNAL_NOT_ACTIVE)));
	PyModule_AddObject(m, "WERR_POTENTIAL_FILE_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_POTENTIAL_FILE_FOUND)));
	PyModule_AddObject(m, "WERR_JOURNAL_ENTRY_DELETED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_JOURNAL_ENTRY_DELETED)));
	PyModule_AddObject(m, "WERR_SHUTDOWN_IS_SCHEDULED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SHUTDOWN_IS_SCHEDULED)));
	PyModule_AddObject(m, "WERR_SHUTDOWN_USERS_LOGGED_ON",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SHUTDOWN_USERS_LOGGED_ON)));
	PyModule_AddObject(m, "WERR_BAD_DEVICE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_DEVICE)));
	PyModule_AddObject(m, "WERR_CONNECTION_UNAVAIL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CONNECTION_UNAVAIL)));
	PyModule_AddObject(m, "WERR_DEVICE_ALREADY_REMEMBERED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DEVICE_ALREADY_REMEMBERED)));
	PyModule_AddObject(m, "WERR_NO_NET_OR_BAD_PATH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_NET_OR_BAD_PATH)));
	PyModule_AddObject(m, "WERR_BAD_PROVIDER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_PROVIDER)));
	PyModule_AddObject(m, "WERR_CANNOT_OPEN_PROFILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANNOT_OPEN_PROFILE)));
	PyModule_AddObject(m, "WERR_BAD_PROFILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_PROFILE)));
	PyModule_AddObject(m, "WERR_NOT_CONTAINER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_CONTAINER)));
	PyModule_AddObject(m, "WERR_EXTENDED_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EXTENDED_ERROR)));
	PyModule_AddObject(m, "WERR_INVALID_GROUPNAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_GROUPNAME)));
	PyModule_AddObject(m, "WERR_INVALID_COMPUTERNAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_COMPUTERNAME)));
	PyModule_AddObject(m, "WERR_INVALID_EVENTNAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_EVENTNAME)));
	PyModule_AddObject(m, "WERR_INVALID_DOMAINNAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_DOMAINNAME)));
	PyModule_AddObject(m, "WERR_INVALID_SERVICENAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_SERVICENAME)));
	PyModule_AddObject(m, "WERR_INVALID_NETNAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_NETNAME)));
	PyModule_AddObject(m, "WERR_INVALID_SHARENAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_SHARENAME)));
	PyModule_AddObject(m, "WERR_INVALID_PASSWORDNAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_PASSWORDNAME)));
	PyModule_AddObject(m, "WERR_INVALID_MESSAGENAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_MESSAGENAME)));
	PyModule_AddObject(m, "WERR_INVALID_MESSAGEDEST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_MESSAGEDEST)));
	PyModule_AddObject(m, "WERR_SESSION_CREDENTIAL_CONFLICT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SESSION_CREDENTIAL_CONFLICT)));
	PyModule_AddObject(m, "WERR_REMOTE_SESSION_LIMIT_EXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REMOTE_SESSION_LIMIT_EXCEEDED)));
	PyModule_AddObject(m, "WERR_DUP_DOMAINNAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DUP_DOMAINNAME)));
	PyModule_AddObject(m, "WERR_NO_NETWORK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_NETWORK)));
	PyModule_AddObject(m, "WERR_CANCELLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANCELLED)));
	PyModule_AddObject(m, "WERR_USER_MAPPED_FILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_USER_MAPPED_FILE)));
	PyModule_AddObject(m, "WERR_CONNECTION_REFUSED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CONNECTION_REFUSED)));
	PyModule_AddObject(m, "WERR_GRACEFUL_DISCONNECT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_GRACEFUL_DISCONNECT)));
	PyModule_AddObject(m, "WERR_ADDRESS_ALREADY_ASSOCIATED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ADDRESS_ALREADY_ASSOCIATED)));
	PyModule_AddObject(m, "WERR_ADDRESS_NOT_ASSOCIATED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ADDRESS_NOT_ASSOCIATED)));
	PyModule_AddObject(m, "WERR_CONNECTION_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CONNECTION_INVALID)));
	PyModule_AddObject(m, "WERR_CONNECTION_ACTIVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CONNECTION_ACTIVE)));
	PyModule_AddObject(m, "WERR_NETWORK_UNREACHABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NETWORK_UNREACHABLE)));
	PyModule_AddObject(m, "WERR_HOST_UNREACHABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_HOST_UNREACHABLE)));
	PyModule_AddObject(m, "WERR_PROTOCOL_UNREACHABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PROTOCOL_UNREACHABLE)));
	PyModule_AddObject(m, "WERR_PORT_UNREACHABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PORT_UNREACHABLE)));
	PyModule_AddObject(m, "WERR_REQUEST_ABORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REQUEST_ABORTED)));
	PyModule_AddObject(m, "WERR_CONNECTION_ABORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CONNECTION_ABORTED)));
	PyModule_AddObject(m, "WERR_RETRY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RETRY)));
	PyModule_AddObject(m, "WERR_CONNECTION_COUNT_LIMIT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CONNECTION_COUNT_LIMIT)));
	PyModule_AddObject(m, "WERR_LOGIN_TIME_RESTRICTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOGIN_TIME_RESTRICTION)));
	PyModule_AddObject(m, "WERR_LOGIN_WKSTA_RESTRICTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOGIN_WKSTA_RESTRICTION)));
	PyModule_AddObject(m, "WERR_INCORRECT_ADDRESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INCORRECT_ADDRESS)));
	PyModule_AddObject(m, "WERR_ALREADY_REGISTERED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ALREADY_REGISTERED)));
	PyModule_AddObject(m, "WERR_SERVICE_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERVICE_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_NOT_AUTHENTICATED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_AUTHENTICATED)));
	PyModule_AddObject(m, "WERR_NOT_LOGGED_ON",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_LOGGED_ON)));
	PyModule_AddObject(m, "WERR_CONTINUE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CONTINUE)));
	PyModule_AddObject(m, "WERR_ALREADY_INITIALIZED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ALREADY_INITIALIZED)));
	PyModule_AddObject(m, "WERR_NO_MORE_DEVICES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_MORE_DEVICES)));
	PyModule_AddObject(m, "WERR_NO_SUCH_SITE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_SUCH_SITE)));
	PyModule_AddObject(m, "WERR_DOMAIN_CONTROLLER_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DOMAIN_CONTROLLER_EXISTS)));
	PyModule_AddObject(m, "WERR_ONLY_IF_CONNECTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ONLY_IF_CONNECTED)));
	PyModule_AddObject(m, "WERR_OVERRIDE_NOCHANGES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_OVERRIDE_NOCHANGES)));
	PyModule_AddObject(m, "WERR_BAD_USER_PROFILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_USER_PROFILE)));
	PyModule_AddObject(m, "WERR_NOT_SUPPORTED_ON_SBS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_SUPPORTED_ON_SBS)));
	PyModule_AddObject(m, "WERR_SERVER_SHUTDOWN_IN_PROGRESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERVER_SHUTDOWN_IN_PROGRESS)));
	PyModule_AddObject(m, "WERR_HOST_DOWN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_HOST_DOWN)));
	PyModule_AddObject(m, "WERR_NON_ACCOUNT_SID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NON_ACCOUNT_SID)));
	PyModule_AddObject(m, "WERR_NON_DOMAIN_SID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NON_DOMAIN_SID)));
	PyModule_AddObject(m, "WERR_APPHELP_BLOCK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_APPHELP_BLOCK)));
	PyModule_AddObject(m, "WERR_ACCESS_DISABLED_BY_POLICY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ACCESS_DISABLED_BY_POLICY)));
	PyModule_AddObject(m, "WERR_REG_NAT_CONSUMPTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REG_NAT_CONSUMPTION)));
	PyModule_AddObject(m, "WERR_CSCSHARE_OFFLINE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CSCSHARE_OFFLINE)));
	PyModule_AddObject(m, "WERR_PKINIT_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PKINIT_FAILURE)));
	PyModule_AddObject(m, "WERR_SMARTCARD_SUBSYSTEM_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SMARTCARD_SUBSYSTEM_FAILURE)));
	PyModule_AddObject(m, "WERR_DOWNGRADE_DETECTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DOWNGRADE_DETECTED)));
	PyModule_AddObject(m, "WERR_MACHINE_LOCKED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MACHINE_LOCKED)));
	PyModule_AddObject(m, "WERR_CALLBACK_SUPPLIED_INVALID_DATA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CALLBACK_SUPPLIED_INVALID_DATA)));
	PyModule_AddObject(m, "WERR_SYNC_FOREGROUND_REFRESH_REQUIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SYNC_FOREGROUND_REFRESH_REQUIRED)));
	PyModule_AddObject(m, "WERR_DRIVER_BLOCKED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DRIVER_BLOCKED)));
	PyModule_AddObject(m, "WERR_INVALID_IMPORT_OF_NON_DLL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_IMPORT_OF_NON_DLL)));
	PyModule_AddObject(m, "WERR_ACCESS_DISABLED_WEBBLADE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ACCESS_DISABLED_WEBBLADE)));
	PyModule_AddObject(m, "WERR_ACCESS_DISABLED_WEBBLADE_TAMPER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ACCESS_DISABLED_WEBBLADE_TAMPER)));
	PyModule_AddObject(m, "WERR_RECOVERY_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RECOVERY_FAILURE)));
	PyModule_AddObject(m, "WERR_ALREADY_FIBER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ALREADY_FIBER)));
	PyModule_AddObject(m, "WERR_ALREADY_THREAD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ALREADY_THREAD)));
	PyModule_AddObject(m, "WERR_STACK_BUFFER_OVERRUN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_STACK_BUFFER_OVERRUN)));
	PyModule_AddObject(m, "WERR_PARAMETER_QUOTA_EXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PARAMETER_QUOTA_EXCEEDED)));
	PyModule_AddObject(m, "WERR_DEBUGGER_INACTIVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DEBUGGER_INACTIVE)));
	PyModule_AddObject(m, "WERR_DELAY_LOAD_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DELAY_LOAD_FAILED)));
	PyModule_AddObject(m, "WERR_VDM_DISALLOWED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_VDM_DISALLOWED)));
	PyModule_AddObject(m, "WERR_UNIDENTIFIED_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNIDENTIFIED_ERROR)));
	PyModule_AddObject(m, "WERR_INVALID_CRUNTIME_PARAMETER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_CRUNTIME_PARAMETER)));
	PyModule_AddObject(m, "WERR_BEYOND_VDL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BEYOND_VDL)));
	PyModule_AddObject(m, "WERR_INCOMPATIBLE_SERVICE_SID_TYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INCOMPATIBLE_SERVICE_SID_TYPE)));
	PyModule_AddObject(m, "WERR_DRIVER_PROCESS_TERMINATED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DRIVER_PROCESS_TERMINATED)));
	PyModule_AddObject(m, "WERR_IMPLEMENTATION_LIMIT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IMPLEMENTATION_LIMIT)));
	PyModule_AddObject(m, "WERR_PROCESS_IS_PROTECTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PROCESS_IS_PROTECTED)));
	PyModule_AddObject(m, "WERR_SERVICE_NOTIFY_CLIENT_LAGGING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERVICE_NOTIFY_CLIENT_LAGGING)));
	PyModule_AddObject(m, "WERR_DISK_QUOTA_EXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DISK_QUOTA_EXCEEDED)));
	PyModule_AddObject(m, "WERR_CONTENT_BLOCKED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CONTENT_BLOCKED)));
	PyModule_AddObject(m, "WERR_INCOMPATIBLE_SERVICE_PRIVILEGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INCOMPATIBLE_SERVICE_PRIVILEGE)));
	PyModule_AddObject(m, "WERR_INVALID_LABEL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_LABEL)));
	PyModule_AddObject(m, "WERR_NOT_ALL_ASSIGNED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_ALL_ASSIGNED)));
	PyModule_AddObject(m, "WERR_SOME_NOT_MAPPED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SOME_NOT_MAPPED)));
	PyModule_AddObject(m, "WERR_NO_QUOTAS_FOR_ACCOUNT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_QUOTAS_FOR_ACCOUNT)));
	PyModule_AddObject(m, "WERR_LOCAL_USER_SESSION_KEY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOCAL_USER_SESSION_KEY)));
	PyModule_AddObject(m, "WERR_NULL_LM_PASSWORD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NULL_LM_PASSWORD)));
	PyModule_AddObject(m, "WERR_UNKNOWN_REVISION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNKNOWN_REVISION)));
	PyModule_AddObject(m, "WERR_REVISION_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REVISION_MISMATCH)));
	PyModule_AddObject(m, "WERR_INVALID_OWNER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_OWNER)));
	PyModule_AddObject(m, "WERR_NO_IMPERSONATION_TOKEN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_IMPERSONATION_TOKEN)));
	PyModule_AddObject(m, "WERR_CANT_DISABLE_MANDATORY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANT_DISABLE_MANDATORY)));
	PyModule_AddObject(m, "WERR_NO_LOGON_SERVERS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_LOGON_SERVERS)));
	PyModule_AddObject(m, "WERR_NO_SUCH_LOGON_SESSION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_SUCH_LOGON_SESSION)));
	PyModule_AddObject(m, "WERR_NO_SUCH_PRIVILEGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_SUCH_PRIVILEGE)));
	PyModule_AddObject(m, "WERR_PRIVILEGE_NOT_HELD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PRIVILEGE_NOT_HELD)));
	PyModule_AddObject(m, "WERR_INVALID_ACCOUNT_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_ACCOUNT_NAME)));
	PyModule_AddObject(m, "WERR_USER_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_USER_EXISTS)));
	PyModule_AddObject(m, "WERR_NO_SUCH_USER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_SUCH_USER)));
	PyModule_AddObject(m, "WERR_GROUP_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_GROUP_EXISTS)));
	PyModule_AddObject(m, "WERR_NO_SUCH_GROUP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_SUCH_GROUP)));
	PyModule_AddObject(m, "WERR_MEMBER_IN_GROUP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MEMBER_IN_GROUP)));
	PyModule_AddObject(m, "WERR_MEMBER_NOT_IN_GROUP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MEMBER_NOT_IN_GROUP)));
	PyModule_AddObject(m, "WERR_LAST_ADMIN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LAST_ADMIN)));
	PyModule_AddObject(m, "WERR_WRONG_PASSWORD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WRONG_PASSWORD)));
	PyModule_AddObject(m, "WERR_ILL_FORMED_PASSWORD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ILL_FORMED_PASSWORD)));
	PyModule_AddObject(m, "WERR_PASSWORD_RESTRICTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PASSWORD_RESTRICTION)));
	PyModule_AddObject(m, "WERR_LOGON_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOGON_FAILURE)));
	PyModule_AddObject(m, "WERR_ACCOUNT_RESTRICTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ACCOUNT_RESTRICTION)));
	PyModule_AddObject(m, "WERR_INVALID_LOGON_HOURS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_LOGON_HOURS)));
	PyModule_AddObject(m, "WERR_INVALID_WORKSTATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_WORKSTATION)));
	PyModule_AddObject(m, "WERR_PASSWORD_EXPIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PASSWORD_EXPIRED)));
	PyModule_AddObject(m, "WERR_ACCOUNT_DISABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ACCOUNT_DISABLED)));
	PyModule_AddObject(m, "WERR_NONE_MAPPED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NONE_MAPPED)));
	PyModule_AddObject(m, "WERR_TOO_MANY_LUIDS_REQUESTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TOO_MANY_LUIDS_REQUESTED)));
	PyModule_AddObject(m, "WERR_LUIDS_EXHAUSTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LUIDS_EXHAUSTED)));
	PyModule_AddObject(m, "WERR_INVALID_SUB_AUTHORITY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_SUB_AUTHORITY)));
	PyModule_AddObject(m, "WERR_INVALID_ACL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_ACL)));
	PyModule_AddObject(m, "WERR_INVALID_SID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_SID)));
	PyModule_AddObject(m, "WERR_INVALID_SECURITY_DESCR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_SECURITY_DESCR)));
	PyModule_AddObject(m, "WERR_BAD_INHERITANCE_ACL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_INHERITANCE_ACL)));
	PyModule_AddObject(m, "WERR_SERVER_DISABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERVER_DISABLED)));
	PyModule_AddObject(m, "WERR_SERVER_NOT_DISABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERVER_NOT_DISABLED)));
	PyModule_AddObject(m, "WERR_INVALID_ID_AUTHORITY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_ID_AUTHORITY)));
	PyModule_AddObject(m, "WERR_ALLOTTED_SPACE_EXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ALLOTTED_SPACE_EXCEEDED)));
	PyModule_AddObject(m, "WERR_INVALID_GROUP_ATTRIBUTES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_GROUP_ATTRIBUTES)));
	PyModule_AddObject(m, "WERR_BAD_IMPERSONATION_LEVEL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_IMPERSONATION_LEVEL)));
	PyModule_AddObject(m, "WERR_CANT_OPEN_ANONYMOUS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANT_OPEN_ANONYMOUS)));
	PyModule_AddObject(m, "WERR_BAD_VALIDATION_CLASS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_VALIDATION_CLASS)));
	PyModule_AddObject(m, "WERR_BAD_TOKEN_TYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_TOKEN_TYPE)));
	PyModule_AddObject(m, "WERR_NO_SECURITY_ON_OBJECT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_SECURITY_ON_OBJECT)));
	PyModule_AddObject(m, "WERR_CANT_ACCESS_DOMAIN_INFO",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANT_ACCESS_DOMAIN_INFO)));
	PyModule_AddObject(m, "WERR_INVALID_SERVER_STATE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_SERVER_STATE)));
	PyModule_AddObject(m, "WERR_INVALID_DOMAIN_STATE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_DOMAIN_STATE)));
	PyModule_AddObject(m, "WERR_INVALID_DOMAIN_ROLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_DOMAIN_ROLE)));
	PyModule_AddObject(m, "WERR_NO_SUCH_DOMAIN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_SUCH_DOMAIN)));
	PyModule_AddObject(m, "WERR_DOMAIN_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DOMAIN_EXISTS)));
	PyModule_AddObject(m, "WERR_DOMAIN_LIMIT_EXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DOMAIN_LIMIT_EXCEEDED)));
	PyModule_AddObject(m, "WERR_INTERNAL_DB_CORRUPTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INTERNAL_DB_CORRUPTION)));
	PyModule_AddObject(m, "WERR_INTERNAL_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INTERNAL_ERROR)));
	PyModule_AddObject(m, "WERR_GENERIC_NOT_MAPPED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_GENERIC_NOT_MAPPED)));
	PyModule_AddObject(m, "WERR_BAD_DESCRIPTOR_FORMAT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_DESCRIPTOR_FORMAT)));
	PyModule_AddObject(m, "WERR_NOT_LOGON_PROCESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_LOGON_PROCESS)));
	PyModule_AddObject(m, "WERR_LOGON_SESSION_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOGON_SESSION_EXISTS)));
	PyModule_AddObject(m, "WERR_NO_SUCH_PACKAGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_SUCH_PACKAGE)));
	PyModule_AddObject(m, "WERR_BAD_LOGON_SESSION_STATE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_LOGON_SESSION_STATE)));
	PyModule_AddObject(m, "WERR_LOGON_SESSION_COLLISION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOGON_SESSION_COLLISION)));
	PyModule_AddObject(m, "WERR_INVALID_LOGON_TYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_LOGON_TYPE)));
	PyModule_AddObject(m, "WERR_CANNOT_IMPERSONATE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANNOT_IMPERSONATE)));
	PyModule_AddObject(m, "WERR_RXACT_INVALID_STATE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RXACT_INVALID_STATE)));
	PyModule_AddObject(m, "WERR_RXACT_COMMIT_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RXACT_COMMIT_FAILURE)));
	PyModule_AddObject(m, "WERR_SPECIAL_ACCOUNT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SPECIAL_ACCOUNT)));
	PyModule_AddObject(m, "WERR_SPECIAL_GROUP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SPECIAL_GROUP)));
	PyModule_AddObject(m, "WERR_SPECIAL_USER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SPECIAL_USER)));
	PyModule_AddObject(m, "WERR_MEMBERS_PRIMARY_GROUP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MEMBERS_PRIMARY_GROUP)));
	PyModule_AddObject(m, "WERR_TOKEN_ALREADY_IN_USE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TOKEN_ALREADY_IN_USE)));
	PyModule_AddObject(m, "WERR_NO_SUCH_ALIAS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_SUCH_ALIAS)));
	PyModule_AddObject(m, "WERR_MEMBER_NOT_IN_ALIAS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MEMBER_NOT_IN_ALIAS)));
	PyModule_AddObject(m, "WERR_MEMBER_IN_ALIAS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MEMBER_IN_ALIAS)));
	PyModule_AddObject(m, "WERR_ALIAS_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ALIAS_EXISTS)));
	PyModule_AddObject(m, "WERR_LOGON_NOT_GRANTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOGON_NOT_GRANTED)));
	PyModule_AddObject(m, "WERR_TOO_MANY_SECRETS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TOO_MANY_SECRETS)));
	PyModule_AddObject(m, "WERR_SECRET_TOO_LONG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SECRET_TOO_LONG)));
	PyModule_AddObject(m, "WERR_INTERNAL_DB_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INTERNAL_DB_ERROR)));
	PyModule_AddObject(m, "WERR_TOO_MANY_CONTEXT_IDS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TOO_MANY_CONTEXT_IDS)));
	PyModule_AddObject(m, "WERR_LOGON_TYPE_NOT_GRANTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOGON_TYPE_NOT_GRANTED)));
	PyModule_AddObject(m, "WERR_NT_CROSS_ENCRYPTION_REQUIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NT_CROSS_ENCRYPTION_REQUIRED)));
	PyModule_AddObject(m, "WERR_NO_SUCH_MEMBER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_SUCH_MEMBER)));
	PyModule_AddObject(m, "WERR_INVALID_MEMBER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_MEMBER)));
	PyModule_AddObject(m, "WERR_TOO_MANY_SIDS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TOO_MANY_SIDS)));
	PyModule_AddObject(m, "WERR_LM_CROSS_ENCRYPTION_REQUIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LM_CROSS_ENCRYPTION_REQUIRED)));
	PyModule_AddObject(m, "WERR_NO_INHERITANCE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_INHERITANCE)));
	PyModule_AddObject(m, "WERR_FILE_CORRUPT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FILE_CORRUPT)));
	PyModule_AddObject(m, "WERR_DISK_CORRUPT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DISK_CORRUPT)));
	PyModule_AddObject(m, "WERR_NO_USER_SESSION_KEY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_USER_SESSION_KEY)));
	PyModule_AddObject(m, "WERR_LICENSE_QUOTA_EXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LICENSE_QUOTA_EXCEEDED)));
	PyModule_AddObject(m, "WERR_WRONG_TARGET_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WRONG_TARGET_NAME)));
	PyModule_AddObject(m, "WERR_MUTUAL_AUTH_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MUTUAL_AUTH_FAILED)));
	PyModule_AddObject(m, "WERR_TIME_SKEW",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TIME_SKEW)));
	PyModule_AddObject(m, "WERR_CURRENT_DOMAIN_NOT_ALLOWED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CURRENT_DOMAIN_NOT_ALLOWED)));
	PyModule_AddObject(m, "WERR_INVALID_WINDOW_HANDLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_WINDOW_HANDLE)));
	PyModule_AddObject(m, "WERR_INVALID_MENU_HANDLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_MENU_HANDLE)));
	PyModule_AddObject(m, "WERR_INVALID_CURSOR_HANDLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_CURSOR_HANDLE)));
	PyModule_AddObject(m, "WERR_INVALID_ACCEL_HANDLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_ACCEL_HANDLE)));
	PyModule_AddObject(m, "WERR_INVALID_HOOK_HANDLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_HOOK_HANDLE)));
	PyModule_AddObject(m, "WERR_INVALID_DWP_HANDLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_DWP_HANDLE)));
	PyModule_AddObject(m, "WERR_TLW_WITH_WSCHILD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TLW_WITH_WSCHILD)));
	PyModule_AddObject(m, "WERR_CANNOT_FIND_WND_CLASS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANNOT_FIND_WND_CLASS)));
	PyModule_AddObject(m, "WERR_WINDOW_OF_OTHER_THREAD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WINDOW_OF_OTHER_THREAD)));
	PyModule_AddObject(m, "WERR_HOTKEY_ALREADY_REGISTERED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_HOTKEY_ALREADY_REGISTERED)));
	PyModule_AddObject(m, "WERR_CLASS_ALREADY_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLASS_ALREADY_EXISTS)));
	PyModule_AddObject(m, "WERR_CLASS_DOES_NOT_EXIST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLASS_DOES_NOT_EXIST)));
	PyModule_AddObject(m, "WERR_CLASS_HAS_WINDOWS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLASS_HAS_WINDOWS)));
	PyModule_AddObject(m, "WERR_INVALID_INDEX",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_INDEX)));
	PyModule_AddObject(m, "WERR_INVALID_ICON_HANDLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_ICON_HANDLE)));
	PyModule_AddObject(m, "WERR_PRIVATE_DIALOG_INDEX",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PRIVATE_DIALOG_INDEX)));
	PyModule_AddObject(m, "WERR_LISTBOX_ID_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LISTBOX_ID_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_NO_WILDCARD_CHARACTERS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_WILDCARD_CHARACTERS)));
	PyModule_AddObject(m, "WERR_CLIPBOARD_NOT_OPEN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLIPBOARD_NOT_OPEN)));
	PyModule_AddObject(m, "WERR_HOTKEY_NOT_REGISTERED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_HOTKEY_NOT_REGISTERED)));
	PyModule_AddObject(m, "WERR_WINDOW_NOT_DIALOG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WINDOW_NOT_DIALOG)));
	PyModule_AddObject(m, "WERR_CONTROL_ID_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CONTROL_ID_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_INVALID_COMBOBOX_MESSAGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_COMBOBOX_MESSAGE)));
	PyModule_AddObject(m, "WERR_WINDOW_NOT_COMBOBOX",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WINDOW_NOT_COMBOBOX)));
	PyModule_AddObject(m, "WERR_INVALID_EDIT_HEIGHT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_EDIT_HEIGHT)));
	PyModule_AddObject(m, "WERR_DC_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DC_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_INVALID_HOOK_FILTER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_HOOK_FILTER)));
	PyModule_AddObject(m, "WERR_INVALID_FILTER_PROC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_FILTER_PROC)));
	PyModule_AddObject(m, "WERR_HOOK_NEEDS_HMOD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_HOOK_NEEDS_HMOD)));
	PyModule_AddObject(m, "WERR_GLOBAL_ONLY_HOOK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_GLOBAL_ONLY_HOOK)));
	PyModule_AddObject(m, "WERR_JOURNAL_HOOK_SET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_JOURNAL_HOOK_SET)));
	PyModule_AddObject(m, "WERR_HOOK_NOT_INSTALLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_HOOK_NOT_INSTALLED)));
	PyModule_AddObject(m, "WERR_INVALID_LB_MESSAGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_LB_MESSAGE)));
	PyModule_AddObject(m, "WERR_SETCOUNT_ON_BAD_LB",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SETCOUNT_ON_BAD_LB)));
	PyModule_AddObject(m, "WERR_LB_WITHOUT_TABSTOPS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LB_WITHOUT_TABSTOPS)));
	PyModule_AddObject(m, "WERR_DESTROY_OBJECT_OF_OTHER_THREAD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DESTROY_OBJECT_OF_OTHER_THREAD)));
	PyModule_AddObject(m, "WERR_CHILD_WINDOW_MENU",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CHILD_WINDOW_MENU)));
	PyModule_AddObject(m, "WERR_NO_SYSTEM_MENU",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_SYSTEM_MENU)));
	PyModule_AddObject(m, "WERR_INVALID_MSGBOX_STYLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_MSGBOX_STYLE)));
	PyModule_AddObject(m, "WERR_INVALID_SPI_VALUE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_SPI_VALUE)));
	PyModule_AddObject(m, "WERR_SCREEN_ALREADY_LOCKED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SCREEN_ALREADY_LOCKED)));
	PyModule_AddObject(m, "WERR_HWNDS_HAVE_DIFF_PARENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_HWNDS_HAVE_DIFF_PARENT)));
	PyModule_AddObject(m, "WERR_NOT_CHILD_WINDOW",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_CHILD_WINDOW)));
	PyModule_AddObject(m, "WERR_INVALID_GW_COMMAND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_GW_COMMAND)));
	PyModule_AddObject(m, "WERR_INVALID_THREAD_ID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_THREAD_ID)));
	PyModule_AddObject(m, "WERR_NON_MDICHILD_WINDOW",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NON_MDICHILD_WINDOW)));
	PyModule_AddObject(m, "WERR_POPUP_ALREADY_ACTIVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_POPUP_ALREADY_ACTIVE)));
	PyModule_AddObject(m, "WERR_NO_SCROLLBARS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_SCROLLBARS)));
	PyModule_AddObject(m, "WERR_INVALID_SCROLLBAR_RANGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_SCROLLBAR_RANGE)));
	PyModule_AddObject(m, "WERR_INVALID_SHOWWIN_COMMAND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_SHOWWIN_COMMAND)));
	PyModule_AddObject(m, "WERR_NO_SYSTEM_RESOURCES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_SYSTEM_RESOURCES)));
	PyModule_AddObject(m, "WERR_NONPAGED_SYSTEM_RESOURCES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NONPAGED_SYSTEM_RESOURCES)));
	PyModule_AddObject(m, "WERR_PAGED_SYSTEM_RESOURCES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PAGED_SYSTEM_RESOURCES)));
	PyModule_AddObject(m, "WERR_WORKING_SET_QUOTA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WORKING_SET_QUOTA)));
	PyModule_AddObject(m, "WERR_PAGEFILE_QUOTA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PAGEFILE_QUOTA)));
	PyModule_AddObject(m, "WERR_COMMITMENT_LIMIT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_COMMITMENT_LIMIT)));
	PyModule_AddObject(m, "WERR_MENU_ITEM_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MENU_ITEM_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_INVALID_KEYBOARD_HANDLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_KEYBOARD_HANDLE)));
	PyModule_AddObject(m, "WERR_HOOK_TYPE_NOT_ALLOWED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_HOOK_TYPE_NOT_ALLOWED)));
	PyModule_AddObject(m, "WERR_REQUIRES_INTERACTIVE_WINDOWSTATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REQUIRES_INTERACTIVE_WINDOWSTATION)));
	PyModule_AddObject(m, "WERR_TIMEOUT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TIMEOUT)));
	PyModule_AddObject(m, "WERR_INVALID_MONITOR_HANDLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_MONITOR_HANDLE)));
	PyModule_AddObject(m, "WERR_INCORRECT_SIZE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INCORRECT_SIZE)));
	PyModule_AddObject(m, "WERR_SYMLINK_CLASS_DISABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SYMLINK_CLASS_DISABLED)));
	PyModule_AddObject(m, "WERR_SYMLINK_NOT_SUPPORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SYMLINK_NOT_SUPPORTED)));
	PyModule_AddObject(m, "WERR_EVENTLOG_FILE_CORRUPT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVENTLOG_FILE_CORRUPT)));
	PyModule_AddObject(m, "WERR_EVENTLOG_CANT_START",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVENTLOG_CANT_START)));
	PyModule_AddObject(m, "WERR_LOG_FILE_FULL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_FILE_FULL)));
	PyModule_AddObject(m, "WERR_EVENTLOG_FILE_CHANGED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVENTLOG_FILE_CHANGED)));
	PyModule_AddObject(m, "WERR_INVALID_TASK_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_TASK_NAME)));
	PyModule_AddObject(m, "WERR_INVALID_TASK_INDEX",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_TASK_INDEX)));
	PyModule_AddObject(m, "WERR_THREAD_ALREADY_IN_TASK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_THREAD_ALREADY_IN_TASK)));
	PyModule_AddObject(m, "WERR_INSTALL_SERVICE_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSTALL_SERVICE_FAILURE)));
	PyModule_AddObject(m, "WERR_INSTALL_USEREXIT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSTALL_USEREXIT)));
	PyModule_AddObject(m, "WERR_INSTALL_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSTALL_FAILURE)));
	PyModule_AddObject(m, "WERR_INSTALL_SUSPEND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSTALL_SUSPEND)));
	PyModule_AddObject(m, "WERR_UNKNOWN_PRODUCT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNKNOWN_PRODUCT)));
	PyModule_AddObject(m, "WERR_UNKNOWN_FEATURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNKNOWN_FEATURE)));
	PyModule_AddObject(m, "WERR_UNKNOWN_COMPONENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNKNOWN_COMPONENT)));
	PyModule_AddObject(m, "WERR_UNKNOWN_PROPERTY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNKNOWN_PROPERTY)));
	PyModule_AddObject(m, "WERR_INVALID_HANDLE_STATE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_HANDLE_STATE)));
	PyModule_AddObject(m, "WERR_BAD_CONFIGURATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_CONFIGURATION)));
	PyModule_AddObject(m, "WERR_INDEX_ABSENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INDEX_ABSENT)));
	PyModule_AddObject(m, "WERR_INSTALL_SOURCE_ABSENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSTALL_SOURCE_ABSENT)));
	PyModule_AddObject(m, "WERR_INSTALL_PACKAGE_VERSION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSTALL_PACKAGE_VERSION)));
	PyModule_AddObject(m, "WERR_PRODUCT_UNINSTALLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PRODUCT_UNINSTALLED)));
	PyModule_AddObject(m, "WERR_BAD_QUERY_SYNTAX",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_QUERY_SYNTAX)));
	PyModule_AddObject(m, "WERR_INVALID_FIELD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_FIELD)));
	PyModule_AddObject(m, "WERR_DEVICE_REMOVED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DEVICE_REMOVED)));
	PyModule_AddObject(m, "WERR_INSTALL_ALREADY_RUNNING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSTALL_ALREADY_RUNNING)));
	PyModule_AddObject(m, "WERR_INSTALL_PACKAGE_OPEN_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSTALL_PACKAGE_OPEN_FAILED)));
	PyModule_AddObject(m, "WERR_INSTALL_PACKAGE_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSTALL_PACKAGE_INVALID)));
	PyModule_AddObject(m, "WERR_INSTALL_UI_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSTALL_UI_FAILURE)));
	PyModule_AddObject(m, "WERR_INSTALL_LOG_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSTALL_LOG_FAILURE)));
	PyModule_AddObject(m, "WERR_INSTALL_LANGUAGE_UNSUPPORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSTALL_LANGUAGE_UNSUPPORTED)));
	PyModule_AddObject(m, "WERR_INSTALL_TRANSFORM_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSTALL_TRANSFORM_FAILURE)));
	PyModule_AddObject(m, "WERR_INSTALL_PACKAGE_REJECTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSTALL_PACKAGE_REJECTED)));
	PyModule_AddObject(m, "WERR_FUNCTION_NOT_CALLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FUNCTION_NOT_CALLED)));
	PyModule_AddObject(m, "WERR_FUNCTION_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FUNCTION_FAILED)));
	PyModule_AddObject(m, "WERR_INVALID_TABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_TABLE)));
	PyModule_AddObject(m, "WERR_DATATYPE_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DATATYPE_MISMATCH)));
	PyModule_AddObject(m, "WERR_UNSUPPORTED_TYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNSUPPORTED_TYPE)));
	PyModule_AddObject(m, "WERR_CREATE_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CREATE_FAILED)));
	PyModule_AddObject(m, "WERR_INSTALL_TEMP_UNWRITABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSTALL_TEMP_UNWRITABLE)));
	PyModule_AddObject(m, "WERR_INSTALL_PLATFORM_UNSUPPORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSTALL_PLATFORM_UNSUPPORTED)));
	PyModule_AddObject(m, "WERR_INSTALL_NOTUSED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSTALL_NOTUSED)));
	PyModule_AddObject(m, "WERR_PATCH_PACKAGE_OPEN_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PATCH_PACKAGE_OPEN_FAILED)));
	PyModule_AddObject(m, "WERR_PATCH_PACKAGE_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PATCH_PACKAGE_INVALID)));
	PyModule_AddObject(m, "WERR_PATCH_PACKAGE_UNSUPPORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PATCH_PACKAGE_UNSUPPORTED)));
	PyModule_AddObject(m, "WERR_PRODUCT_VERSION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PRODUCT_VERSION)));
	PyModule_AddObject(m, "WERR_INVALID_COMMAND_LINE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_COMMAND_LINE)));
	PyModule_AddObject(m, "WERR_INSTALL_REMOTE_DISALLOWED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSTALL_REMOTE_DISALLOWED)));
	PyModule_AddObject(m, "WERR_SUCCESS_REBOOT_INITIATED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SUCCESS_REBOOT_INITIATED)));
	PyModule_AddObject(m, "WERR_PATCH_TARGET_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PATCH_TARGET_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_PATCH_PACKAGE_REJECTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PATCH_PACKAGE_REJECTED)));
	PyModule_AddObject(m, "WERR_INSTALL_TRANSFORM_REJECTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSTALL_TRANSFORM_REJECTED)));
	PyModule_AddObject(m, "WERR_INSTALL_REMOTE_PROHIBITED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSTALL_REMOTE_PROHIBITED)));
	PyModule_AddObject(m, "WERR_PATCH_REMOVAL_UNSUPPORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PATCH_REMOVAL_UNSUPPORTED)));
	PyModule_AddObject(m, "WERR_UNKNOWN_PATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNKNOWN_PATCH)));
	PyModule_AddObject(m, "WERR_PATCH_NO_SEQUENCE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PATCH_NO_SEQUENCE)));
	PyModule_AddObject(m, "WERR_PATCH_REMOVAL_DISALLOWED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PATCH_REMOVAL_DISALLOWED)));
	PyModule_AddObject(m, "WERR_INVALID_PATCH_XML",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_PATCH_XML)));
	PyModule_AddObject(m, "WERR_PATCH_MANAGED_ADVERTISED_PRODUCT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PATCH_MANAGED_ADVERTISED_PRODUCT)));
	PyModule_AddObject(m, "WERR_INSTALL_SERVICE_SAFEBOOT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INSTALL_SERVICE_SAFEBOOT)));
	PyModule_AddObject(m, "WERR_RPC_S_INVALID_STRING_BINDING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_INVALID_STRING_BINDING)));
	PyModule_AddObject(m, "WERR_RPC_S_WRONG_KIND_OF_BINDING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_WRONG_KIND_OF_BINDING)));
	PyModule_AddObject(m, "WERR_RPC_S_INVALID_BINDING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_INVALID_BINDING)));
	PyModule_AddObject(m, "WERR_RPC_S_PROTSEQ_NOT_SUPPORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_PROTSEQ_NOT_SUPPORTED)));
	PyModule_AddObject(m, "WERR_RPC_S_INVALID_RPC_PROTSEQ",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_INVALID_RPC_PROTSEQ)));
	PyModule_AddObject(m, "WERR_RPC_S_INVALID_STRING_UUID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_INVALID_STRING_UUID)));
	PyModule_AddObject(m, "WERR_RPC_S_INVALID_ENDPOINT_FORMAT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_INVALID_ENDPOINT_FORMAT)));
	PyModule_AddObject(m, "WERR_RPC_S_INVALID_NET_ADDR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_INVALID_NET_ADDR)));
	PyModule_AddObject(m, "WERR_RPC_S_NO_ENDPOINT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_NO_ENDPOINT_FOUND)));
	PyModule_AddObject(m, "WERR_RPC_S_INVALID_TIMEOUT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_INVALID_TIMEOUT)));
	PyModule_AddObject(m, "WERR_RPC_S_OBJECT_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_OBJECT_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_RPC_S_ALREADY_REGISTERED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_ALREADY_REGISTERED)));
	PyModule_AddObject(m, "WERR_RPC_S_TYPE_ALREADY_REGISTERED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_TYPE_ALREADY_REGISTERED)));
	PyModule_AddObject(m, "WERR_RPC_S_ALREADY_LISTENING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_ALREADY_LISTENING)));
	PyModule_AddObject(m, "WERR_RPC_S_NO_PROTSEQS_REGISTERED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_NO_PROTSEQS_REGISTERED)));
	PyModule_AddObject(m, "WERR_RPC_S_NOT_LISTENING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_NOT_LISTENING)));
	PyModule_AddObject(m, "WERR_RPC_S_UNKNOWN_MGR_TYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_UNKNOWN_MGR_TYPE)));
	PyModule_AddObject(m, "WERR_RPC_S_UNKNOWN_IF",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_UNKNOWN_IF)));
	PyModule_AddObject(m, "WERR_RPC_S_NO_BINDINGS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_NO_BINDINGS)));
	PyModule_AddObject(m, "WERR_RPC_S_NO_PROTSEQS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_NO_PROTSEQS)));
	PyModule_AddObject(m, "WERR_RPC_S_CANT_CREATE_ENDPOINT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_CANT_CREATE_ENDPOINT)));
	PyModule_AddObject(m, "WERR_RPC_S_OUT_OF_RESOURCES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_OUT_OF_RESOURCES)));
	PyModule_AddObject(m, "WERR_RPC_S_SERVER_UNAVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_SERVER_UNAVAILABLE)));
	PyModule_AddObject(m, "WERR_RPC_S_SERVER_TOO_BUSY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_SERVER_TOO_BUSY)));
	PyModule_AddObject(m, "WERR_RPC_S_INVALID_NETWORK_OPTIONS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_INVALID_NETWORK_OPTIONS)));
	PyModule_AddObject(m, "WERR_RPC_S_NO_CALL_ACTIVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_NO_CALL_ACTIVE)));
	PyModule_AddObject(m, "WERR_RPC_S_CALL_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_CALL_FAILED)));
	PyModule_AddObject(m, "WERR_RPC_S_CALL_FAILED_DNE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_CALL_FAILED_DNE)));
	PyModule_AddObject(m, "WERR_RPC_S_PROTOCOL_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_PROTOCOL_ERROR)));
	PyModule_AddObject(m, "WERR_RPC_S_PROXY_ACCESS_DENIED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_PROXY_ACCESS_DENIED)));
	PyModule_AddObject(m, "WERR_RPC_S_UNSUPPORTED_TRANS_SYN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_UNSUPPORTED_TRANS_SYN)));
	PyModule_AddObject(m, "WERR_RPC_S_UNSUPPORTED_TYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_UNSUPPORTED_TYPE)));
	PyModule_AddObject(m, "WERR_RPC_S_INVALID_TAG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_INVALID_TAG)));
	PyModule_AddObject(m, "WERR_RPC_S_INVALID_BOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_INVALID_BOUND)));
	PyModule_AddObject(m, "WERR_RPC_S_NO_ENTRY_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_NO_ENTRY_NAME)));
	PyModule_AddObject(m, "WERR_RPC_S_INVALID_NAME_SYNTAX",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_INVALID_NAME_SYNTAX)));
	PyModule_AddObject(m, "WERR_RPC_S_UNSUPPORTED_NAME_SYNTAX",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_UNSUPPORTED_NAME_SYNTAX)));
	PyModule_AddObject(m, "WERR_RPC_S_UUID_NO_ADDRESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_UUID_NO_ADDRESS)));
	PyModule_AddObject(m, "WERR_RPC_S_DUPLICATE_ENDPOINT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_DUPLICATE_ENDPOINT)));
	PyModule_AddObject(m, "WERR_RPC_S_UNKNOWN_AUTHN_TYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_UNKNOWN_AUTHN_TYPE)));
	PyModule_AddObject(m, "WERR_RPC_S_MAX_CALLS_TOO_SMALL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_MAX_CALLS_TOO_SMALL)));
	PyModule_AddObject(m, "WERR_RPC_S_STRING_TOO_LONG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_STRING_TOO_LONG)));
	PyModule_AddObject(m, "WERR_RPC_S_PROTSEQ_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_PROTSEQ_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_RPC_S_PROCNUM_OUT_OF_RANGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_PROCNUM_OUT_OF_RANGE)));
	PyModule_AddObject(m, "WERR_RPC_S_BINDING_HAS_NO_AUTH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_BINDING_HAS_NO_AUTH)));
	PyModule_AddObject(m, "WERR_RPC_S_UNKNOWN_AUTHN_SERVICE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_UNKNOWN_AUTHN_SERVICE)));
	PyModule_AddObject(m, "WERR_RPC_S_UNKNOWN_AUTHN_LEVEL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_UNKNOWN_AUTHN_LEVEL)));
	PyModule_AddObject(m, "WERR_RPC_S_INVALID_AUTH_IDENTITY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_INVALID_AUTH_IDENTITY)));
	PyModule_AddObject(m, "WERR_RPC_S_UNKNOWN_AUTHZ_SERVICE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_UNKNOWN_AUTHZ_SERVICE)));
	PyModule_AddObject(m, "WERR_EPT_S_INVALID_ENTRY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EPT_S_INVALID_ENTRY)));
	PyModule_AddObject(m, "WERR_EPT_S_CANT_PERFORM_OP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EPT_S_CANT_PERFORM_OP)));
	PyModule_AddObject(m, "WERR_EPT_S_NOT_REGISTERED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EPT_S_NOT_REGISTERED)));
	PyModule_AddObject(m, "WERR_RPC_S_NOTHING_TO_EXPORT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_NOTHING_TO_EXPORT)));
	PyModule_AddObject(m, "WERR_RPC_S_INCOMPLETE_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_INCOMPLETE_NAME)));
	PyModule_AddObject(m, "WERR_RPC_S_INVALID_VERS_OPTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_INVALID_VERS_OPTION)));
	PyModule_AddObject(m, "WERR_RPC_S_NO_MORE_MEMBERS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_NO_MORE_MEMBERS)));
	PyModule_AddObject(m, "WERR_RPC_S_NOT_ALL_OBJS_UNEXPORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_NOT_ALL_OBJS_UNEXPORTED)));
	PyModule_AddObject(m, "WERR_RPC_S_INTERFACE_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_INTERFACE_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_RPC_S_ENTRY_ALREADY_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_ENTRY_ALREADY_EXISTS)));
	PyModule_AddObject(m, "WERR_RPC_S_ENTRY_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_ENTRY_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_RPC_S_NAME_SERVICE_UNAVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_NAME_SERVICE_UNAVAILABLE)));
	PyModule_AddObject(m, "WERR_RPC_S_INVALID_NAF_ID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_INVALID_NAF_ID)));
	PyModule_AddObject(m, "WERR_RPC_S_CANNOT_SUPPORT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_CANNOT_SUPPORT)));
	PyModule_AddObject(m, "WERR_RPC_S_NO_CONTEXT_AVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_NO_CONTEXT_AVAILABLE)));
	PyModule_AddObject(m, "WERR_RPC_S_INTERNAL_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_INTERNAL_ERROR)));
	PyModule_AddObject(m, "WERR_RPC_S_ZERO_DIVIDE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_ZERO_DIVIDE)));
	PyModule_AddObject(m, "WERR_RPC_S_ADDRESS_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_ADDRESS_ERROR)));
	PyModule_AddObject(m, "WERR_RPC_S_FP_DIV_ZERO",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_FP_DIV_ZERO)));
	PyModule_AddObject(m, "WERR_RPC_S_FP_UNDERFLOW",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_FP_UNDERFLOW)));
	PyModule_AddObject(m, "WERR_RPC_S_FP_OVERFLOW",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_FP_OVERFLOW)));
	PyModule_AddObject(m, "WERR_RPC_X_NO_MORE_ENTRIES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_X_NO_MORE_ENTRIES)));
	PyModule_AddObject(m, "WERR_RPC_X_SS_CHAR_TRANS_OPEN_FAIL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_X_SS_CHAR_TRANS_OPEN_FAIL)));
	PyModule_AddObject(m, "WERR_RPC_X_SS_CHAR_TRANS_SHORT_FILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_X_SS_CHAR_TRANS_SHORT_FILE)));
	PyModule_AddObject(m, "WERR_RPC_X_SS_IN_NULL_CONTEXT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_X_SS_IN_NULL_CONTEXT)));
	PyModule_AddObject(m, "WERR_RPC_X_SS_CONTEXT_DAMAGED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_X_SS_CONTEXT_DAMAGED)));
	PyModule_AddObject(m, "WERR_RPC_X_SS_HANDLES_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_X_SS_HANDLES_MISMATCH)));
	PyModule_AddObject(m, "WERR_RPC_X_SS_CANNOT_GET_CALL_HANDLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_X_SS_CANNOT_GET_CALL_HANDLE)));
	PyModule_AddObject(m, "WERR_RPC_X_NULL_REF_POINTER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_X_NULL_REF_POINTER)));
	PyModule_AddObject(m, "WERR_RPC_X_ENUM_VALUE_OUT_OF_RANGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_X_ENUM_VALUE_OUT_OF_RANGE)));
	PyModule_AddObject(m, "WERR_RPC_X_BYTE_COUNT_TOO_SMALL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_X_BYTE_COUNT_TOO_SMALL)));
	PyModule_AddObject(m, "WERR_RPC_X_BAD_STUB_DATA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_X_BAD_STUB_DATA)));
	PyModule_AddObject(m, "WERR_INVALID_USER_BUFFER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_USER_BUFFER)));
	PyModule_AddObject(m, "WERR_UNRECOGNIZED_MEDIA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNRECOGNIZED_MEDIA)));
	PyModule_AddObject(m, "WERR_NO_TRUST_LSA_SECRET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_TRUST_LSA_SECRET)));
	PyModule_AddObject(m, "WERR_NO_TRUST_SAM_ACCOUNT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_TRUST_SAM_ACCOUNT)));
	PyModule_AddObject(m, "WERR_TRUSTED_DOMAIN_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRUSTED_DOMAIN_FAILURE)));
	PyModule_AddObject(m, "WERR_TRUSTED_RELATIONSHIP_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRUSTED_RELATIONSHIP_FAILURE)));
	PyModule_AddObject(m, "WERR_TRUST_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRUST_FAILURE)));
	PyModule_AddObject(m, "WERR_RPC_S_CALL_IN_PROGRESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_CALL_IN_PROGRESS)));
	PyModule_AddObject(m, "WERR_NETLOGON_NOT_STARTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NETLOGON_NOT_STARTED)));
	PyModule_AddObject(m, "WERR_ACCOUNT_EXPIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ACCOUNT_EXPIRED)));
	PyModule_AddObject(m, "WERR_REDIRECTOR_HAS_OPEN_HANDLES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REDIRECTOR_HAS_OPEN_HANDLES)));
	PyModule_AddObject(m, "WERR_PRINTER_DRIVER_ALREADY_INSTALLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PRINTER_DRIVER_ALREADY_INSTALLED)));
	PyModule_AddObject(m, "WERR_UNKNOWN_PORT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNKNOWN_PORT)));
	PyModule_AddObject(m, "WERR_UNKNOWN_PRINTER_DRIVER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNKNOWN_PRINTER_DRIVER)));
	PyModule_AddObject(m, "WERR_UNKNOWN_PRINTPROCESSOR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNKNOWN_PRINTPROCESSOR)));
	PyModule_AddObject(m, "WERR_INVALID_SEPARATOR_FILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_SEPARATOR_FILE)));
	PyModule_AddObject(m, "WERR_INVALID_PRIORITY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_PRIORITY)));
	PyModule_AddObject(m, "WERR_INVALID_PRINTER_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_PRINTER_NAME)));
	PyModule_AddObject(m, "WERR_PRINTER_ALREADY_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PRINTER_ALREADY_EXISTS)));
	PyModule_AddObject(m, "WERR_INVALID_PRINTER_COMMAND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_PRINTER_COMMAND)));
	PyModule_AddObject(m, "WERR_INVALID_DATATYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_DATATYPE)));
	PyModule_AddObject(m, "WERR_INVALID_ENVIRONMENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_ENVIRONMENT)));
	PyModule_AddObject(m, "WERR_RPC_S_NO_MORE_BINDINGS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_NO_MORE_BINDINGS)));
	PyModule_AddObject(m, "WERR_NOLOGON_INTERDOMAIN_TRUST_ACCOUNT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOLOGON_INTERDOMAIN_TRUST_ACCOUNT)));
	PyModule_AddObject(m, "WERR_NOLOGON_WORKSTATION_TRUST_ACCOUNT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOLOGON_WORKSTATION_TRUST_ACCOUNT)));
	PyModule_AddObject(m, "WERR_NOLOGON_SERVER_TRUST_ACCOUNT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOLOGON_SERVER_TRUST_ACCOUNT)));
	PyModule_AddObject(m, "WERR_DOMAIN_TRUST_INCONSISTENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DOMAIN_TRUST_INCONSISTENT)));
	PyModule_AddObject(m, "WERR_SERVER_HAS_OPEN_HANDLES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SERVER_HAS_OPEN_HANDLES)));
	PyModule_AddObject(m, "WERR_RESOURCE_DATA_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RESOURCE_DATA_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_RESOURCE_TYPE_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RESOURCE_TYPE_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_RESOURCE_NAME_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RESOURCE_NAME_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_RESOURCE_LANG_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RESOURCE_LANG_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_NOT_ENOUGH_QUOTA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_ENOUGH_QUOTA)));
	PyModule_AddObject(m, "WERR_RPC_S_NO_INTERFACES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_NO_INTERFACES)));
	PyModule_AddObject(m, "WERR_RPC_S_CALL_CANCELLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_CALL_CANCELLED)));
	PyModule_AddObject(m, "WERR_RPC_S_BINDING_INCOMPLETE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_BINDING_INCOMPLETE)));
	PyModule_AddObject(m, "WERR_RPC_S_COMM_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_COMM_FAILURE)));
	PyModule_AddObject(m, "WERR_RPC_S_UNSUPPORTED_AUTHN_LEVEL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_UNSUPPORTED_AUTHN_LEVEL)));
	PyModule_AddObject(m, "WERR_RPC_S_NO_PRINC_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_NO_PRINC_NAME)));
	PyModule_AddObject(m, "WERR_RPC_S_NOT_RPC_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_NOT_RPC_ERROR)));
	PyModule_AddObject(m, "WERR_RPC_S_UUID_LOCAL_ONLY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_UUID_LOCAL_ONLY)));
	PyModule_AddObject(m, "WERR_RPC_S_SEC_PKG_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_SEC_PKG_ERROR)));
	PyModule_AddObject(m, "WERR_RPC_S_NOT_CANCELLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_NOT_CANCELLED)));
	PyModule_AddObject(m, "WERR_RPC_X_INVALID_ES_ACTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_X_INVALID_ES_ACTION)));
	PyModule_AddObject(m, "WERR_RPC_X_WRONG_ES_VERSION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_X_WRONG_ES_VERSION)));
	PyModule_AddObject(m, "WERR_RPC_X_WRONG_STUB_VERSION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_X_WRONG_STUB_VERSION)));
	PyModule_AddObject(m, "WERR_RPC_X_INVALID_PIPE_OBJECT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_X_INVALID_PIPE_OBJECT)));
	PyModule_AddObject(m, "WERR_RPC_X_WRONG_PIPE_ORDER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_X_WRONG_PIPE_ORDER)));
	PyModule_AddObject(m, "WERR_RPC_X_WRONG_PIPE_VERSION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_X_WRONG_PIPE_VERSION)));
	PyModule_AddObject(m, "WERR_RPC_S_GROUP_MEMBER_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_GROUP_MEMBER_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_EPT_S_CANT_CREATE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EPT_S_CANT_CREATE)));
	PyModule_AddObject(m, "WERR_RPC_S_INVALID_OBJECT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_INVALID_OBJECT)));
	PyModule_AddObject(m, "WERR_INVALID_TIME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_TIME)));
	PyModule_AddObject(m, "WERR_INVALID_FORM_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_FORM_NAME)));
	PyModule_AddObject(m, "WERR_INVALID_FORM_SIZE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_FORM_SIZE)));
	PyModule_AddObject(m, "WERR_ALREADY_WAITING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ALREADY_WAITING)));
	PyModule_AddObject(m, "WERR_PRINTER_DELETED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PRINTER_DELETED)));
	PyModule_AddObject(m, "WERR_INVALID_PRINTER_STATE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_PRINTER_STATE)));
	PyModule_AddObject(m, "WERR_PASSWORD_MUST_CHANGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PASSWORD_MUST_CHANGE)));
	PyModule_AddObject(m, "WERR_DOMAIN_CONTROLLER_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DOMAIN_CONTROLLER_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_ACCOUNT_LOCKED_OUT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ACCOUNT_LOCKED_OUT)));
	PyModule_AddObject(m, "WERR_OR_INVALID_OXID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_OR_INVALID_OXID)));
	PyModule_AddObject(m, "WERR_OR_INVALID_OID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_OR_INVALID_OID)));
	PyModule_AddObject(m, "WERR_OR_INVALID_SET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_OR_INVALID_SET)));
	PyModule_AddObject(m, "WERR_RPC_S_SEND_INCOMPLETE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_SEND_INCOMPLETE)));
	PyModule_AddObject(m, "WERR_RPC_S_INVALID_ASYNC_HANDLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_INVALID_ASYNC_HANDLE)));
	PyModule_AddObject(m, "WERR_RPC_S_INVALID_ASYNC_CALL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_INVALID_ASYNC_CALL)));
	PyModule_AddObject(m, "WERR_RPC_X_PIPE_CLOSED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_X_PIPE_CLOSED)));
	PyModule_AddObject(m, "WERR_RPC_X_PIPE_DISCIPLINE_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_X_PIPE_DISCIPLINE_ERROR)));
	PyModule_AddObject(m, "WERR_RPC_X_PIPE_EMPTY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_X_PIPE_EMPTY)));
	PyModule_AddObject(m, "WERR_NO_SITENAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_SITENAME)));
	PyModule_AddObject(m, "WERR_CANT_ACCESS_FILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANT_ACCESS_FILE)));
	PyModule_AddObject(m, "WERR_CANT_RESOLVE_FILENAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANT_RESOLVE_FILENAME)));
	PyModule_AddObject(m, "WERR_RPC_S_ENTRY_TYPE_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_ENTRY_TYPE_MISMATCH)));
	PyModule_AddObject(m, "WERR_RPC_S_NOT_ALL_OBJS_EXPORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_NOT_ALL_OBJS_EXPORTED)));
	PyModule_AddObject(m, "WERR_RPC_S_INTERFACE_NOT_EXPORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_INTERFACE_NOT_EXPORTED)));
	PyModule_AddObject(m, "WERR_RPC_S_PROFILE_NOT_ADDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_PROFILE_NOT_ADDED)));
	PyModule_AddObject(m, "WERR_RPC_S_PRF_ELT_NOT_ADDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_PRF_ELT_NOT_ADDED)));
	PyModule_AddObject(m, "WERR_RPC_S_PRF_ELT_NOT_REMOVED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_PRF_ELT_NOT_REMOVED)));
	PyModule_AddObject(m, "WERR_RPC_S_GRP_ELT_NOT_ADDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_GRP_ELT_NOT_ADDED)));
	PyModule_AddObject(m, "WERR_RPC_S_GRP_ELT_NOT_REMOVED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPC_S_GRP_ELT_NOT_REMOVED)));
	PyModule_AddObject(m, "WERR_KM_DRIVER_BLOCKED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_KM_DRIVER_BLOCKED)));
	PyModule_AddObject(m, "WERR_CONTEXT_EXPIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CONTEXT_EXPIRED)));
	PyModule_AddObject(m, "WERR_PER_USER_TRUST_QUOTA_EXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PER_USER_TRUST_QUOTA_EXCEEDED)));
	PyModule_AddObject(m, "WERR_ALL_USER_TRUST_QUOTA_EXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ALL_USER_TRUST_QUOTA_EXCEEDED)));
	PyModule_AddObject(m, "WERR_USER_DELETE_TRUST_QUOTA_EXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_USER_DELETE_TRUST_QUOTA_EXCEEDED)));
	PyModule_AddObject(m, "WERR_AUTHENTICATION_FIREWALL_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_AUTHENTICATION_FIREWALL_FAILED)));
	PyModule_AddObject(m, "WERR_REMOTE_PRINT_CONNECTIONS_BLOCKED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REMOTE_PRINT_CONNECTIONS_BLOCKED)));
	PyModule_AddObject(m, "WERR_INVALID_PIXEL_FORMAT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_PIXEL_FORMAT)));
	PyModule_AddObject(m, "WERR_BAD_DRIVER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_DRIVER)));
	PyModule_AddObject(m, "WERR_INVALID_WINDOW_STYLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_WINDOW_STYLE)));
	PyModule_AddObject(m, "WERR_METAFILE_NOT_SUPPORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_METAFILE_NOT_SUPPORTED)));
	PyModule_AddObject(m, "WERR_TRANSFORM_NOT_SUPPORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRANSFORM_NOT_SUPPORTED)));
	PyModule_AddObject(m, "WERR_CLIPPING_NOT_SUPPORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLIPPING_NOT_SUPPORTED)));
	PyModule_AddObject(m, "WERR_INVALID_CMM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_CMM)));
	PyModule_AddObject(m, "WERR_INVALID_PROFILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_PROFILE)));
	PyModule_AddObject(m, "WERR_TAG_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TAG_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_TAG_NOT_PRESENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TAG_NOT_PRESENT)));
	PyModule_AddObject(m, "WERR_DUPLICATE_TAG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DUPLICATE_TAG)));
	PyModule_AddObject(m, "WERR_PROFILE_NOT_ASSOCIATED_WITH_DEVICE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PROFILE_NOT_ASSOCIATED_WITH_DEVICE)));
	PyModule_AddObject(m, "WERR_PROFILE_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PROFILE_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_INVALID_COLORSPACE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_COLORSPACE)));
	PyModule_AddObject(m, "WERR_ICM_NOT_ENABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ICM_NOT_ENABLED)));
	PyModule_AddObject(m, "WERR_DELETING_ICM_XFORM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DELETING_ICM_XFORM)));
	PyModule_AddObject(m, "WERR_INVALID_TRANSFORM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_TRANSFORM)));
	PyModule_AddObject(m, "WERR_COLORSPACE_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_COLORSPACE_MISMATCH)));
	PyModule_AddObject(m, "WERR_INVALID_COLORINDEX",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_COLORINDEX)));
	PyModule_AddObject(m, "WERR_PROFILE_DOES_NOT_MATCH_DEVICE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PROFILE_DOES_NOT_MATCH_DEVICE)));
	PyModule_AddObject(m, "WERR_NERR_NETNOTSTARTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NETNOTSTARTED)));
	PyModule_AddObject(m, "WERR_NERR_UNKNOWNSERVER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_UNKNOWNSERVER)));
	PyModule_AddObject(m, "WERR_NERR_SHAREMEM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_SHAREMEM)));
	PyModule_AddObject(m, "WERR_NERR_NONETWORKRESOURCE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NONETWORKRESOURCE)));
	PyModule_AddObject(m, "WERR_NERR_REMOTEONLY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_REMOTEONLY)));
	PyModule_AddObject(m, "WERR_NERR_DEVNOTREDIRECTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DEVNOTREDIRECTED)));
	PyModule_AddObject(m, "WERR_CONNECTED_OTHER_PASSWORD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CONNECTED_OTHER_PASSWORD)));
	PyModule_AddObject(m, "WERR_CONNECTED_OTHER_PASSWORD_DEFAULT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CONNECTED_OTHER_PASSWORD_DEFAULT)));
	PyModule_AddObject(m, "WERR_NERR_SERVERNOTSTARTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_SERVERNOTSTARTED)));
	PyModule_AddObject(m, "WERR_NERR_ITEMNOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_ITEMNOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_UNKNOWNDEVDIR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_UNKNOWNDEVDIR)));
	PyModule_AddObject(m, "WERR_NERR_REDIRECTEDPATH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_REDIRECTEDPATH)));
	PyModule_AddObject(m, "WERR_NERR_DUPLICATESHARE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DUPLICATESHARE)));
	PyModule_AddObject(m, "WERR_NERR_NOROOM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NOROOM)));
	PyModule_AddObject(m, "WERR_NERR_TOOMANYITEMS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_TOOMANYITEMS)));
	PyModule_AddObject(m, "WERR_NERR_INVALIDMAXUSERS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_INVALIDMAXUSERS)));
	PyModule_AddObject(m, "WERR_NERR_BUFTOOSMALL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BUFTOOSMALL)));
	PyModule_AddObject(m, "WERR_NERR_REMOTEERR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_REMOTEERR)));
	PyModule_AddObject(m, "WERR_NERR_LANMANINIERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_LANMANINIERROR)));
	PyModule_AddObject(m, "WERR_NERR_NETWORKERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NETWORKERROR)));
	PyModule_AddObject(m, "WERR_NERR_WKSTAINCONSISTENTSTATE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_WKSTAINCONSISTENTSTATE)));
	PyModule_AddObject(m, "WERR_NERR_WKSTANOTSTARTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_WKSTANOTSTARTED)));
	PyModule_AddObject(m, "WERR_NERR_BROWSERNOTSTARTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BROWSERNOTSTARTED)));
	PyModule_AddObject(m, "WERR_NERR_INTERNALERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_INTERNALERROR)));
	PyModule_AddObject(m, "WERR_NERR_BADTRANSACTCONFIG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BADTRANSACTCONFIG)));
	PyModule_AddObject(m, "WERR_NERR_INVALIDAPI",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_INVALIDAPI)));
	PyModule_AddObject(m, "WERR_NERR_BADEVENTNAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BADEVENTNAME)));
	PyModule_AddObject(m, "WERR_NERR_DUPNAMEREBOOT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DUPNAMEREBOOT)));
	PyModule_AddObject(m, "WERR_NERR_CFGCOMPNOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_CFGCOMPNOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_CFGPARAMNOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_CFGPARAMNOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_LINETOOLONG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_LINETOOLONG)));
	PyModule_AddObject(m, "WERR_NERR_QNOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_QNOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_JOBNOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_JOBNOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_DESTNOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DESTNOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_DESTEXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DESTEXISTS)));
	PyModule_AddObject(m, "WERR_NERR_QEXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_QEXISTS)));
	PyModule_AddObject(m, "WERR_NERR_QNOROOM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_QNOROOM)));
	PyModule_AddObject(m, "WERR_NERR_JOBNOROOM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_JOBNOROOM)));
	PyModule_AddObject(m, "WERR_NERR_DESTNOROOM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DESTNOROOM)));
	PyModule_AddObject(m, "WERR_NERR_DESTIDLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DESTIDLE)));
	PyModule_AddObject(m, "WERR_NERR_DESTINVALIDOP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DESTINVALIDOP)));
	PyModule_AddObject(m, "WERR_NERR_PROCNORESPOND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_PROCNORESPOND)));
	PyModule_AddObject(m, "WERR_NERR_SPOOLERNOTLOADED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_SPOOLERNOTLOADED)));
	PyModule_AddObject(m, "WERR_NERR_DESTINVALIDSTATE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DESTINVALIDSTATE)));
	PyModule_AddObject(m, "WERR_NERR_QINVALIDSTATE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_QINVALIDSTATE)));
	PyModule_AddObject(m, "WERR_NERR_JOBINVALIDSTATE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_JOBINVALIDSTATE)));
	PyModule_AddObject(m, "WERR_NERR_SPOOLNOMEMORY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_SPOOLNOMEMORY)));
	PyModule_AddObject(m, "WERR_NERR_DRIVERNOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DRIVERNOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_DATATYPEINVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DATATYPEINVALID)));
	PyModule_AddObject(m, "WERR_NERR_PROCNOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_PROCNOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_SERVICETABLELOCKED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_SERVICETABLELOCKED)));
	PyModule_AddObject(m, "WERR_NERR_SERVICETABLEFULL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_SERVICETABLEFULL)));
	PyModule_AddObject(m, "WERR_NERR_SERVICEINSTALLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_SERVICEINSTALLED)));
	PyModule_AddObject(m, "WERR_NERR_SERVICEENTRYLOCKED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_SERVICEENTRYLOCKED)));
	PyModule_AddObject(m, "WERR_NERR_SERVICENOTINSTALLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_SERVICENOTINSTALLED)));
	PyModule_AddObject(m, "WERR_NERR_BADSERVICENAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BADSERVICENAME)));
	PyModule_AddObject(m, "WERR_NERR_SERVICECTLTIMEOUT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_SERVICECTLTIMEOUT)));
	PyModule_AddObject(m, "WERR_NERR_SERVICECTLBUSY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_SERVICECTLBUSY)));
	PyModule_AddObject(m, "WERR_NERR_BADSERVICEPROGNAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BADSERVICEPROGNAME)));
	PyModule_AddObject(m, "WERR_NERR_SERVICENOTCTRL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_SERVICENOTCTRL)));
	PyModule_AddObject(m, "WERR_NERR_SERVICEKILLPROC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_SERVICEKILLPROC)));
	PyModule_AddObject(m, "WERR_NERR_SERVICECTLNOTVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_SERVICECTLNOTVALID)));
	PyModule_AddObject(m, "WERR_NERR_NOTINDISPATCHTBL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NOTINDISPATCHTBL)));
	PyModule_AddObject(m, "WERR_NERR_BADCONTROLRECV",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BADCONTROLRECV)));
	PyModule_AddObject(m, "WERR_NERR_SERVICENOTSTARTING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_SERVICENOTSTARTING)));
	PyModule_AddObject(m, "WERR_NERR_ALREADYLOGGEDON",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_ALREADYLOGGEDON)));
	PyModule_AddObject(m, "WERR_NERR_NOTLOGGEDON",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NOTLOGGEDON)));
	PyModule_AddObject(m, "WERR_NERR_BADUSERNAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BADUSERNAME)));
	PyModule_AddObject(m, "WERR_NERR_BADPASSWORD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BADPASSWORD)));
	PyModule_AddObject(m, "WERR_NERR_UNABLETOADDNAME_W",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_UNABLETOADDNAME_W)));
	PyModule_AddObject(m, "WERR_NERR_UNABLETOADDNAME_F",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_UNABLETOADDNAME_F)));
	PyModule_AddObject(m, "WERR_NERR_UNABLETODELNAME_W",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_UNABLETODELNAME_W)));
	PyModule_AddObject(m, "WERR_NERR_UNABLETODELNAME_F",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_UNABLETODELNAME_F)));
	PyModule_AddObject(m, "WERR_NERR_LOGONSPAUSED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_LOGONSPAUSED)));
	PyModule_AddObject(m, "WERR_NERR_LOGONSERVERCONFLICT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_LOGONSERVERCONFLICT)));
	PyModule_AddObject(m, "WERR_NERR_LOGONNOUSERPATH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_LOGONNOUSERPATH)));
	PyModule_AddObject(m, "WERR_NERR_LOGONSCRIPTERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_LOGONSCRIPTERROR)));
	PyModule_AddObject(m, "WERR_NERR_STANDALONELOGON",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_STANDALONELOGON)));
	PyModule_AddObject(m, "WERR_NERR_LOGONSERVERNOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_LOGONSERVERNOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_LOGONDOMAINEXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_LOGONDOMAINEXISTS)));
	PyModule_AddObject(m, "WERR_NERR_NONVALIDATEDLOGON",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NONVALIDATEDLOGON)));
	PyModule_AddObject(m, "WERR_NERR_ACFNOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_ACFNOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_GROUPNOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_GROUPNOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_USERNOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_USERNOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_RESOURCENOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RESOURCENOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_GROUPEXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_GROUPEXISTS)));
	PyModule_AddObject(m, "WERR_NERR_USEREXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_USEREXISTS)));
	PyModule_AddObject(m, "WERR_NERR_RESOURCEEXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RESOURCEEXISTS)));
	PyModule_AddObject(m, "WERR_NERR_NOTPRIMARY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NOTPRIMARY)));
	PyModule_AddObject(m, "WERR_NERR_ACFNOTLOADED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_ACFNOTLOADED)));
	PyModule_AddObject(m, "WERR_NERR_ACFNOROOM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_ACFNOROOM)));
	PyModule_AddObject(m, "WERR_NERR_ACFFILEIOFAIL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_ACFFILEIOFAIL)));
	PyModule_AddObject(m, "WERR_NERR_ACFTOOMANYLISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_ACFTOOMANYLISTS)));
	PyModule_AddObject(m, "WERR_NERR_USERLOGON",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_USERLOGON)));
	PyModule_AddObject(m, "WERR_NERR_ACFNOPARENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_ACFNOPARENT)));
	PyModule_AddObject(m, "WERR_NERR_CANNOTGROWSEGMENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_CANNOTGROWSEGMENT)));
	PyModule_AddObject(m, "WERR_NERR_SPEGROUPOP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_SPEGROUPOP)));
	PyModule_AddObject(m, "WERR_NERR_NOTINCACHE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NOTINCACHE)));
	PyModule_AddObject(m, "WERR_NERR_USERINGROUP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_USERINGROUP)));
	PyModule_AddObject(m, "WERR_NERR_USERNOTINGROUP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_USERNOTINGROUP)));
	PyModule_AddObject(m, "WERR_NERR_ACCOUNTUNDEFINED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_ACCOUNTUNDEFINED)));
	PyModule_AddObject(m, "WERR_NERR_ACCOUNTEXPIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_ACCOUNTEXPIRED)));
	PyModule_AddObject(m, "WERR_NERR_INVALIDWORKSTATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_INVALIDWORKSTATION)));
	PyModule_AddObject(m, "WERR_NERR_INVALIDLOGONHOURS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_INVALIDLOGONHOURS)));
	PyModule_AddObject(m, "WERR_NERR_PASSWORDEXPIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_PASSWORDEXPIRED)));
	PyModule_AddObject(m, "WERR_NERR_PASSWORDCANTCHANGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_PASSWORDCANTCHANGE)));
	PyModule_AddObject(m, "WERR_NERR_PASSWORDHISTCONFLICT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_PASSWORDHISTCONFLICT)));
	PyModule_AddObject(m, "WERR_NERR_PASSWORDTOOSHORT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_PASSWORDTOOSHORT)));
	PyModule_AddObject(m, "WERR_NERR_PASSWORDTOORECENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_PASSWORDTOORECENT)));
	PyModule_AddObject(m, "WERR_NERR_INVALIDDATABASE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_INVALIDDATABASE)));
	PyModule_AddObject(m, "WERR_NERR_DATABASEUPTODATE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DATABASEUPTODATE)));
	PyModule_AddObject(m, "WERR_NERR_SYNCREQUIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_SYNCREQUIRED)));
	PyModule_AddObject(m, "WERR_NERR_USENOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_USENOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_BADASGTYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BADASGTYPE)));
	PyModule_AddObject(m, "WERR_NERR_DEVICEISSHARED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DEVICEISSHARED)));
	PyModule_AddObject(m, "WERR_NERR_NOCOMPUTERNAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NOCOMPUTERNAME)));
	PyModule_AddObject(m, "WERR_NERR_MSGALREADYSTARTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_MSGALREADYSTARTED)));
	PyModule_AddObject(m, "WERR_NERR_MSGINITFAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_MSGINITFAILED)));
	PyModule_AddObject(m, "WERR_NERR_NAMENOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NAMENOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_ALREADYFORWARDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_ALREADYFORWARDED)));
	PyModule_AddObject(m, "WERR_NERR_ADDFORWARDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_ADDFORWARDED)));
	PyModule_AddObject(m, "WERR_NERR_ALREADYEXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_ALREADYEXISTS)));
	PyModule_AddObject(m, "WERR_NERR_TOOMANYNAMES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_TOOMANYNAMES)));
	PyModule_AddObject(m, "WERR_NERR_DELCOMPUTERNAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DELCOMPUTERNAME)));
	PyModule_AddObject(m, "WERR_NERR_LOCALFORWARD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_LOCALFORWARD)));
	PyModule_AddObject(m, "WERR_NERR_GRPMSGPROCESSOR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_GRPMSGPROCESSOR)));
	PyModule_AddObject(m, "WERR_NERR_PAUSEDREMOTE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_PAUSEDREMOTE)));
	PyModule_AddObject(m, "WERR_NERR_BADRECEIVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BADRECEIVE)));
	PyModule_AddObject(m, "WERR_NERR_NAMEINUSE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NAMEINUSE)));
	PyModule_AddObject(m, "WERR_NERR_MSGNOTSTARTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_MSGNOTSTARTED)));
	PyModule_AddObject(m, "WERR_NERR_NOTLOCALNAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NOTLOCALNAME)));
	PyModule_AddObject(m, "WERR_NERR_NOFORWARDNAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NOFORWARDNAME)));
	PyModule_AddObject(m, "WERR_NERR_REMOTEFULL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_REMOTEFULL)));
	PyModule_AddObject(m, "WERR_NERR_NAMENOTFORWARDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NAMENOTFORWARDED)));
	PyModule_AddObject(m, "WERR_NERR_TRUNCATEDBROADCAST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_TRUNCATEDBROADCAST)));
	PyModule_AddObject(m, "WERR_NERR_INVALIDDEVICE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_INVALIDDEVICE)));
	PyModule_AddObject(m, "WERR_NERR_WRITEFAULT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_WRITEFAULT)));
	PyModule_AddObject(m, "WERR_NERR_DUPLICATENAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DUPLICATENAME)));
	PyModule_AddObject(m, "WERR_NERR_DELETELATER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DELETELATER)));
	PyModule_AddObject(m, "WERR_NERR_INCOMPLETEDEL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_INCOMPLETEDEL)));
	PyModule_AddObject(m, "WERR_NERR_MULTIPLENETS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_MULTIPLENETS)));
	PyModule_AddObject(m, "WERR_NERR_NETNAMENOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NETNAMENOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_DEVICENOTSHARED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DEVICENOTSHARED)));
	PyModule_AddObject(m, "WERR_NERR_CLIENTNAMENOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_CLIENTNAMENOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_FILEIDNOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_FILEIDNOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_EXECFAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_EXECFAILURE)));
	PyModule_AddObject(m, "WERR_NERR_TMPFILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_TMPFILE)));
	PyModule_AddObject(m, "WERR_NERR_TOOMUCHDATA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_TOOMUCHDATA)));
	PyModule_AddObject(m, "WERR_NERR_DEVICESHARECONFLICT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DEVICESHARECONFLICT)));
	PyModule_AddObject(m, "WERR_NERR_BROWSERTABLEINCOMPLETE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BROWSERTABLEINCOMPLETE)));
	PyModule_AddObject(m, "WERR_NERR_NOTLOCALDOMAIN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NOTLOCALDOMAIN)));
	PyModule_AddObject(m, "WERR_NERR_ISDFSSHARE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_ISDFSSHARE)));
	PyModule_AddObject(m, "WERR_NERR_DEVINVALIDOPCODE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DEVINVALIDOPCODE)));
	PyModule_AddObject(m, "WERR_NERR_DEVNOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DEVNOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_DEVNOTOPEN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DEVNOTOPEN)));
	PyModule_AddObject(m, "WERR_NERR_BADQUEUEDEVSTRING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BADQUEUEDEVSTRING)));
	PyModule_AddObject(m, "WERR_NERR_BADQUEUEPRIORITY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BADQUEUEPRIORITY)));
	PyModule_AddObject(m, "WERR_NERR_NOCOMMDEVS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NOCOMMDEVS)));
	PyModule_AddObject(m, "WERR_NERR_QUEUENOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_QUEUENOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_BADDEVSTRING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BADDEVSTRING)));
	PyModule_AddObject(m, "WERR_NERR_BADDEV",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BADDEV)));
	PyModule_AddObject(m, "WERR_NERR_INUSEBYSPOOLER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_INUSEBYSPOOLER)));
	PyModule_AddObject(m, "WERR_NERR_COMMDEVINUSE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_COMMDEVINUSE)));
	PyModule_AddObject(m, "WERR_NERR_INVALIDCOMPUTER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_INVALIDCOMPUTER)));
	PyModule_AddObject(m, "WERR_NERR_MAXLENEXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_MAXLENEXCEEDED)));
	PyModule_AddObject(m, "WERR_NERR_BADCOMPONENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BADCOMPONENT)));
	PyModule_AddObject(m, "WERR_NERR_CANTTYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_CANTTYPE)));
	PyModule_AddObject(m, "WERR_NERR_TOOMANYENTRIES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_TOOMANYENTRIES)));
	PyModule_AddObject(m, "WERR_NERR_PROFILEFILETOOBIG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_PROFILEFILETOOBIG)));
	PyModule_AddObject(m, "WERR_NERR_PROFILEOFFSET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_PROFILEOFFSET)));
	PyModule_AddObject(m, "WERR_NERR_PROFILECLEANUP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_PROFILECLEANUP)));
	PyModule_AddObject(m, "WERR_NERR_PROFILEUNKNOWNCMD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_PROFILEUNKNOWNCMD)));
	PyModule_AddObject(m, "WERR_NERR_PROFILELOADERR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_PROFILELOADERR)));
	PyModule_AddObject(m, "WERR_NERR_PROFILESAVEERR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_PROFILESAVEERR)));
	PyModule_AddObject(m, "WERR_NERR_LOGOVERFLOW",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_LOGOVERFLOW)));
	PyModule_AddObject(m, "WERR_NERR_LOGFILECHANGED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_LOGFILECHANGED)));
	PyModule_AddObject(m, "WERR_NERR_LOGFILECORRUPT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_LOGFILECORRUPT)));
	PyModule_AddObject(m, "WERR_NERR_SOURCEISDIR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_SOURCEISDIR)));
	PyModule_AddObject(m, "WERR_NERR_BADSOURCE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BADSOURCE)));
	PyModule_AddObject(m, "WERR_NERR_BADDEST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BADDEST)));
	PyModule_AddObject(m, "WERR_NERR_DIFFERENTSERVERS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DIFFERENTSERVERS)));
	PyModule_AddObject(m, "WERR_NERR_RUNSRVPAUSED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RUNSRVPAUSED)));
	PyModule_AddObject(m, "WERR_NERR_ERRCOMMRUNSRV",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_ERRCOMMRUNSRV)));
	PyModule_AddObject(m, "WERR_NERR_ERROREXECINGGHOST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_ERROREXECINGGHOST)));
	PyModule_AddObject(m, "WERR_NERR_SHARENOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_SHARENOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_INVALIDLANA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_INVALIDLANA)));
	PyModule_AddObject(m, "WERR_NERR_OPENFILES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_OPENFILES)));
	PyModule_AddObject(m, "WERR_NERR_ACTIVECONNS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_ACTIVECONNS)));
	PyModule_AddObject(m, "WERR_NERR_BADPASSWORDCORE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BADPASSWORDCORE)));
	PyModule_AddObject(m, "WERR_NERR_DEVINUSE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DEVINUSE)));
	PyModule_AddObject(m, "WERR_NERR_LOCALDRIVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_LOCALDRIVE)));
	PyModule_AddObject(m, "WERR_NERR_ALERTEXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_ALERTEXISTS)));
	PyModule_AddObject(m, "WERR_NERR_TOOMANYALERTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_TOOMANYALERTS)));
	PyModule_AddObject(m, "WERR_NERR_NOSUCHALERT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NOSUCHALERT)));
	PyModule_AddObject(m, "WERR_NERR_BADRECIPIENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BADRECIPIENT)));
	PyModule_AddObject(m, "WERR_NERR_ACCTLIMITEXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_ACCTLIMITEXCEEDED)));
	PyModule_AddObject(m, "WERR_NERR_INVALIDLOGSEEK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_INVALIDLOGSEEK)));
	PyModule_AddObject(m, "WERR_NERR_BADUASCONFIG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BADUASCONFIG)));
	PyModule_AddObject(m, "WERR_NERR_INVALIDUASOP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_INVALIDUASOP)));
	PyModule_AddObject(m, "WERR_NERR_LASTADMIN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_LASTADMIN)));
	PyModule_AddObject(m, "WERR_NERR_DCNOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DCNOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_LOGONTRACKINGERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_LOGONTRACKINGERROR)));
	PyModule_AddObject(m, "WERR_NERR_NETLOGONNOTSTARTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NETLOGONNOTSTARTED)));
	PyModule_AddObject(m, "WERR_NERR_CANNOTGROWUASFILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_CANNOTGROWUASFILE)));
	PyModule_AddObject(m, "WERR_NERR_TIMEDIFFATDC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_TIMEDIFFATDC)));
	PyModule_AddObject(m, "WERR_NERR_PASSWORDMISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_PASSWORDMISMATCH)));
	PyModule_AddObject(m, "WERR_NERR_NOSUCHSERVER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NOSUCHSERVER)));
	PyModule_AddObject(m, "WERR_NERR_NOSUCHSESSION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NOSUCHSESSION)));
	PyModule_AddObject(m, "WERR_NERR_NOSUCHCONNECTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NOSUCHCONNECTION)));
	PyModule_AddObject(m, "WERR_NERR_TOOMANYSERVERS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_TOOMANYSERVERS)));
	PyModule_AddObject(m, "WERR_NERR_TOOMANYSESSIONS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_TOOMANYSESSIONS)));
	PyModule_AddObject(m, "WERR_NERR_TOOMANYCONNECTIONS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_TOOMANYCONNECTIONS)));
	PyModule_AddObject(m, "WERR_NERR_TOOMANYFILES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_TOOMANYFILES)));
	PyModule_AddObject(m, "WERR_NERR_NOALTERNATESERVERS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NOALTERNATESERVERS)));
	PyModule_AddObject(m, "WERR_NERR_TRYDOWNLEVEL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_TRYDOWNLEVEL)));
	PyModule_AddObject(m, "WERR_NERR_UPSDRIVERNOTSTARTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_UPSDRIVERNOTSTARTED)));
	PyModule_AddObject(m, "WERR_NERR_UPSINVALIDCONFIG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_UPSINVALIDCONFIG)));
	PyModule_AddObject(m, "WERR_NERR_UPSINVALIDCOMMPORT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_UPSINVALIDCOMMPORT)));
	PyModule_AddObject(m, "WERR_NERR_UPSSIGNALASSERTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_UPSSIGNALASSERTED)));
	PyModule_AddObject(m, "WERR_NERR_UPSSHUTDOWNFAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_UPSSHUTDOWNFAILED)));
	PyModule_AddObject(m, "WERR_NERR_BADDOSRETCODE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BADDOSRETCODE)));
	PyModule_AddObject(m, "WERR_NERR_PROGNEEDSEXTRAMEM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_PROGNEEDSEXTRAMEM)));
	PyModule_AddObject(m, "WERR_NERR_BADDOSFUNCTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BADDOSFUNCTION)));
	PyModule_AddObject(m, "WERR_NERR_REMOTEBOOTFAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_REMOTEBOOTFAILED)));
	PyModule_AddObject(m, "WERR_NERR_BADFILECHECKSUM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BADFILECHECKSUM)));
	PyModule_AddObject(m, "WERR_NERR_NORPLBOOTSYSTEM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NORPLBOOTSYSTEM)));
	PyModule_AddObject(m, "WERR_NERR_RPLLOADRNETBIOSERR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLLOADRNETBIOSERR)));
	PyModule_AddObject(m, "WERR_NERR_RPLLOADRDISKERR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLLOADRDISKERR)));
	PyModule_AddObject(m, "WERR_NERR_IMAGEPARAMERR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_IMAGEPARAMERR)));
	PyModule_AddObject(m, "WERR_NERR_TOOMANYIMAGEPARAMS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_TOOMANYIMAGEPARAMS)));
	PyModule_AddObject(m, "WERR_NERR_NONDOSFLOPPYUSED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NONDOSFLOPPYUSED)));
	PyModule_AddObject(m, "WERR_NERR_RPLBOOTRESTART",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLBOOTRESTART)));
	PyModule_AddObject(m, "WERR_NERR_RPLSRVRCALLFAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLSRVRCALLFAILED)));
	PyModule_AddObject(m, "WERR_NERR_CANTCONNECTRPLSRVR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_CANTCONNECTRPLSRVR)));
	PyModule_AddObject(m, "WERR_NERR_CANTOPENIMAGEFILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_CANTOPENIMAGEFILE)));
	PyModule_AddObject(m, "WERR_NERR_CALLINGRPLSRVR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_CALLINGRPLSRVR)));
	PyModule_AddObject(m, "WERR_NERR_STARTINGRPLBOOT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_STARTINGRPLBOOT)));
	PyModule_AddObject(m, "WERR_NERR_RPLBOOTSERVICETERM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLBOOTSERVICETERM)));
	PyModule_AddObject(m, "WERR_NERR_RPLBOOTSTARTFAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLBOOTSTARTFAILED)));
	PyModule_AddObject(m, "WERR_NERR_RPL_CONNECTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPL_CONNECTED)));
	PyModule_AddObject(m, "WERR_NERR_BROWSERCONFIGUREDTONOTRUN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_BROWSERCONFIGUREDTONOTRUN)));
	PyModule_AddObject(m, "WERR_NERR_RPLNOADAPTERSSTARTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLNOADAPTERSSTARTED)));
	PyModule_AddObject(m, "WERR_NERR_RPLBADREGISTRY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLBADREGISTRY)));
	PyModule_AddObject(m, "WERR_NERR_RPLBADDATABASE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLBADDATABASE)));
	PyModule_AddObject(m, "WERR_NERR_RPLRPLFILESSHARE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLRPLFILESSHARE)));
	PyModule_AddObject(m, "WERR_NERR_RPLNOTRPLSERVER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLNOTRPLSERVER)));
	PyModule_AddObject(m, "WERR_NERR_RPLCANNOTENUM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLCANNOTENUM)));
	PyModule_AddObject(m, "WERR_NERR_RPLWKSTAINFOCORRUPTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLWKSTAINFOCORRUPTED)));
	PyModule_AddObject(m, "WERR_NERR_RPLWKSTANOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLWKSTANOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_RPLWKSTANAMEUNAVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLWKSTANAMEUNAVAILABLE)));
	PyModule_AddObject(m, "WERR_NERR_RPLPROFILEINFOCORRUPTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLPROFILEINFOCORRUPTED)));
	PyModule_AddObject(m, "WERR_NERR_RPLPROFILENOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLPROFILENOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_RPLPROFILENAMEUNAVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLPROFILENAMEUNAVAILABLE)));
	PyModule_AddObject(m, "WERR_NERR_RPLPROFILENOTEMPTY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLPROFILENOTEMPTY)));
	PyModule_AddObject(m, "WERR_NERR_RPLCONFIGINFOCORRUPTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLCONFIGINFOCORRUPTED)));
	PyModule_AddObject(m, "WERR_NERR_RPLCONFIGNOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLCONFIGNOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_RPLADAPTERINFOCORRUPTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLADAPTERINFOCORRUPTED)));
	PyModule_AddObject(m, "WERR_NERR_RPLINTERNAL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLINTERNAL)));
	PyModule_AddObject(m, "WERR_NERR_RPLVENDORINFOCORRUPTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLVENDORINFOCORRUPTED)));
	PyModule_AddObject(m, "WERR_NERR_RPLBOOTINFOCORRUPTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLBOOTINFOCORRUPTED)));
	PyModule_AddObject(m, "WERR_NERR_RPLWKSTANEEDSUSERACCT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLWKSTANEEDSUSERACCT)));
	PyModule_AddObject(m, "WERR_NERR_RPLNEEDSRPLUSERACCT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLNEEDSRPLUSERACCT)));
	PyModule_AddObject(m, "WERR_NERR_RPLBOOTNOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLBOOTNOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_RPLINCOMPATIBLEPROFILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLINCOMPATIBLEPROFILE)));
	PyModule_AddObject(m, "WERR_NERR_RPLADAPTERNAMEUNAVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLADAPTERNAMEUNAVAILABLE)));
	PyModule_AddObject(m, "WERR_NERR_RPLCONFIGNOTEMPTY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLCONFIGNOTEMPTY)));
	PyModule_AddObject(m, "WERR_NERR_RPLBOOTINUSE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLBOOTINUSE)));
	PyModule_AddObject(m, "WERR_NERR_RPLBACKUPDATABASE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLBACKUPDATABASE)));
	PyModule_AddObject(m, "WERR_NERR_RPLADAPTERNOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLADAPTERNOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_RPLVENDORNOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLVENDORNOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_RPLVENDORNAMEUNAVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLVENDORNAMEUNAVAILABLE)));
	PyModule_AddObject(m, "WERR_NERR_RPLBOOTNAMEUNAVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLBOOTNAMEUNAVAILABLE)));
	PyModule_AddObject(m, "WERR_NERR_RPLCONFIGNAMEUNAVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_RPLCONFIGNAMEUNAVAILABLE)));
	PyModule_AddObject(m, "WERR_NERR_DFSINTERNALCORRUPTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSINTERNALCORRUPTION)));
	PyModule_AddObject(m, "WERR_NERR_DFSVOLUMEDATACORRUPT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSVOLUMEDATACORRUPT)));
	PyModule_AddObject(m, "WERR_NERR_DFSNOSUCHVOLUME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSNOSUCHVOLUME)));
	PyModule_AddObject(m, "WERR_NERR_DFSVOLUMEALREADYEXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSVOLUMEALREADYEXISTS)));
	PyModule_AddObject(m, "WERR_NERR_DFSALREADYSHARED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSALREADYSHARED)));
	PyModule_AddObject(m, "WERR_NERR_DFSNOSUCHSHARE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSNOSUCHSHARE)));
	PyModule_AddObject(m, "WERR_NERR_DFSNOTALEAFVOLUME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSNOTALEAFVOLUME)));
	PyModule_AddObject(m, "WERR_NERR_DFSLEAFVOLUME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSLEAFVOLUME)));
	PyModule_AddObject(m, "WERR_NERR_DFSVOLUMEHASMULTIPLESERVERS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSVOLUMEHASMULTIPLESERVERS)));
	PyModule_AddObject(m, "WERR_NERR_DFSCANTCREATEJUNCTIONPOINT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSCANTCREATEJUNCTIONPOINT)));
	PyModule_AddObject(m, "WERR_NERR_DFSSERVERNOTDFSAWARE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSSERVERNOTDFSAWARE)));
	PyModule_AddObject(m, "WERR_NERR_DFSBADRENAMEPATH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSBADRENAMEPATH)));
	PyModule_AddObject(m, "WERR_NERR_DFSVOLUMEISOFFLINE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSVOLUMEISOFFLINE)));
	PyModule_AddObject(m, "WERR_NERR_DFSNOSUCHSERVER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSNOSUCHSERVER)));
	PyModule_AddObject(m, "WERR_NERR_DFSCYCLICALNAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSCYCLICALNAME)));
	PyModule_AddObject(m, "WERR_NERR_DFSNOTSUPPORTEDINSERVERDFS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSNOTSUPPORTEDINSERVERDFS)));
	PyModule_AddObject(m, "WERR_NERR_DFSDUPLICATESERVICE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSDUPLICATESERVICE)));
	PyModule_AddObject(m, "WERR_NERR_DFSCANTREMOVELASTSERVERSHARE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSCANTREMOVELASTSERVERSHARE)));
	PyModule_AddObject(m, "WERR_NERR_DFSVOLUMEISINTERDFS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSVOLUMEISINTERDFS)));
	PyModule_AddObject(m, "WERR_NERR_DFSINCONSISTENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSINCONSISTENT)));
	PyModule_AddObject(m, "WERR_NERR_DFSSERVERUPGRADED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSSERVERUPGRADED)));
	PyModule_AddObject(m, "WERR_NERR_DFSDATAISIDENTICAL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSDATAISIDENTICAL)));
	PyModule_AddObject(m, "WERR_NERR_DFSCANTREMOVEDFSROOT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSCANTREMOVEDFSROOT)));
	PyModule_AddObject(m, "WERR_NERR_DFSCHILDORPARENTINDFS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSCHILDORPARENTINDFS)));
	PyModule_AddObject(m, "WERR_NERR_DFSINTERNALERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DFSINTERNALERROR)));
	PyModule_AddObject(m, "WERR_NERR_SETUPALREADYJOINED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_SETUPALREADYJOINED)));
	PyModule_AddObject(m, "WERR_NERR_SETUPNOTJOINED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_SETUPNOTJOINED)));
	PyModule_AddObject(m, "WERR_NERR_SETUPDOMAINCONTROLLER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_SETUPDOMAINCONTROLLER)));
	PyModule_AddObject(m, "WERR_NERR_DEFAULTJOINREQUIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_DEFAULTJOINREQUIRED)));
	PyModule_AddObject(m, "WERR_NERR_INVALIDWORKGROUPNAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_INVALIDWORKGROUPNAME)));
	PyModule_AddObject(m, "WERR_NERR_NAMEUSESINCOMPATIBLECODEPAGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_NAMEUSESINCOMPATIBLECODEPAGE)));
	PyModule_AddObject(m, "WERR_NERR_COMPUTERACCOUNTNOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_COMPUTERACCOUNTNOTFOUND)));
	PyModule_AddObject(m, "WERR_NERR_PERSONALSKU",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_PERSONALSKU)));
	PyModule_AddObject(m, "WERR_NERR_PASSWORDMUSTCHANGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_PASSWORDMUSTCHANGE)));
	PyModule_AddObject(m, "WERR_NERR_ACCOUNTLOCKEDOUT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_ACCOUNTLOCKEDOUT)));
	PyModule_AddObject(m, "WERR_NERR_PASSWORDTOOLONG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_PASSWORDTOOLONG)));
	PyModule_AddObject(m, "WERR_NERR_PASSWORDNOTCOMPLEXENOUGH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_PASSWORDNOTCOMPLEXENOUGH)));
	PyModule_AddObject(m, "WERR_NERR_PASSWORDFILTERERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NERR_PASSWORDFILTERERROR)));
	PyModule_AddObject(m, "WERR_UNKNOWN_PRINT_MONITOR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNKNOWN_PRINT_MONITOR)));
	PyModule_AddObject(m, "WERR_PRINTER_DRIVER_IN_USE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PRINTER_DRIVER_IN_USE)));
	PyModule_AddObject(m, "WERR_SPOOL_FILE_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SPOOL_FILE_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_SPL_NO_STARTDOC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SPL_NO_STARTDOC)));
	PyModule_AddObject(m, "WERR_SPL_NO_ADDJOB",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SPL_NO_ADDJOB)));
	PyModule_AddObject(m, "WERR_PRINT_PROCESSOR_ALREADY_INSTALLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PRINT_PROCESSOR_ALREADY_INSTALLED)));
	PyModule_AddObject(m, "WERR_PRINT_MONITOR_ALREADY_INSTALLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PRINT_MONITOR_ALREADY_INSTALLED)));
	PyModule_AddObject(m, "WERR_INVALID_PRINT_MONITOR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_PRINT_MONITOR)));
	PyModule_AddObject(m, "WERR_PRINT_MONITOR_IN_USE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PRINT_MONITOR_IN_USE)));
	PyModule_AddObject(m, "WERR_PRINTER_HAS_JOBS_QUEUED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PRINTER_HAS_JOBS_QUEUED)));
	PyModule_AddObject(m, "WERR_SUCCESS_REBOOT_REQUIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SUCCESS_REBOOT_REQUIRED)));
	PyModule_AddObject(m, "WERR_SUCCESS_RESTART_REQUIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SUCCESS_RESTART_REQUIRED)));
	PyModule_AddObject(m, "WERR_PRINTER_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PRINTER_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_PRINTER_DRIVER_WARNED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PRINTER_DRIVER_WARNED)));
	PyModule_AddObject(m, "WERR_PRINTER_DRIVER_BLOCKED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PRINTER_DRIVER_BLOCKED)));
	PyModule_AddObject(m, "WERR_PRINTER_DRIVER_PACKAGE_IN_USE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PRINTER_DRIVER_PACKAGE_IN_USE)));
	PyModule_AddObject(m, "WERR_CORE_DRIVER_PACKAGE_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CORE_DRIVER_PACKAGE_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_FAIL_REBOOT_REQUIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FAIL_REBOOT_REQUIRED)));
	PyModule_AddObject(m, "WERR_FAIL_REBOOT_INITIATED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FAIL_REBOOT_INITIATED)));
	PyModule_AddObject(m, "WERR_PRINTER_DRIVER_DOWNLOAD_NEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PRINTER_DRIVER_DOWNLOAD_NEEDED)));
	PyModule_AddObject(m, "WERR_IO_REISSUE_AS_CACHED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IO_REISSUE_AS_CACHED)));
	PyModule_AddObject(m, "WERR_WINS_INTERNAL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WINS_INTERNAL)));
	PyModule_AddObject(m, "WERR_CAN_NOT_DEL_LOCAL_WINS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CAN_NOT_DEL_LOCAL_WINS)));
	PyModule_AddObject(m, "WERR_STATIC_INIT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_STATIC_INIT)));
	PyModule_AddObject(m, "WERR_INC_BACKUP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INC_BACKUP)));
	PyModule_AddObject(m, "WERR_FULL_BACKUP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FULL_BACKUP)));
	PyModule_AddObject(m, "WERR_REC_NON_EXISTENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REC_NON_EXISTENT)));
	PyModule_AddObject(m, "WERR_RPL_NOT_ALLOWED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RPL_NOT_ALLOWED)));
	PyModule_AddObject(m, "WERR_PEERDIST_ERROR_CONTENTINFO_VERSION_UNSUPPORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PEERDIST_ERROR_CONTENTINFO_VERSION_UNSUPPORTED)));
	PyModule_AddObject(m, "WERR_PEERDIST_ERROR_CANNOT_PARSE_CONTENTINFO",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PEERDIST_ERROR_CANNOT_PARSE_CONTENTINFO)));
	PyModule_AddObject(m, "WERR_PEERDIST_ERROR_MISSING_DATA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PEERDIST_ERROR_MISSING_DATA)));
	PyModule_AddObject(m, "WERR_PEERDIST_ERROR_NO_MORE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PEERDIST_ERROR_NO_MORE)));
	PyModule_AddObject(m, "WERR_PEERDIST_ERROR_NOT_INITIALIZED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PEERDIST_ERROR_NOT_INITIALIZED)));
	PyModule_AddObject(m, "WERR_PEERDIST_ERROR_ALREADY_INITIALIZED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PEERDIST_ERROR_ALREADY_INITIALIZED)));
	PyModule_AddObject(m, "WERR_PEERDIST_ERROR_SHUTDOWN_IN_PROGRESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PEERDIST_ERROR_SHUTDOWN_IN_PROGRESS)));
	PyModule_AddObject(m, "WERR_PEERDIST_ERROR_INVALIDATED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PEERDIST_ERROR_INVALIDATED)));
	PyModule_AddObject(m, "WERR_PEERDIST_ERROR_ALREADY_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PEERDIST_ERROR_ALREADY_EXISTS)));
	PyModule_AddObject(m, "WERR_PEERDIST_ERROR_OPERATION_NOTFOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PEERDIST_ERROR_OPERATION_NOTFOUND)));
	PyModule_AddObject(m, "WERR_PEERDIST_ERROR_ALREADY_COMPLETED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PEERDIST_ERROR_ALREADY_COMPLETED)));
	PyModule_AddObject(m, "WERR_PEERDIST_ERROR_OUT_OF_BOUNDS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PEERDIST_ERROR_OUT_OF_BOUNDS)));
	PyModule_AddObject(m, "WERR_PEERDIST_ERROR_VERSION_UNSUPPORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PEERDIST_ERROR_VERSION_UNSUPPORTED)));
	PyModule_AddObject(m, "WERR_PEERDIST_ERROR_INVALID_CONFIGURATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PEERDIST_ERROR_INVALID_CONFIGURATION)));
	PyModule_AddObject(m, "WERR_PEERDIST_ERROR_NOT_LICENSED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PEERDIST_ERROR_NOT_LICENSED)));
	PyModule_AddObject(m, "WERR_PEERDIST_ERROR_SERVICE_UNAVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PEERDIST_ERROR_SERVICE_UNAVAILABLE)));
	PyModule_AddObject(m, "WERR_DHCP_ADDRESS_CONFLICT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DHCP_ADDRESS_CONFLICT)));
	PyModule_AddObject(m, "WERR_WMI_GUID_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WMI_GUID_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_WMI_INSTANCE_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WMI_INSTANCE_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_WMI_ITEMID_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WMI_ITEMID_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_WMI_TRY_AGAIN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WMI_TRY_AGAIN)));
	PyModule_AddObject(m, "WERR_WMI_DP_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WMI_DP_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_WMI_UNRESOLVED_INSTANCE_REF",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WMI_UNRESOLVED_INSTANCE_REF)));
	PyModule_AddObject(m, "WERR_WMI_ALREADY_ENABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WMI_ALREADY_ENABLED)));
	PyModule_AddObject(m, "WERR_WMI_GUID_DISCONNECTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WMI_GUID_DISCONNECTED)));
	PyModule_AddObject(m, "WERR_WMI_SERVER_UNAVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WMI_SERVER_UNAVAILABLE)));
	PyModule_AddObject(m, "WERR_WMI_DP_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WMI_DP_FAILED)));
	PyModule_AddObject(m, "WERR_WMI_INVALID_MOF",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WMI_INVALID_MOF)));
	PyModule_AddObject(m, "WERR_WMI_INVALID_REGINFO",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WMI_INVALID_REGINFO)));
	PyModule_AddObject(m, "WERR_WMI_ALREADY_DISABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WMI_ALREADY_DISABLED)));
	PyModule_AddObject(m, "WERR_WMI_READ_ONLY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WMI_READ_ONLY)));
	PyModule_AddObject(m, "WERR_WMI_SET_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WMI_SET_FAILURE)));
	PyModule_AddObject(m, "WERR_INVALID_MEDIA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_MEDIA)));
	PyModule_AddObject(m, "WERR_INVALID_LIBRARY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_LIBRARY)));
	PyModule_AddObject(m, "WERR_INVALID_MEDIA_POOL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_MEDIA_POOL)));
	PyModule_AddObject(m, "WERR_DRIVE_MEDIA_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DRIVE_MEDIA_MISMATCH)));
	PyModule_AddObject(m, "WERR_MEDIA_OFFLINE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MEDIA_OFFLINE)));
	PyModule_AddObject(m, "WERR_LIBRARY_OFFLINE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LIBRARY_OFFLINE)));
	PyModule_AddObject(m, "WERR_EMPTY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EMPTY)));
	PyModule_AddObject(m, "WERR_NOT_EMPTY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_EMPTY)));
	PyModule_AddObject(m, "WERR_MEDIA_UNAVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MEDIA_UNAVAILABLE)));
	PyModule_AddObject(m, "WERR_RESOURCE_DISABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RESOURCE_DISABLED)));
	PyModule_AddObject(m, "WERR_INVALID_CLEANER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_CLEANER)));
	PyModule_AddObject(m, "WERR_UNABLE_TO_CLEAN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNABLE_TO_CLEAN)));
	PyModule_AddObject(m, "WERR_OBJECT_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_OBJECT_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_DATABASE_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DATABASE_FAILURE)));
	PyModule_AddObject(m, "WERR_DATABASE_FULL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DATABASE_FULL)));
	PyModule_AddObject(m, "WERR_MEDIA_INCOMPATIBLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MEDIA_INCOMPATIBLE)));
	PyModule_AddObject(m, "WERR_RESOURCE_NOT_PRESENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RESOURCE_NOT_PRESENT)));
	PyModule_AddObject(m, "WERR_INVALID_OPERATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_OPERATION)));
	PyModule_AddObject(m, "WERR_MEDIA_NOT_AVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MEDIA_NOT_AVAILABLE)));
	PyModule_AddObject(m, "WERR_DEVICE_NOT_AVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DEVICE_NOT_AVAILABLE)));
	PyModule_AddObject(m, "WERR_REQUEST_REFUSED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REQUEST_REFUSED)));
	PyModule_AddObject(m, "WERR_INVALID_DRIVE_OBJECT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_DRIVE_OBJECT)));
	PyModule_AddObject(m, "WERR_LIBRARY_FULL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LIBRARY_FULL)));
	PyModule_AddObject(m, "WERR_MEDIUM_NOT_ACCESSIBLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MEDIUM_NOT_ACCESSIBLE)));
	PyModule_AddObject(m, "WERR_UNABLE_TO_LOAD_MEDIUM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNABLE_TO_LOAD_MEDIUM)));
	PyModule_AddObject(m, "WERR_UNABLE_TO_INVENTORY_DRIVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNABLE_TO_INVENTORY_DRIVE)));
	PyModule_AddObject(m, "WERR_UNABLE_TO_INVENTORY_SLOT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNABLE_TO_INVENTORY_SLOT)));
	PyModule_AddObject(m, "WERR_UNABLE_TO_INVENTORY_TRANSPORT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNABLE_TO_INVENTORY_TRANSPORT)));
	PyModule_AddObject(m, "WERR_TRANSPORT_FULL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRANSPORT_FULL)));
	PyModule_AddObject(m, "WERR_CONTROLLING_IEPORT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CONTROLLING_IEPORT)));
	PyModule_AddObject(m, "WERR_UNABLE_TO_EJECT_MOUNTED_MEDIA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNABLE_TO_EJECT_MOUNTED_MEDIA)));
	PyModule_AddObject(m, "WERR_CLEANER_SLOT_SET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLEANER_SLOT_SET)));
	PyModule_AddObject(m, "WERR_CLEANER_SLOT_NOT_SET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLEANER_SLOT_NOT_SET)));
	PyModule_AddObject(m, "WERR_CLEANER_CARTRIDGE_SPENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLEANER_CARTRIDGE_SPENT)));
	PyModule_AddObject(m, "WERR_UNEXPECTED_OMID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNEXPECTED_OMID)));
	PyModule_AddObject(m, "WERR_CANT_DELETE_LAST_ITEM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANT_DELETE_LAST_ITEM)));
	PyModule_AddObject(m, "WERR_MESSAGE_EXCEEDS_MAX_SIZE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MESSAGE_EXCEEDS_MAX_SIZE)));
	PyModule_AddObject(m, "WERR_VOLUME_CONTAINS_SYS_FILES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_VOLUME_CONTAINS_SYS_FILES)));
	PyModule_AddObject(m, "WERR_INDIGENOUS_TYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INDIGENOUS_TYPE)));
	PyModule_AddObject(m, "WERR_NO_SUPPORTING_DRIVES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_SUPPORTING_DRIVES)));
	PyModule_AddObject(m, "WERR_CLEANER_CARTRIDGE_INSTALLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLEANER_CARTRIDGE_INSTALLED)));
	PyModule_AddObject(m, "WERR_IEPORT_FULL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IEPORT_FULL)));
	PyModule_AddObject(m, "WERR_FILE_OFFLINE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FILE_OFFLINE)));
	PyModule_AddObject(m, "WERR_REMOTE_STORAGE_NOT_ACTIVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REMOTE_STORAGE_NOT_ACTIVE)));
	PyModule_AddObject(m, "WERR_REMOTE_STORAGE_MEDIA_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REMOTE_STORAGE_MEDIA_ERROR)));
	PyModule_AddObject(m, "WERR_NOT_A_REPARSE_POINT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_A_REPARSE_POINT)));
	PyModule_AddObject(m, "WERR_REPARSE_ATTRIBUTE_CONFLICT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REPARSE_ATTRIBUTE_CONFLICT)));
	PyModule_AddObject(m, "WERR_INVALID_REPARSE_DATA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_REPARSE_DATA)));
	PyModule_AddObject(m, "WERR_REPARSE_TAG_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REPARSE_TAG_INVALID)));
	PyModule_AddObject(m, "WERR_REPARSE_TAG_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REPARSE_TAG_MISMATCH)));
	PyModule_AddObject(m, "WERR_VOLUME_NOT_SIS_ENABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_VOLUME_NOT_SIS_ENABLED)));
	PyModule_AddObject(m, "WERR_DEPENDENT_RESOURCE_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DEPENDENT_RESOURCE_EXISTS)));
	PyModule_AddObject(m, "WERR_DEPENDENCY_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DEPENDENCY_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_DEPENDENCY_ALREADY_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DEPENDENCY_ALREADY_EXISTS)));
	PyModule_AddObject(m, "WERR_RESOURCE_NOT_ONLINE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RESOURCE_NOT_ONLINE)));
	PyModule_AddObject(m, "WERR_HOST_NODE_NOT_AVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_HOST_NODE_NOT_AVAILABLE)));
	PyModule_AddObject(m, "WERR_RESOURCE_NOT_AVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RESOURCE_NOT_AVAILABLE)));
	PyModule_AddObject(m, "WERR_RESOURCE_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RESOURCE_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_SHUTDOWN_CLUSTER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SHUTDOWN_CLUSTER)));
	PyModule_AddObject(m, "WERR_CANT_EVICT_ACTIVE_NODE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANT_EVICT_ACTIVE_NODE)));
	PyModule_AddObject(m, "WERR_OBJECT_ALREADY_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_OBJECT_ALREADY_EXISTS)));
	PyModule_AddObject(m, "WERR_OBJECT_IN_LIST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_OBJECT_IN_LIST)));
	PyModule_AddObject(m, "WERR_GROUP_NOT_AVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_GROUP_NOT_AVAILABLE)));
	PyModule_AddObject(m, "WERR_GROUP_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_GROUP_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_GROUP_NOT_ONLINE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_GROUP_NOT_ONLINE)));
	PyModule_AddObject(m, "WERR_HOST_NODE_NOT_RESOURCE_OWNER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_HOST_NODE_NOT_RESOURCE_OWNER)));
	PyModule_AddObject(m, "WERR_HOST_NODE_NOT_GROUP_OWNER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_HOST_NODE_NOT_GROUP_OWNER)));
	PyModule_AddObject(m, "WERR_RESMON_CREATE_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RESMON_CREATE_FAILED)));
	PyModule_AddObject(m, "WERR_RESMON_ONLINE_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RESMON_ONLINE_FAILED)));
	PyModule_AddObject(m, "WERR_RESOURCE_ONLINE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RESOURCE_ONLINE)));
	PyModule_AddObject(m, "WERR_QUORUM_RESOURCE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_QUORUM_RESOURCE)));
	PyModule_AddObject(m, "WERR_NOT_QUORUM_CAPABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_QUORUM_CAPABLE)));
	PyModule_AddObject(m, "WERR_CLUSTER_SHUTTING_DOWN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_SHUTTING_DOWN)));
	PyModule_AddObject(m, "WERR_INVALID_STATE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_STATE)));
	PyModule_AddObject(m, "WERR_RESOURCE_PROPERTIES_STORED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RESOURCE_PROPERTIES_STORED)));
	PyModule_AddObject(m, "WERR_NOT_QUORUM_CLASS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_QUORUM_CLASS)));
	PyModule_AddObject(m, "WERR_CORE_RESOURCE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CORE_RESOURCE)));
	PyModule_AddObject(m, "WERR_QUORUM_RESOURCE_ONLINE_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_QUORUM_RESOURCE_ONLINE_FAILED)));
	PyModule_AddObject(m, "WERR_QUORUMLOG_OPEN_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_QUORUMLOG_OPEN_FAILED)));
	PyModule_AddObject(m, "WERR_CLUSTERLOG_CORRUPT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTERLOG_CORRUPT)));
	PyModule_AddObject(m, "WERR_CLUSTERLOG_RECORD_EXCEEDS_MAXSIZE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTERLOG_RECORD_EXCEEDS_MAXSIZE)));
	PyModule_AddObject(m, "WERR_CLUSTERLOG_EXCEEDS_MAXSIZE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTERLOG_EXCEEDS_MAXSIZE)));
	PyModule_AddObject(m, "WERR_CLUSTERLOG_CHKPOINT_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTERLOG_CHKPOINT_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_CLUSTERLOG_NOT_ENOUGH_SPACE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTERLOG_NOT_ENOUGH_SPACE)));
	PyModule_AddObject(m, "WERR_QUORUM_OWNER_ALIVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_QUORUM_OWNER_ALIVE)));
	PyModule_AddObject(m, "WERR_NETWORK_NOT_AVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NETWORK_NOT_AVAILABLE)));
	PyModule_AddObject(m, "WERR_NODE_NOT_AVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NODE_NOT_AVAILABLE)));
	PyModule_AddObject(m, "WERR_ALL_NODES_NOT_AVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ALL_NODES_NOT_AVAILABLE)));
	PyModule_AddObject(m, "WERR_RESOURCE_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RESOURCE_FAILED)));
	PyModule_AddObject(m, "WERR_CLUSTER_INVALID_NODE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_INVALID_NODE)));
	PyModule_AddObject(m, "WERR_CLUSTER_NODE_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NODE_EXISTS)));
	PyModule_AddObject(m, "WERR_CLUSTER_JOIN_IN_PROGRESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_JOIN_IN_PROGRESS)));
	PyModule_AddObject(m, "WERR_CLUSTER_NODE_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NODE_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_CLUSTER_LOCAL_NODE_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_LOCAL_NODE_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_CLUSTER_NETWORK_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NETWORK_EXISTS)));
	PyModule_AddObject(m, "WERR_CLUSTER_NETWORK_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NETWORK_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_CLUSTER_NETINTERFACE_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NETINTERFACE_EXISTS)));
	PyModule_AddObject(m, "WERR_CLUSTER_NETINTERFACE_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NETINTERFACE_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_CLUSTER_INVALID_REQUEST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_INVALID_REQUEST)));
	PyModule_AddObject(m, "WERR_CLUSTER_INVALID_NETWORK_PROVIDER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_INVALID_NETWORK_PROVIDER)));
	PyModule_AddObject(m, "WERR_CLUSTER_NODE_DOWN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NODE_DOWN)));
	PyModule_AddObject(m, "WERR_CLUSTER_NODE_UNREACHABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NODE_UNREACHABLE)));
	PyModule_AddObject(m, "WERR_CLUSTER_NODE_NOT_MEMBER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NODE_NOT_MEMBER)));
	PyModule_AddObject(m, "WERR_CLUSTER_JOIN_NOT_IN_PROGRESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_JOIN_NOT_IN_PROGRESS)));
	PyModule_AddObject(m, "WERR_CLUSTER_INVALID_NETWORK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_INVALID_NETWORK)));
	PyModule_AddObject(m, "WERR_CLUSTER_NODE_UP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NODE_UP)));
	PyModule_AddObject(m, "WERR_CLUSTER_IPADDR_IN_USE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_IPADDR_IN_USE)));
	PyModule_AddObject(m, "WERR_CLUSTER_NODE_NOT_PAUSED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NODE_NOT_PAUSED)));
	PyModule_AddObject(m, "WERR_CLUSTER_NO_SECURITY_CONTEXT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NO_SECURITY_CONTEXT)));
	PyModule_AddObject(m, "WERR_CLUSTER_NETWORK_NOT_INTERNAL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NETWORK_NOT_INTERNAL)));
	PyModule_AddObject(m, "WERR_CLUSTER_NODE_ALREADY_UP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NODE_ALREADY_UP)));
	PyModule_AddObject(m, "WERR_CLUSTER_NODE_ALREADY_DOWN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NODE_ALREADY_DOWN)));
	PyModule_AddObject(m, "WERR_CLUSTER_NETWORK_ALREADY_ONLINE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NETWORK_ALREADY_ONLINE)));
	PyModule_AddObject(m, "WERR_CLUSTER_NETWORK_ALREADY_OFFLINE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NETWORK_ALREADY_OFFLINE)));
	PyModule_AddObject(m, "WERR_CLUSTER_NODE_ALREADY_MEMBER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NODE_ALREADY_MEMBER)));
	PyModule_AddObject(m, "WERR_CLUSTER_LAST_INTERNAL_NETWORK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_LAST_INTERNAL_NETWORK)));
	PyModule_AddObject(m, "WERR_CLUSTER_NETWORK_HAS_DEPENDENTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NETWORK_HAS_DEPENDENTS)));
	PyModule_AddObject(m, "WERR_INVALID_OPERATION_ON_QUORUM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_OPERATION_ON_QUORUM)));
	PyModule_AddObject(m, "WERR_DEPENDENCY_NOT_ALLOWED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DEPENDENCY_NOT_ALLOWED)));
	PyModule_AddObject(m, "WERR_CLUSTER_NODE_PAUSED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NODE_PAUSED)));
	PyModule_AddObject(m, "WERR_NODE_CANT_HOST_RESOURCE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NODE_CANT_HOST_RESOURCE)));
	PyModule_AddObject(m, "WERR_CLUSTER_NODE_NOT_READY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NODE_NOT_READY)));
	PyModule_AddObject(m, "WERR_CLUSTER_NODE_SHUTTING_DOWN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NODE_SHUTTING_DOWN)));
	PyModule_AddObject(m, "WERR_CLUSTER_JOIN_ABORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_JOIN_ABORTED)));
	PyModule_AddObject(m, "WERR_CLUSTER_INCOMPATIBLE_VERSIONS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_INCOMPATIBLE_VERSIONS)));
	PyModule_AddObject(m, "WERR_CLUSTER_MAXNUM_OF_RESOURCES_EXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_MAXNUM_OF_RESOURCES_EXCEEDED)));
	PyModule_AddObject(m, "WERR_CLUSTER_SYSTEM_CONFIG_CHANGED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_SYSTEM_CONFIG_CHANGED)));
	PyModule_AddObject(m, "WERR_CLUSTER_RESOURCE_TYPE_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_RESOURCE_TYPE_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_CLUSTER_RESTYPE_NOT_SUPPORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_RESTYPE_NOT_SUPPORTED)));
	PyModule_AddObject(m, "WERR_CLUSTER_RESNAME_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_RESNAME_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_CLUSTER_NO_RPC_PACKAGES_REGISTERED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NO_RPC_PACKAGES_REGISTERED)));
	PyModule_AddObject(m, "WERR_CLUSTER_OWNER_NOT_IN_PREFLIST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_OWNER_NOT_IN_PREFLIST)));
	PyModule_AddObject(m, "WERR_CLUSTER_DATABASE_SEQMISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_DATABASE_SEQMISMATCH)));
	PyModule_AddObject(m, "WERR_RESMON_INVALID_STATE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RESMON_INVALID_STATE)));
	PyModule_AddObject(m, "WERR_CLUSTER_GUM_NOT_LOCKER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_GUM_NOT_LOCKER)));
	PyModule_AddObject(m, "WERR_QUORUM_DISK_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_QUORUM_DISK_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_DATABASE_BACKUP_CORRUPT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DATABASE_BACKUP_CORRUPT)));
	PyModule_AddObject(m, "WERR_CLUSTER_NODE_ALREADY_HAS_DFS_ROOT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NODE_ALREADY_HAS_DFS_ROOT)));
	PyModule_AddObject(m, "WERR_RESOURCE_PROPERTY_UNCHANGEABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RESOURCE_PROPERTY_UNCHANGEABLE)));
	PyModule_AddObject(m, "WERR_CLUSTER_MEMBERSHIP_INVALID_STATE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_MEMBERSHIP_INVALID_STATE)));
	PyModule_AddObject(m, "WERR_CLUSTER_QUORUMLOG_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_QUORUMLOG_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_CLUSTER_MEMBERSHIP_HALT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_MEMBERSHIP_HALT)));
	PyModule_AddObject(m, "WERR_CLUSTER_INSTANCE_ID_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_INSTANCE_ID_MISMATCH)));
	PyModule_AddObject(m, "WERR_CLUSTER_NETWORK_NOT_FOUND_FOR_IP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NETWORK_NOT_FOUND_FOR_IP)));
	PyModule_AddObject(m, "WERR_CLUSTER_PROPERTY_DATA_TYPE_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_PROPERTY_DATA_TYPE_MISMATCH)));
	PyModule_AddObject(m, "WERR_CLUSTER_EVICT_WITHOUT_CLEANUP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_EVICT_WITHOUT_CLEANUP)));
	PyModule_AddObject(m, "WERR_CLUSTER_PARAMETER_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_PARAMETER_MISMATCH)));
	PyModule_AddObject(m, "WERR_NODE_CANNOT_BE_CLUSTERED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NODE_CANNOT_BE_CLUSTERED)));
	PyModule_AddObject(m, "WERR_CLUSTER_WRONG_OS_VERSION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_WRONG_OS_VERSION)));
	PyModule_AddObject(m, "WERR_CLUSTER_CANT_CREATE_DUP_CLUSTER_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_CANT_CREATE_DUP_CLUSTER_NAME)));
	PyModule_AddObject(m, "WERR_CLUSCFG_ALREADY_COMMITTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSCFG_ALREADY_COMMITTED)));
	PyModule_AddObject(m, "WERR_CLUSCFG_ROLLBACK_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSCFG_ROLLBACK_FAILED)));
	PyModule_AddObject(m, "WERR_CLUSCFG_SYSTEM_DISK_DRIVE_LETTER_CONFLICT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSCFG_SYSTEM_DISK_DRIVE_LETTER_CONFLICT)));
	PyModule_AddObject(m, "WERR_CLUSTER_OLD_VERSION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_OLD_VERSION)));
	PyModule_AddObject(m, "WERR_CLUSTER_MISMATCHED_COMPUTER_ACCT_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_MISMATCHED_COMPUTER_ACCT_NAME)));
	PyModule_AddObject(m, "WERR_CLUSTER_NO_NET_ADAPTERS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NO_NET_ADAPTERS)));
	PyModule_AddObject(m, "WERR_CLUSTER_POISONED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_POISONED)));
	PyModule_AddObject(m, "WERR_CLUSTER_GROUP_MOVING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_GROUP_MOVING)));
	PyModule_AddObject(m, "WERR_CLUSTER_RESOURCE_TYPE_BUSY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_RESOURCE_TYPE_BUSY)));
	PyModule_AddObject(m, "WERR_RESOURCE_CALL_TIMED_OUT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RESOURCE_CALL_TIMED_OUT)));
	PyModule_AddObject(m, "WERR_INVALID_CLUSTER_IPV6_ADDRESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_CLUSTER_IPV6_ADDRESS)));
	PyModule_AddObject(m, "WERR_CLUSTER_INTERNAL_INVALID_FUNCTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_INTERNAL_INVALID_FUNCTION)));
	PyModule_AddObject(m, "WERR_CLUSTER_PARAMETER_OUT_OF_BOUNDS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_PARAMETER_OUT_OF_BOUNDS)));
	PyModule_AddObject(m, "WERR_CLUSTER_PARTIAL_SEND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_PARTIAL_SEND)));
	PyModule_AddObject(m, "WERR_CLUSTER_REGISTRY_INVALID_FUNCTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_REGISTRY_INVALID_FUNCTION)));
	PyModule_AddObject(m, "WERR_CLUSTER_INVALID_STRING_TERMINATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_INVALID_STRING_TERMINATION)));
	PyModule_AddObject(m, "WERR_CLUSTER_INVALID_STRING_FORMAT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_INVALID_STRING_FORMAT)));
	PyModule_AddObject(m, "WERR_CLUSTER_DATABASE_TRANSACTION_IN_PROGRESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_DATABASE_TRANSACTION_IN_PROGRESS)));
	PyModule_AddObject(m, "WERR_CLUSTER_DATABASE_TRANSACTION_NOT_IN_PROGRESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_DATABASE_TRANSACTION_NOT_IN_PROGRESS)));
	PyModule_AddObject(m, "WERR_CLUSTER_NULL_DATA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NULL_DATA)));
	PyModule_AddObject(m, "WERR_CLUSTER_PARTIAL_READ",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_PARTIAL_READ)));
	PyModule_AddObject(m, "WERR_CLUSTER_PARTIAL_WRITE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_PARTIAL_WRITE)));
	PyModule_AddObject(m, "WERR_CLUSTER_CANT_DESERIALIZE_DATA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_CANT_DESERIALIZE_DATA)));
	PyModule_AddObject(m, "WERR_DEPENDENT_RESOURCE_PROPERTY_CONFLICT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DEPENDENT_RESOURCE_PROPERTY_CONFLICT)));
	PyModule_AddObject(m, "WERR_CLUSTER_NO_QUORUM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_NO_QUORUM)));
	PyModule_AddObject(m, "WERR_CLUSTER_INVALID_IPV6_NETWORK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_INVALID_IPV6_NETWORK)));
	PyModule_AddObject(m, "WERR_CLUSTER_INVALID_IPV6_TUNNEL_NETWORK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CLUSTER_INVALID_IPV6_TUNNEL_NETWORK)));
	PyModule_AddObject(m, "WERR_QUORUM_NOT_ALLOWED_IN_THIS_GROUP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_QUORUM_NOT_ALLOWED_IN_THIS_GROUP)));
	PyModule_AddObject(m, "WERR_ENCRYPTION_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ENCRYPTION_FAILED)));
	PyModule_AddObject(m, "WERR_DECRYPTION_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DECRYPTION_FAILED)));
	PyModule_AddObject(m, "WERR_FILE_ENCRYPTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FILE_ENCRYPTED)));
	PyModule_AddObject(m, "WERR_NO_RECOVERY_POLICY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_RECOVERY_POLICY)));
	PyModule_AddObject(m, "WERR_NO_EFS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_EFS)));
	PyModule_AddObject(m, "WERR_WRONG_EFS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WRONG_EFS)));
	PyModule_AddObject(m, "WERR_NO_USER_KEYS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_USER_KEYS)));
	PyModule_AddObject(m, "WERR_FILE_NOT_ENCRYPTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FILE_NOT_ENCRYPTED)));
	PyModule_AddObject(m, "WERR_NOT_EXPORT_FORMAT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_EXPORT_FORMAT)));
	PyModule_AddObject(m, "WERR_FILE_READ_ONLY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FILE_READ_ONLY)));
	PyModule_AddObject(m, "WERR_DIR_EFS_DISALLOWED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DIR_EFS_DISALLOWED)));
	PyModule_AddObject(m, "WERR_EFS_SERVER_NOT_TRUSTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EFS_SERVER_NOT_TRUSTED)));
	PyModule_AddObject(m, "WERR_BAD_RECOVERY_POLICY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_BAD_RECOVERY_POLICY)));
	PyModule_AddObject(m, "WERR_EFS_ALG_BLOB_TOO_BIG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EFS_ALG_BLOB_TOO_BIG)));
	PyModule_AddObject(m, "WERR_VOLUME_NOT_SUPPORT_EFS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_VOLUME_NOT_SUPPORT_EFS)));
	PyModule_AddObject(m, "WERR_EFS_DISABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EFS_DISABLED)));
	PyModule_AddObject(m, "WERR_EFS_VERSION_NOT_SUPPORT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EFS_VERSION_NOT_SUPPORT)));
	PyModule_AddObject(m, "WERR_CS_ENCRYPTION_INVALID_SERVER_RESPONSE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CS_ENCRYPTION_INVALID_SERVER_RESPONSE)));
	PyModule_AddObject(m, "WERR_CS_ENCRYPTION_UNSUPPORTED_SERVER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CS_ENCRYPTION_UNSUPPORTED_SERVER)));
	PyModule_AddObject(m, "WERR_CS_ENCRYPTION_EXISTING_ENCRYPTED_FILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CS_ENCRYPTION_EXISTING_ENCRYPTED_FILE)));
	PyModule_AddObject(m, "WERR_CS_ENCRYPTION_NEW_ENCRYPTED_FILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CS_ENCRYPTION_NEW_ENCRYPTED_FILE)));
	PyModule_AddObject(m, "WERR_CS_ENCRYPTION_FILE_NOT_CSE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CS_ENCRYPTION_FILE_NOT_CSE)));
	PyModule_AddObject(m, "WERR_NO_BROWSER_SERVERS_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_BROWSER_SERVERS_FOUND)));
	PyModule_AddObject(m, "WERR_SCHED_E_SERVICE_NOT_LOCALSYSTEM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SCHED_E_SERVICE_NOT_LOCALSYSTEM)));
	PyModule_AddObject(m, "WERR_LOG_SECTOR_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_SECTOR_INVALID)));
	PyModule_AddObject(m, "WERR_LOG_SECTOR_PARITY_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_SECTOR_PARITY_INVALID)));
	PyModule_AddObject(m, "WERR_LOG_SECTOR_REMAPPED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_SECTOR_REMAPPED)));
	PyModule_AddObject(m, "WERR_LOG_BLOCK_INCOMPLETE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_BLOCK_INCOMPLETE)));
	PyModule_AddObject(m, "WERR_LOG_INVALID_RANGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_INVALID_RANGE)));
	PyModule_AddObject(m, "WERR_LOG_BLOCKS_EXHAUSTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_BLOCKS_EXHAUSTED)));
	PyModule_AddObject(m, "WERR_LOG_READ_CONTEXT_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_READ_CONTEXT_INVALID)));
	PyModule_AddObject(m, "WERR_LOG_RESTART_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_RESTART_INVALID)));
	PyModule_AddObject(m, "WERR_LOG_BLOCK_VERSION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_BLOCK_VERSION)));
	PyModule_AddObject(m, "WERR_LOG_BLOCK_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_BLOCK_INVALID)));
	PyModule_AddObject(m, "WERR_LOG_READ_MODE_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_READ_MODE_INVALID)));
	PyModule_AddObject(m, "WERR_LOG_NO_RESTART",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_NO_RESTART)));
	PyModule_AddObject(m, "WERR_LOG_METADATA_CORRUPT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_METADATA_CORRUPT)));
	PyModule_AddObject(m, "WERR_LOG_METADATA_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_METADATA_INVALID)));
	PyModule_AddObject(m, "WERR_LOG_METADATA_INCONSISTENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_METADATA_INCONSISTENT)));
	PyModule_AddObject(m, "WERR_LOG_RESERVATION_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_RESERVATION_INVALID)));
	PyModule_AddObject(m, "WERR_LOG_CANT_DELETE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_CANT_DELETE)));
	PyModule_AddObject(m, "WERR_LOG_CONTAINER_LIMIT_EXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_CONTAINER_LIMIT_EXCEEDED)));
	PyModule_AddObject(m, "WERR_LOG_START_OF_LOG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_START_OF_LOG)));
	PyModule_AddObject(m, "WERR_LOG_POLICY_ALREADY_INSTALLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_POLICY_ALREADY_INSTALLED)));
	PyModule_AddObject(m, "WERR_LOG_POLICY_NOT_INSTALLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_POLICY_NOT_INSTALLED)));
	PyModule_AddObject(m, "WERR_LOG_POLICY_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_POLICY_INVALID)));
	PyModule_AddObject(m, "WERR_LOG_POLICY_CONFLICT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_POLICY_CONFLICT)));
	PyModule_AddObject(m, "WERR_LOG_PINNED_ARCHIVE_TAIL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_PINNED_ARCHIVE_TAIL)));
	PyModule_AddObject(m, "WERR_LOG_RECORD_NONEXISTENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_RECORD_NONEXISTENT)));
	PyModule_AddObject(m, "WERR_LOG_RECORDS_RESERVED_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_RECORDS_RESERVED_INVALID)));
	PyModule_AddObject(m, "WERR_LOG_SPACE_RESERVED_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_SPACE_RESERVED_INVALID)));
	PyModule_AddObject(m, "WERR_LOG_TAIL_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_TAIL_INVALID)));
	PyModule_AddObject(m, "WERR_LOG_FULL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_FULL)));
	PyModule_AddObject(m, "WERR_COULD_NOT_RESIZE_LOG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_COULD_NOT_RESIZE_LOG)));
	PyModule_AddObject(m, "WERR_LOG_MULTIPLEXED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_MULTIPLEXED)));
	PyModule_AddObject(m, "WERR_LOG_DEDICATED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_DEDICATED)));
	PyModule_AddObject(m, "WERR_LOG_ARCHIVE_NOT_IN_PROGRESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_ARCHIVE_NOT_IN_PROGRESS)));
	PyModule_AddObject(m, "WERR_LOG_ARCHIVE_IN_PROGRESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_ARCHIVE_IN_PROGRESS)));
	PyModule_AddObject(m, "WERR_LOG_EPHEMERAL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_EPHEMERAL)));
	PyModule_AddObject(m, "WERR_LOG_NOT_ENOUGH_CONTAINERS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_NOT_ENOUGH_CONTAINERS)));
	PyModule_AddObject(m, "WERR_LOG_CLIENT_ALREADY_REGISTERED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_CLIENT_ALREADY_REGISTERED)));
	PyModule_AddObject(m, "WERR_LOG_CLIENT_NOT_REGISTERED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_CLIENT_NOT_REGISTERED)));
	PyModule_AddObject(m, "WERR_LOG_FULL_HANDLER_IN_PROGRESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_FULL_HANDLER_IN_PROGRESS)));
	PyModule_AddObject(m, "WERR_LOG_CONTAINER_READ_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_CONTAINER_READ_FAILED)));
	PyModule_AddObject(m, "WERR_LOG_CONTAINER_WRITE_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_CONTAINER_WRITE_FAILED)));
	PyModule_AddObject(m, "WERR_LOG_CONTAINER_OPEN_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_CONTAINER_OPEN_FAILED)));
	PyModule_AddObject(m, "WERR_LOG_CONTAINER_STATE_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_CONTAINER_STATE_INVALID)));
	PyModule_AddObject(m, "WERR_LOG_STATE_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_STATE_INVALID)));
	PyModule_AddObject(m, "WERR_LOG_PINNED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_PINNED)));
	PyModule_AddObject(m, "WERR_LOG_METADATA_FLUSH_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_METADATA_FLUSH_FAILED)));
	PyModule_AddObject(m, "WERR_LOG_INCONSISTENT_SECURITY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_INCONSISTENT_SECURITY)));
	PyModule_AddObject(m, "WERR_LOG_APPENDED_FLUSH_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_APPENDED_FLUSH_FAILED)));
	PyModule_AddObject(m, "WERR_LOG_PINNED_RESERVATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_PINNED_RESERVATION)));
	PyModule_AddObject(m, "WERR_INVALID_TRANSACTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INVALID_TRANSACTION)));
	PyModule_AddObject(m, "WERR_TRANSACTION_NOT_ACTIVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRANSACTION_NOT_ACTIVE)));
	PyModule_AddObject(m, "WERR_TRANSACTION_REQUEST_NOT_VALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRANSACTION_REQUEST_NOT_VALID)));
	PyModule_AddObject(m, "WERR_TRANSACTION_NOT_REQUESTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRANSACTION_NOT_REQUESTED)));
	PyModule_AddObject(m, "WERR_TRANSACTION_ALREADY_ABORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRANSACTION_ALREADY_ABORTED)));
	PyModule_AddObject(m, "WERR_TRANSACTION_ALREADY_COMMITTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRANSACTION_ALREADY_COMMITTED)));
	PyModule_AddObject(m, "WERR_TM_INITIALIZATION_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TM_INITIALIZATION_FAILED)));
	PyModule_AddObject(m, "WERR_RESOURCEMANAGER_READ_ONLY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RESOURCEMANAGER_READ_ONLY)));
	PyModule_AddObject(m, "WERR_TRANSACTION_NOT_JOINED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRANSACTION_NOT_JOINED)));
	PyModule_AddObject(m, "WERR_TRANSACTION_SUPERIOR_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRANSACTION_SUPERIOR_EXISTS)));
	PyModule_AddObject(m, "WERR_CRM_PROTOCOL_ALREADY_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CRM_PROTOCOL_ALREADY_EXISTS)));
	PyModule_AddObject(m, "WERR_TRANSACTION_PROPAGATION_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRANSACTION_PROPAGATION_FAILED)));
	PyModule_AddObject(m, "WERR_CRM_PROTOCOL_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CRM_PROTOCOL_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_TRANSACTION_INVALID_MARSHALL_BUFFER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRANSACTION_INVALID_MARSHALL_BUFFER)));
	PyModule_AddObject(m, "WERR_CURRENT_TRANSACTION_NOT_VALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CURRENT_TRANSACTION_NOT_VALID)));
	PyModule_AddObject(m, "WERR_TRANSACTION_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRANSACTION_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_RESOURCEMANAGER_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RESOURCEMANAGER_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_ENLISTMENT_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ENLISTMENT_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_TRANSACTIONMANAGER_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRANSACTIONMANAGER_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_TRANSACTIONMANAGER_NOT_ONLINE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRANSACTIONMANAGER_NOT_ONLINE)));
	PyModule_AddObject(m, "WERR_TRANSACTIONMANAGER_RECOVERY_NAME_COLLISION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRANSACTIONMANAGER_RECOVERY_NAME_COLLISION)));
	PyModule_AddObject(m, "WERR_TRANSACTIONAL_CONFLICT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRANSACTIONAL_CONFLICT)));
	PyModule_AddObject(m, "WERR_RM_NOT_ACTIVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RM_NOT_ACTIVE)));
	PyModule_AddObject(m, "WERR_RM_METADATA_CORRUPT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RM_METADATA_CORRUPT)));
	PyModule_AddObject(m, "WERR_DIRECTORY_NOT_RM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DIRECTORY_NOT_RM)));
	PyModule_AddObject(m, "WERR_TRANSACTIONS_UNSUPPORTED_REMOTE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRANSACTIONS_UNSUPPORTED_REMOTE)));
	PyModule_AddObject(m, "WERR_LOG_RESIZE_INVALID_SIZE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_RESIZE_INVALID_SIZE)));
	PyModule_AddObject(m, "WERR_OBJECT_NO_LONGER_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_OBJECT_NO_LONGER_EXISTS)));
	PyModule_AddObject(m, "WERR_STREAM_MINIVERSION_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_STREAM_MINIVERSION_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_STREAM_MINIVERSION_NOT_VALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_STREAM_MINIVERSION_NOT_VALID)));
	PyModule_AddObject(m, "WERR_MINIVERSION_INACCESSIBLE_FROM_SPECIFIED_TRANSACTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MINIVERSION_INACCESSIBLE_FROM_SPECIFIED_TRANSACTION)));
	PyModule_AddObject(m, "WERR_CANT_OPEN_MINIVERSION_WITH_MODIFY_INTENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANT_OPEN_MINIVERSION_WITH_MODIFY_INTENT)));
	PyModule_AddObject(m, "WERR_CANT_CREATE_MORE_STREAM_MINIVERSIONS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANT_CREATE_MORE_STREAM_MINIVERSIONS)));
	PyModule_AddObject(m, "WERR_REMOTE_FILE_VERSION_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_REMOTE_FILE_VERSION_MISMATCH)));
	PyModule_AddObject(m, "WERR_HANDLE_NO_LONGER_VALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_HANDLE_NO_LONGER_VALID)));
	PyModule_AddObject(m, "WERR_NO_TXF_METADATA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_TXF_METADATA)));
	PyModule_AddObject(m, "WERR_LOG_CORRUPTION_DETECTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_CORRUPTION_DETECTED)));
	PyModule_AddObject(m, "WERR_CANT_RECOVER_WITH_HANDLE_OPEN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANT_RECOVER_WITH_HANDLE_OPEN)));
	PyModule_AddObject(m, "WERR_RM_DISCONNECTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RM_DISCONNECTED)));
	PyModule_AddObject(m, "WERR_ENLISTMENT_NOT_SUPERIOR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ENLISTMENT_NOT_SUPERIOR)));
	PyModule_AddObject(m, "WERR_RECOVERY_NOT_NEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RECOVERY_NOT_NEEDED)));
	PyModule_AddObject(m, "WERR_RM_ALREADY_STARTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RM_ALREADY_STARTED)));
	PyModule_AddObject(m, "WERR_FILE_IDENTITY_NOT_PERSISTENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FILE_IDENTITY_NOT_PERSISTENT)));
	PyModule_AddObject(m, "WERR_CANT_BREAK_TRANSACTIONAL_DEPENDENCY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANT_BREAK_TRANSACTIONAL_DEPENDENCY)));
	PyModule_AddObject(m, "WERR_CANT_CROSS_RM_BOUNDARY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANT_CROSS_RM_BOUNDARY)));
	PyModule_AddObject(m, "WERR_TXF_DIR_NOT_EMPTY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TXF_DIR_NOT_EMPTY)));
	PyModule_AddObject(m, "WERR_INDOUBT_TRANSACTIONS_EXIST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_INDOUBT_TRANSACTIONS_EXIST)));
	PyModule_AddObject(m, "WERR_TM_VOLATILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TM_VOLATILE)));
	PyModule_AddObject(m, "WERR_ROLLBACK_TIMER_EXPIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ROLLBACK_TIMER_EXPIRED)));
	PyModule_AddObject(m, "WERR_TXF_ATTRIBUTE_CORRUPT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TXF_ATTRIBUTE_CORRUPT)));
	PyModule_AddObject(m, "WERR_EFS_NOT_ALLOWED_IN_TRANSACTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EFS_NOT_ALLOWED_IN_TRANSACTION)));
	PyModule_AddObject(m, "WERR_TRANSACTIONAL_OPEN_NOT_ALLOWED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRANSACTIONAL_OPEN_NOT_ALLOWED)));
	PyModule_AddObject(m, "WERR_LOG_GROWTH_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_LOG_GROWTH_FAILED)));
	PyModule_AddObject(m, "WERR_TRANSACTED_MAPPING_UNSUPPORTED_REMOTE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRANSACTED_MAPPING_UNSUPPORTED_REMOTE)));
	PyModule_AddObject(m, "WERR_TXF_METADATA_ALREADY_PRESENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TXF_METADATA_ALREADY_PRESENT)));
	PyModule_AddObject(m, "WERR_TRANSACTION_SCOPE_CALLBACKS_NOT_SET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRANSACTION_SCOPE_CALLBACKS_NOT_SET)));
	PyModule_AddObject(m, "WERR_TRANSACTION_REQUIRED_PROMOTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRANSACTION_REQUIRED_PROMOTION)));
	PyModule_AddObject(m, "WERR_CANNOT_EXECUTE_FILE_IN_TRANSACTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANNOT_EXECUTE_FILE_IN_TRANSACTION)));
	PyModule_AddObject(m, "WERR_TRANSACTIONS_NOT_FROZEN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRANSACTIONS_NOT_FROZEN)));
	PyModule_AddObject(m, "WERR_TRANSACTION_FREEZE_IN_PROGRESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TRANSACTION_FREEZE_IN_PROGRESS)));
	PyModule_AddObject(m, "WERR_NOT_SNAPSHOT_VOLUME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_SNAPSHOT_VOLUME)));
	PyModule_AddObject(m, "WERR_NO_SAVEPOINT_WITH_OPEN_FILES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_SAVEPOINT_WITH_OPEN_FILES)));
	PyModule_AddObject(m, "WERR_DATA_LOST_REPAIR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DATA_LOST_REPAIR)));
	PyModule_AddObject(m, "WERR_SPARSE_NOT_ALLOWED_IN_TRANSACTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SPARSE_NOT_ALLOWED_IN_TRANSACTION)));
	PyModule_AddObject(m, "WERR_TM_IDENTITY_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TM_IDENTITY_MISMATCH)));
	PyModule_AddObject(m, "WERR_FLOATED_SECTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FLOATED_SECTION)));
	PyModule_AddObject(m, "WERR_CANNOT_ACCEPT_TRANSACTED_WORK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANNOT_ACCEPT_TRANSACTED_WORK)));
	PyModule_AddObject(m, "WERR_CANNOT_ABORT_TRANSACTIONS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CANNOT_ABORT_TRANSACTIONS)));
	PyModule_AddObject(m, "WERR_CTX_WINSTATION_NAME_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_WINSTATION_NAME_INVALID)));
	PyModule_AddObject(m, "WERR_CTX_INVALID_PD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_INVALID_PD)));
	PyModule_AddObject(m, "WERR_CTX_PD_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_PD_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_CTX_WD_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_WD_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_CTX_CANNOT_MAKE_EVENTLOG_ENTRY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_CANNOT_MAKE_EVENTLOG_ENTRY)));
	PyModule_AddObject(m, "WERR_CTX_SERVICE_NAME_COLLISION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_SERVICE_NAME_COLLISION)));
	PyModule_AddObject(m, "WERR_CTX_CLOSE_PENDING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_CLOSE_PENDING)));
	PyModule_AddObject(m, "WERR_CTX_NO_OUTBUF",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_NO_OUTBUF)));
	PyModule_AddObject(m, "WERR_CTX_MODEM_INF_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_MODEM_INF_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_CTX_INVALID_MODEMNAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_INVALID_MODEMNAME)));
	PyModule_AddObject(m, "WERR_CTX_MODEM_RESPONSE_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_MODEM_RESPONSE_ERROR)));
	PyModule_AddObject(m, "WERR_CTX_MODEM_RESPONSE_TIMEOUT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_MODEM_RESPONSE_TIMEOUT)));
	PyModule_AddObject(m, "WERR_CTX_MODEM_RESPONSE_NO_CARRIER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_MODEM_RESPONSE_NO_CARRIER)));
	PyModule_AddObject(m, "WERR_CTX_MODEM_RESPONSE_NO_DIALTONE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_MODEM_RESPONSE_NO_DIALTONE)));
	PyModule_AddObject(m, "WERR_CTX_MODEM_RESPONSE_BUSY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_MODEM_RESPONSE_BUSY)));
	PyModule_AddObject(m, "WERR_CTX_MODEM_RESPONSE_VOICE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_MODEM_RESPONSE_VOICE)));
	PyModule_AddObject(m, "WERR_CTX_TD_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_TD_ERROR)));
	PyModule_AddObject(m, "WERR_CTX_WINSTATION_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_WINSTATION_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_CTX_WINSTATION_ALREADY_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_WINSTATION_ALREADY_EXISTS)));
	PyModule_AddObject(m, "WERR_CTX_WINSTATION_BUSY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_WINSTATION_BUSY)));
	PyModule_AddObject(m, "WERR_CTX_BAD_VIDEO_MODE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_BAD_VIDEO_MODE)));
	PyModule_AddObject(m, "WERR_CTX_GRAPHICS_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_GRAPHICS_INVALID)));
	PyModule_AddObject(m, "WERR_CTX_LOGON_DISABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_LOGON_DISABLED)));
	PyModule_AddObject(m, "WERR_CTX_NOT_CONSOLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_NOT_CONSOLE)));
	PyModule_AddObject(m, "WERR_CTX_CLIENT_QUERY_TIMEOUT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_CLIENT_QUERY_TIMEOUT)));
	PyModule_AddObject(m, "WERR_CTX_CONSOLE_DISCONNECT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_CONSOLE_DISCONNECT)));
	PyModule_AddObject(m, "WERR_CTX_CONSOLE_CONNECT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_CONSOLE_CONNECT)));
	PyModule_AddObject(m, "WERR_CTX_SHADOW_DENIED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_SHADOW_DENIED)));
	PyModule_AddObject(m, "WERR_CTX_WINSTATION_ACCESS_DENIED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_WINSTATION_ACCESS_DENIED)));
	PyModule_AddObject(m, "WERR_CTX_INVALID_WD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_INVALID_WD)));
	PyModule_AddObject(m, "WERR_CTX_SHADOW_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_SHADOW_INVALID)));
	PyModule_AddObject(m, "WERR_CTX_SHADOW_DISABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_SHADOW_DISABLED)));
	PyModule_AddObject(m, "WERR_CTX_CLIENT_LICENSE_IN_USE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_CLIENT_LICENSE_IN_USE)));
	PyModule_AddObject(m, "WERR_CTX_CLIENT_LICENSE_NOT_SET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_CLIENT_LICENSE_NOT_SET)));
	PyModule_AddObject(m, "WERR_CTX_LICENSE_NOT_AVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_LICENSE_NOT_AVAILABLE)));
	PyModule_AddObject(m, "WERR_CTX_LICENSE_CLIENT_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_LICENSE_CLIENT_INVALID)));
	PyModule_AddObject(m, "WERR_CTX_LICENSE_EXPIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_LICENSE_EXPIRED)));
	PyModule_AddObject(m, "WERR_CTX_SHADOW_NOT_RUNNING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_SHADOW_NOT_RUNNING)));
	PyModule_AddObject(m, "WERR_CTX_SHADOW_ENDED_BY_MODE_CHANGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_SHADOW_ENDED_BY_MODE_CHANGE)));
	PyModule_AddObject(m, "WERR_ACTIVATION_COUNT_EXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ACTIVATION_COUNT_EXCEEDED)));
	PyModule_AddObject(m, "WERR_CTX_WINSTATIONS_DISABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_WINSTATIONS_DISABLED)));
	PyModule_AddObject(m, "WERR_CTX_ENCRYPTION_LEVEL_REQUIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_ENCRYPTION_LEVEL_REQUIRED)));
	PyModule_AddObject(m, "WERR_CTX_SESSION_IN_USE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_SESSION_IN_USE)));
	PyModule_AddObject(m, "WERR_CTX_NO_FORCE_LOGOFF",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_NO_FORCE_LOGOFF)));
	PyModule_AddObject(m, "WERR_CTX_ACCOUNT_RESTRICTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_ACCOUNT_RESTRICTION)));
	PyModule_AddObject(m, "WERR_RDP_PROTOCOL_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RDP_PROTOCOL_ERROR)));
	PyModule_AddObject(m, "WERR_CTX_CDM_CONNECT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_CDM_CONNECT)));
	PyModule_AddObject(m, "WERR_CTX_CDM_DISCONNECT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_CDM_DISCONNECT)));
	PyModule_AddObject(m, "WERR_CTX_SECURITY_LAYER_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_CTX_SECURITY_LAYER_ERROR)));
	PyModule_AddObject(m, "WERR_TS_INCOMPATIBLE_SESSIONS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_TS_INCOMPATIBLE_SESSIONS)));
	PyModule_AddObject(m, "WERR_FRS_ERR_INVALID_API_SEQUENCE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FRS_ERR_INVALID_API_SEQUENCE)));
	PyModule_AddObject(m, "WERR_FRS_ERR_STARTING_SERVICE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FRS_ERR_STARTING_SERVICE)));
	PyModule_AddObject(m, "WERR_FRS_ERR_STOPPING_SERVICE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FRS_ERR_STOPPING_SERVICE)));
	PyModule_AddObject(m, "WERR_FRS_ERR_INTERNAL_API",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FRS_ERR_INTERNAL_API)));
	PyModule_AddObject(m, "WERR_FRS_ERR_INTERNAL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FRS_ERR_INTERNAL)));
	PyModule_AddObject(m, "WERR_FRS_ERR_SERVICE_COMM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FRS_ERR_SERVICE_COMM)));
	PyModule_AddObject(m, "WERR_FRS_ERR_INSUFFICIENT_PRIV",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FRS_ERR_INSUFFICIENT_PRIV)));
	PyModule_AddObject(m, "WERR_FRS_ERR_AUTHENTICATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FRS_ERR_AUTHENTICATION)));
	PyModule_AddObject(m, "WERR_FRS_ERR_PARENT_INSUFFICIENT_PRIV",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FRS_ERR_PARENT_INSUFFICIENT_PRIV)));
	PyModule_AddObject(m, "WERR_FRS_ERR_PARENT_AUTHENTICATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FRS_ERR_PARENT_AUTHENTICATION)));
	PyModule_AddObject(m, "WERR_FRS_ERR_CHILD_TO_PARENT_COMM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FRS_ERR_CHILD_TO_PARENT_COMM)));
	PyModule_AddObject(m, "WERR_FRS_ERR_PARENT_TO_CHILD_COMM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FRS_ERR_PARENT_TO_CHILD_COMM)));
	PyModule_AddObject(m, "WERR_FRS_ERR_SYSVOL_POPULATE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FRS_ERR_SYSVOL_POPULATE)));
	PyModule_AddObject(m, "WERR_FRS_ERR_SYSVOL_POPULATE_TIMEOUT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FRS_ERR_SYSVOL_POPULATE_TIMEOUT)));
	PyModule_AddObject(m, "WERR_FRS_ERR_SYSVOL_IS_BUSY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FRS_ERR_SYSVOL_IS_BUSY)));
	PyModule_AddObject(m, "WERR_FRS_ERR_SYSVOL_DEMOTE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FRS_ERR_SYSVOL_DEMOTE)));
	PyModule_AddObject(m, "WERR_FRS_ERR_INVALID_SERVICE_PARAMETER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_FRS_ERR_INVALID_SERVICE_PARAMETER)));
	PyModule_AddObject(m, "WERR_DS_NOT_INSTALLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NOT_INSTALLED)));
	PyModule_AddObject(m, "WERR_DS_MEMBERSHIP_EVALUATED_LOCALLY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_MEMBERSHIP_EVALUATED_LOCALLY)));
	PyModule_AddObject(m, "WERR_DS_NO_ATTRIBUTE_OR_VALUE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NO_ATTRIBUTE_OR_VALUE)));
	PyModule_AddObject(m, "WERR_DS_INVALID_ATTRIBUTE_SYNTAX",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_INVALID_ATTRIBUTE_SYNTAX)));
	PyModule_AddObject(m, "WERR_DS_ATTRIBUTE_TYPE_UNDEFINED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ATTRIBUTE_TYPE_UNDEFINED)));
	PyModule_AddObject(m, "WERR_DS_ATTRIBUTE_OR_VALUE_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ATTRIBUTE_OR_VALUE_EXISTS)));
	PyModule_AddObject(m, "WERR_DS_BUSY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_BUSY)));
	PyModule_AddObject(m, "WERR_DS_UNAVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_UNAVAILABLE)));
	PyModule_AddObject(m, "WERR_DS_NO_RIDS_ALLOCATED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NO_RIDS_ALLOCATED)));
	PyModule_AddObject(m, "WERR_DS_NO_MORE_RIDS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NO_MORE_RIDS)));
	PyModule_AddObject(m, "WERR_DS_INCORRECT_ROLE_OWNER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_INCORRECT_ROLE_OWNER)));
	PyModule_AddObject(m, "WERR_DS_RIDMGR_INIT_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_RIDMGR_INIT_ERROR)));
	PyModule_AddObject(m, "WERR_DS_OBJ_CLASS_VIOLATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_OBJ_CLASS_VIOLATION)));
	PyModule_AddObject(m, "WERR_DS_CANT_ON_NON_LEAF",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_ON_NON_LEAF)));
	PyModule_AddObject(m, "WERR_DS_CANT_ON_RDN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_ON_RDN)));
	PyModule_AddObject(m, "WERR_DS_CANT_MOD_OBJ_CLASS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_MOD_OBJ_CLASS)));
	PyModule_AddObject(m, "WERR_DS_CROSS_DOM_MOVE_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CROSS_DOM_MOVE_ERROR)));
	PyModule_AddObject(m, "WERR_DS_GC_NOT_AVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_GC_NOT_AVAILABLE)));
	PyModule_AddObject(m, "WERR_SHARED_POLICY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SHARED_POLICY)));
	PyModule_AddObject(m, "WERR_POLICY_OBJECT_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_POLICY_OBJECT_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_POLICY_ONLY_IN_DS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_POLICY_ONLY_IN_DS)));
	PyModule_AddObject(m, "WERR_PROMOTION_ACTIVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_PROMOTION_ACTIVE)));
	PyModule_AddObject(m, "WERR_NO_PROMOTION_ACTIVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_PROMOTION_ACTIVE)));
	PyModule_AddObject(m, "WERR_DS_OPERATIONS_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_OPERATIONS_ERROR)));
	PyModule_AddObject(m, "WERR_DS_PROTOCOL_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_PROTOCOL_ERROR)));
	PyModule_AddObject(m, "WERR_DS_TIMELIMIT_EXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_TIMELIMIT_EXCEEDED)));
	PyModule_AddObject(m, "WERR_DS_SIZELIMIT_EXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SIZELIMIT_EXCEEDED)));
	PyModule_AddObject(m, "WERR_DS_ADMIN_LIMIT_EXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ADMIN_LIMIT_EXCEEDED)));
	PyModule_AddObject(m, "WERR_DS_COMPARE_FALSE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_COMPARE_FALSE)));
	PyModule_AddObject(m, "WERR_DS_COMPARE_TRUE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_COMPARE_TRUE)));
	PyModule_AddObject(m, "WERR_DS_AUTH_METHOD_NOT_SUPPORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_AUTH_METHOD_NOT_SUPPORTED)));
	PyModule_AddObject(m, "WERR_DS_STRONG_AUTH_REQUIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_STRONG_AUTH_REQUIRED)));
	PyModule_AddObject(m, "WERR_DS_INAPPROPRIATE_AUTH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_INAPPROPRIATE_AUTH)));
	PyModule_AddObject(m, "WERR_DS_AUTH_UNKNOWN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_AUTH_UNKNOWN)));
	PyModule_AddObject(m, "WERR_DS_REFERRAL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_REFERRAL)));
	PyModule_AddObject(m, "WERR_DS_UNAVAILABLE_CRIT_EXTENSION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_UNAVAILABLE_CRIT_EXTENSION)));
	PyModule_AddObject(m, "WERR_DS_CONFIDENTIALITY_REQUIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CONFIDENTIALITY_REQUIRED)));
	PyModule_AddObject(m, "WERR_DS_INAPPROPRIATE_MATCHING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_INAPPROPRIATE_MATCHING)));
	PyModule_AddObject(m, "WERR_DS_CONSTRAINT_VIOLATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CONSTRAINT_VIOLATION)));
	PyModule_AddObject(m, "WERR_DS_NO_SUCH_OBJECT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NO_SUCH_OBJECT)));
	PyModule_AddObject(m, "WERR_DS_ALIAS_PROBLEM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ALIAS_PROBLEM)));
	PyModule_AddObject(m, "WERR_DS_INVALID_DN_SYNTAX",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_INVALID_DN_SYNTAX)));
	PyModule_AddObject(m, "WERR_DS_IS_LEAF",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_IS_LEAF)));
	PyModule_AddObject(m, "WERR_DS_ALIAS_DEREF_PROBLEM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ALIAS_DEREF_PROBLEM)));
	PyModule_AddObject(m, "WERR_DS_UNWILLING_TO_PERFORM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_UNWILLING_TO_PERFORM)));
	PyModule_AddObject(m, "WERR_DS_LOOP_DETECT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_LOOP_DETECT)));
	PyModule_AddObject(m, "WERR_DS_NAMING_VIOLATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NAMING_VIOLATION)));
	PyModule_AddObject(m, "WERR_DS_OBJECT_RESULTS_TOO_LARGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_OBJECT_RESULTS_TOO_LARGE)));
	PyModule_AddObject(m, "WERR_DS_AFFECTS_MULTIPLE_DSAS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_AFFECTS_MULTIPLE_DSAS)));
	PyModule_AddObject(m, "WERR_DS_SERVER_DOWN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SERVER_DOWN)));
	PyModule_AddObject(m, "WERR_DS_LOCAL_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_LOCAL_ERROR)));
	PyModule_AddObject(m, "WERR_DS_ENCODING_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ENCODING_ERROR)));
	PyModule_AddObject(m, "WERR_DS_DECODING_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DECODING_ERROR)));
	PyModule_AddObject(m, "WERR_DS_FILTER_UNKNOWN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_FILTER_UNKNOWN)));
	PyModule_AddObject(m, "WERR_DS_PARAM_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_PARAM_ERROR)));
	PyModule_AddObject(m, "WERR_DS_NOT_SUPPORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NOT_SUPPORTED)));
	PyModule_AddObject(m, "WERR_DS_NO_RESULTS_RETURNED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NO_RESULTS_RETURNED)));
	PyModule_AddObject(m, "WERR_DS_CONTROL_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CONTROL_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_DS_CLIENT_LOOP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CLIENT_LOOP)));
	PyModule_AddObject(m, "WERR_DS_REFERRAL_LIMIT_EXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_REFERRAL_LIMIT_EXCEEDED)));
	PyModule_AddObject(m, "WERR_DS_SORT_CONTROL_MISSING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SORT_CONTROL_MISSING)));
	PyModule_AddObject(m, "WERR_DS_OFFSET_RANGE_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_OFFSET_RANGE_ERROR)));
	PyModule_AddObject(m, "WERR_DS_ROOT_MUST_BE_NC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ROOT_MUST_BE_NC)));
	PyModule_AddObject(m, "WERR_DS_ADD_REPLICA_INHIBITED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ADD_REPLICA_INHIBITED)));
	PyModule_AddObject(m, "WERR_DS_ATT_NOT_DEF_IN_SCHEMA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ATT_NOT_DEF_IN_SCHEMA)));
	PyModule_AddObject(m, "WERR_DS_MAX_OBJ_SIZE_EXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_MAX_OBJ_SIZE_EXCEEDED)));
	PyModule_AddObject(m, "WERR_DS_OBJ_STRING_NAME_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_OBJ_STRING_NAME_EXISTS)));
	PyModule_AddObject(m, "WERR_DS_NO_RDN_DEFINED_IN_SCHEMA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NO_RDN_DEFINED_IN_SCHEMA)));
	PyModule_AddObject(m, "WERR_DS_RDN_DOESNT_MATCH_SCHEMA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_RDN_DOESNT_MATCH_SCHEMA)));
	PyModule_AddObject(m, "WERR_DS_NO_REQUESTED_ATTS_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NO_REQUESTED_ATTS_FOUND)));
	PyModule_AddObject(m, "WERR_DS_USER_BUFFER_TO_SMALL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_USER_BUFFER_TO_SMALL)));
	PyModule_AddObject(m, "WERR_DS_ATT_IS_NOT_ON_OBJ",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ATT_IS_NOT_ON_OBJ)));
	PyModule_AddObject(m, "WERR_DS_ILLEGAL_MOD_OPERATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ILLEGAL_MOD_OPERATION)));
	PyModule_AddObject(m, "WERR_DS_OBJ_TOO_LARGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_OBJ_TOO_LARGE)));
	PyModule_AddObject(m, "WERR_DS_BAD_INSTANCE_TYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_BAD_INSTANCE_TYPE)));
	PyModule_AddObject(m, "WERR_DS_MASTERDSA_REQUIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_MASTERDSA_REQUIRED)));
	PyModule_AddObject(m, "WERR_DS_OBJECT_CLASS_REQUIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_OBJECT_CLASS_REQUIRED)));
	PyModule_AddObject(m, "WERR_DS_MISSING_REQUIRED_ATT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_MISSING_REQUIRED_ATT)));
	PyModule_AddObject(m, "WERR_DS_ATT_NOT_DEF_FOR_CLASS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ATT_NOT_DEF_FOR_CLASS)));
	PyModule_AddObject(m, "WERR_DS_ATT_ALREADY_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ATT_ALREADY_EXISTS)));
	PyModule_AddObject(m, "WERR_DS_CANT_ADD_ATT_VALUES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_ADD_ATT_VALUES)));
	PyModule_AddObject(m, "WERR_DS_SINGLE_VALUE_CONSTRAINT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SINGLE_VALUE_CONSTRAINT)));
	PyModule_AddObject(m, "WERR_DS_RANGE_CONSTRAINT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_RANGE_CONSTRAINT)));
	PyModule_AddObject(m, "WERR_DS_ATT_VAL_ALREADY_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ATT_VAL_ALREADY_EXISTS)));
	PyModule_AddObject(m, "WERR_DS_CANT_REM_MISSING_ATT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_REM_MISSING_ATT)));
	PyModule_AddObject(m, "WERR_DS_CANT_REM_MISSING_ATT_VAL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_REM_MISSING_ATT_VAL)));
	PyModule_AddObject(m, "WERR_DS_ROOT_CANT_BE_SUBREF",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ROOT_CANT_BE_SUBREF)));
	PyModule_AddObject(m, "WERR_DS_NO_CHAINING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NO_CHAINING)));
	PyModule_AddObject(m, "WERR_DS_NO_CHAINED_EVAL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NO_CHAINED_EVAL)));
	PyModule_AddObject(m, "WERR_DS_NO_PARENT_OBJECT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NO_PARENT_OBJECT)));
	PyModule_AddObject(m, "WERR_DS_PARENT_IS_AN_ALIAS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_PARENT_IS_AN_ALIAS)));
	PyModule_AddObject(m, "WERR_DS_CANT_MIX_MASTER_AND_REPS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_MIX_MASTER_AND_REPS)));
	PyModule_AddObject(m, "WERR_DS_CHILDREN_EXIST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CHILDREN_EXIST)));
	PyModule_AddObject(m, "WERR_DS_OBJ_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_OBJ_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_DS_ALIASED_OBJ_MISSING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ALIASED_OBJ_MISSING)));
	PyModule_AddObject(m, "WERR_DS_BAD_NAME_SYNTAX",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_BAD_NAME_SYNTAX)));
	PyModule_AddObject(m, "WERR_DS_ALIAS_POINTS_TO_ALIAS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ALIAS_POINTS_TO_ALIAS)));
	PyModule_AddObject(m, "WERR_DS_CANT_DEREF_ALIAS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_DEREF_ALIAS)));
	PyModule_AddObject(m, "WERR_DS_OUT_OF_SCOPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_OUT_OF_SCOPE)));
	PyModule_AddObject(m, "WERR_DS_OBJECT_BEING_REMOVED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_OBJECT_BEING_REMOVED)));
	PyModule_AddObject(m, "WERR_DS_CANT_DELETE_DSA_OBJ",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_DELETE_DSA_OBJ)));
	PyModule_AddObject(m, "WERR_DS_GENERIC_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_GENERIC_ERROR)));
	PyModule_AddObject(m, "WERR_DS_DSA_MUST_BE_INT_MASTER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DSA_MUST_BE_INT_MASTER)));
	PyModule_AddObject(m, "WERR_DS_CLASS_NOT_DSA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CLASS_NOT_DSA)));
	PyModule_AddObject(m, "WERR_DS_INSUFF_ACCESS_RIGHTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_INSUFF_ACCESS_RIGHTS)));
	PyModule_AddObject(m, "WERR_DS_ILLEGAL_SUPERIOR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ILLEGAL_SUPERIOR)));
	PyModule_AddObject(m, "WERR_DS_ATTRIBUTE_OWNED_BY_SAM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ATTRIBUTE_OWNED_BY_SAM)));
	PyModule_AddObject(m, "WERR_DS_NAME_TOO_MANY_PARTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NAME_TOO_MANY_PARTS)));
	PyModule_AddObject(m, "WERR_DS_NAME_TOO_LONG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NAME_TOO_LONG)));
	PyModule_AddObject(m, "WERR_DS_NAME_VALUE_TOO_LONG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NAME_VALUE_TOO_LONG)));
	PyModule_AddObject(m, "WERR_DS_NAME_UNPARSEABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NAME_UNPARSEABLE)));
	PyModule_AddObject(m, "WERR_DS_NAME_TYPE_UNKNOWN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NAME_TYPE_UNKNOWN)));
	PyModule_AddObject(m, "WERR_DS_NOT_AN_OBJECT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NOT_AN_OBJECT)));
	PyModule_AddObject(m, "WERR_DS_SEC_DESC_TOO_SHORT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SEC_DESC_TOO_SHORT)));
	PyModule_AddObject(m, "WERR_DS_SEC_DESC_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SEC_DESC_INVALID)));
	PyModule_AddObject(m, "WERR_DS_NO_DELETED_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NO_DELETED_NAME)));
	PyModule_AddObject(m, "WERR_DS_SUBREF_MUST_HAVE_PARENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SUBREF_MUST_HAVE_PARENT)));
	PyModule_AddObject(m, "WERR_DS_NCNAME_MUST_BE_NC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NCNAME_MUST_BE_NC)));
	PyModule_AddObject(m, "WERR_DS_CANT_ADD_SYSTEM_ONLY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_ADD_SYSTEM_ONLY)));
	PyModule_AddObject(m, "WERR_DS_CLASS_MUST_BE_CONCRETE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CLASS_MUST_BE_CONCRETE)));
	PyModule_AddObject(m, "WERR_DS_INVALID_DMD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_INVALID_DMD)));
	PyModule_AddObject(m, "WERR_DS_OBJ_GUID_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_OBJ_GUID_EXISTS)));
	PyModule_AddObject(m, "WERR_DS_NOT_ON_BACKLINK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NOT_ON_BACKLINK)));
	PyModule_AddObject(m, "WERR_DS_NO_CROSSREF_FOR_NC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NO_CROSSREF_FOR_NC)));
	PyModule_AddObject(m, "WERR_DS_SHUTTING_DOWN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SHUTTING_DOWN)));
	PyModule_AddObject(m, "WERR_DS_UNKNOWN_OPERATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_UNKNOWN_OPERATION)));
	PyModule_AddObject(m, "WERR_DS_INVALID_ROLE_OWNER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_INVALID_ROLE_OWNER)));
	PyModule_AddObject(m, "WERR_DS_COULDNT_CONTACT_FSMO",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_COULDNT_CONTACT_FSMO)));
	PyModule_AddObject(m, "WERR_DS_CROSS_NC_DN_RENAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CROSS_NC_DN_RENAME)));
	PyModule_AddObject(m, "WERR_DS_CANT_MOD_SYSTEM_ONLY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_MOD_SYSTEM_ONLY)));
	PyModule_AddObject(m, "WERR_DS_REPLICATOR_ONLY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_REPLICATOR_ONLY)));
	PyModule_AddObject(m, "WERR_DS_OBJ_CLASS_NOT_DEFINED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_OBJ_CLASS_NOT_DEFINED)));
	PyModule_AddObject(m, "WERR_DS_OBJ_CLASS_NOT_SUBCLASS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_OBJ_CLASS_NOT_SUBCLASS)));
	PyModule_AddObject(m, "WERR_DS_NAME_REFERENCE_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NAME_REFERENCE_INVALID)));
	PyModule_AddObject(m, "WERR_DS_CROSS_REF_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CROSS_REF_EXISTS)));
	PyModule_AddObject(m, "WERR_DS_CANT_DEL_MASTER_CROSSREF",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_DEL_MASTER_CROSSREF)));
	PyModule_AddObject(m, "WERR_DS_SUBTREE_NOTIFY_NOT_NC_HEAD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SUBTREE_NOTIFY_NOT_NC_HEAD)));
	PyModule_AddObject(m, "WERR_DS_NOTIFY_FILTER_TOO_COMPLEX",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NOTIFY_FILTER_TOO_COMPLEX)));
	PyModule_AddObject(m, "WERR_DS_DUP_RDN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DUP_RDN)));
	PyModule_AddObject(m, "WERR_DS_DUP_OID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DUP_OID)));
	PyModule_AddObject(m, "WERR_DS_DUP_MAPI_ID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DUP_MAPI_ID)));
	PyModule_AddObject(m, "WERR_DS_DUP_SCHEMA_ID_GUID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DUP_SCHEMA_ID_GUID)));
	PyModule_AddObject(m, "WERR_DS_DUP_LDAP_DISPLAY_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DUP_LDAP_DISPLAY_NAME)));
	PyModule_AddObject(m, "WERR_DS_SEMANTIC_ATT_TEST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SEMANTIC_ATT_TEST)));
	PyModule_AddObject(m, "WERR_DS_SYNTAX_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SYNTAX_MISMATCH)));
	PyModule_AddObject(m, "WERR_DS_EXISTS_IN_MUST_HAVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_EXISTS_IN_MUST_HAVE)));
	PyModule_AddObject(m, "WERR_DS_EXISTS_IN_MAY_HAVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_EXISTS_IN_MAY_HAVE)));
	PyModule_AddObject(m, "WERR_DS_NONEXISTENT_MAY_HAVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NONEXISTENT_MAY_HAVE)));
	PyModule_AddObject(m, "WERR_DS_NONEXISTENT_MUST_HAVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NONEXISTENT_MUST_HAVE)));
	PyModule_AddObject(m, "WERR_DS_AUX_CLS_TEST_FAIL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_AUX_CLS_TEST_FAIL)));
	PyModule_AddObject(m, "WERR_DS_NONEXISTENT_POSS_SUP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NONEXISTENT_POSS_SUP)));
	PyModule_AddObject(m, "WERR_DS_SUB_CLS_TEST_FAIL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SUB_CLS_TEST_FAIL)));
	PyModule_AddObject(m, "WERR_DS_BAD_RDN_ATT_ID_SYNTAX",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_BAD_RDN_ATT_ID_SYNTAX)));
	PyModule_AddObject(m, "WERR_DS_EXISTS_IN_AUX_CLS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_EXISTS_IN_AUX_CLS)));
	PyModule_AddObject(m, "WERR_DS_EXISTS_IN_SUB_CLS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_EXISTS_IN_SUB_CLS)));
	PyModule_AddObject(m, "WERR_DS_EXISTS_IN_POSS_SUP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_EXISTS_IN_POSS_SUP)));
	PyModule_AddObject(m, "WERR_DS_RECALCSCHEMA_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_RECALCSCHEMA_FAILED)));
	PyModule_AddObject(m, "WERR_DS_TREE_DELETE_NOT_FINISHED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_TREE_DELETE_NOT_FINISHED)));
	PyModule_AddObject(m, "WERR_DS_CANT_DELETE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_DELETE)));
	PyModule_AddObject(m, "WERR_DS_ATT_SCHEMA_REQ_ID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ATT_SCHEMA_REQ_ID)));
	PyModule_AddObject(m, "WERR_DS_BAD_ATT_SCHEMA_SYNTAX",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_BAD_ATT_SCHEMA_SYNTAX)));
	PyModule_AddObject(m, "WERR_DS_CANT_CACHE_ATT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_CACHE_ATT)));
	PyModule_AddObject(m, "WERR_DS_CANT_CACHE_CLASS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_CACHE_CLASS)));
	PyModule_AddObject(m, "WERR_DS_CANT_REMOVE_ATT_CACHE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_REMOVE_ATT_CACHE)));
	PyModule_AddObject(m, "WERR_DS_CANT_REMOVE_CLASS_CACHE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_REMOVE_CLASS_CACHE)));
	PyModule_AddObject(m, "WERR_DS_CANT_RETRIEVE_DN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_RETRIEVE_DN)));
	PyModule_AddObject(m, "WERR_DS_MISSING_SUPREF",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_MISSING_SUPREF)));
	PyModule_AddObject(m, "WERR_DS_CANT_RETRIEVE_INSTANCE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_RETRIEVE_INSTANCE)));
	PyModule_AddObject(m, "WERR_DS_CODE_INCONSISTENCY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CODE_INCONSISTENCY)));
	PyModule_AddObject(m, "WERR_DS_DATABASE_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DATABASE_ERROR)));
	PyModule_AddObject(m, "WERR_DS_GOVERNSID_MISSING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_GOVERNSID_MISSING)));
	PyModule_AddObject(m, "WERR_DS_MISSING_EXPECTED_ATT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_MISSING_EXPECTED_ATT)));
	PyModule_AddObject(m, "WERR_DS_NCNAME_MISSING_CR_REF",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NCNAME_MISSING_CR_REF)));
	PyModule_AddObject(m, "WERR_DS_SECURITY_CHECKING_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SECURITY_CHECKING_ERROR)));
	PyModule_AddObject(m, "WERR_DS_SCHEMA_NOT_LOADED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SCHEMA_NOT_LOADED)));
	PyModule_AddObject(m, "WERR_DS_SCHEMA_ALLOC_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SCHEMA_ALLOC_FAILED)));
	PyModule_AddObject(m, "WERR_DS_ATT_SCHEMA_REQ_SYNTAX",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ATT_SCHEMA_REQ_SYNTAX)));
	PyModule_AddObject(m, "WERR_DS_GCVERIFY_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_GCVERIFY_ERROR)));
	PyModule_AddObject(m, "WERR_DS_DRA_SCHEMA_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_SCHEMA_MISMATCH)));
	PyModule_AddObject(m, "WERR_DS_CANT_FIND_DSA_OBJ",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_FIND_DSA_OBJ)));
	PyModule_AddObject(m, "WERR_DS_CANT_FIND_EXPECTED_NC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_FIND_EXPECTED_NC)));
	PyModule_AddObject(m, "WERR_DS_CANT_FIND_NC_IN_CACHE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_FIND_NC_IN_CACHE)));
	PyModule_AddObject(m, "WERR_DS_CANT_RETRIEVE_CHILD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_RETRIEVE_CHILD)));
	PyModule_AddObject(m, "WERR_DS_SECURITY_ILLEGAL_MODIFY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SECURITY_ILLEGAL_MODIFY)));
	PyModule_AddObject(m, "WERR_DS_CANT_REPLACE_HIDDEN_REC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_REPLACE_HIDDEN_REC)));
	PyModule_AddObject(m, "WERR_DS_BAD_HIERARCHY_FILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_BAD_HIERARCHY_FILE)));
	PyModule_AddObject(m, "WERR_DS_BUILD_HIERARCHY_TABLE_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_BUILD_HIERARCHY_TABLE_FAILED)));
	PyModule_AddObject(m, "WERR_DS_CONFIG_PARAM_MISSING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CONFIG_PARAM_MISSING)));
	PyModule_AddObject(m, "WERR_DS_COUNTING_AB_INDICES_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_COUNTING_AB_INDICES_FAILED)));
	PyModule_AddObject(m, "WERR_DS_HIERARCHY_TABLE_MALLOC_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_HIERARCHY_TABLE_MALLOC_FAILED)));
	PyModule_AddObject(m, "WERR_DS_INTERNAL_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_INTERNAL_FAILURE)));
	PyModule_AddObject(m, "WERR_DS_UNKNOWN_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_UNKNOWN_ERROR)));
	PyModule_AddObject(m, "WERR_DS_ROOT_REQUIRES_CLASS_TOP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ROOT_REQUIRES_CLASS_TOP)));
	PyModule_AddObject(m, "WERR_DS_REFUSING_FSMO_ROLES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_REFUSING_FSMO_ROLES)));
	PyModule_AddObject(m, "WERR_DS_MISSING_FSMO_SETTINGS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_MISSING_FSMO_SETTINGS)));
	PyModule_AddObject(m, "WERR_DS_UNABLE_TO_SURRENDER_ROLES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_UNABLE_TO_SURRENDER_ROLES)));
	PyModule_AddObject(m, "WERR_DS_DRA_GENERIC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_GENERIC)));
	PyModule_AddObject(m, "WERR_DS_DRA_INVALID_PARAMETER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_INVALID_PARAMETER)));
	PyModule_AddObject(m, "WERR_DS_DRA_BUSY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_BUSY)));
	PyModule_AddObject(m, "WERR_DS_DRA_BAD_DN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_BAD_DN)));
	PyModule_AddObject(m, "WERR_DS_DRA_BAD_NC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_BAD_NC)));
	PyModule_AddObject(m, "WERR_DS_DRA_DN_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_DN_EXISTS)));
	PyModule_AddObject(m, "WERR_DS_DRA_INTERNAL_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_INTERNAL_ERROR)));
	PyModule_AddObject(m, "WERR_DS_DRA_INCONSISTENT_DIT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_INCONSISTENT_DIT)));
	PyModule_AddObject(m, "WERR_DS_DRA_CONNECTION_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_CONNECTION_FAILED)));
	PyModule_AddObject(m, "WERR_DS_DRA_BAD_INSTANCE_TYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_BAD_INSTANCE_TYPE)));
	PyModule_AddObject(m, "WERR_DS_DRA_OUT_OF_MEM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_OUT_OF_MEM)));
	PyModule_AddObject(m, "WERR_DS_DRA_MAIL_PROBLEM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_MAIL_PROBLEM)));
	PyModule_AddObject(m, "WERR_DS_DRA_REF_ALREADY_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_REF_ALREADY_EXISTS)));
	PyModule_AddObject(m, "WERR_DS_DRA_REF_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_REF_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_DS_DRA_OBJ_IS_REP_SOURCE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_OBJ_IS_REP_SOURCE)));
	PyModule_AddObject(m, "WERR_DS_DRA_DB_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_DB_ERROR)));
	PyModule_AddObject(m, "WERR_DS_DRA_NO_REPLICA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_NO_REPLICA)));
	PyModule_AddObject(m, "WERR_DS_DRA_ACCESS_DENIED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_ACCESS_DENIED)));
	PyModule_AddObject(m, "WERR_DS_DRA_NOT_SUPPORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_NOT_SUPPORTED)));
	PyModule_AddObject(m, "WERR_DS_DRA_RPC_CANCELLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_RPC_CANCELLED)));
	PyModule_AddObject(m, "WERR_DS_DRA_SOURCE_DISABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_SOURCE_DISABLED)));
	PyModule_AddObject(m, "WERR_DS_DRA_SINK_DISABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_SINK_DISABLED)));
	PyModule_AddObject(m, "WERR_DS_DRA_NAME_COLLISION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_NAME_COLLISION)));
	PyModule_AddObject(m, "WERR_DS_DRA_SOURCE_REINSTALLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_SOURCE_REINSTALLED)));
	PyModule_AddObject(m, "WERR_DS_DRA_MISSING_PARENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_MISSING_PARENT)));
	PyModule_AddObject(m, "WERR_DS_DRA_PREEMPTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_PREEMPTED)));
	PyModule_AddObject(m, "WERR_DS_DRA_ABANDON_SYNC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_ABANDON_SYNC)));
	PyModule_AddObject(m, "WERR_DS_DRA_SHUTDOWN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_SHUTDOWN)));
	PyModule_AddObject(m, "WERR_DS_DRA_INCOMPATIBLE_PARTIAL_SET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_INCOMPATIBLE_PARTIAL_SET)));
	PyModule_AddObject(m, "WERR_DS_DRA_SOURCE_IS_PARTIAL_REPLICA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_SOURCE_IS_PARTIAL_REPLICA)));
	PyModule_AddObject(m, "WERR_DS_DRA_EXTN_CONNECTION_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_EXTN_CONNECTION_FAILED)));
	PyModule_AddObject(m, "WERR_DS_INSTALL_SCHEMA_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_INSTALL_SCHEMA_MISMATCH)));
	PyModule_AddObject(m, "WERR_DS_DUP_LINK_ID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DUP_LINK_ID)));
	PyModule_AddObject(m, "WERR_DS_NAME_ERROR_RESOLVING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NAME_ERROR_RESOLVING)));
	PyModule_AddObject(m, "WERR_DS_NAME_ERROR_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NAME_ERROR_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_DS_NAME_ERROR_NOT_UNIQUE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NAME_ERROR_NOT_UNIQUE)));
	PyModule_AddObject(m, "WERR_DS_NAME_ERROR_NO_MAPPING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NAME_ERROR_NO_MAPPING)));
	PyModule_AddObject(m, "WERR_DS_NAME_ERROR_DOMAIN_ONLY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NAME_ERROR_DOMAIN_ONLY)));
	PyModule_AddObject(m, "WERR_DS_NAME_ERROR_NO_SYNTACTICAL_MAPPING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NAME_ERROR_NO_SYNTACTICAL_MAPPING)));
	PyModule_AddObject(m, "WERR_DS_CONSTRUCTED_ATT_MOD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CONSTRUCTED_ATT_MOD)));
	PyModule_AddObject(m, "WERR_DS_WRONG_OM_OBJ_CLASS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_WRONG_OM_OBJ_CLASS)));
	PyModule_AddObject(m, "WERR_DS_DRA_REPL_PENDING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_REPL_PENDING)));
	PyModule_AddObject(m, "WERR_DS_DS_REQUIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DS_REQUIRED)));
	PyModule_AddObject(m, "WERR_DS_INVALID_LDAP_DISPLAY_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_INVALID_LDAP_DISPLAY_NAME)));
	PyModule_AddObject(m, "WERR_DS_NON_BASE_SEARCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NON_BASE_SEARCH)));
	PyModule_AddObject(m, "WERR_DS_CANT_RETRIEVE_ATTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_RETRIEVE_ATTS)));
	PyModule_AddObject(m, "WERR_DS_BACKLINK_WITHOUT_LINK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_BACKLINK_WITHOUT_LINK)));
	PyModule_AddObject(m, "WERR_DS_EPOCH_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_EPOCH_MISMATCH)));
	PyModule_AddObject(m, "WERR_DS_SRC_NAME_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SRC_NAME_MISMATCH)));
	PyModule_AddObject(m, "WERR_DS_SRC_AND_DST_NC_IDENTICAL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SRC_AND_DST_NC_IDENTICAL)));
	PyModule_AddObject(m, "WERR_DS_DST_NC_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DST_NC_MISMATCH)));
	PyModule_AddObject(m, "WERR_DS_NOT_AUTHORITIVE_FOR_DST_NC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NOT_AUTHORITIVE_FOR_DST_NC)));
	PyModule_AddObject(m, "WERR_DS_SRC_GUID_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SRC_GUID_MISMATCH)));
	PyModule_AddObject(m, "WERR_DS_CANT_MOVE_DELETED_OBJECT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_MOVE_DELETED_OBJECT)));
	PyModule_AddObject(m, "WERR_DS_PDC_OPERATION_IN_PROGRESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_PDC_OPERATION_IN_PROGRESS)));
	PyModule_AddObject(m, "WERR_DS_CROSS_DOMAIN_CLEANUP_REQD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CROSS_DOMAIN_CLEANUP_REQD)));
	PyModule_AddObject(m, "WERR_DS_ILLEGAL_XDOM_MOVE_OPERATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ILLEGAL_XDOM_MOVE_OPERATION)));
	PyModule_AddObject(m, "WERR_DS_CANT_WITH_ACCT_GROUP_MEMBERSHPS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_WITH_ACCT_GROUP_MEMBERSHPS)));
	PyModule_AddObject(m, "WERR_DS_NC_MUST_HAVE_NC_PARENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NC_MUST_HAVE_NC_PARENT)));
	PyModule_AddObject(m, "WERR_DS_CR_IMPOSSIBLE_TO_VALIDATE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CR_IMPOSSIBLE_TO_VALIDATE)));
	PyModule_AddObject(m, "WERR_DS_DST_DOMAIN_NOT_NATIVE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DST_DOMAIN_NOT_NATIVE)));
	PyModule_AddObject(m, "WERR_DS_MISSING_INFRASTRUCTURE_CONTAINER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_MISSING_INFRASTRUCTURE_CONTAINER)));
	PyModule_AddObject(m, "WERR_DS_CANT_MOVE_ACCOUNT_GROUP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_MOVE_ACCOUNT_GROUP)));
	PyModule_AddObject(m, "WERR_DS_CANT_MOVE_RESOURCE_GROUP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_MOVE_RESOURCE_GROUP)));
	PyModule_AddObject(m, "WERR_DS_INVALID_SEARCH_FLAG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_INVALID_SEARCH_FLAG)));
	PyModule_AddObject(m, "WERR_DS_NO_TREE_DELETE_ABOVE_NC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NO_TREE_DELETE_ABOVE_NC)));
	PyModule_AddObject(m, "WERR_DS_COULDNT_LOCK_TREE_FOR_DELETE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_COULDNT_LOCK_TREE_FOR_DELETE)));
	PyModule_AddObject(m, "WERR_DS_COULDNT_IDENTIFY_OBJECTS_FOR_TREE_DELETE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_COULDNT_IDENTIFY_OBJECTS_FOR_TREE_DELETE)));
	PyModule_AddObject(m, "WERR_DS_SAM_INIT_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SAM_INIT_FAILURE)));
	PyModule_AddObject(m, "WERR_DS_SENSITIVE_GROUP_VIOLATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SENSITIVE_GROUP_VIOLATION)));
	PyModule_AddObject(m, "WERR_DS_CANT_MOD_PRIMARYGROUPID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_MOD_PRIMARYGROUPID)));
	PyModule_AddObject(m, "WERR_DS_ILLEGAL_BASE_SCHEMA_MOD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ILLEGAL_BASE_SCHEMA_MOD)));
	PyModule_AddObject(m, "WERR_DS_NONSAFE_SCHEMA_CHANGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NONSAFE_SCHEMA_CHANGE)));
	PyModule_AddObject(m, "WERR_DS_SCHEMA_UPDATE_DISALLOWED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SCHEMA_UPDATE_DISALLOWED)));
	PyModule_AddObject(m, "WERR_DS_CANT_CREATE_UNDER_SCHEMA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_CREATE_UNDER_SCHEMA)));
	PyModule_AddObject(m, "WERR_DS_INSTALL_NO_SRC_SCH_VERSION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_INSTALL_NO_SRC_SCH_VERSION)));
	PyModule_AddObject(m, "WERR_DS_INSTALL_NO_SCH_VERSION_IN_INIFILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_INSTALL_NO_SCH_VERSION_IN_INIFILE)));
	PyModule_AddObject(m, "WERR_DS_INVALID_GROUP_TYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_INVALID_GROUP_TYPE)));
	PyModule_AddObject(m, "WERR_DS_NO_NEST_GLOBALGROUP_IN_MIXEDDOMAIN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NO_NEST_GLOBALGROUP_IN_MIXEDDOMAIN)));
	PyModule_AddObject(m, "WERR_DS_NO_NEST_LOCALGROUP_IN_MIXEDDOMAIN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NO_NEST_LOCALGROUP_IN_MIXEDDOMAIN)));
	PyModule_AddObject(m, "WERR_DS_GLOBAL_CANT_HAVE_LOCAL_MEMBER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_GLOBAL_CANT_HAVE_LOCAL_MEMBER)));
	PyModule_AddObject(m, "WERR_DS_GLOBAL_CANT_HAVE_UNIVERSAL_MEMBER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_GLOBAL_CANT_HAVE_UNIVERSAL_MEMBER)));
	PyModule_AddObject(m, "WERR_DS_UNIVERSAL_CANT_HAVE_LOCAL_MEMBER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_UNIVERSAL_CANT_HAVE_LOCAL_MEMBER)));
	PyModule_AddObject(m, "WERR_DS_GLOBAL_CANT_HAVE_CROSSDOMAIN_MEMBER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_GLOBAL_CANT_HAVE_CROSSDOMAIN_MEMBER)));
	PyModule_AddObject(m, "WERR_DS_LOCAL_CANT_HAVE_CROSSDOMAIN_LOCAL_MEMBER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_LOCAL_CANT_HAVE_CROSSDOMAIN_LOCAL_MEMBER)));
	PyModule_AddObject(m, "WERR_DS_HAVE_PRIMARY_MEMBERS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_HAVE_PRIMARY_MEMBERS)));
	PyModule_AddObject(m, "WERR_DS_STRING_SD_CONVERSION_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_STRING_SD_CONVERSION_FAILED)));
	PyModule_AddObject(m, "WERR_DS_NAMING_MASTER_GC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NAMING_MASTER_GC)));
	PyModule_AddObject(m, "WERR_DS_DNS_LOOKUP_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DNS_LOOKUP_FAILURE)));
	PyModule_AddObject(m, "WERR_DS_COULDNT_UPDATE_SPNS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_COULDNT_UPDATE_SPNS)));
	PyModule_AddObject(m, "WERR_DS_CANT_RETRIEVE_SD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_RETRIEVE_SD)));
	PyModule_AddObject(m, "WERR_DS_KEY_NOT_UNIQUE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_KEY_NOT_UNIQUE)));
	PyModule_AddObject(m, "WERR_DS_WRONG_LINKED_ATT_SYNTAX",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_WRONG_LINKED_ATT_SYNTAX)));
	PyModule_AddObject(m, "WERR_DS_SAM_NEED_BOOTKEY_PASSWORD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SAM_NEED_BOOTKEY_PASSWORD)));
	PyModule_AddObject(m, "WERR_DS_SAM_NEED_BOOTKEY_FLOPPY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SAM_NEED_BOOTKEY_FLOPPY)));
	PyModule_AddObject(m, "WERR_DS_CANT_START",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_START)));
	PyModule_AddObject(m, "WERR_DS_INIT_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_INIT_FAILURE)));
	PyModule_AddObject(m, "WERR_DS_NO_PKT_PRIVACY_ON_CONNECTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NO_PKT_PRIVACY_ON_CONNECTION)));
	PyModule_AddObject(m, "WERR_DS_SOURCE_DOMAIN_IN_FOREST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SOURCE_DOMAIN_IN_FOREST)));
	PyModule_AddObject(m, "WERR_DS_DESTINATION_DOMAIN_NOT_IN_FOREST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DESTINATION_DOMAIN_NOT_IN_FOREST)));
	PyModule_AddObject(m, "WERR_DS_DESTINATION_AUDITING_NOT_ENABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DESTINATION_AUDITING_NOT_ENABLED)));
	PyModule_AddObject(m, "WERR_DS_CANT_FIND_DC_FOR_SRC_DOMAIN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_FIND_DC_FOR_SRC_DOMAIN)));
	PyModule_AddObject(m, "WERR_DS_SRC_OBJ_NOT_GROUP_OR_USER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SRC_OBJ_NOT_GROUP_OR_USER)));
	PyModule_AddObject(m, "WERR_DS_SRC_SID_EXISTS_IN_FOREST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SRC_SID_EXISTS_IN_FOREST)));
	PyModule_AddObject(m, "WERR_DS_SRC_AND_DST_OBJECT_CLASS_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SRC_AND_DST_OBJECT_CLASS_MISMATCH)));
	PyModule_AddObject(m, "WERR_SAM_INIT_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SAM_INIT_FAILURE)));
	PyModule_AddObject(m, "WERR_DS_DRA_SCHEMA_INFO_SHIP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_SCHEMA_INFO_SHIP)));
	PyModule_AddObject(m, "WERR_DS_DRA_SCHEMA_CONFLICT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_SCHEMA_CONFLICT)));
	PyModule_AddObject(m, "WERR_DS_DRA_EARLIER_SCHEMA_CONFLICT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_EARLIER_SCHEMA_CONFLICT)));
	PyModule_AddObject(m, "WERR_DS_DRA_OBJ_NC_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_OBJ_NC_MISMATCH)));
	PyModule_AddObject(m, "WERR_DS_NC_STILL_HAS_DSAS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NC_STILL_HAS_DSAS)));
	PyModule_AddObject(m, "WERR_DS_GC_REQUIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_GC_REQUIRED)));
	PyModule_AddObject(m, "WERR_DS_LOCAL_MEMBER_OF_LOCAL_ONLY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_LOCAL_MEMBER_OF_LOCAL_ONLY)));
	PyModule_AddObject(m, "WERR_DS_NO_FPO_IN_UNIVERSAL_GROUPS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NO_FPO_IN_UNIVERSAL_GROUPS)));
	PyModule_AddObject(m, "WERR_DS_CANT_ADD_TO_GC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_ADD_TO_GC)));
	PyModule_AddObject(m, "WERR_DS_NO_CHECKPOINT_WITH_PDC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NO_CHECKPOINT_WITH_PDC)));
	PyModule_AddObject(m, "WERR_DS_SOURCE_AUDITING_NOT_ENABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SOURCE_AUDITING_NOT_ENABLED)));
	PyModule_AddObject(m, "WERR_DS_CANT_CREATE_IN_NONDOMAIN_NC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_CREATE_IN_NONDOMAIN_NC)));
	PyModule_AddObject(m, "WERR_DS_INVALID_NAME_FOR_SPN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_INVALID_NAME_FOR_SPN)));
	PyModule_AddObject(m, "WERR_DS_FILTER_USES_CONTRUCTED_ATTRS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_FILTER_USES_CONTRUCTED_ATTRS)));
	PyModule_AddObject(m, "WERR_DS_UNICODEPWD_NOT_IN_QUOTES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_UNICODEPWD_NOT_IN_QUOTES)));
	PyModule_AddObject(m, "WERR_DS_MACHINE_ACCOUNT_QUOTA_EXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_MACHINE_ACCOUNT_QUOTA_EXCEEDED)));
	PyModule_AddObject(m, "WERR_DS_MUST_BE_RUN_ON_DST_DC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_MUST_BE_RUN_ON_DST_DC)));
	PyModule_AddObject(m, "WERR_DS_SRC_DC_MUST_BE_SP4_OR_GREATER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SRC_DC_MUST_BE_SP4_OR_GREATER)));
	PyModule_AddObject(m, "WERR_DS_CANT_TREE_DELETE_CRITICAL_OBJ",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_TREE_DELETE_CRITICAL_OBJ)));
	PyModule_AddObject(m, "WERR_DS_INIT_FAILURE_CONSOLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_INIT_FAILURE_CONSOLE)));
	PyModule_AddObject(m, "WERR_DS_SAM_INIT_FAILURE_CONSOLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SAM_INIT_FAILURE_CONSOLE)));
	PyModule_AddObject(m, "WERR_DS_FOREST_VERSION_TOO_HIGH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_FOREST_VERSION_TOO_HIGH)));
	PyModule_AddObject(m, "WERR_DS_DOMAIN_VERSION_TOO_HIGH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DOMAIN_VERSION_TOO_HIGH)));
	PyModule_AddObject(m, "WERR_DS_FOREST_VERSION_TOO_LOW",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_FOREST_VERSION_TOO_LOW)));
	PyModule_AddObject(m, "WERR_DS_DOMAIN_VERSION_TOO_LOW",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DOMAIN_VERSION_TOO_LOW)));
	PyModule_AddObject(m, "WERR_DS_INCOMPATIBLE_VERSION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_INCOMPATIBLE_VERSION)));
	PyModule_AddObject(m, "WERR_DS_LOW_DSA_VERSION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_LOW_DSA_VERSION)));
	PyModule_AddObject(m, "WERR_DS_NO_BEHAVIOR_VERSION_IN_MIXEDDOMAIN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NO_BEHAVIOR_VERSION_IN_MIXEDDOMAIN)));
	PyModule_AddObject(m, "WERR_DS_NOT_SUPPORTED_SORT_ORDER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NOT_SUPPORTED_SORT_ORDER)));
	PyModule_AddObject(m, "WERR_DS_NAME_NOT_UNIQUE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NAME_NOT_UNIQUE)));
	PyModule_AddObject(m, "WERR_DS_MACHINE_ACCOUNT_CREATED_PRENT4",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_MACHINE_ACCOUNT_CREATED_PRENT4)));
	PyModule_AddObject(m, "WERR_DS_OUT_OF_VERSION_STORE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_OUT_OF_VERSION_STORE)));
	PyModule_AddObject(m, "WERR_DS_INCOMPATIBLE_CONTROLS_USED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_INCOMPATIBLE_CONTROLS_USED)));
	PyModule_AddObject(m, "WERR_DS_NO_REF_DOMAIN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NO_REF_DOMAIN)));
	PyModule_AddObject(m, "WERR_DS_RESERVED_LINK_ID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_RESERVED_LINK_ID)));
	PyModule_AddObject(m, "WERR_DS_LINK_ID_NOT_AVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_LINK_ID_NOT_AVAILABLE)));
	PyModule_AddObject(m, "WERR_DS_AG_CANT_HAVE_UNIVERSAL_MEMBER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_AG_CANT_HAVE_UNIVERSAL_MEMBER)));
	PyModule_AddObject(m, "WERR_DS_MODIFYDN_DISALLOWED_BY_INSTANCE_TYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_MODIFYDN_DISALLOWED_BY_INSTANCE_TYPE)));
	PyModule_AddObject(m, "WERR_DS_NO_OBJECT_MOVE_IN_SCHEMA_NC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NO_OBJECT_MOVE_IN_SCHEMA_NC)));
	PyModule_AddObject(m, "WERR_DS_MODIFYDN_DISALLOWED_BY_FLAG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_MODIFYDN_DISALLOWED_BY_FLAG)));
	PyModule_AddObject(m, "WERR_DS_MODIFYDN_WRONG_GRANDPARENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_MODIFYDN_WRONG_GRANDPARENT)));
	PyModule_AddObject(m, "WERR_DS_NAME_ERROR_TRUST_REFERRAL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NAME_ERROR_TRUST_REFERRAL)));
	PyModule_AddObject(m, "WERR_NOT_SUPPORTED_ON_STANDARD_SERVER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NOT_SUPPORTED_ON_STANDARD_SERVER)));
	PyModule_AddObject(m, "WERR_DS_CANT_ACCESS_REMOTE_PART_OF_AD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_ACCESS_REMOTE_PART_OF_AD)));
	PyModule_AddObject(m, "WERR_DS_CR_IMPOSSIBLE_TO_VALIDATE_V2",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CR_IMPOSSIBLE_TO_VALIDATE_V2)));
	PyModule_AddObject(m, "WERR_DS_THREAD_LIMIT_EXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_THREAD_LIMIT_EXCEEDED)));
	PyModule_AddObject(m, "WERR_DS_NOT_CLOSEST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NOT_CLOSEST)));
	PyModule_AddObject(m, "WERR_DS_CANT_DERIVE_SPN_WITHOUT_SERVER_REF",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_DERIVE_SPN_WITHOUT_SERVER_REF)));
	PyModule_AddObject(m, "WERR_DS_SINGLE_USER_MODE_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SINGLE_USER_MODE_FAILED)));
	PyModule_AddObject(m, "WERR_DS_NTDSCRIPT_SYNTAX_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NTDSCRIPT_SYNTAX_ERROR)));
	PyModule_AddObject(m, "WERR_DS_NTDSCRIPT_PROCESS_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NTDSCRIPT_PROCESS_ERROR)));
	PyModule_AddObject(m, "WERR_DS_DIFFERENT_REPL_EPOCHS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DIFFERENT_REPL_EPOCHS)));
	PyModule_AddObject(m, "WERR_DS_DRS_EXTENSIONS_CHANGED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRS_EXTENSIONS_CHANGED)));
	PyModule_AddObject(m, "WERR_DS_REPLICA_SET_CHANGE_NOT_ALLOWED_ON_DISABLED_CR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_REPLICA_SET_CHANGE_NOT_ALLOWED_ON_DISABLED_CR)));
	PyModule_AddObject(m, "WERR_DS_NO_MSDS_INTID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NO_MSDS_INTID)));
	PyModule_AddObject(m, "WERR_DS_DUP_MSDS_INTID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DUP_MSDS_INTID)));
	PyModule_AddObject(m, "WERR_DS_EXISTS_IN_RDNATTID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_EXISTS_IN_RDNATTID)));
	PyModule_AddObject(m, "WERR_DS_AUTHORIZATION_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_AUTHORIZATION_FAILED)));
	PyModule_AddObject(m, "WERR_DS_INVALID_SCRIPT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_INVALID_SCRIPT)));
	PyModule_AddObject(m, "WERR_DS_REMOTE_CROSSREF_OP_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_REMOTE_CROSSREF_OP_FAILED)));
	PyModule_AddObject(m, "WERR_DS_CROSS_REF_BUSY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CROSS_REF_BUSY)));
	PyModule_AddObject(m, "WERR_DS_CANT_DERIVE_SPN_FOR_DELETED_DOMAIN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_DERIVE_SPN_FOR_DELETED_DOMAIN)));
	PyModule_AddObject(m, "WERR_DS_CANT_DEMOTE_WITH_WRITEABLE_NC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_DEMOTE_WITH_WRITEABLE_NC)));
	PyModule_AddObject(m, "WERR_DS_DUPLICATE_ID_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DUPLICATE_ID_FOUND)));
	PyModule_AddObject(m, "WERR_DS_INSUFFICIENT_ATTR_TO_CREATE_OBJECT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_INSUFFICIENT_ATTR_TO_CREATE_OBJECT)));
	PyModule_AddObject(m, "WERR_DS_GROUP_CONVERSION_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_GROUP_CONVERSION_ERROR)));
	PyModule_AddObject(m, "WERR_DS_CANT_MOVE_APP_BASIC_GROUP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_MOVE_APP_BASIC_GROUP)));
	PyModule_AddObject(m, "WERR_DS_CANT_MOVE_APP_QUERY_GROUP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_CANT_MOVE_APP_QUERY_GROUP)));
	PyModule_AddObject(m, "WERR_DS_ROLE_NOT_VERIFIED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_ROLE_NOT_VERIFIED)));
	PyModule_AddObject(m, "WERR_DS_WKO_CONTAINER_CANNOT_BE_SPECIAL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_WKO_CONTAINER_CANNOT_BE_SPECIAL)));
	PyModule_AddObject(m, "WERR_DS_DOMAIN_RENAME_IN_PROGRESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DOMAIN_RENAME_IN_PROGRESS)));
	PyModule_AddObject(m, "WERR_DS_EXISTING_AD_CHILD_NC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_EXISTING_AD_CHILD_NC)));
	PyModule_AddObject(m, "WERR_DS_REPL_LIFETIME_EXCEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_REPL_LIFETIME_EXCEEDED)));
	PyModule_AddObject(m, "WERR_DS_DISALLOWED_IN_SYSTEM_CONTAINER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DISALLOWED_IN_SYSTEM_CONTAINER)));
	PyModule_AddObject(m, "WERR_DS_LDAP_SEND_QUEUE_FULL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_LDAP_SEND_QUEUE_FULL)));
	PyModule_AddObject(m, "WERR_DS_DRA_OUT_SCHEDULE_WINDOW",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_DRA_OUT_SCHEDULE_WINDOW)));
	PyModule_AddObject(m, "WERR_DS_POLICY_NOT_KNOWN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_POLICY_NOT_KNOWN)));
	PyModule_AddObject(m, "WERR_NO_SITE_SETTINGS_OBJECT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_SITE_SETTINGS_OBJECT)));
	PyModule_AddObject(m, "WERR_NO_SECRETS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_SECRETS)));
	PyModule_AddObject(m, "WERR_NO_WRITABLE_DC_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_NO_WRITABLE_DC_FOUND)));
	PyModule_AddObject(m, "WERR_DS_NO_SERVER_OBJECT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NO_SERVER_OBJECT)));
	PyModule_AddObject(m, "WERR_DS_NO_NTDSA_OBJECT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NO_NTDSA_OBJECT)));
	PyModule_AddObject(m, "WERR_DS_NON_ASQ_SEARCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_NON_ASQ_SEARCH)));
	PyModule_AddObject(m, "WERR_DS_AUDIT_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_AUDIT_FAILURE)));
	PyModule_AddObject(m, "WERR_DS_INVALID_SEARCH_FLAG_SUBTREE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_INVALID_SEARCH_FLAG_SUBTREE)));
	PyModule_AddObject(m, "WERR_DS_INVALID_SEARCH_FLAG_TUPLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_INVALID_SEARCH_FLAG_TUPLE)));
	PyModule_AddObject(m, "WERR_DS_HIGH_DSA_VERSION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_HIGH_DSA_VERSION)));
	PyModule_AddObject(m, "WERR_DS_SPN_VALUE_NOT_UNIQUE_IN_FOREST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_SPN_VALUE_NOT_UNIQUE_IN_FOREST)));
	PyModule_AddObject(m, "WERR_DS_UPN_VALUE_NOT_UNIQUE_IN_FOREST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DS_UPN_VALUE_NOT_UNIQUE_IN_FOREST)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_RCODE_FORMAT_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_RCODE_FORMAT_ERROR)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_RCODE_SERVER_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_RCODE_SERVER_FAILURE)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_RCODE_NAME_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_RCODE_NAME_ERROR)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_RCODE_NOT_IMPLEMENTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_RCODE_NOT_IMPLEMENTED)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_RCODE_REFUSED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_RCODE_REFUSED)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_RCODE_YXDOMAIN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_RCODE_YXDOMAIN)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_RCODE_YXRRSET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_RCODE_YXRRSET)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_RCODE_NXRRSET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_RCODE_NXRRSET)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_RCODE_NOTAUTH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_RCODE_NOTAUTH)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_RCODE_NOTZONE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_RCODE_NOTZONE)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_RCODE_BADSIG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_RCODE_BADSIG)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_RCODE_BADKEY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_RCODE_BADKEY)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_RCODE_BADTIME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_RCODE_BADTIME)));
	PyModule_AddObject(m, "WERR_DNS_INFO_NO_RECORDS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_INFO_NO_RECORDS)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_BAD_PACKET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_BAD_PACKET)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_NO_PACKET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_NO_PACKET)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_RCODE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_RCODE)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_UNSECURE_PACKET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_UNSECURE_PACKET)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_INVALID_TYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_INVALID_TYPE)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_INVALID_IP_ADDRESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_INVALID_IP_ADDRESS)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_INVALID_PROPERTY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_INVALID_PROPERTY)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_TRY_AGAIN_LATER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_TRY_AGAIN_LATER)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_NOT_UNIQUE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_NOT_UNIQUE)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_NON_RFC_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_NON_RFC_NAME)));
	PyModule_AddObject(m, "WERR_DNS_STATUS_FQDN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_STATUS_FQDN)));
	PyModule_AddObject(m, "WERR_DNS_STATUS_DOTTED_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_STATUS_DOTTED_NAME)));
	PyModule_AddObject(m, "WERR_DNS_STATUS_SINGLE_PART_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_STATUS_SINGLE_PART_NAME)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_INVALID_NAME_CHAR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_INVALID_NAME_CHAR)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_NUMERIC_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_NUMERIC_NAME)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_NOT_ALLOWED_ON_ROOT_SERVER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_NOT_ALLOWED_ON_ROOT_SERVER)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_NOT_ALLOWED_UNDER_DELEGATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_NOT_ALLOWED_UNDER_DELEGATION)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_CANNOT_FIND_ROOT_HINTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_CANNOT_FIND_ROOT_HINTS)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_INCONSISTENT_ROOT_HINTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_INCONSISTENT_ROOT_HINTS)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_DWORD_VALUE_TOO_SMALL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_DWORD_VALUE_TOO_SMALL)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_DWORD_VALUE_TOO_LARGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_DWORD_VALUE_TOO_LARGE)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_BACKGROUND_LOADING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_BACKGROUND_LOADING)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_NOT_ALLOWED_ON_RODC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_NOT_ALLOWED_ON_RODC)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_ZONE_DOES_NOT_EXIST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_ZONE_DOES_NOT_EXIST)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_NO_ZONE_INFO",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_NO_ZONE_INFO)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_INVALID_ZONE_OPERATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_INVALID_ZONE_OPERATION)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_ZONE_CONFIGURATION_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_ZONE_CONFIGURATION_ERROR)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_ZONE_HAS_NO_SOA_RECORD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_ZONE_HAS_NO_SOA_RECORD)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_ZONE_HAS_NO_NS_RECORDS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_ZONE_HAS_NO_NS_RECORDS)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_ZONE_LOCKED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_ZONE_LOCKED)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_ZONE_CREATION_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_ZONE_CREATION_FAILED)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_ZONE_ALREADY_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_ZONE_ALREADY_EXISTS)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_AUTOZONE_ALREADY_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_AUTOZONE_ALREADY_EXISTS)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_INVALID_ZONE_TYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_INVALID_ZONE_TYPE)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_SECONDARY_REQUIRES_MASTER_IP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_SECONDARY_REQUIRES_MASTER_IP)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_ZONE_NOT_SECONDARY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_ZONE_NOT_SECONDARY)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_NEED_SECONDARY_ADDRESSES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_NEED_SECONDARY_ADDRESSES)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_WINS_INIT_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_WINS_INIT_FAILED)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_NEED_WINS_SERVERS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_NEED_WINS_SERVERS)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_NBSTAT_INIT_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_NBSTAT_INIT_FAILED)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_SOA_DELETE_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_SOA_DELETE_INVALID)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_FORWARDER_ALREADY_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_FORWARDER_ALREADY_EXISTS)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_ZONE_REQUIRES_MASTER_IP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_ZONE_REQUIRES_MASTER_IP)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_ZONE_IS_SHUTDOWN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_ZONE_IS_SHUTDOWN)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_PRIMARY_REQUIRES_DATAFILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_PRIMARY_REQUIRES_DATAFILE)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_INVALID_DATAFILE_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_INVALID_DATAFILE_NAME)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_DATAFILE_OPEN_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_DATAFILE_OPEN_FAILURE)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_FILE_WRITEBACK_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_FILE_WRITEBACK_FAILED)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_DATAFILE_PARSING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_DATAFILE_PARSING)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_RECORD_DOES_NOT_EXIST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_RECORD_DOES_NOT_EXIST)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_RECORD_FORMAT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_RECORD_FORMAT)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_NODE_CREATION_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_NODE_CREATION_FAILED)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_UNKNOWN_RECORD_TYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_UNKNOWN_RECORD_TYPE)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_RECORD_TIMED_OUT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_RECORD_TIMED_OUT)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_NAME_NOT_IN_ZONE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_NAME_NOT_IN_ZONE)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_CNAME_LOOP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_CNAME_LOOP)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_NODE_IS_CNAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_NODE_IS_CNAME)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_CNAME_COLLISION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_CNAME_COLLISION)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_RECORD_ONLY_AT_ZONE_ROOT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_RECORD_ONLY_AT_ZONE_ROOT)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_RECORD_ALREADY_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_RECORD_ALREADY_EXISTS)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_SECONDARY_DATA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_SECONDARY_DATA)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_NO_CREATE_CACHE_DATA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_NO_CREATE_CACHE_DATA)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_NAME_DOES_NOT_EXIST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_NAME_DOES_NOT_EXIST)));
	PyModule_AddObject(m, "WERR_DNS_WARNING_PTR_CREATE_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_WARNING_PTR_CREATE_FAILED)));
	PyModule_AddObject(m, "WERR_DNS_WARNING_DOMAIN_UNDELETED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_WARNING_DOMAIN_UNDELETED)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_DS_UNAVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_DS_UNAVAILABLE)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_DS_ZONE_ALREADY_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_DS_ZONE_ALREADY_EXISTS)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_NO_BOOTFILE_IF_DS_ZONE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_NO_BOOTFILE_IF_DS_ZONE)));
	PyModule_AddObject(m, "WERR_DNS_INFO_AXFR_COMPLETE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_INFO_AXFR_COMPLETE)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_AXFR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_AXFR)));
	PyModule_AddObject(m, "WERR_DNS_INFO_ADDED_LOCAL_WINS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_INFO_ADDED_LOCAL_WINS)));
	PyModule_AddObject(m, "WERR_DNS_STATUS_CONTINUE_NEEDED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_STATUS_CONTINUE_NEEDED)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_NO_TCPIP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_NO_TCPIP)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_NO_DNS_SERVERS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_NO_DNS_SERVERS)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_DP_DOES_NOT_EXIST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_DP_DOES_NOT_EXIST)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_DP_ALREADY_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_DP_ALREADY_EXISTS)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_DP_NOT_ENLISTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_DP_NOT_ENLISTED)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_DP_ALREADY_ENLISTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_DP_ALREADY_ENLISTED)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_DP_NOT_AVAILABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_DP_NOT_AVAILABLE)));
	PyModule_AddObject(m, "WERR_DNS_ERROR_DP_FSMO_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_DNS_ERROR_DP_FSMO_ERROR)));
	PyModule_AddObject(m, "WERR_WSAEINTR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEINTR)));
	PyModule_AddObject(m, "WERR_WSAEBADF",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEBADF)));
	PyModule_AddObject(m, "WERR_WSAEACCES",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEACCES)));
	PyModule_AddObject(m, "WERR_WSAEFAULT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEFAULT)));
	PyModule_AddObject(m, "WERR_WSAEINVAL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEINVAL)));
	PyModule_AddObject(m, "WERR_WSAEMFILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEMFILE)));
	PyModule_AddObject(m, "WERR_WSAEWOULDBLOCK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEWOULDBLOCK)));
	PyModule_AddObject(m, "WERR_WSAEINPROGRESS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEINPROGRESS)));
	PyModule_AddObject(m, "WERR_WSAEALREADY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEALREADY)));
	PyModule_AddObject(m, "WERR_WSAENOTSOCK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAENOTSOCK)));
	PyModule_AddObject(m, "WERR_WSAEDESTADDRREQ",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEDESTADDRREQ)));
	PyModule_AddObject(m, "WERR_WSAEMSGSIZE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEMSGSIZE)));
	PyModule_AddObject(m, "WERR_WSAEPROTOTYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEPROTOTYPE)));
	PyModule_AddObject(m, "WERR_WSAENOPROTOOPT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAENOPROTOOPT)));
	PyModule_AddObject(m, "WERR_WSAEPROTONOSUPPORT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEPROTONOSUPPORT)));
	PyModule_AddObject(m, "WERR_WSAESOCKTNOSUPPORT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAESOCKTNOSUPPORT)));
	PyModule_AddObject(m, "WERR_WSAEOPNOTSUPP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEOPNOTSUPP)));
	PyModule_AddObject(m, "WERR_WSAEPFNOSUPPORT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEPFNOSUPPORT)));
	PyModule_AddObject(m, "WERR_WSAEAFNOSUPPORT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEAFNOSUPPORT)));
	PyModule_AddObject(m, "WERR_WSAEADDRINUSE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEADDRINUSE)));
	PyModule_AddObject(m, "WERR_WSAEADDRNOTAVAIL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEADDRNOTAVAIL)));
	PyModule_AddObject(m, "WERR_WSAENETDOWN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAENETDOWN)));
	PyModule_AddObject(m, "WERR_WSAENETUNREACH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAENETUNREACH)));
	PyModule_AddObject(m, "WERR_WSAENETRESET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAENETRESET)));
	PyModule_AddObject(m, "WERR_WSAECONNABORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAECONNABORTED)));
	PyModule_AddObject(m, "WERR_WSAECONNRESET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAECONNRESET)));
	PyModule_AddObject(m, "WERR_WSAENOBUFS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAENOBUFS)));
	PyModule_AddObject(m, "WERR_WSAEISCONN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEISCONN)));
	PyModule_AddObject(m, "WERR_WSAENOTCONN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAENOTCONN)));
	PyModule_AddObject(m, "WERR_WSAESHUTDOWN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAESHUTDOWN)));
	PyModule_AddObject(m, "WERR_WSAETOOMANYREFS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAETOOMANYREFS)));
	PyModule_AddObject(m, "WERR_WSAETIMEDOUT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAETIMEDOUT)));
	PyModule_AddObject(m, "WERR_WSAECONNREFUSED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAECONNREFUSED)));
	PyModule_AddObject(m, "WERR_WSAELOOP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAELOOP)));
	PyModule_AddObject(m, "WERR_WSAENAMETOOLONG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAENAMETOOLONG)));
	PyModule_AddObject(m, "WERR_WSAEHOSTDOWN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEHOSTDOWN)));
	PyModule_AddObject(m, "WERR_WSAEHOSTUNREACH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEHOSTUNREACH)));
	PyModule_AddObject(m, "WERR_WSAENOTEMPTY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAENOTEMPTY)));
	PyModule_AddObject(m, "WERR_WSAEPROCLIM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEPROCLIM)));
	PyModule_AddObject(m, "WERR_WSAEUSERS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEUSERS)));
	PyModule_AddObject(m, "WERR_WSAEDQUOT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEDQUOT)));
	PyModule_AddObject(m, "WERR_WSAESTALE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAESTALE)));
	PyModule_AddObject(m, "WERR_WSAEREMOTE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEREMOTE)));
	PyModule_AddObject(m, "WERR_WSASYSNOTREADY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSASYSNOTREADY)));
	PyModule_AddObject(m, "WERR_WSAVERNOTSUPPORTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAVERNOTSUPPORTED)));
	PyModule_AddObject(m, "WERR_WSANOTINITIALISED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSANOTINITIALISED)));
	PyModule_AddObject(m, "WERR_WSAEDISCON",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEDISCON)));
	PyModule_AddObject(m, "WERR_WSAENOMORE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAENOMORE)));
	PyModule_AddObject(m, "WERR_WSAECANCELLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAECANCELLED)));
	PyModule_AddObject(m, "WERR_WSAEINVALIDPROCTABLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEINVALIDPROCTABLE)));
	PyModule_AddObject(m, "WERR_WSAEINVALIDPROVIDER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEINVALIDPROVIDER)));
	PyModule_AddObject(m, "WERR_WSAEPROVIDERFAILEDINIT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEPROVIDERFAILEDINIT)));
	PyModule_AddObject(m, "WERR_WSASYSCALLFAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSASYSCALLFAILURE)));
	PyModule_AddObject(m, "WERR_WSASERVICE_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSASERVICE_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_WSATYPE_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSATYPE_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_WSA_E_NO_MORE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_E_NO_MORE)));
	PyModule_AddObject(m, "WERR_WSA_E_CANCELLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_E_CANCELLED)));
	PyModule_AddObject(m, "WERR_WSAEREFUSED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAEREFUSED)));
	PyModule_AddObject(m, "WERR_WSAHOST_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSAHOST_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_WSATRY_AGAIN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSATRY_AGAIN)));
	PyModule_AddObject(m, "WERR_WSANO_RECOVERY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSANO_RECOVERY)));
	PyModule_AddObject(m, "WERR_WSANO_DATA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSANO_DATA)));
	PyModule_AddObject(m, "WERR_WSA_QOS_RECEIVERS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_RECEIVERS)));
	PyModule_AddObject(m, "WERR_WSA_QOS_SENDERS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_SENDERS)));
	PyModule_AddObject(m, "WERR_WSA_QOS_NO_SENDERS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_NO_SENDERS)));
	PyModule_AddObject(m, "WERR_WSA_QOS_NO_RECEIVERS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_NO_RECEIVERS)));
	PyModule_AddObject(m, "WERR_WSA_QOS_REQUEST_CONFIRMED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_REQUEST_CONFIRMED)));
	PyModule_AddObject(m, "WERR_WSA_QOS_ADMISSION_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_ADMISSION_FAILURE)));
	PyModule_AddObject(m, "WERR_WSA_QOS_POLICY_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_POLICY_FAILURE)));
	PyModule_AddObject(m, "WERR_WSA_QOS_BAD_STYLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_BAD_STYLE)));
	PyModule_AddObject(m, "WERR_WSA_QOS_BAD_OBJECT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_BAD_OBJECT)));
	PyModule_AddObject(m, "WERR_WSA_QOS_TRAFFIC_CTRL_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_TRAFFIC_CTRL_ERROR)));
	PyModule_AddObject(m, "WERR_WSA_QOS_GENERIC_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_GENERIC_ERROR)));
	PyModule_AddObject(m, "WERR_WSA_QOS_ESERVICETYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_ESERVICETYPE)));
	PyModule_AddObject(m, "WERR_WSA_QOS_EFLOWSPEC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_EFLOWSPEC)));
	PyModule_AddObject(m, "WERR_WSA_QOS_EPROVSPECBUF",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_EPROVSPECBUF)));
	PyModule_AddObject(m, "WERR_WSA_QOS_EFILTERSTYLE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_EFILTERSTYLE)));
	PyModule_AddObject(m, "WERR_WSA_QOS_EFILTERTYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_EFILTERTYPE)));
	PyModule_AddObject(m, "WERR_WSA_QOS_EFILTERCOUNT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_EFILTERCOUNT)));
	PyModule_AddObject(m, "WERR_WSA_QOS_EOBJLENGTH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_EOBJLENGTH)));
	PyModule_AddObject(m, "WERR_WSA_QOS_EFLOWCOUNT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_EFLOWCOUNT)));
	PyModule_AddObject(m, "WERR_WSA_QOS_EUNKOWNPSOBJ",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_EUNKOWNPSOBJ)));
	PyModule_AddObject(m, "WERR_WSA_QOS_EPOLICYOBJ",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_EPOLICYOBJ)));
	PyModule_AddObject(m, "WERR_WSA_QOS_EFLOWDESC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_EFLOWDESC)));
	PyModule_AddObject(m, "WERR_WSA_QOS_EPSFLOWSPEC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_EPSFLOWSPEC)));
	PyModule_AddObject(m, "WERR_WSA_QOS_EPSFILTERSPEC",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_EPSFILTERSPEC)));
	PyModule_AddObject(m, "WERR_WSA_QOS_ESDMODEOBJ",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_ESDMODEOBJ)));
	PyModule_AddObject(m, "WERR_WSA_QOS_ESHAPERATEOBJ",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_ESHAPERATEOBJ)));
	PyModule_AddObject(m, "WERR_WSA_QOS_RESERVED_PETYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WSA_QOS_RESERVED_PETYPE)));
	PyModule_AddObject(m, "WERR_IPSEC_QM_POLICY_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_QM_POLICY_EXISTS)));
	PyModule_AddObject(m, "WERR_IPSEC_QM_POLICY_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_QM_POLICY_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_IPSEC_QM_POLICY_IN_USE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_QM_POLICY_IN_USE)));
	PyModule_AddObject(m, "WERR_IPSEC_MM_POLICY_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_MM_POLICY_EXISTS)));
	PyModule_AddObject(m, "WERR_IPSEC_MM_POLICY_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_MM_POLICY_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_IPSEC_MM_POLICY_IN_USE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_MM_POLICY_IN_USE)));
	PyModule_AddObject(m, "WERR_IPSEC_MM_FILTER_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_MM_FILTER_EXISTS)));
	PyModule_AddObject(m, "WERR_IPSEC_MM_FILTER_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_MM_FILTER_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_IPSEC_TRANSPORT_FILTER_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_TRANSPORT_FILTER_EXISTS)));
	PyModule_AddObject(m, "WERR_IPSEC_TRANSPORT_FILTER_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_TRANSPORT_FILTER_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_IPSEC_MM_AUTH_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_MM_AUTH_EXISTS)));
	PyModule_AddObject(m, "WERR_IPSEC_MM_AUTH_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_MM_AUTH_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_IPSEC_MM_AUTH_IN_USE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_MM_AUTH_IN_USE)));
	PyModule_AddObject(m, "WERR_IPSEC_DEFAULT_MM_POLICY_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_DEFAULT_MM_POLICY_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_IPSEC_DEFAULT_MM_AUTH_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_DEFAULT_MM_AUTH_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_IPSEC_DEFAULT_QM_POLICY_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_DEFAULT_QM_POLICY_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_IPSEC_TUNNEL_FILTER_EXISTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_TUNNEL_FILTER_EXISTS)));
	PyModule_AddObject(m, "WERR_IPSEC_TUNNEL_FILTER_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_TUNNEL_FILTER_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_IPSEC_MM_FILTER_PENDING_DELETION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_MM_FILTER_PENDING_DELETION)));
	PyModule_AddObject(m, "WERR_IPSEC_TRANSPORT_FILTER_ENDING_DELETION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_TRANSPORT_FILTER_ENDING_DELETION)));
	PyModule_AddObject(m, "WERR_IPSEC_TUNNEL_FILTER_PENDING_DELETION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_TUNNEL_FILTER_PENDING_DELETION)));
	PyModule_AddObject(m, "WERR_IPSEC_MM_POLICY_PENDING_ELETION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_MM_POLICY_PENDING_ELETION)));
	PyModule_AddObject(m, "WERR_IPSEC_MM_AUTH_PENDING_DELETION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_MM_AUTH_PENDING_DELETION)));
	PyModule_AddObject(m, "WERR_IPSEC_QM_POLICY_PENDING_DELETION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_QM_POLICY_PENDING_DELETION)));
	PyModule_AddObject(m, "WERR_WARNING_IPSEC_MM_POLICY_PRUNED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WARNING_IPSEC_MM_POLICY_PRUNED)));
	PyModule_AddObject(m, "WERR_WARNING_IPSEC_QM_POLICY_PRUNED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_WARNING_IPSEC_QM_POLICY_PRUNED)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_NEG_STATUS_BEGIN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_NEG_STATUS_BEGIN)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_AUTH_FAIL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_AUTH_FAIL)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_ATTRIB_FAIL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_ATTRIB_FAIL)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_NEGOTIATION_PENDING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_NEGOTIATION_PENDING)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_GENERAL_PROCESSING_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_GENERAL_PROCESSING_ERROR)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_TIMED_OUT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_TIMED_OUT)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_NO_CERT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_NO_CERT)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_SA_DELETED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_SA_DELETED)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_SA_REAPED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_SA_REAPED)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_MM_ACQUIRE_DROP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_MM_ACQUIRE_DROP)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_QM_ACQUIRE_DROP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_QM_ACQUIRE_DROP)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_QUEUE_DROP_MM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_QUEUE_DROP_MM)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_QUEUE_DROP_NO_MM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_QUEUE_DROP_NO_MM)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_DROP_NO_RESPONSE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_DROP_NO_RESPONSE)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_MM_DELAY_DROP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_MM_DELAY_DROP)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_QM_DELAY_DROP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_QM_DELAY_DROP)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_ERROR)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_CRL_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_CRL_FAILED)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_INVALID_KEY_USAGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_INVALID_KEY_USAGE)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_INVALID_CERT_TYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_INVALID_CERT_TYPE)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_NO_PRIVATE_KEY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_NO_PRIVATE_KEY)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_DH_FAIL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_DH_FAIL)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_INVALID_HEADER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_INVALID_HEADER)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_NO_POLICY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_NO_POLICY)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_INVALID_SIGNATURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_INVALID_SIGNATURE)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_KERBEROS_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_KERBEROS_ERROR)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_NO_PUBLIC_KEY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_NO_PUBLIC_KEY)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_PROCESS_ERR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_PROCESS_ERR)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_PROCESS_ERR_SA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_PROCESS_ERR_SA)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_PROCESS_ERR_PROP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_PROCESS_ERR_PROP)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_PROCESS_ERR_TRANS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_PROCESS_ERR_TRANS)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_PROCESS_ERR_KE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_PROCESS_ERR_KE)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_PROCESS_ERR_ID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_PROCESS_ERR_ID)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_PROCESS_ERR_CERT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_PROCESS_ERR_CERT)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_PROCESS_ERR_CERT_REQ",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_PROCESS_ERR_CERT_REQ)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_PROCESS_ERR_HASH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_PROCESS_ERR_HASH)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_PROCESS_ERR_SIG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_PROCESS_ERR_SIG)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_PROCESS_ERR_NONCE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_PROCESS_ERR_NONCE)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_PROCESS_ERR_NOTIFY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_PROCESS_ERR_NOTIFY)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_PROCESS_ERR_DELETE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_PROCESS_ERR_DELETE)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_PROCESS_ERR_VENDOR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_PROCESS_ERR_VENDOR)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_INVALID_PAYLOAD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_INVALID_PAYLOAD)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_LOAD_SOFT_SA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_LOAD_SOFT_SA)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_SOFT_SA_TORN_DOWN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_SOFT_SA_TORN_DOWN)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_INVALID_COOKIE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_INVALID_COOKIE)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_NO_PEER_CERT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_NO_PEER_CERT)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_PEER_CRL_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_PEER_CRL_FAILED)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_POLICY_CHANGE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_POLICY_CHANGE)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_NO_MM_POLICY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_NO_MM_POLICY)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_NOTCBPRIV",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_NOTCBPRIV)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_SECLOADFAIL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_SECLOADFAIL)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_FAILSSPINIT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_FAILSSPINIT)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_FAILQUERYSSP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_FAILQUERYSSP)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_SRVACQFAIL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_SRVACQFAIL)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_SRVQUERYCRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_SRVQUERYCRED)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_GETSPIFAIL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_GETSPIFAIL)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_INVALID_FILTER",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_INVALID_FILTER)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_OUT_OF_MEMORY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_OUT_OF_MEMORY)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_ADD_UPDATE_KEY_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_ADD_UPDATE_KEY_FAILED)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_INVALID_POLICY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_INVALID_POLICY)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_UNKNOWN_DOI",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_UNKNOWN_DOI)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_INVALID_SITUATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_INVALID_SITUATION)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_DH_FAILURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_DH_FAILURE)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_INVALID_GROUP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_INVALID_GROUP)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_ENCRYPT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_ENCRYPT)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_DECRYPT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_DECRYPT)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_POLICY_MATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_POLICY_MATCH)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_UNSUPPORTED_ID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_UNSUPPORTED_ID)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_INVALID_HASH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_INVALID_HASH)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_INVALID_HASH_ALG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_INVALID_HASH_ALG)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_INVALID_HASH_SIZE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_INVALID_HASH_SIZE)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_INVALID_ENCRYPT_ALG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_INVALID_ENCRYPT_ALG)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_INVALID_AUTH_ALG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_INVALID_AUTH_ALG)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_INVALID_SIG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_INVALID_SIG)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_LOAD_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_LOAD_FAILED)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_RPC_DELETE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_RPC_DELETE)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_BENIGN_REINIT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_BENIGN_REINIT)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_INVALID_RESPONDER_LIFETIME_NOTIFY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_INVALID_RESPONDER_LIFETIME_NOTIFY)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_INVALID_CERT_KEYLEN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_INVALID_CERT_KEYLEN)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_MM_LIMIT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_MM_LIMIT)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_NEGOTIATION_DISABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_NEGOTIATION_DISABLED)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_QM_LIMIT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_QM_LIMIT)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_MM_EXPIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_MM_EXPIRED)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_PEER_MM_ASSUMED_INVALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_PEER_MM_ASSUMED_INVALID)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_CERT_CHAIN_POLICY_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_CERT_CHAIN_POLICY_MISMATCH)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_UNEXPECTED_MESSAGE_ID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_UNEXPECTED_MESSAGE_ID)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_INVALID_UMATTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_INVALID_UMATTS)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_DOS_COOKIE_SENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_DOS_COOKIE_SENT)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_SHUTTING_DOWN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_SHUTTING_DOWN)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_CGA_AUTH_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_CGA_AUTH_FAILED)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_PROCESS_ERR_NATOA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_PROCESS_ERR_NATOA)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_INVALID_MM_FOR_QM",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_INVALID_MM_FOR_QM)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_QM_EXPIRED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_QM_EXPIRED)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_TOO_MANY_FILTERS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_TOO_MANY_FILTERS)));
	PyModule_AddObject(m, "WERR_IPSEC_IKE_NEG_STATUS_END",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_IPSEC_IKE_NEG_STATUS_END)));
	PyModule_AddObject(m, "WERR_SXS_SECTION_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_SECTION_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_SXS_CANT_GEN_ACTCTX",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_CANT_GEN_ACTCTX)));
	PyModule_AddObject(m, "WERR_SXS_INVALID_ACTCTXDATA_FORMAT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_INVALID_ACTCTXDATA_FORMAT)));
	PyModule_AddObject(m, "WERR_SXS_ASSEMBLY_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_ASSEMBLY_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_SXS_MANIFEST_FORMAT_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_MANIFEST_FORMAT_ERROR)));
	PyModule_AddObject(m, "WERR_SXS_MANIFEST_PARSE_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_MANIFEST_PARSE_ERROR)));
	PyModule_AddObject(m, "WERR_SXS_ACTIVATION_CONTEXT_DISABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_ACTIVATION_CONTEXT_DISABLED)));
	PyModule_AddObject(m, "WERR_SXS_KEY_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_KEY_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_SXS_VERSION_CONFLICT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_VERSION_CONFLICT)));
	PyModule_AddObject(m, "WERR_SXS_WRONG_SECTION_TYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_WRONG_SECTION_TYPE)));
	PyModule_AddObject(m, "WERR_SXS_THREAD_QUERIES_DISABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_THREAD_QUERIES_DISABLED)));
	PyModule_AddObject(m, "WERR_SXS_PROCESS_DEFAULT_ALREADY_SET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_PROCESS_DEFAULT_ALREADY_SET)));
	PyModule_AddObject(m, "WERR_SXS_UNKNOWN_ENCODING_GROUP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_UNKNOWN_ENCODING_GROUP)));
	PyModule_AddObject(m, "WERR_SXS_UNKNOWN_ENCODING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_UNKNOWN_ENCODING)));
	PyModule_AddObject(m, "WERR_SXS_INVALID_XML_NAMESPACE_URI",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_INVALID_XML_NAMESPACE_URI)));
	PyModule_AddObject(m, "WERR_SXS_ROOT_MANIFEST_DEPENDENCY_OT_INSTALLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_ROOT_MANIFEST_DEPENDENCY_OT_INSTALLED)));
	PyModule_AddObject(m, "WERR_SXS_LEAF_MANIFEST_DEPENDENCY_NOT_INSTALLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_LEAF_MANIFEST_DEPENDENCY_NOT_INSTALLED)));
	PyModule_AddObject(m, "WERR_SXS_INVALID_ASSEMBLY_IDENTITY_ATTRIBUTE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_INVALID_ASSEMBLY_IDENTITY_ATTRIBUTE)));
	PyModule_AddObject(m, "WERR_SXS_MANIFEST_MISSING_REQUIRED_DEFAULT_NAMESPACE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_MANIFEST_MISSING_REQUIRED_DEFAULT_NAMESPACE)));
	PyModule_AddObject(m, "WERR_SXS_MANIFEST_INVALID_REQUIRED_DEFAULT_NAMESPACE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_MANIFEST_INVALID_REQUIRED_DEFAULT_NAMESPACE)));
	PyModule_AddObject(m, "WERR_SXS_PRIVATE_MANIFEST_CROSS_PATH_WITH_REPARSE_POINT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_PRIVATE_MANIFEST_CROSS_PATH_WITH_REPARSE_POINT)));
	PyModule_AddObject(m, "WERR_SXS_DUPLICATE_DLL_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_DUPLICATE_DLL_NAME)));
	PyModule_AddObject(m, "WERR_SXS_DUPLICATE_WINDOWCLASS_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_DUPLICATE_WINDOWCLASS_NAME)));
	PyModule_AddObject(m, "WERR_SXS_DUPLICATE_CLSID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_DUPLICATE_CLSID)));
	PyModule_AddObject(m, "WERR_SXS_DUPLICATE_IID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_DUPLICATE_IID)));
	PyModule_AddObject(m, "WERR_SXS_DUPLICATE_TLBID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_DUPLICATE_TLBID)));
	PyModule_AddObject(m, "WERR_SXS_DUPLICATE_PROGID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_DUPLICATE_PROGID)));
	PyModule_AddObject(m, "WERR_SXS_DUPLICATE_ASSEMBLY_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_DUPLICATE_ASSEMBLY_NAME)));
	PyModule_AddObject(m, "WERR_SXS_FILE_HASH_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_FILE_HASH_MISMATCH)));
	PyModule_AddObject(m, "WERR_SXS_POLICY_PARSE_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_POLICY_PARSE_ERROR)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_MISSINGQUOTE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_MISSINGQUOTE)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_COMMENTSYNTAX",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_COMMENTSYNTAX)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_BADSTARTNAMECHAR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_BADSTARTNAMECHAR)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_BADNAMECHAR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_BADNAMECHAR)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_BADCHARINSTRING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_BADCHARINSTRING)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_XMLDECLSYNTAX",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_XMLDECLSYNTAX)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_BADCHARDATA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_BADCHARDATA)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_MISSINGWHITESPACE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_MISSINGWHITESPACE)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_EXPECTINGTAGEND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_EXPECTINGTAGEND)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_MISSINGSEMICOLON",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_MISSINGSEMICOLON)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_UNBALANCEDPAREN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_UNBALANCEDPAREN)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_INTERNALERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_INTERNALERROR)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_UNEXPECTED_WHITESPACE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_UNEXPECTED_WHITESPACE)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_INCOMPLETE_ENCODING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_INCOMPLETE_ENCODING)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_MISSING_PAREN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_MISSING_PAREN)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_EXPECTINGCLOSEQUOTE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_EXPECTINGCLOSEQUOTE)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_MULTIPLE_COLONS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_MULTIPLE_COLONS)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_INVALID_DECIMAL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_INVALID_DECIMAL)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_INVALID_HEXIDECIMAL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_INVALID_HEXIDECIMAL)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_INVALID_UNICODE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_INVALID_UNICODE)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_WHITESPACEORQUESTIONMARK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_WHITESPACEORQUESTIONMARK)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_UNEXPECTEDENDTAG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_UNEXPECTEDENDTAG)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_UNCLOSEDTAG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_UNCLOSEDTAG)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_DUPLICATEATTRIBUTE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_DUPLICATEATTRIBUTE)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_MULTIPLEROOTS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_MULTIPLEROOTS)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_INVALIDATROOTLEVEL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_INVALIDATROOTLEVEL)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_BADXMLDECL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_BADXMLDECL)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_MISSINGROOT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_MISSINGROOT)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_UNEXPECTEDEOF",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_UNEXPECTEDEOF)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_BADPEREFINSUBSET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_BADPEREFINSUBSET)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_UNCLOSEDSTARTTAG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_UNCLOSEDSTARTTAG)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_UNCLOSEDENDTAG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_UNCLOSEDENDTAG)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_UNCLOSEDSTRING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_UNCLOSEDSTRING)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_UNCLOSEDCOMMENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_UNCLOSEDCOMMENT)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_UNCLOSEDDECL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_UNCLOSEDDECL)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_UNCLOSEDCDATA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_UNCLOSEDCDATA)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_RESERVEDNAMESPACE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_RESERVEDNAMESPACE)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_INVALIDENCODING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_INVALIDENCODING)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_INVALIDSWITCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_INVALIDSWITCH)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_BADXMLCASE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_BADXMLCASE)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_INVALID_STANDALONE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_INVALID_STANDALONE)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_UNEXPECTED_STANDALONE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_UNEXPECTED_STANDALONE)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_INVALID_VERSION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_INVALID_VERSION)));
	PyModule_AddObject(m, "WERR_SXS_XML_E_MISSINGEQUALS",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_XML_E_MISSINGEQUALS)));
	PyModule_AddObject(m, "WERR_SXS_PROTECTION_RECOVERY_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_PROTECTION_RECOVERY_FAILED)));
	PyModule_AddObject(m, "WERR_SXS_PROTECTION_PUBLIC_KEY_OO_SHORT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_PROTECTION_PUBLIC_KEY_OO_SHORT)));
	PyModule_AddObject(m, "WERR_SXS_PROTECTION_CATALOG_NOT_VALID",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_PROTECTION_CATALOG_NOT_VALID)));
	PyModule_AddObject(m, "WERR_SXS_UNTRANSLATABLE_HRESULT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_UNTRANSLATABLE_HRESULT)));
	PyModule_AddObject(m, "WERR_SXS_PROTECTION_CATALOG_FILE_MISSING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_PROTECTION_CATALOG_FILE_MISSING)));
	PyModule_AddObject(m, "WERR_SXS_MISSING_ASSEMBLY_IDENTITY_ATTRIBUTE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_MISSING_ASSEMBLY_IDENTITY_ATTRIBUTE)));
	PyModule_AddObject(m, "WERR_SXS_INVALID_ASSEMBLY_IDENTITY_ATTRIBUTE_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_INVALID_ASSEMBLY_IDENTITY_ATTRIBUTE_NAME)));
	PyModule_AddObject(m, "WERR_SXS_ASSEMBLY_MISSING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_ASSEMBLY_MISSING)));
	PyModule_AddObject(m, "WERR_SXS_CORRUPT_ACTIVATION_STACK",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_CORRUPT_ACTIVATION_STACK)));
	PyModule_AddObject(m, "WERR_SXS_CORRUPTION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_CORRUPTION)));
	PyModule_AddObject(m, "WERR_SXS_EARLY_DEACTIVATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_EARLY_DEACTIVATION)));
	PyModule_AddObject(m, "WERR_SXS_INVALID_DEACTIVATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_INVALID_DEACTIVATION)));
	PyModule_AddObject(m, "WERR_SXS_MULTIPLE_DEACTIVATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_MULTIPLE_DEACTIVATION)));
	PyModule_AddObject(m, "WERR_SXS_PROCESS_TERMINATION_REQUESTED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_PROCESS_TERMINATION_REQUESTED)));
	PyModule_AddObject(m, "WERR_SXS_RELEASE_ACTIVATION_ONTEXT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_RELEASE_ACTIVATION_ONTEXT)));
	PyModule_AddObject(m, "WERR_SXS_SYSTEM_DEFAULT_ACTIVATION_CONTEXT_EMPTY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_SYSTEM_DEFAULT_ACTIVATION_CONTEXT_EMPTY)));
	PyModule_AddObject(m, "WERR_SXS_INVALID_IDENTITY_ATTRIBUTE_VALUE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_INVALID_IDENTITY_ATTRIBUTE_VALUE)));
	PyModule_AddObject(m, "WERR_SXS_INVALID_IDENTITY_ATTRIBUTE_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_INVALID_IDENTITY_ATTRIBUTE_NAME)));
	PyModule_AddObject(m, "WERR_SXS_IDENTITY_DUPLICATE_ATTRIBUTE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_IDENTITY_DUPLICATE_ATTRIBUTE)));
	PyModule_AddObject(m, "WERR_SXS_IDENTITY_PARSE_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_IDENTITY_PARSE_ERROR)));
	PyModule_AddObject(m, "WERR_MALFORMED_SUBSTITUTION_STRING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MALFORMED_SUBSTITUTION_STRING)));
	PyModule_AddObject(m, "WERR_SXS_INCORRECT_PUBLIC_KEY_OKEN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_INCORRECT_PUBLIC_KEY_OKEN)));
	PyModule_AddObject(m, "WERR_UNMAPPED_SUBSTITUTION_STRING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_UNMAPPED_SUBSTITUTION_STRING)));
	PyModule_AddObject(m, "WERR_SXS_ASSEMBLY_NOT_LOCKED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_ASSEMBLY_NOT_LOCKED)));
	PyModule_AddObject(m, "WERR_SXS_COMPONENT_STORE_CORRUPT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_COMPONENT_STORE_CORRUPT)));
	PyModule_AddObject(m, "WERR_ADVANCED_INSTALLER_FAILED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_ADVANCED_INSTALLER_FAILED)));
	PyModule_AddObject(m, "WERR_XML_ENCODING_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_XML_ENCODING_MISMATCH)));
	PyModule_AddObject(m, "WERR_SXS_MANIFEST_IDENTITY_SAME_BUT_CONTENTS_DIFFERENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_MANIFEST_IDENTITY_SAME_BUT_CONTENTS_DIFFERENT)));
	PyModule_AddObject(m, "WERR_SXS_IDENTITIES_DIFFERENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_IDENTITIES_DIFFERENT)));
	PyModule_AddObject(m, "WERR_SXS_ASSEMBLY_IS_NOT_A_DEPLOYMENT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_ASSEMBLY_IS_NOT_A_DEPLOYMENT)));
	PyModule_AddObject(m, "WERR_SXS_FILE_NOT_PART_OF_ASSEMBLY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_FILE_NOT_PART_OF_ASSEMBLY)));
	PyModule_AddObject(m, "WERR_SXS_MANIFEST_TOO_BIG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_MANIFEST_TOO_BIG)));
	PyModule_AddObject(m, "WERR_SXS_SETTING_NOT_REGISTERED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_SETTING_NOT_REGISTERED)));
	PyModule_AddObject(m, "WERR_SXS_TRANSACTION_CLOSURE_INCOMPLETE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SXS_TRANSACTION_CLOSURE_INCOMPLETE)));
	PyModule_AddObject(m, "WERR_EVT_INVALID_CHANNEL_PATH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_INVALID_CHANNEL_PATH)));
	PyModule_AddObject(m, "WERR_EVT_INVALID_QUERY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_INVALID_QUERY)));
	PyModule_AddObject(m, "WERR_EVT_PUBLISHER_METADATA_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_PUBLISHER_METADATA_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_EVT_EVENT_TEMPLATE_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_EVENT_TEMPLATE_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_EVT_INVALID_PUBLISHER_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_INVALID_PUBLISHER_NAME)));
	PyModule_AddObject(m, "WERR_EVT_INVALID_EVENT_DATA",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_INVALID_EVENT_DATA)));
	PyModule_AddObject(m, "WERR_EVT_CHANNEL_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_CHANNEL_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_EVT_MALFORMED_XML_TEXT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_MALFORMED_XML_TEXT)));
	PyModule_AddObject(m, "WERR_EVT_SUBSCRIPTION_TO_DIRECT_CHANNEL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_SUBSCRIPTION_TO_DIRECT_CHANNEL)));
	PyModule_AddObject(m, "WERR_EVT_CONFIGURATION_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_CONFIGURATION_ERROR)));
	PyModule_AddObject(m, "WERR_EVT_QUERY_RESULT_STALE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_QUERY_RESULT_STALE)));
	PyModule_AddObject(m, "WERR_EVT_QUERY_RESULT_INVALID_POSITION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_QUERY_RESULT_INVALID_POSITION)));
	PyModule_AddObject(m, "WERR_EVT_NON_VALIDATING_MSXML",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_NON_VALIDATING_MSXML)));
	PyModule_AddObject(m, "WERR_EVT_FILTER_ALREADYSCOPED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_FILTER_ALREADYSCOPED)));
	PyModule_AddObject(m, "WERR_EVT_FILTER_NOTELTSET",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_FILTER_NOTELTSET)));
	PyModule_AddObject(m, "WERR_EVT_FILTER_INVARG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_FILTER_INVARG)));
	PyModule_AddObject(m, "WERR_EVT_FILTER_INVTEST",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_FILTER_INVTEST)));
	PyModule_AddObject(m, "WERR_EVT_FILTER_INVTYPE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_FILTER_INVTYPE)));
	PyModule_AddObject(m, "WERR_EVT_FILTER_PARSEERR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_FILTER_PARSEERR)));
	PyModule_AddObject(m, "WERR_EVT_FILTER_UNSUPPORTEDOP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_FILTER_UNSUPPORTEDOP)));
	PyModule_AddObject(m, "WERR_EVT_FILTER_UNEXPECTEDTOKEN",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_FILTER_UNEXPECTEDTOKEN)));
	PyModule_AddObject(m, "WERR_EVT_INVALID_OPERATION_OVER_ENABLED_DIRECT_CHANNEL",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_INVALID_OPERATION_OVER_ENABLED_DIRECT_CHANNEL)));
	PyModule_AddObject(m, "WERR_EVT_INVALID_CHANNEL_PROPERTY_VALUE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_INVALID_CHANNEL_PROPERTY_VALUE)));
	PyModule_AddObject(m, "WERR_EVT_INVALID_PUBLISHER_PROPERTY_VALUE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_INVALID_PUBLISHER_PROPERTY_VALUE)));
	PyModule_AddObject(m, "WERR_EVT_CHANNEL_CANNOT_ACTIVATE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_CHANNEL_CANNOT_ACTIVATE)));
	PyModule_AddObject(m, "WERR_EVT_FILTER_TOO_COMPLEX",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_FILTER_TOO_COMPLEX)));
	PyModule_AddObject(m, "WERR_EVT_MESSAGE_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_MESSAGE_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_EVT_MESSAGE_ID_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_MESSAGE_ID_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_EVT_UNRESOLVED_VALUE_INSERT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_UNRESOLVED_VALUE_INSERT)));
	PyModule_AddObject(m, "WERR_EVT_UNRESOLVED_PARAMETER_INSERT",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_UNRESOLVED_PARAMETER_INSERT)));
	PyModule_AddObject(m, "WERR_EVT_MAX_INSERTS_REACHED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_MAX_INSERTS_REACHED)));
	PyModule_AddObject(m, "WERR_EVT_EVENT_DEFINITION_NOT_OUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_EVENT_DEFINITION_NOT_OUND)));
	PyModule_AddObject(m, "WERR_EVT_MESSAGE_LOCALE_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_MESSAGE_LOCALE_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_EVT_VERSION_TOO_OLD",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_VERSION_TOO_OLD)));
	PyModule_AddObject(m, "WERR_EVT_VERSION_TOO_NEW",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_VERSION_TOO_NEW)));
	PyModule_AddObject(m, "WERR_EVT_CANNOT_OPEN_CHANNEL_OF_QUERY",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_CANNOT_OPEN_CHANNEL_OF_QUERY)));
	PyModule_AddObject(m, "WERR_EVT_PUBLISHER_DISABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EVT_PUBLISHER_DISABLED)));
	PyModule_AddObject(m, "WERR_EC_SUBSCRIPTION_CANNOT_ACTIVATE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EC_SUBSCRIPTION_CANNOT_ACTIVATE)));
	PyModule_AddObject(m, "WERR_EC_LOG_DISABLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_EC_LOG_DISABLED)));
	PyModule_AddObject(m, "WERR_MUI_FILE_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MUI_FILE_NOT_FOUND)));
	PyModule_AddObject(m, "WERR_MUI_INVALID_FILE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MUI_INVALID_FILE)));
	PyModule_AddObject(m, "WERR_MUI_INVALID_RC_CONFIG",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MUI_INVALID_RC_CONFIG)));
	PyModule_AddObject(m, "WERR_MUI_INVALID_LOCALE_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MUI_INVALID_LOCALE_NAME)));
	PyModule_AddObject(m, "WERR_MUI_INVALID_ULTIMATEFALLBACK_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MUI_INVALID_ULTIMATEFALLBACK_NAME)));
	PyModule_AddObject(m, "WERR_MUI_FILE_NOT_LOADED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MUI_FILE_NOT_LOADED)));
	PyModule_AddObject(m, "WERR_RESOURCE_ENUM_USER_STOP",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_RESOURCE_ENUM_USER_STOP)));
	PyModule_AddObject(m, "WERR_MUI_INTLSETTINGS_UILANG_NOT_INSTALLED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MUI_INTLSETTINGS_UILANG_NOT_INSTALLED)));
	PyModule_AddObject(m, "WERR_MUI_INTLSETTINGS_INVALID_LOCALE_NAME",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MUI_INTLSETTINGS_INVALID_LOCALE_NAME)));
	PyModule_AddObject(m, "WERR_MCA_INVALID_CAPABILITIES_STRING",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MCA_INVALID_CAPABILITIES_STRING)));
	PyModule_AddObject(m, "WERR_MCA_INVALID_VCP_VERSION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MCA_INVALID_VCP_VERSION)));
	PyModule_AddObject(m, "WERR_MCA_MONITOR_VIOLATES_MCCS_SPECIFICATION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MCA_MONITOR_VIOLATES_MCCS_SPECIFICATION)));
	PyModule_AddObject(m, "WERR_MCA_MCCS_VERSION_MISMATCH",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MCA_MCCS_VERSION_MISMATCH)));
	PyModule_AddObject(m, "WERR_MCA_UNSUPPORTED_MCCS_VERSION",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MCA_UNSUPPORTED_MCCS_VERSION)));
	PyModule_AddObject(m, "WERR_MCA_INTERNAL_ERROR",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MCA_INTERNAL_ERROR)));
	PyModule_AddObject(m, "WERR_MCA_INVALID_TECHNOLOGY_TYPE_RETURNED",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MCA_INVALID_TECHNOLOGY_TYPE_RETURNED)));
	PyModule_AddObject(m, "WERR_MCA_UNSUPPORTED_COLOR_TEMPERATURE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_MCA_UNSUPPORTED_COLOR_TEMPERATURE)));
	PyModule_AddObject(m, "WERR_AMBIGUOUS_SYSTEM_DEVICE",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_AMBIGUOUS_SYSTEM_DEVICE)));
	PyModule_AddObject(m, "WERR_SYSTEM_DEVICE_NOT_FOUND",
                  		ndr_PyLong_FromUnsignedLongLong(W_ERROR_V(WERR_SYSTEM_DEVICE_NOT_FOUND)));

	return m;
}
