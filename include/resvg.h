// resvg C API — 来自 resvg 官方 crates/c-api（resvg-capi 0.48.1）
// 该头文件由官方 cbindgen 生成，此处为与官方 ABI 一致的核心声明。
// SPDX-License-Identifier: Apache-2.0 OR MIT
#ifndef RESVG_H
#define RESVG_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define RESVG_MAJOR_VERSION 0
#define RESVG_MINOR_VERSION 48
#define RESVG_PATCH_VERSION 1
#define RESVG_VERSION "0.48.1"

/**
 * @brief List of possible errors.
 */
typedef enum {
    /** Everything is ok. */
    RESVG_OK = 0,
    /** Only UTF-8 content are supported. */
    RESVG_ERROR_NOT_AN_UTF8_STR,
    /** SVGZ decoding is unsupported. */
    RESVG_ERROR_SVGZ_UNSUPPORTED,
    /** Failed to open the provided file. */
    RESVG_ERROR_FILE_OPEN_FAILED,
    /** Compressed SVG must use the GZip algorithm. */
    RESVG_ERROR_MALFORMED_GZIP,
    /** We do not allow SVG with more than 1_000_000 elements for security reasons. */
    RESVG_ERROR_ELEMENTS_LIMIT_REACHED,
    /** SVG doesn't have a valid size. */
    RESVG_ERROR_INVALID_SIZE,
    /** Failed to parse an SVG data. */
    RESVG_ERROR_PARSING_FAILED,
} resvg_error;

/** @brief A size representation. */
typedef struct resvg_size {
    float width;
    float height;
} resvg_size;

/** @brief A rectangle representation. */
typedef struct resvg_rect {
    float x;
    float y;
    float width;
    float height;
} resvg_rect;

/** @brief A 2D transform representation. */
typedef struct resvg_transform {
    float a;
    float b;
    float c;
    float d;
    float e;
    float f;
} resvg_transform;

/** @brief An SVG to resvg_render_tree conversion options. */
typedef struct resvg_options resvg_options;

/** @brief An opaque pointer to the rendering tree. */
typedef struct resvg_render_tree resvg_render_tree;

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Creates an identity transform. */
resvg_transform resvg_transform_identity(void);

/** @brief Initializes the library log. */
void resvg_init_log(void);

/** @brief Creates a new resvg_options object. */
resvg_options* resvg_options_create(void);

/** @brief Destroys the resvg_options. */
void resvg_options_destroy(resvg_options* opt);

/** @brief Sets a directory that will be used during relative paths resolving. */
void resvg_options_set_resources_dir(resvg_options* opt, const char* path);

/** @brief Sets the target DPI. Default: 96. */
void resvg_options_set_dpi(resvg_options* opt, float dpi);

/** @brief Provides the content of a stylesheet that will be used when resolving CSS attributes. */
void resvg_options_set_stylesheet(resvg_options* opt, const char* content);

/** @brief Sets the default font family. */
void resvg_options_set_font_family(resvg_options* opt, const char* family);

/** @brief Loads system fonts into the internal fonts database. */
void resvg_options_load_system_fonts(resvg_options* opt);

/** @brief Creates resvg_render_tree from data. Returns resvg_error. */
int resvg_parse_tree_from_data(const char* data, size_t len,
                               const resvg_options* opt,
                               resvg_render_tree** tree);

/** @brief Creates resvg_render_tree from file. Returns resvg_error. */
int resvg_parse_tree_from_file(const char* file_path,
                               const resvg_options* opt,
                               resvg_render_tree** tree);

/** @brief Destroys the resvg_render_tree. */
void resvg_tree_destroy(resvg_render_tree* tree);

/** @brief Checks that tree has any nodes. */
bool resvg_is_image_empty(const resvg_render_tree* tree);

/** @brief Returns an image size required to render this SVG. */
resvg_size resvg_get_image_size(const resvg_render_tree* tree);

/**
 * @brief Renders the resvg_render_tree onto the pixmap.
 *
 * @param tree A render tree.
 * @param transform A root SVG transform.
 * @param width Pixmap width.
 * @param height Pixmap height.
 * @param pixmap Pixmap data. Should have width*height*4 size and contain
 *               premultiplied RGBA8888 pixels.
 */
void resvg_render(const resvg_render_tree* tree, resvg_transform transform,
                  uint32_t width, uint32_t height, char* pixmap);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RESVG_H */
