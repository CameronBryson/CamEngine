#include "pch.hpp"
#include "FrameBuffer.hpp"
#include <iostream>
#include <glad/glad.h>
#include "Texture.hpp"
#include <OpenGLUtil.hpp>
#include "Engine/Util/Logging.hpp"

namespace
{
	// Helper function to convert FrameBufferTextureFormat to OpenGL internal format
	GLenum toGLInternalFormat(const FrameBufferTextureFormat format)
	{
		switch (format)
		{
		case FrameBufferTextureFormat::R16F:            return GL_R16F;
		case FrameBufferTextureFormat::RG16F:           return GL_RG16F;
		case FrameBufferTextureFormat::R32F:            return GL_R32F;
		case FrameBufferTextureFormat::RGBA8:           return GL_RGBA8;
		case FrameBufferTextureFormat::RGBA16F:         return GL_RGBA16F;
		case FrameBufferTextureFormat::RGB10A2:         return GL_RGB10_A2;

		case FrameBufferTextureFormat::Depth24:         return GL_DEPTH_COMPONENT24;
		case FrameBufferTextureFormat::Depth32F:        return GL_DEPTH_COMPONENT32F;
		case FrameBufferTextureFormat::Stencil8:        return GL_STENCIL_INDEX8;
		case FrameBufferTextureFormat::Depth24Stencil8: return GL_DEPTH24_STENCIL8;
		case FrameBufferTextureFormat::Depth32FStencil8:return GL_DEPTH32F_STENCIL8;

		case FrameBufferTextureFormat::None:
		default:
			LOG_ERROR(logging::gGraphicsLogger, "Invalid framebuffer texture format");
			return 0;
		}
	}

	// Helper function to convert FrameBufferTextureFormat to OpenGL format
	GLenum toGLFormat(const FrameBufferTextureFormat format)
	{
		switch (format)
		{
		case FrameBufferTextureFormat::R16F:
		case FrameBufferTextureFormat::R32F:
			return GL_RED;

		case FrameBufferTextureFormat::RG16F:
			return GL_RG;

		case FrameBufferTextureFormat::RGBA8:
		case FrameBufferTextureFormat::RGBA16F:
		case FrameBufferTextureFormat::RGB10A2:
			return GL_RGBA;

		case FrameBufferTextureFormat::Depth24:
		case FrameBufferTextureFormat::Depth32F:
			return GL_DEPTH_COMPONENT;

		case FrameBufferTextureFormat::Stencil8:
			return GL_STENCIL_INDEX;

		case FrameBufferTextureFormat::Depth24Stencil8:
		case FrameBufferTextureFormat::Depth32FStencil8:
			return GL_DEPTH_STENCIL;

		case FrameBufferTextureFormat::None:
		default:
			LOG_ERROR(logging::gGraphicsLogger, "Invalid framebuffer texture format");
			return 0;
		}
	}

	// Helper function to convert FrameBufferTextureFormat to OpenGL data type
	GLenum toGLType(const FrameBufferTextureFormat format)
	{
		switch (format)
		{
		case FrameBufferTextureFormat::R16F:
		case FrameBufferTextureFormat::R32F:
		case FrameBufferTextureFormat::RG16F:
		case FrameBufferTextureFormat::RGBA16F:
		case FrameBufferTextureFormat::Depth32F:
		case FrameBufferTextureFormat::Depth32FStencil8:
			return GL_FLOAT;

		case FrameBufferTextureFormat::RGB10A2:
			return GL_UNSIGNED_INT_2_10_10_10_REV;

		case FrameBufferTextureFormat::Depth24:
		case FrameBufferTextureFormat::Depth24Stencil8:
			return GL_UNSIGNED_INT;

		case FrameBufferTextureFormat::Stencil8:
		case FrameBufferTextureFormat::RGBA8:
			return GL_UNSIGNED_BYTE;

		case FrameBufferTextureFormat::None:
		default:
			LOG_ERROR(logging::gGraphicsLogger, "Invalid framebuffer texture format");
			return 0;
		}
	}

	// Convert attachment type to OpenGL attachment point
	GLenum getAttachmentPoint(const FrameBufferAttachmentType type, const int index = 0)
	{
		switch (type)
		{
		case FrameBufferAttachmentType::Color:
			return GL_COLOR_ATTACHMENT0 + index;
		case FrameBufferAttachmentType::Depth:
		case FrameBufferAttachmentType::DepthCubemap:
			return GL_DEPTH_ATTACHMENT;
		case FrameBufferAttachmentType::Stencil:
			return GL_STENCIL_ATTACHMENT;
		case FrameBufferAttachmentType::DepthStencil:
			return GL_DEPTH_STENCIL_ATTACHMENT;
		default:
			LOG_ERROR(logging::gGraphicsLogger, "Invalid framebuffer attachment type");
			return 0;
		}
	}

}

