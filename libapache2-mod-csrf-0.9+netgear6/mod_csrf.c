/* -*-mode: c; indent-tabs-mode: nil; c-basic-offset: 2; -*-
 */

/**
 * mod_csrf - Cross-site request forgery protection module for
 *            the Apache web server
 *
 * Copyright (C) 2012-2016 Christoph Steigmeier, Pascal Buchbinder
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/************************************************************************
 * Version
 ***********************************************************************/
static const char g_revision[] = "0.9";

/************************************************************************
 * Includes
 ***********************************************************************/
/* openssl */
#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/dh.h>
#include <openssl/bn.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/err.h>

/* apache */
#include <httpd.h>
#include <http_config.h>
#include <http_core.h>
#include <http_main.h>
#include <http_request.h>
#include <http_connection.h>
#include <http_protocol.h>
#include <http_log.h>
#include <util_filter.h>
#include <util_md5.h>
#include <ap_regex.h>

/* apr */
#include <apr_hooks.h>
#include <apr_strings.h>
#include <apr_date.h>
#include <apr_base64.h>
#include <apr_lib.h>

/************************************************************************
 * defines
 ***********************************************************************/
#define CSRF_LOG_PFX(id)  "mod_csrf("#id"): "
#define CSRF_LOGD_PFX  "mod_csrf(): "

#define CSRF_IGNORE_PATTERN ".*(jpg)|(jpeg)|(gif)|(png)|(js)|(css)$"
#define CSRF_IGNORE_CACHE "mod_csrf::ignore"
#define CSRF_IGNORE "CSRF_IGNORE"
#define CSRF_ENFORCE "CSRF_ENFORCE"
#define CSRF_REDIRECT "CSRF_REDIRECT"

#define CSRF_QUERYID "csrfpId"
#define CSRF_WIN 8
#define CSRF_ENABLE_WINDOW 1
#define CSRF_CHUNKED_ONLY 1
/* div */
#define CSRF_RAND_SIZE 16
#define CSRF_IDDELIM "#"
#define CSRF_DEFAULT_TIMEOUT 3600
#define CSRF_DEFAULT_PATH "/csrf.js"

// env variable to read id from
#define CSRF_ATTRIBUTE "CSRF_ATTRIBUTE"

// Apache 2.4 compat
#if (AP_SERVER_MINORVERSION_NUMBER == 4)
#define CSRF_ISDEBUG(s) APLOG_IS_LEVEL(s, APLOG_DEBUG)
#else
#define CSRF_ISDEBUG(s) s->loglevel >= APLOG_DEBUG
#endif

/************************************************************************
 * structures
 ***********************************************************************/
/*
 * server configuration
 */
#define CSRF_FUNC_FLAGS_SCRIPT     0x01
#define CSRF_FUNC_FLAGS_KEY        0x02
#define CSRF_FUNC_FLAGS_TMO        0x04

typedef enum  {
  CSRF_ACTION_DENY_DEFAULT = 0,
  CSRF_ACTION_DENY,
  CSRF_ACTION_LOG
} csrf_action_e;

typedef struct {
  int flags;
  ap_regex_t *ignore_pattern; /** path pattern which disables request check */
  int enabled;                /** enabled by default (-1) or by user (1) */
  csrf_action_e action;
  const char *id;
  unsigned char *sec;
  int sec_len;
  unsigned char key[EVP_MAX_KEY_LENGTH];
  char *path2script;
  apr_time_t timeout;
  int referer_check;          /** enabled by default (-1) or by user (1) */
} csrf_srv_config_t;

typedef struct {
  int flags;
  int enabled;                /** enabled by default (-1) or by user (1) */
  char *path2script;
  int referer_check;          /** enabled by default (-1) or by user (1) */
} csrf_dir_config_t;

typedef enum  {
  CSRF_RES_NEW = 0,
  CSRF_RES_SEARCH_HEAD,
  CSRF_RES_SEARCH_BODY,
  CSRF_RES_SEARCH_END
} csrf_conn_state_e;

typedef struct {
  csrf_conn_state_e state;
  char *search;
  char *body_window;
  char *script; // meta loading the script
  char *method; // javascript code calling the inject method
  apr_pool_t *pool;
} csrf_req_ctx;

/************************************************************************
 * globals
 ***********************************************************************/
module AP_MODULE_DECLARE_DATA csrf_module;

/* mod_parp, forward and optional function */
APR_DECLARE_OPTIONAL_FN(apr_table_t *, parp_hp_table, (request_rec *));
static APR_OPTIONAL_FN_TYPE(parp_hp_table) *csrf_parp_hp_table_fn = NULL;

/************************************************************************
 * private
 ***********************************************************************/

