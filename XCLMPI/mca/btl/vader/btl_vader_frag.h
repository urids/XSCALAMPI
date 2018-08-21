/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2013 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2008      Sun Microsystems, Inc.  All rights reserved.
 * Copyright (c) 2009      Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2011-2013 Los Alamos National Security, LLC. All rights
 *                         reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef MCA_BTL_VADER_SEND_FRAG_H
#define MCA_BTL_VADER_SEND_FRAG_H

#include "ompi_config.h"

enum {
    MCA_BTL_VADER_FLAG_INLINE      = 0,
    MCA_BTL_VADER_FLAG_SINGLE_COPY = 1,
    MCA_BTL_VADER_FLAG_COMPLETE    = 2,
};

struct mca_btl_vader_frag_t;
struct mca_btl_vader_fbox_t;

struct mca_btl_vader_hdr_t {
    volatile intptr_t next; /* next item in fifo. many peers may touch this */
    struct mca_btl_vader_frag_t *frag;
    mca_btl_base_tag_t tag; /* tag associated with this fragment (used to lookup callback) */
    uint8_t flags;          /* vader send flags */
    uint16_t seqn;
    int32_t len;            /* length of data following this header */
    struct iovec sc_iov;    /* io vector containing pointer to single-copy data */
};
typedef struct mca_btl_vader_hdr_t mca_btl_vader_hdr_t;

/**
 * shared memory send fragment derived type.
 */
struct mca_btl_vader_frag_t {
    mca_btl_base_descriptor_t base;
    mca_btl_base_segment_t segments[2];
    struct mca_btl_base_endpoint_t *endpoint;
    struct mca_btl_vader_fbox_t *fbox;
    mca_btl_vader_hdr_t *hdr; /* in the shared memory region */
    ompi_free_list_t *my_list;
};

typedef struct mca_btl_vader_frag_t mca_btl_vader_frag_t;

static inline int mca_btl_vader_frag_alloc (mca_btl_vader_frag_t **frag, ompi_free_list_t *list,
                              struct mca_btl_base_endpoint_t *endpoint) {
    ompi_free_list_item_t *item;

    OMPI_FREE_LIST_GET_MT(list, item);
    *frag = (mca_btl_vader_frag_t *) item;
    if (OPAL_LIKELY(NULL != item)) {
        if (NULL == (*frag)->hdr) {
            OMPI_FREE_LIST_RETURN_MT(list, (ompi_free_list_item_t *)*frag);
            *frag = NULL;
            return OMPI_ERR_TEMP_OUT_OF_RESOURCE;
        }

        (*frag)->endpoint = endpoint;
    }

    return OMPI_SUCCESS;
}

static inline void mca_btl_vader_frag_return (mca_btl_vader_frag_t *frag)
{
    frag->hdr->flags = 0;
    frag->segments[0].seg_addr.pval = (char *)(frag->hdr + 1);
    frag->base.des_src     = frag->segments;
    frag->base.des_src_cnt = 1;
    frag->base.des_dst     = frag->segments;
    frag->base.des_dst_cnt = 1;
    frag->fbox = NULL;

    OMPI_FREE_LIST_RETURN_MT(frag->my_list, (ompi_free_list_item_t *)frag);
}

OBJ_CLASS_DECLARATION(mca_btl_vader_frag_t);

#define MCA_BTL_VADER_FRAG_ALLOC_EAGER(frag, endpoint)                  \
    mca_btl_vader_frag_alloc (&(frag), &mca_btl_vader_component.vader_frags_eager, endpoint)

#if !OMPI_BTL_VADER_HAVE_XPMEM
#define MCA_BTL_VADER_FRAG_ALLOC_MAX(frag, endpoint)                    \
    mca_btl_vader_frag_alloc (&(frag), &mca_btl_vader_component.vader_frags_max_send, endpoint)
#endif

#define MCA_BTL_VADER_FRAG_ALLOC_USER(frag, endpoint)                   \
    mca_btl_vader_frag_alloc (&(frag), &mca_btl_vader_component.vader_frags_user, endpoint)

#define MCA_BTL_VADER_FRAG_RETURN(frag) mca_btl_vader_frag_return(frag)


static inline void mca_btl_vader_frag_complete (mca_btl_vader_frag_t *frag) {
    if (OPAL_UNLIKELY(MCA_BTL_DES_SEND_ALWAYS_CALLBACK & frag->base.des_flags)) {
        /* completion callback */
        frag->base.des_cbfunc(&mca_btl_vader.super, frag->endpoint,
                              &frag->base, OMPI_SUCCESS);
    }

    if (OPAL_LIKELY(frag->base.des_flags & MCA_BTL_DES_FLAGS_BTL_OWNERSHIP)) {
        MCA_BTL_VADER_FRAG_RETURN(frag);
    }
}

void mca_btl_vader_frag_init (ompi_free_list_item_t *item, void *ctx);

#endif /* MCA_BTL_VADER_SEND_FRAG_H */
