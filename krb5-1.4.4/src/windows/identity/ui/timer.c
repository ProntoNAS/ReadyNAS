/*
 * Copyright (c) 2005 Massachusetts Institute of Technology
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* $Id$ */

#include<khmapp.h>
#include<assert.h>

/* The minimum half time interval is 60 seconds*/
#define TT_MIN_HALFLIFE_INTERVAL 60

/* as above, in FILETIME units of 100ns */
#define FT_MIN_HALFLIFE_INTERVAL (TT_MIN_HALFLIFE_INTERVAL * 10000000i64)

/* in seconds */
#if 0
khm_int32 khui_timeout_warn = KHUI_DEF_TIMEOUT_WARN;
khm_int32 khui_timeout_crit = KHUI_DEF_TIMEOUT_CRIT;
khm_int32 khui_timeout_renew = KHUI_DEF_TIMEOUT_RENEW;

khm_boolean khui_do_renew = TRUE;
khm_boolean khui_do_warn = TRUE;
khm_boolean khui_do_crit = TRUE;
#endif

khui_timer_event * khui_timers = NULL;
khm_size khui_n_timers = 0;
khm_size khui_nc_timers = 0;

CRITICAL_SECTION cs_timers;

/*********************************************************************
  Timers
 *********************************************************************/


#define KHUI_TIMER_ALLOC_INCR 16

void 
khm_timer_init(void) {
#ifdef DEBUG
    assert(khui_timers == NULL);
#endif

    khui_nc_timers = KHUI_TIMER_ALLOC_INCR;
    khui_n_timers = 0;
    khui_timers = PMALLOC(sizeof(*khui_timers) * khui_nc_timers);

#ifdef DEBUG
    assert(khui_timers != NULL);
#endif

    InitializeCriticalSection(&cs_timers);
}

void
khm_timer_exit(void) {
    EnterCriticalSection(&cs_timers);

    if (khui_timers)
        PFREE(khui_timers);
    khui_timers = NULL;
    khui_n_timers = 0;
    khui_nc_timers = 0;

    LeaveCriticalSection(&cs_timers);
    DeleteCriticalSection(&cs_timers);
}

