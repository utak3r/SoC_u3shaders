#include "common.h"

uniform float3x4	m_xform		;
uniform float3x4	m_xform_v	;
uniform float4 		consts; 	// {1/quant,1/quant,???,???}
uniform float4 		c_scale,c_bias,wind,wave;
uniform float2 		c_sun;		// x=*, y=+

p_bumped 	main 	(v_tree I)
{
	float3 	pos		= mul			(m_xform, I.P);

	float 	base 	= m_xform._24	;		// take base height from matrix
	float 	dp		= calc_cyclic  	(wave.w+dot(pos,(float3)wave));
	float 	H 		= pos.y - base	;		// height of vertex (scaled, rotated, etc.)
	float 	frac 	= I.tc.z*consts.x;		// fractional (or rigidity)
	float 	inten 	= H * dp;				// intensity
	float2 	result	= calc_xz_wave	(wind.xz*inten, frac);
#ifdef		USE_TREEWAVE
	result	= 0;
#endif
	float4 	w_pos 	= float4(pos.x+result.x, pos.y, pos.z+result.y, 1);
	float2 	tc 		= (I.tc * consts).xy;
	float 	hemi 	= I.Nh.w * c_scale.w + c_bias.w;

	p_bumped 		O;
	float3	Pe		= mul		(m_V,  	w_pos		);
	O.tcdh 			= float4	(tc.xyyy			);
	O.hpos 			= mul		(m_VP,	w_pos		);
	O.position		= float4	(Pe, 	hemi		);

#if defined(USE_R2_STATIC_SUN) && !defined(USE_LM_HEMI)
	float 	suno 	= I.Nh.w * c_sun.x + c_sun.y	;
	O.tcdh.w		= suno;					// (,,,dir-occlusion)
#endif

	float3 	N 		= unpack_bx4(I.Nh);	// just scale (assume normal in the -.5f, .5f)
	float3 	T 		= unpack_bx4(I.T);	//
	float3 	B 		= unpack_bx4(I.B);	//
	float3x3 xform	= mul	((float3x3)m_xform_v, float3x3(
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

	return O;
}
FXVS;
