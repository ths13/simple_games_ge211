#pragma once

#include "ge211_forward.h"

#include <exception>
#include <memory>
#include <sstream>
#include <string>

namespace ge211 {

/// An exception hierarchy for %ge211 to report errors.
namespace exceptions {

/// The root of the %ge211 exception hierarchy. Exceptions thrown by
/// %ge211 are derived from Exception_base.
///
/// The constructor of Exception_base is private, which means that
/// you cannot construct it directly, nor can you derive from it.
/// However, its derived class Client_logic_error has a public
/// constructor, so you can use it as you wish.
class Exception_base : public std::exception
{
public:
    /// The error message associated with the exception. This
    /// pointer is guaranteed to be good as long as the exception
    /// exists and hasn't been mutated. If you need it for longer,
    /// copy it to a std::string.
    const char* what() const noexcept override;

private:
    Exception_base(const std::string& message);

    /// Derived classes
    friend class Client_logic_error;
    friend class Environment_error;

    std::shared_ptr<const std::string> message_;
};

/// An exception that indicates that a logic error was performed
/// by the client. For example, a Client_logic_error is thrown by
/// Abstract_game::get_window() const if that function is called
/// before the Window has been created. Client code may throw or
/// derive from Client_logic_error as well.
class Client_logic_error : public Exception_base
{
public:
    /// Constructs the error, given the provided error message.
    Client_logic_error(const std::string& message);
};

/// Indicates that an error was encountered by the game engine or
/// in the client's environment.
/// This could indicate a problem with your video driver,
/// a missing file, or even a bug in %ge211 itself. The derived
/// classes indicate more precisely the nature of the condition.
class Environment_error : public Exception_base
{
    Environment_error(const std::string& message);

    /// Throwers
    friend class ge211::Window;

    /// Derived classes
    friend class Ge211_logic_error;
    friend class Host_error;
};

/// Indicates a condition unexpected by %ge211, that appears
/// to break its invariants. This may very well indicate a bug
/// in %ge211. Please report it if you see one of these.
class Ge211_logic_error : public Environment_error
{
    Ge211_logic_error(const std::string& message);

    /// Throwers
    friend class detail::Render_sprite;
    friend class sprites::Text_sprite;
};

/// Indicates an exception from the host environment being
/// passed along by %ge211. The host environment is usually
/// SDL2, so these exceptions may include a reason provided
/// by SDL2.
class Host_error : public Environment_error
{
    Host_error(const std::string& extra_message = "");

    /// Derived classes
    friend class File_error;
    friend class Font_error;
    friend class Image_error;

    /// Throwers
    friend class ge211::Window;
    friend class detail::Renderer;
    friend class detail::Render_sprite;
    friend class ge211::Text_sprite;
    friend class detail::Texture;
};

/// Indicates an error opening a file.
class File_error final : public Host_error
{
    File_error(const std::string& message);
    static File_error could_not_open(const std::string& filename);

    /// Thrower
    friend class detail::File_resource;
};

/// Indicates an error loading a font front an already-open file.
class Font_error final : public Host_error
{
    Font_error(const std::string& message);
    static Font_error could_not_load(const std::string& filename);

    /// Thrower
    friend class ge211::Font;
};

/// Indicates an error loading an image from an already-open file.
class Image_error final : public Host_error
{
    Image_error(const std::string& message);
    static Image_error could_not_load(const std::string& filename);

    /// Thrower
    friend class sprites::Image_sprite;
};

} // end namespace exception

namespace detail {

enum class Log_level
{
    debug,
    info,
    warn,
    fatal,
};

// Right now a Logger just keeps track of the current log
// level, and there's only one Logger (Singleton Pattern).
class Logger
{
public:
    using Level = Log_level;

    Level level() const noexcept { return level_; }
    void level(Level level) noexcept { level_ = level; }

    static Logger& instance() noexcept;

private:
    Logger() noexcept {}

    Level level_ = Level::warn;
};

// A Log_message accumulate information and then prints it all at
// once when it's going to be destroyed.
class Log_message
{
public:
    using Level = Log_level;

    Log_message(Level level = Level::debug);
    Log_message(std::string reason, Level level = Level::debug) noexcept;

    template <class T>
    Log_message& operator<<(const T& value)
    {
        if (active_) message_ << value;
        return *this;
    }

    // A Log_message has important work to do when it's destroyed.
    virtual ~Log_message();

    // A Log_message should not be copied.
    Log_message(const Log_message&) = delete;
    Log_message& operator=(const Log_message&) = delete;

    // But it can be moved.
    Log_message(Log_message&&) = default;
    Log_message& operator=(Log_message&&) = default;

private:
    std::string reason_;
    std::ostringstream message_;
    bool active_;
};

Log_message debug(std::string reason = "");
Log_message info(std::string reason = "");
Log_message warn(std::string reason = "");
Log_message fatal(std::string reason = "");

Log_message info_sdl();
Log_message warn_sdl();
Log_message fatal_sdl();

} // end namespace detail

}