/* called with cs_timers held */
static void
tmr_fire_timer(void) {
    int i;
    unsigned __int64 curtime;
    unsigned __int64 err;
    unsigned __int64 next_event;
    int     tmr_count[KHUI_N_TTYPES];
    unsigned __int64 tmr_offset[KHUI_N_TTYPES];
    int t;
    khm_handle eff_ident = NULL;
    khui_timer_type eff_type = 0; /* meaningless */
    int fire_count = 0;
    FILETIME ft;

    TimetToFileTimeInterval(KHUI_TIMEEQ_ERROR_SMALL, &ft);
    err = FtToInt(&ft);
    GetSystemTimeAsFileTime(&ft);
    curtime = FtToInt(&ft);

    next_event = 0;

    ZeroMemory(tmr_count, sizeof(tmr_count));
    ZeroMemory(tmr_offset, sizeof(tmr_offset));

    for (i=0; i < (int) khui_n_timers; i++) {
        if (!(khui_timers[i].flags & 
              (KHUI_TE_FLAG_STALE | KHUI_TE_FLAG_EXPIRED)) &&
            khui_timers[i].type != KHUI_TTYPE_ID_MARK &&
            khui_timers[i].expire < curtime + err) {

            t = khui_timers[i].type;

            switch(t) {
            case KHUI_TTYPE_ID_RENEW:
                khm_cred_renew_identity(khui_timers[i].key);
                khui_timers[i].flags |= KHUI_TE_FLAG_EXPIRED;
                break;

            case KHUI_TTYPE_CRED_RENEW:
                /* the equivalence threshold for setting the timer is
                   a lot larger than what we are testing for here
                   (KHUI_TIMEEQ_ERROR vs KHUI_TIMEEQ_ERROR_SMALL) so
                   we assume that it is safe to trigger a renew_cred
                   call here without checking if there's an imminent
                   renew_identity call. */
                khm_cred_renew_cred(khui_timers[i].key);
                khui_timers[i].flags |= KHUI_TE_FLAG_EXPIRED;
                break;

            default:
                if (t < KHUI_N_TTYPES) {
                    tmr_count[t]++;
                    if (tmr_offset[t] == 0 ||
                        tmr_offset[t] > khui_timers[i].offset)
                        tmr_offset[t] = khui_timers[i].offset;
                    if (next_event == 0 ||
                        next_event > 
                        khui_timers[i].expire + khui_timers[i].offset)
                        next_event = khui_timers[i].expire +
                            khui_timers[i].offset;

                    if (eff_ident == NULL &&
                        (t == KHUI_TTYPE_ID_EXP ||
                         t == KHUI_TTYPE_ID_CRIT ||
                         t == KHUI_TTYPE_ID_WARN)) {
                        /* we don't need a hold since we will be done
                           with the handle before the marker is
                           expired (the marker is the timer with the
                           KHUI_TTYPE_ID_MARK which contains a held
                           handle and is not really a timer.) */
                        eff_ident = khui_timers[i].key;
                        eff_type = t;
                    }

                    fire_count++;

                    khui_timers[i].flags |= KHUI_TE_FLAG_EXPIRED;
                }
                else {
#ifdef DEBUG
                    assert(FALSE);
#endif
                }
            }
        }
    }

    /* See if we have anything to do */
    if (next_event == 0)
        return;
    else {
        wchar_t fmt[128];
        wchar_t wtime[128];
        wchar_t wmsg[256];
        wchar_t wtitle[64];
        unsigned __int64 second;
        khui_alert * alert = NULL;

        khm_size cb;

        next_event -= curtime;

        /* Due to measurement errors we may be slightly off on our
           next_event calculation which shows up as '4 mins 59
           seconds' instead of '5 mins' and so on when converting to a
           string.  So we add half a second to make the message
           neater. */
        TimetToFileTimeInterval(1, &ft);
        second = FtToInt(&ft);
        next_event += second / 2;

        cb = sizeof(wtime);
        ft = IntToFt(next_event);
        FtIntervalToString(&ft,
                           wtime,
                           &cb);

        if (fire_count == 1 &&
            eff_ident != NULL &&
            (eff_type == KHUI_TTYPE_ID_EXP ||
             eff_type == KHUI_TTYPE_ID_CRIT ||
             eff_type == KHUI_TTYPE_ID_WARN)) {

            wchar_t idname[KCDB_IDENT_MAXCCH_NAME];

            cb = sizeof(idname);
            kcdb_identity_get_name(eff_ident, idname, &cb);

            if (next_event < second) {
                LoadString(khm_hInstance, IDS_WARN_EXPIRED_ID,
                           fmt, ARRAYLENGTH(fmt));

                StringCbPrintf(wmsg, sizeof(wmsg), fmt, idname);
            } else {
                LoadString(khm_hInstance, IDS_WARN_EXPIRE_ID,
                           fmt, ARRAYLENGTH(fmt));

                StringCbPrintf(wmsg, sizeof(wmsg), fmt, idname, wtime);
            }
        } else {
            if (next_event < second) {
                LoadString(khm_hInstance, IDS_WARN_EXPIRED,
                           wmsg, ARRAYLENGTH(wmsg));
            } else {
                LoadString(khm_hInstance, IDS_WARN_EXPIRE, 
                           fmt, ARRAYLENGTH(fmt));

                StringCbPrintf(wmsg, sizeof(wmsg), fmt, wtime);
            }
        }

        LoadString(khm_hInstance, IDS_WARN_TITLE,
                   wtitle, ARRAYLENGTH(wtitle));

        khui_alert_create_simple(wtitle, wmsg, KHERR_WARNING, &alert);
        khui_alert_set_flags(alert, KHUI_ALERT_FLAG_REQUEST_BALLOON,
                             KHUI_ALERT_FLAG_REQUEST_BALLOON);
        khui_alert_show(alert);
        khui_alert_release(alert);
    }
}

void 
khm_timer_fire(HWND hwnd) {
    EnterCriticalSection(&cs_timers);
    tmr_fire_timer();
    LeaveCriticalSection(&cs_timers);

    khm_timer_refresh(hwnd);
}

