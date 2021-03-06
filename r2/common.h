#ifndef        COMMON_H
#define        COMMON_H

#include "shared\common.h"

//#define USE_SJITTER
//#define USE_SUNFILTER
//#define USE_HWSMAP                	//- HW-options defined
//#define USE_FETCH4
#define USE_MBLUR                	//- HW-options defined
//#define USE_SUNMASK                		//- shader defined

#define SSAO					// Enables Screen Space Ambient Occlusion. Best effects can be seen in interiors.
#define SSAO_PASSES int(5)		//Amount of SSAO sample passes.
//#define SSAO_HIGH_QUALITY		//Increases the number of unique samples for each pass to 12 (instead of 6 by default).
#define SAO_DENSITY int(1024)	//Higher values increase sao definitition while reducing its size. No performance cost.  As you increase the number of passes, you'll want to increase density. 1024
#define SSAO_QUALITY 2
#define SUN_QUALITY 2
//#define USE_HWSMAP_PCF

#define BLOOM_val 0.7h 		//Bloom brightness - Increases HDR brightness of the sky

// Shadow resolution:   NOTE:  You will STILL have to use the -smapXXXX switch on your shortcut to the game executable, where XXXX = CUSTOM_SHADOW_RESOLUTION!!!
#define SMAP_size 2048
#define CUSTOM_SHADOW_RESOLUTION int(2048) 	// Use a custom shadow size. Default is 1024 (1024x1024).  You'll notice a significant performance hit at 4096.
#define KERNEL float(1.6) 					// IMPORTANT For custom shadow resolution... Minimum recommended values: 1024 = .6, 2048 = 1.2, 4096 = 2.4, etc... Higher values produce softer shadows.
#define XKERNEL float(1.6) 					// Probably should be same as KERNEL!

#define COLOR_SATURATION float(.6)		//Level of Grey. 0 is all grey (Black and White), 1 is no Grey (Why have this enabled?)
#define COLOR_SAT_SUN_COEF float(.7)		//How much influence sun lighting has on the saturation.
#define CONTRAST_FILTER_COEF float(0.1)	//Level of full screen contrast.
#define SATURATION_FILTER	// Enables Saturation Filter, giving a grey like appearance to areas which are "unsafe".	
#define CONTRAST_FILTER		// Enables Contrast Filter, giving a grey like appearance to areas which are "unsafe".

#define SUN_SHAFTS			// Enables Sun Shafts (god rays) # Only noticable on Full Dynamic Lighting, looks very nice but causes poor performance on some computers. Also makes the environment much brighter and natural.
#define RAY_SAMPLES int(16)	//Amount of sun ray samples. The higher this is, the better the quality and the lower your framerate. 12
#define SS_INTENSITY float(1.1)	// intensity of sun shafts. 1.1
#define SUN_SHAFTS_FIX	//Comment or Uncomment to enable or disable fix for far/near boundary.  Adjust SS_FAR_CONTRAST to hide the seam (it will never be perfect in all possible lighting). (By Daemonjax)
#define SS_NEAR_CONTRAST float(6)	//Amount of contrast during sunray calculation (near). 6
#define SS_FAR_CONTRAST float(5.35)	//Amount of contrast during sunray calculation (far). Adjust to hide near/fear seam when sun shafts are enabled and SUN_SHAFTS_FIX is enabled. 5.35


// #define USE_HWSMAP_PCF				//- nVidia GF3+, R600+

// #define USE_BRANCHING        		//- HW-options defined
// #define USE_VTF                		//- HW-options defined, VertexTextureFetch
// #define FP16_FILTER                	//- HW-options defined
// #define FP16_BLEND                	//- HW-options defined
//
#define USE_PARALLAX                	//- shader defined
//#define USE_STEEPPARALLAX				// don't use
//#define USE_TDETAIL                	// don't use
// #define USE_LM_HEMI                	//- shader defined
//#define USE_DISTORT                	//- shader defined
// #define DBG_TMAPPING
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef SMAP_size
#define SMAP_size        2048
#endif
#define PARALLAX_H 0.02
#define parallax float2(PARALLAX_H, -PARALLAX_H/2)

