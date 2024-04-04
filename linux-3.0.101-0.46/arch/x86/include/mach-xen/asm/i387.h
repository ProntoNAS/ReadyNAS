#ifndef _ASM_X86_I387_H
#define switch_fpu_prepare native_switch_fpu_prepare
#define __thread_fpu_begin native_thread_fpu_begin
#include_next <asm/i387.h>
#undef __thread_fpu_begin
#undef switch_fpu_prepare

#ifndef __ASSEMBLY__
static inline void xen_thread_fpu_begin(struct task_struct *tsk,
					multicall_entry_t *mcl)
{
	if (mcl) {
		mcl->op = __HYPERVISOR_fpu_taskswitch;
		mcl->args[0] = 0;
	}
	__thread_set_has_fpu(tsk);
}

static inline fpu_switch_t xen_switch_fpu_prepare(struct task_struct *old,
						  struct task_struct *new,
						  multicall_entry_t **mcl)
{
	fpu_switch_t fpu;

	fpu.preload = tsk_used_math(new) && new->fpu_counter > 5;
	if (__thread_has_fpu(old)) {
		if (!__save_init_fpu(old))
			fpu_lazy_state_intact(old);
		__thread_clear_has_fpu(old);
		old->fpu_counter++;

		/* Don't change CR0.TS if we just switch! */
		if (fpu.preload) {
			__thread_set_has_fpu(new);
			prefetch(new->thread.fpu.state);
		} else {
			(*mcl)->op = __HYPERVISOR_fpu_taskswitch;
			(*mcl)++->args[0] = 1;
		}
	} else {
		old->fpu_counter = 0;
		if (fpu.preload) {
			new->fpu_counter++;
			if (fpu_lazy_restore(new))
				fpu.preload = 0;
			else
				prefetch(new->thread.fpu.state);
			xen_thread_fpu_begin(new, (*mcl)++);
		}
	}
	return fpu;
}
#endif /* __ASSEMBLY__ */

#endif /* _ASM_X86_I387_H */
