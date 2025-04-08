#pragma once
class ShaderStorageBuffer
{
public:
	ShaderStorageBuffer(unsigned int size, unsigned int binding);
	~ShaderStorageBuffer() ;
	void bind(unsigned int binding) const ;
	void unbind() const ;
	void setData(const void* data, unsigned int size) ;
	unsigned int getID() const ;
	void clear() ;
private:
	unsigned int mID;
	unsigned int mSize;
};

