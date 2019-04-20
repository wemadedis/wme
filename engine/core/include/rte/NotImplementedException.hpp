#pragma once

#include "rte/RTEException.h"

class NotImplementedException : public RTEException
{
public:
    explicit NotImplementedException()
        : RTEException("This function has been implemented yet") {}
};