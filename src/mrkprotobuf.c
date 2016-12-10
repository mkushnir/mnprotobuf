#include <assert.h>
#include <stdbool.h>
#include <sys/types.h>

#include <mrkcommon/bytestream.h>
#include <mrkcommon/dumpm.h>
#include <mrkcommon/util.h>

#include "diag.h"

ssize_t
mrkpb_devarint(mnbytestream_t *bs, int fd, uint64_t *v)
{
    ssize_t res;
    int i;

    res = 0;
    *v = 0;

    for (i = 0;; ++i) {
        char c;

        if (SNEEDMORE(bs)) {
            if ((res = bytestream_consume_data(bs, fd)) != 0) {
                //TRACE("res=%s", mrkcommon_diag_str(res));
                res = -1;
                goto end;
            }
        }

        c = *SPDATA(bs);

        if (c & 0x80) {
            c &= 0x7f;
            (*v) |= (c << (7 * i));
            ++res;
            SINCR(bs);
        } else {
            (*v) |= (c << (7 * i));
            ++res;
            SINCR(bs);
            break;
        }
    }

end:
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
mrkpb_dezz64(mnbytestream_t *bs, int fd, int64_t *v)
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
mrkpb_dezz32(mnbytestream_t *bs, int fd, int32_t *v)
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

    vv = (uint64_t)(uint32_t)((v << 1) ^ (v >> 31));
    return mrkpb_envarint(bs, vv);
}


ssize_t
mrkpb_defi64(mnbytestream_t *bs, int fd, int64_t *v)
{
    size_t res;
    union {
        char *c;
        int64_t *i;
    } u;

    res = sizeof(int64_t);

    while (SAVAIL(bs) < (ssize_t)sizeof(int64_t)) {
        if ((res = bytestream_consume_data(bs, fd)) != 0) {
            //TRACE("res=%s", mrkcommon_diag_str(res));
            res = -1;
            goto end;
        }
    }

    u.c = SPDATA(bs);
    *v = *u.i;
    SADVANCEPOS(bs, sizeof(uint64_t));

end:
    return res;
}


ssize_t
mrkpb_enfi64(mnbytestream_t *bs, int64_t v)
{
    SCATI64(bs, v);
    return sizeof(int64_t);
}


ssize_t
mrkpb_dedouble(mnbytestream_t *bs, int fd, double *v)
{
    size_t res;
    union {
        char *c;
        double *d;
    } u;

    res = sizeof(double);

    while (SAVAIL(bs) < (ssize_t)sizeof(double)) {
        if ((res = bytestream_consume_data(bs, fd)) != 0) {
            //TRACE("res=%s", mrkcommon_diag_str(res));
            res = -1;
            goto end;
        }
    }

    u.c = SPDATA(bs);
    *v = *u.d;
    SADVANCEPOS(bs, sizeof(double));

end:
    return res;
}


ssize_t
mrkpb_endouble(mnbytestream_t *bs, double v)
{
    SCATD(bs, v);
    return sizeof(double);
}


