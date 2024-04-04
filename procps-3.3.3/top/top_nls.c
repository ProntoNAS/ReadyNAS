/* top_nls.c - provide the basis for future nls translations */
/*
 * Copyright (c) 2011-2012,     by: James C. Warner
 *    All rights reserved.      8921 Hilloway Road
 *                              Eden Prairie, Minnesota 55347 USA
 *
 * This file may be used subject to the terms and conditions of the
 * GNU Library General Public License Version 2, or any later version
 * at your option, as published by the Free Software Foundation.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 */
/* For contributions to this program, the author wishes to thank:
 *    Craig Small, <csmall@small.dropbear.id.au>
 *    Sami Kerola, <kerolasa@iki.fi>
 */

#include <locale.h>
#include <stdio.h>
#ifdef VALIDATE_NLS
#include <stdlib.h>
#endif
#include <string.h>

#include "../include/nls.h"

#include "top.h"
#include "top_nls.h"

        // Programmer Note(s):
        //  Preparation ---------------------------------------------
        //    Unless you have *something* following the gettext macro,
        //    xgettext will refuse to see any TRANSLATORS comments.
        //    Thus empty strings have been added for potential future
        //    comment additions.
        //
        //    Also, by omitting the argument for the --add-comments
        //    XGETTEXT_OPTION in po/Makevars, *any* preceding c style
        //    comment will be propagated to the .pot file, providing
        //    that the gettext macro isn't empty as discussed above.
        //    However, this is far too aggressive so we have chosen
        //    the word 'Translation' to denote xgettext comments.
        //
        //    /* Need Not Say 'TRANSLATORS': ...
        //    snprintf(buf, sizeof(buf), "%s", _(      // unseen comment
        //
        //    /* Translation Hint: ...
        //    snprintf(buf, sizeof(buf), "%s", _(""    // now it's seen!
        //
        //  Translation, from po/ directory after make --------------
        //  ( this is the procedure used before any translations were  )
        //  ( available in the po/ directory, which contained only the )
        //  ( procps-ng.pot, this domain's template file.              )
        //
        //  ( below: ll_CC = language/country as in 'zh_CN' or 'en_AU' )
        //
        //    msginit --locale=ll_CC --no-wrap
        //     . creates a ll_CC.po file from the template procps-ng.pot
        //     . may also duplicate msgid as msgstr if languages similar
        //    msgen --no-wrap ll_CC.po --output-file=ll_CC.po
        //     . duplicates every msgid literal as msgstr value
        //     . this is the file that's edited
        //        . replace "Content-Type: ... charset=ASCII\n"
        //                           with "... charset=UTF-8\n"
        //        . translate msgstr values, leaving msgid unchanged
        //    msgfmt ll_CC.po --strict --output-file=procps-ng.mo
        //     . after which chmod 644
        //     . move to /usr/share/local/ll_CC/LC_MESSAGES/ directory
        //
        //  Testing -------------------------------------------------
        //    export LC_ALL= && export LANGUAGE=ll_CC
        //    run some capable program like top
        //

        /*
         * These are our three string tables with the following contents:
         *    Desc : fields descriptions not to exceed 20 screen positions
         *    Norm : regular text possibly also containing c-format specifiers
         *    Uniq : show_special specially formatted strings
         *
         * The latter table presents the greatest translation challenge !
         */
const char *Desc_nlstab[P_MAXPFLGS];
const char *Norm_nlstab[norm_MAX];
const char *Uniq_nlstab[uniq_MAX];


        /*
         * This routine builds the nls table containing plain text only
         * used as the field descriptions.  Each translated line MUST be
         * kept to a maximum of 20 characters or less! */
