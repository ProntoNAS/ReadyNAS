/*
 * Copyright (c) 2009-2015 Douglas Gilbert.
 * All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the BSD_LICENSE file.
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#define __STDC_FORMAT_MACROS 1
#include <inttypes.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "sg_lib.h"
#include "sg_pt.h"
#include "sg_cmds_basic.h"
#include "sg_cmds_extra.h"
#include "sg_unaligned.h"
#include "sg_pr2serr.h"

static const char * version_str = "1.11 20151220";


#define ME "sg_write_same: "

#define WRITE_SAME10_OP 0x41
#define WRITE_SAME16_OP 0x93
#define VARIABLE_LEN_OP 0x7f
#define WRITE_SAME32_SA 0xd
#define WRITE_SAME32_ADD 0x18
#define WRITE_SAME10_LEN 10
#define WRITE_SAME16_LEN 16
#define WRITE_SAME32_LEN 32
#define RCAP10_RESP_LEN 8
#define RCAP16_RESP_LEN 32
#define SENSE_BUFF_LEN 64       /* Arbitrary, could be larger */
#define DEF_TIMEOUT_SECS 60
#define DEF_WS_CDB_SIZE WRITE_SAME10_LEN
#define DEF_WS_NUMBLOCKS 1
#define MAX_XFER_LEN (64 * 1024)
#define EBUFF_SZ 256

static struct option long_options[] = {
    {"10", no_argument, 0, 'R'},
    {"16", no_argument, 0, 'S'},
    {"32", no_argument, 0, 'T'},
    {"anchor", no_argument, 0, 'a'},
    {"grpnum", required_argument, 0, 'g'},
    {"help", no_argument, 0, 'h'},
    {"in", required_argument, 0, 'i'},
    {"lba", required_argument, 0, 'l'},
    {"lbdata", no_argument, 0, 'L'},
    {"ndob", no_argument, 0, 'N'},
    {"num", required_argument, 0, 'n'},
    {"pbdata", no_argument, 0, 'P'},
    {"timeout", required_argument, 0, 'r'},
    {"unmap", no_argument, 0, 'U'},
    {"verbose", no_argument, 0, 'v'},
    {"version", no_argument, 0, 'V'},
    {"wrprotect", required_argument, 0, 'w'},
    {"xferlen", required_argument, 0, 'x'},
    {0, 0, 0, 0},
};

struct opts_t {
    int anchor;
    int grpnum;
    char ifilename[256];
    uint64_t lba;
    int lbdata;
    int ndob;
    int numblocks;
    int pbdata;
    int timeout;
    int unmap;
    int verbose;
    int wrprotect;
    int xfer_len;
    int pref_cdb_size;
    int want_ws10;
};