FrameBuffer::FrameBuffer(
	int width,
	int height,
	std::vector<FrameBufferAttachmentSpecification> attachments,
	int samples,
	std::string_view label)
	: mWidth(width)
	, mHeight(height)
	, mSamples(samples)
	, mAttachmentSpecs(std::move(attachments))
{
	LOG_DEBUG(logging::gGraphicsLogger, "Creating framebuffer: unnamed ({0}x{1}, {2} samples)",
			  width, height, samples);

	ASSERT_LOG(logging::gGraphicsLogger, width > 0 && height > 0, "Invalid framebuffer dimensions: {}x{}", width, height);
	ASSERT_LOG(logging::gGraphicsLogger, samples >= 1, "Invalid sample count: {}", samples);

	mViewportX = 0;
	mViewportY = 0;
	mViewportW = width;
	mViewportH = height;

	createFramebuffer();
}

FrameBuffer::~FrameBuffer()
{
	cleanup();
}

FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept
	: mRendererID(other.mRendererID)
	, mWidth(other.mWidth)
	, mHeight(other.mHeight)
	, mSamples(other.mSamples)
	, mViewportX(other.mViewportX)
	, mViewportY(other.mViewportY)
	, mViewportW(other.mViewportW)
	, mViewportH(other.mViewportH)
	, mAttachmentSpecs(std::move(other.mAttachmentSpecs))
	, mColorAttachments(std::move(other.mColorAttachments))
	, mDepthAttachment(std::move(other.mDepthAttachment))
{
	// Prevent double deletion by marking other's ID as 0
	other.mRendererID = 0;
}

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other) noexcept
{
	if (this != &other)
	{
		// Clean up existing resources
		cleanup();

		// Move resources from other
		mRendererID = other.mRendererID;
		mWidth = other.mWidth;
		mHeight = other.mHeight;
		mSamples = other.mSamples;
		mViewportX = other.mViewportX;
		mViewportY = other.mViewportY;
		mViewportW = other.mViewportW;
		mViewportH = other.mViewportH;
		mAttachmentSpecs = std::move(other.mAttachmentSpecs);
		mColorAttachments = std::move(other.mColorAttachments);
		mDepthAttachment = std::move(other.mDepthAttachment);

		// Prevent double deletion
		other.mRendererID = 0;
	}
	return *this;
}

// Implementation of OpenGL wrapper methods
void FrameBuffer::genFramebuffers(const unsigned int count, unsigned int* ids)
{
	GL_CHECK(glGenFramebuffers(count, ids));
}

void FrameBuffer::deleteFramebuffers(const unsigned int count, const unsigned int* ids)
{
	GL_CHECK(glDeleteFramebuffers(count, ids));
}

void FrameBuffer::bindFramebuffer(const unsigned int target, const unsigned int framebuffer)
{
	GL_CHECK(glBindFramebuffer(target, framebuffer));
}

void FrameBuffer::drawBuffer(const unsigned int buffer)
{
	GL_CHECK(glDrawBuffer(buffer));
}

void FrameBuffer::drawBuffers(const unsigned int n, const unsigned int* bufs)
{
	GL_CHECK(glDrawBuffers(n, bufs));
}

void FrameBuffer::readBuffer(const unsigned int src)
{
	GL_CHECK(glReadBuffer(src));
}

void FrameBuffer::blitFramebuffer(const int srcX0, const int srcY0, const int srcX1, const int srcY1,
								  const int dstX0, const int dstY0, const int dstX1, const int dstY1,
								  const unsigned int mask, const unsigned int filter)
{
	GL_CHECK(glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter));
}

void FrameBuffer::clearBuffers(const unsigned int mask)
{
	GL_CHECK(glClear(mask));
}

void FrameBuffer::viewport(const int x, const int y, const int width, const int height)
{
	GL_CHECK(glViewport(x, y, width, height));
}

void FrameBuffer::readPixelsBuffer(const int x, const int y, const int width, const int height, const unsigned int format,
								   const unsigned int type, void* data)
{
	GL_CHECK(glReadPixels(x, y, width, height, format, type, data));
}

