if(NOT DEFINED RB_CARGO)
    message(FATAL_ERROR "RB_CARGO is not defined")
endif()
if(NOT DEFINED RB_DIR)
    message(FATAL_ERROR "RB_DIR is not defined")
endif()
if(NOT DEFINED RB_TARGET_DIR)
    message(FATAL_ERROR "RB_TARGET_DIR is not defined")
endif()
if(NOT DEFINED RB_CONFIG)
    set(RB_CONFIG "Debug")
endif()

set(build_args build --manifest-path "${RB_DIR}/Cargo.toml")
if(NOT RB_CONFIG STREQUAL "Debug")
    list(APPEND build_args --release)
endif()

set(ENV{CARGO_TARGET_DIR} "${RB_TARGET_DIR}")

execute_process(
    COMMAND "${RB_CARGO}" ${build_args}
    WORKING_DIRECTORY "${RB_DIR}"
    RESULT_VARIABLE result
)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "cargo build failed with exit code ${result}")
endif()
