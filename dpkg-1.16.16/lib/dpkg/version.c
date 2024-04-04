/*
 * libdpkg - Debian packaging suite library routines
 * version.c - version handling functions
 *
 * Copyright © 1995 Ian Jackson <ian@chiark.greenend.org.uk>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include <compat.h>

#include <dpkg/dpkg.h> /* cis* */
#include <dpkg/ehandle.h>
#include <dpkg/string.h>
#include <dpkg/version.h>

/**
 * Turn the passed version into an empty version.
 *
 * This can be used to ensure the version is properly initialized.
 *
 * @param version The version to clear.
 */
void
dpkg_version_blank(struct dpkg_version *version)
{
	version->epoch = 0;
	version->version = NULL;
	version->revision = NULL;
}

/**
 * Test if a version is not empty.
 *
 * @param version The version to test.
 *
 * @retval true If the version is informative (i.e. not an empty version).
 * @retval false If the version is empty.
 */
bool
dpkg_version_is_informative(const struct dpkg_version *version)
{
	return (version->epoch ||
	        str_is_set(version->version) ||
	        str_is_set(version->revision));
}

/**
 * Give a weight to the character to order in the version comparison.
 *
 * @param c An ASCII character.
 */
static int
order(int c)
{
	if (cisdigit(c))
		return 0;
	else if (cisalpha(c))
		return c;
	else if (c == '~')
		return -1;
	else if (c)
		return c + 256;
	else
		return 0;
}

static int
verrevcmp(const char *a, const char *b)
{
	if (a == NULL)
		a = "";
	if (b == NULL)
		b = "";

	while (*a || *b) {
		int first_diff = 0;

		while ((*a && !cisdigit(*a)) || (*b && !cisdigit(*b))) {
			int ac = order(*a);
			int bc = order(*b);

			if (ac != bc)
				return ac - bc;

			a++;
			b++;
		}
		while (*a == '0')
			a++;
		while (*b == '0')
			b++;
		while (cisdigit(*a) && cisdigit(*b)) {
			if (!first_diff)
				first_diff = *a - *b;
			a++;
			b++;
		}

		if (cisdigit(*a))
			return 1;
		if (cisdigit(*b))
			return -1;
		if (first_diff)
			return first_diff;
	}

	return 0;
}

/**
 * Compares two Debian versions.
 *
 * This function follows the convention of the comparator functions used by
 * qsort().
 *
 * @see deb-version(5)
 *
 * @param a The first version.
 * @param b The second version.
 *
 * @retval 0 If a and b are equal.
 * @retval <0 If a is smaller than b.
 * @retval >0 If a is greater than b.
 */
int
dpkg_version_compare(const struct dpkg_version *a,
                     const struct dpkg_version *b)
{
	int r;

	if (a->epoch > b->epoch)
		return 1;
	if (a->epoch < b->epoch)
		return -1;

	r = verrevcmp(a->version, b->version);
	if (r)
		return r;

	return verrevcmp(a->revision, b->revision);
}

/**
 * Check if two versions have a certain relation.
 *
 * @param a The first version.
 * @param rel The relation.
 * @param b The second version.
 *
 * @retval true If the expression “a rel b” is true.
 * @retval true If rel is #dpkg_relation_none.
 * @retval false Otherwise.
 *
 * @warning If rel is not a valid relation, this function will terminate
 *          the program.
 */
bool
dpkg_version_relate(const struct dpkg_version *a,
                    enum dpkg_relation rel,
                    const struct dpkg_version *b)
{
	int r;

	if (rel == dpkg_relation_none)
		return true;

	r = dpkg_version_compare(a, b);

	switch (rel) {
	case dpkg_relation_eq:
		return r == 0;
	case dpkg_relation_lt:
		return r < 0;
	case dpkg_relation_le:
		return r <= 0;
	case dpkg_relation_gt:
		return r > 0;
	case dpkg_relation_ge:
		return r >= 0;
	default:
		internerr("unknown dpkg_relation %d", rel);
	}
	return false;
}
