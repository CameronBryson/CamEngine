#include "pch.hpp"
#include "UniformBuffer.hpp"
#include "platform.hpp"
#include "OpenGLUtil.hpp"

UniformBuffer::UniformBuffer(unsigned int size, unsigned int binding) : mSize(size), mBinding(binding)
{
	if (size == 0) {
		// Handle error: UBO size cannot be zero
		LOG_ERROR(logging::gGraphicsLogger, "Attempted to create UniformBuffer with size 0.");
		// Consider throwing an exception or setting an invalid state
		throw error_handling::GraphicsException("UniformBuffer size cannot be zero.");
	}
	// Generate the UBO
	GL_CHECK(glGenBuffers(1, &mRendererID));
	if (mRendererID == 0) {
		// Handle error: Failed to generate buffer ID
		throw error_handling::GraphicsException("Failed to generate UniformBuffer ID.");
	}
	GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, mRendererID));
	GL_CHECK(glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW)); // Use GL_DYNAMIC_DRAW if data changes often

	// Bind it to a specific binding point
	GL_CHECK(glBindBufferBase(GL_UNIFORM_BUFFER, binding, mRendererID));

	GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	LOG_TRACE(logging::gGraphicsLogger, "UniformBuffer created (ID: {}, Size: {}, Binding: {}).", mRendererID, mSize, mBinding);
}

UniformBuffer::~UniformBuffer()
{
	if (mRendererID != 0) {
		LOG_TRACE(logging::gGraphicsLogger, "Deleting UniformBuffer (ID: {}).", mRendererID);
		GL_CHECK(glDeleteBuffers(1, &mRendererID));
		mRendererID = 0; // Ensure ID is zeroed after deletion
	}
}

// Move constructor
UniformBuffer::UniformBuffer(UniformBuffer&& other) noexcept
	: mRendererID(other.mRendererID),
	  mSize(other.mSize),
	  mBinding(other.mBinding)
{
	// Invalidate other object
	other.mRendererID = 0;
	other.mSize = 0;
	other.mBinding = 0;
	LOG_TRACE(logging::gGraphicsLogger, "UniformBuffer move constructed (New ID: {}).", mRendererID);
}

// Move assignment operator
UniformBuffer& UniformBuffer::operator=(UniformBuffer&& other) noexcept
{
	if (this != &other)
	{
		// Clean up existing resource
		if (mRendererID != 0) {
			GL_CHECK(glDeleteBuffers(1, &mRendererID));
		}

		// Move resources from other
		mRendererID = other.mRendererID;
		mSize = other.mSize;
		mBinding = other.mBinding;

		// Invalidate other object
		other.mRendererID = 0;
		other.mSize = 0;
		other.mBinding = 0;
		LOG_TRACE(logging::gGraphicsLogger, "UniformBuffer move assigned (New ID: {}).", mRendererID);
	}
	return *this;
}

void UniformBuffer::setData(const void* data, unsigned int size, unsigned int offset)
{
	if (mRendererID == 0) {
		LOG_ERROR(logging::gGraphicsLogger, "Attempted to set data on invalid UniformBuffer.");
		return;
	}
	if (data == nullptr) {
		LOG_WARN(logging::gGraphicsLogger, "Attempted to set null data on UniformBuffer (ID: {}).", mRendererID);
		// Depending on use case, this might be valid or an error.
		// return; // Or proceed if setting null data is intended.
	}
	if (offset + size > mSize) {
		LOG_ERROR(logging::gGraphicsLogger, "UniformBuffer::setData out of bounds (Offset: {}, Size: {}, Buffer Size: {}).", offset, size, mSize);
		// Option 1: Throw an exception
		// throw std::out_of_range("UniformBuffer::setData out of bounds");
		// Option 2: Clamp the size (may hide errors)
		// size = mSize - offset;
		// if (size <= 0) return;
		// Option 3: Just return and log
		return; 
	}

	GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, mRendererID));
	// Update the subrange [offset, offset+size] in the UBO
	GL_CHECK(glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data));
	GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	// LOG_TRACE(logging::gGraphicsLogger, "Set data for UniformBuffer (ID: {}, Size: {}, Offset: {}).", mRendererID, size, offset); // Can be verbose
}