static const char csrf_basis_64[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-";

static const unsigned char csrf_pr2six[256] = {
    /* ASCII table */
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 63, 64, 64,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 62,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

static int csrf_token64_encode_len(int len) {
  return ((len + 2) / 3 * 4) + 1;
}

static int csrf_token64_encode_binary(char *encoded,
                                      const unsigned char *string, int len) {
  int i;
  char *p;

  p = encoded;
  for (i = 0; i < len - 2; i += 3) {
    *p++ = csrf_basis_64[(string[i] >> 2) & 0x3F];
    *p++ = csrf_basis_64[((string[i] & 0x3) << 4) |
                    ((int) (string[i + 1] & 0xF0) >> 4)];
    *p++ = csrf_basis_64[((string[i + 1] & 0xF) << 2) |
                    ((int) (string[i + 2] & 0xC0) >> 6)];
    *p++ = csrf_basis_64[string[i + 2] & 0x3F];
  }
  if (i < len) {
    *p++ = csrf_basis_64[(string[i] >> 2) & 0x3F];
    if (i == (len - 1)) {
      *p++ = csrf_basis_64[((string[i] & 0x3) << 4)];
      *p++ = '=';
    }
    else {
      *p++ = csrf_basis_64[((string[i] & 0x3) << 4) |
                      ((int) (string[i + 1] & 0xF0) >> 4)];
      *p++ = csrf_basis_64[((string[i + 1] & 0xF) << 2)];
    }
    *p++ = '=';
  }
  
  *p++ = '\0';
  return (int)(p - encoded);
}

/**
 * Base64-like data encoding (no "/" nor "+")
 *
 * @param encoded Pre-allocated buffer to write data to
 * @param buf Data to encode
 * @param len Length of the data in the paramter buf
 * @return Lentgh of the encoded string
 */
static int csrf_token64_encode(char *encoded, const char *buf, int len) {
  return csrf_token64_encode_binary(encoded, (const unsigned char *) buf, len);
}

/**
 * Determines the max. buffer length required to decode the string
 *
 * @param bufcoded String to decode
 * @return The maximum required buffer length
 */ 
static int csrf_token64_decode_len(const char *bufcoded) {
  int nbytesdecoded;
  register const unsigned char *bufin;
  register apr_size_t nprbytes;
  
  bufin = (const unsigned char *) bufcoded;
  while (csrf_pr2six[*(bufin++)] <= 63);
  
  nprbytes = (bufin - (const unsigned char *) bufcoded) - 1;
  nbytesdecoded = (((int)nprbytes + 3) / 4) * 3;
  
  return nbytesdecoded + 1;
}

static int csrf_token64_decode_binary(unsigned char *bufplain,
                                      const char *bufcoded) {
  int nbytesdecoded;
  register const unsigned char *bufin;
  register unsigned char *bufout;
  register apr_size_t nprbytes;
  
  bufin = (const unsigned char *) bufcoded;
  while (csrf_pr2six[*(bufin++)] <= 63);
  nprbytes = (bufin - (const unsigned char *) bufcoded) - 1;
  nbytesdecoded = (((int)nprbytes + 3) / 4) * 3;
  
  bufout = (unsigned char *) bufplain;
  bufin = (const unsigned char *) bufcoded;
  
  while (nprbytes > 4) {
    *(bufout++) =
      (unsigned char) (csrf_pr2six[*bufin] << 2 | csrf_pr2six[bufin[1]] >> 4);
    *(bufout++) =
      (unsigned char) (csrf_pr2six[bufin[1]] << 4 | csrf_pr2six[bufin[2]] >> 2);
    *(bufout++) =
      (unsigned char) (csrf_pr2six[bufin[2]] << 6 | csrf_pr2six[bufin[3]]);
    bufin += 4;
    nprbytes -= 4;
  }
  
  /* Note: (nprbytes == 1) would be an error, so just ingore that case */
  if (nprbytes > 1) {
    *(bufout++) =
      (unsigned char) (csrf_pr2six[*bufin] << 2 | csrf_pr2six[bufin[1]] >> 4);
  }
  if (nprbytes > 2) {
    *(bufout++) =
      (unsigned char) (csrf_pr2six[bufin[1]] << 4 | csrf_pr2six[bufin[2]] >> 2);
  }
  if (nprbytes > 3) {
    *(bufout++) =
      (unsigned char) (csrf_pr2six[bufin[2]] << 6 | csrf_pr2six[bufin[3]]);
  }
  
  nbytesdecoded -= (4 - (int)nprbytes) & 3;
  return nbytesdecoded;
}

/**
 * Base64-like string decoding
 *
 * @param bufplain Pre allocated string to write decoded data to
 * @param bufcoded String of the encoded data.
 * @return Length of bufplain
 */
static int csrf_token64_decode(char *bufplain, const char *bufcoded) {
  return csrf_token64_decode_binary((unsigned char *) bufplain, bufcoded);
}

static const char *csrf_get_uniqueid(request_rec *r) {
  // TODO: requires mod_unique_id (write error message at startup if it has not been loaded)
  const char *id = apr_table_get(r->subprocess_env, "UNIQUE_ID");
  if(id == NULL) {
    id = apr_pstrdup(r->pool, "-");
  }
  return id;
}

/**
 * Similar to standard strstr() but case insensitive and lenght limitation
 * (char which is not 0 terminated).
 *
 * @param s1 String to search in
 * @param s2 Pattern to ind
 * @param len Length of s1
 * @return pointer to the beginning of the substring s2 within s1, or NULL
 *         if the substring is not found
 */
static const char *csrf_strncasestr(const char *s1, const char *s2, int len) {
  const char *e1 = &s1[len-1];
  char *p1, *p2;
  if (*s2 == '\0') {
    /* an empty s2 */
    return((char *)s1);
  }
  while(1) {
    for ( ; (*s1 != '\0') && (s1 <= e1) && (apr_tolower(*s1) != apr_tolower(*s2)); s1++);
    if (*s1 == '\0' || s1 > e1) {
      return(NULL);
    }
    /* found first character of s2, see if the rest matches */
    p1 = (char *)s1;
    p2 = (char *)s2;
    for (++p1, ++p2; (apr_tolower(*p1) == apr_tolower(*p2)) && (p1 <= e1); ++p1, ++p2) {
      if((p1 > e1) && (*p2 != '\0')) {
        // reached the end without match
        return NULL;
      }
      if (*p2 == '\0') {
        /* both strings ended together */
        return((char *)s1);
      }
    }
    if (*p2 == '\0') {
      /* second string ended, a match */
      break;
    }
    /* didn't find a match here, try starting at next character in s1 */
    s1++;
  }
  return((char *)s1);
}

/**
 * Checks if csrf has been enabled.
 *
 * @param r
 * @return 1 if it has been enabled for this request
 */
static int csrf_enabled(request_rec *r) {
  csrf_srv_config_t *sconf = ap_get_module_config(r->server->module_config, &csrf_module);
  csrf_dir_config_t *dconf = ap_get_module_config(r->per_dir_config, &csrf_module);
  if(dconf->enabled == 0) {
    /* disbaled for this location (ignore server config) */
    return 0;
  }
  if(sconf->enabled == 0 && dconf->enabled == -1 ) {
    /* disabled for this server (no location config) */
    return 0;
  }
  return 1;
}

/**
 * We ignore (don't require an id) some types of requests
 * - static content like images
 *
 * @param r
 * @return 1 if we don't check this request, 0 if we do
 */
static int csrf_ignore_req(request_rec *r) {
  csrf_srv_config_t *sconf = ap_get_module_config(r->server->module_config, &csrf_module);

  if (r->parsed_uri.path && r->parsed_uri.query &&
    (r->method_number == M_GET) &&
    !strcmp(r->parsed_uri.path, "/dbbroker") &&
    (!strncmp(r->parsed_uri.query, "start_svc=", 10) ||
    !strncmp(r->parsed_uri.query, "stop_svc=", 9) ||
    !strncmp(r->parsed_uri.query, "download_logs=", 14) ||
    !strcmp(r->parsed_uri.query, "diagnostics") ||
    !strcmp(r->parsed_uri.query, "reboot") ||
    !strcmp(r->parsed_uri.query, "shutdown") ||
    !strcmp(r->parsed_uri.query, "logout"))) {
      ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r,
                    CSRF_LOGD_PFX"ignores request = %s, query = %s",
                    r->parsed_uri.path, r->parsed_uri.query);

      return 1;
  }

  if(apr_table_get(r->subprocess_env, CSRF_ENFORCE)) {
    return 0;
  }
  if(apr_table_get(r->notes, CSRF_IGNORE_CACHE)) {
    // cache: check regex only once
    return 1;
  }
  if(apr_table_get(r->subprocess_env, CSRF_IGNORE)) {
    // ignore by env variable
    ap_log_rerror(APLOG_MARK, APLOG_NOERRNO|APLOG_DEBUG, 0, r,
                  CSRF_LOGD_PFX"ignores request by '"CSRF_IGNORE"' env variable, id=%s",
                  csrf_get_uniqueid(r));
    return 1;
  }
  if(r->parsed_uri.path) {
    const char *path = strrchr(r->parsed_uri.path, '/'); // faster than match against a long string
    if(path == NULL) {
      path = r->parsed_uri.path;
    }
    if(ap_regexec(sconf->ignore_pattern, path, 0, NULL, 0) == 0) {
      apr_table_set(r->notes, CSRF_IGNORE_CACHE, "i");
      ap_log_rerror(APLOG_MARK, APLOG_NOERRNO|APLOG_DEBUG, 0, r,
                    CSRF_LOGD_PFX"ignores request '%s' by pattern, id=%s",
                    r->parsed_uri.path, csrf_get_uniqueid(r));
      // ignored by handler pattern
      return 1;
    }
  }
  return 0;
}

/**
 * Returns a table containing the query name/value pairs.
 *
 * @param r
 * @return Table of NULL if no parameter are available
 */
static apr_table_t *csrf_get_query(request_rec *r) {
  apr_table_t *qt = NULL;
  const char *args = r->args;
  if(args == NULL) {
    return NULL;
  }
  qt = apr_table_make(r->pool, 10);
  while(args[0]) {
    char *value = ap_getword(r->pool, &args, '&');
    char *name = ap_getword_nc(r->pool, &value, '=');
    if(name) {
      apr_table_addn(qt, name, value);   
    }
  }
  return qt;
}

/**
 * Crypto routines (from mod_auth_oid.c)
 */
static char *csrf_dec64(request_rec *r, const char *str) {
  csrf_srv_config_t *sconf = ap_get_module_config(r->server->module_config, &csrf_module);
  EVP_CIPHER_CTX cipher_ctx;
  unsigned char *hashIn;
  unsigned char hash[APR_MD5_DIGESTSIZE];
  apr_md5_ctx_t md5;
  int len = 0;
  int buf_len = 0;
  unsigned char *buf;
  char *dec = (char *)apr_palloc(r->pool, 1 + csrf_token64_decode_len(str));
  int dec_len = csrf_token64_decode(dec, str);
  buf = apr_pcalloc(r->pool, dec_len);

  EVP_CIPHER_CTX_init(&cipher_ctx);
  EVP_DecryptInit(&cipher_ctx, EVP_des_ede3_cbc(), sconf->key, NULL);
  if(!EVP_DecryptUpdate(&cipher_ctx, (unsigned char *)&buf[buf_len], &len,
                        (const unsigned char *)dec, dec_len)) {
    goto failed;
  }
  buf_len+=len;
  if(!EVP_DecryptFinal(&cipher_ctx, (unsigned char *)&buf[buf_len], &len)) {
    goto failed;
  }
  buf_len+=len;
  EVP_CIPHER_CTX_cleanup(&cipher_ctx);
  
  if(buf_len < (CSRF_RAND_SIZE + APR_MD5_DIGESTSIZE)) {
    goto failed;
  }
  hashIn = &buf[CSRF_RAND_SIZE];
  buf = &hashIn[APR_MD5_DIGESTSIZE];
  buf_len = buf_len - CSRF_RAND_SIZE - APR_MD5_DIGESTSIZE;

  apr_md5_init(&md5);
  apr_md5_update(&md5, buf, buf_len);
  apr_md5_final(hash, &md5);

  if(memcmp(hash, hashIn, APR_MD5_DIGESTSIZE) != 0) {
    ap_log_rerror(APLOG_MARK, APLOG_NOERRNO|APLOG_INFO, 0, r,
                  CSRF_LOG_PFX(010)"Failed to decrypt data, id=%s", csrf_get_uniqueid(r));
    return apr_pstrdup(r->pool, "");
  }

  return apr_pstrndup(r->pool, (char *)buf, buf_len);
     
 failed:
  EVP_CIPHER_CTX_cleanup(&cipher_ctx);
  ap_log_rerror(APLOG_MARK, APLOG_NOERRNO|APLOG_INFO, 0, r,
                CSRF_LOG_PFX(010)"Failed to decrypt data, id=%s", csrf_get_uniqueid(r));
  return apr_pstrdup(r->pool, "");
}

static char *csrf_enc64(request_rec *r, const char *str) {
  csrf_srv_config_t *sconf = ap_get_module_config(r->server->module_config, &csrf_module);
  char *e;
  EVP_CIPHER_CTX cipher_ctx;
  unsigned char hash[APR_MD5_DIGESTSIZE];
  apr_md5_ctx_t md5;
  int buf_len = 0;
  int len = 0;
  int str_len = strlen(str);
  unsigned char *rand = apr_pcalloc(r->pool, CSRF_RAND_SIZE);
  unsigned char *buf = apr_pcalloc(r->pool,
                                   CSRF_RAND_SIZE +
                                   APR_MD5_DIGESTSIZE +
                                   str_len +
                                   EVP_CIPHER_block_size(EVP_des_ede3_cbc()));

  // hash
  apr_md5_init(&md5);
  apr_md5_update(&md5, str, str_len);
  apr_md5_final(hash, &md5);

  // make string unique
#if APR_HAS_RANDOM
  apr_generate_random_bytes(rand, CSRF_RAND_SIZE);
#else
  RAND_bytes(rand, CSRF_RAND_SIZE);
#endif

  // encrypt the lot
  EVP_CIPHER_CTX_init(&cipher_ctx);
  EVP_EncryptInit(&cipher_ctx, EVP_des_ede3_cbc(), sconf->key, NULL);
  if(!EVP_EncryptUpdate(&cipher_ctx, &buf[buf_len], &len,
                        rand, CSRF_RAND_SIZE)) {
    goto failed;
  }
  buf_len+=len;
  if(!EVP_EncryptUpdate(&cipher_ctx, &buf[buf_len], &len,
                        hash, APR_MD5_DIGESTSIZE)) {
    goto failed;
  }
  buf_len+=len;
  if(!EVP_EncryptUpdate(&cipher_ctx, &buf[buf_len], &len,
                        (const unsigned char *)str, str_len)) {
    goto failed;
  }
  buf_len+=len;
  if(!EVP_EncryptFinal(&cipher_ctx, &buf[buf_len], &len)) {
    goto failed;
  }
  buf_len+=len;
  EVP_CIPHER_CTX_cleanup(&cipher_ctx);

  // better to use our own encoding (not base64, avoid "+", "/", and "=" chars)
  e = (char *)apr_pcalloc(r->pool, 1 + csrf_token64_encode_len(buf_len));
  len = csrf_token64_encode(e, (const char *)buf, buf_len);
  e[len] = '\0';

  return e;

failed:
  EVP_CIPHER_CTX_cleanup(&cipher_ctx);
  ap_log_rerror(APLOG_MARK, APLOG_NOERRNO|APLOG_INFO, 0, r,
                CSRF_LOG_PFX(011)"Failed to encrypt data, id=%s", csrf_get_uniqueid(r));
  return "";
}

/**
 * Returns this id string for this request
 */
static char *csrf_idstr(request_rec *r) {
  const char *csrf_att = apr_table_get(r->subprocess_env, CSRF_ATTRIBUTE);
  if(CSRF_ISDEBUG(r->server)) {
    ap_log_rerror(APLOG_MARK, APLOG_NOERRNO|APLOG_DEBUG, 0, r,
                  CSRF_LOGD_PFX""CSRF_ATTRIBUTE"=%s, id=%s",
                  csrf_att ? csrf_att : "-",
                  csrf_get_uniqueid(r));
  }
  if(csrf_att == NULL) {
    return apr_pstrdup(r->pool, "-");
  }
  return apr_pstrdup(r->pool, csrf_att);
}

/**
 * Generate an id which contains defined data from the
 * request header if available and a timestamp.
 * Then encrypt the id with a predefined or random
 * secret.
 */
static char *csrf_create_id(request_rec *r) {
  char *csrf_att = csrf_idstr(r);
  char *id = apr_pstrcat(r->pool,
                         apr_psprintf(r->pool, "%"APR_TIME_T_FMT"", r->request_time),
                         CSRF_IDDELIM,
                         csrf_att,
                         NULL);
  return csrf_enc64(r, id);
}

/**
 * Validates the received id.
 * @return 1 on success
 */
static int csrf_validate_id(request_rec *r, const char *encid, char **msg) {
  csrf_srv_config_t *sconf = ap_get_module_config(r->server->module_config, &csrf_module);
  char *valid_csrf_att = csrf_idstr(r);
  char *id = csrf_dec64(r, encid);
  char *csrf_att = strstr(id, CSRF_IDDELIM);
  if(csrf_att && csrf_att[1]) {
    apr_time_t request_time;
    csrf_att[0] = '\0';
    csrf_att++;
    request_time = apr_atoi64(id);
    if((request_time + sconf->timeout) > r->request_time) {
      if(strcmp(valid_csrf_att, csrf_att) == 0) {
        return 1;
      } else {
        *msg = apr_psprintf(r->pool, "invalid id (%s)", csrf_att);
      }
    } else {
      apr_time_t exp = r->request_time - (request_time + sconf->timeout); 
      *msg = apr_psprintf(r->pool, "expired id (%"APR_TIME_T_FMT" sec)",
                          apr_time_sec(exp));
    }
  } else {
    *msg = apr_psprintf(r->pool, "invalid id format or signature");
  }
  return 0;
}

/**
 * Compares the hostname within the Referer http header against the
 * host name within the Host http header.
 * - Direct page access (user has entered the url manually) does not contain a referer header
 * - User following a link within the page has a matching referer
 * - Resources within the page have a matching referer header
 * - Ajax calls within the page have a matching referer header
 * - Page access from a forein web site does contain a referer which does NOT match
 * @return 1 on success
 */
static int csrf_referer_check(request_rec *r, char **msg) {
  csrf_srv_config_t *sconf = ap_get_module_config(r->server->module_config, &csrf_module);
  csrf_dir_config_t *dconf = ap_get_module_config(r->per_dir_config, &csrf_module);
  int enabled = sconf->referer_check;
  if(dconf->referer_check != -1) {
    // dir config may override server default
    enabled = dconf->referer_check;
  }
  if(enabled == 0) {
    // disabled by configuration
    return 1;
  } else {
    const char *referer = apr_table_get(r->headers_in, "Referer");
    const char *host = apr_table_get(r->headers_in, "Host");
    if(!referer) {
      // url entered manually, allow this
      return 1;
    }
    if(host) {
      // allow only matching requests
      apr_uri_t parsed_uri_r;
      apr_uri_t parsed_uri_h;
      host = apr_pstrcat(r->pool, "http://", host, NULL);
      if(apr_uri_parse(r->pool, referer, &parsed_uri_r) == APR_SUCCESS &&
         apr_uri_parse(r->pool, host, &parsed_uri_h) == APR_SUCCESS) {
        if((parsed_uri_r.hostname == NULL) && (parsed_uri_r.path != NULL)) {
          // relative URI
          return 1;
        }
        if(parsed_uri_r.hostname &&
           parsed_uri_h.hostname &&
           strcmp(parsed_uri_r.hostname, parsed_uri_h.hostname) == 0) {
          // matching absolute URI
          return 1;
        }
      }
      *msg = apr_psprintf(r->pool, "referer header check failed, host[%s] referer[%s]", host, referer);
    }
  }
  return 0;
}

/**
 * Verifies that a valid csrf request id could be found
 *
 * @param r
 * @param tl Table containg the request parameters
 * @param idheader The ID may be transmited by a HTTP header
 * @param msg Error message if validation fails
 * @return 1 on success (0 if request id is not available or invalid)
 */
static int csrf_validate_req_id(request_rec *r, apr_table_t *tl, 
                                const char *idheader, char **msg) {
  csrf_srv_config_t *sconf = ap_get_module_config(r->server->module_config, &csrf_module);
  const char *csrfid = NULL;
  if(tl) {
    csrfid = apr_table_get(tl, sconf->id);
  }
  if(csrfid == NULL) {
    // not in query/body: by header? (ajax)
    csrfid = idheader;
  }
  if(csrfid != NULL) {
    return csrf_validate_id(r, csrfid, msg);
  }
  *msg = apr_psprintf(r->pool, "no '%s' parameter in request", sconf->id);
  return 0;
}

static const char *csrf_get_contenttype(request_rec *r) {
  const char* type = NULL;
  type = apr_table_get(r->headers_out, "Content-Type");
  if(type == NULL) {
    type = apr_table_get(r->err_headers_out, "Content-Type"); // maybe an error page
  }
  if(type == NULL) {
    type = r->content_type;
  }
  return type;
}

/**
 * Injects a new bucket containing a reference to the java script.
 *
 * @param r
 * @param bb
 * @param b Bucket to split and insert date new bucket at the postion of the marker
 * @param rctx Request context containing the state of the parser
 * @param buf String representation of the bucket
 * @param sz Position to split the bucket and insert the new content
 * @return Bucket to continue searching (at the marker)
 */
static apr_bucket *csrf_inject_head(request_rec *r, apr_bucket_brigade *bb, apr_bucket *b,
                                    csrf_req_ctx *rctx,
                                    const char *buf, apr_size_t sz) {
  apr_bucket *e;
  apr_bucket_split(b, sz);
  b = APR_BUCKET_NEXT(b);
  e = apr_bucket_pool_create(rctx->script, strlen(rctx->script), r->pool, bb->bucket_alloc);

  APR_BUCKET_INSERT_BEFORE(b, e);
  rctx->state = CSRF_RES_SEARCH_BODY;
  rctx->search = apr_pstrdup(r->pool, "</body>");
  return b;
}

/**
 * Injects a new bucket containing a java script method call incl. the id.
 *
 * @param r
 * @param bb
 * @param b Bucket to split and insert date new bucket at the postion of the marker
 * @param rctx Request context containing the state of the parser
 * @param buf String representation of the bucket
 * @param sz Position to split the bucket and insert the new content
 * @return Bucket to continue searching (at the marker)
 */
static apr_bucket *csrf_inject_body(request_rec *r, apr_bucket_brigade *bb, apr_bucket *b,
                                    csrf_req_ctx *rctx,
                                    const char *buf, apr_size_t sz) {
  apr_bucket *e;
  apr_bucket_split(b, sz);
  b = APR_BUCKET_NEXT(b);
  e = apr_bucket_pool_create(rctx->method, strlen(rctx->method), r->pool, bb->bucket_alloc);

  APR_BUCKET_INSERT_BEFORE(b, e);
  rctx->state = CSRF_RES_SEARCH_END;
  rctx->search = NULL;
  return b;
}

/**
 * Get or create (and init) the pre request context used by the response parser.
 *
 * @param r
 * @return
 */
static csrf_req_ctx *csrf_get_rctx(request_rec *r) {
  csrf_req_ctx *rctx = ap_get_module_config(r->request_config, &csrf_module);
  if(rctx == NULL) {
    csrf_srv_config_t *sconf = ap_get_module_config(r->server->module_config, &csrf_module);
    csrf_dir_config_t *dconf = ap_get_module_config(r->per_dir_config, &csrf_module);
    char time_string[32];
    time_t tm = time(NULL);
    struct tm *ptr = localtime(&tm);

    rctx = apr_pcalloc(r->pool, sizeof(csrf_req_ctx));
    rctx->state = CSRF_RES_NEW;
    rctx->search = NULL;
    rctx->body_window = apr_pcalloc(r->pool, 2*CSRF_WIN+1);
    rctx->body_window[0] = '\0';

    strftime(time_string, sizeof(time_string), "%m%d", ptr); // prevent browser caching

    // TODO: better to inject the id into the js file than the html doc (better protection from
    //       being fetched by a script), or do both (two parts)???
    rctx->method = apr_psprintf(r->pool, "<script type=\"text/javascript\">\n"
                                "<!--\ncsrfInsert(\"%s\", \"%s\");\n"
                                "//-->\n"
                                "</script>\n",
                                sconf->id,
                                csrf_create_id(r));
    rctx->script = apr_psprintf(r->pool, "<script language=\"JavaScript\""
                               " src=\"%s?i=%s\" type=\"text/javascript\">"
                                "</script>\n",
                                dconf->path2script ? dconf->path2script : sconf->path2script,
                                time_string);
    rctx->pool = NULL;
    ap_set_module_config(r->request_config, &csrf_module, rctx);
  }
  return rctx;
}

/************************************************************************
 * handlers
 ***********************************************************************/

static apr_status_t csrf_out_filter(ap_filter_t *f, apr_bucket_brigade *bb) {
  request_rec *r = f->r;
  /*
   * adds the id to redirects (30x, location header) if CSRF_REDIRECT
   * variable has been set
   * e.g. "ResponseSetEnvIfPlus Location <our hostname> CSRF_REDIRECT"
   */
  if(apr_table_get(r->subprocess_env, CSRF_REDIRECT)) {
    csrf_srv_config_t *sconf = ap_get_module_config(r->server->module_config,
                                                    &csrf_module);
    int err = 0;
    const char *location = apr_table_get(r->headers_out, "Location");
    if(!location) {
      err = 1;
      location = apr_table_get(r->err_headers_out, "Location");
    }
    if(location) { // is redirect
      if(strchr(location, '?')) { // has query
        char *name = apr_pstrcat(r->pool, sconf->id, "=", NULL);
        if(strstr(location, name) == NULL) { // add only once
          location = apr_pstrcat(r->pool, location, "&", 
                                 name, csrf_create_id(r), NULL);
          if(err) {
            apr_table_set(r->err_headers_out, "Location", location);
          } else {
            apr_table_set(r->headers_out, "Location", location);
          }
        }
      }
    }
  }
  // done, header only
  ap_remove_output_filter(f);
  return ap_pass_brigade(f->next, bb);
}

/**
 * Out filter to inject our java script to every html page
 */
static apr_status_t csrf_out_filter_body(ap_filter_t *f, apr_bucket_brigade *bb) {
  request_rec *r = f->r;
  csrf_req_ctx *rctx = csrf_get_rctx(r);

  /*
   * states:
   * - new (determine if it's html and force chunked response)
   * - search </head> to insert link to our js
   * - search </body> to insert script method/id
   * - end (all done)
   */
  if(rctx->state == CSRF_RES_NEW) {
    const char *type = csrf_get_contenttype(r);
    if(type == NULL || strncasecmp(type, "text/html", 9) != 0) {
      // we don't want to parse this response (no html)
      rctx->state = CSRF_RES_SEARCH_END;
      rctx->search = NULL;
      ap_remove_output_filter(f);
    } else {
      // start searching head/body to inject our script
      if(CSRF_ISDEBUG(r->server)) {
        ap_log_rerror(APLOG_MARK, APLOG_NOERRNO|APLOG_DEBUG, 0, r,
                      CSRF_LOGD_PFX"enable JavaScript injection filter, id=%s",
                      csrf_get_uniqueid(r));
      }
      if(CSRF_CHUNKED_ONLY) {
        // send as chunked response
        apr_table_unset(r->headers_out, "Content-Length");
        apr_table_unset(r->err_headers_out, "Content-Length");
        r->chunked = 1;
      } else {
        // adjust the content-length header
        // TODO: append dummy bytes if we can't inject the data
        int errh = 0;
        const char* cl =  apr_table_get(r->headers_out, "Content-Length");
        if(!cl) {
          errh = 1;
          cl =  apr_table_get(r->err_headers_out, "Content-Length");
        }
        if(cl) {
          // adjust non-chunked response
          char *length;
          apr_off_t s;
          char *errp = NULL;
          if(apr_strtoff(&s, cl, &errp, 10) == APR_SUCCESS) {
            s = s + strlen(rctx->script) + strlen(rctx->method);
            length = apr_psprintf(r->pool, "%"APR_OFF_T_FMT, s);
            if(!errh) {
              apr_table_set(r->headers_out, "Content-Length", length);
            } else {
              apr_table_set(r->err_headers_out, "Content-Length", length);
            }
          } else {
            // fallback to chunked
            r->chunked = 1;
            if(!errh) {
              apr_table_unset(r->headers_out, "Content-Length");
            } else {
              apr_table_unset(r->err_headers_out, "Content-Length");
            }
          }
        }
      }
      apr_table_add(r->headers_out, "Cache-Control", "no-cache, no-store");
      apr_table_unset(r->headers_out, "Etag");
      rctx->state = CSRF_RES_SEARCH_HEAD;
      rctx->search = apr_pstrdup(r->pool, "</head>");
    }
  }

  // start searching within this brigade...
  if(rctx->search) {
    apr_bucket *b;
    int loop = 0;
    /* pool to allocate buckets from (used to insert buffer from previous loop)
       this pool survices this filter call in we destroy it when we are called
       the next time because we expect that the bucket has been send to the network */
    apr_pool_t *pool;
    apr_pool_create(&pool, r->pool);
    for(b = APR_BRIGADE_FIRST(bb); b != APR_BRIGADE_SENTINEL(bb); b = APR_BUCKET_NEXT(b)) {
      if(APR_BUCKET_IS_EOS(b)) {
        /* ups - we did not find the HTML markers... write the remaining window buffer */
        if(CSRF_ENABLE_WINDOW && rctx->body_window[0]) {
          int wlen = strlen(rctx->body_window);
          apr_bucket *last = apr_bucket_pool_create(apr_pstrndup(pool, rctx->body_window, wlen), wlen,
                                                    pool, bb->bucket_alloc);
          APR_BUCKET_INSERT_BEFORE(b, last);
          rctx->body_window[0] = '\0';
          ap_log_rerror(APLOG_MARK, APLOG_NOERRNO|APLOG_DEBUG, 0, r,
                        CSRF_LOGD_PFX"got EOS without finding all HTML markers,"
                        " %d, id=%s",
                        rctx->state,
                        csrf_get_uniqueid(r));
        }
        /* If we ever see an EOS, make sure to FLUSH. */
        apr_bucket *flush = apr_bucket_flush_create(f->c->bucket_alloc);
        APR_BUCKET_INSERT_BEFORE(b, flush);
      }
      if(!(APR_BUCKET_IS_METADATA(b))) {
        const char *buf = NULL;
        apr_size_t  nbytes = 0;

        if(apr_bucket_read(b, &buf, &nbytes, APR_BLOCK_READ) == APR_SUCCESS) {
          if(nbytes > 0) {
            const char *marker = NULL;

            /*
             * 1. overlap with existing buffer
             */
            if(CSRF_ENABLE_WINDOW && rctx->body_window[0]) {
              int blen = nbytes > CSRF_WIN ? CSRF_WIN : nbytes-1;
              int wlen = strlen(rctx->body_window); // lenght of the previous buffer
              apr_bucket *last;

              // add some bytes of the new bucket to the previous
              strncpy(&rctx->body_window[wlen], buf, blen);
              rctx->body_window[wlen+blen+1] = '\0';

              // add the previous data to the brigade (always - does not matter if we find a match)
              // note: we need the temp. per loop pool to allocate the bucket from
              last = apr_bucket_pool_create(apr_pstrndup(pool, rctx->body_window, wlen), wlen,
                                            pool, bb->bucket_alloc);
              if(loop == 0) {
                // first bucket in the brigade (insert before does not work, see apr_ring.h)
                APR_BRIGADE_INSERT_HEAD(bb, last);
              } else {
                APR_BUCKET_INSERT_BEFORE(b, last);
              }

              // search within the window
              marker = ap_strcasestr(rctx->body_window, rctx->search);
              if(marker) {
                // found pattern
                apr_size_t sz = marker - rctx->body_window;
                if(sz < wlen) {
                  // within the previously stored data
                  // example: [..</he][ad>...]
                  //             ^
                  if(rctx->state == CSRF_RES_SEARCH_HEAD) {
                    csrf_inject_head(r, bb, last, rctx, rctx->body_window, sz);
                  } else if(rctx->state == CSRF_RES_SEARCH_BODY) {
                    csrf_inject_body(r, bb, last, rctx, rctx->body_window, sz);
                  }
                } else {
                  // in window but not within the previously stored data ("2. new buffer" will detect it)
                }
              }
              rctx->body_window[0] = '\0';
            }

            /*
             * 2. new buffer
             */
          restart:
            if(rctx->state != CSRF_RES_SEARCH_END) {
              marker = csrf_strncasestr(buf, rctx->search, nbytes);
              if(marker) {
                // found pattern
                apr_size_t sz = marker - buf;
                if(rctx->state == CSRF_RES_SEARCH_HEAD) {
                  b = csrf_inject_head(r, bb, b, rctx, buf, sz);
                  // TODO: re-calcluate e buffer pointer and size instead of re-reading
                  if(apr_bucket_read(b, &buf, &nbytes, APR_BLOCK_READ) == APR_SUCCESS) {
                    goto restart;
                  } else {
                    // TODO: error handling?
                    break;
                  }
                } else if(rctx->state == CSRF_RES_SEARCH_BODY) {
                  b = csrf_inject_body(r, bb, b, rctx, buf, sz);
                  break;
                }
              }
            }

            /*
             * 3. end parsing if we are done
             */
            if(rctx->state == CSRF_RES_SEARCH_END) {
              break;
            }

            /*
             * 4. store the end (for next loop)
             */
            if(CSRF_ENABLE_WINDOW && rctx->state != CSRF_RES_SEARCH_END) {
              apr_bucket *rb;
              int blen = nbytes > CSRF_WIN ? CSRF_WIN : nbytes-1;
              strncpy(rctx->body_window, &buf[nbytes - blen], blen);
              rctx->body_window[blen] = '\0';
              apr_bucket_split(b, nbytes - blen);
              rb = APR_BUCKET_NEXT(b);
              APR_BUCKET_REMOVE(rb);
            }
          }
        }
      }
      loop++;
    }
    if(rctx->pool) {
      // this data is no longer needed
      apr_pool_destroy(rctx->pool);
    }
    rctx->pool = pool; // store pool (until the buckets are sent)
  }
  return ap_pass_brigade(f->next, bb);
}

/**
 * Request verification.
 *
 * @param r
 * @return
 */
static int csrf_fixup(request_rec * r) {
  if(ap_is_initial_req(r) && csrf_enabled(r)) {
    ap_add_output_filter("csrf_out_filter_body", NULL, r, r->connection);
    if(!csrf_ignore_req(r)) {
      csrf_srv_config_t *sconf = ap_get_module_config(r->server->module_config, &csrf_module);
      apr_table_t *tl = NULL;
      char *msg = NULL;
      const char *idheader = apr_table_get(r->headers_in, sconf->id);
      if(csrf_parp_hp_table_fn) {
        tl = csrf_parp_hp_table_fn(r);
      }
      if(tl == NULL) {
        // parp was not active/loaded, we read the request query ourself
        tl = csrf_get_query(r);
      }

      // ignores GET requests without and form/query parameter
      if((tl == NULL) && 
         (r->method_number == M_GET) && 
         (apr_table_get(r->subprocess_env, CSRF_ENFORCE) == NULL)) {
        /* no request query/body 
         * => nothing to do here since we don't validate "simple" 
         *    requests without any parameters */
        return DECLINED;
      }
      if(!csrf_validate_req_id(r, tl, idheader, &msg)) {
        ap_log_rerror(APLOG_MARK, APLOG_NOERRNO|APLOG_ERR, 0, r,
                      CSRF_LOG_PFX(020)"request denied, %s, action=%s, id=%s", msg ? msg : "-",
                      sconf->action == CSRF_ACTION_LOG ? "log" : "deny",
                      csrf_get_uniqueid(r));
        if(sconf->action != CSRF_ACTION_LOG) {
          apr_table_set(r->err_headers_out, "WWW-Authenticate", "Basic realm=\"ReadyNAS Admin\"");
          return HTTP_FORBIDDEN;
        }
      }
      if(!csrf_referer_check(r, &msg)) {
        ap_log_rerror(APLOG_MARK, APLOG_NOERRNO|APLOG_ERR, 0, r,
                      CSRF_LOG_PFX(021)"request denied, %s, action=%s, id=%s", msg ? msg : "-",
                      sconf->action == CSRF_ACTION_LOG ? "log" : "deny",
                      csrf_get_uniqueid(r));
        if(sconf->action != CSRF_ACTION_LOG) {
          apr_table_set(r->err_headers_out, "WWW-Authenticate", "Basic realm=\"ReadyNAS Admin\"");
          return HTTP_FORBIDDEN;
        }
      }
    }
  }
  return DECLINED;
}

/**
 * Prepares the request.
 * - Enable parameter parser if required (based on content type).
 *
 * @param r
 * @return
 */
static int csrf_header_parser(request_rec * r) {
  if(ap_is_initial_req(r)) {
    /* enables parameter parser */
    if(csrf_enabled(r) && !csrf_ignore_req(r)) {
      const char *ct = apr_table_get(r->headers_in, "Content-Type");
      if(ct) {
        // enable parp for certain content types (parser available)
        if(ap_strcasestr(ct, "application/x-www-form-urlencoded") ||
           ap_strcasestr(ct, "multipart/form-data") ||
           ap_strcasestr(ct, "multipart/mixed") ||
           ap_strcasestr(ct, "application/json")) {
          apr_table_add(r->subprocess_env, "parp", "mod_csrf");
        }
      } else if(r->args) {
        // no content type (no body), process query only
        apr_table_add(r->subprocess_env, "parp", "mod_csrf");
      }
    }
    // tells mod_parp (>= 0.12) to remove our request id
    apr_table_add(r->notes, "PARP_DELETE_PARAM", CSRF_QUERYID);
  }
  return DECLINED;
}

/** finalize configuration */
static int csrf_post_config(apr_pool_t *pconf, apr_pool_t *plog,
                            apr_pool_t *ptemp, server_rec *bs) {
  ap_add_version_component(pconf, apr_psprintf(pconf, "mod_csrf/%s", g_revision));
  if(ap_find_linked_module("mod_parp.c") == NULL) {
    ap_log_error(APLOG_MARK, APLOG_WARNING, 0, bs, 
                 CSRF_LOG_PFX(001)"mod_parp not available");
    csrf_parp_hp_table_fn = NULL;
  } else {
    csrf_parp_hp_table_fn = APR_RETRIEVE_OPTIONAL_FN(parp_hp_table);
  }
  return DECLINED;
}

static void csrf_insert_filter(request_rec *r) {
  ap_add_output_filter("csrf_out_filter", NULL, r, r->connection);
}

/************************************************************************
 * directiv handlers 
 ***********************************************************************/
static void *csrf_dir_config_create(apr_pool_t *p, char *d) {
  csrf_dir_config_t *dconf = apr_pcalloc(p, sizeof(csrf_dir_config_t));
  dconf->enabled = -1;
  dconf->path2script = NULL; // use server config by default
  dconf->referer_check = -1;
  return dconf;
}

static void *csrf_dir_config_merge(apr_pool_t *p, void *basev, void *addv) {
  csrf_dir_config_t *b = (csrf_dir_config_t *)basev;
  csrf_dir_config_t *o = (csrf_dir_config_t *)addv;
  csrf_dir_config_t *m = apr_pcalloc(p, sizeof(csrf_dir_config_t));
  if(o->enabled != -1) {
    m->enabled = o->enabled;
  } else {
    m->enabled = b->enabled;
  }
  if(o->flags & CSRF_FUNC_FLAGS_SCRIPT) {
    m->path2script = o->path2script;
    m->flags |= CSRF_FUNC_FLAGS_SCRIPT;
  } else {
    m->path2script = b->path2script;
  }
  if(o->referer_check != -1) {
    m->referer_check = o->referer_check;
  } else {
    m->referer_check = b->referer_check;
  }
  return m;
}

static void *csrf_srv_config_create(apr_pool_t *p, server_rec *s) {
  csrf_srv_config_t *sconf = apr_pcalloc(p, sizeof(csrf_srv_config_t));
  sconf->ignore_pattern = ap_pregcomp(p, CSRF_IGNORE_PATTERN, AP_REG_ICASE);
  sconf->id = apr_pstrdup(p, CSRF_QUERYID);
  sconf->enabled = -1;
  sconf->path2script = apr_pstrdup(p, CSRF_DEFAULT_PATH);
  sconf->timeout = apr_time_from_sec(CSRF_DEFAULT_TIMEOUT);
  sconf->referer_check = -1;
  sconf->action = CSRF_ACTION_DENY_DEFAULT;
  return sconf;
}

static void *csrf_srv_config_merge(apr_pool_t *p, void *basev, void *addv) {
  csrf_srv_config_t *b = (csrf_srv_config_t *)basev;
  csrf_srv_config_t *o = (csrf_srv_config_t *)addv;
  csrf_srv_config_t *m = apr_pcalloc(p, sizeof(csrf_srv_config_t));
  if(o->enabled != -1) {
    m->enabled = o->enabled;
  } else {
    m->enabled = b->enabled;
  }
  m->ignore_pattern = b->ignore_pattern;
  m->id = b->id;
  if(o->flags & CSRF_FUNC_FLAGS_KEY) {
    m->sec_len = o->sec_len;
    m->sec = o->sec;
    memcpy(m->key, o->key, sizeof(o->key));
    m->flags |= CSRF_FUNC_FLAGS_KEY;
  } else {
    m->sec_len = b->sec_len;
    m->sec = b->sec;
    memcpy(m->key, b->key, sizeof(b->key));
  }
  if(o->flags & CSRF_FUNC_FLAGS_SCRIPT) {
    m->path2script = o->path2script;
    m->flags |= CSRF_FUNC_FLAGS_SCRIPT;
  } else {
    m->path2script = b->path2script;
  }
  if(o->flags & CSRF_FUNC_FLAGS_TMO) {
    m->timeout = o->timeout;
    m->flags |= CSRF_FUNC_FLAGS_TMO;
  } else {
    m->timeout = b->timeout;
  }
  if(o->referer_check != -1) {
    m->referer_check = o->referer_check;
  } else {
    m->referer_check = b->referer_check;
  }
  if(o->action != CSRF_ACTION_DENY_DEFAULT) {
    m->action = o->action;
  } else {
    m->action = b->action;
  }
  return m;
}

const char *csrf_enable_cmd(cmd_parms *cmd, void *dcfg, int flag) {
  if(cmd->path) {
    csrf_dir_config_t *conf = dcfg;
    conf->enabled = flag;
  } else {
    csrf_srv_config_t *conf = ap_get_module_config(cmd->server->module_config, &csrf_module);
    conf->enabled = flag;
  }
  return NULL;
}

const char *csrf_enable_referer_cmd(cmd_parms *cmd, void *dcfg, int flag) {
  if(cmd->path) {
    csrf_srv_config_t *conf = dcfg;
    conf->referer_check = flag;
  } else {
    csrf_srv_config_t *conf = ap_get_module_config(cmd->server->module_config, &csrf_module);
    conf->referer_check = flag;
  }
  return NULL;
}

const char *csrf_action_cmd(cmd_parms *cmd, void *dcfg, const char *action) {
  csrf_srv_config_t *sconf = ap_get_module_config(cmd->server->module_config, &csrf_module);
  if(strcasecmp(action, "deny") == 0) {
    sconf->action = CSRF_ACTION_DENY;
  } else if(strcasecmp(action, "log") == 0) {
    sconf->action = CSRF_ACTION_LOG;
  } else {
    return apr_psprintf(cmd->pool, "%s: invalid action (neither 'deny' nor 'log')",
                        cmd->directive->directive);
  }
  return NULL;
}

const char *csrf_path2script_cmd(cmd_parms *cmd, void *dcfg, const char *path) {
  if(cmd->path) {
    csrf_dir_config_t *dconf = dcfg;
    dconf->path2script = apr_pstrdup(cmd->pool, path);
    dconf->flags |= CSRF_FUNC_FLAGS_SCRIPT;
  } else {
    csrf_srv_config_t *sconf = ap_get_module_config(cmd->server->module_config, &csrf_module);
    sconf->path2script = apr_pstrdup(cmd->pool, path);
    sconf->flags |= CSRF_FUNC_FLAGS_SCRIPT;
  }
  return NULL;
}

/**
 * cmd defines the validity persd of the injected id
 */
const char *csrf_tmo_cmd(cmd_parms *cmd, void *dcfg, const char *sec) {
  csrf_srv_config_t *sconf = ap_get_module_config(cmd->server->module_config, &csrf_module);
  sconf->timeout = atoi(sec);
  if(sconf->timeout <= 0) {
    return apr_psprintf(cmd->pool, "%s: requires numeric values greater than 0",
                        cmd->directive->directive);
  }
  sconf->timeout = apr_time_from_sec(sconf->timeout);
  sconf->flags |= CSRF_FUNC_FLAGS_TMO;
  return NULL;
}

/* CSRF_Passphrase */
const char *csrf_pwd_cmd(cmd_parms *cmd, void *dcfg, const char *pwd) {
  csrf_srv_config_t *sconf = ap_get_module_config(cmd->server->module_config, &csrf_module);
  sconf->sec = (unsigned char *)apr_pstrcat(cmd->pool, pwd, "W9sO.4h7-6PU", NULL);
  sconf->sec_len = strlen((char *)sconf->sec);
  EVP_BytesToKey(EVP_des_ede3_cbc(), EVP_sha1(), NULL, sconf->sec,
                 sconf->sec_len, 1, sconf->key, NULL);
  sconf->flags |= CSRF_FUNC_FLAGS_KEY;
  return NULL;
}

static const command_rec csrf_config_cmds[] = {
  // TODO: add directive (and cfg merge) do override ignore pattern sconf->ignore_pattern
  // TODO: directive to override CSRF_QUERYID
  AP_INIT_FLAG("CSRF_Enable", csrf_enable_cmd, NULL,
               RSRC_CONF|ACCESS_CONF,
               "CSRF_Enable 'on'|'off', enables the module. Default is 'on'."),
  AP_INIT_FLAG("CSRF_EnableReferer", csrf_enable_referer_cmd, NULL,
               RSRC_CONF|ACCESS_CONF,
               "CSRF_EnableReferer 'on'|'off', enables the referer header check."
               " Default is 'on'."),
  AP_INIT_TAKE1("CSRF_Action", csrf_action_cmd, NULL,
                RSRC_CONF,
                "CSRF_Action 'deny'|'log', defines the action to take when a request"
                " does violates the configured rules. Default is 'deny'."),
  AP_INIT_TAKE1("CSRF_PassPhrase", csrf_pwd_cmd, NULL,
                RSRC_CONF,
                "CSRF_PassPhrase <string>, used for the encryption of the mod_csrf"
                " request identifier. Default is a non-persistent random passphrase."),
  AP_INIT_TAKE1("CSRF_Timeout", csrf_tmo_cmd, NULL,
                RSRC_CONF,
                "CSRF_Timeout <seconds>, the validity period of the csrf identifier."
                " Default is 3600 seconds."),
  AP_INIT_TAKE1("CSRF_ScriptPath", csrf_path2script_cmd, NULL,
                RSRC_CONF|ACCESS_CONF,
                "CSRF_ScriptPath <path>, URL path to the JavaScript to inject the"
                " mod_csrf request identifier. Default is '"CSRF_DEFAULT_PATH"'."),
  { NULL }
};

/************************************************************************
 * apache register 
 ***********************************************************************/
static void csrf_register_hooks(apr_pool_t * p) {
  static const char *pre[] = { "mod_setenvif.c", "mod_setenvifplus.c", NULL };
  static const char *parp[] = { "mod_parp.c", NULL };
  ap_hook_header_parser(csrf_header_parser, NULL, parp, APR_HOOK_FIRST);
  ap_hook_fixups(csrf_fixup, pre, NULL, APR_HOOK_MIDDLE);
  ap_hook_post_config(csrf_post_config, pre, NULL, APR_HOOK_MIDDLE);
  ap_register_output_filter("csrf_out_filter_body", csrf_out_filter_body, NULL, AP_FTYPE_RESOURCE);
  ap_register_output_filter("csrf_out_filter", csrf_out_filter, NULL, AP_FTYPE_RESOURCE+1);
  ap_hook_insert_filter(csrf_insert_filter, NULL, NULL, APR_HOOK_MIDDLE);
}

/************************************************************************
 * apache module definition 
 ***********************************************************************/
module AP_MODULE_DECLARE_DATA csrf_module ={ 
  STANDARD20_MODULE_STUFF,
  csrf_dir_config_create,                  /**< dir config */
  csrf_dir_config_merge,                   /**< dir merger */
  csrf_srv_config_create,                  /**< server config */
  csrf_srv_config_merge,                   /**< server merger */
  csrf_config_cmds,                        /**< command table */
  csrf_register_hooks,                     /**< hook registery */
};
