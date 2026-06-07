// Copyright (C) 1998-2000 Alias | Wavefront, a division of Silicon Graphics 
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


// Precompiled Headers
#include "MayaPluginPCH.h"
#include <maya/MFnSpotLight.h>

#ifdef WIN32
#pragma warning(disable: 4244 4305)
#endif // WIN32

// Defines, macros, and magic numbers:
//
//extern int  DtExt_ShapeGetParentShapeNode(int shapeID, MObject &obj); 

#define DT_BLOCK_SIZE       16
#define DT_VALID_BIT_MASK 0x00FFFFFF

// The light table item structure:
//
typedef struct
{
    int valid_bits;         // The valid bits for this light.
    int type;               // Light type id for fast lookup.
    float intensity;          // Intensity of light for fast lookup.
    float intensityScale;     // Scaling constant for light
    MObject lightShapeNode;     // Pointer to the actual light node.
    MObject transformNode;      // Pointer to the transform of the lightNode.
} LightStruct;

// The light object instance data structure:
//
typedef struct
{
    int             light_ct;   // Count of lights.
    LightStruct*    lights;     // Array of LightStructs.
} DtPrivate;

extern const char * objectName( MObject object );

// Private data:
//
static DtPrivate*   local = NULL;

int add_AmbientLight( MObject ambLight, MObject transformNode );
int add_DirectionLight( MObject dirLight, MObject transformNode  );
int add_PointLight( MObject pntLight, MObject transformNode  );
int add_SpotLight( MObject sptLight, MObject transformNode  );
int add_LinearLight( MObject linearLight, MObject transformNode  );

// int add_LightNode(AlLightNode* lightNode);
// int add_LightNode( MObject lightNode);
// AlLightNode* is_LightGroup(AlGroupNode* group);
// MObject      is_LightGroup( MObject group);

float mapFloatRange( double zeroToInfinity );

static float    amb_intensity = 0.0;
static float    amb_red       = 77.0;
static float    amb_green     = 77.0;
static float    amb_blue      = 77.0;

static int iLightGrowSize = 100;
static int iLightsBlockAllocated = 0;


// ========================================================================
// ------------------------------  PUBLIC  FUNCTIONS  ---------------------
// ========================================================================

// MObject accessors for both the Transform node and the Shape node
// defined in MDtExt.h
int  DtExt_LightGetTransform( int lightID, MObject &obj )
{
    if ((lightID < 0) || (lightID >= local->light_ct))
    {
        obj = MObject::kNullObj;
        return 0;  // error, light does not exist
    }

    obj = local->lights[lightID].transformNode;
    
    return 1;
}

int  DtExt_LightGetShapeNode( int lightID, MObject &obj )
{
    if ((lightID < 0) || (lightID >= local->light_ct))
    {
        obj = MObject::kNullObj;
        return 0;  // error, light does not exist
    }

    obj = local->lights[lightID].lightShapeNode;
    
    return 1;
}


//  ========== DtLightGetCount ==========
//
//  SYNOPSIS
//  Return the total number of lights in the scene.
//
int DtLightGetCount( int* count )
{
    // Make sure lights have been loaded.
    //
    if( NULL == local->lights ) 
    { 
        *count = 0;
        return(0); 
    }

    // Return light count.
    //
    *count = local->light_ct;
    return(1);

}  // DtLightGetCount //


//  ========== DtLightGetType ==========
//
//  SYNOPSIS
//  Returns the type of the light.
//
int DtLightGetType( int lightID,
                    int* type )
{
    // Check for error.
    //
    if( ( lightID < 0) || (lightID >= local->light_ct ) ) 
    { 
        *type = 0;
        return(0); 
    }

    *type = local->lights[lightID].type;

    if( 0 == *type )
    { 
        return(0); // not recognized light
    }
    return(1);
}  // DtLightGetType //


//  ========== DtLightGetOn ==========
//
//  SYNOPSIS
//  Returns the current "on" state of the light.
//
int DtLightGetOn( int lightID, 
                  int* on )
{
    // Check for error.
    //
    if( ( lightID < 0 ) || ( lightID >= local->light_ct ) ) 
    { 
        *on = 0;
        return(0); 
    }

    // Set the valid flag.
    //
    // local->lights[lightID].valid_bits|=(DT_VALID_BIT_MASK&DT_LIGHT_ON);

    // Return the light "on" state
    //
    // *on=(local->lights[lightID].light->on.getValue() ? 1 : 0);

    *on = 1; //assume its always on for now ( From PA DT )
    return(1);
}  // DtLightGetOn //



