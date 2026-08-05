#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct native_theme_rgba {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} native_theme_rgba;

typedef struct native_theme_system_theme {
    bool is_dark;
    bool has_accent_color;
    native_theme_rgba accent_color;
    bool has_background_color;
    native_theme_rgba background_color;
    bool has_text_color;
    native_theme_rgba text_color;
} native_theme_system_theme;

bool native_theme_query_system_theme(native_theme_system_theme* out);
bool native_theme_query_is_dark(bool* out);

#ifdef __cplusplus
}
#endif
