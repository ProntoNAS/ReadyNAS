/* modrdn.c - ldbm backend modrdn routine */
/* $OpenLDAP: pkg/ldap/servers/slapd/back-ldbm/modrdn.c,v 1.30.2.17 2002/01/29 19:29:39 kurt Exp $ */
/*
 * Copyright 1998-2002 The OpenLDAP Foundation, All Rights Reserved.
 * COPYING RESTRICTIONS APPLY, see COPYRIGHT file
 */

/*
 * LDAP v3 newSuperior support. Add new rdn as an attribute.
 * (Full support for v2 also used software/ideas contributed
 * by Roy Hooper rhooper@cyberus.ca, thanks to him for his
 * submission!.)
 *
 * Copyright 1999, Juan C. Gomez, All rights reserved.
 * This software is not subject to any license of Silicon Graphics 
 * Inc. or Purdue University.
 *
 * Redistribution and use in source and binary forms are permitted
 * without restriction or fee of any kind as long as this notice
 * is preserved.
 *
 */

#include "portable.h"

#include <stdio.h>

#include <ac/string.h>
#include <ac/socket.h>

#include "slap.h"
#include "back-ldbm.h"
#include "proto-back-ldbm.h"

int
ldbm_back_modrdn(
    Backend	*be,
    Connection	*conn,
    Operation	*op,
    const char	*dn,
    const char	*ndn,
    const char	*newrdn,
    int		deleteoldrdn,
    const char	*newSuperior
)
{
	AttributeDescription *children = slap_schema.si_ad_children;
	struct ldbminfo	*li = (struct ldbminfo *) be->be_private;
	char		*p_dn = NULL, *p_ndn = NULL;
	char		*new_dn = NULL, *new_ndn = NULL;
	Entry		*e, *p = NULL;
	Entry		*matched;
	int		isroot = -1;
	int		rootlock = 0;
#define CAN_ROLLBACK	-1
#define MUST_DESTROY	1
	int		rc = CAN_ROLLBACK;
	int 		rc_id = 0;
	ID              id = NOID;
	const char *text = NULL;
	char textbuf[SLAP_TEXT_BUFLEN];
	size_t textlen = sizeof textbuf;
	/* Added to support LDAP v2 correctly (deleteoldrdn thing) */
	char            **new_rdn_vals = NULL;  /* Vals of new rdn */
	char		**new_rdn_types = NULL;	/* Types of new rdn */
	int             a_cnt, d_cnt;
	char		*old_rdn = NULL;	/* Old rdn's attr type & val */
	char		**old_rdn_types = NULL;	/* Types of old rdn attrs. */
	char		**old_rdn_vals = NULL;	/* Old rdn attribute values */
	/* Added to support newSuperior */ 
	Entry		*np = NULL;	/* newSuperior Entry */
	char		*np_dn = NULL;	/* newSuperior dn */
	char		*np_ndn = NULL; /* newSuperior ndn */
	char		*new_parent_dn = NULL;	/* np_dn, p_dn, or NULL */
	/* Used to interface with ldbm_modify_internal() */
	Modifications	*mod = NULL;		/* Used to delete old/add new rdn */
	int		manageDSAit = get_manageDSAit( op );

	Debug( LDAP_DEBUG_TRACE, "==>ldbm_back_modrdn(newSuperior=%s)\n",
	       (newSuperior ? newSuperior : "NULL"),
	       0, 0 );

	/* grab giant lock for writing */
	ldap_pvt_thread_rdwr_wlock(&li->li_giant_rwlock);

	/* get entry with writer lock */
	if ( (e = dn2entry_w( be, ndn, &matched )) == NULL ) {
		char* matched_dn = NULL;
		struct berval** refs = NULL;

		if( matched != NULL ) {
			matched_dn = strdup( matched->e_dn );
			refs = is_entry_referral( matched )
				? get_entry_referrals( be, conn, op, matched )
				: NULL;
			cache_return_entry_r( &li->li_cache, matched );
		} else {
			refs = default_referral;
		}

		ldap_pvt_thread_rdwr_wunlock(&li->li_giant_rwlock);

		send_ldap_result( conn, op, LDAP_REFERRAL,
			matched_dn, NULL, refs, NULL );

		if ( matched != NULL ) {
			ber_bvecfree( refs );
			free( matched_dn );
		}

		return( -1 );
	}

	if (!manageDSAit && is_entry_referral( e ) ) {
		/* parent is a referral, don't allow add */
		/* parent is an alias, don't allow add */
		struct berval **refs = get_entry_referrals( be,
			conn, op, e );

		Debug( LDAP_DEBUG_TRACE, "entry is referral\n", 0,
		    0, 0 );

		send_ldap_result( conn, op, LDAP_REFERRAL,
		    e->e_dn, NULL, refs, NULL );

		ber_bvecfree( refs );
		goto return_results;
	}

	if ( has_children( be, e ) ) {
		Debug( LDAP_DEBUG_TRACE, "entry %s referral\n", e->e_dn,
		    0, 0 );

		send_ldap_result( conn, op, LDAP_NOT_ALLOWED_ON_NONLEAF,
		    NULL, "subtree rename not supported", NULL, NULL );
		goto return_results;
	}

	if ( (p_ndn = dn_parent( be, e->e_ndn )) != NULL && p_ndn[0] != '\0' ) {
		/* Make sure parent entry exist and we can write its 
		 * children.
		 */

		if( (p = dn2entry_w( be, p_ndn, NULL )) == NULL) {
			Debug( LDAP_DEBUG_TRACE, "parent does not exist\n",
				0, 0, 0);

			send_ldap_result( conn, op, LDAP_OTHER,
				NULL, "parent entry does not exist", NULL, NULL );

			goto return_results;
		}

		/* check parent for "children" acl */
		if ( ! access_allowed( be, conn, op, p,
			children, NULL, ACL_WRITE ) )
		{
			Debug( LDAP_DEBUG_TRACE, "no access to parent\n", 0,
				0, 0 );

			send_ldap_result( conn, op, LDAP_INSUFFICIENT_ACCESS,
				NULL, NULL, NULL, NULL );
			goto return_results;
		}

		Debug( LDAP_DEBUG_TRACE,
		       "ldbm_back_modrdn: wr to children of entry %s OK\n",
		       p_ndn, 0, 0 );

		p_dn = dn_parent( be, e->e_dn );

		Debug( LDAP_DEBUG_TRACE, "ldbm_back_modrdn: parent dn=%s\n",
		       p_dn, 0, 0 );

	} else {
		/* no parent, must be root to modify rdn */
		isroot = be_isroot( be, op->o_ndn );
		if ( ! be_isroot ) {
			if ( be_issuffix( be, "" )
					|| be_isupdate( be, op->o_ndn ) ) {
				static const Entry rootp = { NOID, "", "", NULL, NULL };
				p = (Entry *)&rootp;
				
				rc = access_allowed( be, conn, op, p,
						children, NULL, ACL_WRITE );
				p = NULL;
								
				/* check parent for "children" acl */
				if ( ! rc ) {
					Debug( LDAP_DEBUG_TRACE,
						"<=- ldbm_back_modrdn: no "
						"access to parent\n", 0, 0, 0 );

					send_ldap_result( conn, op, 
						LDAP_INSUFFICIENT_ACCESS,
						NULL, NULL, NULL, NULL );
					goto return_results;
				}

			} else {
				Debug( LDAP_DEBUG_TRACE,
					"<=- ldbm_back_modrdn: no parent & "
					"not root\n", 0, 0, 0);

				send_ldap_result( conn, op, 
					LDAP_INSUFFICIENT_ACCESS,
					NULL, NULL, NULL, NULL );
				goto return_results;
			}
		}

		Debug( LDAP_DEBUG_TRACE,
		       "ldbm_back_modrdn: no parent, locked root\n",
		       0, 0, 0 );
	}

	new_parent_dn = p_dn;	/* New Parent unless newSuperior given */

	if ( newSuperior != NULL ) {
		Debug( LDAP_DEBUG_TRACE, 
			"ldbm_back_modrdn: new parent \"%s\" requested...\n",
			newSuperior, 0, 0 );

		np_dn = ch_strdup( newSuperior );
		np_ndn = ch_strdup( np_dn );
		(void) dn_normalize( np_ndn );

		/* newSuperior == oldParent? */
		if ( strcmp( p_ndn, np_ndn ) == 0 ) {
			Debug( LDAP_DEBUG_TRACE, 
			       "ldbm_back_modrdn: new parent \"%s\" seems to be the same as old parent \"%s\"...\n",
			       newSuperior, p_dn, 0 );

			newSuperior = NULL; /* ignore newSuperior */
		}
	}

	if ( newSuperior != NULL ) {
		/* newSuperior == entry being moved?, if so ==> ERROR */
		/* Get Entry with dn=newSuperior. Does newSuperior exist? */

		if ( newSuperior[ 0 ] != '\0' ) {

			if( (np = dn2entry_w( be, np_ndn, NULL )) == NULL) {
				Debug( LDAP_DEBUG_TRACE,
				       "ldbm_back_modrdn: newSup(ndn=%s) not here!\n",
				       np_ndn, 0, 0);

				send_ldap_result( conn, op, LDAP_OTHER,
					NULL, "newSuperior not found", NULL, NULL );
				goto return_results;
			}

			Debug( LDAP_DEBUG_TRACE,
			       "ldbm_back_modrdn: wr to new parent OK np=%p, id=%ld\n",
			       np, np->e_id, 0 );

			/* check newSuperior for "children" acl */
			if ( !access_allowed( be, conn, op, np, children, NULL,
					      ACL_WRITE ) )
			{
				Debug( LDAP_DEBUG_TRACE,
				       "ldbm_back_modrdn: no wr to newSup children\n",
				       0, 0, 0 );

				send_ldap_result( conn, op, LDAP_INSUFFICIENT_ACCESS,
					NULL, NULL, NULL, NULL );
				goto return_results;
			}

			if ( is_entry_alias( np ) ) {
				/* parent is an alias, don't allow add */
				Debug( LDAP_DEBUG_TRACE, "entry is alias\n", 0, 0, 0 );

				send_ldap_result( conn, op, LDAP_ALIAS_PROBLEM,
				    NULL, "newSuperior is an alias", NULL, NULL );

				goto return_results;
			}

			if ( is_entry_referral( np ) ) {
				/* parent is a referral, don't allow add */
				Debug( LDAP_DEBUG_TRACE, "entry (%s) is referral\n",
					np->e_dn, 0, 0 );

				send_ldap_result( conn, op, LDAP_OPERATIONS_ERROR,
				    NULL, "newSuperior is a referral", NULL, NULL );

				goto return_results;
			}

		} else {

			/* no parent, must be root to modify newSuperior */
			if ( isroot == -1 ) {
				isroot = be_isroot( be, op->o_ndn );
			}

			if ( ! be_isroot ) {
				if ( be_issuffix( be, "" )
						|| be_isupdate( be, op->o_ndn ) ) {
					static const Entry rootp = { NOID, "", "", NULL, NULL };
					np = (Entry *)&rootp;
				
					rc = access_allowed( be, conn, op, np,
							children, NULL, ACL_WRITE );
					np = NULL;
								
					/* check parent for "children" acl */
					if ( ! rc ) {
						Debug( LDAP_DEBUG_TRACE,
							"<=- ldbm_back_modrdn: no "
							"access to new superior\n", 0, 0, 0 );

						send_ldap_result( conn, op, 
							LDAP_INSUFFICIENT_ACCESS,
							NULL, NULL, NULL, NULL );
						goto return_results;
					}

				} else {
					Debug( LDAP_DEBUG_TRACE,
						"<=- ldbm_back_modrdn: \"\" "
						"not allowed as new superior\n", 
						0, 0, 0);

					send_ldap_result( conn, op, 
						LDAP_INSUFFICIENT_ACCESS,
						NULL, NULL, NULL, NULL );
					goto return_results;
				}
			}
		}

		Debug( LDAP_DEBUG_TRACE,
		       "ldbm_back_modrdn: wr to new parent's children OK\n",
		       0, 0 , 0 );

		new_parent_dn = np_dn;
	}
	
	/* Build target dn and make sure target entry doesn't exist already. */
	build_new_dn( &new_dn, e->e_dn, new_parent_dn, newrdn ); 

	new_ndn = ch_strdup(new_dn);
	(void) dn_normalize( new_ndn );

	Debug( LDAP_DEBUG_TRACE, "ldbm_back_modrdn: new ndn=%s\n",
	       new_ndn, 0, 0 );

	/* check for abandon */
	ldap_pvt_thread_mutex_lock( &op->o_abandonmutex );
	if ( op->o_abandon ) {
		ldap_pvt_thread_mutex_unlock( &op->o_abandonmutex );
		goto return_results;
	}

	ldap_pvt_thread_mutex_unlock( &op->o_abandonmutex );
	if ( ( rc_id = dn2id ( be, new_ndn, &id ) ) || id != NOID ) {
		/* if (rc_id) something bad happened to ldbm cache */
		send_ldap_result( conn, op, 
			rc_id ? LDAP_OPERATIONS_ERROR : LDAP_ALREADY_EXISTS,
			NULL, NULL, NULL, NULL );
		goto return_results;
	}

	Debug( LDAP_DEBUG_TRACE,
	       "ldbm_back_modrdn: new ndn=%s does not exist\n",
	       new_ndn, 0, 0 );


	/* Get attribute types and values of our new rdn, we will
	 * need to add that to our new entry
	 */
	if ( rdn_attrs( newrdn, &new_rdn_types, &new_rdn_vals ) ) {
		Debug( LDAP_DEBUG_TRACE,
		       "ldbm_back_modrdn: can't figure out type(s)/value(s) of newrdn\n",
		       0, 0, 0 );

		send_ldap_result( conn, op, LDAP_OPERATIONS_ERROR,
			NULL, "unable to parse type(s)/value(s) used in RDN", NULL, NULL );
		goto return_results;		
	}

	Debug( LDAP_DEBUG_TRACE,
	       "ldbm_back_modrdn: new_rdn_val=\"%s\", new_rdn_type=\"%s\"\n",
	       new_rdn_vals[0], new_rdn_types[0], 0 );

	/* Retrieve the old rdn from the entry's dn */
	if ( (old_rdn = dn_rdn( be, dn )) == NULL ) {
		Debug( LDAP_DEBUG_TRACE,
		       "ldbm_back_modrdn: can't figure out old_rdn from dn\n",
		       0, 0, 0 );

		send_ldap_result( conn, op, LDAP_OTHER,
			NULL, "could not parse old DN", NULL, NULL );
		goto return_results;		
	}

	if ( rdn_attrs( old_rdn, &old_rdn_types, &old_rdn_vals ) ) {
		Debug( LDAP_DEBUG_TRACE,
		       "ldbm_back_modrdn: can't figure out the old_rdn type(s)/value(s)\n",
		       0, 0, 0 );

		send_ldap_result( conn, op, LDAP_OTHER,
			NULL, "unable to parse type(s)/value(s) used in RDN from old DN", NULL, NULL );
		goto return_results;		
	}
	
	if ( newSuperior == NULL
		&& charray_strcasecmp( (const char **)old_rdn_types, (const char **)new_rdn_types ) != 0 )
	{
	    /* Not a big deal but we may say something */
	    Debug( LDAP_DEBUG_TRACE,
		   "ldbm_back_modrdn: old_rdn_type=%s, new_rdn_type=%s!\n",
		   old_rdn_types[0], new_rdn_types[0], 0 );
	}		

	Debug( LDAP_DEBUG_TRACE, "ldbm_back_modrdn: DN_X500\n",
	       0, 0, 0 );

	mod = NULL;
	for ( a_cnt = 0; new_rdn_types[a_cnt]; a_cnt++ ) {
		int rc;
		Modifications *mod_tmp;
		struct berval val;

		mod_tmp = (Modifications *)ch_malloc( sizeof( Modifications ) );

		mod_tmp->sml_desc = NULL;
		rc = slap_str2ad( new_rdn_types[a_cnt], 
				&mod_tmp->sml_desc, &text );

		if ( rc != LDAP_SUCCESS ) {
			Debug( LDAP_DEBUG_TRACE,
				"ldbm_back_modrdn: %s: %s (new)\n",
				text, new_rdn_types[a_cnt], 0 );

			send_ldap_result( conn, op, rc,
				NULL, text, NULL, NULL );

			goto return_results;		
		}

		val.bv_val = new_rdn_vals[a_cnt];
		val.bv_len = strlen( val.bv_val );
		if ( ! access_allowed( be, conn, op, e, 
				mod_tmp->sml_desc, &val, ACL_WRITE ) ) {
			Debug( LDAP_DEBUG_TRACE,
				"ldbm_back_modrdn: access not allowed "
				"to attr \"%s\"\n%s%s",
				new_rdn_types[a_cnt], "", "" );
			send_ldap_result( conn, op, 
				LDAP_INSUFFICIENT_ACCESS,
				NULL, NULL, NULL, NULL );

			goto return_results;
		}

		mod_tmp->sml_bvalues = (struct berval **)ch_malloc( 2 * sizeof(struct berval *) );
		mod_tmp->sml_bvalues[0] = ber_bvstrdup( new_rdn_vals[a_cnt] );
		mod_tmp->sml_bvalues[1] = NULL;
		mod_tmp->sml_op = SLAP_MOD_SOFTADD;
		mod_tmp->sml_next = mod;
		mod = mod_tmp;
	}

	/* Remove old rdn value if required */
	if ( deleteoldrdn ) {
		/* Get value of old rdn */
		if ( old_rdn_vals == NULL ) {
			Debug( LDAP_DEBUG_TRACE,
			       "ldbm_back_modrdn: can't figure out oldRDN value(s) from old RDN\n",
			       0, 0, 0 );

			send_ldap_result( conn, op, LDAP_OTHER,
				NULL, "could not parse value(s) from old RDN", NULL, NULL );
			goto return_results;		
		}

		for ( d_cnt = 0; old_rdn_types[d_cnt]; d_cnt++ ) {    
			int rc;
			Modifications *mod_tmp;
			struct berval val;

			mod_tmp = (Modifications *)ch_malloc( sizeof( Modifications ) );

			mod_tmp->sml_desc = NULL;
			rc = slap_str2ad( old_rdn_types[d_cnt], 
					&mod_tmp->sml_desc, &text );

			if ( rc != LDAP_SUCCESS ) {
				Debug( LDAP_DEBUG_TRACE,
					"ldbm_back_modrdn: %s: %s (old)\n",
					text, old_rdn_types[d_cnt], 0 );

				send_ldap_result( conn, op, rc,
					NULL, text, NULL, NULL );

				goto return_results;
			}

			val.bv_val = old_rdn_vals[d_cnt];
			val.bv_len = strlen( val.bv_val );
			if ( ! access_allowed( be, conn, op, e, 
					mod_tmp->sml_desc, &val, ACL_WRITE ) ) {
				Debug( LDAP_DEBUG_TRACE,
					"ldbm_back_modrdn: access not allowed "
					"to attr \"%s\"\n%s%s",
					old_rdn_types[d_cnt], "", "" );
				send_ldap_result( conn, op, 
					LDAP_INSUFFICIENT_ACCESS,
					NULL, NULL, NULL, NULL );

				goto return_results;
			}

			/* Remove old value of rdn as an attribute. */
			mod_tmp->sml_bvalues = (struct berval **)ch_malloc( 2 * sizeof(struct berval *) );
			mod_tmp->sml_bvalues[0] = ber_bvstrdup( old_rdn_vals[d_cnt] );
			mod_tmp->sml_bvalues[1] = NULL;
			mod_tmp->sml_op = LDAP_MOD_DELETE;
			mod_tmp->sml_next = mod;
			mod = mod_tmp;

			Debug( LDAP_DEBUG_TRACE,
			       "ldbm_back_modrdn: removing old_rdn_val=%s\n",
			       old_rdn_vals[0], 0, 0 );
		}
	}

	
	/* check for abandon */
	ldap_pvt_thread_mutex_lock( &op->o_abandonmutex );
	if ( op->o_abandon ) {
		ldap_pvt_thread_mutex_unlock( &op->o_abandonmutex );
		goto return_results;
	}
	ldap_pvt_thread_mutex_unlock( &op->o_abandonmutex );

	/* delete old one */
	if ( dn2id_delete( be, e->e_ndn, e->e_id ) != 0 ) {
		send_ldap_result( conn, op, LDAP_OTHER,
			NULL, "DN index delete fail", NULL, NULL );
		goto return_results;
	}

	(void) cache_delete_entry( &li->li_cache, e );
	rc = MUST_DESTROY;

	/* XXX: there is no going back! */

	free( e->e_dn );
	free( e->e_ndn );
	e->e_dn = new_dn;
	e->e_ndn = new_ndn;
	new_dn = NULL;
	new_ndn = NULL;

	/* add new one */
	if ( dn2id_add( be, e->e_ndn, e->e_id ) != 0 ) {
		send_ldap_result( conn, op, LDAP_OTHER,
			NULL, "DN index add failed", NULL, NULL );
		goto return_results;
	}

	/* modify memory copy of entry */
	rc = ldbm_modify_internal( be, conn, op, dn, &mod[0], e,
		&text, textbuf, textlen );

	if( rc != LDAP_SUCCESS ) {
		if( rc != SLAPD_ABANDON ) {
			send_ldap_result( conn, op, rc,
				NULL, text, NULL, NULL );
		}

		/* here we may try to delete the newly added dn */
		if ( dn2id_delete( be, e->e_ndn, e->e_id ) != 0 ) {
			/* we already are in trouble ... */
			;
		}
	    
    		goto return_results;
	}
	
	(void) cache_update_entry( &li->li_cache, e );

	/* NOTE: after this you must not free new_dn or new_ndn!
	 * They are used by cache.
	 */

	/* id2entry index */
	if ( id2entry_add( be, e ) != 0 ) {
		send_ldap_result( conn, op, LDAP_OTHER,
			NULL, "entry update failed", NULL, NULL );
		goto return_results;
	}

	send_ldap_result( conn, op, LDAP_SUCCESS,
		NULL, NULL, NULL, NULL );
	rc = 0;
	cache_entry_commit( e );

return_results:
	if( new_dn != NULL ) free( new_dn );
	if( new_ndn != NULL ) free( new_ndn );

	if( p_dn != NULL ) free( p_dn );
	if( p_ndn != NULL ) free( p_ndn );

	/* LDAP v2 supporting correct attribute handling. */
	if( new_rdn_types != NULL ) charray_free( new_rdn_types );
	if( new_rdn_vals != NULL ) charray_free( new_rdn_vals );
	if( old_rdn != NULL ) free(old_rdn);
	if( old_rdn_types != NULL ) charray_free( old_rdn_types );
	if( old_rdn_vals != NULL ) charray_free( old_rdn_vals );

	if ( mod != NULL ) {
		slap_mods_free( mod );
	}

	/* LDAP v3 Support */
	if ( np_dn != NULL ) free( np_dn );
	if ( np_ndn != NULL ) free( np_ndn );

	if( np != NULL ) {
		/* free new parent and writer lock */
		cache_return_entry_w( &li->li_cache, np );
	}

	if( p != NULL ) {
		/* free parent and writer lock */
		cache_return_entry_w( &li->li_cache, p );
	}

	/* free entry and writer lock */
	cache_return_entry_w( &li->li_cache, e );
	if ( rc == MUST_DESTROY ) {
		/* if rc == MUST_DESTROY the entry is uncached 
		 * and its private data is destroyed; 
		 * the entry must be freed */
		entry_free( e );
	}
	ldap_pvt_thread_rdwr_wunlock(&li->li_giant_rwlock);
	return( rc );
}
