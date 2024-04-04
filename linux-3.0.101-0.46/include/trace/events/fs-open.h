#undef TRACE_SYSTEM
#define TRACE_SYSTEM fs-open

#if !defined(_TRACE_FS_OPEN_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_FS_OPEN_H

#include <linux/fs.h>
#include <linux/tracepoint.h>

/*
 * Here we have a problem; the __string macro uses __dynamic_array,
 * which requires the ability to know it's own length before we
 * allocate the buffer - in the get_offsets_ call - which does not
 * know the length of the resulting path we create in TP_fast_assign.
 * So - give up and use a fixed length.
 */
TRACE_EVENT(do_sys_open,

	TP_PROTO(struct file *filp, int flags, int mode),

	TP_ARGS(filp, flags, mode),

	TP_STRUCT__entry(
		__file_p(	filename, filp			)
		__field(	int, flags			)
		__field(	int, mode			)
	),

	TP_fast_assign(
		__assign_file_p(filename, filp);
		__entry->flags = flags;
		__entry->mode = mode;
	),

	TP_printk("\"%s\" %x %o", __get_str(filename),
		  __entry->flags, __entry->mode)
);

TRACE_EVENT(uselib,
	TP_PROTO(struct file *filp),
	TP_ARGS(filp),
	TP_STRUCT__entry(
		__file_p(filename, filp)
	),
	TP_fast_assign(
		__assign_file_p(filename, filp);
	),
	TP_printk("\"%s\"", __get_str(filename))
);

TRACE_EVENT(open_exec,
	TP_PROTO(struct file *filp),
	TP_ARGS(filp),
	TP_STRUCT__entry(
		__file_p(filename, filp)
	),
	TP_fast_assign(
		__assign_file_p(filename, filp);
	),
	TP_printk("\"%s\"", __get_str(filename))
);

#endif /* _TRACE_FS_OPEN_H */

/* This part must be outside protection */
#include <trace/define_trace.h>
