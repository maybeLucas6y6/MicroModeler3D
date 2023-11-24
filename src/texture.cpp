#include "texture.hpp"
#include "log.hpp"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"

Texture::Texture(const std::string& path)
	: mPath(path)
	, mWidth(0)
	, mHeight(0)
	, mNumChannels(0)
	, mPixels(nullptr)
	, mFilter(TextureFilter::Linear)
{
	int width, height, numChannels;
	stbi_set_flip_vertically_on_load(true);
	mPixels = stbi_load(path.c_str(), &width, &height, &numChannels, 0);
	if (mPixels)
	{
		mWidth = (uint32_t)width;
		mHeight = (uint32_t)height;
		mNumChannels = (uint32_t)numChannels;
	}

	LoadTexture();
}

Texture::~Texture()
{
	stbi_image_free(mPixels);
	mPixels = nullptr;
}

void Texture::Bind()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mId);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::LoadTexture()
{
	glGenTextures(1, &mId);
	glBindTexture(GL_TEXTURE_2D, mId);

	GLenum dataFormat = 0;
	if (mNumChannels == 4)
	{
		dataFormat = GL_RGBA;
	}
	else if (mNumChannels == 3)
	{
		dataFormat = GL_RGB;
	}
	else
	{
		LOG("Data type not supported. Number of channels: %u", mNumChannels);
	}

	if (mPixels && dataFormat == 0)
	{
		LOG("Texture data type not supported. Number of channels: %u", mNumChannels);
	}

	if (mPixels && dataFormat != 0)
	{
		LOG("Loading texture with %u channels and %s format", (unsigned int)mNumChannels, dataFormat == GL_RGBA ? "RGBA" : "RGB");
		glTexImage2D(GL_TEXTURE_2D, 0, dataFormat, mWidth, mHeight, 0, dataFormat, GL_UNSIGNED_BYTE, mPixels);
		SetTextureFilter(mFilter);
		LOG("Loaded texture");
	}
	else
	{
		LOG("Could not load texture. Loading a placeholder...");
		float pixels[] = {
			1.f, 0.f, 1.f,		1.f, 1.f, 1.f,		1.f, 0.f, 1.f,		1.f, 1.f, 1.f,
			1.f, 1.f, 1.f,		1.f, 0.f, 1.f,		1.f, 1.f, 1.f,		1.f, 0.f, 1.f,
			1.f, 0.f, 1.f,		1.f, 1.f, 1.f,		1.f, 0.f, 1.f,		1.f, 1.f, 1.f,
			1.f, 1.f, 1.f,		1.f, 0.f, 1.f,		1.f, 1.f, 1.f,		1.f, 0.f, 1.f
		};

		mWidth = 4;
		mHeight = 4;
		mNumChannels = 3;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGB, GL_FLOAT, pixels);
		SetTextureFilter(TextureFilter::Nearest);
		LOG("Loaded placeholder");
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetTextureFilter(TextureFilter filter)
{
	mFilter = filter;

	glBindTexture(GL_TEXTURE_2D, mId);
	switch (mFilter)
	{
	case TextureFilter::Linear:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;

	case TextureFilter::Nearest:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}