#pragma once
#include"Math.h"

class Device
{
public:
	Device(void* fb, int width, int height);
	~Device();
	void clear();
	float getZbuffer(const int& x, const int& y);
	void setZbuffer(const int& x, const int& y, const float& zValue);
	void drawPixel(const int& x, const int& y, const glm::vec4& color);

private:
	int m_width;
	int m_height;
	UINT32** m_framebuffer;
	float** m_zbuffer;
	glm::vec4 m_foreground;
	glm::vec4 m_background;
private:
	UINT32 gdiColorTransform(glm::vec4 color);
};

