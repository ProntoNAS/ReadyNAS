/*
 * New descriptions for existing errors generated from
 * [MS-ERREF] http://msdn.microsoft.com/en-us/library/cc704588.aspx
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
	.m_name = "ntstatus",
	.m_doc = "NTSTATUS error defines",
	.m_size = -1,
};

MODULE_INIT_FUNC(ntstatus)
{
	PyObject *m;

	m = PyModule_Create(&moduledef);
	if (m == NULL)
		return NULL;

	PyModule_AddObject(m, "NT_STATUS_SUCCESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SUCCESS)));
	PyModule_AddObject(m, "NT_STATUS_WAIT_0", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WAIT_0)));
	PyModule_AddObject(m, "NT_STATUS_WAIT_1", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WAIT_1)));
	PyModule_AddObject(m, "NT_STATUS_WAIT_2", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WAIT_2)));
	PyModule_AddObject(m, "NT_STATUS_WAIT_3", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WAIT_3)));
	PyModule_AddObject(m, "NT_STATUS_WAIT_63", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WAIT_63)));
	PyModule_AddObject(m, "NT_STATUS_ABANDONED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ABANDONED)));
	PyModule_AddObject(m, "NT_STATUS_ABANDONED_WAIT_0", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ABANDONED_WAIT_0)));
	PyModule_AddObject(m, "NT_STATUS_ABANDONED_WAIT_63", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ABANDONED_WAIT_63)));
	PyModule_AddObject(m, "NT_STATUS_USER_APC", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_USER_APC)));
	PyModule_AddObject(m, "NT_STATUS_ALERTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ALERTED)));
	PyModule_AddObject(m, "NT_STATUS_TIMEOUT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TIMEOUT)));
	PyModule_AddObject(m, "NT_STATUS_PENDING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PENDING)));
	PyModule_AddObject(m, "NT_STATUS_REPARSE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REPARSE)));
	PyModule_AddObject(m, "NT_STATUS_MORE_ENTRIES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MORE_ENTRIES)));
	PyModule_AddObject(m, "NT_STATUS_NOT_ALL_ASSIGNED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOT_ALL_ASSIGNED)));
	PyModule_AddObject(m, "NT_STATUS_SOME_NOT_MAPPED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SOME_NOT_MAPPED)));
	PyModule_AddObject(m, "NT_STATUS_OPLOCK_BREAK_IN_PROGRESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_OPLOCK_BREAK_IN_PROGRESS)));
	PyModule_AddObject(m, "NT_STATUS_VOLUME_MOUNTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VOLUME_MOUNTED)));
	PyModule_AddObject(m, "NT_STATUS_RXACT_COMMITTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RXACT_COMMITTED)));
	PyModule_AddObject(m, "NT_STATUS_NOTIFY_CLEANUP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOTIFY_CLEANUP)));
	PyModule_AddObject(m, "NT_STATUS_NOTIFY_ENUM_DIR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOTIFY_ENUM_DIR)));
	PyModule_AddObject(m, "NT_STATUS_NO_QUOTAS_FOR_ACCOUNT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_QUOTAS_FOR_ACCOUNT)));
	PyModule_AddObject(m, "NT_STATUS_PRIMARY_TRANSPORT_CONNECT_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PRIMARY_TRANSPORT_CONNECT_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_PAGE_FAULT_TRANSITION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PAGE_FAULT_TRANSITION)));
	PyModule_AddObject(m, "NT_STATUS_PAGE_FAULT_DEMAND_ZERO", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PAGE_FAULT_DEMAND_ZERO)));
	PyModule_AddObject(m, "NT_STATUS_PAGE_FAULT_COPY_ON_WRITE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PAGE_FAULT_COPY_ON_WRITE)));
	PyModule_AddObject(m, "NT_STATUS_PAGE_FAULT_GUARD_PAGE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PAGE_FAULT_GUARD_PAGE)));
	PyModule_AddObject(m, "NT_STATUS_PAGE_FAULT_PAGING_FILE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PAGE_FAULT_PAGING_FILE)));
	PyModule_AddObject(m, "NT_STATUS_CACHE_PAGE_LOCKED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CACHE_PAGE_LOCKED)));
	PyModule_AddObject(m, "NT_STATUS_CRASH_DUMP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CRASH_DUMP)));
	PyModule_AddObject(m, "NT_STATUS_BUFFER_ALL_ZEROS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BUFFER_ALL_ZEROS)));
	PyModule_AddObject(m, "NT_STATUS_REPARSE_OBJECT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REPARSE_OBJECT)));
	PyModule_AddObject(m, "NT_STATUS_RESOURCE_REQUIREMENTS_CHANGED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RESOURCE_REQUIREMENTS_CHANGED)));
	PyModule_AddObject(m, "NT_STATUS_TRANSLATION_COMPLETE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSLATION_COMPLETE)));
	PyModule_AddObject(m, "NT_STATUS_DS_MEMBERSHIP_EVALUATED_LOCALLY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_MEMBERSHIP_EVALUATED_LOCALLY)));
	PyModule_AddObject(m, "NT_STATUS_NOTHING_TO_TERMINATE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOTHING_TO_TERMINATE)));
	PyModule_AddObject(m, "NT_STATUS_PROCESS_NOT_IN_JOB", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PROCESS_NOT_IN_JOB)));
	PyModule_AddObject(m, "NT_STATUS_PROCESS_IN_JOB", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PROCESS_IN_JOB)));
	PyModule_AddObject(m, "NT_STATUS_VOLSNAP_HIBERNATE_READY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VOLSNAP_HIBERNATE_READY)));
	PyModule_AddObject(m, "NT_STATUS_FSFILTER_OP_COMPLETED_SUCCESSFULLY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FSFILTER_OP_COMPLETED_SUCCESSFULLY)));
	PyModule_AddObject(m, "NT_STATUS_INTERRUPT_VECTOR_ALREADY_CONNECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INTERRUPT_VECTOR_ALREADY_CONNECTED)));
	PyModule_AddObject(m, "NT_STATUS_INTERRUPT_STILL_CONNECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INTERRUPT_STILL_CONNECTED)));
	PyModule_AddObject(m, "NT_STATUS_PROCESS_CLONED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PROCESS_CLONED)));
	PyModule_AddObject(m, "NT_STATUS_FILE_LOCKED_WITH_ONLY_READERS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FILE_LOCKED_WITH_ONLY_READERS)));
	PyModule_AddObject(m, "NT_STATUS_FILE_LOCKED_WITH_WRITERS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FILE_LOCKED_WITH_WRITERS)));
	PyModule_AddObject(m, "NT_STATUS_RESOURCEMANAGER_READ_ONLY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RESOURCEMANAGER_READ_ONLY)));
	PyModule_AddObject(m, "NT_STATUS_WAIT_FOR_OPLOCK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WAIT_FOR_OPLOCK)));
	PyModule_AddObject(m, "NT_STATUS_DBG_EXCEPTION_HANDLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DBG_EXCEPTION_HANDLED)));
	PyModule_AddObject(m, "NT_STATUS_DBG_CONTINUE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DBG_CONTINUE)));
	PyModule_AddObject(m, "NT_STATUS_FLT_IO_COMPLETE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_IO_COMPLETE)));
	PyModule_AddObject(m, "NT_STATUS_FILE_NOT_AVAILABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FILE_NOT_AVAILABLE)));
	PyModule_AddObject(m, "NT_STATUS_SHARE_UNAVAILABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SHARE_UNAVAILABLE)));
	PyModule_AddObject(m, "NT_STATUS_CALLBACK_RETURNED_THREAD_AFFINITY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CALLBACK_RETURNED_THREAD_AFFINITY)));
	PyModule_AddObject(m, "NT_STATUS_OBJECT_NAME_EXISTS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_OBJECT_NAME_EXISTS)));
	PyModule_AddObject(m, "NT_STATUS_THREAD_WAS_SUSPENDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_THREAD_WAS_SUSPENDED)));
	PyModule_AddObject(m, "NT_STATUS_WORKING_SET_LIMIT_RANGE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WORKING_SET_LIMIT_RANGE)));
	PyModule_AddObject(m, "NT_STATUS_IMAGE_NOT_AT_BASE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IMAGE_NOT_AT_BASE)));
	PyModule_AddObject(m, "NT_STATUS_RXACT_STATE_CREATED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RXACT_STATE_CREATED)));
	PyModule_AddObject(m, "NT_STATUS_SEGMENT_NOTIFICATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SEGMENT_NOTIFICATION)));
	PyModule_AddObject(m, "NT_STATUS_LOCAL_USER_SESSION_KEY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOCAL_USER_SESSION_KEY)));
	PyModule_AddObject(m, "NT_STATUS_BAD_CURRENT_DIRECTORY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BAD_CURRENT_DIRECTORY)));
	PyModule_AddObject(m, "NT_STATUS_SERIAL_MORE_WRITES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SERIAL_MORE_WRITES)));
	PyModule_AddObject(m, "NT_STATUS_REGISTRY_RECOVERED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REGISTRY_RECOVERED)));
	PyModule_AddObject(m, "NT_STATUS_FT_READ_RECOVERY_FROM_BACKUP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FT_READ_RECOVERY_FROM_BACKUP)));
	PyModule_AddObject(m, "NT_STATUS_FT_WRITE_RECOVERY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FT_WRITE_RECOVERY)));
	PyModule_AddObject(m, "NT_STATUS_SERIAL_COUNTER_TIMEOUT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SERIAL_COUNTER_TIMEOUT)));
	PyModule_AddObject(m, "NT_STATUS_NULL_LM_PASSWORD", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NULL_LM_PASSWORD)));
	PyModule_AddObject(m, "NT_STATUS_IMAGE_MACHINE_TYPE_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IMAGE_MACHINE_TYPE_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_RECEIVE_PARTIAL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RECEIVE_PARTIAL)));
	PyModule_AddObject(m, "NT_STATUS_RECEIVE_EXPEDITED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RECEIVE_EXPEDITED)));
	PyModule_AddObject(m, "NT_STATUS_RECEIVE_PARTIAL_EXPEDITED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RECEIVE_PARTIAL_EXPEDITED)));
	PyModule_AddObject(m, "NT_STATUS_EVENT_DONE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_EVENT_DONE)));
	PyModule_AddObject(m, "NT_STATUS_EVENT_PENDING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_EVENT_PENDING)));
	PyModule_AddObject(m, "NT_STATUS_CHECKING_FILE_SYSTEM", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CHECKING_FILE_SYSTEM)));
	PyModule_AddObject(m, "NT_STATUS_FATAL_APP_EXIT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FATAL_APP_EXIT)));
	PyModule_AddObject(m, "NT_STATUS_PREDEFINED_HANDLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PREDEFINED_HANDLE)));
	PyModule_AddObject(m, "NT_STATUS_WAS_UNLOCKED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WAS_UNLOCKED)));
	PyModule_AddObject(m, "NT_STATUS_SERVICE_NOTIFICATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SERVICE_NOTIFICATION)));
	PyModule_AddObject(m, "NT_STATUS_WAS_LOCKED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WAS_LOCKED)));
	PyModule_AddObject(m, "NT_STATUS_LOG_HARD_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_HARD_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_ALREADY_WIN32", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ALREADY_WIN32)));
	PyModule_AddObject(m, "NT_STATUS_WX86_UNSIMULATE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WX86_UNSIMULATE)));
	PyModule_AddObject(m, "NT_STATUS_WX86_CONTINUE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WX86_CONTINUE)));
	PyModule_AddObject(m, "NT_STATUS_WX86_SINGLE_STEP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WX86_SINGLE_STEP)));
	PyModule_AddObject(m, "NT_STATUS_WX86_BREAKPOINT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WX86_BREAKPOINT)));
	PyModule_AddObject(m, "NT_STATUS_WX86_EXCEPTION_CONTINUE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WX86_EXCEPTION_CONTINUE)));
	PyModule_AddObject(m, "NT_STATUS_WX86_EXCEPTION_LASTCHANCE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WX86_EXCEPTION_LASTCHANCE)));
	PyModule_AddObject(m, "NT_STATUS_WX86_EXCEPTION_CHAIN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WX86_EXCEPTION_CHAIN)));
	PyModule_AddObject(m, "NT_STATUS_IMAGE_MACHINE_TYPE_MISMATCH_EXE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IMAGE_MACHINE_TYPE_MISMATCH_EXE)));
	PyModule_AddObject(m, "NT_STATUS_NO_YIELD_PERFORMED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_YIELD_PERFORMED)));
	PyModule_AddObject(m, "NT_STATUS_TIMER_RESUME_IGNORED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TIMER_RESUME_IGNORED)));
	PyModule_AddObject(m, "NT_STATUS_ARBITRATION_UNHANDLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ARBITRATION_UNHANDLED)));
	PyModule_AddObject(m, "NT_STATUS_CARDBUS_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CARDBUS_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_WX86_CREATEWX86TIB", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WX86_CREATEWX86TIB)));
	PyModule_AddObject(m, "NT_STATUS_MP_PROCESSOR_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MP_PROCESSOR_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_HIBERNATED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_HIBERNATED)));
	PyModule_AddObject(m, "NT_STATUS_RESUME_HIBERNATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RESUME_HIBERNATION)));
	PyModule_AddObject(m, "NT_STATUS_FIRMWARE_UPDATED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FIRMWARE_UPDATED)));
	PyModule_AddObject(m, "NT_STATUS_DRIVERS_LEAKING_LOCKED_PAGES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DRIVERS_LEAKING_LOCKED_PAGES)));
	PyModule_AddObject(m, "NT_STATUS_MESSAGE_RETRIEVED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MESSAGE_RETRIEVED)));
	PyModule_AddObject(m, "NT_STATUS_SYSTEM_POWERSTATE_TRANSITION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SYSTEM_POWERSTATE_TRANSITION)));
	PyModule_AddObject(m, "NT_STATUS_ALPC_CHECK_COMPLETION_LIST", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ALPC_CHECK_COMPLETION_LIST)));
	PyModule_AddObject(m, "NT_STATUS_SYSTEM_POWERSTATE_COMPLEX_TRANSITION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SYSTEM_POWERSTATE_COMPLEX_TRANSITION)));
	PyModule_AddObject(m, "NT_STATUS_ACCESS_AUDIT_BY_POLICY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACCESS_AUDIT_BY_POLICY)));
	PyModule_AddObject(m, "NT_STATUS_ABANDON_HIBERFILE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ABANDON_HIBERFILE)));
	PyModule_AddObject(m, "NT_STATUS_BIZRULES_NOT_ENABLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BIZRULES_NOT_ENABLED)));
	PyModule_AddObject(m, "NT_STATUS_WAKE_SYSTEM", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WAKE_SYSTEM)));
	PyModule_AddObject(m, "NT_STATUS_DS_SHUTTING_DOWN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_SHUTTING_DOWN)));
	PyModule_AddObject(m, "NT_STATUS_DBG_REPLY_LATER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DBG_REPLY_LATER)));
	PyModule_AddObject(m, "NT_STATUS_DBG_UNABLE_TO_PROVIDE_HANDLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DBG_UNABLE_TO_PROVIDE_HANDLE)));
	PyModule_AddObject(m, "NT_STATUS_DBG_TERMINATE_THREAD", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DBG_TERMINATE_THREAD)));
	PyModule_AddObject(m, "NT_STATUS_DBG_TERMINATE_PROCESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DBG_TERMINATE_PROCESS)));
	PyModule_AddObject(m, "NT_STATUS_DBG_CONTROL_C", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DBG_CONTROL_C)));
	PyModule_AddObject(m, "NT_STATUS_DBG_PRINTEXCEPTION_C", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DBG_PRINTEXCEPTION_C)));
	PyModule_AddObject(m, "NT_STATUS_DBG_RIPEXCEPTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DBG_RIPEXCEPTION)));
	PyModule_AddObject(m, "NT_STATUS_DBG_CONTROL_BREAK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DBG_CONTROL_BREAK)));
	PyModule_AddObject(m, "NT_STATUS_DBG_COMMAND_EXCEPTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DBG_COMMAND_EXCEPTION)));
	PyModule_AddObject(m, "NT_STATUS_RPC_UUID_LOCAL_ONLY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_UUID_LOCAL_ONLY)));
	PyModule_AddObject(m, "NT_STATUS_RPC_SEND_INCOMPLETE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_SEND_INCOMPLETE)));
	PyModule_AddObject(m, "NT_STATUS_CTX_CDM_CONNECT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_CDM_CONNECT)));
	PyModule_AddObject(m, "NT_STATUS_CTX_CDM_DISCONNECT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_CDM_DISCONNECT)));
	PyModule_AddObject(m, "NT_STATUS_SXS_RELEASE_ACTIVATION_CONTEXT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_RELEASE_ACTIVATION_CONTEXT)));
	PyModule_AddObject(m, "NT_STATUS_RECOVERY_NOT_NEEDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RECOVERY_NOT_NEEDED)));
	PyModule_AddObject(m, "NT_STATUS_RM_ALREADY_STARTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RM_ALREADY_STARTED)));
	PyModule_AddObject(m, "NT_STATUS_LOG_NO_RESTART", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_NO_RESTART)));
	PyModule_AddObject(m, "NT_STATUS_VIDEO_DRIVER_DEBUG_REPORT_REQUEST", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VIDEO_DRIVER_DEBUG_REPORT_REQUEST)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_PARTIAL_DATA_POPULATED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_PARTIAL_DATA_POPULATED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_DRIVER_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_DRIVER_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_MODE_NOT_PINNED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_MODE_NOT_PINNED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_NO_PREFERRED_MODE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_NO_PREFERRED_MODE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_DATASET_IS_EMPTY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_DATASET_IS_EMPTY)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_NO_MORE_ELEMENTS_IN_DATASET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_NO_MORE_ELEMENTS_IN_DATASET)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_PATH_CONTENT_GEOMETRY_TRANSFORMATION_NOT_PINNED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_PATH_CONTENT_GEOMETRY_TRANSFORMATION_NOT_PINNED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_UNKNOWN_CHILD_STATUS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_UNKNOWN_CHILD_STATUS)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_LEADLINK_START_DEFERRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_LEADLINK_START_DEFERRED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_POLLING_TOO_FREQUENTLY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_POLLING_TOO_FREQUENTLY)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_START_DEFERRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_START_DEFERRED)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_INDICATION_REQUIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_INDICATION_REQUIRED)));
	PyModule_AddObject(m, "NT_STATUS_GUARD_PAGE_VIOLATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GUARD_PAGE_VIOLATION)));
	PyModule_AddObject(m, "NT_STATUS_DATATYPE_MISALIGNMENT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DATATYPE_MISALIGNMENT)));
	PyModule_AddObject(m, "NT_STATUS_BREAKPOINT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BREAKPOINT)));
	PyModule_AddObject(m, "NT_STATUS_SINGLE_STEP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SINGLE_STEP)));
	PyModule_AddObject(m, "NT_STATUS_BUFFER_OVERFLOW", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BUFFER_OVERFLOW)));
	PyModule_AddObject(m, "NT_STATUS_NO_MORE_FILES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_MORE_FILES)));
	PyModule_AddObject(m, "NT_STATUS_WAKE_SYSTEM_DEBUGGER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WAKE_SYSTEM_DEBUGGER)));
	PyModule_AddObject(m, "NT_STATUS_HANDLES_CLOSED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_HANDLES_CLOSED)));
	PyModule_AddObject(m, "NT_STATUS_NO_INHERITANCE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_INHERITANCE)));
	PyModule_AddObject(m, "NT_STATUS_GUID_SUBSTITUTION_MADE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GUID_SUBSTITUTION_MADE)));
	PyModule_AddObject(m, "NT_STATUS_PARTIAL_COPY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PARTIAL_COPY)));
	PyModule_AddObject(m, "NT_STATUS_DEVICE_PAPER_EMPTY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DEVICE_PAPER_EMPTY)));
	PyModule_AddObject(m, "NT_STATUS_DEVICE_POWERED_OFF", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DEVICE_POWERED_OFF)));
	PyModule_AddObject(m, "NT_STATUS_DEVICE_OFF_LINE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DEVICE_OFF_LINE)));
	PyModule_AddObject(m, "NT_STATUS_DEVICE_BUSY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DEVICE_BUSY)));
	PyModule_AddObject(m, "NT_STATUS_NO_MORE_EAS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_MORE_EAS)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_EA_NAME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_EA_NAME)));
	PyModule_AddObject(m, "NT_STATUS_EA_LIST_INCONSISTENT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_EA_LIST_INCONSISTENT)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_EA_FLAG", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_EA_FLAG)));
	PyModule_AddObject(m, "NT_STATUS_VERIFY_REQUIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VERIFY_REQUIRED)));
	PyModule_AddObject(m, "NT_STATUS_EXTRANEOUS_INFORMATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_EXTRANEOUS_INFORMATION)));
	PyModule_AddObject(m, "NT_STATUS_RXACT_COMMIT_NECESSARY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RXACT_COMMIT_NECESSARY)));
	PyModule_AddObject(m, "NT_STATUS_NO_MORE_ENTRIES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_MORE_ENTRIES)));
	PyModule_AddObject(m, "NT_STATUS_FILEMARK_DETECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FILEMARK_DETECTED)));
	PyModule_AddObject(m, "NT_STATUS_MEDIA_CHANGED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MEDIA_CHANGED)));
	PyModule_AddObject(m, "NT_STATUS_BUS_RESET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BUS_RESET)));
	PyModule_AddObject(m, "NT_STATUS_END_OF_MEDIA", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_END_OF_MEDIA)));
	PyModule_AddObject(m, "NT_STATUS_BEGINNING_OF_MEDIA", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BEGINNING_OF_MEDIA)));
	PyModule_AddObject(m, "NT_STATUS_MEDIA_CHECK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MEDIA_CHECK)));
	PyModule_AddObject(m, "NT_STATUS_SETMARK_DETECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SETMARK_DETECTED)));
	PyModule_AddObject(m, "NT_STATUS_NO_DATA_DETECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_DATA_DETECTED)));
	PyModule_AddObject(m, "NT_STATUS_REDIRECTOR_HAS_OPEN_HANDLES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REDIRECTOR_HAS_OPEN_HANDLES)));
	PyModule_AddObject(m, "NT_STATUS_SERVER_HAS_OPEN_HANDLES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SERVER_HAS_OPEN_HANDLES)));
	PyModule_AddObject(m, "NT_STATUS_ALREADY_DISCONNECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ALREADY_DISCONNECTED)));
	PyModule_AddObject(m, "NT_STATUS_LONGJUMP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LONGJUMP)));
	PyModule_AddObject(m, "NT_STATUS_CLEANER_CARTRIDGE_INSTALLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLEANER_CARTRIDGE_INSTALLED)));
	PyModule_AddObject(m, "NT_STATUS_PLUGPLAY_QUERY_VETOED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PLUGPLAY_QUERY_VETOED)));
	PyModule_AddObject(m, "NT_STATUS_UNWIND_CONSOLIDATE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_UNWIND_CONSOLIDATE)));
	PyModule_AddObject(m, "NT_STATUS_REGISTRY_HIVE_RECOVERED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REGISTRY_HIVE_RECOVERED)));
	PyModule_AddObject(m, "NT_STATUS_DLL_MIGHT_BE_INSECURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DLL_MIGHT_BE_INSECURE)));
	PyModule_AddObject(m, "NT_STATUS_DLL_MIGHT_BE_INCOMPATIBLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DLL_MIGHT_BE_INCOMPATIBLE)));
	PyModule_AddObject(m, "NT_STATUS_STOPPED_ON_SYMLINK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_STOPPED_ON_SYMLINK)));
	PyModule_AddObject(m, "NT_STATUS_DEVICE_REQUIRES_CLEANING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DEVICE_REQUIRES_CLEANING)));
	PyModule_AddObject(m, "NT_STATUS_DEVICE_DOOR_OPEN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DEVICE_DOOR_OPEN)));
	PyModule_AddObject(m, "NT_STATUS_DATA_LOST_REPAIR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DATA_LOST_REPAIR)));
	PyModule_AddObject(m, "NT_STATUS_DBG_EXCEPTION_NOT_HANDLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DBG_EXCEPTION_NOT_HANDLED)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_NODE_ALREADY_UP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_NODE_ALREADY_UP)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_NODE_ALREADY_DOWN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_NODE_ALREADY_DOWN)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_NETWORK_ALREADY_ONLINE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_NETWORK_ALREADY_ONLINE)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_NETWORK_ALREADY_OFFLINE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_NETWORK_ALREADY_OFFLINE)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_NODE_ALREADY_MEMBER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_NODE_ALREADY_MEMBER)));
	PyModule_AddObject(m, "NT_STATUS_COULD_NOT_RESIZE_LOG", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_COULD_NOT_RESIZE_LOG)));
	PyModule_AddObject(m, "NT_STATUS_NO_TXF_METADATA", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_TXF_METADATA)));
	PyModule_AddObject(m, "NT_STATUS_CANT_RECOVER_WITH_HANDLE_OPEN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CANT_RECOVER_WITH_HANDLE_OPEN)));
	PyModule_AddObject(m, "NT_STATUS_TXF_METADATA_ALREADY_PRESENT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TXF_METADATA_ALREADY_PRESENT)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_SCOPE_CALLBACKS_NOT_SET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_SCOPE_CALLBACKS_NOT_SET)));
	PyModule_AddObject(m, "NT_STATUS_VIDEO_HUNG_DISPLAY_DRIVER_THREAD_RECOVERED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VIDEO_HUNG_DISPLAY_DRIVER_THREAD_RECOVERED)));
	PyModule_AddObject(m, "NT_STATUS_FLT_BUFFER_TOO_SMALL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_BUFFER_TOO_SMALL)));
	PyModule_AddObject(m, "NT_STATUS_FVE_PARTIAL_METADATA", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_PARTIAL_METADATA)));
	PyModule_AddObject(m, "NT_STATUS_FVE_TRANSIENT_STATE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_TRANSIENT_STATE)));
	PyModule_AddObject(m, "NT_STATUS_UNSUCCESSFUL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_UNSUCCESSFUL)));
	PyModule_AddObject(m, "NT_STATUS_NOT_IMPLEMENTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOT_IMPLEMENTED)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_INFO_CLASS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_INFO_CLASS)));
	PyModule_AddObject(m, "NT_STATUS_INFO_LENGTH_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INFO_LENGTH_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_ACCESS_VIOLATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACCESS_VIOLATION)));
	PyModule_AddObject(m, "NT_STATUS_IN_PAGE_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IN_PAGE_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_PAGEFILE_QUOTA", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PAGEFILE_QUOTA)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_HANDLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_HANDLE)));
	PyModule_AddObject(m, "NT_STATUS_BAD_INITIAL_STACK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BAD_INITIAL_STACK)));
	PyModule_AddObject(m, "NT_STATUS_BAD_INITIAL_PC", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BAD_INITIAL_PC)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_CID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_CID)));
	PyModule_AddObject(m, "NT_STATUS_TIMER_NOT_CANCELED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TIMER_NOT_CANCELED)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_PARAMETER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_PARAMETER)));
	PyModule_AddObject(m, "NT_STATUS_NO_SUCH_DEVICE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_SUCH_DEVICE)));
	PyModule_AddObject(m, "NT_STATUS_NO_SUCH_FILE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_SUCH_FILE)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_DEVICE_REQUEST", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_DEVICE_REQUEST)));
	PyModule_AddObject(m, "NT_STATUS_END_OF_FILE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_END_OF_FILE)));
	PyModule_AddObject(m, "NT_STATUS_WRONG_VOLUME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WRONG_VOLUME)));
	PyModule_AddObject(m, "NT_STATUS_NO_MEDIA_IN_DEVICE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_MEDIA_IN_DEVICE)));
	PyModule_AddObject(m, "NT_STATUS_UNRECOGNIZED_MEDIA", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_UNRECOGNIZED_MEDIA)));
	PyModule_AddObject(m, "NT_STATUS_NONEXISTENT_SECTOR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NONEXISTENT_SECTOR)));
	PyModule_AddObject(m, "NT_STATUS_MORE_PROCESSING_REQUIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MORE_PROCESSING_REQUIRED)));
	PyModule_AddObject(m, "NT_STATUS_NO_MEMORY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_MEMORY)));
	PyModule_AddObject(m, "NT_STATUS_CONFLICTING_ADDRESSES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CONFLICTING_ADDRESSES)));
	PyModule_AddObject(m, "NT_STATUS_NOT_MAPPED_VIEW", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOT_MAPPED_VIEW)));
	PyModule_AddObject(m, "NT_STATUS_UNABLE_TO_FREE_VM", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_UNABLE_TO_FREE_VM)));
	PyModule_AddObject(m, "NT_STATUS_UNABLE_TO_DELETE_SECTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_UNABLE_TO_DELETE_SECTION)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_SYSTEM_SERVICE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_SYSTEM_SERVICE)));
	PyModule_AddObject(m, "NT_STATUS_ILLEGAL_INSTRUCTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ILLEGAL_INSTRUCTION)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_LOCK_SEQUENCE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_LOCK_SEQUENCE)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_VIEW_SIZE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_VIEW_SIZE)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_FILE_FOR_SECTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_FILE_FOR_SECTION)));
	PyModule_AddObject(m, "NT_STATUS_ALREADY_COMMITTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ALREADY_COMMITTED)));
	PyModule_AddObject(m, "NT_STATUS_ACCESS_DENIED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACCESS_DENIED)));
	PyModule_AddObject(m, "NT_STATUS_BUFFER_TOO_SMALL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BUFFER_TOO_SMALL)));
	PyModule_AddObject(m, "NT_STATUS_OBJECT_TYPE_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_OBJECT_TYPE_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_NONCONTINUABLE_EXCEPTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NONCONTINUABLE_EXCEPTION)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_DISPOSITION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_DISPOSITION)));
	PyModule_AddObject(m, "NT_STATUS_UNWIND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_UNWIND)));
	PyModule_AddObject(m, "NT_STATUS_BAD_STACK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BAD_STACK)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_UNWIND_TARGET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_UNWIND_TARGET)));
	PyModule_AddObject(m, "NT_STATUS_NOT_LOCKED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOT_LOCKED)));
	PyModule_AddObject(m, "NT_STATUS_PARITY_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PARITY_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_UNABLE_TO_DECOMMIT_VM", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_UNABLE_TO_DECOMMIT_VM)));
	PyModule_AddObject(m, "NT_STATUS_NOT_COMMITTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOT_COMMITTED)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_PORT_ATTRIBUTES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_PORT_ATTRIBUTES)));
	PyModule_AddObject(m, "NT_STATUS_PORT_MESSAGE_TOO_LONG", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PORT_MESSAGE_TOO_LONG)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_PARAMETER_MIX", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_PARAMETER_MIX)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_QUOTA_LOWER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_QUOTA_LOWER)));
	PyModule_AddObject(m, "NT_STATUS_DISK_CORRUPT_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DISK_CORRUPT_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_OBJECT_NAME_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_OBJECT_NAME_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_OBJECT_NAME_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_OBJECT_NAME_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_OBJECT_NAME_COLLISION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_OBJECT_NAME_COLLISION)));
	PyModule_AddObject(m, "NT_STATUS_PORT_DISCONNECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PORT_DISCONNECTED)));
	PyModule_AddObject(m, "NT_STATUS_DEVICE_ALREADY_ATTACHED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DEVICE_ALREADY_ATTACHED)));
	PyModule_AddObject(m, "NT_STATUS_OBJECT_PATH_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_OBJECT_PATH_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_OBJECT_PATH_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_OBJECT_PATH_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_OBJECT_PATH_SYNTAX_BAD", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_OBJECT_PATH_SYNTAX_BAD)));
	PyModule_AddObject(m, "NT_STATUS_DATA_OVERRUN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DATA_OVERRUN)));
	PyModule_AddObject(m, "NT_STATUS_DATA_LATE_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DATA_LATE_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_DATA_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DATA_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_CRC_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CRC_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_SECTION_TOO_BIG", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SECTION_TOO_BIG)));
	PyModule_AddObject(m, "NT_STATUS_PORT_CONNECTION_REFUSED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PORT_CONNECTION_REFUSED)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_PORT_HANDLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_PORT_HANDLE)));
	PyModule_AddObject(m, "NT_STATUS_SHARING_VIOLATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SHARING_VIOLATION)));
	PyModule_AddObject(m, "NT_STATUS_QUOTA_EXCEEDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_QUOTA_EXCEEDED)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_PAGE_PROTECTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_PAGE_PROTECTION)));
	PyModule_AddObject(m, "NT_STATUS_MUTANT_NOT_OWNED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MUTANT_NOT_OWNED)));
	PyModule_AddObject(m, "NT_STATUS_SEMAPHORE_LIMIT_EXCEEDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SEMAPHORE_LIMIT_EXCEEDED)));
	PyModule_AddObject(m, "NT_STATUS_PORT_ALREADY_SET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PORT_ALREADY_SET)));
	PyModule_AddObject(m, "NT_STATUS_SECTION_NOT_IMAGE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SECTION_NOT_IMAGE)));
	PyModule_AddObject(m, "NT_STATUS_SUSPEND_COUNT_EXCEEDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SUSPEND_COUNT_EXCEEDED)));
	PyModule_AddObject(m, "NT_STATUS_THREAD_IS_TERMINATING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_THREAD_IS_TERMINATING)));
	PyModule_AddObject(m, "NT_STATUS_BAD_WORKING_SET_LIMIT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BAD_WORKING_SET_LIMIT)));
	PyModule_AddObject(m, "NT_STATUS_INCOMPATIBLE_FILE_MAP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INCOMPATIBLE_FILE_MAP)));
	PyModule_AddObject(m, "NT_STATUS_SECTION_PROTECTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SECTION_PROTECTION)));
	PyModule_AddObject(m, "NT_STATUS_EAS_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_EAS_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_EA_TOO_LARGE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_EA_TOO_LARGE)));
	PyModule_AddObject(m, "NT_STATUS_NONEXISTENT_EA_ENTRY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NONEXISTENT_EA_ENTRY)));
	PyModule_AddObject(m, "NT_STATUS_NO_EAS_ON_FILE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_EAS_ON_FILE)));
	PyModule_AddObject(m, "NT_STATUS_EA_CORRUPT_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_EA_CORRUPT_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_FILE_LOCK_CONFLICT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FILE_LOCK_CONFLICT)));
	PyModule_AddObject(m, "NT_STATUS_LOCK_NOT_GRANTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOCK_NOT_GRANTED)));
	PyModule_AddObject(m, "NT_STATUS_DELETE_PENDING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DELETE_PENDING)));
	PyModule_AddObject(m, "NT_STATUS_CTL_FILE_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTL_FILE_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_UNKNOWN_REVISION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_UNKNOWN_REVISION)));
	PyModule_AddObject(m, "NT_STATUS_REVISION_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REVISION_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_OWNER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_OWNER)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_PRIMARY_GROUP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_PRIMARY_GROUP)));
	PyModule_AddObject(m, "NT_STATUS_NO_IMPERSONATION_TOKEN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_IMPERSONATION_TOKEN)));
	PyModule_AddObject(m, "NT_STATUS_CANT_DISABLE_MANDATORY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CANT_DISABLE_MANDATORY)));
	PyModule_AddObject(m, "NT_STATUS_NO_LOGON_SERVERS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_LOGON_SERVERS)));
	PyModule_AddObject(m, "NT_STATUS_NO_SUCH_LOGON_SESSION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_SUCH_LOGON_SESSION)));
	PyModule_AddObject(m, "NT_STATUS_NO_SUCH_PRIVILEGE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_SUCH_PRIVILEGE)));
	PyModule_AddObject(m, "NT_STATUS_PRIVILEGE_NOT_HELD", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PRIVILEGE_NOT_HELD)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_ACCOUNT_NAME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_ACCOUNT_NAME)));
	PyModule_AddObject(m, "NT_STATUS_USER_EXISTS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_USER_EXISTS)));
	PyModule_AddObject(m, "NT_STATUS_NO_SUCH_USER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_SUCH_USER)));
	PyModule_AddObject(m, "NT_STATUS_GROUP_EXISTS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GROUP_EXISTS)));
	PyModule_AddObject(m, "NT_STATUS_NO_SUCH_GROUP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_SUCH_GROUP)));
	PyModule_AddObject(m, "NT_STATUS_MEMBER_IN_GROUP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MEMBER_IN_GROUP)));
	PyModule_AddObject(m, "NT_STATUS_MEMBER_NOT_IN_GROUP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MEMBER_NOT_IN_GROUP)));
	PyModule_AddObject(m, "NT_STATUS_LAST_ADMIN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LAST_ADMIN)));
	PyModule_AddObject(m, "NT_STATUS_WRONG_PASSWORD", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WRONG_PASSWORD)));
	PyModule_AddObject(m, "NT_STATUS_ILL_FORMED_PASSWORD", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ILL_FORMED_PASSWORD)));
	PyModule_AddObject(m, "NT_STATUS_PASSWORD_RESTRICTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PASSWORD_RESTRICTION)));
	PyModule_AddObject(m, "NT_STATUS_LOGON_FAILURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOGON_FAILURE)));
	PyModule_AddObject(m, "NT_STATUS_ACCOUNT_RESTRICTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACCOUNT_RESTRICTION)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_LOGON_HOURS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_LOGON_HOURS)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_WORKSTATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_WORKSTATION)));
	PyModule_AddObject(m, "NT_STATUS_PASSWORD_EXPIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PASSWORD_EXPIRED)));
	PyModule_AddObject(m, "NT_STATUS_ACCOUNT_DISABLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACCOUNT_DISABLED)));
	PyModule_AddObject(m, "NT_STATUS_NONE_MAPPED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NONE_MAPPED)));
	PyModule_AddObject(m, "NT_STATUS_TOO_MANY_LUIDS_REQUESTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TOO_MANY_LUIDS_REQUESTED)));
	PyModule_AddObject(m, "NT_STATUS_LUIDS_EXHAUSTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LUIDS_EXHAUSTED)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_SUB_AUTHORITY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_SUB_AUTHORITY)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_ACL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_ACL)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_SID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_SID)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_SECURITY_DESCR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_SECURITY_DESCR)));
	PyModule_AddObject(m, "NT_STATUS_PROCEDURE_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PROCEDURE_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_IMAGE_FORMAT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_IMAGE_FORMAT)));
	PyModule_AddObject(m, "NT_STATUS_NO_TOKEN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_TOKEN)));
	PyModule_AddObject(m, "NT_STATUS_BAD_INHERITANCE_ACL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BAD_INHERITANCE_ACL)));
	PyModule_AddObject(m, "NT_STATUS_RANGE_NOT_LOCKED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RANGE_NOT_LOCKED)));
	PyModule_AddObject(m, "NT_STATUS_DISK_FULL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DISK_FULL)));
	PyModule_AddObject(m, "NT_STATUS_SERVER_DISABLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SERVER_DISABLED)));
	PyModule_AddObject(m, "NT_STATUS_SERVER_NOT_DISABLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SERVER_NOT_DISABLED)));
	PyModule_AddObject(m, "NT_STATUS_TOO_MANY_GUIDS_REQUESTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TOO_MANY_GUIDS_REQUESTED)));
	PyModule_AddObject(m, "NT_STATUS_GUIDS_EXHAUSTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GUIDS_EXHAUSTED)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_ID_AUTHORITY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_ID_AUTHORITY)));
	PyModule_AddObject(m, "NT_STATUS_AGENTS_EXHAUSTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_AGENTS_EXHAUSTED)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_VOLUME_LABEL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_VOLUME_LABEL)));
	PyModule_AddObject(m, "NT_STATUS_SECTION_NOT_EXTENDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SECTION_NOT_EXTENDED)));
	PyModule_AddObject(m, "NT_STATUS_NOT_MAPPED_DATA", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOT_MAPPED_DATA)));
	PyModule_AddObject(m, "NT_STATUS_RESOURCE_DATA_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RESOURCE_DATA_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_RESOURCE_TYPE_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RESOURCE_TYPE_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_RESOURCE_NAME_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RESOURCE_NAME_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_ARRAY_BOUNDS_EXCEEDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ARRAY_BOUNDS_EXCEEDED)));
	PyModule_AddObject(m, "NT_STATUS_FLOAT_DENORMAL_OPERAND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLOAT_DENORMAL_OPERAND)));
	PyModule_AddObject(m, "NT_STATUS_FLOAT_DIVIDE_BY_ZERO", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLOAT_DIVIDE_BY_ZERO)));
	PyModule_AddObject(m, "NT_STATUS_FLOAT_INEXACT_RESULT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLOAT_INEXACT_RESULT)));
	PyModule_AddObject(m, "NT_STATUS_FLOAT_INVALID_OPERATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLOAT_INVALID_OPERATION)));
	PyModule_AddObject(m, "NT_STATUS_FLOAT_OVERFLOW", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLOAT_OVERFLOW)));
	PyModule_AddObject(m, "NT_STATUS_FLOAT_STACK_CHECK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLOAT_STACK_CHECK)));
	PyModule_AddObject(m, "NT_STATUS_FLOAT_UNDERFLOW", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLOAT_UNDERFLOW)));
	PyModule_AddObject(m, "NT_STATUS_INTEGER_DIVIDE_BY_ZERO", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INTEGER_DIVIDE_BY_ZERO)));
	PyModule_AddObject(m, "NT_STATUS_INTEGER_OVERFLOW", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INTEGER_OVERFLOW)));
	PyModule_AddObject(m, "NT_STATUS_PRIVILEGED_INSTRUCTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PRIVILEGED_INSTRUCTION)));
	PyModule_AddObject(m, "NT_STATUS_TOO_MANY_PAGING_FILES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TOO_MANY_PAGING_FILES)));
	PyModule_AddObject(m, "NT_STATUS_FILE_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FILE_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_ALLOTTED_SPACE_EXCEEDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ALLOTTED_SPACE_EXCEEDED)));
	PyModule_AddObject(m, "NT_STATUS_INSUFFICIENT_RESOURCES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INSUFFICIENT_RESOURCES)));
	PyModule_AddObject(m, "NT_STATUS_DFS_EXIT_PATH_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DFS_EXIT_PATH_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_DEVICE_DATA_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DEVICE_DATA_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_DEVICE_NOT_CONNECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DEVICE_NOT_CONNECTED)));
	PyModule_AddObject(m, "NT_STATUS_FREE_VM_NOT_AT_BASE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FREE_VM_NOT_AT_BASE)));
	PyModule_AddObject(m, "NT_STATUS_MEMORY_NOT_ALLOCATED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MEMORY_NOT_ALLOCATED)));
	PyModule_AddObject(m, "NT_STATUS_WORKING_SET_QUOTA", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WORKING_SET_QUOTA)));
	PyModule_AddObject(m, "NT_STATUS_MEDIA_WRITE_PROTECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MEDIA_WRITE_PROTECTED)));
	PyModule_AddObject(m, "NT_STATUS_DEVICE_NOT_READY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DEVICE_NOT_READY)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_GROUP_ATTRIBUTES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_GROUP_ATTRIBUTES)));
	PyModule_AddObject(m, "NT_STATUS_BAD_IMPERSONATION_LEVEL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BAD_IMPERSONATION_LEVEL)));
	PyModule_AddObject(m, "NT_STATUS_CANT_OPEN_ANONYMOUS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CANT_OPEN_ANONYMOUS)));
	PyModule_AddObject(m, "NT_STATUS_BAD_VALIDATION_CLASS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BAD_VALIDATION_CLASS)));
	PyModule_AddObject(m, "NT_STATUS_BAD_TOKEN_TYPE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BAD_TOKEN_TYPE)));
	PyModule_AddObject(m, "NT_STATUS_BAD_MASTER_BOOT_RECORD", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BAD_MASTER_BOOT_RECORD)));
	PyModule_AddObject(m, "NT_STATUS_INSTRUCTION_MISALIGNMENT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INSTRUCTION_MISALIGNMENT)));
	PyModule_AddObject(m, "NT_STATUS_INSTANCE_NOT_AVAILABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INSTANCE_NOT_AVAILABLE)));
	PyModule_AddObject(m, "NT_STATUS_PIPE_NOT_AVAILABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PIPE_NOT_AVAILABLE)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_PIPE_STATE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_PIPE_STATE)));
	PyModule_AddObject(m, "NT_STATUS_PIPE_BUSY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PIPE_BUSY)));
	PyModule_AddObject(m, "NT_STATUS_ILLEGAL_FUNCTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ILLEGAL_FUNCTION)));
	PyModule_AddObject(m, "NT_STATUS_PIPE_DISCONNECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PIPE_DISCONNECTED)));
	PyModule_AddObject(m, "NT_STATUS_PIPE_CLOSING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PIPE_CLOSING)));
	PyModule_AddObject(m, "NT_STATUS_PIPE_CONNECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PIPE_CONNECTED)));
	PyModule_AddObject(m, "NT_STATUS_PIPE_LISTENING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PIPE_LISTENING)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_READ_MODE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_READ_MODE)));
	PyModule_AddObject(m, "NT_STATUS_IO_TIMEOUT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IO_TIMEOUT)));
	PyModule_AddObject(m, "NT_STATUS_FILE_FORCED_CLOSED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FILE_FORCED_CLOSED)));
	PyModule_AddObject(m, "NT_STATUS_PROFILING_NOT_STARTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PROFILING_NOT_STARTED)));
	PyModule_AddObject(m, "NT_STATUS_PROFILING_NOT_STOPPED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PROFILING_NOT_STOPPED)));
	PyModule_AddObject(m, "NT_STATUS_COULD_NOT_INTERPRET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_COULD_NOT_INTERPRET)));
	PyModule_AddObject(m, "NT_STATUS_FILE_IS_A_DIRECTORY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FILE_IS_A_DIRECTORY)));
	PyModule_AddObject(m, "NT_STATUS_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_REMOTE_NOT_LISTENING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REMOTE_NOT_LISTENING)));
	PyModule_AddObject(m, "NT_STATUS_DUPLICATE_NAME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DUPLICATE_NAME)));
	PyModule_AddObject(m, "NT_STATUS_BAD_NETWORK_PATH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BAD_NETWORK_PATH)));
	PyModule_AddObject(m, "NT_STATUS_NETWORK_BUSY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NETWORK_BUSY)));
	PyModule_AddObject(m, "NT_STATUS_DEVICE_DOES_NOT_EXIST", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DEVICE_DOES_NOT_EXIST)));
	PyModule_AddObject(m, "NT_STATUS_TOO_MANY_COMMANDS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TOO_MANY_COMMANDS)));
	PyModule_AddObject(m, "NT_STATUS_ADAPTER_HARDWARE_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ADAPTER_HARDWARE_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_NETWORK_RESPONSE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_NETWORK_RESPONSE)));
	PyModule_AddObject(m, "NT_STATUS_UNEXPECTED_NETWORK_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_UNEXPECTED_NETWORK_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_BAD_REMOTE_ADAPTER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BAD_REMOTE_ADAPTER)));
	PyModule_AddObject(m, "NT_STATUS_PRINT_QUEUE_FULL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PRINT_QUEUE_FULL)));
	PyModule_AddObject(m, "NT_STATUS_NO_SPOOL_SPACE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_SPOOL_SPACE)));
	PyModule_AddObject(m, "NT_STATUS_PRINT_CANCELLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PRINT_CANCELLED)));
	PyModule_AddObject(m, "NT_STATUS_NETWORK_NAME_DELETED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NETWORK_NAME_DELETED)));
	PyModule_AddObject(m, "NT_STATUS_NETWORK_ACCESS_DENIED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NETWORK_ACCESS_DENIED)));
	PyModule_AddObject(m, "NT_STATUS_BAD_DEVICE_TYPE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BAD_DEVICE_TYPE)));
	PyModule_AddObject(m, "NT_STATUS_BAD_NETWORK_NAME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BAD_NETWORK_NAME)));
	PyModule_AddObject(m, "NT_STATUS_TOO_MANY_NAMES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TOO_MANY_NAMES)));
	PyModule_AddObject(m, "NT_STATUS_TOO_MANY_SESSIONS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TOO_MANY_SESSIONS)));
	PyModule_AddObject(m, "NT_STATUS_SHARING_PAUSED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SHARING_PAUSED)));
	PyModule_AddObject(m, "NT_STATUS_REQUEST_NOT_ACCEPTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REQUEST_NOT_ACCEPTED)));
	PyModule_AddObject(m, "NT_STATUS_REDIRECTOR_PAUSED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REDIRECTOR_PAUSED)));
	PyModule_AddObject(m, "NT_STATUS_NET_WRITE_FAULT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NET_WRITE_FAULT)));
	PyModule_AddObject(m, "NT_STATUS_PROFILING_AT_LIMIT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PROFILING_AT_LIMIT)));
	PyModule_AddObject(m, "NT_STATUS_NOT_SAME_DEVICE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOT_SAME_DEVICE)));
	PyModule_AddObject(m, "NT_STATUS_FILE_RENAMED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FILE_RENAMED)));
	PyModule_AddObject(m, "NT_STATUS_VIRTUAL_CIRCUIT_CLOSED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VIRTUAL_CIRCUIT_CLOSED)));
	PyModule_AddObject(m, "NT_STATUS_NO_SECURITY_ON_OBJECT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_SECURITY_ON_OBJECT)));
	PyModule_AddObject(m, "NT_STATUS_CANT_WAIT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CANT_WAIT)));
	PyModule_AddObject(m, "NT_STATUS_PIPE_EMPTY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PIPE_EMPTY)));
	PyModule_AddObject(m, "NT_STATUS_CANT_ACCESS_DOMAIN_INFO", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CANT_ACCESS_DOMAIN_INFO)));
	PyModule_AddObject(m, "NT_STATUS_CANT_TERMINATE_SELF", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CANT_TERMINATE_SELF)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_SERVER_STATE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_SERVER_STATE)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_DOMAIN_STATE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_DOMAIN_STATE)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_DOMAIN_ROLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_DOMAIN_ROLE)));
	PyModule_AddObject(m, "NT_STATUS_NO_SUCH_DOMAIN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_SUCH_DOMAIN)));
	PyModule_AddObject(m, "NT_STATUS_DOMAIN_EXISTS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DOMAIN_EXISTS)));
	PyModule_AddObject(m, "NT_STATUS_DOMAIN_LIMIT_EXCEEDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DOMAIN_LIMIT_EXCEEDED)));
	PyModule_AddObject(m, "NT_STATUS_OPLOCK_NOT_GRANTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_OPLOCK_NOT_GRANTED)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_OPLOCK_PROTOCOL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_OPLOCK_PROTOCOL)));
	PyModule_AddObject(m, "NT_STATUS_INTERNAL_DB_CORRUPTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INTERNAL_DB_CORRUPTION)));
	PyModule_AddObject(m, "NT_STATUS_INTERNAL_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INTERNAL_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_GENERIC_NOT_MAPPED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GENERIC_NOT_MAPPED)));
	PyModule_AddObject(m, "NT_STATUS_BAD_DESCRIPTOR_FORMAT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BAD_DESCRIPTOR_FORMAT)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_USER_BUFFER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_USER_BUFFER)));
	PyModule_AddObject(m, "NT_STATUS_UNEXPECTED_IO_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_UNEXPECTED_IO_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_UNEXPECTED_MM_CREATE_ERR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_UNEXPECTED_MM_CREATE_ERR)));
	PyModule_AddObject(m, "NT_STATUS_UNEXPECTED_MM_MAP_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_UNEXPECTED_MM_MAP_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_UNEXPECTED_MM_EXTEND_ERR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_UNEXPECTED_MM_EXTEND_ERR)));
	PyModule_AddObject(m, "NT_STATUS_NOT_LOGON_PROCESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOT_LOGON_PROCESS)));
	PyModule_AddObject(m, "NT_STATUS_LOGON_SESSION_EXISTS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOGON_SESSION_EXISTS)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_PARAMETER_1", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_PARAMETER_1)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_PARAMETER_2", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_PARAMETER_2)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_PARAMETER_3", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_PARAMETER_3)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_PARAMETER_4", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_PARAMETER_4)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_PARAMETER_5", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_PARAMETER_5)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_PARAMETER_6", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_PARAMETER_6)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_PARAMETER_7", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_PARAMETER_7)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_PARAMETER_8", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_PARAMETER_8)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_PARAMETER_9", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_PARAMETER_9)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_PARAMETER_10", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_PARAMETER_10)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_PARAMETER_11", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_PARAMETER_11)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_PARAMETER_12", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_PARAMETER_12)));
	PyModule_AddObject(m, "NT_STATUS_REDIRECTOR_NOT_STARTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REDIRECTOR_NOT_STARTED)));
	PyModule_AddObject(m, "NT_STATUS_REDIRECTOR_STARTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REDIRECTOR_STARTED)));
	PyModule_AddObject(m, "NT_STATUS_STACK_OVERFLOW", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_STACK_OVERFLOW)));
	PyModule_AddObject(m, "NT_STATUS_NO_SUCH_PACKAGE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_SUCH_PACKAGE)));
	PyModule_AddObject(m, "NT_STATUS_BAD_FUNCTION_TABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BAD_FUNCTION_TABLE)));
	PyModule_AddObject(m, "NT_STATUS_VARIABLE_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VARIABLE_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_DIRECTORY_NOT_EMPTY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DIRECTORY_NOT_EMPTY)));
	PyModule_AddObject(m, "NT_STATUS_FILE_CORRUPT_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FILE_CORRUPT_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_NOT_A_DIRECTORY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOT_A_DIRECTORY)));
	PyModule_AddObject(m, "NT_STATUS_BAD_LOGON_SESSION_STATE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BAD_LOGON_SESSION_STATE)));
	PyModule_AddObject(m, "NT_STATUS_LOGON_SESSION_COLLISION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOGON_SESSION_COLLISION)));
	PyModule_AddObject(m, "NT_STATUS_NAME_TOO_LONG", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NAME_TOO_LONG)));
	PyModule_AddObject(m, "NT_STATUS_FILES_OPEN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FILES_OPEN)));
	PyModule_AddObject(m, "NT_STATUS_CONNECTION_IN_USE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CONNECTION_IN_USE)));
	PyModule_AddObject(m, "NT_STATUS_MESSAGE_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MESSAGE_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_PROCESS_IS_TERMINATING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PROCESS_IS_TERMINATING)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_LOGON_TYPE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_LOGON_TYPE)));
	PyModule_AddObject(m, "NT_STATUS_NO_GUID_TRANSLATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_GUID_TRANSLATION)));
	PyModule_AddObject(m, "NT_STATUS_CANNOT_IMPERSONATE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CANNOT_IMPERSONATE)));
	PyModule_AddObject(m, "NT_STATUS_IMAGE_ALREADY_LOADED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IMAGE_ALREADY_LOADED)));
	PyModule_AddObject(m, "NT_STATUS_NO_LDT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_LDT)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_LDT_SIZE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_LDT_SIZE)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_LDT_OFFSET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_LDT_OFFSET)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_LDT_DESCRIPTOR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_LDT_DESCRIPTOR)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_IMAGE_NE_FORMAT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_IMAGE_NE_FORMAT)));
	PyModule_AddObject(m, "NT_STATUS_RXACT_INVALID_STATE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RXACT_INVALID_STATE)));
	PyModule_AddObject(m, "NT_STATUS_RXACT_COMMIT_FAILURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RXACT_COMMIT_FAILURE)));
	PyModule_AddObject(m, "NT_STATUS_MAPPED_FILE_SIZE_ZERO", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MAPPED_FILE_SIZE_ZERO)));
	PyModule_AddObject(m, "NT_STATUS_TOO_MANY_OPENED_FILES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TOO_MANY_OPENED_FILES)));
	PyModule_AddObject(m, "NT_STATUS_CANCELLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CANCELLED)));
	PyModule_AddObject(m, "NT_STATUS_CANNOT_DELETE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CANNOT_DELETE)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_COMPUTER_NAME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_COMPUTER_NAME)));
	PyModule_AddObject(m, "NT_STATUS_FILE_DELETED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FILE_DELETED)));
	PyModule_AddObject(m, "NT_STATUS_SPECIAL_ACCOUNT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SPECIAL_ACCOUNT)));
	PyModule_AddObject(m, "NT_STATUS_SPECIAL_GROUP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SPECIAL_GROUP)));
	PyModule_AddObject(m, "NT_STATUS_SPECIAL_USER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SPECIAL_USER)));
	PyModule_AddObject(m, "NT_STATUS_MEMBERS_PRIMARY_GROUP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MEMBERS_PRIMARY_GROUP)));
	PyModule_AddObject(m, "NT_STATUS_FILE_CLOSED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FILE_CLOSED)));
	PyModule_AddObject(m, "NT_STATUS_TOO_MANY_THREADS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TOO_MANY_THREADS)));
	PyModule_AddObject(m, "NT_STATUS_THREAD_NOT_IN_PROCESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_THREAD_NOT_IN_PROCESS)));
	PyModule_AddObject(m, "NT_STATUS_TOKEN_ALREADY_IN_USE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TOKEN_ALREADY_IN_USE)));
	PyModule_AddObject(m, "NT_STATUS_PAGEFILE_QUOTA_EXCEEDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PAGEFILE_QUOTA_EXCEEDED)));
	PyModule_AddObject(m, "NT_STATUS_COMMITMENT_LIMIT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_COMMITMENT_LIMIT)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_IMAGE_LE_FORMAT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_IMAGE_LE_FORMAT)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_IMAGE_NOT_MZ", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_IMAGE_NOT_MZ)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_IMAGE_PROTECT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_IMAGE_PROTECT)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_IMAGE_WIN_16", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_IMAGE_WIN_16)));
	PyModule_AddObject(m, "NT_STATUS_LOGON_SERVER_CONFLICT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOGON_SERVER_CONFLICT)));
	PyModule_AddObject(m, "NT_STATUS_TIME_DIFFERENCE_AT_DC", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TIME_DIFFERENCE_AT_DC)));
	PyModule_AddObject(m, "NT_STATUS_SYNCHRONIZATION_REQUIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SYNCHRONIZATION_REQUIRED)));
	PyModule_AddObject(m, "NT_STATUS_DLL_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DLL_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_OPEN_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_OPEN_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_IO_PRIVILEGE_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IO_PRIVILEGE_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_ORDINAL_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ORDINAL_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_ENTRYPOINT_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ENTRYPOINT_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_CONTROL_C_EXIT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CONTROL_C_EXIT)));
	PyModule_AddObject(m, "NT_STATUS_LOCAL_DISCONNECT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOCAL_DISCONNECT)));
	PyModule_AddObject(m, "NT_STATUS_REMOTE_DISCONNECT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REMOTE_DISCONNECT)));
	PyModule_AddObject(m, "NT_STATUS_REMOTE_RESOURCES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REMOTE_RESOURCES)));
	PyModule_AddObject(m, "NT_STATUS_LINK_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LINK_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_LINK_TIMEOUT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LINK_TIMEOUT)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_CONNECTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_CONNECTION)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_ADDRESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_ADDRESS)));
	PyModule_AddObject(m, "NT_STATUS_DLL_INIT_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DLL_INIT_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_MISSING_SYSTEMFILE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MISSING_SYSTEMFILE)));
	PyModule_AddObject(m, "NT_STATUS_UNHANDLED_EXCEPTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_UNHANDLED_EXCEPTION)));
	PyModule_AddObject(m, "NT_STATUS_APP_INIT_FAILURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_APP_INIT_FAILURE)));
	PyModule_AddObject(m, "NT_STATUS_PAGEFILE_CREATE_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PAGEFILE_CREATE_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_NO_PAGEFILE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_PAGEFILE)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_LEVEL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_LEVEL)));
	PyModule_AddObject(m, "NT_STATUS_WRONG_PASSWORD_CORE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WRONG_PASSWORD_CORE)));
	PyModule_AddObject(m, "NT_STATUS_ILLEGAL_FLOAT_CONTEXT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ILLEGAL_FLOAT_CONTEXT)));
	PyModule_AddObject(m, "NT_STATUS_PIPE_BROKEN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PIPE_BROKEN)));
	PyModule_AddObject(m, "NT_STATUS_REGISTRY_CORRUPT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REGISTRY_CORRUPT)));
	PyModule_AddObject(m, "NT_STATUS_REGISTRY_IO_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REGISTRY_IO_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_NO_EVENT_PAIR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_EVENT_PAIR)));
	PyModule_AddObject(m, "NT_STATUS_UNRECOGNIZED_VOLUME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_UNRECOGNIZED_VOLUME)));
	PyModule_AddObject(m, "NT_STATUS_SERIAL_NO_DEVICE_INITED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SERIAL_NO_DEVICE_INITED)));
	PyModule_AddObject(m, "NT_STATUS_NO_SUCH_ALIAS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_SUCH_ALIAS)));
	PyModule_AddObject(m, "NT_STATUS_MEMBER_NOT_IN_ALIAS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MEMBER_NOT_IN_ALIAS)));
	PyModule_AddObject(m, "NT_STATUS_MEMBER_IN_ALIAS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MEMBER_IN_ALIAS)));
	PyModule_AddObject(m, "NT_STATUS_ALIAS_EXISTS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ALIAS_EXISTS)));
	PyModule_AddObject(m, "NT_STATUS_LOGON_NOT_GRANTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOGON_NOT_GRANTED)));
	PyModule_AddObject(m, "NT_STATUS_TOO_MANY_SECRETS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TOO_MANY_SECRETS)));
	PyModule_AddObject(m, "NT_STATUS_SECRET_TOO_LONG", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SECRET_TOO_LONG)));
	PyModule_AddObject(m, "NT_STATUS_INTERNAL_DB_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INTERNAL_DB_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_FULLSCREEN_MODE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FULLSCREEN_MODE)));
	PyModule_AddObject(m, "NT_STATUS_TOO_MANY_CONTEXT_IDS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TOO_MANY_CONTEXT_IDS)));
	PyModule_AddObject(m, "NT_STATUS_LOGON_TYPE_NOT_GRANTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOGON_TYPE_NOT_GRANTED)));
	PyModule_AddObject(m, "NT_STATUS_NOT_REGISTRY_FILE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOT_REGISTRY_FILE)));
	PyModule_AddObject(m, "NT_STATUS_NT_CROSS_ENCRYPTION_REQUIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NT_CROSS_ENCRYPTION_REQUIRED)));
	PyModule_AddObject(m, "NT_STATUS_DOMAIN_CTRLR_CONFIG_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DOMAIN_CTRLR_CONFIG_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_FT_MISSING_MEMBER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FT_MISSING_MEMBER)));
	PyModule_AddObject(m, "NT_STATUS_ILL_FORMED_SERVICE_ENTRY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ILL_FORMED_SERVICE_ENTRY)));
	PyModule_AddObject(m, "NT_STATUS_ILLEGAL_CHARACTER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ILLEGAL_CHARACTER)));
	PyModule_AddObject(m, "NT_STATUS_UNMAPPABLE_CHARACTER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_UNMAPPABLE_CHARACTER)));
	PyModule_AddObject(m, "NT_STATUS_UNDEFINED_CHARACTER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_UNDEFINED_CHARACTER)));
	PyModule_AddObject(m, "NT_STATUS_FLOPPY_VOLUME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLOPPY_VOLUME)));
	PyModule_AddObject(m, "NT_STATUS_FLOPPY_ID_MARK_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLOPPY_ID_MARK_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_FLOPPY_WRONG_CYLINDER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLOPPY_WRONG_CYLINDER)));
	PyModule_AddObject(m, "NT_STATUS_FLOPPY_UNKNOWN_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLOPPY_UNKNOWN_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_FLOPPY_BAD_REGISTERS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLOPPY_BAD_REGISTERS)));
	PyModule_AddObject(m, "NT_STATUS_DISK_RECALIBRATE_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DISK_RECALIBRATE_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_DISK_OPERATION_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DISK_OPERATION_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_DISK_RESET_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DISK_RESET_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_SHARED_IRQ_BUSY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SHARED_IRQ_BUSY)));
	PyModule_AddObject(m, "NT_STATUS_FT_ORPHANING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FT_ORPHANING)));
	PyModule_AddObject(m, "NT_STATUS_BIOS_FAILED_TO_CONNECT_INTERRUPT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BIOS_FAILED_TO_CONNECT_INTERRUPT)));
	PyModule_AddObject(m, "NT_STATUS_PARTITION_FAILURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PARTITION_FAILURE)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_BLOCK_LENGTH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_BLOCK_LENGTH)));
	PyModule_AddObject(m, "NT_STATUS_DEVICE_NOT_PARTITIONED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DEVICE_NOT_PARTITIONED)));
	PyModule_AddObject(m, "NT_STATUS_UNABLE_TO_LOCK_MEDIA", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_UNABLE_TO_LOCK_MEDIA)));
	PyModule_AddObject(m, "NT_STATUS_UNABLE_TO_UNLOAD_MEDIA", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_UNABLE_TO_UNLOAD_MEDIA)));
	PyModule_AddObject(m, "NT_STATUS_EOM_OVERFLOW", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_EOM_OVERFLOW)));
	PyModule_AddObject(m, "NT_STATUS_NO_MEDIA", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_MEDIA)));
	PyModule_AddObject(m, "NT_STATUS_NO_SUCH_MEMBER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_SUCH_MEMBER)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_MEMBER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_MEMBER)));
	PyModule_AddObject(m, "NT_STATUS_KEY_DELETED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_KEY_DELETED)));
	PyModule_AddObject(m, "NT_STATUS_NO_LOG_SPACE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_LOG_SPACE)));
	PyModule_AddObject(m, "NT_STATUS_TOO_MANY_SIDS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TOO_MANY_SIDS)));
	PyModule_AddObject(m, "NT_STATUS_LM_CROSS_ENCRYPTION_REQUIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LM_CROSS_ENCRYPTION_REQUIRED)));
	PyModule_AddObject(m, "NT_STATUS_KEY_HAS_CHILDREN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_KEY_HAS_CHILDREN)));
	PyModule_AddObject(m, "NT_STATUS_CHILD_MUST_BE_VOLATILE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CHILD_MUST_BE_VOLATILE)));
	PyModule_AddObject(m, "NT_STATUS_DEVICE_CONFIGURATION_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DEVICE_CONFIGURATION_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_DRIVER_INTERNAL_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DRIVER_INTERNAL_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_DEVICE_STATE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_DEVICE_STATE)));
	PyModule_AddObject(m, "NT_STATUS_IO_DEVICE_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IO_DEVICE_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_DEVICE_PROTOCOL_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DEVICE_PROTOCOL_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_BACKUP_CONTROLLER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BACKUP_CONTROLLER)));
	PyModule_AddObject(m, "NT_STATUS_LOG_FILE_FULL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_FILE_FULL)));
	PyModule_AddObject(m, "NT_STATUS_TOO_LATE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TOO_LATE)));
	PyModule_AddObject(m, "NT_STATUS_NO_TRUST_LSA_SECRET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_TRUST_LSA_SECRET)));
	PyModule_AddObject(m, "NT_STATUS_NO_TRUST_SAM_ACCOUNT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_TRUST_SAM_ACCOUNT)));
	PyModule_AddObject(m, "NT_STATUS_TRUSTED_DOMAIN_FAILURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRUSTED_DOMAIN_FAILURE)));
	PyModule_AddObject(m, "NT_STATUS_TRUSTED_RELATIONSHIP_FAILURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRUSTED_RELATIONSHIP_FAILURE)));
	PyModule_AddObject(m, "NT_STATUS_EVENTLOG_FILE_CORRUPT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_EVENTLOG_FILE_CORRUPT)));
	PyModule_AddObject(m, "NT_STATUS_EVENTLOG_CANT_START", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_EVENTLOG_CANT_START)));
	PyModule_AddObject(m, "NT_STATUS_TRUST_FAILURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRUST_FAILURE)));
	PyModule_AddObject(m, "NT_STATUS_MUTANT_LIMIT_EXCEEDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MUTANT_LIMIT_EXCEEDED)));
	PyModule_AddObject(m, "NT_STATUS_NETLOGON_NOT_STARTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NETLOGON_NOT_STARTED)));
	PyModule_AddObject(m, "NT_STATUS_ACCOUNT_EXPIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACCOUNT_EXPIRED)));
	PyModule_AddObject(m, "NT_STATUS_POSSIBLE_DEADLOCK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_POSSIBLE_DEADLOCK)));
	PyModule_AddObject(m, "NT_STATUS_NETWORK_CREDENTIAL_CONFLICT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NETWORK_CREDENTIAL_CONFLICT)));
	PyModule_AddObject(m, "NT_STATUS_REMOTE_SESSION_LIMIT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REMOTE_SESSION_LIMIT)));
	PyModule_AddObject(m, "NT_STATUS_EVENTLOG_FILE_CHANGED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_EVENTLOG_FILE_CHANGED)));
	PyModule_AddObject(m, "NT_STATUS_NOLOGON_INTERDOMAIN_TRUST_ACCOUNT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOLOGON_INTERDOMAIN_TRUST_ACCOUNT)));
	PyModule_AddObject(m, "NT_STATUS_NOLOGON_WORKSTATION_TRUST_ACCOUNT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOLOGON_WORKSTATION_TRUST_ACCOUNT)));
	PyModule_AddObject(m, "NT_STATUS_NOLOGON_SERVER_TRUST_ACCOUNT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOLOGON_SERVER_TRUST_ACCOUNT)));
	PyModule_AddObject(m, "NT_STATUS_DOMAIN_TRUST_INCONSISTENT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DOMAIN_TRUST_INCONSISTENT)));
	PyModule_AddObject(m, "NT_STATUS_FS_DRIVER_REQUIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FS_DRIVER_REQUIRED)));
	PyModule_AddObject(m, "NT_STATUS_IMAGE_ALREADY_LOADED_AS_DLL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IMAGE_ALREADY_LOADED_AS_DLL)));
	PyModule_AddObject(m, "NT_STATUS_INCOMPATIBLE_WITH_GLOBAL_SHORT_NAME_REGISTRY_SETTING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INCOMPATIBLE_WITH_GLOBAL_SHORT_NAME_REGISTRY_SETTING)));
	PyModule_AddObject(m, "NT_STATUS_SHORT_NAMES_NOT_ENABLED_ON_VOLUME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SHORT_NAMES_NOT_ENABLED_ON_VOLUME)));
	PyModule_AddObject(m, "NT_STATUS_SECURITY_STREAM_IS_INCONSISTENT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SECURITY_STREAM_IS_INCONSISTENT)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_LOCK_RANGE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_LOCK_RANGE)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_ACE_CONDITION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_ACE_CONDITION)));
	PyModule_AddObject(m, "NT_STATUS_IMAGE_SUBSYSTEM_NOT_PRESENT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IMAGE_SUBSYSTEM_NOT_PRESENT)));
	PyModule_AddObject(m, "NT_STATUS_NOTIFICATION_GUID_ALREADY_DEFINED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOTIFICATION_GUID_ALREADY_DEFINED)));
	PyModule_AddObject(m, "NT_STATUS_NETWORK_OPEN_RESTRICTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NETWORK_OPEN_RESTRICTION)));
	PyModule_AddObject(m, "NT_STATUS_NO_USER_SESSION_KEY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_USER_SESSION_KEY)));
	PyModule_AddObject(m, "NT_STATUS_USER_SESSION_DELETED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_USER_SESSION_DELETED)));
	PyModule_AddObject(m, "NT_STATUS_RESOURCE_LANG_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RESOURCE_LANG_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_INSUFF_SERVER_RESOURCES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INSUFF_SERVER_RESOURCES)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_BUFFER_SIZE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_BUFFER_SIZE)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_ADDRESS_COMPONENT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_ADDRESS_COMPONENT)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_ADDRESS_WILDCARD", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_ADDRESS_WILDCARD)));
	PyModule_AddObject(m, "NT_STATUS_TOO_MANY_ADDRESSES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TOO_MANY_ADDRESSES)));
	PyModule_AddObject(m, "NT_STATUS_ADDRESS_ALREADY_EXISTS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ADDRESS_ALREADY_EXISTS)));
	PyModule_AddObject(m, "NT_STATUS_ADDRESS_CLOSED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ADDRESS_CLOSED)));
	PyModule_AddObject(m, "NT_STATUS_CONNECTION_DISCONNECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CONNECTION_DISCONNECTED)));
	PyModule_AddObject(m, "NT_STATUS_CONNECTION_RESET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CONNECTION_RESET)));
	PyModule_AddObject(m, "NT_STATUS_TOO_MANY_NODES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TOO_MANY_NODES)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_ABORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_ABORTED)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_TIMED_OUT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_TIMED_OUT)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_NO_RELEASE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_NO_RELEASE)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_NO_MATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_NO_MATCH)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_RESPONDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_RESPONDED)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_INVALID_ID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_INVALID_ID)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_INVALID_TYPE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_INVALID_TYPE)));
	PyModule_AddObject(m, "NT_STATUS_NOT_SERVER_SESSION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOT_SERVER_SESSION)));
	PyModule_AddObject(m, "NT_STATUS_NOT_CLIENT_SESSION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOT_CLIENT_SESSION)));
	PyModule_AddObject(m, "NT_STATUS_CANNOT_LOAD_REGISTRY_FILE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CANNOT_LOAD_REGISTRY_FILE)));
	PyModule_AddObject(m, "NT_STATUS_DEBUG_ATTACH_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DEBUG_ATTACH_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_SYSTEM_PROCESS_TERMINATED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SYSTEM_PROCESS_TERMINATED)));
	PyModule_AddObject(m, "NT_STATUS_DATA_NOT_ACCEPTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DATA_NOT_ACCEPTED)));
	PyModule_AddObject(m, "NT_STATUS_NO_BROWSER_SERVERS_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_BROWSER_SERVERS_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_VDM_HARD_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VDM_HARD_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_DRIVER_CANCEL_TIMEOUT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DRIVER_CANCEL_TIMEOUT)));
	PyModule_AddObject(m, "NT_STATUS_REPLY_MESSAGE_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REPLY_MESSAGE_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_MAPPED_ALIGNMENT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MAPPED_ALIGNMENT)));
	PyModule_AddObject(m, "NT_STATUS_IMAGE_CHECKSUM_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IMAGE_CHECKSUM_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_LOST_WRITEBEHIND_DATA", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOST_WRITEBEHIND_DATA)));
	PyModule_AddObject(m, "NT_STATUS_CLIENT_SERVER_PARAMETERS_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLIENT_SERVER_PARAMETERS_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_PASSWORD_MUST_CHANGE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PASSWORD_MUST_CHANGE)));
	PyModule_AddObject(m, "NT_STATUS_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_NOT_TINY_STREAM", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOT_TINY_STREAM)));
	PyModule_AddObject(m, "NT_STATUS_RECOVERY_FAILURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RECOVERY_FAILURE)));
	PyModule_AddObject(m, "NT_STATUS_STACK_OVERFLOW_READ", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_STACK_OVERFLOW_READ)));
	PyModule_AddObject(m, "NT_STATUS_FAIL_CHECK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FAIL_CHECK)));
	PyModule_AddObject(m, "NT_STATUS_DUPLICATE_OBJECTID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DUPLICATE_OBJECTID)));
	PyModule_AddObject(m, "NT_STATUS_OBJECTID_EXISTS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_OBJECTID_EXISTS)));
	PyModule_AddObject(m, "NT_STATUS_CONVERT_TO_LARGE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CONVERT_TO_LARGE)));
	PyModule_AddObject(m, "NT_STATUS_RETRY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RETRY)));
	PyModule_AddObject(m, "NT_STATUS_FOUND_OUT_OF_SCOPE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FOUND_OUT_OF_SCOPE)));
	PyModule_AddObject(m, "NT_STATUS_ALLOCATE_BUCKET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ALLOCATE_BUCKET)));
	PyModule_AddObject(m, "NT_STATUS_PROPSET_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PROPSET_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_MARSHALL_OVERFLOW", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MARSHALL_OVERFLOW)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_VARIANT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_VARIANT)));
	PyModule_AddObject(m, "NT_STATUS_DOMAIN_CONTROLLER_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DOMAIN_CONTROLLER_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_ACCOUNT_LOCKED_OUT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACCOUNT_LOCKED_OUT)));
	PyModule_AddObject(m, "NT_STATUS_HANDLE_NOT_CLOSABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_HANDLE_NOT_CLOSABLE)));
	PyModule_AddObject(m, "NT_STATUS_CONNECTION_REFUSED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CONNECTION_REFUSED)));
	PyModule_AddObject(m, "NT_STATUS_GRACEFUL_DISCONNECT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRACEFUL_DISCONNECT)));
	PyModule_AddObject(m, "NT_STATUS_ADDRESS_ALREADY_ASSOCIATED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ADDRESS_ALREADY_ASSOCIATED)));
	PyModule_AddObject(m, "NT_STATUS_ADDRESS_NOT_ASSOCIATED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ADDRESS_NOT_ASSOCIATED)));
	PyModule_AddObject(m, "NT_STATUS_CONNECTION_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CONNECTION_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_CONNECTION_ACTIVE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CONNECTION_ACTIVE)));
	PyModule_AddObject(m, "NT_STATUS_NETWORK_UNREACHABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NETWORK_UNREACHABLE)));
	PyModule_AddObject(m, "NT_STATUS_HOST_UNREACHABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_HOST_UNREACHABLE)));
	PyModule_AddObject(m, "NT_STATUS_PROTOCOL_UNREACHABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PROTOCOL_UNREACHABLE)));
	PyModule_AddObject(m, "NT_STATUS_PORT_UNREACHABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PORT_UNREACHABLE)));
	PyModule_AddObject(m, "NT_STATUS_REQUEST_ABORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REQUEST_ABORTED)));
	PyModule_AddObject(m, "NT_STATUS_CONNECTION_ABORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CONNECTION_ABORTED)));
	PyModule_AddObject(m, "NT_STATUS_BAD_COMPRESSION_BUFFER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BAD_COMPRESSION_BUFFER)));
	PyModule_AddObject(m, "NT_STATUS_USER_MAPPED_FILE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_USER_MAPPED_FILE)));
	PyModule_AddObject(m, "NT_STATUS_AUDIT_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_AUDIT_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_TIMER_RESOLUTION_NOT_SET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TIMER_RESOLUTION_NOT_SET)));
	PyModule_AddObject(m, "NT_STATUS_CONNECTION_COUNT_LIMIT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CONNECTION_COUNT_LIMIT)));
	PyModule_AddObject(m, "NT_STATUS_LOGIN_TIME_RESTRICTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOGIN_TIME_RESTRICTION)));
	PyModule_AddObject(m, "NT_STATUS_LOGIN_WKSTA_RESTRICTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOGIN_WKSTA_RESTRICTION)));
	PyModule_AddObject(m, "NT_STATUS_IMAGE_MP_UP_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IMAGE_MP_UP_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_INSUFFICIENT_LOGON_INFO", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INSUFFICIENT_LOGON_INFO)));
	PyModule_AddObject(m, "NT_STATUS_BAD_DLL_ENTRYPOINT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BAD_DLL_ENTRYPOINT)));
	PyModule_AddObject(m, "NT_STATUS_BAD_SERVICE_ENTRYPOINT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BAD_SERVICE_ENTRYPOINT)));
	PyModule_AddObject(m, "NT_STATUS_LPC_REPLY_LOST", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LPC_REPLY_LOST)));
	PyModule_AddObject(m, "NT_STATUS_IP_ADDRESS_CONFLICT1", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IP_ADDRESS_CONFLICT1)));
	PyModule_AddObject(m, "NT_STATUS_IP_ADDRESS_CONFLICT2", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IP_ADDRESS_CONFLICT2)));
	PyModule_AddObject(m, "NT_STATUS_REGISTRY_QUOTA_LIMIT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REGISTRY_QUOTA_LIMIT)));
	PyModule_AddObject(m, "NT_STATUS_PATH_NOT_COVERED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PATH_NOT_COVERED)));
	PyModule_AddObject(m, "NT_STATUS_NO_CALLBACK_ACTIVE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_CALLBACK_ACTIVE)));
	PyModule_AddObject(m, "NT_STATUS_LICENSE_QUOTA_EXCEEDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LICENSE_QUOTA_EXCEEDED)));
	PyModule_AddObject(m, "NT_STATUS_PWD_TOO_SHORT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PWD_TOO_SHORT)));
	PyModule_AddObject(m, "NT_STATUS_PWD_TOO_RECENT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PWD_TOO_RECENT)));
	PyModule_AddObject(m, "NT_STATUS_PWD_HISTORY_CONFLICT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PWD_HISTORY_CONFLICT)));
	PyModule_AddObject(m, "NT_STATUS_PLUGPLAY_NO_DEVICE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PLUGPLAY_NO_DEVICE)));
	PyModule_AddObject(m, "NT_STATUS_UNSUPPORTED_COMPRESSION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_UNSUPPORTED_COMPRESSION)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_HW_PROFILE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_HW_PROFILE)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_PLUGPLAY_DEVICE_PATH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_PLUGPLAY_DEVICE_PATH)));
	PyModule_AddObject(m, "NT_STATUS_DRIVER_ORDINAL_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DRIVER_ORDINAL_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_DRIVER_ENTRYPOINT_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DRIVER_ENTRYPOINT_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_RESOURCE_NOT_OWNED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RESOURCE_NOT_OWNED)));
	PyModule_AddObject(m, "NT_STATUS_TOO_MANY_LINKS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TOO_MANY_LINKS)));
	PyModule_AddObject(m, "NT_STATUS_QUOTA_LIST_INCONSISTENT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_QUOTA_LIST_INCONSISTENT)));
	PyModule_AddObject(m, "NT_STATUS_FILE_IS_OFFLINE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FILE_IS_OFFLINE)));
	PyModule_AddObject(m, "NT_STATUS_EVALUATION_EXPIRATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_EVALUATION_EXPIRATION)));
	PyModule_AddObject(m, "NT_STATUS_ILLEGAL_DLL_RELOCATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ILLEGAL_DLL_RELOCATION)));
	PyModule_AddObject(m, "NT_STATUS_LICENSE_VIOLATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LICENSE_VIOLATION)));
	PyModule_AddObject(m, "NT_STATUS_DLL_INIT_FAILED_LOGOFF", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DLL_INIT_FAILED_LOGOFF)));
	PyModule_AddObject(m, "NT_STATUS_DRIVER_UNABLE_TO_LOAD", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DRIVER_UNABLE_TO_LOAD)));
	PyModule_AddObject(m, "NT_STATUS_DFS_UNAVAILABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DFS_UNAVAILABLE)));
	PyModule_AddObject(m, "NT_STATUS_VOLUME_DISMOUNTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VOLUME_DISMOUNTED)));
	PyModule_AddObject(m, "NT_STATUS_WX86_INTERNAL_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WX86_INTERNAL_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_WX86_FLOAT_STACK_CHECK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WX86_FLOAT_STACK_CHECK)));
	PyModule_AddObject(m, "NT_STATUS_VALIDATE_CONTINUE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VALIDATE_CONTINUE)));
	PyModule_AddObject(m, "NT_STATUS_NO_MATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_MATCH)));
	PyModule_AddObject(m, "NT_STATUS_NO_MORE_MATCHES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_MORE_MATCHES)));
	PyModule_AddObject(m, "NT_STATUS_NOT_A_REPARSE_POINT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOT_A_REPARSE_POINT)));
	PyModule_AddObject(m, "NT_STATUS_IO_REPARSE_TAG_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IO_REPARSE_TAG_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_IO_REPARSE_TAG_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IO_REPARSE_TAG_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_IO_REPARSE_DATA_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IO_REPARSE_DATA_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_IO_REPARSE_TAG_NOT_HANDLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IO_REPARSE_TAG_NOT_HANDLED)));
	PyModule_AddObject(m, "NT_STATUS_REPARSE_POINT_NOT_RESOLVED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REPARSE_POINT_NOT_RESOLVED)));
	PyModule_AddObject(m, "NT_STATUS_DIRECTORY_IS_A_REPARSE_POINT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DIRECTORY_IS_A_REPARSE_POINT)));
	PyModule_AddObject(m, "NT_STATUS_RANGE_LIST_CONFLICT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RANGE_LIST_CONFLICT)));
	PyModule_AddObject(m, "NT_STATUS_SOURCE_ELEMENT_EMPTY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SOURCE_ELEMENT_EMPTY)));
	PyModule_AddObject(m, "NT_STATUS_DESTINATION_ELEMENT_FULL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DESTINATION_ELEMENT_FULL)));
	PyModule_AddObject(m, "NT_STATUS_ILLEGAL_ELEMENT_ADDRESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ILLEGAL_ELEMENT_ADDRESS)));
	PyModule_AddObject(m, "NT_STATUS_MAGAZINE_NOT_PRESENT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MAGAZINE_NOT_PRESENT)));
	PyModule_AddObject(m, "NT_STATUS_REINITIALIZATION_NEEDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REINITIALIZATION_NEEDED)));
	PyModule_AddObject(m, "NT_STATUS_ENCRYPTION_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ENCRYPTION_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_DECRYPTION_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DECRYPTION_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_RANGE_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RANGE_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_NO_RECOVERY_POLICY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_RECOVERY_POLICY)));
	PyModule_AddObject(m, "NT_STATUS_NO_EFS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_EFS)));
	PyModule_AddObject(m, "NT_STATUS_WRONG_EFS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WRONG_EFS)));
	PyModule_AddObject(m, "NT_STATUS_NO_USER_KEYS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_USER_KEYS)));
	PyModule_AddObject(m, "NT_STATUS_FILE_NOT_ENCRYPTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FILE_NOT_ENCRYPTED)));
	PyModule_AddObject(m, "NT_STATUS_NOT_EXPORT_FORMAT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOT_EXPORT_FORMAT)));
	PyModule_AddObject(m, "NT_STATUS_FILE_ENCRYPTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FILE_ENCRYPTED)));
	PyModule_AddObject(m, "NT_STATUS_WMI_GUID_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WMI_GUID_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_WMI_INSTANCE_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WMI_INSTANCE_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_WMI_ITEMID_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WMI_ITEMID_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_WMI_TRY_AGAIN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WMI_TRY_AGAIN)));
	PyModule_AddObject(m, "NT_STATUS_SHARED_POLICY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SHARED_POLICY)));
	PyModule_AddObject(m, "NT_STATUS_POLICY_OBJECT_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_POLICY_OBJECT_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_POLICY_ONLY_IN_DS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_POLICY_ONLY_IN_DS)));
	PyModule_AddObject(m, "NT_STATUS_VOLUME_NOT_UPGRADED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VOLUME_NOT_UPGRADED)));
	PyModule_AddObject(m, "NT_STATUS_REMOTE_STORAGE_NOT_ACTIVE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REMOTE_STORAGE_NOT_ACTIVE)));
	PyModule_AddObject(m, "NT_STATUS_REMOTE_STORAGE_MEDIA_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REMOTE_STORAGE_MEDIA_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_NO_TRACKING_SERVICE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_TRACKING_SERVICE)));
	PyModule_AddObject(m, "NT_STATUS_SERVER_SID_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SERVER_SID_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_DS_NO_ATTRIBUTE_OR_VALUE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_NO_ATTRIBUTE_OR_VALUE)));
	PyModule_AddObject(m, "NT_STATUS_DS_INVALID_ATTRIBUTE_SYNTAX", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_INVALID_ATTRIBUTE_SYNTAX)));
	PyModule_AddObject(m, "NT_STATUS_DS_ATTRIBUTE_TYPE_UNDEFINED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_ATTRIBUTE_TYPE_UNDEFINED)));
	PyModule_AddObject(m, "NT_STATUS_DS_ATTRIBUTE_OR_VALUE_EXISTS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_ATTRIBUTE_OR_VALUE_EXISTS)));
	PyModule_AddObject(m, "NT_STATUS_DS_BUSY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_BUSY)));
	PyModule_AddObject(m, "NT_STATUS_DS_UNAVAILABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_UNAVAILABLE)));
	PyModule_AddObject(m, "NT_STATUS_DS_NO_RIDS_ALLOCATED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_NO_RIDS_ALLOCATED)));
	PyModule_AddObject(m, "NT_STATUS_DS_NO_MORE_RIDS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_NO_MORE_RIDS)));
	PyModule_AddObject(m, "NT_STATUS_DS_INCORRECT_ROLE_OWNER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_INCORRECT_ROLE_OWNER)));
	PyModule_AddObject(m, "NT_STATUS_DS_RIDMGR_INIT_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_RIDMGR_INIT_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_DS_OBJ_CLASS_VIOLATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_OBJ_CLASS_VIOLATION)));
	PyModule_AddObject(m, "NT_STATUS_DS_CANT_ON_NON_LEAF", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_CANT_ON_NON_LEAF)));
	PyModule_AddObject(m, "NT_STATUS_DS_CANT_ON_RDN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_CANT_ON_RDN)));
	PyModule_AddObject(m, "NT_STATUS_DS_CANT_MOD_OBJ_CLASS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_CANT_MOD_OBJ_CLASS)));
	PyModule_AddObject(m, "NT_STATUS_DS_CROSS_DOM_MOVE_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_CROSS_DOM_MOVE_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_DS_GC_NOT_AVAILABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_GC_NOT_AVAILABLE)));
	PyModule_AddObject(m, "NT_STATUS_DIRECTORY_SERVICE_REQUIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DIRECTORY_SERVICE_REQUIRED)));
	PyModule_AddObject(m, "NT_STATUS_REPARSE_ATTRIBUTE_CONFLICT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REPARSE_ATTRIBUTE_CONFLICT)));
	PyModule_AddObject(m, "NT_STATUS_CANT_ENABLE_DENY_ONLY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CANT_ENABLE_DENY_ONLY)));
	PyModule_AddObject(m, "NT_STATUS_FLOAT_MULTIPLE_FAULTS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLOAT_MULTIPLE_FAULTS)));
	PyModule_AddObject(m, "NT_STATUS_FLOAT_MULTIPLE_TRAPS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLOAT_MULTIPLE_TRAPS)));
	PyModule_AddObject(m, "NT_STATUS_DEVICE_REMOVED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DEVICE_REMOVED)));
	PyModule_AddObject(m, "NT_STATUS_JOURNAL_DELETE_IN_PROGRESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_JOURNAL_DELETE_IN_PROGRESS)));
	PyModule_AddObject(m, "NT_STATUS_JOURNAL_NOT_ACTIVE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_JOURNAL_NOT_ACTIVE)));
	PyModule_AddObject(m, "NT_STATUS_NOINTERFACE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOINTERFACE)));
	PyModule_AddObject(m, "NT_STATUS_DS_ADMIN_LIMIT_EXCEEDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_ADMIN_LIMIT_EXCEEDED)));
	PyModule_AddObject(m, "NT_STATUS_DRIVER_FAILED_SLEEP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DRIVER_FAILED_SLEEP)));
	PyModule_AddObject(m, "NT_STATUS_MUTUAL_AUTHENTICATION_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MUTUAL_AUTHENTICATION_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_CORRUPT_SYSTEM_FILE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CORRUPT_SYSTEM_FILE)));
	PyModule_AddObject(m, "NT_STATUS_DATATYPE_MISALIGNMENT_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DATATYPE_MISALIGNMENT_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_WMI_READ_ONLY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WMI_READ_ONLY)));
	PyModule_AddObject(m, "NT_STATUS_WMI_SET_FAILURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WMI_SET_FAILURE)));
	PyModule_AddObject(m, "NT_STATUS_COMMITMENT_MINIMUM", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_COMMITMENT_MINIMUM)));
	PyModule_AddObject(m, "NT_STATUS_REG_NAT_CONSUMPTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REG_NAT_CONSUMPTION)));
	PyModule_AddObject(m, "NT_STATUS_TRANSPORT_FULL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSPORT_FULL)));
	PyModule_AddObject(m, "NT_STATUS_DS_SAM_INIT_FAILURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_SAM_INIT_FAILURE)));
	PyModule_AddObject(m, "NT_STATUS_ONLY_IF_CONNECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ONLY_IF_CONNECTED)));
	PyModule_AddObject(m, "NT_STATUS_DS_SENSITIVE_GROUP_VIOLATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_SENSITIVE_GROUP_VIOLATION)));
	PyModule_AddObject(m, "NT_STATUS_PNP_RESTART_ENUMERATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PNP_RESTART_ENUMERATION)));
	PyModule_AddObject(m, "NT_STATUS_JOURNAL_ENTRY_DELETED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_JOURNAL_ENTRY_DELETED)));
	PyModule_AddObject(m, "NT_STATUS_DS_CANT_MOD_PRIMARYGROUPID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_CANT_MOD_PRIMARYGROUPID)));
	PyModule_AddObject(m, "NT_STATUS_SYSTEM_IMAGE_BAD_SIGNATURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SYSTEM_IMAGE_BAD_SIGNATURE)));
	PyModule_AddObject(m, "NT_STATUS_PNP_REBOOT_REQUIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PNP_REBOOT_REQUIRED)));
	PyModule_AddObject(m, "NT_STATUS_POWER_STATE_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_POWER_STATE_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_DS_INVALID_GROUP_TYPE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_INVALID_GROUP_TYPE)));
	PyModule_AddObject(m, "NT_STATUS_DS_NO_NEST_GLOBALGROUP_IN_MIXEDDOMAIN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_NO_NEST_GLOBALGROUP_IN_MIXEDDOMAIN)));
	PyModule_AddObject(m, "NT_STATUS_DS_NO_NEST_LOCALGROUP_IN_MIXEDDOMAIN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_NO_NEST_LOCALGROUP_IN_MIXEDDOMAIN)));
	PyModule_AddObject(m, "NT_STATUS_DS_GLOBAL_CANT_HAVE_LOCAL_MEMBER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_GLOBAL_CANT_HAVE_LOCAL_MEMBER)));
	PyModule_AddObject(m, "NT_STATUS_DS_GLOBAL_CANT_HAVE_UNIVERSAL_MEMBER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_GLOBAL_CANT_HAVE_UNIVERSAL_MEMBER)));
	PyModule_AddObject(m, "NT_STATUS_DS_UNIVERSAL_CANT_HAVE_LOCAL_MEMBER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_UNIVERSAL_CANT_HAVE_LOCAL_MEMBER)));
	PyModule_AddObject(m, "NT_STATUS_DS_GLOBAL_CANT_HAVE_CROSSDOMAIN_MEMBER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_GLOBAL_CANT_HAVE_CROSSDOMAIN_MEMBER)));
	PyModule_AddObject(m, "NT_STATUS_DS_LOCAL_CANT_HAVE_CROSSDOMAIN_LOCAL_MEMBER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_LOCAL_CANT_HAVE_CROSSDOMAIN_LOCAL_MEMBER)));
	PyModule_AddObject(m, "NT_STATUS_DS_HAVE_PRIMARY_MEMBERS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_HAVE_PRIMARY_MEMBERS)));
	PyModule_AddObject(m, "NT_STATUS_WMI_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WMI_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_INSUFFICIENT_POWER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INSUFFICIENT_POWER)));
	PyModule_AddObject(m, "NT_STATUS_SAM_NEED_BOOTKEY_PASSWORD", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SAM_NEED_BOOTKEY_PASSWORD)));
	PyModule_AddObject(m, "NT_STATUS_SAM_NEED_BOOTKEY_FLOPPY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SAM_NEED_BOOTKEY_FLOPPY)));
	PyModule_AddObject(m, "NT_STATUS_DS_CANT_START", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_CANT_START)));
	PyModule_AddObject(m, "NT_STATUS_DS_INIT_FAILURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_INIT_FAILURE)));
	PyModule_AddObject(m, "NT_STATUS_SAM_INIT_FAILURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SAM_INIT_FAILURE)));
	PyModule_AddObject(m, "NT_STATUS_DS_GC_REQUIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_GC_REQUIRED)));
	PyModule_AddObject(m, "NT_STATUS_DS_LOCAL_MEMBER_OF_LOCAL_ONLY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_LOCAL_MEMBER_OF_LOCAL_ONLY)));
	PyModule_AddObject(m, "NT_STATUS_DS_NO_FPO_IN_UNIVERSAL_GROUPS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_NO_FPO_IN_UNIVERSAL_GROUPS)));
	PyModule_AddObject(m, "NT_STATUS_DS_MACHINE_ACCOUNT_QUOTA_EXCEEDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_MACHINE_ACCOUNT_QUOTA_EXCEEDED)));
	PyModule_AddObject(m, "NT_STATUS_CURRENT_DOMAIN_NOT_ALLOWED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CURRENT_DOMAIN_NOT_ALLOWED)));
	PyModule_AddObject(m, "NT_STATUS_CANNOT_MAKE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CANNOT_MAKE)));
	PyModule_AddObject(m, "NT_STATUS_SYSTEM_SHUTDOWN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SYSTEM_SHUTDOWN)));
	PyModule_AddObject(m, "NT_STATUS_DS_INIT_FAILURE_CONSOLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_INIT_FAILURE_CONSOLE)));
	PyModule_AddObject(m, "NT_STATUS_DS_SAM_INIT_FAILURE_CONSOLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_SAM_INIT_FAILURE_CONSOLE)));
	PyModule_AddObject(m, "NT_STATUS_UNFINISHED_CONTEXT_DELETED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_UNFINISHED_CONTEXT_DELETED)));
	PyModule_AddObject(m, "NT_STATUS_NO_TGT_REPLY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_TGT_REPLY)));
	PyModule_AddObject(m, "NT_STATUS_OBJECTID_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_OBJECTID_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_NO_IP_ADDRESSES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_IP_ADDRESSES)));
	PyModule_AddObject(m, "NT_STATUS_WRONG_CREDENTIAL_HANDLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WRONG_CREDENTIAL_HANDLE)));
	PyModule_AddObject(m, "NT_STATUS_CRYPTO_SYSTEM_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CRYPTO_SYSTEM_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_MAX_REFERRALS_EXCEEDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MAX_REFERRALS_EXCEEDED)));
	PyModule_AddObject(m, "NT_STATUS_MUST_BE_KDC", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MUST_BE_KDC)));
	PyModule_AddObject(m, "NT_STATUS_STRONG_CRYPTO_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_STRONG_CRYPTO_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_TOO_MANY_PRINCIPALS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TOO_MANY_PRINCIPALS)));
	PyModule_AddObject(m, "NT_STATUS_NO_PA_DATA", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_PA_DATA)));
	PyModule_AddObject(m, "NT_STATUS_PKINIT_NAME_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PKINIT_NAME_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_SMARTCARD_LOGON_REQUIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SMARTCARD_LOGON_REQUIRED)));
	PyModule_AddObject(m, "NT_STATUS_KDC_INVALID_REQUEST", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_KDC_INVALID_REQUEST)));
	PyModule_AddObject(m, "NT_STATUS_KDC_UNABLE_TO_REFER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_KDC_UNABLE_TO_REFER)));
	PyModule_AddObject(m, "NT_STATUS_KDC_UNKNOWN_ETYPE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_KDC_UNKNOWN_ETYPE)));
	PyModule_AddObject(m, "NT_STATUS_SHUTDOWN_IN_PROGRESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SHUTDOWN_IN_PROGRESS)));
	PyModule_AddObject(m, "NT_STATUS_SERVER_SHUTDOWN_IN_PROGRESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SERVER_SHUTDOWN_IN_PROGRESS)));
	PyModule_AddObject(m, "NT_STATUS_NOT_SUPPORTED_ON_SBS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOT_SUPPORTED_ON_SBS)));
	PyModule_AddObject(m, "NT_STATUS_WMI_GUID_DISCONNECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WMI_GUID_DISCONNECTED)));
	PyModule_AddObject(m, "NT_STATUS_WMI_ALREADY_DISABLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WMI_ALREADY_DISABLED)));
	PyModule_AddObject(m, "NT_STATUS_WMI_ALREADY_ENABLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WMI_ALREADY_ENABLED)));
	PyModule_AddObject(m, "NT_STATUS_MFT_TOO_FRAGMENTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MFT_TOO_FRAGMENTED)));
	PyModule_AddObject(m, "NT_STATUS_COPY_PROTECTION_FAILURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_COPY_PROTECTION_FAILURE)));
	PyModule_AddObject(m, "NT_STATUS_CSS_AUTHENTICATION_FAILURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CSS_AUTHENTICATION_FAILURE)));
	PyModule_AddObject(m, "NT_STATUS_CSS_KEY_NOT_PRESENT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CSS_KEY_NOT_PRESENT)));
	PyModule_AddObject(m, "NT_STATUS_CSS_KEY_NOT_ESTABLISHED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CSS_KEY_NOT_ESTABLISHED)));
	PyModule_AddObject(m, "NT_STATUS_CSS_SCRAMBLED_SECTOR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CSS_SCRAMBLED_SECTOR)));
	PyModule_AddObject(m, "NT_STATUS_CSS_REGION_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CSS_REGION_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_CSS_RESETS_EXHAUSTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CSS_RESETS_EXHAUSTED)));
	PyModule_AddObject(m, "NT_STATUS_PKINIT_FAILURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PKINIT_FAILURE)));
	PyModule_AddObject(m, "NT_STATUS_SMARTCARD_SUBSYSTEM_FAILURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SMARTCARD_SUBSYSTEM_FAILURE)));
	PyModule_AddObject(m, "NT_STATUS_NO_KERB_KEY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_KERB_KEY)));
	PyModule_AddObject(m, "NT_STATUS_HOST_DOWN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_HOST_DOWN)));
	PyModule_AddObject(m, "NT_STATUS_UNSUPPORTED_PREAUTH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_UNSUPPORTED_PREAUTH)));
	PyModule_AddObject(m, "NT_STATUS_EFS_ALG_BLOB_TOO_BIG", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_EFS_ALG_BLOB_TOO_BIG)));
	PyModule_AddObject(m, "NT_STATUS_PORT_NOT_SET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PORT_NOT_SET)));
	PyModule_AddObject(m, "NT_STATUS_DEBUGGER_INACTIVE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DEBUGGER_INACTIVE)));
	PyModule_AddObject(m, "NT_STATUS_DS_VERSION_CHECK_FAILURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_VERSION_CHECK_FAILURE)));
	PyModule_AddObject(m, "NT_STATUS_AUDITING_DISABLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_AUDITING_DISABLED)));
	PyModule_AddObject(m, "NT_STATUS_PRENT4_MACHINE_ACCOUNT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PRENT4_MACHINE_ACCOUNT)));
	PyModule_AddObject(m, "NT_STATUS_DS_AG_CANT_HAVE_UNIVERSAL_MEMBER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_AG_CANT_HAVE_UNIVERSAL_MEMBER)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_IMAGE_WIN_32", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_IMAGE_WIN_32)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_IMAGE_WIN_64", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_IMAGE_WIN_64)));
	PyModule_AddObject(m, "NT_STATUS_BAD_BINDINGS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BAD_BINDINGS)));
	PyModule_AddObject(m, "NT_STATUS_NETWORK_SESSION_EXPIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NETWORK_SESSION_EXPIRED)));
	PyModule_AddObject(m, "NT_STATUS_APPHELP_BLOCK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_APPHELP_BLOCK)));
	PyModule_AddObject(m, "NT_STATUS_ALL_SIDS_FILTERED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ALL_SIDS_FILTERED)));
	PyModule_AddObject(m, "NT_STATUS_NOT_SAFE_MODE_DRIVER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOT_SAFE_MODE_DRIVER)));
	PyModule_AddObject(m, "NT_STATUS_ACCESS_DISABLED_BY_POLICY_DEFAULT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACCESS_DISABLED_BY_POLICY_DEFAULT)));
	PyModule_AddObject(m, "NT_STATUS_ACCESS_DISABLED_BY_POLICY_PATH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACCESS_DISABLED_BY_POLICY_PATH)));
	PyModule_AddObject(m, "NT_STATUS_ACCESS_DISABLED_BY_POLICY_PUBLISHER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACCESS_DISABLED_BY_POLICY_PUBLISHER)));
	PyModule_AddObject(m, "NT_STATUS_ACCESS_DISABLED_BY_POLICY_OTHER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACCESS_DISABLED_BY_POLICY_OTHER)));
	PyModule_AddObject(m, "NT_STATUS_FAILED_DRIVER_ENTRY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FAILED_DRIVER_ENTRY)));
	PyModule_AddObject(m, "NT_STATUS_DEVICE_ENUMERATION_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DEVICE_ENUMERATION_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_MOUNT_POINT_NOT_RESOLVED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MOUNT_POINT_NOT_RESOLVED)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_DEVICE_OBJECT_PARAMETER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_DEVICE_OBJECT_PARAMETER)));
	PyModule_AddObject(m, "NT_STATUS_MCA_OCCURED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MCA_OCCURED)));
	PyModule_AddObject(m, "NT_STATUS_DRIVER_BLOCKED_CRITICAL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DRIVER_BLOCKED_CRITICAL)));
	PyModule_AddObject(m, "NT_STATUS_DRIVER_BLOCKED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DRIVER_BLOCKED)));
	PyModule_AddObject(m, "NT_STATUS_DRIVER_DATABASE_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DRIVER_DATABASE_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_SYSTEM_HIVE_TOO_LARGE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SYSTEM_HIVE_TOO_LARGE)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_IMPORT_OF_NON_DLL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_IMPORT_OF_NON_DLL)));
	PyModule_AddObject(m, "NT_STATUS_NO_SECRETS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_SECRETS)));
	PyModule_AddObject(m, "NT_STATUS_ACCESS_DISABLED_NO_SAFER_UI_BY_POLICY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACCESS_DISABLED_NO_SAFER_UI_BY_POLICY)));
	PyModule_AddObject(m, "NT_STATUS_FAILED_STACK_SWITCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FAILED_STACK_SWITCH)));
	PyModule_AddObject(m, "NT_STATUS_HEAP_CORRUPTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_HEAP_CORRUPTION)));
	PyModule_AddObject(m, "NT_STATUS_SMARTCARD_WRONG_PIN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SMARTCARD_WRONG_PIN)));
	PyModule_AddObject(m, "NT_STATUS_SMARTCARD_CARD_BLOCKED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SMARTCARD_CARD_BLOCKED)));
	PyModule_AddObject(m, "NT_STATUS_SMARTCARD_CARD_NOT_AUTHENTICATED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SMARTCARD_CARD_NOT_AUTHENTICATED)));
	PyModule_AddObject(m, "NT_STATUS_SMARTCARD_NO_CARD", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SMARTCARD_NO_CARD)));
	PyModule_AddObject(m, "NT_STATUS_SMARTCARD_NO_KEY_CONTAINER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SMARTCARD_NO_KEY_CONTAINER)));
	PyModule_AddObject(m, "NT_STATUS_SMARTCARD_NO_CERTIFICATE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SMARTCARD_NO_CERTIFICATE)));
	PyModule_AddObject(m, "NT_STATUS_SMARTCARD_NO_KEYSET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SMARTCARD_NO_KEYSET)));
	PyModule_AddObject(m, "NT_STATUS_SMARTCARD_IO_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SMARTCARD_IO_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_DOWNGRADE_DETECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DOWNGRADE_DETECTED)));
	PyModule_AddObject(m, "NT_STATUS_SMARTCARD_CERT_REVOKED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SMARTCARD_CERT_REVOKED)));
	PyModule_AddObject(m, "NT_STATUS_ISSUING_CA_UNTRUSTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ISSUING_CA_UNTRUSTED)));
	PyModule_AddObject(m, "NT_STATUS_REVOCATION_OFFLINE_C", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REVOCATION_OFFLINE_C)));
	PyModule_AddObject(m, "NT_STATUS_PKINIT_CLIENT_FAILURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PKINIT_CLIENT_FAILURE)));
	PyModule_AddObject(m, "NT_STATUS_SMARTCARD_CERT_EXPIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SMARTCARD_CERT_EXPIRED)));
	PyModule_AddObject(m, "NT_STATUS_DRIVER_FAILED_PRIOR_UNLOAD", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DRIVER_FAILED_PRIOR_UNLOAD)));
	PyModule_AddObject(m, "NT_STATUS_SMARTCARD_SILENT_CONTEXT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SMARTCARD_SILENT_CONTEXT)));
	PyModule_AddObject(m, "NT_STATUS_PER_USER_TRUST_QUOTA_EXCEEDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PER_USER_TRUST_QUOTA_EXCEEDED)));
	PyModule_AddObject(m, "NT_STATUS_ALL_USER_TRUST_QUOTA_EXCEEDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ALL_USER_TRUST_QUOTA_EXCEEDED)));
	PyModule_AddObject(m, "NT_STATUS_USER_DELETE_TRUST_QUOTA_EXCEEDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_USER_DELETE_TRUST_QUOTA_EXCEEDED)));
	PyModule_AddObject(m, "NT_STATUS_DS_NAME_NOT_UNIQUE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_NAME_NOT_UNIQUE)));
	PyModule_AddObject(m, "NT_STATUS_DS_DUPLICATE_ID_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_DUPLICATE_ID_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_DS_GROUP_CONVERSION_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_GROUP_CONVERSION_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_VOLSNAP_PREPARE_HIBERNATE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VOLSNAP_PREPARE_HIBERNATE)));
	PyModule_AddObject(m, "NT_STATUS_USER2USER_REQUIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_USER2USER_REQUIRED)));
	PyModule_AddObject(m, "NT_STATUS_STACK_BUFFER_OVERRUN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_STACK_BUFFER_OVERRUN)));
	PyModule_AddObject(m, "NT_STATUS_NO_S4U_PROT_SUPPORT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_S4U_PROT_SUPPORT)));
	PyModule_AddObject(m, "NT_STATUS_CROSSREALM_DELEGATION_FAILURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CROSSREALM_DELEGATION_FAILURE)));
	PyModule_AddObject(m, "NT_STATUS_REVOCATION_OFFLINE_KDC", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REVOCATION_OFFLINE_KDC)));
	PyModule_AddObject(m, "NT_STATUS_ISSUING_CA_UNTRUSTED_KDC", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ISSUING_CA_UNTRUSTED_KDC)));
	PyModule_AddObject(m, "NT_STATUS_KDC_CERT_EXPIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_KDC_CERT_EXPIRED)));
	PyModule_AddObject(m, "NT_STATUS_KDC_CERT_REVOKED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_KDC_CERT_REVOKED)));
	PyModule_AddObject(m, "NT_STATUS_PARAMETER_QUOTA_EXCEEDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PARAMETER_QUOTA_EXCEEDED)));
	PyModule_AddObject(m, "NT_STATUS_HIBERNATION_FAILURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_HIBERNATION_FAILURE)));
	PyModule_AddObject(m, "NT_STATUS_DELAY_LOAD_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DELAY_LOAD_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_AUTHENTICATION_FIREWALL_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_AUTHENTICATION_FIREWALL_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_VDM_DISALLOWED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VDM_DISALLOWED)));
	PyModule_AddObject(m, "NT_STATUS_HUNG_DISPLAY_DRIVER_THREAD", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_HUNG_DISPLAY_DRIVER_THREAD)));
	PyModule_AddObject(m, "NT_STATUS_INSUFFICIENT_RESOURCE_FOR_SPECIFIED_SHARED_SECTION_SIZE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INSUFFICIENT_RESOURCE_FOR_SPECIFIED_SHARED_SECTION_SIZE)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_CRUNTIME_PARAMETER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_CRUNTIME_PARAMETER)));
	PyModule_AddObject(m, "NT_STATUS_NTLM_BLOCKED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NTLM_BLOCKED)));
	PyModule_AddObject(m, "NT_STATUS_DS_SRC_SID_EXISTS_IN_FOREST", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_SRC_SID_EXISTS_IN_FOREST)));
	PyModule_AddObject(m, "NT_STATUS_DS_DOMAIN_NAME_EXISTS_IN_FOREST", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_DOMAIN_NAME_EXISTS_IN_FOREST)));
	PyModule_AddObject(m, "NT_STATUS_DS_FLAT_NAME_EXISTS_IN_FOREST", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_FLAT_NAME_EXISTS_IN_FOREST)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_USER_PRINCIPAL_NAME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_USER_PRINCIPAL_NAME)));
	PyModule_AddObject(m, "NT_STATUS_ASSERTION_FAILURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ASSERTION_FAILURE)));
	PyModule_AddObject(m, "NT_STATUS_VERIFIER_STOP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VERIFIER_STOP)));
	PyModule_AddObject(m, "NT_STATUS_CALLBACK_POP_STACK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CALLBACK_POP_STACK)));
	PyModule_AddObject(m, "NT_STATUS_INCOMPATIBLE_DRIVER_BLOCKED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INCOMPATIBLE_DRIVER_BLOCKED)));
	PyModule_AddObject(m, "NT_STATUS_HIVE_UNLOADED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_HIVE_UNLOADED)));
	PyModule_AddObject(m, "NT_STATUS_COMPRESSION_DISABLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_COMPRESSION_DISABLED)));
	PyModule_AddObject(m, "NT_STATUS_FILE_SYSTEM_LIMITATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FILE_SYSTEM_LIMITATION)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_IMAGE_HASH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_IMAGE_HASH)));
	PyModule_AddObject(m, "NT_STATUS_NOT_CAPABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOT_CAPABLE)));
	PyModule_AddObject(m, "NT_STATUS_REQUEST_OUT_OF_SEQUENCE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REQUEST_OUT_OF_SEQUENCE)));
	PyModule_AddObject(m, "NT_STATUS_IMPLEMENTATION_LIMIT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IMPLEMENTATION_LIMIT)));
	PyModule_AddObject(m, "NT_STATUS_ELEVATION_REQUIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ELEVATION_REQUIRED)));
	PyModule_AddObject(m, "NT_STATUS_NO_SECURITY_CONTEXT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_SECURITY_CONTEXT)));
	PyModule_AddObject(m, "NT_STATUS_PKU2U_CERT_FAILURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PKU2U_CERT_FAILURE)));
	PyModule_AddObject(m, "NT_STATUS_BEYOND_VDL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BEYOND_VDL)));
	PyModule_AddObject(m, "NT_STATUS_ENCOUNTERED_WRITE_IN_PROGRESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ENCOUNTERED_WRITE_IN_PROGRESS)));
	PyModule_AddObject(m, "NT_STATUS_PTE_CHANGED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PTE_CHANGED)));
	PyModule_AddObject(m, "NT_STATUS_PURGE_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PURGE_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_CRED_REQUIRES_CONFIRMATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CRED_REQUIRES_CONFIRMATION)));
	PyModule_AddObject(m, "NT_STATUS_CS_ENCRYPTION_INVALID_SERVER_RESPONSE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CS_ENCRYPTION_INVALID_SERVER_RESPONSE)));
	PyModule_AddObject(m, "NT_STATUS_CS_ENCRYPTION_UNSUPPORTED_SERVER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CS_ENCRYPTION_UNSUPPORTED_SERVER)));
	PyModule_AddObject(m, "NT_STATUS_CS_ENCRYPTION_EXISTING_ENCRYPTED_FILE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CS_ENCRYPTION_EXISTING_ENCRYPTED_FILE)));
	PyModule_AddObject(m, "NT_STATUS_CS_ENCRYPTION_NEW_ENCRYPTED_FILE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CS_ENCRYPTION_NEW_ENCRYPTED_FILE)));
	PyModule_AddObject(m, "NT_STATUS_CS_ENCRYPTION_FILE_NOT_CSE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CS_ENCRYPTION_FILE_NOT_CSE)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_LABEL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_LABEL)));
	PyModule_AddObject(m, "NT_STATUS_DRIVER_PROCESS_TERMINATED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DRIVER_PROCESS_TERMINATED)));
	PyModule_AddObject(m, "NT_STATUS_AMBIGUOUS_SYSTEM_DEVICE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_AMBIGUOUS_SYSTEM_DEVICE)));
	PyModule_AddObject(m, "NT_STATUS_SYSTEM_DEVICE_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SYSTEM_DEVICE_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_RESTART_BOOT_APPLICATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RESTART_BOOT_APPLICATION)));
	PyModule_AddObject(m, "NT_STATUS_INSUFFICIENT_NVRAM_RESOURCES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INSUFFICIENT_NVRAM_RESOURCES)));
	PyModule_AddObject(m, "NT_STATUS_NO_RANGES_PROCESSED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_RANGES_PROCESSED)));
	PyModule_AddObject(m, "NT_STATUS_DEVICE_FEATURE_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DEVICE_FEATURE_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_DEVICE_UNREACHABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DEVICE_UNREACHABLE)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_TOKEN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_TOKEN)));
	PyModule_AddObject(m, "NT_STATUS_SERVER_UNAVAILABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SERVER_UNAVAILABLE)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_TASK_NAME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_TASK_NAME)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_TASK_INDEX", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_TASK_INDEX)));
	PyModule_AddObject(m, "NT_STATUS_THREAD_ALREADY_IN_TASK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_THREAD_ALREADY_IN_TASK)));
	PyModule_AddObject(m, "NT_STATUS_CALLBACK_BYPASS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CALLBACK_BYPASS)));
	PyModule_AddObject(m, "NT_STATUS_FAIL_FAST_EXCEPTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FAIL_FAST_EXCEPTION)));
	PyModule_AddObject(m, "NT_STATUS_IMAGE_CERT_REVOKED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IMAGE_CERT_REVOKED)));
	PyModule_AddObject(m, "NT_STATUS_PORT_CLOSED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PORT_CLOSED)));
	PyModule_AddObject(m, "NT_STATUS_MESSAGE_LOST", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MESSAGE_LOST)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_MESSAGE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_MESSAGE)));
	PyModule_AddObject(m, "NT_STATUS_REQUEST_CANCELED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REQUEST_CANCELED)));
	PyModule_AddObject(m, "NT_STATUS_RECURSIVE_DISPATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RECURSIVE_DISPATCH)));
	PyModule_AddObject(m, "NT_STATUS_LPC_RECEIVE_BUFFER_EXPECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LPC_RECEIVE_BUFFER_EXPECTED)));
	PyModule_AddObject(m, "NT_STATUS_LPC_INVALID_CONNECTION_USAGE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LPC_INVALID_CONNECTION_USAGE)));
	PyModule_AddObject(m, "NT_STATUS_LPC_REQUESTS_NOT_ALLOWED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LPC_REQUESTS_NOT_ALLOWED)));
	PyModule_AddObject(m, "NT_STATUS_RESOURCE_IN_USE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RESOURCE_IN_USE)));
	PyModule_AddObject(m, "NT_STATUS_HARDWARE_MEMORY_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_HARDWARE_MEMORY_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_THREADPOOL_HANDLE_EXCEPTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_THREADPOOL_HANDLE_EXCEPTION)));
	PyModule_AddObject(m, "NT_STATUS_THREADPOOL_SET_EVENT_ON_COMPLETION_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_THREADPOOL_SET_EVENT_ON_COMPLETION_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_THREADPOOL_RELEASE_SEMAPHORE_ON_COMPLETION_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_THREADPOOL_RELEASE_SEMAPHORE_ON_COMPLETION_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_THREADPOOL_RELEASE_MUTEX_ON_COMPLETION_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_THREADPOOL_RELEASE_MUTEX_ON_COMPLETION_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_THREADPOOL_FREE_LIBRARY_ON_COMPLETION_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_THREADPOOL_FREE_LIBRARY_ON_COMPLETION_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_THREADPOOL_RELEASED_DURING_OPERATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_THREADPOOL_RELEASED_DURING_OPERATION)));
	PyModule_AddObject(m, "NT_STATUS_CALLBACK_RETURNED_WHILE_IMPERSONATING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CALLBACK_RETURNED_WHILE_IMPERSONATING)));
	PyModule_AddObject(m, "NT_STATUS_APC_RETURNED_WHILE_IMPERSONATING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_APC_RETURNED_WHILE_IMPERSONATING)));
	PyModule_AddObject(m, "NT_STATUS_PROCESS_IS_PROTECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PROCESS_IS_PROTECTED)));
	PyModule_AddObject(m, "NT_STATUS_MCA_EXCEPTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MCA_EXCEPTION)));
	PyModule_AddObject(m, "NT_STATUS_CERTIFICATE_MAPPING_NOT_UNIQUE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CERTIFICATE_MAPPING_NOT_UNIQUE)));
	PyModule_AddObject(m, "NT_STATUS_SYMLINK_CLASS_DISABLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SYMLINK_CLASS_DISABLED)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_IDN_NORMALIZATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_IDN_NORMALIZATION)));
	PyModule_AddObject(m, "NT_STATUS_NO_UNICODE_TRANSLATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_UNICODE_TRANSLATION)));
	PyModule_AddObject(m, "NT_STATUS_ALREADY_REGISTERED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ALREADY_REGISTERED)));
	PyModule_AddObject(m, "NT_STATUS_CONTEXT_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CONTEXT_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_PORT_ALREADY_HAS_COMPLETION_LIST", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PORT_ALREADY_HAS_COMPLETION_LIST)));
	PyModule_AddObject(m, "NT_STATUS_CALLBACK_RETURNED_THREAD_PRIORITY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CALLBACK_RETURNED_THREAD_PRIORITY)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_THREAD", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_THREAD)));
	PyModule_AddObject(m, "NT_STATUS_CALLBACK_RETURNED_TRANSACTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CALLBACK_RETURNED_TRANSACTION)));
	PyModule_AddObject(m, "NT_STATUS_CALLBACK_RETURNED_LDR_LOCK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CALLBACK_RETURNED_LDR_LOCK)));
	PyModule_AddObject(m, "NT_STATUS_CALLBACK_RETURNED_LANG", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CALLBACK_RETURNED_LANG)));
	PyModule_AddObject(m, "NT_STATUS_CALLBACK_RETURNED_PRI_BACK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CALLBACK_RETURNED_PRI_BACK)));
	PyModule_AddObject(m, "NT_STATUS_DISK_REPAIR_DISABLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DISK_REPAIR_DISABLED)));
	PyModule_AddObject(m, "NT_STATUS_DS_DOMAIN_RENAME_IN_PROGRESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_DOMAIN_RENAME_IN_PROGRESS)));
	PyModule_AddObject(m, "NT_STATUS_DISK_QUOTA_EXCEEDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DISK_QUOTA_EXCEEDED)));
	PyModule_AddObject(m, "NT_STATUS_CONTENT_BLOCKED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CONTENT_BLOCKED)));
	PyModule_AddObject(m, "NT_STATUS_BAD_CLUSTERS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BAD_CLUSTERS)));
	PyModule_AddObject(m, "NT_STATUS_VOLUME_DIRTY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VOLUME_DIRTY)));
	PyModule_AddObject(m, "NT_STATUS_FILE_CHECKED_OUT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FILE_CHECKED_OUT)));
	PyModule_AddObject(m, "NT_STATUS_CHECKOUT_REQUIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CHECKOUT_REQUIRED)));
	PyModule_AddObject(m, "NT_STATUS_BAD_FILE_TYPE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BAD_FILE_TYPE)));
	PyModule_AddObject(m, "NT_STATUS_FILE_TOO_LARGE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FILE_TOO_LARGE)));
	PyModule_AddObject(m, "NT_STATUS_FORMS_AUTH_REQUIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FORMS_AUTH_REQUIRED)));
	PyModule_AddObject(m, "NT_STATUS_VIRUS_INFECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VIRUS_INFECTED)));
	PyModule_AddObject(m, "NT_STATUS_VIRUS_DELETED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VIRUS_DELETED)));
	PyModule_AddObject(m, "NT_STATUS_BAD_MCFG_TABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_BAD_MCFG_TABLE)));
	PyModule_AddObject(m, "NT_STATUS_CANNOT_BREAK_OPLOCK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CANNOT_BREAK_OPLOCK)));
	PyModule_AddObject(m, "NT_STATUS_WOW_ASSERTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WOW_ASSERTION)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_SIGNATURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_SIGNATURE)));
	PyModule_AddObject(m, "NT_STATUS_HMAC_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_HMAC_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_IPSEC_QUEUE_OVERFLOW", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IPSEC_QUEUE_OVERFLOW)));
	PyModule_AddObject(m, "NT_STATUS_ND_QUEUE_OVERFLOW", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ND_QUEUE_OVERFLOW)));
	PyModule_AddObject(m, "NT_STATUS_HOPLIMIT_EXCEEDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_HOPLIMIT_EXCEEDED)));
	PyModule_AddObject(m, "NT_STATUS_PROTOCOL_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PROTOCOL_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_LOST_WRITEBEHIND_DATA_NETWORK_DISCONNECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOST_WRITEBEHIND_DATA_NETWORK_DISCONNECTED)));
	PyModule_AddObject(m, "NT_STATUS_LOST_WRITEBEHIND_DATA_NETWORK_SERVER_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOST_WRITEBEHIND_DATA_NETWORK_SERVER_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_LOST_WRITEBEHIND_DATA_LOCAL_DISK_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOST_WRITEBEHIND_DATA_LOCAL_DISK_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_XML_PARSE_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_XML_PARSE_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_XMLDSIG_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_XMLDSIG_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_WRONG_COMPARTMENT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_WRONG_COMPARTMENT)));
	PyModule_AddObject(m, "NT_STATUS_AUTHIP_FAILURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_AUTHIP_FAILURE)));
	PyModule_AddObject(m, "NT_STATUS_DS_OID_MAPPED_GROUP_CANT_HAVE_MEMBERS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_OID_MAPPED_GROUP_CANT_HAVE_MEMBERS)));
	PyModule_AddObject(m, "NT_STATUS_DS_OID_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DS_OID_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_HASH_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_HASH_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_HASH_NOT_PRESENT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_HASH_NOT_PRESENT)));
	PyModule_AddObject(m, "NT_STATUS_OFFLOAD_READ_FLT_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_OFFLOAD_READ_FLT_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_OFFLOAD_WRITE_FLT_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_OFFLOAD_WRITE_FLT_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_OFFLOAD_READ_FILE_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_OFFLOAD_READ_FILE_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_OFFLOAD_WRITE_FILE_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_OFFLOAD_WRITE_FILE_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_DBG_NO_STATE_CHANGE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DBG_NO_STATE_CHANGE)));
	PyModule_AddObject(m, "NT_STATUS_DBG_APP_NOT_IDLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DBG_APP_NOT_IDLE)));
	PyModule_AddObject(m, "NT_STATUS_RPC_INVALID_STRING_BINDING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_INVALID_STRING_BINDING)));
	PyModule_AddObject(m, "NT_STATUS_RPC_WRONG_KIND_OF_BINDING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_WRONG_KIND_OF_BINDING)));
	PyModule_AddObject(m, "NT_STATUS_RPC_INVALID_BINDING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_INVALID_BINDING)));
	PyModule_AddObject(m, "NT_STATUS_RPC_PROTSEQ_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_PROTSEQ_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_RPC_INVALID_RPC_PROTSEQ", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_INVALID_RPC_PROTSEQ)));
	PyModule_AddObject(m, "NT_STATUS_RPC_INVALID_STRING_UUID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_INVALID_STRING_UUID)));
	PyModule_AddObject(m, "NT_STATUS_RPC_INVALID_ENDPOINT_FORMAT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_INVALID_ENDPOINT_FORMAT)));
	PyModule_AddObject(m, "NT_STATUS_RPC_INVALID_NET_ADDR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_INVALID_NET_ADDR)));
	PyModule_AddObject(m, "NT_STATUS_RPC_NO_ENDPOINT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_NO_ENDPOINT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_RPC_INVALID_TIMEOUT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_INVALID_TIMEOUT)));
	PyModule_AddObject(m, "NT_STATUS_RPC_OBJECT_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_OBJECT_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_RPC_ALREADY_REGISTERED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_ALREADY_REGISTERED)));
	PyModule_AddObject(m, "NT_STATUS_RPC_TYPE_ALREADY_REGISTERED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_TYPE_ALREADY_REGISTERED)));
	PyModule_AddObject(m, "NT_STATUS_RPC_ALREADY_LISTENING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_ALREADY_LISTENING)));
	PyModule_AddObject(m, "NT_STATUS_RPC_NO_PROTSEQS_REGISTERED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_NO_PROTSEQS_REGISTERED)));
	PyModule_AddObject(m, "NT_STATUS_RPC_NOT_LISTENING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_NOT_LISTENING)));
	PyModule_AddObject(m, "NT_STATUS_RPC_UNKNOWN_MGR_TYPE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_UNKNOWN_MGR_TYPE)));
	PyModule_AddObject(m, "NT_STATUS_RPC_UNKNOWN_IF", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_UNKNOWN_IF)));
	PyModule_AddObject(m, "NT_STATUS_RPC_NO_BINDINGS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_NO_BINDINGS)));
	PyModule_AddObject(m, "NT_STATUS_RPC_NO_PROTSEQS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_NO_PROTSEQS)));
	PyModule_AddObject(m, "NT_STATUS_RPC_CANT_CREATE_ENDPOINT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_CANT_CREATE_ENDPOINT)));
	PyModule_AddObject(m, "NT_STATUS_RPC_OUT_OF_RESOURCES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_OUT_OF_RESOURCES)));
	PyModule_AddObject(m, "NT_STATUS_RPC_SERVER_UNAVAILABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_SERVER_UNAVAILABLE)));
	PyModule_AddObject(m, "NT_STATUS_RPC_SERVER_TOO_BUSY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_SERVER_TOO_BUSY)));
	PyModule_AddObject(m, "NT_STATUS_RPC_INVALID_NETWORK_OPTIONS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_INVALID_NETWORK_OPTIONS)));
	PyModule_AddObject(m, "NT_STATUS_RPC_NO_CALL_ACTIVE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_NO_CALL_ACTIVE)));
	PyModule_AddObject(m, "NT_STATUS_RPC_CALL_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_CALL_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_RPC_CALL_FAILED_DNE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_CALL_FAILED_DNE)));
	PyModule_AddObject(m, "NT_STATUS_RPC_PROTOCOL_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_PROTOCOL_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_RPC_UNSUPPORTED_TRANS_SYN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_UNSUPPORTED_TRANS_SYN)));
	PyModule_AddObject(m, "NT_STATUS_RPC_UNSUPPORTED_TYPE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_UNSUPPORTED_TYPE)));
	PyModule_AddObject(m, "NT_STATUS_RPC_INVALID_TAG", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_INVALID_TAG)));
	PyModule_AddObject(m, "NT_STATUS_RPC_INVALID_BOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_INVALID_BOUND)));
	PyModule_AddObject(m, "NT_STATUS_RPC_NO_ENTRY_NAME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_NO_ENTRY_NAME)));
	PyModule_AddObject(m, "NT_STATUS_RPC_INVALID_NAME_SYNTAX", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_INVALID_NAME_SYNTAX)));
	PyModule_AddObject(m, "NT_STATUS_RPC_UNSUPPORTED_NAME_SYNTAX", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_UNSUPPORTED_NAME_SYNTAX)));
	PyModule_AddObject(m, "NT_STATUS_RPC_UUID_NO_ADDRESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_UUID_NO_ADDRESS)));
	PyModule_AddObject(m, "NT_STATUS_RPC_DUPLICATE_ENDPOINT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_DUPLICATE_ENDPOINT)));
	PyModule_AddObject(m, "NT_STATUS_RPC_UNKNOWN_AUTHN_TYPE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_UNKNOWN_AUTHN_TYPE)));
	PyModule_AddObject(m, "NT_STATUS_RPC_MAX_CALLS_TOO_SMALL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_MAX_CALLS_TOO_SMALL)));
	PyModule_AddObject(m, "NT_STATUS_RPC_STRING_TOO_LONG", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_STRING_TOO_LONG)));
	PyModule_AddObject(m, "NT_STATUS_RPC_PROTSEQ_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_PROTSEQ_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_RPC_PROCNUM_OUT_OF_RANGE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_PROCNUM_OUT_OF_RANGE)));
	PyModule_AddObject(m, "NT_STATUS_RPC_BINDING_HAS_NO_AUTH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_BINDING_HAS_NO_AUTH)));
	PyModule_AddObject(m, "NT_STATUS_RPC_UNKNOWN_AUTHN_SERVICE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_UNKNOWN_AUTHN_SERVICE)));
	PyModule_AddObject(m, "NT_STATUS_RPC_UNKNOWN_AUTHN_LEVEL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_UNKNOWN_AUTHN_LEVEL)));
	PyModule_AddObject(m, "NT_STATUS_RPC_INVALID_AUTH_IDENTITY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_INVALID_AUTH_IDENTITY)));
	PyModule_AddObject(m, "NT_STATUS_RPC_UNKNOWN_AUTHZ_SERVICE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_UNKNOWN_AUTHZ_SERVICE)));
	PyModule_AddObject(m, "NT_STATUS_EPT_INVALID_ENTRY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_EPT_INVALID_ENTRY)));
	PyModule_AddObject(m, "NT_STATUS_EPT_CANT_PERFORM_OP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_EPT_CANT_PERFORM_OP)));
	PyModule_AddObject(m, "NT_STATUS_EPT_NOT_REGISTERED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_EPT_NOT_REGISTERED)));
	PyModule_AddObject(m, "NT_STATUS_RPC_NOTHING_TO_EXPORT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_NOTHING_TO_EXPORT)));
	PyModule_AddObject(m, "NT_STATUS_RPC_INCOMPLETE_NAME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_INCOMPLETE_NAME)));
	PyModule_AddObject(m, "NT_STATUS_RPC_INVALID_VERS_OPTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_INVALID_VERS_OPTION)));
	PyModule_AddObject(m, "NT_STATUS_RPC_NO_MORE_MEMBERS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_NO_MORE_MEMBERS)));
	PyModule_AddObject(m, "NT_STATUS_RPC_NOT_ALL_OBJS_UNEXPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_NOT_ALL_OBJS_UNEXPORTED)));
	PyModule_AddObject(m, "NT_STATUS_RPC_INTERFACE_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_INTERFACE_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_RPC_ENTRY_ALREADY_EXISTS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_ENTRY_ALREADY_EXISTS)));
	PyModule_AddObject(m, "NT_STATUS_RPC_ENTRY_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_ENTRY_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_RPC_NAME_SERVICE_UNAVAILABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_NAME_SERVICE_UNAVAILABLE)));
	PyModule_AddObject(m, "NT_STATUS_RPC_INVALID_NAF_ID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_INVALID_NAF_ID)));
	PyModule_AddObject(m, "NT_STATUS_RPC_CANNOT_SUPPORT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_CANNOT_SUPPORT)));
	PyModule_AddObject(m, "NT_STATUS_RPC_NO_CONTEXT_AVAILABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_NO_CONTEXT_AVAILABLE)));
	PyModule_AddObject(m, "NT_STATUS_RPC_INTERNAL_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_INTERNAL_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_RPC_ZERO_DIVIDE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_ZERO_DIVIDE)));
	PyModule_AddObject(m, "NT_STATUS_RPC_ADDRESS_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_ADDRESS_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_RPC_FP_DIV_ZERO", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_FP_DIV_ZERO)));
	PyModule_AddObject(m, "NT_STATUS_RPC_FP_UNDERFLOW", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_FP_UNDERFLOW)));
	PyModule_AddObject(m, "NT_STATUS_RPC_FP_OVERFLOW", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_FP_OVERFLOW)));
	PyModule_AddObject(m, "NT_STATUS_RPC_CALL_IN_PROGRESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_CALL_IN_PROGRESS)));
	PyModule_AddObject(m, "NT_STATUS_RPC_NO_MORE_BINDINGS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_NO_MORE_BINDINGS)));
	PyModule_AddObject(m, "NT_STATUS_RPC_GROUP_MEMBER_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_GROUP_MEMBER_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_EPT_CANT_CREATE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_EPT_CANT_CREATE)));
	PyModule_AddObject(m, "NT_STATUS_RPC_INVALID_OBJECT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_INVALID_OBJECT)));
	PyModule_AddObject(m, "NT_STATUS_RPC_NO_INTERFACES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_NO_INTERFACES)));
	PyModule_AddObject(m, "NT_STATUS_RPC_CALL_CANCELLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_CALL_CANCELLED)));
	PyModule_AddObject(m, "NT_STATUS_RPC_BINDING_INCOMPLETE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_BINDING_INCOMPLETE)));
	PyModule_AddObject(m, "NT_STATUS_RPC_COMM_FAILURE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_COMM_FAILURE)));
	PyModule_AddObject(m, "NT_STATUS_RPC_UNSUPPORTED_AUTHN_LEVEL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_UNSUPPORTED_AUTHN_LEVEL)));
	PyModule_AddObject(m, "NT_STATUS_RPC_NO_PRINC_NAME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_NO_PRINC_NAME)));
	PyModule_AddObject(m, "NT_STATUS_RPC_NOT_RPC_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_NOT_RPC_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_RPC_SEC_PKG_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_SEC_PKG_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_RPC_NOT_CANCELLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_NOT_CANCELLED)));
	PyModule_AddObject(m, "NT_STATUS_RPC_INVALID_ASYNC_HANDLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_INVALID_ASYNC_HANDLE)));
	PyModule_AddObject(m, "NT_STATUS_RPC_INVALID_ASYNC_CALL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_INVALID_ASYNC_CALL)));
	PyModule_AddObject(m, "NT_STATUS_RPC_PROXY_ACCESS_DENIED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_PROXY_ACCESS_DENIED)));
	PyModule_AddObject(m, "NT_STATUS_RPC_NO_MORE_ENTRIES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_NO_MORE_ENTRIES)));
	PyModule_AddObject(m, "NT_STATUS_RPC_SS_CHAR_TRANS_OPEN_FAIL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_SS_CHAR_TRANS_OPEN_FAIL)));
	PyModule_AddObject(m, "NT_STATUS_RPC_SS_CHAR_TRANS_SHORT_FILE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_SS_CHAR_TRANS_SHORT_FILE)));
	PyModule_AddObject(m, "NT_STATUS_RPC_SS_IN_NULL_CONTEXT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_SS_IN_NULL_CONTEXT)));
	PyModule_AddObject(m, "NT_STATUS_RPC_SS_CONTEXT_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_SS_CONTEXT_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_RPC_SS_CONTEXT_DAMAGED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_SS_CONTEXT_DAMAGED)));
	PyModule_AddObject(m, "NT_STATUS_RPC_SS_HANDLES_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_SS_HANDLES_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_RPC_SS_CANNOT_GET_CALL_HANDLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_SS_CANNOT_GET_CALL_HANDLE)));
	PyModule_AddObject(m, "NT_STATUS_RPC_NULL_REF_POINTER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_NULL_REF_POINTER)));
	PyModule_AddObject(m, "NT_STATUS_RPC_ENUM_VALUE_OUT_OF_RANGE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_ENUM_VALUE_OUT_OF_RANGE)));
	PyModule_AddObject(m, "NT_STATUS_RPC_BYTE_COUNT_TOO_SMALL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_BYTE_COUNT_TOO_SMALL)));
	PyModule_AddObject(m, "NT_STATUS_RPC_BAD_STUB_DATA", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_BAD_STUB_DATA)));
	PyModule_AddObject(m, "NT_STATUS_RPC_INVALID_ES_ACTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_INVALID_ES_ACTION)));
	PyModule_AddObject(m, "NT_STATUS_RPC_WRONG_ES_VERSION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_WRONG_ES_VERSION)));
	PyModule_AddObject(m, "NT_STATUS_RPC_WRONG_STUB_VERSION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_WRONG_STUB_VERSION)));
	PyModule_AddObject(m, "NT_STATUS_RPC_INVALID_PIPE_OBJECT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_INVALID_PIPE_OBJECT)));
	PyModule_AddObject(m, "NT_STATUS_RPC_INVALID_PIPE_OPERATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_INVALID_PIPE_OPERATION)));
	PyModule_AddObject(m, "NT_STATUS_RPC_WRONG_PIPE_VERSION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_WRONG_PIPE_VERSION)));
	PyModule_AddObject(m, "NT_STATUS_RPC_PIPE_CLOSED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_PIPE_CLOSED)));
	PyModule_AddObject(m, "NT_STATUS_RPC_PIPE_DISCIPLINE_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_PIPE_DISCIPLINE_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_RPC_PIPE_EMPTY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RPC_PIPE_EMPTY)));
	PyModule_AddObject(m, "NT_STATUS_PNP_BAD_MPS_TABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PNP_BAD_MPS_TABLE)));
	PyModule_AddObject(m, "NT_STATUS_PNP_TRANSLATION_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PNP_TRANSLATION_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_PNP_IRQ_TRANSLATION_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PNP_IRQ_TRANSLATION_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_PNP_INVALID_ID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_PNP_INVALID_ID)));
	PyModule_AddObject(m, "NT_STATUS_IO_REISSUE_AS_CACHED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IO_REISSUE_AS_CACHED)));
	PyModule_AddObject(m, "NT_STATUS_CTX_WINSTATION_NAME_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_WINSTATION_NAME_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_CTX_INVALID_PD", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_INVALID_PD)));
	PyModule_AddObject(m, "NT_STATUS_CTX_PD_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_PD_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_CTX_CLOSE_PENDING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_CLOSE_PENDING)));
	PyModule_AddObject(m, "NT_STATUS_CTX_NO_OUTBUF", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_NO_OUTBUF)));
	PyModule_AddObject(m, "NT_STATUS_CTX_MODEM_INF_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_MODEM_INF_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_CTX_INVALID_MODEMNAME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_INVALID_MODEMNAME)));
	PyModule_AddObject(m, "NT_STATUS_CTX_RESPONSE_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_RESPONSE_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_CTX_MODEM_RESPONSE_TIMEOUT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_MODEM_RESPONSE_TIMEOUT)));
	PyModule_AddObject(m, "NT_STATUS_CTX_MODEM_RESPONSE_NO_CARRIER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_MODEM_RESPONSE_NO_CARRIER)));
	PyModule_AddObject(m, "NT_STATUS_CTX_MODEM_RESPONSE_NO_DIALTONE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_MODEM_RESPONSE_NO_DIALTONE)));
	PyModule_AddObject(m, "NT_STATUS_CTX_MODEM_RESPONSE_BUSY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_MODEM_RESPONSE_BUSY)));
	PyModule_AddObject(m, "NT_STATUS_CTX_MODEM_RESPONSE_VOICE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_MODEM_RESPONSE_VOICE)));
	PyModule_AddObject(m, "NT_STATUS_CTX_TD_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_TD_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_CTX_LICENSE_CLIENT_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_LICENSE_CLIENT_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_CTX_LICENSE_NOT_AVAILABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_LICENSE_NOT_AVAILABLE)));
	PyModule_AddObject(m, "NT_STATUS_CTX_LICENSE_EXPIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_LICENSE_EXPIRED)));
	PyModule_AddObject(m, "NT_STATUS_CTX_WINSTATION_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_WINSTATION_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_CTX_WINSTATION_NAME_COLLISION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_WINSTATION_NAME_COLLISION)));
	PyModule_AddObject(m, "NT_STATUS_CTX_WINSTATION_BUSY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_WINSTATION_BUSY)));
	PyModule_AddObject(m, "NT_STATUS_CTX_BAD_VIDEO_MODE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_BAD_VIDEO_MODE)));
	PyModule_AddObject(m, "NT_STATUS_CTX_GRAPHICS_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_GRAPHICS_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_CTX_NOT_CONSOLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_NOT_CONSOLE)));
	PyModule_AddObject(m, "NT_STATUS_CTX_CLIENT_QUERY_TIMEOUT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_CLIENT_QUERY_TIMEOUT)));
	PyModule_AddObject(m, "NT_STATUS_CTX_CONSOLE_DISCONNECT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_CONSOLE_DISCONNECT)));
	PyModule_AddObject(m, "NT_STATUS_CTX_CONSOLE_CONNECT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_CONSOLE_CONNECT)));
	PyModule_AddObject(m, "NT_STATUS_CTX_SHADOW_DENIED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_SHADOW_DENIED)));
	PyModule_AddObject(m, "NT_STATUS_CTX_WINSTATION_ACCESS_DENIED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_WINSTATION_ACCESS_DENIED)));
	PyModule_AddObject(m, "NT_STATUS_CTX_INVALID_WD", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_INVALID_WD)));
	PyModule_AddObject(m, "NT_STATUS_CTX_WD_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_WD_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_CTX_SHADOW_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_SHADOW_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_CTX_SHADOW_DISABLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_SHADOW_DISABLED)));
	PyModule_AddObject(m, "NT_STATUS_RDP_PROTOCOL_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RDP_PROTOCOL_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_CTX_CLIENT_LICENSE_NOT_SET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_CLIENT_LICENSE_NOT_SET)));
	PyModule_AddObject(m, "NT_STATUS_CTX_CLIENT_LICENSE_IN_USE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_CLIENT_LICENSE_IN_USE)));
	PyModule_AddObject(m, "NT_STATUS_CTX_SHADOW_ENDED_BY_MODE_CHANGE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_SHADOW_ENDED_BY_MODE_CHANGE)));
	PyModule_AddObject(m, "NT_STATUS_CTX_SHADOW_NOT_RUNNING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_SHADOW_NOT_RUNNING)));
	PyModule_AddObject(m, "NT_STATUS_CTX_LOGON_DISABLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_LOGON_DISABLED)));
	PyModule_AddObject(m, "NT_STATUS_CTX_SECURITY_LAYER_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CTX_SECURITY_LAYER_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_TS_INCOMPATIBLE_SESSIONS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TS_INCOMPATIBLE_SESSIONS)));
	PyModule_AddObject(m, "NT_STATUS_MUI_FILE_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MUI_FILE_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_MUI_INVALID_FILE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MUI_INVALID_FILE)));
	PyModule_AddObject(m, "NT_STATUS_MUI_INVALID_RC_CONFIG", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MUI_INVALID_RC_CONFIG)));
	PyModule_AddObject(m, "NT_STATUS_MUI_INVALID_LOCALE_NAME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MUI_INVALID_LOCALE_NAME)));
	PyModule_AddObject(m, "NT_STATUS_MUI_INVALID_ULTIMATEFALLBACK_NAME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MUI_INVALID_ULTIMATEFALLBACK_NAME)));
	PyModule_AddObject(m, "NT_STATUS_MUI_FILE_NOT_LOADED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MUI_FILE_NOT_LOADED)));
	PyModule_AddObject(m, "NT_STATUS_RESOURCE_ENUM_USER_STOP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RESOURCE_ENUM_USER_STOP)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_INVALID_NODE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_INVALID_NODE)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_NODE_EXISTS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_NODE_EXISTS)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_JOIN_IN_PROGRESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_JOIN_IN_PROGRESS)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_NODE_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_NODE_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_LOCAL_NODE_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_LOCAL_NODE_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_NETWORK_EXISTS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_NETWORK_EXISTS)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_NETWORK_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_NETWORK_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_NETINTERFACE_EXISTS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_NETINTERFACE_EXISTS)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_NETINTERFACE_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_NETINTERFACE_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_INVALID_REQUEST", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_INVALID_REQUEST)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_INVALID_NETWORK_PROVIDER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_INVALID_NETWORK_PROVIDER)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_NODE_DOWN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_NODE_DOWN)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_NODE_UNREACHABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_NODE_UNREACHABLE)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_NODE_NOT_MEMBER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_NODE_NOT_MEMBER)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_JOIN_NOT_IN_PROGRESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_JOIN_NOT_IN_PROGRESS)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_INVALID_NETWORK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_INVALID_NETWORK)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_NO_NET_ADAPTERS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_NO_NET_ADAPTERS)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_NODE_UP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_NODE_UP)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_NODE_PAUSED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_NODE_PAUSED)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_NODE_NOT_PAUSED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_NODE_NOT_PAUSED)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_NO_SECURITY_CONTEXT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_NO_SECURITY_CONTEXT)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_NETWORK_NOT_INTERNAL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_NETWORK_NOT_INTERNAL)));
	PyModule_AddObject(m, "NT_STATUS_CLUSTER_POISONED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CLUSTER_POISONED)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_INVALID_OPCODE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_INVALID_OPCODE)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_STACK_OVERFLOW", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_STACK_OVERFLOW)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_ASSERT_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_ASSERT_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_INVALID_INDEX", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_INVALID_INDEX)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_INVALID_ARGUMENT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_INVALID_ARGUMENT)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_FATAL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_FATAL)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_INVALID_SUPERNAME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_INVALID_SUPERNAME)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_INVALID_ARGTYPE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_INVALID_ARGTYPE)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_INVALID_OBJTYPE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_INVALID_OBJTYPE)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_INVALID_TARGETTYPE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_INVALID_TARGETTYPE)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_INCORRECT_ARGUMENT_COUNT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_INCORRECT_ARGUMENT_COUNT)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_ADDRESS_NOT_MAPPED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_ADDRESS_NOT_MAPPED)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_INVALID_EVENTTYPE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_INVALID_EVENTTYPE)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_HANDLER_COLLISION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_HANDLER_COLLISION)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_INVALID_DATA", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_INVALID_DATA)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_INVALID_REGION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_INVALID_REGION)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_INVALID_ACCESS_SIZE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_INVALID_ACCESS_SIZE)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_ACQUIRE_GLOBAL_LOCK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_ACQUIRE_GLOBAL_LOCK)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_ALREADY_INITIALIZED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_ALREADY_INITIALIZED)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_NOT_INITIALIZED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_NOT_INITIALIZED)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_INVALID_MUTEX_LEVEL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_INVALID_MUTEX_LEVEL)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_MUTEX_NOT_OWNED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_MUTEX_NOT_OWNED)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_MUTEX_NOT_OWNER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_MUTEX_NOT_OWNER)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_RS_ACCESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_RS_ACCESS)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_INVALID_TABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_INVALID_TABLE)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_REG_HANDLER_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_REG_HANDLER_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_ACPI_POWER_REQUEST_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ACPI_POWER_REQUEST_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_SXS_SECTION_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_SECTION_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_SXS_CANT_GEN_ACTCTX", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_CANT_GEN_ACTCTX)));
	PyModule_AddObject(m, "NT_STATUS_SXS_INVALID_ACTCTXDATA_FORMAT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_INVALID_ACTCTXDATA_FORMAT)));
	PyModule_AddObject(m, "NT_STATUS_SXS_ASSEMBLY_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_ASSEMBLY_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_SXS_MANIFEST_FORMAT_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_MANIFEST_FORMAT_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_SXS_MANIFEST_PARSE_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_MANIFEST_PARSE_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_SXS_ACTIVATION_CONTEXT_DISABLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_ACTIVATION_CONTEXT_DISABLED)));
	PyModule_AddObject(m, "NT_STATUS_SXS_KEY_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_KEY_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_SXS_VERSION_CONFLICT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_VERSION_CONFLICT)));
	PyModule_AddObject(m, "NT_STATUS_SXS_WRONG_SECTION_TYPE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_WRONG_SECTION_TYPE)));
	PyModule_AddObject(m, "NT_STATUS_SXS_THREAD_QUERIES_DISABLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_THREAD_QUERIES_DISABLED)));
	PyModule_AddObject(m, "NT_STATUS_SXS_ASSEMBLY_MISSING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_ASSEMBLY_MISSING)));
	PyModule_AddObject(m, "NT_STATUS_SXS_PROCESS_DEFAULT_ALREADY_SET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_PROCESS_DEFAULT_ALREADY_SET)));
	PyModule_AddObject(m, "NT_STATUS_SXS_EARLY_DEACTIVATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_EARLY_DEACTIVATION)));
	PyModule_AddObject(m, "NT_STATUS_SXS_INVALID_DEACTIVATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_INVALID_DEACTIVATION)));
	PyModule_AddObject(m, "NT_STATUS_SXS_MULTIPLE_DEACTIVATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_MULTIPLE_DEACTIVATION)));
	PyModule_AddObject(m, "NT_STATUS_SXS_SYSTEM_DEFAULT_ACTIVATION_CONTEXT_EMPTY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_SYSTEM_DEFAULT_ACTIVATION_CONTEXT_EMPTY)));
	PyModule_AddObject(m, "NT_STATUS_SXS_PROCESS_TERMINATION_REQUESTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_PROCESS_TERMINATION_REQUESTED)));
	PyModule_AddObject(m, "NT_STATUS_SXS_CORRUPT_ACTIVATION_STACK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_CORRUPT_ACTIVATION_STACK)));
	PyModule_AddObject(m, "NT_STATUS_SXS_CORRUPTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_CORRUPTION)));
	PyModule_AddObject(m, "NT_STATUS_SXS_INVALID_IDENTITY_ATTRIBUTE_VALUE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_INVALID_IDENTITY_ATTRIBUTE_VALUE)));
	PyModule_AddObject(m, "NT_STATUS_SXS_INVALID_IDENTITY_ATTRIBUTE_NAME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_INVALID_IDENTITY_ATTRIBUTE_NAME)));
	PyModule_AddObject(m, "NT_STATUS_SXS_IDENTITY_DUPLICATE_ATTRIBUTE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_IDENTITY_DUPLICATE_ATTRIBUTE)));
	PyModule_AddObject(m, "NT_STATUS_SXS_IDENTITY_PARSE_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_IDENTITY_PARSE_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_SXS_COMPONENT_STORE_CORRUPT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_COMPONENT_STORE_CORRUPT)));
	PyModule_AddObject(m, "NT_STATUS_SXS_FILE_HASH_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_FILE_HASH_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_SXS_MANIFEST_IDENTITY_SAME_BUT_CONTENTS_DIFFERENT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_MANIFEST_IDENTITY_SAME_BUT_CONTENTS_DIFFERENT)));
	PyModule_AddObject(m, "NT_STATUS_SXS_IDENTITIES_DIFFERENT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_IDENTITIES_DIFFERENT)));
	PyModule_AddObject(m, "NT_STATUS_SXS_ASSEMBLY_IS_NOT_A_DEPLOYMENT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_ASSEMBLY_IS_NOT_A_DEPLOYMENT)));
	PyModule_AddObject(m, "NT_STATUS_SXS_FILE_NOT_PART_OF_ASSEMBLY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_FILE_NOT_PART_OF_ASSEMBLY)));
	PyModule_AddObject(m, "NT_STATUS_ADVANCED_INSTALLER_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ADVANCED_INSTALLER_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_XML_ENCODING_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_XML_ENCODING_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_SXS_MANIFEST_TOO_BIG", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_MANIFEST_TOO_BIG)));
	PyModule_AddObject(m, "NT_STATUS_SXS_SETTING_NOT_REGISTERED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_SETTING_NOT_REGISTERED)));
	PyModule_AddObject(m, "NT_STATUS_SXS_TRANSACTION_CLOSURE_INCOMPLETE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_TRANSACTION_CLOSURE_INCOMPLETE)));
	PyModule_AddObject(m, "NT_STATUS_SMI_PRIMITIVE_INSTALLER_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SMI_PRIMITIVE_INSTALLER_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_GENERIC_COMMAND_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GENERIC_COMMAND_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_SXS_FILE_HASH_MISSING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SXS_FILE_HASH_MISSING)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTIONAL_CONFLICT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTIONAL_CONFLICT)));
	PyModule_AddObject(m, "NT_STATUS_INVALID_TRANSACTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INVALID_TRANSACTION)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_NOT_ACTIVE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_NOT_ACTIVE)));
	PyModule_AddObject(m, "NT_STATUS_TM_INITIALIZATION_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TM_INITIALIZATION_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_RM_NOT_ACTIVE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RM_NOT_ACTIVE)));
	PyModule_AddObject(m, "NT_STATUS_RM_METADATA_CORRUPT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RM_METADATA_CORRUPT)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_NOT_JOINED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_NOT_JOINED)));
	PyModule_AddObject(m, "NT_STATUS_DIRECTORY_NOT_RM", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_DIRECTORY_NOT_RM)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTIONS_UNSUPPORTED_REMOTE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTIONS_UNSUPPORTED_REMOTE)));
	PyModule_AddObject(m, "NT_STATUS_LOG_RESIZE_INVALID_SIZE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_RESIZE_INVALID_SIZE)));
	PyModule_AddObject(m, "NT_STATUS_REMOTE_FILE_VERSION_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_REMOTE_FILE_VERSION_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_CRM_PROTOCOL_ALREADY_EXISTS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CRM_PROTOCOL_ALREADY_EXISTS)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_PROPAGATION_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_PROPAGATION_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_CRM_PROTOCOL_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CRM_PROTOCOL_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_SUPERIOR_EXISTS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_SUPERIOR_EXISTS)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_REQUEST_NOT_VALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_REQUEST_NOT_VALID)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_NOT_REQUESTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_NOT_REQUESTED)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_ALREADY_ABORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_ALREADY_ABORTED)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_ALREADY_COMMITTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_ALREADY_COMMITTED)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_INVALID_MARSHALL_BUFFER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_INVALID_MARSHALL_BUFFER)));
	PyModule_AddObject(m, "NT_STATUS_CURRENT_TRANSACTION_NOT_VALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CURRENT_TRANSACTION_NOT_VALID)));
	PyModule_AddObject(m, "NT_STATUS_LOG_GROWTH_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_GROWTH_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_OBJECT_NO_LONGER_EXISTS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_OBJECT_NO_LONGER_EXISTS)));
	PyModule_AddObject(m, "NT_STATUS_STREAM_MINIVERSION_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_STREAM_MINIVERSION_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_STREAM_MINIVERSION_NOT_VALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_STREAM_MINIVERSION_NOT_VALID)));
	PyModule_AddObject(m, "NT_STATUS_MINIVERSION_INACCESSIBLE_FROM_SPECIFIED_TRANSACTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MINIVERSION_INACCESSIBLE_FROM_SPECIFIED_TRANSACTION)));
	PyModule_AddObject(m, "NT_STATUS_CANT_OPEN_MINIVERSION_WITH_MODIFY_INTENT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CANT_OPEN_MINIVERSION_WITH_MODIFY_INTENT)));
	PyModule_AddObject(m, "NT_STATUS_CANT_CREATE_MORE_STREAM_MINIVERSIONS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CANT_CREATE_MORE_STREAM_MINIVERSIONS)));
	PyModule_AddObject(m, "NT_STATUS_HANDLE_NO_LONGER_VALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_HANDLE_NO_LONGER_VALID)));
	PyModule_AddObject(m, "NT_STATUS_LOG_CORRUPTION_DETECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_CORRUPTION_DETECTED)));
	PyModule_AddObject(m, "NT_STATUS_RM_DISCONNECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RM_DISCONNECTED)));
	PyModule_AddObject(m, "NT_STATUS_ENLISTMENT_NOT_SUPERIOR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ENLISTMENT_NOT_SUPERIOR)));
	PyModule_AddObject(m, "NT_STATUS_FILE_IDENTITY_NOT_PERSISTENT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FILE_IDENTITY_NOT_PERSISTENT)));
	PyModule_AddObject(m, "NT_STATUS_CANT_BREAK_TRANSACTIONAL_DEPENDENCY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CANT_BREAK_TRANSACTIONAL_DEPENDENCY)));
	PyModule_AddObject(m, "NT_STATUS_CANT_CROSS_RM_BOUNDARY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CANT_CROSS_RM_BOUNDARY)));
	PyModule_AddObject(m, "NT_STATUS_TXF_DIR_NOT_EMPTY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TXF_DIR_NOT_EMPTY)));
	PyModule_AddObject(m, "NT_STATUS_INDOUBT_TRANSACTIONS_EXIST", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_INDOUBT_TRANSACTIONS_EXIST)));
	PyModule_AddObject(m, "NT_STATUS_TM_VOLATILE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TM_VOLATILE)));
	PyModule_AddObject(m, "NT_STATUS_ROLLBACK_TIMER_EXPIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ROLLBACK_TIMER_EXPIRED)));
	PyModule_AddObject(m, "NT_STATUS_TXF_ATTRIBUTE_CORRUPT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TXF_ATTRIBUTE_CORRUPT)));
	PyModule_AddObject(m, "NT_STATUS_EFS_NOT_ALLOWED_IN_TRANSACTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_EFS_NOT_ALLOWED_IN_TRANSACTION)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTIONAL_OPEN_NOT_ALLOWED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTIONAL_OPEN_NOT_ALLOWED)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTED_MAPPING_UNSUPPORTED_REMOTE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTED_MAPPING_UNSUPPORTED_REMOTE)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_REQUIRED_PROMOTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_REQUIRED_PROMOTION)));
	PyModule_AddObject(m, "NT_STATUS_CANNOT_EXECUTE_FILE_IN_TRANSACTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CANNOT_EXECUTE_FILE_IN_TRANSACTION)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTIONS_NOT_FROZEN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTIONS_NOT_FROZEN)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_FREEZE_IN_PROGRESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_FREEZE_IN_PROGRESS)));
	PyModule_AddObject(m, "NT_STATUS_NOT_SNAPSHOT_VOLUME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NOT_SNAPSHOT_VOLUME)));
	PyModule_AddObject(m, "NT_STATUS_NO_SAVEPOINT_WITH_OPEN_FILES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_SAVEPOINT_WITH_OPEN_FILES)));
	PyModule_AddObject(m, "NT_STATUS_SPARSE_NOT_ALLOWED_IN_TRANSACTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_SPARSE_NOT_ALLOWED_IN_TRANSACTION)));
	PyModule_AddObject(m, "NT_STATUS_TM_IDENTITY_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TM_IDENTITY_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_FLOATED_SECTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLOATED_SECTION)));
	PyModule_AddObject(m, "NT_STATUS_CANNOT_ACCEPT_TRANSACTED_WORK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CANNOT_ACCEPT_TRANSACTED_WORK)));
	PyModule_AddObject(m, "NT_STATUS_CANNOT_ABORT_TRANSACTIONS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_CANNOT_ABORT_TRANSACTIONS)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_RESOURCEMANAGER_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_RESOURCEMANAGER_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_ENLISTMENT_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_ENLISTMENT_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTIONMANAGER_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTIONMANAGER_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTIONMANAGER_NOT_ONLINE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTIONMANAGER_NOT_ONLINE)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTIONMANAGER_RECOVERY_NAME_COLLISION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTIONMANAGER_RECOVERY_NAME_COLLISION)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_NOT_ROOT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_NOT_ROOT)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_OBJECT_EXPIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_OBJECT_EXPIRED)));
	PyModule_AddObject(m, "NT_STATUS_COMPRESSION_NOT_ALLOWED_IN_TRANSACTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_COMPRESSION_NOT_ALLOWED_IN_TRANSACTION)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_RESPONSE_NOT_ENLISTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_RESPONSE_NOT_ENLISTED)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_RECORD_TOO_LONG", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_RECORD_TOO_LONG)));
	PyModule_AddObject(m, "NT_STATUS_NO_LINK_TRACKING_IN_TRANSACTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NO_LINK_TRACKING_IN_TRANSACTION)));
	PyModule_AddObject(m, "NT_STATUS_OPERATION_NOT_SUPPORTED_IN_TRANSACTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_OPERATION_NOT_SUPPORTED_IN_TRANSACTION)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_INTEGRITY_VIOLATED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_INTEGRITY_VIOLATED)));
	PyModule_AddObject(m, "NT_STATUS_EXPIRED_HANDLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_EXPIRED_HANDLE)));
	PyModule_AddObject(m, "NT_STATUS_TRANSACTION_NOT_ENLISTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_TRANSACTION_NOT_ENLISTED)));
	PyModule_AddObject(m, "NT_STATUS_LOG_SECTOR_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_SECTOR_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_LOG_SECTOR_PARITY_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_SECTOR_PARITY_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_LOG_SECTOR_REMAPPED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_SECTOR_REMAPPED)));
	PyModule_AddObject(m, "NT_STATUS_LOG_BLOCK_INCOMPLETE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_BLOCK_INCOMPLETE)));
	PyModule_AddObject(m, "NT_STATUS_LOG_INVALID_RANGE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_INVALID_RANGE)));
	PyModule_AddObject(m, "NT_STATUS_LOG_BLOCKS_EXHAUSTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_BLOCKS_EXHAUSTED)));
	PyModule_AddObject(m, "NT_STATUS_LOG_READ_CONTEXT_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_READ_CONTEXT_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_LOG_RESTART_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_RESTART_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_LOG_BLOCK_VERSION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_BLOCK_VERSION)));
	PyModule_AddObject(m, "NT_STATUS_LOG_BLOCK_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_BLOCK_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_LOG_READ_MODE_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_READ_MODE_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_LOG_METADATA_CORRUPT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_METADATA_CORRUPT)));
	PyModule_AddObject(m, "NT_STATUS_LOG_METADATA_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_METADATA_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_LOG_METADATA_INCONSISTENT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_METADATA_INCONSISTENT)));
	PyModule_AddObject(m, "NT_STATUS_LOG_RESERVATION_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_RESERVATION_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_LOG_CANT_DELETE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_CANT_DELETE)));
	PyModule_AddObject(m, "NT_STATUS_LOG_CONTAINER_LIMIT_EXCEEDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_CONTAINER_LIMIT_EXCEEDED)));
	PyModule_AddObject(m, "NT_STATUS_LOG_START_OF_LOG", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_START_OF_LOG)));
	PyModule_AddObject(m, "NT_STATUS_LOG_POLICY_ALREADY_INSTALLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_POLICY_ALREADY_INSTALLED)));
	PyModule_AddObject(m, "NT_STATUS_LOG_POLICY_NOT_INSTALLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_POLICY_NOT_INSTALLED)));
	PyModule_AddObject(m, "NT_STATUS_LOG_POLICY_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_POLICY_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_LOG_POLICY_CONFLICT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_POLICY_CONFLICT)));
	PyModule_AddObject(m, "NT_STATUS_LOG_PINNED_ARCHIVE_TAIL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_PINNED_ARCHIVE_TAIL)));
	PyModule_AddObject(m, "NT_STATUS_LOG_RECORD_NONEXISTENT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_RECORD_NONEXISTENT)));
	PyModule_AddObject(m, "NT_STATUS_LOG_RECORDS_RESERVED_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_RECORDS_RESERVED_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_LOG_SPACE_RESERVED_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_SPACE_RESERVED_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_LOG_TAIL_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_TAIL_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_LOG_FULL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_FULL)));
	PyModule_AddObject(m, "NT_STATUS_LOG_MULTIPLEXED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_MULTIPLEXED)));
	PyModule_AddObject(m, "NT_STATUS_LOG_DEDICATED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_DEDICATED)));
	PyModule_AddObject(m, "NT_STATUS_LOG_ARCHIVE_NOT_IN_PROGRESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_ARCHIVE_NOT_IN_PROGRESS)));
	PyModule_AddObject(m, "NT_STATUS_LOG_ARCHIVE_IN_PROGRESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_ARCHIVE_IN_PROGRESS)));
	PyModule_AddObject(m, "NT_STATUS_LOG_EPHEMERAL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_EPHEMERAL)));
	PyModule_AddObject(m, "NT_STATUS_LOG_NOT_ENOUGH_CONTAINERS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_NOT_ENOUGH_CONTAINERS)));
	PyModule_AddObject(m, "NT_STATUS_LOG_CLIENT_ALREADY_REGISTERED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_CLIENT_ALREADY_REGISTERED)));
	PyModule_AddObject(m, "NT_STATUS_LOG_CLIENT_NOT_REGISTERED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_CLIENT_NOT_REGISTERED)));
	PyModule_AddObject(m, "NT_STATUS_LOG_FULL_HANDLER_IN_PROGRESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_FULL_HANDLER_IN_PROGRESS)));
	PyModule_AddObject(m, "NT_STATUS_LOG_CONTAINER_READ_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_CONTAINER_READ_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_LOG_CONTAINER_WRITE_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_CONTAINER_WRITE_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_LOG_CONTAINER_OPEN_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_CONTAINER_OPEN_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_LOG_CONTAINER_STATE_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_CONTAINER_STATE_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_LOG_STATE_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_STATE_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_LOG_PINNED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_PINNED)));
	PyModule_AddObject(m, "NT_STATUS_LOG_METADATA_FLUSH_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_METADATA_FLUSH_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_LOG_INCONSISTENT_SECURITY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_INCONSISTENT_SECURITY)));
	PyModule_AddObject(m, "NT_STATUS_LOG_APPENDED_FLUSH_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_APPENDED_FLUSH_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_LOG_PINNED_RESERVATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_LOG_PINNED_RESERVATION)));
	PyModule_AddObject(m, "NT_STATUS_VIDEO_HUNG_DISPLAY_DRIVER_THREAD", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VIDEO_HUNG_DISPLAY_DRIVER_THREAD)));
	PyModule_AddObject(m, "NT_STATUS_FLT_NO_HANDLER_DEFINED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_NO_HANDLER_DEFINED)));
	PyModule_AddObject(m, "NT_STATUS_FLT_CONTEXT_ALREADY_DEFINED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_CONTEXT_ALREADY_DEFINED)));
	PyModule_AddObject(m, "NT_STATUS_FLT_INVALID_ASYNCHRONOUS_REQUEST", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_INVALID_ASYNCHRONOUS_REQUEST)));
	PyModule_AddObject(m, "NT_STATUS_FLT_DISALLOW_FAST_IO", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_DISALLOW_FAST_IO)));
	PyModule_AddObject(m, "NT_STATUS_FLT_INVALID_NAME_REQUEST", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_INVALID_NAME_REQUEST)));
	PyModule_AddObject(m, "NT_STATUS_FLT_NOT_SAFE_TO_POST_OPERATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_NOT_SAFE_TO_POST_OPERATION)));
	PyModule_AddObject(m, "NT_STATUS_FLT_NOT_INITIALIZED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_NOT_INITIALIZED)));
	PyModule_AddObject(m, "NT_STATUS_FLT_FILTER_NOT_READY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_FILTER_NOT_READY)));
	PyModule_AddObject(m, "NT_STATUS_FLT_POST_OPERATION_CLEANUP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_POST_OPERATION_CLEANUP)));
	PyModule_AddObject(m, "NT_STATUS_FLT_INTERNAL_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_INTERNAL_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_FLT_DELETING_OBJECT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_DELETING_OBJECT)));
	PyModule_AddObject(m, "NT_STATUS_FLT_MUST_BE_NONPAGED_POOL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_MUST_BE_NONPAGED_POOL)));
	PyModule_AddObject(m, "NT_STATUS_FLT_DUPLICATE_ENTRY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_DUPLICATE_ENTRY)));
	PyModule_AddObject(m, "NT_STATUS_FLT_CBDQ_DISABLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_CBDQ_DISABLED)));
	PyModule_AddObject(m, "NT_STATUS_FLT_DO_NOT_ATTACH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_DO_NOT_ATTACH)));
	PyModule_AddObject(m, "NT_STATUS_FLT_DO_NOT_DETACH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_DO_NOT_DETACH)));
	PyModule_AddObject(m, "NT_STATUS_FLT_INSTANCE_ALTITUDE_COLLISION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_INSTANCE_ALTITUDE_COLLISION)));
	PyModule_AddObject(m, "NT_STATUS_FLT_INSTANCE_NAME_COLLISION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_INSTANCE_NAME_COLLISION)));
	PyModule_AddObject(m, "NT_STATUS_FLT_FILTER_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_FILTER_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_FLT_VOLUME_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_VOLUME_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_FLT_INSTANCE_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_INSTANCE_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_FLT_CONTEXT_ALLOCATION_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_CONTEXT_ALLOCATION_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_FLT_INVALID_CONTEXT_REGISTRATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_INVALID_CONTEXT_REGISTRATION)));
	PyModule_AddObject(m, "NT_STATUS_FLT_NAME_CACHE_MISS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_NAME_CACHE_MISS)));
	PyModule_AddObject(m, "NT_STATUS_FLT_NO_DEVICE_OBJECT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_NO_DEVICE_OBJECT)));
	PyModule_AddObject(m, "NT_STATUS_FLT_VOLUME_ALREADY_MOUNTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_VOLUME_ALREADY_MOUNTED)));
	PyModule_AddObject(m, "NT_STATUS_FLT_ALREADY_ENLISTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_ALREADY_ENLISTED)));
	PyModule_AddObject(m, "NT_STATUS_FLT_CONTEXT_ALREADY_LINKED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_CONTEXT_ALREADY_LINKED)));
	PyModule_AddObject(m, "NT_STATUS_FLT_NO_WAITER_FOR_REPLY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FLT_NO_WAITER_FOR_REPLY)));
	PyModule_AddObject(m, "NT_STATUS_MONITOR_NO_DESCRIPTOR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MONITOR_NO_DESCRIPTOR)));
	PyModule_AddObject(m, "NT_STATUS_MONITOR_UNKNOWN_DESCRIPTOR_FORMAT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MONITOR_UNKNOWN_DESCRIPTOR_FORMAT)));
	PyModule_AddObject(m, "NT_STATUS_MONITOR_INVALID_DESCRIPTOR_CHECKSUM", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MONITOR_INVALID_DESCRIPTOR_CHECKSUM)));
	PyModule_AddObject(m, "NT_STATUS_MONITOR_INVALID_STANDARD_TIMING_BLOCK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MONITOR_INVALID_STANDARD_TIMING_BLOCK)));
	PyModule_AddObject(m, "NT_STATUS_MONITOR_WMI_DATABLOCK_REGISTRATION_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MONITOR_WMI_DATABLOCK_REGISTRATION_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_MONITOR_INVALID_SERIAL_NUMBER_MONDSC_BLOCK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MONITOR_INVALID_SERIAL_NUMBER_MONDSC_BLOCK)));
	PyModule_AddObject(m, "NT_STATUS_MONITOR_INVALID_USER_FRIENDLY_MONDSC_BLOCK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MONITOR_INVALID_USER_FRIENDLY_MONDSC_BLOCK)));
	PyModule_AddObject(m, "NT_STATUS_MONITOR_NO_MORE_DESCRIPTOR_DATA", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MONITOR_NO_MORE_DESCRIPTOR_DATA)));
	PyModule_AddObject(m, "NT_STATUS_MONITOR_INVALID_DETAILED_TIMING_BLOCK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MONITOR_INVALID_DETAILED_TIMING_BLOCK)));
	PyModule_AddObject(m, "NT_STATUS_MONITOR_INVALID_MANUFACTURE_DATE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_MONITOR_INVALID_MANUFACTURE_DATE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_NOT_EXCLUSIVE_MODE_OWNER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_NOT_EXCLUSIVE_MODE_OWNER)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INSUFFICIENT_DMA_BUFFER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INSUFFICIENT_DMA_BUFFER)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_DISPLAY_ADAPTER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_DISPLAY_ADAPTER)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_ADAPTER_WAS_RESET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_ADAPTER_WAS_RESET)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_DRIVER_MODEL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_DRIVER_MODEL)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_PRESENT_MODE_CHANGED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_PRESENT_MODE_CHANGED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_PRESENT_OCCLUDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_PRESENT_OCCLUDED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_PRESENT_DENIED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_PRESENT_DENIED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_CANNOTCOLORCONVERT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_CANNOTCOLORCONVERT)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_PRESENT_REDIRECTION_DISABLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_PRESENT_REDIRECTION_DISABLED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_PRESENT_UNOCCLUDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_PRESENT_UNOCCLUDED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_NO_VIDEO_MEMORY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_NO_VIDEO_MEMORY)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_CANT_LOCK_MEMORY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_CANT_LOCK_MEMORY)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_ALLOCATION_BUSY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_ALLOCATION_BUSY)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_TOO_MANY_REFERENCES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_TOO_MANY_REFERENCES)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_TRY_AGAIN_LATER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_TRY_AGAIN_LATER)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_TRY_AGAIN_NOW", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_TRY_AGAIN_NOW)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_ALLOCATION_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_ALLOCATION_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_UNSWIZZLING_APERTURE_UNAVAILABLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_UNSWIZZLING_APERTURE_UNAVAILABLE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_UNSWIZZLING_APERTURE_UNSUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_UNSWIZZLING_APERTURE_UNSUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_CANT_EVICT_PINNED_ALLOCATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_CANT_EVICT_PINNED_ALLOCATION)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_ALLOCATION_USAGE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_ALLOCATION_USAGE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_CANT_RENDER_LOCKED_ALLOCATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_CANT_RENDER_LOCKED_ALLOCATION)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_ALLOCATION_CLOSED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_ALLOCATION_CLOSED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_ALLOCATION_INSTANCE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_ALLOCATION_INSTANCE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_ALLOCATION_HANDLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_ALLOCATION_HANDLE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_WRONG_ALLOCATION_DEVICE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_WRONG_ALLOCATION_DEVICE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_ALLOCATION_CONTENT_LOST", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_ALLOCATION_CONTENT_LOST)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_GPU_EXCEPTION_ON_DEVICE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_GPU_EXCEPTION_ON_DEVICE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_VIDPN_TOPOLOGY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_VIDPN_TOPOLOGY)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_VIDPN_TOPOLOGY_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_VIDPN_TOPOLOGY_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_VIDPN_TOPOLOGY_CURRENTLY_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_VIDPN_TOPOLOGY_CURRENTLY_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_VIDPN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_VIDPN)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_VIDEO_PRESENT_SOURCE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_VIDEO_PRESENT_SOURCE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_VIDEO_PRESENT_TARGET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_VIDEO_PRESENT_TARGET)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_VIDPN_MODALITY_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_VIDPN_MODALITY_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_VIDPN_SOURCEMODESET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_VIDPN_SOURCEMODESET)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_VIDPN_TARGETMODESET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_VIDPN_TARGETMODESET)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_FREQUENCY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_FREQUENCY)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_ACTIVE_REGION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_ACTIVE_REGION)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_TOTAL_REGION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_TOTAL_REGION)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_VIDEO_PRESENT_SOURCE_MODE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_VIDEO_PRESENT_SOURCE_MODE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_VIDEO_PRESENT_TARGET_MODE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_VIDEO_PRESENT_TARGET_MODE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_PINNED_MODE_MUST_REMAIN_IN_SET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_PINNED_MODE_MUST_REMAIN_IN_SET)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_PATH_ALREADY_IN_TOPOLOGY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_PATH_ALREADY_IN_TOPOLOGY)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_MODE_ALREADY_IN_MODESET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_MODE_ALREADY_IN_MODESET)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_VIDEOPRESENTSOURCESET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_VIDEOPRESENTSOURCESET)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_VIDEOPRESENTTARGETSET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_VIDEOPRESENTTARGETSET)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_SOURCE_ALREADY_IN_SET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_SOURCE_ALREADY_IN_SET)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_TARGET_ALREADY_IN_SET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_TARGET_ALREADY_IN_SET)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_VIDPN_PRESENT_PATH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_VIDPN_PRESENT_PATH)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_NO_RECOMMENDED_VIDPN_TOPOLOGY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_NO_RECOMMENDED_VIDPN_TOPOLOGY)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_MONITOR_FREQUENCYRANGESET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_MONITOR_FREQUENCYRANGESET)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_MONITOR_FREQUENCYRANGE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_MONITOR_FREQUENCYRANGE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_FREQUENCYRANGE_NOT_IN_SET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_FREQUENCYRANGE_NOT_IN_SET)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_FREQUENCYRANGE_ALREADY_IN_SET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_FREQUENCYRANGE_ALREADY_IN_SET)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_STALE_MODESET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_STALE_MODESET)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_MONITOR_SOURCEMODESET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_MONITOR_SOURCEMODESET)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_MONITOR_SOURCE_MODE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_MONITOR_SOURCE_MODE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_NO_RECOMMENDED_FUNCTIONAL_VIDPN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_NO_RECOMMENDED_FUNCTIONAL_VIDPN)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_MODE_ID_MUST_BE_UNIQUE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_MODE_ID_MUST_BE_UNIQUE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_EMPTY_ADAPTER_MONITOR_MODE_SUPPORT_INTERSECTION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_EMPTY_ADAPTER_MONITOR_MODE_SUPPORT_INTERSECTION)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_VIDEO_PRESENT_TARGETS_LESS_THAN_SOURCES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_VIDEO_PRESENT_TARGETS_LESS_THAN_SOURCES)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_PATH_NOT_IN_TOPOLOGY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_PATH_NOT_IN_TOPOLOGY)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_ADAPTER_MUST_HAVE_AT_LEAST_ONE_SOURCE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_ADAPTER_MUST_HAVE_AT_LEAST_ONE_SOURCE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_ADAPTER_MUST_HAVE_AT_LEAST_ONE_TARGET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_ADAPTER_MUST_HAVE_AT_LEAST_ONE_TARGET)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_MONITORDESCRIPTORSET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_MONITORDESCRIPTORSET)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_MONITORDESCRIPTOR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_MONITORDESCRIPTOR)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_MONITORDESCRIPTOR_NOT_IN_SET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_MONITORDESCRIPTOR_NOT_IN_SET)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_MONITORDESCRIPTOR_ALREADY_IN_SET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_MONITORDESCRIPTOR_ALREADY_IN_SET)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_MONITORDESCRIPTOR_ID_MUST_BE_UNIQUE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_MONITORDESCRIPTOR_ID_MUST_BE_UNIQUE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_VIDPN_TARGET_SUBSET_TYPE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_VIDPN_TARGET_SUBSET_TYPE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_RESOURCES_NOT_RELATED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_RESOURCES_NOT_RELATED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_SOURCE_ID_MUST_BE_UNIQUE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_SOURCE_ID_MUST_BE_UNIQUE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_TARGET_ID_MUST_BE_UNIQUE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_TARGET_ID_MUST_BE_UNIQUE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_NO_AVAILABLE_VIDPN_TARGET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_NO_AVAILABLE_VIDPN_TARGET)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_MONITOR_COULD_NOT_BE_ASSOCIATED_WITH_ADAPTER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_MONITOR_COULD_NOT_BE_ASSOCIATED_WITH_ADAPTER)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_NO_VIDPNMGR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_NO_VIDPNMGR)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_NO_ACTIVE_VIDPN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_NO_ACTIVE_VIDPN)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_STALE_VIDPN_TOPOLOGY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_STALE_VIDPN_TOPOLOGY)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_MONITOR_NOT_CONNECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_MONITOR_NOT_CONNECTED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_SOURCE_NOT_IN_TOPOLOGY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_SOURCE_NOT_IN_TOPOLOGY)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_PRIMARYSURFACE_SIZE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_PRIMARYSURFACE_SIZE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_VISIBLEREGION_SIZE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_VISIBLEREGION_SIZE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_STRIDE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_STRIDE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_PIXELFORMAT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_PIXELFORMAT)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_COLORBASIS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_COLORBASIS)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_PIXELVALUEACCESSMODE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_PIXELVALUEACCESSMODE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_TARGET_NOT_IN_TOPOLOGY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_TARGET_NOT_IN_TOPOLOGY)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_NO_DISPLAY_MODE_MANAGEMENT_SUPPORT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_NO_DISPLAY_MODE_MANAGEMENT_SUPPORT)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_VIDPN_SOURCE_IN_USE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_VIDPN_SOURCE_IN_USE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_CANT_ACCESS_ACTIVE_VIDPN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_CANT_ACCESS_ACTIVE_VIDPN)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_PATH_IMPORTANCE_ORDINAL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_PATH_IMPORTANCE_ORDINAL)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_PATH_CONTENT_GEOMETRY_TRANSFORMATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_PATH_CONTENT_GEOMETRY_TRANSFORMATION)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_PATH_CONTENT_GEOMETRY_TRANSFORMATION_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_PATH_CONTENT_GEOMETRY_TRANSFORMATION_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_GAMMA_RAMP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_GAMMA_RAMP)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_GAMMA_RAMP_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_GAMMA_RAMP_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_MULTISAMPLING_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_MULTISAMPLING_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_MODE_NOT_IN_MODESET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_MODE_NOT_IN_MODESET)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_VIDPN_TOPOLOGY_RECOMMENDATION_REASON", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_VIDPN_TOPOLOGY_RECOMMENDATION_REASON)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_PATH_CONTENT_TYPE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_PATH_CONTENT_TYPE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_COPYPROTECTION_TYPE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_COPYPROTECTION_TYPE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_UNASSIGNED_MODESET_ALREADY_EXISTS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_UNASSIGNED_MODESET_ALREADY_EXISTS)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_SCANLINE_ORDERING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_SCANLINE_ORDERING)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_TOPOLOGY_CHANGES_NOT_ALLOWED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_TOPOLOGY_CHANGES_NOT_ALLOWED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_NO_AVAILABLE_IMPORTANCE_ORDINALS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_NO_AVAILABLE_IMPORTANCE_ORDINALS)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INCOMPATIBLE_PRIVATE_FORMAT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INCOMPATIBLE_PRIVATE_FORMAT)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_MODE_PRUNING_ALGORITHM", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_MODE_PRUNING_ALGORITHM)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_MONITOR_CAPABILITY_ORIGIN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_MONITOR_CAPABILITY_ORIGIN)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_MONITOR_FREQUENCYRANGE_CONSTRAINT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_MONITOR_FREQUENCYRANGE_CONSTRAINT)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_MAX_NUM_PATHS_REACHED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_MAX_NUM_PATHS_REACHED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_CANCEL_VIDPN_TOPOLOGY_AUGMENTATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_CANCEL_VIDPN_TOPOLOGY_AUGMENTATION)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_CLIENT_TYPE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_CLIENT_TYPE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_CLIENTVIDPN_NOT_SET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_CLIENTVIDPN_NOT_SET)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_SPECIFIED_CHILD_ALREADY_CONNECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_SPECIFIED_CHILD_ALREADY_CONNECTED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_CHILD_DESCRIPTOR_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_CHILD_DESCRIPTOR_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_NOT_A_LINKED_ADAPTER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_NOT_A_LINKED_ADAPTER)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_LEADLINK_NOT_ENUMERATED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_LEADLINK_NOT_ENUMERATED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_CHAINLINKS_NOT_ENUMERATED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_CHAINLINKS_NOT_ENUMERATED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_ADAPTER_CHAIN_NOT_READY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_ADAPTER_CHAIN_NOT_READY)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_CHAINLINKS_NOT_STARTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_CHAINLINKS_NOT_STARTED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_CHAINLINKS_NOT_POWERED_ON", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_CHAINLINKS_NOT_POWERED_ON)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INCONSISTENT_DEVICE_LINK_STATE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INCONSISTENT_DEVICE_LINK_STATE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_NOT_POST_DEVICE_DRIVER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_NOT_POST_DEVICE_DRIVER)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_ADAPTER_ACCESS_NOT_EXCLUDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_ADAPTER_ACCESS_NOT_EXCLUDED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_OPM_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_OPM_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_COPP_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_COPP_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_UAB_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_UAB_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_OPM_INVALID_ENCRYPTED_PARAMETERS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_OPM_INVALID_ENCRYPTED_PARAMETERS)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_OPM_PARAMETER_ARRAY_TOO_SMALL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_OPM_PARAMETER_ARRAY_TOO_SMALL)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_OPM_NO_PROTECTED_OUTPUTS_EXIST", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_OPM_NO_PROTECTED_OUTPUTS_EXIST)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_PVP_NO_DISPLAY_DEVICE_CORRESPONDS_TO_NAME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_PVP_NO_DISPLAY_DEVICE_CORRESPONDS_TO_NAME)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_PVP_DISPLAY_DEVICE_NOT_ATTACHED_TO_DESKTOP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_PVP_DISPLAY_DEVICE_NOT_ATTACHED_TO_DESKTOP)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_PVP_MIRRORING_DEVICES_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_PVP_MIRRORING_DEVICES_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_OPM_INVALID_POINTER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_OPM_INVALID_POINTER)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_OPM_INTERNAL_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_OPM_INTERNAL_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_OPM_INVALID_HANDLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_OPM_INVALID_HANDLE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_PVP_NO_MONITORS_CORRESPOND_TO_DISPLAY_DEVICE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_PVP_NO_MONITORS_CORRESPOND_TO_DISPLAY_DEVICE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_PVP_INVALID_CERTIFICATE_LENGTH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_PVP_INVALID_CERTIFICATE_LENGTH)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_OPM_SPANNING_MODE_ENABLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_OPM_SPANNING_MODE_ENABLED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_OPM_THEATER_MODE_ENABLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_OPM_THEATER_MODE_ENABLED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_PVP_HFS_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_PVP_HFS_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_OPM_INVALID_SRM", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_OPM_INVALID_SRM)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_OPM_OUTPUT_DOES_NOT_SUPPORT_HDCP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_OPM_OUTPUT_DOES_NOT_SUPPORT_HDCP)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_OPM_OUTPUT_DOES_NOT_SUPPORT_ACP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_OPM_OUTPUT_DOES_NOT_SUPPORT_ACP)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_OPM_OUTPUT_DOES_NOT_SUPPORT_CGMSA", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_OPM_OUTPUT_DOES_NOT_SUPPORT_CGMSA)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_OPM_HDCP_SRM_NEVER_SET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_OPM_HDCP_SRM_NEVER_SET)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_OPM_RESOLUTION_TOO_HIGH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_OPM_RESOLUTION_TOO_HIGH)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_OPM_ALL_HDCP_HARDWARE_ALREADY_IN_USE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_OPM_ALL_HDCP_HARDWARE_ALREADY_IN_USE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_OPM_PROTECTED_OUTPUT_NO_LONGER_EXISTS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_OPM_PROTECTED_OUTPUT_NO_LONGER_EXISTS)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_OPM_SESSION_TYPE_CHANGE_IN_PROGRESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_OPM_SESSION_TYPE_CHANGE_IN_PROGRESS)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_OPM_PROTECTED_OUTPUT_DOES_NOT_HAVE_COPP_SEMANTICS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_OPM_PROTECTED_OUTPUT_DOES_NOT_HAVE_COPP_SEMANTICS)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_OPM_INVALID_INFORMATION_REQUEST", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_OPM_INVALID_INFORMATION_REQUEST)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_OPM_DRIVER_INTERNAL_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_OPM_DRIVER_INTERNAL_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_OPM_PROTECTED_OUTPUT_DOES_NOT_HAVE_OPM_SEMANTICS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_OPM_PROTECTED_OUTPUT_DOES_NOT_HAVE_OPM_SEMANTICS)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_OPM_SIGNALING_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_OPM_SIGNALING_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_OPM_INVALID_CONFIGURATION_REQUEST", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_OPM_INVALID_CONFIGURATION_REQUEST)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_I2C_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_I2C_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_I2C_DEVICE_DOES_NOT_EXIST", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_I2C_DEVICE_DOES_NOT_EXIST)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_I2C_ERROR_TRANSMITTING_DATA", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_I2C_ERROR_TRANSMITTING_DATA)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_I2C_ERROR_RECEIVING_DATA", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_I2C_ERROR_RECEIVING_DATA)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_DDCCI_VCP_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_DDCCI_VCP_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_DDCCI_INVALID_DATA", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_DDCCI_INVALID_DATA)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_DDCCI_MONITOR_RETURNED_INVALID_TIMING_STATUS_BYTE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_DDCCI_MONITOR_RETURNED_INVALID_TIMING_STATUS_BYTE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_DDCCI_INVALID_CAPABILITIES_STRING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_DDCCI_INVALID_CAPABILITIES_STRING)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_MCA_INTERNAL_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_MCA_INTERNAL_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_DDCCI_INVALID_MESSAGE_COMMAND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_DDCCI_INVALID_MESSAGE_COMMAND)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_DDCCI_INVALID_MESSAGE_LENGTH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_DDCCI_INVALID_MESSAGE_LENGTH)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_DDCCI_INVALID_MESSAGE_CHECKSUM", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_DDCCI_INVALID_MESSAGE_CHECKSUM)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_PHYSICAL_MONITOR_HANDLE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_PHYSICAL_MONITOR_HANDLE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_MONITOR_NO_LONGER_EXISTS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_MONITOR_NO_LONGER_EXISTS)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_ONLY_CONSOLE_SESSION_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_ONLY_CONSOLE_SESSION_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_NO_DISPLAY_DEVICE_CORRESPONDS_TO_NAME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_NO_DISPLAY_DEVICE_CORRESPONDS_TO_NAME)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_DISPLAY_DEVICE_NOT_ATTACHED_TO_DESKTOP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_DISPLAY_DEVICE_NOT_ATTACHED_TO_DESKTOP)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_MIRRORING_DEVICES_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_MIRRORING_DEVICES_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INVALID_POINTER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INVALID_POINTER)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_NO_MONITORS_CORRESPOND_TO_DISPLAY_DEVICE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_NO_MONITORS_CORRESPOND_TO_DISPLAY_DEVICE)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_PARAMETER_ARRAY_TOO_SMALL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_PARAMETER_ARRAY_TOO_SMALL)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_INTERNAL_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_INTERNAL_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_GRAPHICS_SESSION_TYPE_CHANGE_IN_PROGRESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_GRAPHICS_SESSION_TYPE_CHANGE_IN_PROGRESS)));
	PyModule_AddObject(m, "NT_STATUS_FVE_LOCKED_VOLUME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_LOCKED_VOLUME)));
	PyModule_AddObject(m, "NT_STATUS_FVE_NOT_ENCRYPTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_NOT_ENCRYPTED)));
	PyModule_AddObject(m, "NT_STATUS_FVE_BAD_INFORMATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_BAD_INFORMATION)));
	PyModule_AddObject(m, "NT_STATUS_FVE_TOO_SMALL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_TOO_SMALL)));
	PyModule_AddObject(m, "NT_STATUS_FVE_FAILED_WRONG_FS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_FAILED_WRONG_FS)));
	PyModule_AddObject(m, "NT_STATUS_FVE_FAILED_BAD_FS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_FAILED_BAD_FS)));
	PyModule_AddObject(m, "NT_STATUS_FVE_FS_NOT_EXTENDED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_FS_NOT_EXTENDED)));
	PyModule_AddObject(m, "NT_STATUS_FVE_FS_MOUNTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_FS_MOUNTED)));
	PyModule_AddObject(m, "NT_STATUS_FVE_NO_LICENSE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_NO_LICENSE)));
	PyModule_AddObject(m, "NT_STATUS_FVE_ACTION_NOT_ALLOWED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_ACTION_NOT_ALLOWED)));
	PyModule_AddObject(m, "NT_STATUS_FVE_BAD_DATA", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_BAD_DATA)));
	PyModule_AddObject(m, "NT_STATUS_FVE_VOLUME_NOT_BOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_VOLUME_NOT_BOUND)));
	PyModule_AddObject(m, "NT_STATUS_FVE_NOT_DATA_VOLUME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_NOT_DATA_VOLUME)));
	PyModule_AddObject(m, "NT_STATUS_FVE_CONV_READ_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_CONV_READ_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_FVE_CONV_WRITE_ERROR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_CONV_WRITE_ERROR)));
	PyModule_AddObject(m, "NT_STATUS_FVE_OVERLAPPED_UPDATE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_OVERLAPPED_UPDATE)));
	PyModule_AddObject(m, "NT_STATUS_FVE_FAILED_SECTOR_SIZE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_FAILED_SECTOR_SIZE)));
	PyModule_AddObject(m, "NT_STATUS_FVE_FAILED_AUTHENTICATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_FAILED_AUTHENTICATION)));
	PyModule_AddObject(m, "NT_STATUS_FVE_NOT_OS_VOLUME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_NOT_OS_VOLUME)));
	PyModule_AddObject(m, "NT_STATUS_FVE_KEYFILE_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_KEYFILE_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_FVE_KEYFILE_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_KEYFILE_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_FVE_KEYFILE_NO_VMK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_KEYFILE_NO_VMK)));
	PyModule_AddObject(m, "NT_STATUS_FVE_TPM_DISABLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_TPM_DISABLED)));
	PyModule_AddObject(m, "NT_STATUS_FVE_TPM_SRK_AUTH_NOT_ZERO", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_TPM_SRK_AUTH_NOT_ZERO)));
	PyModule_AddObject(m, "NT_STATUS_FVE_TPM_INVALID_PCR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_TPM_INVALID_PCR)));
	PyModule_AddObject(m, "NT_STATUS_FVE_TPM_NO_VMK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_TPM_NO_VMK)));
	PyModule_AddObject(m, "NT_STATUS_FVE_PIN_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_PIN_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_FVE_AUTH_INVALID_APPLICATION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_AUTH_INVALID_APPLICATION)));
	PyModule_AddObject(m, "NT_STATUS_FVE_AUTH_INVALID_CONFIG", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_AUTH_INVALID_CONFIG)));
	PyModule_AddObject(m, "NT_STATUS_FVE_DEBUGGER_ENABLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_DEBUGGER_ENABLED)));
	PyModule_AddObject(m, "NT_STATUS_FVE_DRY_RUN_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_DRY_RUN_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_FVE_BAD_METADATA_POINTER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_BAD_METADATA_POINTER)));
	PyModule_AddObject(m, "NT_STATUS_FVE_OLD_METADATA_COPY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_OLD_METADATA_COPY)));
	PyModule_AddObject(m, "NT_STATUS_FVE_REBOOT_REQUIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_REBOOT_REQUIRED)));
	PyModule_AddObject(m, "NT_STATUS_FVE_RAW_ACCESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_RAW_ACCESS)));
	PyModule_AddObject(m, "NT_STATUS_FVE_RAW_BLOCKED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_RAW_BLOCKED)));
	PyModule_AddObject(m, "NT_STATUS_FVE_NO_FEATURE_LICENSE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_NO_FEATURE_LICENSE)));
	PyModule_AddObject(m, "NT_STATUS_FVE_POLICY_USER_DISABLE_RDV_NOT_ALLOWED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_POLICY_USER_DISABLE_RDV_NOT_ALLOWED)));
	PyModule_AddObject(m, "NT_STATUS_FVE_CONV_RECOVERY_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_CONV_RECOVERY_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_FVE_VIRTUALIZED_SPACE_TOO_BIG", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_VIRTUALIZED_SPACE_TOO_BIG)));
	PyModule_AddObject(m, "NT_STATUS_FVE_VOLUME_TOO_SMALL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FVE_VOLUME_TOO_SMALL)));
	PyModule_AddObject(m, "NT_STATUS_FWP_CALLOUT_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_CALLOUT_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_FWP_CONDITION_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_CONDITION_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_FWP_FILTER_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_FILTER_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_FWP_LAYER_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_LAYER_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_FWP_PROVIDER_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_PROVIDER_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_FWP_PROVIDER_CONTEXT_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_PROVIDER_CONTEXT_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_FWP_SUBLAYER_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_SUBLAYER_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_FWP_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_FWP_ALREADY_EXISTS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_ALREADY_EXISTS)));
	PyModule_AddObject(m, "NT_STATUS_FWP_IN_USE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_IN_USE)));
	PyModule_AddObject(m, "NT_STATUS_FWP_DYNAMIC_SESSION_IN_PROGRESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_DYNAMIC_SESSION_IN_PROGRESS)));
	PyModule_AddObject(m, "NT_STATUS_FWP_WRONG_SESSION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_WRONG_SESSION)));
	PyModule_AddObject(m, "NT_STATUS_FWP_NO_TXN_IN_PROGRESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_NO_TXN_IN_PROGRESS)));
	PyModule_AddObject(m, "NT_STATUS_FWP_TXN_IN_PROGRESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_TXN_IN_PROGRESS)));
	PyModule_AddObject(m, "NT_STATUS_FWP_TXN_ABORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_TXN_ABORTED)));
	PyModule_AddObject(m, "NT_STATUS_FWP_SESSION_ABORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_SESSION_ABORTED)));
	PyModule_AddObject(m, "NT_STATUS_FWP_INCOMPATIBLE_TXN", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_INCOMPATIBLE_TXN)));
	PyModule_AddObject(m, "NT_STATUS_FWP_TIMEOUT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_TIMEOUT)));
	PyModule_AddObject(m, "NT_STATUS_FWP_NET_EVENTS_DISABLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_NET_EVENTS_DISABLED)));
	PyModule_AddObject(m, "NT_STATUS_FWP_INCOMPATIBLE_LAYER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_INCOMPATIBLE_LAYER)));
	PyModule_AddObject(m, "NT_STATUS_FWP_KM_CLIENTS_ONLY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_KM_CLIENTS_ONLY)));
	PyModule_AddObject(m, "NT_STATUS_FWP_LIFETIME_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_LIFETIME_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_FWP_BUILTIN_OBJECT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_BUILTIN_OBJECT)));
	PyModule_AddObject(m, "NT_STATUS_FWP_TOO_MANY_BOOTTIME_FILTERS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_TOO_MANY_BOOTTIME_FILTERS)));
	PyModule_AddObject(m, "NT_STATUS_FWP_TOO_MANY_CALLOUTS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_TOO_MANY_CALLOUTS)));
	PyModule_AddObject(m, "NT_STATUS_FWP_NOTIFICATION_DROPPED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_NOTIFICATION_DROPPED)));
	PyModule_AddObject(m, "NT_STATUS_FWP_TRAFFIC_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_TRAFFIC_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_FWP_INCOMPATIBLE_SA_STATE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_INCOMPATIBLE_SA_STATE)));
	PyModule_AddObject(m, "NT_STATUS_FWP_NULL_POINTER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_NULL_POINTER)));
	PyModule_AddObject(m, "NT_STATUS_FWP_INVALID_ENUMERATOR", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_INVALID_ENUMERATOR)));
	PyModule_AddObject(m, "NT_STATUS_FWP_INVALID_FLAGS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_INVALID_FLAGS)));
	PyModule_AddObject(m, "NT_STATUS_FWP_INVALID_NET_MASK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_INVALID_NET_MASK)));
	PyModule_AddObject(m, "NT_STATUS_FWP_INVALID_RANGE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_INVALID_RANGE)));
	PyModule_AddObject(m, "NT_STATUS_FWP_INVALID_INTERVAL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_INVALID_INTERVAL)));
	PyModule_AddObject(m, "NT_STATUS_FWP_ZERO_LENGTH_ARRAY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_ZERO_LENGTH_ARRAY)));
	PyModule_AddObject(m, "NT_STATUS_FWP_NULL_DISPLAY_NAME", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_NULL_DISPLAY_NAME)));
	PyModule_AddObject(m, "NT_STATUS_FWP_INVALID_ACTION_TYPE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_INVALID_ACTION_TYPE)));
	PyModule_AddObject(m, "NT_STATUS_FWP_INVALID_WEIGHT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_INVALID_WEIGHT)));
	PyModule_AddObject(m, "NT_STATUS_FWP_MATCH_TYPE_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_MATCH_TYPE_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_FWP_TYPE_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_TYPE_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_FWP_OUT_OF_BOUNDS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_OUT_OF_BOUNDS)));
	PyModule_AddObject(m, "NT_STATUS_FWP_RESERVED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_RESERVED)));
	PyModule_AddObject(m, "NT_STATUS_FWP_DUPLICATE_CONDITION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_DUPLICATE_CONDITION)));
	PyModule_AddObject(m, "NT_STATUS_FWP_DUPLICATE_KEYMOD", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_DUPLICATE_KEYMOD)));
	PyModule_AddObject(m, "NT_STATUS_FWP_ACTION_INCOMPATIBLE_WITH_LAYER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_ACTION_INCOMPATIBLE_WITH_LAYER)));
	PyModule_AddObject(m, "NT_STATUS_FWP_ACTION_INCOMPATIBLE_WITH_SUBLAYER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_ACTION_INCOMPATIBLE_WITH_SUBLAYER)));
	PyModule_AddObject(m, "NT_STATUS_FWP_CONTEXT_INCOMPATIBLE_WITH_LAYER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_CONTEXT_INCOMPATIBLE_WITH_LAYER)));
	PyModule_AddObject(m, "NT_STATUS_FWP_CONTEXT_INCOMPATIBLE_WITH_CALLOUT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_CONTEXT_INCOMPATIBLE_WITH_CALLOUT)));
	PyModule_AddObject(m, "NT_STATUS_FWP_INCOMPATIBLE_AUTH_METHOD", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_INCOMPATIBLE_AUTH_METHOD)));
	PyModule_AddObject(m, "NT_STATUS_FWP_INCOMPATIBLE_DH_GROUP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_INCOMPATIBLE_DH_GROUP)));
	PyModule_AddObject(m, "NT_STATUS_FWP_EM_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_EM_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_FWP_NEVER_MATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_NEVER_MATCH)));
	PyModule_AddObject(m, "NT_STATUS_FWP_PROVIDER_CONTEXT_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_PROVIDER_CONTEXT_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_FWP_INVALID_PARAMETER", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_INVALID_PARAMETER)));
	PyModule_AddObject(m, "NT_STATUS_FWP_TOO_MANY_SUBLAYERS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_TOO_MANY_SUBLAYERS)));
	PyModule_AddObject(m, "NT_STATUS_FWP_CALLOUT_NOTIFICATION_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_CALLOUT_NOTIFICATION_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_FWP_INCOMPATIBLE_AUTH_CONFIG", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_INCOMPATIBLE_AUTH_CONFIG)));
	PyModule_AddObject(m, "NT_STATUS_FWP_INCOMPATIBLE_CIPHER_CONFIG", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_INCOMPATIBLE_CIPHER_CONFIG)));
	PyModule_AddObject(m, "NT_STATUS_FWP_DUPLICATE_AUTH_METHOD", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_DUPLICATE_AUTH_METHOD)));
	PyModule_AddObject(m, "NT_STATUS_FWP_TCPIP_NOT_READY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_TCPIP_NOT_READY)));
	PyModule_AddObject(m, "NT_STATUS_FWP_INJECT_HANDLE_CLOSING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_INJECT_HANDLE_CLOSING)));
	PyModule_AddObject(m, "NT_STATUS_FWP_INJECT_HANDLE_STALE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_INJECT_HANDLE_STALE)));
	PyModule_AddObject(m, "NT_STATUS_FWP_CANNOT_PEND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_FWP_CANNOT_PEND)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_CLOSING", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_CLOSING)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_BAD_VERSION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_BAD_VERSION)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_BAD_CHARACTERISTICS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_BAD_CHARACTERISTICS)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_ADAPTER_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_ADAPTER_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_OPEN_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_OPEN_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_DEVICE_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_DEVICE_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_MULTICAST_FULL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_MULTICAST_FULL)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_MULTICAST_EXISTS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_MULTICAST_EXISTS)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_MULTICAST_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_MULTICAST_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_REQUEST_ABORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_REQUEST_ABORTED)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_RESET_IN_PROGRESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_RESET_IN_PROGRESS)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_INVALID_PACKET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_INVALID_PACKET)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_INVALID_DEVICE_REQUEST", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_INVALID_DEVICE_REQUEST)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_ADAPTER_NOT_READY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_ADAPTER_NOT_READY)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_INVALID_LENGTH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_INVALID_LENGTH)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_INVALID_DATA", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_INVALID_DATA)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_BUFFER_TOO_SHORT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_BUFFER_TOO_SHORT)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_INVALID_OID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_INVALID_OID)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_ADAPTER_REMOVED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_ADAPTER_REMOVED)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_UNSUPPORTED_MEDIA", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_UNSUPPORTED_MEDIA)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_GROUP_ADDRESS_IN_USE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_GROUP_ADDRESS_IN_USE)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_FILE_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_FILE_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_ERROR_READING_FILE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_ERROR_READING_FILE)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_ALREADY_MAPPED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_ALREADY_MAPPED)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_RESOURCE_CONFLICT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_RESOURCE_CONFLICT)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_MEDIA_DISCONNECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_MEDIA_DISCONNECTED)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_INVALID_ADDRESS", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_INVALID_ADDRESS)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_PAUSED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_PAUSED)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_INTERFACE_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_INTERFACE_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_UNSUPPORTED_REVISION", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_UNSUPPORTED_REVISION)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_INVALID_PORT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_INVALID_PORT)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_INVALID_PORT_STATE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_INVALID_PORT_STATE)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_LOW_POWER_STATE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_LOW_POWER_STATE)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_OFFLOAD_POLICY", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_OFFLOAD_POLICY)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_OFFLOAD_CONNECTION_REJECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_OFFLOAD_CONNECTION_REJECTED)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_OFFLOAD_PATH_REJECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_OFFLOAD_PATH_REJECTED)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_DOT11_AUTO_CONFIG_ENABLED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_DOT11_AUTO_CONFIG_ENABLED)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_DOT11_MEDIA_IN_USE", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_DOT11_MEDIA_IN_USE)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_DOT11_POWER_STATE_INVALID", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_DOT11_POWER_STATE_INVALID)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_PM_WOL_PATTERN_LIST_FULL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_PM_WOL_PATTERN_LIST_FULL)));
	PyModule_AddObject(m, "NT_STATUS_NDIS_PM_PROTOCOL_OFFLOAD_LIST_FULL", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_NDIS_PM_PROTOCOL_OFFLOAD_LIST_FULL)));
	PyModule_AddObject(m, "NT_STATUS_IPSEC_BAD_SPI", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IPSEC_BAD_SPI)));
	PyModule_AddObject(m, "NT_STATUS_IPSEC_SA_LIFETIME_EXPIRED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IPSEC_SA_LIFETIME_EXPIRED)));
	PyModule_AddObject(m, "NT_STATUS_IPSEC_WRONG_SA", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IPSEC_WRONG_SA)));
	PyModule_AddObject(m, "NT_STATUS_IPSEC_REPLAY_CHECK_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IPSEC_REPLAY_CHECK_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_IPSEC_INVALID_PACKET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IPSEC_INVALID_PACKET)));
	PyModule_AddObject(m, "NT_STATUS_IPSEC_INTEGRITY_CHECK_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IPSEC_INTEGRITY_CHECK_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_IPSEC_CLEAR_TEXT_DROP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IPSEC_CLEAR_TEXT_DROP)));
	PyModule_AddObject(m, "NT_STATUS_IPSEC_AUTH_FIREWALL_DROP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IPSEC_AUTH_FIREWALL_DROP)));
	PyModule_AddObject(m, "NT_STATUS_IPSEC_THROTTLE_DROP", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IPSEC_THROTTLE_DROP)));
	PyModule_AddObject(m, "NT_STATUS_IPSEC_DOSP_BLOCK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IPSEC_DOSP_BLOCK)));
	PyModule_AddObject(m, "NT_STATUS_IPSEC_DOSP_RECEIVED_MULTICAST", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IPSEC_DOSP_RECEIVED_MULTICAST)));
	PyModule_AddObject(m, "NT_STATUS_IPSEC_DOSP_INVALID_PACKET", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IPSEC_DOSP_INVALID_PACKET)));
	PyModule_AddObject(m, "NT_STATUS_IPSEC_DOSP_STATE_LOOKUP_FAILED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IPSEC_DOSP_STATE_LOOKUP_FAILED)));
	PyModule_AddObject(m, "NT_STATUS_IPSEC_DOSP_MAX_ENTRIES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IPSEC_DOSP_MAX_ENTRIES)));
	PyModule_AddObject(m, "NT_STATUS_IPSEC_DOSP_KEYMOD_NOT_ALLOWED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IPSEC_DOSP_KEYMOD_NOT_ALLOWED)));
	PyModule_AddObject(m, "NT_STATUS_IPSEC_DOSP_MAX_PER_IP_RATELIMIT_QUEUES", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_IPSEC_DOSP_MAX_PER_IP_RATELIMIT_QUEUES)));
	PyModule_AddObject(m, "NT_STATUS_VOLMGR_MIRROR_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VOLMGR_MIRROR_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_VOLMGR_RAID5_NOT_SUPPORTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VOLMGR_RAID5_NOT_SUPPORTED)));
	PyModule_AddObject(m, "NT_STATUS_VIRTDISK_PROVIDER_NOT_FOUND", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VIRTDISK_PROVIDER_NOT_FOUND)));
	PyModule_AddObject(m, "NT_STATUS_VIRTDISK_NOT_VIRTUAL_DISK", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VIRTDISK_NOT_VIRTUAL_DISK)));
	PyModule_AddObject(m, "NT_STATUS_VHD_PARENT_VHD_ACCESS_DENIED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VHD_PARENT_VHD_ACCESS_DENIED)));
	PyModule_AddObject(m, "NT_STATUS_VHD_CHILD_PARENT_SIZE_MISMATCH", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VHD_CHILD_PARENT_SIZE_MISMATCH)));
	PyModule_AddObject(m, "NT_STATUS_VHD_DIFFERENCING_CHAIN_CYCLE_DETECTED", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VHD_DIFFERENCING_CHAIN_CYCLE_DETECTED)));
	PyModule_AddObject(m, "NT_STATUS_VHD_DIFFERENCING_CHAIN_ERROR_IN_PARENT", 
                  		ndr_PyLong_FromUnsignedLongLong(NT_STATUS_V(NT_STATUS_VHD_DIFFERENCING_CHAIN_ERROR_IN_PARENT)));

	return m;
}
