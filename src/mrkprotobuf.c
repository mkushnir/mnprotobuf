#include <assert.h>
#include <stdbool.h>
#include <sys/types.h>
#include <inttypes.h>

#include <mrkcommon/bytestream.h>
#include <mrkcommon/dumpm.h>
#include <mrkcommon/util.h>

#include "mrkprotobuf_private.h"

#include "diag.h"

/*
 * protocol buffer runtime
 */
#if __STDC_VERSION__ >= 201112
#   define MNFLS(v)                    \
    _Generic(v,                        \
             unsigned long long: flsll,\
             long long: flsll,         \
             default: flsl,            \
             unsigned int: fls,        \
             int: fls,                 \
             )(v)                      \


#else
#   define MNFLS(v) flsl((long)(v))
#endif

void mndiag_mrkcommon_str(int, char *, size_t);

ssize_t
mrkpb_devarint(mnbytestream_t *bs, void *fd, uint64_t *v)
{
    ssize_t res;
    int i;

    res = 0;
    *v = 0;

    for (i = 0;; ++i) {
        unsigned char c;

        if (SNEEDMORE(bs)) {
            if ((res = bytestream_consume_data(bs, fd)) != 0) {
                //char buf[64];
                //mndiag_mrkcommon_str(res, buf, sizeof(buf));
                //TRACE("res=%s", buf);
                res = MRKPB_EIO;
                goto end;
            }
        }

        c = *SPDATA(bs);

        if (c & 0x80) {
            uint64_t cc;

            cc = c & 0x7f;
            (*v) |= (cc << (7 * i));
            ++res;
            SINCR(bs);
        } else {
            uint64_t cc;

            cc = c;
            (*v) |= (cc << (7 * i));
            ++res;
            SINCR(bs);
            break;
        }
    }

end:
    assert(res != 0);
    return res;
}


ssize_t
mrkpb_envarint(mnbytestream_t *bs, uint64_t v)
{
    ssize_t res;

    res = 0;
    while (true) {
        char c;

        c = v & 0x7f;
        v >>= 7;
        if (v != 0) {
            ++res;
            SCATC(bs, c | 0x80);
        } else {
            ++res;
            SCATC(bs, c);
            break;
        }
    }

    return res;
}


ssize_t
mrkpb_szvarint(uint64_t v)
{
    ssize_t res;

    if (v == 0) {
        res = 1;
    } else {
        int msb;
        div_t d;

        msb = MNFLS(v);
        d = div(msb, 7);
        res = d.quot + (d.rem ? 1 : 0);
    }
    return res;
}


ssize_t
mrkpb_dumpvarint(mnbytestream_t *bs, uint64_t v)
{
    return bytestream_nprintf(bs, 64, "%"PRId64, v);
}


ssize_t
mrkpb_dezz64(mnbytestream_t *bs, void *fd, int64_t *v)
{
    ssize_t res;
    uint64_t vv;

    res = mrkpb_devarint(bs, fd, &vv);
    *v = (int64_t)((vv >> 1) ^ ((int64_t)(vv << 63) >> 63));
    return res;
}


ssize_t
mrkpb_enzz64(mnbytestream_t *bs, int64_t v)
{
    uint64_t vv;

    vv = (uint64_t)((v << 1) ^ (v >> 63));
    return mrkpb_envarint(bs, vv);
}


ssize_t
mrkpb_szzz64(int64_t v)
{
    v = (v << 1) ^ (v >> 63);
    return mrkpb_szvarint((uint64_t)v);
}


ssize_t
mrkpb_dumpzz64(mnbytestream_t *bs, int64_t v)
{
    return bytestream_nprintf(bs, 64, "%"PRId64, v);
}


ssize_t
mrkpb_dezz32(mnbytestream_t *bs, void *fd, int32_t *v)
{
    ssize_t res;
    uint64_t vv;

    res = mrkpb_devarint(bs, fd, &vv);
    *v = ((uint32_t)vv >> 1) ^ ((int32_t)(vv << 31) >> 31);
    return res;
}


