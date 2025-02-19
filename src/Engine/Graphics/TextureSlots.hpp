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
  
}  
namespace ShadowSlots {
    constexpr unsigned int DIRECTIONAL = 7;
    constexpr unsigned int SPOT = 8;
    constexpr unsigned int POINT = 9;
}
namespace IBLSlots {  
    constexpr unsigned int IRRADIANCE = 10;  
    constexpr unsigned int PREFILTER = 11;  
    constexpr unsigned int BRDFLUT =12;  
    constexpr unsigned int SKYBOX = 0;  
}  

namespace PostProcessSlots {  
    constexpr unsigned int HDR = 0;  
    constexpr unsigned int BLOOM = 1;  
}
