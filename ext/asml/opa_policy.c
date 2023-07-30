#include "opa_policy.h"

const rb_data_type_t asml_opa_policy_data_type = {
    .wrap_struct_name = "opa_policy",
    .function = {
        .dmark = 0,
        .dfree = rb_asml_opa_policy_free,
        .dsize = rb_asml_opa_policy_size,
        .dcompact = 0
    },
    .flags = RUBY_TYPED_FREE_IMMEDIATELY
};

void rb_asml_opa_policy_free(void* ptr) {
    rb_asml_opa_policy_t* self = (rb_asml_opa_policy_t*) ptr;
    ruby_xfree(self->id);
    ruby_xfree(self);
}

size_t rb_asml_opa_policy_size(const void *ptr) {
    rb_asml_opa_policy_t* self = (rb_asml_opa_policy_t*) ptr;
    return sizeof(rb_asml_opa_policy_t) + strlen(self->id);
}

VALUE rb_asml_opa_policy_alloc(VALUE klass) {
    rb_asml_opa_policy_t* self;

    VALUE obj = TypedData_Make_Struct(klass, rb_asml_opa_policy_t, &asml_opa_policy_data_type, self);
    return obj;
}

VALUE rb_asml_opa_policy_init(VALUE self, VALUE id) {
    rb_asml_opa_policy_t* p;

    Check_Type(id, T_STRING);

    TypedData_Get_Struct(self, rb_asml_opa_policy_t, &asml_opa_policy_data_type, p);
    p->id = (char *)ruby_xmalloc(RSTRING_LEN(id) + 1);
    memcpy(p->id, StringValuePtr(id), RSTRING_LEN(id) + 1);

    return self;
}

VALUE rb_asml_opa_policy_id(VALUE self) {
    rb_asml_opa_policy_t* p;

    TypedData_Get_Struct(self, rb_asml_opa_policy_t, &asml_opa_policy_data_type, p);
    VALUE id = rb_str_new(p->id, strlen(p->id));
    return id;
}
