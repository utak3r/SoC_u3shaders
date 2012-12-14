#ifndef        COMMON_H
#define        COMMON_H

// #define USE_SUPER_SPECULAR
#include "options.h"
#include "shared\common.h"
//////////////////////////////////////////////////////////////////////////////////////////
// *** options

// #define USE_GAMMA_22
#define USE_SJITTER
// #define USE_SUNFILTER
//
// #define USE_MBLUR                	//- HW-options defined
// #define USE_HWSMAP                	//- HW-options defined

#define USE_HWSMAP_PCF				//- nVidia GF3+, R600+
#define SUN_SHAFTS_QUALITY	3

#define USE_BRANCHING        		//- HW-options defined
// #define USE_VTF                		//- HW-options defined, VertexTextureFetch
// #define FP16_FILTER                	//- HW-options defined
// #define FP16_BLEND                	//- HW-options defined
//
#define USE_PARALLAX                	//- shader defined
// #define USE_TDETAIL                	//- shader defined
// #define USE_LM_HEMI                	//- shader defined
// #define USE_DISTORT                	//- shader defined
// #define USE_SUNMASK                		//- shader defined
//#define DBG_TMAPPING

const int shadowRes = SHADOW_RESOLUTION;

//////////////////////////////////////////////////////////////////////////////////////////
#ifndef SMAP_size
	#ifdef USE_CUSTOM_SHADOW_RESOLUTION
		#define SMAP_size	shadowRes
	#else
		#define SMAP_size	1024
	#endif
#endif
#define PARALLAX_H 0.02
#define parallax float2(PARALLAX_H, -PARALLAX_H/2)

#ifdef        USE_R2_STATIC_SUN
#  define xmaterial half(1.0h/4.h)
#else
#  define xmaterial half(L_material.w)
#endif
//////////////////////////////////////////////////////////////////////////////////////////
uniform half4                L_material;                            // 0,0,0,mid
uniform half4                Ldynamic_color;                      // dynamic light color (rgb1)        - spot/point
uniform half4                Ldynamic_pos;                       // dynamic light pos+1/range(w) - spot/point
uniform half4                Ldynamic_dir;                        // dynamic light direction         - sun

uniform half4                J_direct        [6];
uniform half4                J_spot                [6];
	#ifndef USE_F32_FOG
half          calc_fogging               (half4 w_pos)      { return dot(w_pos,fog_plane);         }
	#else
