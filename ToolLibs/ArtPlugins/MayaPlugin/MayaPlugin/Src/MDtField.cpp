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

// MDtField.cpp


// Precompiled Headers
#include "MayaPluginPCH.h"



//---------------------------------------------------------------------------
//
//      Field Super Class
//
//---------------------------------------------------------------------------
kMDtField::kMDtField()
{
    m_szName = NULL;
    m_MObjectField = MObject::kNullObj;

    // Translate Attributes
    m_fTranslateX = 0.0f;
    m_fTranslateY = 0.0f;
    m_fTranslateZ = 0.0f;
    m_fRotateX = 0.0f;
    m_fRotateY = 0.0f;
    m_fRotateZ = 0.0f;
    m_fScaleX = 0.0f;
    m_fScaleY = 0.0f;
    m_fScaleZ = 0.0f;

    m_fWorldTranslateX = 0.0f;
    m_fWorldTranslateY = 0.0f;
    m_fWorldTranslateZ = 0.0f;
    m_fWorldRotateX = 0.0f;
    m_fWorldRotateY = 0.0f;
    m_fWorldRotateZ = 0.0f;
    m_fWorldScaleX = 0.0f;
    m_fWorldScaleY = 0.0f;
    m_fWorldScaleZ = 0.0f;

    m_fMagnitude = 0.0f;
    m_fAttenuation = 0.0f;
    m_bUseMaxDistance = false;
    m_fMaxDistance = 0.0f;

    m_lFieldType = -1;
}
//---------------------------------------------------------------------------
kMDtField::~kMDtField()
{
    if(m_szName != NULL)
    {
        NiFree(m_szName);
        m_szName = NULL;
    }
}
//---------------------------------------------------------------------------
void kMDtField::Load(MObject MOField)
{
    MStatus stat = MS::kSuccess;

    m_MObjectField = MOField;

    MFnDependencyNode FieldNode(m_MObjectField, &stat);

    NIASSERT(stat == MS::kSuccess);

    MFnDagNode NewFieldNode(m_MObjectField, &stat);
    NIASSERT(stat == MS::kSuccess);


    if(!GetWorldAttributes(NewFieldNode.fullPathName().asChar()))
        return;


    // Create the Name buffer
    size_t stNameLength = strlen( FieldNode.name().asChar() ) + 1;

    m_szName = NiAlloc(char, stNameLength);
    
    NiStrcpy( m_szName, stNameLength, FieldNode.name().asChar() );

    // ----------------------------------------------------------
    MPlug Plug = FieldNode.findPlug("translateX");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fTranslateX ) != MS::kSuccess )
        printf("Error Reading translateX\n");

    // ----------------------------------------------------------
    Plug = FieldNode.findPlug("translateY");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fTranslateY ) != MS::kSuccess )
        printf("Error Reading translateY\n");

    // ----------------------------------------------------------
    Plug = FieldNode.findPlug("translateZ");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fTranslateZ ) != MS::kSuccess )
        printf("Error Reading translateZ\n");

    // ----------------------------------------------------------
    Plug = FieldNode.findPlug("rotateX");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fRotateX ) != MS::kSuccess )
        printf("Error Reading rotateX\n");

    // ----------------------------------------------------------
    Plug = FieldNode.findPlug("rotateY");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fRotateY ) != MS::kSuccess )
        printf("Error Reading rotateY\n");

    // ----------------------------------------------------------
    Plug = FieldNode.findPlug("rotateZ");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fRotateZ ) != MS::kSuccess )
        printf("Error Reading rotateZ\n");

    // ----------------------------------------------------------
    Plug = FieldNode.findPlug("scaleX");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fScaleX ) != MS::kSuccess )
        printf("Error Reading scaleX\n");

    // ----------------------------------------------------------
    Plug = FieldNode.findPlug("scaleY");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fScaleY ) != MS::kSuccess )
        printf("Error Reading scaleY\n");

    // ----------------------------------------------------------
    Plug = FieldNode.findPlug("scaleZ");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fScaleZ ) != MS::kSuccess )
        printf("Error Reading scaleZ\n");

    // ----------------------------------------------------------

    Plug = FieldNode.findPlug("magnitude");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fMagnitude ) != MS::kSuccess )
        printf("Error Reading magnitude\n");

    // ----------------------------------------------------------

    Plug = FieldNode.findPlug("attenuation");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fAttenuation ) != MS::kSuccess )
        printf("Error Reading attenuation\n");

    // ----------------------------------------------------------

    Plug = FieldNode.findPlug("useMaxDistance");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bUseMaxDistance ) != MS::kSuccess )
        printf("Error Reading useMaxDistance\n");

    // ----------------------------------------------------------

    Plug = FieldNode.findPlug("maxDistance");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fMaxDistance ) != MS::kSuccess )
        printf("Error Reading maxDistance\n");

    // ----------------------------------------------------------

    SetFieldType();

}
//---------------------------------------------------------------------------
void kMDtField::ScaleForLinearUnits(float fUnitMultiplier)
{
    // Translate Attributes
    m_fTranslateX *= fUnitMultiplier;
    m_fTranslateY *= fUnitMultiplier;
    m_fTranslateZ *= fUnitMultiplier;

    m_fWorldTranslateX *= fUnitMultiplier;
    m_fWorldTranslateY *= fUnitMultiplier;
    m_fWorldTranslateZ *= fUnitMultiplier;

    m_fMagnitude *= fUnitMultiplier;
    m_fAttenuation *= fUnitMultiplier;
    m_fMaxDistance *= fUnitMultiplier;
}
//---------------------------------------------------------------------------
bool kMDtField::GetWorldAttributes(const char* szObject)
{
    MString   command;
    MDoubleArray result;

    MStatus stat = MS::kSuccess;

    command = MString("getAttr ") + MString(szObject) + 
        MString(".worldMatrix");

    stat = MGlobal::executeCommand( command, result );

    if ( stat != MS::kSuccess ) 
    {
        return false;
    }


    double dDoubleArray[4][4];
    
    int iLoop = 0;
    for(int iXLoop = 0; iXLoop < 4; iXLoop++)
    {
        for(int iYLoop = 0; iYLoop < 4; iYLoop++)
        {
            dDoubleArray[iXLoop][iYLoop] = result[iLoop++];
        }
    }


    MMatrix WorldMatrix(dDoubleArray);
    MTransformationMatrix WorldTransform(WorldMatrix);


    // Get the World Translation for this object
    MVector Translate = 
        WorldTransform.translation(MSpace::kTransform, &stat);

    if ( stat != MS::kSuccess ) 
    {
        return false;
    }

    m_fWorldTranslateX = (float)Translate[0];
    m_fWorldTranslateY = (float)Translate[1];
    m_fWorldTranslateZ = (float)Translate[2];

    // Get the World Rotations for this object

    double dRotations[3];
    MTransformationMatrix::RotationOrder Order = MTransformationMatrix::kXYZ;

    if( WorldTransform.getRotation( dRotations, Order) != MS::kSuccess ) 
    {
        return false;
    }

    m_fWorldRotateX = (float)dRotations[0];
    m_fWorldRotateY = (float)dRotations[1];
    m_fWorldRotateZ = (float)dRotations[2];


    // Get the World Scale for this object

    double dScales[3];

    if( WorldTransform.getScale( dScales, MSpace::kTransform) != 
        MS::kSuccess ) 
    {
        return false;
    }

    m_fWorldScaleX = (float)dScales[0];
    m_fWorldScaleY = (float)dScales[1];
    m_fWorldScaleZ = (float)dScales[2];
    return true;
}
//---------------------------------------------------------------------------
void kMDtField::SetFieldType()
{

    // ****************************************************
    // ****************************************************
    //
    //      IMPORTANT: This will find cannot distinguish
    //                  between Gravity and Uniform fields
    //
    // ****************************************************
    // ****************************************************


    NIASSERT(m_MObjectField != MObject::kNullObj);

    MStatus stat = MS::kSuccess;

    MFnDependencyNode FieldNode(m_MObjectField, &stat);

    NIASSERT(stat == MS::kSuccess);


    // ----------------------------------------------------------
    MPlug Plug = FieldNode.findPlug("axisX");  
    if(!Plug.isNull())
    {
        m_lFieldType = kMDtField::VortexType;
        return;
    }

    // ----------------------------------------------------------
    Plug = FieldNode.findPlug("directionalSpeed");
    if(!Plug.isNull())
    {
        m_lFieldType = kMDtField::VolumeAxisType;
        return;
    }

    // ----------------------------------------------------------
    Plug = FieldNode.findPlug("frequency");
    if(!Plug.isNull())
    {
        m_lFieldType = kMDtField::TurbulenceType;
        return;
    }

    // ----------------------------------------------------------
    Plug = FieldNode.findPlug("minDistance");
    if(!Plug.isNull())
    {
        m_lFieldType = kMDtField::NewtonType;
        return;
    }

    // ----------------------------------------------------------
    Plug = FieldNode.findPlug("spread");
    if(!Plug.isNull())
    {
        m_lFieldType = kMDtField::AirType;
        return;
    }

    // ----------------------------------------------------------
    Plug = FieldNode.findPlug("useDirection");
    if(!Plug.isNull())
    {
        m_lFieldType = kMDtField::DragType;
        return;
    }

    // ----------------------------------------------------------
    Plug = FieldNode.findPlug("directionX");
    if(!Plug.isNull())
    {
        m_lFieldType = kMDtField::GravityType;
        return;
    }


    m_lFieldType = kMDtField::RadialType;
}
//---------------------------------------------------------------------------
//
//      Gravity Field
//
//---------------------------------------------------------------------------
kMDtGravityField::kMDtGravityField()
{
}
//---------------------------------------------------------------------------
kMDtGravityField::~kMDtGravityField()
{
}
//---------------------------------------------------------------------------
void kMDtGravityField::Load(MObject MOField)
{
    // Load the Super Class
    kMDtField::Load(MOField);


    MStatus stat = MS::kSuccess;

    // Get the direction of gravity
    MFnDependencyNode FieldNode(m_MObjectField, &stat);

    NIASSERT(stat == MS::kSuccess);

    // ----------------------------------------------------------

    MPlug Plug = FieldNode.findPlug("directionX");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fDirectionX ) != MS::kSuccess )
        printf("Error Reading directionX\n");

    // ----------------------------------------------------------

    Plug = FieldNode.findPlug("directionY");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fDirectionY ) != MS::kSuccess )
        printf("Error Reading directionY\n");

    // ----------------------------------------------------------

    Plug = FieldNode.findPlug("directionZ");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fDirectionZ ) != MS::kSuccess )
        printf("Error Reading directionZ\n");

}
//---------------------------------------------------------------------------
//
//      Radial Field
//
//---------------------------------------------------------------------------
kMDtRadialField::kMDtRadialField()
{
    m_fRadialType = 0.0f;
}
//---------------------------------------------------------------------------
kMDtRadialField::~kMDtRadialField()
{
}
//---------------------------------------------------------------------------
void kMDtRadialField::Load(MObject MOField)
{
    // Load the Super Class
    kMDtField::Load(MOField);

    MStatus stat = MS::kSuccess;

    // Get the radial Type
    MFnDependencyNode FieldNode(m_MObjectField, &stat);

    NIASSERT(stat == MS::kSuccess);

    // ----------------------------------------------------------

    MPlug Plug = FieldNode.findPlug("radialType");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fRadialType ) != MS::kSuccess )
        printf("Error Reading radialType\n");

}
//---------------------------------------------------------------------------
//
//      Vortex Field
//
//---------------------------------------------------------------------------
kMDtVortexField::kMDtVortexField()
{
}
//---------------------------------------------------------------------------
kMDtVortexField::~kMDtVortexField()
{
}
//---------------------------------------------------------------------------
void kMDtVortexField::Load(MObject MOField)
{
    // Load the Super Class
    kMDtField::Load(MOField);


    MStatus stat = MS::kSuccess;

    // Get the direction of gravity
    MFnDependencyNode VortexNode(m_MObjectField, &stat);

    NIASSERT(stat == MS::kSuccess);

    // ----------------------------------------------------------

    MPlug Plug = VortexNode.findPlug("axisX");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fAxisX ) != MS::kSuccess )
        printf("Error Reading axisX\n");

    // ----------------------------------------------------------

    Plug = VortexNode.findPlug("axisY");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fAxisY ) != MS::kSuccess )
        printf("Error Reading axisY\n");

    // ----------------------------------------------------------

    Plug = VortexNode.findPlug("axisZ");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fAxisZ ) != MS::kSuccess )
        printf("Error Reading axisZ\n");
}
//---------------------------------------------------------------------------
//
//      Volume Axis Field
//
//---------------------------------------------------------------------------
kMDtVolumeAxisField::kMDtVolumeAxisField()
{
}
//---------------------------------------------------------------------------
kMDtVolumeAxisField::~kMDtVolumeAxisField()
{
}
//---------------------------------------------------------------------------
void kMDtVolumeAxisField::Load(MObject MOField)
{
    kMDtField::Load(MOField);

    printf("Error:: Volume Axis Field: %s Unsupported within Gamebryo.\n", 
        m_szName );
}
//---------------------------------------------------------------------------
//
//      Uniform Field
//
//---------------------------------------------------------------------------
kMDtUniformField::kMDtUniformField()
{
}
//---------------------------------------------------------------------------
kMDtUniformField::~kMDtUniformField()
{
}
//---------------------------------------------------------------------------
void kMDtUniformField::Load(MObject MOField)
{
    kMDtField::Load(MOField);

    printf("Error:: Uniform Field: %s Unsupported within Gamebryo.\n", 
        m_szName );
}
//---------------------------------------------------------------------------
//
//      Turbulence Field
//
//---------------------------------------------------------------------------
kMDtTurbulenceField::kMDtTurbulenceField()
{
}
//---------------------------------------------------------------------------
kMDtTurbulenceField::~kMDtTurbulenceField()
{
}
//---------------------------------------------------------------------------
void kMDtTurbulenceField::Load(MObject MOField)
{
    kMDtField::Load(MOField);

    MStatus stat = MS::kSuccess;

    // Get the direction of gravity
    MFnDependencyNode FieldNode(m_MObjectField, &stat);

    NIASSERT(stat == MS::kSuccess);

    MPlug Plug = FieldNode.findPlug("frequency");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fFrequency ) != MS::kSuccess )
        printf("Error Reading frequency\n");
    
}
//---------------------------------------------------------------------------
//
//      Newton Field
//
//---------------------------------------------------------------------------
kMDtNewtonField::kMDtNewtonField()
{
}
//---------------------------------------------------------------------------
kMDtNewtonField::~kMDtNewtonField()
{
}
//---------------------------------------------------------------------------
void kMDtNewtonField::Load(MObject MOField)
{
    kMDtField::Load(MOField);

    printf("Error:: Newton Field: %s Unsupported within Gamebryo.\n", 
        m_szName );
}
//---------------------------------------------------------------------------
//
//      Drag Field
//
//---------------------------------------------------------------------------
kMDtDragField::kMDtDragField()
{
}
//---------------------------------------------------------------------------
kMDtDragField::~kMDtDragField()
{
}
//---------------------------------------------------------------------------
void kMDtDragField::Load(MObject MOField)
{
    // Load the Super Class
    kMDtField::Load(MOField);


    MStatus stat = MS::kSuccess;

    // Get the direction of drag
    MFnDependencyNode FieldNode(m_MObjectField, &stat);

    NIASSERT(stat == MS::kSuccess);

    MPlug Plug = FieldNode.findPlug("useDirection");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bUseDirection ) != MS::kSuccess )
        printf("Error Reading directionZ\n");

    // ----------------------------------------------------------

    Plug = FieldNode.findPlug("directionX");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fDirectionX ) != MS::kSuccess )
        printf("Error Reading directionX\n");

    // ----------------------------------------------------------

    Plug = FieldNode.findPlug("directionY");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fDirectionY ) != MS::kSuccess )
        printf("Error Reading directionY\n");

    // ----------------------------------------------------------

    Plug = FieldNode.findPlug("directionZ");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fDirectionZ ) != MS::kSuccess )
        printf("Error Reading directionZ\n");

}
//---------------------------------------------------------------------------
//
//      Air Field
//
//---------------------------------------------------------------------------
kMDtAirField::kMDtAirField()
{
}
//---------------------------------------------------------------------------
kMDtAirField::~kMDtAirField()
{
}
//---------------------------------------------------------------------------
void kMDtAirField::Load(MObject MOField)
{
    kMDtField::Load(MOField);

    MStatus stat = MS::kSuccess;

    // Get the direction of drag
    MFnDependencyNode FieldNode(m_MObjectField, &stat);

    NIASSERT(stat == MS::kSuccess);

    // ----------------------------------------------------------

    MPlug Plug = FieldNode.findPlug("directionX");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fDirectionX ) != MS::kSuccess )
        printf("Error Reading directionX\n");

    // ----------------------------------------------------------

    Plug = FieldNode.findPlug("directionY");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fDirectionY ) != MS::kSuccess )
        printf("Error Reading directionY\n");

    // ----------------------------------------------------------

    Plug = FieldNode.findPlug("directionZ");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fDirectionZ ) != MS::kSuccess )
        printf("Error Reading directionZ\n");

    // ----------------------------------------------------------

    Plug = FieldNode.findPlug("speed");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fSpeed ) != MS::kSuccess )
        printf("Error Reading Speed\n");

    // ----------------------------------------------------------

    Plug = FieldNode.findPlug("inheritVelocity");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fInheritVelocity ) != MS::kSuccess )
        printf("Error Reading Inherit Velocity\n");

    // ----------------------------------------------------------

    Plug = FieldNode.findPlug("inheritRotation");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bInheritRotation ) != MS::kSuccess )
        printf("Error Reading Inherit Rotation\n");

    // ----------------------------------------------------------

    Plug = FieldNode.findPlug("componentOnly");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bComponentOnly ) != MS::kSuccess )
        printf("Error Reading Component Only\n");

    // ----------------------------------------------------------

    Plug = FieldNode.findPlug("enableSpread");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bEnableSpread ) != MS::kSuccess )
        printf("Error Reading Enable Spread\n");

    // ----------------------------------------------------------

    Plug = FieldNode.findPlug("spread");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fSpread ) != MS::kSuccess )
        printf("Error Reading Spread\n");

}