static void
usage()
{
    pr2serr("Usage: sg_write_same [--10] [--16] [--32] [--anchor] "
            "[--grpnum=GN] [--help]\n"
            "                     [--in=IF] [--lba=LBA] [--lbdata] "
            "[--ndob] [--num=NUM]\n"
            "                     [--pbdata] [--timeout=TO] [--unmap] "
            "[--verbose]\n"
            "                     [--version] [--wrprotect=WRP] "
            "[xferlen=LEN]\n"
            "                     DEVICE\n"
            "  where:\n"
            "    --10|-R              do WRITE SAME(10) (even if '--unmap' "
            "is given)\n"
            "    --16|-S              do WRITE SAME(16) (def: 10 unless "
            "'--unmap' given,\n"
            "                         LBA+NUM > 32 bits, or NUM > 65535; "
            "then def 16)\n"
            "    --32|-T              do WRITE SAME(32) (def: 10 or 16)\n"
            "    --anchor|-a          set anchor field in cdb\n"
            "    --grpnum=GN|-g GN    GN is group number field (def: 0)\n"
            "    --help|-h            print out usage message\n"
            "    --in=IF|-i IF        IF is file to fetch one block of data "
            "from (use LEN\n"
            "                         bytes or whole file). Block written to "
            "DEVICE\n"
            "    --lba=LBA|-l LBA     LBA is the logical block address to "
            "start (def: 0)\n"
            "    --lbdata|-L          set LBDATA bit (obsolete)\n"
            "    --ndob|-N            set 'no data-out buffer' bit\n"
            "    --num=NUM|-n NUM     NUM is number of logical blocks to "
            "write (def: 1)\n"
            "                         [Beware NUM==0 may mean rest of "
            "device]\n"
            "    --pbdata|-P          set PBDATA bit (obsolete)\n"
            "    --timeout=TO|-t TO    command timeout (unit: seconds) (def: "
            "60)\n"
            "    --unmap|-U           set UNMAP bit\n"
            "    --verbose|-v         increase verbosity\n"
            "    --version|-V         print version string then exit\n"
            "    --wrprotect=WPR|-w WPR    WPR is the WRPROTECT field value "
            "(def: 0)\n"
            "    --xferlen=LEN|-x LEN    LEN is number of bytes from IF to "
            "send to\n"
            "                            DEVICE (def: IF file length)\n\n"
            "Performs a SCSI WRITE SAME (10, 16 or 32) command\n"
            );
}

