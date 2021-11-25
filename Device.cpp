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
	ptr += width * height * 8;

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
{
	int y, x;
	for (y = 0; y < m_height; y++)
	{
		UINT32* dst = m_framebuffer[y];
		UINT32 cc = gdiColorTransform(m_background);
		for (x = m_width; x > 0; dst++, x--) dst[0] = cc;
	}
	for (y = 0; y < m_height; y++)
	{
		float* dst = m_zbuffer[y];
		for (x = m_width; x > 0; dst++, x--) dst[0] = 0.f;
	}
}

float Device::getZbuffer(const int& x, const int& y)
{
	if (x >= 0 && x < m_width && y >= 0 && y < m_height)
		return m_zbuffer[y][x];
	else
		return std::numeric_limits<float>::max();
}

void Device::setZbuffer(const int& x, const int& y, const float& zValue)
{
	if (x >= 0 && x < m_width && y >= 0 && y < m_height)
		m_zbuffer[y][x] = zValue;
}

void Device::drawPixel(const int& x, const int& y, const Color& color)
{
	if (x >= 0 && x < m_width && y >= 0 && y < m_height)
		m_framebuffer[y][x] = gdiColorTransform(color);
}

UINT32 Device::gdiColorTransform(Color color)
{
	color.r = (color.r > 255.f) ? 255.f : color.r;
	color.g = (color.g > 255.f) ? 255.f : color.g;
	color.b = (color.b > 255.f) ? 255.f : color.b;

	UINT32 r = static_cast<UINT32>(color.r);
	UINT32 g = static_cast<UINT32>(color.g);
	UINT32 b = static_cast<UINT32>(color.b);
	return (r << 16) | (g << 8) | b;
}