//
//  ========== DtLightGetIntensity ==========
//
//  SYNOPSIS
//  Returns the intensity value of the light.
//

int DtLightGetIntensity( int lightID,
                         float* ret_intensity )
{
    // Check for error.
    //
    if( ( lightID < 0 ) || ( lightID >= local->light_ct ) ) 
    { 
        *ret_intensity = 0.0;
        return(0); 
    }

    MStatus status = MS::kSuccess;
    MObject lightShape = local->lights[lightID].lightShapeNode;
    MFnDependencyNode lightDG( lightShape, &status );

    MObject lightAttr = lightDG.attribute( MString( "intensity" ), &status );
    MPlug lightIntenPlug( lightShape, lightAttr );

    double intensity = 0.0;
    status = lightIntenPlug.getValue( intensity );

    // Lets check to see what kind of range we want.

    if ( DtExt_RescaleRange() )
    {
        intensity *= local->lights[lightID].intensityScale;
        *ret_intensity = mapFloatRange( intensity );
    } else {
        *ret_intensity = intensity;
    }


    return(1);
}  // DtLightGetIntensity //


/*
 *  ========== DtLightGetColor ==========
 *
 *  SYNOPSIS
 *  Return the color of the light. Values are
 *  normalized red, green, and blue in the 
 *  range [0.0 - 1.0].
 */

int DtLightGetColor( int lightID, 
                     float *red, 
                     float *green, 
                     float *blue )
{
    double r = 0.0; 
    double g = 0.0;
    double b = 0.0;

    // Check for error.
    //
    if( ( lightID < 0 )||( lightID >= local->light_ct ) )
    {
        *red = 0.0;
        *green = 0.0;
        *blue = 0.0;
        return(0);
    }

    // Set the valid flag.
    //
    local->lights[lightID].valid_bits |= (DT_VALID_BIT_MASK & DT_LIGHT_COLOR);

    MStatus returnStatus = MS::kSuccess;
    MObject lightShape = local->lights[lightID].lightShapeNode;
    MFnDependencyNode lightDG( lightShape, &returnStatus );

    MObject lightAttr = lightDG.attribute( MString( "colorR" ), 
        &returnStatus );

    MPlug lightColorRPlug( lightShape, lightAttr );
    returnStatus = lightColorRPlug.getValue( r );
    
    lightAttr = lightDG.attribute( MString( "colorG" ), &returnStatus );
    MPlug lightColorGPlug( lightShape, lightAttr );
    returnStatus = lightColorGPlug.getValue( g );
    
    lightAttr = lightDG.attribute( MString( "colorB" ), &returnStatus );
    MPlug lightColorBPlug( lightShape, lightAttr );
    returnStatus = lightColorBPlug.getValue( b );
    
    *red   =(float) r;
    *green =(float) g;
    *blue  =(float) b;

    return(1);

}  /* DtLightGetColor */


//  ========== DtLightGetPosition ==========
//
//  SYNOPSIS
//  Returns the x,y,z translation of the light.
//

int  DtLightGetPosition( int lightID, 
                         float *xTrans, 
                         float *yTrans, 
                         float *zTrans)
{
    // Initialize return values.
    //
    *xTrans = 0.0;
    *yTrans = 0.0;
    *zTrans = 0.0;

    // Check for error.
    //
    if( ( lightID < 0) || ( lightID >= local->light_ct ) ) 
    {
        return(0);
    }

    // Return the light position.
    //
    MStatus returnStatus = MS::kSuccess;
    MFnDagNode fnTransNode( local->lights[lightID].transformNode, 
        &returnStatus );

    MDagPath dagPath;
    returnStatus = fnTransNode.getPath( dagPath );
    if( MS::kSuccess == returnStatus )
    {
        if( DtExt_Debug() & DEBUG_LIGHT )
        {
            cerr << "Got the dagPath\n";
            cerr << "length of the dagpath is " << dagPath.length() << endl;
        }
    }

    MFnDagNode fnDagPath( dagPath, &returnStatus );

    MMatrix localMatrix;
    MMatrix globalMatrix;
    static float globalMtx[4][4];
    static float localMtx[4][4]; 

    localMatrix = fnTransNode.transformationMatrix ( &returnStatus );
    globalMatrix = dagPath.inclusiveMatrix();

    localMatrix.get( localMtx );
    globalMatrix.get( globalMtx );

    if( DtExt_Debug() & DEBUG_LIGHT )
    {
        int i = 0;
        int j = 0;

        cerr << "light's global transformation matrix:\n";
        
        for( i = 0; i < 4; i++ )
        {
            for( j = 0; j < 4; j++ )
            {
                cerr << globalMtx[i][j] << " ";
            }
            cerr << endl;
        }   

        cerr << "light's local transformation matrix:\n";

        for( i = 0; i < 4; i++ )
        {
            for( j = 0; j < 4; j++ )
            {
                cerr << localMtx[i][j] << " ";
            }
            cerr << endl;
        }   
    }

    *xTrans = globalMtx[3][0];
    *yTrans = globalMtx[3][1];
    *zTrans = globalMtx[3][2];

    if( DtExt_Debug() & DEBUG_LIGHT )
    {
        cerr << "The light position is " 
             << *xTrans << " " << *yTrans << " "  << *zTrans << endl;
    }
    return(1);

}  // DtLightGetPosition //


