#include "assemblylift.h"

#include "ruby.h"

VALUE rb_cAsml;

VALUE 
rb_asml_runtime_log(VALUE self, VALUE str) {
    if (RB_TYPE_P(str, T_STRING) == 1) {
        __asml_abi_runtime_log((uint8_t*) RSTRING_PTR(str), (uint32_t) RSTRING_LEN(str));
    }
    return Qnil;
}

void
Init_asml(void)
{
    rb_cAsml = rb_define_class("Asml", rb_cObject);
    rb_define_singleton_method(rb_cAsml, "log", rb_asml_runtime_log, 1);
}

__attribute__((export_name("__asml_guest_get_function_input_buffer_pointer"))) 
char* __asml_guest_get_function_input_buffer_pointer(void) {
    // TODO
    return NULL;
}

__attribute__((export_name("__asml_guest_get_io_buffer_pointer"))) 
char* __asml_guest_get_io_buffer_pointer(void) {
    // TODO
    return NULL;
}
