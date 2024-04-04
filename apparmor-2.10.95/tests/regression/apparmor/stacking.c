/*
 * Copyright (C) 2014-2016 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, contact Canonical Ltd.
 */

#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/apparmor.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "changehat.h" /* for do_open() */

#define STACK_DELIM	"//&"
#define STACK_DELIM_LEN	strlen(STACK_DELIM)

#define NO_MODE		"(null)"

static void file_io(const char *file)
{
	int rc = do_open(file);

	if (rc != 0)
		exit(rc);
}

struct single_label {
	const char *label;
	size_t len;
};

#define MAX_LABELS	32

struct compound_label {
	size_t num_labels;
	struct single_label labels[MAX_LABELS];
};

/**
 * Initializes @sl by parsing @compound_label. Returns a pointer to the
 * location of the next label in the compound label string, which should be
 * passed in as @compound_label the next time that next_label() is called. NULL
 * is returned when there are no more labels in @compound_label.
 */
static const char *next_label(struct single_label *sl,
			      const char *compound_label)
{
	const char *delim;

	if (!compound_label || compound_label[0] == '\0')
		return NULL;

	delim = strstr(compound_label, STACK_DELIM);
	if (!delim) {
		sl->label = compound_label;
		sl->len = strlen(sl->label);
		return sl->label + sl->len;
	}

	sl->label = compound_label;
	sl->len = delim - sl->label;
	return delim + STACK_DELIM_LEN;
}

/* Returns true if the compound label was constructed successfully */
static bool compound_label_init(struct compound_label *cl,
				const char *compound_label)
{
	memset(cl, 0, sizeof(*cl));
	while ((compound_label = next_label(&cl->labels[cl->num_labels],
					    compound_label))) {
		cl->num_labels++;

		if (cl->num_labels == MAX_LABELS)
			return false;
	}

	return true;
}

/* Returns true if the compound label contains the single label */
static bool compound_label_contains(struct compound_label *cl,
				    struct single_label *sl)
{
	bool matched = false;
	size_t i;

	for (i = 0; !matched && i < cl->num_labels; i++) {
		if (cl->labels[i].len != sl->len)
			continue;

		if (strncmp(cl->labels[i].label, sl->label, sl->len))
			continue;

		matched = true;
	}

	return matched;
}

/* Returns true if the two compound labels contain the same label sets */
static bool compound_labels_equal(struct compound_label *cl1,
				  struct compound_label *cl2)
{
	size_t i;

	if (cl1->num_labels != cl2->num_labels)
		return false;

	for (i = 0; i < cl1->num_labels; i++) {
		if (!compound_label_contains(cl2, &cl1->labels[i]))
			return false;
	}

	return true;
}

/**
 * Verifies that the current confinement context matches the expected context.
 *
 * Either @expected_label or @expected_mode can be NULL if their values should
 * not be verified. If a NULL mode is expected, as what happens when an
 * unconfined process calls aa_getcon(2), then @expected_mode should be equal
 * to NO_MODE.
 */
static void verify_confinement_context(const char *expected_label,
				       const char *expected_mode)
{
	char *label, *mode;
	int expected_rc, rc;
	bool null_expected_mode = expected_mode ?
				  strcmp(NO_MODE, expected_mode) == 0 : false;

	rc = aa_getcon(&label, &mode);
	if (rc < 0) {
		int err = errno;
		fprintf(stderr, "FAIL - aa_getcon: %m");
		exit(err);
	}

	if (expected_label) {
		struct compound_label cl, expected_cl;

		if (!compound_label_init(&cl, label)) {
			fprintf(stderr, "FAIL - could not parse current compound label: %s\n",
				label);
			rc = EINVAL;
			goto err;
		}

		if (!compound_label_init(&expected_cl, expected_label)) {
			fprintf(stderr, "FAIL - could not parse expected compound label: %s\n",
				expected_label);
			rc = EINVAL;
			goto err;
		}

		if (!compound_labels_equal(&cl, &expected_cl)) {
			fprintf(stderr, "FAIL - label \"%s\" != expected_label \"%s\"\n",
				label, expected_label);
			rc = EINVAL;
			goto err;
		}
	}

	if (expected_mode &&
	    ((!mode && !null_expected_mode) ||
	     (mode && strcmp(mode, expected_mode)))) {
		fprintf(stderr, "FAIL - mode \"%s\" != expected_mode \"%s\"\n",
			mode, expected_mode);
		rc = EINVAL;
		goto err;
	}

	expected_rc = expected_label ? strlen(expected_label) : strlen(label);

	/**
	 * Add the expected bytes following the returned label string:
	 *
	 *   ' ' + '(' + mode + ')'
	 */
	if (expected_mode && !null_expected_mode)
		expected_rc += 1 + 1 + strlen(expected_mode) + 1;
	else if (mode)
		expected_rc += 1 + 1 + strlen(mode) + 1;

	expected_rc++; /* Trailing NUL terminator */

	if (rc != expected_rc) {
		fprintf(stderr, "FAIL - rc (%d) != expected_rc (%d)\n",
			rc, expected_rc);
		rc = EINVAL;
		goto err;
	}

	return;
err:
	free(label);
	exit(EINVAL);
}

