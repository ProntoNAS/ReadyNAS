/* -*-mode: c; indent-tabs-mode: nil; c-basic-offset: 2; -*-
 * The line above sets XEmacs indention to offset 2,
 * and does not insert tabs
 */
/*  ____  _____  ____ ____  
 * |H _ \(____ |/ ___)  _ \ 
 * |T|_| / ___ | |   | |_| |
 * |T __/\_____|_|   |  __/ 
 * |P|ParameterParser|_|    
 * http://parp.sourceforge.net
 *
 * Copyright (C) 2008-2014 Christian Liesch / Pascal Buchbinder / Lukas Funk
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements. 
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/************************************************************************
 * Version
 ***********************************************************************/
static const char revision[] = "$Id: mod_parp.c,v 1.46 2016/06/15 15:55:02 lukasfunk Exp $";
static const char g_revision[] = "0.16";

/************************************************************************
 * Includes
 ***********************************************************************/
/* apache */
#include <httpd.h>
#include <http_main.h>
#include <http_request.h>
#include <http_protocol.h>
#include <http_config.h>
#include <http_log.h>

/* apr */
#include <apr_hooks.h>
#include <apr_strings.h>
#include <apr_buckets.h>
#include <apr_hash.h>

/* this */
#include "mod_parp.h"

/************************************************************************
 * defines
 ***********************************************************************/
#define PARP_LOG_PFX(id)  "mod_parp("#id"): "
#define PARP_DELETE_PARAM "PARP_DELETE_PARAM"

#define PARP_FLAGS_NONE 0
#define PARP_FLAGS_CONT_ON_ERR 1
#define PARP_FLAGS_FIRST_PARAM_WRITTEN 2

#define PARP_ERR_BRIGADE_FULL (APR_OS_START_USERERR + 1)

/************************************************************************
 * structures
 ***********************************************************************/
typedef enum {
  NONE, FORMDATA, MULTIPART
} body_content_t;

typedef enum {
  QUERY, BODY
} parameter_t;
/**
 * parp hook
 */
typedef struct parp_s{
  apr_pool_t *pool;
  request_rec *r;
  apr_bucket_brigade *bb;
  char *raw_body_data; /** raw data received from the client */
  apr_size_t raw_body_data_len; /** total length of the raw data (excluding modifications) */
  int use_raw_body; /** indicates the input filter to read the raw data instead of the bb  (body content has changed)*/
  apr_table_t *params; /** readonly parameter table (query+body) */
  apr_array_header_t *rw_params; /** writable table of parp_entry_t entries (null if
   no body or query available or no module has registered) */
  apr_array_header_t *rw_params_query_structure;
  apr_array_header_t *rw_params_body_structure;

  body_content_t content_typeclass;
  apr_table_t *parsers; /** body parser per content type */

  char *error;
  int flags;
  int recursion;

  char *data_query;
  apr_size_t len_query;
  char *data_body;
  apr_size_t len_body;

  char *tmp_buffer;                  /* partial written data */
  apr_off_t len_tmp_buffer;

} parp_t;

typedef struct parp_query_structure_s{
  int rw_array_index;
  const char *key;
  const char *key_addr;
  const char *value_addr;
} parp_query_structure_t;

typedef struct parp_body_structure_s{
  int rw_array_index;
  const char *key; // name attribute in content-disposition
  const char *key_addr;
  const char *value_addr; // pointer to the value in the mulitpart body

  const char *multipart_addr; // pointer to the multipart with the start boundary delimiter
  int mulitpart_nested_header_len; // header length of nested multipart entries
  int raw_len; // mulitpart part length incl start boundary.
  int raw_len_modified; // multipart length
  const char *multipart_boundary; // boundary with starting --
  apr_array_header_t *multipart_parameters;
  int multipart_parameters_ndelete; /* the number of parameters to delete in
                                       the nested multipart. can be compared to
                                       multipart_parameters->nelts so see if the
                                       whole multipart must be deleted.*/

  int written_to_brigade;
} parp_body_structure_t;

/**
 * server configuration
 */
typedef struct parp_srv_config_s{
  int onerror;
  apr_table_t *parsers;
} parp_srv_config;

/**
 * block
 */
typedef struct parp_block_s{
  apr_size_t len;
  char *data;
  char *raw_data;
  apr_size_t raw_data_len;
} parp_block_t;

/************************************************************************
 * globals
 ***********************************************************************/
module AP_MODULE_DECLARE_DATA parp_module;

APR_IMPLEMENT_OPTIONAL_HOOK_RUN_ALL(parp, PARP, apr_status_t, hp_hook,
    (request_rec *r, apr_table_t *table),
    (r, table),
    OK, DECLINED)

/**
 * DEPRECATED - only for backwards compatibility - use modify_hook
 */
APR_IMPLEMENT_OPTIONAL_HOOK_RUN_ALL(parp, PARP, apr_status_t, modify_body_hook,
    (request_rec *r, apr_array_header_t *array),
    (r, array),
    OK, DECLINED)

APR_IMPLEMENT_OPTIONAL_HOOK_RUN_ALL(parp, PARP, apr_status_t, modify_hook,
    (request_rec *r, apr_array_header_t *array),
    (r, array),
    OK, DECLINED)
/************************************************************************
 * functions
 ***********************************************************************/

typedef apr_status_t (*parp_parser_f)(parp_t *, parameter_t, apr_table_t *,
    char *, apr_size_t, apr_array_header_t *);

static parp_parser_f parp_get_parser(parp_t *self, const char *ct);

/**
 * Verifies if we may expext any body request data.
 */
static int parp_has_body(parp_t *self) {
  request_rec *r = self->r;
  const char *tenc = apr_table_get(r->headers_in, "Transfer-Encoding");
  const char *lenp = apr_table_get(r->headers_in, "Content-Length");
  if (tenc) {
    if (strcasecmp(tenc, "chunked") == 0) {
      return 1;
    }
  }
  if (lenp) {
    char *endstr;
    apr_off_t remaining;
    if ((apr_strtoff(&remaining, lenp, &endstr, 10) == APR_SUCCESS)
        && (remaining > 0)) {
      return 1;
    }
  }
  return 0;
}

/**
 * apr_brigade_pflatten() to null terminated string
 */
apr_status_t parp_flatten(apr_bucket_brigade *bb, char **c, apr_size_t *len,
    apr_pool_t *pool) {
  apr_off_t actual;
  apr_size_t total;
  apr_status_t rv;

  apr_brigade_length(bb, 1, &actual);
  total = (apr_size_t) actual;
  *c = apr_palloc(pool, total + 1);
  rv = apr_brigade_flatten(bb, *c, &total);
  *len = total;
  if (rv != APR_SUCCESS) {
    return rv;
  }
  (*c)[total] = '\0';
  return APR_SUCCESS;
}

/**
 * Read payload of this request (null terminated)
 *
 * @param r IN request record
 * @param data OUT flatten payload
 * @param len OUT len of payload
 *
 * @return APR_SUCCESS, any apr status code on error
 */
static apr_status_t parp_get_payload(parp_t *self) {
  char *data;
  apr_size_t len;
  apr_status_t status;

  request_rec *r = self->r;

  if ((status = parp_read_payload(r, self->bb, &self->error)) != APR_SUCCESS) {
    return status;
  }

  if ((status = parp_flatten(self->bb, &data, &len, self->pool)) != APR_SUCCESS) {
    self->error = apr_pstrdup(r->pool,
        "Input filter: apr_brigade_pflatten failed");
  }
  else {
    self->raw_body_data = data;
    self->raw_body_data_len = len;
  }
  return status;
}

/**
 * read the content type contents
 *
 * @param self IN instance
 * @param headers IN headers
 * @param result OUT
 *
 * @return APR_SUCCESS or APR_EINVAL
 */
