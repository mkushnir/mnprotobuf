#ifndef MNPROTOBUF_H_DEFINED
#define MNPROTOBUF_H_DEFINED

#include <sys/types.h>

#include <mncommon/array.h>
#include <mncommon/hash.h>
#include <mncommon/bytestream.h>
#include <mncommon/bytes.h>

/*
 * protocol buffer runtime
 */

#ifdef __cplusplus
extern "C" {
#endif

#define MNPB_EIO       (-2)
#define MNPB_ESIZE     (-3)
#define MNPB_ETYPE     (-4)
#define MNPB_EMEMORY   (-5)
ssize_t mnpb_devarint(mnbytestream_t *, void *, uint64_t *);
ssize_t mnpb_envarint(mnbytestream_t *, uint64_t);
ssize_t mnpb_szvarint(uint64_t);
ssize_t mnpb_dumpvarint(mnbytestream_t *, uint64_t);

ssize_t mnpb_dezz64(mnbytestream_t *, void *, int64_t *);
ssize_t mnpb_enzz64(mnbytestream_t *, int64_t);
ssize_t mnpb_szzz64(int64_t);
ssize_t mnpb_dumpzz64(mnbytestream_t *, int64_t);

ssize_t mnpb_dezz32(mnbytestream_t *, void *, int32_t *);
ssize_t mnpb_enzz32(mnbytestream_t *, int32_t);
ssize_t mnpb_szzz32(int32_t);
ssize_t mnpb_dumpzz32(mnbytestream_t *, int32_t);

ssize_t mnpb_defi64(mnbytestream_t *, void *, uint64_t *);
ssize_t mnpb_enfi64(mnbytestream_t *, uint64_t);
ssize_t mnpb_szfi64(uint64_t);
ssize_t mnpb_dumpfi64(mnbytestream_t *, uint64_t);

ssize_t mnpb_defi32(mnbytestream_t *, void *, uint32_t *);
ssize_t mnpb_enfi32(mnbytestream_t *, uint32_t);
ssize_t mnpb_szfi32(uint32_t);
ssize_t mnpb_dumpfi32(mnbytestream_t *, uint32_t);

ssize_t mnpb_dedouble(mnbytestream_t *, void *, double *);
ssize_t mnpb_endouble(mnbytestream_t *, double);
ssize_t mnpb_szdouble(double);
ssize_t mnpb_dumpdouble(mnbytestream_t *, double);

ssize_t mnpb_defloat(mnbytestream_t *, void *, float *);
ssize_t mnpb_enfloat(mnbytestream_t *, float);
ssize_t mnpb_szfloat(float);
ssize_t mnpb_dumpfloat(mnbytestream_t *, float);

ssize_t mnpb_debytes(mnbytestream_t *, void *, mnbytes_t **);
/* adding terminating zero */
ssize_t mnpb_destr(mnbytestream_t *, void *, mnbytes_t **);
ssize_t mnpb_enbytes(mnbytestream_t *, mnbytes_t *);
/* removing terminating zero */
ssize_t mnpb_enstr(mnbytestream_t *, mnbytes_t *);
ssize_t mnpb_szbytes(mnbytes_t *);
/* removing terminating zero */
ssize_t mnpb_szstr(mnbytes_t *);
ssize_t mnpb_dumpbytes(mnbytestream_t *, mnbytes_t *);
ssize_t mnpb_dumpstr(mnbytestream_t *, mnbytes_t *);

ssize_t mnpb_deldelim(mnbytestream_t *,
                       void *,
                       ssize_t (*)(mnbytestream_t *, void *, ssize_t, void *),
                       void *);
ssize_t mnpb_enldelim(mnbytestream_t *,
                       size_t,
                       ssize_t (*)(mnbytestream_t *, ssize_t, void *),
                       void *);


ssize_t mnpb_pack_int32(mnbytestream_t *, int32_t);
ssize_t mnpb_sz_int32(int32_t);

ssize_t mnpb_unpack_double(mnbytestream_t *, void *, int, double *);
ssize_t mnpb_unpack_float(mnbytestream_t *, void *, int, float *);
ssize_t mnpb_unpack_int32(mnbytestream_t *, void *, int, int32_t *);
ssize_t mnpb_unpack_int64(mnbytestream_t *, void *, int, int64_t *);
ssize_t mnpb_unpack_uint32(mnbytestream_t *, void *, int, uint32_t *);
ssize_t mnpb_unpack_uint64(mnbytestream_t *, void *, int, uint64_t *);
ssize_t mnpb_unpack_sint32(mnbytestream_t *, void *, int, int32_t *);
ssize_t mnpb_unpack_sint64(mnbytestream_t *, void *, int, int64_t *);
ssize_t mnpb_unpack_fixed32(mnbytestream_t *, void *, int, uint32_t *);
ssize_t mnpb_unpack_fixed64(mnbytestream_t *, void *, int, uint64_t *);
ssize_t mnpb_unpack_sfixed32(mnbytestream_t *, void *, int, int32_t *);
ssize_t mnpb_unpack_sfixed64(mnbytestream_t *, void *, int, int64_t *);
ssize_t mnpb_unpack_bool(mnbytestream_t *, void *, int, bool *);
ssize_t mnpb_unpack_string(mnbytestream_t *, void *, int, mnbytes_t **);
ssize_t mnpb_unpack_bytes(mnbytestream_t *, void *, int, mnbytes_t **);
ssize_t mnpb_unpack_key(mnbytestream_t *, void *f, uint64_t *, int *);
ssize_t mnpb_devoid(mnbytestream_t *, void *, uint64_t, int);

#ifdef __cplusplus
}
#endif

#endif /* MNPROTOBUF_H_DEFINED */
