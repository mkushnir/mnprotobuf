#include <assert.h>

#include <mncommon/bytes.h>
#include <mncommon/bytestream_aux.h>
#include <mncommon/dumpm.h>
#include <mncommon/util.h>

#include <mnprotobuf.h>

#include "data/scalar-01.h"

#include "unittest.h"

#ifndef NDEBUG
const char *_malloc_options = "AJ";
#endif

static mnbytes_t _foo = BYTES_INITIALIZER("FOO");


int
main(void)
{
    struct scalar_01 *sc0, *sc1;
    mnbytestream_t bs0, bs1;
    mnbytes_t *s;
    ssize_t sz;

    sc0 = scalar_01_new();
    assert(sc0 != NULL);
    sc1 = scalar_01_new();
    assert(sc1 != NULL);

    sc0->id = 123;
    sc0->name = &_foo;
    BYTES_INCREF(sc0->name);
    sc0->weight = 234;
    sc0->impression = EXCELLENT;

    (void)bytestream_init(&bs0, 32);

    sz = scalar_01_pack(&bs0, sc0);
    D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("sz=%zd", sz);

    s = bytes_new_from_mem_len(SPDATA(&bs0), SEOD(&bs0));
    bytestream_from_bytes(&bs1, s);
    SEOD(&bs1) = BSZ(s);
    //D8(BDATA(s), BSZ(s));

    sz = scalar_01_unpack(&bs1, NULL, sc1);
    TRACE("sz=%zd", sz);


    bytestream_rewind(&bs0);
    sz = scalar_01_dump(&bs0, sc0);
    //D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("dump: %s", SPDATA(&bs0));
    bytestream_rewind(&bs0);
    sz = scalar_01_dump(&bs0, sc1);
    //D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("dump: %s", SPDATA(&bs0));

    scalar_01_destroy(&sc0);
    assert(sc0 == NULL);
    scalar_01_destroy(&sc1);
    assert(sc1 == NULL);

    bytestream_fini(&bs0);
    //bytestream_fini(&bs1);
    BYTES_DECREF(&s);

    BYTES_NREF_STATIC_INVARIANT(_foo);

    return 0;
}
