#include "stdafx.h"
#include "Renderer.h"

void Renderer::Update()
{
}

void Renderer::Render()
{
}

Renderer::Renderer(std::shared_ptr<DeviceResources> deviceResources)
	:
	m_frameCount(0),
	m_deviceResources(deviceResources)
{
	m_frameCount = 0; // init frame count
}
