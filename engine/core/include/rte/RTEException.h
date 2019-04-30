#pragma once

#include "rte/Utility.hpp"
#include <exception>

class RTEException : public std::exception
{
public:
    explicit RTEException(const char *message)
        : std::exception(message), _message(message)
    {
        Exception(message);
    }

    virtual ~RTEException() throw() {}

    virtual const char *what() const throw()
    {
        return _message;
    }

protected:
    const char *_message;
};