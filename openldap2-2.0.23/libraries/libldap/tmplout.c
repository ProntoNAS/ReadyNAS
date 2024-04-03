/* $OpenLDAP: pkg/ldap/libraries/libldap/tmplout.c,v 1.24.2.6 2002/01/04 20:38:22 kurt Exp $ */
/*
 * Copyright 1998-2002 The OpenLDAP Foundation, All Rights Reserved.
 * COPYING RESTRICTIONS APPLY, see COPYRIGHT file
 */
/*
 * tmplout.c:  display template library output routines for LDAP clients
 * 12 April 1994 by Mark C Smith
 */

#include "portable.h"
#include <stdarg.h>
#include <errno.h>

#include <stdio.h>

#include <ac/socket.h>
#include <ac/stdlib.h>

#include <ac/ctype.h>
#include <ac/string.h>
#include <ac/time.h>

#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif

#include <ldap.h>
#include <disptmpl.h>

#include "ldap_log.h"

#include "ldap-int.h"
#include "ldap_defaults.h"

#define writeptype ldap_writeptype

/* This struct contains all the data required for output */
struct obuf {
	writeptype	func;
	void *		parm;
	int		error;

	char *		base;
	unsigned int	ptr, size;
};

/* local functions */
static int  obuf_init(LDAP *, struct obuf *, writeptype, void *);
static int obuf_close(struct obuf *);
static void obuf_puts(struct obuf *, const char *);
static void obuf_puts_escaped(struct obuf *, const char *);
static void obuf_printf(struct obuf *, const char *, ...);

static int do_entry2text LDAP_P((
	LDAP *ld, struct obuf *, char *base, LDAPMessage *entry,
	struct ldap_disptmpl *tmpl, char **defattrs, char ***defvals,
	char *eol, int rdncount,
	unsigned long opts, char *urlprefix ));
static int do_entry2text_search LDAP_P((
	LDAP *ld, struct obuf *, char *dn, char *base,
	LDAPMessage *entry, struct ldap_disptmpl *tmpllist, char **defattrs,
	char ***defvals, char *eol,
	int rdncount, unsigned long opts, char *urlprefix ));
static int do_vals2text LDAP_P((
	LDAP *ld, struct obuf *, char **vals, char *label,
	int labelwidth, unsigned long syntaxid,
	char *eol, int rdncount, char *urlprefix ));
static int max_label_len LDAP_P(( struct ldap_disptmpl *tmpl ));
static int output_label LDAP_P((
	struct obuf *, char *label, int width, char *eol, int html ));
static int output_dn LDAP_P((
	struct obuf *, char *dn, int width, int rdncount,
	char *eol, char *urlprefix ));
static char *time2text LDAP_P(( char *ldtimestr, int dateonly ));
static long gtime LDAP_P(( struct tm *tm ));
static int searchaction LDAP_P((
	LDAP *ld, struct obuf *, char *base, LDAPMessage *entry,
	char *dn, struct ldap_tmplitem *tip, int labelwidth, int rdncount,
	char *eol, char *urlprefix ));

#define DEF_LABEL_WIDTH		15
#define SEARCH_TIMEOUT_SECS	120
#define OCATTRNAME		"objectClass"


#define NONFATAL_LDAP_ERR( err )	( (err) == LDAP_SUCCESS || \
	(err) == LDAP_TIMELIMIT_EXCEEDED || (err) == LDAP_SIZELIMIT_EXCEEDED )

#define DEF_LDAP_URL_PREFIX	"ldap:///"

 
int
ldap_entry2text(
	LDAP			*ld,
	char			*buf,		/* NULL for "use internal" */
	LDAPMessage		*entry,
	struct ldap_disptmpl	*tmpl,
	char			**defattrs,
	char			***defvals,
	writeptype		writeproc,
	void			*writeparm,
	char			*eol,
	int			rdncount,
	unsigned long		opts
)
{
    struct obuf	obuf;
    int		res;

    Debug( LDAP_DEBUG_TRACE, "ldap_entry2text\n", 0, 0, 0 );

    if (!obuf_init(ld, &obuf, writeproc, writeparm))
        return ld->ld_errno;
    res = do_entry2text(ld, &obuf, NULL, entry, tmpl, defattrs, defvals,
			eol, rdncount, opts, NULL );
    obuf_close(&obuf);

    return res;
}



int
ldap_entry2html(
	LDAP			*ld,
	char			*buf,		/* NULL for "use internal" */
	LDAPMessage		*entry,
	struct ldap_disptmpl	*tmpl,
	char			**defattrs,
	char			***defvals,
	writeptype		writeproc,
	void			*writeparm,
	char			*eol,
	int			rdncount,
	unsigned long		opts,
	char			*base,
	char			*urlprefix
)
{
    struct obuf	obuf;
    int		res;

    Debug( LDAP_DEBUG_TRACE, "ldap_entry2html\n", 0, 0, 0 );

    if ( urlprefix == NULL ) {
	urlprefix = DEF_LDAP_URL_PREFIX;
    }

    if (!obuf_init(ld, &obuf, writeproc, writeparm))
        return ld->ld_errno;
    res = do_entry2text( ld, &obuf, base, entry, tmpl, defattrs, defvals,
		eol, rdncount, opts, urlprefix );
    obuf_close(&obuf);

    return res;
}


