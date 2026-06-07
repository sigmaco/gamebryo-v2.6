// Copyright (C) 1998 Alias | Wavefront, a division of Silicon Graphics 
// Limited.
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
// $Author: warnold $
// $Revision: /main/11 $
// $Date: 1999/03/08 11:43:53 $
//
//
// Original Author:    XCW
//

#ifndef __DtExt_h__
#define __DtExt_h__

#include <maya/MObject.h>
#include <maya/MDagPath.h>
#include <maya/MFnAnimCurve.h>

// Defined in DtLayer.c++:
//
int    DtExt_tesselate(); 
void   DtExt_setTesselate( int mode );
int    DtExt_outputTransforms();
void    DtExt_setOutputTransforms( int mode );
int    DtExt_xTextureRes();
int    DtExt_yTextureRes();
int     DtExt_MaxXTextureRes();
int     DtExt_MaxYTextureRes();
int     DtExt_outputCameras();
int     DtExt_Parents();
int     DtExt_Winding();
void    DtExt_setInlineTextures( int status );
int     DtExt_inlineTextures();
void    DtExt_setXTextureRes( int res );
void    DtExt_setYTextureRes( int res );
void    DtExt_setMaxXTextureRes( int res );
void    DtExt_setMaxYTextureRes( int res );
void    DtExt_setOutputCameras( int status );
void    DtExt_setParents( int status );
void    DtExt_setWinding( int mode );
void    DtExt_addTextureSearchPath( char *path );
char *  DtExt_getTextureSearchPath( void );

bool    DtExt_HaveGui(void);

int     DtExt_RescaleRange();
int     DtExt_VertexAnimation();
void    DtExt_setVertexAnimation( int mode );
int     DtExt_MultiTexture();
int     DtExt_OriginalTexture();

// Defined in MDtShape.cc:
//
int DtExt_SetupWorldVertices( void );
const char * objectName( MObject object );
const char * objectType( MObject object );
bool isObjectSelected( const MDagPath& path );
bool isObjectVisible( const MDagPath& path);
bool isMObjectVisible( MObject& kObject );


// Walking of the Dag Tree methods:
//
#define ALL_Nodes        0
#define PICKED_Nodes     1 // corresponds to the ACTIVE_Nodes in PA DT
#define ACTIVE_Nodes     2 // corresponds to the UNDER_Nodes in PA DT


#define DEBUG_GEOMAT    1
#define DEBUG_CAMERA    2
#define DEBUG_LIGHT     4

// Defined in MDtLayer.cc:
//
int     DtExt_WalkMode( void );
void    DtExt_setWalkMode( int mode );

// Shape extensions 

// MayaDT can not support these functions with the same arguments!
//

// Implemented now as
int DtExt_ShapeGetTransform(int shapeID, MObject &obj );
int DtExt_ShapeGetShapeNode(int shapeID, MObject &obj );
int DtExt_ShapeGetDagPath(int shapeID, MDagPath &dagPath);
void DtShapeGetUVSetNameForTexture( int iShapeNum, MObject MTexture, 
    char* pcUVSetName, unsigned int uiNameLen);

MFnDependencyNode DtExt_GetPlace2dTexture(const MObject& kShader,
    const char* pcTextureType, MStatus* pkStatus = NULL);

int DtExt_ShapeGetShader( int shapeID, int groupID, MObject &obj );

int DtExt_MtlGetShader( int mtlID, MObject &obj );
int DtExt_MtlGetIDByShader(MObject kShader,  int& mtlID);
int DtExt_MtlHasTextuesByID(int iMaterialID);

int  DtExt_ShapeGetOriginal(int shapeID, int groupID, MObject &obj);

int DtExt_FindShapeWithNode(MObject obj);

// Light extensions
int DtExt_LightGetTransform(int lightID, MObject &obj );
int DtExt_LightGetShapeNode(int lightID, MObject &obj );

// Camera extensions
int DtExt_CameraGetTransform(int cameraID, MObject &obj );
int DtExt_CameraGetShapeNode(int cameraID, MObject &obj );

// Emitter extensions
int DtExt_EmitterGetShapeNode(int emitterID, MObject &obj );

// Particles extensions
int DtExt_ParticlesGetTransform(int ParticlesID, MObject &obj );
int DtExt_ParticlesGetShapeNode(int ParticlesID, MObject &obj );

// PhysX extensions
int DtExt_PhysXGetShapeNode(int shapeID, MObject &obj );
int DtExt_PhysXGetTransform(int shapeID, MObject &obj );

// Level of detail extensions
int DtExt_LevelOfDetailGetTransform(int LevelOfDetailID, MObject &obj );

int DtExt_ShapeGetTexCnt( int shapeID, int *cnt );
int DtExt_ShapeIncTexCnt( int shapeID );

int DtExt_ShapeIsInstanced( int shapeID );

// Defined in DtLayer.c++:
//
void   DtExt_setDebug( int );

int    DtExt_Debug();

void   DtExt_Msg( char *, ... );

void   DtExt_Err( const char *, ... );

void   DtExt_SceneInit( char * );
int    DtExt_dbInit( void );
void    DtExt_CleanUp( void );

//---------------------------------------------------------------------------
void gAnimDataGetCSlopeIn( int iComponentID, int iGroupID, int iFrame, 
    float *pIn);

