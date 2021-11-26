#include "Texture.h"
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

Texture::Texture(const UINT& width, const UINT& height) : m_width(width), m_height(height)
{
	m_buffer = new Color * [height];
	for (UINT i = 0; i < height; i++)
	{
		m_buffer[i] = new Color[width];
	}
}

Texture::~Texture()
{
	for (UINT i = 0; i < m_height; i++)
	{
		delete m_buffer[i];
	}
}

Texture::Texture(const Texture& rhs) :m_width(rhs.m_width), m_height(rhs.m_height)
{
	m_buffer = new Color * [m_height];
	for (int i = 0; i < m_height; ++i)
	{
		m_buffer[i] = new Color[m_width];
	}
	for (int i = 0; i < m_width; ++i)
	{
		for (int j = 0; j < m_height; ++j)
		{
			m_buffer[j][i] = rhs.m_buffer[j][i];
		}
	}
}

Texture& Texture::operator=(const Texture& rhs)
{
	if (this == &rhs)
		return *this;
	m_width = rhs.m_width;
	m_height = rhs.m_height;
	m_buffer = new Color * [m_height];
	for (int i = 0; i < m_height; ++i)
	{
		m_buffer[i] = new Color[m_width];
	}
	for (int i = 0; i < m_width; ++i)
	{
		for (int j = 0; j < m_height; ++j)
		{
			m_buffer[j][i] = rhs.m_buffer[j][i];
		}
	}
	return *this;
}

void Texture::setBuffer(const UINT& x, const UINT& y, const Color& color)
{
	m_buffer[y][x] = color;
}

Color Texture::sampleTex(const float& u, const float& v)
{
	UINT x = static_cast<UINT>(u * (m_width - 1) + 0.5f);
	UINT y = static_cast<UINT>(v * (m_height - 1) + 0.5f);

	return m_buffer[y][x];
}

Texture Texture::loadTexture(std::wstring filePath)
{
	Gdiplus::GdiplusStartupInput gdiplusstartupinput;
	ULONG_PTR gdiplustoken;
	GdiplusStartup(&gdiplustoken, &gdiplusstartupinput, nullptr);

	Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(filePath.c_str());

	if (!bmp)
	{
		MessageBox(nullptr, "main.cpp: texture filepath is not a full path or right path!", "Error", MB_OK);
		delete bmp;
		Gdiplus::GdiplusShutdown(gdiplustoken);
		return Texture(0, 0);
	}
	else
	{
		UINT width = bmp->GetWidth();
		UINT height = bmp->GetHeight();

		if (width == 0 || height == 0)
		{
			MessageBox(nullptr, "main.cpp: texture filepath is not a full path or right path!", "Error", MB_OK);
			delete bmp;
			Gdiplus::GdiplusShutdown(gdiplustoken);
			return Texture(0, 0);
		}

		Texture tex(width, height);

		Gdiplus::Color pix;
		for (UINT i = 0; i < width; i++)
		{
			for (UINT j = 0; j < height; j++)
			{
				bmp->GetPixel(i, j, &pix);
				Color color(
					pix.GetRed() / 255.f,
					pix.GetGreen() / 255.f,
					pix.GetBlue() / 255.f,
					1.f
				);
				tex.setBuffer(i, j, color);
			}
		}

		delete bmp;
		Gdiplus::GdiplusShutdown(gdiplustoken);
		return tex;
	}
}
