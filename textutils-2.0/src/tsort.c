/* tsort - topological sort.
   Copyright (C) 1998, 1999 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* Written by Mark Kettenis <kettenis@phys.uva.nl>.  */

/* The topological sort is done according to Algorithm T (Topological
   sort) in Donald E. Knuth, The Art of Computer Programming, Volume
   1/Fundamental Algorithms, page 262.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <assert.h>
#include <getopt.h>

#include "system.h"
#include "long-options.h"
#include "error.h"
#include "readtokens.h"

/* The official name of this program (e.g., no `g' prefix).  */
#define PROGRAM_NAME "tsort"

#define AUTHORS "Mark Kettenis"

/* Token delimiters when reading from a file.  */
#define DELIM " \t\n"

char *xstrdup ();

/* Members of the list of successors.  */
struct successor
{
  struct item *suc;
  struct successor *next;
};

/* Each string is held in core as the head of a list of successors.  */
struct item
{
  const char *str;
  struct item *left, *right;
  int balance;
  struct item *eqnext, *eqtail;
  enum { unvisited, visiting, visited } status;
  int depth;
  struct item *qlink;
  struct successor *top;
};

/* The name this program was run with. */
char *program_name;

/* Nonzero if any of the input files are the standard input. */
static int have_read_stdin;

/* The head of the sorted list.  */
static struct item *head = NULL;

/* True if there are loops. */
static int loop_found = 0;

static struct option const long_options[] =
{
  { NULL, 0, NULL, 0}
};

void
usage (int status)
{
  if (status != 0)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
	     program_name);
  else
    {
      printf (_("\
Usage: %s [OPTION] [FILE]\n\
Write totally ordered list consistent with the partial ordering in FILE.\n\
With no FILE, or when FILE is -, read standard input.\n\
\n\
      --help       display this help and exit\n\
      --version    output version information and exit\n"),
	      program_name);
      puts (_("\nReport bugs to <textutils-bugs@gnu.org>."));
    }

  exit (status == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}

/* Create a new item/node for STR.  */
static struct item *
new_item (const char *str)
{
  struct item *k = xmalloc (sizeof (struct item));

  k->str = (str ? xstrdup (str): NULL);
  k->left = k->right = NULL;
  k->balance = 0;

  k->qlink = NULL;
  k->top = NULL;
  k->eqnext = NULL;
  k->eqtail = k;
  k->status = unvisited;

  return k;
}

/* Search binary tree rooted at *ROOT for STR.  Allocate a new tree if
   *ROOT is NULL.  Insert a node/item for STR if not found.  Return
   the node/item found/created for STR.

   This is done according to Algorithm A (Balanced tree search and
   insertion) in Donald E. Knuth, The Art of Computer Programming,
   Volume 3/Searching and Sorting, pages 455--457.  */

static struct item *
search_item (struct item *root, const char *str)
{
  struct item *p, *q, *r, *s, *t;
  int a;

  assert (root);

  /* Make sure the tree is not empty, since that is what the algorithm
     below expects.  */
  if (root->right == NULL)
    return (root->right = new_item (str));

  /* A1. Initialize.  */
  t = root;
  s = p = root->right;

  for (;;)
    {
      /* A2. Compare.  */
      a = strcmp (str, p->str);
      if (a == 0)
	return p;

      /* A3 & A4.  Move left & right.  */
      if (a < 0)
	q = p->left;
      else
	q = p->right;

      if (q == NULL)
	{
	  /* A5. Insert.  */
	  q = new_item (str);

	  /* A3 & A4.  (continued).  */
	  if (a < 0)
	    p->left = q;
	  else
	    p->right = q;

	  /* A6. Adjust balance factors.  */
	  assert (!STREQ (str, s->str));
	  if (strcmp (str, s->str) < 0)
	    {
	      r = p = s->left;
	      a = -1;
	    }
	  else
	    {
	      r = p = s->right;
	      a = 1;
	    }

	  while (p != q)
	    {
	      assert (!STREQ (str, p->str));
	      if (strcmp (str, p->str) < 0)
		{
		  p->balance = -1;
		  p = p->left;
		}
	      else
		{
		  p->balance = 1;
		  p = p->right;
		}
	    }

	  /* A7. Balancing act.  */
	  if (s->balance == 0 || s->balance == -a)
	    {
	      s->balance += a;
	      return q;
	    }

	  if (r->balance == a)
	    {
	      /* A8. Single Rotation.  */
	      p = r;
	      if (a < 0)
		{
		  s->left = r->right;
		  r->right = s;
		}
	      else
		{
		  s->right = r->left;
		  r->left = s;
		}
	      s->balance = r->balance = 0;
	    }
	  else
	    {
	      /* A9. Double rotation.  */
	      if (a < 0)
		{
		  p = r->right;
		  r->right = p->left;
		  p->left = r;
		  s->left = p->right;
		  p->right = s;
		}
	      else
		{
		  p = r->left;
		  r->left = p->right;
		  p->right = r;
		  s->right = p->left;
		  p->left = s;
		}

	      s->balance = 0;
	      r->balance = 0;
	      if (p->balance == a)
		s->balance = -a;
	      else if (p->balance == -a)
		r->balance = a;
	      p->balance = 0;
	    }

	  /* A10. Finishing touch.  */
	  if (s == t->right)
	    t->right = p;
	  else
	    t->left = p;

	  return q;
	}

      /* A3 & A4.  (continued).  */
      if (q->balance)
	{
	  t = p;
	  s = q;
	}

      p = q;
    }

  /* NOTREACHED */
}

