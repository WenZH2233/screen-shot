#pragma once

#include <GLFW/glfw3.h>
#include <core/SkPaint.h>
#include <core/SkCanvas.h>
#include <core/SkSurface.h>
#include <skia/gpu/ganesh/GrDirectContext.h>
#include <thread>
#include <atomic>
#include <string>
#include <unordered_map>

#include "Items/button.h"

namespace window{
    enum class WindowType {
        MainWindow,
        TipWindow,
        SettingWindow,
        MaskWindow
    };
    class Window{
    protected:
        GLFWwindow* window=nullptr;
        int width=0, height=0;
        std::string title="";
		sk_sp<SkSurface> surface=nullptr;
		sk_sp<GrDirectContext> grContext=nullptr;
		std::atomic_bool running{ false };
        std::atomic_bool wantVisible{ false };
        std::thread thread;
        WindowType type;
        std::vector<std::unique_ptr<window::Button>> buttons;
        Window()=default;
        Window(WindowType type, int width, int height, const std::string& title);
        void createWindow();
        void resize();
		void mainLoop();
    public:
        virtual ~Window();
		void start();
        void show();
        void hide();
        bool isOpen() const;
        void pollEvents();
        void swapBuffers();
		virtual void onDraw(SkCanvas* canvas) = 0;
        void stop();
        void join();
		void stopAndJoin();
    };
    extern std::unordered_map<WindowType, Window*> windows;
}