//  ========== DtLightGetDirection ==========
//
//  SYNOPSIS
//  Returns the direction of the light. This is only applicable
//  for spot lights and directional lights.
//

int  DtLightGetDirection( int lightID,  
                          float *xRot, 
                          float *yRot, 
                          float *zRot )
{
    MStatus returnStatus;

    // double x = 0.0;
    // double y = 0.0;
    // double z = 0.0;

    // Initialize return values.
    //
    *xRot = 0.0;
    *yRot = 0.0;
    *zRot = 0.0;

    // Check for error.
    //
    if( ( lightID < 0) || ( lightID >= local->light_ct ) ) 
    {
        return(0);
    }

    // Make sure light is of a valid type for this attribute.
    //
    switch( local->lights[lightID].type )
    {
        // valid types
        //
        case DT_DIRECTIONAL_LIGHT:
        case DT_SPOT_LIGHT:
            break;

            // invalid types
            //

        default:
            return(0);
    }

    MVector lightDirection = MVector::zNegAxis;
    MTransformationMatrix::RotationOrder order; 
    double  rotation[3];

    MObject lightTransform = local->lights[lightID].transformNode;
    MFnTransform fnLightTransform( lightTransform, &returnStatus );
    if ( MS::kSuccess != returnStatus )
        return 0;

    returnStatus = fnLightTransform.getRotation( rotation, order );
    if ( MS::kSuccess != returnStatus )
        return 0;

    lightDirection = lightDirection.rotateBy( rotation, order );

    *xRot = lightDirection.x;
    *yRot = lightDirection.y;
    *zRot = lightDirection.z;

    return 1;

}  // DtLightGetDirection //


//  ========== DtLightGetDropOffRate ==========
//
//  SYNOPSIS
//  Returns the rate at which the light intensity
//  drops off from the priamry direction. Only
//  applicable to spot lights.
//

int  DtLightGetDropOffRate( int lightID, 
                            float *rate )
{
    // Initialize return values.
    //
    *rate = 0.0;

    // Check for error.
    //
    if( ( lightID < 0) || ( lightID >= local->light_ct ) ) 
    {
        return(0);
    }

    // Make sure light is of a valid type for this attribute.
    //
    switch (local->lights[lightID].type)
    {
        // valid types
        //
        case DT_SPOT_LIGHT :
            break;

            // invalid types
            //
        default:
            return( 0 );
    }

    MStatus returnStatus = MS::kSuccess;
    MObject lightShape = local->lights[lightID].lightShapeNode;

    MFnSpotLight fnSpotLight( lightShape, &returnStatus );
    *rate = (float)fnSpotLight.dropOff( &returnStatus );
    
    if( DtExt_Debug() & DEBUG_LIGHT )
    {
        cerr << "The light drop off rate is " << *rate << endl;
    }

    return(1);

}  // DtLightGetDropOffRate //


//  ========== DtLightGetCutOffAngle ==========
//
//  SYNOPSIS
//  Returns the angle, in radians, outside of which the light
//  intensity is 0.0. This angle is measured from one edge
//  of the cone to the other.
//

