use native_theme::detect;
use serde::Serialize;
use std::sync::OnceLock;

#[repr(C)]
#[derive(Clone, Copy, Debug, Default, Serialize)]
pub struct NativeThemeRgba {
    pub r: u8,
    pub g: u8,
    pub b: u8,
    pub a: u8,
}

#[repr(C)]
#[derive(Clone, Copy, Debug, Default, Serialize)]
pub struct NativeThemeSystemTheme {
    pub is_dark: bool,
    pub has_accent_color: bool,
    pub accent_color: NativeThemeRgba,
    pub has_background_color: bool,
    pub background_color: NativeThemeRgba,
    pub has_text_color: bool,
    pub text_color: NativeThemeRgba,
}

impl From<native_theme::color::Rgba> for NativeThemeRgba {
    fn from(value: native_theme::color::Rgba) -> Self {
        Self {
            r: value.r,
            g: value.g,
            b: value.b,
            a: value.a,
        }
    }
}

fn map_system_theme() -> Option<NativeThemeSystemTheme> {
    let system = native_theme::SystemTheme::from_system().ok()?;
    let resolved = system.pick(system.mode);
    Some(NativeThemeSystemTheme {
        is_dark: system.mode.is_dark(),
        has_accent_color: true,
        accent_color: resolved.defaults.accent_color.into(),
        has_background_color: true,
        background_color: resolved.defaults.background_color.into(),
        has_text_color: true,
        text_color: resolved.defaults.text_color.into(),
    })
}

fn last_theme_cell() -> &'static OnceLock<NativeThemeSystemTheme> {
    static LAST_THEME: OnceLock<NativeThemeSystemTheme> = OnceLock::new();
    &LAST_THEME
}

#[unsafe(no_mangle)]
pub extern "C" fn native_theme_query_system_theme(out: *mut NativeThemeSystemTheme) -> bool {
    if out.is_null() {
        return false;
    }

    let Some(theme) = map_system_theme() else {
        return false;
    };

    unsafe {
        *out = theme;
    }

    let _ = last_theme_cell().set(theme);
    true
}

#[unsafe(no_mangle)]
pub extern "C" fn native_theme_query_is_dark(out: *mut bool) -> bool {
    if out.is_null() {
        return false;
    }

    let dark = detect::system_is_dark();
    unsafe {
        *out = dark;
    }
    true
}