static void build_desc_nlstab (void) {

/* Translation Notes ------------------------------------------------
   .  It is strongly recommend that the --no-wrap command line option
   .  be used with all supporting translation tools, when available.
   .
   .  The following single lines contain only plain text used as the
   .  descriptions under Field Management when the 'f' key is typed.
   .
   .  To avoid truncation, each translated line MUST be kept to a
   .  length of 20 characters or less.
   . */

   Desc_nlstab[P_PID] = _("Process Id");
   Desc_nlstab[P_PPD] = _("Parent Process pid");
   Desc_nlstab[P_UED] = _("Effective User Id");
   Desc_nlstab[P_UEN] = _("Effective User Name");
   Desc_nlstab[P_URD] = _("Real User Id");
   Desc_nlstab[P_URN] = _("Real User Name");
   Desc_nlstab[P_USD] = _("Saved User Id");
   Desc_nlstab[P_USN] = _("Saved User Name");
   Desc_nlstab[P_GID] = _("Group Id");
   Desc_nlstab[P_GRP] = _("Group Name");
   Desc_nlstab[P_PGD] = _("Process Group Id");
   Desc_nlstab[P_TTY] = _("Controlling Tty");
   Desc_nlstab[P_TPG] = _("Tty Process Grp Id");
   Desc_nlstab[P_SID] = _("Session Id");
   Desc_nlstab[P_PRI] = _("Priority");
   Desc_nlstab[P_NCE] = _("Nice Value");
   Desc_nlstab[P_THD] = _("Number of Threads");
   Desc_nlstab[P_CPN] = _("Last Used Cpu (SMP)");
   Desc_nlstab[P_CPU] = _("CPU Usage");
   Desc_nlstab[P_TME] = _("CPU Time");
   Desc_nlstab[P_TM2] = _("CPU Time, hundredths");
   Desc_nlstab[P_MEM] = _("Memory Usage (RES)");
   Desc_nlstab[P_VRT] = _("Virtual Image (KiB)");
   Desc_nlstab[P_SWP] = _("Swapped Size (KiB)");
   Desc_nlstab[P_RES] = _("Resident Size (KiB)");
   Desc_nlstab[P_COD] = _("Code Size (KiB)");
   Desc_nlstab[P_DAT] = _("Data+Stack (KiB)");
   Desc_nlstab[P_SHR] = _("Shared Memory (KiB)");
   Desc_nlstab[P_FL1] = _("Major Page Faults");
   Desc_nlstab[P_FL2] = _("Minor Page Faults");
   Desc_nlstab[P_DRT] = _("Dirty Pages Count");
   Desc_nlstab[P_STA] = _("Process Status");
   Desc_nlstab[P_CMD] = _("Command Name/Line");
   Desc_nlstab[P_WCH] = _("Sleeping in Function");
   Desc_nlstab[P_FLG] = _("Task Flags <sched.h>");
   Desc_nlstab[P_CGR] = _("Control Groups");
   Desc_nlstab[P_SGD] = _("Supp Groups IDs");
   Desc_nlstab[P_SGN] = _("Supp Groups Names");
   Desc_nlstab[P_TGD] = _("Thread Group Id");
#ifdef OOMEM_ENABLE
   Desc_nlstab[P_OOA] = _("oom_adjustment (2^X)");
   Desc_nlstab[P_OOM] = _("oom_score (badness)");
#endif
}


        /*
         * This routine builds the nls table containing both plain text
         * and regular c-format strings. */
