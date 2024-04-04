/**
 * @file cipher_utils.c
 *
 * @detail Simple ciper utils.
 */

#include <alloca.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <syslog.h>
#include <stdio.h>

#include "config.h"

#ifdef HAVE_LIBGNUTLS
#include <nettle/rsa.h>
#include <nettle/knuth-lfib.h>
#include <gnutls/gnutls.h>
#include <gnutls/x509.h>
#include <gnutls/abstract.h>
#endif /* HAVE_LIBGNUTLS */

#ifdef HAVE_LIBSSL
#include <openssl/crypto.h>
#include <openssl/rsa.h>
#include <openssl/engine.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#endif /* HAVE_LIBSSL */

static const char* const common_n =
	"C4AEDE1BCE8E4B3142C622A88E6C3616D70DD7E500B7FEE5AFA771D56BE839F8"
	"4C6D2D6F63DFD65307F4F1BF25F95F40663A30B14A2818C2E5453B3AD30F49E2"
	"7FB4AE60DC87E1C232264A9C661E3E0F9B4FD9DAD8DD298034D9C7EEEF90D234"
	"2E66B53F2079C99D026468C28F54C9C97B91321415FAF3F2544DCB20A2348387";
static const char* const common_e =
	"10001";
static const char* const private_d =
	"74BFB04C254245D409E4FEF1BB08F4DA7DF153B435C7026168F1261B1FA08BB8"
	"9DF538C12431DC51112A0ECDFE469689EDA4A0FEBC1C2D06937D92F90A0BCB38"
	"C1C79E69670DA69F34A12A7BC9ED1AFC92911E32825F846384696D3B7A3E0103"
	"363F62D79B2CF308AA7AE9E253009FC1019022E813F168525D78BC0828976B71";
static const char* const secret_p =
	"FA35B9649B9882235036C4ECC5E104AD5DE48A1AD8CCBF70BC7AD08C0E74EF85"
	"94F081A1968FA290748567A462634173809812B701F321AF3B441D271A1C3865";
static const char* const secret_q =
	"C93C0B354237C9FC04455DAB62A3747782F0680FB3813A4DF46589428849710A"
	"20BAB9A80DF1E734CA999A411CE824C2941C3875F28D38E88CB11EE123C18F7B";
static const char* const secret_dmp1 =
	"55D62672D85A3A9308CAB14615207BED49DFC46D2F6116700389129069C93DFE"
	"92B8EB8FD6CA11D802D15D5BEC90E67C4C6A5F493F36FCA053F1AB1AA5FF766D";
static const char* const secret_dmq1 =
	"50DA8B364EF0D37B2ACC2422A473406C7473DB9BD367ECE13D7D3E7539052922"
	"049C81D400265DDB5C3BF33A8E3EC7E65654306B539562742D734D10F0A8E8D9";
static const char* const secret_iqmp =
	"54BCCC6EEF59852F0F33C0AF69F399E341E6ECCB6818C72259673B48D4E76A75"
	"FEEDFF348A77362469F82F5C1DE5AEF10E3D213B4403E0F82E6F7DD318858DDC";

#ifdef HAVE_LIBSSL
static void* realloc_safe(void* ptr, size_t size)
{
	void* p = realloc(ptr, size);
	if (!p)
		free(ptr);
	return p;
}

static RSA*
alloc_our_RSA()
{
	RSA* rsa = RSA_new();
	BN_hex2bn(&rsa->n, common_n);
	BN_hex2bn(&rsa->e, common_e);
	BN_hex2bn(&rsa->d, private_d);
	BN_hex2bn(&rsa->p, secret_p);
	BN_hex2bn(&rsa->q, secret_q);
	BN_hex2bn(&rsa->dmp1, secret_dmp1);
	BN_hex2bn(&rsa->dmq1, secret_dmq1);
	BN_hex2bn(&rsa->iqmp, secret_iqmp);
	return rsa;
}

static int
_hex2dec(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	else if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	else if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	else
		return 0;
}

