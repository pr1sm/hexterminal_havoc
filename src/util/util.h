//
//  util.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/16/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef util_h
#define util_h

#include "../point/point.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int  point_to_index(point_t* p);
void index_to_point(int index, point_t* p);
    
#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* util_h */