unsigned int FrameBuffer::checkFramebufferStatus(const unsigned int target)
{
	return glCheckFramebufferStatus(target);
}

void FrameBuffer::getIntegerv(const unsigned int pname, int* params)
{
	GL_CHECK(glGetIntegerv(pname, params));
}

// Update FrameBuffer methods to use the new wrapper methods

void FrameBuffer::cleanup()
{
	if (mRendererID != 0)
	{
		LOG_DEBUG(logging::gGraphicsLogger, "Deleting framebuffer: unnamed (ID: {0})",
				  mRendererID);

		deleteFramebuffers(1, &mRendererID);
		mRendererID = 0;
	}

	// We don't need to explicitly delete the textures as they're managed by shared_ptr
}

void FrameBuffer::bind() const
{
	if (mRendererID == 0)
	{
		LOG_ERROR(logging::gGraphicsLogger, "Trying to bind invalid framebuffer with ID 0");
		return;
	}

	bindFramebuffer(GL_FRAMEBUFFER, mRendererID);
	viewport(mViewportX, mViewportY, mViewportW, mViewportH);

	LOG_TRACE(logging::gGraphicsLogger, "Bound framebuffer: unnamed (ID: {0})",
			  mRendererID);
}

void FrameBuffer::unbind()
{
	bindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::bindRead() const
{
	if (mRendererID == 0)
	{
		LOG_ERROR(logging::gGraphicsLogger, "Trying to bind invalid framebuffer with ID 0 for reading");
		return;
	}

	bindFramebuffer(GL_READ_FRAMEBUFFER, mRendererID);
}

void FrameBuffer::bindDraw() const
{
	if (mRendererID == 0)
	{
		LOG_ERROR(logging::gGraphicsLogger, "Trying to bind invalid framebuffer with ID 0 for drawing");
		return;
	}

	bindFramebuffer(GL_DRAW_FRAMEBUFFER, mRendererID);
}

void FrameBuffer::resize(int width, int height)
{
	if (width <= 0 || height <= 0)
	{
		LOG_WARN(logging::gGraphicsLogger, "Attempted to resize FBO 'unnamed' to invalid dimensions: {0}x{1}",
				 width, height);
		return;
	}

	if (width == mWidth && height == mHeight)
	{
		LOG_DEBUG(logging::gGraphicsLogger, "Skipping resize of FBO 'unnamed' - dimensions unchanged: {0}x{1}",
				  width, height);
		return;
	}

	LOG_DEBUG(logging::gGraphicsLogger, "Resizing FBO 'unnamed' from {0}x{1} to {2}x{3}",
			  mWidth, mHeight, width, height);

	mWidth = width;
	mHeight = height;

	// Update viewport to match if it was previously matching
	if (mViewportW == mWidth && mViewportH == mHeight)
	{
		mViewportW = width;
		mViewportH = height;
	}

}

void FrameBuffer::setViewport(int x, int y, int width, int height)
{
	mViewportX = x;
	mViewportY = y;
	mViewportW = width;
	mViewportH = height;
}

void FrameBuffer::getViewport(int& x, int& y, int& width, int& height) const
{
	x = mViewportX;
	y = mViewportY;
	width = mViewportW;
	height = mViewportH;
}

void FrameBuffer::clear(unsigned int mask) const
{
	bind();
	clearBuffers(mask);
}


int FrameBuffer::addAttachment(FrameBufferAttachmentSpecification attachmentSpec)
{
	LOG_DEBUG(logging::gGraphicsLogger, "Adding attachment to framebuffer 'unnamed'");

	int index = 0;
	if (attachmentSpec.Type == FrameBufferAttachmentType::Color)
	{
		// Count existing color attachments for index
		for (const auto& spec : mAttachmentSpecs)
		{
			if (spec.Type == FrameBufferAttachmentType::Color)
				index++;
		}
	}
	const FrameBufferAttachmentType specType = attachmentSpec.Type;
	mAttachmentSpecs.push_back(std::move(attachmentSpec));

	return specType == FrameBufferAttachmentType::Color ? index : 0;
}

bool FrameBuffer::removeAttachment(FrameBufferAttachmentType type, int index)
{
	bool removed = false;

	if (type == FrameBufferAttachmentType::Color)
	{
		// Find the specific color attachment at the given index
		int colorIndex = 0;
		for (auto it = mAttachmentSpecs.begin(); it != mAttachmentSpecs.end(); ++it)
		{
			if (it->Type == FrameBufferAttachmentType::Color)
			{
				if (colorIndex == index)
				{
					LOG_DEBUG(logging::gGraphicsLogger, "Removing color attachment {0} from framebuffer 'unnamed'",
							  index);
					mAttachmentSpecs.erase(it);
					removed = true;
					break;
				}
				colorIndex++;
			}
		}
	}
	else
	{
		// For other types, just remove the first match
		for (auto it = mAttachmentSpecs.begin(); it != mAttachmentSpecs.end(); ++it)
		{
			if (it->Type == type)
			{
				LOG_DEBUG(logging::gGraphicsLogger, "Removing attachment of type {0} from framebuffer 'unnamed'",
						  static_cast<int>(type));
				mAttachmentSpecs.erase(it);
				removed = true;
				break;
			}
		}
	}

	if (removed)
	{
		invalidate();
	}
	else
	{
		LOG_WARN(logging::gGraphicsLogger, "No matching attachment found to remove from framebuffer 'unnamed'");
	}

	return removed;
}


void FrameBuffer::setSamples(int samples)
{
	if (samples < 1)
	{
		LOG_WARN(logging::gGraphicsLogger, "Invalid sample count {0}, must be at least 1", samples);
		return;
	}

	if (samples == mSamples)
	{
		return; // No change needed
	}

	LOG_DEBUG(logging::gGraphicsLogger, "Changing FBO 'unnamed' samples from {0} to {1}",
			  mSamples, samples);

	mSamples = samples;
}


Texture& FrameBuffer::getColorAttachment(int index) const
{
	if (index < 0 || index >= static_cast<int>(mColorAttachments.size()))
	{
		if (!mColorAttachments.empty()) // Only log if there are attachments
		{
			LOG_WARN(logging::gGraphicsLogger,
					 "Requesting invalid color attachment index {0} (max: {1}) from framebuffer 'unnamed'",
					 index, mColorAttachments.size() - 1);
		}
		//Throw error
	}
	return *mColorAttachments[index];
}

void FrameBuffer::setDrawBuffers(const std::vector<unsigned int>& attachments) const
{
	bindDraw();
	if (attachments.empty())
	{
		drawBuffer(GL_NONE);
	}
	else
	{
		drawBuffers(static_cast<GLsizei>(attachments.size()), attachments.data());
	}
}

void FrameBuffer::setReadBuffer(const unsigned int attachment) const
{
	bindRead();
	readBuffer(attachment);
}

void FrameBuffer::blitTo(
	const std::shared_ptr<FrameBuffer>& dst,
	int srcX0, int srcY0, int srcX1, int srcY1,
	int dstX0, int dstY0, int dstX1, int dstY1,
	const unsigned int mask,
	const unsigned int filter) const
{
	if (!dst)
	{
		LOG_ERROR(logging::gGraphicsLogger, "Cannot blit to null framebuffer");
		return;
	}

	// Source region validation
	if (srcX0 < 0 || srcY0 < 0 || srcX1 > mWidth || srcY1 > mHeight ||
		srcX0 >= srcX1 || srcY0 >= srcY1)
	{
		LOG_WARN(logging::gGraphicsLogger, "Invalid source region for blit: ({0},{1}) to ({2},{3})",
				 srcX0, srcY0, srcX1, srcY1);
	}

	// Destination region validation
	if (dstX0 < 0 || dstY0 < 0 || dstX1 > dst->getWidth() || dstY1 > dst->getHeight() ||
		dstX0 >= dstX1 || dstY0 >= dstY1)
	{
		LOG_WARN(logging::gGraphicsLogger, "Invalid destination region for blit: ({0},{1}) to ({2},{3})",
				 dstX0, dstY0, dstX1, dstY1);
	}

	bindFramebuffer(GL_READ_FRAMEBUFFER, mRendererID);
	bindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->getRendererID());

	blitFramebuffer(
		srcX0, srcY0, srcX1, srcY1,
		dstX0, dstY0, dstX1, dstY1,
		mask,
		filter
	);

	// Restore default framebuffer
	bindFramebuffer(GL_FRAMEBUFFER, 0);
}


