#pragma once
#include "Mesh.hpp"
#include "Texture2D.hpp"
#include "TextureCubemap.hpp"
#include <memory>
class EnvironmentMap
{
public:
private:
	std::shared_ptr<TextureCubemap> mTexture;
	std::shared_ptr<TextureCubemap> mIrradianceMap;
	std::shared_ptr<TextureCubemap> mPrefilterMap;
	std::shared_ptr<Texture2D> mBRDFLUT;
	std::shared_ptr<Mesh> mSkybox;

};

