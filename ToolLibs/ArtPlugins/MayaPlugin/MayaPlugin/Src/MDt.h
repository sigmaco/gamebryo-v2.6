// Copyright (C) 1998 Alias|Wavefront, a division of Silicon Graphics Limited.
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
// $Author: warnold $
// $Revision: /main/maya2.1/1 $
// $Date: 1999/05/27 16:48:01 $
//
// Original Author:    XCW
//

#ifndef __MDt_h__
#define __MDt_h__

#ifdef WIN32
#include <windows.h>
#else
#include <Xm/Xm.h>
#endif

#include <maya/MObject.h>
#include <maya/MIntArray.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern bool g_bSampleAllRotation;
extern bool g_bSampleRotationAsQuaternion;
extern bool g_bSampleAllTranslation;
// End of face/line set delimiter:
//
#define DtEND_OF_FACE -1

// File types:
//
#define DtInvalid     -1
#define DtStatic       0
#define DtAnimation    1
#define DtKinematic    2

#define DT_INVALID      DtInvalid
#define DT_STATIC       DtStatic
#define DT_ANIMATION    DtAnimation
#define DT_KINEMATIC    DtKinematic

// General data types: 
//
typedef struct { float  vec[3]; } DtVec3f;
typedef struct { float  vec[2]; } DtVec2f;
typedef struct { float  r,g,b,a; } DtRGBA;

#define DT_ERROR                        (-1)
#define DT_CANCEL                       0
#define DT_SUCCESS                      1

#define DT_CAMERA           0x02000001  /* all camera functions        */
#define DT_CAMERA_POSITION      0x02000002  /* DtCameraGetPosition()       */
#define DT_CAMERA_ORIENTATION       0x02000004  /* DtCameraGetOrientation()*/
#define DT_CAMERA_MATRIX        0x02000006  /* DtCameraGetMatrix()     */
#define DT_CAMERA_ASPECT        0x02000010  /* DtCameraGetAspect()     */
#define DT_CAMERA_NEAR_CLIP     0x02000020  /* DtCameraGetNearClip()       */
#define DT_CAMERA_FAR_CLIP      0x02000040  /* DtCameraGetFarClip()        */
#define DT_CAMERA_FOCAL_DISTANCE    0x02000080  /*DtCameraGetFocalDistance()*/
#define DT_CAMERA_HEIGHT_ANGLE      0x02000100  /*DtCameraGetHeightAngle()  */
#define DT_CAMERA_HEIGHT        0x02000200  /*DtCameraGetHeight()     */

#define DT_LIGHT            0x04000001  /*all light functions     */
#define DT_LIGHT_ON         0x04000002  /*DtLightGetOn()          */
#define DT_LIGHT_INTENSITY      0x04000004  /*DtLightGetIntensity()       */
#define DT_LIGHT_COLOR          0x04000008  /*DtLightGetColor()       */
#define DT_LIGHT_POSITION       0x04000010  /*DtLightGetPosition()        */
#define DT_LIGHT_DIRECTION      0x04000020  /*DtLightGetDirection()       */
#define DT_LIGHT_DROP_OFF_RATE      0x04000040  /*DtLightGetDropOffRate() */
#define DT_LIGHT_CUT_OFF_ANGLE      0x04000080  /*DtLightGetCutOffAngle() */

#define DT_SHAPE            0x08000001  /*all shape functions     */
#define DT_SHAPE_MATRIX         0x08000002  /*DtShapeGetMatrix()      */
#define DT_SHAPE_VISIBLE        0x08000004
#define DT_SHAPE_VERTICIES      0x08000008  /*DtShapeGetVertices()        */
#define DT_SHAPE_NORMALS        0x08000010  /*DtShapeGetNormals()     */
#define DT_SHAPE_TEXTURE_VERTICIES  0x08000020  /*DtShapeGetTextureVertices*/

#define DT_FACE             0x08000040  /*the next three face functions   */
#define DT_FACE_VERTEX_INDEX        0x08000080  /*DtFaceGetIndex()        */
#define DT_FACE_NORMAL_INDEX        0x08000100  /*DtFaceGetNormalIndex()*/
#define DT_FACE_TEXTURE_INDEX       0x08000200  /*DtFaceGetTextureIndex()*/

// Camera types, returned by DtCameraGetType(): 
#define DT_PERSPECTIVE_CAMERA   1
#define DT_ORTHOGRAPHIC_CAMERA  2

