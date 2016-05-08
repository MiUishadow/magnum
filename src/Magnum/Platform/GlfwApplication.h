#ifndef Magnum_Platform_GlfwApplication_h
#define Magnum_Platform_GlfwApplication_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016 Jonathan Hale <squareys@googlemail.com>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

/** @file
 * @brief Class @ref Magnum::Platform::GlfwApplication, macro @ref MAGNUM_GLFWAPPLICATION_MAIN()
 */

#include <memory>
#include <string>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Platform/Platform.h"

/* We must include our own GL headers first to avoid conflicts */
#include "Magnum/OpenGL.h"

#include <glfw3.h>

namespace Magnum { namespace Platform {

/** @nosubgrouping
@brief GLFW application

Application using GLFW toolkit. Supports keyboard and mouse handling with
support for changing cursor and mouse tracking and warping.

This application library is available on all platforms where GLFW is ported. It
depends on **GLFW** library and is built if `WITH_GLFWAPPLICATION` is enabled
in CMake.

## Bootstrap application

Fully contained base application using @ref GlfwApplication along with CMake
setup is available in `base-glfw` branch of
[Magnum Bootstrap](https://github.com/mosra/magnum-bootstrap) repository,
download it as [tar.gz](https://github.com/mosra/magnum-bootstrap/archive/base-glfw.tar.gz)
or [zip](https://github.com/mosra/magnum-bootstrap/archive/base-glfw.zip) file.
After extracting the downloaded archive you can build and run the application
with these four commands:

    mkdir build && cd build
    cmake ..
    cmake --build .
    ./src/MyApplication # or ./src/Debug/MyApplication

See @ref cmake for more information.

## General usage

In CMake you need to request `GlfwApplication` component of `Magnum` package
and link to `Magnum::GlfwApplication` target. If no other application is
requested, you can also use generic `Magnum::Application` alias to simplify
porting. Again, see @ref building and @ref cmake for more information.

In C++ code you need to implement at least @ref drawEvent() to be able to draw
on the screen. The subclass can be then used directly in `main()` -- see
convenience macro @ref MAGNUM_GLFWAPPLICATION_MAIN(). See @ref platform for
more information.
@code
class MyApplication: public Platform::GlfwApplication {
    // implement required methods...
};
MAGNUM_GLFWAPPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included, this class is also aliased to
`Platform::Application` and the macro is aliased to `MAGNUM_APPLICATION_MAIN()`
to simplify porting.
*/
class GlfwApplication {
    public:
        /** @brief Application arguments */
        struct Arguments {
            /** @brief Constructor */
            /*implicit*/ constexpr Arguments(int& argc, char** argv) noexcept: argc{argc}, argv{argv} {}

            int& argc;      /**< @brief Argument count */
            char** argv;    /**< @brief Argument values */
        };

        class Configuration;
        class InputEvent;
        class KeyEvent;
        class MouseEvent;
        class MouseMoveEvent;
        class MouseScrollEvent;

        /** @copydoc Sdl2Application::Sdl2Application(const Arguments&, const Configuration&) */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit GlfwApplication(const Arguments& arguments, const Configuration& configuration = Configuration());
        #else
        /* To avoid "invalid use of incomplete type" */
        explicit GlfwApplication(const Arguments& arguments, const Configuration& configuration);
        explicit GlfwApplication(const Arguments& arguments);
        #endif

        /** @copydoc Sdl2Application::Sdl2Application(const Arguments&, std::nullptr_t) */
        explicit GlfwApplication(const Arguments& arguments, std::nullptr_t);

        /** @brief Copying is not allowed */
        GlfwApplication(const GlfwApplication&) = delete;

        /** @brief Moving is not allowed */
        GlfwApplication(GlfwApplication&&) = delete;

        /** @brief Copying is not allowed */
        GlfwApplication& operator=(const GlfwApplication&) = delete;

        /** @brief Moving is not allowed */
        GlfwApplication& operator=(GlfwApplication&&) = delete;