ssize_t
mrkpb_enzz32(mnbytestream_t *bs, int32_t v)
{
    uint64_t vv;

    vv = (uint64_t)((((uint32_t)v) << 1) ^ (((uint32_t)v) >> 31));
    return mrkpb_envarint(bs, vv);
}


ssize_t
mrkpb_szzz32(int32_t v)
{
    uint64_t vv;

    vv = (uint64_t)((((uint32_t)v) << 1) ^ (((uint32_t)v) >> 31));
    return mrkpb_szvarint(v);
}


ssize_t
mrkpb_dumpzz32(mnbytestream_t *bs, int32_t v)
{
    return bytestream_nprintf(bs, 64, "%"PRId32, v);
}


ssize_t
mrkpb_defi64(mnbytestream_t *bs, void *fd, uint64_t *v)
{
    ssize_t res;
    union {
        char *c;
        uint64_t *i;
    } u;

    res = sizeof(uint64_t);

    while (SAVAIL(bs) < (ssize_t)sizeof(uint64_t)) {
        if ((res = bytestream_consume_data(bs, fd)) != 0) {
            //TRACE("res=%s", mrkcommon_diag_str(res));
            res = MRKPB_EIO;
            goto end;
        }
    }

    u.c = SPDATA(bs);
    *v = *u.i;
    SADVANCEPOS(bs, sizeof(uint64_t));

end:
    assert(res != 0);
    return res;
}


ssize_t
mrkpb_enfi64(mnbytestream_t *bs, uint64_t v)
{
    SCATI64(bs, v);
    return sizeof(uint64_t);
}


ssize_t
mrkpb_szfi64(uint64_t v)
{
    return sizeof(v);
}


ssize_t
mrkpb_dumpfi64(mnbytestream_t *bs, uint64_t v)
{
    return bytestream_nprintf(bs, 64, "%"PRIu64, v);
}


ssize_t
mrkpb_defi32(mnbytestream_t *bs, void *fd, uint32_t *v)
{
    ssize_t res;
    union {
        char *c;
        uint32_t *i;
    } u;

    res = sizeof(uint32_t);

    while (SAVAIL(bs) < (ssize_t)sizeof(uint32_t)) {
        if ((res = bytestream_consume_data(bs, fd)) != 0) {
            //TRACE("res=%s", mrkcommon_diag_str(res));
            res = MRKPB_EIO;
            goto end;
        }
    }

    u.c = SPDATA(bs);
    *v = *u.i;
    SADVANCEPOS(bs, sizeof(uint32_t));

end:
    assert(res != 0);
    return res;
}


ssize_t
mrkpb_enfi32(mnbytestream_t *bs, uint32_t v)
{
    SCATI32(bs, v);
    return sizeof(uint32_t);
}


ssize_t
mrkpb_szfi32(uint32_t v)
{
    return sizeof(v);
}


ssize_t
mrkpb_dumpfi32(mnbytestream_t *bs, uint32_t v)
{
    return bytestream_nprintf(bs, 64, "%"PRIu32, v);
}


ssize_t
mrkpb_dedouble(mnbytestream_t *bs, void *fd, double *v)
{
    ssize_t res;
    union {
        char *c;
        double *d;
    } u;

    res = sizeof(double);

    while (SAVAIL(bs) < (ssize_t)sizeof(double)) {
        if ((res = bytestream_consume_data(bs, fd)) != 0) {
            //TRACE("res=%s", mrkcommon_diag_str(res));
            res = MRKPB_EIO;
            goto end;
        }
    }

    u.c = SPDATA(bs);
    *v = *u.d;
    SADVANCEPOS(bs, sizeof(double));

end:
    assert(res != 0);
    return res;
}


ssize_t
mrkpb_endouble(mnbytestream_t *bs, double v)
{
    SCATD(bs, v);
    return sizeof(double);
}


ssize_t
mrkpb_szdouble(double v)
{
    return sizeof(v);
}


ssize_t
mrkpb_dumpdouble(mnbytestream_t *bs, double v)
{
    return bytestream_nprintf(bs, 512, "%lg", v);
}


