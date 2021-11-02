#pragma once
#include <string>
#include <GameEngine/utilities/External.hpp>
#include "Keyboard.hpp"
#include "Mouse.hpp"

namespace GameEngine
{
    class WindowSettings
    {
    public:
        ///< Title of the window
        std::string title = "window";
        ///< Width of the window
        unsigned int width = 500;
        ///< Height of the window
        unsigned int height = 500;
        ///< Number of transparency layers (for 3D rendering)
        unsigned int transparency_layers = 0;
        ///< If true the window is created in full screen mode
        bool full_screen = false;
        ///< If true, the window is resizable
        bool resizable = true;
        ///< If true the window has borders
        bool borders = true;
        ///< If true the window is transparent (if the alpha channel of the displayed image is inferior to 1.)
        bool transparent = false;
        ///< If true, the vsync of the window is enabled
        bool vsync = true;
        ///< Number of samples for the Multi Sample Anti Aliasing
        unsigned int anti_aliasing = 1;
    };

    class Window
    {
    public:
        Window();
        Window(const Window& other);
        Window(const std::string& title, unsigned int width, unsigned int height);
        Window(const WindowSettings& settings);
        ~Window();
    public:
        ///< Update the window's display, and the window's inputs (keyboard and mouse)
        void update();
        ///< Resize the window to the given width and height
        void resize(unsigned int width, unsigned int height);
        ///< Close the window
        void close();
        ///< Returns false if the user asked for the window to close
        bool is_open() const;
        ///< Set the title
        void title(const std::string& name);
        ///< Returns the title of the window
        const std::string& title() const;
        ///< Returns true if the window is currently selected
        bool has_focus();
        ///< Make this window the currently selected window
        void focus();
        ///< Returns true if the window is resizable by the user
        bool resizable();
        ///< Make the window resizable by the user or not
        void resizable(bool resizable);
        ///< Returns true if the window has borders
        bool borders();
        ///< Enable or disable the borders of the window
        void borders(bool borders);
        ///< Returns true if the window is transparent (it can been see through when the alpha of the image displayed is inferior to 1.)
        bool transparent();
        ///< Set whether the window is transparent or not
        void transparent(bool transparent);
        ///< Returns true if the vertical syncing is enabled (limits the FPS to monitor's refresh rate)
        bool vsync() const;
        ///< Enables or disable vertical syncing
        void vsync(bool enabled);
    public:
        void operator=(const Window& other);
    public:
        const VkSurfaceKHR& _get_vk_surface() const;
        ///< Returns the GLFW window pointer
        GLFWwindow* _glfw() const;
    public:
        ///< This function is called when a window is resized
        static void _resize_callback(GLFWwindow* window, int width, int height);
    public:
        Keyboard keyboard;
        Mouse mouse;
    protected:
        GLFWwindow* _glfw_window;
        VkSurfaceKHR _vk_surface;
        bool _vsync = true;
        std::string _title = "window";
    protected:
        void create_window(const WindowSettings& settings);
    };
}