static apr_status_t parp_read_header(parp_t *self, const char *header,
    apr_table_t **result) {
  char *pair;
  char *key;
  char *val;
  char *last;
  apr_size_t len;

  apr_table_t *tl = apr_table_make(self->pool, 3);

  *result = tl;

  /* iterate over multipart key/value pairs */
  pair = apr_strtok(apr_pstrdup(self->pool, header), ";,", &last);
  if (!pair) {
    return APR_SUCCESS;
  }
  do {
    /* eat spaces */
    while (*pair == ' ') {
      ++pair;
    }
    /* get key/value */
    key = apr_strtok(pair, "=", &val);
    if (key) {
      /* strip " away */
      if (val && val[0] == '"') {
        ++val;
        len = strlen(val);
        if (len > 0) {
          if (self->rw_params) {
            /* don't modify the raw data since we still need them */
            val = apr_pstrndup(self->pool, val, len - 1);
          }
          else {
            val[len - 1] = 0;
          }
        }
      }
      apr_table_addn(tl, key, val);
    }
  }
  while ((pair = apr_strtok(NULL, ";,", &last)));

  return APR_SUCCESS;
}

/**
 * read the all boundaries 
 *
 * @param self IN instance
 * @param data IN data to parse
 * @param len IN len of data
 * @param tag IN boundary tag
 * @param result OUT table of boundaries
 *
 * @return APR_SUCCESS or APR_EINVAL
 */
static apr_status_t parp_read_boundaries(parp_t *self, char *data,
    apr_size_t len, const char *tag, apr_table_t **result) {

  apr_size_t i;
  apr_size_t start;
  apr_size_t match;
  apr_size_t tag_len;
  apr_size_t boundary_start;
  apr_size_t preamble;
  int incr;
  apr_table_t *tl;
  parp_block_t *boundary;

  tl = apr_table_make(self->pool, 5);
  *result = tl;
  tag_len = strlen(tag);
  for (i = 0, match = 0, start = 0, boundary_start = 0, preamble = 1; i < len; i++) {
    /* test if match complete */
    if (match == tag_len) {
      preamble = 0;
      if (strncmp(&data[i], "\r\n", 2) == 0) {
        incr = 2;
      }
      else if (strncmp(&data[i], "--\r\n", 4) == 0) {
        incr = 4;
      }
      else if (strcmp(&data[i], "--") == 0) {
        incr = 2;
      }
      else if (data[i] == '\n') {
        incr = 1;
      }
      else {
        match = 0;
        continue;
      }
      /* prepare data finalize string with 0 */
      //if(self->rw_body_params == NULL) {
      /* don't modify the raw data since we still need them */
      //data[i - match] = 0;
      //}

      /* got it, store it (if>0) */
      if (data[start] && ((i - match) - start)) {
        boundary = apr_pcalloc(self->pool, sizeof(*boundary));
        boundary->len = (i - match) - start;
        //if(self->rw_body_params) {
        /* don't modify the raw data since we still need them */
        //boundary->data = apr_pstrndup(self->pool, &data[start], boundary->len);
        //} else {
        boundary->data = &data[start];
        boundary->raw_data = &data[boundary_start];
        boundary->raw_data_len = (i - match) - boundary_start;
        //}
        apr_table_addn(tl, tag, (char *) boundary);
        //        char* data_cut = apr_pstrmemdup(self->pool, boundary->data, boundary->len);
        //        printf("***\n%s\n***\n", data_cut);
        //        char* multipart = apr_pstrmemdup(self->pool, boundary->raw_data, boundary->raw_data_len);
        //        printf("+++\n%s\n+++\n", multipart);

        boundary_start = (i - match);
      }
      i += incr;
      if (boundary_start <= start) {
        boundary_start = start;
      }
      start = i;
    }
    /* pattern matching */
    if (match < tag_len && data[i] == tag[match]) {
      ++match;
    }
    else {
      match = 0;
      if (preamble == 1) {
        start = boundary_start = i+1;
      }
    }
  }

  return APR_SUCCESS;
}

static char *parp_strtok(apr_pool_t *pool, char *str, const char *sep,
    char **last) {
  char *token;

  if (!str) /* subsequent call */
    str = *last; /* start where we left off */

  /* skip characters in sep (will terminate at '\0') */
  while (*str && strchr(sep, *str))
    ++str;

  if (!*str) /* no more tokens */
    return NULL;

  token = str;

  /* skip valid token characters to terminate token and
   * prepare for the next call (will terminate at '\0)
   */
  *last = token + 1;
  while (**last && !strchr(sep, **last))
    ++*last;
  token = apr_pstrndup(pool, token, *last - token);
  if (**last) {
    ++*last;
  }

  return token;
}

/**
 * Get headers from data, all lines until first empty line will be 
 * split into header/value stored in the headers table.
 *
 * @param self IN instance
 * @param data IN data
 * @param len IN len of data
 * @param headers OUT found headers
 *
 * @return APR_SUCCESS or APR_EINVAL
 */
static apr_status_t parp_get_headers(parp_t *self, parp_block_t *b,
    apr_table_t **headers) {
  char *last = NULL;
  char *header = NULL;
  char *key = NULL;
  char *val = NULL;
  char *data = b->data;

  apr_table_t *tl = apr_table_make(self->pool, 3);
  *headers = tl;
  header = parp_strtok(self->pool, data, "\r\n", &last);
  while (header) {
    key = apr_strtok(header, ":", &val);
    if (val) {
      while (*val == ' ')
        ++val;
    }
    apr_table_addn(tl, key, val);

    if (last && (*last == '\n')) {
      ++last;
    }
    /* look if we have a empty line in front (header/body separator)*/
    if (strncmp(last, "\r\n", 2) == 0) {
      ++last;
      break;
    }
    header = parp_strtok(self->pool, NULL, "\r\n", &last);
  }
  if (last && (*last == '\n')) {
    ++last;
    b->len -= last - data;
    b->data = last;
  }
  else {
    b->len = 0;
    b->data = NULL;
  }

  return APR_SUCCESS;
}

/**
 * Urlencode parser
 *
 * @param self IN instance
 * @param headers IN headers with additional data 
 * @param data IN data with urlencoded content
 * @param len IN len of data
 *
 * @return APR_SUCCESS or APR_EINVAL on parser error
 *
 * @note: Get parp_get_error for more detailed report
 */
static apr_status_t parp_parser_urlencode(parp_t *self,
    parameter_t parameter_type, apr_table_t *headers, const char *data,
    apr_size_t len, apr_array_header_t *structure_array) {
  char *key;
  char *val;
  char *pair;
  const char *rest = data;

  if (parameter_type == BODY && self->content_typeclass == NONE) {
    self->content_typeclass = FORMDATA;
  }
  while (rest[0]) {
    const char *here = rest;
    pair = ap_getword(self->pool, &rest, '&');
    /* get key/value */
    val = pair;
    key = ap_getword_nc(self->pool, &val, '=');
    if (key && (key[0] >= ' ')) {
      /* store it to a table */
      int val_len = strlen(val);
      if (val_len >= 2 && strncmp(&val[val_len - 2], "\r\n", 2) == 0) {
        if (self->rw_params) { // TODO why???
          val[val_len - 2] = 0;
        }
      }
      else if (val_len >= 1 && val[val_len - 1] == '\n') {
        val[val_len - 1] = 0;
      }

      apr_table_addn(self->params, key, val);

      /* store rw ref */
      if (self->rw_params) {
        parp_entry_t *entry = apr_array_push(self->rw_params);
        entry->key = key;
        entry->value = val;
        entry->new_value = NULL;
        entry->delete = 0;

        if (structure_array) {
          if (parameter_type == QUERY) {
            parp_query_structure_t *structure = apr_array_push(structure_array);
            structure->key = key;
            structure->key_addr = &here[0];
            structure->value_addr = &here[strlen(key) + 1];
            structure->rw_array_index = self->rw_params->nelts - 1;
          }
          else {
            parp_body_structure_t *structure = apr_array_push(structure_array);
            structure->key = key;
            structure->key_addr = &here[0];
            structure->value_addr = &here[strlen(key) + 1];
            structure->rw_array_index = self->rw_params->nelts - 1;
            structure->multipart_parameters = NULL;
            structure->multipart_addr = NULL;
            structure->raw_len = strlen(key) + 1 + strlen(val);
            structure->raw_len_modified = structure->raw_len;
            structure->multipart_parameters_ndelete = 0;
            structure->written_to_brigade = 0;
          }
        }
      }
    }
  }

  return APR_SUCCESS;
}

/**
 * Multipart parser
 *
 * @param self IN instance
 * @param headers IN headers with additional data 
 * @param data IN data
 * @param len IN len of data
 *
 * @return APR_SUCCESS or APR_EINVAL on parser error
 *
 * @note: Get parp_get_error for more detailed report
 */
