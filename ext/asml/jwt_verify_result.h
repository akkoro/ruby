#ifndef JWT_VERIFY_RESULT_H
#define JWT_VERIFY_RESULT_H

#include "ruby.h"

typedef struct rb_asml_jwt_verify_result {
    bool valid;
} rb_asml_jwt_verify_result_t;

void rb_asml_jwt_verify_result_free(void* ptr);

size_t rb_asml_jwt_verify_result_size(const void *ptr);

VALUE rb_asml_jwt_verify_result_alloc(VALUE klass);

VALUE rb_asml_jwt_verify_result_init(VALUE self, VALUE valid);

VALUE rb_asml_jwt_verify_result_valid(VALUE self);

#endif
