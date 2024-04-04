/*-*- Mode: C; c-basic-offset: 8; indent-tabs-mode: nil -*-*/

/***
  This file is part of systemd.

  Copyright 2010 Lennart Poettering

  systemd is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  systemd is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with systemd; If not, see <http://www.gnu.org/licenses/>.
***/

#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "util.h"
#include "strv.h"

char *strv_find(char **l, const char *name) {
        char **i;

        assert(name);

        STRV_FOREACH(i, l)
                if (streq(*i, name))
                        return *i;

        return NULL;
}

char *strv_find_prefix(char **l, const char *name) {
        char **i;

        assert(name);

        STRV_FOREACH(i, l)
                if (startswith(*i, name))
                        return *i;

        return NULL;
}

void strv_free(char **l) {
        char **k;

        if (!l)
                return;

        for (k = l; *k; k++)
                free(*k);

        free(l);
}

char **strv_copy(char **l) {
        char **r, **k;

        k = r = new(char*, strv_length(l)+1);
        if (!k)
                return NULL;

        if (l)
                for (; *l; k++, l++)
                        if (!(*k = strdup(*l)))
                                goto fail;

        *k = NULL;
        return r;

fail:
        for (k--; k >= r; k--)
                free(*k);

        free(r);

        return NULL;
}

unsigned strv_length(char **l) {
        unsigned n = 0;

        if (!l)
                return 0;

        for (; *l; l++)
                n++;

        return n;
}

char **strv_new_ap(const char *x, va_list ap) {
        const char *s;
        char **a;
        unsigned n = 0, i = 0;
        va_list aq;

        if (x) {
                n = 1;

                va_copy(aq, ap);
                while (va_arg(aq, const char*))
                        n++;
                va_end(aq);
        }

        if (!(a = new(char*, n+1)))
                return NULL;

        if (x) {
                if (!(a[i] = strdup(x))) {
                        free(a);
                        return NULL;
                }

                i++;

                while ((s = va_arg(ap, const char*))) {
                        if (!(a[i] = strdup(s)))
                                goto fail;

                        i++;
                }
        }

        a[i] = NULL;

        return a;

fail:

        for (; i > 0; i--)
                if (a[i-1])
                        free(a[i-1]);

        free(a);

        return NULL;
}

char **strv_new(const char *x, ...) {
        char **r;
        va_list ap;

        va_start(ap, x);
        r = strv_new_ap(x, ap);
        va_end(ap);

        return r;
}

char **strv_merge(char **a, char **b) {
        char **r, **k;

        if (!a)
                return strv_copy(b);

        if (!b)
                return strv_copy(a);

        if (!(r = new(char*, strv_length(a)+strv_length(b)+1)))
                return NULL;

        for (k = r; *a; k++, a++)
                if (!(*k = strdup(*a)))
                        goto fail;
        for (; *b; k++, b++)
                if (!(*k = strdup(*b)))
                        goto fail;

        *k = NULL;
        return r;

fail:
        for (k--; k >= r; k--)
                free(*k);

        free(r);

        return NULL;
}

char **strv_merge_concat(char **a, char **b, const char *suffix) {
        char **r, **k;

        /* Like strv_merge(), but appends suffix to all strings in b, before adding */

        if (!b)
                return strv_copy(a);

        r = new(char*, strv_length(a) + strv_length(b) + 1);
        if (!r)
                return NULL;

        k = r;
        if (a)
                for (; *a; k++, a++) {
                        *k = strdup(*a);
                        if (!*k)
                                goto fail;
                }

        for (; *b; k++, b++) {
                *k = strappend(*b, suffix);
                if (!*k)
                        goto fail;
        }

        *k = NULL;
        return r;

fail:
        for (k--; k >= r; k--)
                free(*k);

        free(r);

        return NULL;

}

char **strv_split(const char *s, const char *separator) {
        char *state;
        char *w;
        size_t l;
        unsigned n, i;
        char **r;

        assert(s);

        n = 0;
        FOREACH_WORD_SEPARATOR(w, l, s, separator, state)
                n++;

        if (!(r = new(char*, n+1)))
                return NULL;

        i = 0;
        FOREACH_WORD_SEPARATOR(w, l, s, separator, state)
                if (!(r[i++] = strndup(w, l))) {
                        strv_free(r);
                        return NULL;
                }

        r[i] = NULL;
        return r;
}

