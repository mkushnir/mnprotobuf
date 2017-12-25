%{
#pragma GCC diagnostic ignored "-Wunused-function"

#include <mrkcommon/bytes.h>
#include <mrkcommon/dumpm.h>
#include <mrkcommon/util.h>

#include "mrkpbc.h"
#include "mrkpbcg.h"

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

syntax      return MRKPBC_SYNTAX;
message     return MRKPBC_MESSAGE;
reserved    return MRKPBC_RESERVED;
repeated    return MRKPBC_REPEATED;
enum        return MRKPBC_ENUM;
option      return MRKPBC_OPTION;
oneof       return MRKPBC_ONEOF;
import      return MRKPBC_IMPORT;
package     return MRKPBC_PACKAGE;

double|float|int32|int64|uint32|uint64|sint32|sint64|fixed32|fixed64|sfixed32|sfixed64|bool|string|bytes    return MRKPBC_BUILTIN_TYPE;

;           return MRKPBC_SEMI;
\{          return MRKPBC_LCURLY;
\}          return MRKPBC_RCURLY;
=           return MRKPBC_EQUALS;

{DIGIT1}{DIGIT0}*            return MRKPBC_NZNUM;
0|{DIGIT1}{DIGIT0}*   return MRKPBC_ZNUM;

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
<DQ>\"      BEGIN(INITIAL); yydqend(); return MRKPBC_DQSTR;


\/\*.*?\*\/ BEGIN(C1);BEGIN(INITIAL); /* TRACE("C1") */;
\/\*.*      BEGIN(C1); /* TRACE("C1") */;
<C1>.*?\*\/ BEGIN(INITIAL); /* TRACE("IN") */;
<C1>{SPACE}+ ; //{ TRACE("SPACE"); }
<C1>.*      /* TRACE("...C1") */;

\/\/.*      ;


{SPACE}+    ; //{ TRACE("SPACE"); }

{ALPHA}{WORD}*     return MRKPBC_TOKEN;

<<EOF>>     return 0;


%%

void
yyerror(UNUSED mrkpbc_ctx_t *ctx, const char *s)
{
    TRACE("error: %s", s);
}

int
mrkpbc_error(void)
{
    return 0;
}

int
mrkpbc_scan(mrkpbc_ctx_t *ctx)
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
