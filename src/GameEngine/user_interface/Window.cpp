#include <GameEngine/user_interface/Window.hpp>
#include <GameEngine/user_interface/Keyboard.hpp>
#include <GameEngine/user_interface/Mouse.hpp>
#include <GameEngine/utilities/External.hpp>
#include <GameEngine/Engine.hpp>
using namespace GameEngine;

WindowSettings Window::settings;

Window::Window() : keyboard(this), mouse(this)
{
    create_window(Window::settings);
}

Window::Window(const std::string& title, unsigned int width, unsigned int height) : keyboard(this), mouse(this)
{
    WindowSettings settings;
    settings.title = title;
    settings.width = width;
    settings.height = height;
    create_window(settings);
}

Window::Window(const WindowSettings& settings) : keyboard(this), mouse(this)
{
    create_window(settings);
}

Window::~Window()
{
    glfwDestroyWindow(_glfw_window);
    vkDestroySurfaceKHR(Engine::get_vulkan_instance(), _vk_surface, nullptr);
}

void Window::update()
{
    //Polling events
    keyboard.set_unchanged();
    mouse.set_unchanged();
    glfwPollEvents();
    //Drawing to screen

    //Engine::shader_test->draw();
    glfwSwapBuffers(_glfw_window);
}

void Window::resize(unsigned int width, unsigned int height)
{
    glfwSetWindowSize(_glfw_window, width, height);
}

void Window::close()
{
    glfwSetWindowShouldClose(_glfw_window, true);
}

bool Window::is_open() const
{
    return !glfwWindowShouldClose(_glfw_window);
}

void Window::title(const std::string& name)
{
    _title = name;
    return glfwSetWindowTitle(_glfw_window, name.c_str());
}

const std::string& Window::title() const
{
    return _title;
}

bool Window::has_focus()
{
    return glfwGetWindowAttrib(_glfw_window, GLFW_FOCUSED);
}

void Window::focus()
{
    glfwFocusWindow(_glfw_window);
}

bool Window::resizable()
{
    return glfwGetWindowAttrib(_glfw_window, GLFW_RESIZABLE);
}

void Window::resizable(bool resizable)
{
    glfwSetWindowAttrib(_glfw_window, GLFW_RESIZABLE, resizable);
}

bool Window::borders()
{
    return glfwGetWindowAttrib(_glfw_window, GLFW_DECORATED);
}

void Window::borders(bool borders)
{
    glfwSetWindowAttrib(_glfw_window, GLFW_DECORATED, borders);
}

bool Window::transparent()
{
    return glfwGetWindowAttrib(_glfw_window, GLFW_TRANSPARENT_FRAMEBUFFER);
}

void Window::transparent(bool transparent)
{
    glfwSetWindowAttrib(_glfw_window, GLFW_TRANSPARENT_FRAMEBUFFER, transparent);
}

bool Window::vsync() const
{
    return _vsync;
}

void Window::vsync(bool enabled)
{
    _vsync = enabled;
    if (enabled)
    {
        glfwSwapInterval(1);
    }
    else
    {
        glfwSwapInterval(0);
    }
}

GLFWwindow* Window::glfw() const
{
    return _glfw_window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
    Window* h = static_cast<Window*>(glfwGetWindowUserPointer(window));
    h->resize(width, height);
}

void Window::create_window(const WindowSettings& settings)
{
    Engine::initialize();
    //Create the GLFW window
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, settings.transparent);
    glfwWindowHint(GLFW_DECORATED, settings.borders);
    unsigned int width = settings.width;
    unsigned int height = settings.height;
    GLFWmonitor* monitor = nullptr;
    const GLFWvidmode* mode = nullptr;
    if (settings.full_screen)
    {
        monitor = glfwGetPrimaryMonitor();
        mode = glfwGetVideoMode(monitor);
        width = mode->width;
        height = mode->height;
        resize(width, height);
    }
    _vsync = settings.vsync;
    _title = settings.title;
    _glfw_window = glfwCreateWindow(width, height, _title.c_str(), monitor, nullptr);
    if (_glfw_window == nullptr)
    {
        throw std::runtime_error("Failed to create the window");
    }
    //Link to the keyboard and mouse events
    glfwSetWindowUserPointer(_glfw_window, this);
    mouse.link();
    keyboard.link();
    //set the resize callback
    glfwSetWindowSizeCallback(_glfw_window, resize_callback);
    // Create the vkSurface
    VkResult result = glfwCreateWindowSurface(Engine::get_vulkan_instance(), _glfw_window, NULL, &_vk_surface);
    if (result != VK_SUCCESS)
    {
        THROW_ERROR("Failed to create the Vulkan surface")
    }

}
