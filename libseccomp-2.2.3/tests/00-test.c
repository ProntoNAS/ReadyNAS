#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <seccomp.h>

const int blocks[] = {
	3,  4,  5,  6,
	7,  8,  9,  11,
	12, 13, 14, 15
};

int block_call(int syscall, enum scmp_compare op, uint64_t value,
	       scmp_filter_ctx ctx);

int main(int argc, char **argv)
{
	int ret_code, socket_syscall_num, i;
	scmp_filter_ctx filter;

	// Let us make a Seccomp filter
	filter = seccomp_init(SCMP_ACT_ALLOW);

	if(filter == NULL) {
		printf("Error initializing filter!\n");
		return -1;
	}

	// We need to know which syscall Socket is
	socket_syscall_num = seccomp_syscall_resolve_name("socket");
	if(socket_syscall_num == __NR_SCMP_ERROR) {
		printf("Error looking up syscall number of Socket!\n");
		return -1;
	}

#if 0
	printf("Got syscall number of Socket as %d\n", socket_syscall_num);
#endif

	// Block all our predefined argument numbers
	for(i = 0; i < 12; i++) {
		ret_code = block_call(socket_syscall_num, SCMP_CMP_EQ, blocks[i], filter);
		if(ret_code != 0) {
			printf("Error creating filter rule!\n");
			return -1;
		}
	}

#if 1
	// Block everything over 16
	ret_code = block_call(socket_syscall_num, SCMP_CMP_GT, 16, filter);
	if(ret_code != 0) {
		printf("Error creating filter rule!\n");
		return -1;
	}
#endif

	seccomp_export_bpf(filter, STDOUT_FILENO);
	exit(0);

	printf("Filter initialized and filled!\n");

	// Load the filter into the kernel
	ret_code = seccomp_load(filter);
	if(ret_code != 0) {
		printf("Filter load failed!\n");
		return -1;
	}

	printf("Filter load successful!\n");

	// Release the filter when we're done to free memory
	seccomp_release(filter);

	// All right. Go through socket invocations. See which ones succeed, which ones fail.
	for(i = 1; i < AF_MAX; i++) {
		ret_code = socket(i, 0, SOCK_STREAM);

		if(ret_code != -1) {
			printf("Socket call for address family %d succeeded!\n", i);
		} else {
			printf("Error creating socket for address family %d: %d %s\n", i, ret_code,
			       strerror(errno));
		}
	}

	return 0;
}


/**
 * Add a single rule to a preexisting filter
 */
int block_call(int syscall, enum scmp_compare op, uint64_t value,
	       scmp_filter_ctx ctx)
{
	struct scmp_arg_cmp *arg;
	int ret_code;

	// Malloc/free to mimic the way the bindings behave to the greatest extent possible
	arg = (struct scmp_arg_cmp *)malloc(sizeof(struct scmp_arg_cmp));
	if(arg == NULL) {
		printf("Malloc failed!\n");
		return -1;
	}

	arg->arg = 0;
	arg->op = op;
	arg->datum_a = value;
	arg->datum_b = 0;

	ret_code = seccomp_rule_add_exact_array(ctx, SCMP_ACT_ERRNO(EPERM), syscall, 1,
						arg);
	if(ret_code != 0) {
		return -1;
	}

	free(arg);

	return 0;
}
