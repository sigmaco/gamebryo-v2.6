// Copyright (C) 1998-2007 Alias | Wavefront, a division of Silicon Graphics 
// Limited.
//
// The information in this file is provided for the exclusive use of the
// licensees of Alias | Wavefront.  Such users have the right to use, modify,
// and incorporate this code into other products for purposes authorized
// by the Alias | Wavefront license agreement, without fee.
//
// ALIAS | WAVEFRONT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
// INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
// EVENT SHALL ALIAS | WAVEFRONT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
// CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
// DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

//
// $Revision: /main/13 $
// $Date: 2000/04/19 15:29:15 $
//

// Precompiled Headers
#include "MayaPluginPCH.h"
#include <maya/MFnCamera.h>



// defines, macros, and magic numbers
//

// Used to limit debug output to make it easier to read

#define DT_BLOCK_SIZE       16

#define DT_VALID_BIT_MASK 0x00FFFFFF

#define RADIAN_TO_DEGREE    180.0/3.141592654
#define DEGREE_TO_RADIAN    3.141592654/180.0

// The camera table item structure:
//
typedef struct
{
    int                     valid_bits; // the valid bits for this camera
    int                     type;       // camera type id for fast lookup
    MObject                 cameraShapeNode;
    MObject                 transformNode;
} CameraStruct;

// The camera object instance data structure:
//
typedef struct
{
    int             camera_ct;  // count of cameras
    CameraStruct*   cameras;    // array of CameraStruct's
} DtPrivate;


extern const char *objectName( MObject obj );

// Private data:
//
static DtPrivate*   local = NULL;

// Private function prototypes:
//
// static void  extract_xyz(AlTM inMat,float &x,float &y,float &z);

//======================================
// Get angle of a 2D vector     
//======================================

float DtGetAngle2D( double LXPos, double LYPos )
{
    float   LAtnVal = 0.0;

    if( 0.0 == LXPos )
    {
        if( 0.0 == LYPos ) 
        {
            return(0.0);
        }
        if( 0.0 < LYPos ) 
        {
            return(90.0);
        }
        if( 0.0 > LYPos ) 
        {
            return(-90.0);
        }
    }
    else
    {
        if( 0.0 == LYPos )
        {
            if( 0.0 < LXPos ) 
            {
                return(0.0);
            }
            if( 0.0 > LXPos ) 
            {
                return(180.0);
            }
        }
        else
        {
            LAtnVal = (float)(RADIAN_TO_DEGREE * atan( LYPos / LXPos ));
            if( 0.0 < LXPos ) 
            {
                return( LAtnVal );
            }
            else
            {
                if( 0.0 < LYPos ) 
                {
                    return( 180.0f + LAtnVal );
                }
                else
                {
                    if( 0.0 > LYPos ) 
                    {
                        return( -180.0f + LAtnVal );
                    }
                    else 
                    {
                        return( 180.0f );
                    }
                }
            }
        }
    }
    return( 0.0 );
}


/*==========================================================================*/
/*------------------------------  PUBLIC  FUNCTIONS  -----------------------*/
/*==========================================================================*/

// MObject accessors for both the Transform node and the Shape node
// defined in MDtExt.h
int DtExt_CameraGetTransform(int cameraID, MObject &obj )
{
    if ((cameraID < 0) || (cameraID >= local->camera_ct))
    {
        obj = MObject::kNullObj;
        return 0;  // error, camera does not exist
    }

    obj = local->cameras[cameraID].transformNode;
    
    return 1;
}

int DtExt_CameraGetShapeNode(int cameraID, MObject &obj )
{
    if ((cameraID < 0) || (cameraID >= local->camera_ct))
    {
        obj = MObject::kNullObj;
        return 0;  // error, camera does not exist
    }

    obj = local->cameras[cameraID].cameraShapeNode;
    
    return 1;
}

//  ========== DtCameraGetCount ==========
//
//  SYNOPSIS
//  Return the number of cameras in the scene graph.
//
int DtCameraGetCount( int* count )
{

    // Make sure cameras have been loaded.
    //
    if( NULL == local->cameras )
    {
        *count = 0;
        return( 0 );
    }

    // Return camera count.
    //
    *count = local->camera_ct;
    return( 1 );
}  // DtCameraGetCount //


//  ========== DtCameraGetType ==========
//
//  SYNOPSIS
//  Returns the camera type, perspective or orthographic.
//
int DtCameraGetType( int cameraID,
                     int* type )
{
    // Check for error.
    //
    if( ( cameraID < 0 ) ||
        ( cameraID >= local->camera_ct ) )
    {
        *type = 0;
        return( 0 );
    }

    // Return value.
    //
    *type = local->cameras[cameraID].type;
    return(1);
}  // DtCameraGetType //