        /**
         * @brief Execute main loop
         * @return Value for returning from `main()`
         *
         * See @ref MAGNUM_GLFWAPPLICATION_MAIN() for usage information.
         */
        int exec();

        /** @brief Exit application main loop */
        void exit() {
            glfwSetWindowShouldClose(_window, true);
        }

    protected:
        /* Nobody will need to have (and delete) GlfwApplication*, thus this is
           faster than public pure virtual destructor */
        ~GlfwApplication();

        /** @copydoc Sdl2Application::createContext() */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        void createContext(const Configuration& configuration = Configuration());
        #else
        /* To avoid "invalid use of incomplete type" */
        void createContext(const Configuration& configuration);
        void createContext();
        #endif

        /** @copydoc Sdl2Application::tryCreateContext() */
        bool tryCreateContext(const Configuration& configuration);

        /** @{ @name Screen handling */

        /**
         * @brief Swap buffers
         *
         * Paints currently rendered framebuffer on screen.
         */
        void swapBuffers() { glfwSwapBuffers(_window); }

        /**
         * @brief Set swap interval
         *
         * Set `0` for no VSync, `1` for enabled VSync. Some platforms support
         * `-1` for late swap tearing. Default is driver-dependent.
         */
        void setSwapInterval(Int interval);

        /** @copydoc Sdl2Application::redraw() */
        void redraw() { _needsRedraw = true; }

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /** @copydoc Sdl2Application::viewportEvent() */
        virtual void viewportEvent(const Vector2i& size);

        /** @copydoc Sdl2Application::drawEvent() */
        virtual void drawEvent() = 0;

        /*@}*/

        /** @{ @name Keyboard handling */

        /** @copydoc Sdl2Application::keyPressEvent() */
        virtual void keyPressEvent(KeyEvent& event);

        /** @copydoc Sdl2Application::keyReleaseEvent() */
        virtual void keyReleaseEvent(KeyEvent& event);

        /*@}*/

        /** @{ @name Mouse handling */

    public:
        /**
         * @brief Mouse cursor
         *
         * @see @ref setMouseCursor()
         */
        enum class MouseCursor: int {
            Default = GLFW_CURSOR_NORMAL,  /**< Default cursor provided by parent window */
            Hidden = GLFW_CURSOR_HIDDEN,   /**< Hidden cursor */
            None = GLFW_CURSOR_DISABLED    /**< No cursor */
        };

        /** @brief Warp mouse cursor to given coordinates */
        void warpCursor(const Vector2i& position) {
            glfwSetCursorPos(_window, Double(position.x()), Double(position.y()));
        }

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /** @copydoc Sdl2Application::mousePressEvent() */
        virtual void mousePressEvent(MouseEvent& event);

        /** @copydoc Sdl2Application::mouseReleaseEvent() */
        virtual void mouseReleaseEvent(MouseEvent& event);

        /**
         * @brief Mouse move event
         *
         * Called when any mouse button is pressed and mouse is moved. Default
         * implementation does nothing.
         * @see @ref setMouseTracking()
         */
        virtual void mouseMoveEvent(MouseMoveEvent& event);

        /**
         * @brief Mouse scroll event
         *
         * Called when a scrolling device is used (mouse wheel or scrolling area
         * on touchpad). Default implementation does nothing.
         */
        virtual void mouseScrollEvent(MouseScrollEvent& event);

        /*@}*/

    private:
        static void staticViewportEvent(GLFWwindow*, int w, int h) {
            _instance->viewportEvent({w, h});
        }

        static void staticKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mod);

        static void staticMouseEvent(GLFWwindow* window, int button, int action, int mods);

        static void staticMouseMoveEvent(GLFWwindow* window, double x, double y);

        static void staticMouseScrollEvent(GLFWwindow* window, double xoffset, double yoffset);

        static void staticErrorCallback(int error, const char* description);

        static GlfwApplication* _instance;

