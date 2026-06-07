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

#ifndef NIOLDSCREENSPACE_H
#define NIOLDSCREENSPACE_H

// DEPRECATED CLASSES.  The only member data items and member functions that
// are retained are those required in order to stream in the objects.  Only
// the persistent data is stored, as that is the only data that could have 
// been stored in the NIF.  The classes exist solely to allow any users who
// generated objects of these types in their own internal tools to load and
// convert these deprecated objects into supported objects.  All of these
// objects are consolidated into a single header/source file pair in order to
// avoid cluttering the SDK

#include "NiMainLibType.h"
#include "NiCamera.h"
#include "NiScreenTexture.h"
#include "NiTriShape.h"
#include "NiTriShapeData.h"
#include "NiTPool.h"
#include "NiTArray.h"

//---------------------------------------------------------------------------

class NIMAIN_ENTRY NiScreenGeometryData : public NiTriShapeData
{
public:
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    class NIMAIN_ENTRY ScreenElement : public NiMemObject
    {
    public:
        ScreenElement();
        virtual ~ScreenElement();

        unsigned short m_usNumVerts;
        unsigned short m_usNumTexCoordSets;
        NiPoint2* m_pkVerts;
        NiColorA* m_pkColors;
        NiPoint2* m_pkTexCoords;
    };    
    
    typedef NiTPrimitiveArray<ScreenElement*> ScreenElementArray;
    
    NiScreenGeometryData();
    virtual ~NiScreenGeometryData();
    static void _SDMInit();
    static void _SDMShutdown();

    bool m_bPixelAccurate;
    bool m_bUnorderedElements;
    unsigned int m_usActiveVerts;
    ScreenElementArray m_kScreenElements;

    typedef NiTObjectPool<ScreenElement> ScreenElementPool;

    static ScreenElementPool* ms_pkScreenElementPool;
};

NiSmartPointer(NiScreenGeometryData);

//---------------------------------------------------------------------------

class NIMAIN_ENTRY NiScreenGeometry : public NiTriShape
{
    NiDeclareRTTI;
    NiDeclareClone(NiScreenGeometry);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiScreenGeometry(NiScreenGeometryData* pkModelData);
    virtual ~NiScreenGeometry();
    NiScreenGeometry();
};

typedef NiPointer<NiScreenGeometry> NiScreenGeometryPtr;

//---------------------------------------------------------------------------

class NIMAIN_ENTRY NiScreenPolygon : public NiObject
{
    NiDeclareRTTI;
    NiDeclareClone(NiScreenPolygon);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiScreenPolygon(unsigned short usVertices, const NiPoint3* pkVertex,
        const NiPoint2* pkTexture, const NiColorA* pkColor);
    virtual ~NiScreenPolygon();
    NiScreenPolygon();

    NiPropertyStatePtr m_spPropertyState;
    unsigned short m_usVertices;
    NiPoint3* m_pkVertex;
    NiPoint2* m_pkTexture;
    NiColorA* m_pkColor;
};

typedef NiPointer<NiScreenPolygon> NiScreenPolygonPtr;
typedef NiTObjectArray<NiScreenPolygonPtr> NiScreenPolygonArray;

//---------------------------------------------------------------------------

class NIMAIN_ENTRY NiScreenSpaceCamera : public NiCamera
{
    NiDeclareRTTI;
    NiDeclareClone(NiScreenSpaceCamera);
    NiDeclareStream;

public:
    NiScreenSpaceCamera();
    virtual ~NiScreenSpaceCamera();
    virtual void ProcessClone(NiCloningProcess& kCloning);

    NiScreenPolygonArray m_kScreenPolygons;
    NiScreenTextureArray m_kScreenTextures;
};

typedef NiPointer<NiScreenSpaceCamera> NiScreenSpaceCameraPtr;

#endif // NIOLDSCREENSPACE_H

