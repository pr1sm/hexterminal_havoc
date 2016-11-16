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

class env_constants {
public:
    static int USE_IPARSE;
    static int USE_MPARSE;
    static int PARSE_ONLY_MODE;
    static int DEBUG_MODE;
    static int NCURSES_MODE; // pc control enabled by default (so ncurses is enabled implicitly)
    static int PC_AI_MODE;
    static int LOAD_DUNGEON;
    static int SAVE_DUNGEON;
    static int NUM_MONSTERS;
    static int QUIT_FLAG;
    static int STAIR_FLAG;
    static int USE_MON_DESC;
    static int USE_OBJ_DESC;
    static char* HOME;
    static char* LOAD_FILE;
    static char* SAVE_FILE;
    static uint8_t X_START;
    static uint8_t Y_START;
};

class env {
public:
    static void parse_args(int argc, char** argv);
    static void setup_environment();
    static void exit_gracefully();
    static void cleanup();
    static void move_floors();
};

#endif /* env_h */
