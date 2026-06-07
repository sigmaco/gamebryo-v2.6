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
// $Revision: /main/18 $
// $Date: 2000/04/19 15:31:12 $
//


// Precompiled Headers
#include "MayaPluginPCH.h"
#include "MDtJoint.h"
#include "MDtMultParents.h"
#include "MDtSkinBoneIK.h"


//
//  Required functions form other modules.
//  Need to call in the order used in DtExt_Init
//  in order to setup the working environment properly.
//
extern void lightNew();
extern void cameraNew();
extern int shapeNew(void);
extern int mtlNew();
extern void DtMtlsUpdate();
extern int gMDtObjectSetParents(void);


// Private data.
//
static char *out_dir  = NULL; // The output directory for exporting.
static char *out_name = NULL; // The output name for exporting.

// The Scene module data structure.
//
typedef struct
{
    char    *name;
    int     frame_start;
    int     frame_end;
    int     frame_by;
    int     frame_current;
    double  start;
    double  end;
    double  by;
    int     keyCount;    // Number of key frames.
    int     *keyFrames;  // Array of key frames.
    int     type;        // DtStatic, DtAnimation, DtKinematics
} DtPrivate;

// By default: export static scene
//
const int kFrameStartDefault = 0;
const int kFrameEndDefault = 0;
const int kFrameByDefault = 0;

static DtPrivate *local = NULL;


//  Forward declarations.
//
void  sceneNew(void);
void  sceneSetName( char * );
void  sceneCreateCaches(void);

// Private data initialization:
//
static  int _outputTransforms = kTRANSFORMALL;
static  int _outputPolys = kTESSTRI;
static  int _materialInventory = FALSE;
static  int _xTextureRes = 256;
static  int _yTextureRes = 256;
static  int _MaxxTextureRes = 4096;
static  int _MaxyTextureRes = 4096;
static  int _inlineTextures = TRUE;
static  int _outputCameras = FALSE;
static  int _outputParents = TRUE;
static  int _walkMode = ALL_Nodes;
static  int _debugoutput = FALSE;
static  int _updateShaders = 0;
static  int _reverseWinding = 0;
static  char *textureSearchPath = NULL;
static  int _rescaleRange = 0;
static  int _vertexAnimation = TRUE;
static  int _multiTexture = FALSE;
static  int _originalTexture = FALSE;
static  bool _HaveGui = true;

int
DtExt_MultiTexture()
{
    return _multiTexture;
}

int
DtExt_OriginalTexture()
{
    return _originalTexture;
}

int
DtExt_VertexAnimation()
{
    return _vertexAnimation;
}   

void
DtExt_setVertexAnimation( int mode )
{
    _vertexAnimation = mode;
}   

int 
DtExt_WalkMode()
{
    return _walkMode;
}

void 
DtExt_setWalkMode( int mode )
{
    _walkMode = mode;
}

int 
DtExt_tesselate()
{
    return _outputPolys;
}

void 
DtExt_setTesselate( int mode )
{
    _outputPolys = mode;
}

void    
DtExt_setOutputTransforms( int mode )
{
    _outputTransforms = mode;
}

int     
DtExt_outputTransforms()
{
    return  _outputTransforms;
}


int     
DtExt_xTextureRes()
{
    return  _xTextureRes;
}

int     
DtExt_yTextureRes()
{
    return  _yTextureRes;
}

void    
DtExt_setXTextureRes( int res )
{
    _xTextureRes = res;
}

void    
DtExt_setYTextureRes( int res )
{
    _yTextureRes = res;
}

int
DtExt_MaxXTextureRes()
{
    return  _MaxxTextureRes;
}

int
DtExt_MaxYTextureRes()
{
    return  _MaxyTextureRes;
}

void
DtExt_setMaxXTextureRes( int res )
{
  _MaxxTextureRes = res;
}

void
DtExt_setMaxYTextureRes( int res )
{
  _MaxyTextureRes = res;
}

int   
DtExt_inlineTextures()
{
    return _inlineTextures;
}

void    
DtExt_setOutputCameras( int status )
{
    _outputCameras = status;
}

int   
DtExt_outputCameras()
{
    return _outputCameras;
}

void    
DtExt_setParents( int status )
{
    _outputParents = status;
}

int     
DtExt_Parents()
{
    return _outputParents;
}

void 
DtExt_setDebug( int status )
{
    _debugoutput = status;
}

int 
DtExt_Debug()
{
    return _debugoutput;
}

void
DtExt_setWinding( int winding )
{
    _reverseWinding = winding;
}

int
DtExt_Winding()
{
    return _reverseWinding;
}

int
DtExt_RescaleRange()
{
    return _rescaleRange;
}

void
DtExt_addTextureSearchPath( char *tPath )
{
    if ( tPath && *tPath )
    {
        if ( textureSearchPath )
        {
            size_t stLen = 
                strlen( textureSearchPath ) + strlen( tPath ) + 1;
            textureSearchPath = (char *)NiRealloc( textureSearchPath, stLen );
            NiStrcat( textureSearchPath, stLen, tPath );
        } 
        else 
        {
            textureSearchPath = NiStrdup( tPath );
        }
    }

}