ssize_t
mrkpb_defloat(mnbytestream_t *bs, void *fd, float *v)
{
    ssize_t res;
    union {
        char *c;
        float *d;
    } u;

    res = sizeof(float);

    while (SAVAIL(bs) < (ssize_t)sizeof(float)) {
        if ((res = bytestream_consume_data(bs, fd)) != 0) {
            //TRACE("res=%s", mrkcommon_diag_str(res));
            res = MRKPB_EIO;
            goto end;
        }
    }

    u.c = SPDATA(bs);
    *v = *u.d;
    SADVANCEPOS(bs, sizeof(float));

end:
    assert(res != 0);
    return res;
}


ssize_t
mrkpb_enfloat(mnbytestream_t *bs, float v)
{
    SCATF(bs, v);
    return sizeof(float);
}


ssize_t
mrkpb_szfloat(float v)
{
    return sizeof(v);
}


ssize_t
mrkpb_dumpfloat(mnbytestream_t *bs, float v)
{
    return bytestream_nprintf(bs, 512, "%g", v);
}


ssize_t
mrkpb_debytes(mnbytestream_t *bs, void *fd, mnbytes_t **v)
{
    ssize_t res;
    ssize_t sz;

    if ((res = mrkpb_devarint(bs, fd, (uint64_t *)&sz)) < 0) {
        goto end;
    }
    if (sz > MRKPB_MAX_BYTES) {
        res = MRKPB_ESIZE;
        goto end;
    }

    if (sz == 0) {
        *v = NULL;
        goto end;
    }

    while (SAVAIL(bs) < sz) {
        if ((res = bytestream_consume_data(bs, fd)) != 0) {
            //TRACE("res=%s", mrkcommon_diag_str(res));
            res = MRKPB_EIO;
            goto end;
        }
    }

    *v = bytes_new_from_mem_len(SPDATA(bs), sz);
    SADVANCEPOS(bs, sz);
    res += sz;

end:
    assert(res != 0);
    return res;
}


ssize_t
mrkpb_destr(mnbytestream_t *bs, void *fd, mnbytes_t **v)
{
    ssize_t res;
    ssize_t sz;

    if ((res = mrkpb_devarint(bs, fd, (uint64_t *)&sz)) < 0) {
        goto end;
    }
    if (sz > MRKPB_MAX_BYTES) {
        res = MRKPB_ESIZE;
        goto end;
    }

    if (sz == 0) {
        *v = NULL;
        goto end;
    }

    while (SAVAIL(bs) < sz) {
        if ((res = bytestream_consume_data(bs, fd)) != 0) {
            //TRACE("res=%s", mrkcommon_diag_str(res));
            res = MRKPB_EIO;
            goto end;
        }
    }

    *v = bytes_new_from_str_len(SPDATA(bs), sz);
    SADVANCEPOS(bs, sz);
    res += sz;

end:
    assert(res != 0);
    return res;
}


ssize_t
mrkpb_enbytes(mnbytestream_t *bs, mnbytes_t *v)
{
    ssize_t res0, res1;

    if (v == NULL) {
        return 0;
    }

    if ((res0 = mrkpb_envarint(bs, BSZ(v))) < 0) {
        goto end;
    }
    if ((res1 = bytestream_cat(bs, BSZ(v), BCDATA(v))) < 0) {
        res0 = MRKPB_EIO;
        goto end;
    }
    res0 += res1;

end:
    assert(res0 != 0);
    return res0;
}


ssize_t
mrkpb_enstr(mnbytestream_t *bs, mnbytes_t *v)
{
    ssize_t res0, res1;
    uint64_t sz;

    if (v == NULL) {
        return 0;
    }

    assert(BSZ(v) > 0);
    sz = BSZ(v) - 1;

    if ((res0 = mrkpb_envarint(bs, sz)) < 0) {
        goto end;
    }
    if ((res1 = bytestream_cat(bs, sz, BCDATA(v))) < 0) {
        res0 = MRKPB_EIO;
        goto end;
    }
    res0 += res1;

end:
    assert(res0 != 0);
    return res0;
}


ssize_t
mrkpb_szbytes(mnbytes_t *s)
{
    if (s == NULL) {
        return 0;
    }
    return mrkpb_szvarint(BSZ(s)) + BSZ(s);
}


