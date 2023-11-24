#include "framebuffer.hpp"
#include "log.hpp"

#include <glad/glad.h>

Framebuffer::Framebuffer(uint32_t width, uint32_t height)
	: mFB(0)
	, mTextureId(0)
	, mRenderbufferId(0)
	, mSize({ width, height })
	, mClearColour(1.f)
{
	glGenFramebuffers(1, &mFB);
	glBindFramebuffer(GL_FRAMEBUFFER, mFB);

	// Create color texture
	glGenTextures(1, &mTextureId);
	glBindTexture(GL_TEXTURE_2D, mTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mSize.x, mSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureId, 0);

	// Create depth/stencil renderbuffer
	glGenRenderbuffers(1, &mRenderbufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, mRenderbufferId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mSize.x, mSize.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRenderbufferId);

	// Check for completeness
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG("Framebuffer incomplete");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &mFB);
	mFB = 0;
	mTextureId = 0;
	mRenderbufferId = 0;
}