//  ========== DtCameraGetName ==========
//
//  SYNOPSIS
//  Returns the camera name.
//
int DtCameraGetName( int cameraID, char** name )
{
    // check for error
    //
    if( (cameraID < 0) || (cameraID >= local->camera_ct) )
    {
        *name = NULL;
        return(0);
    }

    // may want to change the object that we use to get the name of the
    // camera.  for now lets use the transform node - 

    *name = (char *)objectName( local->cameras[cameraID].transformNode );
    
    return(1);

}  // DtCameraGetName //


//  ========== DtCameraGetPosition ==========
//
//  SYNOPSIS
//  Return the camera position.
//
int DtCameraGetPosition( int cameraID,
                         float* xTran,
                         float* yTran,
                         float* zTran )
{
    // Check for error.
    //
    if( (cameraID < 0) || (cameraID >= local->camera_ct ) )
    {
        *xTran = 0.0;
        *yTran = 0.0;
        *zTran = 0.0;
        return( 0 );
    }

    MStatus stat = MS::kSuccess;

    MFnDagNode fnDagNode( local->cameras[cameraID].transformNode, &stat );
    MDagPath aPath;
    stat = fnDagNode.getPath( aPath );
    
    // Get the global transformation matrix of the camera node.
    //
    MMatrix globalMatrix = aPath.inclusiveMatrix();

    MFnCamera fnCamera( local->cameras[cameraID].cameraShapeNode, &stat );

    MPoint eyePt;
    double eyePos[4]; 

    if( MS::kSuccess == stat )
    {
        eyePt = fnCamera.eyePoint( MSpace::kObject, &stat );

        if( DtExt_Debug() & DEBUG_CAMERA )
        {
            cerr << "eye point is at " 
                 << eyePt.x << " " << eyePt.y << " " << eyePt.z << endl;
        }

        eyePt *= globalMatrix;
        eyePt.get( eyePos );

    }
    else
    {
        DtExt_Err( "Error in getting the camera function set\n" );
    }

    // Return values:
    //
    *xTran = (float)eyePos[0];
    *yTran = (float)eyePos[1];
    *zTran = (float)eyePos[2];

    return( 1 );
}  // DtCameraGetPosition //


//  ========== DtCameraGetInterest ==========
//
//  SYNOPSIS
//  Return the camera position.
//
int DtCameraGetInterest(int cameraID,float* xTran,float* yTran,float* zTran)
{
    // check for error
    //

    if ( (cameraID < 0) || (cameraID >= local->camera_ct) )
    {
        *xTran = 0.0;
        *yTran = 0.0;
        *zTran = 0.0;
        return(0);
    }
 
    // return values
    //
    
    MStatus stat = MS::kSuccess;

    MFnDagNode fnDagNode( local->cameras[cameraID].transformNode, &stat );
    MDagPath aPath;
    stat = fnDagNode.getPath( aPath );

    // Get the global transformation matrix of the camera node.
    //
    MMatrix globalMatrix = aPath.inclusiveMatrix();

    MFnCamera fnCamera( local->cameras[cameraID].cameraShapeNode, &stat );

    // Center of interest point: view node point.
    //
    MPoint coiPoint = fnCamera.centerOfInterestPoint( MSpace::kObject, &stat );
    double coiPos[4]; 
    if( DtExt_Debug() & DEBUG_CAMERA )
    {
        coiPoint.get( coiPos );
        cerr << "local center of interest( view point position ) is " <<
            coiPos[0] << " " << coiPos[1] << " " << coiPos[2] << " "  
             << coiPos[3] << endl;
    }        
    coiPoint *= globalMatrix;
    coiPoint.get( coiPos );

    *xTran = (float)coiPos[0];
    *yTran = (float)coiPos[1];
    *zTran = (float)coiPos[2];
 
    return(1);

}  // DtCameraGetInterest //

