#pragma once

#include <exception>

class RTEException: public std::exception
{
public:
    explicit RTEException(const char* message)
     :_message(message) { }

    virtual ~RTEException() throw (){}


    virtual const char* What() const throw (){
       return _message;
    }

protected:
    const char *_message;
};