static int
do_entry2text(
	LDAP			*ld,
	struct obuf		*bp,
	char			*base,		/* used for search actions */
	LDAPMessage		*entry,
	struct ldap_disptmpl	*tmpl,
	char			**defattrs,
	char			***defvals,
	char			*eol,
	int			rdncount,
	unsigned long		opts,
	char			*urlprefix	/* if non-NULL, do HTML */
)
{
    int				i, err, html, show, labelwidth;
    int				 freevals;
    char			*dn, **vals;
    struct ldap_tmplitem	*rowp, *colp;

    if (( dn = ldap_get_dn( ld, entry )) == NULL ) {
	return( ld->ld_errno );
    }

    html = ( urlprefix != NULL );

    if ( html ) {
	/*
	 * add HTML intro. and title
	 */
	if (!(( opts & LDAP_DISP_OPT_HTMLBODYONLY ) != 0 )) {
	    obuf_printf(bp,
	    	    "<HTML>%s<HEAD>%s<TITLE>%s%s - ", eol, eol, eol,
		    ( tmpl == NULL ) ? "Entry" : tmpl->dt_name );
	    output_dn( bp, dn, 0, rdncount, "", NULL );
	    obuf_printf(bp,
	    	    "%s</TITLE>%s</HEAD>%s<BODY>%s<H3>%s - ", eol, eol,
		    eol, eol, ( tmpl == NULL ) ? "Entry" : tmpl->dt_name );
	    output_dn( bp, dn, 0, rdncount, "", NULL );
	    obuf_printf(bp, "</H3>%s", eol );
	}

	if (( opts & LDAP_DISP_OPT_NONLEAF ) != 0 &&
		( vals = ldap_explode_dn( dn, 0 )) != NULL ) {
	    char	*untagged;

	    /*
	     * add "Move Up" link
	     */
	    obuf_printf(bp, "<A HREF=\"%s", urlprefix );
	    for ( i = 1; vals[ i ] != NULL; ++i ) {
		if ( i > 1 ) {
		     obuf_puts_escaped(bp, ", " );
		}
		obuf_puts_escaped(bp, vals[ i ] );
	    }
	    if ( vals[ 1 ] != NULL ) {
		untagged = strchr( vals[ 1 ], '=' );
	    } else {
		untagged = "=The World";
	    }
	    obuf_printf(bp,
		    "%s\">Move Up To <EM>%s</EM></A>%s<BR>",
		    ( vals[ 1 ] == NULL ) ? "??one" : "",
		    ( untagged != NULL ) ? untagged + 1 : vals[ 1 ], eol );

	    /*
	     * add "Browse" link
	     */
	    untagged = strchr( vals[ 0 ], '=' );
	    obuf_printf(bp, "<A HREF=\"%s", urlprefix );
	    obuf_puts_escaped(bp, dn);
	    obuf_printf(bp,
	    	    "??one?(!(objectClass=dsa))\">Browse Below <EM>%s</EM>"
		    "</A>%s%s",
		    ( untagged != NULL ) ? untagged + 1 : vals[ 0 ], eol, eol );

	    ldap_value_free( vals );
	}

	obuf_puts(bp, "<HR>");		/* horizontal rule */
    } else {
	obuf_puts(bp, "\"");
	output_dn(bp, dn, 0, rdncount, "", NULL );
	obuf_printf(bp, "\"%s", eol );
    }

    if ( tmpl != NULL && ( opts & LDAP_DISP_OPT_AUTOLABELWIDTH ) != 0 ) {
	labelwidth = max_label_len( tmpl ) + 3;
    } else {
	labelwidth = DEF_LABEL_WIDTH;;
    }

    err = LDAP_SUCCESS;

    if ( tmpl == NULL ) {
	BerElement	*ber;
	char		*attr;

	ber = NULL;
	for ( attr = ldap_first_attribute( ld, entry, &ber );
		NONFATAL_LDAP_ERR( err ) && attr != NULL;
		attr = ldap_next_attribute( ld, entry, ber )) {
	    if (( vals = ldap_get_values( ld, entry, attr )) == NULL ) {
		freevals = 0;
		if ( defattrs != NULL ) {
		    for ( i = 0; defattrs[ i ] != NULL; ++i ) {
			if ( strcasecmp( attr, defattrs[ i ] ) == 0 ) {
			    break;
			}
		    }
		    if ( defattrs[ i ] != NULL ) {
			vals = defvals[ i ];
		    }
		}
	    } else {
		freevals = 1;
	    }

	    *attr = TOUPPER( (unsigned char) *attr );

	    err = do_vals2text( ld, bp, vals, attr, labelwidth,
		    LDAP_SYN_CASEIGNORESTR, eol, 
		    rdncount, urlprefix );
	    if ( freevals ) {
		ldap_value_free( vals );
	    }
	}

	if( ber != NULL) {
		ber_free( ber, 0 );
	}
    } else {
	for ( rowp = ldap_first_tmplrow( tmpl );
		NONFATAL_LDAP_ERR( err ) && rowp != NULL;
		rowp = ldap_next_tmplrow( tmpl, rowp )) {
	    for ( colp = ldap_first_tmplcol( tmpl, rowp ); colp != NULL;
		    colp = ldap_next_tmplcol( tmpl, rowp, colp )) {
		vals = NULL;
		if ( colp->ti_attrname == NULL || ( vals = ldap_get_values( ld,
			entry, colp->ti_attrname )) == NULL ) {
		    freevals = 0;
		    if ( !LDAP_IS_TMPLITEM_OPTION_SET( colp,
			    LDAP_DITEM_OPT_HIDEIFEMPTY ) && defattrs != NULL
			    && colp->ti_attrname != NULL ) {
			for ( i = 0; defattrs[ i ] != NULL; ++i ) {
			    if ( strcasecmp( colp->ti_attrname, defattrs[ i ] )
				    == 0 ) {
				break;
			    }
			}
			if ( defattrs[ i ] != NULL ) {
			    vals = defvals[ i ];
			}
		    }
		} else {
		    freevals = 1;
		    if ( LDAP_IS_TMPLITEM_OPTION_SET( colp,
			    LDAP_DITEM_OPT_SORTVALUES ) && vals[ 0 ] != NULL
			    && vals[ 1 ] != NULL ) {
			ldap_sort_values( ld, vals, ldap_sort_strcasecmp );
		    }
		}

		/*
		 * don't bother even calling do_vals2text() if no values
		 * or boolean with value false and "hide if false" option set
		 */
		show = ( vals != NULL && vals[ 0 ] != NULL );
		if ( show && LDAP_GET_SYN_TYPE( colp->ti_syntaxid )
			== LDAP_SYN_TYPE_BOOLEAN && LDAP_IS_TMPLITEM_OPTION_SET(
			colp, LDAP_DITEM_OPT_HIDEIFFALSE ) &&
			TOUPPER( (unsigned char) vals[ 0 ][ 0 ] ) != 'T' ) {
		    show = 0;
		}

		if ( colp->ti_syntaxid == LDAP_SYN_SEARCHACTION ) {
		    if (( opts & LDAP_DISP_OPT_DOSEARCHACTIONS ) != 0 ) {
			if ( colp->ti_attrname == NULL || ( show &&
				TOUPPER( (unsigned char) vals[ 0 ][ 0 ] ) == 'T' )) {
			    err = searchaction( ld, bp, base, entry, dn, colp,
				    labelwidth, rdncount,
				    eol, urlprefix );
			}
		    }
		    show = 0;
		}

		if ( show ) {
		    err = do_vals2text( ld, bp, vals, colp->ti_label,
			labelwidth, colp->ti_syntaxid,
			eol, rdncount, urlprefix );
		}

		if ( freevals ) {
		    ldap_value_free( vals );
		}
	    }
	}
    }

    if ( html  && !(( opts & LDAP_DISP_OPT_HTMLBODYONLY ) != 0 )) {
	obuf_printf(bp, "</BODY>%s</HTML>%s", eol, eol );
    }

    LDAP_FREE( dn );
    return( err );
}

	
int
ldap_entry2text_search(
	LDAP			*ld,
	char			*dn,		/* if NULL, use entry */
	char			*base,		/* if NULL, no search actions */
	LDAPMessage		*entry, 	/* if NULL, use dn */
	struct ldap_disptmpl*	tmpllist,	/* if NULL, load default file */
	char			**defattrs,
	char			***defvals,
	writeptype		writeproc,
	void			*writeparm,
	char			*eol,
	int			rdncount,	/* if 0, display full DN */
	unsigned long		opts
)
{
    struct obuf	obuf;
    int		res;

    Debug( LDAP_DEBUG_TRACE, "ldap_entry2text_search\n", 0, 0, 0 );

    if (!obuf_init(ld, &obuf, writeproc, writeparm))
    	return ld->ld_errno;

    res = do_entry2text_search( ld, &obuf, dn, base, entry, tmpllist, defattrs,
	    defvals, eol, rdncount, opts, NULL );
    
    obuf_close(&obuf);
    return res;
}



