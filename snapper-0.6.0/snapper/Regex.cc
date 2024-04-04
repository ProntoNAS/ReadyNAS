/*
 * Copyright (c) [2004-2011] Novell, Inc.
 *
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, contact Novell, Inc.
 *
 * To contact Novell about this file by physical or electronic mail, you may
 * find current contact information at www.novell.com.
 */


#include "snapper/Regex.h"

extern int _nl_msg_cat_cntr;


namespace snapper
{

Regex::Regex (const char* pattern, int cflags, unsigned int nm)
    : pattern (pattern),
      cflags (cflags),
      nm (cflags & REG_NOSUB ? 0 : nm)
{
    regcomp (&rx, pattern, cflags);
    my_nl_msg_cat_cntr = _nl_msg_cat_cntr;
    rm = new regmatch_t[nm];
}


Regex::Regex (const string& pattern, int cflags, unsigned int nm)
    : pattern (pattern),
      cflags (cflags),
      nm (cflags & REG_NOSUB ? 0 : nm)
{
    regcomp (&rx, pattern.c_str (), cflags);
    my_nl_msg_cat_cntr = _nl_msg_cat_cntr;
    rm = new regmatch_t[nm];
}


Regex::~Regex ()
{
    delete [] rm;
    regfree (&rx);
}


bool
Regex::match (const string& str, int eflags) const
{
    if (my_nl_msg_cat_cntr != _nl_msg_cat_cntr) {
	regfree (&rx);
	regcomp (&rx, pattern.c_str (), cflags);
	my_nl_msg_cat_cntr = _nl_msg_cat_cntr;
    }

    last_str = str;

    return regexec (&rx, str.c_str (), nm, rm, eflags) == 0;
}


regoff_t
Regex::so (unsigned int i) const
{
    return i < nm ? rm[i].rm_so : -1;
}


regoff_t
Regex::eo (unsigned int i) const
{
    return i < nm ? rm[i].rm_eo : -1;
}


string
Regex::cap (unsigned int i) const
{
    if (i < nm && rm[i].rm_so > -1)
	return last_str.substr (rm[i].rm_so, rm[i].rm_eo - rm[i].rm_so);
    return "";
}


const string Regex::ws = "[ \t]*";
const string Regex::number = "[0123456789]+";

}