#ifdef        USE_R2_STATIC_SUN
#  define xmaterial half(1.0h/4.h)
#else
#  define xmaterial half(L_material.w)
#endif
//////////////////////////////////////////////////////////////////////////////////////////
uniform half4                hemi_cube_pos_faces;
uniform half4                hemi_cube_neg_faces;
uniform half4                L_material;                            // 0,0,0,mid
uniform half4                Ldynamic_color;                      // dynamic light color (rgb1)        - spot/point
uniform half4                Ldynamic_pos;                       // dynamic light pos+1/range(w) - spot/point
uniform half4                Ldynamic_dir;                        // dynamic light direction         - sun

uniform half4                J_direct        [6];
uniform half4                J_spot                [6];

half calc_fogging(half4 w_pos)      
{ 
	return dot(w_pos, fog_plane);         
}

half2 calc_detail(half3 w_pos)      
{
	float dtl = distance(w_pos, eye_position)*dt_params.w;
	dtl = min(dtl*dtl, 1);
	half dt_mul = 1 - dtl;
	half dt_add = .5 * dtl;
	return half2(dt_mul, dt_add);
}

float3 calc_reflection(float3 pos_w, float3 norm_w)
{
	return reflect(normalize(pos_w-eye_position), norm_w);
}

float3 calc_sun_r1(float3 norm_w)
{
	return L_sun_color*saturate(dot((norm_w),-L_sun_dir_w));
}

float3 calc_model_hemi_r1(float3 norm_w)
{
	return max(0,norm_w.y)*L_hemi_color;
}

float3 calc_model_lq_lighting(float3 norm_w)
{
	return L_material.x*calc_model_hemi_r1(norm_w) + L_ambient + L_material.y*calc_sun_r1(norm_w);
}

//////////////////////////////////////////////////////////////////////////////////////////
struct         v_static                {
        float4      P                	: POSITION;                        // (float,float,float,1)
        float4      Nh                	: NORMAL;                // (nx,ny,nz,hemi occlusion)
        float4      T                 	: TANGENT;                // tangent
        float4      B                 	: BINORMAL;                // binormal
        float2      tc                	: TEXCOORD0;        // (u,v)
        float2      lmh                	: TEXCOORD1;        // (lmu,lmv)
        float4      color               : COLOR0;                        // (r,g,b,dir-occlusion)
};

struct         v_tree               	{
        float4      P                	: POSITION;                // (float,float,float,1)
        float4      Nh                	: NORMAL;                // (nx,ny,nz)
        float3      T                 	: TANGENT;                // tangent
        float3      B                 	: BINORMAL;                // binormal
        float4      tc                	: TEXCOORD0;        // (u,v,frac,???)
};

struct         v_model                 	{
        float4      P                   : POSITION;                // (float,float,float,1)
        float3      N                	: NORMAL;                // (nx,ny,nz)
        float3      T                	: TANGENT;                // (nx,ny,nz)
        float3      B                	: BINORMAL;                // (nx,ny,nz)
        float2      tc                	: TEXCOORD0;        // (u,v)
};

struct        v_detail                    {
        float4      pos                : POSITION;                // (float,float,float,1)
        int4        misc        : TEXCOORD0;        // (u(Q),v(Q),frac,matrix-id)
};

#ifdef  USE_HWSMAP
struct         v_shadow_direct_aref
{
        float4      hpos:        POSITION;       // Clip-space position         (for rasterization)
        float2      tc0:        TEXCOORD1;       // Diffuse map for aref
};
struct         v_shadow_direct
{
        float4      hpos:        POSITION;       // Clip-space position         (for rasterization)
};
#else
struct         v_shadow_direct_aref
{
        float4      hpos:        POSITION;       // Clip-space position         (for rasterization)
        float       depth:         TEXCOORD0;     // Depth
        float2      tc0:        TEXCOORD1;       // Diffuse map for aref
};
struct         v_shadow_direct
{
        float4      hpos:        POSITION;       // Clip-space position         (for rasterization)
        float       depth:         TEXCOORD0;     // Depth
};


#endif