static apr_status_t parp_parser_multipart(parp_t *self,
    parameter_t parameter_type, apr_table_t *headers, char *data,
    apr_size_t len, apr_array_header_t* structure_array) {
  apr_status_t status;
  apr_size_t val_len;
  const char *boundary;
  apr_table_t *ctt;
  apr_table_t *bs;
  apr_table_t *ctds;
  apr_table_entry_t *e;
  int i;
  const char *ctd;
  const char *ct;
  const char *key;
  parp_parser_f parser;
  parp_block_t *b;
  apr_table_t *hs = apr_table_make(self->pool, 3);
  if (self->recursion > 3) {
    self->error = apr_pstrdup(self->pool, "Too deep recursion of multiparts");
    return APR_EINVAL;
  }
  if (self->content_typeclass == NONE) {
    self->content_typeclass = MULTIPART;
  }

  ++self->recursion;

  ct = apr_table_get(headers, "Content-Type");
  if (ct == NULL) {
    self->error = apr_pstrdup(self->pool, "No content type available");
    return APR_EINVAL;
  }

  if ((status = parp_read_header(self, ct, &ctt)) != APR_SUCCESS) {
    return status;
  }

  if (!(boundary = apr_table_get(ctt, "boundary"))) {
    return APR_EINVAL;
  }

  /* prefix boundary wiht a -- */
  boundary = apr_pstrcat(self->pool, "--", boundary, NULL);

  parp_body_structure_t* body_structure = NULL;
  parp_body_structure_t* body_block_structure = NULL;
  if (structure_array != NULL && self->recursion == 1) {
    body_structure = apr_array_push(structure_array);
    body_structure->rw_array_index = -1;
    body_structure->multipart_addr = data;
    body_structure->mulitpart_nested_header_len = 0;
    body_structure->raw_len = len;
    body_structure->raw_len_modified = len;
    body_structure->multipart_parameters = apr_array_make(self->pool, 50,
        sizeof(parp_body_structure_t));
    body_structure->multipart_parameters_ndelete = 0;
    body_structure->multipart_boundary = apr_pstrndup(self->pool, boundary,
        strlen(boundary));
    body_structure->written_to_brigade = 0;
  }

  if ((status = parp_read_boundaries(self, data, len, boundary, &bs))
      != APR_SUCCESS) {
    self->error = apr_pstrdup(self->pool, "failed to read boundaries");
    return status;
  }

  // get boundaries elements
  e = (apr_table_entry_t *) apr_table_elts(bs)->elts;

  // remove pre- and postamble from the multipart structure if exists...
  if (body_structure != NULL && apr_table_elts(bs)->nelts > 0) {
    // first boundary
    b = (parp_block_t *) e[0].val;
    body_structure->multipart_addr = b->raw_data;
    //last boundary
    b = (parp_block_t *) e[apr_table_elts(bs)->nelts - 1].val;
    char *multipart_end = b->raw_data;
    multipart_end += b->raw_data_len;
    int data_len_remaining = len - (multipart_end - data);
    int match;
    int boundary_len = strlen(boundary);
    for (i = 0, match = 0; i < data_len_remaining; ++i) {

      if (match == boundary_len) {
        if (strncmp(&multipart_end[i], "--\r\n", 4) == 0) {
          i += 4;
        }
        else if (strcmp(&multipart_end[i], "--") == 0) {
          i += 2;
        }
        else {
          // should not happen as it need to be the last boundary
          break;
        }
        multipart_end += i;
        body_structure->raw_len = multipart_end - body_structure->multipart_addr;
        body_structure->raw_len_modified = body_structure->raw_len;
        break;
      }
      /* pattern matching */
      if (match < boundary_len && multipart_end[i] == boundary[match]) {
        ++match;
      }
      else {
        match = 0;
      }
    }
  }


  /* iterate over boundaries and store their param/value pairs */
  for (i = 0; i < apr_table_elts(bs)->nelts; ++i) {
    /* read boundary headers */
    b = (parp_block_t *) e[i].val;

    if (body_structure != NULL) {
      body_block_structure = apr_array_push(
          body_structure->multipart_parameters);
      body_block_structure->multipart_addr = b->raw_data;
      body_block_structure->raw_len = b->raw_data_len;
      body_block_structure->raw_len_modified = b->raw_data_len;
      body_block_structure->written_to_brigade = 0;
    } else if (structure_array != NULL) {
      body_block_structure = apr_array_push(
          structure_array);
      body_block_structure->multipart_addr = b->raw_data;
      body_block_structure->raw_len = b->raw_data_len;
      body_block_structure->raw_len_modified = b->raw_data_len;
      body_block_structure->written_to_brigade = 0;
    }

    if ((status = parp_get_headers(self, b, &hs)) != APR_SUCCESS) {
      self->error = apr_pstrdup(self->pool,
          "failed to read headers within boundary");
      return status;
    }

    if ((ct = apr_table_get(hs, "Content-Type")) && apr_strnatcasecmp(ct,
        "text/plain") != 0) {
      parser = parp_get_parser(self, ct);

      if (parser == parp_parser_multipart) {
        if (body_block_structure != NULL) {
          int nested_mulitpart_header_len = b->data - b->raw_data;
          body_block_structure->mulitpart_nested_header_len =
              (nested_mulitpart_header_len > 0 ? nested_mulitpart_header_len : 0);
          body_block_structure->multipart_parameters = apr_array_make(self->pool,
              50, sizeof(parp_body_structure_t));
          body_block_structure->rw_array_index = -1;
          body_block_structure->multipart_parameters_ndelete = 0;
          status = parser(self, parameter_type, hs, b->data, b->len,
              body_block_structure->multipart_parameters);
        } else {
          status = parser(self, parameter_type, hs, b->data, b->len, NULL);
        }
      }
      else {
        status = parser(self, parameter_type, hs, b->data, b->len, NULL);
      }
      if (status != APR_SUCCESS && status != APR_ENOTIMPL) {
        return status;
      }
    }

    if (!(ctd = apr_table_get(hs, "Content-Disposition"))) {
      self->error = apr_pstrdup(self->pool,
          "failed to read content disposition");
      return APR_EINVAL;
    }

    if ((status = parp_read_header(self, ctd, &ctds)) != APR_SUCCESS) {
      return status;
    }

    /* skip all parts with top-level content-type "multipart" */
    char *tlct = apr_pstrndup(self->pool, ct, sizeof "multipart" - 1);
    if (!ct || apr_strnatcasecmp(tlct, "multipart")) {
      char *val = b->data;
      if ((key = apr_table_get(ctds, "name")) == NULL) {
        return APR_EINVAL;
      }
      val_len = b->len;
      /* there must be a \r\n or at least a \n */
      if (val_len >= 2 && strncmp(&val[val_len - 2], "\r\n", 2) == 0) {
        if (self->rw_params) {
          /* don't modify the raw data since we still need them */
          val = apr_pstrndup(self->pool, val, val_len - 2);
        }
        else {
          val[val_len - 2] = 0;
        }
      }
      else if (val_len >= 1 && val[val_len - 1] == '\n') {
        if (self->rw_params) {
          /* don't modify the raw data since we still need them */
          val = apr_pstrndup(self->pool, val, val_len - 1);
        }
        else {
          val[val_len - 1] = 0;
        }
      }
      else {
        return APR_EINVAL;
      }

      apr_table_addn(self->params, key, val);

      if (self->rw_params) {
        parp_entry_t *entry = apr_array_push(self->rw_params);
        entry->key = key;
        entry->value = val;
        entry->new_value = NULL;
        entry->delete = 0;

        if (body_block_structure != NULL) {
          body_block_structure->key = key;
          body_block_structure->key_addr = b->raw_data;
          body_block_structure->value_addr = b->data;
          body_block_structure->rw_array_index = self->rw_params->nelts - 1;
          body_block_structure->written_to_brigade = 0;
        }
      }
    }
  }
  /* now do all boundaries */
  --self->recursion;
  return APR_SUCCESS;
}

/**
 * Not implemented parser used if there is no corresponding parser found
 *
 * @param self IN instance
 * @param headers IN headers with additional data 
 * @param data IN data with urlencoded content
 * @param len IN len of data
 *
 * @return APR_ENOTIMPL
 */
