#pragma once
class texture
{
public:
    texture()
    {

    }
    unsigned int ID;
    unsigned int width;
    unsigned int height;
    void generate();
    void bind();
};