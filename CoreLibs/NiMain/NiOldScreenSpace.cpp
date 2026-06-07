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

#include "NiMainPCH.h"

#include "NiOldScreenSpace.h"

#include "NiBool.h"
#include "NiZBufferProperty.h"

NiImplementRTTI(NiScreenGeometryData,NiTriShapeData);
NiImplementRTTI(NiScreenGeometry,NiTriShape);
NiImplementRTTI(NiScreenPolygon, NiObject);
NiImplementRTTI(NiScreenSpaceCamera,NiCamera);

NiScreenGeometryData::ScreenElementPool* 
    NiScreenGeometryData::ms_pkScreenElementPool = 0;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
NiScreenGeometryData::ScreenElement::ScreenElement() :
    m_usNumVerts(0),
    m_usNumTexCoordSets(0),
    m_pkVerts(NULL),
    m_pkColors(NULL),
    m_pkTexCoords(NULL)
{
}
//---------------------------------------------------------------------------
NiScreenGeometryData::ScreenElement::~ScreenElement()
{
    NiDelete[] m_pkVerts;
    NiDelete[] m_pkColors;
    NiDelete[] m_pkTexCoords;
}
//---------------------------------------------------------------------------
NiScreenGeometryData::NiScreenGeometryData() :
    m_bPixelAccurate(false),
    m_bUnorderedElements(false),
    m_usActiveVerts(0),
    m_kScreenElements(4, 4)
{
}
//---------------------------------------------------------------------------
NiScreenGeometryData::~NiScreenGeometryData()
{
    for (unsigned int i = 0; i < m_kScreenElements.GetSize(); i++)
    {
        ScreenElement* pkElement = m_kScreenElements.GetAt(i);
        m_kScreenElements.SetAt(i, 0);
        if (pkElement)
        {
            NiDelete[] pkElement->m_pkVerts;
            NiDelete[] pkElement->m_pkColors;
            NiDelete[] pkElement->m_pkTexCoords;
            ms_pkScreenElementPool->ReleaseObject(pkElement);
        }
    }

    m_kScreenElements.SetSize(0);
}
//---------------------------------------------------------------------------
void NiScreenGeometryData::_SDMInit()
{
    ms_pkScreenElementPool = NiNew ScreenElementPool;
}
//---------------------------------------------------------------------------
void NiScreenGeometryData::_SDMShutdown()
{
    NiDelete ms_pkScreenElementPool;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiScreenGeometryData);