// Light types, returned by DtLightGetType():
//
#define DT_POINT_LIGHT      1
#define DT_DIRECTIONAL_LIGHT    2
#define DT_SPOT_LIGHT       3
#define DT_AMBIENT_LIGHT    4

// Texture defines:
//
#define DT_REPEAT   1
#define DT_CLAMP    2
#define DT_MODULATE 1
#define DT_DECAL    2
#define DT_BLEND    3


// GENERAL DT FUNCTIONS: 
int DtGetFilename           (char** basename);
int DtGetDirectory          (char** directory);
int DtSetDirectory          (char *name);

// SCENE FUNCTIONS:
void    DtSceneGetName      (char** name);

int DtFrameSet          (int frame);
int DtFrameGet          (void);
void DtFrameSetStart     (int start);
int DtFrameGetStart     (void);
void DtFrameSetEnd       (int end);
void DtFrameSetBy        (int by);

// CAMERA FUNCTIONS: 
int     DtCameraGetCount       (int* count);
int     DtCameraGetType        (int cameraID,int* type);
int     DtCameraGetName        (int cameraID,char** name);
int     DtCameraGetPosition    (int cameraID, float* xTran,
                                              float* yTran,float* zTran);
int     DtCameraGetInterest    (int cameraID, float* xTran,
                                              float* yTran,float* zTran);
    
int     DtCameraGetUpPosition  (int cameraID, float* xTran, 
                                              float* yTran,float* zTran); 
int     DtCameraGetOrientation (int cameraID, float* xRot,
                                              float* yRot,float* zRot);
int     DtCameraGetOrientationQuaternion(int cameraID,
                                                        float* xRot,
                                                        float* yRot,
                                                        float* zRot,
                                                        float* wRot);
int     DtCameraGetMatrix       (int cameraID,float** matrix);
int     DtCameraGetAspect       (int cameraID,float* aspect);
int     DtCameraGetNearClip     (int cameraID,float* Lnear);
int     DtCameraGetFarClip      (int cameraID,float* Lfar);
int     DtCameraGetFocalDistance   (int cameraID,float* focal);
int     DtCameraGetHeightAngle  (int cameraID,float* angle);
int     DtCameraGetHeight       (int cameraID,float* height);
int     DtCameraIsValid         (int cameraID,int valid_bit);
int     DtCameraGetPerspectiveCount( int* count );
int     DtCameraGetNthPerspective( int iNth, int *iIndex );

// LIGHT FUNCTIONS:
int    DtEnvGetAmbientIntensity  (float *intensity);
void   DtEnvGetAmbientColor      (float* red, float* green, float* blue);
int     DtLightGetCount      (int* count);
int     DtLightGetType       (int lightID,int* type);
int     DtLightGetOn         (int lightID,int* on);
int     DtLightGetIntensity  (int lightID,float* intensity);
int     DtLightGetColor      (int lightID, float* red, float* green,
                              float* blue);
int     DtLightGetPosition   (int lightID, float* xTrans, float* yTrans,
                              float* zTrans);
int     DtLightGetDirection  (int lightID, float* xRot, float* yRot,
                              float* zRot);
int     DtLightGetDropOffRate  (int lightID,float* rate);
int     DtLightGetCutOffAngle  (int lightID,float* angle);
int     DtLightGetPenumbraAngle(int lightID,float* angle);
int     DtLightFindByTransformName(MString PartialPathName);
bool    DtLightGetTextureName(int lightID, char* szTextureName, 
    unsigned int uiNameLen);
int     DtLightGetTranslation(int lightID, float *x, float *y, float *z);
int     DtLightGetRotation(int lightID, float *x, float *y, float *z);

const char *DtLightGetName   ( int lightID );

// SHAPE FUNCTIONS: 
int DtShapeGetCount(void);
int DtShapeGetShapeName (int shapeID,const char** name); 
int DtShapeGetParentID (int shapeID );
int DtShapeGetInverseMatrix ( int shapeID, float mat[4][4] ); 
int DtShapeGetVertex(int shapeID,int index,DtVec3f* vertex);
int DtShapeGetVertices(int shapeID,int* count,DtVec3f** vertices);
int DtShapeGetVerticesAnimated(int shapeID, int *count, int **vertices);
int DtShapeGetTextureVertices(int shapeID,int* count,DtVec2f** vertices, 
    char* pcUVSet = NULL);
