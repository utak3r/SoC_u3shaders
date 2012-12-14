#ifndef SLOAD_H
#define SLOAD_H

#include "common.h"


//////////////////////////////////////////////////////////////////////////////////////////
// Bumped surface loader                //
//////////////////////////////////////////////////////////////////////////////////////////
struct        surface_bumped
{
        half4         	base;
        half3         	normal;
        half         	gloss;
        half         	height;

};

float4   tbase                 (float2 tc)        {
        return                 tex2D                (s_base,         tc);
}
#if defined(USE_Sky4CE_PARALLAXOCCLUSION) && defined(PARALLAX_OCCLUSION) && !defined(USE_PARALLAX)
surface_bumped                sload_i         ( p_bumped I)
{
    surface_bumped      S;
    	float2 vPlx = Parallax(I.eye,I.tcdh,s_bumpX);
     	float2 vTexCoord = vPlx;	
   		float2 dTexCoords = ddx(I.tcdh) * ddx(I.tcdh) + ddy(I.tcdh) * ddy(I.tcdh);
   		float fMipDistance = max(dTexCoords.x,dTexCoords.y);
        #ifdef USE_TEXTURE_PACK
        	#ifdef PARALLAX_FADE
        if(fMipDistance <= STOP_FADE) vTexCoord = (fMipDistance >= START_FADE)?(Sky4CE_ParallaxOcclusion(I.eye,I.tcdh,s_base)+(((vPlx-Sky4CE_ParallaxOcclusion(I.eye,I.tcdh,s_base))*(fMipDistance-START_FADE))/(STOP_FADE-START_FADE))):Sky4CE_ParallaxOcclusion(I.eye,I.tcdh,s_base);
        	#else
        if(fMipDistance <= STOP_FADE) vTexCoord = Sky4CE_ParallaxOcclusion(I.eye,I.tcdh,s_base);
        	#endif
        #else
        	#ifdef PARALLAX_FADE
        if(fMipDistance <= STOP_FADE) vTexCoord = (fMipDistance >= START_FADE)?(Sky4CE_ParallaxOcclusion(I.eye,I.tcdh,s_bumpX)+(((vPlx-Sky4CE_ParallaxOcclusion(I.eye,I.tcdh,s_bumpX))*(fMipDistance-START_FADE))/(STOP_FADE-START_FADE))):Sky4CE_ParallaxOcclusion(I.eye,I.tcdh,s_bumpX);
        	#else
        if(fMipDistance <= STOP_FADE) vTexCoord = Sky4CE_ParallaxOcclusion(I.eye,I.tcdh,s_bumpX);
        	#endif
        #endif
        float4       Nu      =       tex2D		(s_bump,         vTexCoord);                // IN:  normal.gloss
        float4       NuE     =       tex2D      	(s_bumpX,       vTexCoord);                // IN:         normal_error.height
        S.base              =       tbase                (vTexCoord);                                // IN:  rgb.a
        S.normal            = 		Nu.wzyx + (NuE.xyz - 1.0h);							 // (Nu.wzyx - .5h) + (E-.5)
        S.gloss             =       Nu.x*Nu.x	;                                        //        S.gloss             =        Nu.x*Nu.x;
        S.height            =       NuE.z       ;

		#ifdef USE_TDETAIL
        float4       detail  =		tex2D(s_detail,vTexCoord * dt_params )        	;
        S.base.rgb          =		S.base.rgb     * detail.rgb*2		;
        S.gloss             =  		S.gloss * detail.w * 2				;
		#endif

        return                S;
}
#else

#ifdef         USE_PARALLAX
surface_bumped                sload_i         ( p_bumped I)        // + texld, mad, nrm(3), mad  = 1+1+3+1 = 6, 15+6=21, OK
{

        surface_bumped      S;
        half        height	=       tex2D      (s_bumpX, I.tcdh).w;                                //
                    height  =       height*(parallax.x) + (parallax.y);                        //
        float2		new_tc  =       I.tcdh + height*normalize        (I.eye);                //
        half4       Nu      =       tex2D		(s_bump,         new_tc);                // IN:  normal.gloss
        half4       NuE     =       tex2D      	(s_bumpX,       new_tc);                // IN:         normal_error.height
        S.base              =       tbase                (new_tc);                                // IN:  rgb.a
        S.normal            = 		Nu.wzyx + (NuE.xyz - 1.0h);							 // (Nu.wzyx - .5h) + (E-.5)
//        S.normal            = 		half3(1,0,0);
        S.gloss             =       Nu.x*Nu.x	;                                        //        S.gloss             =        Nu.x*Nu.x;
        S.height            =       NuE.z       ;


#ifdef        USE_TDETAIL
        half4       detail  =		tex2D(s_detail,I.tcdbump)        	;
       S.base.rgb          =		S.base.rgb     * detail.rgb*2		;
        S.gloss             =  		S.gloss * detail.w * 2				;
#endif

        return                S;
}
#else
surface_bumped                sload_i         ( p_bumped I)
{
        surface_bumped        S;
          half4 Nu 			=		tex2D                (s_bump, I.tcdh);                        // IN:  normal.gloss
        half4 NuE           =		tex2D                (s_bumpX,I.tcdh);                        // IN:         normal_error.height
        S.base              =		tbase                (I.tcdh)		;                         // IN:  rgb.a
        S.normal            =		Nu.wzyx + (NuE.xyz - 1.0h)			;
//        S.normal            = 		half3(1,0,0);
        S.gloss             =		Nu.x*Nu.x							;                         //        S.gloss             =        Nu.x*Nu.x;
        S.height            = 		NuE.z;

#ifdef        USE_TDETAIL
        half4 detail		=        tex2D(s_detail,I.tcdbump)    ;
        S.base.rgb          =      	S.base.rgb*detail.rgb        	*2      ;
        S.gloss             =  		S.gloss * detail.w * 2			;
#endif
        return              S;
}
#endif
#endif
surface_bumped              sload                 ( p_bumped I)
{
        surface_bumped      S   = sload_i	(I);
		S.normal.z			*=	0.5;		//. make bump twice as contrast (fake, remove me if possible)

        return              S;
}

#endif