int
ldap_entry2html_search(
	LDAP			*ld,
	char			*dn,		/* if NULL, use entry */
	char			*base,		/* if NULL, no search actions */
	LDAPMessage		*entry, 	/* if NULL, use dn */
	struct ldap_disptmpl*	tmpllist,	/* if NULL, load default file */
	char			**defattrs,
	char			***defvals,
	writeptype		writeproc,
	void			*writeparm,
	char			*eol,
	int			rdncount,	/* if 0, display full DN */
	unsigned long		opts,
	char			*urlprefix
)
{
    struct obuf	obuf;
    int		res;

    Debug( LDAP_DEBUG_TRACE, "ldap_entry2html_search\n", 0, 0, 0 );

    if (!obuf_init(ld, &obuf, writeproc, writeparm))
    	return ld->ld_errno;

    res = do_entry2text_search( ld, &obuf, dn, base, entry, tmpllist, defattrs,
	    defvals, eol, rdncount, opts, urlprefix );
    
    obuf_close(&obuf);
    return res;
}


static int
do_entry2text_search(
	LDAP			*ld,
	struct obuf		*bp,
	char			*dn,		/* if NULL, use entry */
	char			*base,		/* if NULL, no search actions */
	LDAPMessage		*entry, 	/* if NULL, use dn */
	struct ldap_disptmpl*	tmpllist,	/* if NULL, load default file */
	char			**defattrs,
	char			***defvals,
	char			*eol,
	int			rdncount,	/* if 0, display full DN */
	unsigned long		opts,
	char			*urlprefix
)
{
    int				err, freedn, freetmpls, html;
    char			**fetchattrs, **vals;
    LDAPMessage			*ldmp;
    struct ldap_disptmpl	*tmpl;
    struct timeval		timeout;

    if ( dn == NULL && entry == NULL ) {
	ld->ld_errno = LDAP_PARAM_ERROR;
	return( ld->ld_errno );
    }

    html = ( urlprefix != NULL );

    timeout.tv_sec = SEARCH_TIMEOUT_SECS;
    timeout.tv_usec = 0;

    freedn = freetmpls = 0;
    tmpl = NULL;

    if ( tmpllist == NULL ) {
	if (( err = ldap_init_templates( TEMPLATEFILE, &tmpllist )) != 0 ) {
	    obuf_printf(bp, "%sUnable to read template file %s (error %d)%s%s",
		    html ? "<!-- " : "", TEMPLATEFILE, err,
		    html ? "-->" : "", eol );
	}
	freetmpls = 1;
    }

    if ( dn == NULL ) {
	if (( dn = ldap_get_dn( ld, entry )) == NULL ) {
	    if ( freetmpls ) {
		ldap_free_templates( tmpllist );
	    }
	    return( ld->ld_errno );
	}
	freedn = 1;
    }


    if ( tmpllist != NULL ) {
	ldmp = NULL;

	if ( entry == NULL ) {
	    char	*ocattrs[2];

	    ocattrs[0] = OCATTRNAME;
	    ocattrs[1] = NULL;

		err = ldap_search_st( ld, dn, LDAP_SCOPE_BASE,
			NULL, ocattrs, 0, &timeout, &ldmp );
	    if ( err == LDAP_SUCCESS ) {
		entry = ldap_first_entry( ld, ldmp );
	    }
	}

	if ( entry != NULL ) {
	    vals = ldap_get_values( ld, entry, OCATTRNAME );
	    tmpl = ldap_oc2template( vals, tmpllist );
	    if ( vals != NULL ) {
		ldap_value_free( vals );
	    }
	}
	if ( ldmp != NULL ) {
	    ldap_msgfree( ldmp );
	}
    }

    entry = NULL;

    if ( tmpl == NULL ) {
	fetchattrs = NULL;
    } else {
	fetchattrs = ldap_tmplattrs( tmpl, NULL, 1, LDAP_SYN_OPT_DEFER );
    }

	err = ldap_search_st( ld, dn, LDAP_SCOPE_BASE, NULL,
		fetchattrs, 0, &timeout, &ldmp );

    if ( freedn ) {
	LDAP_FREE( dn );
    }
    if ( fetchattrs != NULL ) {
	ldap_value_free( fetchattrs );
    }

    if ( err != LDAP_SUCCESS ||
	    ( entry = ldap_first_entry( ld, ldmp )) == NULL ) {
	if ( freetmpls ) {
            ldap_free_templates( tmpllist );
        }
	return( ld->ld_errno );
    }

    err = do_entry2text( ld, bp, base, entry, tmpl, defattrs, defvals,
	    eol, rdncount, opts, urlprefix );

    if ( freetmpls ) {
	ldap_free_templates( tmpllist );
    }
    ldap_msgfree( ldmp );
    return( err );
}
	    

