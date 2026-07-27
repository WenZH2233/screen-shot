# ============================================================================
#  📋 CMakeLists.txt — 跨平台 C++ 构建模板
#  📌 使用说明：只需修改下方 >>> CONFIGURATION <<< 区域即可适配新项目
#  🚀 使用 vcpkg 管理依赖
#  🌐 支持 Windows / macOS / Linux 三平台 (含 WSL 优化)
#  🔧 自动检测 编译器 / 架构 / 构建类型 / Git 信息
# ============================================================================
#  如果你正在将本模板用于一个新项目，请依次：
#     1. 修改 CONFIGURATION 区的项目名称和版本号
#     2. 在 DEPENDENCIES 区域按需添加你的 find_package
#     3. 在 link 区域添加对应的 target_link_libraries
#     4. 如果不需要 upgrade 子程序，设置 ENABLE_UPGRADE=OFF
#     5. 检查 src/ include/ 目录结构是否匹配
# ============================================================================


# ============================================================================
#  >>> START OF CONFIGURATION — 从这里开始定制你的项目 <<<
# ============================================================================

# --- 项目标识 ------------------------------------------------
set(PROJECT_NAME                  "screen-shot")          # 项目名称 = 可执行文件名
set(PROJECT_DESCRIPTION           "A screen capture tool") # 项目简要描述
set(PROJECT_HOMEPAGE_URL          "")                     # 项目主页（可选）

# --- 版本号 --------------------------------------------------
set(PROJECT_VERSION_MAJOR         0)
set(PROJECT_VERSION_MINOR         1)
set(PROJECT_VERSION_PATCH         0)
set(PROJECT_VERSION_BUILD         0)      # 构建号，会被 build_number.txt 覆盖

# --- 目录结构 ------------------------------------------------
set(SRC_DIR                       "src")                  # 主程序源文件目录
set(INC_DIR                       "include")              # 头文件目录
set(UPGRADE_DIR                   "upgrade")              # 自动升级程序源目录（ENABLE_UPGRADE=ON 时有效）
set(RESOURCE_DIR                  "files")                # 需要复制到输出目录的资源文件夹
set(VERSION_H_IN                  "${INC_DIR}/version.h.in")  # 版本头文件模板

# --- 可选特性 ------------------------------------------------
option(ENABLE_UPGRADE             "Build the auto-upgrade executable" OFF)  # 是否编译 upgrade 子程序
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# （依赖管理固定为 vcpkg）

# --- Windows 图标与资源文件 -----------------------------------
set(APP_ICON                      "${CMAKE_CURRENT_SOURCE_DIR}/screen-shot.ico")  # 主程序图标
set(APP_RC_IN                     "${CMAKE_CURRENT_SOURCE_DIR}/app.rc.in")         # 主程序资源模板
set(UPGRADE_ICON                  "${CMAKE_CURRENT_SOURCE_DIR}/upgrade.ico")       # upgrade 图标
set(UPGRADE_RC_IN                 "${CMAKE_CURRENT_SOURCE_DIR}/upgrade.rc.in")     # upgrade 资源模板

# --- 升级程序的目标名称 ---------------------------------------
set(UPGRADE_TARGET                "upgrade")              # upgrade 可执行文件名称

# --- C++ 标准 ------------------------------------------------
set(CPP_STANDARD                  23)                     # C++ 标准 (17 / 20 / 23)

# ========================================================================
#  >>> DEPENDENCIES — 你的项目依赖在这里添加 <<<
# ========================================================================
#  用 find_package(xxx CONFIG REQUIRED) 查找，vcpkg 会处理安装
#  添加后，同步更新下方 [15. 查找并链接依赖库] 中对应的
#  target_include_directories / target_link_libraries
#
#  示例：
#    find_package(glfw3 CONFIG REQUIRED)
#    find_package(nlohmann_json CONFIG REQUIRED)

# ========================================================================
#  <<< END OF CONFIGURATION — 以下内容无需手动修改 >>>
# ========================================================================


