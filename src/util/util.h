//
//  util.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/31/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef util_h
#define util_h

#include "../point/point.h"

namespace util {
    int  point_to_index(point* p);
    void index_to_point(int index, point* p);
}

#endif /* util_h */
