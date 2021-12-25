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


}

