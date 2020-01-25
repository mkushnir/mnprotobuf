#ifndef MNPBC_H
#define MNPBC_H

#include <stdio.h>

#include <mncommon/array.h>
#include <mncommon/hash.h>
#include <mncommon/bytes.h>

#ifdef __cplusplus
extern "C" {
#endif


#define MNPB_WT_UNDEF   (-1)
#define MNPB_WT_VARINT  (0)
#define MNPB_WT_64BIT   (1)
#define MNPB_WT_LDELIM  (2)
#define MNPB_WT_SGRP    (3)
#define MNPB_WT_EGRP    (4)
#define MNPB_WT_32BIT   (5)
#define MNPB_WT_INTERN  (6)

#define MNPB_MAKEKEY(wtype, fnum) (((fnum)<<3)|wtype)

#define MNPB_WT_NUMERIC(wtype)        \
    ((wtype) == MNPB_WT_VARINT ||     \
     (wtype) == MNPB_WT_64BIT ||      \
     (wtype) == MNPB_WT_32BIT)        \

#define MNPB_WT_CHAR(wtype)           \
    ((wtype) == MNPB_WT_VARINT ? "V" :\
     (wtype) == MNPB_WT_64BIT ? "8" : \
     (wtype) == MNPB_WT_LDELIM ? "L" :\
     (wtype) == MNPB_WT_32BIT ? "4" : \
     "")                               \

#ifndef MNPB_MAX_BYTES
#   define MNPB_MAX_BYTES (0x100000)
#endif


struct _mnpbc_container;
struct _mnpbc_ctx;

typedef struct _mnpbc_field {
    /* weakref */
    struct _mnpbc_container *parent;
    mnbytes_t *ty;
    /* weakref */
    struct _mnpbc_container *cty;
    struct {
        mnbytes_t *name;
        mnbytes_t *fqname;
    } pb;
    struct {
        mnbytes_t *name;
        mnbytes_t *fqname;
    } be;
    /* field number */
#define MNPBC_FNUM_ONEOF (-1l)
    int64_t fnum;
    int wtype;
    struct {
        int repeated:1;
    } flags;
} mnpbc_field_t;

typedef struct _mnpbc_container {
    /* weakref */
    struct _mnpbc_ctx *ctx;

    /* weakref */
    struct _mnpbc_container *parent;

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

    /* weakref mnpbc_field_t * */
    mnarray_t fields;

    /* weakref mnpbc_container_t * */
    mnarray_t containers;

#define MNPBC_CONT_KBUILTIN    0
#define MNPBC_CONT_KMESSAGE    1
#define MNPBC_CONT_KENUM       2
#define MNPBC_CONT_KONEOF      3
    int kind;

    struct {
        int allow_alias:1;
        int visited:1;
    } flags;
} mnpbc_container_t;


typedef struct _mnpbc_ctx {
    /* strongref */
    mnbytes_t *namein;
    mnbytes_t *nameout0;
    mnbytes_t *nameout1;
    FILE *in;
    FILE *out0;
    FILE *out1;
    /*
     * strongref mnbytes_t *
     * strongref mnpbc_container_t *
     */
    mnhash_t containers;

    /* strongref mnbytes_t *
     * strongref mnpbc_field_t *
     */
    mnhash_t fields;

    /* weakref mnpbc_container_t * */
    mnarray_t stack;
} mnpbc_ctx_t;






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





void mnpbc_ctx_init(mnpbc_ctx_t *);

mnpbc_container_t *mnpbc_ctx_top_container(mnpbc_ctx_t *);

mnpbc_container_t *mnpbc_ctx_add_container(mnpbc_ctx_t *,
                                             mnpbc_container_t *,
                                             mnbytes_t *,
                                             int);

void mnpbc_ctx_push_container(mnpbc_ctx_t *, mnpbc_container_t *);

void mnpbc_ctx_pop_container(mnpbc_ctx_t *);

mnpbc_container_t *mnpbc_ctx_get_container(mnpbc_ctx_t *,
                                             mnbytes_t *);

int mnpbc_container_add_field(mnpbc_container_t *,
                               mnbytes_t *,
                               mnbytes_t *,
                               int,
                               int);

mnbytes_t *mnpbc_container_fqname(mnpbc_container_t *,
                                   mnbytes_t *);

void mnpbc_container_set_pb_fqname(mnpbc_container_t *,
                                 mnbytes_t *);

void mnpbc_container_set_be_encode(mnpbc_container_t *,
                                    mnbytes_t *);

void mnpbc_container_set_be_decode(mnpbc_container_t *,
                                    mnbytes_t *);

void mnpbc_container_set_be_sz(mnpbc_container_t *,
                                mnbytes_t *);

void mnpbc_container_set_be_rawsz(mnpbc_container_t *,
                                   mnbytes_t *);

void mnpbc_container_set_be_dump(mnpbc_container_t *,
                                  mnbytes_t *);

void mnpbc_container_traverse_fields(mnpbc_container_t *,
                                      array_traverser_t,
                                      void *);

void mnpbc_container_traverse_containers(mnpbc_container_t *,
                                          array_traverser_t,
                                          void *);

int mnpbc_ctx_traverse(mnpbc_ctx_t *, hash_traverser_t, void *);

#define MNPB_CTX_VALIDATE_DUPLICATE_FNUM       (-1)
#define MNPB_CTX_VALIDATE_FIRST_ENUM_NONZERO   (-2)
#define MNPB_CTX_VALIDATE_ENUM_FNUM_RESERVED   (-3)
#define MNPB_CTX_VALIDATE_ONEOF_REPEATED       (-4)
int mnpbc_ctx_validate(mnpbc_ctx_t *);

void mnpbc_ctx_dump(mnpbc_ctx_t *);

void mnpbc_ctx_init_c(mnpbc_ctx_t *,
                       mnbytes_t *,
                       FILE *,
                       mnbytes_t *,
                       FILE *,
                       mnbytes_t *,
                       FILE *);
void mnpbc_ctx_render_c(mnpbc_ctx_t *);

int mnpbc_scan(mnpbc_ctx_t *);
void mnpbc_ctx_fini(mnpbc_ctx_t *);

#ifdef __cplusplus
}
#endif

#endif
