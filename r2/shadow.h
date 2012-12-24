#ifndef SHADOW_H
#define SHADOW_H
#include "common.h"

uniform sampler s_smap:register(ps,s0);
uniform sampler2D jitter0;
uniform sampler2D jitter1;

half sample_sw(float2 tc,float2 shift,float depth_cmp) 
{
	static const float ts=KERNEL/float(SMAP_size);
	tc+=shift*ts;
	float texsize=SMAP_size;
	float offset=0.5f/texsize;
	float2 Tex00=tc+float2(-offset,-offset);
	float2 Tex01=tc+float2(-offset,offset);
	float2 Tex10=tc+float2(offset,-offset);
	float2 Tex11=tc+float2(offset,offset);
	float4 depth=float4(
	depth_cmp-tex2D(s_smap,Tex00).x,
	depth_cmp-tex2D(s_smap,Tex01).x,
	depth_cmp-tex2D(s_smap,Tex10).x,
	depth_cmp-tex2D(s_smap,Tex11).x);
	half4 compare=step(depth,0);
	float2 fr=frac(Tex00*texsize);
	half2 ifr=half2(1,1)-fr;
	half4 fr4=half4(ifr.x*ifr.y,ifr.x*fr.y,fr.x*ifr.y,fr.x*fr.y);
	return dot (compare,fr4);
}


half shadow_sw(float4 tc) 
{ 
	float2 tc_dw=tc.xy/tc.w;
	half4 s;
	s.x=sample_sw(tc_dw,float2(-1,-1),tc.z);
	s.y=sample_sw(tc_dw,float2(+1,-1),tc.z);
	s.z=sample_sw(tc_dw,float2(-1,+1),tc.z);
	s.w=sample_sw(tc_dw,float2(+1,+1),tc.z);
	return dot(s,1.h/4.h);
}


half sample_hw_pcf (float4 tc,float4 shift)
{
	const float ts=KERNEL/float(SMAP_size);
	return tex2Dproj(s_smap, tc + tc.w * shift * ts).x;			
}
		
half shadow_hw(float4 tc)
{	
	#ifdef USE_SHADOW_16
		return (sample_hw_pcf(tc, float4( 1,  1, 0, 0)) + sample_hw_pcf(tc, float4( -1, -1, 0, 0)) +
				sample_hw_pcf(tc, float4( 1, -1, 0, 0)) + sample_hw_pcf(tc, float4( -1,  1, 0, 0)) + 							
				sample_hw_pcf(tc, float4( 0,  1, 0, 0)) + sample_hw_pcf(tc, float4(  1,  0, 0, 0)) + 
				sample_hw_pcf(tc, float4( 0, -1, 0, 0)) + sample_hw_pcf(tc, float4( -1,  0, 0, 0)) + 						
				sample_hw_pcf(tc, float4( 0,  2, 0, 0)) + sample_hw_pcf(tc, float4(  2,  0, 0, 0)) + 	
				sample_hw_pcf(tc, float4( 0, -2, 0, 0)) + sample_hw_pcf(tc, float4( -2,  0, 0, 0)) +								
				sample_hw_pcf(tc, float4( 2,  2, 0, 0)) + sample_hw_pcf(tc, float4( -2, -2, 0, 0)) +
				sample_hw_pcf(tc, float4( 2, -2, 0, 0)) + sample_hw_pcf(tc, float4( -2,  2, 0, 0))) * (1.h/16.h);
	#else
		return (sample_hw_pcf(tc, float4( 0,  1, 0, 0)) + sample_hw_pcf(tc, float4(  1, 0, 0, 0)) + 
				sample_hw_pcf(tc, float4( 0, -1, 0, 0)) + sample_hw_pcf(tc, float4( -1, 0, 0, 0))) * (1.h/4.h);
	#endif			
} 


half sample_hw_f4(float4 tc,float4 shift)
{
	static const float ts=KERNEL/float(SMAP_size);
	float4 T4=tc/tc.w;
	T4.xy+=shift.xy*ts;
	float4 D4=tex2D(s_smap,T4);
	float4 compare=T4.z<D4;
	float texsize=SMAP_size;
	float2 fr=frac(T4.xy*texsize);
	half2 ifr=half2(1,1)-fr;
	half4 fr4=half4(ifr.x*ifr.y,ifr.x*fr.y,fr.x*ifr.y,fr.x*fr.y);
	half4 fr4s=fr4.zywx;
	return dot(compare,fr4s);
}


