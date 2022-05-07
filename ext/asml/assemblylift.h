#ifndef ASSEMBLYLIFT_H
#define ASSEMBLYLIFT_H

#include <stdint.h>
#include <math.h>

#define IO_BUFFER_SIZE_BYTES 32768
#define FUNCTION_INPUT_BUFFER_SIZE_BYTES 8192

// IO
__attribute__((import_module("env"), import_name("__asml_abi_io_poll"))) 
extern int32_t __asml_abi_io_poll(uint32_t id);
__attribute__((import_module("env"), import_name("__asml_abi_io_len"))) 
extern uint32_t __asml_abi_io_len(uint32_t id);
__attribute__((import_module("env"), import_name("__asml_abi_io_load"))) 
extern int32_t __asml_abi_io_load(uint32_t id);
__attribute__((import_module("env"), import_name("__asml_abi_io_next"))) 
extern int32_t __asml_abi_io_next();

// System clock
__attribute__((import_module("env"), import_name("__asml_abi_clock_time_get"))) 
extern uint64_t __asml_abi_clock_time_get();

// Runtime
__attribute__((import_module("env"), import_name("__asml_abi_runtime_log"))) 
void __asml_abi_runtime_log(uint8_t* ptr, uint32_t len);
__attribute__((import_module("env"), import_name("__asml_abi_runtime_success"))) 
void __asml_abi_runtime_success(uint8_t* ptr, uint32_t len);

// Function Input
__attribute__((import_module("env"), import_name("__asml_abi_input_start"))) 
extern int32_t __asml_abi_input_start();
__attribute__((import_module("env"), import_name("__asml_abi_input_next"))) 
extern int32_t __asml_abi_input_next();
__attribute__((import_module("env"), import_name("__asml_abi_input_length_get"))) 
extern uint64_t __asml_abi_input_length_get();

#endif
