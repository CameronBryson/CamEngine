#pragma once  

// It's crucial that texture slots used concurrently in the *same shader pass*
// (especially the lighting and forward passes) do not overlap.

namespace ShadowSlots
{
	constexpr unsigned int DIRECTIONAL = 18;
	constexpr unsigned int SPOT = 19;
	constexpr unsigned int POINT = 20;
}

namespace MaterialSlots
{
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

namespace GBufferSlots
{
	constexpr unsigned int ALBEDO_AO = 10;       // Reuses ALBEDO
	constexpr unsigned int NORMAL_METALLIC = 11;  // Reuses NORMAL
	constexpr unsigned int ROUGH_EMISSIVE = 12;   // Reuses METALLIC
	constexpr unsigned int DEPTH = 13;            // Reuses ROUGHNESS
	constexpr unsigned int VELOCITY_REFLECTIVE = 14;         // Reuses AO
}

namespace PostProcessSlots
{
	constexpr unsigned int HDR_INPUT_OUTPUT = 24; // Main buffer for HDR pipeline stages
	constexpr unsigned int BLOOM_EXTRACT = 25;    // Bright parts extracted for bloom
	constexpr unsigned int BLOOM_PING = 26;       // Ping-pong buffer 1 for bloom blur
	constexpr unsigned int BLOOM_PONG = 27;       // Ping-pong buffer 2 for bloom blur
	constexpr unsigned int TAA_HISTORY = 28;      // Previous frame's TAA result
}

namespace IBLSlots
{
	constexpr unsigned int IRRADIANCE = 15;
	constexpr unsigned int PREFILTER = 16;
	constexpr unsigned int BRDFLUT = 17;
	// Special case - reuses first material slot since skybox is rendered separately
	constexpr unsigned int SKYBOX = 3;
}

namespace SSAOSlots
{
	constexpr unsigned int DEPTH = GBufferSlots::DEPTH;            // Reuses DEPTH
	constexpr unsigned int NORMAL_METALLIC = GBufferSlots::NORMAL_METALLIC;  // Reuses NORMAL_METALLIC
	constexpr unsigned int NOISE = 21;           // Reuses DISPLACEMENT
	constexpr unsigned int SSAO_RAW_RESULT = 22; // Output: Result of main SSAO pass, input to blur
	constexpr unsigned int SSAO_BLURRED_RESULT = 23; // Output: Result of blur pass, input to lighting/forward
}
