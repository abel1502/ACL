#ifndef ACL_LOG_H
#define ACL_LOG_H

#include <ACL/general.h>
#include <sstream>
#include <source_location>
#include <string_view>
#include <format>


// TODO: Lots of finishing, source_locations everywhere, etc...
namespace abel {


class Logger {
public:
    DECLARE_ERROR(error, abel::error)

    enum Level {
        L_CRITICAL  = -1,
        L_ERR       =  0,
        L_WARN      =  1,
        L_INFO      =  2,
        L_DBG       =  3,
    };


    constexpr Logger &global() {
        return instance;
    }

    #pragma region Log wrappers
    template <typename ... As>
    inline void log(Level level, const std::string_view &fmt_str, As &&... args,
                    const std::source_location &srcLoc = std::source_location::current()) const {
        return instance.log_(level, fmt_str,
                             std::make_format_args(std::forward<As>(args)...), srcLoc);
    }

    template <typename ... As>
    inline void critical(const char *fmt_str, As &&... args) {
        return log(L_CRITICAL, fmt_str, std::forward<As>(args)...);
    }

    template <typename ... As>
    inline void err(const char *fmt_str, As &&... args) {
        return log(L_ERR, fmt_str, std::forward<As>(args)...);
    }

    template <typename ... As>
    inline void warn(const char *fmt_str, As &&... args) {
        return log(L_WARN, fmt_str, std::forward<As>(args)...);
    }

    template <typename ... As>
    inline void info(const char *fmt_str, As &&... args) {
        return log(L_INFO, fmt_str, std::forward<As>(args)...);
    }

    template <typename ... As>
    inline void dbg(const char *fmt_str, As &&... args) {
        return log(L_DBG, fmt_str, std::forward<As>(args)...);
    }
    #pragma endregion Log wrappers

    #pragma region Static log wrappers
    template <typename ... As>
    static inline void glog(Level level, const std::string_view &fmt_str, As &&... args,
                           const std::source_location &srcLoc = std::source_location::current()) {
        return instance.log<As...>(level, fmt_str, std::forward<As>(args), srcLoc);
    }

    template <typename ... As>
    static inline void gcritical(const char *fmt_str, As &&... args) {
        return instance.critical(fmt_str, std::forward<As>(args)...);
    }

    template <typename ... As>
    static inline void gerr(const char *fmt_str, As &&... args) {
        return instance.err(fmt_str, std::forward<As>(args)...);
    }

    template <typename ... As>
    static inline void gwarn(const char *fmt_str, As &&... args) {
        return instance.warn(fmt_str, std::forward<As>(args)...);
    }

    template <typename ... As>
    static inline void ginfo(const char *fmt_str, As &&... args) {
        return instance.info(fmt_str, std::forward<As>(args)...);
    }

    template <typename ... As>
    static inline void gdbg(const char *fmt_str, As &&... args) {
        return instance.dbg(fmt_str, std::forward<As>(args)...);
    }
    #pragma endregion Static log wrappers

    #pragma region Configuration
    /// A message is displayed if and only if its level is <= verbosity
    Level verbosity = L_ERR;
    /// Messages are formatted twice: first by the user's per-call fmt string, and
    /// then by this special fmt string, which is required to use indexed formatting
    /// specifiers. Here's the available indices:
    ///  - 0 - The message itself
    ///  - 1 - The verbocity level flare ("DBG", "ERR", etc.)
    ///  - 2 - Current time (uses format args)
    ///  - 3 - Current source location (uses format args)
    ///  - Nothing more yet, to be continued...
    std::string_view message_fmt = "[{2:}] [{1}] [{3:}]: {0}\n";
    #pragma endregion Configuration

    #pragma region Ctors & dtors
    Logger() noexcept = default;
    Logger(const Logger &other) noexcept = default;
    Logger &operator=(const Logger &other) noexcept = default;
    Logger(Logger &&other) noexcept = default;
    Logger &operator=(Logger &&other) noexcept = default;
    ~Logger() noexcept = default;
    #pragma endregion Ctors & dtors

protected:
    static Logger instance;


    void log_(Level level, const std::string_view &fmt_str,
              std::format_args args, const std::source_location &srcLoc) const {
        if (level > verbosity) {
            return;
        }

        std::string msg = std::vformat(fmt_str, std::move(args));
        std::string result = std::format(message_fmt, msg /*, TODO: Expand */);

        write_(result);
    }

    void write_(const std::string_view &data) const {
        //
    }

};


}

#endif // ACL_LOG_H
