#include <assert.h>
#include <stdlib.h>

#include <mncommon/array.h>
#include <mncommon/hash.h>
#include <mncommon/bytes.h>

#define TRRET_DEBUG
#include <mncommon/dumpm.h>
#include <mncommon/util.h>

#include "diag.h"

#include "mnpbc.h"

mnbytes_t _int32 = BYTES_INITIALIZER("int32");
mnbytes_t _int64 = BYTES_INITIALIZER("int64");
mnbytes_t _uint32 = BYTES_INITIALIZER("uint32");
mnbytes_t _uint64 = BYTES_INITIALIZER("uint64");
mnbytes_t _sint32 = BYTES_INITIALIZER("sint32");
mnbytes_t _sint64 = BYTES_INITIALIZER("sint64");
mnbytes_t _fixed32 = BYTES_INITIALIZER("fixed32");
mnbytes_t _fixed64 = BYTES_INITIALIZER("fixed64");
mnbytes_t _sfixed32 = BYTES_INITIALIZER("sfixed32");
mnbytes_t _sfixed64 = BYTES_INITIALIZER("sfixed64");
mnbytes_t _float = BYTES_INITIALIZER("float");
mnbytes_t _double = BYTES_INITIALIZER("double");
mnbytes_t _string = BYTES_INITIALIZER("string");
mnbytes_t _bytes = BYTES_INITIALIZER("bytes");
mnbytes_t _bool = BYTES_INITIALIZER("bool");

static void mnpbc_container_dump(mnpbc_container_t *);


static mnpbc_field_t *
mnpbc_field_new(void)
{
    mnpbc_field_t *res;

    if (MNUNLIKELY((res = malloc(sizeof(mnpbc_field_t))) == NULL)) {
        FAIL("malloc");
    }
    res->parent = NULL;
    res->ty = NULL;
    res->cty = NULL;
    res->pb.name = NULL;
    res->pb.fqname = NULL;
    res->be.name = NULL;
    res->be.fqname = NULL;
    res->fnum = 0l;
    res->wtype = MNPB_WT_UNDEF;
    res->flags.repeated = 0;
    return res;
}


static void
mnpbc_field_destroy(mnpbc_field_t **field)
{
    if (*field != NULL) {
        BYTES_DECREF(&(*field)->ty);
        BYTES_DECREF(&(*field)->pb.name);
        BYTES_DECREF(&(*field)->pb.fqname);
        BYTES_DECREF(&(*field)->be.name);
        BYTES_DECREF(&(*field)->be.fqname);
        free(*field);
        *field = NULL;
    }
}


static int
mnpbc_field_item_fini(mnbytes_t *key, mnpbc_field_t *value)
{
    BYTES_DECREF(&key);
    mnpbc_field_destroy(&value);
    return 0;
}


static mnpbc_container_t *
mnpbc_field_get_type(mnpbc_field_t *field)
{
    mnpbc_container_t *ty;

    assert(field->parent != NULL);
    if (field->ty == NULL) {
        return NULL;
    }

    ty = mnpbc_ctx_get_container(field->parent->ctx, field->ty);
    if (ty == NULL) {
        mnbytes_t *fqty;

        fqty = mnpbc_container_fqname(field->parent, field->ty);
        ty = mnpbc_ctx_get_container(field->parent->ctx, fqty);
        BYTES_DECREF(&fqty);
    }
    return ty;
}