//---------------------------------------------------------------------------
void NiScreenGeometryData::LoadBinary(NiStream& kStream)
{
    NiTriShapeData::LoadBinary(kStream);

    NiBool bPixelAccurate;
    NiStreamLoadBinary(kStream, bPixelAccurate);
    m_bPixelAccurate = (bPixelAccurate != 0);

    unsigned int uiArraySize = 0;
    NiStreamLoadBinary(kStream, uiArraySize);
    if (uiArraySize != 0)
    {
        m_kScreenElements.SetSize(uiArraySize);

        for (unsigned int i = 0; i < uiArraySize; i++)
        {
            unsigned short usNumVerts = 0;
            NiStreamLoadBinary(kStream, usNumVerts);
            if (usNumVerts != 0)
            {
                NiPoint2* pkVertex = NULL;
                NiColorA* pkColor = NULL;
                NiPoint2* pkTexture = NULL;
                unsigned short usNumTextureSets = 0;

                // Vertices
                NiBool bHasVertex;
                NiStreamLoadBinary(kStream, bHasVertex);
                if (bHasVertex != 0)
                {
                    pkVertex = NiNew NiPoint2[usNumVerts];
                    NIASSERT(pkVertex);
                    NiStreamLoadBinary(kStream, pkVertex, usNumVerts);
                }

                // Colors
                NiBool bHasColor;
                NiStreamLoadBinary(kStream, bHasColor);
                if (bHasColor != 0)
                {
                    pkColor = NiNew NiColorA[usNumVerts];
                    NIASSERT(pkColor);
                    NiStreamLoadBinary(kStream, pkColor, usNumVerts);
                }

                // TexCoords
                NiStreamLoadBinary(kStream, usNumTextureSets);
                if (usNumTextureSets != 0)
                {
                    unsigned int uiNumCoords = usNumVerts * usNumTextureSets;
                    pkTexture = NiNew NiPoint2[uiNumCoords];
                    NIASSERT(pkTexture);
                    NiStreamLoadBinary(kStream, pkTexture, uiNumCoords);
                }

                ScreenElement* pkScreenElement = 
                    ms_pkScreenElementPool->GetFreeObject();

                NiDelete[] pkScreenElement->m_pkVerts;
                pkScreenElement->m_usNumVerts = usNumVerts;
                pkScreenElement->m_pkVerts = pkVertex;

                NiDelete[] pkScreenElement->m_pkColors;
                pkScreenElement->m_pkColors = pkColor;

                NiDelete[] pkScreenElement->m_pkTexCoords;
                pkScreenElement->m_pkTexCoords = pkTexture;
                pkScreenElement->m_usNumTexCoordSets = usNumTextureSets;

                m_kScreenElements.SetAt(i, pkScreenElement);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiScreenGeometryData::LinkObject(NiStream& kStream)
{
    NiGeometryData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiScreenGeometryData::RegisterStreamables(NiStream& kStream)
{
    return NiGeometryData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiScreenGeometryData::SaveBinary(NiStream& kStream)
{
    NiTriShapeData::SaveBinary(kStream);

    NiBool bPixelAccurate = m_bPixelAccurate;
    NiStreamSaveBinary(kStream, bPixelAccurate);

    unsigned int uiArraySize = m_kScreenElements.GetSize();
    NiStreamSaveBinary(kStream, uiArraySize);
    for (unsigned int i = 0; i < uiArraySize; i++)
    {
        ScreenElement* pkScreenElement = 
            m_kScreenElements.GetAt(i);

        unsigned short usNumVerts = 
            (pkScreenElement ? pkScreenElement->m_usNumVerts : 0);
        NiStreamSaveBinary(kStream, usNumVerts);

        if (usNumVerts != 0)
        {
            NiPoint2* pkVertex = (NiPoint2*)pkScreenElement->m_pkVerts;
            NiColorA* pkColor = (NiColorA*)pkScreenElement->m_pkColors;
            NiPoint2* pkTexture = (NiPoint2*)pkScreenElement->m_pkTexCoords;
            unsigned short usNumTextureSets = 
                pkScreenElement->m_usNumTexCoordSets;

            // Vertices
            NiBool bHasVertex = (pkVertex != NULL);
            NiStreamSaveBinary(kStream, bHasVertex);
            if (bHasVertex != 0)
            {
                for (unsigned int j = 0; j < usNumVerts; j++)
                    pkVertex[j].SaveBinary(kStream);
            }

            // Colors
            NiBool bHasColor = (pkColor != NULL);
            NiStreamSaveBinary(kStream, bHasColor);
            if (bHasColor != 0)
            {
                for (unsigned int j = 0; j < usNumVerts; j++)
                    pkColor[j].SaveBinary(kStream);
            }

            // TexCoords
            NiStreamSaveBinary(kStream, usNumTextureSets);
            if (usNumTextureSets != 0)
            {
                unsigned int uiNumCoords = usNumVerts * usNumTextureSets;
                for (unsigned int j = 0; j < uiNumCoords; j++)
                    pkTexture[j].SaveBinary(kStream);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiScreenGeometryData::IsEqual(NiObject* pkObject)
{
    if (!NiTriShapeData::IsEqual(pkObject))
        return false;

    NiScreenGeometryData* pkData = (NiScreenGeometryData*) pkObject;

    if (pkData->m_bPixelAccurate != m_bPixelAccurate)
    {
        return false;
    }

    unsigned int uiEffSize0 = pkData->m_kScreenElements.GetEffectiveSize();
    unsigned int uiEffSize1 = m_kScreenElements.GetEffectiveSize();
    unsigned int uiSize0 = pkData->m_kScreenElements.GetSize();
    unsigned int uiSize1 = m_kScreenElements.GetSize();
    if (uiEffSize0 != uiEffSize1 || uiSize0 != uiSize1)
        return false;

    for (unsigned int i = 0; i < uiEffSize0; i++)
    {
        ScreenElement* pkElt0 = pkData->m_kScreenElements.GetAt(i);
        ScreenElement* pkElt1 = m_kScreenElements.GetAt(i);
        if (pkElt0)
        {
            if (pkElt1 == NULL)
                return false;

            unsigned int uiNumVerts0 = pkElt0->m_usNumVerts;
            unsigned int uiNumVerts1 = pkElt1->m_usNumVerts;
            unsigned int uiNumSets0 = pkElt0->m_usNumTexCoordSets;
            unsigned int uiNumSets1 = pkElt1->m_usNumTexCoordSets;

            if (uiNumVerts0 != uiNumVerts1 ||
                uiNumSets0 != uiNumSets1)
            {
                return false;
            }

            const NiPoint2* pkVerts0 = pkElt0->m_pkVerts;
            const NiPoint2* pkVerts1 = pkElt1->m_pkVerts;
            const NiColorA* pkColors0 = pkElt0->m_pkColors;
            const NiColorA* pkColors1 = pkElt1->m_pkColors;
            const NiPoint2* pkTexSets0 = pkElt0->m_pkTexCoords;
            const NiPoint2* pkTexSets1 = pkElt1->m_pkTexCoords;

            for (unsigned int j = 0; j < uiNumVerts0; j++)
            {
                if (pkVerts0[j] != pkVerts1[j] ||
                    pkColors0[j] != pkColors1[j])
                {
                    return false;
                }

                for (unsigned int k = 0; k < uiNumSets0; k++)
                {
                    if (pkTexSets0[j * uiNumSets0 + k] != 
                        pkTexSets1[j * uiNumSets0 + k])
                    {
                        return false;
                    }
                }
            }
        }
        else if (pkElt1 != NULL)
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiScreenGeometryData::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiGeometryData::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiScreenGeometryData::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_bPixelAccurate", m_bPixelAccurate));
    pkStrings->Add(NiGetViewerString("Num Elements",
        m_kScreenElements.GetEffectiveSize()));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
NiScreenGeometry::NiScreenGeometry(NiScreenGeometryData* pkModelData) :
    NiTriShape(pkModelData)
{
}
//---------------------------------------------------------------------------
NiScreenGeometry::NiScreenGeometry()
{
}
//---------------------------------------------------------------------------
NiScreenGeometry::~NiScreenGeometry()
{
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiScreenGeometry);
//---------------------------------------------------------------------------
void NiScreenGeometry::CopyMembers(NiScreenGeometry* pkDest,
    NiCloningProcess& kCloning)
{
    NiTriShape::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiScreenGeometry);
//---------------------------------------------------------------------------
void NiScreenGeometry::LoadBinary(NiStream& kStream)
{
    NiTriShape::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiScreenGeometry::LinkObject(NiStream& kStream)
{
    NiTriShape::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiScreenGeometry::RegisterStreamables(NiStream& kStream)
{
    return NiTriShape::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiScreenGeometry::SaveBinary(NiStream& kStream)
{
    NiTriShape::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiScreenGeometry::IsEqual(NiObject* pkObject)
{
    return NiTriShape::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
void NiScreenGeometry::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiTriShape::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiScreenGeometry::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
NiScreenPolygon::NiScreenPolygon(unsigned short usVertices, 
    const NiPoint3* pVertex, const NiPoint2* pTexture, const NiColorA* pColor)
{
    m_usVertices = usVertices;

    m_pkVertex = NiNew NiPoint3[usVertices];
    unsigned int uiDestSize = usVertices * sizeof(NiPoint3);

    NiMemcpy(m_pkVertex, pVertex, uiDestSize);
    
    if (pTexture)
    {
        m_pkTexture = NiNew NiPoint2[usVertices];
        uiDestSize = usVertices * sizeof(NiPoint2);
        NiMemcpy(m_pkTexture, pTexture, uiDestSize);
    }
    else
    {
        m_pkTexture = NULL;
    }

    if (pColor)
    {
        m_pkColor = NiNew NiColorA[usVertices];
        uiDestSize = usVertices * sizeof(NiColorA);
        NiMemcpy(m_pkColor, pColor, uiDestSize);
    }
    else
    {
        m_pkColor = NULL;
    }

    m_spPropertyState = NiNew NiPropertyState;
}
//---------------------------------------------------------------------------
NiScreenPolygon::NiScreenPolygon()
{
    m_usVertices = 0;
    m_pkVertex = 0;
    m_pkTexture = 0;
    m_pkColor = 0;
}
//---------------------------------------------------------------------------
NiScreenPolygon::~NiScreenPolygon()
{
    NiDelete[] m_pkVertex;
    NiDelete[] m_pkTexture;
    NiDelete[] m_pkColor;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiScreenPolygon);
//---------------------------------------------------------------------------
void NiScreenPolygon::CopyMembers(NiScreenPolygon* pDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pDest, kCloning);

    // share properties
    pDest->m_spPropertyState = NiNew NiPropertyState(*m_spPropertyState);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiScreenPolygon);
//---------------------------------------------------------------------------
void NiScreenPolygon::LoadBinary(NiStream& stream)
{
    NiObject::LoadBinary(stream);

    unsigned short i;

    // flag to indicate existence of vertices (always true)
    NiStreamLoadBinary(stream, m_usVertices);
    NIASSERT(m_usVertices);
    m_pkVertex = NiNew NiPoint3[m_usVertices];
    NIASSERT(m_pkVertex);
    for (i = 0; i < m_usVertices; i++)
    {
        m_pkVertex[i].LoadBinary(stream);
    }

    // flag to indicate existence of texture coordinates
    NiBool bHasTexture;
    NiStreamLoadBinary(stream, bHasTexture);

    if (bHasTexture)
    {
        m_pkTexture = NiNew NiPoint2[m_usVertices];
        NIASSERT(m_pkTexture);
        for (i = 0; i < m_usVertices; i++)
        {
            m_pkTexture[i].LoadBinary(stream);
        }
    }

    // flag to indicate existence of colors
    NiBool bHasColor;
    NiStreamLoadBinary(stream, bHasColor);

    if (bHasColor)
    {
        m_pkColor = NiNew NiColorA[m_usVertices];
        NIASSERT(m_pkColor);
        for (i = 0; i < m_usVertices; i++)
        {
            m_pkColor[i].LoadBinary(stream);
        }
    }

    stream.ReadMultipleLinkIDs();   // m_spPropertyState
}
//---------------------------------------------------------------------------
void NiScreenPolygon::LinkObject(NiStream& stream)
{
    NiObject::LinkObject(stream);

    // link properties
    unsigned int uiListSize = stream.GetNumberOfLinkIDs();
    while (uiListSize--)
    {
        m_spPropertyState->SetProperty(
            (NiProperty*) stream.GetObjectFromLinkID());
    }
}
//---------------------------------------------------------------------------
bool NiScreenPolygon::RegisterStreamables(NiStream& stream)
{
    if(!NiObject::RegisterStreamables(stream))
        return false;

    // save properties
    for(unsigned int i = 0; i < NiProperty::MAX_TYPES; i++)
    {
        (*m_spPropertyState)[i]->RegisterStreamables(stream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiScreenPolygon::SaveBinary(NiStream& stream)
{
    NiObject::SaveBinary(stream);

    unsigned short i;

    // flag to indicate existence of vertices (always true)
    NIASSERT(m_usVertices);    
    NiStreamSaveBinary(stream, m_usVertices);
    for (i = 0; i < m_usVertices; i++)
    {
        m_pkVertex[i].SaveBinary(stream);
    }

    // flag to indicate existence of texture coordinates
    NiBool bHasTexture = (m_pkTexture != NULL);
    NiStreamSaveBinary(stream, bHasTexture);
    if (bHasTexture)
    {
        for (i = 0; i < m_usVertices; i++)
        {
            m_pkTexture[i].SaveBinary(stream);
        }
    }

    // flag to indicate existence of colors
    NiBool bHasColor = (m_pkColor != NULL);
    NiStreamSaveBinary(stream, bHasColor);
    if (bHasColor)
    {
        for (i = 0; i < m_usVertices; i++)
        {
            m_pkColor[i].SaveBinary(stream);
        }
    }

    // save properties
    unsigned int uiListSize = NiProperty::MAX_TYPES;
    NiStreamSaveBinary(stream, uiListSize);
    if (uiListSize > 0)
    {
        // save properties
        for(i = 0; i < uiListSize; i++)
        {
            NiProperty* pProperty = (*m_spPropertyState)[i];
            stream.SaveLinkID(pProperty);        
        }
    }
}
//---------------------------------------------------------------------------
bool NiScreenPolygon::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
        return false;

    NiScreenPolygon* pkPoly = (NiScreenPolygon*)pkObject;

    unsigned short i;

    if (m_usVertices != pkPoly->m_usVertices)
        return false;

    // vertices
    for (i = 0; i < m_usVertices; i++)
    {
        if (m_pkVertex[i] != pkPoly->m_pkVertex[i])
            return false;
    }

    // texture coordinates
    if ((m_pkTexture && !pkPoly->m_pkTexture) || 
        (!m_pkTexture && pkPoly->m_pkTexture))
    {
        return false;
    }

    if (m_pkTexture)
    {
        for (i = 0; i < m_usVertices; i++)
        {
            if (m_pkTexture[i] != pkPoly->m_pkTexture[i])
                return false;
        }
    }

    // flag to indicate existence of colors
    if ((m_pkColor && !pkPoly->m_pkColor) ||
        (!m_pkColor && pkPoly->m_pkColor))
    {
        return false;
    }

    if (m_pkColor)
    {
        for (i = 0; i < m_usVertices; i++)
        {
            if (m_pkColor[i] != pkPoly->m_pkColor[i])
                return false;
        }
    }

    // properties

    for (i = 0; i < NiProperty::MAX_TYPES; i++)
    {
        NiProperty* pkP1 = (*m_spPropertyState)[i];
        NiProperty* pkP2 = (*(pkPoly->m_spPropertyState))[i];
        if (pkP1 != pkP2)
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiScreenPolygon::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiScreenPolygon::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_usVertices", m_usVertices));
    pkStrings->Add(NiGetViewerString("m_pkVertex", m_pkVertex));
    pkStrings->Add(NiGetViewerString("m_pkTexture", m_pkTexture));
    pkStrings->Add(NiGetViewerString("m_pkColor", m_pkColor));

    for (unsigned int i = 0; i < NiProperty::MAX_TYPES; i++)
    {
        NiProperty* pkProperty = (*m_spPropertyState)[i];
        pkStrings->Add(NiGetViewerString(
            pkProperty->GetRTTI()->GetName(), pkProperty));
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
NiScreenSpaceCamera::NiScreenSpaceCamera() :
    m_kScreenPolygons(5,5),
    m_kScreenTextures(5,5)
{
    m_kViewFrustum.m_bOrtho = true;
    UpdateWorldData();   
    UpdateWorldBound();  
}
//---------------------------------------------------------------------------
NiScreenSpaceCamera::~NiScreenSpaceCamera()
{
    unsigned int i;
    for (i = 0; i < m_kScreenPolygons.GetSize(); i++)
        m_kScreenPolygons.SetAt(i,0);
    m_kScreenPolygons.RemoveAll();

    for (i = 0; i < m_kScreenTextures.GetSize(); i++)
        m_kScreenTextures.SetAt(i,0);
    m_kScreenTextures.RemoveAll();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiScreenSpaceCamera);
//---------------------------------------------------------------------------
void NiScreenSpaceCamera::CopyMembers(NiScreenSpaceCamera* pkDest,
    NiCloningProcess& kCloning)
{
    NiCamera::CopyMembers(pkDest, kCloning);

    pkDest->m_kScreenPolygons.SetSize(m_kScreenPolygons.GetSize());
    pkDest->m_kScreenPolygons.SetGrowBy(m_kScreenPolygons.GetGrowBy());
    unsigned int i;
    for (i = 0; i < m_kScreenPolygons.GetSize(); i++)
    {
        NiScreenPolygonPtr spPoly = m_kScreenPolygons.GetAt(i);
        if (spPoly)
        {
            NiScreenPolygon* pkClone =
                (NiScreenPolygon*) spPoly->CreateClone(kCloning);
            NIASSERT(pkClone);
            pkDest->m_kScreenPolygons.SetAt(i, pkClone);
        }
        else
        {
            pkDest->m_kScreenPolygons.SetAt(i, 0);
        }
    }

    pkDest->m_kScreenTextures.SetSize(m_kScreenTextures.GetSize());
    pkDest->m_kScreenTextures.SetGrowBy(m_kScreenTextures.GetGrowBy());
    for (i = 0; i < m_kScreenTextures.GetSize(); i++)
    {
        NiScreenTexturePtr spTexture = m_kScreenTextures.GetAt(i);
        if (spTexture)
        {
            NiScreenTexture* pkClone =
                (NiScreenTexture*) spTexture->CreateClone(kCloning);
            NIASSERT(pkClone);
            pkDest->m_kScreenTextures.SetAt(i, pkClone);
        }
        else
        {
            pkDest->m_kScreenTextures.SetAt(i, 0);
        }
    }
}
//---------------------------------------------------------------------------
void NiScreenSpaceCamera::ProcessClone(NiCloningProcess& kCloning)
{
    NiCamera::ProcessClone(kCloning);

    unsigned int i;
    for (i = 0; i < m_kScreenPolygons.GetSize(); i++)
    {
        NiScreenPolygon* pkPoly = m_kScreenPolygons.GetAt(i);
        if (pkPoly)
        {
            pkPoly->ProcessClone(kCloning);
        }
    }
    for (i = 0; i < m_kScreenTextures.GetSize(); i++)
    {
        NiScreenTexture* pkTexture = m_kScreenTextures.GetAt(i);
        if (pkTexture)
        {
            pkTexture->ProcessClone(kCloning);
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiScreenSpaceCamera);
//---------------------------------------------------------------------------
void NiScreenSpaceCamera::LoadBinary(NiStream& kStream)
{
    NiCamera::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiScreenSpaceCamera::LinkObject(NiStream& kStream)
{
    NiCamera::LinkObject(kStream);

    // Copy the screen polygons and screen textures from the temporary
    // NiCamera members.
    unsigned int i, uiSize = m_kScreenPolygons.GetSize();
    for (i = 0; i < uiSize; i++)
    {
        m_kScreenPolygons.Add(m_kScreenPolygons.GetAt(i));
    }
    m_kScreenPolygons.RemoveAll();

    uiSize = m_kScreenTextures.GetSize();
    for (i = 0; i < uiSize; i++)
    {
        m_kScreenTextures.Add(m_kScreenTextures.GetAt(i));
    }
    m_kScreenTextures.RemoveAll();
}
//---------------------------------------------------------------------------
bool NiScreenSpaceCamera::RegisterStreamables(NiStream& kStream)
{
    return NiCamera::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiScreenSpaceCamera::SaveBinary(NiStream& kStream)
{
    NiCamera::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiScreenSpaceCamera::IsEqual(NiObject* pkObject)
{
    return NiCamera::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
