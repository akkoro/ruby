#include "asml.h"

VALUE rb_mAsmlRt;
VALUE rb_mAsmlIo;
VALUE rb_mAsmlOpa;
VALUE rb_mAsmlJwt;
VALUE rb_cAsmlOpaPolicy;
VALUE rb_cAsmlJwtValidationParams;
VALUE rb_cAsmlJwtVerifyResult;

extern const rb_data_type_t asml_jwt_validation_params_data_type;

void c_bytes_from_value(VALUE bytes, uint8_t **ret, size_t *len) {
    Check_Type(bytes, T_ARRAY);

    VALUE *rb_ptr = RARRAY_PTR(bytes);
    size_t rb_len = RARRAY_LEN(bytes);
    uint8_t *c_ptr = ruby_xmalloc(rb_len);
    for (size_t i = 0; i < rb_len; ++i) {
        c_ptr[i] = (uint8_t) FIX2INT(rb_ptr[i]);
    }
    *ret = c_ptr;
    *len = rb_len;
}

static VALUE rb_asml_runtime_log(VALUE self, VALUE str) {
    Check_Type(str, T_STRING);

    assemblylift_string_t context = {0};
    assemblylift_string_set(&context, "func");
    assemblylift_string_t message = { .ptr = RSTRING_PTR(str), .len = RSTRING_LEN(str) };
    akkoro_assemblylift_asml_rt_log(AKKORO_ASSEMBLYLIFT_ASML_RT_LOG_LEVEL_INFO, &context, &message);

    return Qnil;
}

static VALUE rb_asml_runtime_success(VALUE self, VALUE bytes) {
    Check_Type(bytes, T_ARRAY);

    uint8_t *c_ptr = NULL;
    size_t len = 0;
    c_bytes_from_value(bytes, &c_ptr, &len);

    akkoro_assemblylift_asml_rt_bytes_t asml_bytes = { .ptr = c_ptr, .len = len };
    akkoro_assemblylift_asml_rt_success(&asml_bytes);
    akkoro_assemblylift_asml_rt_bytes_free(&asml_bytes);

    return Qnil;
}

static VALUE rb_asml_runtime_failure(VALUE self, VALUE bytes) {
    Check_Type(bytes, T_ARRAY);

    uint8_t *c_ptr = NULL;
    size_t len = 0;
    c_bytes_from_value(bytes, &c_ptr, &len);

    akkoro_assemblylift_asml_rt_bytes_t asml_bytes = { .ptr = c_ptr, .len = len };
    akkoro_assemblylift_asml_rt_failure(&asml_bytes);
    akkoro_assemblylift_asml_rt_bytes_free(&asml_bytes);

    return Qnil;
}