int DtShapeGetNormal(int shapeID,int index,DtVec3f* normal);
int DtShapeGetPolygonNormal(int shapeID,int index,DtVec3f* normal);
int DtShapeGetVerticesFaceColors (int shapeID,int* count,DtRGBA** colors);
int DtShapeGetChildCount(int shapeID);
int DtShapeIsDoubleSided(int shapeID);
int DtShapeIsOpposite(int shapeID);
bool DtShapeIsLocator(int shapeID);
void DtShapeGetAllJoints( int iShapeNum, int *iNumJoints, int **piJoints );
void DtShapeGetSkinToBoneMat( int iShapeNum,int iJoint, float mat[4][4]);

int DtGroupGetCount(int shapeID);
int DtGroupGetNormal(int shapeID,int groupID,int index, DtVec3f* normal);
int DtGroupGetExplodedTextureVertices( int shapeID, int groupID, int *count, 
    DtVec2f **vertices, char* pcUVSet = NULL );

int DtFaceGetColorIndexByShape     (int shapeID,int groupID,int* count,
    long** indices);

// MATERIAL FUNCTIONS:
int DtMtlGetSceneCount  (int* count);       

int DtMtlGetName        (int shapeID,int groupID,char** name);

int DtMtlGetNameByID    (int mtl_id,char** name);       
int DtMtlGetID          (int shapeID,int groupID,int* mtl_id);
int DtMtlGetAmbientClr  (char* name,int index,
                                    float* red,float* green,float* blue);
int DtMtlGetDiffuseClr  (char* name,int index,
                                    float* red,float* green,float* blue);
int DtMtlGetSpecularClr (char* name,int index,
                                    float* red,float* green,float* blue);
int DtMtlGetEmissiveClr (char* name,int index,
                                    float* red,float* green,float* blue);
int DtMtlGetDiffuseCoeff(char* name,int index, float* coeff);

int DtMtlGetShininess   (char* name,int index,float* shininess);
int DtMtlGetTransparency    (char* name,int index,float* transparency);

int DtTextureGetSceneCount(int* count);
int DtTextureGetName(char* mtl_name,char** name);
int DtTextureGetFileName(char *mtl_name, char **name);
int DtTextureGetID(int mtl_id,int* textureID);
int DtTextureGetNameID(int textureID,char** name);      
int DtTextureGetFileNameID(int textureID,char** name);
bool DtTextureGetBumpMapID(int textureID);
int DtTextureGetTextureNodeID(int textureID, MObject& kTextureNode);
int DtTextureGetIDMulti(int mtl_id, const char *texType, int* textureID);
int DtTextureGetWrap(char* name,int* horizontal,int* vertical);
int DtTextureGetImageSize(char* name,int* width,int* height, int* components);
int DtTextureGetImageSizeByID(int LID,int* width,int* height, int* components);
int DtTextureGetImage(char* name,unsigned char** image);
int DtTextureGetImageByID(int LID,unsigned char** LImage);
bool DtTextureGetRepeatOffsetRotateUVs(int mtlID, const char* pcTextureType,
    float& fRepeatUVx, float& fRepeatUVy, float& fOffsetUVx, 
    float& fOffsetUVy, float& fRotate);
bool DtTextureNeedsMipMap(int iTextureID);

// CHARACTER NETWORK STATE TABLE FUNCTIONS:
int DtCnetCharacterCount(int* count);
int DtCnetCharacterGetShapes(int cnetId, int *count, int ** shapelist);
int DtCnetCharacterGetFrameRange(int cnetId, int *start, int *end);

void dtPolygonCopyFaceList(int iDestShapeID, int iSrcShapeID, int iGroupID);
void dtPolygonGetFaceList(int shapeID,int groupID,int** list,int* count);

#ifdef __cplusplus
};
#endif

//
//  Now for some cplusplus enabled functions
//

int DtShapeGetVtxAnimKeys( int shapeID, MIntArray *keyFrames );
int DtLightGetAnimKeys( int lightID, MIntArray *keyFrames );

int DtTextureGetID( int shapeID, int groupID, int* tex_id );
int DtTextureGetIDMulti( int shapeID, int groupID, char *texType, 
    int* tex_id );
int DtExt_ShapeGetUVSetByName( int iShapeID, char* pcUVSetName);
int  DtGroupGetExplodedTextureVertices( int shapeID, int groupID, int *count,
    DtVec2f **vertices, char* pcUVSet);

int  DtGroupGetExplodedTextureVerticesByIndex( int shapeID, int groupID, 
    int uvSetID, int *count, DtVec2f **vertices);

