#ifndef JWT_VALIDATION_PARAMS_H
#define JWT_VALIDATION_PARAMS_H

#include "ruby.h"

typedef struct rb_asml_jwt_validation_params {
    char* iss;
    char* aud;
} rb_asml_jwt_validation_params_t;

void rb_asml_jwt_validation_params_free(void* ptr);

size_t rb_asml_jwt_validation_params_size(const void *ptr);

VALUE rb_asml_jwt_validation_params_alloc(VALUE klass);

VALUE rb_asml_jwt_validation_params_init(VALUE self, VALUE iss, VALUE aud);

VALUE rb_asml_jwt_validation_params_iss(VALUE self);

VALUE rb_asml_jwt_validation_params_aud(VALUE self);


#endif
