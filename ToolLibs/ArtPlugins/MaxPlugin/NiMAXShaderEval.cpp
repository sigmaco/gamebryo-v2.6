// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
//
//      Copyright (c) 1996-2008 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#include "MaxImmerse.h"
#include "NiMAXShader.h"

//---------------------------------------------------------------------------
// Actually shade the surface
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void NiMAXShader::GetIllumParams( ShadeContext &, IllumParams &ip )
{
    ip.stdParams = SupportStdParams();
    ip.channels[S_BASE] = m_diffuse;
    ip.channels[S_DARK] = m_dark;
    ip.channels[S_DETAIL] = m_detail;
    ip.channels[S_DECAL] = m_decal;
    ip.channels[S_BUMP] = m_bump;
    ip.channels[S_GLOSS] = m_gloss;
    ip.channels[S_GLOW] = m_glow;
    ip.channels[S_REFLECTION] = m_reflection;
}

//---------------------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable: 4239)
void NiMAXShader::Illum(ShadeContext &sc, IllumParams &ip)
{
    LightDesc *pLight;
    Color lightCol;

    // Get our parameters our of the channels
    Color base      = ip.channels[ S_BASE ];
    Color dark      = ip.channels[ S_DARK ];
    Color detail    = ip.channels[ S_DETAIL ];
    Color decal     = ip.channels[ S_DECAL ];
    Color bump      = ip.channels[ S_BUMP ];
    Color gloss     = ip.channels[ S_GLOSS ];
    Color glow      = ip.channels[ S_GLOW ];
    Color reflection = ip.channels[ S_REFLECTION];

    ip.specIllumOut.Black();

    if (m_iApplyMode)
    {
        for (int i = 0; i < sc.nLights; i++) 
        {
            register float fNdotL, fDiffCoef;
            Point3 L;
            
            pLight = sc.Light(i);
            if (pLight->Illuminate(sc, sc.Normal(), lightCol, L, fNdotL,
                fDiffCoef)) 
            {
                if (pLight->ambientOnly) 
                {
                    ip.ambIllumOut += lightCol;
                    continue;
                }
                
                if (fNdotL <= 0.0f) 
                    continue;
                
                if (pLight->affectDiffuse)
                    ip.diffIllumOut += fDiffCoef * lightCol;
                
                if (m_bSpecularOn && pLight->affectSpecular) 
                {
                    Point3 H = Normalize(L - sc.V());
                    float c = DotProd(sc.Normal(), H);  
                    if (c > 0.0f) 
                    {
                        c = (float)pow(c, m_fShininess); 
                        // c * bright * lightCol;
                        ip.specIllumOut += c * lightCol; 
                    }
                }
            }
        }
    }
    else    
    {
        ip.ambIllumOut.Black();
        ip.diffIllumOut.White();
    }

    ip.ambIllumOut *= m_ambient;
    ip.diffIllumOut *= dark * (base * m_diffuse * detail);  // + decal;
    ip.selfIllumOut = m_emittance + glow + decal;
    ip.specIllumOut *= m_specular;

//  ShadeTransmission(sc, ip, ip.channels[ID_RR], ip.refractAmt);
//  ShadeReflection( sc, ip, ip.channels[ID_RL] ); 
    CombineComponents( sc, ip ); 
}
#pragma warning(pop)
//---------------------------------------------------------------------------
void NiMAXShader::AffectReflection(ShadeContext &, 
                                        IllumParams &, Color &rcol) 
{ 
    rcol *= m_specular; 
}

//---------------------------------------------------------------------------
// Called to combine the various color and shading components
void NiMAXShader::CombineComponents( ShadeContext &, IllumParams& ip )
{ 
//  ip.finalC = ip.finalOpac * 
//    (ip.ambIllumOut + ip.diffIllumOut  + ip.selfIllumOut)
//          + ip.specIllumOut + ip.reflIllumOut + ip.transIllumOut; 

    ip.finalC = (ip.ambIllumOut + ip.diffIllumOut +
        ip.selfIllumOut) + ip.specIllumOut;
}

//---------------------------------------------------------------------------
BOOL NiMAXShader::IsMetal() 
{ 
    return FALSE; 
}
//---------------------------------------------------------------------------

