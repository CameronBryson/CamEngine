#include "pch.hpp"
#include "ShaderStorageBuffer.hpp"
#include "platform.hpp"
#include "OpenGLUtil.hpp"
#include "Engine/Util/Logging.hpp"

ShaderStorageBuffer::ShaderStorageBuffer(unsigned int size, unsigned int binding)
	: mSize(size)
{
	ASSERT_LOG(logging::gGraphicsLogger, size > 0, "Attempted to create ShaderStorageBuffer with size 0.");

	// Generate the SSBO
	GL_CHECK(glGenBuffers(1, &mID));
	ASSERT_LOG(logging::gGraphicsLogger, mID != 0, "Failed to generate ShaderStorageBuffer ID.");

	GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, mID));
	GL_CHECK(glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));

	// Bind it to a specific binding point
	GL_CHECK(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, mID));

	GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
	LOG_TRACE(logging::gGraphicsLogger, "ShaderStorageBuffer created (ID: {}, Size: {}, Binding: {}).", mID, mSize, binding);
}

ShaderStorageBuffer::~ShaderStorageBuffer()
{
	if (mID != 0) {
		LOG_TRACE(logging::gGraphicsLogger, "Deleting ShaderStorageBuffer (ID: {}).", mID);
		GL_CHECK(glDeleteBuffers(1, &mID));
		mID = 0;
	}
}

// Move constructor
ShaderStorageBuffer::ShaderStorageBuffer(ShaderStorageBuffer&& other) noexcept
	: mID(other.mID),
	  mSize(other.mSize)
{
	// Invalidate other object
	other.mID = 0;
	other.mSize = 0;
	LOG_TRACE(logging::gGraphicsLogger, "ShaderStorageBuffer move constructed (New ID: {}).", mID);
}

// Move assignment operator
ShaderStorageBuffer& ShaderStorageBuffer::operator=(ShaderStorageBuffer&& other) noexcept
{
	if (this != &other)
	{
		// Clean up existing resource
		if (mID != 0) {
			GL_CHECK(glDeleteBuffers(1, &mID));
		}

		// Move resources from other
		mID = other.mID;
		mSize = other.mSize;

		// Invalidate other object
		other.mID = 0;
		other.mSize = 0;
		LOG_TRACE(logging::gGraphicsLogger, "ShaderStorageBuffer move assigned (New ID: {}).", mID);
	}
	return *this;
}

void ShaderStorageBuffer::bind(unsigned int binding) const
{
	GL_CHECK(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, mID));
}

void ShaderStorageBuffer::unbind() const
{
	// Unbind the generic target, not a specific index
	GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
}

void ShaderStorageBuffer::setData(const void* data, unsigned int size, unsigned int offset)
{
	if (mID == 0) {
		LOG_ERROR(logging::gGraphicsLogger, "Attempted to set data on invalid ShaderStorageBuffer.");
		return;
	}
	if (data == nullptr) {
		LOG_WARN(logging::gGraphicsLogger, "Attempted to set null data on ShaderStorageBuffer (ID: {}).", mID);
		// return; // Or proceed if intended
	}
	if (offset + size > mSize) {
		LOG_ERROR(logging::gGraphicsLogger, "ShaderStorageBuffer::setData out of bounds (Offset: {}, Size: {}, Buffer Size: {}).", offset, size, mSize);
		return; 
	}

	GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, mID));
	// Use glBufferSubData to update a portion of the buffer
	GL_CHECK(glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data));
	GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
	// LOG_TRACE(logging::gGraphicsLogger, "Set data for SSBO (ID: {}, Size: {}, Offset: {}).", mID, size, offset);
}

void ShaderStorageBuffer::clear()
{
	if (mID == 0) {
		LOG_ERROR(logging::gGraphicsLogger, "Attempted to clear invalid ShaderStorageBuffer.");
		return;
	}
	// Use glClearBufferData for potentially more efficient clearing
	// Note: GL_R32UI / GL_RED / GL_UNSIGNED_INT might not be the correct format/type 
	//       depending on how the data is interpreted in the shader. 
	//       Choose a format that matches your data structure (e.g., GL_RGBA32F, GL_RGBA, GL_FLOAT for vec4s)
	//       Using nullptr for data initializes to zeros.
	uint32_t zero = 0; // Example: clear with zero
	GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, mID));
	// GL_CHECK(glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, &zero)); 
	GL_CHECK(glClearBufferSubData(GL_SHADER_STORAGE_BUFFER, GL_R8, 0, mSize, GL_RED, GL_UNSIGNED_BYTE, &zero)); // Clear with single byte 0s
	GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
	LOG_TRACE(logging::gGraphicsLogger, "Cleared ShaderStorageBuffer (ID: {}).", mID);
}

