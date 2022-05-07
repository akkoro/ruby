#include "assemblylift.h"

#include "ruby.h"
#include "ruby/encoding.h"

uint8_t IO_BUFFER[IO_BUFFER_SIZE_BYTES];
uint8_t FUNCTION_INPUT_BUFFER[FUNCTION_INPUT_BUFFER_SIZE_BYTES];

VALUE rb_cAsml;

VALUE 
rb_asml_runtime_log(VALUE self, VALUE str) {
    if (RB_TYPE_P(str, T_STRING) == 1) {
        __asml_abi_runtime_log((uint8_t*) RSTRING_PTR(str), (uint32_t) RSTRING_LEN(str));
    }
    return Qnil;
}

VALUE 
rb_asml_runtime_success(VALUE self, VALUE str) {
    if (RB_TYPE_P(str, T_STRING) == 1) {
        __asml_abi_runtime_success((uint8_t*) RSTRING_PTR(str), (uint32_t) RSTRING_LEN(str));
    }
    return Qnil;
}

VALUE
rb_asml_get_function_input(VALUE self) {
    size_t bytes_read = 0;
    size_t input_len = (size_t) __asml_abi_input_length_get();

    VALUE str = rb_str_buf_new(input_len);
    rb_encoding* enc = rb_utf8_encoding();
    rb_enc_associate(str, enc);

    __asml_abi_input_start();
    while (bytes_read < input_len) {
        size_t read_len = fmin(input_len, FUNCTION_INPUT_BUFFER_SIZE_BYTES);
        rb_str_cat(str, FUNCTION_INPUT_BUFFER, read_len);
        bytes_read += read_len;
        if (bytes_read % FUNCTION_INPUT_BUFFER_SIZE_BYTES == 0) {
            __asml_abi_input_next();
        }
    }

    return str;
}

VALUE
rb_asml_get_io_document(VALUE self, VALUE id) {
    int ioid = NUM2INT(id);
    if (__asml_abi_io_poll(ioid) == 1) {
        size_t input_len = __asml_abi_io_len(ioid);
        size_t bytes_read = 0;

        VALUE str = rb_str_buf_new(input_len);
        rb_encoding* enc = rb_utf8_encoding();
        rb_enc_associate(str, enc);

        __asml_abi_io_load(ioid);
        while (bytes_read < input_len) {
            size_t read_len = fmin(input_len, IO_BUFFER_SIZE_BYTES);
            rb_str_cat(str, IO_BUFFER, read_len);
            bytes_read += read_len;
            if (bytes_read % IO_BUFFER_SIZE_BYTES == 0) {
                __asml_abi_io_next();
            }
        }

        return str;
    }

    return Qnil;
}

void
Init_asml(void)
{
    rb_cAsml = rb_define_class("Asml", rb_cObject);
    rb_define_singleton_method(rb_cAsml, "log", rb_asml_runtime_log, 1);
    rb_define_singleton_method(rb_cAsml, "success", rb_asml_runtime_success, 1);
    
    rb_define_singleton_method(rb_cAsml, "get_function_input", rb_asml_get_function_input, 0);
    rb_define_singleton_method(rb_cAsml, "get_io_document", rb_asml_get_io_document, 1);
}

__attribute__((export_name("__asml_guest_get_function_input_buffer_pointer"))) 
uint8_t* __asml_guest_get_function_input_buffer_pointer(void) {
    return FUNCTION_INPUT_BUFFER; 
}

__attribute__((export_name("__asml_guest_get_io_buffer_pointer"))) 
uint8_t* __asml_guest_get_io_buffer_pointer(void) {
    return IO_BUFFER;
}