ssize_t
mrkpb_szstr(mnbytes_t *s)
{
    if (s == NULL) {
        return 0;
    }
    return mrkpb_szvarint(BSZ(s) - 1) + BSZ(s) - 1;
}


ssize_t
mrkpb_dumpbytes(mnbytestream_t *bs, mnbytes_t *v)
{
    if (v == NULL) {
        return 0;
    }

    return bytestream_nprintf(bs, 512, "<bytes of %zd>", BSZ(v));
}


ssize_t
mrkpb_dumpstr(mnbytestream_t *bs, mnbytes_t *v)
{
    if (v == NULL) {
        return 0;
    }

    return bytestream_nprintf(bs, 8 + BSZ(v), "\"%s\"", BDATA(v));
}


ssize_t
mrkpb_deldelim(mnbytestream_t *bs,
               void *fd,
               ssize_t (*cb)(mnbytestream_t *, void *, ssize_t, void *),
               void *udata)
{
    ssize_t res;
    ssize_t sz;

    if ((res = mrkpb_devarint(bs, fd, (uint64_t *)&sz)) < 0) {
        goto end;
    }
    if (sz > MRKPB_MAX_BYTES) {
        res = MRKPB_ESIZE;
        goto end;
    }

    res = cb(bs, fd, sz, udata);

end:
    return res;
}


ssize_t
mrkpb_enldelim(mnbytestream_t *bs,
               size_t sz,
               ssize_t (*cb)(mnbytestream_t *, ssize_t, void *),
               void *udata)
{
    ssize_t res0, res1;

    if ((res0 = mrkpb_envarint(bs, sz)) < 0) {
        goto end;
    }

    if ((res1 = cb(bs, sz, udata)) < 0) {
        res0 = MRKPB_EIO;
        goto end;
    }

    res0 += res1;

end:
    assert(res0 != 0);
    return res0;
}






ssize_t
mrkpb_unpack_double(mnbytestream_t *bs, void *fd, int wtype, double *value)
{
    ssize_t nread;

    if (wtype == -1) {
        wtype = MRKPB_WT_64BIT;
    }

    if (wtype == MRKPB_WT_64BIT) {
        nread = mrkpb_dedouble(bs, fd, value);

    } else if (wtype == MRKPB_WT_32BIT) {
        float v;

        if ((nread = mrkpb_defloat(bs, fd, &v)) < 0) {
            goto end;
        }
        *value = v;

    } else {
        nread = MRKPB_ETYPE;
        goto end;
    }

end:
    return nread;
}


ssize_t
mrkpb_unpack_float(mnbytestream_t *bs, void *fd, int wtype, float *value)
{
    ssize_t nread;

    if (wtype == -1) {
        wtype = MRKPB_WT_32BIT;
    }

    if (wtype == MRKPB_WT_32BIT) {
        nread = mrkpb_defloat(bs, fd, value);

    } else if (wtype == MRKPB_WT_64BIT) {
        double v;

        if ((nread = mrkpb_dedouble(bs, fd, &v)) < 0) {
            goto end;
        }
        *value = v;

    } else {
        nread = MRKPB_ETYPE;
        goto end;
    }

end:
    return nread;
}


ssize_t
mrkpb_pack_int32(mnbytestream_t *bs, int32_t v)
{
    return mrkpb_envarint(bs, (uint32_t)v);
}


ssize_t
mrkpb_sz_int32(int32_t v)
{
    return mrkpb_szvarint((uint32_t)v);
}


ssize_t
mrkpb_unpack_int32(mnbytestream_t *bs, void *fd, int wtype, int32_t *value)
{
    ssize_t nread;

    if (wtype == -1) {
        wtype = MRKPB_WT_VARINT;
    }

    if (wtype == MRKPB_WT_VARINT) {
        uint64_t v;

        if ((nread = mrkpb_devarint(bs, fd, &v)) < 0) {
            goto end;
        }
        *value = (int32_t)(uint32_t)v;

    } else if (wtype == MRKPB_WT_32BIT) {
        nread = mrkpb_defi32(bs, fd, (uint32_t *)value);

    } else {
        nread = MRKPB_ETYPE;
        goto end;
    }

end:
    return nread;
}