//////////////////////////////////////////////////////////////////////////////////////////
struct p_bumped        
{
	float4 hpos : POSITION;
#if defined(USE_R2_STATIC_SUN) && !defined(USE_LM_HEMI)
	float4 tcdh : TEXCOORD0;
#else
	float2 tcdh : TEXCOORD0;
#endif
	float4 position : TEXCOORD1;
	half3 M1 : TEXCOORD2;
	half3 M2 : TEXCOORD3;
	half3 M3 : TEXCOORD4;
#if defined(USE_PARALLAX)
	half3 eye : TEXCOORD5;
#ifdef USE_TDETAIL
	float2 tcdbump : TEXCOORD6;
#ifdef USE_LM_HEMI
	float2 lmh : TEXCOORD7;
#endif
#else
#ifdef USE_LM_HEMI
	float2 lmh : TEXCOORD6;
#endif
#endif
#else // USE_PARALLAX
#ifdef USE_TDETAIL
	float2 tcdbump : TEXCOORD5;
#ifdef USE_LM_HEMI
	float2 lmh : TEXCOORD6;
#endif
#else
#ifdef USE_LM_HEMI
	float2 lmh : TEXCOORD5;
#endif
#endif
#endif
};
//////////////////////////////////////////////////////////////////////////////////////////
struct         p_flat                  {
        float4                 hpos        : POSITION;
#if defined(USE_R2_STATIC_SUN) && !defined(USE_LM_HEMI)
    float4                    tcdh        : TEXCOORD0;        // Texture coordinates,         w=sun_occlusion
#else
    float2                    tcdh        : TEXCOORD0;        // Texture coordinates
#endif
        float4                position        : TEXCOORD1;        // position + hemi
        half3                N                : TEXCOORD2;        // Eye-space normal        (for lighting)
  #ifdef USE_TDETAIL
        float2                tcdbump                : TEXCOORD3;        // d-bump
    #ifdef USE_LM_HEMI
        float2         lmh      : TEXCOORD4;        // lm-hemi
    #endif
  #else
    #ifdef USE_LM_HEMI
        float2         lmh      : TEXCOORD3;        // lm-hemi
    #endif
  #endif
};

//////////////////////////////////////////////////////////////////////////////////////////
struct                  f_deffer        		{
        half4           position        		: COLOR0;        // px,py,pz, m-id
        half4           Ne                		: COLOR1;        // nx,ny,nz, hemi
        half4       	C                		: COLOR2;        // r, g, b,  gloss
};
//////////////////////////////////////////////////////////////////////////////////////////
struct  				p_screen                {
        float4          hpos               		: POSITION;
        float2          tc0                		: TEXCOORD0;        // Texture coordinates         (for sampling maps)
};
//////////////////////////////////////////////////////////////////////////////////////////
// Geometry phase / deferring               	//
uniform sampler2D       s_base;             	//
uniform sampler2D       s_bump;             	//
uniform sampler2D       s_bumpX;                //
uniform sampler2D       s_detail;               //
uniform sampler2D       s_detailBump;           //
uniform sampler2D       s_detailBumpX;          //	Error for bump detail
uniform sampler2D       s_bumpD;                //
uniform sampler2D       s_hemi;             	//

uniform sampler2D       s_mask;                	//

uniform sampler2D       s_dt_r;                	//
uniform sampler2D       s_dt_g;                	//
uniform sampler2D       s_dt_b;                	//
uniform sampler2D       s_dt_a;                	//

uniform sampler2D       s_dn_r;                	//
uniform sampler2D       s_dn_g;                	//
uniform sampler2D       s_dn_b;                	//
uniform sampler2D       s_dn_a;                	//

//////////////////////////////////////////////////////////////////////////////////////////
// Lighting/shadowing phase                     //
uniform sampler2D       s_depth;                //
uniform sampler2D       s_position;             //
uniform sampler2D       s_normal;               //
uniform sampler         s_lmap;             	// 2D/cube projector lightmap
uniform sampler3D       s_material;             //
uniform sampler1D       s_attenuate;        	//
//////////////////////////////////////////////////////////////////////////////////////////
// Combine phase                                //
uniform sampler2D       s_diffuse;              // rgb.a = diffuse.gloss
uniform sampler2D       s_accumulator;      	// rgb.a = diffuse.specular
uniform sampler2D       s_generic;              //
uniform sampler2D       s_bloom;                //
uniform sampler         s_image;                // used in various post-processing
uniform sampler2D       s_tonemap;              // actually MidleGray / exp(Lw + eps)
//////////////////////////////////////////////////////////////////////////////////////////
// Defines                                		//
#define def_gloss       half(2.f /255.f)
#define def_aref        half(210.f/255.f)
#define def_dbumph      half(0.34f)
#define def_virtualh    half(0.05f)              // 5cm
#define def_distort     half(0.05f)             // we get -0.5 .. 0.5 range, this is -512 .. 512 for 1024, so scale it
#define def_hdr         half(9.h)         		// hight luminance range half(3.h)
#define def_hdr_clip	half(0.75h)        		//

