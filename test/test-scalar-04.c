#include <assert.h>

#include <mncommon/bytes.h>
#include <mncommon/bytestream_aux.h>
#include <mncommon/dumpm.h>
#include <mncommon/util.h>

#include <mnprotobuf.h>

#include "data/scalar-04.h"

#include "unittest.h"

#ifndef NDEBUG
const char *_malloc_options = "AJ";
#endif

static mnbytes_t _foo = BYTES_INITIALIZER("FOO");
static mnbytes_t _john = BYTES_INITIALIZER("John");
static mnbytes_t _doe = BYTES_INITIALIZER("Doe");


int
main(void)
{
    struct scalar_04_req *sc0, *sc1, *sc2, *sc3;
    mnbytes_t **pload;
    mnbytestream_t bs0, bs1;
    mnbytes_t *s;
    ssize_t sz;

    sc0 = scalar_04_req_new();
    assert(sc0 != NULL);
    sc1 = scalar_04_req_new();
    assert(sc1 != NULL);
    sc2 = scalar_04_req_new();
    assert(sc2 != NULL);
    sc3 = scalar_04_req_new();
    assert(sc3 != NULL);

    sc0->id = 123;
    SCALAR_04_PROTO_SETFNUM(sc0, body, r1);
    SCALAR_04_PROTO_SETDATA(sc0, body, r1.payload, &_foo);
    BYTES_INCREF(&_foo);

    sc2->id = 234;
    SCALAR_04_PROTO_SETFNUM(sc2, body, r2);
    pload = req2_payload_alloc(&sc2->body.data.r2, 1);
    *pload = &_john;
    BYTES_INCREF(&_john);
    pload = req2_payload_alloc(&sc2->body.data.r2, 1);
    *pload = &_doe;
    BYTES_INCREF(&_doe);

    (void)bytestream_init(&bs0, 32);

    sz = scalar_04_req_pack(&bs0, sc0);
    D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("sz=%zd", sz);

    s = bytes_new_from_mem_len(SPDATA(&bs0), SEOD(&bs0));
    bytestream_from_bytes(&bs1, s);
    SEOD(&bs1) = BSZ(s);
    D8(SPDATA(&bs1), SEOD(&bs1));

    sz = scalar_04_req_unpack(&bs1, NULL, sc1);
    TRACE("sz=%zd spos=%ld", sz, (long)SPOS(&bs1));

    bytestream_rewind(&bs0);
    sz = scalar_04_req_dump(&bs0, sc0);
    //D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("dump: %s", SPDATA(&bs0));
    bytestream_rewind(&bs0);
    sz = scalar_04_req_dump(&bs0, sc1);
    //D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("dump: %s", SPDATA(&bs0));


    bytestream_rewind(&bs0);
    sz = scalar_04_req_pack(&bs0, sc2);
    D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("sz=%zd", sz);
    BYTES_DECREF(&s);
    s = bytes_new_from_mem_len(SPDATA(&bs0), SEOD(&bs0));
    bytestream_from_bytes(&bs1, s);
    SEOD(&bs1) = BSZ(s);
    D8(SPDATA(&bs1), SEOD(&bs1));
    sz = scalar_04_req_unpack(&bs1, NULL, sc3);
    TRACE("sz=%zd spos=%ld", sz, (long)SPOS(&bs1));
    bytestream_rewind(&bs0);
    sz = scalar_04_req_dump(&bs0, sc2);
    //D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("dump: %s", SPDATA(&bs0));
    bytestream_rewind(&bs0);
    sz = scalar_04_req_dump(&bs0, sc3);
    //D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("dump: %s", SPDATA(&bs0));


    scalar_04_req_destroy(&sc0);
    assert(sc0 == NULL);
    scalar_04_req_destroy(&sc1);
    assert(sc1 == NULL);
    scalar_04_req_destroy(&sc2);
    assert(sc2 == NULL);
    scalar_04_req_destroy(&sc3);
    assert(sc3 == NULL);

    bytestream_fini(&bs0);
    //bytestream_fini(&bs1);
    BYTES_DECREF(&s);

    BYTES_NREF_STATIC_INVARIANT(_foo);
    BYTES_NREF_STATIC_INVARIANT(_john);
    BYTES_NREF_STATIC_INVARIANT(_doe);

    return 0;
}

