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

// The data here in may be redundant while combined with the original
// MDt libs - however, this is organize with all objects in a centralized
// place where as the MDt libs separate out lights, shapes, joints, etc.
// into there own unique databases.

// Precompiled Headers
#include "MayaPluginPCH.h"
#include "kMDtObject.h"
#include <maya/MFnIkJoint.h>



// The global object list...
kTDynArray<kMDtObject> gMDtObjects;


//---------------------------------------------------------------------------
kMDtObject::kMDtObject( int iIndex, EType eType, MDagPath dp, MObject xnode, 
    MObject parentNode )
{
    m_pszName   = NULL;
    m_pszType   = NULL;
    m_iIndex    = -1;
    m_eType     = ETypeUnknown;

    //kMDtObject( iIndex, dp, xnode, parentNode );
    m_iIndex        = iIndex;
    m_DagPath       = dp;
    m_XformNode     = xnode;
    m_ParentNode    = parentNode;

    // Figure out eType and name from what we have to work with...
    m_pszType   = objectType( xnode );
    m_pszName   = objectName( xnode  );

    m_eType = eType;
}
//---------------------------------------------------------------------------
kMDtObject::kMDtObject( int iIndex,
    MDagPath dp, MObject xnode, MObject parentNode )
{
    m_pszName   = NULL;
    m_pszType   = NULL;
    m_iIndex    = -1;
    m_eType     = ETypeUnknown;

    m_iIndex        = iIndex;
    m_DagPath       = dp;
    m_XformNode     = xnode;
    m_ParentNode    = parentNode;

    // Figure out eType and name from what we have to work with...
    m_pszType   = objectType( xnode );
    m_pszName   = objectName( xnode  );
}
//---------------------------------------------------------------------------
kMDtObject::kMDtObject(void)
{
    m_pszName   = NULL;
    m_pszType   = NULL;
    m_iIndex    = -1;
    m_eType     = ETypeUnknown;
}
//---------------------------------------------------------------------------
kMDtObject::~kMDtObject(void)
{
    // m_pszName & m_pszType are const pointers, so don't need deletion
    // m_idynParents & m_idynChildren destructed as kMDtObject dies.
}
//---------------------------------------------------------------------------
EType kMDtObject::GetType(void)
{
    return m_eType;
}
//---------------------------------------------------------------------------
int kMDtObject::GetIndex(void)
{
    return m_iIndex;
}
//---------------------------------------------------------------------------
int kMDtObject::GetTransform(MObject &XformNode)
{
    XformNode = m_XformNode;
    return 1;
}
//---------------------------------------------------------------------------
int kMDtObject::GetJointOrientation( float *x, float *y, float *z)
{
    MTransformationMatrix::RotationOrder order = MTransformationMatrix::kXYZ;
    MFnIkJoint j(m_DagPath);
    double dO[3];
    j.getOrientation(dO,order);
    *x = (float)dO[0];
    *y = (float)dO[1];
    *z = (float)dO[2];
    return 1;
}
//---------------------------------------------------------------------------
int kMDtObject::GetRotateAxis( float *x, float *y, float *z)
{

    // Save the angular working units
    MString sAngularUnits;
    MStatus stat = MGlobal::executeCommand(MString("currentUnit -q -f -a;"), 
        sAngularUnits);

    if(stat != MS::kSuccess)
        return 0;

    // Set the working units to radians
    stat = MGlobal::executeCommand(MString("currentUnit -a rad;"));


    MFnTransform transFn( m_XformNode ); // Get the transform function set

    // Create the command to get our Attribute
    MString command = MString("getAttr ") + transFn.fullPathName(&stat) + 
        MString(".rotateAxis");

    if(stat != MS::kSuccess)
    {
        // Reset the working units
        stat = MGlobal::executeCommand(MString("currentUnit -a ") + 
            sAngularUnits + MString(";"));
        return 0;
    }


    MDoubleArray RotateAxisValues;
    stat = MGlobal::executeCommand(command, RotateAxisValues);

    if(stat != MS::kSuccess)
    {
        // Reset the working units
        stat = MGlobal::executeCommand(MString("currentUnit -a ") + 
            sAngularUnits + MString(";"));
        return 0;
    }

    *x = (float)RotateAxisValues[0];
    *y = (float)RotateAxisValues[1];
    *z = (float)RotateAxisValues[2];

        // Reset the working units
    stat = MGlobal::executeCommand(MString("currentUnit -a ") + 
        sAngularUnits + MString(";"));
    return 1;
}
//---------------------------------------------------------------------------
//  Added so that orientation is not factored.
int kMDtObject::GetRotationWOOrientation( float *x, float *y, float *z)
{
    MStatus stat;
    MTransformationMatrix::RotationOrder order = MTransformationMatrix::kXYZ;
    MFnTransform transFn( m_XformNode ); // Get the transform function set

    double dR[3];   // Rotation angles 

    // Get the Rotations values dR...
    stat = transFn.getRotation( dR, order, MSpace::kTransform );

    if ( stat == MS::kSuccess )
    {
        *x = (float)dR[0];
        *y = (float)dR[1];
        *z = (float)dR[2];
        
        return 1;
    }
    return 0;
}
//---------------------------------------------------------------------------
int kMDtObject::GetRotation( float *x, float *y, float *z)
{
    MStatus stat;
    MTransformationMatrix::RotationOrder order = MTransformationMatrix::kXYZ;
    MFnTransform transFn( m_XformNode ); // Get the transform function set

    MTransformationMatrix kMatrix = transFn.transformation(&stat);

    if(stat != MStatus::kSuccess)
        return(0);


    // Compress the transformation Matrix
    MMatrix kFinalRotationMatrix = kMatrix.asRotateMatrix();
    MTransformationMatrix kCompressedMatrix(kFinalRotationMatrix);


    double Rotations[3];
    stat = kCompressedMatrix.getRotation(Rotations, order);

    if(stat != MStatus::kSuccess)
        return (0);

    *x = (float)Rotations[0];
    *y = (float)Rotations[1];
    *z = (float)Rotations[2];

    return(1);
}
//---------------------------------------------------------------------------
int kMDtObject::GetPivot( float *x, float *y, float *z)
{
    // Get the Transform from the object.
    MStatus stat;
    MObject transformNode( m_XformNode );
    
    // Take the first dag path.
    //
    MFnDagNode fnTransNode( transformNode, &stat );
    MDagPath dagPath;
    stat = fnTransNode.getPath( dagPath );
    
    MFnDagNode fnDagPath( dagPath, &stat );

    MFnTransform transFn1( dagPath );

    MPoint rP = transFn1.rotatePivot( MSpace::kTransform, &stat );

    if ( stat == MS::kSuccess )
    {
        *x = (float)rP.x;
        *y = (float)rP.y;
        *z = (float)rP.z;
        return 1;
    }
    return(0);
}
//---------------------------------------------------------------------------
int kMDtObject::GetPivotTranslation( float *x, float *y, float *z)
{
    // Get the Transform from the object.
    MStatus stat;
    MObject transformNode( m_XformNode );
    
    // Take the first dag path.
    //
    MFnDagNode fnTransNode( transformNode, &stat );
    MDagPath dagPath;
    stat = fnTransNode.getPath( dagPath );
    
    MFnDagNode fnDagPath( dagPath, &stat );

    MFnTransform transFn1( dagPath );

    MPoint rP = transFn1.rotatePivotTranslation( MSpace::kTransform, &stat );

    if ( stat == MS::kSuccess )
    {
        *x = (float)rP.x;
        *y = (float)rP.y;
        *z = (float)rP.z;
        return 1;
    }
    return(0);
}
//---------------------------------------------------------------------------
int kMDtObject::GetTranslation( float *x, float *y, float *z)
{
    // Get the Transform from the object.
    MStatus stat;
    MObject transformNode( m_XformNode );
    
    // Take the first dag path.
    //
    MFnDagNode fnTransNode( transformNode, &stat );
    MDagPath dagPath;
    stat = fnTransNode.getPath( dagPath );
    
    MFnDagNode fnDagPath( dagPath, &stat );

    MFnTransform transFn1( dagPath );

    MVector rP = transFn1.translation( MSpace::kTransform, &stat );

    if ( stat == MS::kSuccess )
    {
        *x = (float)rP[0];
        *y = (float)rP[1];
        *z = (float)rP[2];
        return 1;
    }
    return(0);
}
//---------------------------------------------------------------------------
int kMDtObject::GetFinalTranslation( float *x, float *y, float *z)
{
    // Get the Transform from the object.
    MStatus stat;
    MFnTransform transFn( m_XformNode );

    MTransformationMatrix xform = transFn.transformation( &stat );
    if (stat == MS::kSuccess )
    {
        MMatrix m = xform.asMatrix();
        float fmat[4][4];
        m.get(fmat);
        
        *x = fmat[3][0];
        *y = fmat[3][1];
        *z = fmat[3][2];
        return 1;
    }
    return(0);
}
//---------------------------------------------------------------------------
int kMDtObject::GetFinalNonAnimatingNotUniformScale(float *x, float *y, 
    float *z)
{
    // The assumption about the parent is 
    // that there is only one... if there
    // are more, then, we still only grab the first.
    int iParent = m_idynParents.GetCopy(0);

    if (iParent != -1)
    {
        // Has a parent...
        gMDtObjectGetFinalNonAnimatingNotUniformScale( iParent, x, y, z);
    }


    if( AnimatedPlugOnTransform("scaleX") ||
        AnimatedPlugOnTransform("scaleY") ||
        AnimatedPlugOnTransform("scaleZ") )
    {
        return 1;
    }

    // Get the Transform from the object.
    MStatus stat;
    MFnTransform transFn( m_XformNode );
   
    double sP[3]; 

    stat = transFn.getScale( sP );
    if ( stat == MS::kSuccess )
    {
        // Only add in the Non Uniform Scale
        float fEpsilon = (float)sP[0] * 0.001f;
        
        if( !NiOptimize::CloseTo((float)sP[0], (float)sP[1], fEpsilon) || 
            !NiOptimize::CloseTo((float)sP[0], (float)sP[2], fEpsilon) )
        {
            *x *= (float)sP[0];
            *y *= (float)sP[1];
            *z *= (float)sP[2];
        }
        return 1;
    }   

    return(0);
}
//---------------------------------------------------------------------------
int kMDtObject::GetScale(float *x, float *y, float *z)
{
    // Get the Transform from the object.
    MStatus stat;
    MFnTransform transFn( m_XformNode );
   
    double sP[3]; 

    stat = transFn.getScale( sP );
    if ( stat == MS::kSuccess )
    {
        *x = (float)sP[0];
        *y = (float)sP[1];
        *z = (float)sP[2];
        
        return 1;
    }   

    return(0);
}
//---------------------------------------------------------------------------
int kMDtObject::GetScalePivot(float *x, float *y, float *z)
{
    // Get the Transform from the object.
    MStatus stat;
    MFnTransform transFn( m_XformNode );
   
    MPoint rP = transFn.scalePivot(MSpace::kTransform, &stat );

    if ( stat == MS::kSuccess )
    {
        *x = (float)rP.x;
        *y = (float)rP.y;
        *z = (float)rP.z;
        return 1;
    }

    return(0);
}
//---------------------------------------------------------------------------
int kMDtObject::GetScalePivotTranslation(float *x, float *y, float *z)
{
    // Get the Transform from the object.
    MStatus stat;
    MFnTransform transFn( m_XformNode );
   
    MPoint rP = transFn.scalePivotTranslation(MSpace::kTransform, &stat );

    if ( stat == MS::kSuccess )
    {
        *x = (float)rP.x;
        *y = (float)rP.y;
        *z = (float)rP.z;
        return 1;
    }

    return(0);
}
//---------------------------------------------------------------------------
bool kMDtObject::GetWorldMatrix(MMatrix &WorldMatrix)
{
    MStatus status;

        // Get the full path so we can uniquely identify the object
    size_t stLen = strlen(m_DagPath.fullPathName(&status).asChar()) + 1;
    char* szPathName = NiAlloc(char, stLen);

    if(status != MS::kSuccess)
    {
        NiFree(szPathName);
        return false;
    }

    NiStrcpy(szPathName, stLen, m_DagPath.fullPathName().asChar());

    if(status != MS::kSuccess)
    {
        NiFree(szPathName);
        return false;
    }

        // Get the world matrix
    MString command = MString("getAttr ") + MString(szPathName) + 
        MString(".worldMatrix");

    MDoubleArray result;

    status = MGlobal::executeCommand( command, result );

    if ( status != MS::kSuccess ) 
    {
        NiFree(szPathName);
        return false;
    }

        // Copy the linear array into a two dimmentional double array
    double dDoubleArray[4][4];
    
    int iLoop = 0;
    for(int iXLoop = 0; iXLoop < 4; iXLoop++)
    {
        for(int iYLoop = 0; iYLoop < 4; iYLoop++)
        {
            dDoubleArray[iXLoop][iYLoop] = result[iLoop++];
        }
    }

        // Contrstruct the Matrix
    MMatrix ObjectWorldMatrix(dDoubleArray);
    
        // Copy to our new results
    WorldMatrix = ObjectWorldMatrix;
    return true;
}
//---------------------------------------------------------------------------
bool kMDtObject::GetWorldMatrix(MTransformationMatrix &WorldMatrix)
{
    MStatus status;
    MMatrix Matrix;

    if(!GetWorldMatrix(Matrix))
        return 0;

    MTransformationMatrix WorldTransform(Matrix);

    WorldMatrix = WorldTransform;
    return true;
}
//---------------------------------------------------------------------------
int kMDtObject::GetWorldTranslate(float *x, float *y, float *z)
{
    MStatus status;

    MTransformationMatrix WorldTransform;

    if(!GetWorldMatrix(WorldTransform))
        return 0;


    // Get the World Translation for this object
    MVector Translate = 
        WorldTransform.translation(MSpace::kTransform, &status);

    if ( status != MS::kSuccess ) 
    {
        return false;
    }

    *x = (float)Translate[0];
    *y = (float)Translate[1];
    *z = (float)Translate[2];

    return 1;
}
//---------------------------------------------------------------------------
bool kMDtObject::AnimatedPlugOnTransform(char* szPlug)
{
    // Get a particular animcurve from a shape...
    MStatus status;
    MFnDependencyNode transformDependNode(m_XformNode, &status);

    if (status!=MS::kSuccess)
        return false;

    MPlug transPlug = transformDependNode.findPlug(szPlug, &status);

    if (status!=MS::kSuccess)
        return false;

    if (transPlug.isConnected()) 
    {
        MPlugArray plugArr;
        bool isConn = transPlug.connectedTo(plugArr, true, false, &status);
        NI_UNUSED_ARG(isConn);

        unsigned int length = plugArr.length();
        if (length != 1)
        {
            //if length > 1, plug assigned to multiple shapes?  Not supported
            NIASSERT(0);
            return false;
        }

        MPlug destPlug = plugArr[0];
        MObject destNodeObj = destPlug.node();
        if (destNodeObj.hasFn(MFn::kAnimCurve))
        {
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
const char *kMDtObject::GetName()
{
    return( objectName( m_XformNode ) );
}


//---------------------------------------------------------------------------
bool kMDtObject::IsObjectSelected()
{
    return( isObjectSelected( m_DagPath ) );
}
//---------------------------------------------------------------------------
bool kMDtObject::IsObjectVisible()
{
    return(isMObjectVisible(m_XformNode));
}


//---------------------------------------------------------------------------
// C WRAPPERS
//---------------------------------------------------------------------------
int gMDtObjectSetParents(void)
{
// This function was created because some nodes are processed out of order.
// The code use to only look to the left of the current processed node for
// the parent because it assumed parents would be processed first.  This
// isn't necessarly true with objects that are instanced(ie multiple parents)
// The code use to exist in gMDtObjectAdd, but has been moved here so that
// we wait until all nodes have been processed.
//
// Return value:
// 0: Success
// 1: Cancelled

    int iObjects = gMDtObjects.GetUsed();

    if(DtExt_HaveGui() == true)
    {
        MProgressWindow::setProgressStatus("Initializing Parents");
        MProgressWindow::setProgressRange(0, iObjects);
        MProgressWindow::setProgress(0);
    }

    for(int iObj=0; iObj < iObjects; iObj++)
    {
        if(DtExt_HaveGui() == true)
        {
            if (MProgressWindow::isCancelled())
                return 1;

            // Increment the progress window
            MProgressWindow::advanceProgress(1);
        }

        // for each object, find it's parent.

        // most parents will be to the left.
        kMDtObject *p = gMDtObjects.Get(iObj);
        bool bFoundAParent = false;

        for (int i=iObj-1;i>=0;i--)
        {
            kMDtObject *pParent = gMDtObjects.Get(i);
            if (pParent->m_XformNode == p->m_ParentNode)
            {
                // Then, we have found a parent.
                // Add it from both sides and break.
                int* pI = NiExternalNew int;
                *pI = iObj;
                int* pPI = NiExternalNew int;
                *pPI = i; 
                pParent->m_idynChildren.Add( pI );
                p->m_idynParents.Add( pPI );
                bFoundAParent = true;
                break;
            }
        }

        EType etype = p->GetType();
        if (etype == ETypeShape)
        {
            int iShapeNum = p->GetIndex();
            int iInstance = DtExt_ShapeIsInstanced( iShapeNum );
            
            if (iInstance != -1 && bFoundAParent == false)
            {
                // This is the code to check to the right of the parent -
                // It is unfortunate that we have to do this...
                // I believe we only need to check this direction if the
                // object in question happens to be instanced.
                for (int i=iObj+1;i<iObjects;i++)
                {
                    kMDtObject *pParent = gMDtObjects.Get(i);
                    if (pParent->m_XformNode == p->m_ParentNode)
                    {
                        // Then, we have found a parent.
                        // Add it from both sides and break.
                        int* pI = NiExternalNew int;
                        *pI = iObj;
                        int* pPI = NiExternalNew int;
                        *pPI = i; 
                        pParent->m_idynChildren.Add( pI );
                        p->m_idynParents.Add( pPI );
                        bFoundAParent = true;
                        break;
                    }
                }
            }

        }

        if (bFoundAParent == false)
        {
            // Then, say that the parent is -1 (just so we have an entry).
            int* pPI = NiExternalNew int;
            *pPI = -1;
            p->m_idynParents.Add( pPI );
        }
    }
    
    return 0;
}

//---------------------------------------------------------------------------
void gMDtObjectAdd( int iIndex, EType eType, MDagPath dp, MObject xnode, 
    MObject parentNode )
{
    kMDtObject *p = NiExternalNew kMDtObject(iIndex,eType,dp,xnode,
        parentNode );

    int iAddedIndex = gMDtObjects.Add(p);    
    NI_UNUSED_ARG(iAddedIndex);
}
//---------------------------------------------------------------------------
void gMDtObjectReset(void)
{
    gMDtObjects.Reset();
}
//---------------------------------------------------------------------------
int gMDtObjectGetUsed(void)
{
    return gMDtObjects.GetUsed();
}
//---------------------------------------------------------------------------
int gMDtObjectGetParentID( int iIndex )
{
    // Just return the first parent
    // Should be defaulted to -1;
    return (gMDtObjects.Get(iIndex))->m_idynParents.GetCopy(0);
}
//---------------------------------------------------------------------------
int gMDtObjectGetChildren(int iIndex, int *piNumChildren, int **ppiChildren)
{
    *piNumChildren = (gMDtObjects.Get(iIndex))->m_idynChildren.GetUsed();

    if (*piNumChildren < 1) 
        return false;

    // Allocate memory and return it in piChildren.
    *ppiChildren = NiAlloc(int, *piNumChildren);
    
    int i;
    for(i=0;i<*piNumChildren;i++)
        (*ppiChildren)[i] = 
            (gMDtObjects.Get(iIndex))->m_idynChildren.GetCopy(i);
    
    return true;
}
//---------------------------------------------------------------------------
EType gMDtObjectGetType(int iIndex)
{
    return (gMDtObjects.Get(iIndex))->GetType();
}
//---------------------------------------------------------------------------
int gMDtObjectGetTypeIndex(int iIndex)
{
    return (gMDtObjects.Get(iIndex))->GetIndex();
}
//---------------------------------------------------------------------------
int gMDtObjectGetTransform(int iComponentID, MObject &XformNode)
{
    return (gMDtObjects.Get(iComponentID))->GetTransform(XformNode);
}
//---------------------------------------------------------------------------
int gMDtObjectGetRotationWOOrientation( int iComponentID, float *x, float *y,
    float *z)
{
    return (gMDtObjects.Get(iComponentID))->GetRotationWOOrientation(x,y,z);
}
//---------------------------------------------------------------------------
int gMDtObjectGetRotation(int iComponentID, float *x, float *y, float *z)
{
    return (gMDtObjects.Get(iComponentID))->GetRotation(x,y,z);
}
//---------------------------------------------------------------------------
int gMDtObjectGetJointOrientation(int iComponentID, float *x, float *y, 
    float *z)
{
    return (gMDtObjects.Get(iComponentID))->GetJointOrientation(x,y,z);
}
//---------------------------------------------------------------------------
int gMDtObjectGetRotateAxis(int iComponentID, float *x, float *y, float *z)
{
    return (gMDtObjects.Get(iComponentID))->GetRotateAxis(x,y,z);
}
//---------------------------------------------------------------------------
int gMDtObjectGetPivot( int iComponentID, float *x, float *y, float *z)
{
    if(!(gMDtObjects.Get(iComponentID))->GetPivot(x,y,z))
        return 0;

    return 1;
}
//---------------------------------------------------------------------------
int gMDtObjectGetPivotTranslation( int iComponentID, float *x, float *y, 
    float *z)
{

    if(!(gMDtObjects.Get(iComponentID))->GetPivotTranslation(x,y,z))
        return 0;

    return 1;
}
//---------------------------------------------------------------------------
int gMDtObjectGetTranslation(int iComponentID, float *x, float *y, float *z)
{
    return (gMDtObjects.Get(iComponentID))->GetTranslation(x,y,z);
}
//---------------------------------------------------------------------------
int gMDtObjectGetFinalTranslation(int iComponentID, float *x, float *y, 
    float *z)
{
    return (gMDtObjects.Get(iComponentID))->GetFinalTranslation(x,y,z);
}
//---------------------------------------------------------------------------
int gMDtObjectGetFinalNonAnimatingNotUniformScale(int iComponentID, float *x, 
    float *y, float *z)
{
    // The first time this function is called, user should set x=y=z=1
    return (gMDtObjects.Get(iComponentID))->
                GetFinalNonAnimatingNotUniformScale(x,y,z);
}
//---------------------------------------------------------------------------
int gMDtObjectGetScale(int iComponentID, float *x, float *y, float *z)
{
    return (gMDtObjects.Get(iComponentID))->GetScale(x,y,z);
}
//---------------------------------------------------------------------------
int gMDtObjectGetScalePivot(int iComponentID, float *x, float *y, float *z)
{
    return (gMDtObjects.Get(iComponentID))->GetScalePivot(x,y,z);
}
//---------------------------------------------------------------------------
int gMDtObjectGetScalePivotTranslation(int iComponentID, float *x, float *y, 
    float *z)
{
    return (gMDtObjects.Get(iComponentID))->GetScalePivotTranslation(x,y,z);
}
//---------------------------------------------------------------------------
bool gMDtObjectGetAnimatedScale(int iComponentID)
{
return((gMDtObjects.Get(iComponentID))->AnimatedPlugOnTransform("scaleX") ||
       (gMDtObjects.Get(iComponentID))->AnimatedPlugOnTransform("scaleY") ||
       (gMDtObjects.Get(iComponentID))->AnimatedPlugOnTransform("scaleZ") );
}
//---------------------------------------------------------------------------
int gMDtObjectFindComponentID(EType eType, int iTypeID)
{
    int num=gMDtObjects.GetUsed();

    // function looks for the componeID for specified type:id
    for(int i=0; i<num; i++)
    {
        if (gMDtObjectGetType(i) == eType)
        {
            if (gMDtObjectGetTypeIndex(i) == iTypeID)
                return i;
        }
    }
    return -1;
}
//---------------------------------------------------------------------------
int gMDtObjectFindComponentIDByNode(MObject XNode)
{
    int num=gMDtObjects.GetUsed();

    // function looks for the componeID for specified type:id
    for(int i=0; i<num; i++)
    {
        if((gMDtObjects.Get(i))->m_XformNode == XNode)
        {
            return i;
        }
    }
    return -1;
}
//---------------------------------------------------------------------------
int gMDtObjectGetWorldTranslate(int iComponentID, float *x, float *y, 
    float *z)
{
    return (gMDtObjects.Get(iComponentID))->GetWorldTranslate(x,y,z);
}
//---------------------------------------------------------------------------

// *** DREW
const char *gMDtObjectGetName( int iComponentID )
{
    return ( gMDtObjects.Get(iComponentID)->GetName() );
}

//---------------------------------------------------------------------------
bool gMDtObjectIsSelected(int iComponentID)
{
    return (gMDtObjects.Get(iComponentID))->IsObjectSelected();
}
//---------------------------------------------------------------------------
bool gMDtObjectIsVisible(int iComponentID)
{
    return (gMDtObjects.Get(iComponentID))->IsObjectVisible();
}
//---------------------------------------------------------------------------
  