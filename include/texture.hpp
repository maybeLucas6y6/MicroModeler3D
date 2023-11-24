#pragma once

#include <string>

enum class TextureFilter
{
	Nearest,
	Linear
};

class Texture
{
public:
	Texture(const std::string& path);
	~Texture();

	uint32_t GetId() const { return mId; }
	uint32_t GetWidth() const { return mWidth; }
	uint32_t GetHeight() const { return mHeight; }
	uint32_t GetNumChannels() const { return mNumChannels; }
	const std::string& GetPath() const { return mPath; }
	TextureFilter GetTextureFilter() const { return mFilter; }

	void Bind();
	void Unbind();

	void SetTextureFilter(TextureFilter filter);

private:
	void LoadTexture();

private:
	TextureFilter mFilter;

	std::string mPath;
	uint32_t mId;
	uint32_t mWidth, mHeight;
	uint32_t mNumChannels;

	unsigned char* mPixels;
};