static void build_norm_nlstab (void) {

/* Translation Notes ------------------------------------------------
   .  It is strongly recommend that the --no-wrap command line option
   .  be used with all supporting translation tools, when available.
   .
   .  This group of lines contains both plain text and c-format strings.
   .
   .  Some strings reflect switches used to affect the running program
   .  and should not be translated without also making corresponding
   .  c-code logic changes.
   . */

   Norm_nlstab[EXIT_signals_fmt] = _("\n"
      "\tsignal %d (%s) was caught by %s, please\n"
      "\tsee http://www.debian.org/Bugs/Reporting\n");
   Norm_nlstab[WRONG_switch_fmt] = _(""
      "inappropriate '%s'\n"
      "usage:\t%s%s");
   Norm_nlstab[HELP_cmdline_fmt] = _(""
      "\t%s\n"
      "usage:\t%s%s");
   Norm_nlstab[FAIL_statopn_fmt] = _("failed /proc/stat open: %s");
   Norm_nlstab[FAIL_openlib_fmt] = _("failed openproc: %s");
   Norm_nlstab[BAD_delayint_fmt] = _("bad delay interval '%s'");
   Norm_nlstab[BAD_niterate_fmt] = _("bad iterations argument '%s'");
   Norm_nlstab[LIMIT_exceed_fmt] = _("pid limit (%d) exceeded");
   Norm_nlstab[BAD_mon_pids_fmt] = _("bad pid '%s'");
   Norm_nlstab[MISSING_args_fmt] = _("-%c requires argument");
   Norm_nlstab[BAD_widtharg_fmt] = _("bad width arg '%s', must > %d");
   Norm_nlstab[UNKNOWN_opts_fmt] = _(""
      "unknown option '%c'\n"
      "usage:\t%s%s");
   Norm_nlstab[DELAY_secure_txt] = _("-d disallowed in \"secure\" mode");
   Norm_nlstab[DELAY_badarg_txt] = _("-d requires positive argument");
   Norm_nlstab[ON_word_only_txt] = _("On");
   Norm_nlstab[OFF_one_word_txt] = _("Off");
/* Translation Hint: Only the following words should be translated
   .                 delay, limit, user, cols (abbreviation for columns)*/
   Norm_nlstab[USAGE_abbrev_txt] = _(" -hv | -bcHiSs -d delay -n limit -u|U user | -p pid[,pid] -w [cols]");
   Norm_nlstab[FAIL_statget_txt] = _("failed /proc/stat read");
   Norm_nlstab[FOREST_modes_fmt] = _("Forest mode %s");
   Norm_nlstab[FAIL_tty_get_txt] = _("failed tty get");
   Norm_nlstab[FAIL_tty_mod_fmt] = _("failed Tty_tweaked set: %s");
   Norm_nlstab[FAIL_tty_raw_fmt] = _("failed Tty_raw set: %s");
   Norm_nlstab[CHOOSE_group_txt] = _("Choose field group (1 - 4)");
   Norm_nlstab[DISABLED_cmd_txt] = _("Command disabled, 'A' mode required");
   Norm_nlstab[DISABLED_win_fmt] = _("Command disabled, activate %s with '-' or '_'");
   Norm_nlstab[COLORS_nomap_txt] = _("No colors to map!");
   Norm_nlstab[FAIL_rc_open_fmt] = _("Failed '%s' open: %s");
   Norm_nlstab[WRITE_rcfile_fmt] = _("Wrote configuration to '%s'");
   Norm_nlstab[DELAY_change_fmt] = _("Change delay from %.1f to");
   Norm_nlstab[THREADS_show_fmt] = _("Show threads %s");
   Norm_nlstab[IRIX_curmode_fmt] = _("Irix mode %s");
   Norm_nlstab[GET_pid2kill_txt] = _("pid to signal/kill");
   Norm_nlstab[GET_sigs_num_fmt] = _("Send pid %d signal [%d/sigterm]");
   Norm_nlstab[FAIL_signals_fmt] = _("Failed signal pid '%d' with '%d': %s");
   Norm_nlstab[BAD_signalid_txt] = _("Invalid signal");
   Norm_nlstab[GET_pid2nice_txt] = _("PID to renice");
   Norm_nlstab[GET_nice_num_fmt] = _("Renice PID %d to value");
   Norm_nlstab[FAIL_re_nice_fmt] = _("Failed renice of PID %d to %d: %s");
   Norm_nlstab[NAME_windows_fmt] = _("Rename window '%s' to (1-3 chars)");
   Norm_nlstab[TIME_accumed_fmt] = _("Cumulative time %s");
   Norm_nlstab[GET_max_task_fmt] = _("Maximum tasks = %d, change to (0 is unlimited)");
   Norm_nlstab[BAD_max_task_txt] = _("Invalid maximum");
   Norm_nlstab[HILIGHT_cant_txt] = _("Nothing to highlight!");
   Norm_nlstab[GET_user_ids_txt] = _("Which user (blank for all)");
   Norm_nlstab[UNKNOWN_cmds_txt] = _("Unknown command - try 'h' for help");
   Norm_nlstab[SCROLL_coord_fmt] = _("scroll coordinates: y = %d/%d (tasks), x = %d/%d (fields)");
   Norm_nlstab[FAIL_alloc_c_txt] = _("failed memory allocate");
   Norm_nlstab[FAIL_alloc_r_txt] = _("failed memory re-allocate");
   Norm_nlstab[BAD_numfloat_txt] = _("Unacceptable floating point");
   Norm_nlstab[BAD_username_txt] = _("Invalid user");
   Norm_nlstab[FAIL_sigstop_fmt] = _("failed sigprocmask, SIG_BLOCK: %s");
   Norm_nlstab[FAIL_sigmask_fmt] = _("failed sigprocmask, SIG_SETMASK: %s");
   Norm_nlstab[FOREST_views_txt] = _("forest view");
   Norm_nlstab[FAIL_widepid_txt] = _("failed pid maximum size test");
   Norm_nlstab[FAIL_widecpu_txt] = _("failed number of cpus test");
   Norm_nlstab[RC_bad_files_fmt] = _("incompatible rcfile, you should delete '%s'");
   Norm_nlstab[RC_bad_entry_fmt] = _("window entry #%d corrupt, please delete '%s'");
   Norm_nlstab[NOT_onsecure_txt] = _("Unavailable in secure mode");
   Norm_nlstab[NOT_smp_cpus_txt] = _("Only 1 cpu detected");
   Norm_nlstab[BAD_integers_txt] = _("Unacceptable integer");
   Norm_nlstab[SELECT_clash_txt] = _("conflicting process selections (U/p/u)");
/* Translation Hint: This is an abbreviation (limit 3 characters) for:
   .                 kibibytes (1024 bytes) */
   Norm_nlstab[AMT_kilobyte_txt] = _("KiB");
/* Translation Hint: This is an abbreviation (limit 3 characters) for:
   .                 mebibytes (1,048,576 bytes) */
   Norm_nlstab[AMT_megabyte_txt] = _("MiB");
/* Translation Hint: This is an abbreviation (limit 3 characters) for:
   .                 gibibytes (1,073,741,824 bytes) */
   Norm_nlstab[AMT_gigabyte_txt] = _("GiB");
   Norm_nlstab[WORD_threads_txt] = _("Threads");
   Norm_nlstab[WORD_process_txt] = _("Tasks");
/* Translation Hint: The following "word" is meant to represent either a single
   .                 cpu or all of the processors in a multi-processor computer
   .                 (should be exactly 6 characters, not counting the colon)*/
   Norm_nlstab[WORD_allcpus_txt] = _("Cpu(s):");
/* Translation Hint: The following "word" is meant to represent a single processor
   .                 (should be exactly 3 characters) */
   Norm_nlstab[WORD_eachcpu_fmt] = _("Cpu%-3d:");
/* Translation Hint: The following word "another" must have 1 trailing space */
   Norm_nlstab[WORD_another_txt] = _("another ");
   Norm_nlstab[FIND_no_next_txt] = _("Locate next inactive, use \"L\"");
   Norm_nlstab[GET_find_str_txt] = _("Locate string");
   Norm_nlstab[FIND_no_find_fmt] = _("%s\"%s\" not found");
#ifndef WARN_CFG_OFF
   Norm_nlstab[XTRA_warncfg_txt] = _("Overwrite existing old style rcfile?");
#endif
}


        /*
         * This routine builds the nls table containing specially
         * formatted strings designed to fit within an 80x24 terminal. */
