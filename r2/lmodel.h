#ifndef	LMODEL_H
#define LMODEL_H

#include "common.h"
	#ifndef USE_F32
//////////////////////////////////////////////////////////////////////////////////////////
// Lighting formulas			// 
half4 	plight_infinity		(half m, half3 point, half3 normal, half3 light_direction)       				{
  half3 N		= normal;							// normal 
  half3 V 		= -normalize	(point);					// vector2eye
  half3 L 		= -light_direction;						// vector2light
  half3 H		= normalize	(L+V);						// half-angle-vector  
  return  tex3D 		(s_material,	half3( dot(L,N), dot(H,N), m ) );		// sample material
}
half4 	plight_infinity2	(half m, half3 point, half3 normal, half3 light_direction)       				{
  	half3 N		= normal;							// normal 
  	half3 V 	= -normalize		(point);		// vector2eye
  	half3 L 	= -light_direction;					// vector2light
 	half3 H		= normalize			(L+V);			// half-angle-vector 
	half3 R     = reflect         	(-V,N);
	half 	s	= saturate(dot(L,R));
			s	= saturate(dot(H,N));
	half 	f 	= saturate(dot(-V,R));
			s  *= f;
	half4	r	= tex3D 			(s_material,	half3( dot(L,N), s, m ) );	// sample material
			r.w	= pow(saturate(s),4);
  	return	r	;
}
half4 	plight_local		(half m, half3 point, half3 normal, half3 light_position, half light_range_rsq, out float rsqr)  {
  half3 N		= normal;							// normal 
  half3 L2P 	= point-light_position;                         		// light2point 
  half3 V 		= -normalize	(point);					// vector2eye
  half3 L 		= -normalize	((half3)L2P);					// vector2light
  half3 H		= normalize	(L+V);						// half-angle-vector
		rsqr	= dot		(L2P,L2P);					// distance 2 light (squared)
  half  att 	= saturate	(1 - rsqr*light_range_rsq);			// q-linear attenuate
  half4 light	= tex3D		(s_material, half3( dot(L,N), dot(H,N), m ) ); 	// sample material
  return att*light;
}
	#else

////////////////////////////////////////////////////////////////////////////////////////
//	Created by:
//	 (Cameron Sneed - http:\\www.thefloatingpoint.org ).
//    This algorithm and code is copyright of Cameron Sneed. It is not to be used
//    for any comercial nor private purposes. You must contact the owner before use. 
//
////////////////////////////////////////////////////////////////////////////////////////
//
//	© The below code is copyright of Cameron Sneed. All rights reserved. 
////////////////////////////////////////////////////////////////////////////////////////

const float cPi = 3.14159265;
const float4 ePi = float4(3.14159e-00,2.65358e-06,9.79323e-12,8.46264e-18);       
const float getPi_dp24() {float rPi; rPi = ePi[3] + ePi[2] +  ePi[1] + ePi[0] ;return rPi;} 

half4 plight_infinity	(half m, half3 point, half3 normal, half4 light_dir)       				
   {

	half3 T		= tan(normal);
	half3 N		= normalize(normal);					 
  	half3 V 	= -normalize		(point);		
  	half3 L 	= -light_dir.xyz;				
	half3 tx	= normalize(T);
	float3	H	= normalize(L+V);
	half   CP	= get_val();
	float	NX	= saturate(dot(N,L));
	float4  LV 	= ((saturate(dot(N,H))),(saturate(dot(V,H))),		
		    	  (saturate(dot(N,V))),(saturate(dot(L,N))));
	float3	P	= normalize(H - LV.x * N);
	float   TP	= dot (tx,P);
	float	NH2	= (LV.x * LV.x);
const float	RI	= 0.01f;
const float	Pi	= 3.14;
	float	m2	= m * m;
	half	cx	= get_val2();
	float4	vD	= ((RI + (1 - RI) * pow((1 - LV.z), 5)),
			  (1 / m2 * NH2 * NH2) * (exp(-((1 - NH2) / (m2 * NH2)))),
			  min(1, min((2 * LV.x * LV.w) / LV.y, (2 * LV.x * LV.z) / LV.y)),
			  (0.5 * exp (((TP / 0.65f) + ((1 - TP) / 0.3f)) * (NX - 1) / NX)));
	float2	Si	= ((vD.x * vD.y * vD.z) / (Pi * LV.w * LV.z),1);
	float	ret	= (CT_int * (vD.w*LV.w) + (LV.w*LV.x) + (LV.w * saturate(Si.y * ((0.4f * LV.w) + (0.6f * Si.x)))) * -light_dir.w);
	return (ret);}
