/**
 * IFJ21 Compiler
 *
 *  Copyright 2021 xruzaa00 Adam Ruza
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * @file debug.h
 *
 * @brief Debugging utilities
 */
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
