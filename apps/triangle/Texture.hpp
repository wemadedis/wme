#pragma once
#include "Image.hpp"

struct Texture
{
	/*
	Vulkan sampler specifying how to sample the texture.
	*/
	VkSampler sampler;
	/*
	Information about the texture image.
	*/
	Image image;
};