        GLFWwindow* _window;
        std::unique_ptr<Platform::Context> _context;
        bool _needsRedraw;
};

/**
@brief Configuration

Double-buffered RGBA window with depth and stencil buffers.
@see @ref GlfwApplication(), @ref createContext(), @ref tryCreateContext()
*/
class GlfwApplication::Configuration {
    public:
        /**
         * @brief Context flag
         *
         * @see @ref Flags, @ref setFlags(), @ref Context::Flag
         */
        enum class Flag: Int {
            #ifdef GLFW_CONTEXT_NO_ERROR
            /**
             * Specifies whether errors should be generated by the context.
             * If enabled, situations that would have generated errors instead
             * cause undefined behavior.
             *
             * @note Supported since GLFW 3.2.
             */
            NoError = GLFW_CONTEXT_NO_ERROR,
            #endif

            Debug = GLFW_OPENGL_DEBUG_CONTEXT,  /**< Debug context */
            Stereo = GLFW_STEREO,               /**< Stereo rendering */
        };

        /**
         * @brief Context flags
         *
         * @see @ref setFlags()
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Window flag
         *
         * @see @ref WindowFlags, @ref setWindowFlags()
         */
        enum class WindowFlag: UnsignedShort {
            Fullscreen = 1 << 0,   /**< Fullscreen window */
            Resizeable = 1 << 1,   /**< Resizeable window */
            Hidden = 1 << 2,       /**< Hidden window */

            #ifdef GLFW_MAXIMIZED
            /**
             * Maximized window
             *
             * @note Supported since GLFW 3.2.
             */
            Maximized = 1 << 3,
            #endif

            Minimized = 1 << 4,    /**< Minimized window */
            Floating = 1 << 5,     /**< Window floating above others, top-most */

            /**
             * Automatically iconify (minimize) if fullscreen window loses
             * input focus
             */
            AutoIconify = 1 << 6,

            Focused = 1 << 7       /**< Window has input focus */
        };

        /**
         * @brief Window flags
         *
         * @see @ref setWindowFlags()
         */
        typedef Containers::EnumSet<WindowFlag> WindowFlags;

        /** @brief Cursor mode */
        enum class CursorMode: Int {
            /** Visible unconstrained cursor */
            Normal = GLFW_CURSOR_NORMAL,

            /** Hidden cursor */
            Hidden = GLFW_CURSOR_HIDDEN,

            /** Cursor hidden and locked window */
            Disabled = GLFW_CURSOR_DISABLED
        };

        /*implicit*/ Configuration();
        ~Configuration();

        /** @brief Window title */
        std::string title() const { return _title; }

        /**
         * @brief Set window title
         * @return Reference to self (for method chaining)
         *
         * Default is `"Magnum GLFW Application"`.
         */
        Configuration& setTitle(std::string title) {
            _title = std::move(title);
            return *this;
        }

        /** @brief Window size */
        Vector2i size() const { return _size; }

        /**
         * @brief Set window size
         * @return Reference to self (for method chaining)
         *
         * Default is `{800, 600}`.
         */
        Configuration& setSize(const Vector2i& size) {
            _size = size;
            return *this;
        }

        /** @brief Context flags */
        Flags flags() const { return _flags; }

        /**
         * @brief Set context flags
         * @return Reference to self (for method chaining)
         *
         * Default is no flag.
         */
        Configuration& setFlags(Flags flags) {
            _flags = flags;
            return *this;
        }

        /** @brief Window flags */
        WindowFlags windowFlags() const {
            return _windowFlags;
        }

        /**
         * @brief Set window flags
         * @return  Reference to self (for method chaining)
         *
         * Default is @ref WindowFlag::Focused.
         */
        Configuration& setWindowFlags(WindowFlags windowFlags) {
            _windowFlags = windowFlags;
            return *this;
        }

        /** @brief Cursor mode */
        CursorMode cursorMode() const {
            return _cursorMode;
        }

