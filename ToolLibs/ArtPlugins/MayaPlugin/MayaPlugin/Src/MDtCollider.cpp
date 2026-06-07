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

// Precompiled Headers
#include "MayaPluginPCH.h"


//---------------------------------------------------------------------------
kMDtCollider::kMDtCollider()
{
    m_szName = NULL;
    m_MObjectParticles = MObject::kNullObj;
    m_MObjectColliderTransform = MObject::kNullObj;

    // Transform Attributes
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

    m_fWidth = 0.0f;
    m_fHeight = 0.0f;
    m_fRadius = 0.0f;

    m_fResilience = 0.0f;
    m_fFriction = 0.0f;
    m_kWorldRotationMatrix = NiMatrix3::IDENTITY;

}
//---------------------------------------------------------------------------
kMDtCollider::~kMDtCollider()
{
    if(m_szName != NULL)
    {
        NiFree(m_szName);
        m_szName = NULL;
    }
}
//---------------------------------------------------------------------------
bool kMDtCollider::Load(MObject MObjectParticleName, int iGeometryID)
{
    MStatus stat = MS::kSuccess;

    m_MObjectParticles = MObjectParticleName;


    MObject GeomConnection = FindConnectedDependency(m_MObjectParticles, 
        "collisionGeometry", iGeometryID);

    if(GeomConnection == MObject::kNullObj)
    {
        return false;
    }

    //Load the Resilience and Friction
    // Check To see if it is a sphere
    MFnDependencyNode GeomConnectionNode(GeomConnection);
    MPlug Plug = GeomConnectionNode.findPlug("resilience");

    if( Plug.isNull() )
    {
        return false;
    }
    if( Plug.getValue( m_fResilience ) != MS::kSuccess )
    {
        return false;
    }


    Plug = GeomConnectionNode.findPlug("friction");

    if( Plug.isNull() )
    {
        return false;
    }
    if( Plug.getValue( m_fFriction ) != MS::kSuccess )
    {
        return false;
    }


    MObject Shape = FindConnectedDependency(GeomConnection, "localGeometry" );

    if(Shape == MObject::kNullObj)
    {
        return false;
    }



    // Look for the Transform parented to this shape
    MFnDagNode kDagNode(Shape);

    if ((kDagNode.parentCount(&stat) == 0) ||
        (stat != MS::kSuccess))
    {
        return false;
    }

    m_MObjectColliderTransform = kDagNode.parent(0, &stat);

    if (stat != MS::kSuccess)
        return false;


    // Find the triangulation node, if it exists
    MObject kPoly = FindConnectedDependency(Shape, "inMesh" );

    if(kPoly != MObject::kNullObj)
    {
        MFnDependencyNode PolyShape(kPoly, &stat);

        if(stat != MS::kSuccess)
        {
            return false;
        }


        // Find the Width and Height from the PolyShape

        // Check To see if it is a sphere
        Plug = PolyShape.findPlug("radius");

        if( Plug.isNull() )
        {
            m_fRadius = 0.0f;
                       
            // Looking at a Plane?

            MPlug WidthPlug = PolyShape.findPlug("width");
            MPlug HeightPlug = PolyShape.findPlug("height");
            
            if( WidthPlug.isNull() ||
                HeightPlug.isNull())
            {
                // Default to a sphere representation of this data
                m_fRadius = 1.0f;
            }
            
            if ((WidthPlug.getValue( m_fWidth ) != MS::kSuccess ) ||
                (HeightPlug.getValue( m_fHeight ) != MS::kSuccess))
            {
                // Default to a sphere representation of this data
                m_fRadius = 1.0f;
            }
            
        }
        else
        {
            // Looking at a sphere ?
            if( Plug.getValue( m_fRadius ) != MS::kSuccess )
            {
                return false;
            }
        }
    }

    MFnDependencyNode PolyNode(Shape, &stat);
    if(stat != MS::kSuccess)
        return false;

    size_t stLen = strlen(PolyNode.name().asChar()) + 1;
    m_szName = NiAlloc(char, stLen);
    NiStrcpy(m_szName, stLen, PolyNode.name().asChar());

    CreateNameFromPolyName(m_szName, (unsigned int)stLen);

    if( !GetAttribute(m_szName, "translateX", m_fTranslateX) )
        return false;

    if( !GetAttribute(m_szName, "translateY", m_fTranslateY) )
        return false;

    if( !GetAttribute(m_szName, "translateZ", m_fTranslateZ) )
        return false;

    if( !GetAttribute(m_szName, "rotateX", m_fRotateX) )
        return false;

    if( !GetAttribute(m_szName, "rotateY", m_fRotateY) )
        return false;

    if( !GetAttribute(m_szName, "rotateZ", m_fRotateZ) )
        return false;

    if( !GetAttribute(m_szName, "scaleX", m_fScaleX) )
        return false;

    if( !GetAttribute(m_szName, "scaleY", m_fScaleY) )
        return false;

    if( !GetAttribute(m_szName, "scaleZ", m_fScaleZ) )
        return false;

    GetWorldAttributes(m_szName);

    return true;
}
//---------------------------------------------------------------------------
void kMDtCollider::ScaleForLinearUnits(float fUnitMultiplier)
{
    m_fTranslateX *= fUnitMultiplier;
    m_fTranslateY *= fUnitMultiplier;
    m_fTranslateZ *= fUnitMultiplier;
    m_fWorldTranslateX *= fUnitMultiplier;
    m_fWorldTranslateY *= fUnitMultiplier;
    m_fWorldTranslateZ *= fUnitMultiplier;
    m_fWidth *= fUnitMultiplier;
    m_fHeight *= fUnitMultiplier;
    m_fRadius *= fUnitMultiplier;
}
//---------------------------------------------------------------------------
MObject kMDtCollider::FindConnectedDependency(MObject CurrentDependency, 
    char* szPlugName, int iElement/* = -1*/)
{
    MStatus stat = MS::kSuccess;


    MFnDependencyNode StartNode(CurrentDependency, &stat);
    NIASSERT(stat == MS::kSuccess);

    
    // Get the input force
    MPlug Plug = StartNode.findPlug(szPlugName);  
    if( Plug.isNull() )
    {
        return MObject::kNullObj;
    }

    MPlug CurrentPlug = Plug;

        // Find the next plug if we have multiple plugs connected here
    if( iElement != -1)
    {
        int iNumConnectedElements = Plug.numConnectedElements(&stat);

        if((stat != MS::kSuccess) || (iNumConnectedElements <= iElement))
        {
            return MObject::kNullObj;
        }


        CurrentPlug = Plug.elementByPhysicalIndex(iElement, &stat);

        if(stat != MS::kSuccess)
        {
            return MObject::kNullObj;
        }
    }


    MPlugArray PlugConnections;

    CurrentPlug.connectedTo(PlugConnections, true, false, &stat);

    if(stat != MS::kSuccess)
    {
        return MObject::kNullObj;
    }

    int iNumPlugConnection = PlugConnections.length();
    NI_UNUSED_ARG(iNumPlugConnection);

    if(PlugConnections.length() == 1)
    {
        // Find the Geometry Connector we are connected to
        MPlug DestinationPlug = PlugConnections[0];
        MObject DestinationName = DestinationPlug.node(&stat);

        if(stat != MS::kSuccess)
        {
            return MObject::kNullObj;
        }

        MFnDependencyNode DestinationNode(DestinationName, &stat);

        if(stat != MS::kSuccess)
        {
            return MObject::kNullObj;
        }

        return DestinationName;
    }

    return MObject::kNullObj;
}
//---------------------------------------------------------------------------
void kMDtCollider::CreateNameFromPolyName(char* szName,
    unsigned int uiNameLen)
{
    //***  Find and remove "Shape" from the name ***
    char *szShape = strstr(szName,"Shape");
    NIASSERT(szShape != NULL);

        // Find the Last occurance of "Shape" in the string
    while( strstr(szShape+1, "Shape") != NULL)
    {
        szShape = strstr(szShape+1, "Shape");
    }

    *szShape = '\0';
    szShape += strlen("Shape");

    NiStrcat(szName, uiNameLen, szShape);
}
//---------------------------------------------------------------------------
bool kMDtCollider::GetAttribute(char* szObject, char* szAttribute, 
    float& fResult)
{
    MString   command;
    MDoubleArray result;
    MStatus stat = MS::kSuccess;

    command = MString("getAttr ") + MString(szObject) + MString(".") + 
        MString(szAttribute);

    stat = MGlobal::executeCommand( command, result );

    if ( stat != MS::kSuccess ) 
    {
        return false;
    }

    fResult = (float)result[0];
    
    return true;
}
//---------------------------------------------------------------------------
bool kMDtCollider::GetWorldAttributes(char* szObject)
{
    MString   command;
    MDoubleArray result;

    MStatus stat = MS::kSuccess;

    command = MString("getAttr ") + MString(szObject) + 
        MString(".worldMatrix") ;

    stat = MGlobal::executeCommand( command, result );

    if ( stat != MS::kSuccess ) 
    {
        return false;
    }

    int iLength = result.length();
    NI_UNUSED_ARG(iLength);

    double dDoubleArray[4][4];

    m_kWorldRotationMatrix;
    
    int iLoop = 0;
    for(int iXLoop = 0; iXLoop < 4; iXLoop++)
    {
        for(int iYLoop = 0; iYLoop < 4; iYLoop++)
        {
            if((iXLoop < 3) && (iYLoop < 3))
                m_kWorldRotationMatrix.SetEntry(iXLoop, iYLoop, 
                    (float)result[iLoop]);

            dDoubleArray[iXLoop][iYLoop] = result[iLoop++];
        }
    }

    m_kWorldRotationMatrix.ToEulerAnglesYXZ(m_fWorldRotateX, m_fWorldRotateY, 
        m_fWorldRotateZ);


    MMatrix WorldMatrix(dDoubleArray);
    MTransformationMatrix WorldTransform(WorldMatrix);


    // Get the World Translation for this object
    MVector Translate = WorldTransform.translation(MSpace::kTransform, &stat);

    if ( stat != MS::kSuccess ) 
    {
        return false;
    }

    m_fWorldTranslateX = (float)Translate[0];
    m_fWorldTranslateY = (float)Translate[1];
    m_fWorldTranslateZ = (float)Translate[2];

    // Get the World Scale for this object

    double dScales[3];

    if( WorldTransform.getScale( dScales, MSpace::kTransform)!= MS::kSuccess) 
    {
        return false;
    }

    m_fWorldScaleX = (float)dScales[0];
    m_fWorldScaleY = (float)dScales[1];
    m_fWorldScaleZ = (float)dScales[2];

    return true;
}