static apr_status_t parp_parser_not_impl(parp_t *self,
    parameter_t parameter_type, apr_table_t *headers, char *data,
    apr_size_t len, apr_array_header_t* structure_array) {
  return APR_ENOTIMPL;
}

/**
 * To get body data from a content type not parsed
 *
 * @param self IN instance
 * @param headers IN headers with additional data 
 * @param data IN data with urlencoded content
 * @param len IN len of data
 *
 * @return APR_SUCCESS
 */
static apr_status_t parp_parser_get_body(parp_t *self,
    parameter_t parameter_type, apr_table_t *headers, char *data,
    apr_size_t len, apr_array_header_t* structure_array) {
  self->data_body = data;
  self->len_body = len;
  return APR_SUCCESS;
}

/**
 * Get content type parser
 *
 * @param self IN instance
 * @param ct IN content type (or NULL)
 *
 * @return content type parser
 */
static parp_parser_f parp_get_parser(parp_t *self, const char *ct) {
  const char *type;
  char *last;

  parp_parser_f parser = NULL;
  parp_srv_config *sconf = ap_get_module_config(self->r->server->module_config,
                                                &parp_module);
  
  if (ct) {
    type = apr_strtok(apr_pstrdup(self->pool, ct), ";,", &last);
    if (type) {
      if (sconf->parsers) {
        parser = (parp_parser_f) apr_table_get(sconf->parsers, type);
      }
      if (!parser) {
        parser = (parp_parser_f) apr_table_get(self->parsers, type);
      }
      if(!parser) {
                                if (sconf->parsers) 
        parser = (parp_parser_f) apr_table_get(sconf->parsers, "*/*");
      }
      if(!parser) {
                                parser = (parp_parser_f) apr_table_get(self->parsers, "*/*");
      }
    }
  }
  if (parser) {
    return parser;
  }
  self->error = apr_psprintf(self->pool,
                             "No parser available for this content type (%s)",
                             ct == NULL ? "-" : ct);
  return parp_parser_not_impl;
}

/**************************************************************************
 * Public
 **************************************************************************/

/**
 * Read payload of this request
 *
 * @param r IN request record
 * @param out IN bucket brigade to fill
 * @param error OUT error text if status != APR_SUCCESS
 *
 * @return APR_SUCCESS, any apr status code on error
 */
AP_DECLARE(apr_status_t ) parp_read_payload(request_rec *r,
    apr_bucket_brigade *out, char **error) {
  apr_status_t status;
  apr_bucket_brigade *bb;
  apr_bucket *b;
  const char *buf;
  apr_size_t len;
  apr_off_t off;
  const char *enc;
  const char *len_str;

  int seen_eos = 0;

  if ((status = ap_setup_client_block(r, REQUEST_CHUNKED_DECHUNK)) != OK) {
    *error = apr_pstrdup(r->pool, "ap_setup_client_block failed");
    return status;
  }

  bb = apr_brigade_create(r->pool, r->connection->bucket_alloc);

  do {
    status = ap_get_brigade(r->input_filters, bb, AP_MODE_READBYTES,
        APR_BLOCK_READ, HUGE_STRING_LEN);

    if (status == APR_SUCCESS) {
      while (!APR_BRIGADE_EMPTY(bb)) {
        b = APR_BRIGADE_FIRST(bb);
        APR_BUCKET_REMOVE(b);

        if (APR_BUCKET_IS_EOS(b)) {
          seen_eos = 1;
          APR_BRIGADE_INSERT_TAIL(out, b);
        }
        else if (APR_BUCKET_IS_FLUSH(b)) {
          APR_BRIGADE_INSERT_TAIL(out, b);
        }
        else {
          status = apr_bucket_read(b, &buf, &len, APR_BLOCK_READ);
          if (status != APR_SUCCESS) {
            *error = apr_pstrdup(r->pool, "Input filter: Failed reading input");
            return status;
          }
          apr_brigade_write(out, NULL, NULL, buf, len);
          apr_bucket_destroy(b);
        }
      }
      apr_brigade_cleanup(bb);
    }
    else {
      /* we expext a bb (even it might be empty)!
       client may have closed the connection?
       or any other filter in the chain has canceled the request? */
      char buf[MAX_STRING_LEN];
      buf[0] = '\0';
      if (status > 0) {
        apr_strerror(status, buf, sizeof(buf));
      }
      *error = apr_psprintf(r->pool,
          "Input filter: Failed reading data from client."
            " Blocked by another filter in chain? [%s]", buf);
      seen_eos = 1;
    }
  }
  while (!seen_eos);

  apr_brigade_length(out, 1, &off);

  /* correct content-length header if deflate filter runs before */
  enc = apr_table_get(r->headers_in, "Transfer-Encoding");
  if (!enc || strcasecmp(enc, "chunked") != 0) {
    len_str = apr_off_t_toa(r->pool, off);
    apr_table_set(r->headers_in, "Content-Length", len_str);
    r->remaining = off;
  }

  return status;
}

/**
 * Creates a new parameter parser.
 *
 * @param r IN request record
 *
 * @return new parameter parser instance
 */
AP_DECLARE(parp_t *) parp_new(request_rec *r, int flags) {
  parp_t *self = apr_pcalloc(r->pool, sizeof(parp_t));

  self->pool = r->pool;
  self->r = r;
  self->bb = apr_brigade_create(r->pool, r->connection->bucket_alloc);
  self->params = apr_table_make(r->pool, 5);
  self->rw_params = NULL;
  self->rw_params_query_structure = NULL;
  self->rw_params_body_structure = NULL;
  self->parsers = apr_table_make(r->pool, 3);
  apr_table_setn(self->parsers, apr_pstrdup(r->pool,
      "application/x-www-form-urlencoded"), (char *) parp_parser_urlencode);
  apr_table_setn(self->parsers, apr_pstrdup(r->pool, "multipart/form-data"),
      (char *) parp_parser_multipart);
  apr_table_setn(self->parsers, apr_pstrdup(r->pool, "multipart/mixed"),
      (char *) parp_parser_multipart);
  self->flags = flags;

  self->raw_body_data = NULL;
  self->raw_body_data_len = 0;
  self->use_raw_body = 0;

  self->content_typeclass = NONE;

  self->data_body = NULL;
  self->len_body = 0;

  self->recursion = 0;

  self->tmp_buffer = NULL;
  self->len_tmp_buffer = 0;

  return self;
}

/**
 * Get all parameter/value pairs in this request
 *
 * @param self IN instance
 * @param params OUT table of key/value pairs
 *
 * @return APR_SUCCESS or APR_EINVAL on parser errors
 *
 * @note: see parap_error(self) for detailed error message
 */
AP_DECLARE(apr_status_t) parp_read_params(parp_t *self) {
  apr_status_t status;
  parp_parser_f parser;
  request_rec *r = self->r;
  int modify = 0;
  apr_array_header_t *hs = apr_optional_hook_get("modify_body_hook"); // only for backwards compatibility
  apr_array_header_t *hs2 = apr_optional_hook_get("modify_hook");
  if (((hs != NULL) && (hs->nelts > 0)) || ((hs2 != NULL) && (hs2->nelts > 0))) {
    /* module has registered */
    self->rw_params = apr_array_make(r->pool, 50, sizeof(parp_entry_t));
    modify = 1;
  }
  if (r->args) { // read query parameters
    if (modify == 1) {
      self->rw_params_query_structure = apr_array_make(r->pool, 50,
          sizeof(parp_query_structure_t));
    }
    if ((status = parp_parser_urlencode(self, QUERY, r->headers_in, r->args,
        strlen(r->args), self->rw_params_query_structure))
        != APR_SUCCESS) {
      return status;
    }

  }
  if (parp_has_body(self)) {
    if (modify == 1) {
      self->rw_params_body_structure = apr_array_make(r->pool, 50,
          sizeof(parp_body_structure_t));
    }
    if ((status = parp_get_payload(self)) != APR_SUCCESS) {
      return status;
    }
    parser
        = parp_get_parser(self, apr_table_get(r->headers_in, "Content-Type"));
    if ((status = parser(self, BODY, r->headers_in, self->raw_body_data,
        self->raw_body_data_len, self->rw_params_body_structure))
        != APR_SUCCESS) {
      /* only set data to self pointer if untouched by parser, 
       * because parser could modify body data */
      if (status == APR_ENOTIMPL) {
      }
      return status;
    }
  }
  return APR_SUCCESS;
}


