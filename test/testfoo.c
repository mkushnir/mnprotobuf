#include <assert.h>

#include <mrkcommon/bytes.h>
#include <mrkcommon/bytestream_aux.h>
#include <mrkcommon/dumpm.h>
#include <mrkcommon/util.h>

#include <mrkprotobuf.h>

#include "unittest.h"

#ifndef NDEBUG
const char *_malloc_options = "AJ";
#endif


static MRKPB_DEF_INT64(id, 1, 0, 0);
static MRKPB_DEF_STR(name, 2, 0, 0);
MRKPB_DEF_MESSAGE(pair, 3, 0, 0,
        MRKPB_REF(id),
        MRKPB_REF(name),
        NULL);

MRKPB_DEF_FIELD(qwe, pair, 0, 0, 0);
MRKPB_DEF_FIELD(asd, pair, 0, 0, 0);
MRKPB_DEF_FIELD(zxc, pair, 0, 0, 0);

MRKPB_DEF_MESSAGE(msg, 6, 0, 0,
        MRKPB_REF(qwe),
        MRKPB_REF(asd),
        MRKPB_REF(zxc),
        MRKPB_REF(pair),
        MRKPB_REF(pair),
        MRKPB_REF(pair),
        NULL);


typedef struct _pair {
    int64_t id;
    mnbytes_t *name;
} pair_t;
typedef struct _msg {
    pair_t qwe;
    pair_t asd;
    pair_t zxc;
    pair_t f3;
    pair_t f4;
    pair_t f5;
} msg_t;

UNUSED static void
test0(void)
{
    struct {
        long rnd;
        int in;
        int expected;
    } data[] = {
        {0, 0, 0},
    };
    UNITTEST_PROLOG_RAND;

    FOREACHDATA {
        //TRACE("in=%d expected=%d", CDATA.in, CDATA.expected);
        assert(CDATA.in == CDATA.expected);
    }
}


static mnbytes_t b0 = BYTES_INITIALIZER("\xac\x02\xf0\xac\x02");

UNUSED static void
test1(void)
{
    ssize_t res;
    mnbytestream_t ins, outs;
    uint64_t v;

    bytestream_from_bytes(&ins, &b0);
    SEOD(&ins) += BSZ(&b0);

    bytestream_init(&outs, 1024);

    res = mrkpb_devarint(&ins, NULL, &v);
    TRACE("res=%ld v=%016lx spos=%ld seod=%ld", res, v, SPOS(&ins), SEOD(&ins));
    res = mrkpb_devarint(&ins, NULL, &v);
    TRACE("res=%ld v=%016lx spos=%ld seod=%ld", res, v, SPOS(&ins), SEOD(&ins));
    res = mrkpb_devarint(&ins, NULL, &v);
    TRACE("res=%ld v=%016lx spos=%ld seod=%ld", res, v, SPOS(&ins), SEOD(&ins));
    res = mrkpb_devarint(&ins, NULL, &v);
    TRACE("res=%ld v=%016lx spos=%ld seod=%ld", res, v, SPOS(&ins), SEOD(&ins));
    res = mrkpb_devarint(&ins, NULL, &v);
    TRACE("res=%ld v=%016lx spos=%ld seod=%ld", res, v, SPOS(&ins), SEOD(&ins));
    res = mrkpb_devarint(&ins, NULL, &v);
    TRACE("res=%ld v=%016lx spos=%ld seod=%ld", res, v, SPOS(&ins), SEOD(&ins));

    res = mrkpb_envarint(&outs, 300);
    TRACE("res=%ld", res);
    res = mrkpb_envarint(&outs, 0x9670);
    TRACE("res=%ld", res);
    res = mrkpb_envarint(&outs, 0x0);
    TRACE("res=%ld", res);
    res = mrkpb_envarint(&outs, 0x12345678);
    TRACE("res=%ld", res);
    res = mrkpb_envarint(&outs, 0xffffffffffffffff);
    TRACE("res=%ld", res);
    res = mrkpb_envarint(&outs, 0x7fffffffffffffff);
    TRACE("res=%ld", res);
    D8(SDATA(&outs, 0), SEOD(&outs));

    bytestream_fini(&outs);
}


UNUSED static void
test2(void)
{
    ssize_t res;
    mnbytestream_t outs;
    int64_t v;
    struct {
        long rnd;
        int64_t in;
    } data[] = {
        {0, -10},
        {0, -9},
        {0, -8},
        {0, -7},
        {0, -6},
        {0, -1},
        {0, 0},
        {0, 1},
        {0, 2},
        {0, 3},
        {0, 4},
        {0, 5},
        {0, -0x7fffffffffffffff},
        {0, 0x7fffffffffffffff},
    };

    bytestream_init(&outs, 1024);

    UNITTEST_PROLOG;

    FOREACHDATA {
        res = mrkpb_enzz64(&outs, CDATA.in);
        TRACE("res=%ld", res);
    }

    D8(SDATA(&outs, 0), SEOD(&outs));

    SPOS(&outs) = 0;
    FOREACHDATA {
        res = mrkpb_dezz64(&outs, NULL, &v);
        TRACE("res=%ld v=%ld spos=%ld seod=%ld", res, v, SPOS(&outs), SEOD(&outs));
    }

    bytestream_fini(&outs);
}


UNUSED static void
test3(void)
{
    ssize_t res;
    mnbytestream_t outs;
    int32_t v;
    struct {
        long rnd;
        int32_t in;
    } data[] = {
        {0, -10},
        {0, -9},
        {0, -8},
        {0, -7},
        {0, -6},
        {0, -1},
        {0, 0},
        {0, 1},
        {0, 2},
        {0, 3},
        {0, 4},
        {0, 5},
        {0, -0x7fffffff},
        {0, -0x7ffffffe},
        {0, 0x7fffffff},
    };

    bytestream_init(&outs, 1024);

    UNITTEST_PROLOG;

    FOREACHDATA {
        res = mrkpb_enzz32(&outs, CDATA.in);
        //TRACE("res=%ld", res);
    }

    D8(SDATA(&outs, 0), SEOD(&outs));

    SPOS(&outs) = 0;
    FOREACHDATA {
        res = mrkpb_dezz32(&outs, NULL, &v);
        TRACE("res=%ld v=%d spos=%ld seod=%ld", res, v, SPOS(&outs), SEOD(&outs));
    }

    bytestream_fini(&outs);
}


int
main(void)
{
    //test0();
    //test1();
    test2();
    test3();
    return 0;
}