        /**
         * @brief Set cursor mode
         * @return  Reference to self (for method chaining)
         *
         * Default is @ref CursorMode::Normal.
         */
        Configuration& setCursorMode(CursorMode cursorMode) {
            _cursorMode = cursorMode;
            return *this;
        }

        /** @brief Context version */
        Version version() const { return _version; }

        /**
         * @brief Set context version
         *
         * If requesting version greater or equal to OpenGL 3.1, core profile
         * is used. The created context will then have any version which is
         * backwards-compatible with requested one. Default is
         * @ref Version::None, i.e. any provided version is used.
         */
        Configuration& setVersion(Version version) {
            _version = version;
            return *this;
        }

        /** @brief Sample count */
        Int sampleCount() const { return _sampleCount; }

        /**
         * @brief Set sample count
         * @return Reference to self (for method chaining)
         *
         * Default is `0`, thus no multisampling. The actual sample count is
         * ignored, GLFW either enables it or disables. See also
         * @ref Renderer::Feature::Multisampling.
         */
        Configuration& setSampleCount(Int count) {
            _sampleCount = count;
            return *this;
        }

        /** @brief sRGB-capable default framebuffer */
        bool isSRGBCapable() const {
            return _srgbCapable;
        }

        /**
         * @brief Set sRGB-capable default framebuffer
         * @return Reference to self (for method chaining)
         */
        Configuration& setSRGBCapable(bool enabled) {
            _srgbCapable = enabled;
            return *this;
        }

    private:
        std::string _title;
        Vector2i _size;
        Int _sampleCount;
        Version _version;
        Flags _flags;
        WindowFlags _windowFlags;
        CursorMode _cursorMode;
        bool _srgbCapable;
};

CORRADE_ENUMSET_OPERATORS(GlfwApplication::Configuration::Flags)
CORRADE_ENUMSET_OPERATORS(GlfwApplication::Configuration::WindowFlags)

/**
@brief Base for input events

@see @ref KeyEvent, @ref MouseEvent, @ref MouseMoveEvent, @ref keyPressEvent(),
    @ref mousePressEvent(), @ref mouseReleaseEvent(), @ref mouseMoveEvent()
*/
class GlfwApplication::InputEvent {
    public:
        /**
         * @brief Modifier
         *
         * @see @ref Modifiers, @ref KeyEvent::modifiers(),
         *      @ref MouseEvent::modifiers()
         */
        enum class Modifier: Int {
            Shift = GLFW_MOD_SHIFT,         /**< Shift */
            Ctrl = GLFW_MOD_CONTROL,        /**< Ctrl */
            Alt = GLFW_MOD_ALT,             /**< Alt */
            AltGr = GLFW_MOD_SUPER          /**< AltGr */
        };

        /**
         * @brief Set of modifiers
         *
         * @see @ref KeyEvent::modifiers(), @ref MouseEvent::modifiers(),
         *      @ref MouseMoveEvent::modifiers()
         */
        typedef Containers::EnumSet<Modifier> Modifiers;

        /** @brief Copying is not allowed */
        InputEvent(const InputEvent&) = delete;

        /** @brief Moving is not allowed */
        InputEvent(InputEvent&&) = delete;

        /** @brief Copying is not allowed */
        InputEvent& operator=(const InputEvent&) = delete;

        /** @brief Moving is not allowed */
        InputEvent& operator=(InputEvent&&) = delete;

        /** @copydoc Sdl2Application::InputEvent::setAccepted() */
        void setAccepted(bool accepted = true) { _accepted = accepted; }

        /** @copydoc Sdl2Application::InputEvent::isAccepted() */
        constexpr bool isAccepted() const { return _accepted; }

    protected:
        constexpr InputEvent(): _accepted(false) {}

        ~InputEvent() = default;

    private:
        bool _accepted;
};

CORRADE_ENUMSET_OPERATORS(GlfwApplication::InputEvent::Modifiers)

/**
@brief Key event

@see @ref keyPressEvent()
*/
class GlfwApplication::KeyEvent: public GlfwApplication::InputEvent {
    friend GlfwApplication;