static int
tmr_update(khm_handle key, khui_timer_type type, __int64 expire,
           __int64 offset, void * data, khm_boolean reinstate) {
    int i;

    for (i=0; i < (int) khui_n_timers; i++) {
        if (khui_timers[i].key == key &&
            khui_timers[i].type == type)
            break;
    }

    if (i >= (int) khui_n_timers) {
        i = (int) khui_n_timers;

        if (i >= (int) khui_nc_timers) {
            khui_timer_event * nt;
#ifdef DEBUG
            assert(khui_timers);
#endif
            khui_nc_timers = UBOUNDSS(i+1, KHUI_TIMER_ALLOC_INCR,
                                      KHUI_TIMER_ALLOC_INCR);
            nt = PMALLOC(sizeof(*nt) * khui_nc_timers);
#ifdef DEBUG
            assert(nt);
#endif
            memcpy(nt, khui_timers, sizeof(*nt) * khui_n_timers);

            PFREE(khui_timers);
            khui_timers = nt;
        }

        khui_timers[i].key = key;
        khui_timers[i].type = type;
        khui_timers[i].flags = 0;
        khui_n_timers++;
    }

    khui_timers[i].expire = expire;
    khui_timers[i].offset = offset;
    khui_timers[i].data = data;

    khui_timers[i].flags &= ~KHUI_TE_FLAG_STALE;
    if (reinstate)
        khui_timers[i].flags &= ~KHUI_TE_FLAG_EXPIRED;

    return i;
}

/* called with cs_timers held */
static int
tmr_find(khm_handle key, khui_timer_type type,
         khm_int32 and_flags, khm_int32 eq_flags) {
    int i;

    eq_flags &= and_flags;

    for (i=0; i < (int) khui_n_timers; i++) {
        if (khui_timers[i].key == key &&
            khui_timers[i].type == type &&
            (khui_timers[i].flags & and_flags) == eq_flags)
            break;
    }

    if (i < (int) khui_n_timers)
        return i;
    else
        return -1;
}

/* called with cs_timers held. */
static FILETIME
tmr_next_halflife_timeout(int idx, FILETIME * issue, FILETIME * expire) {
    FILETIME lifetime;
    FILETIME current;
    FILETIME ret;

    khm_int64 ilife;
    khm_int64 icurrent;
    khm_int64 iexpire;

    khm_int64 iret;

    GetSystemTimeAsFileTime(&current);

    /* wha?? */
    if (CompareFileTime(issue, expire) >= 0)
        return current;

    lifetime = FtSub(expire, issue);
    icurrent = FtToInt(&current);
    iexpire = FtToInt(expire);

    ilife = FtToInt(&lifetime);

    while(ilife / 2 > FT_MIN_HALFLIFE_INTERVAL) {
        ilife /= 2;

        /* is this the next renewal time? */
        if (iexpire - ilife > icurrent) {
            if (idx >= 0 &&
                khui_timers[idx].expire == iexpire - ilife &&
                (khui_timers[idx].flags & KHUI_TE_FLAG_EXPIRED)) {

                /* if this renewal time has already been triggered
                   (note that when the timer fires, it also fires all
                   events that are within a few seconds of the current
                   time) then we need to set the alarm for the next
                   slot down the line. */

                continue;

            } else {
                break;
            }
        }
    }

    iret = iexpire - ilife;

    ret = IntToFt(iret);

    /* if the previous renew timer had fired, we need to mark it as
       not expired.  However, we leave it to the caller to update the
       actual timer and mark it as not stale. */
    if (idx >= 0 &&
        khui_timers[idx].expire < (khm_ui_8) iret) {

        khui_timers[idx].flags &= ~KHUI_TE_FLAG_EXPIRED;
        khui_timers[idx].expire = iret;
    }

    return ret;
}

