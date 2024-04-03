[= autogen5 template x =]
[= (dne " * " "/* ")
=]
 *
[= (gpl "shar" " * ")
;; /* =]
 *
 *  This file defines the shell script strings in C format.
 *  The compiler will reconstruct the original string found in scripts.def.
 *  shar.c will emit these strings into the constructed shar archive.
 *  See "scripts.def" for rationale.
 */
[=

 (define body-text "")
 (make-header-guard "shar") =][=

FOR text    =][=

 (set! body-text (get "body"))
 (if (not (exist? "omit-nl"))
     (set! body-text (string-append body-text "\n")) )

 (string-append
     (sprintf "\n\nstatic const char %s_z[%d] = \n"
	(get "name") (+ 1 (string-length body-text))  )

     (kr-string body-text) ";" )        =][=

ENDFOR text

=]

#define SHAR_MSG_CT  [= (count "shar-msg") =]
[= (out-push-new)
   (out-suspend "msg-map")
   (out-push-new)
=][=
   (out-suspend "msg-size")
   (out-push-new)
=]
#ifdef TRANSLATION_EXTRACTION
static void translation_strings (void) {
  /* TRANSLATORS:  This is a phony function generated for the purpose of
     constructing the strings to be extracted for translation.  Each string
     is associated with a name (e.g. SM_ans_yes) that can be found in shar.c,
     if you need to look up context.  These strings are all inserted into
     the shar-generated shell script so the shar user can create shar archives
     with messages localized for one locale.
   */[=
   (out-suspend "msg-gettext")

   (out-push-new)
   (out-suspend "msg-xform")
=]
static char const * shar_msg_table[SHAR_MSG_CT] = {[=

FOR shar-msg  ","  =][=

 (set! body-text (get "sm-body"))
 (if (exist? "sm-add-nl")
     (set! body-text (string-append body-text "\n")) )

 (ag-fprintf "msg-map" "#define SM_%-20s shar_msg_table[%3d]\n"
             (get "sm-name") (for-index))

 (ag-fprintf "msg-size" "%d\n" (string-length body-text))

 (ag-fprintf "msg-xform" "XFORM_%s\n"
     (if (not (exist? "sm-xform")) "PLAIN"
         (string-upcase (get "sm-xform")) ))

 (set! body-text (kr-string body-text))

 (ag-fprintf "msg-gettext" "\nSM_%-20s = N_(%s);" (get "sm-name") body-text)

 (sprintf "\n  %s" body-text)

  =][=

ENDFOR text

=]
};

[= (out-resume "msg-map")     (out-pop #t) =]
[= (out-resume "msg-gettext") (out-pop #t) =]
}
#endif /* TRANSLATION_EXTRACTION */

static size_t const shar_msg_size[SHAR_MSG_CT] = {
[= (out-resume "msg-size")
   (shell (string-append
   "( columns -I2 --spread=1 -S, | sed '$s/,$//') <<_EOF_\n"
    (out-pop #t)
    "\n_EOF_")) =] };


typedef enum {
  XFORM_PLAIN,
  XFORM_APOSTROPHE,
  XFORM_DBL_QUOTE
} msg_xform_t;

static msg_xform_t shar_msg_xform[SHAR_MSG_CT] = {
[= (out-resume "msg-xform")
   (shell (string-append
   "( columns -I2 --spread=1 -S, | sed '$s/,$//') <<_EOF_\n"
    (out-pop #t)
    "\n_EOF_"))

 =] };

#endif /* [= (. header-guard) =] */
/*
 * Local Variables:
 * mode: C
 * c-file-style: "gnu"
 * indent-tabs-mode: nil
 * End:
 * end of [= (out-name) =] */
