#include "Device.h"
#include <iostream>

Device::Device(void* fb, int width, int height) : m_width(width), m_height(height)
{
	int need = sizeof(void*) * (height * 2 + 1024) + width * height * 8;
	char* ptr = new char[need + 64];
	char* framebuffer, * zbuffer;

	assert(ptr);

	m_framebuffer = (UINT32**)ptr;
	m_zbuffer = (float**)(ptr + sizeof(void*) * height);

	ptr += sizeof(void*) * height * 2;
	ptr += sizeof(void*) * 1024;
	framebuffer = (char*)ptr;
	zbuffer = (char*)ptr + width * height * 4;
	//ptr += width * height * 8;

	if (fb != nullptr) framebuffer = (char*)fb;

	for (int i = 0; i < height; i++)
	{
		m_framebuffer[i] = (UINT32*)(framebuffer + width * 4 * i);
		m_zbuffer[i] = (float*)(zbuffer + width * 4 * i);
	}
	m_foreground = Color(1.f, 1.f, 1.f, 1.f);
	m_background = Color(0.f, 0.f, 0.f, 0.f);
}

Device::~Device()
{
	if (m_framebuffer)
		delete[] m_framebuffer;

	m_framebuffer = nullptr;
	m_zbuffer = nullptr;
}

void Device::clear()
{}

float Device::getZbuffer(const int& x, const int& y)
{
	return 0.0f;
}

float Device::setZbuffer(const int& x, const int& y, const float& zValue)
{
	return 0.0f;
}

void Device::drawPixel(const int& x, const int& y, const Color color)
{}

UINT32 Device::gdiColorTransform(Color color)
{
	return UINT32();
}