int DtLightGetCutOffAngle(int lightID,float* angle)
{

    // initialize return values
    //
    *angle=0.0;

    // check for error
    //
    if((lightID<0)||(lightID>=local->light_ct)) 
    {
        return(0);
    }

    // make sure light is of a valid type for this attribute
    //
    switch (local->lights[lightID].type )
    {
        // valid types
        //
        case DT_SPOT_LIGHT:
            break;

            // invalid types
            //
        default:
            return(0);
    }

    MStatus returnStatus = MS::kSuccess;
    MObject lightShape = local->lights[lightID].lightShapeNode;

    MFnSpotLight fnSpotLight( lightShape, &returnStatus );
    *angle = (float)fnSpotLight.coneAngle( &returnStatus );
    
    if( DtExt_Debug() & DEBUG_LIGHT )
    {
        cerr << "The light cone angle is (should be in rads) " << *angle << 
            endl;
    }

    return(1);

}  // DtLightGetCutOffAngle //

//  ========== DtLightGetPenumbraAngle ==========
//
//  SYNOPSIS
//  Returns the penumbra angle, in radians, where the light intencity
//  goes from full to 0.0. 

int DtLightGetPenumbraAngle(int lightID,float* angle)
{

    // initialize return values
    //
    *angle=0.0;

    // check for error
    //
    if((lightID<0)||(lightID>=local->light_ct)) 
    {
        return(0);
    }

    // make sure light is of a valid type for this attribute
    //
    switch (local->lights[lightID].type )
    {
        // valid types
        //
        case DT_SPOT_LIGHT:
            break;

            // invalid types
            //
        default:
            return(0);
    }

    MStatus returnStatus = MS::kSuccess;
    MObject lightShape = local->lights[lightID].lightShapeNode;

    MFnSpotLight fnSpotLight( lightShape, &returnStatus );
    *angle = (float)fnSpotLight.penumbraAngle( &returnStatus );
    
    if( DtExt_Debug() & DEBUG_LIGHT )
    {
        cerr << "The light cone angle is (should be in rads) " << *angle << 
            endl;
    }

    return(1);

}  // DtLightGetPenumbraAngle //


//  ========== DtLightGetName ==========
//
//  SYNOPSIS
//  Returns the object name for this light
const char *DtLightGetName(int lightID)
{

    // check for error
    //
    if((lightID<0)||(lightID>=local->light_ct)) return(0);

    return ( objectName( local->lights[lightID].lightShapeNode ) ); 
}


/*
 *  ========== DtEnvGetAmbientIntensity ==========
 *
 *  SYNOPSIS
 *  Returns the global ambient intensity of the scene.
 */
int DtEnvGetAmbientIntensity(float* intensity)
{
    *intensity=amb_intensity;
    return(1);
}  /* DtEnvGetAmbientIntensity */


/*
 *  ========== DtEnvGetAmbientColor ==========
 *
 *  SYNOPSIS
 *  Returns the global ambient color for the scene.
 */

void DtEnvGetAmbientColor(float* red,float* green,float * blue)
{
    *red=amb_red;
    *green=amb_green;
    *blue=amb_blue;
    return;

}  /* DtEnvGetAmbientColor */



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

