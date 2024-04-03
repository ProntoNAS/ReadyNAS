/**
 * @file op_padre_model.h
 * interface to padre model-specific MSR operations
 *
 * @remark Copyright 2002 OProfile authors
 * @remark Read the file COPYING
 *
 * @author Graydon Hoare
 */

#ifndef OP_PADRE_MODEL_H
#define OP_PADRE_MODEL_H

struct op_saved_msr {
	unsigned int high;
	unsigned int low;
};

struct op_msr {
	unsigned long addr;
	struct op_saved_msr saved;
};

struct op_msrs {
	struct op_msr * counters;
	struct op_msr * controls;
};

struct pt_regs;

/* The model vtable abstracts the differences between
 * various padre CPU model's perfctr support.
 */
struct op_padre_model_spec {
	unsigned int const num_counters;
	unsigned int const num_controls;
	void (*fill_in_addresses)(struct op_msrs * const msrs);
	void (*setup_ctrs)(struct op_msrs const * const msrs);
	int (*check_ctrs)(struct pt_regs * const regs,
		struct op_msrs const * const msrs);
	void (*start)(struct op_msrs const * const msrs);
	void (*stop)(struct op_msrs const * const msrs);
};

extern struct op_padre_model_spec const op_padre_spec;

#endif /* OP_PADRE_MODEL_H */
