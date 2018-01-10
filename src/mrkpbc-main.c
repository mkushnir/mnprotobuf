#include <assert.h>
#include <err.h>
#include <getopt.h>
#include <libgen.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

#ifdef HAVE_CONfIG_H
#   include "config.h"
#endif
#ifdef HAVE_MALLOC_H
#   include <malloc.h>
#endif

#include <mrkcommon/dumpm.h>
#include <mrkcommon/util.h>

#include "diag.h"
#include "mrkpbc.h"

#ifndef NDEBUG
const char *_malloc_options = "AJ";
#endif

static struct option longopts[] = {
#define GENDATA_OPT_HELP    1
    {"help", no_argument, NULL, 'h'},
#define GENDATA_OPT_HFILE   2
    {"hfile", required_argument, NULL, 'H'},
#define GENDATA_OPT_CFILE   3
    {"cfile", required_argument, NULL, 'C'},
    {NULL, 0, NULL, 0},
};


static void
usage(char *progname)
{
    printf("Usage: %s [OPTIONS] [file.proto]\n"
        "\n"
        "Options:\n"
        "  -h, --help               Print help message and exit.\n"
        "  -H, --hfile              Path to C header file.  Default to <file.proto>.h.\n"
        "  -C, --cfile              Path to C source file.  Default to <file.proto>.c.\n"
        "\n",
        basename(progname));
}


int
main(UNUSED int argc, char **argv)
{
    char ch;
    int res;
    mrkpbc_ctx_t ctx;
    mnbytes_t *namein, *nameout0, *nameout1;
    FILE *in, *out0, *out1;

#ifdef HAVE_MALLOC_H
#   ifndef NDEBUG
    /*
     * malloc options
     */
    if (mallopt(M_CHECK_ACTION, 1) != 1) {
        FAIL("mallopt");
    }
    if (mallopt(M_PERTURB, 0x5a) != 1) {
        FAIL("mallopt");
    }
#   endif
#endif

    while ((ch = getopt_long(argc, argv, "h", longopts, NULL)) != -1) {
        switch (ch) {
        case 'h':
            usage(argv[0]);
            exit(0);
            break;

        case ':':
            /* missing option argument */
            usage(argv[0]);
            errx(1, "Missing option argument");
            break;

        case '?':
            /* unknown option */
            usage(argv[0]);
            errx(1, "Unknown option");
            break;

        default:
            usage(argv[0]);
            errx(1, "Unknown error");
            break;

        }
    }

    argc -= optind;
    argv += optind;

    mrkpbc_ctx_init(&ctx);

    if (argc < 1) {
        namein = bytes_new_from_str("test");
        in = stdin;
        //nameout0 = namein;
        nameout0 = NULL;
        out0 = stdout;
        nameout1 = namein;
        out1 = stdout;

    } else {
        char *s0, *s1;
        char *fname;

        if ((s0 = strdup(argv[0])) == NULL) {
            errx(1, "strdup error");
        }
        if ((s1 = strdup(argv[0])) == NULL) {
            errx(1, "strdup error");
        }
        fname = basename(s1);

        namein = bytes_new_from_str(fname);

        nameout0 = bytes_printf("%s.h", argv[0]);
        nameout1 = bytes_printf("%s.c", argv[0]);

        if ((in = fopen(argv[0], "r")) == NULL) {
            errx(1, "fopen error on argv[0]");
        }

        if ((out0 = fopen((char *)BDATA(nameout0), "w")) == NULL) {
            errx(1, "fopen error on hfile");
        }
        if ((out1 = fopen((char *)BDATA(nameout1), "w")) == NULL) {
            errx(1, "fopen error on hfile");
        }

        free(s0);
        free(s1);
    }

    mrkpbc_ctx_init_c(&ctx,
                      namein,
                      in,
                      nameout0,
                      out0,
                      nameout1,
                      out1);
    if ((res = mrkpbc_scan(&ctx)) != 0) {
        goto end;
    }
    if ((mrkpbc_ctx_validate(&ctx)) != 0) {
        goto end;
    }

    mrkpbc_ctx_render_c(&ctx);

end:
    mrkpbc_ctx_fini(&ctx);

    return 0;
}