/* called with cs_timers held */
static khm_int32 KHMAPI
tmr_cred_apply_proc(khm_handle cred, void * rock) {
    khm_handle ident = NULL;
    int mark_idx;
    int idx;
    FILETIME ft_expiry;
    FILETIME ft_current;
    FILETIME ft_creinst;
    FILETIME ft_cred_expiry;
    FILETIME ft_cred_issue;
    FILETIME ft_issue;
    FILETIME ft;
    FILETIME fte;
    FILETIME ft_reinst;
    khm_size cb;

    kcdb_cred_get_identity(cred, &ident);
#ifdef DEBUG
    assert(ident);
#endif

    /* now get the expiry */
    cb = sizeof(ft_expiry);
    if (KHM_FAILED(kcdb_identity_get_attr(ident, KCDB_ATTR_EXPIRE,
                                          NULL,
                                          &ft_expiry, &cb))) {
        cb = sizeof(ft_expiry);
        if (KHM_FAILED(kcdb_cred_get_attr(cred, KCDB_ATTR_EXPIRE,
                                          NULL,
                                          &ft_expiry, &cb))) {
            /* we don't have an expiry time to work with */
            kcdb_identity_release(ident);
            return KHM_ERROR_SUCCESS;
        }
    }

    cb = sizeof(ft_issue);
    if (KHM_FAILED(kcdb_identity_get_attr(ident, KCDB_ATTR_ISSUE,
                                          NULL,
                                          &ft_issue, &cb))) {
        cb = sizeof(ft_issue);
        if (KHM_FAILED(kcdb_cred_get_attr(cred, KCDB_ATTR_ISSUE,
                                          NULL,
                                          &ft_issue, &cb))) {
            /* we don't really abandon the timer.  In this case, we
               fall back to using the threshold value to set the
               expiry timer. */
            ZeroMemory(&ft_issue, sizeof(ft_issue));
        }
    }

    /* and the current time */
    GetSystemTimeAsFileTime(&ft_current);

    TimetToFileTimeInterval(KHUI_TIMEEQ_ERROR, &ft_reinst);

    ft_creinst = FtAdd(&ft_current, &ft_reinst);

    mark_idx = tmr_find(ident, KHUI_TTYPE_ID_MARK, 0, 0);

    if (mark_idx < 0) {
        mark_idx = tmr_update(ident, KHUI_TTYPE_ID_MARK, 0, 0, 0, FALSE);
        kcdb_identity_hold(ident);
#ifdef DEBUG
        assert(mark_idx >= 0);
#endif
        khui_timers[mark_idx].flags |= KHUI_TE_FLAG_STALE;
    }

    if (khui_timers[mark_idx].flags & KHUI_TE_FLAG_STALE) {
        /* first time we are touching this */
        khm_handle csp_cw = NULL;
        khm_handle csp_id = NULL;
        khm_int32 rv;
        khm_int32 t;
        khm_boolean do_warn = TRUE;
        khm_boolean do_crit = TRUE;
        khm_boolean do_renew = TRUE;
        khm_boolean do_halflife = TRUE;
        khm_boolean renew_done = FALSE;
        khm_boolean monitor = TRUE;
        khm_int32 to_warn = KHUI_DEF_TIMEOUT_WARN;
        khm_int32 to_crit = KHUI_DEF_TIMEOUT_CRIT;
        khm_int32 to_renew = KHUI_DEF_TIMEOUT_RENEW;

        if (CompareFileTime(&ft_expiry, &ft_current) < 0)
            /* already expired */
            goto _done_with_ident;

        rv = khc_open_space(NULL, L"CredWindow", KHM_PERM_READ, 
                            &csp_cw);

        assert(KHM_SUCCEEDED(rv));

        rv = kcdb_identity_get_config(ident, KHM_PERM_READ, &csp_id);
        if (KHM_SUCCEEDED(rv)) {
            khc_shadow_space(csp_id, csp_cw);
            khc_close_space(csp_cw);
        } else {
            csp_id = csp_cw;
        }
        csp_cw = NULL;

        rv = khc_read_int32(csp_id, L"Monitor", &t);
        if (KHM_SUCCEEDED(rv))
            monitor = t;

        rv = khc_read_int32(csp_id, L"AllowWarn", &t);
        if (KHM_SUCCEEDED(rv))
            do_warn = t;

        rv = khc_read_int32(csp_id, L"AllowCritical", &t);
        if (KHM_SUCCEEDED(rv)) 
            do_crit = t;

        rv = khc_read_int32(csp_id, L"AllowAutoRenew", &t);
        if (KHM_SUCCEEDED(rv))
            do_renew = t;

        rv = khc_read_int32(csp_id, L"RenewAtHalfLife", &t);
        if (KHM_SUCCEEDED(rv))
            do_halflife = t;

        rv = khc_read_int32(csp_id, L"WarnThreshold", &t);
        if (KHM_SUCCEEDED(rv))
            to_warn = t;

        rv = khc_read_int32(csp_id, L"CriticalThreshold", &t);
        if (KHM_SUCCEEDED(rv))
            to_crit = t;

        rv = khc_read_int32(csp_id, L"AutoRenewThreshold", &t);
        if (KHM_SUCCEEDED(rv))
            to_renew = t;

        khc_close_space(csp_id);

        if (monitor && do_renew) {
            int prev;

            TimetToFileTimeInterval(to_renew, &ft);

            prev =
                tmr_find(ident, KHUI_TTYPE_ID_RENEW, 0, 0);

            if (do_halflife)
                fte = tmr_next_halflife_timeout(prev, &ft_issue, &ft_expiry);
            else
                fte = FtSub(&ft_expiry, &ft);

            /* we set off a renew notification immediately if the
               renew threshold has passed but a renew was never sent.
               This maybe because that NetIDMgr was started at the
               last minute, or because for some reason the renew timer
               could not be triggered earlier. */

            if (CompareFileTime(&fte, &ft_current) > 0 ||
                prev == -1 ||
                !(khui_timers[prev].flags & KHUI_TE_FLAG_EXPIRED)) {

                if (CompareFileTime(&fte, &ft_current) < 0)
                    fte = ft_current;

                tmr_update(ident, KHUI_TTYPE_ID_RENEW, 
                           FtToInt(&fte), FtToInt(&ft), 0,
                           CompareFileTime(&fte,&ft_creinst) > 0);
                renew_done = TRUE;

            } else {

                /* special case.  If the renew timer was in the past
                   and it was expired, then we retain the record as
                   long as the credentials are around.  If the renewal
                   failed we don't want to automatically retry
                   everytime we check the timers. */

                tmr_update(ident, KHUI_TTYPE_ID_RENEW,
                           FtToInt(&fte), FtToInt(&ft), 0, FALSE);

            }
        }

        if (monitor && do_warn && !renew_done) {

            TimetToFileTimeInterval(to_warn, &ft);
            fte = FtSub(&ft_expiry, &ft);

            if (CompareFileTime(&fte, &ft_current) > 0)
                tmr_update(ident, KHUI_TTYPE_ID_WARN,
                           FtToInt(&fte), FtToInt(&ft), 0,
                           CompareFileTime(&fte, &ft_creinst) > 0);
        }

        if (monitor && do_crit && !renew_done) {
            TimetToFileTimeInterval(to_crit, &ft);
            fte = FtSub(&ft_expiry, &ft);

            if (CompareFileTime(&fte, &ft_current) > 0)
                tmr_update(ident, KHUI_TTYPE_ID_CRIT,
                           FtToInt(&fte), FtToInt(&ft), 0,
                           CompareFileTime(&fte, &ft_creinst) > 0);
        }

        if (monitor && !renew_done) {
            if (CompareFileTime(&ft_expiry, &ft_current) > 0)
                tmr_update(ident, KHUI_TTYPE_ID_EXP, 
                           FtToInt(&ft_expiry), 0, 0,
                           CompareFileTime(&fte, &ft_creinst) > 0);
        }

    _done_with_ident:
        khui_timers[mark_idx].flags &= ~KHUI_TE_FLAG_STALE;
    }

    cb = sizeof(ft_cred_expiry);
    if (KHM_FAILED(kcdb_cred_get_attr(cred, KCDB_ATTR_EXPIRE,
                                      NULL,
                                      &ft_cred_expiry,
                                      &cb)))
        goto _cleanup;

    cb = sizeof(ft_cred_issue);
    if (KHM_FAILED(kcdb_cred_get_attr(cred, KCDB_ATTR_ISSUE,
                                      NULL,
                                      &ft_cred_issue,
                                      &cb)))
        goto _cleanup;

    TimetToFileTimeInterval(KHUI_TIMEEQ_ERROR, &ft);

    {
        /* if the credential has a longer lifetime than the identity,
           or it expires within KHUI_TIMEEQ_ERROR seconds of the
           identity, then we don't need to set any alerts for this
           credential. */

        FILETIME ft_delta;

        ft_delta = FtSub(&ft_expiry, &ft_cred_expiry);

        if (CompareFileTime(&ft_cred_expiry, &ft_expiry) >= 0 ||
            CompareFileTime(&ft_delta, &ft) < 0)
            goto _cleanup;
    }

    if ((idx = tmr_find(ident, KHUI_TTYPE_ID_WARN, 0, 0)) >= 0 &&
        !(khui_timers[idx].flags & KHUI_TE_FLAG_STALE)) {

        fte = IntToFt(FtToInt(&ft_cred_expiry) - khui_timers[idx].offset);
        if (CompareFileTime(&fte, &ft_current) > 0) {
	    tmr_update(cred, KHUI_TTYPE_CRED_WARN,
		       FtToInt(&fte), 
		       khui_timers[idx].offset, 0,
		       CompareFileTime(&fte, &ft_creinst) > 0);
	    kcdb_cred_hold(cred);
	}
    }

    if ((idx = tmr_find(ident, KHUI_TTYPE_ID_CRIT, 0, 0)) >= 0 &&
        !(khui_timers[idx].flags & KHUI_TE_FLAG_STALE)) {

        fte = IntToFt(FtToInt(&ft_cred_expiry) - khui_timers[idx].offset);
        if (CompareFileTime(&fte, &ft_current) > 0) {
            tmr_update(cred, KHUI_TTYPE_CRED_CRIT,
                       FtToInt(&fte),
                       khui_timers[idx].offset, 0,
                       CompareFileTime(&fte, &ft_creinst) > 0);
            kcdb_cred_hold(cred);
        }
    }

    if ((idx = tmr_find(ident, KHUI_TTYPE_ID_RENEW, 0, 0)) >= 0 &&
        !(khui_timers[idx].flags & KHUI_TE_FLAG_STALE)) {

        //fte = IntToFt(FtToInt(&ft_cred_expiry) - khui_timers[idx].offset);
        fte = tmr_next_halflife_timeout(idx, &ft_cred_issue, &ft_cred_expiry);

        if (CompareFileTime(&fte, &ft_current) > 0) {
            tmr_update(cred, KHUI_TTYPE_CRED_RENEW,
                       FtToInt(&fte),
                       khui_timers[idx].offset, 0,
                       CompareFileTime(&fte, &ft_creinst) > 0);
            kcdb_cred_hold(cred);
        }
    }

    if ((idx = tmr_find(ident, KHUI_TTYPE_ID_EXP, 0, 0)) >= 0 &&
        !(khui_timers[idx].flags & KHUI_TE_FLAG_STALE)) {

        if (CompareFileTime(&ft_cred_expiry, &ft_current) > 0) {
            tmr_update(cred, KHUI_TTYPE_CRED_EXP,
                       FtToInt(&ft_cred_expiry),
                       0, 0,
                       CompareFileTime(&ft_cred_expiry, &ft_creinst) > 0);
        }
    }

 _cleanup:

    if (ident)
        kcdb_identity_release(ident);

    return KHM_ERROR_SUCCESS;
}

