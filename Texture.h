#pragma once
#include "math.h"
#include <string>
#include <windows.h>

#include <iostream>

class Texture
{
public:
	UINT m_width, m_height;
	glm::vec4** m_buffer;

public:
	Texture() = default;
	Texture(const UINT& width, const UINT& height);
	~Texture();
	Texture(const Texture& rhs);
	Texture& operator=(const Texture& rhs);

	void setBuffer(const UINT& x, const UINT& y, const glm::vec4& color);
	glm::vec4 sampleTex(const float& u, const float& v);

private:
	Texture loadTexture(std::wstring filePath);
};