# ============================================================================
#  0. cmake 最小版本
# ============================================================================
cmake_minimum_required(VERSION 3.15)


# ============================================================================
#  1. 依赖管理基础设施 — vcpkg 工具链检测（必须在 project() 之前）
# ============================================================================

# 检查是否在 GitHub Actions 环境中
if(DEFINED ENV{GITHUB_ACTIONS})
    message(STATUS "Running in GitHub Actions environment")
    set(IN_GITHUB_ACTIONS TRUE)
else()
    message(STATUS "Running in local development environment")
    set(IN_GITHUB_ACTIONS FALSE)
endif()

message(STATUS "Using vcpkg for dependency management")

    # 设置 vcpkg target triplet
    if(NOT IN_GITHUB_ACTIONS)
        if(WIN32)
            set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "Vcpkg target triplet")
        elseif(APPLE)
            set(VCPKG_TARGET_TRIPLET "x64-osx" CACHE STRING "Vcpkg target triplet")
        else()
            set(VCPKG_TARGET_TRIPLET "x64-linux" CACHE STRING "Vcpkg target triplet")
        endif()
    else()
        if(DEFINED ENV{VCPKG_TARGET_TRIPLET})
            set(VCPKG_TARGET_TRIPLET $ENV{VCPKG_TARGET_TRIPLET} CACHE STRING "Vcpkg target triplet")
        else()
            if(WIN32)
                set(VCPKG_TARGET_TRIPLET "x64-windows-static" CACHE STRING "Vcpkg target triplet")
            elseif(APPLE)
                set(VCPKG_TARGET_TRIPLET "x64-osx" CACHE STRING "Vcpkg target triplet")
            else()
                set(VCPKG_TARGET_TRIPLET "x64-linux" CACHE STRING "Vcpkg target triplet")
            endif()
        endif()
    endif()

    # 检测是否使用混合链接模式（静态库 + 动态 FFMPEG）
    set(USE_MIXED_LINKING FALSE)
    if(WIN32 AND VCPKG_TARGET_TRIPLET MATCHES "static-dynamic-ffmpeg")
        message(STATUS "Using mixed linking: Static libraries + Dynamic FFMPEG")
        set(USE_MIXED_LINKING TRUE)
    endif()

    # vcpkg 工具链文件设置
    if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        if(DEFINED ENV{VCPKG_ROOT})
            message(STATUS "Using VCPKG_ROOT: $ENV{VCPKG_ROOT}")
            set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
                CACHE STRING "Vcpkg toolchain file")
        elseif(DEFINED ENV{VCPKG_INSTALLATION_ROOT})
            message(STATUS "Using VCPKG_INSTALLATION_ROOT: $ENV{VCPKG_INSTALLATION_ROOT}")
            set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_INSTALLATION_ROOT}/scripts/buildsystems/vcpkg.cmake"
                CACHE STRING "Vcpkg toolchain file")
        else()
            message(STATUS "Using default vcpkg path: /opt/vcpkg")
            set(CMAKE_TOOLCHAIN_FILE "/opt/vcpkg/scripts/buildsystems/vcpkg.cmake")
        endif()
    endif()
    message(STATUS "Vcpkg target triplet: ${VCPKG_TARGET_TRIPLET}")
    message(STATUS "Vcpkg toolchain file: ${CMAKE_TOOLCHAIN_FILE}")

# 强制启用 vcpkg manifest mode（覆盖旧缓存值）
set(VCPKG_MANIFEST_MODE ON CACHE BOOL "Use vcpkg manifest mode for auto-install" FORCE)
# 指明 vcpkg.json 所在目录（覆盖旧缓存中可能残留的空值）
set(VCPKG_MANIFEST_DIR "${CMAKE_SOURCE_DIR}" CACHE PATH "Directory containing vcpkg.json" FORCE)

