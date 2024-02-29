#include "jwt_verify_result.h"

const rb_data_type_t asml_jwt_verify_result_data_type = {
    .wrap_struct_name = "jwt_verify_result",
    .function = {
        .dmark = 0,
        .dfree = rb_asml_jwt_verify_result_free,
        .dsize = rb_asml_jwt_verify_result_size,
        .dcompact = 0
    },
    .flags = RUBY_TYPED_FREE_IMMEDIATELY
};

void rb_asml_jwt_verify_result_free(void* ptr) {
    rb_asml_jwt_verify_result_t* self = (rb_asml_jwt_verify_result_t*) ptr;
    ruby_xfree(self);
}

size_t rb_asml_jwt_verify_result_size(const void *ptr) {
    rb_asml_jwt_verify_result_t* self = (rb_asml_jwt_verify_result_t*) ptr;
    return sizeof(rb_asml_jwt_verify_result_t);
}

VALUE rb_asml_jwt_verify_result_alloc(VALUE klass) {
    rb_asml_jwt_verify_result_t* self;

    VALUE obj = TypedData_Make_Struct(klass, rb_asml_jwt_verify_result_t, &asml_jwt_verify_result_data_type, self);
    return obj;
}

VALUE rb_asml_jwt_verify_result_init(VALUE self, VALUE valid) {
    rb_asml_jwt_verify_result_t* p;

    int boolean = RTEST(valid);

    TypedData_Get_Struct(self, rb_asml_jwt_verify_result_t, &asml_jwt_verify_result_data_type, p);
    p->valid = boolean == 1 ? true : false;

    return self;
}

VALUE rb_asml_jwt_verify_result_valid(VALUE self) {
    rb_asml_jwt_verify_result_t* p;

    TypedData_Get_Struct(self, rb_asml_jwt_verify_result_t, &asml_jwt_verify_result_data_type, p);
    return p->valid ? Qtrue : Qfalse;
}