half          calc_fogging               (half4 w_pos)      { return dot(w_pos,fog_plane*1.125f);
	#endif
half2         calc_detail                (half3 w_pos)      {
        float                 dtl        = distance                (w_pos,eye_position)*dt_params.w;
                              dtl        = min              (dtl*dtl, 1);
        half                  dt_mul     = 1  - dtl;        // dt*  [1 ..  0 ]
        half                  dt_add     = .5 * dtl;        // dt+  [0 .. 0.5]
        return                half2      (dt_mul,dt_add);
}
float3         calc_reflection     (float3 pos_w, float3 norm_w)
{
    return reflect(normalize(pos_w-eye_position), norm_w);
}

float3        calc_sun_r1                (float3 norm_w)    { return L_sun_color*saturate(dot((norm_w),-L_sun_dir_w));                 }
float3        calc_model_hemi_r1         (float3 norm_w)    { return max(0,norm_w.y)*L_hemi_color;                                         }
float3        calc_model_lq_lighting     (float3 norm_w)    { return L_material.x*calc_model_hemi_r1(norm_w) + L_ambient + L_material.y*calc_sun_r1(norm_w);         }

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
struct         p_bumped        {
        float4      hpos        : POSITION;
#if defined(USE_R2_STATIC_SUN) && !defined(USE_LM_HEMI)
        float4            tcdh        : TEXCOORD0;        // Texture coordinates,         w=sun_occlusion
#else
        float2            tcdh        : TEXCOORD0;        // Texture coordinates
#endif
        float4      position        : TEXCOORD1;        // position + hemi
        half3       M1                : TEXCOORD2;        // nmap 2 eye - 1
        half3       M2                : TEXCOORD3;        // nmap 2 eye - 2
        half3       M3                : TEXCOORD4;        // nmap 2 eye - 3
#if defined(USE_PARALLAX) || defined(USE_Sky4CE_PARALLAXOCCLUSION)
        half3       eye                : TEXCOORD5;        // vector to point in tangent space
  #ifdef USE_TDETAIL
        float2      tcdbump     : TEXCOORD6;        // d-bump
    #ifdef USE_LM_HEMI
        float2      lmh                    : TEXCOORD7;        // lm-hemi
    #endif
  #else
    #ifdef USE_LM_HEMI
        float2      lmh                   : TEXCOORD6;        // lm-hemi
    #endif
  #endif
#else
  #ifdef USE_TDETAIL
        float2      tcdbump          : TEXCOORD5;        // d-bump
    #ifdef USE_LM_HEMI
        float2      lmh                    : TEXCOORD6;        // lm-hemi
    #endif
  #else
    #ifdef USE_LM_HEMI
        float2      lmh                   : TEXCOORD5;        // lm-hemi
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
float get_val2(){
return 0;}
const float4 ePq = float(1);
const float get_val() { float rPq; rPq = ePq; half fu = get_val2();  return rPq;}  
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
#define def_aref        half(200.f/255.f)
#define def_dbumph      half(0.333f)
#define def_virtualh    half(.05f)              // 5cm
#define def_distort     half(0.05f)             // we get -0.5 .. 0.5 range, this is -512 .. 512 for 1024, so scale it
	#ifndef USE_F32
#define def_hdr         half(8.h)         		// hight luminance range half(3.h)
	#else
#define def_hdr         half(HDR_int)
	#endif
#define def_hdr_clip	half(0.75h)        		//

//////////////////////////////////////////////////////////////////////////////////////////
#define	LUMINANCE_VECTOR                 half3(0.3f, 0.48f, 0.22f)
void        tonemap              (out half4 low, out half4 high, half3 rgb, half scale)
{
        rgb     =      	rgb*scale       ;
#ifdef	USE_BRANCHING		// ps_3_0
        low		=       rgb.xyzz		;
        high	=		low/def_hdr		;        // 8x dynamic range
#else
        low		=       half4           (rgb,           0 )	;
        high	=       half4       	(rgb/def_hdr,   0 )	;		// 8x dynamic range
#endif

//		low		= 	half4	(rgb, 0);
//		rgb		/=	def_hdr	;
//		high	= 	half4	(rgb, dot(rgb,0.333f)-def_hdr_clip)		;
}
half4		combine_bloom        (half3  low, half4 high)	{
        return        half4(low + high*high.a, 1.f);
}

float3	v_hemi        	(float3 n)                        	{        return L_hemi_color*(.5f + .5f*n.y);                   }
float3	v_hemi_wrap     (float3 n, float w)                	{        return L_hemi_color*(w + (1-w)*n.y);                   }
float3	v_sun           (float3 n)                        	{        return L_sun_color*dot(n,-L_sun_dir_w);                }
float3	v_sun_wrap      (float3 n, float w)                	{        return L_sun_color*(w+(1-w)*dot(n,-L_sun_dir_w));      }
half3   p_hemi          (float2 tc)                         {
        half3        	t_lmh         = tex2D             	(s_hemi, tc);
        return  dot     (t_lmh,1.h/3.h);
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$//
//--- © The below code is copyright of Sky4CE ---//
//----------- mailto: Sky4CE@inbox.ru -----------//
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$//

float2 Parallax(float3 eye,float2 tc,sampler2D s_base)
{
	half height	= tex2D(s_base, tc).w * 0.016 - 0.009;    
    return tc + height*normalize(eye);
}
float2 Sky4CE_ParallaxOcclusion(float3 eye,float2 tc,sampler2D s_base)
{
	const int maxSamples = MAX_SAMPLES;
  	const int minSamples = MIN_SAMPLES;
	float fParallaxOffset = PARALLAX_OFFSET;
 	int nNumSteps = lerp( maxSamples, minSamples, normalize(eye).z );
  	float2 vDelta = -normalize(eye).xy * fParallaxOffset;
  	#ifdef CORRECT_PERSPECTIVE
  	vDelta /= normalize(eye).z;
  	#endif
   	float fStepSize = 1.0 / nNumSteps;
   	float2 vTexOffsetPerStep = fStepSize * vDelta;
   	double2 vTexCurrentOffset = tc;
   	float fCurrHeight = 0;
   	float  fCurrentBound = 1.0;
   	float Q1 = CONTRAST_COEF_Q1;
   	float Q2 = CONTRAST_COEF_Q2;
   	float Brightness = BRIGHTNESS_COEF;
   	for(;fCurrHeight < fCurrentBound;fCurrentBound -= fStepSize)
   	{
   		vTexCurrentOffset.xy += vTexOffsetPerStep;
   		#ifndef USE_TEXTURE_PACK
    	fCurrHeight = tex2Dlod( s_base, float4(vTexCurrentOffset.xy,0,0) ).a+Brightness;
    	if (fCurrHeight > Q2) fCurrHeight  = 1.0f;
    	else if (fCurrHeight < Q1) fCurrHeight = 0.0f;
    	else fCurrHeight = (fCurrHeight - Q1)/(Q2-Q1);
    	#else
    	fCurrHeight = tex2Dlod( s_base, float4(vTexCurrentOffset.xy,0,0) ).a;
    	#endif
    }
    float4 offsetBest = float4(vTexCurrentOffset,0,0);
    vTexCurrentOffset.xy -= vTexOffsetPerStep;
    #ifndef USE_TEXTURE_PACK
   	float fPrevHeight = tex2Dlod( s_base, float4(vTexCurrentOffset.xy,0,0) ).a+Brightness;
   	if (fPrevHeight > Q2) fPrevHeight = 1.0f;
  	else if (fPrevHeight < Q1) fPrevHeight = 0.0f;
  	else fPrevHeight = (fPrevHeight - Q1)/(Q2-Q1);
  	#else
  	float fPrevHeight = tex2Dlod( s_base, float4(vTexCurrentOffset.xy,0,0) ).a;
  	#endif
    float error = 1.0;
    float t1 = fCurrentBound ;
    float t0 = t1 + fStepSize;
    float delta1 = t1 - fCurrHeight;
    float delta0 = t0 - fPrevHeight;
    float4 intersect = float4(vDelta, vDelta + tc);
    for (int i=0; i<FINAL_INTERSECTION_LOOPS && abs(error) > 0.01; i++)
    { 
      float denom = (delta1 - delta0);
      float t = (t0 * delta1 - t1 * delta0) / denom;
      offsetBest.xy = -t * intersect.xy + intersect.zw;
      #ifndef USE_TEXTURE_PACK
      float NB = tex2Dlod(s_base, offsetBest).a+Brightness;
      if (NB > Q2) NB = 1.0f;
  	  else if (NB < Q1) NB = 0.0f;
  	  else NB = (NB - Q1)/(Q2-Q1);
  	  #else
  	  float NB = tex2Dlod(s_base, offsetBest).a;
  	  #endif
      error = t - NB;
      if (error < 0)
      {
         delta1 = error;
         t1 = t;
      }
      else
      {
         delta0 = error;
         t0 = t;
      }
    }
    return offsetBest.xy;
}

static const half2 poisson_disc12[12] = 
{
	half2(-0.326212f , -0.405810f),
	half2(-0.840144f , -0.073580f),
	half2(-0.695914f ,  0.457137f),
	half2(-0.203345f ,  0.620716f),
	half2( 0.962340f , -0.194983f),
	half2( 0.473434f , -0.480026f),
	half2( 0.519456f ,  0.767022f),
	half2( 0.185461f , -0.893124f),
	half2( 0.507431f ,  0.064425f),
	half2( 0.896420f ,  0.412458f),
	half2(-0.321940f , -0.932615f),
	half2(-0.791559f , -0.597710f)
};

half	calc_ssao( half3 P, half3 N, half2 tc)
{
	half2 	scale 	= half2	(.5f / 1024.h, .5f / 768.h)*150/max(P.z,1.3);
	half occ = 0;
	half num_dir = 0;
	float c = 1;
	if(P.z<FADE_DIST) c = FADE_COEF + ((1-FADE_COEF)*P.z)/FADE_DIST;
for (int a=1; a<SSAO_QUALITY; ++a)
{
	half2	scale_tmp = scale*a;
	for (int i=0; i<12; i++)
	{
		float2 	tap 	= tc + poisson_disc12[i]*scale_tmp;
		half3	tap_pos	= tex2D	(s_position,tap);
		half3 	dir 	= tap_pos-P.xyz;
		half	dist	= length(dir);
		dir 	= normalize(dir);
		half 	infl 	= clamp(dot( dir, N.xyz),0,c);
		half 	occ_factor = saturate(dist);
		occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
		num_dir += (infl+0.01)/(occ_factor+0.1);
	}
}
	occ /= num_dir;

	return occ;
} 

#define FXPS technique _render{pass _code{PixelShader=compile ps_3_0 main();}}
#define FXVS technique _render{pass _code{VertexShader=compile vs_3_0 main();}}

#endif