static void build_uniq_nlstab (void) {

/* Translation Notes ------------------------------------------------
   .  It is strongly recommend that the --no-wrap command line option
   .  be used with all supporting translation tools, when available.
   .
   .  The next several text groups contain special escape sequences
   .  representing values used to index a table at run-time.
   .
   .  Each such sequence consists of a tilde (~) followed by an ascii
   .  number in the range of '1' - '8'.  Examples are '~2', '~8', etc.
   .  These escape sequences must never themselves be translated but
   .  could be deleted.
   .
   .  If you remove these escape sequences (both tilde and number) it
   .  would make translation easier.  However, the ability to display
   .  colors and bold text at run-time will have been lost.
   .
   .  Additionally, each of these text groups was designed to display
   .  in a 80x24 terminal window.  Hopefully, any translations will
   .  adhere to that goal lest the translated text be truncated.
   .
   .  If you would like additional information regarding these strings,
   .  please see the prolog to the show_special function in the top.c
   .  source file.
   . */

   Uniq_nlstab[KEYS_helpbas_fmt] = _(""
      "Help for Interactive Commands~2 - %s\n"
      "Window ~1%s~6: ~1Cumulative mode ~3%s~2.  ~1System~6: ~1Delay ~3%.1f secs~2; ~1Secure mode ~3%s~2.\n"
      "\n"
      "  Z~5,~1B~5       Global: '~1Z~2' change color mappings; '~1B~2' disable/enable bold\n"
      "  l,t,m     Toggle Summaries: '~1l~2' load avg; '~1t~2' task/cpu stats; '~1m~2' mem info\n"
      "  1,I       Toggle SMP view: '~11~2' single/separate states; '~1I~2' Irix/Solaris mode\n"
      "  f,F       Manage Fields: add/remove; change order; select sort field\n"
      "\n"
      "  L,&,<,> . Locate: '~1L~2'/'~1&~2' find/again; Move sort column: '~1<~2'/'~1>~2' left/right\n" \
      "  R,H,V   . Toggle: '~1R~2' norm/rev sort; '~1H~2' show threads; '~1V~2' forest view\n"
      "  c,i,S   . Toggle: '~1c~2' cmd name/line; '~1i~2' idle tasks; '~1S~2' cumulative time\n"
      "  x~5,~1y~5     . Toggle highlights: '~1x~2' sort field; '~1y~2' running tasks\n"
      "  z~5,~1b~5     . Toggle: '~1z~2' color/mono; '~1b~2' bold/reverse (only if 'x' or 'y')\n"
      "  u,U     . Show: '~1u~2' effective user; '~1U~2' real, saved, file or effective user\n"
      "  n or #  . Set maximum tasks displayed\n"
      "  C,...   . Toggle scroll coordinates msg for: ~1up~2,~1down~2,~1left~2,right~2,~1home~2,~1end~2\n"
      "\n"
      "%s"
      "  W         Write configuration file\n"
      "  q         Quit\n"
      "          ( commands shown with '.' require a ~1visible~2 task display ~1window~2 ) \n"
      "Press '~1h~2' or '~1?~2' for help with ~1Windows~2,\n"
      "any other key to continue ");

/* Translation Hint: As is true for the text above, the "keys" shown to the left and
   .                 also imbedded in the translatable text (along with escape seqs)
   .                 should never themselves be translated. */
   Uniq_nlstab[KEYS_helpext_fmt] = _(""
      "  k,r       Manipulate tasks: '~1k~2' kill; '~1r~2' renice\n"
      "  d or s    Set update interval\n");

   Uniq_nlstab[WINDOWS_help_fmt] = _(""
      "Help for Windows / Field Groups~2 - \"Current Window\" = ~1 %s ~6\n"
      "\n"
      ". Use multiple ~1windows~2, each with separate config opts (color,fields,sort,etc)\n"
      ". The 'current' window controls the ~1Summary Area~2 and responds to your ~1Commands~2\n"
      "  . that window's ~1task display~2 can be turned ~1Off~2 & ~1On~2, growing/shrinking others\n"
      "  . with ~1NO~2 task display, some commands will be ~1disabled~2 ('i','R','n','c', etc)\n"
      "    until a ~1different window~2 has been activated, making it the 'current' window\n"
      ". You ~1change~2 the 'current' window by: ~1 1~2) cycling forward/backward;~1 2~2) choosing\n"
      "  a specific field group; or~1 3~2) exiting the color mapping or fields screens\n"
      ". Commands ~1available anytime   -------------~2\n"
      "    A       . Alternate display mode toggle, show ~1Single~2 / ~1Multiple~2 windows\n"
      "    g       . Choose another field group and make it 'current', or change now\n"
      "              by selecting a number from: ~1 1~2 =%s;~1 2~2 =%s;~1 3~2 =%s; or~1 4~2 =%s\n"
      ". Commands ~1requiring~2 '~1A~2' mode~1  -------------~2\n"
      "    G       . Change the ~1Name~5 of the 'current' window/field group\n"
      " ~1*~4  a , w   . Cycle through all four windows:  '~1a~5' Forward; '~1w~5' Backward\n"
      " ~1*~4  - , _   . Show/Hide:  '~1-~5' ~1Current~2 window; '~1_~5' all ~1Visible~2/~1Invisible~2\n"
      "  The screen will be divided evenly between task displays.  But you can make\n"
      "  some ~1larger~2 or ~1smaller~2, using '~1n~2' and '~1i~2' commands.  Then later you could:\n"
      " ~1*~4  = , +   . Rebalance tasks:  '~1=~5' ~1Current~2 window; '~1+~5' ~1Every~2 window\n"
      "              (this also forces the ~1current~2 or ~1every~2 window to become visible)\n"
      "\n"
      "In '~1A~2' mode, '~1*~4' keys are your ~1essential~2 commands.  Please try the '~1a~2' and '~1w~2'\n"
      "commands plus the 'g' sub-commands NOW.  Press <Enter> to make 'Current' ");

/* Translation Notes ------------------------------------------------
   .  The following 'Help for color mapping' simulated screen should
   .  probably NOT be translated.  It is terribly hard to follow in
   .  this form and any translation could produce unpleasing results
   .  that are unlikely to parallel the running top program.
   .
   .  If you decide to proceed with translation, do the following
   .  lines only, taking care not to disturbe the tilde + number.
   .
   .  Simulated screen excerpt:
   .     --> "   Tasks:~3  64 ~2total,~3   2 ~3running,~3  62
   .     --> "   %%Cpu(s):~3  76.5 ~2user,~3  11.2 ~2system,~
   .     --> "   ~1 Nasty Message! ~4  -or-  ~1Input Prompt~5
   .
   .     --> "   available toggles: ~1B~2 =disable bold globa
   .     --> "       ~1z~2 =color/mono (~1%s~2), ~1b~2 =tasks
   .
   .     --> "Select ~1target~2 as upper case letter:\n"
   .     --> "   S~2 = Summary Data,~1  M~2 = Messages/Prompt
   .     --> "   H~2 = Column Heads,~1  T~2 = Task Informatio
   .     --> "Select ~1color~2 as number:\n"
   .     --> "   0~2 = black,~1  1~2 = red,    ~1  2~2 = gree
   .     --> "   4~2 = blue, ~1  5~2 = magenta,~1  6~2 = cyan
   . */
   Uniq_nlstab[COLOR_custom_fmt] = _(""
      "Help for color mapping~2 - %s\n"
      "current window: ~1%s~6\n"
      "\n"
      "   color - 04:25:44 up 8 days, 50 min,  7 users,  load average:\n"
      "   Tasks:~3  64 ~2total,~3   2 ~3running,~3  62 ~2sleeping,~3   0 ~2stopped,~3\n"
      "   %%Cpu(s):~3  76.5 ~2user,~3  11.2 ~2system,~3   0.0 ~2nice,~3  12.3 ~2idle~3\n"
      "   ~1 Nasty Message! ~4  -or-  ~1Input Prompt~5\n"
      "   ~1  PID TTY     PR  NI %%CPU    TIME+   VIRT SWAP S COMMAND    ~6\n"
      "   17284 ~8pts/2  ~7  8   0  0.0   0:00.75  1380    0 S /bin/bash   ~8\n"
      "   ~1 8601 pts/1    7 -10  0.4   0:00.03   916    0 R color -b -z~7\n"
      "   11005 ~8?      ~7  9   0  0.0   0:02.50  2852 1008 S amor -sessi~8\n"
      "   available toggles: ~1B~2 =disable bold globally (~1%s~2),\n"
      "       ~1z~2 =color/mono (~1%s~2), ~1b~2 =tasks \"bold\"/reverse (~1%s~2)\n"
      "\n"
      "Select ~1target~2 as upper case letter:\n"
      "   S~2 = Summary Data,~1  M~2 = Messages/Prompts,\n"
      "   H~2 = Column Heads,~1  T~2 = Task Information\n"
      "Select ~1color~2 as number:\n"
      "   0~2 = black,~1  1~2 = red,    ~1  2~2 = green,~1  3~2 = yellow,\n"
      "   4~2 = blue, ~1  5~2 = magenta,~1  6~2 = cyan, ~1  7~2 = white\n"
      "\n"
      "Selected: ~1target~2 ~1 %c ~4; ~1color~2 ~1 %d ~4\n"
      "   press 'q' to abort changes to window '~1%s~2'\n"
      "   press 'a' or 'w' to commit & change another, <Enter> to commit and end ");

   Uniq_nlstab[FIELD_header_fmt] = _(""
      "Fields Management~2 for window ~1%s~6, whose current sort field is ~1%s~2\n"
      "   Navigate with Up/Dn, Right selects for move then <Enter> or Left commits,\n"
      "   'd' or <Space> toggles display, 's' sets sort.  Use 'q' or <Esc> to end! ");

   Uniq_nlstab[STATE_line_1_fmt] = _("%s:~3"
      " %3u ~2total,~3 %3u ~2running,~3 %3u ~2sleeping,~3 %3u ~2stopped,~3 %3u ~2zombie~3\n");

   Uniq_nlstab[STATE_lin2x4_fmt] = _("%%%s~3"
      " %#5.1f  ~2user,~3 %#5.1f  ~2system,~3 %#5.1f  ~2nice,~3 %#5.1f  ~2idle~3\n");

   Uniq_nlstab[STATE_lin2x5_fmt] = _("%%%s~3"
      " %#5.1f  ~2user,~3 %#5.1f  ~2system,~3 %#5.1f  ~2nice,~3 %#5.1f  ~2idle,~3 %#5.1f  ~2IO-wait~3\n");

/* Translation Hint: Only the following abbreviations need be translated
   .                 us = user, sy = system, ni = nice, id = idle, wa = wait,
   .                 hi hardware interrupt, si = software interrupt */
   Uniq_nlstab[STATE_lin2x6_fmt] = _("%%%s~3"
      " %#5.1f ~2us,~3 %#5.1f ~2sy,~3 %#5.1f ~2ni,~3 %#5.1f ~2id,~3 %#5.1f ~2wa,~3 %#5.1f ~2hi,~3 %#5.1f ~2si~3\n");

/* Translation Hint: Only the following abbreviations need be translated
   .                 us = user, sy = system, ni = nice, id = idle, wa = wait,
   .                 hi hardware interrupt, si = software interrupt, st = steal time */
   Uniq_nlstab[STATE_lin2x7_fmt] = _("%%%s~3"
      "%#5.1f ~2us,~3%#5.1f ~2sy,~3%#5.1f ~2ni,~3%#5.1f ~2id,~3%#5.1f ~2wa,~3%#5.1f ~2hi,~3%#5.1f ~2si,~3%#5.1f ~2st~3\n");

/* Translation Hint: Only the following need be translated
   .                 abbreviations: Mem = physical memory/ram, Swap = the linux swap file
   .                 words:         total, used, free, buffers, cached */
   Uniq_nlstab[MEMORY_lines_fmt] = _(""
      "%s Mem: ~3 %8lu ~2total,~3 %8lu ~2used,~3 %8lu ~2free,~3 %8lu ~2buffers~3\n"
      "%s Swap:~3 %8lu ~2total,~3 %8lu ~2used,~3 %8lu ~2free,~3 %8lu ~2cached~3\n");
}


        /*
         * This function must be called very early at startup, before
         * any other function call, and especially before any changes
         * have been made to the terminal if VALIDATE_NLS is defined!
         *
         * The gettext documentation suggests that alone among locale
         * variables LANGUAGE=ll_CC may be abbreviated as LANGUAGE=ll
         * to denote the language's main dialect.  Unfortunately this
         * does not appear to be true.  One must specify the complete
         * ll_CC.  Optionally, a '.UTF-8' or '.uft8' suffix, as shown
         * in the following examples, may also be included:
         *    export LANGUAGE=ll_CC          # minimal requirement
         *    export LANGUAGE=ll_CC.UTF-8    # optional convention
         *    export LANGUAGE=ll_CC.utf8     # ok, too
         *
         * Additionally, as suggested in the gettext documentation, a
         * user will also have to export an empty LC_ALL= to actually
         * enable any translations.
         */