char **strv_split_quoted(const char *s) {
        char *state;
        char *w;
        size_t l;
        unsigned n, i;
        char **r;

        assert(s);

        n = 0;
        FOREACH_WORD_QUOTED(w, l, s, state)
                n++;

        if (!(r = new(char*, n+1)))
                return NULL;

        i = 0;
        FOREACH_WORD_QUOTED(w, l, s, state)
                if (!(r[i++] = cunescape_length(w, l))) {
                        strv_free(r);
                        return NULL;
                }

        r[i] = NULL;
        return r;
}

char *strv_join(char **l, const char *separator) {
        char *r, *e;
        char **s;
        size_t n, k;

        if (!separator)
                separator = " ";

        k = strlen(separator);

        n = 0;
        STRV_FOREACH(s, l) {
                if (n != 0)
                        n += k;
                n += strlen(*s);
        }

        if (!(r = new(char, n+1)))
                return NULL;

        e = r;
        STRV_FOREACH(s, l) {
                if (e != r)
                        e = stpcpy(e, separator);

                e = stpcpy(e, *s);
        }

        *e = 0;

        return r;
}

char **strv_append(char **l, const char *s) {
        char **r, **k;

        if (!l)
                return strv_new(s, NULL);

        if (!s)
                return strv_copy(l);

        r = new(char*, strv_length(l)+2);
        if (!r)
                return NULL;

        for (k = r; *l; k++, l++)
                if (!(*k = strdup(*l)))
                        goto fail;

        if (!(*(k++) = strdup(s)))
                goto fail;

        *k = NULL;
        return r;

fail:
        for (k--; k >= r; k--)
                free(*k);

        free(r);

        return NULL;
}

char **strv_uniq(char **l) {
        char **i;

        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        return l;
}

char **strv_remove(char **l, const char *s) {
        char **f, **t;

        if (!l)
                return NULL;

        assert(s);

        /* Drops every occurrence of s in the string list, edits
         * in-place. */

        for (f = t = l; *f; f++) {

                if (streq(*f, s)) {
                        free(*f);
                        continue;
                }

                *(t++) = *f;
        }

        *t = NULL;
        return l;
}

char **strv_remove_prefix(char **l, const char *s) {
        char **f, **t;

        if (!l)
                return NULL;

        assert(s);

        /* Drops every occurrence of a string prefixed with s in the
         * string list, edits in-place. */

        for (f = t = l; *f; f++) {

                if (startswith(*f, s)) {
                        free(*f);
                        continue;
                }

                *(t++) = *f;
        }

        *t = NULL;
        return l;
}

static int env_append(char **r, char ***k, char **a) {
        assert(r);
        assert(k);

        if (!a)
                return 0;

        /* Add the entries of a to *k unless they already exist in *r
         * in which case they are overridden instead. This assumes
         * there is enough space in the r array. */

        for (; *a; a++) {
                char **j;
                size_t n;

                n = strcspn(*a, "=");

                if ((*a)[n] == '=')
                        n++;

                for (j = r; j < *k; j++)
                        if (strncmp(*j, *a, n) == 0)
                                break;

                if (j >= *k)
                        (*k)++;
                else
                        free(*j);

                if (!(*j = strdup(*a)))
                        return -ENOMEM;
        }

        return 0;
}

char **strv_env_merge(unsigned n_lists, ...) {
        size_t n = 0;
        char **l, **k, **r;
        va_list ap;
        unsigned i;

        /* Merges an arbitrary number of environment sets */

        va_start(ap, n_lists);
        for (i = 0; i < n_lists; i++) {
                l = va_arg(ap, char**);
                n += strv_length(l);
        }
        va_end(ap);

        if (!(r = new(char*, n+1)))
                return NULL;

        k = r;

        va_start(ap, n_lists);
        for (i = 0; i < n_lists; i++) {
                l = va_arg(ap, char**);
                if (env_append(r, &k, l) < 0)
                        goto fail;
        }
        va_end(ap);

        *k = NULL;

        return r;

fail:
        va_end(ap);

        for (k--; k >= r; k--)
                free(*k);

        free(r);

        return NULL;
}