//  ========== DtCameraGetUpPosition ==========
//
//  SYNOPSIS
//  Return the camera Up position.
//
int DtCameraGetUpPosition( int cameraID,
                         float* xTran,
                         float* yTran,
                         float* zTran )
{                        
    // Check for error.  
    //
    if( (cameraID < 0) || (cameraID >= local->camera_ct ) )
    {
        *xTran = 0.0;
        *yTran = 0.0;
        *zTran = 0.0;
        return( 0 );
    }   
    
    MStatus stat = MS::kSuccess;
    
    MFnDagNode fnDagNode( local->cameras[cameraID].transformNode, &stat );
    MDagPath aPath;
    stat = fnDagNode.getPath( aPath );
    
    // Get the global transformation matrix of the camera node.
    //
    MMatrix globalMatrix = aPath.inclusiveMatrix();
    
    MFnCamera fnCamera( local->cameras[cameraID].cameraShapeNode, &stat );
    
    MPoint eyePt;
    double eyePos[4];
    double upPos[4];
    
    if( MS::kSuccess == stat )
    {
        eyePt = fnCamera.eyePoint( MSpace::kObject, &stat );
        
        eyePt *= globalMatrix;
        eyePt.get( eyePos );

        MVector upVec = fnCamera.upDirection( MSpace::kObject, &stat );
        upVec *= globalMatrix;
        
        if( DtExt_Debug() & DEBUG_CAMERA )
        {
            double up[3];
            upVec.get( up );
            cerr << "local up vector is " << up[0] << " " 
                                        << up[1] << " " << up[2] << endl; 
        }

        MPoint upPoint = eyePt + upVec;
        upPoint.get( upPos );
        if( DtExt_Debug() & DEBUG_CAMERA )
        {
            cerr << "global up point position is " 
                        << upPos[0] << " " << upPos[1]
                        << " " << upPos[2] << " " << upPos[3] << endl;
        }        
        
    }   
    else
    {
        DtExt_Err( "Error in getting the camera function set\n" );
    }   

    *xTran = (float)upPos[0];
    *yTran = (float)upPos[1];
    *zTran = (float)upPos[2];

    return(1);

}

//  ========== DtCameraGetOrientation ==========
//
//  SYNOPSIS
//  Return the camera orientation as x,y,z 
//  Euler angles.
//
int DtCameraGetOrientation( int cameraID,
                            float* xRot,
                            float* yRot,
                            float* zRot )
{

    float   LRotX, LRotY, LRotZ;

    double  LX = 0.0;
    double  LY = 0.0;
    double  LZ = 0.0;
    double  LXV = 0.0;
    double  LYV = 0.0;
    double  LZV = 0.0;
    double  LXUp = 0.0;
    double  LYUp = 0.0;
    double  LZUp = 0.0;

    MMatrix LMatrix;

    // Check for error.
    //
    if( (cameraID<0) || (cameraID>=local->camera_ct) )
    {
        *xRot=0.0;
        *yRot=0.0;
        *zRot=0.0;
        return( 0 );
    }

    MStatus returnStatus = MS::kSuccess;
    MFnCamera fnCamera( local->cameras[cameraID].cameraShapeNode, 
        &returnStatus );

    if( MS::kSuccess == returnStatus )
    {
        MPoint eyePt = fnCamera.eyePoint( MSpace::kObject, &returnStatus );

        LX = eyePt.x;
        LY = eyePt.y;
        LZ = eyePt.z;

        if( DtExt_Debug() & DEBUG_CAMERA )
        {
            cerr << "eye point is at " 
                 << LX << " " << LY << " " << LZ << endl;
        }
        
        MVector upDir = fnCamera.upDirection( MSpace::kObject, &returnStatus );
        if( DtExt_Debug() & DEBUG_CAMERA )
        {
            cerr << "up direction is " 
                 << upDir.x << " " << upDir.y << " " << upDir.z << endl;
        }

        LXUp = upDir.x;
        LYUp = upDir.y;
        LZUp = upDir.z;

        
        MVector viewDir = fnCamera.viewDirection( MSpace::kObject, 
            &returnStatus );
        if( DtExt_Debug() & DEBUG_CAMERA )
        {
            cerr << "view direction is " 
                 << viewDir.x << " " << viewDir.y << " " << viewDir.z << endl;
        }   
        LXV = -viewDir.x;
        LYV = -viewDir.y;
        LZV = -viewDir.z;
    }
    else
    {
        DtExt_Err( "Error in getting the camera function set\n" );
        return 0;
    }


// If the up-vector is a null-vector (a problem), set it to 0,1,0

    if ( (LXUp == 0) && (LYUp == 0) && (LZUp == 0) )
    {
        LXUp=0.0;
        LYUp=1.0;
        LZUp=0.0;
    }   

// Compute camera orbital angles LXV, LYV, LZV: direction vector of the camera

    LRotX = DtGetAngle2D( LYV, sqrt(LXV*LXV + LZV*LZV) );
    LRotY = DtGetAngle2D(LZV, LXV);
    if ( LRotY < 0.0 ) LRotY += 360.0;
   
    
    LX=0.0;
    LY=1.0;
    LZ=0.0;   // Unit-length up-vector
    
//  Create the inverse camera rotation matrix and transform the 
//  default up-vector with it

    MVector vec( LX, LY, LZ );
    vec.rotateBy( MVector::kXaxis, LRotX-90.0 );
    vec.rotateBy( MVector::kYaxis, LRotY );

    LX = vec.x;
    LY = vec.y;
    LZ = vec.z;

    LRotZ = (float)(acos((LXUp*LX + LYUp*LY + LZUp*LZ) / 
                sqrt(LXUp*LXUp + LYUp*LYUp + LZUp*LZUp)) * RADIAN_TO_DEGREE);

    *xRot = LRotX;
    *yRot = LRotY;
    *zRot = LRotZ;

    return(1);
}  // DtCameraGetOrientation //