/* Record the fact that J precedes K.  */

static void
record_relation (struct item *j, struct item *k)
{
  struct successor *p;

  if (!STREQ (j->str, k->str))
    {
      p = xmalloc (sizeof (struct successor));
      p->suc = k;
      p->next = j->top;
      j->top = p;
    }
}

static int
dfs(struct item *k, int *depth)
{
  struct successor *p;
  int cycle_start, new_cycle_start;

  k->depth = cycle_start = (*depth)++;
  k->status = visiting;
  for (p = k->top; p; p = p->next)
    {
      switch (p->suc->status)
	{
	case unvisited:
	  new_cycle_start = dfs(p->suc, depth);
	  if (new_cycle_start < p->suc->depth)
	    {
	      k->eqtail->eqnext = p->suc;
	      k->eqtail = p->suc->eqtail;
	      if (new_cycle_start < cycle_start)
		cycle_start = new_cycle_start;
	    }
	  break;
	case visiting:
	  if (p->suc->depth < cycle_start)
	    cycle_start = p->suc->depth;
	  break;
	case visited:
	  break;
	}
    }

  if (cycle_start == k->depth)
    {
      if (k->eqnext)
	{
	  struct item *j = k;
	  error (0, 0, _("input contains a loop:\n"));
	  do
	    {
	      fprintf (stderr, "%s: %s\n", program_name, j->str);
	      j->status = visited;
	      j->qlink = head;
	      head = j;
	    }
	  while ((j = j->eqnext));
	  loop_found = 1;
	}
      else
	{
	  k->status = visited;
	  k->qlink = head;
	  head = k;
	}
    }

  return cycle_start;
}

static void
tsort_node (struct item *k)
{
  int depth;

  if (k->status == visited)
    return;

  depth = 0;
  dfs(k, &depth);
}

/* Recurse (sub)tree rooted at ROOT, calling ACTION for each node.  */

static void
recurse_tree (struct item *root, void (*action) (struct item *))
{
  if (root->left == NULL && root->right == NULL)
    (*action) (root);
  else
    {
      if (root->left != NULL)
	recurse_tree (root->left, action);
      (*action) (root);
      if (root->right != NULL)
	recurse_tree (root->right, action);
    }
}

/* Walk the tree specified by the head ROOT, calling ACTION for
   each node.  */

static void
walk_tree (struct item *root, void (*action) (struct item *))
{
  if (root->right)
    recurse_tree (root->right, action);
}

/* Do a topological sort on FILE.   */

static void
tsort (const char *file)
{
  struct item *root;
  struct item *j = NULL;
  struct item *k = NULL;
  register FILE *fp;
  token_buffer tokenbuffer;

  /* Intialize the head of the tree will hold the strings we're sorting.  */
  root = new_item (NULL);

  if (STREQ (file, "-"))
    {
      fp = stdin;
      have_read_stdin = 1;
    }
  else
    {
      fp = fopen (file, "r");
      if (fp == NULL)
	error (EXIT_FAILURE, errno, "%s", file);
    }

  init_tokenbuffer (&tokenbuffer);

  while (1)
    {
      long int len;

      /* T2. Next Relation.  */
      len = readtoken (fp, DELIM, sizeof (DELIM) - 1, &tokenbuffer);
      if (len < 0)
	break;

      assert (len != 0);

      k = search_item (root, tokenbuffer.buffer);
      if (j)
	{
	  /* T3. Record the relation.  */
	  record_relation (j, k);
	  k = NULL;
	}

      j = k;
    }

  walk_tree (root, tsort_node);
  while (head)
    {
      printf ("%s\n", head->str);
      head = head->qlink;
    }
  if (loop_found)
    exit (EXIT_FAILURE);
}

int
main (int argc, char **argv)
{
  int opt;

  program_name = argv[0];
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  parse_long_options (argc, argv, PROGRAM_NAME, GNU_PACKAGE, VERSION,
		      AUTHORS, usage);

  while ((opt = getopt_long (argc, argv, "", long_options, NULL)) != -1)
    switch (opt)
      {
      case 0:			/* long option */
	break;
      default:
	usage (EXIT_FAILURE);
      }

  have_read_stdin = 0;

  if (optind + 1 < argc)
    {
      error (0, 0, _("only one argument may be specified"));
      usage (EXIT_FAILURE);
    }

  if (optind < argc)
    tsort (argv[optind]);
  else
    tsort ("-");

  if (fclose (stdout) == EOF)
    error (EXIT_FAILURE, errno, _("write error"));

  if (have_read_stdin && fclose (stdin) == EOF)
    error (EXIT_FAILURE, errno, _("standard input"));

  exit (EXIT_SUCCESS);
}