void FrameBuffer::attachExternalTexture(
	unsigned int attachment,
	unsigned int target,
	unsigned int textureID,
	int mipLevel) const
{
	if (textureID == 0)
	{
		LOG_ERROR(logging::gGraphicsLogger, "Cannot attach invalid (zero) texture ID");
		return;
	}

	bind();

	if (target == GL_TEXTURE_CUBE_MAP)
	{
		Texture::framebufferTexture(GL_FRAMEBUFFER, attachment, textureID, mipLevel);
	}
	else
	{
		Texture::framebufferTexture2D(GL_FRAMEBUFFER, attachment, target, textureID, mipLevel);
	}

	// Verify attachment was successful
	unsigned int status = checkFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR(logging::gGraphicsLogger,
				  "Failed to attach external texture to framebuffer 'unnamed' (status: 0x{0:x})",
				  status);
	}
}

void FrameBuffer::invalidate()
{
	// Clean up existing framebuffer
	cleanup();

	// Recreate the framebuffer with current specifications
	createFramebuffer();
}

void FrameBuffer::createFramebuffer()
{
	LOG_DEBUG(logging::gGraphicsLogger, "Creating framebuffer with {0} attachment specifications",
			  mAttachmentSpecs.size());

	genFramebuffers(1, &mRendererID);
	ASSERT_LOG(logging::gGraphicsLogger, mRendererID != 0, "Failed to generate framebuffer ID");


	bindFramebuffer(GL_FRAMEBUFFER, mRendererID);

	GLuint colorAttachmentIndex = 0;
	std::vector<GLenum> drawBuffers;

	for (const auto& spec : mAttachmentSpecs)
	{
		GLenum glInternalFormat = toGLInternalFormat(spec.Format);
		GLenum glFormat = toGLFormat(spec.Format);
		GLenum glType = toGLType(spec.Format);

		if (glInternalFormat == 0 || glFormat == 0 || glType == 0)
		{
			LOG_ERROR(logging::gGraphicsLogger, "Unsupported framebuffer format: {0}",
					  static_cast<int>(spec.Format));
			continue;
		}

		// Create a texture ID
		GLuint texID = 0;
		Texture::genTextures(1, &texID);
		if (texID == 0)
		{
			LOG_ERROR(logging::gGraphicsLogger, "Failed to generate texture ID for framebuffer attachment");
			continue;
		}

		// Set debug label for texture if we have attachment name
		if (!spec.Name.empty())
		{
			std::string texLabel = spec.Name + " Texture";
			gl::labelObject(GL_TEXTURE, texID, texLabel.c_str());
		}

		// Decide how to bind texture based on type/samples
		GLenum bindTarget = (mSamples > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

		// Special handling for cubemap depth attachment
		if (spec.Type == FrameBufferAttachmentType::DepthCubemap && mSamples <= 1)
		{
			bindTarget = GL_TEXTURE_CUBE_MAP;
			Texture::bindTexture(bindTarget, texID);

			for (int face = 0; face < 6; ++face)
			{
				Texture::texImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
									0,
									glInternalFormat,
									mWidth,
									mHeight,
									0,
									glFormat,
									glType,
									nullptr);
			}

			// Setup cubemap parameters
			Texture::texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			Texture::texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			Texture::texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			Texture::texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			Texture::texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			// Enable depth comparison for shadow sampling
			Texture::texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			Texture::texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

			// Attach as depth-only
			Texture::framebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texID, 0);

			// Wrap in our universal Texture class
			auto attachmentTexture = std::make_unique<Texture>(texID, mWidth, mHeight, Texture::Type::CUBEMAP);

			mDepthAttachment = std::move(attachmentTexture);

			// Disable color buffer for a depth-only cubemap
			drawBuffer(GL_NONE);
			readBuffer(GL_NONE);

			continue; // Done handling this attachment
		}

		// Regular 2D texture or 2D multisampled texture handling
		Texture::bindTexture(bindTarget, texID);

		if (mSamples > 1)
		{
			// Multisampled texture
			Texture::texImage2DMultisample(
				GL_TEXTURE_2D_MULTISAMPLE,
				mSamples,
				glInternalFormat,
				mWidth,
				mHeight,
				GL_FALSE
			);
		}
		else
		{
			// Regular 2D texture
			Texture::texImage2D(
				GL_TEXTURE_2D,
				0,
				glInternalFormat,
				mWidth,
				mHeight,
				0,
				glFormat,
				glType,
				nullptr
			);

			// Set texture parameters for non-multisampled textures
			Texture::texParameteri(bindTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			Texture::texParameteri(bindTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			Texture::texParameteri(bindTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			Texture::texParameteri(bindTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		// Create and attach appropriate texture type based on attachment spec
		std::unique_ptr<Texture> attachmentTexture;

		switch (spec.Type)
		{
		case FrameBufferAttachmentType::Color:
		{
			attachmentTexture = std::make_unique<Texture>(texID, mWidth, mHeight, Texture::Type::TEXTURE_2D);
			GLenum attachmentPoint = GL_COLOR_ATTACHMENT0 + colorAttachmentIndex;

			if (mSamples > 1)
			{
				Texture::framebufferTexture2D(
					GL_FRAMEBUFFER,
					attachmentPoint,
					GL_TEXTURE_2D_MULTISAMPLE,
					texID,
					0
				);
			}
			else
			{
				Texture::framebufferTexture2D(
					GL_FRAMEBUFFER,
					attachmentPoint,
					GL_TEXTURE_2D,
					texID,
					0
				);
			}

			mColorAttachments.push_back(std::move(attachmentTexture));
			drawBuffers.push_back(attachmentPoint);
			colorAttachmentIndex++;
			break;
		}

		case FrameBufferAttachmentType::Depth:
		{
			attachmentTexture = std::make_unique<Texture>(texID, mWidth, mHeight, Texture::Type::TEXTURE_2D);

			if (mSamples > 1)
			{
				Texture::framebufferTexture2D(
					GL_FRAMEBUFFER,
					GL_DEPTH_ATTACHMENT,
					GL_TEXTURE_2D_MULTISAMPLE,
					texID,
					0
				);
			}
			else
			{
				Texture::framebufferTexture2D(
					GL_FRAMEBUFFER,
					GL_DEPTH_ATTACHMENT,
					GL_TEXTURE_2D,
					texID,
					0
				);
			}

			mDepthAttachment = std::move(attachmentTexture);
			break;
		}

		case FrameBufferAttachmentType::Stencil:
		{
			attachmentTexture = std::make_unique<Texture>(texID, mWidth, mHeight, Texture::Type::TEXTURE_2D);

			if (mSamples > 1)
			{
				Texture::framebufferTexture2D(
					GL_FRAMEBUFFER,
					GL_STENCIL_ATTACHMENT,
					GL_TEXTURE_2D_MULTISAMPLE,
					texID,
					0
				);
			}
			else
			{
				Texture::framebufferTexture2D(
					GL_FRAMEBUFFER,
					GL_STENCIL_ATTACHMENT,
					GL_TEXTURE_2D,
					texID,
					0
				);
			}
			break;
		}

		case FrameBufferAttachmentType::DepthStencil:
		{
			attachmentTexture = std::make_unique<Texture>(texID, mWidth, mHeight, Texture::Type::TEXTURE_2D);

			if (mSamples > 1)
			{
				Texture::framebufferTexture2D(
					GL_FRAMEBUFFER,
					GL_DEPTH_STENCIL_ATTACHMENT,
					GL_TEXTURE_2D_MULTISAMPLE,
					texID,
					0
				);
			}
			else
			{
				Texture::framebufferTexture2D(
					GL_FRAMEBUFFER,
					GL_DEPTH_STENCIL_ATTACHMENT,
					GL_TEXTURE_2D,
					texID,
					0
				);
			}

			mDepthAttachment = std::move(attachmentTexture);
			break;
		}

		default:
			LOG_WARN(logging::gGraphicsLogger, "Unhandled attachment type: {0}",
					 static_cast<int>(spec.Type));
		}
	}

	// Configure draw/read buffers based on attachments
	if (!drawBuffers.empty())
	{
		this->drawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());
	}
	else if (mDepthAttachment)
	{
		// Depth-only framebuffer
		drawBuffer(GL_NONE);
		readBuffer(GL_NONE);
	}
	else
	{
		LOG_WARN(logging::gGraphicsLogger, "Creating framebuffer with no attachments");
	}

	// Validate completeness
	GLenum status = checkFramebufferStatus(GL_FRAMEBUFFER);

	ASSERT_LOG(logging::gGraphicsLogger, status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete: status code 0x{:x}", status);

	bindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool FrameBuffer::isComplete() const
{
	if (mRendererID == 0)
	{
		return false;
	}

	// Save current framebuffer binding to restore it after checking
	int previousFramebuffer = 0;
	getIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer);

	// Bind this framebuffer to check its status
	bindFramebuffer(GL_FRAMEBUFFER, mRendererID);

	// Check if the framebuffer is complete
	unsigned int status = checkFramebufferStatus(GL_FRAMEBUFFER);

	// Restore previous framebuffer binding
	bindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer);

	return status == GL_FRAMEBUFFER_COMPLETE;
}


