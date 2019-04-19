#pragma once
#include "rte/RTEException.h"

class ImportException : public RTEException
{
  public:
    ImportException() : RTEException("Failed to import")
    {

    }   
};