    public:
        /**
         * @brief Key
         *
         * @see @ref key()
         */
        enum class Key: Int {
            Unknown = GLFW_KEY_UNKNOWN, /**< Unknown key */

            Enter = GLFW_KEY_ENTER,     /**< Enter */
            Esc = GLFW_KEY_ESCAPE,      /**< Escape */

            Up = GLFW_KEY_UP,           /**< Up arrow */
            Down = GLFW_KEY_DOWN,       /**< Down arrow */
            Left = GLFW_KEY_LEFT,       /**< Left arrow */
            Right = GLFW_KEY_RIGHT,     /**< Right arrow */
            Home = GLFW_KEY_HOME,       /**< Home */
            End = GLFW_KEY_END,         /**< End */
            PageUp = GLFW_KEY_PAGE_UP,  /**< Page up */
            PageDown = GLFW_KEY_PAGE_DOWN, /**< Page down */
            Backspace = GLFW_KEY_BACKSPACE, /**< Backspace */
            Insert = GLFW_KEY_INSERT,   /**< Insert */
            Delete = GLFW_KEY_DELETE,   /**< Delete */

            F1 = GLFW_KEY_F1,           /**< F1 */
            F2 = GLFW_KEY_F2,           /**< F2 */
            F3 = GLFW_KEY_F3,           /**< F3 */
            F4 = GLFW_KEY_F4,           /**< F4 */
            F5 = GLFW_KEY_F5,           /**< F5 */
            F6 = GLFW_KEY_F6,           /**< F6 */
            F7 = GLFW_KEY_F7,           /**< F7 */
            F8 = GLFW_KEY_F8,           /**< F8 */
            F9 = GLFW_KEY_F9,           /**< F9 */
            F10 = GLFW_KEY_F10,         /**< F10 */
            F11 = GLFW_KEY_F11,         /**< F11 */
            F12 = GLFW_KEY_F12,         /**< F12 */

            Space = ' ',                /**< Space */
            Tab = '\t',                 /**< Tab */
            Comma = ',',                /**< Comma */
            Period = '.',               /**< Period */
            Minus = '-',                /**< Minus */
            Plus = '+',                 /**< Plus */
            Slash = '/',                /**< Slash */
            Percent = '%',              /**< Percent */
            Smicolon = ';',             /**< Semicolon */
            Equal = '=',                /**< Equal */

            Zero = '0',                 /**< Zero */
            One = '1',                  /**< One */
            Two = '2',                  /**< Two */
            Three = '3',                /**< Three */
            Four = '4',                 /**< Four */
            Five = '5',                 /**< Five */
            Six = '6',                  /**< Six */
            Seven = '7',                /**< Seven */
            Eight = '8',                /**< Eight */
            Nine = '9',                 /**< Nine */

            A = 'a',                    /**< Letter A */
            B = 'b',                    /**< Letter B */
            C = 'c',                    /**< Letter C */
            D = 'd',                    /**< Letter D */
            E = 'e',                    /**< Letter E */
            F = 'f',                    /**< Letter F */
            G = 'g',                    /**< Letter G */
            H = 'h',                    /**< Letter H */
            I = 'i',                    /**< Letter I */
            J = 'j',                    /**< Letter J */
            K = 'k',                    /**< Letter K */
            L = 'l',                    /**< Letter L */
            M = 'm',                    /**< Letter M */
            N = 'n',                    /**< Letter N */
            O = 'o',                    /**< Letter O */
            P = 'p',                    /**< Letter P */
            Q = 'q',                    /**< Letter Q */
            R = 'r',                    /**< Letter R */
            S = 's',                    /**< Letter S */
            T = 't',                    /**< Letter T */
            U = 'u',                    /**< Letter U */
            V = 'v',                    /**< Letter V */
            W = 'w',                    /**< Letter W */
            X = 'x',                    /**< Letter X */
            Y = 'y',                    /**< Letter Y */
            Z = 'z',                    /**< Letter Z */

