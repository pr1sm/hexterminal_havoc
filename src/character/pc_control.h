//
//  pc_control.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/19/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef pc_control_h
#define pc_control_h

#define PC_QUIT          81
#define PC_UP_LEFT_7     55
#define PC_UP_8          56
#define PC_UP_RIGHT_9    57
#define PC_LEFT_4        52
#define PC_RIGHT_6       54
#define PC_DOWN_LEFT_1   49
#define PC_DOWN_2        50
#define PC_DOWN_RIGHT_3  51
#define PC_REST_5        53
#define PC_UP_LEFT_y    121
#define PC_UP_k         107
#define PC_UP_RIGHT_u   117
#define PC_LEFT_h       104
#define PC_RIGHT_l      108
#define PC_DOWN_LEFT_b   98
#define PC_DOWN_j       106
#define PC_DOWN_RIGHT_n 110
#define PC_REST_SPACE    32
#define PC_DOWNSTAIRS    62
#define PC_UPSTAIRS      60
#define PC_MONSTER_LIST 109
#define PC_ML_SCRL_UP   259
#define PC_ML_SCRL_DOWN 258
#define PC_ML_CLOSE      27

void setup_control_movement();
void handle_control_move();

#endif /* pc_control_h */