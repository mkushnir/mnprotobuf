#ifndef MRKPROTOBUF_H_DEFINED
#define MRKPROTOBUF_H_DEFINED

#include <sys/types.h>

#include <mrkcommon/array.h>
#include <mrkcommon/hash.h>
#include <mrkcommon/bytestream.h>
#include <mrkcommon/bytes.h>

/*
 * protocol buffer runtime
 */

#ifdef __cplusplus
extern "C" {
#endif

#define MRKPB_EIO    (-2)
#define MRKPB_ESIZE  (-3)
#define MRKPB_ETYPE  (-4)
ssize_t mrkpb_devarint(mnbytestream_t *, void *, uint64_t *);
ssize_t mrkpb_envarint(mnbytestream_t *, uint64_t);
ssize_t mrkpb_szvarint(uint64_t);
ssize_t mrkpb_dumpvarint(mnbytestream_t *, uint64_t);

ssize_t mrkpb_dezz64(mnbytestream_t *, void *, int64_t *);
ssize_t mrkpb_enzz64(mnbytestream_t *, int64_t);
ssize_t mrkpb_szzz64(int64_t);
ssize_t mrkpb_dumpzz64(mnbytestream_t *, int64_t);

ssize_t mrkpb_dezz32(mnbytestream_t *, void *, int32_t *);
ssize_t mrkpb_enzz32(mnbytestream_t *, int32_t);
ssize_t mrkpb_szzz32(int32_t);
ssize_t mrkpb_dumpzz32(mnbytestream_t *, int32_t);

ssize_t mrkpb_defi64(mnbytestream_t *, void *, uint64_t *);
ssize_t mrkpb_enfi64(mnbytestream_t *, uint64_t);
ssize_t mrkpb_szfi64(uint64_t);
ssize_t mrkpb_dumpfi64(mnbytestream_t *, uint64_t);

ssize_t mrkpb_defi32(mnbytestream_t *, void *, uint32_t *);
ssize_t mrkpb_enfi32(mnbytestream_t *, uint32_t);
ssize_t mrkpb_szfi32(uint32_t);
ssize_t mrkpb_dumpfi32(mnbytestream_t *, uint32_t);

ssize_t mrkpb_dedouble(mnbytestream_t *, void *, double *);
ssize_t mrkpb_endouble(mnbytestream_t *, double);
ssize_t mrkpb_szdouble(double);
ssize_t mrkpb_dumpdouble(mnbytestream_t *, double);

ssize_t mrkpb_defloat(mnbytestream_t *, void *, float *);
ssize_t mrkpb_enfloat(mnbytestream_t *, float);
ssize_t mrkpb_szfloat(float);
ssize_t mrkpb_dumpfloat(mnbytestream_t *, float);

ssize_t mrkpb_debytes(mnbytestream_t *, void *, mnbytes_t **);
/* adding terminating zero */
ssize_t mrkpb_destr(mnbytestream_t *, void *, mnbytes_t **);
ssize_t mrkpb_enbytes(mnbytestream_t *, mnbytes_t *);
/* removing terminating zero */
ssize_t mrkpb_enstr(mnbytestream_t *, mnbytes_t *);
ssize_t mrkpb_szbytes(mnbytes_t *);
/* removing terminating zero */
ssize_t mrkpb_szstr(mnbytes_t *);
ssize_t mrkpb_dumpbytes(mnbytestream_t *, mnbytes_t *);
ssize_t mrkpb_dumpstr(mnbytestream_t *, mnbytes_t *);

ssize_t mrkpb_deldelim(mnbytestream_t *,
                       void *,
                       ssize_t (*)(mnbytestream_t *, void *, void *),
                       void *);
ssize_t mrkpb_enldelim(mnbytestream_t *,
                       size_t,
                       ssize_t (*)(mnbytestream_t *, void *),
                       void *);


ssize_t mrkpb_pack_int32(mnbytestream_t *, int32_t);
ssize_t mrkpb_sz_int32(int32_t);

ssize_t mrkpb_unpack_double(mnbytestream_t *, void *, int, double *);
ssize_t mrkpb_unpack_float(mnbytestream_t *, void *, int, float *);
ssize_t mrkpb_unpack_int32(mnbytestream_t *, void *, int, int32_t *);
ssize_t mrkpb_unpack_int64(mnbytestream_t *, void *, int, int64_t *);
ssize_t mrkpb_unpack_uint32(mnbytestream_t *, void *, int, uint32_t *);
ssize_t mrkpb_unpack_uint64(mnbytestream_t *, void *, int, uint64_t *);
ssize_t mrkpb_unpack_sint32(mnbytestream_t *, void *, int, int32_t *);
ssize_t mrkpb_unpack_sint64(mnbytestream_t *, void *, int, int64_t *);
ssize_t mrkpb_unpack_fixed32(mnbytestream_t *, void *, int, uint32_t *);
ssize_t mrkpb_unpack_fixed64(mnbytestream_t *, void *, int, uint64_t *);
ssize_t mrkpb_unpack_sfixed32(mnbytestream_t *, void *, int, int32_t *);
ssize_t mrkpb_unpack_sfixed64(mnbytestream_t *, void *, int, int64_t *);
ssize_t mrkpb_unpack_bool(mnbytestream_t *, void *, int, bool *);
ssize_t mrkpb_unpack_string(mnbytestream_t *, void *, int, mnbytes_t **);
ssize_t mrkpb_unpack_bytes(mnbytestream_t *, void *, int, mnbytes_t **);
ssize_t mrkpb_unpack_key(mnbytestream_t *, void *f, uint64_t *, int *);
ssize_t mrkpb_devoid(mnbytestream_t *, void *, uint64_t, int);

#ifdef __cplusplus
}
#endif

#endif /* MRKPROTOBUF_H_DEFINED */
