%{
#include <stdlib.h>
#include <limits.h>

#include <mncommon/bytes.h>
#include <mncommon/dumpm.h>

#include "mnpbc.h"

int yylex (void);
void yyerror (mnpbc_ctx_t *ctx, const char *);
extern char *yytext;

extern mnbytes_t *yydqstr;

#define YY_NO_UNPUT
#define YY_NO_INPUT

#define YYERROR_VERBOSE

static mnbytes_t _proto3 = BYTES_INITIALIZER("proto3");

%}

%parse-param {mnpbc_ctx_t *ctx}
%locations

%union {
    mnbytes_t *str;
    int num;
}

%token YYEOF 0

%token MNPBC_SYNTAX MNPBC_MESSAGE MNPBC_RESERVED MNPBC_REPEATED MNPBC_ENUM MNPBC_OPTION MNPBC_ONEOF MNPBC_IMPORT MNPBC_PACKAGE MNPBC_SEMI MNPBC_LCURLY MNPBC_RCURLY MNPBC_EQUALS MNPBC_NZNUM MNPBC_ZNUM MNPBC_DQSTR MNPBC_TOKEN

%token MNPBC_BUILTIN_TYPE

%type <str> token builtin type qstr
%type <num> tag etag
%destructor { BYTES_DECREF(&$$); } <str>

%start proto

%%

qstr:
    MNPBC_DQSTR {
        $$ = yydqstr;
        yydqstr = NULL;
    }
    ;

token:
    MNPBC_TOKEN {
        $$ = bytes_new_from_str(yytext);
    }
    ;

builtin:
    MNPBC_BUILTIN_TYPE {
        $$ = bytes_new_from_str(yytext);
    }
    ;

type:
    builtin | token ;


tag:
    MNPBC_NZNUM {
        $$ = (int)strtol(yytext, NULL, 10);
    };

etag:
    MNPBC_NZNUM {
        $$ = (int)strtol(yytext, NULL, 10);
    }
    |
    MNPBC_ZNUM {
        $$ = (int)strtol(yytext, NULL, 10);
    };

//sbfield:
//     builtin token MNPBC_EQUALS tag MNPBC_SEMI {
//        mnpbc_container_t *cont;
//
//        if ((cont = mnpbc_ctx_top_container(ctx)) != NULL) {
//            int res;
//
//            if ((res = mnpbc_container_add_field(cont,
//                                                  $1,
//                                                  $2,
//                                                  $4,
//                                                  0)) != 0) {
//                YYERROR;
//            }
//        }
//     };

sfield:
     type token MNPBC_EQUALS tag MNPBC_SEMI {
        mnpbc_container_t *cont;

        if ((cont = mnpbc_ctx_top_container(ctx)) != NULL) {
            int res;

            if ((res = mnpbc_container_add_field(cont,
                                                  $1,
                                                  $2,
                                                  $4,
                                                  0)) != 0) {
                YYERROR;
            }
        }
     };

rfield:
     MNPBC_REPEATED type token MNPBC_EQUALS tag MNPBC_SEMI {
        mnpbc_container_t *cont;

        if ((cont = mnpbc_ctx_top_container(ctx)) != NULL) {
            int res;

            if ((res = mnpbc_container_add_field(cont,
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
    token MNPBC_EQUALS etag MNPBC_SEMI {
        mnpbc_container_t *cont;

        if ((cont = mnpbc_ctx_top_container(ctx)) != NULL) {
            int res;

            if ((res = mnpbc_container_add_field(cont,
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
    MNPBC_ENUM token {
        mnpbc_container_t *parent, *cont;

        parent = mnpbc_ctx_top_container(ctx);
        if ((cont = mnpbc_ctx_add_container(ctx,
                                             parent,
                                             $2,
                                             MNPBC_CONT_KENUM)) == NULL) {
            YYERROR;
        } else {
            mnpbc_ctx_push_container(ctx, cont);
        }
    }
    ;

enum:
    estart MNPBC_LCURLY eitems MNPBC_RCURLY {
        mnpbc_ctx_pop_container(ctx);
    }
    ;

//sbfields:
//    | sbfield sbfields;

sfields:
    | sfield sfields;

ostart:
    MNPBC_ONEOF token {
        mnpbc_container_t *parent, *cont;

        if ((parent = mnpbc_ctx_top_container(ctx)) != NULL) {
            if ((cont = mnpbc_ctx_add_container(
                    ctx, parent, $2, MNPBC_CONT_KONEOF)) == NULL) {
                YYERROR;
            } else {
                mnpbc_ctx_push_container(ctx, cont);
            }
        }
    }
    ;

oneof:
    ostart MNPBC_LCURLY sfields MNPBC_RCURLY {
        mnpbc_container_t *un, *cont;
        int res;

        un = mnpbc_ctx_top_container(ctx);
        assert(un != NULL);
        mnpbc_ctx_pop_container(ctx);
        cont = mnpbc_ctx_top_container(ctx);
        assert(cont != NULL);
        if ((res = mnpbc_container_add_field(cont,
                                              un->pb.fqname,
                                              un->pb.name,
                                              MNPBC_FNUM_ONEOF,
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
    MNPBC_MESSAGE token {
        mnpbc_container_t *parent, *cont;

        parent = mnpbc_ctx_top_container(ctx);
        if ((cont = mnpbc_ctx_add_container(ctx,
                                             parent,
                                             $2,
                                             MNPBC_CONT_KMESSAGE)) == NULL) {
            YYERROR;
        } else {
            mnpbc_ctx_push_container(ctx, cont);
        }
    }
    ;

message:
    mstart MNPBC_LCURLY mitems MNPBC_RCURLY {
        mnpbc_ctx_pop_container(ctx);
    };

pitem:
     message | enum

pitems:
    | pitem pitems;

syntax:
    MNPBC_SYNTAX MNPBC_EQUALS qstr MNPBC_SEMI {
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
 *  - sub-messages in oneof
 *  - oneof of unknown fnum
 *  - map<> support
 *  - service
 *  - json
 *  - options: deprecated
 */
/*
 * vim:softtabstop=4
 */
