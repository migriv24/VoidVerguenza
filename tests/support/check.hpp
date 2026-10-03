/* tests/support/check.hpp — the smallest test harness that says where it failed. */
#pragma once

#include <cstdio>

namespace vtest {
inline int& failures() {
    static int n = 0;
    return n;
}
inline int finish(const char* suite) {
    if (failures()) std::fprintf(stderr, "%s: %d failure(s)\n", suite, failures());
    else std::printf("%s: all checks passed\n", suite);
    return failures() ? 1 : 0;
}
} // namespace vtest

#define CHECK(cond)                                                                  \
    do {                                                                             \
        if (!(cond)) {                                                               \
            ++vtest::failures();                                                     \
            std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);     \
        }                                                                            \
    } while (0)

#define CHECK_OK(status_expr)                                                        \
    do {                                                                             \
        auto _st = (status_expr);                                                    \
        if (!_st) {                                                                  \
            ++vtest::failures();                                                     \
            std::fprintf(stderr, "FAIL %s:%d: %s -> %s\n", __FILE__, __LINE__, #status_expr, _st.error().c_str()); \
        }                                                                            \
    } while (0)
