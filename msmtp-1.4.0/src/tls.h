/*
 * tls.h
 * 
 * This file is part of msmtp, an SMTP client.
 *
 * Copyright (C) 2000, 2003, 2004, 2005
 * Martin Lambers <marlam@marlam.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *   msmtp is released under the GPL with the additional exemption that
 *   compiling, linking, and/or using OpenSSL is allowed.
 */

#ifndef TLS_H
#define TLS_H

#ifdef HAVE_GNUTLS
#include <gnutls/gnutls.h>
#endif /* HAVE_GNUTLS */
#ifdef HAVE_OPENSSL
#include <openssl/ssl.h>
#endif /* HAVE_OPENSSL */


/* All 'errstr' arguments must point to buffers that are at least
 * 'errstr_bufsize' characters long.
 * If a function with an 'errstr' argument returns a value != TLS_EOK,
 * 'errstr' will contain an error message (maybe just "").
 * If such a function returns TLS_EOK, 'errstr' will not be changed.
 */
extern const size_t errstr_bufsize;
#define TLS_EOK		0	/* no error */
#define TLS_ELIBFAILED	1	/* The underlying library failed */
#define TLS_ESEED	2	/* Cannot seed pseudo random number generator */
#define TLS_ECERT	3	/* Certificate check or verification failed */
#define TLS_EIO		4	/* Input/output error */
#define TLS_EFILE	5	/* A file does not exist/cannot be read */
#define TLS_EHANDSHAKE	6	/* TLS handshake failed */


/*
 * Always use tls_clear() before using a tls_t!
 * Never call a tls_*() function with tls_t NULL!
 */

typedef struct
{
    int have_trust_file;
    int is_active;
#ifdef HAVE_GNUTLS
    gnutls_session_t session;
    gnutls_certificate_credentials_t cred;
#endif /* HAVE_GNUTLS */
#ifdef HAVE_OPENSSL
    SSL_CTX *ssl_ctx;
    SSL *ssl;
#endif /* HAVE_OPENSSL */
} tls_t;

/*
 * Information about a X509 certificate.
 * The 6 owner_info and issuer_info fields are:
 *   Common Name
 *   Organization
 *   Organizational unit
 *   Locality
 *   State/Province
 *   Country
 * Each of these entries may be NULL if it was not provided.
 */
typedef struct
{
    unsigned char sha1_fingerprint[20];
    unsigned char md5_fingerprint[16];
    time_t activation_time;
    time_t expiration_time;
    char *owner_info[6];		
    char *issuer_info[6];
} tls_cert_info_t;

/*
 * tls_lib_init()
 *
 * Initialize underlying TLS library.
 * Used error codes: TLS_ELIBFAILED
 */
int tls_lib_init(char *errstr);

/*
 * tls_clear()
 *
 * Clears a tls_t type (marks it inactive).
 */
void tls_clear(tls_t *tls);

/*
 * tls_init()
 *
 * Initializes a tls_t. If both 'key_file' and 'ca_file' are not NULL, they are
 * set to be used when the peer request a certificate. If 'trust_file' is not
 * NULL, it will be used to verify the peer certificate.
 * All files must be in PEM format.
 * Used error codes: TLS_ELIBFAILED, TLS_EFILE
 */
int tls_init(tls_t *tls, 
	const char *key_file, const char *ca_file, const char *trust_file, 
	char *errstr);

/*
 * tls_start()
 *
 * Starts TLS encryption on a socket.
 * 'tls' must be initialized using tls_init().
 * If 'no_certcheck' is true, then no checks will be performed on the peer
 * certificate. If it is false and no trust file was set with tls_init(), 
 * only sanity checks are performed on the peer certificate. If it is false 
 * and a trust file was set, real verification of the peer certificate is 
 * performed.
 * 'hostname' is the host to start TLS with. It is needed for sanity checks/
 * verification.
 * 'tci' must be allocated with tls_cert_info_new(). Information about the 
 * peer's certificata will be stored in it. It can later be freed with
 * tls_cert_info_free(). 'tci' is allowed to be NULL; no certificate 
 * information will be passed in this case.
 * Used error codes: TLS_ELIBFAILED, TLS_ECERT, TLS_EHANDSHAKE
 */
int tls_start(tls_t *tls, int fd, const char *hostname, int no_certcheck,
	tls_cert_info_t *tci, char *errstr);

/*
 * tls_is_active()
 *
 * Returns whether 'tls' is an active TLS connection.
 */
int tls_is_active(tls_t *tls);

/*
 * tls_cert_info_new()
 * Returns a new tls_cert_info_t
 */
tls_cert_info_t *tls_cert_info_new(void);

/*
 * tls_cert_info_free()
 * Frees a tls_cert_info_t
 */
void tls_cert_info_free(tls_cert_info_t *tci);

/*
 * tls_cert_info_get()
 *
 * Extracts certificate information from the TLS connection 'tls' and stores
 * it in 'tci'. See the description of tls_cert_info_t above.
 * Used error codes: TLS_ECERT
 */
int tls_cert_info_get(tls_t *tls, tls_cert_info_t *tci, char *errstr);

/*
 * tls_getchar()
 *
 * Reads a character using TLS and stores it in 'c'.
 * Sets the 'eof' flag to 1 if EOF occurs and to 0 otherwise.
 * Used error codes: TLS_EIO
 */
int tls_getchar(tls_t *tls, char *c, int *eof, char *errstr);

/*
 * tls_gets()
 *
 * Reads in at most one less than 'size' characters from 'tls' and stores them
 * into the buffer pointed 'str'. Reading stops after an EOF or a newline.  
 * If a newline is read, it is stored into the buffer. A '\0' is stored after 
 * the last character in the buffer. The length of the resulting string (the
 * number of characters excluding the terminating '\0') will be stored in 'len'.
 * Used error codes: TLS_EIO
 */
int tls_gets(tls_t *tls, char *str, size_t size, size_t *len, char *errstr);

/*
 * tls_puts()
 *
 * Writes 'len' characters from the string 's' using TLS.
 * Used error codes: TLS_EIO
 */
int tls_puts(tls_t *tls, const char *s, size_t len, char *errstr);

/*
 * tls_close()
 *
 * Close a TLS connection and mark it inactive
 */
void tls_close(tls_t *tls);

/*
 * tls_lib_deinit()
 *
 * Deinit underlying TLS library.
 */
void tls_lib_deinit(void);

#endif