            /* Function keys */
            CapsLock = GLFW_KEY_CAPS_LOCK,      /**< Caps lock */
            ScrollLock = GLFW_KEY_SCROLL_LOCK,  /**< Scroll lock */
            NumLock = GLFW_KEY_NUM_LOCK,        /**< Num lock */
            PrintScreen = GLFW_KEY_PRINT_SCREEN,/**< Print screen */
            Pause = GLFW_KEY_PAUSE,             /**< Pause */
            NumZero = GLFW_KEY_KP_0,            /**< Numpad zero */
            NumOne = GLFW_KEY_KP_1,             /**< Numpad one */
            NumTwo = GLFW_KEY_KP_2,             /**< Numpad two */
            NumThree = GLFW_KEY_KP_3,           /**< Numpad three */
            NumFour = GLFW_KEY_KP_4,            /**< Numpad four */
            NumFive = GLFW_KEY_KP_5,            /**< Numpad five */
            NumSix = GLFW_KEY_KP_6,             /**< Numpad six */
            NumSeven = GLFW_KEY_KP_7,           /**< Numpad seven */
            NumEight = GLFW_KEY_KP_8,           /**< Numpad eight */
            NumNine = GLFW_KEY_KP_9,            /**< Numpad nine */
            NumDecimal = GLFW_KEY_KP_DECIMAL,   /**< Numpad decimal */
            NumDivide = GLFW_KEY_KP_DIVIDE,     /**< Numpad divide */
            NumMultiply = GLFW_KEY_KP_MULTIPLY, /**< Numpad multiply */
            NumSubtract = GLFW_KEY_KP_SUBTRACT, /**< Numpad subtract */
            NumAdd = GLFW_KEY_KP_ADD,           /**< Numpad add */
            NumEnter = GLFW_KEY_KP_ENTER,       /**< Numpad enter */
            NumEqual = GLFW_KEY_KP_EQUAL,       /**< Numpad equal */
            LeftShift = GLFW_KEY_LEFT_SHIFT,    /**< Left shift */
            LeftCtrl = GLFW_KEY_LEFT_CONTROL,   /**< Left control */
            LeftAlt = GLFW_KEY_LEFT_ALT,        /**< Left alt */
            LeftSuper = GLFW_KEY_LEFT_SUPER,    /**< Left super */
            RightShift = GLFW_KEY_RIGHT_SHIFT,  /**< Right shift */
            RightCtrl = GLFW_KEY_RIGHT_CONTROL, /**< Right control */
            RightAlt = GLFW_KEY_RIGHT_ALT,      /**< Right alt */
            RightSuper = GLFW_KEY_RIGHT_SUPER,  /**< Right super */
            Menu = GLFW_KEY_MENU                /**< Menu */
        };

        /** @brief Key */
        constexpr Key key() const { return _key; }

        /** @brief Modifiers */
        constexpr Modifiers modifiers() const { return _modifiers; }

    private:
        static Modifiers getCurrentGlfwModifiers(GLFWwindow* window);

        constexpr KeyEvent(Key key, Modifiers modifiers): _key(key), _modifiers(modifiers) {}

        const Key _key;
        const Modifiers _modifiers;
};

/**
@brief Mouse event

@see @ref MouseMoveEvent, @ref MouseScrollEvent, @ref mousePressEvent(), @ref mouseReleaseEvent()
*/
class GlfwApplication::MouseEvent: public GlfwApplication::InputEvent {
    friend GlfwApplication;

