#include <assert.h>

#include <mrkcommon/bytes.h>
#include <mrkcommon/bytestream_aux.h>
#include <mrkcommon/dumpm.h>
#include <mrkcommon/util.h>

#include <mrkprotobuf.h>

#include "data/partial-01.proto.h"

#include "unittest.h"

#ifndef NDEBUG
const char *_malloc_options = "AJ";
#endif

static mnbytes_t _foo = BYTES_INITIALIZER("FOO");
static mnbytes_t _john = BYTES_INITIALIZER("John");
static mnbytes_t _doe = BYTES_INITIALIZER("DoeJR");

static ssize_t
myreadmore(UNUSED mnbytestream_t *bs,
           UNUSED void *fd,
           UNUSED ssize_t sz)
{
    return -1;
}

//#define MRK_INTERLEAVE

int
main(void)
{
    struct partial_01 *part0, *part1, *part2, *part3, *part4, *part5, *part6, *part7, *part8, *part9, *part10, *part11, *part12, *part13, *part14, *part15;

    mnbytestream_t bs0, bs1;
    mnbytes_t *s;
    ssize_t sz;

    part0 = partial_01_new();
    part1 = partial_01_new();
    part2 = partial_01_new();
    part3 = partial_01_new();
    part4 = partial_01_new();
    part5 = partial_01_new();
    part6 = partial_01_new();
    part7 = partial_01_new();
    part8 = partial_01_new();
    part9 = partial_01_new();
    part10 = partial_01_new();
    part11 = partial_01_new();
    part12 = partial_01_new();
    part13 = partial_01_new();
    part14 = partial_01_new();
    part15 = partial_01_new();

    (void)bytestream_init(&bs0, 32);
    bs0.read_more = myreadmore;

    part1->i32 = 1;
    part2->ui32 = 2;
    part3->si32 = 3;
    part4->f32 = 4;
    part5->sf32 = 5;
    part6->i64 = 6;
    part7->ui64 = 7;
    part8->si64 = 8;
    part9->f64 = 9;
    part10->sf64 = 10;
    part11->f = 11;
    part12->d = 12;
    part13->s = &_foo;
    BYTES_INCREF(&_foo);
    part14->b = &_john;
    BYTES_INCREF(&_john);
    part15->bb = 1;

    sz = partial_01_pack(&bs0, part1);
    D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("sz=%zd", sz);

#ifdef MRK_INTERLEAVE
    sz = partial_01_unpack(&bs0, NULL, part0);
    TRACE("sz=%zd spos=%ld", sz, (long)SPOS(&bs0));
#endif

    sz = partial_01_pack(&bs0, part2);
    D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("sz=%zd", sz);

#ifdef MRK_INTERLEAVE
    sz = partial_01_unpack(&bs0, NULL, part0);
    TRACE("sz=%zd spos=%ld", sz, (long)SPOS(&bs0));
#endif

    sz = partial_01_pack(&bs0, part3);
    D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("sz=%zd", sz);

#ifdef MRK_INTERLEAVE
    sz = partial_01_unpack(&bs0, NULL, part0);
    TRACE("sz=%zd spos=%ld", sz, (long)SPOS(&bs0));
#endif

    sz = partial_01_pack(&bs0, part4);
    D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("sz=%zd", sz);

#ifdef MRK_INTERLEAVE
    sz = partial_01_unpack(&bs0, NULL, part0);
    TRACE("sz=%zd spos=%ld", sz, (long)SPOS(&bs0));
#endif

    sz = partial_01_pack(&bs0, part5);
    D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("sz=%zd", sz);

#ifdef MRK_INTERLEAVE
    sz = partial_01_unpack(&bs0, NULL, part0);
    TRACE("sz=%zd spos=%ld", sz, (long)SPOS(&bs0));
#endif

    sz = partial_01_pack(&bs0, part6);
    D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("sz=%zd", sz);

#ifdef MRK_INTERLEAVE
    sz = partial_01_unpack(&bs0, NULL, part0);
    TRACE("sz=%zd spos=%ld", sz, (long)SPOS(&bs0));
#endif

    sz = partial_01_pack(&bs0, part7);
    D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("sz=%zd", sz);

#ifdef MRK_INTERLEAVE
    sz = partial_01_unpack(&bs0, NULL, part0);
    TRACE("sz=%zd spos=%ld", sz, (long)SPOS(&bs0));
#endif

    sz = partial_01_pack(&bs0, part8);
    D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("sz=%zd", sz);

#ifdef MRK_INTERLEAVE
    sz = partial_01_unpack(&bs0, NULL, part0);
    TRACE("sz=%zd spos=%ld", sz, (long)SPOS(&bs0));
#endif

    sz = partial_01_pack(&bs0, part9);
    D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("sz=%zd", sz);

#ifdef MRK_INTERLEAVE
    sz = partial_01_unpack(&bs0, NULL, part0);
    TRACE("sz=%zd spos=%ld", sz, (long)SPOS(&bs0));
#endif

    sz = partial_01_pack(&bs0, part10);
    D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("sz=%zd", sz);

#ifdef MRK_INTERLEAVE
    sz = partial_01_unpack(&bs0, NULL, part0);
    TRACE("sz=%zd spos=%ld", sz, (long)SPOS(&bs0));
#endif

    sz = partial_01_pack(&bs0, part11);
    D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("sz=%zd", sz);

#ifdef MRK_INTERLEAVE
    sz = partial_01_unpack(&bs0, NULL, part0);
    TRACE("sz=%zd spos=%ld", sz, (long)SPOS(&bs0));
#endif

    sz = partial_01_pack(&bs0, part12);
    D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("sz=%zd", sz);

#ifdef MRK_INTERLEAVE
    sz = partial_01_unpack(&bs0, NULL, part0);
    TRACE("sz=%zd spos=%ld", sz, (long)SPOS(&bs0));
#endif

    sz = partial_01_pack(&bs0, part13);
    D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("sz=%zd", sz);

#ifdef MRK_INTERLEAVE
    sz = partial_01_unpack(&bs0, NULL, part0);
    TRACE("sz=%zd spos=%ld", sz, (long)SPOS(&bs0));
#endif

    sz = partial_01_pack(&bs0, part14);
    D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("sz=%zd", sz);

#ifdef MRK_INTERLEAVE
    sz = partial_01_unpack(&bs0, NULL, part0);
    TRACE("sz=%zd spos=%ld", sz, (long)SPOS(&bs0));
#endif

    sz = partial_01_pack(&bs0, part15);
    D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("sz=%zd", sz);

#ifdef MRK_INTERLEAVE
    sz = partial_01_unpack(&bs0, NULL, part0);
    TRACE("sz=%zd spos=%ld", sz, (long)SPOS(&bs0));
#endif

    s = bytes_new_from_mem_len(SPDATA(&bs0), SEOD(&bs0));
    bytestream_from_bytes(&bs1, s);
    SEOD(&bs1) = BSZ(s);
    //D8(SPDATA(&bs1), SEOD(&bs1));

#ifndef MRK_INTERLEAVE
    sz = partial_01_unpack(&bs1, NULL, part0);
    TRACE("sz=%zd spos=%ld", sz, (long)SPOS(&bs1));
#endif

    bytestream_rewind(&bs0);
    sz = partial_01_dump(&bs0, part0);
    //D8(SPDATA(&bs0), SEOD(&bs0));
    TRACE("dump: %s", SPDATA(&bs0));

    partial_01_destroy(&part0);
    partial_01_destroy(&part1);
    partial_01_destroy(&part2);
    partial_01_destroy(&part3);
    partial_01_destroy(&part4);
    partial_01_destroy(&part5);
    partial_01_destroy(&part6);
    partial_01_destroy(&part7);
    partial_01_destroy(&part8);
    partial_01_destroy(&part9);
    partial_01_destroy(&part10);
    partial_01_destroy(&part11);
    partial_01_destroy(&part12);
    partial_01_destroy(&part13);
    partial_01_destroy(&part14);
    partial_01_destroy(&part15);

    bytestream_fini(&bs0);
    //bytestream_fini(&bs1);
    BYTES_DECREF(&s);

    BYTES_NREF_STATIC_INVARIANT(_foo);
    BYTES_NREF_STATIC_INVARIANT(_john);
    BYTES_NREF_STATIC_INVARIANT(_doe);

    return 0;
}