AP_DECLARE (apr_status_t) parp_write_nested_multipart(parp_t *self,
    apr_bucket_brigade * bb, apr_off_t* freebytes, parp_body_structure_t *multipart) {

  int i;
  apr_status_t rv;
  parp_entry_t *rw_entries = (parp_entry_t *) self->rw_params->elts;


  if (multipart->multipart_parameters && multipart->multipart_parameters->nelts == multipart->multipart_parameters_ndelete) { // all multipart elements are deleted
    self->raw_body_data = &self->raw_body_data[multipart->raw_len];
    self->raw_body_data_len -= multipart->raw_len;
    multipart->written_to_brigade = 1;
  } else {

    // writing nested mulitpart header
    if (*freebytes >= multipart->mulitpart_nested_header_len) {
      if ((rv = apr_brigade_write(bb, NULL, NULL, self->raw_body_data, multipart->mulitpart_nested_header_len)) != APR_SUCCESS) { return rv;}
      self->raw_body_data_len -= multipart->mulitpart_nested_header_len;
      self->raw_body_data = &self->raw_body_data[multipart->mulitpart_nested_header_len];
      *freebytes -= multipart->mulitpart_nested_header_len;
    } else {
      return PARP_ERR_BRIGADE_FULL;
    }

    // writing elements
    parp_body_structure_t *multipart_param_entries =
                      (parp_body_structure_t *) multipart->multipart_parameters->elts;
    for (i = 0; i < multipart->multipart_parameters->nelts; ++i) {
      parp_body_structure_t *mp = &multipart_param_entries[i];
      if (mp->rw_array_index >= 0 && mp->rw_array_index < self->rw_params->nelts && mp->written_to_brigade == 0) {
        parp_entry_t *e = &rw_entries[mp->rw_array_index];
        if (e->delete != 0) { // delete
          self->raw_body_data = &self->raw_body_data[mp->raw_len];
          self->raw_body_data_len -= mp->raw_len;
          mp->written_to_brigade = 1;
        } else if (e->new_value != NULL) { // new value
          if (*freebytes >= mp->raw_len_modified) {
            int key_len = mp->value_addr-mp->key_addr;
            if ((rv = apr_brigade_write(bb, NULL, NULL, self->raw_body_data, key_len)) != APR_SUCCESS) { return rv;}
            self->raw_body_data_len -= key_len;
            self->raw_body_data = &self->raw_body_data[key_len];

            // remove old value fom raw_body_data
            self->raw_body_data = &self->raw_body_data[strlen(e->value)];
            self->raw_body_data_len -= strlen(e->value);
            // write new value
            if ((rv = apr_brigade_write(bb, NULL, NULL, e->new_value, strlen(e->new_value))) != APR_SUCCESS) { return rv;}
            // write rest of multipartdata
            int rest_len = &mp->multipart_addr[mp->raw_len] - self->raw_body_data;
            if ((rv = apr_brigade_write(bb, NULL, NULL, self->raw_body_data, rest_len)) != APR_SUCCESS) { return rv;}
            self->raw_body_data_len -= rest_len;
            self->raw_body_data = &self->raw_body_data[rest_len];
            *freebytes -= mp->raw_len_modified;
            mp->written_to_brigade = 1;
          } else {
            return PARP_ERR_BRIGADE_FULL;
          }
        } else { // no changes
          if (*freebytes >= mp->raw_len) {
            if ((rv = apr_brigade_write(bb, NULL, NULL, self->raw_body_data, mp->raw_len)) != APR_SUCCESS) { return rv;}
            self->raw_body_data = &self->raw_body_data[mp->raw_len];
            self->raw_body_data_len -= mp->raw_len;
            *freebytes -= mp->raw_len;
            mp->written_to_brigade = 1;
          } else {
            return PARP_ERR_BRIGADE_FULL;
          }
        }

      } else if (mp->multipart_parameters->nelts > 0 && mp->rw_array_index < 0) { // nested multipart
        if ((rv = parp_write_nested_multipart(self, bb, freebytes, mp)) != APR_SUCCESS) {
          return rv;
        }
      }
    }
    // writing end boundary
    int rest_len = &multipart->multipart_addr[multipart->raw_len] - self->raw_body_data;
    if (rest_len > 0) {
      if (*freebytes >= rest_len) {
        if ((rv = apr_brigade_write(bb, NULL, NULL, self->raw_body_data, rest_len)) != APR_SUCCESS) { return rv;}
        self->raw_body_data = &self->raw_body_data[rest_len];
        self->raw_body_data_len -= rest_len;
        multipart->written_to_brigade = 1;
        *freebytes -= rest_len;
      } else {
        return PARP_ERR_BRIGADE_FULL;
      }
    }


  }
  return APR_SUCCESS;
}

/**
 * Forward all data back to request.
 *
 * @param f IN filter
 * @param bb IN bucket brigade
 * @param mode IN 
 * @param block IN block mode
 * @param nbytes IN requested bytes
 *
 * @return any apr status
 */
