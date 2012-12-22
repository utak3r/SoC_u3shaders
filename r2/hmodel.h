#ifndef HMODEL_H
#define HMODEL_H

#include "common.h"

uniform samplerCUBE env_s0;
uniform samplerCUBE env_s1;
uniform samplerCUBE sky_s0;
uniform samplerCUBE sky_s1;
uniform half4 env_color;
uniform half3x4 m_v2w;

void hmodel(out half3 hdiffuse, out half3 hspecular, half m, half h, half s, float3 point, half3 normal)
{
	half3 nw = mul(m_v2w,normal);
	half hscale = h;
	hscale = (hscale*(hscale/2.f));

	float3 v2pointL = normalize(point);
	half3 v2point = mul(m_v2w,v2pointL);
	half3 vreflect = reflect(v2point,nw);
	half hspec = .5h + .5h*dot(vreflect,v2point);

	half4 light = tex3D(s_material, half3(hscale, hspec, m));

	half3 e0d = texCUBE(env_s0,nw);
	half3 e1d = texCUBE(env_s1,nw);
	half3 env_d = env_color.xyz * lerp(e0d,e1d,env_color.w);
	env_d *= env_d;
	hdiffuse = env_d * light.xyz + L_ambient.rgb;

	vreflect.y = vreflect.y*2 - 1;
	half3 e0s = texCUBE(env_s0,vreflect);
	half3 e1s = texCUBE(env_s1,vreflect);
	half3 env_s = env_color.xyz * lerp(e0s,e1s,env_color.w);
	env_s *= env_s;
	hspecular = env_s * light.w * s;
}

void hmodel_table(out half3 hdiffuse, out half3 hspecular, half m, half h, half s, half3 point, half3 normal)
{
	half hscale = h;

	// reflection vector
	half3 v2point = normalize(point);
	half3 vreflect = reflect(v2point,normal);
	half hspec = .5h + .5h * dot(vreflect, v2point);

	// material
	half4 light = tex3D(s_material, half3(hscale, hspec, m));

	// diffuse color
	half3 env_d = texCUBE(env_s0,normal);

	// specular color
	half3 env_s = texCUBE(env_s0,vreflect);

	hdiffuse = env_d * light.xyz + L_ambient.rgb;
	hspecular = env_s * light.w * s;
}

#endif