int
mnpbc_container_add_field(mnpbc_container_t *cont,
                           mnbytes_t *ty,
                           mnbytes_t *name,
                           int fnum,
                           int repeated)
{
    int res;
    mnpbc_field_t *field, **pfield;
    mnhash_item_t *hit;

    field = mnpbc_field_new();

    field->parent = cont;
    field->ty = ty;
    if (field->ty != NULL) {
        BYTES_INCREF(field->ty);
    }
    assert(name != NULL);
    field->pb.name = name;
    BYTES_INCREF(field->pb.name);
    field->pb.fqname = mnpbc_container_fqname(cont, name);
    BYTES_INCREF(field->pb.fqname);
    field->fnum = fnum;
    field->flags.repeated = repeated;

    if ((hit = hash_get_item(&cont->ctx->fields, field->pb.fqname)) != NULL) {
        TRACE("Validation error: duplicate field name (%s = %ld) in %s",
              BDATA(field->pb.name),
              (long)field->fnum,
              BDATA(cont->pb.fqname));
        res = MNPBC_CONTAINER_ADD_FIELD + 1;
        goto err;
    }
    hash_set_item(&cont->ctx->fields, field->pb.fqname, field);
    BYTES_INCREF(field->pb.fqname);

    if (MNUNLIKELY((pfield = array_incr(&cont->fields)) == NULL)) {
        FAIL("array_incr");
    }
    *pfield = field;
    res = 0;

end:
    TRRET(res);

err:
    mnpbc_field_destroy(&field);
    goto end;
}


static mnpbc_container_t *
mnpbc_container_new(void)
{
    mnpbc_container_t *res;

    if (MNUNLIKELY((res = malloc(sizeof(mnpbc_container_t))) == NULL)) {
        FAIL("malloc");
    }

    res->ctx = NULL;
    res->parent = NULL;
    res->pb.name = NULL;
    res->pb.fqname = NULL;
    res->be.name = NULL;
    res->be.fqname = NULL;
    res->be.encode = NULL;
    res->be.decode = NULL;
    res->be.sz = NULL;
    if (MNUNLIKELY(array_init(&res->fields,
                               sizeof(mnpbc_field_t *),
                               0,
                               NULL,
                               NULL) != 0)) {
        FAIL("array_init");
    }
    if (MNUNLIKELY(array_init(&res->containers,
                               sizeof(mnpbc_container_t *),
                               0,
                               NULL,
                               NULL))) {
        FAIL("array_init");
    }
    res->kind = 0;
    res->flags.allow_alias = 0;
    res->flags.visited = 0;

    return res;
}


static void
mnpbc_container_destroy(mnpbc_container_t **cont)
{
    if (*cont != NULL) {
        BYTES_DECREF(&(*cont)->pb.name);
        BYTES_DECREF(&(*cont)->pb.fqname);
        BYTES_DECREF(&(*cont)->be.name);
        BYTES_DECREF(&(*cont)->be.fqname);
        BYTES_DECREF(&(*cont)->be.encode);
        BYTES_DECREF(&(*cont)->be.decode);
        BYTES_DECREF(&(*cont)->be.sz);
        (void)array_fini(&(*cont)->fields);
        (void)array_fini(&(*cont)->containers);
        free(*cont);
        *cont = NULL;
    }
}


static int
mnpbc_container_item_fini(mnbytes_t *key, mnpbc_container_t *value)
{
    BYTES_DECREF(&key);
    mnpbc_container_destroy(&value);
    return 0;
}


mnbytes_t *
mnpbc_container_fqname(mnpbc_container_t *cont, mnbytes_t *name)
{
    mnbytes_t *res;

    assert(cont->pb.fqname != NULL);
    res = bytes_printf("%s.%s", BDATA(cont->pb.fqname), BDATA(name));
    return res;
}

void
mnpbc_container_set_pb_fqname(mnpbc_container_t *cont, mnbytes_t *fqname)
{
    BYTES_DECREF(&cont->pb.fqname);
    cont->pb.fqname = fqname;
    BYTES_INCREF(cont->pb.fqname);
}


void
mnpbc_container_set_be_encode(mnpbc_container_t *cont, mnbytes_t *encode)
{
    BYTES_DECREF(&cont->be.encode);
    cont->be.encode = encode;
    BYTES_INCREF(cont->be.encode);
}


void
mnpbc_container_set_be_decode(mnpbc_container_t *cont, mnbytes_t *decode)
{
    BYTES_DECREF(&cont->be.decode);
    cont->be.decode = decode;
    BYTES_INCREF(cont->be.decode);
}