AP_DECLARE (apr_status_t) parp_forward_filter(ap_filter_t * f,
    apr_bucket_brigade * bb, ap_input_mode_t mode, apr_read_type_e block,
    apr_off_t nbytes) {

  int i;
  apr_status_t rv;
  apr_bucket *e;
  apr_size_t len;
  const char *buf;
  apr_off_t read = 0;
  parp_t *self = f->ctx;

  if (self == NULL || (f->r && f->r->status != 200)) {
    /* nothing to do ... */
    return ap_get_brigade(f->next, bb, mode, block, nbytes);
  }

  if (self->use_raw_body) {
    parp_entry_t *rw_entries = (parp_entry_t *) self->rw_params->elts;
    /* forward data from the raw buffer and apply modifications */
    apr_off_t bytes = nbytes <= self->raw_body_data_len ? nbytes : self->raw_body_data_len;
    apr_off_t freebytes = nbytes;

    if (self->content_typeclass == FORMDATA) {

      parp_body_structure_t *body_structure_entries =
          (parp_body_structure_t *) self->rw_params_body_structure->elts;

      if(self->len_tmp_buffer > 0) {
        // we still have some data left from the last filter call
        apr_off_t toSend = self->len_tmp_buffer;
        if(toSend > freebytes) {
          toSend = freebytes;
        }
        self->len_tmp_buffer -= toSend;
        if ((rv = apr_brigade_write(bb, NULL, NULL, self->tmp_buffer, toSend)) != APR_SUCCESS) {
          return rv;
        }
        self->tmp_buffer += toSend;
        freebytes -= toSend;
        if(self->len_tmp_buffer > 0) {
          // still not done...
          return APR_SUCCESS;
        }
      };

      for (i = 0; i < self->rw_params_body_structure->nelts; ++i) {
        parp_body_structure_t *bs = &body_structure_entries[i];
        
        if (bs->rw_array_index >= 0 && bs->rw_array_index < self->rw_params->nelts
            && bs->written_to_brigade == 0) {
          parp_entry_t *e = &rw_entries[bs->rw_array_index];
          char *tmp_param = NULL;
          if (e->new_value != NULL) {
            tmp_param = apr_pstrcat(self->pool, e->key, "=", e->new_value, NULL);
          } else if (e->delete == 0) { // value has not changed and is not delete
            tmp_param = apr_pstrcat(self->pool, e->key, "=", e->value, NULL);
          }
          if (tmp_param != NULL) {
            if (self->flags & PARP_FLAGS_FIRST_PARAM_WRITTEN) {
              tmp_param = apr_pstrcat(self->pool, "&", tmp_param, NULL);
            } else {
              self->flags |= PARP_FLAGS_FIRST_PARAM_WRITTEN;
            }
            apr_off_t slen = strlen(tmp_param);
            if (freebytes >= slen) { // enough space in brigade
              if ((rv = apr_brigade_write(bb, NULL, NULL, tmp_param, slen)) != APR_SUCCESS) {
                return rv;
              }
              bs->written_to_brigade = 1;
              freebytes -= slen;
              self->raw_body_data = &self->raw_body_data[bs->raw_len];
              self->raw_body_data_len -= bs->raw_len;
              if (self->raw_body_data[0] == '&') {
                self->raw_body_data++;
                self->raw_body_data_len--;
              }
            } else {
              // not enough space in brigade, write as much as we can
              // ans store the remaining data...
              if ((rv = apr_brigade_write(bb, NULL, NULL, tmp_param, freebytes)) != APR_SUCCESS) {
                return rv;
              }
              slen -= freebytes;
              self->len_tmp_buffer = slen;
              self->tmp_buffer = &tmp_param[freebytes];
              bs->written_to_brigade = 1;
              self->raw_body_data = &self->raw_body_data[bs->raw_len];
              self->raw_body_data_len -= bs->raw_len;
              if (self->raw_body_data[0] == '&') {
                self->raw_body_data++;
                self->raw_body_data_len--;
              }
              freebytes = 0;
              // ...and process further in the next round
              return APR_SUCCESS;
              //break;
            }
            
          } else {
            bs->written_to_brigade = 1;
            self->raw_body_data = &self->raw_body_data[bs->raw_len];
            self->raw_body_data_len -= bs->raw_len;
            if (self->raw_body_data[0] == '&') {
              self->raw_body_data++;
              self->raw_body_data_len--;
            }
          }
        }
      }
      if (i == self->rw_params_body_structure->nelts) { // all parameters written, clean raw_body_data up
        if (self->raw_body_data_len > 0) {
          if (freebytes >= self->raw_body_data_len) {
            rv = apr_brigade_write(bb, NULL, NULL, self->raw_body_data, self->raw_body_data_len);
            freebytes -= self->raw_body_data_len;
            self->raw_body_data = &self->raw_body_data[self->raw_body_data_len];
            self->raw_body_data_len = 0;
          } else {
            rv = apr_brigade_write(bb, NULL, NULL, self->raw_body_data, freebytes);
            freebytes = 0;
            self->raw_body_data = &self->raw_body_data[freebytes];
            self->raw_body_data_len -= freebytes;
          }
        }
      }
    } else if (self->content_typeclass == MULTIPART) {

      parp_body_structure_t *body_structure_entries =
                (parp_body_structure_t *) self->rw_params_body_structure->elts;

      if(self->len_tmp_buffer > 0) {
        // we still have some data left from the last filter call
        apr_off_t toSend = self->len_tmp_buffer;
        if(toSend > freebytes) {
          toSend = freebytes;
        }
        self->len_tmp_buffer -= toSend;
        if ((rv = apr_brigade_write(bb, NULL, NULL, self->tmp_buffer, toSend)) != APR_SUCCESS) { return rv;}
        self->tmp_buffer += toSend;
        freebytes -= toSend;
        if(self->len_tmp_buffer > 0) {
          // still not done...
          return APR_SUCCESS;
        }
      };

      for (i = 0; i < self->rw_params_body_structure->nelts; ++i) {
        parp_body_structure_t *bs = &body_structure_entries[i];

        // write preamble if exist
        bytes = bs->multipart_addr - self->raw_body_data;
        if (bytes > 0) {
          if (freebytes >= bytes) {
            rv = apr_brigade_write(bb, NULL, NULL, self->raw_body_data, bytes);
            self->raw_body_data = &self->raw_body_data[bytes];
            self->raw_body_data_len -= bytes;
            freebytes -= bytes;
          } else {
            rv = apr_brigade_write(bb, NULL, NULL, self->raw_body_data, freebytes);
            self->raw_body_data = &self->raw_body_data[freebytes];
            self->raw_body_data_len -= freebytes;
            freebytes = 0;
            break;
          }
        }
        if (bs->written_to_brigade == 0) {
          if (bs->multipart_parameters && bs->multipart_parameters->nelts == bs->multipart_parameters_ndelete) { // all multipart elements are deleted
            self->raw_body_data = &self->raw_body_data[bs->raw_len];
            self->raw_body_data_len -= bs->raw_len;
            bs->written_to_brigade = 1;
          } else {
            parp_body_structure_t *multipart_param_entries =
                            (parp_body_structure_t *) bs->multipart_parameters->elts;
            for (i = 0; i < bs->multipart_parameters->nelts; ++i) {
              parp_body_structure_t *mp = &multipart_param_entries[i];
              if (mp->written_to_brigade == 0) {
                if (mp->rw_array_index >= 0 && mp->rw_array_index < self->rw_params->nelts) {
                  parp_entry_t *e = &rw_entries[mp->rw_array_index];
                  if (e->delete != 0) { // delete
                    self->raw_body_data = &self->raw_body_data[mp->raw_len];
                    self->raw_body_data_len -= mp->raw_len;
                    mp->written_to_brigade = 1;
                  } else if (e->new_value != NULL) { // new value

                    if (freebytes >= mp->raw_len_modified) {
                      int key_len = mp->value_addr - mp->key_addr;

                      if ((rv = apr_brigade_write(bb, NULL, NULL, self->raw_body_data, key_len)) != APR_SUCCESS) { return rv;}
                      self->raw_body_data_len -= key_len;
                      self->raw_body_data = &self->raw_body_data[key_len];

                      // remove old value fom raw_body_data
                      self->raw_body_data = &self->raw_body_data[strlen(e->value)];
                      self->raw_body_data_len -= strlen(e->value);
                      // write new value
                      if ((rv = apr_brigade_write(bb, NULL, NULL, e->new_value, strlen(e->new_value))) != APR_SUCCESS) { return rv;}
                      // write rest of multipartdata
                      int rest_len = &mp->multipart_addr[mp->raw_len] - self->raw_body_data;
                      if ((rv = apr_brigade_write(bb, NULL, NULL, self->raw_body_data, rest_len)) != APR_SUCCESS) { return rv;}
                      self->raw_body_data_len -= rest_len;
                      self->raw_body_data = &self->raw_body_data[rest_len];
                      mp->written_to_brigade = 1;
                    } else {
                      return APR_SUCCESS;
                    }
                  } else { // no changes
                    if (freebytes >= mp->raw_len) {
                      if ((rv = apr_brigade_write(bb, NULL, NULL, self->raw_body_data, mp->raw_len)) != APR_SUCCESS) { return rv;}
                      self->raw_body_data = &self->raw_body_data[mp->raw_len];
                      self->raw_body_data_len -= mp->raw_len;
                      mp->written_to_brigade = 1;
                    } else {
                      self->tmp_buffer = self->raw_body_data;
                      self->len_tmp_buffer = mp->raw_len;
                      self->raw_body_data += mp->raw_len;
                      self->raw_body_data_len -= mp->raw_len;
                      mp->written_to_brigade = 1;
                      if ((rv = apr_brigade_write(bb, NULL, NULL, self->tmp_buffer, freebytes)) != APR_SUCCESS) { return rv;}
                      self->len_tmp_buffer -= freebytes;
                      self->tmp_buffer += freebytes;
                      return APR_SUCCESS;
                    }
                  }

                } else if (mp->multipart_parameters && mp->multipart_parameters->nelts > 0 && mp->rw_array_index < 0) { // nested multipart
                  rv = parp_write_nested_multipart(self, bb, &freebytes, mp);
                  if (rv == PARP_ERR_BRIGADE_FULL) {
                    return APR_SUCCESS;
                  } else if (rv != APR_SUCCESS) {
                    return rv;
                  }
                  mp->written_to_brigade = 1;
                }
              }
            }
            bs->written_to_brigade = 1;
          }
        }
        // write postamble if exist
        if (self->raw_body_data_len > 0) {
          if (freebytes >= self->raw_body_data_len) {
            rv = apr_brigade_write(bb, NULL, NULL, self->raw_body_data, self->raw_body_data_len);
            freebytes -= self->raw_body_data_len;
            self->raw_body_data = &self->raw_body_data[self->raw_body_data_len];
            self->raw_body_data_len = 0;
          } else {
            rv = apr_brigade_write(bb, NULL, NULL, self->raw_body_data, freebytes);
            freebytes = 0;
            self->raw_body_data = &self->raw_body_data[freebytes];
            self->raw_body_data_len -= freebytes;
          }
        }
      }
    }

    if (self->raw_body_data_len == 0) {
      ap_remove_input_filter(f);
    }
  }
  else {
    /* transparent forwarding */
    /* do never send a bigger brigade than request with "nbytes"! */
    while (read < nbytes && !APR_BRIGADE_EMPTY(self->bb)) {
      e = APR_BRIGADE_FIRST(self->bb);
      rv = apr_bucket_read(e, &buf, &len, block);
      if (rv != APR_SUCCESS) {
        return rv;
      }
      if (len + read > nbytes) {
        apr_bucket_split(e, nbytes - read);
        APR_BUCKET_REMOVE(e);
        APR_BRIGADE_INSERT_TAIL(bb, e);
        return APR_SUCCESS;
      }
      APR_BUCKET_REMOVE(e);
      APR_BRIGADE_INSERT_TAIL(bb, e);
      read += len;
    }
    if (APR_BRIGADE_EMPTY(self->bb)) {
      /* our work is done so remove this filter */
      ap_remove_input_filter(f);
    }
  }
  return APR_SUCCESS;
}

