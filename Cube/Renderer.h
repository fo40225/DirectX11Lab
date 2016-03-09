#pragma once
#include "DeviceResources.h"

class Renderer
{
public:
	void Update();
	void Render();
	Renderer(std::shared_ptr<DeviceResources> deviceResources);
private:
	std::shared_ptr<DeviceResources> m_deviceResources;
	unsigned int  m_indexCount;
	unsigned int  m_frameCount;
};