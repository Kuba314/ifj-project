#pragma once

#ifdef DBG
int debug = 5;
// 5 or less: debug
// 6: info
// 7: warning
// 8: error
// 9: critical

    #define PRINT(severity, ...)                                                                   \
        if(severity >= dbgseverity && severity >= debug) {                                         \
            fprintf(stderr, __VA_ARGS__);                                                          \
        }

    #define DPRINT(severity, depth, ...)                                                           \
        if(severity >= dbgseverity) {                                                              \
            fprintf(stderr, "D%d: ", depth);                                                       \
            fprintf(stderr, __VA_ARGS__);                                                          \
        }

#else

    #define PRINT(...)                                                                             \
        do {                                                                                       \
        } while(0);

    #define DPRINT(...)                                                                            \
        do {                                                                                       \
        } while(0);

#endif
