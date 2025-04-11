#pragma once

#include <memory>
#include <vector>
#include <string>
#include "Engine/Util/Logging.hpp"

class Texture;

/**
 * @brief Types of attachments that can be added to a framebuffer
 */
enum class FrameBufferAttachmentType
{
	Color,       ///< Color attachment
	Depth,       ///< Depth-only attachment
	Stencil,     ///< Stencil-only attachment
	DepthStencil, ///< Combined depth-stencil attachment
	DepthCubemap ///< Cubemap depth attachment (for point light shadows)
};

/**
 * @brief Texture formats supported for framebuffer attachments
 */
enum class FrameBufferTextureFormat
{
	None = 0,      ///< No format specified
	
	// Color formats
	R16F,          ///< 16-bit float, red channel only
	R32F,          ///< 32-bit float, red channel only
	RG16F,         ///< 16-bit float per channel, red and green
	RGBA8,         ///< 8-bit uint per channel, standard RGBA
	RGBA16F,       ///< 16-bit float per channel, RGBA (HDR)
	RGB10A2,       ///< 10 bits RGB + 2 bits alpha (good for HDR with alpha)
	
	// Depth formats
	Depth24,       ///< 24-bit depth
	Depth32F,      ///< 32-bit float depth
	
	// Stencil format
	Stencil8,      ///< 8-bit stencil
	
	// Combined formats
	Depth24Stencil8,   ///< 24-bit depth + 8-bit stencil
	Depth32FStencil8   ///< 32-bit float depth + 8-bit stencil
};

/**
 * @brief Specification for a framebuffer attachment
 */
struct FrameBufferAttachmentSpecification
{
	FrameBufferAttachmentType Type;  ///< Type of attachment
	FrameBufferTextureFormat Format;  ///< Format of the texture
	std::string Name;                ///< Debug name for the attachment (optional)

	FrameBufferAttachmentSpecification(
		FrameBufferAttachmentType type = FrameBufferAttachmentType::Color,
		FrameBufferTextureFormat format = FrameBufferTextureFormat::RGBA8,
		const std::string& name = "")
		: Type(type), Format(format), Name(name)
	{
	}
};


class FrameBuffer
{
public:

	FrameBuffer(int width,
				int height,
				const std::vector<FrameBufferAttachmentSpecification>& attachments,
				int samples = 1,
				const std::string& label = "");
				
	/**
	 * @brief Destructor - cleans up all GPU resources
	 */
	~FrameBuffer();
	
	// Prevent copying to avoid double-freeing GPU resources
	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer& operator=(const FrameBuffer&) = delete;
	
	// Allow moving for efficient container usage
	FrameBuffer(FrameBuffer&& other) noexcept;
	FrameBuffer& operator=(FrameBuffer&& other) noexcept;


	void bind();
	

	void unbind();
	

	void bindRead();
	

	void bindDraw();


	void resize(int width, int height);
	

	int getWidth() const { return mWidth; }
	

	int getHeight() const { return mHeight; }
	

	void setViewport(int x, int y, int width, int height);
	

	void getViewport(int& x, int& y, int& width, int& height) const;


	void clear(unsigned int mask);
	

	void readPixels(int x, int y, int width, int height, void* data, 
	                unsigned int format = 0x1908, // GL_RGBA
	                unsigned int type = 0x1401);  // GL_UNSIGNED_BYTE


	int addAttachment(const FrameBufferAttachmentSpecification& attachmentSpec);
	

	bool removeAttachment(FrameBufferAttachmentType type, int index = 0);
	

	const std::vector<FrameBufferAttachmentSpecification>& getAttachments() const { return mAttachmentSpecs; }
	

	void invalidate();


	void setSamples(int samples);
	

	int getSamples() const { return mSamples; }


	bool isComplete() const;


	std::shared_ptr<Texture> getColorAttachment(int index = 0) const;
	

	std::shared_ptr<Texture> getDepthAttachment() const { return mDepthAttachment; }


	void setDrawBuffers(const std::vector<unsigned int>& attachments);
	

	void setReadBuffer(unsigned int attachment);


	void blitTo(std::shared_ptr<FrameBuffer> dst,
				int srcX0, int srcY0, int srcX1, int srcY1,
				int dstX0, int dstY0, int dstX1, int dstY1,
				unsigned int mask,
				unsigned int filter);


	void resolveToFBO(std::shared_ptr<FrameBuffer> dst, unsigned int mask);


	unsigned int getRendererID() const { return mRendererID; }


	void attachExternalTexture(
		unsigned int attachment,
		unsigned int target,
		unsigned int textureID,
		int mipLevel = 0);
		
	void setLabel(const std::string& label);
	
	const std::string& getLabel() const { return mLabel; }

protected:
    // OpenGL wrapper methods to minimize direct OpenGL calls
    static void genFramebuffers(unsigned int count, unsigned int* ids);
    static void deleteFramebuffers(unsigned int count, const unsigned int* ids);
    static void bindFramebuffer(unsigned int target, unsigned int framebuffer);
    static void drawBuffer(unsigned int buffer);
    static void drawBuffers(unsigned int n, const unsigned int* bufs);
    static void readBuffer(unsigned int src);
    static void blitFramebuffer(int srcX0, int srcY0, int srcX1, int srcY1,
                              int dstX0, int dstY0, int dstX1, int dstY1,
                              unsigned int mask, unsigned int filter);
    static void clearBuffers(unsigned int mask);
    static void viewport(int x, int y, int width, int height);
    static void readPixelsBuffer(int x, int y, int width, int height, unsigned int format, 
                                unsigned int type, void* data);
    static unsigned int checkFramebufferStatus(unsigned int target);
    static void getIntegerv(unsigned int pname, int* params);

private:
	void createFramebuffer();
	
	void cleanup();

private:
	unsigned int mRendererID = 0; 
	int mWidth = 0, mHeight = 0;
	int mSamples = 1;
	std::string mLabel;

	int mViewportX = 0;
	int mViewportY = 0;
	int mViewportW = 0;
	int mViewportH = 0;

	std::vector<FrameBufferAttachmentSpecification> mAttachmentSpecs;
	std::vector<std::shared_ptr<Texture>> mColorAttachments;
	std::shared_ptr<Texture> mDepthAttachment;
};
