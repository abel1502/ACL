#include <cstdio>
#include <cstdarg>
#include <cerrno>

#include <ACL/general.h>


namespace abel {


int verbosity = 0;

void dbg_(bool isError, int level, const char *funcName, int lineNo, const char *msg, ...) {
    va_list args = {};
    va_start(args, msg);

    if (verbosity >= level) {
        fprintf(stderr, "[%s in %s() on #%d] ", isError ? "ERROR" : "DBG", funcName, lineNo);
        vfprintf(stderr, msg, args);
        fprintf(stderr, "\n");
        if (errno != 0 && isError) {
            perror("System error");
        }
    }

    va_end(args);
}

unsigned long long randSeed = 123;

unsigned long long randLL() {
    randSeed ^= randSeed << 21;
    randSeed ^= randSeed >> 35;
    randSeed ^= randSeed << 4;

    return randSeed;
}

std::string vsprintfxx(const char *fmt, va_list args) {
    va_list args2{};

    va_copy(args2, args);
    int reqSize = vsnprintf(nullptr, 0, fmt, args2);
    va_end(args2);

    assert(reqSize >= 0);

    std::string result{};
    result.resize((size_t)reqSize + 1);

    int writtenSize = vsnprintf(result.data(), (size_t)reqSize + 1, fmt, args);

    assert(writtenSize > 0 && writtenSize <= reqSize);

    result.resize(writtenSize);

    return result;
}


}