#define QX  0
#define QY  1
#define QZ  2
#define QW  3

int D_NXT[3]={ QY,QZ,QX };

// ========== DtCameraGetOrientationQuaternion ==========
//
//  SYNOPSIS
//  Return the camera orientation as a quaternion
//  This is far not optimal, but works
//
//  Not implemented yet in Maya DT.
//
int DtCameraGetOrientationQuaternion( int cameraID,
                                      float* LXRot,
                                      float* LYRot,
                                      float* LZRot,
                                      float* LWRot )
{
    MStatus stat = MS::kSuccess;
    double              Xrot, Yrot, Zrot, Wrot;

    *LXRot = 0.0;
    *LYRot = 0.0;
    *LZRot = 0.0;
    *LWRot = 0.0;
    

    // Check for error.
    //
    if( ( cameraID < 0 ) || ( cameraID >= local->camera_ct ) )
    {
        return( 0 );
    }

    MFnDagNode fnDagNode( local->cameras[cameraID].transformNode, &stat );
    MDagPath aPath;
    stat = fnDagNode.getPath( aPath );
    
    MFnTransform fnTransform( aPath, &stat );

    fnTransform.getRotationQuaternion ( Xrot, Yrot, Zrot, Wrot, 
        MSpace::kWorld);

    *LXRot = (float)Xrot;
    *LYRot = (float)Yrot;
    *LZRot = (float)Zrot;
    *LWRot = (float)Wrot;

    return 1;

}  // DtCameraGetOrientationQuaternion //


//  ========== DtCameraGetMatrix ==========
//
//  SYNOPSIS
//  Return the camera transformation matrix. This matrix
//  includes the camera rotation, translation, and scale
//  transforms. This function also sets the valid bits
//  for DT_CAMERA_POSITION and DT_CAMERA_ORIENTATION.
//  The matrix is in row-major order.
//
//  From PA DT:
//          Not implemented: returns a pointer to an identity matrix
//          under the OpenModel implementation.
//
//  For Maya DT:
//          This fuction returns the camera's global transformation matrix.
//
int DtCameraGetMatrix( int cameraID,
                       float** matrix )
{
    // static float mtx[4][4];
    static float globalMtx[4][4];
    static float localMtx[4][4]; 

    // Check for error.
    //
    if( ( cameraID < 0) || ( cameraID >= local->camera_ct ) )
    {
        *matrix = NULL;
        return( 0 );
    }

    // Camera transformation matrix is set on the transform node.
    //  
    MStatus returnStatus = MS::kSuccess;
    MFnDagNode fnTransNode( local->cameras[cameraID].transformNode, 
        &returnStatus );

    MDagPath dagPath;
    returnStatus = fnTransNode.getPath( dagPath );
    if( MS::kSuccess == returnStatus )
    {
        if( DtExt_Debug() & DEBUG_CAMERA )
        {
            cerr << "Got the dagPath\n";
            cerr << "length of the dagpath is " << dagPath.length() << endl;
        }
    }

    MFnDagNode fnDagPath( dagPath, &returnStatus );

    MMatrix localMatrix;
    MMatrix globalMatrix;

    localMatrix = fnTransNode.transformationMatrix ( &returnStatus );
    globalMatrix = dagPath.inclusiveMatrix();

    localMatrix.get( localMtx );
    globalMatrix.get( globalMtx );

    if( DtExt_Debug() & DEBUG_CAMERA )
    {
        int i = 0;
        int j = 0;

        cerr << "camera's global transformation matrix:\n";
        
        for( i = 0; i < 4; i++ )
        {
            for( j = 0; j < 4; j++ )
            {
                cerr << globalMtx[i][j] << " ";
            }
            cerr << endl;
        }   

        cerr << "camera's local transformation matrix:\n";

        for( i = 0; i < 4; i++ )
        {
            for( j = 0; j < 4; j++ )
            {
                cerr << localMtx[i][j] << " ";
            }
            cerr << endl;
        }   
    }

    // *matrix = (float*)&mtx;
    *matrix = (float*)&globalMtx;

    return(1);
}  // DtCameraGetMatrix //