void gAnimDataGetCSlopeOut( int iComponentID, int iGroupID, int iFrame, 
    float *pOut);
int  gAnimDataGetCKeyFrames(int iComponentID, int iGroupID);
float gAnimDataGetCKeyFrameTime(int iComponentID,int iGroupID, int iFrame);
void gAnimDataGetCKeyFrameValue( int iComponentID, int iGroupID, int iFrame, 
    float *fvals);

void gAnimDataReset(void);
int gAnimDataAnimNew(void);

void gAnimDataGetSlopeInOut( int iComponentID, int iCurveType, int iFrame, 
    float *pIn, float *pOut);

int gAnimDataGetNumKeyFrames(int iComponentID, int iCurveType );
float  gAnimDataGetKeyFrameTime(int iComponentID, int iCurveType, 
    int iFrame);

double gAnimDataGetKeyFrameValue(int iComponentID, int iCurveType, 
    int iFrame);

bool gAnimDataIsObjectAnimated(int iComponentID, bool *bPos, bool *bRot, 
    bool *bScale, bool *bVisible);

double gAnimControlGetPlayBackSpeed(void);
double gAnimControlGetFramesPerSecond(void);
double gAnimControlGetMinTime(void);
double gAnimControlGetMaxTime(void);
int    gAnimControlGetPlayBackMode(void);
double gMDtGetLinearUnitMultiplier(void);

float FindSlope(float fX, float fY);

MObject& gJointGetJointNode( int iJoint );

// Defined in kMDtObject
//---------------------------------------------------------------------------
void gMDtObjectReset(void);
int gMDtObjectGetUsed(void);
int gMDtObjectGetParentID( int iIndex );
int gMDtObjectGetChildren(int iIndex, int *piNumChildren, 
    int **ppiChildren);
//---------------------------------------------------------------------------
typedef enum 
{ 
    ETypeLight, 
    ETypeShape, 
    ETypeCamera, 
    ETypeJoint, 
    ETypeEmitter, 
    ETypeParticles, 
    ETypeLevelOfDetail, 
    ETypeField,
    ETypePhysXJoint,
    ETypePhysXShape,
    ETypePhysXClothConstr,
    ETypeUnknown 
} EType;
//---------------------------------------------------------------------------
EType gMDtObjectGetType(int iIndex);
int gMDtObjectGetTypeIndex(int iIndex);
int gMDtObjectGetTransform(int iComponentID, MObject &XformNode);
int gMDtObjectGetRotationWOOrientation( int iComponentID, float *x, float *y,
    float *z);

int gMDtObjectGetRotation(int iComponentID, float *x, float *y, float *z);
int gMDtObjectGetPivot( int iComponentID, float *x, float *y, float *z);
int gMDtObjectGetPivotTranslation( int iComponentID, float *x, float *y, 
    float *z);

int gMDtObjectGetTranslation(int iComponentID, float *x, float *y, float *z);
int gMDtObjectGetFinalTranslation(int iComponentID, float *x, float *y, 
    float *z);

int gMDtObjectGetJointOrientation(int iComponentID, float *x, float *y, 
    float *z);

int gMDtObjectGetRotateAxis(int iComponentID, float *x, float *y, float *z);
int gMDtObjectFindComponentID(EType eType, int iTypeID);
int gMDtObjectFindComponentIDByNode(MObject XNode);


int gMDtObjectGetWorldTranslate(int iComponentID, float *x, float *y, 
    float *z);

bool gMDtObjectIsSelected(int iComponentID);
bool gMDtObjectIsVisible(int iComponentID);

const char *gMDtObjectGetName( int iComponentID );

//---------------------------------------------------------------------------
    // User should set x=y=z=1 when first calling getfinalscale
int gMDtObjectGetFinalNonAnimatingNotUniformScale(int iComponentID, float *x, 
        float *y, float *z);

int gMDtObjectGetScale(int iComponentID, float *x, float *y, float *z);
int gMDtObjectGetScalePivot(int iComponentID, float *x, float *y, float *z);
int gMDtObjectGetScalePivotTranslation(int iComponentID, float *x, float *y,
    float *z);

bool gMDtObjectGetAnimatedScale(int iComponentID);
//---------------------------------------------------------------------------

class kMDtVertexWeights
{
public:
    int m_iSize;
    float *m_pfWeights;
    int   *m_piJointIndices;

    kMDtVertexWeights(void) 
    { 
        m_pfWeights = NULL; 
        m_piJointIndices = NULL; 
        m_iSize = 0; 
    };

    ~kMDtVertexWeights(void) 
    { 
        delete[] m_pfWeights; 
        delete [] m_piJointIndices; 
    };
};
int DtShapeGetExplodedWeights(int shapeID, int groupID, int *count, 
    kMDtVertexWeights **ppVertexWeights);
//---------------------------------------------------------------------------

// General definitions for MDtExt functions:
#define kTESSNONE 0
#define kTESSTRI  3
#define kTESSQUAD 4

#define kTRANSFORMNONE    0
#define kTRANSFORMMINIMAL 1
#define kTRANSFORMALL     2

#define  AP_PGM_NAME     "GameExport"
#define  AP_PGM_VERSION    "2.1"

#endif




