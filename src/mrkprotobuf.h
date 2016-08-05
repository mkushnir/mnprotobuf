#ifndef MRKPROTOBUF_H_DEFINED
#define MRKPROTOBUF_H_DEFINED

#include <sys/types.h>

#include <mrkcommon/array.h>
#include <mrkcommon/hash.h>
#include <mrkcommon/bytestream.h>
#include <mrkcommon/bytes.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MRKPB_TDOUBLE    (0)
#define MRKPB_TFLOAT     (1)
#define MRKPB_TINT32     (2)
#define MRKPB_TINT64     (3)
#define MRKPB_TUINT32    (4)
#define MRKPB_TSINT32    (5)
#define MRKPB_TSINT64    (6)
#define MRKPB_TFIX32     (7)
#define MRKPB_TFIX64     (8)
#define MRKPB_TSFIX32    (9)
#define MRKPB_TSFIX64   (10)
#define MRKPB_TBOOL     (11)
#define MRKPB_TSTR      (12)
#define MRKPB_TBYTES    (13)
#define MRKPB_TMESSAGE  (14)
#define MRKPB_TENUM     (15)
#define MRKPB_TMAP      (16)
#define MRKPB_TFIELD    (17)

#define MRKPB_WT_VARINT  (0)
#define MRKPB_WT_64BIT   (1)
#define MRKPB_WT_LDELIM  (2)
#define MRKPB_WT_SGRP    (3)
#define MRKPB_WT_EGRP    (4)
#define MRKPB_WT_32BIT   (5)
#define MRKPB_WT_INTERN  (6)

#define MRKPB_MAKETAG(t, id) (((id)<<3)|t)

typedef struct _mrkpb_spec {
    char *name;
    int64_t type;
    // id << 3 | wtype
    int64_t tag;
    struct {
        int required:1;
        int repeated:1;
    } flags;
    struct _mrkpb_spec *fields[];
} mrkpb_spec_t;


#define MRKPB_VARINT_INITIALIZER(name, ty, id, req, rep)       \
{                                                              \
    name,                                                      \
    ty,                                                        \
    MRKPB_MAKETAG(MRKPB_WT_VARINT, id),                        \
    {req, rep},                                                \
    {NULL}                                                     \
}                                                              \


#define MRKPB_64BIT_INITIALIZER(name, ty, id, req, rep)        \
{                                                              \
    name,                                                      \
    ty,                                                        \
    MRKPB_MAKETAG(MRKPB_WT_64BIT, id),                         \
    {req, rep},                                                \
    {NULL}                                                     \
}                                                              \


#define MRKPB_32BIT_INITIALIZER(name, ty, id, req, rep)        \
{                                                              \
    name,                                                      \
    ty,                                                        \
    MRKPB_MAKETAG(MRKPB_WT_32BIT, id),                         \
    {req, rep},                                                \
    {NULL}                                                     \
}                                                              \


#define MRKPB_LDELIM_INITIALIZER(name, ty, id, req, rep, ...)  \
{                                                              \
    name,                                                      \
    ty,                                                        \
    MRKPB_MAKETAG(MRKPB_WT_LDELIM, id),                        \
    {req, rep},                                                \
    {__VA_ARGS__, NULL}                                        \
}                                                              \



#define MRKPB_INTERN_INITIALIZER(name, ty, id, req, rep, ...)  \
{                                                              \
    name,                                                      \
    ty,                                                        \
    MRKPB_MAKETAG(MRKPB_WT_INTERN, id),                        \
    {req, rep},                                                \
    {__VA_ARGS__, NULL}                                        \
}                                                              \



#define MRKPB_DEF_VARINT(name, ty, id, req, rep)               \
mrkpb_spec_t _mrkpb_ ## name = MRKPB_VARINT_INITIALIZER(       \
    #name, ty, id, req, rep)                                   \


#define MRKPB_DEF_64BIT(name, ty, id, req, rep)                \
mrkpb_spec_t _mrkpb_ ## name = MRKPB_64BIT_INITIALIZER(        \
    #name, ty, id, req, rep)                                   \


#define MRKPB_DEF_32BIT(name, ty, id, req, rep)                \
mrkpb_spec_t _mrkpb_ ## name = MRKPB_32BIT_INITIALIZER(        \
    #name, ty, id, req, rep)                                   \


#define MRKPB_DEF_LDELIM(name, ty, id, req, rep, ...)          \
mrkpb_spec_t _mrkpb_ ## name = MRKPB_LDELIM_INITIALIZER(       \
    #name, ty, id, req, rep, __VA_ARGS__)                      \


