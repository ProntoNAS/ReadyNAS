/*
 * Off-load CRC32C calculation with hardware CRC32C engine with fall-back
 *
 * Copyright (c) 2013 NETGEAR, Inc.
 * Copyright (c) 2013 Hiro Sugawara <hiro.sugawara@netgear.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 */

#include <crypto/internal/hash.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/async_tx.h>
#include <linux/mm.h>
#include <linux/kernel.h>

#define CHKSUM_BLOCK_SIZE	1
#define CHKSUM_DIGEST_SIZE	sizeof(u32)

struct dma_crc32c_ctx {
	u32 key;
};

struct dma_crc32c_desc_ctx {
	u32 crc;
};

static inline struct crypto_alg *__dma_crc32c_alg(void);
static struct crypto_shash *fb_tfm;

/*
 * Fall back to next priority algorithm.
 */
static u32 fb_crc32c(u32 crc, const u8 *data, unsigned int length)
{
	static u32 (*fb_crc32c)(u32 crc, const u8 *data, unsigned int length);

	/*
	 * Look for fall-back calculator.
	 */
	if (!fb_tfm) {
		struct shash_alg *fb_alg;

		fb_tfm = crypto_next_prio_shash(__dma_crc32c_alg());
		BUG_ON(!fb_tfm);

		/*
		 * Short-cut must have same or more relaxed alignment.
		 */
		fb_alg = crypto_shash_alg(fb_tfm);
		if (fb_alg->base.cra_alignmask <=
			__dma_crc32c_alg()->cra_alignmask)
			fb_crc32c = fb_alg->crc32c;
	}

	if (fb_crc32c)
		crc = fb_crc32c(crc, data, length);	/* Use short-cut */
	else {/* Copied from lib/libcrc32c.c */
		struct {
			struct shash_desc shash;
			char ctx[crypto_shash_descsize(fb_tfm)];
		} fb_desc;
		int err;

		fb_desc.shash.tfm = fb_tfm;
		fb_desc.shash.flags = 0;
		*(u32 *)fb_desc.ctx = crc;

		err = crypto_shash_update(&fb_desc.shash, data, length);
		BUG_ON(err);

		crc = *(u32 *)fb_desc.ctx;
	}
	return crc;
}

#ifdef CONFIG_CRYPTO_CRC32C_DMA_MIN_SIZE
#define MIN_DMA_CRC32C	CONFIG_CRYPTO_CRC32C_DMA_MIN_SIZE
#else
#define MIN_DMA_CRC32C	0
#endif

#if MIN_DMA_CRC32C == 0
#define _CONST
#else
#define _CONST const
#endif

static _CONST unsigned int min_dma_crc32c = MIN_DMA_CRC32C;

static u32 dma_crc32c(u32 crc, const u8 *data, unsigned int length)
{
	unsigned int len = length;
	struct async_submit_ctl submit;
	struct dma_chan *chan;
	struct dma_device *device;
	struct dma_async_tx_descriptor *tx;
	dma_addr_t src;

	unsigned int len2, src_offset;
	dma_addr_t src2;
	const u8 *data2;

	static int alignment_set;

	if (unlikely((unsigned long)data < PAGE_OFFSET)	||
		unlikely(high_memory <= (void *)data))
		goto done;

	init_async_submit(&submit, 0, NULL, NULL, NULL, NULL);

	chan = async_tx_find_channel(&submit, DMA_CRC32C, NULL, 0, NULL, 0, 0);
	if (!chan)
		goto done;

	device = chan->device;
	if (!device->device_prep_dma_crc32c)
		goto done;

	/* TODO: This assumes all channels share the same alignment. */
	if (unlikely(!alignment_set)) {
		__dma_crc32c_alg()->cra_alignmask =
			(1 << device->crc32c_align) - 1;
		alignment_set = 1;
	}

	len = min(length, (unsigned int)(high_memory - (void *)data));
	len &= ~__dma_crc32c_alg()->cra_alignmask;

	if (len < min_dma_crc32c)
		goto done;

	src = dma_map_single(device->dev, (void *)data, len, DMA_TO_DEVICE);

	/* Determine how many consecutive pages there are. */
	src_offset = src & ~PAGE_MASK;
	len2 = len;
	data2 = data;
	src2 = src;
	while (src_offset + len2 > PAGE_SIZE) {
		unsigned int rip = PAGE_SIZE - src_offset;

		data2 += rip;
		src2 += rip;
		len2 -= rip;
		src_offset = 0;
		if (dma_map_single(device->dev,
				(void *)data2, len2, DMA_TO_DEVICE) != src2) {
			len -= len2;
			printk("%s:%d Fragmented pages @ %p.\n",
				__func__, __LINE__, data);
			break;
		}
	}

	tx = device->device_prep_dma_crc32c(chan, src, len, &crc, 0);

	if (unlikely(tx == (struct dma_async_tx_descriptor *)1))
		goto done;	/* HW is unable to handle this context. */
	else if (unlikely(!tx)) {/* Resources are unavailable. */
		async_tx_quiesce(&submit.depend_tx);

		while (!tx) {
			dma_async_issue_pending(chan);
			tx = device->device_prep_dma_crc32c(chan, src, len,
								&crc, 0);
		}
	}

	async_tx_submit(chan, tx, &submit);
	async_tx_quiesce(&tx);
	length -= len;
	data += len;

done:
	/* Process any remainder that DMA engine could not handle. */
	if (length > 0)
		crc = fb_crc32c(crc, data, length);

	return crc;
}

