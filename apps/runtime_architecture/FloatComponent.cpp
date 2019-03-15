#include "FloatComponent.h"

void FloatComponent::Initialize(float startValue) 
{
    value = startValue;
}

void FloatComponent::Update()
{
    std::cout << (value++) << std::endl;
}