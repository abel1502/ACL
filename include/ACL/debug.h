#ifndef ACL_DEBUG_H
#define ACL_DEBUG_H

#include <ACL/general.h>
#include <string>
#include <typeinfo>
#include <cstdlib>


namespace abel {


class OpTracer {
public:
    OpTracer();

    OpTracer(const OpTracer &other);

    OpTracer &operator=(const OpTracer &other);

    OpTracer(OpTracer &&other);

    OpTracer &operator=(OpTracer &&other);

    ~OpTracer();

protected:
    int balance = 0;

    void doCtor();

    void doDtor();

    void doAsgn();

    void msg(const char *src);

    void msg(const char *src, const OpTracer &other);

    [[noreturn]] void bad();

};


std::string demangle(const char *name);


template <typename T>
std::string getTypeName() {
    return demangle(typeid(T).name());
}


}


#endif // ACL_DEBUG_H