static int dma_crc32c_init(struct shash_desc *desc)
{
	struct dma_crc32c_ctx *mctx = crypto_shash_ctx(desc->tfm);
	struct dma_crc32c_desc_ctx *ctx = shash_desc_ctx(desc);

	ctx->crc = mctx->key;

	return 0;
}

/*
 * Setting the seed allows arbitrary accumulators and flexible XOR policy
 * If your algorithm starts with ~0, then XOR with ~0 before you set
 * the seed.
 */
static int dma_crc32c_setkey(struct crypto_shash *tfm, const u8 *key,
			 unsigned int keylen)
{
	struct dma_crc32c_ctx *mctx = crypto_shash_ctx(tfm);

	if (keylen != sizeof(mctx->key)) {
		crypto_shash_set_flags(tfm, CRYPTO_TFM_RES_BAD_KEY_LEN);
		return -EINVAL;
	}
	mctx->key = le32_to_cpu(*(__le32 *)key);
	return 0;
}

#if MIN_DMA_CRC32C != 0
static inline void dma_crc32c_benchmark(u32 seed) {}
#else

#define BENCHMARK_TRY	1000
static u32 benchmark_one(u32 seed, void *data, unsigned int len,
			unsigned long *elapsed)
{
	struct timespec start, end;
	u32 crc = seed;
	int try = BENCHMARK_TRY;

	getnstimeofday(&start);
	while (try-- > 0) {
		crc = dma_crc32c(seed, data, len);
	}
	getnstimeofday(&end);

	if (elapsed) {
		*elapsed = (end.tv_sec - start.tv_sec) * 1000000000;
		*elapsed += end.tv_nsec;
		*elapsed -= start.tv_nsec;
		*elapsed /= BENCHMARK_TRY;
	}
	return crc;
}

#define MAX_BENCHMARK_LEN	8192
#define MIN_BENCHMARK_LEN	32
static void dma_crc32c_benchmark(u32 seed)
{
	unsigned int len;
	void *p = kmalloc(MAX_BENCHMARK_LEN, GFP_KERNEL);
	unsigned long mindns = 0;

	if (!p)
		return;

	/* Discard the first brenchmark result as it may include delayed
	 * setup overhead.
	 */
	min_dma_crc32c	= MAX_BENCHMARK_LEN * 2;
	benchmark_one(seed, p, min_dma_crc32c/2, NULL);

	for (len = MIN_BENCHMARK_LEN; len <= MAX_BENCHMARK_LEN; ) {
		unsigned long sns, dns;
		u32 scrc, dcrc;
		unsigned int inc = (len & (len - 1))	? len/3
							: len/2;
		
		min_dma_crc32c	= MAX_BENCHMARK_LEN * 2;
		scrc = benchmark_one(seed, p, len, &sns);
		min_dma_crc32c	= 1;
		dcrc = benchmark_one(seed, p, len, &dns);
		pr_info("%s: len=%d sw=%lu.%03luus (%x) dma=%lu.%03luus (%x)\n",
			__func__,
			len, sns/1000, sns%1000, scrc,
			dns/1000, dns%1000, dcrc);

		/* This is a self test, too. */
		if (scrc != dcrc) {
			pr_err("%s: HW CRC32C failed (%x vs. %x).\n",
				__func__, scrc, dcrc);
			len = (unsigned int)~0;
			goto done;
		}

		if (!mindns)
			mindns = dns;
		if (sns >= mindns)
			break;
		len += inc;
	}
done:
	kfree(p);
	min_dma_crc32c = len;
}
#endif

