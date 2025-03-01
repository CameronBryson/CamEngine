#pragma once  


namespace ShadowSlots {
    constexpr unsigned int DIRECTIONAL = 0;
    constexpr unsigned int SPOT = 1;
    constexpr unsigned int POINT = 2;
}


namespace MaterialSlots {  
    constexpr unsigned int ALBEDO = 3;  
    constexpr unsigned int NORMAL = 4;  
    constexpr unsigned int METALLIC = 5;  
    constexpr unsigned int ROUGHNESS = 6;  
    constexpr unsigned int AO = 7;  
    constexpr unsigned int EMISSIVE = 8;  
    constexpr unsigned int METALROUGH = 9;  
    constexpr unsigned int DISPLACEMENT = 10;
    constexpr unsigned int OPACITY = 11;
}



namespace IBLSlots {  
    constexpr unsigned int IRRADIANCE = 12;  
    constexpr unsigned int PREFILTER = 13;  
    constexpr unsigned int BRDFLUT = 14;  
    // Special case - reuses first material slot since skybox is rendered separately
    constexpr unsigned int SKYBOX = 3;  
}  


namespace GBufferSlots {
    constexpr unsigned int ALBEDO_AO = 3;       // Reuses ALBEDO
    constexpr unsigned int NORMAL_METALLIC = 4;  // Reuses NORMAL
    constexpr unsigned int ROUGH_EMISSIVE = 5;   // Reuses METALLIC
    constexpr unsigned int DEPTH = 6;            // Reuses ROUGHNESS
    constexpr unsigned int VELOCITY = 7;         // Reuses AO
}


namespace PostProcessSlots {  
    constexpr unsigned int HDR = 3;           // Reuses ALBEDO
    constexpr unsigned int BLOOM = 4;         // Reuses NORMAL
    constexpr unsigned int DEPTH = 5;         // Reuses METALLIC
    constexpr unsigned int TAA_HISTORY = 6;   // Reuses ROUGHNESS
	constexpr unsigned int LUMINANCE = 12;    
}


namespace SSAOSlots {
    constexpr unsigned int DEPTH = 8;            // Reuses EMISSIVE
    constexpr unsigned int NORMAL_METALLIC = 9;  // Reuses METALROUGH
    constexpr unsigned int NOISE = 10;           // Reuses DISPLACEMENT
    constexpr unsigned int SSAO = 11;            // Reuses OPACITY
    constexpr unsigned int SSAO_BLUR = 8;        // Reuses EMISSIVE (after SSAO pass)
}