static void stack_onexec(const char *label)
{
	if (aa_stack_onexec(label) != 0) {
		int err = errno;
		perror("FAIL - aa_stack_onexec");
		exit(err);
	}
}

static void stack_profile(const char *label)
{
	if (aa_stack_profile(label) != 0) {
		int err = errno;
		perror("FAIL - aa_stack_profile");
		exit(err);
	}
}

static void exec(const char *prog, char **argv)
{
	int err;

	execv(prog, argv);
	err = errno;
	perror("FAIL - execv");
	exit(err);
}

static void usage(const char *prog)
{
	fprintf(stderr,
		"%s: [-o <LABEL> | -p <LABEL>] [-l <LABEL>] [-m <MODE>] [-f <FILE>] [-- ... [-- ...]]\n"
		"  -o <LABEL>\tCall aa_stack_onexec(LABEL)\n"
		"  -p <LABEL>\tCall aa_stack_profile(LABEL)\n"
		"  -l <LABEL>\tVerify that aa_getcon() returns LABEL\n"
		"  -m <MODE>\tVerify that aa_getcon() returns MODE. Set to \"%s\" if a NULL mode is expected.\n"
		"  -f <FILE>\tOpen FILE and attempt to write to and read from it\n\n"
		"If \"--\" is encountered, execv() will be called using the following argument\n"
		"as the program to execute and passing it all of the arguments following the\n"
		"program name.\n", prog, NO_MODE);
	exit(EINVAL);
}

struct options {
	const char *file;
	const char *expected_label;
	const char *expected_mode;
	const char *stack_onexec;
	const char *stack_profile;
	const char *exec;
	char **exec_argv;
};

static void parse_opts(int argc, char **argv, struct options *opts)
{
	int o;

	memset(opts, 0, sizeof(*opts));
	while ((o = getopt(argc, argv, "f:l:m:o:p:")) != -1) {
		switch (o) {
		case 'f': /* file */
			opts->file = optarg;
			break;
		case 'l': /* expected label */
			opts->expected_label = optarg;
			break;
		case 'm': /* expected mode */
			opts->expected_mode = optarg;
			break;
		case 'o': /* aa_stack_onexec */
			opts->stack_onexec = optarg;
			break;
		case 'p': /* aa_stack_profile */
			opts->stack_profile = optarg;
			break;
		default: /* '?' */
			usage(argv[0]);
		}
	}

	/* Can only specify one or the other */
	if (opts->stack_onexec && opts->stack_profile) {
		usage(argv[0]);
	}

	if (optind < argc) {
		/* Ensure that the previous option was "--" */
		if (optind == 0 || strcmp("--", argv[optind - 1]))
			usage(argv[0]);

		opts->exec = argv[optind];
		opts->exec_argv = &argv[optind];
	}
}

int main(int argc, char **argv)
{
	struct options opts;

	parse_opts(argc, argv, &opts);

	if (opts.stack_onexec)
		stack_onexec(opts.stack_onexec);
	else if (opts.stack_profile)
		stack_profile(opts.stack_profile);

	if (opts.file)
		file_io(opts.file);

	if (opts.expected_label || opts.expected_mode)
		verify_confinement_context(opts.expected_label,
					   opts.expected_mode);

	if (opts.exec)
		exec(opts.exec, opts.exec_argv);

	printf("PASS\n");
	exit(0);
}