int  DtGroupGetExplodedVertices( int shapeID, int groupID, int *count, 
    DtVec3f **vertices);

int  dtPolygonGetExplodedFaceList( int shapeID, int groupID, int **list, 
    int *count );

int  DtGroupGetExplodedNormals( int shapeID, int groupID, int *count, 
    DtVec3f **normals);

int  DtShapeGetFinalNonUniformNonAnimatedScale(int shapeID, float *x, 
    float *y, float *z);

int  DtShapeGetFinalNonUniformNonAnimatedScale(MObject MTransform, float *x, 
    float *y, float *z);

bool DtExt_GetShapeHasSkin(int iShapeID);

bool DtTextureGetMaterialShading(int iMtlID, char &cShading);

bool DtTextureGetMaterialTextureApplyMode(int iMtlID, 
    char &cTextureApplyMode);


bool DtTextureGetMaterialVertexColorOverride(int shapeID, int groupID);
bool DtTextureGetMaterialVertexColorOverride(int iMtlID);

bool DtTextureGetMaterialVertexColorSrcMode(int shapeID, int groupID, 
    char &cSrcMode);

bool DtTextureGetMaterialVertexColorSrcMode(int iMtlID, char &cSrcMode);

bool DtTextureGetMaterialVertexColorLightMode(int shapeID, int groupID, 
    char &cLightMode);

bool DtTextureGetMaterialVertexColorLightMode(int iMtlID, char &cLightMode);

bool DtTextureGetMaterialTransparencyOverride(int iMtlID);

bool DtTextureGetMaterialTransparencyUseAlphaBlending(int iMtlID);

bool DtTextureGetMaterialTransparencySource(int iMtlID, char &cSrcBlendMode);

bool DtTextureGetMaterialTransparencyDestination(int iMtlID, 
    char &cDestBlendMode);

bool DtTextureGetMaterialTransparencyUseAlphaTesting(int iMtlID);

bool DtTextureGetMaterialTransparencyNoTestingSorter(int iMtlID);

bool DtTextureGetMaterialTransparencyTestReference(int iMtlID, 
    short &sTestReference);

bool DtTextureGetMaterialTransparencyTestingTestMode(int iMtlID, 
    char &cTestingMode);

bool DtTextureGetMaterialNBTMethod(int iMtlID, char &cNBTMethod);


bool DtTextureGetMaterialMapProperties(
    int shapeID, 
    int groupID, 
    char* pType, 
    char &cFilterMode, 
    char &cMap, 
    unsigned short& usMaxAnisotropy);

bool DtTextureGetMaterialMapProperties(
    int iMtlID, 
    char* pType, 
    char &cFilterMode, 
    char& cMap, 
    unsigned short& usMaxAnisotropy);

bool DtTextureGetMaterialMapFlipBookProperties(int iShapeID, int iGroupID, 
    char* pType, bool &bFlipBook, short &sStartFrame, short &sEndFrame, 
    float &fFramesPerSecond, float &fStartTime, char &cCycleType);
bool DtTextureGetMaterialMapFlipBookProperties(int iMtlID, char* pType, 
    bool &bFlipBook, short &sStartFrame, short &sEndFrame, 
    float &fFramesPerSecond, float &fStartTime, char &cCycleType);

void DtTextureGetMaterialBumpMapProperties(int shapeID, int groupID,
    float &fBumpMapLuminaScale, float &fBumpMapLuminaOffset,
    float &fBumpMapMat00, float &fBumpMapMat01,
    float &fBumpMapMat10, float &fBumpMapMat11);
void DtTextureGetMaterialBumpMapProperties(int iMtlID,
    float &fBumpMapLuminaScale, float &fBumpMapLuminaOffset,
    float &fBumpMapMat00, float &fBumpMapMat01,
    float &fBumpMapMat10, float &fBumpMapMat11);

void DtTextureGetMaterialParallaxMapProperties(int shapeID, int groupID,
    float &fParallaxOffset);
void DtTextureGetMaterialParallaxMapProperties(int iMtlID, 
    float &fParallaxOffset);

int DtExt_MtlGetNumTextureMapsByID(int iMaterialID);
const char* DtExt_MtlGetTextureMapNameByID(int iMaterialID, int iMap);


// Follow the Input attribute provided to it's origin.
MObject FollowInputAttribute(MObject kInitialObject, MString kAttributeName);


#endif  /* __MDt_h__ */
