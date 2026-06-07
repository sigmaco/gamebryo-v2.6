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

#ifndef NI_XSI_PPG_H
#define NI_XSI_PPG_H

#include "xsi_rtshaders.h"      // for XSI_RTS_Texture
#include "NiXSIShader.h"        // for NiXSIShader & NiXSIStdMaterial
#include "NiXSIMaps.h"          // for maps enums
#include "GamebryoShaders.h"    // for CgParameterCacheEntry
#include "NiXSIStdMaterial.h"   // for NiXSIStdMaterial

#define CG_NB_LIGHTS	7

// Silence warnings about deprecated functions
#pragma warning(disable : 4996)

//---------------------------------------------------------------------------
// Gamebryo material per-instance shader data.
//---------------------------------------------------------------------------
struct CgAutoLightTracking
{
	CGparameter		silighttype;
	CGparameter		silightambient;
	CGparameter		silightdiffuse;
	CGparameter		silightspecular;
	CGparameter		silightposition;
	CGparameter		silightdirection;
	CGparameter		silightfalloff;
	CGparameter		silightattenuation;
	CGparameter		silightcone;
};

struct GamebryoMaterialInstanceData
{
	CGcontext					m_Context;
	CGprogram					m_Program[2];           // vertex & fragment
	unsigned int				m_ContextCount;
	char*                       m_pProgramString;
	XSI_RTS_FXSemantics_inuse	m_Semantics;

	// automatic uniform parameters
	CGparameter					siviewportsize;
	CgAutoLightTracking			silight[CG_NB_LIGHTS];

	// Custom PSet parameters
	CgParameterCacheEntry**     m_pXSIParams;
	int							m_lNbXSIParam;

    // Maps
    int			                m_Handle[SI_NUM_MATERIALMAPS];
	LONG		                m_MipMap[SI_NUM_MATERIALMAPS];
    LONG		                m_Compression[SI_NUM_MATERIALMAPS];
	LONG		                m_Format[SI_NUM_MATERIALMAPS];

    // Instance material
    NiXSIStdMaterial            m_XSIStdMaterial; 
};

//---------------------------------------------------------------------------
// All structures that are used to construct a PPG must be compiled without padding 
// between struct fields.  This is because a pointer to the PPG struct will be initialized 
// directly by type casting a void pointer as in the following:
//
//   MyPPG * my_ppg = (MyPPG*) in_pVoid;
//
// The pragma is not necessary under a 32 bit architecture since a PPG will only contain
// floats (4 bytes), longs (4 bytes) and pointers (4 bytes).  Therefore structs will 
// never contain padding.  But, in a 64 bit architecture where pointers are 8 bytes, 
// if we don't force the compiler to remove the automatic padding, the struct may contain
// padding.  This padding is not considered when initializing the void pointer (in_pVoid).
// Therefore, the direct type casting would not work.
//
// These structures MUST correspond to the SPDL file. 
//---------------------------------------------------------------------------
#pragma pack(push, 1)

struct TexPPG
{   
    LONG                Map_Enabled;    
    LONG                Map_TexSpace;
    LONG                Map_FilteringMode;
	LONG                Map_MipMapEnabled;
	LONG                Map_ApplyMode;    

    LONG		        TexCoord_Enabled;
    LONG		        TexCoord_uwrap;
	LONG		        TexCoord_vwrap;
	LONG		        TexCoord_wwrap;	
    LONG                TexCoord_transform;
	float		        TexCoord_transx;
	float		        TexCoord_transy;
	float		        TexCoord_transz;
	float		        TexCoord_rotx;
	float		        TexCoord_roty;
	float		        TexCoord_rotz;
	float		        TexCoord_scalex;
	float		        TexCoord_scaley;
	float		        TexCoord_scalez;
};

struct GamebryoMaterialPPG
{
    XSI_RTS_ColorRGBAf	cKa;
    XSI_RTS_ColorRGBAf	cKd;
    XSI_RTS_ColorRGBAf	cKs;
    XSI_RTS_ColorRGBAf  cKe;

    float               Shininess;    
    float               Alpha;

    LONG                ziPreviewTexture;    

    TexPPG              Tex[SI_NUM_MATERIALMAPS];
    
    float               BumpMap_Scale;
    LONG                EvironmentMap_CoordinateType;

    XSI_RTS_Texture		Map_Image[SI_NUM_MATERIALMAPS];    
};

#pragma pack(pop)

//---------------------------------------------------------------------------

#endif // NI_XSI_PPG_H
