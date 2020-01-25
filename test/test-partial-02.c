#include <assert.h>

#include <mncommon/bytes.h>
#include <mncommon/bytestream_aux.h>
#include <mncommon/dumpm.h>
#include <mncommon/util.h>

#include <mnprotobuf.h>

#include "data/partial-02.h"

#include "unittest.h"

#ifndef NDEBUG
const char *_malloc_options = "AJ";
#endif

static mnbytes_t _foo = BYTES_INITIALIZER("FOO");
static mnbytes_t _john = BYTES_INITIALIZER("John");
static mnbytes_t _doe = BYTES_INITIALIZER("DoeJR");
static mnbytes_t _the = BYTES_INITIALIZER("the");
static mnbytes_t _quick = BYTES_INITIALIZER("quick");
static mnbytes_t _brown = BYTES_INITIALIZER("brown");
static mnbytes_t _fox = BYTES_INITIALIZER("fox");
static mnbytes_t _jumps = BYTES_INITIALIZER("jumps");
static mnbytes_t _over = BYTES_INITIALIZER("over");
static mnbytes_t _lazy = BYTES_INITIALIZER("lazy");
static mnbytes_t _dog = BYTES_INITIALIZER("DOG");


int
main(void)
{
    struct partial_02 *part0, *part00, *part1, *part11;

    mnbytestream_t bs0, bs1;
    mnbytes_t *s, **thing;
    UNUSED ssize_t sz;

    (void)bytestream_init(&bs0, 32);
    //(void)bytestream_init(&bs1, 32);

    part0 = partial_02_new();
    part00 = partial_02_new();
    part1 = partial_02_new();
    part11 = partial_02_new();

    thing = partial_02_things_alloc(part0, 1);

    *thing = &_john;
    sz = partial_02_pack(&bs0, part0);
    *thing = &_doe;
    sz = partial_02_pack(&bs0, part0);
    *thing = &_the;
    sz = partial_02_pack(&bs0, part0);
    *thing = &_quick;
    sz = partial_02_pack(&bs0, part0);
    *thing = &_brown;
    sz = partial_02_pack(&bs0, part0);
    *thing = &_fox;
    sz = partial_02_pack(&bs0, part0);
    *thing = &_jumps;
    sz = partial_02_pack(&bs0, part0);
    *thing = &_over;
    sz = partial_02_pack(&bs0, part0);
    *thing = &_the;
    sz = partial_02_pack(&bs0, part0);
    *thing = &_lazy;
    sz = partial_02_pack(&bs0, part0);
    *thing = &_dog;
    sz = partial_02_pack(&bs0, part0);
    *thing = &_foo;
    sz = partial_02_pack(&bs0, part0);
    *thing = NULL;

    s = bytes_new_from_mem_len(SPDATA(&bs0), SEOD(&bs0));
    bytestream_from_bytes(&bs1, s);
    SEOD(&bs1) = BSZ(s);
    D8(SPDATA(&bs1), SEOD(&bs1));

    sz = partial_02_unpack(&bs1, NULL, part1);

    bytestream_rewind(&bs0);
    sz = partial_02_dump(&bs0, part1);
    TRACE("dump part1: %s", SPDATA(&bs0));

    bytestream_rewind(&bs0);
    sz = partial_02_pack(&bs0, part1);
    D8(SPDATA(&bs0), SEOD(&bs0));

    thing = partial_02_things_alloc(part00, 10);
    thing[0] = &_foo;
    BYTES_INCREF(&_foo);
    thing[1] = &_the;
    BYTES_INCREF(&_the);
    thing[2] = &_quick;
    BYTES_INCREF(&_quick);
    thing[3] = &_brown;
    BYTES_INCREF(&_brown);
    thing[4] = &_fox;
    BYTES_INCREF(&_fox);
    thing[5] = &_jumps;
    BYTES_INCREF(&_jumps);
    thing[6] = &_over;
    BYTES_INCREF(&_over);
    thing[7] = &_the;
    BYTES_INCREF(&_the);
    thing[8] = &_lazy;
    BYTES_INCREF(&_lazy);
    thing[9] = &_dog;
    BYTES_INCREF(&_dog);

    bytestream_rewind(&bs0);
    sz = partial_02_pack(&bs0, part00);
    D8(SPDATA(&bs0), SEOD(&bs0));

    BYTES_DECREF(&s);
    s = bytes_new_from_mem_len(SPDATA(&bs0), SEOD(&bs0));
    bytestream_from_bytes(&bs1, s);
    SEOD(&bs1) = BSZ(s);
    sz = partial_02_unpack(&bs1, NULL, part11);

    bytestream_rewind(&bs0);
    sz = partial_02_dump(&bs0, part00);
    TRACE("dump part00: %s", SPDATA(&bs0));

    bytestream_rewind(&bs0);
    sz = partial_02_dump(&bs0, part11);
    TRACE("dump part11: %s", SPDATA(&bs0));

    partial_02_destroy(&part0);
    partial_02_destroy(&part00);
    partial_02_destroy(&part1);
    partial_02_destroy(&part11);

    bytestream_fini(&bs0);
    //bytestream_fini(&bs1);
    BYTES_DECREF(&s);

    BYTES_NREF_STATIC_INVARIANT(_foo);
    BYTES_NREF_STATIC_INVARIANT(_john);
    BYTES_NREF_STATIC_INVARIANT(_doe);
    BYTES_NREF_STATIC_INVARIANT(_the);
    BYTES_NREF_STATIC_INVARIANT(_quick);
    BYTES_NREF_STATIC_INVARIANT(_brown);
    BYTES_NREF_STATIC_INVARIANT(_fox);
    BYTES_NREF_STATIC_INVARIANT(_jumps);
    BYTES_NREF_STATIC_INVARIANT(_over);
    BYTES_NREF_STATIC_INVARIANT(_lazy);
    BYTES_NREF_STATIC_INVARIANT(_dog);
}