void initialize_nls (void) {
#ifdef VALIDATE_NLS
   static const char *nls_err ="\t%s_nlstab[%d] == NULL\n";
   int i;

   setlocale (LC_ALL, "");
   bindtextdomain(PACKAGE, LOCALEDIR);
   textdomain(PACKAGE);

   memset(Desc_nlstab, 0, sizeof(Desc_nlstab));
   build_desc_nlstab();
   for (i = 0; i < P_MAXPFLGS; i++)
      if (!Desc_nlstab[i]) {
         fprintf(stderr, nls_err, "Desc", i);
         exit(1);
      }
   memset(Norm_nlstab, 0, sizeof(Norm_nlstab));
   build_norm_nlstab();
   for (i = 0; i < norm_MAX; i++)
      if (!Norm_nlstab[i]) {
         fprintf(stderr, nls_err, "Norm", i);
         exit(1);
      }
   memset(Uniq_nlstab, 0, sizeof(Uniq_nlstab));
   build_uniq_nlstab();
   for (i = 0; i < uniq_MAX; i++)
      if (!Uniq_nlstab[i]) {
         fprintf(stderr, nls_err, "Uniq", i);
         exit(1);
      }
#else
   setlocale (LC_ALL, "");
   bindtextdomain(PACKAGE, LOCALEDIR);
   textdomain(PACKAGE);

   build_desc_nlstab();
   build_norm_nlstab();
   build_uniq_nlstab();
#endif
}
