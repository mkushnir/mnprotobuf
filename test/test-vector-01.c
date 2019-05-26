#include <assert.h>

#include <mrkcommon/bytes.h>
#include <mrkcommon/bytestream_aux.h>
#include <mrkcommon/dumpm.h>
#include <mrkcommon/util.h>

#include <mrkprotobuf.h>

#include "data/vector-01.h"

#include "unittest.h"

#ifndef NDEBUG
const char *_malloc_options = "AJ";
#endif

static mnbytes_t _foo = BYTES_INITIALIZER("FOO");
static mnbytes_t _john = BYTES_INITIALIZER("John");
static mnbytes_t _doe = BYTES_INITIALIZER("DoeJR");


int
main(void)
{
    struct vector_01 *vec0, *vec1;

    uint32_t *tag;
    struct vector_01_Extra *ex;
    mnbytes_t **thing;

    mnbytestream_t bs0, bs1;
    mnbytes_t *s;
    ssize_t sz;

    vec0 = vector_01_new();
    assert(vec0 != NULL);
    vec1 = vector_01_new();
    assert(vec1 != NULL);

    vec0->id = 123;
    tag = vector_01_tags_alloc(vec0, 3);
    tag[0] = 0x11223344;
    tag[2] = 0x22334455;
    (void)vector_01_tags_alloc(vec0, 10);
    ex = vector_01_extra_alloc(vec0, 1);
    thing = vector_01_Extra_things_alloc(ex, 3);
    thing[0] = &_foo;
    BYTES_INCREF(thing[0]);
    //thing[1] = &_john;
    //BYTES_INCREF(thing[1]);
    thing[2] = &_doe;
    BYTES_INCREF(thing[2]);

    (void)bytestream_init(&bs0, 32);

    sz = vector_01_pack(&bs0, vec0);
    D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("sz=%zd", sz);

    s = bytes_new_from_mem_len(SPDATA(&bs0), SEOD(&bs0));
    bytestream_from_bytes(&bs1, s);
    SEOD(&bs1) = BSZ(s);
    D8(SPDATA(&bs1), SEOD(&bs1));

    sz = vector_01_unpack(&bs1, NULL, vec1);
    TRACE("sz=%zd spos=%ld", sz, (long)SPOS(&bs1));

    bytestream_rewind(&bs0);
    sz = vector_01_dump(&bs0, vec0);
    //D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("dump: %s", SPDATA(&bs0));
    bytestream_rewind(&bs0);
    sz = vector_01_dump(&bs0, vec1);
    //D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("dump: %s", SPDATA(&bs0));

    vector_01_destroy(&vec0);
    assert(vec0 == NULL);
    vector_01_destroy(&vec1);
    assert(vec1 == NULL);

    bytestream_fini(&bs0);
    //bytestream_fini(&bs1);
    BYTES_DECREF(&s);

    BYTES_NREF_STATIC_INVARIANT(_foo);
    BYTES_NREF_STATIC_INVARIANT(_john);
    BYTES_NREF_STATIC_INVARIANT(_doe);

    return 0;
}