//  ========== DtCameraGetAspect ==========
//
//  SYNOPSIS
//  Return the camera aspect ratio.
//
//  From PA DT:
//  Not implemented: always sets aspect value to 1.0.
//
int DtCameraGetAspect( int cameraID,
                       float* aspect )
{

    if( DtExt_Debug() & DEBUG_CAMERA )
    {
        cerr << "In DtCameraGetAspect\n";
        cerr << "cameraID is " << cameraID << endl; 
        cerr << "local->camera_ct is " << local->camera_ct << endl;
    }
    // Check for error.
    //
    if( ( cameraID < 0) || ( cameraID >= local->camera_ct ) )
    {
        *aspect = 1.0;
        return(0);
    }

    double ar = 0.0;

    // Set the valid flag.
    //
// local->cameras[cameraID].valid_bits|=(DT_VALID_BIT_MASK&DT_CAMERA_ASPECT);

    MStatus returnStatus = MS::kSuccess;
    MFnCamera fnCamera( local->cameras[cameraID].cameraShapeNode, 
        &returnStatus );

    if( MS::kSuccess == returnStatus )
    {
        ar = fnCamera.aspectRatio( &returnStatus );

        if( DtExt_Debug() & DEBUG_CAMERA )
        {
            cerr << "aspect ratio is " << ar << endl;
        }
    }

    *aspect = (float)ar;

    return(1);
}  // DtCameraGetAspect //


//  ========== DtCameraGetNearClip ==========
//
//  SYNOPSIS
//  Return the camera new clip distance.
//
int DtCameraGetNearClip( int cameraID,
                         float* lnear )
{
    double  nclip = 0.0;
    MStatus returnStatus = MS::kSuccess;

    if( DtExt_Debug() & DEBUG_CAMERA )
    {
        cerr << "In DtCameraGetNearClip\n";
        cerr << "cameraID is " << cameraID << endl; 
        cerr << "local->camera_ct is " << local->camera_ct << endl;
    }

    // Check for error.
    //
    if( ( cameraID < 0 ) || ( cameraID >= local->camera_ct ) )
    {
        *lnear = 1.0;
        return(0);
    }

    // Set the valid flag.
    //
// local->cameras[cameraID].valid_bits|=
//    (DT_VALID_BIT_MASK&DT_CAMERA_NEAR_CLIP);

    // Return values:
    //
    MFnCamera fnCamera( local->cameras[cameraID].cameraShapeNode, 
        &returnStatus );

    if( MS::kSuccess == returnStatus )
    {
        nclip = fnCamera.nearClippingPlane( &returnStatus );

        if( DtExt_Debug() & DEBUG_CAMERA )
        {
            cerr << "nclip is " << nclip << endl;
        }
    }
    else
    {
        DtExt_Err( "Error in getting the camera function set\n" );
    }
    
    *lnear = (float)nclip;

    return(1);
}  // DtCameraGetNearClip //


//  ========== DtCameraGetFarClip ==========
//
//  SYNOPSIS
//  Return the camera far clip distance.
//
int DtCameraGetFarClip( int cameraID, 
                        float* lfar )
{
    double fclip = 0.0;

    // Check for error.
    //
    if( ( cameraID < 0) || (cameraID >= local->camera_ct ) )
    {
        *lfar = 1.0;    
        return(0);
    }

    if( DtExt_Debug() & DEBUG_CAMERA )
    {
        cerr << "In DtCameraGetFarClip\n";      
    }
    MStatus returnStatus = MS::kSuccess;
    MFnCamera fnCamera( local->cameras[cameraID].cameraShapeNode, 
        &returnStatus );

    if( MS::kSuccess == returnStatus )
    {
        fclip = fnCamera.farClippingPlane( &returnStatus );
        if( DtExt_Debug() & DEBUG_CAMERA )
        {   
            cerr << "fclip is " << fclip << endl;
        }
    }
    else
    {
        DtExt_Err( "Error in getting the camera function set\n" );
    }

    // Set the valid flag.
    //
// local->cameras[cameraID].valid_bits|=(DT_VALID_BIT_MASK&DT_CAMERA_FAR_CLIP);

    // Return values
    //
    *lfar = (float)fclip;

    return(1);
}  // DtCameraGetFarClip //


//  ========== DtCameraGetFocalDistance ==========
//
//  SYNOPSIS
//  Return the distance from the camera viewpoint to
//  the point of focus.
//
int DtCameraGetFocalDistance( int cameraID,
                              float* focal )
{
    *focal = 1.0;

    // Check for error.
    //
    if( ( cameraID < 0) || ( cameraID >= local->camera_ct ) )
    {
        return(0);
    }

    if( DtExt_Debug() & DEBUG_CAMERA )
    {
        cerr << "In DtCameraGetFocalDistance\n";        
    }
    MStatus returnStatus = MS::kSuccess;
    MFnCamera fnCamera( local->cameras[cameraID].cameraShapeNode, 
        &returnStatus );

    if( MS::kSuccess == returnStatus )
    {   
        *focal = (float)fnCamera.focalLength( &returnStatus );
        if( DtExt_Debug() & DEBUG_CAMERA )
        {
            cerr << "focal length is " << *focal << endl;
        }
    }
    else
    {
        DtExt_Err( "Error in getting the camera function set\n" );
    }

    // Set the valid flag.
    //
// local->cameras[cameraID].valid_bits|=
//      (DT_VALID_BIT_MASK&DT_CAMERA_FOCAL_DISTANCE);

    return(1);
}  // DtCameraGetFocalDistance //