#define MRKPB_DEF_INTERN(name, ty, id, req, rep, ...)          \
mrkpb_spec_t _mrkpb_ ## name = MRKPB_INTERN_INITIALIZER(       \
    #name, ty, id, req, rep, __VA_ARGS__)                      \


#define MRKPB_DEF_DOUBLE(name, id, req, rep)           \
    MRKPB_DEF_64BIT(name, MRKPB_TDOUBLE, id, req, rep) \


#define MRKPB_DEF_FLOAT(name, id, req, rep)            \
    MRKPB_DEF_32BIT(name, MRKPB_TFLOAT, id, req, rep)  \


#define MRKPB_DEF_INT32(name, id, req, rep)            \
    MRKPB_DEF_VARINT(name, MRKPB_TINT32, id, req, rep) \


#define MRKPB_DEF_INT64(name, id, req, rep)            \
    MRKPB_DEF_VARINT(name, MRKPB_TINT64, id, req, rep) \


#define MRKPB_DEF_UINT32(name, id, req, rep)           \
    MRKPB_DEF_VARINT(name, MRKPB_TUINT32, id, req, rep)\


#define MRKPB_DEF_UINT64(name, id, req, rep)           \
    MRKPB_DEF_VARINT(name, MRKPB_TUINT64, id, req, rep)\


#define MRKPB_DEF_SINT32(name, id, req, rep)           \
    MRKPB_DEF_VARINT(name, MRKPB_TSINT32, id, req, rep)\


#define MRKPB_DEF_SINT64(name, id, req, rep)           \
    MRKPB_DEF_VARINT(name, MRKPB_TSINT64, id, req, rep)\


#define MRKPB_DEF_FIX32(name, id, req, rep)            \
    MRKPB_DEF_32BIT(name, MRKPB_TFIX32, id, req, rep)  \


#define MRKPB_DEF_FIX64(name, id, req, rep)            \
    MRKPB_DEF_64BIT(name, MRKPB_TFIX64, id, req, rep)  \


#define MRKPB_DEF_SFIX32(name, id, req, rep)           \
    MRKPB_DEF_32BIT(name, MRKPB_TSFIX32, id, req, rep) \


#define MRKPB_DEF_SFIX64(name, id, req, rep)           \
    MRKPB_DEF_64BIT(name, MRKPB_TSFIX64, id, req, rep) \


#define MRKPB_DEF_BOOL(name, id, req, rep)             \
    MRKPB_DEF_VARINT(name, MRKPB_TBOOL, id, req, rep)  \


#define MRKPB_DEF_STR(name, id, req, rep)                      \
    MRKPB_DEF_LDELIM(name, MRKPB_TSTR, id, req, rep, NULL)     \


#define MRKPB_DEF_BYTES(name, id, req, rep)                    \
    MRKPB_DEF_LDELIM(name, MRKPB_TBYTES, id, req, rep, NULL)   \


#define MRKPB_DEF_MESSAGE(name, id, req, rep, ...)                     \
    MRKPB_DEF_LDELIM(name, MRKPB_TMESSAGE, id, req, rep, __VA_ARGS__)  \


#define MRKPB_DEF_FIELD(name, ref, id, req, rep)                               \
    MRKPB_DEF_INTERN(name, MRKPB_TFIELD, id, req, rep, MRKPB_REF(ref), NULL)   \




#define MRKPB_REF(name) &_mrkpb_ ## name

typedef struct _mrkpb_value {
    int64_t type;
    // id << 3 | wtype
    int64_t tag;
    union {
        int64_t i;
        double d;
        bytes_t *s;
    } value;
} mrkpb_value_t;


typedef struct _mrkpb_repvalue {
    int64_t type;
    // id << 3 | wtype
    int64_t tag;
    array_t values;
} mrkpb_repvalue_t;


typedef struct _mrkpb_msg {
    int64_t type;
    // id << 3 | wtype
    int64_t tag;
    hash_t fields;
} mrkpb_msg_t;

ssize_t mrkpb_devarint(bytestream_t *, int, uint64_t *);
ssize_t mrkpb_envarint(bytestream_t *, uint64_t);

ssize_t mrkpb_dezz64(bytestream_t *, int, int64_t *);
ssize_t mrkpb_enzz64(bytestream_t *, int64_t);
ssize_t mrkpb_dezz32(bytestream_t *, int, int32_t *);
ssize_t mrkpb_enzz32(bytestream_t *, int32_t);

#ifdef __cplusplus
}
#endif
#endif /* MRKPROTOBUF_H_DEFINED */
