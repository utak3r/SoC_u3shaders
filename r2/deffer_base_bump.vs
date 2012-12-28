#include	"common.h"

p_bumped	main	( v_static I )
{
	float4	w_pos	= I.P				;
	float2 	tc		= unpack_tc_base	(I.tc,I.T.w,I.B.w);	// copy tc
	float 	hemi 	= I.Nh.w			;

	p_bumped 	O;
	float3	Pe	= mul		(m_WV,  w_pos		);
	O.hpos 		= mul		(m_WVP,	w_pos		);
	O.tcdh 		= float4	(tc.xyyy			);
	O.position	= float4	(Pe, hemi			);

#if defined(USE_R2_STATIC_SUN) && !defined(USE_LM_HEMI)
	O.tcdh.w	= I.color.w;					// (r,g,b,dir-occlusion)
#endif
	float3 	N 	= unpack_bx4(I.Nh);	// just scale (assume normal in the -.5f, .5f)
	float3 	T 	= unpack_bx4(I.T);	// 
	float3 	B 	= unpack_bx4(I.B);	// 
	float3x3 xform	= mul	((float3x3)m_WV, float3x3(
						T.x,B.x,N.x,
						T.y,B.y,N.y,
						T.z,B.z,N.z
						));

	O.M1 			= xform[0]; 
	O.M2 			= xform[1]; 
	O.M3 			= xform[2]; 
#ifdef USE_PARALLAX
	O.eye = mul(float3x3(T,B,N),-(w_pos-eye_position));
#endif
#ifdef 	USE_TDETAIL
	O.tcdbump		= O.tcdh * dt_params;		// dt tc
#endif

#ifdef	USE_LM_HEMI
	O.lmh 			= unpack_tc_lmap	(I.lmh);
#endif
	return	O;
}

FXVS;
