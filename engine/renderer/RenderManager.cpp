#include "RenderManager.h"
#include "RenderStructs.h"

namespace RTE_Renderer
{
	RenderManager::RenderManager()
	{
		Light l;
		l._LightType = LightType::DIRECTIONAL;
		l._Color = {0.0f, 0.0f, 0.0f, 0.0f};
		l._Properties._DirectionalLight._Direction = {25,25,25};

		if (l._LightType == LightType::DIRECTIONAL)
		{
			auto memes =  l._Properties._PointLight._Intensity;
		}

		Vertex v;
	}


	RenderManager::~RenderManager()
	{
	}
}