//  ========== DtCameraGetHeightAngle ==========
//
//  SYNOPSIS
//  Returns the camera vertical angle in radians of the
//  camera view volume. This is only applicable to 
//  perspective cameras.
//
int DtCameraGetHeightAngle( int cameraID,
                            float* angle )
{
    // Initialize return values.
    //
    *angle = 0.0;

    // Check for error.
    //
    if( ( cameraID < 0 ) || ( cameraID >= local->camera_ct ) ) 
    {
        return(0);
    }

    if( DtExt_Debug() & DEBUG_CAMERA )
    {
        cerr << "In DtCameraGetHeightAngle\n";      
    }
    MStatus returnStatus = MS::kSuccess;
    MFnCamera fnCamera( local->cameras[cameraID].cameraShapeNode, 
        &returnStatus );

    double focal;
    double horiz;
    
    if( MS::kSuccess == returnStatus )
    {   
        // Make sure camera is of a valid type for this attribute.
        //
        switch( local->cameras[cameraID].type )
        {
            // Valid types:
            //
            case DT_PERSPECTIVE_CAMERA:
                // Changed the following to match that of the Mel 
                // scripts which shows the field of view to the user.
                focal = fnCamera.focalLength( &returnStatus );
                horiz = fnCamera.horizontalFilmAperture( &returnStatus );

                *angle = (float)(2.0f * RADIAN_TO_DEGREE * 
                                    atan( 0.5*horiz / ( 0.03937 * focal )));

                if( DtExt_Debug() & DEBUG_CAMERA )
                {
                    cerr << "height angle (vertical field of view) is " << 
                        *angle << endl;
                }
                break;

                // Invalid types
                //
            default:    
                return(0);
        }
    }
    else
    {
        DtExt_Err( "Error in getting the camera function set\n" );
    }

    // Set the valid flag.
    //
    // local->cameras[cameraID].valid_bits|=
    //      (DT_VALID_BIT_MASK&DT_CAMERA_HEIGHT_ANGLE);

    return(1);
}  // DtCameraGetHeightAngle //


//  ========== DtCameraGetHeight ==========
//
//  SYNOPSIS
//  Returns the height of the camera view volume. This is
//  only applicable to orthographic cameras.
//
int DtCameraGetHeight( int cameraID, float* height )
{
    // Initialize return values.
    //
    *height = 0.0;

    // Check for error.
    //
    if( ( cameraID < 0 ) || ( cameraID >= local->camera_ct ) ) 
    {
        return(0);
    }
    if( DtExt_Debug() & DEBUG_CAMERA )
    {
        cerr << "In DtCameraGetHeight\n";       
    }
    MStatus returnStatus = MS::kSuccess;
    MFnCamera fnCamera( local->cameras[cameraID].cameraShapeNode, 
        &returnStatus );

    if( MS::kSuccess == returnStatus )
    {       
        // Make sure camera is of a valid type for this attribute.
        //
        switch( local->cameras[cameraID].type )
        {
            // Valid types:
            //
            case DT_ORTHOGRAPHIC_CAMERA:    
                // For Maya ortho camera, height = width?
                // 
                *height = (float)fnCamera.orthoWidth( &returnStatus );
//cerr << "height (width of orthographic camera ) is " << *height << endl;
                break;
                // Invalid types:
                //
            default:    
                return(0);
        }
    }
    else
    {
        DtExt_Err( "Error in getting the camera function set\n" );
    }

    return(1);
}  // DtCameraGetHeight //