    public:
        /**
         * @brief Mouse button
         *
         * @see @ref button()
         */
        enum class Button: int {
            Left = GLFW_MOUSE_BUTTON_LEFT,        /**< Left button */
            Middle = GLFW_MOUSE_BUTTON_MIDDLE,    /**< Middle button */
            Right = GLFW_MOUSE_BUTTON_RIGHT,      /**< Right button */
            Button1 = GLFW_MOUSE_BUTTON_1,        /**< Mouse button 1 */
            Button2 = GLFW_MOUSE_BUTTON_2,        /**< Mouse button 2 */
            Button3 = GLFW_MOUSE_BUTTON_3,        /**< Mouse button 3 */
            Button4 = GLFW_MOUSE_BUTTON_4,        /**< Mouse button 4 */
            Button5 = GLFW_MOUSE_BUTTON_5,        /**< Mouse button 5 */
            Button6 = GLFW_MOUSE_BUTTON_6,        /**< Mouse button 6 */
            Button7 = GLFW_MOUSE_BUTTON_7,        /**< Mouse button 7 */
            Button8 = GLFW_MOUSE_BUTTON_8,        /**< Mouse button 8 */

            WheelUp = GLFW_MOUSE_BUTTON_LAST + 1,   /**< Mouse wheel up */
            WheelDown = GLFW_MOUSE_BUTTON_LAST + 2  /**< Mouse wheel down */
        };

        /** @brief Button */
        constexpr Button button() const { return _button; }

        /** @brief Modifiers */
        constexpr Modifiers modifiers() const { return _modifiers; }

    private:
        constexpr MouseEvent(Button button, Modifiers modifiers): _button(button), _modifiers(modifiers) {}

        const Button _button;
        const Modifiers _modifiers;
};

/**
@brief Mouse move event

@see @ref MouseEvent, @ref MouseScrollEvent, @ref mouseMoveEvent()
*/
class GlfwApplication::MouseMoveEvent: public GlfwApplication::InputEvent {
    friend GlfwApplication;

    public:
        /** @brief Position */
        constexpr Vector2i position() const { return _position; }

        /** @brief Modifiers */
        constexpr Modifiers modifiers() const { return _modifiers; }

    private:
        constexpr MouseMoveEvent(const Vector2i& position, Modifiers modifiers): _position(position), _modifiers(modifiers) {}

        const Vector2i _position;
        const Modifiers _modifiers;
};

/**
@brief Mouse scroll event

@see @ref MouseEvent, @ref MouseMoveEvent, @ref mouseScrollEvent()
*/
class GlfwApplication::MouseScrollEvent: public GlfwApplication::InputEvent {
    friend GlfwApplication;

    public:
        /** @brief Scroll offset */
        constexpr Vector2d offset() const { return _offset; }

        /** @brief Modifiers */
        constexpr Modifiers modifiers() const { return _modifiers; }

    private:
        constexpr MouseScrollEvent(const Vector2d& offset, Modifiers modifiers): _offset(offset), _modifiers(modifiers) {}

        const Vector2d _offset;
        const Modifiers _modifiers;
};

/** @hideinitializer
@brief Entry point for GLFW-based applications
@param className Class name

See @ref Magnum::Platform::GlfwApplication "Platform::GlfwApplication" for
usage information. This macro abstracts out platform-specific entry point code
and is equivalent to the following, see @ref portability-applications for more
information.
@code
int main(int argc, char** argv) {
    className app({argc, argv});
    return app.exec();
}
@endcode
When no other application header is included this macro is also aliased to
`MAGNUM_APPLICATION_MAIN()`.
*/
#define MAGNUM_GLFWAPPLICATION_MAIN(className)                              \
    int main(int argc, char** argv) {                                       \
        className app({argc, argv});                                        \
        return app.exec();                                                  \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_APPLICATION_MAIN
typedef GlfwApplication Application;
typedef BasicScreen<GlfwApplication> Screen;
typedef BasicScreenedApplication<GlfwApplication> ScreenedApplication;
#define MAGNUM_APPLICATION_MAIN(className) MAGNUM_GLFWAPPLICATION_MAIN(className)
#else
#undef MAGNUM_APPLICATION_MAIN
#endif
#endif

}}

#endif