void
mnpbc_container_set_be_sz(mnpbc_container_t *cont, mnbytes_t *sz)
{
    BYTES_DECREF(&cont->be.sz);
    cont->be.sz = sz;
    BYTES_INCREF(cont->be.sz);
}


void
mnpbc_container_set_be_rawsz(mnpbc_container_t *cont, mnbytes_t *rawsz)
{
    BYTES_DECREF(&cont->be.rawsz);
    cont->be.rawsz = rawsz;
    BYTES_INCREF(cont->be.rawsz);
}


void
mnpbc_container_set_be_dump(mnpbc_container_t *cont, mnbytes_t *dump)
{
    BYTES_DECREF(&cont->be.dump);
    cont->be.dump = dump;
    BYTES_INCREF(cont->be.dump);
}


void
mnpbc_container_traverse_fields(mnpbc_container_t *cont,
                                 array_traverser_t cb,
                                 void *udata)
{
    (void)array_traverse(&cont->fields, cb, udata);
}


void
mnpbc_container_traverse_containers(mnpbc_container_t *cont,
                                     array_traverser_t cb,
                                     void *udata)
{
    (void)array_traverse(&cont->containers, cb, udata);
}


mnpbc_container_t *
mnpbc_ctx_add_container(mnpbc_ctx_t *ctx,
                         mnpbc_container_t *parent,
                         mnbytes_t *name,
                         int kind)
{
    mnpbc_container_t *cont;
    mnhash_item_t *hit;

    cont = mnpbc_container_new();
    cont->ctx = ctx;
    cont->pb.name = name;
    BYTES_INCREF(cont->pb.name);

    if (parent == NULL) {
        cont->pb.fqname = name;
    } else {
        mnpbc_container_t **pcont;

        if (MNUNLIKELY((pcont = array_incr(&parent->containers)) == NULL)) {
            FAIL("array_incr");
        }
        *pcont = cont;
        cont->parent = parent;

        cont->pb.fqname = mnpbc_container_fqname(parent, name);
    }
    BYTES_INCREF(cont->pb.fqname);
    cont->kind = kind;

    if ((hit = hash_get_item(&ctx->containers, cont->pb.fqname)) != NULL) {
        TRACE("Validation error: duplicate container (%s)",
              BDATA(cont->pb.fqname));
        TR(MNPBC_CTX_ADD_CONTAINER + 1);
        goto err;
    }
    hash_set_item(&ctx->containers, cont->pb.fqname, cont);
    BYTES_INCREF(cont->pb.fqname);

end:
    return cont;

err:
    mnpbc_container_destroy(&cont);
    goto end;
}


mnpbc_container_t *
mnpbc_ctx_get_container(mnpbc_ctx_t *ctx, mnbytes_t *fqname)
{
    mnhash_item_t *hit;
    mnpbc_container_t *cont;

    if ((hit = hash_get_item(&ctx->containers, fqname)) == NULL) {
        cont = NULL;
    } else {
        cont = hit->value;
    }

    return cont;
}


mnpbc_container_t *
mnpbc_ctx_top_container(mnpbc_ctx_t *ctx)
{
    mnpbc_container_t **pcont;
    mnarray_iter_t it;

    if ((pcont = array_last(&ctx->stack, &it)) == NULL) {
        return NULL;
    }

    return *pcont;
}

void
mnpbc_ctx_push_container(mnpbc_ctx_t *ctx, mnpbc_container_t *cont)
{
    mnpbc_container_t **pcont;
    if (MNUNLIKELY((pcont = array_incr(&ctx->stack)) == NULL)) {
        FAIL("array_incr");
    }
    *pcont = cont;
}


void
mnpbc_ctx_pop_container(mnpbc_ctx_t *ctx)
{
    UNUSED mnpbc_container_t *cont;

    cont = mnpbc_ctx_top_container(ctx);
    (void)array_decr(&ctx->stack);
}


static int
reset_visited(UNUSED mnbytes_t *key,
              mnpbc_container_t *cont,
              UNUSED void *udata)
{
    cont->flags.visited = 0;
    return 0;
}