//  ========== DtCameraIsValid ==========
//
//  SYNOPSIS
//  Returns the state of the modified flag for the
//  camera. This flag is set or cleared when DtSetFrame()
//  is called. If this function returns 1, then none
//  of the camera attributes have changed.
//
int DtCameraIsValid( int cameraID,
                     int valid_bit )
{
    int state = 0;
    int ret = 0;

    // Check for error.
    //
    if((cameraID<0)||(cameraID>=local->camera_ct)) return(0);

    // Get the current valid bit state.
    //
    state=local->cameras[cameraID].valid_bits;

    // Check the requested valid bit
    //
    switch (valid_bit)
    {
        case DT_CAMERA:
            if((state&(DT_VALID_BIT_MASK&DT_CAMERA_POSITION))&&
               (state&(DT_VALID_BIT_MASK&DT_CAMERA_ORIENTATION))&&
               (state&(DT_VALID_BIT_MASK&DT_CAMERA_ASPECT))&&
               (state&(DT_VALID_BIT_MASK&DT_CAMERA_NEAR_CLIP))&&
               (state&(DT_VALID_BIT_MASK&DT_CAMERA_FAR_CLIP))&&
               (state&(DT_VALID_BIT_MASK&DT_CAMERA_FOCAL_DISTANCE))&&
               (state&(DT_VALID_BIT_MASK&DT_CAMERA_HEIGHT_ANGLE))&&
               (state&(DT_VALID_BIT_MASK&DT_CAMERA_HEIGHT))) 
            {
                ret = 1;
            }
            else    
            {
                ret = 0;
            }
            break;

        case DT_CAMERA_POSITION:
        case DT_CAMERA_ORIENTATION:
        case DT_CAMERA_ASPECT:
        case DT_CAMERA_NEAR_CLIP:
        case DT_CAMERA_FAR_CLIP:
        case DT_CAMERA_FOCAL_DISTANCE:
        case DT_CAMERA_HEIGHT_ANGLE:
        case DT_CAMERA_HEIGHT:
            ret = (state&(DT_VALID_BIT_MASK & valid_bit) ? 1 : 0);
            break;

        case DT_CAMERA_MATRIX:
            if( ( state&(DT_VALID_BIT_MASK&DT_CAMERA_POSITION)) &&
                (state&(DT_VALID_BIT_MASK&DT_CAMERA_ORIENTATION))) 
            {
                ret=1;
            }
            else 
            {
                ret=0;
            }
            break;

        default:    
            ret=0;
    }

    // Return valid state.
    //
    return(ret);

}  // DtCameraIsValid //


//  ================ Helper Function  =================
//

static bool addElement( MIntArray  *intArray, int newElem )
{
    unsigned int currIndex;

    for ( currIndex = 0; currIndex < intArray->length(); currIndex++ )
    {
        // Don't add if it's there already
        if ( newElem == (*intArray)[currIndex] ) 
            return false;

        if ( newElem < (*intArray)[currIndex] )
        {
            intArray->insert( newElem, currIndex );
            return true;
        }
    }

    // If we made it here it should go at the end...
    intArray->append( newElem );
    return true;
}

//  ========== DtLightGetAnimKeys ==========
//
//  SYNOPSIS
//  Returns a list of keyframes on the T/R/S parameters of the light.

int DtCameraGetAnimKeys( int cameraID, MIntArray *keyFrames )
{
    MStatus         status;
    MObject         shapeNode;
    
    MObject         anim;
    MFnDependencyNode dgNode;
    MDagPath        dagPath;
    
    int             currKey,
                    numKeys,
                    keyTime,
                    stat;
                    
                    
    MDagPath        shapeDagPath;
    
    MItDependencyGraph::Direction direction = MItDependencyGraph::kUpstream;
    MItDependencyGraph::Traversal traversalType = 
        MItDependencyGraph::kBreadthFirst;

    MItDependencyGraph::Level level = MItDependencyGraph::kNodeLevel;
    MFn::Type filter = MFn::kAnimCurve;
    
    // A quick check to see if the user has actually given us a valid
    // pointer.
    
    if ( !keyFrames )
    {
        return 0;
    }   
    
    stat = DtExt_CameraGetShapeNode( cameraID, shapeNode );
    if ( 1 != stat )
    {
        cerr << "Problems in cameraGetShapeNode" << endl;
        return 0;
    }   
    
    MItDependencyGraph dgIter( shapeNode, filter, direction,
                            traversalType, level, &status );
                            
    for ( ; !dgIter.isDone(); dgIter.next() )
    {
        anim = dgIter.thisNode( &status );
        MFnAnimCurve animCurve( anim, &status );
        if ( MS::kSuccess == status ) 
        {
            numKeys = animCurve.numKeyframes( &status );
            for ( currKey = 0; currKey < numKeys; currKey++ )
            {
                // Truncating values here; may need more control
                keyTime = (int) animCurve.time( currKey, &status ).value();
                addElement( keyFrames, keyTime );
            }
        }
    }

    return 1;

}

//  ========== cameraNew ==========
//
//  SYNOPSIS
//  A private function. Used to reset all internal states.
//
void cameraNew(void)
{
    // Create the object instance structure.
    //
    if( NULL == local ) 
    {
        local = (DtPrivate *)NiAlloc( DtPrivate, 1);
        memset(local, 0, sizeof(DtPrivate));
    }

    // Free the camera structure array.
    //
    if( NULL != local->cameras )
    {
        NiFree( local->cameras );
        local->cameras = NULL;
        local->camera_ct = 0;
    }

}  // cameraNew //

void DtExt_CameraDelete( void )
{
    int i;

    if( NULL == local ) 
    {
        return;
    }

    if( NULL != local->cameras )
    {
        for( i = 0; i < local->camera_ct; i++ )
        {
            // delete local->cameras[i].root;
            // delete local->cameras[i].camera;
        }
        NiFree( local->cameras );        
    }
    NiFree( local );
    local = NULL;
} 

