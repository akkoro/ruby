#include <stdint.h>

// IO
extern int32_t __asml_abi_io_poll(uint32_t id);
extern uint32_t __asml_abi_io_len(uint32_t id);
extern int32_t __asml_abi_io_load(uint32_t id);
extern int32_t __asml_abi_io_next();

// System clock
extern uint64_t __asml_abi_clock_time_get();

// Runtime
__attribute__((import_module("env"), import_name("__asml_abi_runtime_log"))) void __asml_abi_runtime_log(uint8_t* ptr, uint32_t len);

// Function Input
extern int32_t __asml_abi_input_start();
extern int32_t __asml_abi_input_next();
extern uint64_t __asml_abi_input_length_get();
