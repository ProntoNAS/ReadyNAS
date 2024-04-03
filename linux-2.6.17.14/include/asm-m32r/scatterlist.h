#ifndef _ASM_M32R_SCATTERLIST_H
#define _ASM_M32R_SCATTERLIST_H

/* $Id: scatterlist.h,v 1.1.1.1 2006-08-28 17:51:50 bzhang Exp $ */

struct scatterlist {
    char *  address;    /* Location data is to be transferred to, NULL for
                         * highmem page */
    struct page * page; /* Location for highmem page, if any */
    unsigned int offset;/* for highmem, page offset */

    dma_addr_t dma_address;
    unsigned int length;
};

#define ISA_DMA_THRESHOLD (0x1fffffff)

#endif /* _ASM_M32R_SCATTERLIST_H */
