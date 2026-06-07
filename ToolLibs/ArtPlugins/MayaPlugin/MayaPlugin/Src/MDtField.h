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

// MDtField.h

#ifndef MDTFIELD_H
#define MDTFIELD_H

#include "MDt.h"
#include "MDtExt.h"


//---------------------------------------------------------------------------
class kMDtField
{

public:

    char*       m_szName;
    MObject     m_MObjectField;

    // Transform Attributes
    float       m_fTranslateX;
    float       m_fTranslateY;
    float       m_fTranslateZ;
    float       m_fRotateX;
    float       m_fRotateY;
    float       m_fRotateZ;
    float       m_fScaleX;
    float       m_fScaleY;
    float       m_fScaleZ;

    float       m_fWorldTranslateX;
    float       m_fWorldTranslateY;
    float       m_fWorldTranslateZ;
    float       m_fWorldRotateX;
    float       m_fWorldRotateY;
    float       m_fWorldRotateZ;
    float       m_fWorldScaleX;
    float       m_fWorldScaleY;
    float       m_fWorldScaleZ;

    float       m_fMagnitude;
    float       m_fAttenuation;
    bool        m_bUseMaxDistance;
    float       m_fMaxDistance;

    long        m_lFieldType;
    kMDtField();
    ~kMDtField();

    void Load(MObject MOField);
    virtual void ScaleForLinearUnits(float fUnitMultiplier);

    enum
    {
        GravityType,
        RadialType,
        VortexType,
        VolumeAxisType,
        UniformType,
        TurbulenceType,
        NewtonType,
        DragType,
        AirType
    };

protected:

    void SetFieldType();
    bool GetWorldAttributes(const char* szObject);
};
//---------------------------------------------------------------------------
class kMDtGravityField : public kMDtField
{

public:

    float       m_fDirectionX;
    float       m_fDirectionY;
    float       m_fDirectionZ;

    kMDtGravityField();
    ~kMDtGravityField();

    void Load(MObject MOField);

};
//---------------------------------------------------------------------------
class kMDtRadialField : public kMDtField
{
public:

    // Radial fields don't have any additional 
    // Data elements but this class was created
    // for consistancy.

    kMDtRadialField();
    ~kMDtRadialField();

    float m_fRadialType;

    void Load(MObject MOField);
};
//---------------------------------------------------------------------------
class kMDtVortexField : public kMDtField
{
public:


    float       m_fAxisX;
    float       m_fAxisY;
    float       m_fAxisZ;

    kMDtVortexField();
    ~kMDtVortexField();

    void Load(MObject MOField);
};
//---------------------------------------------------------------------------
class kMDtVolumeAxisField : public kMDtField
{
public:

    float   m_fDirectionX;
    float   m_fDirectionY;
    float   m_fDirectionZ;

    float   m_fSpeedAlongAxis;
    float   m_fSpeedAroundAxis;
    float   m_fSpeedAwayFromCenter;
    float   m_fDirectionalSpeed;

    kMDtVolumeAxisField();
    ~kMDtVolumeAxisField();

    void Load(MObject MOField);
};
//---------------------------------------------------------------------------
class kMDtUniformField : public kMDtField
{
public:
        
    float m_fDirectionX;
    float m_fDirectionY;
    float m_fDirectionZ;

    kMDtUniformField();
    ~kMDtUniformField();

    void Load(MObject MOField);
};
//---------------------------------------------------------------------------
class kMDtTurbulenceField : public kMDtField
{
public:
    float m_fFrequency;
    
    kMDtTurbulenceField();
    ~kMDtTurbulenceField();

    void Load(MObject MOField);
};
//---------------------------------------------------------------------------
class kMDtNewtonField : public kMDtField
{
public:

    float m_fMinDistance;

    kMDtNewtonField();
    ~kMDtNewtonField();

    void Load(MObject MOField);
};
//---------------------------------------------------------------------------
class kMDtDragField : public kMDtField
{
public:

    float m_fDirectionX;
    float m_fDirectionY;
    float m_fDirectionZ;

    bool m_bUseDirection;

    kMDtDragField();
    ~kMDtDragField();

    void Load(MObject MOField);
};
//---------------------------------------------------------------------------
class kMDtAirField : public kMDtField
{
public:

    float m_fDirectionX;
    float m_fDirectionY;
    float m_fDirectionZ;

    float m_fSpeed;
    float m_fInheritVelocity;
    bool m_bInheritRotation;
    bool m_bComponentOnly;

    float m_fSpread;
    bool m_bEnableSpread;

    kMDtAirField();
    ~kMDtAirField();

    void Load(MObject MOField);
};
//---------------------------------------------------------------------------

#endif