static bool env_match(const char *t, const char *pattern) {
        assert(t);
        assert(pattern);

        /* pattern a matches string a
         *         a matches a=
         *         a matches a=b
         *         a= matches a=
         *         a=b matches a=b
         *         a= does not match a
         *         a=b does not match a=
         *         a=b does not match a
         *         a=b does not match a=c */

        if (streq(t, pattern))
                return true;

        if (!strchr(pattern, '=')) {
                size_t l = strlen(pattern);

                return strncmp(t, pattern, l) == 0 && t[l] == '=';
        }

        return false;
}

char **strv_env_delete(char **x, unsigned n_lists, ...) {
        size_t n, i = 0;
        char **k, **r;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_length(x);

        r = new(char*, n+1);
        if (!r)
                return NULL;

        STRV_FOREACH(k, x) {
                unsigned v;

                va_start(ap, n_lists);
                for (v = 0; v < n_lists; v++) {
                        char **l, **j;

                        l = va_arg(ap, char**);
                        STRV_FOREACH(j, l)
                                if (env_match(*k, *j))
                                        goto skip;
                }
                va_end(ap);

                r[i] = strdup(*k);
                if (!r[i]) {
                        strv_free(r);
                        return NULL;
                }

                i++;
                continue;

        skip:
                va_end(ap);
        }

        r[i] = NULL;

        assert(i <= n);

        return r;
}

char **strv_env_unset(char **l, const char *p) {

        char **f, **t;

        if (!l)
                return NULL;

        assert(p);

        /* Drops every occurrence of the env var setting p in the
         * string list. edits in-place. */

        for (f = t = l; *f; f++) {

                if (env_match(*f, p)) {
                        free(*f);
                        continue;
                }

                *(t++) = *f;
        }

        *t = NULL;
        return l;
}

char **strv_env_set(char **x, const char *p) {

        char **k, **r;
        char* m[2] = { (char*) p, NULL };

        /* Overrides the env var setting of p, returns a new copy */

        if (!(r = new(char*, strv_length(x)+2)))
                return NULL;

        k = r;
        if (env_append(r, &k, x) < 0)
                goto fail;

        if (env_append(r, &k, m) < 0)
                goto fail;

        *k = NULL;

        return r;

fail:
        for (k--; k >= r; k--)
                free(*k);

        free(r);

        return NULL;

}

char *strv_env_get_with_length(char **l, const char *name, size_t k) {
        char **i;

        assert(name);

        STRV_FOREACH(i, l)
                if (strncmp(*i, name, k) == 0 &&
                    (*i)[k] == '=')
                        return *i + k + 1;

        return NULL;
}

char *strv_env_get(char **l, const char *name) {
        return strv_env_get_with_length(l, name, strlen(name));
}

char **strv_env_clean(char **l) {
        char **r, **ret;

        for (r = ret = l; *l; l++) {
                const char *equal;

                equal = strchr(*l, '=');

                if (equal && equal[1] == 0) {
                        free(*l);
                        continue;
                }

                *(r++) = *l;
        }

        *r = NULL;

        return ret;
}

char **strv_parse_nulstr(const char *s, size_t l) {
        const char *p;
        unsigned c = 0, i = 0;
        char **v;

        assert(s || l <= 0);

        if (l <= 0)
                return strv_new(NULL, NULL);

        for (p = s; p < s + l; p++)
                if (*p == 0)
                        c++;

        if (s[l-1] != 0)
                c++;

        if (!(v = new0(char*, c+1)))
                return NULL;

        p = s;
        while (p < s + l) {
                const char *e;

                e = memchr(p, 0, s + l - p);

                if (!(v[i++] = strndup(p, e ? e - p : s + l - p))) {
                        strv_free(v);
                        return NULL;
                }

                if (!e)
                        break;

                p = e + 1;
        }

        assert(i == c);

        return v;
}

bool strv_overlap(char **a, char **b) {
        char **i, **j;

        STRV_FOREACH(i, a) {
                STRV_FOREACH(j, b) {
                        if (streq(*i, *j))
                                return true;
                }
        }

        return false;
}
