if(NOT DEFINED NT_BRIDGE_CARGO)
    message(FATAL_ERROR "NT_BRIDGE_CARGO is not defined")
endif()
if(NOT DEFINED NT_BRIDGE_DIR)
    message(FATAL_ERROR "NT_BRIDGE_DIR is not defined")
endif()
if(NOT DEFINED NT_BRIDGE_TARGET_DIR)
    message(FATAL_ERROR "NT_BRIDGE_TARGET_DIR is not defined")
endif()
if(NOT DEFINED NT_BRIDGE_CONFIG)
    set(NT_BRIDGE_CONFIG "Debug")
endif()

set(build_args build --manifest-path "${NT_BRIDGE_DIR}/Cargo.toml")
if(NOT NT_BRIDGE_CONFIG STREQUAL "Debug")
    list(APPEND build_args --release)
endif()

set(ENV{CARGO_TARGET_DIR} "${NT_BRIDGE_TARGET_DIR}")

execute_process(
    COMMAND "${NT_BRIDGE_CARGO}" ${build_args}
    WORKING_DIRECTORY "${NT_BRIDGE_DIR}"
    RESULT_VARIABLE result
)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "cargo build failed with exit code ${result}")
endif()