int
ldap_vals2text(
	LDAP			*ld,
	char			*buf,		/* NULL for "use internal" */
	char			**vals,
	char			*label,
	int			labelwidth,	/* 0 means use default */
	unsigned long		syntaxid,
	writeptype		writeproc,
	void			*writeparm,
	char			*eol,
	int			rdncount
)
{
    struct obuf	obuf;
    int		res;

    Debug( LDAP_DEBUG_TRACE, "ldap_vals2text\n", 0, 0, 0 );

    if (!obuf_init(ld, &obuf, writeproc, writeparm))
    	return ld->ld_errno;

    res = do_vals2text( ld, &obuf, vals, label, labelwidth, syntaxid,
		eol, rdncount, NULL );

    obuf_close(&obuf);
    return res;
}


int
ldap_vals2html(
	LDAP			*ld,
	char			*buf,		/* NULL for "use internal" */
	char			**vals,
	char			*label,
	int			labelwidth,	/* 0 means use default */
	unsigned long		syntaxid,
	writeptype		writeproc,
	void			*writeparm,
	char			*eol,
	int			rdncount,
	char			*urlprefix
)
{
    struct obuf	obuf;
    int		res;

    Debug( LDAP_DEBUG_TRACE, "ldap_vals2html\n", 0, 0, 0 );

    if ( urlprefix == NULL ) {
	urlprefix = DEF_LDAP_URL_PREFIX;
    }

    if (!obuf_init(ld, &obuf, writeproc, writeparm))
    	return ld->ld_errno;

    res = do_vals2text( ld, &obuf, vals, label, labelwidth, syntaxid,
			eol, rdncount, urlprefix );

    obuf_close(&obuf);
    return res;
}


