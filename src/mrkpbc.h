#ifndef MRKPBC_H
#define MRKPBC_H

#include <stdio.h>

#include <mrkcommon/array.h>
#include <mrkcommon/hash.h>
#include <mrkcommon/bytes.h>

#ifdef __cplusplus
extern "C" {
#endif




#ifdef MRKPROTOBUF_OBSOLETE
/*
 *
 */
#define MRKPB_TUNDEF     (0)
#define MRKPB_TDOUBLE    (1)
#define MRKPB_TFLOAT     (2)
#define MRKPB_TINT32     (3)
#define MRKPB_TINT64     (4)
#define MRKPB_TUINT32    (5)
#define MRKPB_TUINT64    (6)
#define MRKPB_TSINT32    (7)
#define MRKPB_TSINT64    (8)
#define MRKPB_TFIX32     (9)
#define MRKPB_TFIX64    (10)
#define MRKPB_TSFIX32   (11)
#define MRKPB_TSFIX64   (12)
#define MRKPB_TBOOL     (13)
#define MRKPB_TSTR      (14)
#define MRKPB_TBYTES    (15)
#define MRKPB_TMESSAGE  (16)
#define MRKPB_TENUM     (17)
#define MRKPB_TMAP      (18)
#define MRKPB_TFIELD    (19)


#define MRKPB_T2STR(t)                 \
(                                      \
 (t) == MRKPB_TUNDEF ? "<undef>" :     \
 (t) == MRKPB_TDOUBLE ? "double" :     \
 (t) == MRKPB_TFLOAT ? "float" :       \
 (t) == MRKPB_TINT32 ? "int32" :       \
 (t) == MRKPB_TINT64 ? "int64" :       \
 (t) == MRKPB_TUINT32 ? "uint32" :     \
 (t) == MRKPB_TUINT64 ? "uint64" :     \
 (t) == MRKPB_TSINT32 ? "sint32" :     \
 (t) == MRKPB_TSINT64 ? "sint64" :     \
 (t) == MRKPB_TFIX32 ? "fixed32" :     \
 (t) == MRKPB_TFIX64 ? "fixed64" :     \
 (t) == MRKPB_TSFIX32 ? "sfixed32" :   \
 (t) == MRKPB_TSFIX64 ? "sfixed64" :   \
 (t) == MRKPB_TBOOL ? "bool" :         \
 (t) == MRKPB_TSTR ? "string" :        \
 (t) == MRKPB_TBYTES ? "bytes" :       \
 "<other>"                             \
)                                      \


typedef struct _mrkpb_spec {
    char *name;
    int64_t type;
    // fnum << 3 | wtype, key in the
    int64_t key;
    struct {
        int required:1;
        int repeated:1;
    } flags;
    struct _mrkpb_spec *fields[];
} mrkpb_spec_t;


#define MRKPB_VARINT_INITIALIZER(name, ty, fnum, req, rep)     \
{                                                              \
    name,                                                      \
    ty,                                                        \
    MRKPB_MAKEKEY(MRKPB_WT_VARINT, fnum),                      \
    {req, rep},                                                \
    {NULL}                                                     \
}                                                              \


#define MRKPB_64BIT_INITIALIZER(name, ty, fnum, req, rep)      \
{                                                              \
    name,                                                      \
    ty,                                                        \
    MRKPB_MAKEKEY(MRKPB_WT_64BIT, fnum),                       \
    {req, rep},                                                \
    {NULL}                                                     \
}                                                              \


#define MRKPB_32BIT_INITIALIZER(name, ty, fnum, req, rep)      \
{                                                              \
    name,                                                      \
    ty,                                                        \
    MRKPB_MAKEKEY(MRKPB_WT_32BIT, fnum),                       \
    {req, rep},                                                \
    {NULL}                                                     \
}                                                              \


#define MRKPB_LDELIM_INITIALIZER(name, ty, fnum, req, rep, ...)\
{                                                              \
    name,                                                      \
    ty,                                                        \
    MRKPB_MAKEKEY(MRKPB_WT_LDELIM, fnum),                      \
    {req, rep},                                                \
    {__VA_ARGS__, NULL}                                        \
}                                                              \



#define MRKPB_INTERN_INITIALIZER(name, ty, fnum, req, rep, ...)\
{                                                              \
    name,                                                      \
    ty,                                                        \
    MRKPB_MAKEKEY(MRKPB_WT_INTERN, fnum),                      \
    {req, rep},                                                \
    {__VA_ARGS__, NULL}                                        \
}                                                              \



#define MRKPB_DEF_VARINT(name, ty, fnum, req, rep)             \
mrkpb_spec_t _mrkpb_ ## name = MRKPB_VARINT_INITIALIZER(       \
    #name, ty, fnum, req, rep)                                 \


#define MRKPB_DEF_64BIT(name, ty, fnum, req, rep)              \
mrkpb_spec_t _mrkpb_ ## name = MRKPB_64BIT_INITIALIZER(        \
    #name, ty, fnum, req, rep)                                 \


#define MRKPB_DEF_32BIT(name, ty, fnum, req, rep)              \
mrkpb_spec_t _mrkpb_ ## name = MRKPB_32BIT_INITIALIZER(        \
    #name, ty, fnum, req, rep)                                 \


#define MRKPB_DEF_LDELIM(name, ty, fnum, req, rep, ...)        \
mrkpb_spec_t _mrkpb_ ## name = MRKPB_LDELIM_INITIALIZER(       \
    #name, ty, fnum, req, rep, __VA_ARGS__)                    \


#define MRKPB_DEF_INTERN(name, ty, fnum, req, rep, ...)        \
mrkpb_spec_t _mrkpb_ ## name = MRKPB_INTERN_INITIALIZER(       \
    #name, ty, fnum, req, rep, __VA_ARGS__)                    \


#define MRKPB_DEF_DOUBLE(name, fnum, req, rep)                 \
    MRKPB_DEF_64BIT(name, MRKPB_TDOUBLE, snum, req, rep)       \


#define MRKPB_DEF_FLOAT(name, fnum, req, rep)          \
    MRKPB_DEF_32BIT(name, MRKPB_TFLOAT, fnum, req, rep)\


#define MRKPB_DEF_INT32(name, fnum, req, rep)                  \
    MRKPB_DEF_VARINT(name, MRKPB_TINT32, fnum, req, rep)       \


#define MRKPB_DEF_INT64(name, fnum, req, rep)                  \
    MRKPB_DEF_VARINT(name, MRKPB_TINT64, fnum, req, rep)       \


#define MRKPB_DEF_UINT32(name, fnum, req, rep)                 \
    MRKPB_DEF_VARINT(name, MRKPB_TUINT32, fnum, req, rep)      \


#define MRKPB_DEF_UINT64(name, fnum, req, rep)                 \
    MRKPB_DEF_VARINT(name, MRKPB_TUINT64, fnum, req, rep)      \


#define MRKPB_DEF_SINT32(name, fnum, req, rep)                 \
    MRKPB_DEF_VARINT(name, MRKPB_TSINT32, fnum, req, rep)      \


#define MRKPB_DEF_SINT64(name, fnum, req, rep)                 \
    MRKPB_DEF_VARINT(name, MRKPB_TSINT64, fnum, req, rep)      \


#define MRKPB_DEF_FIX32(name, fnum, req, rep)          \
    MRKPB_DEF_32BIT(name, MRKPB_TFIX32, fnum, req, rep)\


#define MRKPB_DEF_FIX64(name, fnum, req, rep)          \
    MRKPB_DEF_64BIT(name, MRKPB_TFIX64, fnum, req, rep)\


#define MRKPB_DEF_SFIX32(name, fnum, req, rep)                 \
    MRKPB_DEF_32BIT(name, MRKPB_TSFIX32, fnum, req, rep)       \


#define MRKPB_DEF_SFIX64(name, fnum, req, rep)                 \
    MRKPB_DEF_64BIT(name, MRKPB_TSFIX64, fnum, req, rep)       \


#define MRKPB_DEF_BOOL(name, fnum, req, rep)           \
    MRKPB_DEF_VARINT(name, MRKPB_TBOOL, fnum, req, rep)\


#define MRKPB_DEF_STR(name, fnum, req, rep)                    \
    MRKPB_DEF_LDELIM(name, MRKPB_TSTR, fnum, req, rep, NULL)   \


#define MRKPB_DEF_BYTES(name, fnum, req, rep)                  \
    MRKPB_DEF_LDELIM(name, MRKPB_TBYTES, fnum, req, rep, NULL) \


#define MRKPB_DEF_MESSAGE(name, fnum, req, rep, ...)                   \
    MRKPB_DEF_LDELIM(name, MRKPB_TMESSAGE, fnum, req, rep, __VA_ARGS__)\


#define MRKPB_DEF_FIELD(name, ref, fnum, req, rep)                             \
    MRKPB_DEF_INTERN(name, MRKPB_TFIELD, fnum, req, rep, MRKPB_REF(ref), NULL) \


#define MRKPB_REF(name) &_mrkpb_ ## name

/*
 *
 */
#endif /* MRKPROTOBUF_OBSOLETE */




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
     "-")                              \

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