half shadow_hw_f4(float4 tc) 
{
	half s0=sample_hw_f4(tc,float4(-1,-1,0,0));
	half s1=sample_hw_f4(tc,float4(+1,-1,0,0));
	half s2=sample_hw_f4(tc,float4(-1,+1,0,0));
	half s3=sample_hw_f4(tc,float4(+1,+1,0,0));
	return (s0+s1+s2+s3)/4.h;
}


#ifdef USE_HWSMAP_PCF
	half shadow(float4 tc) { return shadow_hw (tc);}
#else
	#ifdef USE_FETCH4
		half shadow(float4 tc) { return shadow_hw_f4(tc);}
	#else
		half shadow(float4 tc) { return shadow_sw (tc);}
	#endif
#endif

half shadowtest(float4 tc, float4 tcJ) 
{
	const half4 jitter = tex2Dproj(jitter0, tcJ) * tc.w *(KERNEL/half(SMAP_size));			
			
		return (tex2Dproj(s_smap, float4(tc.xy + jitter.xy, tc.zw)).x  + 
				tex2Dproj(s_smap, float4(tc.xy + jitter.zw, tc.zw)).x  +					
				tex2Dproj(s_smap, float4(tc.xy - jitter.xy, tc.zw)).x  +
				tex2Dproj(s_smap, float4(tc.xy - jitter.zw, tc.zw)).x) * (1/4.h);
}

#ifdef HIGH_QUALITY_JITTER	
	half shadowtest_sun(float4 tc, float4 tcJ) 
	{													
		half shadow = 0;					
		const float offset = 1.f + (6 / float(JITTER_SAMPLES));
		for (int i = JITTER_SAMPLES / 8; i > 0; --i) 
		{										
			half4 jitter = tex2D(jitter0, tcJ.xy) * tc.w * (KERNEL/half(SMAP_size));
			
			shadow += tex2Dproj(s_smap, float4(tc.xy + jitter.xy, tc.zw)).x +							  
					  tex2Dproj(s_smap, float4(tc.xy - jitter.xy, tc.zw)).x +
					  tex2Dproj(s_smap, float4(tc.xy + jitter.wz, tc.zw)).x +							  
					  tex2Dproj(s_smap, float4(tc.xy - jitter.wz, tc.zw)).x +							  
					  tex2Dproj(s_smap, float4(tc.xy + jitter.yz, tc.zw)).x +							  
					  tex2Dproj(s_smap, float4(tc.xy - jitter.yz, tc.zw)).x +
					  tex2Dproj(s_smap, float4(tc.xy + jitter.zx, tc.zw)).x +							  
					  tex2Dproj(s_smap, float4(tc.xy - jitter.zx, tc.zw)).x;			  
					  
			tcJ.xy *= offset + (jitter.x * (4 / float(JITTER_SAMPLES)));
		}				
		return shadow * (1 / half(JITTER_SAMPLES));																	
	}
#else
	half shadowtest_sun(float4 tc, float4 tcJ) 
	{				
		const half4 jitter = tex2D(jitter0, tcJ) * tc.w *(KERNEL/half(SMAP_size));			
			
		return (tex2Dproj(s_smap, float4(tc.xy + jitter.xy, tc.zw)).x  + 
				tex2Dproj(s_smap, float4(tc.xy + jitter.zw, tc.zw)).x  +					
				tex2Dproj(s_smap, float4(tc.xy - jitter.xy, tc.zw)).x  +
				tex2Dproj(s_smap, float4(tc.xy - jitter.zw, tc.zw)).x) * (1/4.h);
	}
#endif

#ifdef USE_SUNMASK 
	uniform float3x4 m_sunmask;

	half sunmask(float4 P) 
	{
		const float2 tc=mul(m_sunmask,P);
		return tex2D(s_lmap,tc).w;
	}

#else
	half sunmask (float4 P) { return 1.h;}
#endif

uniform float4x4 m_shadow;
#endif