/**
 * Get all parameter/value pairs in this request
 *
 * @param self IN instance
 * @param params OUT table of key/value pairs
 *
 * @return APR_SUCCESS
 */
AP_DECLARE(apr_status_t) parp_get_params(parp_t *self, apr_table_t **params) {
  *params = self->params;
  return APR_SUCCESS;
}

/**
 * Get error message on error
 *
 * @param self IN instance
 *
 * @return error message, empty message or NULL if instance not valid
 */
AP_DECLARE(char *) parp_get_error(parp_t *self) {
  if (self && self->error) {
    return apr_pstrdup(self->pool, self->error);
  }
  else {
    return NULL;
  }
}

/**
 * Optional function which may be used by Apache modules
 * to access the parameter table.
 *
 * @param r IN request record
 *
 * @return table with the request parameter or NULL if not available
 */
AP_DECLARE(apr_table_t *)parp_hp_table(request_rec *r) {
  parp_t *parp = ap_get_module_config(r->request_config, &parp_module);
  apr_table_t *tl = NULL;
  if (parp) {
    parp_get_params(parp, &tl);
  }
  return tl;
}

/**
 * Optional function which may be used by Apache modules
 * to access the body data. Only get data if not allready
 * parsed (and modified) and parser was active.
 *
 * @param r IN request record
 * @param len OUT body data len
 *
 * @return body data or NULL
 */
AP_DECLARE(const char *)parp_body_data(request_rec *r, apr_size_t *len) {
  parp_t *parp = ap_get_module_config(r->request_config, &parp_module);
  *len = 0;
  if (parp && parp->data_body) {
    *len = parp->len_body;
    return parp->data_body;
  }
  return NULL;
}

/**
 * Verifies if some values have been changed and adjust content length header. Also
 * sets the "use_raw_body" flag to signalize the input filter to forward the modifed data.
 */
static void parp_update_content_length_multipart(parp_t *self, parp_body_structure_t *parent,
    apr_off_t *contentlen) {

  int i;
  parp_entry_t *rw_entries = (parp_entry_t *) self->rw_params->elts;

  parp_body_structure_t *body_structure_entries =
      (parp_body_structure_t *) parent->multipart_parameters->elts;
  for (i = 0; i < parent->multipart_parameters->nelts; ++i) {
    parp_body_structure_t *bs = &body_structure_entries[i];
    if (bs->rw_array_index == -1 && bs->multipart_parameters != NULL) { //multipart
      parp_update_content_length_multipart(self,bs, contentlen);
      if (bs->multipart_parameters_ndelete == bs->multipart_parameters->nelts) {
        *contentlen = *contentlen - bs->raw_len_modified;
        parent->raw_len_modified -= bs->raw_len;
        parent->multipart_parameters_ndelete++;
      }
    } else {
      if (bs->rw_array_index >= 0 && bs->rw_array_index < self->rw_params->nelts) { // valid index
        parp_entry_t *e = &rw_entries[bs->rw_array_index];
        if (e->new_value != NULL) {
          *contentlen = *contentlen + strlen(e->new_value) - strlen(e->value); // TODO: this is not safe from security coding prospective
          self->use_raw_body = 1;
        } else if (e->delete != 0) {
          *contentlen = *contentlen - bs->raw_len;
          parent->raw_len_modified -= bs->raw_len;
          parent->multipart_parameters_ndelete++;
          self->use_raw_body = 1;
        }
      }
    }
  }
}

/**
 * Verifies if some values have been changed and adjust content length header. Also
 * sets the "use_raw_body" flag to signalize the input filter to forward the modifed data.
 */
static void parp_update_content_length(request_rec *r, parp_t *self,
    apr_off_t *contentlen) {

  int i;
  if (self->rw_params_body_structure && self->rw_params) {
    parp_entry_t *rw_entries = (parp_entry_t *) self->rw_params->elts;
    parp_body_structure_t *body_structure_entries =
        (parp_body_structure_t *) self->rw_params_body_structure->elts;
    for (i = 0; i < self->rw_params_body_structure->nelts; ++i) {
      parp_body_structure_t *bs = &body_structure_entries[i];

      if (bs->rw_array_index >= 0 && bs->multipart_parameters == NULL) { // no multipart
        parp_entry_t *pe = &rw_entries[bs->rw_array_index];
        if (pe->new_value) {
          int diff = strlen(pe->new_value) - strlen(pe->value);  // TODO: this is not safe from security coding prospective
          *contentlen = *contentlen + diff;
          bs->raw_len_modified = bs->raw_len_modified + diff;
          self->use_raw_body = 1;
        }
        else if (pe->delete == 1) {
          int temp_len = strlen(pe->key) + 1 + strlen(pe->value);
          if (*contentlen == temp_len) {
            *contentlen = 0;
            bs->raw_len_modified = 0;
          }
          else {
            *contentlen = *contentlen - temp_len - 1; // remove also '&'
            bs->raw_len_modified -= temp_len;
          }
          self->use_raw_body = 1;
        }
      }
      else { // multipart
        parp_update_content_length_multipart(self, bs, contentlen);
        if (bs->multipart_parameters_ndelete == bs->multipart_parameters->nelts) {
          *contentlen = *contentlen - bs->raw_len_modified;
        }
      }
    }
    if (apr_table_get(r->headers_in, "Content-Length")) {
      apr_table_set  (r->headers_in, "Content-Length", apr_psprintf(r->pool, "%"APR_OFF_T_FMT, *contentlen));
    }
  }
}


static void parp_update_query_parameter(request_rec *r, parp_t *self) {

  int i;
  // update query parameters
  if (!apr_is_empty_array(self->rw_params_query_structure)) {
    char *new_arg = NULL;
    int query_has_changed = 0;

    parp_entry_t *rw_entries = (parp_entry_t *) self->rw_params->elts;
    parp_query_structure_t *query_structure_entries =
        (parp_query_structure_t *) self->rw_params_query_structure->elts;
    for (i = 0; i < self->rw_params_query_structure->nelts; ++i) {
      parp_query_structure_t *qs = &query_structure_entries[i];
      if (qs->rw_array_index >= 0 && qs->rw_array_index < self->rw_params->nelts) {
        parp_entry_t *e = &rw_entries[qs->rw_array_index];
        char *tmp_param = NULL;
        if (e->new_value != NULL) {
          tmp_param = apr_pstrcat(self->pool, e->key, "=", e->new_value, NULL);
          query_has_changed = 1;
        } else if (e->delete != 0) {
          query_has_changed = 1;
        } else {
          tmp_param = apr_pstrcat(self->pool, e->key, "=", e->value, NULL);
        }
        if (tmp_param != NULL) {
          if (new_arg != NULL) {
            new_arg = apr_pstrcat(self->pool, new_arg, "&", tmp_param, NULL);
          } else {
            new_arg = apr_pstrdup(self->pool, tmp_param);
          }
        }
      }
    }
    if (query_has_changed == 1) {
      char *unparsed_uri = apr_pstrdup(self->pool, r->unparsed_uri);

      char *anchorstart = strchr(unparsed_uri, '#');
      char *querystart = strchr(unparsed_uri, '?');
      if (querystart != NULL) {
        querystart[0] = '\0';
      }

      char *new_uri;
      if (new_arg != NULL) {
        new_uri = apr_pstrcat(self->pool, unparsed_uri, "?", new_arg, NULL);
      }
      else {
        new_uri = apr_pstrcat(self->pool, unparsed_uri, NULL);
      }
      if (anchorstart != NULL) {
        new_uri = apr_pstrcat(self->pool, new_uri, anchorstart, NULL);
      }

      // update r->the_request
      char *hp = strstr(r->the_request, r->unparsed_uri);
      if(hp) {
        hp[0] = '\0';
        r->the_request = apr_pstrdup(r->pool, r->the_request);
        hp += strlen(r->unparsed_uri);
        r->the_request = apr_pstrcat(r->pool, r->the_request, new_uri, hp, NULL);
      }
      // restore all uri parameter
      ap_parse_uri(r, new_uri);
    }
  }
}
/************************************************************************
 * handlers
 ***********************************************************************/

