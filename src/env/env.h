//
//  env.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/7/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef env_h
#define env_h

#ifndef _UINT8_T
#define _UINT8_T
typedef unsigned char uint8_t;
#endif // _UINT8_T

namespace env_constants {
    int DEBUG_MODE = 0;
    int NCURSES_MODE = 0; // pc control enabled by default (so ncurses is enabled implicitly)
    int PC_AI_MODE = 0;
    int LOAD_DUNGEON = 0;
    int SAVE_DUNGEON = 0;
    int NUM_MONSTERS = 10;
    int QUIT_FLAG = 0;
    int STAIR_FLAG = 0;
    char* HOME;
    char* LOAD_FILE;
    char* SAVE_FILE;
    uint8_t X_START = 255;
    uint8_t Y_START = 255;
}

class env {
public:
    static void parse_args(int argc, char** argv);
    static void setup_environment();
    static void exit_gracefully();
    static void cleanup();
    static void move_floors();
};

#endif /* env_h */
