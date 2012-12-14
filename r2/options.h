////////////////////////////////////////////////
// utak3r's shaders config file
// Modified by utak3r
// Includes SkyGRAPHICS 2.0 RC4.1, Float32 2.0, and Call of Pripyat shaders
////////////////////////////////////////////////
#ifndef SHADERSOPTIONS_CFG
#define SHADERSOPTIONS_CFG

	#define DEPTH_OF_FIELD	//Enables DOF. Objects in the distace are given a slight blur.
	#define SSAO	//Enables SSAO. Small framerate decrease for a very large graphical difference.
	// #define SUN_SHAFTS	//Enables sun shafts from shadows. Severely decreases framerate.
	#define PARALLAX_OCCLUSION	// Enables Parallax Occlusion Mapping.
	// #define USE_SUNMASK	//Enables sun shadow masking. Cannot be used with Float32.
	#define USE_SJITTER	//Uses shadow jittering, a modern method of smoothing out shadow edges. Applies to flashlight, and to world if SUN_FILTER is enabled.

	#define USE_F32	//Enables Float32 options (HDR/Model Lighting). Breaks saturation and contrast filters.
		//#define USE_F32_FOG	//Enables Float32 style fog. Requires Float32 to be enabled.
	#define USE_CUSTOM_SHADOW_RESOLUTION	//Use the custom shadow size defined below instead of the default 1024x1024 (1024)
		#define SHADOW_RESOLUTION int(2048) //Shadow size. Defaults to 2048 (2048x2048). Do not comment this, even if you aren't using it.

////////////////////////////////////////////////
//Parallax Occlusion Mapping options:
////////////////////////////////////////////////
	//Basic Options:
	#define MAX_SAMPLES int(32)	//Maximum amount of samples done to a texture.
	#define MIN_SAMPLES int(4)	//Minimum amount of samples done to a texture.
	#define PARALLAX_OFFSET float(0.015)	//Height of surface per sample.
	#define FINAL_INTERSECTION_LOOPS int(5)	//Amount of additional samples to increase accuracy.

	//Performance Options:
	#define PARALLAX_FADE	//Parllax textures fade back to regular normals with distance; increases FPS and fixes anisotropic filtering.
	#define START_FADE float(0.0003)	//Distance the fading starts
	#define STOP_FADE float(0.0004)		//Distance the fading stops, and the texture returns to just using normals.
	
	//Other:
	#define CONTRAST_COEF_Q1 float(0.6)	//Amount of contrast in calculations.
	#define CONTRAST_COEF_Q2 float(1)	//Amount of contrast in calculations.
	#define BRIGHTNESS_COEF float(0.25)	//Amount of brightness in calculations.

////////////////////////////////////////////////
//Sreen Space Ambient Occlusion options:
////////////////////////////////////////////////
	//Basic Options:
	#define SSAO_QUALITY int(3) 	//SSAO samples. 0 and 1 are not valid. Use 2 for low quality an 3 for high quality.
	#define SSAO_TEX_CONT		//Increases texture contrast for diffuse lighting.
	#define CONT_COEF float(0.3)	//Brightness increase amount.

	#define FADE_DIST float(4)
	#define FADE_COEF float(0.2)

////////////////////////////////////////////////
//Depth of Field options:
////////////////////////////////////////////////
	//Basic options:
	#define DOF_QUALITY int(12)	//Amount of DOF samples. Maximum value is 12.

	//Distance Options:
	#define MINDIST float(0.4)	//Minimum distance DOF starts.
	#define MAXDIST float(375)	//Distance at which DOF stops rendering.
	#define MAXCOF	float(3)

////////////////////////////////////////////////
//Sun Shaft options:
////////////////////////////////////////////////
	//Basic options:
	#define RAY_SAMPLES int(20)	//Amount of sun ray samples. The higher this is, the lower your framerate.
	#define SS_INTENSITY float(1)	//Intensity of sun rays per sample.

	#define SS_CONTRAST float(1)	//Amount of contrast during sunray calculation.
	#define SS_FARNEARLERP

////////////////////////////////////////////////
//Float32 2.0 Options:
////////////////////////////////////////////////
	//Basic options:
	#define CT_int  1.25f 		//Cook Tolerance of the sun.
	#define CT_int_in   1.25f 	//Cook Tolerance of indoor lighting.
	#define BLOOM_val 0.75h 		//Bloom brightness - Increases HDR brightness of the sky
	#define HDR_int  10.h 		//HDR cutoff - Total HDR brightness

	//For those who can not use Fetch4 or have issues!
	//Not needed on most modern day GPUs.
		//#define NO_F4

#endif