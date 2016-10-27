//
//  character_common.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/26/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef character_common_h
#define character_common_h

#include "../point/point.h"

#define INTEL_BIT 0
#define TELEP_BIT 1
#define TUNNL_BIT 2
#define ERATC_BIT 3
#define INTEL_VAL (1 << INTEL_BIT)
#define TELEP_VAL (1 << TELEP_BIT)
#define TUNNL_VAL (1 << TUNNL_BIT)
#define ERATC_VAL (1 << ERATC_BIT)

typedef enum character_type {
    NONE, PC, NPC
} character_type;

typedef int character_id_t;


#endif /* character_common_h */
