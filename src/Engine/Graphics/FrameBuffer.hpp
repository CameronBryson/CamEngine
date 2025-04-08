#pragma once

#include <memory>
#include <vector>

class Texture;

enum class FrameBufferAttachmentType
{
	Color,
	Depth,
	Stencil,
	DepthStencil,
	DepthCubemap
};

enum class FrameBufferTextureFormat
{
	None = 0,
	R16F,
	R32F,
	RG16F,
	RGBA8,
	RGBA16F,
	RGB10A2,

	Depth24,
	Depth32F,

	Stencil8,

	Depth24Stencil8,
	Depth32FStencil8
};

struct FrameBufferAttachmentSpecification
{
	FrameBufferAttachmentType Type;
	FrameBufferTextureFormat Format;

	FrameBufferAttachmentSpecification(
		FrameBufferAttachmentType type = FrameBufferAttachmentType::Color,
		FrameBufferTextureFormat format = FrameBufferTextureFormat::RGBA8)
		: Type(type), Format(format)
	{
	}
};

class FrameBuffer
{
public:

	FrameBuffer(int width,
					  int height,
					  const std::vector<FrameBufferAttachmentSpecification>& attachments,
					  int samples = 1);
	~FrameBuffer();

	void bind();
	void unbind();
	void bindRead();
	void bindDraw();

	void resize(int width, int height);
	int getWidth() const;
	int getHeight() const;
	void setViewport(int x, int y, int width, int height);
	void getViewport(int& x, int& y, int& width, int& height) const;

	void clear(unsigned int mask);
	void readPixels(int x, int y, int width, int height, void* data);

	void addAttachment(const FrameBufferAttachmentSpecification& attachmentSpec);
	void removeAttachment(FrameBufferAttachmentType type, int index = 0);
	const std::vector<FrameBufferAttachmentSpecification>& getAttachments() const;
	void invalidate();

	void setSamples(int samples);
	int getSamples() const;

	bool isComplete() const;

	std::shared_ptr<Texture> getColorAttachment(int index = 0) const;
	std::shared_ptr<Texture> getDepthAttachment() const;

	void setDrawBuffers(const std::vector<unsigned int>& attachments);
	void setReadBuffer(unsigned int attachment);

	void blitTo(std::shared_ptr<FrameBuffer> dst,
				int srcX0, int srcY0, int srcX1, int srcY1,
				int dstX0, int dstY0, int dstX1, int dstY1,
				unsigned int mask,
				unsigned int filter);

	unsigned int getRendererID() const { return mRendererID; }

	void attachExternalTexture(unsigned int attachment,
							   unsigned int target,
							   unsigned int textureID,
							   int mipLevel = 0);

private:
	void createFramebuffer();

private:
	unsigned int mRendererID = 0; 
	int mWidth, mHeight;
	int mSamples;

	int mViewportX = 0;
	int mViewportY = 0;
	int mViewportW = 0;
	int mViewportH = 0;

	std::vector<FrameBufferAttachmentSpecification> mAttachmentSpecs;

	std::vector<std::shared_ptr<Texture>> mColorAttachments;
	std::shared_ptr<Texture> mDepthAttachment;
};