static int
do_write_same(int sg_fd, const struct opts_t * op, const void * dataoutp,
              int * act_cdb_lenp)
{
    int k, ret, res, sense_cat, cdb_len;
    uint64_t llba;
    unsigned char wsCmdBlk[WRITE_SAME32_LEN];
    unsigned char sense_b[SENSE_BUFF_LEN];
    struct sg_pt_base * ptvp;

    cdb_len = op->pref_cdb_size;
    if (WRITE_SAME10_LEN == cdb_len) {
        llba = op->lba + op->numblocks;
        if ((op->numblocks > 0xffff) || (llba > ULONG_MAX) ||
            op->ndob || (op->unmap && (0 == op->want_ws10))) {
            cdb_len = WRITE_SAME16_LEN;
            if (op->verbose) {
                const char * cp = "use WRITE SAME(16) instead of 10 byte "
                                  "cdb";

                if (op->numblocks > 0xffff)
                    pr2serr("%s since blocks exceed 65535\n", cp);
                else if (llba > ULONG_MAX)
                    pr2serr("%s since LBA may exceed 32 bits\n", cp);
                else
                    pr2serr("%s due to ndob or unmap settings\n", cp);
            }
        }
    }
    if (act_cdb_lenp)
        *act_cdb_lenp = cdb_len;
    memset(wsCmdBlk, 0, sizeof(wsCmdBlk));
    switch (cdb_len) {
    case WRITE_SAME10_LEN:
        wsCmdBlk[0] = WRITE_SAME10_OP;
        wsCmdBlk[1] = ((op->wrprotect & 0x7) << 5);
        /* ANCHOR + UNMAP not allowed for WRITE_SAME10 in sbc3r24+r25 but
         * a proposal has been made to allow it. Anticipate approval. */
        if (op->anchor)
            wsCmdBlk[1] |= 0x10;
        if (op->unmap)
            wsCmdBlk[1] |= 0x8;
        if (op->pbdata)
            wsCmdBlk[1] |= 0x4;
        if (op->lbdata)
            wsCmdBlk[1] |= 0x2;
        sg_put_unaligned_be32((uint32_t)op->lba, wsCmdBlk + 2);
        wsCmdBlk[6] = (op->grpnum & 0x1f);
        sg_put_unaligned_be16((uint16_t)op->numblocks, wsCmdBlk + 7);
        break;
    case WRITE_SAME16_LEN:
        wsCmdBlk[0] = WRITE_SAME16_OP;
        wsCmdBlk[1] = ((op->wrprotect & 0x7) << 5);
        if (op->anchor)
            wsCmdBlk[1] |= 0x10;
        if (op->unmap)
            wsCmdBlk[1] |= 0x8;
        if (op->pbdata)
            wsCmdBlk[1] |= 0x4;
        if (op->lbdata)
            wsCmdBlk[1] |= 0x2;
        if (op->ndob)
            wsCmdBlk[1] |= 0x1;
        sg_put_unaligned_be64(op->lba, wsCmdBlk + 2);
        sg_put_unaligned_be32((uint32_t)op->numblocks, wsCmdBlk + 10);
        wsCmdBlk[14] = (op->grpnum & 0x1f);
        break;
    case WRITE_SAME32_LEN:
        /* Note: In Linux at this time the sg driver does not support
         * cdb_s > 16 bytes long, but the bsg driver does. */
        wsCmdBlk[0] = VARIABLE_LEN_OP;
        wsCmdBlk[6] = (op->grpnum & 0x1f);
        wsCmdBlk[7] = WRITE_SAME32_ADD;
        sg_put_unaligned_be16((uint16_t)WRITE_SAME32_SA, wsCmdBlk + 8);
        wsCmdBlk[10] = ((op->wrprotect & 0x7) << 5);
        if (op->anchor)
            wsCmdBlk[10] |= 0x10;
        if (op->unmap)
            wsCmdBlk[10] |= 0x8;
        if (op->pbdata)
            wsCmdBlk[10] |= 0x4;
        if (op->lbdata)
            wsCmdBlk[10] |= 0x2;
        if (op->ndob)
            wsCmdBlk[10] |= 0x1;
        sg_put_unaligned_be64(op->lba, wsCmdBlk + 12);
        sg_put_unaligned_be32((uint32_t)op->numblocks, wsCmdBlk + 28);
        break;
    default:
        pr2serr("do_write_same: bad cdb length %d\n", cdb_len);
        return -1;
    }

    if (op->verbose > 1) {
        pr2serr("    Write same(%d) cmd: ", cdb_len);
        for (k = 0; k < cdb_len; ++k)
            pr2serr("%02x ", wsCmdBlk[k]);
        pr2serr("\n    Data-out buffer length=%d\n",
                op->xfer_len);
    }
    if ((op->verbose > 3) && (op->xfer_len > 0)) {
        pr2serr("    Data-out buffer contents:\n");
        dStrHexErr((const char *)dataoutp, op->xfer_len, 1);
    }
    ptvp = construct_scsi_pt_obj();
    if (NULL == ptvp) {
        pr2serr("Write same(%d): out of memory\n", cdb_len);
        return -1;
    }
    set_scsi_pt_cdb(ptvp, wsCmdBlk, cdb_len);
    set_scsi_pt_sense(ptvp, sense_b, sizeof(sense_b));
    set_scsi_pt_data_out(ptvp, (unsigned char *)dataoutp, op->xfer_len);
    res = do_scsi_pt(ptvp, sg_fd, op->timeout, op->verbose);
    ret = sg_cmds_process_resp(ptvp, "Write same", res, 0, sense_b,
                               1 /*noisy */, op->verbose, &sense_cat);
    if (-1 == ret)
        ;
    else if (-2 == ret) {
        switch (sense_cat) {
        case SG_LIB_CAT_RECOVERED:
        case SG_LIB_CAT_NO_SENSE:
            ret = 0;
            break;
        case SG_LIB_CAT_MEDIUM_HARD:
            {
                int valid, slen;
                uint64_t ull = 0;

                slen = get_scsi_pt_sense_len(ptvp);
                valid = sg_get_sense_info_fld(sense_b, slen, &ull);
                if (valid)
                    pr2serr("Medium or hardware error starting at lba=%"
                            PRIu64 " [0x%" PRIx64 "]\n", ull, ull);
            }
            ret = sense_cat;
            break;
        default:
            ret = sense_cat;
            break;
        }
    } else
        ret = 0;

    destruct_scsi_pt_obj(ptvp);
    return ret;
}