/* called with cs_timers held */
static void
tmr_purge(void) {
    int i, j;

    for (i=0,j=0; i < (int) khui_n_timers; i++) {
        if (khui_timers[i].flags & KHUI_TE_FLAG_STALE) {
            if (khui_timers[i].type == KHUI_TTYPE_ID_MARK) {
                kcdb_identity_release(khui_timers[i].key);
#ifdef DEBUG
                {
                    int idx;

                    idx = tmr_find(khui_timers[i].key, 
                                   KHUI_TTYPE_ID_CRIT, 0, 0);
                    assert(idx < 0 || 
                           (khui_timers[idx].flags & 
                            KHUI_TE_FLAG_STALE));

                    idx = tmr_find(khui_timers[i].key, 
                                   KHUI_TTYPE_ID_RENEW, 0, 0);
                    assert(idx < 0 || 
                           (khui_timers[idx].flags & 
                            KHUI_TE_FLAG_STALE));

                    idx = tmr_find(khui_timers[i].key, 
                                   KHUI_TTYPE_ID_WARN, 0, 0);
                    assert(idx < 0 || 
                           (khui_timers[idx].flags & 
                            KHUI_TE_FLAG_STALE));

                    idx = tmr_find(khui_timers[i].key, 
                                   KHUI_TTYPE_ID_EXP, 0, 0);
                    assert(idx < 0 || 
                           (khui_timers[idx].flags & 
                            KHUI_TE_FLAG_STALE));
                }
#endif
            } else if (khui_timers[i].type == KHUI_TTYPE_CRED_WARN ||
                       khui_timers[i].type == KHUI_TTYPE_CRED_CRIT ||
                       khui_timers[i].type == KHUI_TTYPE_CRED_RENEW ||
                       khui_timers[i].type == KHUI_TTYPE_CRED_EXP) {
                kcdb_cred_release(khui_timers[i].key);
            }
        } else {
            if (i != j)
                khui_timers[j] = khui_timers[i];
            j++;
        }
    }

    khui_n_timers = j;
}

