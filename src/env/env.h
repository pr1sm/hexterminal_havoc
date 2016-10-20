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

extern int DEBUG_MODE;
extern int NCURSES_MODE;
extern int PC_AI_MODE;
extern int LOAD_DUNGEON;
extern int SAVE_DUNGEON;
extern int NUM_MONSTERS;
extern int QUIT_FLAG;
extern int STAIR_FLAG;
extern char* HOME;
extern char* LOAD_FILE;
extern char* SAVE_FILE;
extern uint8_t X_START;
extern uint8_t Y_START;

typedef struct env_namespace {
    void (*const parse_args)(int argc, char** argv);
    void (*const setup_environment)();
    void (*const exit_gracefully)();
    void (*const cleanup)();
    void (*const move_floors)();
} env_namespace;
extern const env_namespace envAPI;

#endif /* env_h */