//
// ========== DtCameraCreateCaches ==========
//
// SYNOPSIS
// Create all nodes caches and prepare for exporting.
//

void  DtCameraCreateCaches(void)
{

//
//  This routine is not used.  Cameras are included when scanning
//  for the lights.  Followed the AlToIv format and way scan twice
//  for the cameras when not much to do with them.
//

}  // DtCameraCreateCaches //

int addTransformCamera( MObject transformNode,
                        MObject cameraShapeNode )
{
    if( !DtExt_outputCameras() ) 
    {
        return 0;
    }
    
    if( DtExt_Debug() & DEBUG_CAMERA )
    {
        cerr << "In addTransformCamera\n";
    }
    local->cameras =    
        (CameraStruct*)NiRealloc(local->cameras, 
        (1+local->camera_ct)*sizeof(CameraStruct));

    memset( &local->cameras[local->camera_ct], 0, sizeof(CameraStruct) );

    local->cameras[local->camera_ct].cameraShapeNode = cameraShapeNode;
    local->cameras[local->camera_ct].transformNode = transformNode;

    MStatus returnStatus = MS::kSuccess;
    MFnCamera fnCamera( cameraShapeNode );

    if( false == fnCamera.isOrtho( &returnStatus ) )
    {   
        if( DtExt_Debug() & DEBUG_CAMERA )
        {
            cerr << "perspective camera\n"; 
        }
        local->cameras[local->camera_ct].type = DT_PERSPECTIVE_CAMERA;
    }
    else
    {
        if( DtExt_Debug() & DEBUG_CAMERA )
        {
            cerr << "ortho camera\n";
        }
        local->cameras[local->camera_ct].type = DT_ORTHOGRAPHIC_CAMERA;
    }

    local->camera_ct++;


    // For testing purpose:
    //
    if( DtExt_Debug() & DEBUG_CAMERA )
    {

        float v = 0.0;
        
        cerr << "Calling DtCameraGetNearClip\n";
        DtCameraGetNearClip( local->camera_ct-1, &v );

        cerr << "Calling DtCameraGetFarClip\n";
        DtCameraGetFarClip( local->camera_ct-1, &v );

        float p[3];
        cerr << "Calling DtCameraGetPosition\n";
        DtCameraGetPosition( local->camera_ct-1, &p[0], &p[1], &p[2] );

        float o[3];
        cerr << "Calling DtCameraGetOrientation\n";
        DtCameraGetOrientation( local->camera_ct-1, &o[0], &o[1], &o[2] );

        float matrix[4][4];
        cerr << "Calling DtCameraGetMatrix\n";
        DtCameraGetMatrix( local->camera_ct-1, (float ** )matrix );

        float aspect = 0.0;
        cerr << "Calling DtCameraGetAspect\n";
        DtCameraGetAspect( local->camera_ct-1, &aspect );

        float focal = 0.0;
        cerr << "Calling DtCameraGetFocalDistance\n";
        DtCameraGetFocalDistance( local->camera_ct-1, &focal );
    }

    return (local->camera_ct-1); // Use to return 1.
}

//  ========== DtCameraGetPerspectiveCount ==========
//
//  NOTES: Added 9-7-99 (Andrew Jones)
//
//  SYNOPSIS
//  Return the number of persp. cameras in the scene graph.
//
int DtCameraGetPerspectiveCount( int* count )
{
    *count = 0;

    // Make sure cameras have been loaded.
    //
    if( NULL == local->cameras )
        return( 0 );

    // Return the count of perspective cameras
    for (int i=0;i < local->camera_ct; i++)
    {
        if (local->cameras[i].type == DT_PERSPECTIVE_CAMERA)
            (*count)++;
    }

    return( 1 );
}  // DtCameraGetPerspectiveCount //

//  ========== DtCameraGetPerspectiveCount ==========
//
//  NOTES: Added 9-7-99 (Andrew Jones)
//
//  SYNOPSIS
//  Acquire the index to the nth perspective camera.
//  Nth values start from the 0th.
//
int DtCameraGetNthPerspective( int iNth, int *iIndex )
{

    int iCount = 0;

    // Make sure cameras have been loaded.
    if( NULL == local->cameras )
        return( 0 );

    // Return the count of perspective cameras
    for (int i=0;i < local->camera_ct; i++)
    {
        if (local->cameras[i].type == DT_PERSPECTIVE_CAMERA)
        {
            if (iCount == iNth)
            {
                *iIndex = i;
                return(1);
            }
            iCount++;
        }
    }

    return( 0 );
}  // DtCameraGetNthPerspective //

#ifdef WIN32
#pragma warning(default: 4244 4305)
#endif // WIN32