//////////////////////////////////////////////////////////////////////////////////////////
#define	LUMINANCE_VECTOR                 half3(0.3f, 0.38f, 0.22f)
void        tonemap              (out half4 low, out half4 high, half3 rgb, half scale)
{
        rgb     =      	rgb*scale       ;

		const float fWhiteIntensity = 1.7;

		const float fWhiteIntensitySQR = fWhiteIntensity*fWhiteIntensity;
#ifdef	USE_BRANCHING		// ps_3_0
        //low		=       rgb.xyzz		;

		low		=	( (rgb*(1+rgb/fWhiteIntensitySQR)) / (rgb+1) ).xyzz;

        high	=		low/def_hdr		;        // 8x dynamic range
#else
        low		=       half4           ( ( (rgb*(1+rgb/fWhiteIntensitySQR)) / (rgb+1) ),           0 )	;
        high	=       half4       	(rgb/def_hdr,   0 )	;		// 8x dynamic range
#endif

/*
	rgb		=	rgb*scale;

	low		=	rgb.xyzz;
	high	=	low/def_hdr;	// 8x dynamic range
*/

//		low		= 	half4	(rgb, 0);
//		rgb		/=	def_hdr	;
//		high	= 	half4	(rgb, dot(rgb,0.333f)-def_hdr_clip)		;
}

half4 combine_bloom(half3 low, half4 high)
{
	return half4(low + high*high.a, 1.f);
}

float3	v_hemi        	(float3 n)                        	{        return L_hemi_color*(.5f + .5f*n.y);                   }
float3	v_hemi_wrap     (float3 n, float w)                	{        return L_hemi_color*(w + (1-w)*n.y);                   }
float3	v_sun           (float3 n)                        	{        return L_sun_color*dot(n,-L_sun_dir_w);                }
float3	v_sun_wrap      (float3 n, float w)                	{        return L_sun_color*(w+(1-w)*dot(n,-L_sun_dir_w));      }
half3   p_hemi          (float2 tc)                         {
//        half3        	t_lmh         = tex2D             	(s_hemi, tc);
//        return  dot     (t_lmh,1.h/4.h);
        half4        	t_lmh         = tex2D             	(s_hemi, tc);
        return t_lmh.a;
}

half   get_hemi( half4 lmh)
{
	return lmh.a;
}

half   get_sun( half4 lmh)
{
	return lmh.g;
}

//	contrast function
half Contrast(half Input, half ContrastPower)
{
     //piecewise contrast function
     bool IsAboveHalf = Input > 0.5 ;
     half ToRaise = saturate(2*(IsAboveHalf ? 1-Input : Input));
     half Output = 0.5*pow(ToRaise, ContrastPower);
     Output = IsAboveHalf ? 1-Output : Output;
     return Output;
}

