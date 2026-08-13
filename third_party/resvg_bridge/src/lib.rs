// resvg 官方 C API 实现。
// 等价于 resvg 官方仓库 crates/c-api/lib.rs（resvg-capi 0.48.1），
// 依赖 crates.io 的 resvg 主 crate（0.48）。
// SPDX-License-Identifier: Apache-2.0 OR MIT

#![allow(non_camel_case_types)]

use std::ffi::CStr;
use std::os::raw::c_char;
use std::slice;

use resvg::tiny_skia;
use resvg::usvg;

/// @brief List of possible errors.
#[repr(C)]
#[derive(Copy, Clone)]
pub enum resvg_error {
    /// Everything is ok.
    OK = 0,
    /// Only UTF-8 content are supported.
    NOT_AN_UTF8_STR,
    /// SVGZ decoding is unsupported.
    SVGZ_UNSUPPORTED,
    /// Failed to open the provided file.
    FILE_OPEN_FAILED,
    /// Compressed SVG must use the GZip algorithm.
    MALFORMED_GZIP,
    /// We do not allow SVG with more than 1_000_000 elements for security reasons.
    ELEMENTS_LIMIT_REACHED,
    /// SVG doesn't have a valid size.
    INVALID_SIZE,
    /// Failed to parse an SVG data.
    PARSING_FAILED,
}

/// @brief A rectangle representation.
#[repr(C)]
#[derive(Copy, Clone)]
pub struct resvg_rect {
    pub x: f32,
    pub y: f32,
    pub width: f32,
    pub height: f32,
}

/// @brief A size representation.
#[repr(C)]
#[derive(Copy, Clone)]
pub struct resvg_size {
    pub width: f32,
    pub height: f32,
}

/// @brief A 2D transform representation.
#[repr(C)]
#[derive(Copy, Clone)]
pub struct resvg_transform {
    pub a: f32,
    pub b: f32,
    pub c: f32,
    pub d: f32,
    pub e: f32,
    pub f: f32,
}

impl resvg_transform {
    #[inline]
    fn to_tiny_skia(&self) -> tiny_skia::Transform {
        tiny_skia::Transform::from_row(self.a, self.b, self.c, self.d, self.e, self.f)
    }
}

/// @brief Creates an identity transform.
#[unsafe(no_mangle)]
pub extern "C" fn resvg_transform_identity() -> resvg_transform {
    resvg_transform { a: 1.0, b: 0.0, c: 0.0, d: 1.0, e: 0.0, f: 0.0 }
}

/// A simple stderr logger.
static LOGGER: SimpleLogger = SimpleLogger;
struct SimpleLogger;
impl log::Log for SimpleLogger {
    fn enabled(&self, metadata: &log::Metadata) -> bool {
        metadata.level() <= log::LevelFilter::Warn
    }

    fn log(&self, record: &log::Record) {
        if self.enabled(record.metadata()) {
            let target = if record.target().len() > 0 {
                record.target()
            } else {
                record.module_path().unwrap_or_default()
            };
            let line = record.line().unwrap_or(0);
            let args = record.args();
            match record.level() {
                log::Level::Error => eprintln!("Error (in {}:{}): {}", target, line, args),
                log::Level::Warn => eprintln!("Warning (in {}:{}): {}", target, line, args),
                log::Level::Info => eprintln!("Info (in {}:{}): {}", target, line, args),
                log::Level::Debug => eprintln!("Debug (in {}:{}): {}", target, line, args),
                log::Level::Trace => eprintln!("Trace (in {}:{}): {}", target, line, args),
            }
        }
    }

    fn flush(&self) {}
}

/// @brief Initializes the library log.
#[unsafe(no_mangle)]
pub extern "C" fn resvg_init_log() {
    if let Ok(()) = log::set_logger(&LOGGER) {
        log::set_max_level(log::LevelFilter::Warn);
    }
}

/// @brief An SVG to resvg_render_tree conversion options.
pub struct resvg_options {
    options: usvg::Options<'static>,
}

/// @brief Creates a new resvg_options object.
#[unsafe(no_mangle)]
pub extern "C" fn resvg_options_create() -> *mut resvg_options {
    Box::into_raw(Box::new(resvg_options {
        options: usvg::Options::default(),
    }))
}

#[inline]
fn cast_opt(opt: *mut resvg_options) -> &'static mut usvg::Options<'static> {
    unsafe {
        assert!(!opt.is_null());
        &mut (*opt).options
    }
}

/// @brief Sets a directory that will be used during relative paths resolving.
#[unsafe(no_mangle)]
pub extern "C" fn resvg_options_set_resources_dir(opt: *mut resvg_options, path: *const c_char) {
    if path.is_null() {
        cast_opt(opt).resources_dir = None;
    } else {
        cast_opt(opt).resources_dir = Some(cstr_to_str(path).unwrap().into());
    }
}

/// @brief Sets the target DPI. Default: 96.
#[unsafe(no_mangle)]
pub extern "C" fn resvg_options_set_dpi(opt: *mut resvg_options, dpi: f32) {
    cast_opt(opt).dpi = dpi;
}

/// @brief Provides the content of a stylesheet that will be used when resolving CSS attributes.
#[unsafe(no_mangle)]
pub extern "C" fn resvg_options_set_stylesheet(opt: *mut resvg_options, content: *const c_char) {
    if content.is_null() {
        cast_opt(opt).style_sheet = None;
    } else {
        cast_opt(opt).style_sheet = Some(cstr_to_str(content).unwrap().into());
    }
}

