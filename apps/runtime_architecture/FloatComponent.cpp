#include "FloatComponent.h"

void FloatComponent::Initialize(float value) 
{
    this->value = value;
}

void FloatComponent::Update()
{
    std::cout << (value++) << std::endl;
}

FloatComponent::~FloatComponent()
{
    
}