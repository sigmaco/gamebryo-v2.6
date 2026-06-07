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

// Precompiled Header
#include "NiMainPCH.h"

#include "NiRenderer.h"
#include "NiTextureEffect.h"

NiImplementRTTI(NiTextureEffect,NiDynamicEffect);

NiFixedString NiTextureEffect::ms_akTextureTypeStrings[TEXTURE_TYPE_MAX];

//---------------------------------------------------------------------------
NiTextureEffect::NiTextureEffect() :
    m_kModelProjMat(NiMatrix3::IDENTITY),
    m_kModelProjTrans(0.0f, 0.0f, 0.0f),
    m_kWorldProjMat(NiMatrix3::IDENTITY),
    m_kWorldProjTrans(0.0f, 0.0f, 0.0f),
    m_kModelPlane(NiPoint3::UNIT_X, 0.0f),
    m_kWorldPlane(NiPoint3::UNIT_X, 0.0f),
    m_eFilter(NiTexturingProperty::FILTER_NEAREST),
    m_eClamp(NiTexturingProperty::WRAP_S_WRAP_T),
    m_eTextureMode(PROJECTED_LIGHT),
    m_eCoordMode(WORLD_PARALLEL),
    m_usMaxAnisotropy(1),
    m_bPlaneEnable(false)
{
    m_ucEffectType = NiDynamicEffect::TEXTURE_EFFECT;
}
//---------------------------------------------------------------------------
NiTextureEffect::~NiTextureEffect()
{
    // This must remain in NiDynamicEffect derived classes rather than in the
    // base class.  Some renderers rely on ability to dynamic cast 
    // NiDynamicEffects to determine if they are NiLight derived, and this is
    // not possible if the derived portion of the object has been destroyed.
    NiRenderer::RemoveEffect(this);
}
//---------------------------------------------------------------------------
// NiStaticDataManager
//---------------------------------------------------------------------------
void NiTextureEffect::_SDMInit()
{
    ms_akTextureTypeStrings[PROJECTED_LIGHT] = "ProjectedLight";
    ms_akTextureTypeStrings[PROJECTED_SHADOW] = "ProjectedShadow";
    ms_akTextureTypeStrings[ENVIRONMENT_MAP] = "EnvironmentMap";
    ms_akTextureTypeStrings[FOG_MAP] = "FogMap";
}
//---------------------------------------------------------------------------
void NiTextureEffect::_SDMShutdown()
{
    ms_akTextureTypeStrings[PROJECTED_LIGHT] = NULL;
    ms_akTextureTypeStrings[PROJECTED_SHADOW] = NULL;
    ms_akTextureTypeStrings[ENVIRONMENT_MAP] = NULL;
    ms_akTextureTypeStrings[FOG_MAP] = NULL;
}
//---------------------------------------------------------------------------
const NiFixedString& NiTextureEffect::GetTypeNameFromID(unsigned int uiID)
{
    NIASSERT(uiID < TEXTURE_TYPE_MAX);
    return ms_akTextureTypeStrings[uiID];
}
//---------------------------------------------------------------------------
bool NiTextureEffect::GetTypeIDFromName(const NiFixedString& kName, 
    unsigned int& uiID)
{
    for (unsigned int ui = 0; ui < TEXTURE_TYPE_MAX; ui++)
    {
        if (ms_akTextureTypeStrings[ui].EqualsNoCase(kName))
        {
            uiID = ui;
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
// Projection
//---------------------------------------------------------------------------
void NiTextureEffect::UpdateProjection()
{
    if (m_eCoordMode == SPHERE_MAP)
    {
        // Sphere map uses a fixed model-space projection matrix, so in this
        // case, m_kModelProjMat is ignored

        // There is no translation of a sphere map - it is at infinity
        m_kWorldProjTrans.x = 
            m_kWorldProjTrans.y = 0.5f;
        m_kWorldProjTrans.z = 0.0f;

        // must ignore scale - only rotation matters
        // sphere map is orthographic
        m_kWorldProjMat.SetCol( 0, 
            0.5f * m_kWorld.m_Rotate.GetEntry( 0,2 ),
            -0.5f * m_kWorld.m_Rotate.GetEntry( 0,1 ),
            0.0f );

        m_kWorldProjMat.SetCol( 1, 
            0.5f * m_kWorld.m_Rotate.GetEntry( 1,2 ),
            -0.5f * m_kWorld.m_Rotate.GetEntry( 1,1 ),
            0.0f ); 
        
        m_kWorldProjMat.SetCol( 2, 
            0.5f * m_kWorld.m_Rotate.GetEntry( 2,2 ),
            -0.5f * m_kWorld.m_Rotate.GetEntry( 2,1 ),
            0.0f );
    }
    else if (m_eCoordMode == SPECULAR_CUBE_MAP ||
        m_eCoordMode == DIFFUSE_CUBE_MAP)
    {
        // Cubic environment maps use a fixed model-space projection matrix, 
        // so in this case, m_kModelProjMat is largely ignored

        // There is no translation of a cube map - it is at infinity
        m_kWorldProjTrans.x = 
            m_kWorldProjTrans.y = 
            m_kWorldProjTrans.z = 0.0f;

        // must ignore scale - only rotation matters
        // cube map is orthographic
        m_kWorldProjMat = (m_kModelProjMat * m_kWorld.m_Rotate.Transpose());
    }
    else
    {
        // update the world-space projections
        float fInvScale = 1.0f/m_kWorld.m_fScale;
        m_kWorldProjMat 
            = (m_kModelProjMat * m_kWorld.m_Rotate.Transpose()) * fInvScale;
        m_kWorldProjTrans 
            = m_kModelProjTrans - m_kWorldProjMat * m_kWorld.m_Translate;
    }

    // The model plane has normal N0, constant C0 and is given by
    // Dot(N0,X) = C0.  If Y = s*R*X+T where s is world scale, R is world
    // rotation, and T is world translation for current node, then
    // X = (1/s)*R^t*(Y-T) and
    //     C0 = Dot(N0,X) = Dot(N0,(1/s)*R^t*(Y-T)) = (1/s)*Dot(R*N0,Y-T)
    // so
    //     Dot(R*N0,Y) = s*c0+Dot(R*N0,T)
    // The world plane has
    //     normal N1 = R*N0
    //     constant C1 = s*C0+Dot(R*N0,T) = s*C0+Dot(N1,T)

    NiPoint3 normal = m_kWorld.m_Rotate*m_kModelPlane.GetNormal();
    float fConstant = m_kWorld.m_fScale*m_kModelPlane.GetConstant() +
        normal*m_kWorld.m_Translate;
    m_kWorldPlane.SetNormal(normal);
    m_kWorldPlane.SetConstant(fConstant);

    IncRevisionID();
}
//---------------------------------------------------------------------------
void NiTextureEffect::UpdateWorldData()
{
    NiDynamicEffect::UpdateWorldData();

    UpdateProjection();
}
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiTextureEffect);
//---------------------------------------------------------------------------
void NiTextureEffect::CopyMembers(NiTextureEffect* pDest,
    NiCloningProcess& kCloning)
{
    NiDynamicEffect::CopyMembers(pDest, kCloning);

    pDest->m_spTexture = m_spTexture;

    pDest->m_kModelProjMat = m_kModelProjMat;
    pDest->m_kModelProjTrans = m_kModelProjTrans;
    pDest->m_eFilter = m_eFilter;
    pDest->m_usMaxAnisotropy = m_usMaxAnisotropy;    
    pDest->m_eClamp = m_eClamp;
    pDest->m_eTextureMode = m_eTextureMode;
    pDest->m_eCoordMode = m_eCoordMode;

    pDest->m_bPlaneEnable = m_bPlaneEnable;
    pDest->m_kModelPlane = m_kModelPlane;
    pDest->m_kWorldPlane = m_kWorldPlane;

    // The list m_illuminatedNodeList is not processed.  The application
    // has the responsibility for cloning the relationships between the
    // lights and nodes.
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiTextureEffect);
//---------------------------------------------------------------------------
void NiTextureEffect::LoadBinary(NiStream& kStream)
{
    NiDynamicEffect::LoadBinary(kStream);

    m_kModelProjMat.LoadBinary(kStream);
    m_kModelProjTrans.LoadBinary(kStream);
    NiStreamLoadEnum(kStream,m_eFilter);

    // Anisotropy settings were added in NIF revision 20.5.0.4
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 5, 0, 4))
    {
        m_usMaxAnisotropy = 1;
    }
    else
    {
        NiStreamLoadBinary(kStream, m_usMaxAnisotropy);
    }

    NiStreamLoadEnum(kStream,m_eClamp);
    NiStreamLoadEnum(kStream,m_eTextureMode);
    NiStreamLoadEnum(kStream,m_eCoordMode);

    // load image
    kStream.ReadLinkID();    // m_spTexture
    
    NiBool bPlaneEnable;
    NiStreamLoadBinary(kStream, bPlaneEnable);
    m_bPlaneEnable = (bPlaneEnable != 0);
    m_kModelPlane.LoadBinary(kStream);
    
    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 3, 0, 4))
    {
        short sLandK;
        NiStreamLoadBinary(kStream, sLandK);
        NiStreamLoadBinary(kStream, sLandK);
    }
}
//---------------------------------------------------------------------------
void NiTextureEffect::LinkObject(NiStream& kStream)
{
    NiDynamicEffect::LinkObject(kStream);

    // link image
    m_spTexture = (NiTexture*)kStream.GetObjectFromLinkID();

    // Illuminated nodes will be linked by the nodes themselves.  They
    // should not be linked here (else you get a recursive loop).
}
//---------------------------------------------------------------------------
bool NiTextureEffect::RegisterStreamables(NiStream& kStream)
{
    if ( !NiDynamicEffect::RegisterStreamables(kStream) )
        return false;

    // register image
    if ( m_spTexture )
        m_spTexture->RegisterStreamables(kStream);

    // Illuminated nodes are already registered by the nodes themselves.
    // They should not be registered here (else you get a recursive loop).

    return true;
}
//---------------------------------------------------------------------------
void NiTextureEffect::SaveBinary(NiStream& kStream)
{
    NiDynamicEffect::SaveBinary(kStream);

    // attributes
    m_kModelProjMat.SaveBinary(kStream);
    m_kModelProjTrans.SaveBinary(kStream);
    NiStreamSaveEnum(kStream,m_eFilter);
    NiStreamSaveBinary(kStream, m_usMaxAnisotropy);
    NiStreamSaveEnum(kStream,m_eClamp);
    NiStreamSaveEnum(kStream,m_eTextureMode);
    NiStreamSaveEnum(kStream,m_eCoordMode);

    kStream.SaveLinkID(m_spTexture);

    NiBool bPlaneEnable = m_bPlaneEnable;
    NiStreamSaveBinary(kStream, bPlaneEnable);
    m_kModelPlane.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiTextureEffect::IsEqual(NiObject* pObject)
{
    if ( !NiDynamicEffect::IsEqual(pObject) )
        return false;

    NiTextureEffect* pEffect = (NiTextureEffect*) pObject;

    // attributes
    if ( pEffect->m_kModelProjMat != m_kModelProjMat ||
         pEffect->m_kModelProjTrans != m_kModelProjTrans ||
         pEffect->m_eFilter != m_eFilter ||
         pEffect->m_eClamp != m_eClamp ||
         pEffect->m_eTextureMode != m_eTextureMode ||
         pEffect->m_eCoordMode != m_eCoordMode ||
         pEffect->m_bPlaneEnable != m_bPlaneEnable ||
         pEffect->m_kModelPlane != m_kModelPlane ||
         pEffect->m_usMaxAnisotropy != m_usMaxAnisotropy)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiTextureEffect::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiDynamicEffect::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiTextureEffect::ms_RTTI.GetName()));

    pStrings->Add(m_kModelProjMat.GetViewerString("m_kModelProjMat"));
    pStrings->Add(m_kModelProjTrans.GetViewerString("m_kModelProjTrans"));
    pStrings->Add(GetViewerString("m_eTextureMode", m_eTextureMode));
    pStrings->Add(NiTexturingProperty::GetViewerString("m_eFilter", 
        m_eFilter));
    pStrings->Add(NiTexturingProperty::GetViewerString("m_eClamp", 
        m_eClamp));
    pStrings->Add(GetViewerString("m_eTextureMode", m_eTextureMode));
    pStrings->Add(GetViewerString("m_eCoordMode", m_eCoordMode));
    pStrings->Add(NiGetViewerString("m_bPlaneEnable", m_bPlaneEnable));
    pStrings->Add(m_kModelPlane.GetViewerString("m_kModelPlane"));
}
//---------------------------------------------------------------------------
char* NiTextureEffect::GetViewerString(const char* pPrefix,
    TextureType eMode)
{
    size_t stLen = strlen(pPrefix) + 25;
    char* pString = NiAlloc(char, stLen);

    switch ( eMode )
    {
    case PROJECTED_LIGHT:
        NiSprintf(pString, stLen, "%s = PROJECTED_LIGHT", pPrefix);
        break;
    case PROJECTED_SHADOW:
        NiSprintf(pString, stLen, "%s = PROJECTED_SHADOW", pPrefix);
        break;
    case ENVIRONMENT_MAP:
        NiSprintf(pString, stLen, "%s = ENVIRONMENT_MAP", pPrefix);
        break;
    case FOG_MAP:
        NiSprintf(pString, stLen, "%s = FOG_MAP", pPrefix);
        break;
    default:
        NIASSERT(!"Unknown NiTextureEffect");
        break;
    }

    return pString;
}
//---------------------------------------------------------------------------
char* NiTextureEffect::GetViewerString(const char* pPrefix,
    CoordGenType eMode)
{
    size_t stLen = strlen(pPrefix) + 25;
    char* pString = NiAlloc(char,stLen);

    switch ( eMode )
    {
    case WORLD_PARALLEL:
        NiSprintf(pString, stLen, "%s = WORLD_PARALLEL", pPrefix);
        break;
    case WORLD_PERSPECTIVE:
        NiSprintf(pString, stLen, "%s = WORLD_PERSPECTIVE", pPrefix);
        break;
    case SPHERE_MAP:
        NiSprintf(pString, stLen, "%s = SPHERE_MAP", pPrefix);
        break;
    case SPECULAR_CUBE_MAP:
        NiSprintf(pString, stLen, "%s = SPECULAR_CUBE_MAP", pPrefix);
        break;
    case DIFFUSE_CUBE_MAP:
        NiSprintf(pString, stLen, "%s = DIFFUSE_CUBE_MAP", pPrefix);
        break;
    default:
        NiSprintf(pString, stLen, "%s = UNKNOWN!!!", pPrefix);
        break;
    }

    return pString;
}
//---------------------------------------------------------------------------