/* go through all the credentials and set timers as appropriate. */
void 
khm_timer_refresh(HWND hwnd) {
    int i;
    unsigned __int64 next_event = 0;
    unsigned __int64 curtime;
    unsigned __int64 diff;

    EnterCriticalSection(&cs_timers);

    KillTimer(hwnd, KHUI_TRIGGER_TIMER_ID);

    /* When refreshing timers, we go through all of them and mark them
       as stale.  Then we go through the credentials in the root
       credential set and add or refresh the timers associated with
       each identity and credential.  Once this is done, we remove the
       timers that are still stale, since they are no longer in
       use. */

    for (i=0; i < (int) khui_n_timers; i++) {
#ifdef NOT_IMPLEMENTED_YET
        if (khui_timers[i].type == KHUI_TTYPE_BMSG ||
            khui_timers[i].type == KHUI_TTYPE_SMSG) {
            khui_timers[i].flags &= ~KHUI_TE_FLAG_STALE;
        } else {
#endif

            khui_timers[i].flags |= KHUI_TE_FLAG_STALE;

#ifdef NOT_IMPLEMENTED_YET
	}
#endif
    }

    kcdb_credset_apply(NULL,
                       tmr_cred_apply_proc,
                       NULL);

    tmr_purge();

 _check_next_event:

    /* Before we return, we should check if any timers are set to
       expire right now.  If there are, we should fire the timer
       before returning. */

    next_event = 0;
    for (i=0; i < (int) khui_n_timers; i++) {
        if (!(khui_timers[i].flags & KHUI_TE_FLAG_EXPIRED) &&
            khui_timers[i].type != KHUI_TTYPE_ID_MARK &&
            (next_event == 0 ||
             next_event > khui_timers[i].expire))

            next_event = khui_timers[i].expire;
    }

    if (next_event != 0) {
        FILETIME ft;

        GetSystemTimeAsFileTime(&ft);
        curtime = FtToInt(&ft);

        TimetToFileTimeInterval(KHUI_TIMEEQ_ERROR_SMALL, &ft);
        diff = FtToInt(&ft);

        if (curtime + diff > next_event) {
            tmr_fire_timer();
            goto _check_next_event;
        } else {
            diff = next_event - curtime;
            ft = IntToFt(diff);
            SetTimer(hwnd,
                     KHUI_TRIGGER_TIMER_ID,
                     FtIntervalToMilliseconds(&ft),
                     NULL);
        }
    }

    LeaveCriticalSection(&cs_timers);
}
