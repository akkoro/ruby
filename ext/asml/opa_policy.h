#ifndef OPA_POLICY_H
#define OPA_POLICY_H

#include "ruby.h"

typedef struct rb_asml_opa_policy {
    char* id;
    // TODO entrypoints
} rb_asml_opa_policy_t;

void rb_asml_opa_policy_free(void* ptr);

size_t rb_asml_opa_policy_size(const void *ptr);

VALUE rb_asml_opa_policy_alloc(VALUE klass);

VALUE rb_asml_opa_policy_init(VALUE self, VALUE id);

VALUE rb_asml_opa_policy_id(VALUE self);

#endif