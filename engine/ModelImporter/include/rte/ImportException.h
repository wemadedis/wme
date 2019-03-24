#pragma once
#include "RTEException.h"

class ImportException : public RTEException
{
  public:
    ImportException() : RTEException("Failed to import")
    {

    }   
};