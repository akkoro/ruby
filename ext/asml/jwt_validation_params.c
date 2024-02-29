#include "jwt_validation_params.h"

const rb_data_type_t asml_jwt_validation_params_data_type = {
    .wrap_struct_name = "jwt_validation_params",
    .function = {
        .dmark = 0,
        .dfree = rb_asml_jwt_validation_params_free,
        .dsize = rb_asml_jwt_validation_params_size,
        .dcompact = 0
    },
    .flags = RUBY_TYPED_FREE_IMMEDIATELY
};

void rb_asml_jwt_validation_params_free(void* ptr) {
    rb_asml_jwt_validation_params_t* self = (rb_asml_jwt_validation_params_t*) ptr;
    ruby_xfree(self->iss);
    ruby_xfree(self->aud);
    ruby_xfree(self);
}

size_t rb_asml_jwt_validation_params_size(const void *ptr) {
    rb_asml_jwt_validation_params_t* self = (rb_asml_jwt_validation_params_t*) ptr;
    return sizeof(rb_asml_jwt_validation_params_t) + strlen(self->iss) + strlen(self->aud);
}

VALUE rb_asml_jwt_validation_params_alloc(VALUE klass) {
    rb_asml_jwt_validation_params_t* self;

    VALUE obj = TypedData_Make_Struct(klass, rb_asml_jwt_validation_params_t, &asml_jwt_validation_params_data_type, self);
    return obj;
}

VALUE rb_asml_jwt_validation_params_init(VALUE self, VALUE iss, VALUE aud) {
    rb_asml_jwt_validation_params_t* p;

    Check_Type(iss, T_STRING);
    Check_Type(aud, T_STRING);

    TypedData_Get_Struct(self, rb_asml_jwt_validation_params_t, &asml_jwt_validation_params_data_type, p);
    p->iss = (char *)ruby_xmalloc(RSTRING_LEN(iss) + 1);
    p->aud = (char *)ruby_xmalloc(RSTRING_LEN(aud) + 1);
    memcpy(p->iss, StringValuePtr(iss), RSTRING_LEN(iss) + 1);
    memcpy(p->aud, StringValuePtr(aud), RSTRING_LEN(aud) + 1);

    return self;
}

VALUE rb_asml_jwt_validation_params_iss(VALUE self) {
    rb_asml_jwt_validation_params_t* p;

    TypedData_Get_Struct(self, rb_asml_jwt_validation_params_t, &asml_jwt_validation_params_data_type, p);
    VALUE iss = rb_str_new(p->iss, strlen(p->iss));
    return iss;
}

VALUE rb_asml_jwt_validation_params_aud(VALUE self) {
    rb_asml_jwt_validation_params_t* p;

    TypedData_Get_Struct(self, rb_asml_jwt_validation_params_t, &asml_jwt_validation_params_data_type, p);
    VALUE aud = rb_str_new(p->aud, strlen(p->aud));
    return aud;
}