static int
do_vals2text(
	LDAP			*ld,
	struct obuf		*bp,
	char			**vals,
	char			*label,
	int			labelwidth,	/* 0 means use default */
	unsigned long		syntaxid,
	char			*eol,
	int			rdncount,
	char			*urlprefix
)
{
    int		i, html, writeoutval, notascii;
    char	*p, *s, *outval;


    if ( vals == NULL ) {
	return( LDAP_SUCCESS );
    }

    html = ( urlprefix != NULL );

    switch( LDAP_GET_SYN_TYPE( syntaxid )) {
    case LDAP_SYN_TYPE_TEXT:
    case LDAP_SYN_TYPE_BOOLEAN:
	break;		/* we only bother with these two types... */
    default:
	return( LDAP_SUCCESS );
    }

    if ( labelwidth == 0 || labelwidth < 0 ) {
	labelwidth = DEF_LABEL_WIDTH;
    }

    output_label( bp, label, labelwidth, eol, html );

    for ( i = 0; vals[ i ] != NULL; ++i ) {
	for ( p = vals[ i ]; *p != '\0'; ++p ) {
	    if ( !isascii( *p )) {
		break;
	    }
	}
	notascii = ( *p != '\0' );
	outval = notascii ? "(unable to display non-ASCII text value)"
		: vals[ i ];

	writeoutval = 0;	/* if non-zero, write outval after switch */

	switch( syntaxid ) {
	case LDAP_SYN_CASEIGNORESTR:
	    ++writeoutval;
	    break;

	case LDAP_SYN_RFC822ADDR:
	    if ( html ) {
		obuf_printf(bp, "<DD><A HREF=\"mailto:" );
		obuf_puts_escaped(bp, outval );
		obuf_printf(bp, "\">%s</A><BR>%s", outval, eol );
	    } else {
		++writeoutval;
	    }
	    break;

	case LDAP_SYN_DN:	/* for now */
	    output_dn( bp, outval, labelwidth, rdncount,
		    eol, urlprefix );
	    break;

	case LDAP_SYN_MULTILINESTR:
	    if ( i > 0 && !html ) {
		output_label( bp, label, labelwidth, eol, html );
	    }

	    p = s = outval;
	    while (( s = strchr( s, '$' )) != NULL ) {
		*s++ = '\0';
		while ( isspace( (unsigned char) *s )) {
		    ++s;
		}
		if ( html ) {
		    obuf_printf( bp, "<DD>%s<BR>%s", p, eol );
		} else {
		    obuf_printf( bp, "%-*s%s%s", labelwidth, " ", p, eol );
		}
		p = s;
	    }
	    outval = p;
	    ++writeoutval;
	    break;

	case LDAP_SYN_BOOLEAN:
	    outval = TOUPPER((unsigned char) outval[0]) == 'T' ? "TRUE" : "FALSE";
	    ++writeoutval;
	    break;

	case LDAP_SYN_TIME:
	case LDAP_SYN_DATE:
	    outval = time2text( outval, syntaxid == LDAP_SYN_DATE );
	    ++writeoutval;
	    break;

	case LDAP_SYN_LABELEDURL:
	    if ( !notascii && ( p = strchr( outval, '$' )) != NULL ) {
		*p++ = '\0';
		while ( isspace( (unsigned char) *p )) {
		    ++p;
		}
		s = outval;
	    } else if ( !notascii && ( s = strchr( outval, ' ' )) != NULL ) {
		*s++ = '\0';
		while ( isspace( (unsigned char) *s )) {
		    ++s;
		}
		p = outval;
	    } else {
		s = "URL";
		p = outval;
	    }

	    /*
	     * at this point `s' points to the label & `p' to the URL
	     */
	    if ( html ) {
		obuf_printf( bp, "<DD><A HREF=\"%s\">%s</A><BR>%s", p, s, eol );
	    } else {
		obuf_printf( bp, "%-*s%s%s%-*s%s%s", labelwidth, " ",
		    s, eol, labelwidth + 2, " ",p , eol );
	    }
	    break;

	default:
	    obuf_printf( bp, " Can't display item type %ld%s",
		    syntaxid, eol );
	}

	if ( writeoutval ) {
	    if ( html ) {
		obuf_printf( bp, "<DD>%s<BR>%s", outval, eol );
	    } else {
		obuf_printf( bp, "%-*s%s%s", labelwidth, " ", outval, eol );
	    }
	}
    }

    return( LDAP_SUCCESS );
}