static int dma_crc32c_update(struct shash_desc *desc, const u8 *data,
			 unsigned int length)
{
	struct dma_crc32c_ctx *mctx = crypto_shash_ctx(desc->tfm);
	struct dma_crc32c_desc_ctx *ctx = shash_desc_ctx(desc);

	if (!min_dma_crc32c)
		dma_crc32c_benchmark(mctx->key);

	ctx->crc = dma_crc32c(ctx->crc, data, length);
	return 0;
}

static int dma_crc32c_final(struct shash_desc *desc, u8 *out)
{
	struct dma_crc32c_desc_ctx *ctx = shash_desc_ctx(desc);

	*(__le32 *)out = ~cpu_to_le32p(&ctx->crc);
	return 0;
}

static int __dma_crc32c_finup(u32 *crcp, const u8 *data, unsigned int len, u8 *out)
{
	*(__le32 *)out = ~cpu_to_le32(dma_crc32c(*crcp, data, len));
	return 0;
}

static int dma_crc32c_finup(struct shash_desc *desc, const u8 *data,
			unsigned int len, u8 *out)
{
	struct dma_crc32c_desc_ctx *ctx = shash_desc_ctx(desc);

	return __dma_crc32c_finup(&ctx->crc, data, len, out);
}

static int dma_crc32c_digest(struct shash_desc *desc, const u8 *data,
			 unsigned int length, u8 *out)
{
	struct dma_crc32c_ctx *mctx = crypto_shash_ctx(desc->tfm);

	return __dma_crc32c_finup(&mctx->key, data, length, out);
}

static int dma_crc32c_cra_init(struct crypto_tfm *tfm)
{
	struct dma_crc32c_ctx *mctx = crypto_tfm_ctx(tfm);

	mctx->key = ~0;
	return 0;
}

static struct shash_alg dma_crc32c_alg = {
	.digestsize		=	CHKSUM_DIGEST_SIZE,
	.setkey			=	dma_crc32c_setkey,
	.init   		= 	dma_crc32c_init,
	.update 		=	dma_crc32c_update,
	.final  		=	dma_crc32c_final,
	.finup  		=	dma_crc32c_finup,
	.digest  		=	dma_crc32c_digest,
	.crc32c  		=	dma_crc32c,
	.descsize		=	sizeof(struct dma_crc32c_desc_ctx),
	.base			=	{
		.cra_name		= "crc32c",
		.cra_driver_name	= "crc32c-dma",
		.cra_priority		= 300,	/* Higher than generic's */
		.cra_blocksize		= CHKSUM_BLOCK_SIZE,
		.cra_alignmask		= 3,	/* temporary */
		.cra_ctxsize		= sizeof(u32),
		.cra_module		= THIS_MODULE,
		.cra_init		= dma_crc32c_cra_init,
	}
};

static inline struct crypto_alg *__dma_crc32c_alg(void)
{
	return &dma_crc32c_alg.base;
}

static int __init dma_crc32c_mod_init(void)
{
	return crypto_register_shash(&dma_crc32c_alg);
}

static void __exit dma_crc32c_mod_fini(void)
{
	if (fb_tfm)
		crypto_free_shash(fb_tfm);
	crypto_unregister_shash(&dma_crc32c_alg);
}

module_init(dma_crc32c_mod_init);
module_exit(dma_crc32c_mod_fini);

MODULE_AUTHOR("Clay Haapala <chaapala@cisco.com>");
MODULE_AUTHOR("Hiro Sugawara <hiro.sugawara@netgear.com>");
MODULE_DESCRIPTION("CRC32c (Castagnoli) calculations w/ HW CRC engine");
MODULE_LICENSE("GPL");