char *
DtExt_getTextureSearchPath()
{
    return textureSearchPath;
}

//
// Msg( format, arg1, arg2, ... )
//
void
DtExt_Msg( char *fmt, ... )
{
    va_list  args;
    char     buffer[2048];

    if( DtExt_Debug() )
    {
        // Print out the message.
        // 
        va_start( args, fmt );
        (void) NiVsprintf( buffer, 2048, fmt, args );
        va_end( args );

        fprintf(stderr, "%s", buffer );
        cerr << buffer << flush;

    }
}

//
// Err( format, arg1, arg2, ... )
//
void
DtExt_Err( const char *fmt, ... )
{
    va_list  args;
    char     buffer[2048];

    // Print out the message.
    //
    va_start( args, fmt );
    (void) NiVsprintf( buffer, 2048, fmt, args );
    va_end( args );

//    fprintf(stderr, "%s", buffer );

    cerr << buffer << flush;
}

//  ========== DtExt_SceneInit ==========
//
//  SYNOPSIS
//  Setup the scene structures for further calls to the Dt layer.
//
void  DtExt_SceneInit( char *name )
{
    // Now need to create the Dt database.
    //
    sceneNew();

    // Now need to setup the output file/scene name.
    //
    sceneSetName( name );
}


//  ========== DtExt_dbInit ==========
//
//  SYNOPSIS
//  Setup the database for further calls to the Dt layer
//
//  Return 0 for success, 1 for cancellation, 2 for error
//
//  In DtExt_.h:
//
int DtExt_dbInit( ) 
{
    // Call all of the routines needed to setup the database.
    //

    int body = 0;
    int frameCnt = local->frame_end - local->frame_start;
    if( (0 != body) && (0 != frameCnt) )
    {
        local->type = DtKinematic;
    }
    else if( 0 != frameCnt )
    {
        local->type = DtAnimation;
    }
    else
    {
        local->type = DtStatic;
    }

    // The order of the following function calls is important.
    //
    
    // These cannot be cancelled - they clear old data
    gPhysXClothConstrNew();
    gPhysXJointNew();
    gPhysXShapeNew();
    gParticleSystemNew();
    gLevelOfDetailNew();
    lightNew();
    cameraNew();
    
    // These can be cancelled or may have errors.
    int iShapeNewResult = shapeNew();
    if (iShapeNewResult != 0)
    {
        return iShapeNewResult;
    }
    
    int iSetParentsResult = gMDtObjectSetParents();
    if (iSetParentsResult != 0)
    {
        return iSetParentsResult;
    }
    
    int iMtlNewResult = mtlNew();
    if (iMtlNewResult != 0)
    {
        return iMtlNewResult;
    }
    
    int iAnimResult = gAnimDataAnimNew();
    if (iAnimResult != 0)
    {
        return iAnimResult;
    }
    
    int iSkinResult = gIKDataNew();
    if (iSkinResult != 0)
    {
        return iSkinResult;
    }

    // This can't be cancelled.
    gParticleSystemManager.SetParticleMaterials();
    
    return 0;
}

//  ========== DtExt_CleanUp ==========
//
//  SYNOPSIS
//  Setup the database for further calls to the Dt layer
//

extern void DtExt_LightDelete( void );
extern void DtExt_CameraDelete( void );
extern void DtExt_ShapeDelete( void );
extern void DtExt_MaterialDelete( void );

void  DtExt_CleanUp()
{
    DtExt_LightDelete();
    DtExt_CameraDelete();
    DtExt_ShapeDelete();
    DtExt_MaterialDelete();

    gAnimDataReset();
    gJointXformMan.Reset();
    gMultParentMan.Reset();

    gMDtObjectReset();

    if ( textureSearchPath )
    {
        NiFree( textureSearchPath );
        textureSearchPath = NULL;
    }

    if( local )
    {
        if (local->name)
        {
            NiFree( local->name);
            local->name = NULL;
        }

        NiFree( local );
        local = NULL;
    }

    if (out_dir)
    {
        NiFree(out_dir);
        out_dir = NULL;
    }

    gParticleSystemManager.Reset();
    gLevelOfDetailManager.Reset();
}

/* ================================================================ *
 * -------------------  PUBLIC  FUNCTIONS  ------------------------ *
 * ================================================================ */

//  ========== DtSceneGetName ==========
//
//  SYNOPSIS
//  Returns a pointer to the scene name. This is an
//  internal buffer and should not be changed.
//
void  DtSceneGetName( char **name )
{
    if( NULL == local ) 
    {   
        return;
    }
    *name = local->name;
    return;

}  // DtSceneGetName //