/**
 * @brief Convert hex-encode string to plain text
 * @param[in] hexstr Hex encoded string. Must be multiple of 256.
 * @return Malloc'ed plain text string terminated with NULL char.
 */
char*
decrypt_hexstr2str(const char* hexstr)
{
	// hex string ==> cipher binary
	if (!hexstr)
		return NULL;

	size_t hexstr_len = strlen(hexstr);
	if (hexstr_len & 0xff)
	{
		//XPRINTF(DEBUG, "strlen(hexstr)=%zu\n", hexstr_len);
		return NULL;
	}

	char* plain = NULL;
	size_t pos = 0;
	size_t sz = 0;
	for (size_t i = 0; i < hexstr_len; i += 256)
	{
		unsigned char cipher[128];
		for (int j = 0; j < 128; j++)
		{
			int c = (_hex2dec(hexstr[i + (j << 1) + 0]) << 4) | _hex2dec(hexstr[i + (j << 1) + 1]);
			cipher[j] = c;
		}

		// cipher binary ==> plain text
		RSA* rsa = alloc_our_RSA();
		sz = sz ? sz + 128 : 128 + 1;
		plain = realloc_safe(plain, sz);
		int rc = RSA_private_decrypt(128, cipher, (unsigned char*)(plain + pos), rsa, RSA_PKCS1_PADDING);
		if (rc < 0)
		{
			unsigned long err;
			ERR_load_crypto_strings();
			while ((err = ERR_get_error()))
			{
				//XPRINTF(DEBUG, "Encryption Error. err=%lu %s\n", err, ERR_reason_error_string(err));
			}
			RSA_free(rsa);
			if (plain)
				free(plain);
			return NULL;
		}
		pos += rc;
		plain[pos] = '\0';
		RSA_free(rsa);
	}

	return plain;
}
#endif /* HAVE_LIBSSL */

#ifdef HAVE_LIBGNUTLS
static void
rsa_set_key(struct rsa_public_key *pub, struct rsa_private_key *key)
{
  mpz_set_str(pub->n, common_n, 16);
  mpz_set_str(pub->e, common_e, 16);

  rsa_public_key_prepare(pub);

  mpz_set_str(key->d, private_d, 16);
  mpz_set_str(key->p, secret_p, 16);
  mpz_set_str(key->q, secret_q, 16);
  mpz_set_str(key->a, secret_dmp1, 16);
  mpz_set_str(key->b, secret_dmq1, 16);
  mpz_set_str(key->c, secret_iqmp, 16);

  rsa_private_key_prepare(key);
}

/**
 * @brief Convert hex-encode string to plain text
 * @param[in] hexstr Hex encoded string. Must be multiple of 256.
 * @return Malloc'ed plain text string terminated with NULL char.
 */
char*
decrypt_hexstr2str(const char* hexstr)
{
	if (!hexstr)
		return NULL;

	size_t hexstr_len = strlen(hexstr);
	if (hexstr_len & 0xff)
	{
		return NULL;
	}

	struct rsa_public_key pub;
	struct rsa_private_key key;
	struct knuth_lfib_ctx lfib;

	rsa_private_key_init(&key);
	rsa_public_key_init(&pub);
	knuth_lfib_init(&lfib, 17);
	rsa_set_key(&pub, &key);

	mpz_t gibberish;
	mpz_init(gibberish);
	mpz_set_str(gibberish, hexstr, 16);

	char *decrypted = (char *)malloc(key.size);
	memset(decrypted, 0, key.size);
	int decrypted_length = key.size;
	if (1 != rsa_decrypt(&key, &decrypted_length, decrypted, gibberish))
	{
		rsa_public_key_clear(&pub);
		rsa_private_key_clear(&key);
		free(decrypted);
		return NULL;
	}
	decrypted[decrypted_length] = '\0';

	rsa_public_key_clear(&pub);
	rsa_private_key_clear(&key);

	return decrypted;
}
#endif /* HAVE_LIBGNUTLS */
