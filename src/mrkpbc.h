#ifndef MRKPBC_H
#define MRKPBC_H

#include <stdio.h>

#include <mrkcommon/array.h>
#include <mrkcommon/hash.h>
#include <mrkcommon/bytes.h>

#ifdef __cplusplus
extern "C" {
#endif


#define MRKPB_WT_UNDEF   (-1)
#define MRKPB_WT_VARINT  (0)
#define MRKPB_WT_64BIT   (1)
#define MRKPB_WT_LDELIM  (2)
#define MRKPB_WT_SGRP    (3)
#define MRKPB_WT_EGRP    (4)
#define MRKPB_WT_32BIT   (5)
#define MRKPB_WT_INTERN  (6)

#define MRKPB_MAKEKEY(wtype, fnum) (((fnum)<<3)|wtype)

#define MRKPB_WT_NUMERIC(wtype)        \
    ((wtype) == MRKPB_WT_VARINT ||     \
     (wtype) == MRKPB_WT_64BIT ||      \
     (wtype) == MRKPB_WT_32BIT)        \

#define MRKPB_WT_CHAR(wtype)           \
    ((wtype) == MRKPB_WT_VARINT ? "V" :\
     (wtype) == MRKPB_WT_64BIT ? "8" : \
     (wtype) == MRKPB_WT_LDELIM ? "L" :\
     (wtype) == MRKPB_WT_32BIT ? "4" : \
     "")                               \

#ifndef MRKPB_MAX_BYTES
#   define MRKPB_MAX_BYTES (0x100000)
#endif


struct _mrkpbc_container;
struct _mrkpbc_ctx;

typedef struct _mrkpbc_field {
    /* weakref */
    struct _mrkpbc_container *parent;
    mnbytes_t *ty;
    /* weakref */
    struct _mrkpbc_container *cty;
    struct {
        mnbytes_t *name;
        mnbytes_t *fqname;
    } pb;
    struct {
        mnbytes_t *name;
        mnbytes_t *fqname;
    } be;
    /* field number */
#define MRKPBC_FNUM_ONEOF (-1l)
    int64_t fnum;
    int wtype;
    struct {
        int repeated:1;
    } flags;
} mrkpbc_field_t;

typedef struct _mrkpbc_container {
    /* weakref */
    struct _mrkpbc_ctx *ctx;

    /* weakref */
    struct _mrkpbc_container *parent;

    struct {
        mnbytes_t *name;
        mnbytes_t *fqname;
    } pb;
    struct {
        mnbytes_t *name;
        mnbytes_t *fqname;
        mnbytes_t *encode;
        mnbytes_t *decode;
        mnbytes_t *sz;
        mnbytes_t *rawsz;
        mnbytes_t *dump;
    } be;

    /* weakref mrkpbc_field_t * */
    mnarray_t fields;

    /* weakref mrkpbc_container_t * */
    mnarray_t containers;

#define MRKPBC_CONT_KBUILTIN    0
#define MRKPBC_CONT_KMESSAGE    1
#define MRKPBC_CONT_KENUM       2
#define MRKPBC_CONT_KONEOF      3
    int kind;

    struct {
        int allow_alias:1;
        int visited:1;
    } flags;
} mrkpbc_container_t;


typedef struct _mrkpbc_ctx {
    /* strongref */
    mnbytes_t *namein;
    mnbytes_t *nameout0;
    mnbytes_t *nameout1;
    FILE *in;
    FILE *out0;
    FILE *out1;
    /*
     * strongref mnbytes_t *
     * strongref mrkpbc_container_t *
     */
    mnhash_t containers;

    /* strongref mnbytes_t *
     * strongref mrkpbc_field_t *
     */
    mnhash_t fields;

    /* weakref mrkpbc_container_t * */
    mnarray_t stack;
} mrkpbc_ctx_t;






extern mnbytes_t _int32;
extern mnbytes_t _int64;
extern mnbytes_t _uint32;
extern mnbytes_t _uint64;
extern mnbytes_t _sint32;
extern mnbytes_t _sint64;
extern mnbytes_t _fixed32;
extern mnbytes_t _fixed64;
extern mnbytes_t _sfixed32;
extern mnbytes_t _sfixed64;
extern mnbytes_t _float;
extern mnbytes_t _double;
extern mnbytes_t _string;
extern mnbytes_t _bytes;
extern mnbytes_t _bool;





void mrkpbc_ctx_init(mrkpbc_ctx_t *);

mrkpbc_container_t *mrkpbc_ctx_top_container(mrkpbc_ctx_t *);

mrkpbc_container_t *mrkpbc_ctx_add_container(mrkpbc_ctx_t *,
                                             mrkpbc_container_t *,
                                             mnbytes_t *,
                                             int);

void mrkpbc_ctx_push_container(mrkpbc_ctx_t *, mrkpbc_container_t *);

void mrkpbc_ctx_pop_container(mrkpbc_ctx_t *);

mrkpbc_container_t *mrkpbc_ctx_get_container(mrkpbc_ctx_t *,
                                             mnbytes_t *);

int mrkpbc_container_add_field(mrkpbc_container_t *,
                               mnbytes_t *,
                               mnbytes_t *,
                               int,
                               int);

mnbytes_t *mrkpbc_container_fqname(mrkpbc_container_t *,
                                   mnbytes_t *);

void mrkpbc_container_set_pb_fqname(mrkpbc_container_t *,
                                 mnbytes_t *);

void mrkpbc_container_set_be_encode(mrkpbc_container_t *,
                                    mnbytes_t *);

void mrkpbc_container_set_be_decode(mrkpbc_container_t *,
                                    mnbytes_t *);

void mrkpbc_container_set_be_sz(mrkpbc_container_t *,
                                mnbytes_t *);

void mrkpbc_container_set_be_rawsz(mrkpbc_container_t *,
                                   mnbytes_t *);

void mrkpbc_container_set_be_dump(mrkpbc_container_t *,
                                  mnbytes_t *);

void mrkpbc_container_traverse_fields(mrkpbc_container_t *,
                                      array_traverser_t,
                                      void *);

void mrkpbc_container_traverse_containers(mrkpbc_container_t *,
                                          array_traverser_t,
                                          void *);

int mrkpbc_ctx_traverse(mrkpbc_ctx_t *, hash_traverser_t, void *);

#define MRKPB_CTX_VALIDATE_DUPLICATE_FNUM       (-1)
#define MRKPB_CTX_VALIDATE_FIRST_ENUM_NONZERO   (-2)
#define MRKPB_CTX_VALIDATE_ENUM_FNUM_RESERVED   (-3)
#define MRKPB_CTX_VALIDATE_ONEOF_REPEATED       (-4)
int mrkpbc_ctx_validate(mrkpbc_ctx_t *);

void mrkpbc_ctx_dump(mrkpbc_ctx_t *);

void mrkpbc_ctx_init_c(mrkpbc_ctx_t *,
                       mnbytes_t *,
                       FILE *,
                       mnbytes_t *,
                       FILE *,
                       mnbytes_t *,
                       FILE *);
void mrkpbc_ctx_render_c(mrkpbc_ctx_t *);

int mrkpbc_scan(mrkpbc_ctx_t *);
void mrkpbc_ctx_fini(mrkpbc_ctx_t *);

#ifdef __cplusplus
}
#endif

#endif