static VALUE rb_asml_get_function_input(VALUE self) {
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

static VALUE rb_asml_iomod_invoke(VALUE self, VALUE method, VALUE input) {
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

static VALUE rb_asml_opa_new_policy(VALUE self, VALUE bytes) {
    uint8_t *c_ptr = NULL;
    size_t len = 0;
    c_bytes_from_value(bytes, &c_ptr, &len);
    if (c_ptr != NULL) {
        akkoro_opa_module_policy_t policy = {0};
        akkoro_opa_module_policy_error_t error = {0};
        if (!akkoro_opa_module_new_policy(&(opa_list_u8_t) { .ptr = c_ptr, .len = RARRAY_LEN(bytes) }, &policy, &error)) {
            rb_raise(rb_eRuntimeError, "opa policy error: %d", error);
        } else {
            VALUE args[1];
            args[0] = rb_str_new(policy.id.ptr, policy.id.len);
            return rb_class_new_instance(1, args, rb_cAsmlOpaPolicy);
        }
    }
    rb_raise(rb_eRuntimeError, "opa policy error: could not read bytes");
    return Qnil;
}

static VALUE rb_asml_opa_eval(VALUE self, VALUE id_s, VALUE data_s, VALUE input_s) {
    Check_Type(id_s, T_STRING);
    Check_Type(data_s, T_STRING);
    Check_Type(input_s, T_STRING);

    opa_string_t id = { .ptr = RSTRING_PTR(id_s), .len = RSTRING_LEN(id_s) };
    opa_string_t data = { .ptr = RSTRING_PTR(data_s), .len = RSTRING_LEN(data_s) };
    opa_string_t input = { .ptr = RSTRING_PTR(input_s), .len = RSTRING_LEN(input_s) };

    opa_string_t ret = {0};
    akkoro_opa_module_eval(&id, &data, &input, &ret);
    return rb_str_new(ret.ptr, ret.len);
}

static VALUE rb_asml_jwt_decode_verify(VALUE self, VALUE token_s, VALUE jwks_s, VALUE params_c) {
    Check_Type(token_s, T_STRING);
    Check_Type(jwks_s, T_STRING);
    // Check_Type(params_c, T_OBJECT);

    jwt_string_t token = { .ptr = RSTRING_PTR(token_s), .len = RSTRING_LEN(token_s) };
    jwt_string_t jwks = { .ptr = RSTRING_PTR(jwks_s), .len = RSTRING_LEN(jwks_s) };

    rb_asml_jwt_validation_params_t* p;
    TypedData_Get_Struct(params_c, rb_asml_jwt_validation_params_t, &asml_jwt_validation_params_data_type, p);

    jwt_string_t iss;
    jwt_string_t aud;
    jwt_string_set(&iss, p->iss);
    jwt_string_set(&aud, p->aud);
    akkoro_jwt_decoder_validation_params_t params = { .iss = iss, .aud = aud };

    akkoro_jwt_decoder_verify_result_t ret = {0};
    akkoro_jwt_decoder_jwt_error_t err = 0;
    if (!akkoro_jwt_decoder_decode_verify(&token, &jwks, &params, &ret, &err)) {
        rb_raise(rb_eRuntimeError, "jwt decode error: %d", err);
    } else {
        VALUE args[1];
        args[0] = ret.valid;
        return rb_class_new_instance(1, args, rb_cAsmlJwtVerifyResult);
    }
}

void Init_asml(void)
{
    /* akkoro:assemblylift/asml-rt */
    rb_mAsmlRt = rb_define_module("AsmlRuntime");
    rb_define_singleton_method(rb_mAsmlRt, "log", rb_asml_runtime_log, 1);
    rb_define_singleton_method(rb_mAsmlRt, "success", rb_asml_runtime_success, 1);
    rb_define_singleton_method(rb_mAsmlRt, "failure", rb_asml_runtime_failure, 1);
    rb_define_singleton_method(rb_mAsmlRt, "input", rb_asml_get_function_input, 0);

    /* akkoro:assemblylift/asml-io */
    rb_mAsmlIo = rb_define_module("AsmlIo");
//    rb_define_singleton_method(rb_mAsmlIo, "get_io_document", rb_asml_get_io_document, 1);
    rb_define_singleton_method(rb_mAsmlIo, "invoke", rb_asml_iomod_invoke, 2);

    /* akkoro:opa/module */
    rb_mAsmlOpa = rb_define_module("AsmlOpa");

    rb_define_singleton_method(rb_mAsmlOpa, "new_policy", rb_asml_opa_new_policy, 1);
    rb_define_singleton_method(rb_mAsmlOpa, "eval", rb_asml_opa_eval, 3);

    rb_cAsmlOpaPolicy = rb_define_class_under(rb_mAsmlOpa, "Policy", rb_cObject);
    rb_define_alloc_func(rb_cAsmlOpaPolicy, rb_asml_opa_policy_alloc);
    rb_define_method(rb_cAsmlOpaPolicy, "initialize", rb_asml_opa_policy_init, 1);
    rb_define_method(rb_cAsmlOpaPolicy, "id", rb_asml_opa_policy_id, 0);

    /* akkoro:jwt/decoder */
    rb_mAsmlJwt = rb_define_module("AsmlJwt");
    rb_define_singleton_method(rb_mAsmlJwt, "decode_verify", rb_asml_jwt_decode_verify, 3);

    rb_cAsmlJwtValidationParams = rb_define_class_under(rb_mAsmlJwt, "ValidationParams", rb_cObject);
    rb_define_alloc_func(rb_cAsmlJwtValidationParams, rb_asml_jwt_validation_params_alloc);
    rb_define_method(rb_cAsmlJwtValidationParams, "initialize", rb_asml_jwt_validation_params_init, 2);
    rb_define_method(rb_cAsmlJwtValidationParams, "iss", rb_asml_jwt_validation_params_iss, 0);
    rb_define_method(rb_cAsmlJwtValidationParams, "aud", rb_asml_jwt_validation_params_aud, 0);

    rb_cAsmlJwtVerifyResult = rb_define_class_under(rb_mAsmlJwt, "VerifyResult", rb_cObject);
    rb_define_alloc_func(rb_cAsmlJwtVerifyResult, rb_asml_jwt_verify_result_alloc);
    rb_define_method(rb_cAsmlJwtVerifyResult, "initialize", rb_asml_jwt_verify_result_init, 1);
    rb_define_method(rb_cAsmlJwtVerifyResult, "valid", rb_asml_jwt_verify_result_valid, 0);
}