int DtLightGetAnimKeys( int lightID, MIntArray *keyFrames )
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
    
    stat = DtExt_LightGetShapeNode( lightID, shapeNode );
    if ( 1 != stat )
    {
        cerr << "Problems in lightGetShapeNode" << endl;
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

/*
 *  ========== lightNew ==========
 *
 *  SYNOPSIS
 *  A private function. Used to reset all internal states.
 */

void  lightNew(void)
{
    // Create the object instance structure.
    //
    if( NULL == local ) 
    {
        local = (DtPrivate *)NiAlloc(DtPrivate, 1);
        memset(local, 0, sizeof(DtPrivate));
    }

    // Free the light structure array.
    //
    if( NULL != local->lights )
    {
        NiFree(local->lights);
        local->lights=NULL;
        local->light_ct=0;
        iLightsBlockAllocated = 0;
    }
}  /* lightNew */


//  ========== DtLightCreateCaches ==========
//
//  SYNOPSIS
//  Create the node caches and prepare for exporting.
//
//  Not used.
// 
void  DtLightCreateCaches( void )
{
    
}  // DtLightCreateCaches //


//  mapFloatRange -   map value from Alias zeroToInfinity range to
//  Inventor zeroToOne range; used, among other places, for light intensity
//  Taken from the Alias to Inventor tree
//

//  This scaling should really be in the translator level and not here.

float mapFloatRange( double zeroToInfinity )
{
    float zeroToOne = zeroToInfinity;
    
    if ( DtExt_RescaleRange() )
    {
        if( 0.00001 >= zeroToInfinity )
        {
            return 0.0;
        }

        if( 100000.0 <= zeroToInfinity )
        {
            return 1.0;
        }

        int ordMag = ( int ) log10( zeroToInfinity );

        zeroToOne = 0.1 * (( 5.0 + ( float ) ordMag ) +
                            ( zeroToInfinity * powf( .1, ( ordMag +1 ))));
    }

    return  zeroToOne;
}


//  Routines for making tranversal of DagNodes for lights
//  easier.
//
int add_AmbientLight( MObject ambLightShape, 
                      MObject transformNode )
{
    double  r = 0.0;
    double  g = 0.0;
    double  b = 0.0;

    MStatus returnStatus = MS::kSuccess;
    MFnDependencyNode lightDG( ambLightShape, &returnStatus );
    // In TlightShape.attr: intensity
    //
    MObject lightAttr = lightDG.attribute( MString( "intensity" ), 
        &returnStatus );

    MPlug lightIntenPlug( ambLightShape, lightAttr );
    
    double intensity = 0.0;
    returnStatus = lightIntenPlug.getValue( intensity );
    
    amb_intensity = mapFloatRange( intensity );

    lightAttr = lightDG.attribute( MString( "colorR" ), &returnStatus );
    MPlug lightColorRPlug( ambLightShape, lightAttr );
    returnStatus = lightColorRPlug.getValue( r );
    
    lightAttr = lightDG.attribute( MString( "colorG" ), &returnStatus );
    MPlug lightColorGPlug( ambLightShape, lightAttr );
    returnStatus = lightColorGPlug.getValue( g );
    
    lightAttr = lightDG.attribute( MString( "colorB" ), &returnStatus );
    MPlug lightColorBPlug( ambLightShape, lightAttr );
    returnStatus = lightColorBPlug.getValue( b );
    
    amb_red  = (float) r;
    amb_green = (float) g;
    amb_blue = (float) b;


    if(local->light_ct >= iLightsBlockAllocated)
    {
        iLightsBlockAllocated += iLightGrowSize;

        local->lights = (LightStruct *)NiRealloc( local->lights, 
            (iLightsBlockAllocated)*sizeof(LightStruct));
    }

    memset( &local->lights[local->light_ct], 0, sizeof(LightStruct) );

    local->lights[local->light_ct].lightShapeNode = ambLightShape;
    local->lights[local->light_ct].transformNode = transformNode;
    local->lights[local->light_ct].type = DT_AMBIENT_LIGHT;
    local->lights[local->light_ct].intensityScale = 1.0;
    local->lights[local->light_ct].intensity =mapFloatRange( intensity );
    local->light_ct++;

    return 1;
}

int add_DirectionLight( MObject dirLightShape, 
                        MObject transformNode )
{
    if(local->light_ct >= iLightsBlockAllocated)
    {
        iLightsBlockAllocated += iLightGrowSize;

        local->lights = (LightStruct *)NiRealloc( local->lights, 
            (iLightsBlockAllocated) * sizeof(LightStruct));
    }
    memset( &local->lights[local->light_ct], 0, sizeof(LightStruct) );

    local->lights[local->light_ct].lightShapeNode = dirLightShape;
    local->lights[local->light_ct].transformNode = transformNode;
    local->lights[local->light_ct].type = DT_DIRECTIONAL_LIGHT;

    MStatus returnStatus = MS::kSuccess;
    MFnDependencyNode lightDG( dirLightShape, &returnStatus );
    // In TlightShape.attr: intensity
    //
    MObject lightAttr = lightDG.attribute( MString( "intensity" ), 
        &returnStatus );

    MPlug lightPlug( dirLightShape, lightAttr );
    
    double intensity = 0.0;
    returnStatus = lightPlug.getValue( intensity );

    local->lights[local->light_ct].intensityScale = 1.0;
    local->lights[local->light_ct].intensity = mapFloatRange( intensity );
    local->light_ct++;

    return 1;
}

int add_PointLight( MObject pntLightShape,
                    MObject transformNode )
{
    if(local->light_ct >= iLightsBlockAllocated)
    {
        iLightsBlockAllocated += iLightGrowSize;

        local->lights = (LightStruct *)NiRealloc( local->lights, 
            (iLightsBlockAllocated)*sizeof(LightStruct));
    }
    memset( &local->lights[local->light_ct], 0, sizeof(LightStruct) );

    local->lights[local->light_ct].lightShapeNode = pntLightShape;
    local->lights[local->light_ct].transformNode = transformNode;
    local->lights[local->light_ct].type = DT_POINT_LIGHT;

    MStatus returnStatus = MS::kSuccess;
    MFnDependencyNode lightDG( pntLightShape, &returnStatus );
    // In TlightShape.attr: intensity
    //
    MObject lightAttr = lightDG.attribute( MString( "intensity" ), 
        &returnStatus );

    MPlug lightPlug( pntLightShape, lightAttr );
    
    double intensity = 0.0;
    returnStatus = lightPlug.getValue( intensity );
    
    //intensity *= 1.4;
    local->lights[local->light_ct].intensityScale = 1.4;
    local->lights[local->light_ct].intensity =mapFloatRange( intensity );

    local->light_ct++;

    return 1;
}

int add_SpotLight( MObject sptLightShape, 
                   MObject transformNode )
{
    if(local->light_ct >= iLightsBlockAllocated)
    {
        iLightsBlockAllocated += iLightGrowSize;

        local->lights = (LightStruct *)NiRealloc( local->lights, 
            (iLightsBlockAllocated)*sizeof(LightStruct));
    }
    memset( &local->lights[local->light_ct], 0, sizeof(LightStruct) );

    local->lights[local->light_ct].lightShapeNode = sptLightShape;
    local->lights[local->light_ct].transformNode = transformNode;
    local->lights[local->light_ct].type = DT_SPOT_LIGHT;

    MStatus returnStatus = MS::kSuccess;
    MFnDependencyNode lightDG( sptLightShape, &returnStatus );
    // In TlightShape.attr: intensity
    //
    MObject lightAttr = lightDG.attribute( MString( "intensity" ), 
        &returnStatus );

    MPlug lightPlug( sptLightShape, lightAttr );
    
    double intensity = 0.0;
    returnStatus = lightPlug.getValue( intensity );

    //intensity *= 1.5;
    local->lights[local->light_ct].intensityScale = 1.5;
    local->lights[local->light_ct].intensity =mapFloatRange( intensity );
    local->light_ct++;

    return 1;
}

void DtExt_LightDelete( void )
{
    if( NULL == local ) 
    {
        return;
    }
    if( NULL != local->lights )
    {
        NiFree( local->lights );
    }
    NiFree( local );
    local = NULL;
    iLightsBlockAllocated = 0;
}

int addTransformLight( MObject transformNode,
                       MObject shapeNode )
{
    if( shapeNode.hasFn( MFn::kAmbientLight ) )
    {
        add_AmbientLight( shapeNode, transformNode );
    }
    else if( shapeNode.hasFn( MFn::kDirectionalLight ) )
    {
        add_DirectionLight( shapeNode, transformNode );     
    }
    else if( shapeNode.hasFn( MFn::kPointLight ) )
    {
        add_PointLight( shapeNode, transformNode );
    }
    else if( shapeNode.hasFn( MFn::kSpotLight ) )
    {
        add_SpotLight( shapeNode, transformNode );
    }

    return (local->light_ct-1); // Use to return 1;
}

int DtLightGetFirstParent(int iLightID, MObject &FirstParent)
{
    MObject LightNode;
    if (DtExt_LightGetTransform( iLightID, LightNode ) == false)
        return false;

    MStatus status;
    MFnDagNode fnDagNode(LightNode,&status);

    if (status != MS::kSuccess)
        return false;

    FirstParent = fnDagNode.parent(0,&status);

    if (status != MS::kSuccess)
        return false;

    return true;
}

//  ========== DtLightGetRotation ==========
//  SYNOPSIS
//      Returns the rotation of the light about the
//      x, y and z axes respectively.
//
//  NOTES: Created 10-28-99 Andrew Jones
//
//  DESCRIPTION
//
//  DtLightGetRotation() returns the rotation about the x, y, and z axes 
//  in the given shape's local transformation matrix. The rotation angles
//  (in degrees) are stored at the addresses of x, y and z. 
int  DtLightGetRotation(int lightID, float *x, float *y, float *z)
{
    // Check for error
    //
    if( (lightID < 0) || (lightID >= local->light_ct) )
    {
        return(0);
    }   
    
    // Get the Transform from the object.
    //
    MStatus stat;
    MFnTransform transFn( local->lights[lightID].transformNode );

    double rP[3];    
    MTransformationMatrix::RotationOrder order = MTransformationMatrix::kXYZ;

    stat = transFn.getRotation( rP, order, MSpace::kObject );
    if ( stat == MS::kSuccess )
    {
        *x = rP[0];
        *y = rP[1];
        *z = rP[2];
        
        return 1;
        
    }   

    return(0);
}   

//  ========== DtLightGetTranslation ==========
//  SYNOPSIS
//      Returns the translation of the given shape.
//
//  NOTES: Created 10-28-99 Andrew Jones
//
//  DESCRIPTION
//
//  DtLightGetTranslation() returns the translation of the given shape's 
//  local transformation matrix. The coordinates of the translation are
//  stored at the addresses of x, y and z. 
int  DtLightGetTranslation(int lightID, float *x, float *y, float *z)
{
    // Check for error
    if( (lightID < 0) || (lightID >= local->light_ct) )
    {
        return(0);
    }   
    
    // Get the Transform from the object.
    MStatus stat;
    MFnTransform transFn( local->lights[lightID].transformNode );

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


//  ========== DtLightGetShapeNode ==========
//  SYNOPSIS
//      Returns the shape node for the given light
//
//  NOTES: Created 6-5-2001 Drew Gugliotta
//
bool DtLightGetShapeNode(int lightID, MObject& node)
{
    // Check for error
    if( (lightID < 0) || (lightID >= local->light_ct) )
    {
        return false;
    }   

    node = local->lights[lightID].lightShapeNode;
    return true;
}

//  ========== DtLightGetShapeNode ==========
//  SYNOPSIS
//      Returns the shape node for the given light
//
//  NOTES: Created 6-5-2001 Drew Gugliotta
//
bool DtLightGetTransformNode(int lightID, MObject& node)
{
    // Check for error
    if( (lightID < 0) || (lightID >= local->light_ct) )
    {
        return false;
    }   

    node = local->lights[lightID].transformNode;
    return true;
}

//  ========== DtLightFindByTransformName ==========
//  SYNOPSIS
//      Returns the Light ID for the given light name
//
//  NOTES: Created 6-5-2001 Drew Gugliotta
//
int DtLightFindByTransformName(MString PartialPathName)
{
    for(int iLightID = 0; iLightID < local->light_ct; iLightID++)
    {
        MStatus returnStatus = MS::kSuccess;
        MFnDagNode fnTransNode( local->lights[iLightID].transformNode, 
            &returnStatus );

        MDagPath dagPath;
        returnStatus = fnTransNode.getPath( dagPath );

        if( MS::kSuccess == returnStatus )
        {
            if(dagPath.partialPathName() == PartialPathName)
            {
                return iLightID;
            }
        }
    }

    return -1;
}


//  ========== DtLightGetTextureName ==========
//  SYNOPSIS
//      Returns the Light ID for the given light name
//
//  NOTES: Created 6-5-2001 Drew Gugliotta
//
bool DtLightGetTextureName(int lightID, char* szTextureName, 
    unsigned int uiNameLen)
{
    // Check for error
    if( (lightID < 0) || (lightID >= local->light_ct) )
    {
        return false;
    }   


    MStatus status;
    MFnDagNode fnDagNode(local->lights[lightID].lightShapeNode, &status);

    MStringArray    arrayResult;
    MString connection = MString("listConnections ") + 
        fnDagNode.fullPathName() + MString(".color");

    status = MGlobal::executeCommand( connection, arrayResult );
    if ( status != MS::kSuccess ) 
        return false;


    MString command = MString("ls -st ") + arrayResult[0];

    status = MGlobal::executeCommand( command, arrayResult );
    if ( status != MS::kSuccess ) 
        return false;


    if ( (arrayResult.length() ==2) && arrayResult[1].length() &&
        (arrayResult[1] == MString("file") )  ) 
    {
        MString textureName = arrayResult[0];
        command = MString("getAttr ") + textureName +
            MString(".fileTextureName");
        
        MStringArray    result;
        status = MGlobal::executeCommand( command, result );
        
        if ( status != MS::kSuccess ) 
            return false;
        
        NiStrcpy(szTextureName, uiNameLen, result[0].asChar());
    }
    return true;
}

#ifdef WIN32
#pragma warning(default: 4244 4305)
#endif // WIN32
