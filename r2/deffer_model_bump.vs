#include	"common.h"
#include	"skin.h"

#if defined(USE_PARALLAX) || defined(USE_STEEPPARALLAX)
	uniform half3x4	    m_invW;
#endif 	//	defined(USE_PARALLAX) || defined(USE_STEEPPARALLAX)

p_bumped	_main	( v_model I )
{
	float4	w_pos	= I.P;

	p_bumped 	O;
	O.hpos 		= mul(m_WVP,	w_pos);
	float2 	tc 	= I.tc;
	float3	Pe	= mul(m_WV,  w_pos);
	O.tcdh 		= float4(tc.xyyy);

	O.position = float4(Pe, L_material.x);

#if defined(USE_R2_STATIC_SUN) && !defined(USE_LM_HEMI)
	O.tcdh.w	= L_material.y;					// (,,,dir-occlusion)
#endif
	float3 	N 	= I.N;		// just scale (assume normal in the -.5f, .5f)
	float3 	T 	= I.T;		// 
	float3 	B 	= I.B;		// 
	float3x3 xform	= mul	((float3x3)m_WV, float3x3(
						2*T.x,2*B.x,2*N.x,
						2*T.y,2*B.y,2*N.y,
						2*T.z,2*B.z,2*N.z
				));

	O.M1 			= xform	[0]; 
	O.M2 			= xform	[1]; 
	O.M3 			= xform	[2]; 

#ifdef USE_PARALLAX
	O.eye = mul(float3x3(T,B,N),-(mul(m_W,w_pos)-eye_position));
#endif
#ifdef 	USE_TDETAIL
	O.tcdbump		= O.tcdh * dt_params;		// dt tc
#endif

	return	O;
}

/////////////////////////////////////////////////////////////////////////
#ifdef 	SKIN_NONE
p_bumped	main(v_model v) 		{ return _main(v); 		}
#endif

#ifdef 	SKIN_0
p_bumped	main(v_model_skinned_0 v) 	{ return _main(skinning_0(v)); }
#endif

#ifdef	SKIN_1
p_bumped	main(v_model_skinned_1 v) 	{ return _main(skinning_1(v)); }
#endif

#ifdef	SKIN_2
p_bumped	main(v_model_skinned_2 v) 	{ return _main(skinning_2(v)); }
#endif

#ifdef	SKIN_3
p_bumped	main(v_model_skinned_3 v) 	{ return _main(skinning_3(v)); }
#endif

#ifdef	SKIN_4
p_bumped	main(v_model_skinned_4 v) 	{ return _main(skinning_4(v)); }
#endif

FXVS;
