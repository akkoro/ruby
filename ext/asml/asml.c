#include "asml.h"

VALUE rb_mAsmlRt;
VALUE rb_mAsmlIo;
VALUE rb_mAsmlOpa;

void c_bytes_from_value(VALUE bytes, uint8_t **ret, size_t *len) {
    if (RB_TYPE_P(bytes, T_ARRAY) == 1) {
        VALUE *rb_ptr = RARRAY_PTR(bytes);
        size_t rb_len = RARRAY_LEN(bytes);
        uint8_t *c_ptr = malloc(rb_len);
        for (size_t i = 0; i < rb_len; ++i) {
            c_ptr[i] = (uint8_t) FIX2INT(rb_ptr[i]);
        }
        *ret = c_ptr;
        *len = rb_len;
    }
}

VALUE
rb_asml_runtime_log(VALUE self, VALUE str) {
    if (RB_TYPE_P(str, T_STRING) == 1) {
        assemblylift_string_t context = {0};
        assemblylift_string_set(&context, "func");
        assemblylift_string_t message = { .ptr = RSTRING_PTR(str), .len = RSTRING_LEN(str) };
        akkoro_assemblylift_asml_rt_log(AKKORO_ASSEMBLYLIFT_ASML_RT_LOG_LEVEL_INFO, &context, &message);
    }
    return Qnil;
}

VALUE
rb_asml_runtime_success(VALUE self, VALUE bytes) {
    if (RB_TYPE_P(bytes, T_ARRAY) == 1) {
        uint8_t *c_ptr = NULL;
        size_t len = 0;
        c_bytes_from_value(bytes, &c_ptr, &len);

        akkoro_assemblylift_asml_rt_bytes_t asml_bytes = { .ptr = c_ptr, .len = len };
        akkoro_assemblylift_asml_rt_success(&asml_bytes);
        akkoro_assemblylift_asml_rt_bytes_free(&asml_bytes);
    }
    return Qnil;
}

VALUE
rb_asml_runtime_failure(VALUE self, VALUE bytes) {
    if (RB_TYPE_P(bytes, T_ARRAY) == 1) {
        uint8_t *c_ptr = NULL;
        size_t len = 0;
        c_bytes_from_value(bytes, &c_ptr, &len);

        akkoro_assemblylift_asml_rt_bytes_t asml_bytes = { .ptr = c_ptr, .len = len };
        akkoro_assemblylift_asml_rt_failure(&asml_bytes);
        akkoro_assemblylift_asml_rt_bytes_free(&asml_bytes);
    }
    return Qnil;
}

VALUE
rb_asml_get_function_input(VALUE self) {
    assemblylift_list_u8_t input = {0};
    akkoro_assemblylift_asml_rt_get_input(&input);

    VALUE rb_arr = rb_ary_new_capa(input.len);
    for (size_t i = 0; i < input.len; ++i) {
        rb_ary_push(rb_arr, INT2FIX(input.ptr[i]));
    }

    return rb_arr;
}

// VALUE
// rb_asml_get_io_document(VALUE self, VALUE id) {
//     int ioid = NUM2INT(id);
//     if (0/*__asml_abi_io_poll(ioid)*/ == 1) {
//         size_t input_len = 0;
// //        size_t input_len = __asml_abi_io_len(ioid);
//         size_t bytes_read = 0;

//         VALUE str = rb_str_buf_new(input_len);
//         rb_encoding* enc = rb_utf8_encoding();
//         rb_enc_associate(str, enc);

// //        __asml_abi_io_load(ioid);
// //        while (bytes_read < input_len) {
// //            size_t read_len = fmin(input_len, IO_BUFFER_SIZE_BYTES);
// //            rb_str_cat(str, IO_BUFFER, read_len);
// //            bytes_read += read_len;
// //            if (bytes_read % IO_BUFFER_SIZE_BYTES == 0) {
// //                __asml_abi_io_next();
// //            }
// //        }

//         return str;
//     }

//     return Qnil;
// }

VALUE
rb_asml_iomod_invoke(VALUE self, VALUE method, VALUE input) {
    if (RB_TYPE_P(method, T_STRING) == 1) {
        return INT2NUM(-1);
//        return INT2NUM(__asml_abi_io_invoke(
//            (uint8_t*) RSTRING_PTR(method),
//            (uint32_t) RSTRING_LEN(method),
//            (uint8_t*) RSTRING_PTR(input),
//            (uint32_t) RSTRING_LEN(input)
//        ));
    }
    return INT2NUM(-1);
}

VALUE
rb_asml_opa_new_policy(VALUE self, VALUE bytes) {
    uint8_t *c_ptr = NULL;
    size_t len = 0;
    c_bytes_from_value(bytes, &c_ptr, &len);
    if (c_ptr != NULL) {
        akkoro_opa_module_policy_t policy = {0};
        akkoro_opa_module_policy_error_t error = {0};
        if (!akkoro_opa_module_new_policy(&(opa_list_u8_t) { .ptr = c_ptr, .len = RARRAY_LEN(bytes) }, &policy, &error)) {
            rb_raise(rb_eRuntimeError, "TODO: my error message");
        } else {
            // TODO return the policy
        }
    }
    return Qnil;
}

VALUE
rb_asml_opa_eval(VALUE self, VALUE id, VALUE data, VALUE input) {
    return Qnil;
}

void
Init_asml(void)
{
    /* akkoro:assemblylift/asml-rt */
    rb_mAsmlRt = rb_define_module("AsmlRuntime");
    rb_define_singleton_method(rb_mAsmlRt, "log", rb_asml_runtime_log, 1);
    rb_define_singleton_method(rb_mAsmlRt, "success", rb_asml_runtime_success, 1);
    rb_define_singleton_method(rb_mAsmlRt, "failure", rb_asml_runtime_failure, 1);
    rb_define_singleton_method(rb_mAsmlRt, "get_function_input", rb_asml_get_function_input, 0);

    /* akkoro:assemblylift/asml-io */
    rb_mAsmlIo = rb_define_module("AsmlIo");
    // rb_define_singleton_method(rb_mAsmlIo, "get_io_document", rb_asml_get_io_document, 1);
    rb_define_singleton_method(rb_mAsmlIo, "invoke", rb_asml_iomod_invoke, 2);

    /* akkoro:opa/module */
    rb_mAsmlOpa = rb_define_module("AsmlOpa");
    rb_define_singleton_method(rb_mAsmlOpa, "new_policy", rb_asml_opa_new_policy, 1);
    rb_define_singleton_method(rb_mAsmlOpa, "eval", rb_asml_opa_eval, 3);
}