//
//  ========== DtFrameSet ==========
//
//  SYNOPSIS
//  This function is called by the DSO during the export translation. 
//  It initiates the needed actions to update the scene graph to the 
//  given frame. All animated channel data will be current
//  to the give frame after this call.
int  DtFrameSet( int frame )
{
    double frame_use;
    
    // Check for valid frame.
    //
    if( ( local->frame_start > frame ) || 
        ( local->frame_end < frame ) ) 
    {
        return 0;
    }

    // We should update the scene graph with the new channel data.
    // Sets the global time to the specified time.
    //
    frame_use = frame;
    MGlobal::viewFrame( frame_use );

    local->frame_current = frame;

    //  If we are not outputting transforms then assume the vertex data
    //  is in world space and we need to refresh the data.
    //

    // We will always try to do this.

    DtExt_SetupWorldVertices();

    // Here we will go thru the materials again and update the 
    // data

    if ( _updateShaders )
        DtMtlsUpdate();

    return 1;

}  // DtFrameSet

/*
 *  ========== DtFrameGet ==========
 *
 *  SYNOPSIS
 *  This function is called by the DSO to get the
 *      current frame number.
 */
int  DtFrameGet( void)
{
    return local->frame_current;

}  /* DtFrameGet */

/*
 *  ========== DtFrameGetStart ==========
 *
 *  SYNOPSIS
 *  Returns the frame number of the first frame in the animation.
 */
int  DtFrameGetStart()
{
    return( local->frame_start );

}  /* DtFrameGetStart */

/*
 *  ========== DtFrameSetStart ==========
 *
 *  SYNOPSIS
 *  Set the frame number of the first frame in the animation to use.
 */
void  DtFrameSetStart( int start )
{
    local->frame_start = start;

}  /* DtFrameSetStart */

/*
 *  ========== DtFrameSetEnd ==========
 *
 *  SYNOPSIS
 *  Sets the frame number of the last frame in the animation to use.
 */
void  DtFrameSetEnd( int end )
{
    local->frame_end = end;

}  /* DtFrameSetEnd */

/*
 *  ========== DtFrameSetBy ==========
 *
 *  SYNOPSIS
 *  Sets the frame 'by' value which determines intervals of frame samples.
 */
void DtFrameSetBy( int by )
{
    local->frame_by = by;
} /* DtFrameSetBy */

/*
 *  ========== DtGetFilename ==========
 *
 *  SYNOPSIS
 *  Returns the selected output directory and basename
 *  of the output files to be created by the export DSO.
 */

int  DtGetFilename(char **basename)
{
    if (out_name == NULL) return(0);

    *basename = out_name;
    return(1);

}  /* DtGetFilename */


/*
 *  ========== DtGetDirectory ==========
 *
 *  SYNOPSIS
 *  Returns the output directory name.
 */

int  DtGetDirectory(char **directory)
{

    if (out_dir == NULL) return(0);

    *directory = out_dir;
    return(1);

}  /* DtGetDirectory */

/*
 *  ========== DtSetDirectory ==========
 *
 *  SYNOPSIS
 *  Set the output directory.
 */

int DtSetDirectory( char *directory )
{
    if( NULL != out_dir ) 
    {
        NiFree(out_dir);
    }
    out_dir = NiStrdup( directory ); // where is out_dir freed?

    return(1);

}  /* DtSetDirectory */


//
// ===========================================================
// ===================== S H A P E ===========================
// ===========================================================


/* ======================================================================= *
 * --------------------  PRIVATE FUNCTIONS  ------------------------------ *
 * ======================================================================= */


/*
 *  ========== sceneNew ==========
 *
 *  SYNOPSIS
 *  A private function. Reset the internal states of this file.
 */

void  sceneNew(void)
{

    // Create the object instance structure.
    //
    if( NULL == local ) 
    {
        local = (DtPrivate *)NiAlloc(DtPrivate, 1);
        memset(local, 0, sizeof(DtPrivate));
    }

    // Clear the old scene name.
    //
    if( NULL != local->name )
    {
        NiFree( local->name );
        local->name = NULL;
    }

    // Clear old key frame array.
    //
    if( NULL != local->keyFrames )
    {
        NiFree( local->keyFrames );
        local->keyCount = 0;
    }

    // Fill in the scene
    // 
    sceneCreateCaches();

}  /* sceneNew */


/*
 *  ========== sceneCreateCaches ==========
 *
 *  SYNOPSIS
 *  Create the node caches and prepare for exporting.
 */

void  sceneCreateCaches(void)
{
    local->frame_start  = kFrameStartDefault;
    local->frame_end    = kFrameEndDefault;
    local->frame_by     = kFrameByDefault;
    local->start        = kFrameStartDefault;
    local->end          = kFrameEndDefault;
    local->by           = kFrameByDefault;
}  // sceneCreateCaches //


//  ========== sceneSetName ==========
//
//  SYNOPSIS
//  Set the scene name.
//
void  sceneSetName(char *name)
{

    if( NULL == local ) 
    {
        return;
    }

    // Replace old name with new name.
    //
    if( NULL != local->name ) 
    {
        NiFree( local->name );
    }
    if( NULL != name ) 
    {
        local->name = NiStrdup( name );
    }
    return;
}  // sceneSetName //

#ifdef WIN32
#pragma warning(default: 4244 4305)
#endif // WIN32


