//
//  logger.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/3/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef logger_h
#define logger_h

#include <stdarg.h>

#include "../env/env.h"

typedef enum logger_mode {
    LOG_T = 1,
    LOG_D = 2,
    LOG_I = 4,
    LOG_W = 8,
    LOG_E = 16,
    LOG_F = 32
} logger_mode;

class logger {
private:
    static bool initialized;
    static const char* log_name;
    static int modes_enabled;
    
    static char* get_date_string();
    static bool  file_exists(char* file_name);
    static void  create_bak(char* prev, char* dir_path);
    static void  create_bak_r(char* prev, char* dir_path, int idx);
    static void  get_full_path(char* path);
    static void  wrap_lines(char* msg, int msg_size);
    static void  write_log(logger_mode mode, const char* str, va_list args);
    
public:
    static void t(const char* str, ...);
    static void d(const char* str, ...);
    static void i(const char* str, ...);
    static void w(const char* str, ...);
    static void e(const char* str, ...);
    static void f(const char* str, ...);
    static void create(const char* name);
    static void set_modes_enabled(int modes);
};

#endif /* logger_h */