static int
max_label_len( struct ldap_disptmpl *tmpl )
{
    struct ldap_tmplitem	*rowp, *colp;
    int				len, maxlen;

    maxlen = 0;

    for ( rowp = ldap_first_tmplrow( tmpl ); rowp != NULL;
	    rowp = ldap_next_tmplrow( tmpl, rowp )) {
	for ( colp = ldap_first_tmplcol( tmpl, rowp ); colp != NULL;
		colp = ldap_next_tmplcol( tmpl, rowp, colp )) {
	    if (( len = strlen( colp->ti_label )) > maxlen ) {
		maxlen = len;
	    }
	}
    }

    return( maxlen );
}


static int
output_label( struct obuf *bp, char *label, int width,
	char *eol, int html )
{
    unsigned int	len;

    if ( html ) {
	obuf_printf( bp, "<DT><B>%s</B>", label );
    } else {
	obuf_printf( bp, " %s:", label );

	len = strlen(label) + 2;
	if (len < width)
	    obuf_printf( bp, "%-*s", width-len, " " );
	obuf_puts(bp, eol );
    }
}


static int
output_dn( struct obuf *bp, char *dn, int width, int rdncount,
	char *eol, char *urlprefix )
{
    char	**dnrdns;
    int		i;

    if (( dnrdns = ldap_explode_dn( dn, 1 )) == NULL ) {
	return( -1 );
    }

    if ( urlprefix != NULL ) {
	obuf_printf( bp, "<DD><A HREF=\"%s", urlprefix );
	obuf_puts_escaped(bp, dn );
	obuf_puts( bp, "\">" );
    } else if ( width > 0 ) {
	obuf_printf( bp, "%-*s", width, " " );
    }

    for ( i = 0; dnrdns[ i ] != NULL && ( rdncount == 0 || i < rdncount );
	    ++i ) {
	if ( i > 0 ) {
	    obuf_puts( bp, ", " );
	}
	obuf_puts( bp, dnrdns[ i ] );
    }

    if ( urlprefix != NULL ) {
	obuf_puts( bp, "</A><BR>" );
    }

    ldap_value_free( dnrdns );

    obuf_puts( bp, eol );
    return 0;
}



#define HREF_CHAR_ACCEPTABLE( c )	(( (c) >= '-' && (c) <= '9' ) || \
					 ( (c) >= '@' && (c) <= 'Z' ) || \
					 ( (c) == '_' )               || \
					 ( (c) >= 'a' && (c) <= 'z' ))

static void
strcat_escaped( char *s1, char *s2 )
{
    char	*p, *q;
    char	*hexdig = "0123456789ABCDEF";

    p = s1 + strlen( s1 );
    for ( q = s2; *q != '\0'; ++q ) {
	if ( HREF_CHAR_ACCEPTABLE( *q )) {
	    *p++ = *q;
	} else {
	    *p++ = '%';
	    *p++ = hexdig[ *q >> 4 ];
	    *p++ = hexdig[ *q & 0x0F ];
	}
    }

    *p = '\0';
}


#define GET2BYTENUM( p )	(( *(p) - '0' ) * 10 + ( *((p)+1) - '0' ))