ssize_t
mrkpb_unpack_int64(mnbytestream_t *bs, void *fd, int wtype, int64_t *value)
{
    ssize_t nread;

    if (wtype == -1) {
        wtype = MRKPB_WT_VARINT;
    }

    if (wtype == MRKPB_WT_VARINT) {
        nread = mrkpb_devarint(bs, fd, (uint64_t *)value);

    } else if (wtype == MRKPB_WT_64BIT) {
        nread = mrkpb_defi64(bs, fd, (uint64_t *)value);

    } else {
        nread = MRKPB_ETYPE;
        goto end;
    }

end:
    return nread;
}


ssize_t
mrkpb_unpack_uint32(mnbytestream_t *bs, void *fd, int wtype, uint32_t *value)
{
    ssize_t nread;

    if (wtype == -1) {
        wtype = MRKPB_WT_VARINT;
    }

    if (wtype == MRKPB_WT_VARINT) {
        uint64_t v;

        if ((nread = mrkpb_devarint(bs, fd, &v)) < 0) {
            goto end;
        }
        *value = v;

    } else if (wtype == MRKPB_WT_32BIT) {
        nread = mrkpb_defi32(bs, fd, value);

    } else {
        nread = MRKPB_ETYPE;
        goto end;
    }

end:
    return nread;
}


ssize_t
mrkpb_unpack_uint64(mnbytestream_t *bs, void *fd, int wtype, uint64_t *value)
{
    ssize_t nread;

    if (wtype == -1) {
        wtype = MRKPB_WT_VARINT;
    }

    if (wtype == MRKPB_WT_VARINT) {
        nread = mrkpb_devarint(bs, fd, value);

    } else if (wtype == MRKPB_WT_64BIT) {
        nread = mrkpb_defi64(bs, fd, value);

    } else {
        nread = MRKPB_ETYPE;
        goto end;
    }

end:
    return nread;
}


ssize_t
mrkpb_unpack_sint32(mnbytestream_t *bs, void *fd, int wtype, int32_t *value)
{
    ssize_t nread;

    if (wtype == -1) {
        wtype = MRKPB_WT_VARINT;
    }

    if (wtype == MRKPB_WT_VARINT) {
        nread = mrkpb_dezz32(bs, fd, value);

    } else {
        nread = MRKPB_ETYPE;
        goto end;
    }

end:
    return nread;
}


ssize_t
mrkpb_unpack_sint64(mnbytestream_t *bs, void *fd, int wtype, int64_t *value)
{
    ssize_t nread;

    if (wtype == -1) {
        wtype = MRKPB_WT_VARINT;
    }

    if (wtype == MRKPB_WT_VARINT) {
        nread = mrkpb_dezz64(bs, fd, value);

    } else {
        nread = MRKPB_ETYPE;
        goto end;
    }

end:
    return nread;
}


ssize_t
mrkpb_unpack_fixed32(mnbytestream_t *bs, void *fd, int wtype, uint32_t *value)
{
    ssize_t nread;

    if (wtype == -1) {
        wtype = MRKPB_WT_32BIT;
    }

    if (wtype == MRKPB_WT_32BIT) {
        nread = mrkpb_defi32(bs, fd, value);

    } else {
        nread = MRKPB_ETYPE;
        goto end;
    }

end:
    return nread;
}


ssize_t
mrkpb_unpack_fixed64(mnbytestream_t *bs, void *fd, int wtype, uint64_t *value)
{
    ssize_t nread;

    if (wtype == -1) {
        wtype = MRKPB_WT_64BIT;
    }

    if (wtype == MRKPB_WT_64BIT) {
        nread = mrkpb_defi64(bs, fd, value);

    } else {
        nread = MRKPB_ETYPE;
        goto end;
    }

end:
    return nread;
}


ssize_t
mrkpb_unpack_sfixed32(mnbytestream_t *bs, void *fd, int wtype, int32_t *value)
{
    ssize_t nread;

    if (wtype == -1) {
        wtype = MRKPB_WT_32BIT;
    }

    if (wtype == MRKPB_WT_32BIT) {
        nread = mrkpb_defi32(bs, fd, (uint32_t *)value);

    } else {
        nread = MRKPB_ETYPE;
        goto end;
    }

end:
    return nread;
}