# ============================================================================
#  2. 项目定义 — project() 必须在 vcpkg 工具链之后
# ============================================================================
project(${PROJECT_NAME}
    VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
    DESCRIPTION ${PROJECT_DESCRIPTION}
    LANGUAGES CXX
)


# ============================================================================
#  3. 版本相关变量（从 project() 自动获取，保证一致性）
# ============================================================================
set(PROJECT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")


# ============================================================================
#  4. 构建信息 — 日期、时间、Git 提交
# ============================================================================
string(TIMESTAMP BUILD_DATE "%Y-%m-%d")
string(TIMESTAMP BUILD_TIME "%H:%M:%S")

find_package(Git QUIET)
if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
else()
    set(GIT_COMMIT_HASH "unknown")
    set(GIT_BRANCH "unknown")
endif()


# ============================================================================
#  5. 平台检测
# ============================================================================
if(WIN32)
    set(PLATFORM_NAME "Windows")
    set(PLATFORM_WINDOWS TRUE)
    execute_process(
        COMMAND cmd /c "ver"
        OUTPUT_VARIABLE WINDOWS_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(WINDOWS_VERSION MATCHES "Version ([0-9]+\\.[0-9]+\\.[0-9]+)")
        set(PLATFORM_VERSION "${CMAKE_MATCH_1}")
    else()
        set(PLATFORM_VERSION "Unknown")
    endif()

elseif(APPLE)
    set(PLATFORM_NAME "macOS")
    set(PLATFORM_MACOS TRUE)
    execute_process(
        COMMAND sw_vers -productVersion
        OUTPUT_VARIABLE PLATFORM_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(NOT PLATFORM_VERSION)
        set(PLATFORM_VERSION "Unknown")
    endif()

elseif(UNIX)
    set(PLATFORM_NAME "Linux")
    set(PLATFORM_LINUX TRUE)
    if(EXISTS "/etc/os-release")
        execute_process(
            COMMAND grep "^PRETTY_NAME=" /etc/os-release
            COMMAND sed "s/PRETTY_NAME=\"\\(.*\\)\"/\\1/"
            OUTPUT_VARIABLE PLATFORM_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
    endif()
    if(NOT PLATFORM_VERSION)
        set(PLATFORM_VERSION "Unknown Linux")
    endif()
else()
    set(PLATFORM_NAME "Unknown")
    set(PLATFORM_VERSION "Unknown")
endif()


# ============================================================================
#  6. 编译器检测
# ============================================================================
if(MSVC)
    set(COMPILER_NAME "MSVC")
    set(COMPILER_MSVC TRUE)
    set(COMPILER_VERSION "${MSVC_VERSION}")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(COMPILER_NAME "GCC")
    set(COMPILER_GCC TRUE)
    set(COMPILER_VERSION "${CMAKE_CXX_COMPILER_VERSION}")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(COMPILER_NAME "Clang")
    set(COMPILER_CLANG TRUE)
    set(COMPILER_VERSION "${CMAKE_CXX_COMPILER_VERSION}")
else()
    set(COMPILER_NAME "Unknown")
    set(COMPILER_VERSION "Unknown")
endif()


# ============================================================================
#  7. 架构检测
# ============================================================================
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)")
        set(ARCHITECTURE "arm64")
        set(ARCH_ARM64 TRUE)
    else()
        set(ARCHITECTURE "x64")
        set(ARCH_X64 TRUE)
    endif()
else()
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
        set(ARCHITECTURE "arm")
        set(ARCH_ARM TRUE)
    else()
        set(ARCHITECTURE "x86")
        set(ARCH_X86 TRUE)
    endif()
endif()


# ============================================================================
#  8. 构建类型检测
# ============================================================================
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_TYPE "Debug")
    set(BUILD_DEBUG TRUE)
else()
    set(BUILD_TYPE "Release")
    set(BUILD_RELEASE TRUE)
endif()


# 依赖管理标记（供 version.h 等使用）
set(USE_VCPKG_MACRO TRUE)
if(USE_MIXED_LINKING)
    set(USE_MIXED_LINKING_MACRO TRUE)
endif()


# ============================================================================
#  10. 配置版本头文件
# ============================================================================
configure_file(
    "${CMAKE_SOURCE_DIR}/${VERSION_H_IN}"
    "${CMAKE_BINARY_DIR}/generated/version.h"
    @ONLY
)


# ============================================================================
#  11. Windows 资源文件处理
# ============================================================================
if(WIN32)
    configure_file(
        "${APP_RC_IN}"
        "${CMAKE_BINARY_DIR}/generated/${PROJECT_NAME}.rc"
        @ONLY
    )
    set(RESOURCE_FILES "${CMAKE_BINARY_DIR}/generated/${PROJECT_NAME}.rc")

    if(EXISTS "${APP_ICON}")
        configure_file(
            "${APP_ICON}"
            "${CMAKE_BINARY_DIR}/generated/${PROJECT_NAME}.ico"
            COPYONLY
        )
    else()
        message(WARNING "App icon not found: ${APP_ICON}")
    endif()

    if(ENABLE_UPGRADE)
        configure_file(
            "${UPGRADE_RC_IN}"
            "${CMAKE_BINARY_DIR}/generated/${UPGRADE_TARGET}.rc"
            @ONLY
        )
        set(UPGRADE_RESOURCE_FILES "${CMAKE_BINARY_DIR}/generated/${UPGRADE_TARGET}.rc")

        if(EXISTS "${UPGRADE_ICON}")
            configure_file(
                "${UPGRADE_ICON}"
                "${CMAKE_BINARY_DIR}/generated/${UPGRADE_TARGET}.ico"
                COPYONLY
            )
        else()
            message(WARNING "Upgrade icon not found: ${UPGRADE_ICON}")
        endif()
    endif()
else()
    set(RESOURCE_FILES "")
    set(UPGRADE_RESOURCE_FILES "")
endif()


# ============================================================================
#  12. C++ 标准与输出目录
# ============================================================================
set(CMAKE_CXX_STANDARD ${CPP_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# ============================================================================
#  13. 收集源文件
# ============================================================================
file(GLOB_RECURSE SOURCES       "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.c")
file(GLOB_RECURSE HEADERS       "${INC_DIR}/*.hpp" "${INC_DIR}/*.h")

if(ENABLE_UPGRADE)
    file(GLOB_RECURSE UPGRADE_SOURCES "${UPGRADE_DIR}/*.cpp" "${UPGRADE_DIR}/*.c")
endif()


# ============================================================================
#  14. 创建可执行目标
# ============================================================================
if(WIN32)
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCE_FILES})
else()
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
endif()

if(ENABLE_UPGRADE AND UPGRADE_SOURCES)
    if(WIN32)
        add_executable(${UPGRADE_TARGET} ${UPGRADE_SOURCES} ${UPGRADE_RESOURCE_FILES})
    else()
        add_executable(${UPGRADE_TARGET} ${UPGRADE_SOURCES})
    endif()
endif()


# ============================================================================
#  15. 查找并链接依赖库 — 在这里添加你的 find_package 和链接
# ============================================================================
message(STATUS "Using vcpkg dependencies")

# 在这里添加你的 find_package 调用
# 示例：
#   find_package(glfw3 CONFIG REQUIRED)
#   find_package(nlohmann_json CONFIG REQUIRED)

set(TRAYPP_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/traypp")
if(NOT EXISTS "${TRAYPP_SOURCE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR "traypp source directory not found: ${TRAYPP_SOURCE_DIR}")
endif()

add_subdirectory("${TRAYPP_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/traypp" EXCLUDE_FROM_ALL)

find_package(glfw3 CONFIG REQUIRED)
find_package(clip CONFIG REQUIRED)
find_package(unofficial-skia CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(utf8cpp CONFIG REQUIRED)

# ---- 编译定义 ----
target_compile_definitions(${PROJECT_NAME} PRIVATE
    NOMINMAX
    $<$<CONFIG:Debug>:DEBUG_MODE>
)

# ---- 包含目录 ----
target_include_directories(${PROJECT_NAME} PRIVATE
    ${INC_DIR}
    ${CMAKE_BINARY_DIR}/generated
    ${SRC_DIR}
    # 在这里添加你的 xxx_INCLUDE_DIRS
)

# ---- 链接 ----
target_link_libraries(${PROJECT_NAME} PRIVATE
    # 在这里添加你的链接库
    tray
    glfw
    clip::clip
    unofficial::skia::skia
    unofficial::skia::modules::skparagraph
    nlohmann_json::nlohmann_json
    utf8::cpp utf8cpp::utf8cpp
)

# ---- upgrade 子程序（如启用） ----
if(ENABLE_UPGRADE)
    target_include_directories(${UPGRADE_TARGET} PRIVATE
        ${CMAKE_BINARY_DIR}/generated
    )
    target_link_libraries(${UPGRADE_TARGET} PRIVATE
        # 在这里添加 upgrade 的链接库
    )
endif()


# ============================================================================
#  16. 自动递增构建号（可选）
# ============================================================================
if(EXISTS "${CMAKE_SOURCE_DIR}/build_number.txt")
    file(READ "${CMAKE_SOURCE_DIR}/build_number.txt" BUILD_NUMBER)
    string(STRIP "${BUILD_NUMBER}" BUILD_NUMBER)
    math(EXPR BUILD_NUMBER "${BUILD_NUMBER} + 1")
else()
    set(BUILD_NUMBER 1)
endif()
file(WRITE "${CMAKE_SOURCE_DIR}/build_number.txt" "${BUILD_NUMBER}")
set(PROJECT_VERSION_BUILD ${BUILD_NUMBER})


# ============================================================================
#  17. 平台特定设置
# ============================================================================
if(WIN32)
    # --- Windows 通用链接 ---
    target_link_libraries(${PROJECT_NAME} PRIVATE
        opengl32
        gdi32
        user32
        kernel32
        shell32
        ole32
    )

    if(ENABLE_UPGRADE)
        target_link_libraries(${UPGRADE_TARGET} PRIVATE
            kernel32 user32 wininet urlmon shell32
        )
        target_link_options(${UPGRADE_TARGET} PRIVATE
            $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>
            $<$<CONFIG:Release>:/ENTRY:mainCRTStartup>
            $<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE>
        )
    endif()

    # 复制图标到输出目录
    if(EXISTS "${APP_ICON}")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${APP_ICON}"
                "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/${PROJECT_NAME}.ico"
            COMMENT "Copying app icon"
        )
    endif()

    if(ENABLE_UPGRADE AND EXISTS "${UPGRADE_ICON}")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${UPGRADE_ICON}"
                "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/${UPGRADE_TARGET}.ico"
            COMMENT "Copying upgrade icon"
        )
    endif()

    # 静态/混合链接：添加系统库
    if(VCPKG_TARGET_TRIPLET MATCHES "static" OR USE_MIXED_LINKING)
        target_link_libraries(${PROJECT_NAME} PRIVATE
            kernel32 user32 gdi32 winspool shell32 ole32 oleaut32
            uuid comdlg32 advapi32 secur32 ws2_32 winmm
        )
        if(ENABLE_UPGRADE)
            target_link_libraries(${UPGRADE_TARGET} PRIVATE
                kernel32 user32 gdi32 winspool shell32 ole32 oleaut32
                uuid comdlg32 advapi32 secur32 ws2_32 winmm wininet urlmon
            )
        endif()
    endif()

    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
    if(ENABLE_UPGRADE)
        target_compile_options(${UPGRADE_TARGET} PRIVATE /utf-8)
    endif()

    if(VCPKG_TARGET_TRIPLET MATCHES "static")
        set_property(TARGET ${PROJECT_NAME} PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        if(ENABLE_UPGRADE)
            set_property(TARGET ${UPGRADE_TARGET} PROPERTY
                MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        endif()
    endif()

elseif(APPLE)
    # --- macOS 框架 ---
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${COREVIDEO_LIBRARY}
    )

elseif(UNIX)
    # --- Linux X11 ---
    find_package(X11 REQUIRED)
    if(X11_FOUND)
        target_include_directories(${PROJECT_NAME} PRIVATE ${X11_INCLUDE_DIR})
        target_link_libraries(${PROJECT_NAME} PRIVATE ${X11_LIBRARIES})
    endif()

    # glfw 别名
    if(NOT TARGET glfw)
        find_package(glfw3 REQUIRED)
        if(TARGET glfw3)
            set_target_properties(glfw3 PROPERTIES IMPORTED_GLOBAL TRUE)
            add_library(glfw ALIAS glfw3)
        endif()
    endif()
endif()


# ============================================================================
#  18. WSL 环境优化
# ============================================================================
if(UNIX AND NOT APPLE)
    execute_process(
        COMMAND grep -q Microsoft /proc/version
        RESULT_VARIABLE IS_WSL
        OUTPUT_QUIET ERROR_QUIET
    )
    if(IS_WSL EQUAL 0)
        message(STATUS "WSL environment detected, applying optimizations")
        target_compile_definitions(${PROJECT_NAME} PRIVATE WSL_BUILD=1)
        target_compile_options(${PROJECT_NAME} PRIVATE
            -O1
            -fno-strict-aliasing
            -fstack-protector-strong
            -D_FORTIFY_SOURCE=2
            -fno-omit-frame-pointer
            -fsanitize=address
            -g
        )
        target_link_options(${PROJECT_NAME} PRIVATE
            -fsanitize=address
            -Wl,-z,relro,-z,now
        )
        target_link_libraries(${PROJECT_NAME}
            ${OPENGL_LIBRARIES}
            ${GLFW_LIBRARIES}
            ${CMAKE_DL_LIBS}
            pthread
        )
    endif()
endif()


# ============================================================================
#  19. 安装规则
# ============================================================================
if(ENABLE_UPGRADE)
    install(TARGETS ${PROJECT_NAME} ${UPGRADE_TARGET} DESTINATION bin)
else()
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
endif()


# ============================================================================
#  20. 复制资源文件到输出目录
# ============================================================================
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCE_DIR}")
    # 确定最终输出目录
    if(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        set(OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    else()
        set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")
    endif()

    # 在构建后复制
    add_custom_command(TARGET screen-shot POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCE_DIR}"
            "${OUTPUT_DIR}/$<CONFIG>/files"
        COMMENT "Copying resources to executable directory"
    )
endif()


# ============================================================================
#  21. DLL 复制（仅 Windows 动态链接时需要）
# ============================================================================
if(WIN32 AND VCPKG_TARGET_TRIPLET MATCHES "static")
    add_custom_target(copy_dlls ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release"
        COMMAND ${CMAKE_COMMAND} -E echo "Static linking - no DLLs to copy"
        COMMENT "Static linking mode"
    )
elseif(WIN32)
    add_custom_target(copy_dlls ALL)
    set(VCPKG_BIN_DIR "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/bin")
    add_custom_command(TARGET copy_dlls POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release"
        COMMAND ${CMAKE_COMMAND} -E echo "Checking for DLLs in ${VCPKG_BIN_DIR}"
        COMMAND if exist "${VCPKG_BIN_DIR}" (
            ${CMAKE_COMMAND} -E copy_directory "${VCPKG_BIN_DIR}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release"
        ) else (
            echo "No DLL directory found"
        )
        COMMENT "Copying DLLs for dynamic linking"
    )
else()
    add_custom_target(copy_dlls ALL
        COMMAND ${CMAKE_COMMAND} -E echo "Non-Windows platform - no DLLs to copy"
        COMMENT "Non-Windows platform"
    )
endif()