int
main(int argc, char * argv[])
{
    int sg_fd, res, c, infd, prot_en, act_cdb_len, vb;
    int num_given = 0;
    int lba_given = 0;
    int if_given = 0;
    int got_stdin = 0;
    int64_t ll;
    uint32_t block_size;
    const char * device_name = NULL;
    char ebuff[EBUFF_SZ];
    char b[80];
    unsigned char resp_buff[RCAP16_RESP_LEN];
    unsigned char * wBuff = NULL;
    int ret = -1;
    struct opts_t opts;
    struct opts_t * op;
    struct stat a_stat;

    op = &opts;
    memset(op, 0, sizeof(opts));
    op->numblocks = DEF_WS_NUMBLOCKS;
    op->pref_cdb_size = DEF_WS_CDB_SIZE;
    op->timeout = DEF_TIMEOUT_SECS;
    vb = 0;
    while (1) {
        int option_index = 0;

        c = getopt_long(argc, argv, "ag:hi:l:Ln:NPRSt:TUvVw:x:",
                        long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
        case 'a':
            ++op->anchor;
            break;
        case 'g':
            op->grpnum = sg_get_num(optarg);
            if ((op->grpnum < 0) || (op->grpnum > 31))  {
                pr2serr("bad argument to '--grpnum'\n");
                return SG_LIB_SYNTAX_ERROR;
            }
            break;
        case 'h':
        case '?':
            usage();
            return 0;
        case 'i':
            strncpy(op->ifilename, optarg, sizeof(op->ifilename));
            if_given = 1;
            break;
        case 'l':
            ll = sg_get_llnum(optarg);
            if (-1 == ll) {
                pr2serr("bad argument to '--lba'\n");
                return SG_LIB_SYNTAX_ERROR;
            }
            op->lba = (uint64_t)ll;
            lba_given = 1;
            break;
        case 'L':
            ++op->lbdata;
            break;
        case 'n':
            op->numblocks = sg_get_num(optarg);
            if (op->numblocks < 0)  {
                pr2serr("bad argument to '--num'\n");
                return SG_LIB_SYNTAX_ERROR;
            }
            num_given = 1;
            break;
        case 'N':
            ++op->ndob;
            break;
        case 'P':
            ++op->pbdata;
            break;
        case 'R':
            ++op->want_ws10;
            break;
        case 'S':
            if (DEF_WS_CDB_SIZE != op->pref_cdb_size) {
                pr2serr("only one '--10', '--16' or '--32' please\n");
                return SG_LIB_SYNTAX_ERROR;
            }
            op->pref_cdb_size = 16;
            break;
        case 't':
            op->timeout = sg_get_num(optarg);
            if (op->timeout < 0)  {
                pr2serr("bad argument to '--timeout'\n");
                return SG_LIB_SYNTAX_ERROR;
            }
            break;
        case 'T':
            if (DEF_WS_CDB_SIZE != op->pref_cdb_size) {
                pr2serr("only one '--10', '--16' or '--32' please\n");
                return SG_LIB_SYNTAX_ERROR;
            }
            op->pref_cdb_size = 32;
            break;
        case 'U':
            ++op->unmap;
            break;
        case 'v':
            ++op->verbose;
            break;
        case 'V':
            pr2serr(ME "version: %s\n", version_str);
            return 0;
        case 'w':
            op->wrprotect = sg_get_num(optarg);
            if ((op->wrprotect < 0) || (op->wrprotect > 7))  {
                pr2serr("bad argument to '--wrprotect'\n");
                return SG_LIB_SYNTAX_ERROR;
            }
            break;
        case 'x':
            op->xfer_len = sg_get_num(optarg);
            if (op->xfer_len < 0) {
                pr2serr("bad argument to '--xferlen'\n");
                return SG_LIB_SYNTAX_ERROR;
            }
            break;
        default:
            pr2serr("unrecognised option code 0x%x ??\n", c);
            usage();
            return SG_LIB_SYNTAX_ERROR;
        }
    }
    if (optind < argc) {
        if (NULL == device_name) {
            device_name = argv[optind];
            ++optind;
        }
        if (optind < argc) {
            for (; optind < argc; ++optind)
                pr2serr("Unexpected extra argument: %s\n", argv[optind]);
            usage();
            return SG_LIB_SYNTAX_ERROR;
        }
    }
    if (op->want_ws10 && (DEF_WS_CDB_SIZE != op->pref_cdb_size)) {
        pr2serr("only one '--10', '--16' or '--32' please\n");
        return SG_LIB_SYNTAX_ERROR;
    }
    if (NULL == device_name) {
        pr2serr("missing device name!\n");
        usage();
        return SG_LIB_SYNTAX_ERROR;
    }
    vb = op->verbose;

    if ((! if_given) && (! lba_given) && (! num_given)) {
        pr2serr("As a precaution, one of '--in=', '--lba=' or '--num=' is "
                "required\n");
        return SG_LIB_SYNTAX_ERROR;
    }

    if (op->ndob) {
        if (if_given) {
            pr2serr("Can't have both --ndob and '--in='\n");
            return SG_LIB_SYNTAX_ERROR;
        }
        if (0 != op->xfer_len) {
            pr2serr("With --ndob only '--xferlen=0' (or not given) is "
                    "acceptable\n");
            return SG_LIB_SYNTAX_ERROR;
        }
    } else if (op->ifilename[0]) {
        got_stdin = (0 == strcmp(op->ifilename, "-")) ? 1 : 0;
        if (! got_stdin) {
            memset(&a_stat, 0, sizeof(a_stat));
            if (stat(op->ifilename, &a_stat) < 0) {
                if (vb)
                    pr2serr("unable to stat(%s): %s\n", op->ifilename,
                            safe_strerror(errno));
                return SG_LIB_FILE_ERROR;
            }
            if (op->xfer_len <= 0)
                op->xfer_len = (int)a_stat.st_size;
        }
    }

    sg_fd = sg_cmds_open_device(device_name, 0 /* rw */, vb);
    if (sg_fd < 0) {
        pr2serr(ME "open error: %s: %s\n", device_name, safe_strerror(-sg_fd));
        return SG_LIB_FILE_ERROR;
    }

    if (! op->ndob) {
        prot_en = 0;
        if (0 == op->xfer_len) {
            res = sg_ll_readcap_16(sg_fd, 0 /* pmi */, 0 /* llba */, resp_buff,
                                   RCAP16_RESP_LEN, 1, (vb ? (vb - 1): 0));
            if (SG_LIB_CAT_UNIT_ATTENTION == res) {
                pr2serr("Read capacity(16) unit attention, try again\n");
                res = sg_ll_readcap_16(sg_fd, 0, 0, resp_buff,
                                       RCAP16_RESP_LEN, 1, (vb ? (vb - 1): 0));
            }
            if (0 == res) {
                if (vb > 3)
                    dStrHexErr((const char *)resp_buff, RCAP16_RESP_LEN, 1);
                block_size = sg_get_unaligned_be32(resp_buff + 8);
                prot_en = !!(resp_buff[12] & 0x1);
                op->xfer_len = block_size;
                if (prot_en && (op->wrprotect > 0))
                    op->xfer_len += 8;
            } else if ((SG_LIB_CAT_INVALID_OP == res) ||
                       (SG_LIB_CAT_ILLEGAL_REQ == res)) {
                if (vb)
                    pr2serr("Read capacity(16) not supported, try Read "
                            "capacity(10)\n");
                res = sg_ll_readcap_10(sg_fd, 0 /* pmi */, 0 /* lba */,
                                       resp_buff, RCAP10_RESP_LEN, 1,
                                       (vb ? (vb - 1): 0));
                if (0 == res) {
                    if (vb > 3)
                        dStrHexErr((const char *)resp_buff, RCAP10_RESP_LEN,
                                   1);
                    block_size = sg_get_unaligned_be32(resp_buff + 4);
                    op->xfer_len = block_size;
                } else {
                    sg_get_category_sense_str(res, sizeof(b), b, vb);
                    pr2serr("Read capacity(10): %s\n", b);
                    pr2serr("Unable to calculate block size\n");
                }
            } else if (vb) {
                sg_get_category_sense_str(res, sizeof(b), b, vb);
                pr2serr("Read capacity(16): %s\n", b);
                pr2serr("Unable to calculate block size\n");
            }
        }
        if (op->xfer_len < 1) {
            pr2serr("unable to deduce block size, please give '--xferlen=' "
                    "argument\n");
            ret = SG_LIB_SYNTAX_ERROR;
            goto err_out;
        }
        if (op->xfer_len > MAX_XFER_LEN) {
            pr2serr("'--xferlen=%d is out of range ( want <= %d)\n",
                    op->xfer_len, MAX_XFER_LEN);
            ret = SG_LIB_SYNTAX_ERROR;
            goto err_out;
        }
        wBuff = (unsigned char*)calloc(op->xfer_len, 1);
        if (NULL == wBuff) {
            pr2serr("unable to allocate %d bytes of memory with calloc()\n",
                    op->xfer_len);
            ret = SG_LIB_SYNTAX_ERROR;
            goto err_out;
        }
        if (op->ifilename[0]) {
            if (got_stdin) {
                infd = STDIN_FILENO;
                if (sg_set_binary_mode(STDIN_FILENO) < 0)
                    perror("sg_set_binary_mode");
            } else {
                if ((infd = open(op->ifilename, O_RDONLY)) < 0) {
                    snprintf(ebuff, EBUFF_SZ, ME "could not open %s for "
                             "reading", op->ifilename);
                    perror(ebuff);
                    ret = SG_LIB_FILE_ERROR;
                    goto err_out;
                } else if (sg_set_binary_mode(infd) < 0)
                    perror("sg_set_binary_mode");
            }
            res = read(infd, wBuff, op->xfer_len);
            if (res < 0) {
                snprintf(ebuff, EBUFF_SZ, ME "couldn't read from %s",
                         op->ifilename);
                perror(ebuff);
                if (! got_stdin)
                    close(infd);
                ret = SG_LIB_FILE_ERROR;
                goto err_out;
            }
            if (res < op->xfer_len) {
                pr2serr("tried to read %d bytes from %s, got %d bytes\n",
                        op->xfer_len, op->ifilename, res);
                pr2serr("  so pad with 0x0 bytes and continue\n");
            }
            if (! got_stdin)
                close(infd);
        } else {
            if (vb)
                pr2serr("Default data-out buffer set to %d zeros\n",
                        op->xfer_len);
            if (prot_en && (op->wrprotect > 0)) {
               /* default for protection is 0xff, rest get 0x0 */
                memset(wBuff + op->xfer_len - 8, 0xff, 8);
                if (vb)
                    pr2serr(" ... apart from last 8 bytes which are set to "
                            "0xff\n");
            }
        }
    }

    ret = do_write_same(sg_fd, op, wBuff, &act_cdb_len);
    if (ret) {
        sg_get_category_sense_str(ret, sizeof(b), b, vb);
        pr2serr("Write same(%d): %s\n", act_cdb_len, b);
    }

err_out:
    if (wBuff)
        free(wBuff);
    res = sg_cmds_close_device(sg_fd);
    if (res < 0) {
        pr2serr("close error: %s\n", safe_strerror(-res));
        if (0 == ret)
            return SG_LIB_FILE_ERROR;
    }
    return (ret >= 0) ? ret : SG_LIB_CAT_OTHER;
}
