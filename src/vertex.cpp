#include "vertex.hpp"

#include <glad/glad.h>

#include <numeric>

VertexArray::VertexArray()
	: mVA(0)
	, mEB(0)
	, mVertexCount(0)
	, mElementCount(0)
	, mIsValid(false)
{
	glGenVertexArrays(1, &mVA);
}

VertexArray::~VertexArray()
{
	mVBs.clear();
	glDeleteVertexArrays(1, &mVA);
}

void VertexArray::PushBuffer(std::unique_ptr<VertexBuffer> vb)
{
	if (vb->GetLayout().size() > 0)
	{
		mVBs.push_back(std::move(vb));
		mVertexCount = (uint32_t)mVBs[0]->GetVertexCount();
	}
}

void VertexArray::SetElements(const std::vector<uint32_t>& elements)
{
	mElementCount = (uint32_t)elements.size();
	glBindVertexArray(mVA);
	glGenBuffers(1, &mEB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(uint32_t), &elements[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void VertexArray::Upload()
{
	glBindVertexArray(mVA);
	uint32_t attributeIndex = 0;
	for (auto& vb : mVBs) {
		if (!vb->IsUploaded()) {
			vb->Upload(false);
		}
		vb->Bind();
		uint32_t offset = 0;
		for (uint32_t attribSize : vb->GetLayout())
		{
			glEnableVertexAttribArray(attributeIndex);
			glVertexAttribPointer(attributeIndex, attribSize, GL_FLOAT, GL_FALSE, vb->GetStride(), (void*)(intptr_t)offset);

			attributeIndex++;
			offset += (attribSize * sizeof(float));
		}
		vb->Unbind();
	}
	glBindVertexArray(0);
	mIsValid = true;
}

void VertexArray::Bind()
{
	glBindVertexArray(mVA);
}

void VertexArray::Unbind()
{
	glBindVertexArray(0);
}

VertexBuffer::VertexBuffer()
	: mVB(0)
	, mVertexCount(0)
	, mPerVertexValueCount(0)
	, mStride(0)
	, mIsUploaded(false)
{
	glGenBuffers(1, &mVB);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &mVB);
}

void VertexBuffer::PushVertex(const std::vector<float>& vert)
{
	if (mBufferData.size() == 0)
	{
		mPerVertexValueCount = (uint32_t)vert.size();
	}

	if (vert.size() == mPerVertexValueCount)
	{
		mVertexCount++;
		mBufferData.insert(mBufferData.end(), vert.begin(), vert.end());
	}
}

void VertexBuffer::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, mVB);
}

void VertexBuffer::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::SetLayout(const std::vector<uint32_t>& layout)
{
	mLayout = layout;
	mStride = std::accumulate(layout.begin(), layout.end(), (uint32_t)0);
}

void VertexBuffer::Upload(bool dynamic)
{
	mStride *= sizeof(float);
	uint32_t size = sizeof(float) * mBufferData.size();

	glBindBuffer(GL_ARRAY_BUFFER, mVB);
	glBufferData(GL_ARRAY_BUFFER, size, mBufferData.data(), dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	mIsUploaded = true;
}
