#include "window/Window.h"

#include <skia/gpu/ganesh/gl/GrGLDirectContext.h>
#include <skia/gpu/ganesh/gl/GrGLInterface.h>
#include <skia/gpu/ganesh/GrBackendSurface.h>
#include <skia/gpu/ganesh/gl/GrGLBackendSurface.h>
#include <skia/gpu/ganesh/SkSurfaceGanesh.h>
#include <core/SkColor.h>
#include <core/SkColorSpace.h>

#include "log.h"
#include "base.h"

#include <algorithm>
#include <chrono>

using namespace window;
using namespace core;

std::unordered_map<WindowType, Window*> window::windows;

Window::Window(WindowType type, int width, int height, const std::string& title) :
	type(type), width(width), height(height), title(title) {
	window::windows[type] = this;
}

void Window::start() {
	if (running) return;
	running = true;
	thread = std::thread(&Window::mainLoop, this);
}

void Window::stop() {
	if (!running) return;
	if (window) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		glfwPostEmptyEvent();
	}
	running = false;
}

void Window::join() {
	if (thread.joinable()) {
		thread.join();
	}
}

void Window::stopAndJoin() {
	stop();
	join();
}

void Window::createWindow() {
	if (window) {
		return;
	}

	if (!glfwInit()) {
		Log.level(Level::ERROR) << "Failed to initialize GLFW" << op::endl;
		running = false;
		return;
	}

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); 
	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!window) {
		Log.level(Level::ERROR) << "Failed to create GLFW window" << op::endl;
		running = false;
		return;
	}

	glfwSetWindowPos(window, 100, 100);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwFocusWindow(window);
	glfwPollEvents();
	if (!wantVisible.load()) {
		glfwHideWindow(window);
	}
	glfwSetWindowUserPointer(window, this);
	glfwSetWindowCloseCallback(window, [](GLFWwindow* win) {
		glfwHideWindow(win);
		glfwSetWindowShouldClose(win, GLFW_FALSE);
	});
	running = true;
}

void Window::resize() {
	if (!window) {
		return;
	}

	int fbWidth = 0;
	int fbHeight = 0;
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
	if (fbWidth <= 0 || fbHeight <= 0) {
		fbWidth = std::max(1, width);
		fbHeight = std::max(1, height);
	}

	width = fbWidth;
	height = fbHeight;

	surface.reset();
	grContext.reset();

	glfwMakeContextCurrent(window);

	auto interface = GrGLMakeNativeInterface();
	grContext = GrDirectContexts::MakeGL(interface);
	if (!grContext) {
		Log.level(Level::ERROR) << "Failed to create GrDirectContext" << op::endl;
		return;
	}

	GrGLFramebufferInfo framebufferInfo{};
	framebufferInfo.fFBOID = 0;
	framebufferInfo.fFormat = GL_RGBA8;

	auto backendRT = GrBackendRenderTargets::MakeGL(
		width,
		height,
		0,
		8,
		framebufferInfo);
	if (backendRT.isValid()) {
		surface = SkSurfaces::WrapBackendRenderTarget(
			grContext.get(),
			backendRT,
			kBottomLeft_GrSurfaceOrigin,
			kRGBA_8888_SkColorType,
			nullptr,
			nullptr);
	}

	if (!surface) {
		SkImageInfo info = SkImageInfo::Make(width, height, kN32_SkColorType, kPremul_SkAlphaType);
		surface = SkSurfaces::Raster(info);
	}
}

void Window::show() {
	wantVisible.store(true);
	if (window) {
		glfwShowWindow(window);
		glfwFocusWindow(window);
		glfwPollEvents();
		if(glfwGetWindowAttrib(window, GLFW_ICONIFIED)) {
			glfwRestoreWindow(window);
		}
	}
	else {
		start();
	}
}

void Window::hide() {
	wantVisible.store(false);
	if (window) {
		glfwHideWindow(window);
	}
}

bool Window::isOpen() const {
	return window != nullptr && !glfwWindowShouldClose(window);
}

void Window::pollEvents() {
	glfwPollEvents();
}

void Window::swapBuffers() {
	if (window) {
		glfwSwapBuffers(window);
	}
}

void Window::mainLoop() {
	createWindow();
	if (!window) {
		running = false;
		return;
	}

	resize();

	while (running.load()) {
		pollEvents();

		if (surface) {
			int fbWidth = 0;
			int fbHeight = 0;
			glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
			if (fbWidth > 0 && fbHeight > 0 && (fbWidth != width || fbHeight != height)) {
				resize();
			}

			SkCanvas* canvas = surface->getCanvas();
			if (canvas) {
				SkColor bgColor = core::isDarkMode ? SK_ColorDKGRAY : SK_ColorWHITE;
				canvas->clear(bgColor);
				onDraw(canvas);
			}
			if (grContext) {
				grContext->flush();
			}
		}

		swapBuffers();
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

	surface.reset();
	grContext.reset();
	if (window) {
		glfwDestroyWindow(window);
		window = nullptr;
	}
	running = false;
}

Window::~Window() {
	stop();
	join();
	auto it = windows.find(type);
	if (it != windows.end() && it->second == this) {
		windows.erase(it);
	}
}
