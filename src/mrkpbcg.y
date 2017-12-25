%{
#include <stdlib.h>
#include <limits.h>

#include <mrkcommon/bytes.h>
#include <mrkcommon/dumpm.h>

#include "mrkpbc.h"

int yylex (void);
void yyerror (mrkpbc_ctx_t *ctx, const char *);
extern char *yytext;

extern mnbytes_t *yydqstr;

#define YY_NO_UNPUT
#define YY_NO_INPUT

#define YYERROR_VERBOSE

static mnbytes_t _proto3 = BYTES_INITIALIZER("proto3");

%}

%parse-param {mrkpbc_ctx_t *ctx}
%locations

%union {
    mnbytes_t *str;
    int num;
}

%token YYEOF 0

%token MRKPBC_SYNTAX MRKPBC_MESSAGE MRKPBC_RESERVED MRKPBC_REPEATED MRKPBC_ENUM MRKPBC_OPTION MRKPBC_ONEOF MRKPBC_IMPORT MRKPBC_PACKAGE MRKPBC_SEMI MRKPBC_LCURLY MRKPBC_RCURLY MRKPBC_EQUALS MRKPBC_NZNUM MRKPBC_ZNUM MRKPBC_DQSTR MRKPBC_TOKEN

%token MRKPBC_BUILTIN_TYPE

%type <str> token builtin type qstr
%type <num> tag etag
%destructor { BYTES_DECREF(&$$); } <str>

%start proto

%%

qstr:
    MRKPBC_DQSTR {
        $$ = yydqstr;
        yydqstr = NULL;
    }
    ;

token:
    MRKPBC_TOKEN {
        $$ = bytes_new_from_str(yytext);
    }
    ;

builtin:
    MRKPBC_BUILTIN_TYPE {
        $$ = bytes_new_from_str(yytext);
    }
    ;

type:
    builtin | token ;


tag:
    MRKPBC_NZNUM {
        $$ = (int)strtol(yytext, NULL, 10);
    };

etag:
    MRKPBC_NZNUM {
        $$ = (int)strtol(yytext, NULL, 10);
    }
    |
    MRKPBC_ZNUM {
        $$ = (int)strtol(yytext, NULL, 10);
    };

sbfield:
     builtin token MRKPBC_EQUALS tag MRKPBC_SEMI {
        mrkpbc_container_t *cont;

        if ((cont = mrkpbc_ctx_top_container(ctx)) != NULL) {
            int res;

            if ((res = mrkpbc_container_add_field(cont,
                                                  $1,
                                                  $2,
                                                  $4,
                                                  0)) != 0) {
                YYERROR;
            }
        }
     };

sfield:
     type token MRKPBC_EQUALS tag MRKPBC_SEMI {
        mrkpbc_container_t *cont;

        if ((cont = mrkpbc_ctx_top_container(ctx)) != NULL) {
            int res;

            if ((res = mrkpbc_container_add_field(cont,
                                                  $1,
                                                  $2,
                                                  $4,
                                                  0)) != 0) {
                YYERROR;
            }
        }
     };

rfield:
     MRKPBC_REPEATED type token MRKPBC_EQUALS tag MRKPBC_SEMI {
        mrkpbc_container_t *cont;

        if ((cont = mrkpbc_ctx_top_container(ctx)) != NULL) {
            int res;

            if ((res = mrkpbc_container_add_field(cont,
                                                  $2,
                                                  $3,
                                                  $5,
                                                  1)) != 0) {
                YYERROR;
            }
        }
     };


field:
    sfield | rfield

eitem:
    token MRKPBC_EQUALS etag MRKPBC_SEMI {
        mrkpbc_container_t *cont;

        if ((cont = mrkpbc_ctx_top_container(ctx)) != NULL) {
            int res;

            if ((res = mrkpbc_container_add_field(cont,
                                                  NULL,
                                                  $1,
                                                  $3,
                                                  0)) != 0) {
                YYERROR;
            }
        }
    }
    ;

eitems:
    | eitem eitems;

estart:
    MRKPBC_ENUM token {
        mrkpbc_container_t *parent, *cont;

        parent = mrkpbc_ctx_top_container(ctx);
        if ((cont = mrkpbc_ctx_add_container(ctx,
                                             parent,
                                             $2,
                                             MRKPBC_CONT_KENUM)) == NULL) {
            YYERROR;
        } else {
            mrkpbc_ctx_push_container(ctx, cont);
        }
    }
    ;

enum:
    estart MRKPBC_LCURLY eitems MRKPBC_RCURLY {
        mrkpbc_ctx_pop_container(ctx);
    }
    ;

sbfields:
    | sbfield sbfields;

ostart:
    MRKPBC_ONEOF token {
        mrkpbc_container_t *parent, *cont;

        if ((parent = mrkpbc_ctx_top_container(ctx)) != NULL) {
            if ((cont = mrkpbc_ctx_add_container(
                    ctx, parent, $2, MRKPBC_CONT_KONEOF)) == NULL) {
                YYERROR;
            } else {
                mrkpbc_ctx_push_container(ctx, cont);
            }
        }
    }
    ;

oneof:
    ostart MRKPBC_LCURLY sbfields MRKPBC_RCURLY {
        mrkpbc_container_t *un, *cont;
        int res;

        un = mrkpbc_ctx_top_container(ctx);
        assert(un != NULL);
        mrkpbc_ctx_pop_container(ctx);
        cont = mrkpbc_ctx_top_container(ctx);
        assert(cont != NULL);
        if ((res = mrkpbc_container_add_field(cont,
                                              un->pb.fqname,
                                              un->pb.name,
                                              MRKPBC_FNUM_ONEOF,
                                              0)) != 0) {
            YYERROR;
        }
    }
    ;

mitem:
    field | enum | oneof | message; 

mitems:
    | mitem mitems;

mstart:
    MRKPBC_MESSAGE token {
        mrkpbc_container_t *parent, *cont;

        parent = mrkpbc_ctx_top_container(ctx);
        if ((cont = mrkpbc_ctx_add_container(ctx,
                                             parent,
                                             $2,
                                             MRKPBC_CONT_KMESSAGE)) == NULL) {
            YYERROR;
        } else {
            mrkpbc_ctx_push_container(ctx, cont);
        }
    }
    ;

message:
    mstart MRKPBC_LCURLY mitems MRKPBC_RCURLY {
        mrkpbc_ctx_pop_container(ctx);
    };

pitem:
     message | enum

pitems:
    | pitem pitems;

syntax:
    MRKPBC_SYNTAX MRKPBC_EQUALS qstr MRKPBC_SEMI {
        if (bytes_cmp($3, &_proto3) != 0) {
            TRACE("Syntax is not supported: %s", BDATA($3));
            YYERROR;
        }
    }
    ;

proto:
    syntax pitems YYEOF {
    };

%%

/*
 * TODO and edge cases:
 *
 *  - reserved, both message fields and enum
 *  - imports
 *  - packages
 *  - allow_alias in enum
 *  - compatibility:
 *      - int32, uint32, int64, uint64, and bool
 *      - sint32 and sint64
 *      - string and bytes
 *      - nested messages with bytes
 *      - fixed32 with sfixed32
 *      - fixed64 with sfixed64
 *      - enum with int32, uint32, int64, and uint64
 *  - unknown fields: unpack and ignore
 *  - the Any messages
 *  - oneof of unknown fnum
 *  - map<> support
 *  - service
 *  - json
 *  - options: deprecated
 */
/*
 * vim:softtabstop=4
 */
