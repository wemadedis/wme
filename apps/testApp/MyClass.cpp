#include "MyClass.h"
#include <iostream>
#include <string>

MyClass::MyClass()
{
    std::cout << "Constructor" << std::endl;
}

MyClass::~MyClass()
{
    std::cout << "Destructor" << std::endl;
}

void MyClass::Say(std::string msg)
{
    std::cout << msg << std::endl;
}