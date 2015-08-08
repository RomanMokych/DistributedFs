//
//  Utils.h
//  DistributedFs
//
//  Created by Роман on 8/8/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#pragma once

#include <stdexcept>
#include <sstream>

#define THROW(str_) \
{ \
    std::stringstream ostr; \
    ostr << __func__ << "(): #" << __LINE__ << ": " << str_; \
    throw std::runtime_error(ostr.str()); \
}