// this SSAO calculation algorithm is based on 
// Meltac's and Daemonjax's ideas. Kudos to them for
// this great piece of code :)
half calc_ssao_new (half3 P, half3 N, half2 tc)
{
	#ifndef SSAO
		return 1.h;
	#else
		
		half2 scale = half2(.5f / SAO_DENSITY, .67f / SAO_DENSITY)*(150/max(P.z,1.3));
		
		half 	occ	 = 0.0h;	
		half num_dir = 0.0h;
		
		#ifndef SSAO_NOLOOP
		for (int a = 1; a < SSAO_PASSES; ++a)
		#else
			int a = 1;
		#endif
		{
			half2	scale_tmp = scale*a;		
			half3 	dir 	= tex2D	(s_position,tc + half2(-0.416212f, -0.665810f)*scale_tmp)-P.xyz;
			half 	occ_factor = saturate(length(dir));
			half 	infl 	= saturate(dot(normalize(dir), N.xyz));
			
			occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
			num_dir += (infl+0.01)/(occ_factor+0.1);		

			
			dir 	= tex2D	(s_position,tc +half2(0.432340f,-0.093580f)*scale_tmp)-P.xyz;
			occ_factor = saturate(length(dir));
			infl 	= saturate(dot(normalize(dir), N.xyz));			
			occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
			num_dir += (infl+0.01)/(occ_factor+0.1);
			
			dir 	= tex2D	(s_position,tc +half2(-0.455914f,0.647137f)*scale_tmp)-P.xyz;
			occ_factor = saturate(length(dir));
			infl 	= saturate(dot(normalize(dir), N.xyz));			
			occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
			num_dir += (infl+0.01)/(occ_factor+0.1);
			
			dir 	= tex2D	(s_position,tc +half2(0.479456f,0.627022f)*scale_tmp)-P.xyz;
			occ_factor = saturate(length(dir));
			infl 	= saturate(dot(normalize(dir), N.xyz));			
			occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
			num_dir += (infl+0.01)/(occ_factor+0.1);
						
			dir 	= tex2D	(s_position,tc +half2(-0.492340f,0.090983f)*scale_tmp)-P.xyz;
			occ_factor = saturate(length(dir));
			infl 	= saturate(dot(normalize(dir), N.xyz));			
			occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
			num_dir += (infl+0.01)/(occ_factor+0.1);
			
			dir 	= tex2D	(s_position,tc +half2(0.413434f,-0.680026f)*scale_tmp)-P.xyz;
			occ_factor = saturate(length(dir));
			infl 	= saturate(dot(normalize(dir), N.xyz));			
			occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
			num_dir += (infl+0.01)/(occ_factor+0.1);
						
			#ifdef SSAO_HIGH_QUALITY
			
				dir 	= tex2D	(s_position,tc +half2(0.726212f, 0.305810f)*scale_tmp)-P.xyz;
				occ_factor = saturate(length(dir));
				infl 	= saturate(dot(normalize(dir), N.xyz));			
				occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
				num_dir += (infl+0.01)/(occ_factor+0.1);
				
				dir 	= tex2D	(s_position,tc +half2(-0.742340f,0.173580f)*scale_tmp)-P.xyz;
				occ_factor = saturate(length(dir));
				infl 	= saturate(dot(normalize(dir), N.xyz));			
				occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
				num_dir += (infl+0.01)/(occ_factor+0.1);
				
				dir 	= tex2D	(s_position,tc +half2(0.395914f,-0.757137f)*scale_tmp)-P.xyz;
				occ_factor = saturate(length(dir));
				infl 	= saturate(dot(normalize(dir), N.xyz));			
				occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
				num_dir += (infl+0.01)/(occ_factor+0.1);
				
				dir 	= tex2D	(s_position,tc +half2(-0.719456f,-0.367022f)*scale_tmp)-P.xyz;
				occ_factor = saturate(length(dir));
				infl 	= saturate(dot(normalize(dir), N.xyz));			
				occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
				num_dir += (infl+0.01)/(occ_factor+0.1);
							
				dir 	= tex2D	(s_position,tc +half2(0.772340f,-0.094983f)*scale_tmp)-P.xyz;
				occ_factor = saturate(length(dir));
				infl 	= saturate(dot(normalize(dir), N.xyz));			
				occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
				num_dir += (infl+0.01)/(occ_factor+0.1);
				
				dir 	= tex2D	(s_position,tc +half2(-0.373434f,0.780026f)*scale_tmp)-P.xyz;
				occ_factor = saturate(length(dir));
				infl 	= saturate(dot(normalize(dir), N.xyz));			
				occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
				num_dir += (infl+0.01)/(occ_factor+0.1);
				

			#endif
		}
		return (occ / num_dir);
	#endif
}

#define FXPS technique _render{pass _code{PixelShader=compile ps_3_0 main();}}
#define FXVS technique _render{pass _code{VertexShader=compile vs_3_0 main();}}

#endif