ssize_t
mrkpb_unpack_sfixed64(mnbytestream_t *bs, void *fd, int wtype, int64_t *value)
{
    ssize_t nread;

    if (wtype == -1) {
        wtype = MRKPB_WT_64BIT;
    }

    if (wtype == MRKPB_WT_64BIT) {
        nread = mrkpb_defi64(bs, fd, (uint64_t *)value);

    } else {
        nread = MRKPB_ETYPE;
        goto end;
    }

end:
    return nread;
}


ssize_t
mrkpb_unpack_bool(mnbytestream_t *bs, void *fd, int wtype, bool *value)
{
    ssize_t nread;

    if (wtype == -1) {
        wtype = MRKPB_WT_VARINT;
    }

    if (wtype == MRKPB_WT_VARINT) {
        uint64_t v;

        if ((nread = mrkpb_devarint(bs, fd, &v)) < 0) {
            goto end;
        }
        *value = (bool)v;

    } else if (wtype == MRKPB_WT_32BIT) {
        uint32_t v;

        nread = mrkpb_defi32(bs, fd, &v);
        *value = (bool)v;

    } else if (wtype == MRKPB_WT_64BIT) {
        uint64_t v;

        nread = mrkpb_defi64(bs, fd, &v);
        *value = (bool)v;

    } else {
        nread = MRKPB_ETYPE;
        goto end;
    }

end:
    return nread;
}


ssize_t
mrkpb_unpack_string(mnbytestream_t *bs, void *fd, int wtype, mnbytes_t **value)
{
    ssize_t nread;

    if (wtype == -1) {
        wtype = MRKPB_WT_LDELIM;
    }

    if (wtype == MRKPB_WT_LDELIM) {
        BYTES_DECREF(value);
        nread = mrkpb_destr(bs, fd, value);
        BYTES_INCREF(*value);

    } else {
        nread = MRKPB_ETYPE;
        goto end;
    }

end:
    return nread;
}


ssize_t
mrkpb_unpack_bytes(mnbytestream_t *bs, void *fd, int wtype, mnbytes_t **value)
{
    ssize_t nread;

    if (wtype == -1) {
        wtype = MRKPB_WT_LDELIM;
    }

    if (wtype == MRKPB_WT_LDELIM) {
        BYTES_DECREF(value);
        nread = mrkpb_debytes(bs, fd, value);
        BYTES_INCREF(*value);

    } else {
        nread = MRKPB_ETYPE;
        goto end;
    }

end:
    return nread;
}


ssize_t
mrkpb_unpack_key(mnbytestream_t *bs, void *fd, uint64_t *tag, int *wtype)
{
    ssize_t nread;
    uint64_t key;

    nread = 0;
    key = 0;

    if ((nread = mrkpb_devarint(bs, fd, &key)) < 0) {
        goto end;
    }

    *wtype = key & 0x7ul;
    *tag = key >> 3;

end:
    return nread;
}


ssize_t
mrkpb_devoid(mnbytestream_t *bs, void *fd, UNUSED uint64_t tag, int wtype)
{
    ssize_t res;
    union {
        uint64_t i8;
        uint32_t i4;
        mnbytes_t *s;
    } u;

    if (wtype == -1) {
        wtype = MRKPB_WT_LDELIM;
    }

    switch (wtype) {
        case MRKPB_WT_VARINT:
            res = mrkpb_devarint(bs, fd, &u.i8);
            break;

        case MRKPB_WT_64BIT:
            res = mrkpb_defi64(bs, fd, &u.i8);
            break;

        case MRKPB_WT_LDELIM:
            u.s = NULL;
            res = mrkpb_debytes(bs, fd, &u.s);
            BYTES_DECREF(&u.s);
            break;

        case MRKPB_WT_32BIT:
            res = mrkpb_defi32(bs, fd, &u.i4);
            break;

        default:
            res = MRKPB_ETYPE;
    }

    return res;
}
