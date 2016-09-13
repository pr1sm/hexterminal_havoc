//
//  env.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/7/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef env_h
#define env_h

extern int DEBUG_MODE;
extern int LOAD_DUNGEON;
extern int SAVE_DUNGEON;

void parse_args(int argc, char** argv);
void setup_environment();
void exit_gracefully();

#endif /* env_h */
