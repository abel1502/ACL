#ifndef ACL_LOG_H
#define ACL_LOG_H

#if 0  // TODO: finish
#include <ACL/general.h>
#include <cstdio>
#include <cstdint>


namespace abel {


class Logger {
public:
    DECLARE_ERROR(error, abel::error)

    static Logger instance;

    using tag_t = uint64_t;

    /// Specialize this to define a tag
    template <tag_t TAG>
    static constexpr bool tagExists = false;

    enum Level {
        L_CRITICAL  = -1,
        L_ERR       =  0,
        L_WARN      =  1,
        L_INFO      =  2,
        L_DBG       =  3,
    };

    struct Params {
        /// Is compared to the verbosity to determine
        /// whether the message should be logged at all
        /// (level <= verbosity)
        int level = L_INFO;

        /// Determines the message category [TAG]s, as well as
        /// allowing to mask parts of the program's debug output.
        /// It should contain several or'ed tags.
        /// If it has any tags from the whitelist, and none from the blacklist,
        /// the message will be displayed
        tag_t tags = 0;

        /// Source file name
        const char *source = nullptr;

        /// Name of the function that has requested the logging,
        /// or nullptr if it shouldn't be displayed
        const char *func = nullptr;

        /// Same as above, but in a more expanded form.
        /// One of these is chosen depending on availability and logger configuration
        const char *prettyFunc = nullptr;

        /// Line number, if available
        unsigned lineno = nullptr;

        /// A quick flag to mark a log entry to be specially outlines
        bool specialAttention = false;

    };


    // TODO: Think of some arguments
    void init(...);

    void registerTag(tag_t tag, const char *name, bool enabled = true);

    tag_t registerTag(const char *name, bool enabled = true);

    // Unregistering tags is not an option - just compile without
    // registering them if you no longer need it
    // void unregisterTag(tag_t tag);

    void switchTag(tag_t tag, bool enabled);
    inline void  enableTag(tag_t tag) { switchTag(tag, true ); }
    inline void disableTag(tag_t tag) { switchTag(tag, false); }

    void log(const char *msg);

    /*template <typename ... Ts>
    void log(const char *fmt, Ts ... args);*/

protected:
    static const char *tagNames


    FILE *ofile;
    bool fileOwned;
    int verbosity;
    tag_t curMask;  // The categories that are debugged. If curMask & params.tags is non-zero, the message is displayed


};


}

#endif


#endif // ACL_LOG_H