/**
 * Hook to delete parameter which has been defined by the notes PARP_DELETE_PARAM.
 */
static apr_status_t parp_delete_parameter(request_rec *r, apr_array_header_t *array) {
  int i;
  parp_entry_t *entries = (parp_entry_t *)array->elts;

  /* create a table from the r->notes with all defined parameter names to be remoced */
  apr_table_t *param_table = apr_table_make(r->pool, 10);
  apr_table_entry_t *e = (apr_table_entry_t *) apr_table_elts(r->notes)->elts;
  for(i = 0; i < apr_table_elts(r->notes)->nelts; ++i) {
    if(e[i].key && e[i].val && strcmp(e[i].key, PARP_DELETE_PARAM) == 0) {
      apr_table_set(param_table, e[i].val, "");
    }
  }

  /* iterate through the received parameters and remove those within our param_table */
  for(i = 0; i < array->nelts; ++i) {
    parp_entry_t *b = &entries[i];
    if(apr_table_get(param_table, b->key)) {
      b->delete = 1;
    }
  }
  return DECLINED;
}

static int parp_post_config(apr_pool_t *pconf, apr_pool_t *plog,
                            apr_pool_t *ptemp, server_rec *bs) {
  // register hook to delete parameters as defined by r->notes
  APR_OPTIONAL_HOOK(parp, modify_hook, parp_delete_parameter, NULL, NULL, APR_HOOK_MIDDLE);
  return DECLINED;
}

/**
 * Header parser starts body parsing when reading "parp" in
 * the process environment or request notes and calls all
 * functions  registered to the hs_hook.
 *
 * @param r IN request record
 * @return DECLINED if inactive, return code of the registered
 *         functions or the value defined by PARP_ExitOnError
 *         on any parser error.
 */
static int parp_header_parser(request_rec * r) {
  apr_status_t status = DECLINED;
  if (ap_is_initial_req(r)) {
    const char *e = apr_table_get(r->notes, "parp");
    if (e == NULL) {
      e = apr_table_get(r->subprocess_env, "parp");
    }
    if (e == NULL) {
      /* no event */
      return DECLINED;
    } else {
      apr_table_t *tl;
      parp_t *parp = parp_new(r, PARP_FLAGS_NONE);
      ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r,
                    PARP_LOG_PFX(000)"enabled (%s)", e);

      status = parp_read_params(parp);
      ap_set_module_config(r->request_config, &parp_module, parp);
      ap_add_input_filter("parp-forward-filter", parp, r, r->connection);
      if (status == APR_SUCCESS) {
        apr_off_t contentlen;
        parp_get_params(parp, &tl);
        apr_brigade_length(parp->bb, 1, &contentlen);

        status = parp_run_hp_hook(r, tl);
        if (parp->rw_params) {
          parp_run_modify_body_hook(r, parp->rw_params); // only for backwards compatibility
          parp_run_modify_hook(r, parp->rw_params);
          parp_update_content_length(r, parp, &contentlen);
          parp_update_query_parameter(r, parp);
        }
        apr_table_set (r->subprocess_env,
          "PARPContentLength",
          apr_psprintf(r->pool, "%"APR_OFF_T_FMT, contentlen));
      } else {
        parp_srv_config *sconf = ap_get_module_config(r->server->module_config,
                                                      &parp_module);
        char *error = parp_get_error(parp);

        ap_log_rerror(APLOG_MARK, sconf->onerror == 200 ? APLOG_WARNING : APLOG_ERR, 0, r,
                      PARP_LOG_PFX(010)"parser error, rc=%d (%s)",
                      sconf->onerror == -1 ? 500 : sconf->onerror,
                      error == NULL ? "-" : error);
        if(sconf->onerror == 200) {
          return DECLINED;
        }
        if(sconf->onerror == -1) {
          status = HTTP_INTERNAL_SERVER_ERROR;
        } else {
          status = sconf->onerror;
        }
      }
    }
  }
  return status;
}

static void *parp_srv_config_create(apr_pool_t *p, server_rec *s) {
  parp_srv_config *sconf = apr_pcalloc(p, sizeof(parp_srv_config));
  sconf->onerror = -1; /* -1 is handles same as 500 but is the default (used for merger) */
  return sconf;
}

static void *parp_srv_config_merge(apr_pool_t *p, void *basev, void *addv) {
  parp_srv_config *b = (parp_srv_config *) basev;
  parp_srv_config *o = (parp_srv_config *) addv;
  if (o->onerror == -1) {
    o->onerror = b->onerror;
  }
  if (o->parsers == NULL) {
    o->parsers = b->parsers;
  }
  return o;
}

/************************************************************************
 * directiv handlers 
 ***********************************************************************/
const char *parp_error_code_cmd(cmd_parms *cmd, void *dcfg, const char *arg) {
  parp_srv_config *sconf = ap_get_module_config(cmd->server->module_config,
                                                &parp_module);
  sconf->onerror = atoi(arg);
  if (sconf->onerror == 200) {
    return NULL;
  }
  if ((sconf->onerror < 400) || (sconf->onerror > 599)) {
    return apr_psprintf(cmd->pool, "%s: error code must be a numeric value between 400 and 599"
                        " (or set 200 to ignore errors)",
                        cmd->directive->directive);
  }
  return NULL;
}

const char *parp_body_data_cmd(cmd_parms *cmd, void *dcfg, const char *arg) {
  parp_srv_config *sconf = ap_get_module_config(cmd->server->module_config,
                                                &parp_module);
  if (!sconf->parsers) {
    sconf->parsers = apr_table_make(cmd->pool, 5);
  }
  apr_table_setn(sconf->parsers, apr_pstrdup(cmd->pool, arg),
      (char *) parp_parser_get_body);
  return NULL;
}

static const command_rec parp_config_cmds[] = {
  AP_INIT_TAKE1("PARP_ExitOnError", parp_error_code_cmd, NULL,
                RSRC_CONF,
                "PARP_ExitOnError <code>, defines the HTTP error code"
                " to return on parsing errors. Default is 500."
                " Specify 200 in order to ignore errors."),
  AP_INIT_ITERATE("PARP_BodyData", parp_body_data_cmd, NULL,
                  RSRC_CONF,
                  "PARP_BodyData <content-type>, defines content"
                  " types where only the body data are read. Default is"
                  " no content type. Use '*/*' no activate the body read"
                  " parser for any content type."), 
  { NULL }
};

/************************************************************************
 * apache register 
 ***********************************************************************/
static void parp_register_hooks(apr_pool_t * p) {
  static const char *pre[] = { "mod_setenvif.c", "mod_deflate.c", NULL };
  /* header parser is invoked after mod_setenvif */
  ap_hook_header_parser(parp_header_parser, pre, NULL, APR_HOOK_MIDDLE);
  ap_hook_post_config(parp_post_config, pre, NULL, APR_HOOK_MIDDLE);
  ap_register_input_filter("parp-forward-filter", parp_forward_filter, NULL, AP_FTYPE_RESOURCE);
  APR_REGISTER_OPTIONAL_FN(parp_hp_table);
  APR_REGISTER_OPTIONAL_FN(parp_body_data);
}

/************************************************************************
 * apache module definition 
 ***********************************************************************/
module AP_MODULE_DECLARE_DATA parp_module ={ 
  STANDARD20_MODULE_STUFF,
  NULL,                                     /**< dir config creater */
  NULL,                                     /**< dir merger */
  parp_srv_config_create,                   /**< server config */
  parp_srv_config_merge,                    /**< server merger */
  parp_config_cmds,                         /**< command table */
  parp_register_hooks,                      /**< hook registery */
};