static char *
time2text( char *ldtimestr, int dateonly )
{
    struct tm		t;
    char		*p, *timestr, zone, *fmterr = "badly formatted time";
    time_t		gmttime;
    char		timebuf[32];
	int ndigits;

	if (strlen( ldtimestr ) < 12 ) {
		return( fmterr );
	}

    for ( ndigits=0; isdigit((unsigned char) ldtimestr[ndigits]); ndigits++) {
		; /* EMPTY */
    }

	if ( ndigits != 12 && ndigits != 14) {
	    return( fmterr );
	}
	
    memset( (char *)&t, '\0', sizeof( struct tm ));

    p = ldtimestr;

	if( ndigits == 14) {
		/* came with a century */
		/* POSIX says tm_year should be year - 1900 */
    	t.tm_year = 100 * GET2BYTENUM( p ) - 1900;
		p += 2;
		t.tm_year += GET2BYTENUM( p ); p += 2;

	} else {
		/* came without a century */
		t.tm_year = GET2BYTENUM( p ); p += 2;

		/* Y2K hack - 2 digit years < 70 are 21st century */
		if( t.tm_year < 70 ) {
			t.tm_year += 100;
		}
	}

    t.tm_mon = GET2BYTENUM( p ) - 1; p += 2;
    t.tm_mday = GET2BYTENUM( p ); p += 2;
    t.tm_hour = GET2BYTENUM( p ); p += 2;
    t.tm_min = GET2BYTENUM( p ); p += 2;
    t.tm_sec = GET2BYTENUM( p ); p += 2;

    if (( zone = *p ) == 'Z' ) {	/* GMT */
	zone = '\0';	/* no need to indicate on screen, so we make it null */
    }

    gmttime = gtime( &t );

    timestr = ldap_pvt_ctime( &gmttime, timebuf );
   
    timestr[ strlen( timestr ) - 1 ] = zone;	/* replace trailing newline */
    if ( dateonly ) {
		AC_MEMCPY( timestr + 11, timestr + 20, strlen( timestr + 20 ) + 1 );
    }

    return( timestr );
}



/* gtime.c - inverse gmtime */
/* gtime(): the inverse of localtime().
	This routine was supplied by Mike Accetta at CMU many years ago.
 */

static const int dmsize[] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

#define	dysize(y)	\
	(((y) % 4) ? 365 : (((y) % 100) ? 366 : (((y) % 400) ? 365 : 366)))

/*
 * Y2K YEAR
 */
	/* per STDC & POSIX tm_year should be offset by 1900 */
#define YEAR_POSIX(y)		((y) + 1900)

	/*
	 * year is < 1900, year is offset by 1900
	 */
#define YEAR_CAREFUL(y)		((y) < 1900 ? (y) + 1900 : (y))

#define YEAR(y) YEAR_CAREFUL(y)

/*  */

static long	gtime ( struct tm *tm )
{
    register int    i,
                    sec,
                    mins,
                    hour,
                    mday,
                    mon,
                    year;
    register long   result;

    if ((sec = tm -> tm_sec) < 0 || sec > 59
	    || (mins = tm -> tm_min) < 0 || mins > 59
	    || (hour = tm -> tm_hour) < 0 || hour > 24
	    || (mday = tm -> tm_mday) < 1 || mday > 31
	    || (mon = tm -> tm_mon + 1) < 1 || mon > 12)
	return ((long) -1);
    if (hour == 24) {
	hour = 0;
	mday++;
    }
    year = YEAR (tm -> tm_year);

    result = 0L;
    for (i = 1970; i < year; i++)
	result += dysize (i);
    if (dysize (year) == 366 && mon >= 3)
	result++;
    while (--mon)
	result += dmsize[mon - 1];
    result += mday - 1;
    result = 24 * result + hour;
    result = 60 * result + mins;
    result = 60 * result + sec;

    return result;
}

