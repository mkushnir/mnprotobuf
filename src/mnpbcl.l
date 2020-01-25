%{
#pragma GCC diagnostic ignored "-Wunused-function"

#include <mncommon/bytes.h>
#include <mncommon/dumpm.h>
#include <mncommon/util.h>

#include "mnpbc.h"
#include "mnpbcg.h"

#include "diag.h"

#define YY_NO_UNPUT
#define YY_NO_INPUT
#define YY_NEVER_INTERACTIVE 1

static char *yydqstr0, *yydqstr1;
mnbytes_t *yydqstr = NULL;

static void
yydqstart(void)
{
    yydqstr0 = yytext + 1;
}

static void
yydqend(void)
{
    yydqstr1 = yytext;
    BYTES_DECREF(&yydqstr);
    yydqstr = bytes_new_from_str_len(yydqstr0, yydqstr1 - yydqstr0);
    BYTES_INCREF(yydqstr);
}
%}


SPACE [ \t\r\n]
DCHR [^"]
SCHR [^']
DIGIT0 [0-9]
DIGIT1 [1-9]
ALPHA [a-zA-Z_]
ALNUM [a-zA-Z0-9_]
WORD [a-zA-Z0-9._]
HEX [a-fA-F0-9]

%x DQ
%x SQ
%x C1

%option yylineno

%%

syntax      return MNPBC_SYNTAX;
message     return MNPBC_MESSAGE;
reserved    return MNPBC_RESERVED;
repeated    return MNPBC_REPEATED;
enum        return MNPBC_ENUM;
option      return MNPBC_OPTION;
oneof       return MNPBC_ONEOF;
import      return MNPBC_IMPORT;
package     return MNPBC_PACKAGE;

double|float|int32|int64|uint32|uint64|sint32|sint64|fixed32|fixed64|sfixed32|sfixed64|bool|string|bytes    return MNPBC_BUILTIN_TYPE;

;           return MNPBC_SEMI;
\{          return MNPBC_LCURLY;
\}          return MNPBC_RCURLY;
=           return MNPBC_EQUALS;

{DIGIT1}{DIGIT0}*            return MNPBC_NZNUM;
0|{DIGIT1}{DIGIT0}*   return MNPBC_ZNUM;

\"          BEGIN(DQ); yydqstart();
<DQ>\\\" | <DQ>\\\\ ;
<DQ>\\a     ;
<DQ>\\b     ;
<DQ>\\f     ;
<DQ>\\n     ;
<DQ>\\r     ;
<DQ>\\t     ;
<DQ>\\v     ;
<DQ>[^"\\]+ ;
<DQ>\"      BEGIN(INITIAL); yydqend(); return MNPBC_DQSTR;


\/\*.*?\*\/ BEGIN(C1);BEGIN(INITIAL); /* TRACE("C1") */;
\/\*.*      BEGIN(C1); /* TRACE("C1") */;
<C1>.*?\*\/ BEGIN(INITIAL); /* TRACE("IN") */;
<C1>{SPACE}+ ; //{ TRACE("SPACE"); }
<C1>.*      /* TRACE("...C1") */;

\/\/.*      ;


{SPACE}+    ; //{ TRACE("SPACE"); }

{ALPHA}{WORD}*     return MNPBC_TOKEN;

<<EOF>>     return 0;


%%

void
yyerror(UNUSED mnpbc_ctx_t *ctx, const char *s)
{
    TRACE("error: %s", s);
}

int
mnpbc_error(void)
{
    return 0;
}

int
mnpbc_scan(mnpbc_ctx_t *ctx)
{
    int res;
    yyin = ctx->in;
    yylineno = 1;
    if ((res = yyparse(ctx)) != 0) {
        TRACE("parse error at line %d: %s", yylineno, yytext);
    }
    yylex_destroy();
    return res;
}