int
mnpbc_ctx_traverse(mnpbc_ctx_t *ctx, hash_traverser_t cb, void *udata)
{
    (void)hash_traverse(&ctx->containers,
                        (hash_traverser_t)reset_visited,
                        NULL);
    return hash_traverse(&ctx->containers, cb, udata);
}


static uint64_t
fnum_hash(void *v)
{
    return (uint64_t)(intptr_t)v;
}

static int
fnum_cmp(void *a, void *b)
{
    return MNCMP(a, b);
}


static int
validate_container(UNUSED mnbytes_t *key,
                   mnpbc_container_t *cont,
                   UNUSED void *udata)
{
    int res;
    mnhash_t fnums;
    mnpbc_field_t **field;
    mnarray_iter_t it;
    /*
     * unique fnums within a container
     *
     * reserved fnums 19000-19999
     *
     * the first enum is zero, values < 2**32
     *
     * oneof cannot be repeated
     *
     */

    res = 0;

    hash_init(&fnums,
              31,
              (hash_hashfn_t)fnum_hash,
              (hash_item_comparator_t)fnum_cmp,
              NULL);

    for (field = array_first(&cont->fields, &it);
         field != NULL;
         field = array_next(&cont->fields, &it)) {

        mnhash_item_t *hit;

        (*field)->cty = mnpbc_field_get_type(*field);

        if ((*field)->cty != NULL &&
            (*field)->cty->kind == MNPBC_CONT_KONEOF) {

            mnpbc_field_t **ufield;
            mnarray_iter_t uit;

            for (ufield = array_first(&(*field)->cty->fields, &uit);
                 ufield != NULL;
                 ufield = array_next(&(*field)->cty->fields, &uit)) {

                if ((hit = hash_get_item(
                        &fnums, (void *)(intptr_t)(*ufield)->fnum)) != NULL) {

                    TRACE("Validation error: duplicate field number "
                          "(%s = %ld) in %s",
                          BDATA((*ufield)->pb.name),
                          (long)(*ufield)->fnum,
                          BDATA(cont->pb.fqname));
                    res = MNPB_CTX_VALIDATE_DUPLICATE_FNUM;
                    goto end;

                } else {
                    hash_set_item(&fnums,
                                  (void *)(intptr_t)(*ufield)->fnum,
                                  NULL);
                }


            }
        } else {
            if ((hit = hash_get_item(
                    &fnums, (void *)(intptr_t)(*field)->fnum)) != NULL) {

                TRACE("Validation error: duplicate field number "
                      "(%s = %ld) in %s",
                      BDATA((*field)->pb.name),
                      (long)(*field)->fnum,
                      BDATA(cont->pb.fqname));
                res = MNPB_CTX_VALIDATE_DUPLICATE_FNUM;
                goto end;

            } else {
                hash_set_item(&fnums, (void *)(intptr_t)(*field)->fnum, NULL);
            }
        }
    }

    if (cont->kind != MNPBC_CONT_KENUM) {
        for (field = array_first(&cont->fields, &it);
             field != NULL;
             field = array_next(&cont->fields, &it)) {
            if (INB0(19000, (*field)->fnum, 19999)) {
                TRACE("Validation error: implementation reserved "
                      "field number (%s = %ld) in %s",
                      BDATA((*field)->pb.name),
                      (long)(*field)->fnum,
                      BDATA(cont->pb.fqname));
                res = MNPB_CTX_VALIDATE_ENUM_FNUM_RESERVED;
                goto end;
            }
        }
    }

    if (cont->kind == MNPBC_CONT_KENUM) {
        if ((field = array_get(&cont->fields, 0)) != NULL) {
            if ((*field)->fnum != 0) {
                TRACE("Validation error: first enum field nonzero "
                      "(%s = %ld) in %s",
                      BDATA((*field)->pb.name),
                      (long)(*field)->fnum,
                      BDATA(cont->pb.fqname));
                res = MNPB_CTX_VALIDATE_FIRST_ENUM_NONZERO;
                goto end;
            }
        }
    }

    if (cont->kind == MNPBC_CONT_KONEOF) {
        for (field = array_first(&cont->fields, &it);
             field != NULL;
             field = array_next(&cont->fields, &it)) {
            if ((*field)->flags.repeated) {
                res = MNPB_CTX_VALIDATE_ONEOF_REPEATED;
                goto end;
            }
        }
    }

end:
    hash_fini(&fnums);
    return res;
}


