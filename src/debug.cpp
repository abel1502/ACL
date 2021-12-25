#include <ACL/debug.h>
#include <cassert>

#ifdef __GNUC__
#define IMPLEMENT_GCC_DEMANGLE_
#include <cxxabi.h>
#endif


namespace abel {


OpTracer::OpTracer() {
    msg("ctor");

    doCtor();
}

OpTracer::OpTracer(const OpTracer &other) {
    msg("copy ctor", other);

    doCtor();
}

OpTracer &OpTracer::operator=(const OpTracer &other) {
    msg("copy asgn", other);

    doAsgn();

    return *this;
}

#pragma warning(suppress : 26439)
OpTracer::OpTracer(OpTracer &&other) {
    msg("move ctor", other);

    doCtor();
}

#pragma warning(suppress : 26439)
OpTracer &OpTracer::operator=(OpTracer &&other) {
    msg("move asgn", other);

    doAsgn();

    return *this;
}

OpTracer::~OpTracer() {
    msg("dtor");

    doDtor();
}


void OpTracer::doCtor() {
    if (balance != 0)
        bad();

    balance++;
}

void OpTracer::doDtor() {
    if (balance != 1)
        bad();

    balance--;
}

void OpTracer::doAsgn() {
    if (balance != 1)
        bad();
}

void OpTracer::msg(const char *src) {
    DBG("OpTracer [%p].%d %s\n", this, balance, src);
}

void OpTracer::msg(const char *src, const OpTracer &other) {
    DBG("OpTracer [%p].%d %s [%p].%d\n", this, balance, src, &other, other.balance);
}

void OpTracer::bad() {
    ERR("OpTracer bad usage!!");
    abort();
}


#ifdef IMPLEMENT_GCC_DEMANGLE_
std::string demangle(const char *name) {
    assert(name);

    int status = 0;
    char *result = abi::__cxa_demangle(name, nullptr, nullptr, &status);

    if (status || !result)
        throw error("__cxa_demangle failed");

    std::string strResult{result};
    free(result);
    return strResult;
}
#else
// Can't guarantee demangling there
std::string demangle(const char *name) {
    assert(name);

    return std::string{name};
}
#endif


}

#ifdef IMPLEMENT_GCC_DEMANGLE_
#undef IMPLEMENT_GCC_DEMANGLE_
#endif
