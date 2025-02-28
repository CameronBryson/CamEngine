// TextureSlots.hpp  
#pragma once  

namespace MaterialSlots {  
    constexpr unsigned int ALBEDO = 0;  
    constexpr unsigned int NORMAL = 1;  
    constexpr unsigned int METALLIC = 2;  
    constexpr unsigned int ROUGHNESS = 3;  
    constexpr unsigned int AO = 4;  
    constexpr unsigned int EMISSIVE = 5;  
    constexpr unsigned int METALROUGH = 6;  
	constexpr unsigned int DISPLACEMENT = 7;
	constexpr unsigned int OPACITY = 8;
  
}  
namespace ShadowSlots {
    constexpr unsigned int DIRECTIONAL = 9;
    constexpr unsigned int SPOT = 10;
    constexpr unsigned int POINT = 11;
}
namespace IBLSlots {  
    constexpr unsigned int IRRADIANCE = 12;  
    constexpr unsigned int PREFILTER = 13;  
    constexpr unsigned int BRDFLUT =14;  
    constexpr unsigned int SKYBOX = 0;  
}  

namespace PostProcessSlots {  
    constexpr unsigned int HDR = 0;  
    constexpr unsigned int BLOOM = 1;
    constexpr unsigned int DEPTH = 2;
	constexpr unsigned int TAA_HISTORY = 3;
}
namespace GBufferSlots {
    constexpr unsigned int ALBEDO_AO = 15;    // After IBL slots
    constexpr unsigned int NORMAL_METALLIC = 16;
    constexpr unsigned int ROUGH_EMISSIVE = 17;
    constexpr unsigned int DEPTH = 18;
	constexpr unsigned int VELOCITY = 24;
}

namespace SSAOSlots {
	constexpr unsigned int DEPTH = 19;         // Depth texture
	constexpr unsigned int NORMAL_METALLIC = 20;         // Random rotation vectors texture
    constexpr unsigned int NOISE = 21;         // Random rotation vectors texture
    constexpr unsigned int SSAO = 22;          // Raw SSAO result
    constexpr unsigned int SSAO_BLUR = 23;     // Blurred SSAO result
}