/// @brief Sets the default font family.
#[unsafe(no_mangle)]
pub extern "C" fn resvg_options_set_font_family(opt: *mut resvg_options, family: *const c_char) {
    cast_opt(opt).font_family = cstr_to_str(family).unwrap().to_string();
}

/// @brief Sets the default font size. Default: 12.
#[unsafe(no_mangle)]
pub extern "C" fn resvg_options_set_font_size(opt: *mut resvg_options, size: f32) {
    cast_opt(opt).font_size = size;
}

/// @brief Loads system fonts into the internal fonts database.
#[unsafe(no_mangle)]
pub extern "C" fn resvg_options_load_system_fonts(opt: *mut resvg_options) {
    cast_opt(opt).fontdb_mut().load_system_fonts();
}

/// @brief Destroys the resvg_options.
#[unsafe(no_mangle)]
pub extern "C" fn resvg_options_destroy(opt: *mut resvg_options) {
    unsafe {
        assert!(!opt.is_null());
        let _ = Box::from_raw(opt);
    };
}

/// @brief An opaque pointer to the rendering tree.
pub struct resvg_render_tree(pub usvg::Tree);

/// @brief Creates resvg_render_tree from file.
#[unsafe(no_mangle)]
pub extern "C" fn resvg_parse_tree_from_file(
    file_path: *const c_char,
    opt: *const resvg_options,
    tree: *mut *mut resvg_render_tree,
) -> i32 {
    let file_path = match cstr_to_str(file_path) {
        Some(v) => v,
        None => return resvg_error::NOT_AN_UTF8_STR as i32,
    };

    let raw_opt = unsafe {
        assert!(!opt.is_null());
        &*opt
    };

    let file_data = match std::fs::read(file_path) {
        Ok(data) => data,
        Err(_) => return resvg_error::FILE_OPEN_FAILED as i32,
    };

    let utree = usvg::Tree::from_data(&file_data, &raw_opt.options);
    let utree = match utree {
        Ok(t) => t,
        Err(e) => return convert_error(e) as i32,
    };

    let tree_box = Box::new(resvg_render_tree(utree));
    unsafe {
        *tree = Box::into_raw(tree_box);
    }
    resvg_error::OK as i32
}

/// @brief Creates resvg_render_tree from data.
#[unsafe(no_mangle)]
pub extern "C" fn resvg_parse_tree_from_data(
    data: *const c_char,
    len: usize,
    opt: *const resvg_options,
    tree: *mut *mut resvg_render_tree,
) -> i32 {
    let data = unsafe { slice::from_raw_parts(data as *const u8, len) };

    let raw_opt = unsafe {
        assert!(!opt.is_null());
        &*opt
    };

    let utree = usvg::Tree::from_data(data, &raw_opt.options);
    let utree = match utree {
        Ok(t) => t,
        Err(e) => return convert_error(e) as i32,
    };

    let tree_box = Box::new(resvg_render_tree(utree));
    unsafe {
        *tree = Box::into_raw(tree_box);
    }
    resvg_error::OK as i32
}

/// @brief Checks that tree has any nodes.
#[unsafe(no_mangle)]
pub extern "C" fn resvg_is_image_empty(tree: *const resvg_render_tree) -> bool {
    let tree = unsafe {
        assert!(!tree.is_null());
        &*tree
    };
    !tree.0.root().has_children()
}

/// @brief Returns an image size required to render this SVG.
#[unsafe(no_mangle)]
pub extern "C" fn resvg_get_image_size(tree: *const resvg_render_tree) -> resvg_size {
    let tree = unsafe {
        assert!(!tree.is_null());
        &*tree
    };
    let size = tree.0.size();
    resvg_size { width: size.width(), height: size.height() }
}

/// @brief Destroys the resvg_render_tree.
#[unsafe(no_mangle)]
pub extern "C" fn resvg_tree_destroy(tree: *mut resvg_render_tree) {
    unsafe {
        assert!(!tree.is_null());
        let _ = Box::from_raw(tree);
    };
}

fn cstr_to_str(text: *const c_char) -> Option<&'static str> {
    let text = unsafe {
        assert!(!text.is_null());
        CStr::from_ptr(text)
    };
    text.to_str().ok()
}

fn convert_error(e: usvg::Error) -> resvg_error {
    match e {
        usvg::Error::NotAnUtf8Str => resvg_error::NOT_AN_UTF8_STR,
        usvg::Error::SvgzFeatureNotEnabled => resvg_error::SVGZ_UNSUPPORTED,
        usvg::Error::MalformedGZip => resvg_error::MALFORMED_GZIP,
        usvg::Error::ElementsLimitReached => resvg_error::ELEMENTS_LIMIT_REACHED,
        usvg::Error::InvalidSize => resvg_error::INVALID_SIZE,
        usvg::Error::ParsingFailed(_) => resvg_error::PARSING_FAILED,
    }
}

/// @brief Renders the resvg_render_tree onto the pixmap.
#[unsafe(no_mangle)]
pub extern "C" fn resvg_render(
    tree: *const resvg_render_tree,
    transform: resvg_transform,
    width: u32,
    height: u32,
    pixmap: *mut c_char,
) {
    let tree = unsafe {
        assert!(!tree.is_null());
        &*tree
    };

    let pixmap_len = width as usize * height as usize * tiny_skia::BYTES_PER_PIXEL;
    let pixmap: &mut [u8] =
        unsafe { std::slice::from_raw_parts_mut(pixmap as *mut u8, pixmap_len) };
    let mut pixmap = tiny_skia::PixmapMut::from_bytes(pixmap, width, height).unwrap();

    resvg::render(&tree.0, transform.to_tiny_skia(), &mut pixmap)
}