uniform half pre_alpha;
uniform half this_is_not_supposed_to_be_here;	
half4 	plight_local		(half m, half3 point, half3 normal, half4 light_position, half light_range_rsq, out float rsqr)  {
  half3 N		= normalize (normal);							
  half3 L2P 	= point-light_position.xyz;                
  half3 V 		= -normalize	(point.xyz);					
  half3 L 		= -normalize	((half3)L2P);					
const float	RI	= 0.01f;
const float	Pi	= getPi_dp24();			
	half3	H	= normalize(L+V);
	float4 LV = ((saturate(dot(N,H))),(saturate(dot(V,H))),
				(saturate(dot(N,V))),(saturate(dot(L,N))));
	float	NH2	= LV.x * LV.x;
	float	m2	= m * m;
	float3	vD	= ((RI + (1 - RI) * pow((1 - LV.z), 5.f)),
			  (1 / m2 * NH2 * NH2) * (exp(-((1 - NH2) / (m2 * NH2)))),
			  min(1.f, min((2 * LV.x * LV.w) / LV.y, (2 * LV.x * LV.z) / LV.y)));
	float2	Si	= ((vD.x * vD.y * vD.z) / (Pi * LV.w * LV.z),1.0f);
	float	ret	= CT_int_in * (LV.w*LV.x) + (LV.w * saturate(Si.y * ((m2 * LV.w) + (m2 * Si)))) * -light_position.w;
	rsqr	= dot		(L2P,L2P);					
	  half  att 	= saturate	(1 - rsqr*light_range_rsq);			
	  	 return (att)*(ret);}
	
	half4 plight_infinity2	(half m, half3 point, half3 normal, half4 light_dir, float3 X, float4 C0) {
		half3 N		= normalize(normal);					 
	  	half3 V 	= -normalize		(point);		
	  	half3 L 	= -light_dir.xyz;				

		float3 tx	= X;
		float3	H	= normalize(L+V);
		half   CP	= get_val();
		
		float4  LV 	= float4((saturate(dot(N,H))),(saturate(dot(V,H))),		
			    	  (saturate(dot(N,V))),(saturate(dot(N,L))));
		float3	P	= normalize(H - LV.x * N);
		float   TP	= dot (tx,P);
		float	TP2	= pow(TP,2);
		float	NH2	= pow(LV.x,2);
	const float	RI	= 0.01f;
	const float	Pi	= getPi_dp24();
		half	m2	= m*m;
		half	cx	= get_val2();
		float	E	= (exp (((TP2 / m2) + ((1 - TP2) / m2)) * (NH2 - 1) / NH2));
		float	Fr 	= ((RI + (1 - RI) * pow((1 - LV.z), 5)));
		float	Bd	= (m / m2 * NH2 * NH2) * (exp(-((m - NH2) / (m2 * NH2))));
		float	SS	= min(1, min((2 * LV.x * LV.w) / LV.y, (2 * LV.x * LV.z) / LV.y));
				  
		float	Si	= (Fr * Bd * SS);
		float	E2	= (7*((L_sun_color) /Pi + (Si * (((1 / sqrt(LV.w * LV.z)) * (1/(4 * CP * m2))))))*E)*LV.w;
		half4	Diff    = tex3D (s_material,(half3(LV.w,E2,m)));
// Not optimized!		
if ( m <= 1.0f)	
		 return tex3D (s_material,(half3(LV.w,LV.x,m)));			
		

else
if ( m == 4.0f)
		return float(dot(N,L));	
else
		 return ((Diff));
	
		 	}	 
	
half4 	plight_local2		(half m, half3 point, half3 normal, half4 light_position, half light_range_rsq, out float rsqr, float3 X, float4 C0)  {
	  half3 N		= normalize(normal);					 
	  half3 L2P 	= point-light_position.xyz;                
	  half3 V 		= -normalize	(point.xyz);					
	  half3 L 		= -normalize	((half3)L2P);				

		float3 tx	= X;
		float3	H	= normalize(L+V);
		half   CP	= get_val();
		
		float4  LV 	= float4((saturate(dot(N,H))),(saturate(dot(V,H))),		
			    	  (saturate(dot(N,V))),(saturate(dot(N,L))));
		float3	P	= normalize(H - LV.x * N);
		float   TP	= dot (tx,P);
		float	TP2	= pow(TP,2);
		float	NH2	= pow(LV.x,2);
	const float	RI	= 0.01f;
	const float	Pi	= getPi_dp24();
		half	m2	= m*m;
		half	cx	= get_val2();
		float	E	= (exp (((TP2 / m2) + ((1 - TP2) / m2)) * (NH2 - 1) / NH2));
	

		float	Fr 	= ((RI + (1 - RI) * pow((1 - LV.z), 5)));
		float	Bd	= (m / m2 * NH2 * NH2) * (exp(-((m - NH2) / (m2 * NH2))));
		float	SS	= min(1, min((2 * LV.x * LV.w) / LV.y, (2 * LV.x * LV.z) / LV.y));
				  
		float	Si	= (((Fr * Bd * SS)) / ((Pi * LV.w * LV.z)));
		float	E2	= (7*((Ldynamic_color) /Pi + (Si * (((1 / sqrt(LV.w * LV.z)) * (1/(4 * CP * Si * m2 ))))))*E);
		half4	Diff    = tex3D (s_material,(half3(LV.w,E2,m)));
		rsqr	= dot		(L2P,L2P);					
		half  att 	= saturate	(1 - rsqr*light_range_rsq);
		if ( m >= 0.0f)				
		 return (att)*(Diff);
		else
		return (att) * (half4(tex3D (s_material,(half3(LV.w,LV.x,m)))));			
			}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// © Code above is copyright of Cameron Sneed. All rights reserved. Copyright infringement is subject to lawsuit under US law.
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	#endif
half4	blendp	(half4	value, float4 	tcp)    		{
	#ifndef FP16_BLEND  
		value 	+= (half4)tex2Dproj 	(s_accumulator, tcp); 	// emulate blend
	#endif
	return 	value	;
}
half4	blend	(half4	value, float2 	tc)			{
	#ifndef FP16_BLEND  
		value 	+= (half4)tex2D 	(s_accumulator, tc); 	// emulate blend
	#endif
	return 	value	;
}

#endif
