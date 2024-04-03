/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* lib/crypto/openssl/enc_provider/des.c
 *
 * Copyright (C) 2009 by the Massachusetts Institute of Technology.
 * All rights reserved.
 *
 * Export of this software from the United States of America may
 *   require a specific license from the United States Government.
 *   It is the responsibility of any person or organization contemplating
 *   export to obtain such a license before exporting.
 *
 * WITHIN THAT CONSTRAINT, permission to use, copy, modify, and
 * distribute this software and its documentation for any purpose and
 * without fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation, and that
 * the name of M.I.T. not be used in advertising or publicity pertaining
 * to distribution of the software without specific, written prior
 * permission.  Furthermore if you modify this software you must label
 * your software as modified software and not distribute it in such a
 * fashion that it might be confused with the original M.I.T. software.
 * M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 */

/*
 * Copyright (C) 1998 by the FundsXpress, INC.
 *
 * All rights reserved.
 *
 * Export of this software from the United States of America may require
 * a specific license from the United States Government.  It is the
 * responsibility of any person or organization contemplating export to
 * obtain such a license before exporting.
 *
 * WITHIN THAT CONSTRAINT, permission to use, copy, modify, and
 * distribute this software and its documentation for any purpose and
 * without fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation, and that
 * the name of FundsXpress. not be used in advertising or publicity pertaining
 * to distribution of the software without specific, written prior
 * permission.  FundsXpress makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "k5-int.h"
#include <aead.h>
#include <rand2key.h>
#include <openssl/evp.h>
#include "des_int.h"

#define DES_BLOCK_SIZE  8
#define DES_KEY_BYTES   7

static krb5_error_code
validate(krb5_key key, const krb5_data *ivec, const krb5_crypto_iov *data,
         size_t num_data, krb5_boolean *empty)
{
    size_t i, input_length;

    for (i = 0, input_length = 0; i < num_data; i++) {
        const krb5_crypto_iov *iov = &data[i];
        if (ENCRYPT_IOV(iov))
            input_length += iov->data.length;
    }

    if (key->keyblock.length != KRB5_MIT_DES_KEYSIZE)
        return(KRB5_BAD_KEYSIZE);
    if ((input_length%DES_BLOCK_SIZE) != 0)
        return(KRB5_BAD_MSIZE);
    if (ivec && (ivec->length != 8))
        return(KRB5_BAD_MSIZE);

    *empty = (input_length == 0);
    return 0;
}

static krb5_error_code
k5_des_encrypt(krb5_key key, const krb5_data *ivec, krb5_crypto_iov *data,
               size_t num_data)
{
    int ret, olen = MIT_DES_BLOCK_LENGTH;
    unsigned char iblock[MIT_DES_BLOCK_LENGTH], oblock[MIT_DES_BLOCK_LENGTH];
    struct iov_block_state input_pos, output_pos;
    EVP_CIPHER_CTX ciph_ctx;
    krb5_boolean empty;

    IOV_BLOCK_STATE_INIT(&input_pos);
    IOV_BLOCK_STATE_INIT(&output_pos);

    ret = validate(key, ivec, data, num_data, &empty);
    if (ret != 0 || empty)
        return ret;

    EVP_CIPHER_CTX_init(&ciph_ctx);

    ret = EVP_EncryptInit_ex(&ciph_ctx, EVP_des_cbc(), NULL,
                             key->keyblock.contents, (ivec && ivec->data) ? (unsigned char*)ivec->data : NULL);
    if (!ret)
        return KRB5_CRYPTO_INTERNAL;

    EVP_CIPHER_CTX_set_padding(&ciph_ctx,0);

    for (;;) {

        if (!krb5int_c_iov_get_block(iblock, MIT_DES_BLOCK_LENGTH, data,
                                     num_data, &input_pos))
            break;

        ret = EVP_EncryptUpdate(&ciph_ctx, oblock, &olen,
                                (unsigned char *)iblock, MIT_DES_BLOCK_LENGTH);
        if (!ret)
            break;

        krb5int_c_iov_put_block(data, num_data, oblock, MIT_DES_BLOCK_LENGTH,
                                &output_pos);
    }

    if (ivec != NULL)
        memcpy(ivec->data, oblock, MIT_DES_BLOCK_LENGTH);

    EVP_CIPHER_CTX_cleanup(&ciph_ctx);

    zap(iblock, sizeof(iblock));
    zap(oblock, sizeof(oblock));

    if (ret != 1)
        return KRB5_CRYPTO_INTERNAL;
    return 0;
}

static krb5_error_code
k5_des_decrypt(krb5_key key, const krb5_data *ivec, krb5_crypto_iov *data,
               size_t num_data)
{
    int ret, olen = MIT_DES_BLOCK_LENGTH;
    unsigned char iblock[MIT_DES_BLOCK_LENGTH], oblock[MIT_DES_BLOCK_LENGTH];
    struct iov_block_state input_pos, output_pos;
    EVP_CIPHER_CTX ciph_ctx;
    krb5_boolean empty;

    IOV_BLOCK_STATE_INIT(&input_pos);
    IOV_BLOCK_STATE_INIT(&output_pos);

    ret = validate(key, ivec, data, num_data, &empty);
    if (ret != 0 || empty)
        return ret;

    EVP_CIPHER_CTX_init(&ciph_ctx);

    ret = EVP_DecryptInit_ex(&ciph_ctx, EVP_des_cbc(), NULL,
                             key->keyblock.contents,
                             (ivec) ? (unsigned char*)ivec->data : NULL);
    if (!ret)
        return KRB5_CRYPTO_INTERNAL;

    EVP_CIPHER_CTX_set_padding(&ciph_ctx,0);

    for (;;) {

        if (!krb5int_c_iov_get_block(iblock, MIT_DES_BLOCK_LENGTH,
                                     data, num_data, &input_pos))
            break;

        ret = EVP_DecryptUpdate(&ciph_ctx, oblock, &olen,
                                iblock, MIT_DES_BLOCK_LENGTH);
        if (!ret) break;

        krb5int_c_iov_put_block(data, num_data, oblock,
                                MIT_DES_BLOCK_LENGTH, &output_pos);
    }

    if (ivec != NULL)
        memcpy(ivec->data, iblock, MIT_DES_BLOCK_LENGTH);

    EVP_CIPHER_CTX_cleanup(&ciph_ctx);

    zap(iblock, sizeof(iblock));
    zap(oblock, sizeof(oblock));

    if (ret != 1)
        return KRB5_CRYPTO_INTERNAL;
    return 0;
}

const struct krb5_enc_provider krb5int_enc_des = {
    DES_BLOCK_SIZE,
    DES_KEY_BYTES, KRB5_MIT_DES_KEYSIZE,
    k5_des_encrypt,
    k5_des_decrypt,
    NULL,
    krb5int_des_make_key,
    krb5int_des_init_state,
    krb5int_default_free_state
};
