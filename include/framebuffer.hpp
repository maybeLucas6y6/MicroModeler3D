#pragma once

#include <glm/glm.hpp>

#include <cstdint>

class Framebuffer
{
public:
	Framebuffer(uint32_t width, uint32_t height);
	~Framebuffer();

	uint32_t GetFB() const { return mFB; }
	uint32_t GetTextureId() const { return mTextureId; }
	uint32_t GetRenderbufferId() const { return mRenderbufferId; }
	const glm::ivec2& GetSize() { return mSize; }
	void SetClearColour(const glm::vec4& cc) { mClearColour = cc; }
	const glm::vec4& GetClearColour() { return mClearColour; }

private:
	uint32_t mFB;
	uint32_t mTextureId;
	uint32_t mRenderbufferId;

	glm::ivec2 mSize;
	glm::vec4 mClearColour;
};