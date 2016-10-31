//
//  main.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/31/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdio.h>

int main(int argc, char * argv[]) {
 
#ifdef DEBUG // Xcode terminal debugging with ncurses
    int argi;
    for (argi = 1; argi < argc; argi++)
    {
        if (strcmp(argv[argi], "--debug-in-terminal") == 0)
        {
            printf("Debugging in terminal enabled\n");
            getchar(); // Without this call debugging will be skipped
            break;
        }
    }
#endif // DEBUG
    
    envAPI.parse_args(argc, argv);
    envAPI.setup_environment();
    
    if(DEBUG_MODE) {
        logger.set_modes_enabled(LOG_T | LOG_D | LOG_I | LOG_W | LOG_E | LOG_F);
    } else {
        logger.set_modes_enabled(LOG_I | LOG_W | LOG_E | LOG_F);
    }
    
    dungeon_t* d = dungeonAPI.get_dungeon();
    
    if(LOAD_DUNGEON) {
        d->load(d);
    } else {
        dungeonAPI.generate(d);
    }
    
    d->update_path_maps(d);
    
    characterStoreAPI.setup();
    
    if(DEBUG_MODE) {
        d->print(d, PM_ROOM_PATH_MAP);
        d->print(d, PM_TUNN_PATH_MAP);
    }
    
    d->print(d, PM_DUNGEON);
    
    int next_turn = 1;
    int win_status = characterStoreAPI.is_finished();
    while(!win_status && next_turn) {
        next_turn = eventQueueAPI.perform_event();
        if(STAIR_FLAG == 1 || STAIR_FLAG == 2) {
            envAPI.move_floors();
            d = dungeonAPI.get_dungeon();
        }
        win_status = characterStoreAPI.is_finished();
        d->print(d, PM_DUNGEON);
        usleep(100000);
    }
    
    if(SAVE_DUNGEON) {
        d->save(d);
    }
    
    envAPI.cleanup();
    
    if(win_status == 1) {
        printf("YOU LOSE!\n");
    } else if(win_status == 2) {
        printf("BY SHEER LUCK, YOU WON!\n");
    } else {
        printf("YOU QUIT\n");
    }
    
    return 0;
}