static int
searchaction( LDAP *ld, struct obuf *bp, char *base, LDAPMessage *entry,
	char *dn, struct ldap_tmplitem *tip, int labelwidth, int rdncount,
	char *eol, char *urlprefix )
{
    int			err = 0, lderr, i, count, html;
    char		**vals, **members;
    char		*value, *filtpattern, *attr, *selectname;
    char		*retattrs[2], filter[ 256 ];
    LDAPMessage		*ldmp;
    struct timeval	timeout;

    html = ( urlprefix != NULL );

    for ( i = 0; tip->ti_args != NULL && tip->ti_args[ i ] != NULL; ++i ) {
	;
    }
    if ( i < 3 ) {
	return( LDAP_PARAM_ERROR );
    }
    attr = tip->ti_args[ 0 ];
    filtpattern = tip->ti_args[ 1 ];
    retattrs[ 0 ] = tip->ti_args[ 2 ];
    retattrs[ 1 ] = NULL;
    selectname = tip->ti_args[ 3 ];

    vals = NULL;
    if ( attr == NULL ) {
	value = NULL;
    } else if ( strcasecmp( attr, "-dnb" ) == 0 ) {
	return( LDAP_PARAM_ERROR );
    } else if ( strcasecmp( attr, "-dnt" ) == 0 ) {
	value = dn;
    } else if ( strcasecmp( attr, "-dn" ) == 0 ) {
	value = dn;
    } else if (( vals = ldap_get_values( ld, entry, attr )) != NULL ) {
	value = vals[ 0 ];
    } else {
	value = NULL;
    }

    ldap_build_filter( filter, sizeof( filter ), filtpattern, NULL, NULL, NULL,
	    value, NULL );

    if ( html ) {
	/*
	 * if we are generating HTML, we add an HREF link that embodies this
	 * search action as an LDAP URL, instead of actually doing the search
	 * now.
	 */
	obuf_printf( bp, "<DT><A HREF=\"%s", urlprefix );
	if ( base != NULL ) {
	    obuf_puts_escaped(bp, base );
	}
	obuf_puts_escaped(bp, "??sub?" );
	obuf_puts_escaped(bp, filter );
	obuf_printf(bp, "\"><B>%s</B></A><DD><BR>%s",
		tip->ti_label, eol );
	return( LDAP_SUCCESS );
    }

    timeout.tv_sec = SEARCH_TIMEOUT_SECS;
    timeout.tv_usec = 0;

	lderr = ldap_search_st( ld, base, LDAP_SCOPE_SUBTREE, filter, retattrs,
		0, &timeout, &ldmp );

    if ( lderr == LDAP_SUCCESS || NONFATAL_LDAP_ERR( lderr )) {
	if (( count = ldap_count_entries( ld, ldmp )) > 0 ) {
	    if (( members = (char **)LDAP_MALLOC( (count + 1) * sizeof(char *)))
		    == NULL ) {
		err = LDAP_NO_MEMORY;
	    } else {
		for ( i = 0, entry = ldap_first_entry( ld, ldmp );
			entry != NULL;
			entry = ldap_next_entry( ld, entry ), ++i ) {
		    members[ i ] = ldap_get_dn( ld, entry );
		}
		members[ i ] = NULL;

		ldap_sort_values( ld, members, ldap_sort_strcasecmp );

		err = do_vals2text( ld, bp, members, tip->ti_label,
			html ? -1 : 0, LDAP_SYN_DN, 
			eol, rdncount, urlprefix );

		ldap_value_free( members );
	    }
	}
	ldap_msgfree( ldmp );
    }

    
    if ( vals != NULL ) {
	ldap_value_free( vals );
    }

    return(( err == LDAP_SUCCESS ) ? lderr : err );
}

/*
 * All the obuf stuff
 * No big need to be efficient here...
 */
int
obuf_init(LDAP *ld, struct obuf *bp, writeptype func, void *parm)
{
    memset(bp, 0, sizeof(*bp));
    bp->func = func;
    bp->parm = parm;

    bp->size = LDAP_DTMPL_BUFSIZ;
    bp->base = (char *) LDAP_MALLOC(bp->size+1);
    if (bp->base == NULL) {
        ld->ld_errno = LDAP_NO_MEMORY;
        return 0;
    }
    return 1;
}

void
obuf_printf(struct obuf *bp, const char *fmt, ...)
{
    char	buffer[1024];
    va_list	ap;

    va_start(ap, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);

    obuf_puts(bp, buffer);
}

static void
obuf_flush(struct obuf *bp)
{
    if (bp->ptr && !bp->error) {
    	if (bp->func(bp->parm, bp->base, bp->ptr) < 0) {
	    /* Save the error for later if we haven't done so */
	    if (bp->error == 0)
	    	bp->error = errno;
	}
    }
    bp->base[0] = '\0';
    bp->ptr = 0;
}

static inline void
obuf_putc(struct obuf *bp, char c)
{
    if (bp->ptr >= bp->size)
        obuf_flush(bp);
    bp->base[bp->ptr++] = c;
}

void
obuf_puts(struct obuf *bp, const char *s)
{
    while (*s)
        obuf_putc(bp, *s++);
    obuf_flush(bp);
}

#define HREF_CHAR_ACCEPTABLE( c )	(( (c) >= '-' && (c) <= '9' ) || \
					 ( (c) >= '@' && (c) <= 'Z' ) || \
					 ( (c) == '_' )               || \
					 ( (c) >= 'a' && (c) <= 'z' ))

void
obuf_puts_escaped(struct obuf *bp, const char *s)
{
    static const char *hexdig = "0123456789ABCDEF";

    while (*s) {
	if (HREF_CHAR_ACCEPTABLE(*s))
            obuf_putc(bp, *s++);
	else {
            obuf_putc(bp, '%');
            obuf_putc(bp, hexdig[(*s >> 4) & 0xF]);
            obuf_putc(bp, hexdig[*s++ & 0xF]);
	}
    }
    obuf_flush(bp);
}

int
obuf_close(struct obuf *bp)
{
    int	res = 0;

    LDAP_FREE(bp->base);
    if (bp->error) {
    	errno = bp->error;
	res = -1;
    }
    memset(bp, 0, sizeof(*bp));
    return res;
}
