#pragma once

#include <cstdint>
#include <memory>
#include <vector>

class VertexBuffer
{
public:
	VertexBuffer();
	~VertexBuffer();

	bool IsUploaded() const { return mIsUploaded; }
	uint32_t GetId() const { return mVB; }
	uint32_t GetVertexCount() const { return mVertexCount; }
	uint32_t GetStride() const { return mStride; }
	const std::vector<uint32_t>& GetLayout() const { return mLayout; }

	void SetLayout(const std::vector<uint32_t>& layout);
	void Upload(bool dynamic = false);

	void PushVertex(const std::vector<float>& vert);

	void Bind();
	void Unbind();
private:
	uint32_t mVB;

	uint32_t mVertexCount;
	uint32_t mPerVertexValueCount; // no of data per vertex
	std::vector<float> mBufferData; // al vertices data

	std::vector<uint32_t> mLayout; // no of data per attribute, sums up to mPerVertexValueCount
	uint32_t mStride; // no of bytes a vertex covers (with all his attributes)
	bool mIsUploaded;
};

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	bool IsValid() const { return mIsValid; }
	uint32_t GetVertexCount() const { return mVertexCount; }
	uint32_t GetElementCount() const { return mElementCount; }

	void PushBuffer(std::unique_ptr<VertexBuffer> vb);
	void SetElements(const std::vector<uint32_t>& elements);

	void Upload();

	void Bind();
	void Unbind();
private:
	bool mIsValid;
	uint32_t mVertexCount, mElementCount;
	uint32_t mVA, mEB;
	std::vector<std::unique_ptr<VertexBuffer>> mVBs;
};