int
mnpbc_ctx_validate(mnpbc_ctx_t *ctx)
{
    int res;

    if ((res = mnpbc_ctx_traverse(ctx,
                                   (hash_traverser_t)validate_container,
                                   NULL)) != 0) {
        goto end;
    }

end:
    return res;
}

/*
 * dump
 */
static int
mnpbc_field_aitem_dump(mnpbc_field_t **field, UNUSED void *udata)
{
    TRACE(" %s %s%s = %ld",
          BDATASAFE((*field)->ty),
          BDATA((*field)->pb.fqname),
          (*field)->flags.repeated ? "[]" : "",
          (long)(*field)->fnum);
    return 0;
}


static int
mnpbc_container_aitem_dump(mnpbc_container_t **cont, UNUSED void *udata)
{
    mnpbc_container_dump(*cont);
    return 0;
}


static void
mnpbc_container_dump(mnpbc_container_t *cont)
{
    (void)array_traverse(&cont->containers,
                         (array_traverser_t)mnpbc_container_aitem_dump,
                         NULL);
    TRACE("%s %s",
          (
           cont->kind == MNPBC_CONT_KBUILTIN ? "<builtin>" :
           cont->kind == MNPBC_CONT_KMESSAGE ? "STRUCT" :
           cont->kind == MNPBC_CONT_KENUM ? "ENUM" :
           cont->kind == MNPBC_CONT_KONEOF ? "UNION" :
           "<unknown>"
          ),
          BDATA(cont->pb.fqname));
    (void)array_traverse(&cont->fields,
                         (array_traverser_t)mnpbc_field_aitem_dump,
                         NULL);
}


static int
mnpbc_container_hitem_dump(UNUSED mnbytes_t *key,
                            mnpbc_container_t *cont,
                            UNUSED void *udata)
{
    if (cont->parent == NULL) {
        mnpbc_container_dump(cont);
    }
    return 0;
}


void
mnpbc_ctx_dump(mnpbc_ctx_t *ctx)
{
    mnpbc_ctx_traverse(ctx,
                        (hash_traverser_t)mnpbc_container_hitem_dump,
                        NULL);
}


void
mnpbc_ctx_init(mnpbc_ctx_t *ctx)
{
    ctx->namein = NULL;
    ctx->nameout0 = NULL;
    ctx->nameout1 = NULL;
    ctx->in = NULL;
    ctx->out0 = NULL;
    ctx->out1 = NULL;
    hash_init(&ctx->containers,
              127,
              (hash_hashfn_t)bytes_hash,
              (hash_item_comparator_t)bytes_cmp,
              (hash_item_finalizer_t)mnpbc_container_item_fini);

    hash_init(&ctx->fields,
              521,
              (hash_hashfn_t)bytes_hash,
              (hash_item_comparator_t)bytes_cmp,
              (hash_item_finalizer_t)mnpbc_field_item_fini);

    if (MNUNLIKELY(array_init(&ctx->stack,
                               sizeof(mnpbc_container_t *),
                               0,
                               NULL,
                               NULL) != 0)) {
        FAIL("array_init");
    }
}


void
mnpbc_ctx_fini(mnpbc_ctx_t *ctx)
{
    (void)array_fini(&ctx->stack);
    hash_fini(&ctx->fields);
    hash_fini(&ctx->containers);
    ctx->in = NULL;
    ctx->out0 = NULL;
    ctx->out1 = NULL;
    BYTES_DECREF(&ctx->namein);
    BYTES_DECREF(&ctx->nameout0);
    BYTES_DECREF(&ctx->nameout1);
}
