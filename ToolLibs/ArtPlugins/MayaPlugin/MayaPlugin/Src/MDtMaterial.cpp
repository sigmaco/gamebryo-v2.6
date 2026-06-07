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
// $Revision: /main/maya3.0/1 $
// $Date: 2000/05/24 13:54:28 $
//

// Precompiled Headers
#include "MayaPluginPCH.h"
#include "maya/MImage.h"
#include "iffreader.h"
#include <maya/MFnBlinnShader.h>
#include <maya/MFnLambertShader.h>
#include <maya/MFnPhongShader.h>
#include <maya/MFnReflectShader.h>
#include <maya/MFloatPointArray.h>
#include <maya/MRenderUtil.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFloatMatrix.h>
#include <process.h>


// Forward definitions

int
generate_textureImages( MString &shaderName,
                        MString &texNode,
                        MString &texType,
                        MStringArray& texMap,
                        MDagPath &dagPath );


int generate_NiMultiTextureImages(
                        MString &shaderName,
                        MString &texNode,
                        MString &texType,
                        MStringArray& texMap,
                        MDagPath &dagPath,
                        MObject alShader );

unsigned char *CreateTextureFromNode(MObject TextureNode, int iUSamples, 
    int iVSamples);

static unsigned char *readTextureFile( MString textureFile, MString transFile,
                    int useTransparency, int &xres, int &yres, int &zres);

void SetMtlTypeName(MString &shaderName, MString &texType,
    char *szTextureName);

bool IsTextureExternal( MString textureName );

// Maya Games Data Translator header files

#include "MDt.h"
#include "MDtExt.h"

// Defines, macros, and magic numbers:
//
#define MT_BLOCK_SIZE       16
#define DT_BLOCK_SIZE       16
#define DT_VALID_BIT_MASK 0x00ffffff

typedef struct
{
    double  r;
    double  g;
    double  b;
    double  a;
} ShaderColor;

typedef struct
{
    float   r;
    float   g;
    float   b;
    float   a;
} MtlColor;

typedef struct
{
    float       shininess;
    float       transparency;
    float       diffuseCoeff;
    MtlColor    diffuse;
    MtlColor    emissive;
    MtlColor    color;
    MtlColor    specular;
    MtlColor    ambient;
    int         lightsource;
} Material;


class TextureMap
{
public:
    TextureMap()
    {
        m_pcMapName = NULL;
        m_pcMapTextureName = NULL;
        m_sMapL = 0;
        m_sMapK = 0;
        m_cMapFilter = 0;
        m_usMaxAnisotropy = 1;
        m_bMapFlipBook = 0;
        m_sMapFlipBookStartFrame = 0;
        m_sMapFlipBookEndFrame = 0;
        m_fMapFlipBookFramesPerSecond = 0;
        m_fMapFlipBookStartTime = 0;
        m_cMapFlipBookCycleType = 0;
        m_cMap = 0;
    }
    
    ~TextureMap()
    {
        NiFree(m_pcMapName);
        NiFree(m_pcMapTextureName);
    }
    
    char* m_pcMapName;
    char* m_pcMapTextureName;
    short m_sMapL;
    short m_sMapK;
    char m_cMapFilter;
    unsigned short m_usMaxAnisotropy;
    bool m_bMapFlipBook;
    short m_sMapFlipBookStartFrame;
    short m_sMapFlipBookEndFrame;
    float m_fMapFlipBookFramesPerSecond;
    float m_fMapFlipBookStartTime;
    char m_cMapFlipBookCycleType;
    char m_cMap;
};

// The material data structure:
//
class MtlStruct
{
public:
    MtlStruct();
    ~MtlStruct();
    void RemoveAllTextureMaps();
    TextureMap* AddTextureMap(const char* pcMapName);
    TextureMap* FindTextureMap(const char* pcMapName);




    char*       name;           // the material name
    Material*   mtl;            // pointer to the material data
    MObject     shader;         // Maya shader
    char*       texture_name;   // name of texture if it has one
    char*       texture_filename; // name of the texture filename 
    char*       ambient_name;
    char*       diffuse_name;
    char*       translucence_name;
    char*       incandescence_name;
    char*       bump_name;
    char*       shininess_name;
    char*       specular_name;
    char*       reflectivity_name;
    char*       reflected_name;
    int         hasAlpha;       // Flag if material has Alpha texture map
    int         isRflMap; // Flag indicating if material is a reflection map.

    int         m_iNumTextureMaps;
    TextureMap** m_ppkTextureMaps;

    // Specific for Gamebryo
    float       BumpMapLuminaScale;
    float       BumpMapLuminaOffset;
    float       BumpMapMat00;
    float       BumpMapMat01;
    float       BumpMapMat10;
    float       BumpMapMat11;

    float       ParallaxMapOffset;

    char        Shading;
    bool        VertexColorOverride;
    char        VertexColorSrcMode;
    char        VertexColorLightMode;

    char        TextureApplyMode;

    bool        TransparencyOverride;
    bool        TransparencyUseAlphaBlending;
    char        TransparencySource;
    char        TransparencyDestination;
    bool        TransparencyUseAlphaTesting;
    bool        TransparencyNoTestingSorter;
    short       TransparencyTestReference;
    char        TransparencyTestingTestMode;

    char        NBTMethod;
};
// =========================================================================
MtlStruct::MtlStruct()
{
    m_iNumTextureMaps = 0;
    m_ppkTextureMaps = NULL;
}
// ==========================================================================
MtlStruct::~MtlStruct()
{
    RemoveAllTextureMaps();
}
// ==========================================================================
void MtlStruct::RemoveAllTextureMaps()

{
    for(int iLoop = 0; iLoop < m_iNumTextureMaps; iLoop++)
    {
        NiExternalDelete m_ppkTextureMaps[iLoop];
    }

    NiExternalDelete[] m_ppkTextureMaps;

    m_ppkTextureMaps = NULL;
    m_iNumTextureMaps = 0;
}
// ==========================================================================
TextureMap* MtlStruct::AddTextureMap(const char* pcMapName)
{
    // Search to make sure the map doesn't already exist
    int iLoop;
    if(FindTextureMap(pcMapName) != NULL)
    {
        NIASSERT(0);  // Map already exists
        return NULL;
    }

    // Yes this will thrash memory but we can change it later

    // Create the new array
    TextureMap** pcTemp = NiExternalNew TextureMap*[m_iNumTextureMaps+1];

    // Copy over the entries
    for(iLoop = 0; iLoop < m_iNumTextureMaps; iLoop++)
    {
        pcTemp[iLoop] = m_ppkTextureMaps[iLoop];
    }

    // Delete the old array
    NiExternalDelete[] m_ppkTextureMaps;
    m_ppkTextureMaps = pcTemp;

    // Add the new Map
    m_ppkTextureMaps[m_iNumTextureMaps] = NiExternalNew TextureMap;
    size_t stLen = strlen(pcMapName) + 1;
    m_ppkTextureMaps[m_iNumTextureMaps]->m_pcMapName = NiAlloc(char, stLen);
    NiStrcpy(m_ppkTextureMaps[m_iNumTextureMaps]->m_pcMapName, stLen,
        pcMapName);


    // Increment the number of texture Maps
    m_iNumTextureMaps++;
    
    return m_ppkTextureMaps[m_iNumTextureMaps - 1];
}
// ==============================================================
TextureMap* MtlStruct::FindTextureMap(const char* pcMapName)
{
    // Search to make sure the map doesn't already exist
    int iLoop;
    for(iLoop = 0; iLoop < m_iNumTextureMaps; iLoop++)
    {
        if(!strcmp(m_ppkTextureMaps[iLoop]->m_pcMapName, pcMapName))
        {
            return m_ppkTextureMaps[iLoop]; 
        }
    }
    return NULL;
}
// ==============================================================


// The texture data structure:
//
typedef struct
{
    char*           name;       // the texture name
    char*           texture;    // Maya texture node name
    char*           filename;
    char*           nodeName;
    char*           placeName;
    float**         trans;      // the texture transform node
    int             filetexture;
    unsigned char*  image;      // the image of the file texture
    int             xres;
    int             yres;
    int             zres;
    MObject         kTextureNode;
    bool            bBumpMap;
} TexStruct;


// The object structure:
//
typedef struct
{
    int         shape_ct;   // count of nodes in the shapes cache
    int         mtl_ct;     // count of materials in mtl array
    int         mtl_sz;     // size of material array
    MtlStruct*  mtl;        // array of material structures
    int         tex_ct;     // count of textures in the tex array
    int         tex_sz;     // size of texture array
    TexStruct*  tex;        // array of texture structures
} DtPrivate;


// private function prototypes
//
int                mtlCreateCaches     (void);
static void         MtAddMtl            (DtPrivate* local, MObject mtl);
static MtlStruct*   MtFindMtl           (DtPrivate* local,char* name);

static int MtAddTex(DtPrivate* local,char* texture, char *filename,
    int filetexture,unsigned char* image, int xres,int yres,int zres, 
    MObject kTextureNode, bool bBumpMap);

static TexStruct*   MtFindTex           (DtPrivate* local,char* name);

Material* shader_material_multi(MObject alShader, Material* material,
    int index);

void shader_material_multi( MObject alShader, MtlStruct& material);

int shader_texture2(MObject alShader, MObject object, MDagPath &dagPath);
int shader_multiTextures(MObject alShader, MObject object, 
    MDagPath &dagPath);

int shader_NiMultiShader(MObject alShader, MObject object, 
    MDagPath &dagPath );

// private data
//
static DtPrivate* local = NULL;


MObject FollowInputAttribute(MObject kInitialObject, MString kAttributeName)
{
    MStatus kStatus;

    // Create the initial dependency node
    MFnDependencyNode dgNode(kInitialObject, &kStatus);

    if(kStatus != MS::kSuccess)
        return MObject::kNullObj;

    char szBuffer[256];
    NiStrcpy(szBuffer, 256, dgNode.name().asChar());

    // Find the attributes plug
    MPlug kPlug = dgNode.findPlug(kAttributeName, &kStatus);
    
    if(kStatus != MS::kSuccess)
        return MObject::kNullObj;

    MPlugArray kConnectedPlugs;
    kPlug.connectedTo(kConnectedPlugs, true, false, &kStatus);

    if(kStatus != MS::kSuccess)
        return MObject::kNullObj;
    
    MObject kDestinationNode = MObject::kNullObj;

    if(kConnectedPlugs.length() == 1)
    {
        kDestinationNode = kConnectedPlugs[0].node(&kStatus);

        if(kStatus != MS::kSuccess)
            return MObject::kNullObj;

/*  // Debugging
        MFnDependencyNode kdnDestination(kDestinationNode);
        char szBuffer[256];
        strcpy(szBuffer, kdnDestination.name().asChar());
*/
    }

    return kDestinationNode;
}

// ==============================================================
// ------------------------------  PUBLIC  FUNCTIONS  ---------------------
// ==============================================================

//  ========== DtExt_GetPlace2dTexture ==========
MFnDependencyNode DtExt_GetPlace2dTexture(const MObject& kShader,
    const char* pcTextureType, MStatus* pkStatus)
{
    // pcTextureType is assumed to be an assigned map on this material

    MFnDependencyNode kMtlDepend(kShader, pkStatus);
    MPlug kMapPlug = kMtlDepend.findPlug(pcTextureType);

    NIASSERT(kMapPlug.isConnected());

    MPlugArray kPlugArr;
    kMapPlug.connectedTo(kPlugArr, true, false, pkStatus);

    // I think the apiType of kAttribute3Float guarantees
    // that there will only be one connection, but
    // I'm not sure.  EH.
    NIASSERT(kPlugArr.length() == 1);

    MObject kDestObj = kPlugArr[0].node();

    // find kPlace2dTexture object from shader
    MItDependencyGraph::Direction direction = MItDependencyGraph::kUpstream;

    MItDependencyGraph::Traversal traversalType = 
        MItDependencyGraph::kBreadthFirst;

    MItDependencyGraph::Level level = MItDependencyGraph::kNodeLevel;
    MFn::Type filter = MFn::kPlace2dTexture;

    MItDependencyGraph dgIter( kDestObj, filter, direction,
        traversalType, level, pkStatus );

    // return first kPlace2dTexture object found 
    if (!dgIter.isDone())
    {        
        MObject k2dTexture = dgIter.thisNode( pkStatus );
        MFnDependencyNode k2dDepend(k2dTexture);
        return k2dDepend;
    }
    else
    {
        MFnDependencyNode kNone;
        if (pkStatus)
            *pkStatus = MS::kFailure;
        return kNone;
    }
}


//  ========== DtExt_MtlGetShader ==========
//
//  SYNOPSIS
//  Return the MObject for the material
//
int DtExt_MtlGetShader( int mtlID, MObject &obj )
{
    if ( local &&
        (mtlID >= 0) &&
        (mtlID < local->mtl_ct) )
    {
        obj = local->mtl[mtlID].shader;
        return 1;
    }
    return 0;

}

//  ========== DtMtlGetName ==========
//
//  SYNOPSIS
//  Return the material name used by the group.
//

int DtMtlGetName(int shapeID, int groupID, char** name)
{
    MObject group;
    int     ret = 0;

    if( DtShapeGetCount() == 0 ) 
    {
        return(0);
    }

    // Find the material group node by group ID.
    //

    ret = DtExt_ShapeGetShader( shapeID, groupID, group );

    if( (1 == ret) && (! group.isNull() ) ) 
    {
        *name = (char*)objectName( group );
    }
    
    return 1;

}  // DtMtlGetName //


//  ========== DtMtlGetNameByID ==========
//
//  SYNOPSIS
//  Return the material name used by the group.
//

int DtMtlGetNameByID( int LID, char** LName )
{
    *LName = NULL;

    if ( local )
        *LName = local->mtl[LID].name;

    return 1;

}  // DtMtlGetNameByID //


//  ============== DtMtlGetSceneCount ==============
//
//  SYNOPSIS
//  Return the number of materials in the scene.
//
int DtMtlGetSceneCount( int* LMatNum )
{
    *LMatNum = 0;
    
    if ( local )
        *LMatNum = local->mtl_ct;

    return 1;

}  // DtMtlGetSceneCount //


//  ========== DtMtlGetID ==========
//
//  SYNOPSIS
//  Return the material id used by the group.
//

int DtMtlGetID( int shapeID, int groupID, int* mtl_id )
{
    MObject     group;
    int         ret = 0;
    MtlStruct   *mtl = NULL;
    int         i = 0;

    *mtl_id = -1;
    if( DtShapeGetCount() == 0 ) 
    {   
        return(0);
    }

    if( NULL == local ) 
    {
        return 0;
    }

    // Find the material group node by group ID.
    //
    ret = DtExt_ShapeGetShader( shapeID, groupID, group );
    if( (1 == ret) && (! group.isNull()) )
    {
       if( NULL == objectName( group ) )
            return 0;

        for( i = 0; i < local->mtl_ct; i++ )
        {
            mtl = &local->mtl[i];
            
            if( mtl && (strcmp( mtl->name, objectName( group ) ) == 0) ) 
            {
                *mtl_id = i;
                return 1;
            }
        }
    }

    return 1;
}  // DtMtlGetID //

//  ========== DtMtlGetAmbientClr ==========
//
//  SYNOPSIS
//  Return the ambient color for the material.
//

int  DtMtlGetAmbientClr( char* name, int /* index */, float *red, 
    float *green, float *blue )
{
    MtlStruct   *material = NULL;

    // Initialize return values.
    //
    *red   = 0.0;
    *green = 0.0;
    *blue  = 0.0;

    // Search for the material by name.
    //
    if((material = MtFindMtl( local, name )) == NULL) 
    {
        return( 0 );
    }

    if ( material->mtl == NULL )
        return( 0 );

    // return values
    //
    *red   = material->mtl->ambient.r;
    *green = material->mtl->ambient.g;
    *blue  = material->mtl->ambient.b;

    return( 1 );
}  // DtMtlGetFClr //

//  ========== DtMtlGetDiffuseClr ==========
//
//  SYNOPSIS
//  Return the diffues color of the material.
//
int  DtMtlGetDiffuseClr( char* name, int /* index */, float *red, 
    float *green, float *blue )
{
    MtlStruct   *material = NULL;

    // Initialize return values
    //
    *red   = 0.0;
    *green = 0.0;
    *blue  = 0.0;

    // Search for the material by name
    //
    if((material = MtFindMtl( local, name )) == NULL) 
    {
        return( 0 );
    }

    if ( material->mtl == NULL )
        return( 0 );

    // Return values.
    //
    *red   = material->mtl->diffuse.r;
    *green = material->mtl->diffuse.g;
    *blue  = material->mtl->diffuse.b;

    return( 1 );

}  // DtMtlGetDiffuseClr //


//  ========== DtMtlGetDiffuseClr ==========
//
//  SYNOPSIS
//  Return the diffues color of the material.
//
int DtMtlGetDiffuseCoeff(char* name,int index, float* coeff)
{
    NI_UNUSED_ARG(index);
    MtlStruct   *material = NULL;

    // Initialize return values
    //
    *coeff   = 0.0;

    // Search for the material by name
    //
    if((material = MtFindMtl( local, name )) == NULL) 
    {
        return( 0 );
    }

    if ( material->mtl == NULL )
        return( 0 );

    // Return values.
    //
    *coeff   = material->mtl->diffuseCoeff;

    return( 1 );
}

//  ========== DtMtlGetSpecularClr ==========
//
//  SYNOPSIS
//  Return the specular color of the material.
//

int  DtMtlGetSpecularClr( char* name, int /* index */, float *red, 
    float *green, float *blue )
{
    MtlStruct   *material = NULL;

    // Initialize return values.
    //
    *red   = 0.0;
    *green = 0.0;
    *blue  = 0.0;

    // Search for the material by name.
    //
    if((material = MtFindMtl( local, name )) == NULL) 
    {
        return( 0 );
    }

    if ( material->mtl == NULL )
        return( 0 );
        
    // Return values.
    //
    *red =   material->mtl->specular.r;
    *green = material->mtl->specular.g;
    *blue =  material->mtl->specular.b;

    return( 1 );

}  // DtMtlGetSpecularClr //



//  ========== DtMtlGetEmissiveClr ==========
//
//  SYNOPSIS
//  Return the emissive color of the material.
//
int  DtMtlGetEmissiveClr( char* name, int /* index */, float *red, 
    float *green, float *blue )
{
    MtlStruct   *material = NULL;

    // Initialize return values.
    //
    *red   = 0.0;
    *green = 0.0;
    *blue  = 0.0;

    // Search for the material by name.
    //
    if((material = MtFindMtl( local, name )) == NULL) 
    {
        return( 0 );
    }

    if ( material->mtl == NULL )
        return( 0 );

    // Return values.
    //
    *red =   material->mtl->emissive.r;
    *green = material->mtl->emissive.g;
    *blue =  material->mtl->emissive.b;

    return( 1 );

}  // DtMtlGetEmissiveClr //


//  ========== DtMtlGetShininess ==========
//
//  SYNOPSIS
//  Return the shininess of the material.
//
int  DtMtlGetShininess( char* name, int /* index */, float *shininess )
{
    MtlStruct   *material = NULL;

    // Initialize return value.
    //
    *shininess = 0.0;

    // Search for the material by name.
    //
    if((material = MtFindMtl( local, name )) == NULL) return(0);

    if ( material->mtl == NULL )
         return( 0 );

    // return values
    //
    *shininess = material->mtl->shininess;

    return( 1 );

}  // DtMtlGetShininess //


//  ========== DtMtlGetTransparency ==========
//
//  SYNOPSIS
//  Return the transparency of the material.
//

int  DtMtlGetTransparency( char* name, int /* index */, float *transparency )
{
    MtlStruct   *material = NULL;

    // initialize return value
    //
    *transparency = 0.0;

    // search for the material by name
    //
    if((material = MtFindMtl( local, name )) == NULL) return( 0 );

    if ( material->mtl == NULL )
        return( 0 );

    // return values
    //
    *transparency =  material->mtl->transparency;

    return( 1 );

}  // DtMtlGetTransparency //

//  ========== DtMtlsUpdate ==========
//
//  SYNOPSIS
//  Update materials
//
void DtMtlsUpdate( void )
{
    unsigned int   LCnt,LNum;
    MtlStruct* LMtl;

    for( LCnt = 0, LNum = local->mtl_ct, LMtl = local->mtl;
            LCnt < LNum;
            LCnt++, LMtl++ )
    {
        shader_material_multi( LMtl->shader, LMtl->mtl, 0 );
    }

}

//  ========== DtExt_MtlGetIDByShader ==========
//
//  SYNOPSIS
//  Search the Materials by shader returning the ID
//

int DtExt_MtlGetIDByShader(MObject kShader,  int& mtlID)
{

    if (!local)
        return 0;

    for(int iLoop = 0; iLoop < local->mtl_ct; iLoop++)
    {
        if(local->mtl[iLoop].shader == kShader)
        {
            mtlID = iLoop;
            return 1;
        }
    }

    return 0;
}

//  ========== DtExt_MtlGetNumTextureMapsByID ==========
int DtExt_MtlGetNumTextureMapsByID(int iMaterialID)
{

    MtlStruct   *mtl = NULL;
        
    if( NULL == local )
    {
        return(0);
    }   
    if( iMaterialID >= local->mtl_ct )
    {
        DtExt_Err(" Material #%d. was not found, bad material ID ?\n",
            iMaterialID);
        return(0);
    }   
    
    mtl = &local->mtl[iMaterialID];

    return mtl->m_iNumTextureMaps;
}

//  ========== DtExt_MtlGetTextureMapNameByID ==========
//
//  SYNOPSIS
//

const char* DtExt_MtlGetTextureMapNameByID(int iMaterialID, int iMap)
{

    MtlStruct   *mtl = NULL;
        
    if( NULL == local )
    {
        return(0);
    }   
    if( iMaterialID >= local->mtl_ct )
    {
        DtExt_Err(" Material #%d. was not found, bad material ID ?\n",
            iMaterialID);
        return(0);
    }   
    
    mtl = &local->mtl[iMaterialID];

    return mtl->m_ppkTextureMaps[iMap]->m_pcMapName;
}

//  ========== DtTextureGetSceneCount ==========
//
//  SYNOPSIS
//  Return the count of textures in the entire scene.
//

int  DtTextureGetSceneCount(int *count)
{
    if( NULL != local )
    {
        *count = local->tex_ct;
    }

    return(1);

}  // DtTextureGetSceneCount //

//  ========== DtTextureGetFileName ==========
//
//  SYNOPSIS
//  Return the texture file name of the material.
//
int  DtTextureGetFileName(char *mtl_name, char **name)
{

    // initialize return value
    //
    *name = NULL;


    // look up the material by name
    //

    MtlStruct  *mtl = MtFindMtl(local, mtl_name);
    if (mtl == NULL)
    {
        return(0);  // error, bad material name
    }


    // return texture name
    //
    *name = mtl->texture_filename;
    return(1);

}  // DtTextureGetFileName //


//  ========== DtTextureGetName ==========
//
//  SYNOPSIS
//  Return the texture name of the material.
//

int  DtTextureGetName(char* mtl_name,char** name)
{
    // initialize return value
    //
    *name = NULL;

    // look up the material by name
    //
    MtlStruct  *mtl = MtFindMtl(local, mtl_name);
    if(mtl == NULL) 
    {
        DtExt_Err("\"%s\" was not found, bad material name ?\n", mtl_name );
        return(0);  // error, bad material name
    }

    // return texture name
    //
    *name = mtl->texture_name;
    return(1);

}  // DtTextureGetName //

//  ========== DtTextureGetID ==========
//
//  SYNOPSIS
//  Return the texture id of the material.
//
int  DtTextureGetID(int mtl_id,int* txt_id)
{
    int     i = 0;
    TexStruct   *tex = NULL;
    MtlStruct   *mtl = NULL;

    // initialize return value
    //
    *txt_id = -1;

    if( NULL == local ) 
    {
        return 0;
    }
    if(mtl_id >= local->mtl_ct)
    {
        DtExt_Err("Material #%d. was not found, bad material ID ?\n",mtl_id);
        return(0);
    }

    mtl = &local->mtl[mtl_id];
    if(mtl && mtl->texture_name!=NULL)
    {
        for (i = 0; i < local->tex_ct; i++)
        {
            tex = &local->tex[i];
            if(tex && (strcmp(tex->name, mtl->texture_name) == 0) ) 
            {
                *txt_id = i;
                return(1);
            }
        }
    }
    return(1);

}  // DtTextureGetID //


//  ========== DtTextureGetIDMulti ==========
//
//  SYNOPSIS
//  Return the texture id of the material.
//
int  DtTextureGetIDMulti(int mtl_id,const char *texType, int* txt_id)
{
    int     i = 0;
    TexStruct   *tex = NULL;
    MtlStruct   *mtl = NULL;
    char        *name = NULL;
    
    // initialize return value
    //
    *txt_id = -1;

    if( NULL == local )
    {
        return 0;
    }
    if(mtl_id >= local->mtl_ct)
    {
        DtExt_Err("Material #%d. was not found, bad material ID ?\n",mtl_id);
        return(0);
    }

    mtl = &local->mtl[mtl_id];

    if ( !strcmp( texType, "color" ) )
        name = mtl->texture_name; 
    else if ( !strcmp( texType, "opacity" ) )
        name = mtl->texture_name;
    else if ( !strcmp( texType, "ambient" ) )
        name = mtl->ambient_name;
    else if ( !strcmp( texType, "diffuse" ) )
        name = mtl->diffuse_name;
    else if ( !strcmp( texType, "translucence" ) )
        name = mtl->translucence_name;
    else if ( !strcmp( texType, "incandescence" ) )
        name = mtl->incandescence_name;
    else if ( !strcmp( texType, "bump" ) )
        name = mtl->bump_name;
    else if ( !strcmp( texType, "shininess" ) )
        name = mtl->shininess_name;
    else if ( !strcmp( texType, "specular" ) )
        name = mtl->specular_name;
    else if ( !strcmp( texType, "reflectivity" ) )
        name = mtl->reflectivity_name;
    else if ( !strcmp( texType, "reflected" ) )
        name = mtl->reflected_name;

    for(int iLoop = 0; iLoop < mtl->m_iNumTextureMaps; iLoop++)
    {
        if( !strcmp(texType, mtl->m_ppkTextureMaps[iLoop]->m_pcMapName))
            name = mtl->m_ppkTextureMaps[iLoop]->m_pcMapTextureName;
    }


    if ( mtl && name != NULL)
    {
        for (i = 0; i < local->tex_ct; i++)
        {
            tex = &local->tex[i];
            if(tex && (strcmp(tex->name, name) == 0) )
            {
                *txt_id = i;
                return(1);
            }
        }
    }

    return(1);

}  // DtTextureGetID //

//  ========== DtTextureGetNameID ==========
//
//  SYNOPSIS
//  Return the texture name by texture ID. The name is
//  stored in an internal buffer and should not be modified
//  by the caller.
//

int  DtTextureGetNameID(int textureID,char** name)
{
    // check for error
    //
    if(textureID >= local->tex_ct)
    {
        *name = NULL;
        return(0);
    }

    // return a pointer to the name
    //
    *name = local->tex[textureID].name;
    return(1);

}  // DtTextureNameID //

//  ========== DtTextureGetNameID ==========
//
//  SYNOPSIS
//  Return the texture name by texture ID. The name is
//  stored in an internal buffer and should not be modified
//  by the caller.
//

int  DtTextureGetFileNameID(int textureID, char **name)
{

    // check for error
    //

    if (textureID >= local->tex_ct)
    {
    *name = NULL;
    return(0);
    }

    // return a pointer to the name
    //
    *name = local->tex[textureID].filename;
    return(1);

}  // DtTextureFileNameID //

//  ========== DtTextureGetBumpMapID ==========
//
//  SYNOPSIS
//  Return the texture name by texture ID. The name is
//  stored in an internal buffer and should not be modified
//  by the caller.
//

bool DtTextureGetBumpMapID(int textureID)
{

    // check for error
    //

    if (textureID >= local->tex_ct)
    {
    return(false);
    }

    // return the bump map for this texture
    //
    return local->tex[textureID].bBumpMap;

}  // DtTextureGetBumpMapID //

//  ========== DtTextureGetTextureNodeID ==========
//
//  SYNOPSIS
//  Return the texture node by texture ID. 
//
int  DtTextureGetTextureNodeID(int textureID, MObject& kTextureNode)
{

    // check for error
    //
    if (textureID >= local->tex_ct)
    {
        kTextureNode = MObject::kNullObj;
        return(0);
    }

    // return a pointer to the name
    //
    kTextureNode = local->tex[textureID].kTextureNode;
    return(1);

}  // DtTextureGetTextureNodeID //

//  ========== DtTextureGetWrap ==========
//
//  SYNOPSIS
//  Return the wrap type of the texture.
//
int  DtTextureGetWrap(char* name, int *horizontal, int *vertical)
{
    // double wraps, wrapt;

    // initialize return value
    //
    *horizontal = DT_REPEAT;
    *vertical = DT_REPEAT;

    // search for the texture by name
    //
    TexStruct  *tex = MtFindTex(local, name);

    if ( tex == NULL ) return(0);
    if ( tex->placeName == NULL ) return ( 0 );

    // get pointer to texture node
    //
    MStatus status;
    MString command;
    MIntArray wrapU, wrapV;

    command = MString("getAttr ") + MString( tex->placeName ) +
                                        MString(".wrapU");

    status = MGlobal::executeCommand( command, wrapU );

    if ( status != MS::kSuccess )
    {
           printf( "bad return from \"%s\"\n", command.asChar() );
           return 0;
    }

    command = MString("getAttr ") + MString( tex->placeName ) +
                                        MString(".wrapV");
                                        
    status = MGlobal::executeCommand( command, wrapV );
    
    if ( status != MS::kSuccess )
    {
           printf( "bad return from \"%s\"\n", command.asChar() );
           return 0;
    }      

    // return the horizontal wrap type
    //
    *horizontal = wrapU[0] ? DT_REPEAT : DT_CLAMP;

    // return the vertical wrap type
    //
    *vertical = wrapV[0] ? DT_REPEAT : DT_CLAMP;

    return(1);

}  // DtTextureGetWrap //

//  ========== DtTextureGetImageSizeByID ==========
//
//  SYNOPSIS
//  Return the size and number of components in the texture image.
//
int DtTextureGetImageSizeByID(int LID, int* LXSize, int* LYSize, 
    int* LComponents)
{
    *LXSize = *LYSize = *LComponents = 0;

    TexStruct*  LTex=&(local->tex[LID]);
    
    if ( LTex == NULL ) 
        return(0);

    *LXSize = LTex->xres;
    *LYSize = LTex->yres;
    *LComponents = LTex->zres;
    
    return(1);

}  // DtTextureGetImageSizeByID //

//  ========== DtTextureGetImageSize ==========
//
//  SYNOPSIS
//  Return the size and number of components in the texture image.
//
int DtTextureGetImageSize(char* name,int* width,int* height,int* components)
{
    // initialize return value
    //
    *width=0;
    *height=0;
    *components=0;

    // search for the texture by name
    //
    TexStruct*  tex=MtFindTex(local,name);
    if ( tex == NULL ) 
        return(0);

    // return values
    //
    *width = tex->xres;
    *height = tex->yres;
    *components = tex->zres;

    return(1);

}  // DtTextureGetImageSize //


//  ========== DtTextureGetImageByID ==========
//
//  SYNOPSIS
//  Return the texture image.
//
int DtTextureGetImageByID(int LID,unsigned char** LImage)
{
    TexStruct*  LTex=&(local->tex[LID]);

    *LImage = (unsigned char*)LTex->image;

    return(1);

} // DtTextureGetImageByID //


//  ========== DtTextureGetImage ==========
//
//  SYNOPSIS
//  Return the texture image.
//
int  DtTextureGetImage(char* name, unsigned char** img)
{
    // initialize return value
    //
    *img = NULL;

    // search for the texture by name
    //

    TexStruct*  tex=MtFindTex(local,name);
    if ( tex == NULL ) 
    {
        DtExt_Err(" \"%s\" Texture was Not Found\n",name);
        return(0);
    }

    // return value
    //
    *img = (unsigned char* )tex->image;

    return(1);

}  // DtTextureGetImage //

//  ========= addElemnt  ===============
//
//  Helper function to ensure unique values in the keyframe array
//

/*static bool addElement( MIntArray  *intArray, int newElem )
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
}*/

/*
 *  ========== mtlNew ==========
 *
 *  SYNOPSIS
 *  A private function. Used to reset all internal states and populate
 *  material caches.
 *
 *  Return value:
 *    0: Success
 *    1: Cancelled
 *    2: Error
 */

int  mtlNew(void)
{
    int i = 0;

    // create the object instance structure
    //
    if( NULL == local ) 
    {
        local = (DtPrivate *)NiAlloc(DtPrivate, 1);
        memset(local, 0, sizeof(DtPrivate));
    }
    
    local->shape_ct = 0;

    // Free data within each material structure.
    //
    for( i = 0; i < local->mtl_ct; i++ )
    {
        if( NULL != local->mtl[i].name ) 
        {
            NiFree( local->mtl[i].name );
            local->mtl[i].name = NULL;
        }
        if( NULL != local->mtl[i].texture_name ) 
        {
            NiFree( local->mtl[i].texture_name );
            local->mtl[i].texture_name = NULL;
        }
    }

    // Free the material structure array.
    //
    if( NULL != local->mtl )
    {
        NiFree( local->mtl );
        local->mtl = NULL;
        local->mtl_ct = 0;
        local->mtl_sz = 0;
    }

    // Free the data within each texture structure.
    //
    for( i = 0; i < local->tex_ct; i++ )
    {
        NiFree( local->tex[i].name );
        local->tex[i].name = NULL;
    }

    // Free the texture structure array.
    //
    if( NULL != local->tex )
    {
        NiFree(local->tex);
        local->tex = NULL;
        local->tex_ct = 0;
        local->tex_sz = 0;
    }


    // Now call the routine to setup the cache for the shader values.
    //
    return mtlCreateCaches();


}  // mtlNew //



//  ========== mtlCreateCaches ==========
//
//  SYNOPSIS
//  Create a node cache of the root nodes of all the
//  shapes. Create a list of materials. Create a list
//  of textures.
//
// Return value:
//   0: Success
//   1: Cancelled
//   2: Error

int  mtlCreateCaches( void )
{
    int          i = 0;
    int          j = 0;
    int          group_ct = 0;
    MObject      shader;
    MObject      object;
    MDagPath     dagPath;
    MtlStruct    *mtl = NULL;

    local->shape_ct = DtShapeGetCount();
    
    int count = 0;
    DtTextureGetSceneCount( &count );
    if(DtExt_HaveGui() == true)
    {
        // Update the progress window
        MProgressWindow::setProgressStatus("Creating Material Caches");
        MProgressWindow::setProgressRange(0, local->shape_ct + 
            gParticleSystemManager.GetNumParticles());
        MProgressWindow::setProgress(0);
    }

    for( i = 0; i < local->shape_ct; i++ )
    {
        if(DtExt_HaveGui() == true)
        {
            if (MProgressWindow::isCancelled())
                return 1;

            // Increment the progress window
            MProgressWindow::advanceProgress(1);
        }

        group_ct = DtGroupGetCount( i );

        DtExt_ShapeGetDagPath( i, dagPath );
        
        for( j = 0; j < group_ct; j++ )
        {
            if( DtExt_ShapeGetShader( i, j, shader ) ) 
            {   
                MtAddMtl(local, shader);

                DtExt_ShapeGetOriginal( i, j, object );
                
                // Assume no texture on shaders at this point.  
                // Check for shader/transparency textures

                if( shader_texture2( shader, object, dagPath ) )
                {
                    DtExt_ShapeIncTexCnt( i );
                }

                int iExtra = shader_NiMultiShader( shader, object, dagPath ); 
                for(int xx=0;xx<iExtra;xx++)
                {
                    DtExt_ShapeIncTexCnt( i );
                }

                // Now if the user wants the other possible textures
                // to be sampled then this flag needs to be set.
                // Off by default, due to the extra processing time
                // that not all translators need this done.
                
                if ( DtExt_MultiTexture() )
                {
                    if ( shader_multiTextures( shader, object, dagPath ) )
                    {
                        DtExt_ShapeIncTexCnt( i );
                    }   
                }
                
                // See if this material is being used as a reflection map.
                //
                mtl = MtFindMtl( local, (char* )objectName( shader ) );
                mtl->isRflMap = false;
            }
        }
    }


    // Create the Material Entries for Particle Systems
    for(i = 0; i < gParticleSystemManager.GetNumParticles(); i++)
    {
        if(DtExt_HaveGui() == true)
        {
            if (MProgressWindow::isCancelled())
                return 1;

            // Increment the progress window
            MProgressWindow::advanceProgress(1);
        }

        kMDtParticles* kParticles = gParticleSystemManager.GetParticles(i);

        MtAddMtl(local, kParticles->m_MObjectShader);
        
        // Assume no texture on shaders at this point.  
        // Check for shader/transparency textures
        
        if( shader_texture2( kParticles->m_MObjectShader, 
                             kParticles->m_MObjectParticles, 
                             kParticles->m_ShapePath ) )
        {
            DtExt_ShapeIncTexCnt( i );
        }
        
        int iExtra = shader_NiMultiShader( kParticles->m_MObjectShader, 
            kParticles->m_MObjectParticles, kParticles->m_ShapePath );
        for(int xx=0;xx<iExtra;xx++)
        {
            DtExt_ShapeIncTexCnt( i );
        }
    }
    
    return 0;
}  // mtlCreateCaches //


//---------------------------------------------------------------------------

bool DtTextureGetRepeatOffsetRotateUVs(int mtlID, const char* pcTextureType,
    float& fRepeatUVx, float& fRepeatUVy, float& fOffsetUVx, 
    float& fOffsetUVy, float& fRotate)
{
    fRepeatUVx = 1.0;
    fRepeatUVy = 1.0;
    fOffsetUVx = 0.0f;
    fOffsetUVy = 0.0f;
    fRotate = 0.0f;

    char *pcTextureName;
    if (DtTextureGetNameID(mtlID, &pcTextureName))
    {
        if (!IsTextureExternal( MString(pcTextureName) ))
        {
            return true;
        }
    }

    MObject kMtlNode;
    int iStatus = DtExt_MtlGetShader( mtlID, kMtlNode );
    if (iStatus != 1)
        return false;

    MStatus kStatus;
    // get place2dTexture object for this texture
    MFnDependencyNode kPlace2d = DtExt_GetPlace2dTexture(kMtlNode, 
        pcTextureType, &kStatus);

    if (kStatus != MS::kSuccess)
        return true; // Return using defaults.

    MPlug kPlug = kPlace2d.findPlug("repeatU", &kStatus);
    kPlug.getValue(fRepeatUVx);

    kPlug = kPlace2d.findPlug("repeatV", &kStatus);
    kPlug.getValue(fRepeatUVy);

    kPlug = kPlace2d.findPlug("offsetU", &kStatus);
    kPlug.getValue(fOffsetUVx);

    kPlug = kPlace2d.findPlug("offsetV", &kStatus);
    kPlug.getValue(fOffsetUVy);

    kPlug = kPlace2d.findPlug("rotateUV", &kStatus);
    kPlug.getValue(fRotate);

    return true;
}





void
DtExt_MaterialDelete()
{
    int i = 0;
    MtlStruct* mtl = NULL;
    TexStruct* tex = NULL;

    if( local == NULL ) 
    {
        return;
    }
    if( local->mtl != NULL )
    {
        for ( i = 0; i < local->mtl_ct; i++ )
        {
            mtl = &local->mtl[i];
            NiFree( mtl->name );
            NiFree( mtl->mtl );
            if( mtl->texture_name )
                NiFree( mtl->texture_name );
            if( mtl->ambient_name )
                NiFree( mtl->ambient_name );
            if( mtl->diffuse_name )
                NiFree( mtl->diffuse_name );
            if( mtl->translucence_name )
                NiFree( mtl->translucence_name );
            if( mtl->incandescence_name )
                NiFree( mtl->incandescence_name );
            if( mtl->bump_name )
                NiFree( mtl->bump_name );
            if( mtl->shininess_name )
                NiFree( mtl->shininess_name );
            if( mtl->specular_name )
                NiFree( mtl->specular_name );
            if( mtl->reflectivity_name )
                NiFree( mtl->reflectivity_name );
            if( mtl->reflected_name )
                NiFree( mtl->reflected_name );
            if( mtl->texture_filename )
                NiFree( mtl->texture_filename );

            for(int iLoop = 0; iLoop < mtl->m_iNumTextureMaps; iLoop++)
            {
                NiExternalDelete mtl->m_ppkTextureMaps[iLoop];
            }
            NiExternalDelete[] mtl->m_ppkTextureMaps;
            mtl->name = NULL; 
            mtl->mtl = NULL; 
            mtl->texture_name = NULL; 
            mtl->ambient_name = NULL; 
            mtl->diffuse_name = NULL;
            mtl->translucence_name = NULL;
            mtl->incandescence_name = NULL;
            mtl->bump_name = NULL;
            mtl->shininess_name = NULL;
            mtl->specular_name = NULL;
            mtl->reflectivity_name = NULL;
            mtl->reflected_name = NULL;
        }
        NiFree( local->mtl );
        local->mtl = NULL;
    }
    if( local->tex != NULL )
    {
        for ( i = 0; i < local->tex_ct; i++ )
        {
            tex = &local->tex[i];
            if( tex->image )
                NiFree( tex->image );
            if( tex->name )
                NiFree( tex->name );
            if( tex->placeName )
                NiFree( tex->placeName );
            if( tex->nodeName )
                NiFree( tex->nodeName );
            if( tex->filename )
                NiFree( tex->filename );

            tex->image = NULL;
            tex->name = NULL;
            tex->placeName = NULL;
            tex->nodeName = NULL;
            tex->filename = NULL;
        }
        NiFree( local->tex );
        local->tex = NULL;
    }
    NiFree( local );
    local = NULL;
}

//  ========== MtAddMtl ==========
//
//  SYNOPSIS
//  Add a material node to the material list.
//  First search the list to make sure the node is 
//  not already in the list.
//
static void  MtAddMtl( DtPrivate *local_2, MObject mtl_node )
{
    MtlStruct   *mtl = NULL;
    Material    *mtl_data = NULL;

    // Simply return if node is NULL.
    //
    if( mtl_node.isNull() ) 
    {
        return;
    }

    // Do nothing if material already in list.
    //
    if( NULL != MtFindMtl(local_2, (char* )objectName( mtl_node ) ) ) 
    {
        DtExt_Msg("MtAddMtl: \"%s\" is already in list\n", 
            objectName( mtl_node ) );
        return;
    }

    // Increase array if needed.
    //
    if( local_2->mtl_ct == local_2->mtl_sz )
    {
        //long current_sz = local_2->mtl_sz * sizeof(MtlStruct);
        local_2->mtl_sz += MT_BLOCK_SIZE;
        local_2->mtl = (MtlStruct *)NiRealloc(local_2->mtl, 
            local_2->mtl_sz * sizeof(MtlStruct));

        memset( &local_2->mtl[local_2->mtl_ct], 0, 
            MT_BLOCK_SIZE*sizeof(MtlStruct) );
    }

    // Add material to list.
    //
    mtl = &local_2->mtl[local_2->mtl_ct++];

    const char* pcName = objectName( mtl_node );
    mtl->name = NiAlloc(char, strlen(pcName) + 1);
    NiStrcpy(mtl->name, strlen(pcName) + 1, pcName);

    mtl_data = (Material *)NiAlloc(Material, 1);
    memset(mtl_data, 0, sizeof(Material));

    mtl->mtl = shader_material_multi( mtl_node, mtl_data, 0 );
    mtl->texture_name = NULL;
    mtl->shader = mtl_node;
    mtl->isRflMap = 0;

    // Initialize the Gamebryo Material Values
    mtl->Shading = -1;
    mtl->VertexColorOverride = false;
    mtl->VertexColorSrcMode = -1;
    mtl->VertexColorLightMode = -1;
    mtl->TextureApplyMode = -1;
    mtl->TransparencyOverride = false;
    mtl->TransparencyUseAlphaBlending = false;
    mtl->TransparencySource = -1;
    mtl->TransparencyDestination = -1;
    mtl->TransparencyUseAlphaTesting = false;
    mtl->TransparencyNoTestingSorter = false;
    mtl->TransparencyTestReference = 0;
    mtl->TransparencyTestingTestMode = -1;
    mtl->NBTMethod = -1;


    shader_material_multi(mtl_node, *mtl);
}  // MtAddMtl //


//  ========== MtFindMtl ==========
//
//  SYNOPSIS
//  Search for a material by name. This function searches
//  the material list for the material of the given name.
//  If found, a pointer to that material structure is returned.
//  Otherwise, NULL is returned.
//
static MtlStruct  *MtFindMtl(DtPrivate *local, char* name)
{
    MtlStruct   *mtl;
    int         i;

    if( NULL == local ) 
    {
        return( NULL );
    }
    // Search the material array.
    //
    for( i = 0; i < local->mtl_ct; i++)
    {
        mtl = &local->mtl[i];
        if( 0 == strcmp(mtl->name, name) ) 
        {
            return( mtl );
        }
    }

    // Material not found.
    //
    return( NULL );

}  // MtFindMtl //




//  ========== MtAddTex ==========
//
//  SYNOPSIS
//  Add a texture (and texture transform) to the texture list.
//  First search the list to make sure the texture is not 
//  already in the list.
//
static int MtAddTex(DtPrivate* local,char *texture, char *filename,
                    int filetexture,unsigned char* image, 
                    int xres, int yres, int zres, MObject kTextureNode,
                    bool bBumpMap)
{
    // simply return if nodes are NULL
    //
    if( texture == NULL ) return 0;

    // do nothing if texture is already in the list
    //
    if(MtFindTex(local, texture) != NULL) 
    {
        DtExt_Msg("MtAddTex: \"%s\" is already in list\n", texture );
        return 0;
    }

    // increase array if needed
    //
    if(local->tex_ct == local->tex_sz)
    {
        local->tex_sz += MT_BLOCK_SIZE;
        local->tex = (TexStruct *)NiRealloc(local->tex, 
            local->tex_sz * sizeof(TexStruct));
    }

    // add texture to list
    //
    TexStruct  *tex = &local->tex[local->tex_ct++];

    // Initialize the tex structure
    memset(tex, 0, sizeof(TexStruct));

    if ( texture ) 
    {
        tex->name    = NiStrdup( texture );
        tex->nodeName = NiStrdup( texture );
    }

    tex->texture = NULL;

    if ( filename )
        tex->filename = NiStrdup( filename );

    // Lets now add reference to the Placement Node
    // We will check both 2d and 3d for now.

    tex->placeName = NULL;

    MString command = "listConnections -type place2dTexture " + 
                                MString( texture );
    MStatus status;
    MStringArray result;
    
    status = MGlobal::executeCommand( command, result );

    if ( status == MS::kSuccess ) 
    {
        if ( result.length() > 0 ) 
        {
            tex->placeName = NiStrdup( result[0].asChar() );
        }
    }

    tex->trans   = NULL;

    tex->filetexture = filetexture;
    tex->image   = image;
    tex->xres    = xres;
    tex->yres    = yres;
    tex->zres    = zres;

    tex->kTextureNode = kTextureNode;
    tex->bBumpMap = bBumpMap;

    return 1;

}  // MtAddTex //

//  ========== MtFindTex ==========
//
//  SYNOPSIS
//  Search for a material by name. This function searches
//  the material list for the material of the given name.
//  If found, a pointer to that material structure is returned.
//  Otherwise, NULL is returned.
//

static TexStruct  *MtFindTex(DtPrivate *local, char* name)
{
    TexStruct   *tex = NULL;
    int     i = 0;

    // search the texture array
    //
    for (i = 0; i < local->tex_ct; i++)
    {
        tex = &local->tex[i];
        if(strcmp(tex->name, name) == 0) return(tex);
    }

    // texture not found
    //
    return(NULL);

}  // MtFindTex //

// static double uoffset, voffset, rotation, uscale, vscale;
// static double tuoffset, tvoffset, trotation, tuscale, tvscale;

static MDoubleArray Save_rotateFrame, Save_translateFrame, Save_repeatUV;
static MDoubleArray Save_rotateUV, Save_offset;

void
saveTransforms( MString textureName )
{

    if( textureName.length() == 0 )
        return; 

    int     placeFound = false;

    MString placeName;
    
    MString command = "listConnections -type place2dTexture " + textureName;
    MStatus status;
    MStringArray result;
    
    status = MGlobal::executeCommand( command, result );
    
    if ( status == MS::kSuccess )
    {   
        if ( result.length() > 0 )
        {   
            placeName = result[0];
            placeFound = true;
        }
    }

    if ( placeFound )
    {
        MStatus status;
        MString command;
        MIntArray   wrapU, wrapV;
        
        command = MString("getAttr ") + placeName + 
            MString(".translateFrame");

        status = MGlobal::executeCommand( command, Save_translateFrame );
        command = MString("getAttr ") + placeName + MString(".rotateFrame");
        status = MGlobal::executeCommand( command, Save_rotateFrame );

        command = MString("getAttr ") + placeName + MString(".offset");
        status = MGlobal::executeCommand( command, Save_offset );
        command = MString("getAttr ") + placeName + MString(".rotateUV");
        status = MGlobal::executeCommand( command, Save_rotateUV );

        command = MString("getAttr ") + placeName + MString(".repeatUV");
        status = MGlobal::executeCommand( command, Save_repeatUV );
        command = MString("getAttr ") + placeName + MString(".wrapU");
        status = MGlobal::executeCommand( command, wrapU );
        command = MString("getAttr ") + placeName + MString(".wrapV");
        status = MGlobal::executeCommand( command, wrapV );

        if (IsTextureExternal( textureName ))
        {

#if defined(_MSC_VER) && _MSC_VER >= 1400
            unsigned int uiBuffSize;
            if (getenv_s(&uiBuffSize, NULL, 0, "MDT_USE_TEXTURE_TRANSFORMS") 
                == 0 && uiBuffSize > 0)
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
            if ( getenv("MDT_USE_TEXTURE_TRANSFORMS" ) )
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
            {
                command = MString("setAttr ") + placeName 
                                                + MString(".rotateFrame 0");
                status = MGlobal::executeCommand( command );
                command = MString("setAttr ") + placeName + 
                    MString(".translateFrame 0 0");

                status = MGlobal::executeCommand( command );
            } else {
                command = MString("setAttr ") + placeName
                                                + MString(".rotateUV 0");
                status = MGlobal::executeCommand( command );
                command = MString("setAttr ") + placeName + 
                                                MString(".offset 0 0");
                status = MGlobal::executeCommand( command );
            }

            if ( wrapU[0] )
            {
                command = MString("setAttr ") + placeName + 
                                                MString(".repeatU 1");
                status = MGlobal::executeCommand( command );
            }
            if ( wrapV[0] )
            {
                command = MString("setAttr ") + placeName +
                                                MString(".repeatV 1");
                status = MGlobal::executeCommand( command );
            }

        }
    }
}

void
restoreTransforms( MString textureName )
{
    if( textureName.length() == 0 )
        return;
    
    int     placeFound = false;

    MString placeName;
    
    MString command = "listConnections -type place2dTexture " + textureName;
    MStatus status;
    MStringArray result;
    
    status = MGlobal::executeCommand( command, result );
    
    if ( status == MS::kSuccess )
    {   
        if ( result.length() > 0 )
        {   
            placeName = result[0];
            placeFound = true;
        }
    }

    if ( placeFound && IsTextureExternal( textureName ))
    {   
        char command[1024];

        NiSprintf( command, 1024, "setAttr %s.rotateFrame %f", 
                                placeName.asChar(), Save_rotateFrame[0] );
        status = MGlobal::executeCommand( command );

        NiSprintf( command, 1024, "setAttr %s.translateFrame %f %f", 
                        placeName.asChar(), Save_translateFrame[0], 
                                            Save_translateFrame[1] );
        status = MGlobal::executeCommand( command );

        NiSprintf( command, 1024, "setAttr %s.rotateUV %f",
                                placeName.asChar(), Save_rotateUV[0] );
        status = MGlobal::executeCommand( command );

        NiSprintf( command, 1024, "setAttr %s.offset %f %f",
                        placeName.asChar(), Save_offset[0],
                                            Save_offset[1] );
        status = MGlobal::executeCommand( command );

        NiSprintf( command, 1024, "setAttr %s.repeatUV %f %f", 
                     placeName.asChar(), Save_repeatUV[0], Save_repeatUV[1] );
        status = MGlobal::executeCommand( command );
    }
}


void CreateMultiShaderMaterial(MObject alShader, Material *material)
{
    // But for now lets return a default material setting
    
    material->color.r = 0.5;
    material->color.g = 0.5;
    material->color.b = 0.5;
    
    material->diffuse.r = 0.5;
    material->diffuse.g = 0.5;
    material->diffuse.b = 0.5;
    
    material->ambient.r = 0.5;
    material->ambient.g = 0.5;
    material->ambient.b = 0.5;
    
    material->diffuseCoeff = 1.0;
    
    material->transparency = 0.0;

    MStatus kStatus;

    MFnDependencyNode kNode;
    kStatus = kNode.setObject(alShader);

    NIASSERT(kStatus == MS::kSuccess);

    MPlug kPlug;
    
    kPlug = kNode.findPlug(MString("MaterialDiffuseR"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material->diffuse.r);

    kPlug = kNode.findPlug(MString("MaterialDiffuseG"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material->diffuse.g);

    kPlug = kNode.findPlug(MString("MaterialDiffuseB"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material->diffuse.b);


    kPlug = kNode.findPlug(MString("MaterialSpecularR"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material->specular.r);

    kPlug = kNode.findPlug(MString("MaterialSpecularG"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material->specular.g);

    kPlug = kNode.findPlug(MString("MaterialSpecularB"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material->specular.b);



    kPlug = kNode.findPlug(MString("MaterialAmbientR"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material->ambient.r);

    kPlug = kNode.findPlug(MString("MaterialAmbientG"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material->ambient.g);

    kPlug = kNode.findPlug(MString("MaterialAmbientB"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material->ambient.b);



    kPlug = kNode.findPlug(MString("MaterialEmittanceR"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material->emissive.r);

    kPlug = kNode.findPlug(MString("MaterialEmittanceG"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material->emissive.g);

    kPlug = kNode.findPlug(MString("MaterialEmittanceB"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material->emissive.b);



    kPlug = kNode.findPlug(MString("MaterialShineness"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material->shininess);


    kPlug = kNode.findPlug(MString("transparencyR"), &kStatus);
    if(kStatus == MS::kSuccess)
    {
        float fR;
        kPlug.getValue(fR);
        material->transparency += fR;
    }
   
    kPlug = kNode.findPlug(MString("transparencyG"), &kStatus);
    if(kStatus == MS::kSuccess)
    {
        float fG;
        kPlug.getValue(fG);
        material->transparency += fG;
    }

    kPlug = kNode.findPlug(MString("transparencyB"), &kStatus);
    if(kStatus == MS::kSuccess)
    {
        float fB;
        kPlug.getValue(fB);
        material->transparency += fB;
    }

    // Determine the average of the transparency components
    material->transparency /= 3.0f;

}

void ShaderMaterialMultiFillMap(MFnDependencyNode kNode, MString kName, 
    TextureMap* pkMap)
{
    MPlug kPlug;
    MStatus kStatus;

    kPlug = kNode.findPlug(kName + MString("L"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(pkMap->m_sMapL);

    kPlug = kNode.findPlug(kName + MString("K"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(pkMap->m_sMapK);

    kPlug = kNode.findPlug(kName + MString("Filter"), &kStatus);
    if(kStatus == MS::kSuccess)
    {
        kPlug.getValue(pkMap->m_cMapFilter);

        // Convert from Maya Filter Modes to Gamebryo Filter Modes
        switch(pkMap->m_cMapFilter)
        {
        case 2:     // Nearest Mip Nearest
            pkMap->m_cMapFilter = 3;
            break;
        case 3:     // Bilerp Mip Nearest
            pkMap->m_cMapFilter = 5;
            break;
        case 5:     // Trilep
            pkMap->m_cMapFilter = 2;
            break;
        }
    }
    kPlug = kNode.findPlug(kName + MString("MaxAnisotropy"), kStatus);
    if (kStatus == MS::kSuccess)
    {
        short sMaxAnisotropy;
        kPlug.getValue(sMaxAnisotropy);
        pkMap->m_usMaxAnisotropy = static_cast<unsigned short>(sMaxAnisotropy);
    }

    kPlug = kNode.findPlug(kName + MString("FlipBook"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(pkMap->m_bMapFlipBook);

    kPlug = kNode.findPlug(kName + MString("FlipBookStartFrame"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(pkMap->m_sMapFlipBookStartFrame);

    kPlug = kNode.findPlug(kName + MString("FlipBookEndFrame"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(pkMap->m_sMapFlipBookEndFrame);

    kPlug = kNode.findPlug(kName + MString("FlipBookFramesPerSecond"), 
        &kStatus);

    if(kStatus == MS::kSuccess)
        kPlug.getValue(pkMap->m_fMapFlipBookFramesPerSecond);

    kPlug = kNode.findPlug(kName + MString("FlipBookStartTime"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(pkMap->m_fMapFlipBookStartTime);

    kPlug = kNode.findPlug(kName + MString("FlipBookCycleType"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(pkMap->m_cMapFlipBookCycleType);
}


void ShaderMaterialMultiPixelShaderFillMap(MFnDependencyNode kNode, 
    MString kName, TextureMap* pkMap)
{
    MPlug kPlug;
    MStatus kStatus;

    // Strip the _Texture off of the name;
    unsigned int uiLen = kName.length() + 1;
    char* pcTemp = NiAlloc(char, uiLen);
    NiStrcpy(pcTemp, uiLen, kName.asChar());
    pcTemp[ strlen(pcTemp) - strlen("_Texture")] = '\0';

    // Create the AttributeName
    MString kAttribute(pcTemp);

    // delete the temp name
    NiFree(pcTemp);

    kPlug = kNode.findPlug(kAttribute + MString("_Filter"), &kStatus);
    if(kStatus == MS::kSuccess)
    {
        kPlug.getValue(pkMap->m_cMapFilter);

        // Convert from Maya Filter Modes to Gamebryo Filter Modes
        switch(pkMap->m_cMapFilter)
        {
        case 2:     // Nearest Mip Nearest
            pkMap->m_cMapFilter = 3;
            break;
        case 3:     // Bilerp Mip Nearest
            pkMap->m_cMapFilter = 5;
            break;
        case 5:     // Trilep
            pkMap->m_cMapFilter = 2;
            break;
        }
    }

    kPlug = kNode.findPlug(kAttribute + MString("_FlipBook"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(pkMap->m_bMapFlipBook);

    kPlug = kNode.findPlug(kAttribute + MString("_FlipBookStartFrame"), 
        &kStatus);

    if(kStatus == MS::kSuccess)
        kPlug.getValue(pkMap->m_sMapFlipBookStartFrame);

    kPlug = kNode.findPlug(kAttribute + MString("_FlipBookEndFrame"), 
        &kStatus);

    if(kStatus == MS::kSuccess)
        kPlug.getValue(pkMap->m_sMapFlipBookEndFrame);

    kPlug = kNode.findPlug(kAttribute + MString("_FlipBookFramesPerSecond"), 
        &kStatus);

    if(kStatus == MS::kSuccess)
        kPlug.getValue(pkMap->m_fMapFlipBookFramesPerSecond);

    kPlug = kNode.findPlug(kAttribute + MString("_FlipBookStartTime"), 
        &kStatus);

    if(kStatus == MS::kSuccess)
        kPlug.getValue(pkMap->m_fMapFlipBookStartTime);

    kPlug = kNode.findPlug(kAttribute + MString("_FlipBookCycleType"), 
        &kStatus);

    if(kStatus == MS::kSuccess)
        kPlug.getValue(pkMap->m_cMapFlipBookCycleType);

    kPlug = kNode.findPlug(kAttribute + MString("_Map"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(pkMap->m_cMap);
}

void ShaderMaterialMultiFillAllPixelShaderMap(MFnDependencyNode dgNode, 
    MtlStruct& material)
{
    char* pcAttHead = "Ni_PixelShaderAttribute_";
    char* pcAttTail = "_Texture";
    MStringArray Tex_mapped;


    // Find all of the attributes
    MStatus status;
    MString shaderName = dgNode.name(&status);
    NIASSERT(status == MS::kSuccess);


    unsigned int uiAttributeCount = dgNode.attributeCount(&status);
    NIASSERT(status == MS::kSuccess);

    for(unsigned int uiLoop = 0; uiLoop < uiAttributeCount; uiLoop++)
    {
        MFnAttribute kAttribute( dgNode.attribute(uiLoop, &status));

        NIASSERT(status == MS::kSuccess);

        MString kName = kAttribute.name(&status);
        NIASSERT(status == MS::kSuccess);

        // Check for the header
        const char* pcTemp = kName.asChar();
        NI_UNUSED_ARG(pcTemp);

        if(!strncmp(kName.asChar(), pcAttHead, strlen(pcAttHead)))
        {
            const char* pcTail = kName.asChar() + strlen(kName.asChar()) - 
                strlen(pcAttTail);

            // Check for the Tail
            if(!strcmp(pcTail, pcAttTail))
            {

                // Check the Pixel Shader attributes for connectinos
                TextureMap* pkMap = material.AddTextureMap(kName.asChar());
                NIASSERT(pkMap != NULL);
                ShaderMaterialMultiPixelShaderFillMap(dgNode, kName, pkMap);
            }
        }
    }
}



void shader_material_multi( MObject alShader, MtlStruct& material)
{
    MFnDependencyNode kNode;
    MStatus kStatus = kNode.setObject(alShader);

    NIASSERT(kStatus == MS::kSuccess);

    MPlug kPlug;

    TextureMap* pkMap = material.AddTextureMap("color");
    NIASSERT(pkMap != NULL);

    kPlug = kNode.findPlug(MString("Ni_TextureFilter"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(pkMap->m_cMapFilter);
    else
        pkMap->m_cMapFilter = -1;   // Initialize to a default value

    // only if map filter is anisotropic 
    if (pkMap->m_cMapFilter == 6)
    {    
        kPlug = kNode.findPlug(MString("Ni_MaxAnisotropy"), &kStatus);
        if (kStatus == MS::kSuccess)
        {
            short sMaxAnisotropy;
            kPlug.getValue(sMaxAnisotropy);
            pkMap->m_usMaxAnisotropy = static_cast<unsigned short>(sMaxAnisotropy);
        }
    }

    if(0 != strcmp( "NiMultiShader", objectType( alShader ) ) )
        return;

    ShaderMaterialMultiFillMap(kNode, MString("color"), pkMap);

    pkMap = material.AddTextureMap("DarkMap");
    NIASSERT(pkMap != NULL);
    ShaderMaterialMultiFillMap(kNode, MString("DarkMap"), pkMap);

    pkMap = material.AddTextureMap("DetailMap");
    NIASSERT(pkMap != NULL);
    ShaderMaterialMultiFillMap(kNode, MString("DetailMap"), pkMap);

    pkMap = material.AddTextureMap("Decal1Map");
    NIASSERT(pkMap != NULL);
    ShaderMaterialMultiFillMap(kNode, MString("Decal1Map"), pkMap);

    pkMap = material.AddTextureMap("Decal2Map");
    NIASSERT(pkMap != NULL);
    ShaderMaterialMultiFillMap(kNode, MString("Decal2Map"), pkMap);

    pkMap = material.AddTextureMap("GlowMap");
    NIASSERT(pkMap != NULL);
    ShaderMaterialMultiFillMap(kNode, MString("GlowMap"), pkMap);

    pkMap = material.AddTextureMap("GlossMap");
    NIASSERT(pkMap != NULL);
    ShaderMaterialMultiFillMap(kNode, MString("GlossMap"), pkMap);

    pkMap = material.AddTextureMap("BumpMap");
    NIASSERT(pkMap != NULL);
    ShaderMaterialMultiFillMap(kNode, MString("BumpMap"), pkMap);

    pkMap = material.AddTextureMap("NormalMap");
    NIASSERT(pkMap != NULL);
    ShaderMaterialMultiFillMap(kNode, MString("NormalMap"), pkMap);

    pkMap = material.AddTextureMap("ParallaxMap");
    NIASSERT(pkMap != NULL);
    ShaderMaterialMultiFillMap(kNode, MString("ParallaxMap"), pkMap);

    ShaderMaterialMultiFillAllPixelShaderMap(kNode, material);

    kPlug = kNode.findPlug(MString("BumpMapLuminaScale"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material.BumpMapLuminaScale);

    kPlug = kNode.findPlug(MString("BumpMapLuminaOffset"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material.BumpMapLuminaOffset);

    kPlug = kNode.findPlug(MString("BumpMapMat00"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material.BumpMapMat00);

    kPlug = kNode.findPlug(MString("BumpMapMat01"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material.BumpMapMat01);

    kPlug = kNode.findPlug(MString("BumpMapMat10"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material.BumpMapMat10);

    kPlug = kNode.findPlug(MString("BumpMapMat11"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material.BumpMapMat11);

    kPlug = kNode.findPlug(MString("ParallaxMapOffset"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material.ParallaxMapOffset);

    kPlug = kNode.findPlug(MString("Shading"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material.Shading);

    kPlug = kNode.findPlug(MString("VertexColorOverride"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material.VertexColorOverride);

    kPlug = kNode.findPlug(MString("VertexColorSrcMode"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material.VertexColorSrcMode);

    kPlug = kNode.findPlug(MString("VertexColorLightMode"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material.VertexColorLightMode);

    kPlug = kNode.findPlug(MString("TextureApplyMode"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material.TextureApplyMode);

    // Removed the PS2 Highlight options and use modulated
    if (material.TextureApplyMode > 2)
        material.TextureApplyMode = 2;

    kPlug = kNode.findPlug(MString("NBTMethod"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material.NBTMethod);

    kPlug = kNode.findPlug(MString("TransparencyOverride"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material.TransparencyOverride);

    kPlug = kNode.findPlug(MString("TransparencyUseAlphaBlending"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material.TransparencyUseAlphaBlending);

    kPlug = kNode.findPlug(MString("TransparencySource"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material.TransparencySource);

    kPlug = kNode.findPlug(MString("TransparencyDestination"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material.TransparencyDestination);

    kPlug = kNode.findPlug(MString("TransparencyUseAlphaTesting"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material.TransparencyUseAlphaTesting);

    kPlug = kNode.findPlug(MString("TransparencyNoTestingSorter"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material.TransparencyNoTestingSorter);

    kPlug = kNode.findPlug(MString("TransparencyTestReference"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material.TransparencyTestReference);

    kPlug = kNode.findPlug(MString("TransparencyTestingTestMode"), &kStatus);
    if(kStatus == MS::kSuccess)
        kPlug.getValue(material.TransparencyTestingTestMode);
}


Material *
shader_material_multi( MObject alShader, Material *material, int index )
{
    NI_UNUSED_ARG(index);
    if( alShader.isNull() )
    {
        return NULL;
    }

    float       specintense = 0.0;
    float       rolloff = 0.0;
    float       shiny=0.0;
    float       transparency[3];
    float       diffuse[3];
    float       emmission[3];
    float       color[3];
    float       specular[3];
    float       ambient[3];
    // float    diffuseAlpha = 0.0;
    // float    emmissionAlpha = 0.0;

    if( // alShader.hasFn( MFn::kPhong )  ||
        // alShader.hasFn( MFn::kLambert )  ||
        // alShader.hasFn( MFn::kBlinn)  )
        ( 0 == strcmp( "phong", objectType( alShader ) ) ) ||
        ( 0 == strcmp( "phongE", objectType( alShader ) ) ) ||
        ( 0 == strcmp( "blinn", objectType( alShader ) ) ) ||
        ( 0 == strcmp( "lambert", objectType( alShader ) ) ) ||
        ( 0 == strcmp( "layeredShader", objectType( alShader ) ) )  ||
        ( 0 == strcmp( "anisotropic", objectType( alShader ) ) ) )
    {
        if( DtExt_Debug() )
        {   
            cerr << objectName( alShader ) << " is indeed a shader of type "
                    << objectType( alShader ) << endl;
        }
    }
    else
    {
        if(0 != strcmp( "NiMultiShader", objectType( alShader ) ) )
        {
            cerr << "ERROR: " << objectName( alShader ) << 
                " is not a supported shader type: " << 
                objectType( alShader) << endl;
        }
        
        CreateMultiShaderMaterial(alShader, material);
        return material;

    }


    MStatus stat = MS::kSuccess;

    if( 0 == strcmp( "lambert", objectType( alShader )) ||
        0 == strcmp( "anisotropic", objectType( alShader )) )   
    {
        MFnLambertShader fnShader;

        stat = fnShader.setObject( alShader );
        MColor mcolor;

        mcolor = fnShader.color( &stat );
        if ( stat != MS::kSuccess )
        {
            DtExt_Err( "Error:: Failed to Get Color:\n");
        }
        mcolor.get( color );

        mcolor = fnShader.ambientColor( &stat );
        if ( stat != MS::kSuccess )
        {
            DtExt_Err( "Error:: Failed to Get Ambient Color:\n");
        }
        mcolor.get( ambient );

        
        // Diffuse in Maya becomes diffuse coefficient... confirm...
        material->diffuseCoeff = fnShader.diffuseCoeff( &stat );
        
        if ( stat != MS::kSuccess )
        {
            DtExt_Err( "Error:: Failed to Get DiffuseCoeff:\n");
        }

        // Calculate diffuse value
        for( int i = 0; i < 3; i++ )
        {
            diffuse[i] = material->diffuseCoeff * color[i];
        }

        // Incandescence
        MColor mincan;
        mincan = fnShader.incandescence( &stat );

        if ( stat != MS::kSuccess )
        {
            DtExt_Err( "Error:: Failed to Get Incandescence:\n");
        }

        mincan.get( emmission );
        
        MColor mtrans;
        mtrans = fnShader.transparency( &stat );

        if ( stat != MS::kSuccess )
        {
            DtExt_Err( "Error:: Failed to Get Transparency:\n");
        }

        mtrans.get( transparency );
        
        // emmissionAlpha = diffuseAlpha = 255.0 *
        // (1.0 - ((transparency[0] + transparency[1] + 
        //      transparency[2])/3.0) );

        material->color.r = color[0];
        material->color.g = color[1];
        material->color.b = color[2];
       
        material->diffuse.r = diffuse[0];
        material->diffuse.g = diffuse[1];
        material->diffuse.b = diffuse[2];

        material->emissive.r = emmission[0];
        material->emissive.g = emmission[1];
        material->emissive.b = emmission[2];

        material->shininess = 0;

        material->ambient.r = ambient[0];
        material->ambient.g = ambient[1];
        material->ambient.b = ambient[2];


        if ( 0 == strcmp( "lambert", objectType( alShader )) )
        {
            material->specular.r =  0;
            material->specular.g =  0;
            material->specular.b =  0;

        } else {

            MFnReflectShader fnAniso;
            stat = fnAniso.setObject( alShader );

            MColor mspecular;
            mspecular = fnAniso.specularColor( &stat );
            mspecular.get( specular );

            // reflectivity
            //
            specintense = fnAniso.reflectivity( &stat );

            material->specular.r = specular[0] * specintense;
            material->specular.g = specular[1] * specintense;
            material->specular.b = specular[2] * specintense;

            // use the "roughness" attribute for shininess

            MFnDependencyNode curSNode( alShader, &stat );
            MPlug vPlug = curSNode.findPlug( "roughness", &stat );
            double roughness;

            if ( stat == MS::kSuccess )
            {
                vPlug.getValue( roughness );
                material->shininess = (float)roughness;
            }
        }

        material->lightsource = false;
    }
    else if( (0 == strcmp( "phong",  objectType( alShader ) ) )  ||     
             (0 == strcmp( "phongE", objectType( alShader ) ) ) ) 

    {
        MFnReflectShader fnPhong;
        stat = fnPhong.setObject( alShader );

        MColor mcolor;
        mcolor = fnPhong.color( &stat );
        mcolor.get( color );

        mcolor = fnPhong.ambientColor( &stat );
        if ( stat != MS::kSuccess )
        {
            DtExt_Err( "Error:: Failed to Get Ambient Color:\n");
        }
        mcolor.get( ambient );

        
        // Diffuse in Maya becomes diffuse coefficient... confirm...
        //
        material->diffuseCoeff = fnPhong.diffuseCoeff( &stat );
        
        // Calculate diffuse value
        //
        for( int i = 0; i < 3; i++ )
        {
            diffuse[i] = material->diffuseCoeff * color[i];
        }

        MColor mincan;
        mincan = fnPhong.incandescence( &stat );
        mincan.get( emmission );
        
        MColor mtrans;
        mtrans = fnPhong.transparency( &stat );
        mtrans.get( transparency );
    
        // emmissionAlpha = diffuseAlpha = 255.0 *
        // (1.0 - ((transparency[0] + transparency[1] + 
        //      transparency[2])/3.0) );

        material->color.r = color[0];
        material->color.g = color[1];
        material->color.b = color[2];
    
        material->diffuse.r = diffuse[0];
        material->diffuse.g = diffuse[1];
        material->diffuse.b = diffuse[2];

        material->ambient.r = ambient[0];
        material->ambient.g = ambient[1];
        material->ambient.b = ambient[2];

        MColor mspecular;
        mspecular = fnPhong.specularColor( &stat );
        mspecular.get( specular );

        // reflectivity
        //
        specintense = fnPhong.reflectivity( &stat );

        material->emissive.r = emmission[0];
        material->emissive.g = emmission[1];
        material->emissive.b = emmission[2];

        material->specular.r = specular[0] * specintense;
        material->specular.g = specular[1] * specintense;
        material->specular.b = specular[2] * specintense;

        // Fill in the shininess depending on type

        if( (0 == strcmp( "phongE",  objectType( alShader ) ) )  )
        {
            // use the "roughness" attribute for shininess
           
            MFnDependencyNode curSNode( alShader, &stat );
            MPlug vPlug = curSNode.findPlug( "roughness", &stat );
            double roughness;
            
            if ( stat == MS::kSuccess )
            {
                vPlug.getValue( roughness );
                material->shininess = (float)roughness;
            }   
        }
        else
        {
            MFnPhongShader fnPhong;
            stat = fnPhong.setObject( alShader );

            // Shinyness becomes cosinePower in Maya:
            //
            //shiny = sqrt(sqrt((float)(factor)/210.0)) * 300.0 - 90.0;
            material->shininess = fnPhong.cosPower( &stat );;

        }

        material->lightsource = false;
    }
    else if( 0 == strcmp( "blinn", objectType( alShader )  ) ) 
    {   
        MFnBlinnShader fnBlinn;
        stat = fnBlinn.setObject( alShader );

        MColor mcolor;
        mcolor = fnBlinn.color( &stat );
        mcolor.get( color );
        
        // Diffuse in Maya becomes diffuse coefficient... confirm...
        //
        material->diffuseCoeff = fnBlinn.diffuseCoeff( &stat );
        
        // Calculate diffuse value
        //
        for( int i = 0; i < 3; i++ )
        {
            diffuse[i] = material->diffuseCoeff * color[i];
        }

        MColor mincan;
        mincan = fnBlinn.incandescence( &stat );
        mincan.get( emmission );
        
        MColor mtrans;
        mtrans = fnBlinn.transparency( &stat );
        mtrans.get( transparency );
        
        // emmissionAlpha = diffuseAlpha = 255.0 *
        // (1.0 - ((transparency[0] + transparency[1] + 
        //      transparency[2])/3.0) );

        material->color.r = color[0];
        material->color.g = color[1];
        material->color.b = color[2];

        material->diffuse.r = diffuse[0];
        material->diffuse.g = diffuse[1];
        material->diffuse.b = diffuse[2];

        MColor mambient;
        mambient = fnBlinn.ambientColor( &stat );
        mambient.get( ambient );

        material->ambient.r = ambient[0];
        material->ambient.g = ambient[1];
        material->ambient.b = ambient[2];

        MColor mspecular;
        mspecular = fnBlinn.specularColor( &stat );
        mspecular.get( specular );
        
        // reflectivity
        //
        specintense = fnBlinn.reflectivity( &stat );
    
        rolloff = fnBlinn.specularRollOff( &stat );
        
        specintense *= (float)(1.0 - (rolloff * 0.8) );

        //factor = fnBlinn.eccentricity( &stat );
        //shiny = 1.15/(factor + 0.1);
        //shiny = (shiny * shiny * shiny) - 0.5;
        
        shiny = fnBlinn.eccentricity( &stat );;
        
        material->emissive.r = emmission[0];
        material->emissive.g = emmission[1];
        material->emissive.b = emmission[2];

        material->specular.r = specular[0] * specintense;
        material->specular.g = specular[1] * specintense;
        material->specular.b = specular[2] * specintense;

        material->shininess = shiny;

        material->lightsource = false;

    } 
    else if( 0 == strcmp( "layeredShader", objectType( alShader )  ) )
    {   
        material->color.r = 0.5;
        material->color.g = 0.5;
        material->color.b = 0.5;
        
        material->diffuse.r = 0.5;
        material->diffuse.g = 0.5;
        material->diffuse.b = 0.5;

        material->diffuseCoeff = 1.0;

        transparency[0] = 0.0;
        transparency[1] = 0.0;
        transparency[2] = 0.0;

  
    }
    else 
    {
        DtExt_Err("shading model not recognized: \"%s\"\n", 
                 objectType( alShader ) );
    }


    // Fill in the missing info for phong shader

    if( (0 == strcmp( "phong",  objectType( alShader ) ) )  )
    {
        MFnPhongShader fnPhong;
        stat = fnPhong.setObject( alShader );

        // Shinyness becomes cosinePower in Maya:
        //
        material->shininess = fnPhong.cosPower( &stat );;
    }
    
    material->transparency = (transparency[0] +
                              transparency[1] +
                              transparency[2]) / 3.0f;
    return material;
}


//  Routine to walk thru the possible texture mapped fields and generate
//  texture images for those that are mapped

int
shader_multiTextures(MObject alShader, MObject object, MDagPath &dagPath )
{
    MString   shaderName;
    MString   textureName;
    MString   textureFile = "";
    MString   projectionName = "";
    MString   convertName;
    MString   command;
    MStringArray result;
    MStringArray arrayResult;

    MStatus   status;

    MStringArray    Tex_mapped;

    int texCnt = 0;


    MFnDependencyNode dgNode;
    status = dgNode.setObject( alShader );
    shaderName = dgNode.name( &status );

    result.clear();

    // Lets return without truing if we get a null shader name

    if ( shaderName.length() == 0 )
    {
        return 0;
    }


    // If this is a layered shader, we are going to skip this for now.
    // will need to run thru the layered shaders each by themselves ?
    // Leave for later.

    if ( 0 == strcmp( "layeredShader", objectType( alShader ) ) )
    {
        return 0;
    }


    // We are not going to do the color and transparency thing here

    // We will be doing the other textures here

    command = MString("listConnections ") + shaderName
                                          + MString(".ambientColor;");
    if ( MS::kSuccess == MGlobal::executeCommand( command, Tex_mapped ) )
    {
        MString texType( "ambient" );
        MString texNode( ".ambientColor" );
        texCnt += generate_textureImages( shaderName, texNode, texType, 
                                                    Tex_mapped, dagPath);
    }

    command = MString("listConnections ") + shaderName
                                          + MString(".diffuse;");
    if ( MS::kSuccess == MGlobal::executeCommand( command, Tex_mapped ) )
    {
        MString texType( "diffuse" );
        MString texNode( ".diffuse" );
        texCnt += generate_textureImages( shaderName, texNode, texType, 
                                                    Tex_mapped, dagPath);
    }

    command = MString("listConnections ") + shaderName
                                          + MString(".translucence;");
    if ( MS::kSuccess == MGlobal::executeCommand( command, Tex_mapped ) )
    {
        MString texType( "translucence" );
        MString texNode( ".translucence");
        texCnt += generate_textureImages( shaderName, texNode, texType, 
            Tex_mapped, dagPath);
    }

    command = MString("listConnections ") + shaderName
                                          + MString(".incandescence;");
    if ( MS::kSuccess == MGlobal::executeCommand( command, Tex_mapped ) )
    {
        MString texType( "incandescence" );
        MString texNode( ".incandescence");
        texCnt += generate_textureImages( shaderName, texNode, texType, 
                                                    Tex_mapped, dagPath);
    }


    command = MString("listConnections ") + shaderName
                                          + MString(".normalCamera;");
    if ( MS::kSuccess == MGlobal::executeCommand( command, Tex_mapped ) )
    {
        MString texType( "bump" );
        MString texNode( ".normalCamera");
        texCnt += generate_textureImages( shaderName, texNode, texType, 
                                                    Tex_mapped, dagPath);
    } 

    command = MString("listConnections ") + shaderName
                                          + MString(".cosinePower;");
    if ( MS::kSuccess == MGlobal::executeCommand( command, Tex_mapped ) )
    {
        MString texType( "shininess" );        
        MString texNode( ".cosinePower");
        texCnt += generate_textureImages( shaderName, texNode, texType, 
                                                    Tex_mapped, dagPath);
    }   

    command = MString("listConnections ") + shaderName
                                          + MString(".specularColor;");
    if ( MS::kSuccess == MGlobal::executeCommand( command, Tex_mapped ) )
    {
        MString texType( "specular" );
        MString texNode( ".specularColor");
        texCnt += generate_textureImages( shaderName, texNode, texType, 
                                                    Tex_mapped, dagPath);
    }   

    command = MString("listConnections ") + shaderName
                                          + MString(".reflectivity;");
    if ( MS::kSuccess == MGlobal::executeCommand( command, Tex_mapped ) )
    {
        MString texType( "reflectivity" );
        MString texNode( ".reflectivity");
        texCnt += generate_textureImages( shaderName, texNode, texType, 
                                                    Tex_mapped, dagPath);
    }

    command = MString("listConnections ") + shaderName
                                          + MString(".reflectedColor;");
    if ( MS::kSuccess == MGlobal::executeCommand( command, Tex_mapped ) )
    {
        MString texType( "reflected" );
        MString texNode( ".reflectedColor");
        texCnt += generate_textureImages( shaderName, texNode, texType, 
                                                    Tex_mapped, dagPath);
    }   


    return texCnt; 

    // Other textures that could be added include:
    //
    // Common:
    //
    // diffuse
    // translucence
    //
    // phong: 
    //
    // reflectivity
    //
    // phongE:
    //
    // roughness
    // highlightSize
    // whiteness
    // 
    // blinn:
    //
    // eccentricity
    // specularRollOff
    //

}

int
getTextureFileSize( bool fileTexture, MString &textureName, 
                                      MString &textureFile, 
                                      int &xres, int &yres, int &zres )
{
    NI_UNUSED_ARG(textureFile);
    // If we are doing file texture lets see how large the file is
    // else lets use our default sizes.

    xres = DtExt_xTextureRes();
    yres = DtExt_yTextureRes();
    zres = 4;

    if ( fileTexture )
    {
#if 1
        // Lets try getting the size of the textures this way.

        MStatus     status;
        MString     command;
        MIntArray   sizes;

        command = MString( "getAttr " ) + textureName + 
            MString( ".outSize;" );

        status = MGlobal::executeCommand( command, sizes );
        if ( MS::kSuccess == status )
        {
            xres = sizes[0];
            yres = sizes[1];
        }

#else
    
        // Old method 
        IFFimageReader reader;
        MStatus Rstat;

        Rstat = reader.open( textureFile );
        if ( Rstat != MS::kSuccess )
        {
            cerr << "Error reading file " << textureFile.asChar() << " "
                 << reader.errorString() << endl;
            return 0;
        }

        int bpp = reader.getBytesPerChannel();
        int w,h;

        Rstat = reader.getSize (w, h);
        if ( Rstat != MS::kSuccess )
        {
            reader.close();
            return 0;
        }

        xres = w;
        yres = h;
        zres = 1;

        if (reader.isRGB () || reader.isGrayscale ())
        {
            if (reader.isRGB ())
            {
                zres = 3;
                if (reader.hasAlpha ())
                    zres = 4;
            }
        }

        reader.close();
#endif

    }

    return 1;
}



int
generate_textureImages( MString &shaderName, 
                        MString &texNode,
                        MString &texType, 
                        MStringArray& texMap,
                        MDagPath &dagPath )
{
    NI_UNUSED_ARG(texNode);
    MString         command;
    MString         textureName;
    MString         convertName;
    MString         textureFile;

    MString         bumpTexture;

    MStatus         status;

    MStringArray    result;
    MStringArray    arrayResult;

    
    MtlStruct       *mtl;
    
    int             xres, yres, zres;

    bool            fileTexture;

    // Now check to see if something was there

    if ( (texMap.length() && texMap[0].length()) ) 
    {
        command = MString("ls -st ") + texMap[0];
        status = MGlobal::executeCommand( command, arrayResult );

        if ( status != MS::kSuccess ) {
            return 0;
        }

        // Now see if it is a file texture in order to get the size of
        // the image to use

        fileTexture = false;

        if ( (arrayResult.length() ==2) && arrayResult[1].length() &&
            (arrayResult[1] == MString("file") )  ) {

            textureName = arrayResult[0];
            command = MString("getAttr ") + textureName +
                                        MString(".fileTextureName");

            status = MGlobal::executeCommand( command, result );

            if ( status != MS::kSuccess ) {
                return 0;
            }

            textureFile = result[0];
            fileTexture = true;

            if( DtExt_Debug() )
                cerr << "file: texture is: " << textureFile.asChar() << endl;

        } else if ( (arrayResult.length() ==2) && arrayResult[1].length() &&
                    (arrayResult[1] == MString("bump2d") )  ) {

            command = MString("listConnections ") + texMap[0]
                                       + MString(".bumpValue;");
            status = MGlobal::executeCommand( command, result );

            if ( (result.length() && result[0].length()) )
            {   

                bumpTexture = result[0];

                command = MString("ls -st ") + result[0];
                status = MGlobal::executeCommand( command, arrayResult );
        
                if ( status != MS::kSuccess ) {
                    return 0;
                }
        
                // Now see if it is a file texture in order to get the size 
                // of the image to use
        
                fileTexture = false;
        
                if ( (arrayResult.length() ==2) && arrayResult[1].length() &&
                    (arrayResult[1] == MString("file") )  ) {
            
                    textureName = arrayResult[0]; 
                    command = MString("getAttr ") + textureName +
                                            MString(".fileTextureName");
            
                    status = MGlobal::executeCommand( command, result );
            
                    if ( status != MS::kSuccess ) {
                        return 0;
                    }

                    textureFile = result[0];
                    fileTexture = true;
            
                    if( DtExt_Debug() )
                        cerr << "file: texture is: " << 
                            textureFile.asChar() << endl;
                }
            }
        }
                

        // There is a texture, I don't care what kind it is, going to use
        // convertSolidTx to generate a file texture

        textureName = texMap[0];

        // Do nothing if texture is already in the list.
        //
        if( MtFindTex(local, (char *) textureName.asChar() ) != NULL)
        {
            // add texture name to material
            //
            mtl = MtFindMtl(local, (char *) shaderName.asChar() );

            if ( texType == "ambient" )
            {
                if ( mtl->ambient_name )
                    NiFree( mtl->ambient_name );
                mtl->ambient_name = NiStrdup( textureName.asChar() );
            }
            else if ( texType == "diffuse" )
            {
                if ( mtl->diffuse_name )
                    NiFree( mtl->diffuse_name );
                mtl->diffuse_name = NiStrdup( textureName.asChar() );
            }

            else if ( texType == "translucence" )
            {
                if ( mtl->translucence_name )
                    NiFree( mtl->translucence_name );
                mtl->translucence_name = NiStrdup( textureName.asChar() );
            }

            else if ( texType == "incandescence" ) 
            {
                if ( mtl->incandescence_name )
                    NiFree( mtl->incandescence_name );
                mtl->incandescence_name = NiStrdup( textureName.asChar() );
            } 
            else if ( texType == "bump" )
            {
                if ( mtl->bump_name )
                    NiFree( mtl->bump_name );
                mtl->bump_name = NiStrdup( textureName.asChar() );
            } 
            else if ( texType == "shininess" )
            {
                if ( mtl->shininess_name )
                    NiFree( mtl->shininess_name );
                mtl->shininess_name = NiStrdup( textureName.asChar() );
            }
            else if ( texType == "specular" )
            {
                if ( mtl->specular_name )
                    NiFree( mtl->specular_name );
                mtl->specular_name = NiStrdup( textureName.asChar() );
            } 

            else if ( texType == "reflectivity" )
            {
                if ( mtl->reflectivity_name )
                    NiFree( mtl->reflectivity_name );
                mtl->reflectivity_name = NiStrdup( textureName.asChar() );
            }

            else if ( texType == "reflected" )
            {
                if ( mtl->reflected_name )
                    NiFree( mtl->reflected_name );
                mtl->reflected_name = NiStrdup( textureName.asChar() );
            }

            return( 1 );
        }
        
        // Will use the convertSolidTx command to generate the image to use.
        //
    
        unsigned char *image = NULL;
           
        char    cmdStr[2048];

        saveTransforms( textureName );

        // need to have different temp location for SGI and NT

        char tmpFile[2048];
        char outTmpPath[_MAX_PATH];
        GetTempPath( _MAX_PATH, outTmpPath );
        GetTempFileName( outTmpPath, "mat", 
#if defined(_MSC_VER) && _MSC_VER >= 1400
            _getpid(), 
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
            getpid(), 
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
            tmpFile );
            for ( unsigned int ii=0; ii < strlen( tmpFile ); ii++ )
                if ( tmpFile[ii] == '\\' )
                    tmpFile[ii] = '/';

        // 
        // Check to see if a projection was found and if so then use it
        // as the base object to do the convert solid texture command on
        //

        // Need to check for the bump mapping.
        // as there is a utitity node in front of the actual texture
        // that is to be converted.


        MString forSizeName;

        if ( texType == "bump" )
        {
            // convertName = textureName + MString(".bumpValue");
            convertName = bumpTexture + MString( ".outColor" );
            forSizeName = bumpTexture;
        } 
        else 
        {
            convertName = textureName + MString( ".outColor" );
            forSizeName = textureName;
        }


        // Use the supplied x/y size to generate the texture files

        getTextureFileSize( fileTexture, forSizeName, textureFile, 
                                                    xres, yres, zres );

        if ( DtExt_OriginalTexture() )
        {
            // We will use a temp file
            restoreTransforms( textureName );

            // allocate a little memory to make other things happy

            image = (unsigned char *)NiStrdup("originalFile");

        } else {
            NiSprintf( cmdStr, 2048,
                "convertSolidTx -rx %d -ry %d -n \"%s\" -fin \"%s\" -uvr "
                "0.0 1.0 0.0 1.0 %s %s",
                        xres, yres, 
                        "mdt_tmp_texture_name",
                        tmpFile,
                        convertName.asChar(), 
                        dagPath.fullPathName().asChar() );
            if( DtExt_Debug() )
                cerr << "command to execute: " << cmdStr << endl;

            status = MGlobal::executeCommand( cmdStr );

            restoreTransforms( textureName );

            if ( status != MS::kSuccess )
            {
                cerr << "error from convertSolidTx " << 
                    status.errorString().asChar() << endl;
                return 0;
            }   

            // Read in the image file generated 

            image = readTextureFile( MString( tmpFile ), MString( " " ),
                                    0,  xres, yres, zres );

            // Need to delete the node that was generated by the 
            // convertSolidTx command.

            MGlobal::executeCommand( "delete mdt_tmp_texture_name" );
            
            // Now lets remove the temp file that we generated

#if defined(_MSC_VER) && _MSC_VER >= 1400
            _unlink ( tmpFile );
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
            unlink ( tmpFile );
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400

        }

        // If we have an image in memory, then we add it to our local table
        // references.


        // Need to find file name for texture if possible.

        if ( image )
        {   
            if( !MtAddTex(local, (char *)textureName.asChar(), 
                    (char *)textureFile.asChar(), fileTexture, 
                    image, xres, yres, 4, MObject::kNullObj,
                    texType == "bump") )
            {
                DtExt_Msg("\"%s\"already in list delete this copy ofimage\n",
                        (char *)textureName.asChar() );
                NiFree( image );
                image = NULL;
            }

            // add texture name to material
            //
            mtl = MtFindMtl(local, (char* )shaderName.asChar() );
        
            // color and opacity are not done here as in the 
            // normal texture generation
    
            if ( texType == "ambient" )
            {
                if ( mtl->ambient_name )
                    NiFree( mtl->ambient_name );
                mtl->ambient_name = NiStrdup( textureName.asChar() );
            }
            else if ( texType == "diffuse" )
            {   
                if ( mtl->diffuse_name )
                    NiFree( mtl->diffuse_name );
                mtl->diffuse_name = NiStrdup( textureName.asChar() );
            } 
            else if ( texType == "translucence" )
            {   
                if ( mtl->translucence_name )
                    NiFree( mtl->translucence_name );
                mtl->translucence_name = NiStrdup( textureName.asChar() );
            } 

            else if ( texType == "incandescence" ) 
            {
                if ( mtl->incandescence_name )
                    NiFree( mtl->incandescence_name );
                mtl->incandescence_name = NiStrdup( textureName.asChar() );
            } 
            else if ( texType == "bump" )
            {
                if ( mtl->bump_name )
                    NiFree( mtl->bump_name );
                mtl->bump_name = NiStrdup( textureName.asChar() );
            } 
            else if ( texType == "shininess" )
            {
                if ( mtl->shininess_name )
                    NiFree( mtl->shininess_name );
                mtl->shininess_name = NiStrdup( textureName.asChar() );
            }   
            else if ( texType == "specular" )
            {
                if ( mtl->specular_name )
                    NiFree( mtl->specular_name );
                mtl->specular_name = NiStrdup( textureName.asChar() );
            } 
            else if ( texType == "reflectivity" )
            {   
                if ( mtl->reflectivity_name )
                    NiFree( mtl->reflectivity_name );
                mtl->reflectivity_name = NiStrdup( textureName.asChar() );
            } 
            else if ( texType == "reflected" )
            {
                if ( mtl->reflected_name )
                    NiFree( mtl->reflected_name );
                mtl->reflected_name = NiStrdup( textureName.asChar() );
            }

            
            return 1;
        }
    }

    return 0;
}


bool IsSelfLoadTexture(const char *szFileName)
{

    // FILE NAME DOES NOT EXIST
    if(szFileName == NULL)
        return false;

    // CHECK FOR EMPTY STRING
    if(strcmp(szFileName, "")==0)
        return false;

    // FIND THE BEGINNING OF THE EXTENSION
    const char *pCurrent = strrchr(szFileName, '.');

    if(pCurrent == NULL)
        return false;

    // IS IT A TARGA FILE?
    if(NiStricmp(pCurrent, ".TGA") == 0 || NiStricmp(pCurrent, ".TARGA") == 0)
        return true;

    // IS IT A BMP FILE?
    if(NiStricmp( pCurrent, ".BMP" )==0)
        return true;

    // IS IT A RGB FILE?
    if((NiStricmp(pCurrent, ".SGI") == 0) || 
       (NiStricmp(pCurrent, ".RGB") == 0) || 
       (NiStricmp(pCurrent, ".RGBA") == 0) || 
       (NiStricmp(pCurrent, ".INT") == 0) || 
       (NiStricmp(pCurrent, ".INTA") == 0))
        return true;

    // IS IT A DDS FILE?
    if(NiStricmp( pCurrent, ".DDS" )==0)
        return true;

    // IS IT A Open Exr FILE?
    if(NiStricmp( pCurrent, ".EXR" )==0)
        return true;

    // IS IT An HDR FILE?
    if(NiStricmp( pCurrent, ".HDR" )==0)
        return true;


    // NON SUPPORTED FILE FORMAT
    return false;
}



int
shader_texture2( MObject alShader, MObject object, MDagPath &dagPath )
{
    NI_UNUSED_ARG(dagPath);

    MtlStruct     *mtl;

    MString   shaderName;
    MString   textureName;
    MString   textureFile = "";
    MString   projectionName = "";
    MString   convertName;
    MString   command;
    MStringArray result;
    MStringArray resultT;
    MStringArray arrayResult;

    MStatus   status;
    MStatus   statusT;
    
    bool        projectionFound = false;
    bool        layeredFound = false;

    int xres, yres, zres;
    bool fileTexture = false;
    bool bSelfLoadingTexture = false;

    // The following variables are not used yet.

    //int colorFound = false;
    int transFound = false;
    //unsigned char *image = NULL;
    //ShaderColor color={0.,0.,0.,0.};
    //ShaderColor transparency={0.,0.,0.,0.};


    MFnDependencyNode dgNode;
    status = dgNode.setObject( alShader );
    shaderName = dgNode.name( &status );

    if ( DtExt_Debug() )
    {
        cerr << "checking shader: " << shaderName.asChar() << endl;
        cerr << "2nd Check: name = " << objectName( alShader ) 
                << "type = " << objectType(alShader ) << endl;
    }

    result.clear();
    resultT.clear();

    // Lets return without truing if we get a null shader name

    if ( shaderName.length() == 0 )
    {
        return 0;
    }
    

    // Find what the color is connected to 
    MObject kTextureNode = FollowInputAttribute(alShader, MString("color"));


    if ( 0 == strcmp( "layeredShader", objectType( alShader ) ) )
    {
        layeredFound = true;
    }

    if ( !layeredFound )
    {
        command = MString("listConnections ") + shaderName + 
            MString(".color;");

        status = MGlobal::executeCommand( command, result ); 

        command = MString("listConnections ") + shaderName 
                                                + MString(".transparency;");

        statusT = MGlobal::executeCommand( command, resultT );
    
        if ( status != MS::kSuccess ) {
            return 0;
        }
    }


    if ( layeredFound || (result.length() && result[0].length()) ) {

        if ( layeredFound )
        {
            textureName = shaderName;
            fileTexture = false;
            arrayResult.clear();
            arrayResult[0] = shaderName;
        }   
        else
        {
            command = MString("ls -st ") + result[0];

            status = MGlobal::executeCommand( command, arrayResult );

            if ( status != MS::kSuccess ) {
                return 0;
            }
        
            if( DtExt_Debug() )
            {
                cerr << "ls -st result length " << arrayResult.length() << 
                    endl;

                cerr << "ls -st results " << arrayResult[0].asChar() << 
                    arrayResult[1].asChar() << endl;
            }
        }

        if ( (arrayResult.length() ==2) && arrayResult[1].length() &&
            (arrayResult[1] == MString("file") )  ) {

            textureName = arrayResult[0];
            command = MString("getAttr ") + textureName +
                                        MString(".fileTextureName");

            status = MGlobal::executeCommand( command, result );

            if ( status != MS::kSuccess ) {
                return 0;
            }

            textureFile = result[0];
            fileTexture = true;

            // Check to see if this will be a self loading texture for the 
            // Exporter
            bSelfLoadingTexture = IsSelfLoadTexture(textureFile.asChar());
            
            if( DtExt_Debug() )
                cerr << "file: texture is: " << textureFile.asChar() << endl;

        } else if ( (arrayResult.length() ==2) && arrayResult[1].length() &&
                    (arrayResult[1] == MString("projection") )  ) {

            projectionName = arrayResult[0];
            
            projectionFound = true;
            
            if( DtExt_Debug() )
                cerr << "found projection " << projectionName.asChar() << 
                    endl;

            command = MString("listConnections ") + projectionName + 
                                                        MString(".image;");
    
            status = MGlobal::executeCommand( command, result );
            
            if ( status != MS::kSuccess ) {
        
                return 0;
            }

            if ( result.length() && result[0].length() ) {
    
                command = MString("ls -st ") + result[0];
        
        
                status = MGlobal::executeCommand( command, arrayResult );
        
        
                if ( status != MS::kSuccess ) {
                    return 0;
                }   
       
                if( DtExt_Debug() )
                    cerr << "proj types: " << arrayResult[0].asChar() << 
                    " " << arrayResult[1].asChar() << endl;
                
                if ( (arrayResult.length() ==2) && arrayResult[1].length() &&
                    (arrayResult[1] == MString("file") )  ) {
            
            
                    textureName = arrayResult[0];
                    fileTexture = true;
                    
                    command = MString("getAttr ") + textureName + 
                                            MString(".fileTextureName");
            
                    status = MGlobal::executeCommand( command, result );
            
                    if ( status != MS::kSuccess ) {
                        return 0;
                    }   
            
                    textureFile = result[0];

                    // Check to see if this will be a self loading texture 
                    // for the Exporter
                    bSelfLoadingTexture = 
                        IsSelfLoadTexture(textureFile.asChar());

                    if( DtExt_Debug() )
                        cerr << "use texture file: " << 
                            textureFile.asChar() << endl;

                }
            }

        } else {

            // There is a texture, but it is not a file, or projection, but 
            // rather a procedural

            textureName = arrayResult[0];

            if( DtExt_Debug() )
                cerr << "is a texture, but not file, name is: " 
                        << textureName.asChar() << endl;
        }

        // Do nothing if texture is already in the list.
        //
        if( MtFindTex(local, (char *) textureName.asChar() ) != NULL)
        {
            // add texture name to material
            //
            mtl = MtFindMtl(local, (char *) shaderName.asChar() );

            if ( mtl->texture_name )
                 NiFree( mtl->texture_name );

            mtl->texture_name = NiStrdup( textureName.asChar() );

            TextureMap* pkMap = mtl->FindTextureMap("color");
            NIASSERT(pkMap != NULL);

            // Create the Texture Name
            NiFree(pkMap->m_pcMapTextureName);
            pkMap->m_pcMapTextureName = NiStrdup(mtl->texture_name);

            if ( mtl->texture_filename )
                NiFree( mtl->texture_filename );

            mtl->texture_filename = NULL;

            if ( textureFile.asChar() )
            {
                mtl->texture_filename = NiStrdup( textureFile.asChar() );
            }

            return( 1 );
        }
        

        // If we are doing file texture lets see how large the file is
        // else lets use our default sizes.  If we can't open the file
        // we will use the default size.

        getTextureFileSize( fileTexture, textureName, textureFile, 
                                                    xres, yres, zres );


        // This portion to be completed for transparency
        //
        // Will use the convertSolidTx command to generate the image to use.
        //
        // Also to check for transparency if the convertSolidTx command 
        // doesn't do it for me.


        // If we are just reading in file textures do it that way.
    
        unsigned char *image = NULL;

        if ( !DtExt_inlineTextures() && fileTexture )
        {
            image = readTextureFile( MString(textureFile), MString( " " ), 
                                                    0, xres, yres, zres );

            // Here we are not just reading in file textures, but will
            // call the convertSolidTx command to do the evaluation for us
            
        } else {
           
            saveTransforms( textureName );

            // need to have different temp location for SGI and NT

            char tmpFile[2048] = "";

            char outTmpPath[_MAX_PATH];
            GetTempPath( _MAX_PATH, outTmpPath );
            GetTempFileName( outTmpPath, "mat", 
#if defined(_MSC_VER) && _MSC_VER >= 1400
                _getpid(), 
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
                getpid(), 
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
                tmpFile );
            for ( unsigned int ii=0; ii < strlen( tmpFile ); ii++ )
                if ( tmpFile[ii] == '\\' )
                    tmpFile[ii] = '/';

            // 
            // Check to see if a projection was found and if so then use it
            // as the base object to do the convert solid texture command on
            //

            if ( projectionFound )
            {
                convertName = projectionName;
            } else {
                convertName = textureName;
            }


            if ( DtExt_OriginalTexture() || bSelfLoadingTexture )
            {
                restoreTransforms( textureName );

                image = (unsigned char *)NiStrdup( "originalFile" );

            } 
            else 
            {
                image = CreateTextureFromNode( kTextureNode, xres, yres);

                restoreTransforms( textureName );
            }
        }

        // If we have an image in memory, then we add it to our local table
        // references. OR if it is bSelfLoadingTexture

        if ( image || bSelfLoadingTexture)
        {   
            if( !MtAddTex(local, (char *)textureName.asChar(), 
                    (char *)textureFile.asChar(), fileTexture, 
                        image, xres, yres, 4, kTextureNode, false) )
            {
                DtExt_Msg("\"%s\" already inlist delete this copy ofimage\n",
                        (char *)textureName.asChar() );
                NiFree( image );
                image = NULL;
            }

            // add texture name to material
            //
            mtl = MtFindMtl(local, (char* )shaderName.asChar() );
            mtl->texture_name = NiStrdup( textureName.asChar() );
            mtl->texture_filename = NiStrdup( textureFile.asChar() );
            mtl->hasAlpha = transFound;

            TextureMap* pkMap = mtl->FindTextureMap("color");
            NIASSERT(pkMap != NULL);

            // Create the Texture Name
            NiFree(pkMap->m_pcMapTextureName);
            pkMap->m_pcMapTextureName = NiStrdup(mtl->texture_name);

            return 1;

        } else {

            return 0;

        }


    }

    return 0;
}


// Create a 32 bit texture buffer from a node
unsigned char *CreateTextureFromNode(MObject TextureNode, int iUSamples, 
    int iVSamples)
{
    MStatus stat = MS::kSuccess;

    // Create a node so we can see about the attributes
    MFnDependencyNode kNode(TextureNode, &stat);

    if (stat != MS::kSuccess)
        return NULL;

    // Get the Name
    MString MTextureName = kNode.name(&stat);

    if (stat != MS::kSuccess)
        return NULL;


    // Check for outColor
    if ((kNode.attribute(MString("outColor"), &stat) == MObject::kNullObj) ||
        (stat != MS::kSuccess))
    {
        return NULL;
    }

    //I think this is a bad way of checking this.
    //// Check for Transparency
    bool bHasTransparency = true;
    //if ((kNode.attribute(MString("outTransparency"), 
    //    &stat) == MObject::kNullObj) || (stat != MS::kSuccess))
    //{
    //    bHasTransparency = false;
    //}


    MPlug kHasAlphaPlug = kNode.findPlug("fileHasAlpha", &stat);
    bool hasAlpha = false;
    kHasAlphaPlug.getValue(hasAlpha);
    if(hasAlpha)
    {
        bHasTransparency = true;
    }
    else
    {
        bHasTransparency = false;
    }

    MString kShadingNodeColorName = MTextureName + MString(".outColor");
    MString kShadingNodeTransparencyName = 
        MTextureName + MString(".outTransparency");

    unsigned char* pucImage32 = NiAlloc(unsigned char, iUSamples * iVSamples * 4);
    unsigned char* pucCurrentPixel = pucImage32;
    unsigned int uiCurrentPixelIndex = 0;

    float fUOffset = 1.0f / (float)iUSamples;
    float fVOffset = 1.0f / (float)iVSamples;

    float fUHalf = fUOffset / 2.0f;
    float fVHalf = fUOffset / 2.0f;

    stat = MS::kSuccess;

    bool bUseShadowMaps = false;
    bool bResueMaps = true;
    MFloatMatrix kCameraMatrix;
    MFloatPointArray akPoints;
    MFloatArray afUCoords;
    MFloatArray afVCoords;
    MFloatVectorArray avNormals;
    MFloatPointArray akRefPoints;
    MFloatVectorArray akTangentUs;
    MFloatVectorArray akTangentVs;
    MFloatArray afFilterSizes;
    MFloatVectorArray akResultColors;
    MFloatVectorArray akResultColorsT;
    MFloatVectorArray akResultTransparencies;
    MFloatVectorArray akResultTransparenciesT;
    kCameraMatrix.setToIdentity();

    while(uiCurrentPixelIndex < (unsigned int)(iUSamples * iVSamples))
    {
        
//        int iNumSamples = iUSamples * iVSamples;
        int iNumSamples = 
            min(65535, (iUSamples * iVSamples) - uiCurrentPixelIndex);

        akPoints.setLength(iNumSamples);
        afUCoords.setLength(iNumSamples);
        afVCoords.setLength(iNumSamples);
        avNormals.setLength(iNumSamples);
        akRefPoints.setLength(iNumSamples);
        akTangentUs.setLength(iNumSamples);
        akTangentVs.setLength(iNumSamples);
        afFilterSizes.setLength(iNumSamples);
        akResultColors.setLength(iNumSamples);
        akResultTransparencies.setLength(iNumSamples);

        for(int iSample = 0; iSample < iNumSamples; iSample++, 
            uiCurrentPixelIndex++)
        {

//          IMPORTANT? Textures in Maya are reversed
//            int iV = (iVSamples - 1) - (uiCurrentPixelIndex / iUSamples);
            int iV = uiCurrentPixelIndex / iUSamples;
            int iU = uiCurrentPixelIndex % iUSamples;

            afUCoords[iSample] = fUHalf + ((float)iU * fUOffset);
            afVCoords[iSample] = fVHalf + ((float)iV * fVOffset);

            afFilterSizes[iSample] = 0.0f;
        }

        stat = MRenderUtil:: sampleShadingNetwork(
            kShadingNodeColorName,
            iNumSamples,
            bUseShadowMaps,
            bResueMaps,
            kCameraMatrix,
            NULL,
            &afUCoords,
            &afVCoords,
            NULL,
            NULL,
            NULL,
            NULL,
            &afFilterSizes,
            akResultColors,
            akResultTransparencies);

        if (stat != MS::kSuccess)
        {
            delete[] pucImage32;
            return NULL;
        }

        if (bHasTransparency)
        {
            stat = MRenderUtil::sampleShadingNetwork(
                kShadingNodeTransparencyName,
                iNumSamples,
                bUseShadowMaps,
                bResueMaps,
                kCameraMatrix,
                NULL,
                &afUCoords,
                &afVCoords,
                NULL,
                NULL,
                NULL,
                NULL,
                &afFilterSizes,
                akResultColorsT,
                akResultTransparenciesT);

            if (stat != MS::kSuccess)
            {
                delete[] pucImage32;
                return NULL;
            }
        }

        float afResult[3];
        float afResultT[3];

        int iLength = akResultColors.length();

        for(int iLoop = 0; iLoop < iLength; iLoop++)
        {
            akResultColors[iLoop].get(afResult);

            afResult[2] *= 255.0f;
            afResult[2] += 0.5f;
            afResult[2] = max(afResult[2], 0.0f);
            afResult[2] = min(afResult[2], 255.0f);
            *pucCurrentPixel++ = (unsigned char)afResult[2];

            afResult[1] *= 255.0f;
            afResult[1] += 0.5f;
            afResult[1] = max(afResult[1], 0.0f);
            afResult[1] = min(afResult[1], 255.0f);
            *pucCurrentPixel++ = (unsigned char)afResult[1];

            afResult[0] *= 255.0f;
            afResult[0] += 0.5f;
            afResult[0] = max(afResult[0], 0.0f);
            afResult[0] = min(afResult[0], 255.0f);
            *pucCurrentPixel++ = (unsigned char)afResult[0];

            if (!bHasTransparency)
            {
                *pucCurrentPixel++ = (unsigned char)255;
            }
            else
            {
                akResultColorsT[iLoop].get(afResultT);

                afResultT[0] *= 255.0f;
                afResultT[0] += 0.5f;
                afResultT[0] = max(afResultT[0], 0.0f);
                afResultT[0] = min(afResultT[0], 255.0f);
                *pucCurrentPixel++ = (unsigned char)255 - 
                    (unsigned char)afResultT[0];
            }
/*
            int iR = (unsigned char)afResult[0];
            int iG = (unsigned char)afResult[1];
            int iB = (unsigned char)afResult[2];
            int iA = 255 - (unsigned char)afResultT[0];

            cerr << iLoop << "\t UV " << afUCoords[iLoop] << " , " << 
                afVCoords[iLoop] << "\t\tRed " << iR << "\t\tGreen " << iG <<
                "\t\tBlue " << iB << "\t\tAlpha " << iA << endl;
*/
        }

    }

    return pucImage32;

//#endif
}

//
//  Be careful of Textures now that we may not be generating them all of
//  the time.
//


static
unsigned char *readTextureFile( MString textureFile, MString transFile,
    int useTransparency, int &xres, int &yres, int &zres)
{
    IFFimageReader reader;
    MStatus Rstat;
            
    Rstat = reader.open( textureFile );
    if ( Rstat != MS::kSuccess )
    {   
        cerr << "Error reading file " << textureFile.asChar() << " "
                << reader.errorString().asChar() << endl;
        return 0;
    }
            
    int imageWidth,imageHeight,bytesPerChannel;
    
    //int imageComp;
    
    Rstat = reader.getSize (imageWidth,imageHeight);
    if ( Rstat != MS::kSuccess )
    {   
        reader.close();
        return 0;
    }

    bytesPerChannel = reader.getBytesPerChannel ();

            
    unsigned char *image = (unsigned char *)NiAlloc(char, imageWidth*imageHeight*4);
    unsigned char *imagePtr = image;
            
    Rstat = reader.readImage ();
    if ( Rstat != MS::kSuccess )
    {
        reader.close();
        NiFree( image );
        image = NULL;
        return 0;
    }

    // Now we can read the image into our area.  We only deal with 8 bit 
    // colours so if it is a 16bit file, we will truncate down to 8 bits.

    int x, y;
    const byte *bitmap = reader.getPixelMap ();
    const byte *pixel = bitmap;
    int bytesPerPixel = (bytesPerChannel == 1) ? 4 : 8;
           

    // Need to see if we actually read in an image file

    if ( pixel == 0 )
    {
        reader.close();
        NiFree( image );
        return 0;
    }
        
    //printf(" widht %d, height %d, bpp %d \n", 
    //          imageWidth, imageHeight, bytesPerPixel );
    
    for ( y = 0; y < imageHeight; y++ ) {
           
        for ( x = 0; x < imageWidth; x++, pixel += bytesPerPixel ) {
                
            if ( bytesPerChannel == 1 )
            {
                imagePtr[0] = pixel[0];
                imagePtr[1] = pixel[1];
                imagePtr[2] = pixel[2];
                imagePtr[3] = pixel[3];
            } else {
                imagePtr[0] = pixel[0];
                imagePtr[1] = pixel[2];
                imagePtr[2] = pixel[4];
                imagePtr[3] = pixel[6];
            }   

            imagePtr += 4;
                    
        }   
                
    }   
            
    Rstat = reader.close();

    xres = imageWidth;
    yres = imageHeight;
    zres = 4;

    //
    // Now most of this is repeated to read in the transparency mask
    //

    if ( useTransparency )
    {

        Rstat = reader.open( transFile );
        if ( Rstat != MS::kSuccess )
        {    
            cerr << "Error reading transparency file " 
                << transFile.asChar() << " "
                << reader.errorString().asChar() << endl;
            return image;
        }   
        
    
        Rstat = reader.getSize (imageWidth,imageHeight);
        if ( Rstat != MS::kSuccess )
        {    
            reader.close();
            return image;
        }   
    
        bytesPerChannel = reader.getBytesPerChannel ();

        imagePtr = image; 
             
        Rstat = reader.readImage ();
        if ( Rstat != MS::kSuccess )
        {
            reader.close();
            return image;
        }   
    
        // Now we can read the image into our area.  
        // We only deal with 8 bit colours    
        // so if it is a 16bit file, we will truncate down to 8 bits.
    
        bitmap = reader.getPixelMap ();
        pixel = bitmap;
        bytesPerPixel = (bytesPerChannel == 1) ? 4 : 8;

        // Need to see if we actually read in an image file
    
        if ( pixel == 0 )
        {   
            reader.close();
            return image;
        }   

        //printf(" widht %d, height %d, bpp %d \n", 
        //          imageWidth, imageHeight, bytesPerPixel );

        for ( y = 0; y < imageHeight; y++ )
        {
            for ( x = 0; x < imageWidth; x++, pixel += bytesPerPixel ) 
            {
                //
                // For now we check to see which platform we are on and 
                // generate the alpha based on the ordering.  Seems to be
                // different on the different platforms.  Maybe taked care 
                // of in the IFF library.
                //

                if ( bytesPerChannel == 1 )
                {
#ifdef WIN32
                    imagePtr[3] = 255 - (unsigned char)((pixel[0] + pixel[1] +
                        pixel[2])/3.0f);
#else
                    imagePtr[0] = 255 - (unsigned char)((pixel[1] + pixel[2] + 
                        pixel[3])/3.0f);
#endif
                } 
                else 
                {
#ifdef WIN32
                    imagePtr[6] = 255 - (unsigned char)((pixel[0] + pixel[2] + 
                        pixel[4])/3.0f);
#else
                    imagePtr[0] = 255 - (unsigned char)((pixel[0] + pixel[2] + 
                        pixel[4])/3.0f);
#endif
                }

                imagePtr += 4;

            }
        }

        Rstat = reader.close();
    }

    return image;
}

//  ========== DtTextureGetID ==========
//
//
//  NOTES:  Added by Andrew Jones 8-31-99.  
//          I added this function so that I could get some information
//          about the texture based on the shape (and group)
//
//  SYNOPSIS
//  Return the material id used by the group.
//
int DtTextureGetID( int shapeID, int groupID, int* tex_id )
{
    int iMtlID;
    DtMtlGetID( shapeID, groupID, &iMtlID);
    DtTextureGetID( iMtlID, tex_id );
    return 1;
}  // DtTextureGetID //

//  ========== DtTextureGetIDMulti ==========
//
//
//  NOTES:  Added by Andrew Jones 8-31-99.  
//          I added this function so that I could get some information
//          about the texture based on the shape (and group)
//
//  SYNOPSIS
//  Return the material id used by the group.
//
int DtTextureGetIDMulti( int shapeID, int groupID, char *texType, 
    int* tex_id )
{
    int iMtlID;
    DtMtlGetID( shapeID, groupID, &iMtlID);
    DtTextureGetIDMulti( iMtlID, texType, tex_id );
    return 1;
}  // DtTextureGetID //


int shader_NiMultiShader_PixelShaderTextures(MObject alShader, 
    MObject object, MDagPath &dagPath)
{
    char* pcAttHead = "Ni_PixelShaderAttribute_";
    char* pcAttTail = "_Texture";
    int texCnt = 0;
    MStringArray Tex_mapped;


    // Find all of the attributes
    MStatus status;
    MFnDependencyNode dgNode;
    status = dgNode.setObject( alShader );
    MString shaderName = dgNode.name(&status);
    NIASSERT(status == MS::kSuccess);


    unsigned int uiAttributeCount = dgNode.attributeCount(&status);
    NIASSERT(status == MS::kSuccess);

    for(unsigned int uiLoop = 0; uiLoop < uiAttributeCount; uiLoop++)
    {
        MFnAttribute kAttribute( dgNode.attribute(uiLoop, &status));

        NIASSERT(status == MS::kSuccess);

        MString kName = kAttribute.name(&status);
        NIASSERT(status == MS::kSuccess);

        // Check for the header
        const char* pcTemp = kName.asChar();
        NI_UNUSED_ARG(pcTemp);

        if(!strncmp(kName.asChar(), pcAttHead, strlen(pcAttHead)))
        {
            const char* pcTail = kName.asChar() + strlen(kName.asChar()) - 
                strlen(pcAttTail);

            // Check for the Tail
            if(!strcmp(pcTail, pcAttTail))
            {

                // Check the Pixel Shader attributes for connectinos
                MString command = MString("listConnections ") + shaderName
                    + MString(".") + kName + MString(";");

                if ( MS::kSuccess == MGlobal::executeCommand( command, 
                    Tex_mapped ) )
                {
                    MString texNode = MString(".") + kName;
                    texCnt += generate_NiMultiTextureImages( 
                        shaderName, texNode, kName,   Tex_mapped, dagPath, 
                        alShader);
                }
                
            }
        }
    }

    return texCnt;
}

//  ========== shader_NiMultiShader ==========
//
//
//  NOTES:  Added by Andrew Jones 9-10-2K.  
//          Adding support for a new type of shader.  This shader
//          will be used by the artist to create darkmaps, glowmaps,
//          decalmaps, shadowmaps, in short, anything requiring
//          multiple uv's.
//
int shader_NiMultiShader(MObject alShader, MObject object, MDagPath &dagPath)
{
    // This only handles NiMultiShader's
    if ( 0 != strcmp( "NiMultiShader", objectType( alShader ) ) )
    {
        return 0;
    }

    MString   shaderName;
    MString   textureName;
    MString   textureFile = "";
    MString   projectionName = "";
    MString   convertName;
    MString   command;
    MStringArray result;
    MStringArray arrayResult;

    MStatus   status;

    MStringArray    Tex_mapped;

    int texCnt = 0;


    MFnDependencyNode dgNode;
    status = dgNode.setObject( alShader );
    shaderName = dgNode.name( &status );

    result.clear();

    // Lets return without truing if we get a null shader name

    if ( shaderName.length() == 0 )
    {
        return 0;
    }


    // We are not going to do the color and transparency thing here

    // We will be doing the other textures here
    // DARK MAP
    command = MString("listConnections ") + shaderName
                                          + MString(".DarkMap;");
    if ( MS::kSuccess == MGlobal::executeCommand( command, Tex_mapped ) )
    {
        MString texType( "DarkMap" );
        MString texNode( ".DarkMap" );
        texCnt += generate_NiMultiTextureImages( 
            shaderName, texNode, texType,   Tex_mapped, dagPath, alShader);
    }

    // DETAIL MAP
    command = MString("listConnections ") + shaderName
                                          + MString(".DetailMap;");
    if ( MS::kSuccess == MGlobal::executeCommand( command, Tex_mapped ) )
    {
        MString texType( "DetailMap" );
        MString texNode( ".DetailMap" );
        texCnt += generate_NiMultiTextureImages( 
            shaderName, texNode, texType,   Tex_mapped, dagPath, alShader);
    }

    // DECAL1 MAP
    command = MString("listConnections ") + shaderName
                                          + MString(".Decal1Map;");
    if ( MS::kSuccess == MGlobal::executeCommand( command, Tex_mapped ) )
    {
        MString texType( "Decal1Map" );
        MString texNode( ".Decal1Map" );
        texCnt += generate_NiMultiTextureImages( 
            shaderName, texNode, texType,   Tex_mapped, dagPath, alShader);
    }

    // DECAL2 MAP
    command = MString("listConnections ") + shaderName
                                          + MString(".Decal2Map;");
    if ( MS::kSuccess == MGlobal::executeCommand( command, Tex_mapped ) )
    {
        MString texType( "Decal2Map" );
        MString texNode( ".Decal2Map" );
        texCnt += generate_NiMultiTextureImages( 
            shaderName, texNode, texType,   Tex_mapped, dagPath, alShader);
    }
    // GLOW MAP
    command = MString("listConnections ") + shaderName
                                          + MString(".GlowMap;");
    if ( MS::kSuccess == MGlobal::executeCommand( command, Tex_mapped ) )
    {
        MString texType( "GlowMap" );
        MString texNode( ".GlowMap" );
        texCnt += generate_NiMultiTextureImages( 
            shaderName, texNode, texType,   Tex_mapped, dagPath, alShader);
    }

    // GLOSS MAP
    command = MString("listConnections ") + shaderName
                                          + MString(".GlossMap;");
    if ( MS::kSuccess == MGlobal::executeCommand( command, Tex_mapped ) )
    {
        MString texType( "GlossMap" );
        MString texNode( ".GlossMap" );
        texCnt += generate_NiMultiTextureImages( 
            shaderName, texNode, texType,   Tex_mapped, dagPath, alShader);
    }

       // BUMP MAP
    command = MString("listConnections ") + shaderName
                                          + MString(".BumpMap;");
    if ( MS::kSuccess == MGlobal::executeCommand( command, Tex_mapped ) )
    {
        MString texType( "BumpMap" );
        MString texNode( ".BumpMap" );
        texCnt += generate_NiMultiTextureImages( 
            shaderName, texNode, texType,   Tex_mapped, dagPath, alShader);
    }

    // Normal MAP
    command = MString("listConnections ") + shaderName
        + MString(".NormalMap;");
    if ( MS::kSuccess == MGlobal::executeCommand( command, Tex_mapped ) )
    {
        MString texType( "NormalMap" );
        MString texNode( ".NormalMap" );
        texCnt += generate_NiMultiTextureImages( 
            shaderName, texNode, texType,   Tex_mapped, dagPath, alShader);
    }

    // Parallax MAP
    command = MString("listConnections ") + shaderName
        + MString(".ParallaxMap;");
    if ( MS::kSuccess == MGlobal::executeCommand( command, Tex_mapped ) )
    {
        MString texType( "ParallaxMap" );
        MString texNode( ".ParallaxMap" );
        texCnt += generate_NiMultiTextureImages( 
            shaderName, texNode, texType,   Tex_mapped, dagPath, alShader);
    }

    texCnt += shader_NiMultiShader_PixelShaderTextures(alShader, object, 
        dagPath);
    
    return texCnt; 
}

int generate_NiMultiTextureImages(  
                        MString &shaderName, 
                        MString &texNode,
                        MString &texType, 
                        MStringArray& texMap,
                        MDagPath &dagPath,
                        MObject alShader)
{
    NI_UNUSED_ARG(texNode
        );
    NI_UNUSED_ARG(dagPath);

    MString         command;
    MString         textureName;
    MString         convertName;
    MString         textureFile;

    MString         bumpTexture;

    MStatus         status;

    MStringArray    result;
    MStringArray    arrayResult;

    
    //MtlStruct     *mtl;
    
    int             xres, yres, zres;

    bool            fileTexture;
    bool            bSelfLoadingTexture = false;


    MObject kTextureNode = FollowInputAttribute(alShader, texType);

    // Now check to see if something was there

    if ( (texMap.length() && texMap[0].length()) ) 
    {
        command = MString("ls -st ") + texMap[0];
        status = MGlobal::executeCommand( command, arrayResult );

        if ( status != MS::kSuccess ) 
            return 0;

        // Now see if it is a file texture in order to get the size of
        // the image to use

        fileTexture = false;

        if ( (arrayResult.length() ==2) && arrayResult[1].length() &&
            (arrayResult[1] == MString("file") )  ) 
        {
            textureName = arrayResult[0];
            command = MString("getAttr ") + textureName +
                                        MString(".fileTextureName");

            status = MGlobal::executeCommand( command, result );

            if ( status != MS::kSuccess ) 
                return 0;

            textureFile = result[0];
            fileTexture = true;

            // Check to see if this will be a self loading texture for the 
            // Exporter
            bSelfLoadingTexture = IsSelfLoadTexture(textureFile.asChar());


            if( DtExt_Debug() )
                cerr << "file: texture is: " << textureFile.asChar() << endl;

        }
        else if ( (arrayResult.length() ==2) && arrayResult[1].length() &&
                    (arrayResult[1] == MString("bump2d") )  ) 
        {
            command = MString("listConnections ") + texMap[0]
                                       + MString(".bumpValue;");
            status = MGlobal::executeCommand( command, result );

            if ( (result.length() && result[0].length()) )
            {   

                bumpTexture = result[0];

                command = MString("ls -st ") + result[0];
                status = MGlobal::executeCommand( command, arrayResult );
        
                if ( status != MS::kSuccess ) 
                    return 0;
        
                // Now see if it is a file texture in order to get the size
                // of the image to use
        
                fileTexture = false;
        
                if ( (arrayResult.length() ==2) && arrayResult[1].length() &&
                    (arrayResult[1] == MString("file") )  )
                {
                    textureName = arrayResult[0]; 
                    command = MString("getAttr ") + textureName +
                                            MString(".fileTextureName");
            
                    status = MGlobal::executeCommand( command, result );
            
                    if ( status != MS::kSuccess )
                        return 0;

                    textureFile = result[0];
                    fileTexture = true;

                    // Check to see if this will be a self loading texture 
                    // for the Exporter
                    bSelfLoadingTexture = IsSelfLoadTexture(
                        textureFile.asChar());

            
                    if( DtExt_Debug() )
                        cerr << "file: texture is: " << 
                            textureFile.asChar() << endl;
                }
            }
        }

        // There is a texture, I don't care what kind it is, going to use
        // convertSolidTx to generate a file texture

        textureName = texMap[0];

        // Do nothing if texture is already in the list.
        //
        if( MtFindTex(local, (char *) textureName.asChar() ) != NULL)
        {
            SetMtlTypeName(shaderName, texType,(char *) textureName.asChar());

            return( 1 );
        }
        
        // Will use the convertSolidTx command to generate the image to use.
        //
    
        unsigned char *image = NULL;
           
        saveTransforms( textureName );

        // need to have different temp location for SGI and NT

        char tmpFile[2048];
        char outTmpPath[_MAX_PATH];
        GetTempPath( _MAX_PATH, outTmpPath );
        GetTempFileName( outTmpPath, "mat", 
#if defined(_MSC_VER) && _MSC_VER >= 1400
            _getpid(), 
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
            getpid(), 
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
            tmpFile );
            for ( unsigned int ii=0; ii < strlen( tmpFile ); ii++ )
                if ( tmpFile[ii] == '\\' )
                    tmpFile[ii] = '/';

        // 
        // Check to see if a projection was found and if so then use it
        // as the base object to do the convert solid texture command on
        //

        // Need to check for the bump mapping.
        // as there is a utitity node in front of the actual texture
        // that is to be converted.

        MString forSizeName;

        if ( texType == "bump" )
        {
            // convertName = textureName + MString(".bumpValue");
            convertName = bumpTexture + MString( ".outColor" );
            forSizeName = bumpTexture;
        } 
        else
        {
            convertName = textureName + MString( ".outColor" );
            forSizeName = textureName;
        }


        // Use the supplied x/y size to generate the texture files

        getTextureFileSize( fileTexture, forSizeName, textureFile, 
                                                    xres, yres, zres );

        if ( DtExt_OriginalTexture() )
        {
            // We will use a temp file
            restoreTransforms( textureName );

            // allocate a little memory to make other things happy

            image = (unsigned char *)NiStrdup("originalFile");

        } else if(!bSelfLoadingTexture)
        {

            image = CreateTextureFromNode( kTextureNode, xres, yres);

            restoreTransforms( textureName );
        }


        // Restore the transforms
        restoreTransforms( textureName );

        // If we have an image in memory, then we add it to our local table
        // references.

        // Need to find file name for texture if possible.

        if ( image || bSelfLoadingTexture )
        {   
            if( !MtAddTex(local, (char *)textureName.asChar(), 
                    (char *)textureFile.asChar(), fileTexture, 
                        image, xres, yres, 4, kTextureNode,
                        texType == "BumpMap") )
            {
                DtExt_Msg("\"%s\" already inlist delete this copy ofimage\n",
                        (char *)textureName.asChar() );
                NiFree( image );
                image = NULL;
            }

            // add texture name to material
            //
            SetMtlTypeName(shaderName, texType,(char *) textureName.asChar());

            return 1;
        }
    }

    return 0;
}

void SetMtlTypeName(MString &shaderName, MString &texType,char *szTextureName)
{
    
    // add texture name to material
    //
    MtlStruct       *mtl = MtFindMtl(local, (char *) shaderName.asChar() );

    // Check for an invalid material
    if (mtl == NULL)
        return;

    if ( texType == "ambient" )
    {
        if ( mtl->ambient_name )
            NiFree( mtl->ambient_name );
        mtl->ambient_name = NiStrdup( szTextureName );
    }
    else if ( texType == "diffuse" )
    {
        if ( mtl->diffuse_name )
            NiFree( mtl->diffuse_name );
        mtl->diffuse_name = NiStrdup( szTextureName );
    }
    else if ( texType == "translucence" )
    {
        if ( mtl->translucence_name )
            NiFree( mtl->translucence_name );
        mtl->translucence_name = NiStrdup( szTextureName );
    }
    else if ( texType == "incandescence" ) 
    {
        if ( mtl->incandescence_name )
            NiFree( mtl->incandescence_name );
        mtl->incandescence_name = NiStrdup( szTextureName );
    } 
    else if ( texType == "bump" )
    {
        if ( mtl->bump_name )
            NiFree( mtl->bump_name );
        mtl->bump_name = NiStrdup( szTextureName );
    } 
    else if ( texType == "shininess" )
    {
        if ( mtl->shininess_name )
            NiFree( mtl->shininess_name );
        mtl->shininess_name = NiStrdup( szTextureName );
    }
    else if ( texType == "specular" )
    {
        if ( mtl->specular_name )
            NiFree( mtl->specular_name );
        mtl->specular_name = NiStrdup( szTextureName );
    } 
    else if ( texType == "reflectivity" )
    {
        if ( mtl->reflectivity_name )
            NiFree( mtl->reflectivity_name );
        mtl->reflectivity_name = NiStrdup( szTextureName );
    }
    else if ( texType == "reflected" )
    {
        if ( mtl->reflected_name )
            NiFree( mtl->reflected_name );
        mtl->reflected_name = NiStrdup( szTextureName );
    }
    else
    {
        TextureMap* pkMap = mtl->FindTextureMap(texType.asChar());
        if(pkMap == NULL)
        {
            pkMap = mtl->AddTextureMap(texType.asChar());
        }

        NiFree(pkMap->m_pcMapTextureName);
        pkMap->m_pcMapTextureName = NiStrdup(szTextureName);
    }
}

//  ========== bool IsTextureExternal( MString textureName ) ==========
//
//
//  NOTES:  Added by Andrew Jones 12-03-2K.  
//          This function is intended to replace DtExt_softTextures()
//          because sometimes we have textures that are external,
//          and sometimes they are sampled from Maya.
bool IsTextureExternal( MString textureName )
{
    MStatus status;
    MString command;
    int iExists;

    command = MString("attributeExists fileTextureName ") + textureName;
    status = MGlobal::executeCommand( command, iExists );

    return (( status == MS::kSuccess ) && (iExists != 0));
}

//  ========== bool DtAttributeExists ==========
//  NOTES:  Added by Drew Gugliotta 2001
bool DtAttributeExists(const char* szObject, const char* szAttribute)
{
    MStatus status;
    MString command;

    command = MString("attributeExists ") + MString(szAttribute) + 
        MString(" ") + MString(szObject) + MString(";");

    int iExists = 0;

    status = MGlobal::executeCommand( command, iExists );

    if ( status != MS::kSuccess )
    {
        return false;
    }

    return (iExists != 0);
}

//  ========== DtTextureGetMaterialShading ==========
bool DtTextureGetMaterialShading(int iMtlID, char &cShading)
{
    if((local == NULL) || (iMtlID >= local->mtl_ct))
    {
        return 0;
    }

    cShading = local->mtl[iMtlID].Shading;

        // Return true if it is un initialized (-1)
    return (local->mtl[iMtlID].Shading != -1);
}

//  ========== DtTextureGetMaterialTextureApplyMode ==========
bool DtTextureGetMaterialTextureApplyMode(int iMtlID, char &cTextureApplyMode)
{
    if((local == NULL) || (iMtlID >= local->mtl_ct))
    {
        return 0;
    }

    cTextureApplyMode = local->mtl[iMtlID].TextureApplyMode;

        // Return true if it is un initialized (-1)
    return (local->mtl[iMtlID].TextureApplyMode != -1);
}


//  ========== DtTextureGetMaterialNBTMethod ==========
bool DtTextureGetMaterialNBTMethod(int iMtlID, char &cNBTMethod)
{
    if((local == NULL) || (iMtlID >= local->mtl_ct))
    {
        return 0;
    }

    cNBTMethod = local->mtl[iMtlID].NBTMethod;

        // Return true if it is un initialized (-1)
    return (local->mtl[iMtlID].NBTMethod != -1);
}


//  ========== DtTextureGetMaterialVertexColorOverride ==========
//
bool DtTextureGetMaterialVertexColorOverride(int iMtlID)
{
    if((local == NULL) || (iMtlID >= local->mtl_ct))
    {
        return 0;
    }

    return local->mtl[iMtlID].VertexColorOverride;
}

//  ========== DtTextureGetMaterialVertexColorOverride ==========
//
bool DtTextureGetMaterialVertexColorOverride(int shapeID, int groupID)
{
    int iMtlID;
    DtMtlGetID( shapeID, groupID, &iMtlID);

    return DtTextureGetMaterialVertexColorOverride(iMtlID);
}

//  ========== DtTextureGetMaterialVertexColorSrcMode ==========
//
bool DtTextureGetMaterialVertexColorSrcMode(int iMtlID, char &cSrcMode)
{
    if((local == NULL) || (iMtlID >= local->mtl_ct))
    {
        return 0;
    }

    cSrcMode = local->mtl[iMtlID].VertexColorSrcMode;

        // Return true if it is un initialized (-1)
    return (local->mtl[iMtlID].VertexColorSrcMode != -1);
}

//  ========== DtTextureGetMaterialVertexColorSrcMode ==========
//
bool DtTextureGetMaterialVertexColorSrcMode(int shapeID, int groupID, 
    char &cSrcMode)
{
    int iMtlID;
    DtMtlGetID( shapeID, groupID, &iMtlID);

        // Return true if it is un initialized (-1)
    return DtTextureGetMaterialVertexColorSrcMode(iMtlID, cSrcMode);
}

//  ========== DtTextureGetMaterialVertexColorLightMode ==========
//
bool DtTextureGetMaterialVertexColorLightMode(int iMtlID, char &cLightMode)
{
    if((local == NULL) || (iMtlID >= local->mtl_ct))
    {
        return 0;
    }

    cLightMode = local->mtl[iMtlID].VertexColorLightMode;

        // Return true if it is un initialized (-1)
    return (local->mtl[iMtlID].VertexColorLightMode != -1);
}

//  ========== DtTextureGetMaterialVertexColorLightMode ==========
//
bool DtTextureGetMaterialVertexColorLightMode(int shapeID, int groupID, 
    char &cLightMode)
{
    int iMtlID;
    DtMtlGetID( shapeID, groupID, &iMtlID);

        // Return true if it is un initialized (-1)
    return DtTextureGetMaterialVertexColorLightMode(iMtlID, cLightMode);
}

//  ========== DtTextureGetMaterialTransparencyOverride ==========
bool DtTextureGetMaterialTransparencyOverride(int iMtlID)
{
    return local->mtl[iMtlID].TransparencyOverride;
}

//  ========== DtTextureGetMaterialTransparencyUseAlphaBlending ==========
bool DtTextureGetMaterialTransparencyUseAlphaBlending(int iMtlID)
{
    return local->mtl[iMtlID].TransparencyUseAlphaBlending;
}

//  ========== DtTextureGetMaterialTransparencySource ==========
bool DtTextureGetMaterialTransparencySource(int iMtlID, char &cSrcBlendMode)
{
    if((local == NULL) || (iMtlID >= local->mtl_ct))
    {
        return 0;
    }

    cSrcBlendMode = local->mtl[iMtlID].TransparencySource;

        // Return true if it is un initialized (-1)
    return (local->mtl[iMtlID].TransparencySource != -1);
}

//  ========== DtTextureGetMaterialTransparencyDestination ==========
bool DtTextureGetMaterialTransparencyDestination(int iMtlID, 
    char &cDestBlendMode)
{
    if((local == NULL) || (iMtlID >= local->mtl_ct))
    {
        return 0;
    }

    cDestBlendMode = local->mtl[iMtlID].TransparencyDestination;

        // Return true if it is un initialized (-1)
    return (local->mtl[iMtlID].TransparencyDestination != -1);
}

//  ========== DtTextureGetMaterialTransparencyUseAlphaTesting ==========
bool DtTextureGetMaterialTransparencyUseAlphaTesting(int iMtlID)
{
    if((local == NULL) || (iMtlID >= local->mtl_ct))
    {
        return 0;
    }

    return local->mtl[iMtlID].TransparencyUseAlphaTesting;
}

//  ========== DtTextureGetMaterialTransparencyNoTestingSorter ==========
bool DtTextureGetMaterialTransparencyNoTestingSorter(int iMtlID)
{
    if((local == NULL) || (iMtlID >= local->mtl_ct))
    {
        return 0;
    }

    return local->mtl[iMtlID].TransparencyNoTestingSorter;
}

//  ========== DtTextureGetMaterialTransparencyTestReference ==========
bool DtTextureGetMaterialTransparencyTestReference(int iMtlID, 
    short &sTestReference)
{
    if((local == NULL) || (iMtlID >= local->mtl_ct))
    {
        return 0;
    }

    sTestReference = local->mtl[iMtlID].TransparencyTestReference;

        // Return true if it is un initialized (-1)
    return (local->mtl[iMtlID].TransparencyTestReference != -1);
}

//  ========== DtTextureGetMaterialVertexColorLightMode ==========
bool DtTextureGetMaterialTransparencyTestingTestMode(int iMtlID, 
    char &cTestingMode)
{
    if((local == NULL) || (iMtlID >= local->mtl_ct))
    {
        return 0;
    }

    cTestingMode = local->mtl[iMtlID].TransparencyTestingTestMode;

        // Return true if it is un initialized (-1)
    return (local->mtl[iMtlID].TransparencyTestingTestMode != -1);
}
//-------------------------------------------------------------------------------------------------
bool DtTextureGetMaterialMapProperties(
    int iMtlID, 
    char* pType, 
    char &cFilterMode, 
    char &cMap, 
    unsigned short& usMaxAnisotropy)
{
    TextureMap* pkMap = local->mtl[iMtlID].FindTextureMap(pType);
    
    if (pkMap == NULL)
        return false;

    cMap = pkMap->m_cMap;
    cFilterMode = pkMap->m_cMapFilter;

    // if anisotropy only
    if (cFilterMode == 6)
        usMaxAnisotropy = pkMap->m_usMaxAnisotropy;

    return true;
}
//-------------------------------------------------------------------------------------------------
bool DtTextureGetMaterialMapProperties(
    int shapeID, 
    int groupID, 
    char* pType, 
    char &cFilterMode,
    char& cMap,
    unsigned short& usMaxAnisotropy)
{
    int iMtlID;
    DtMtlGetID( shapeID, groupID, &iMtlID);

    return DtTextureGetMaterialMapProperties(iMtlID, pType, cFilterMode, cMap, usMaxAnisotropy);
}
//-------------------------------------------------------------------------------------------------
bool DtTextureGetMaterialMapFlipBookProperties(int iMtlID, char* pType, 
        bool &bFlipBook, short &sStartFrame, short &sEndFrame, 
        float &fFramesPerSecond, float &fStartTime, char &cCycleType)
{

    TextureMap* pkMap = local->mtl[iMtlID].FindTextureMap(pType);

    if(pkMap == NULL)
    {
        return false;
    }

    bFlipBook = pkMap->m_bMapFlipBook;
    sStartFrame = pkMap->m_sMapFlipBookStartFrame;
    sEndFrame = pkMap->m_sMapFlipBookEndFrame;
    fFramesPerSecond = pkMap->m_fMapFlipBookFramesPerSecond;
    fStartTime = pkMap->m_fMapFlipBookStartTime;
    cCycleType = pkMap->m_cMapFlipBookCycleType;

/*
    if(strcmp(pType, "color") == 0)
    {
        bFlipBook = local->mtl[iMtlID].BaseMapFlipBook;
        sStartFrame = local->mtl[iMtlID].BaseMapFlipBookStartFrame;
        sEndFrame = local->mtl[iMtlID].BaseMapFlipBookEndFrame;
        fFramesPerSecond = local->mtl[iMtlID].BaseMapFlipBookFramesPerSecond;
        fStartTime = local->mtl[iMtlID].BaseMapFlipBookStartTime;
        cCycleType = local->mtl[iMtlID].BaseMapFlipBookCycleType;
    }

    if(strcmp(pType, "DarkMap") == 0)
    {
        bFlipBook = local->mtl[iMtlID].DarkMapFlipBook;
        sStartFrame = local->mtl[iMtlID].DarkMapFlipBookStartFrame;
        sEndFrame = local->mtl[iMtlID].DarkMapFlipBookEndFrame;
        fFramesPerSecond = local->mtl[iMtlID].DarkMapFlipBookFramesPerSecond;
        fStartTime = local->mtl[iMtlID].DarkMapFlipBookStartTime;
        cCycleType = local->mtl[iMtlID].DarkMapFlipBookCycleType;
    }

    if(strcmp(pType, "DetailMap") == 0)
    {
        bFlipBook = local->mtl[iMtlID].DetailMapFlipBook;
        sStartFrame = local->mtl[iMtlID].DetailMapFlipBookStartFrame;
        sEndFrame = local->mtl[iMtlID].DetailMapFlipBookEndFrame;
        fFramesPerSecond = 
            local->mtl[iMtlID].DetailMapFlipBookFramesPerSecond;

        fStartTime = local->mtl[iMtlID].DetailMapFlipBookStartTime;
        cCycleType = local->mtl[iMtlID].DetailMapFlipBookCycleType;
    }

    if(strcmp(pType, "Decal1Map") == 0)
    {
        bFlipBook = local->mtl[iMtlID].Decal1MapFlipBook;
        sStartFrame = local->mtl[iMtlID].Decal1MapFlipBookStartFrame;
        sEndFrame = local->mtl[iMtlID].Decal1MapFlipBookEndFrame;
        fFramesPerSecond = 
            local->mtl[iMtlID].Decal1MapFlipBookFramesPerSecond;

        fStartTime = local->mtl[iMtlID].Decal1MapFlipBookStartTime;
        cCycleType = local->mtl[iMtlID].Decal1MapFlipBookCycleType;
    }

    if(strcmp(pType, "Decal2Map") == 0)
    {
        bFlipBook = local->mtl[iMtlID].Decal2MapFlipBook;
        sStartFrame = local->mtl[iMtlID].Decal2MapFlipBookStartFrame;
        sEndFrame = local->mtl[iMtlID].Decal2MapFlipBookEndFrame;
        fFramesPerSecond = 
            local->mtl[iMtlID].Decal2MapFlipBookFramesPerSecond;

        fStartTime = local->mtl[iMtlID].Decal2MapFlipBookStartTime;
        cCycleType = local->mtl[iMtlID].Decal2MapFlipBookCycleType;
    }

    if(strcmp(pType, "GlowMap") == 0)
    {
        bFlipBook = local->mtl[iMtlID].GlowMapFlipBook;
        sStartFrame = local->mtl[iMtlID].GlowMapFlipBookStartFrame;
        sEndFrame = local->mtl[iMtlID].GlowMapFlipBookEndFrame;
        fFramesPerSecond = local->mtl[iMtlID].GlowMapFlipBookFramesPerSecond;
        fStartTime = local->mtl[iMtlID].GlowMapFlipBookStartTime;
        cCycleType = local->mtl[iMtlID].GlowMapFlipBookCycleType;
    }

    if(strcmp(pType, "GlossMap") == 0)
    {
        bFlipBook = local->mtl[iMtlID].GlossMapFlipBook;
        sStartFrame = local->mtl[iMtlID].GlossMapFlipBookStartFrame;
        sEndFrame = local->mtl[iMtlID].GlossMapFlipBookEndFrame;
        fFramesPerSecond = 
            local->mtl[iMtlID].GlossMapFlipBookFramesPerSecond;

        fStartTime = local->mtl[iMtlID].GlossMapFlipBookStartTime;
        cCycleType = local->mtl[iMtlID].GlossMapFlipBookCycleType;
    }

    if(strcmp(pType, "BumpMap") == 0)
    {
        bFlipBook = local->mtl[iMtlID].BumpMapFlipBook;
        sStartFrame = local->mtl[iMtlID].BumpMapFlipBookStartFrame;
        sEndFrame = local->mtl[iMtlID].BumpMapFlipBookEndFrame;
        fFramesPerSecond = local->mtl[iMtlID].BumpMapFlipBookFramesPerSecond;
        fStartTime = local->mtl[iMtlID].BumpMapFlipBookStartTime;
        cCycleType = local->mtl[iMtlID].BumpMapFlipBookCycleType;
    }
*/

    if(!bFlipBook)
        return false;


    // VERIFY IT IS ATTACHED TEXTURE
    int iTextureID;
    DtTextureGetIDMulti(iMtlID, pType, &iTextureID);

    if(iTextureID < 0)
        return false;

    char *szFullPath = NULL;
    DtTextureGetFileNameID(iTextureID, &szFullPath);


    // VERIFY IT IS ATTACHED TO A FILE TEXTURE AND IS A FLIP BOOK
    if( (szFullPath != NULL) &&
        (strcmp(szFullPath,"") != 0) &&
        (sStartFrame <= sEndFrame) )
        return true;


    return false;
}


//  ========== DtTextureGetMapFlipBookProperties ==========
//
bool DtTextureGetMaterialMapFlipBookProperties(int shapeID, int groupID, 
    char* pType, bool &bFlipBook, short &sStartFrame, short &sEndFrame, 
    float &fFramesPerSecond, float &fStartTime, char &cCycleType)
{
    int iMtlID;
    DtMtlGetID( shapeID, groupID, &iMtlID);

    return DtTextureGetMaterialMapFlipBookProperties(iMtlID, pType, 
        bFlipBook, sStartFrame, sEndFrame, fFramesPerSecond, 
        fStartTime, cCycleType);

}


//  ========== DtTextureGetMaterialBumpMapProperties ==========
//
void DtTextureGetMaterialBumpMapProperties(int iMtlID,
        float &fBumpMapLuminaScale, float &fBumpMapLuminaOffset,
        float &fBumpMapMat00, float &fBumpMapMat01,
        float &fBumpMapMat10, float &fBumpMapMat11)
{
    fBumpMapLuminaScale = local->mtl[iMtlID].BumpMapLuminaScale;
    fBumpMapLuminaOffset = local->mtl[iMtlID].BumpMapLuminaOffset;
    fBumpMapMat00 = local->mtl[iMtlID].BumpMapMat00;
    fBumpMapMat01 = local->mtl[iMtlID].BumpMapMat01;
    fBumpMapMat10 = local->mtl[iMtlID].BumpMapMat10;
    fBumpMapMat11 = local->mtl[iMtlID].BumpMapMat11;

}

//  ========== DtTextureGetMaterialBumpMapProperties ==========
//
void DtTextureGetMaterialBumpMapProperties(int shapeID, int groupID,
        float &fBumpMapLuminaScale, float &fBumpMapLuminaOffset,
        float &fBumpMapMat00, float &fBumpMapMat01,
        float &fBumpMapMat10, float &fBumpMapMat11)
{
    int iMtlID;
    DtMtlGetID( shapeID, groupID, &iMtlID);

    DtTextureGetMaterialBumpMapProperties(iMtlID,
        fBumpMapLuminaScale, fBumpMapLuminaOffset,
        fBumpMapMat00, fBumpMapMat01,
        fBumpMapMat10, fBumpMapMat11);
}

//  ========== DtTextureGetMaterialParallaxMapProperties ==========
//
void DtTextureGetMaterialParallaxMapProperties(int iMtlID,
    float &fParallaxOffset)
{
    fParallaxOffset = local->mtl[iMtlID].ParallaxMapOffset;

}

//  ========== DtTextureGetMaterialParallaxMapProperties ==========
//
void DtTextureGetMaterialParallaxMapProperties(int shapeID, int groupID,
    float &fParallaxOffset)
{
    int iMtlID;
    DtMtlGetID( shapeID, groupID, &iMtlID);

    DtTextureGetMaterialParallaxMapProperties(iMtlID,
        fParallaxOffset);
}

//  ========== DtTextureNeedsMipMap ==========
//
bool DtTextureNeedsMipMap(int iTextureID)
{

    // Look at each Material
    for( int iMaterialID = 0; iMaterialID < local->mtl_ct; iMaterialID++)
    {

        int iNumMaps = DtExt_MtlGetNumTextureMapsByID(iMaterialID); 

        // Look at each Map
        for(int iLoop = 0; iLoop < iNumMaps; iLoop++)
        {
            const char* pcMapName = DtExt_MtlGetTextureMapNameByID(
                iMaterialID, iLoop);

            // Get the TextureID for this Material and Map
            int iTexID;
            DtTextureGetIDMulti(iMaterialID, pcMapName, &iTexID);

            // Check if the texture ID's match
            if (iTextureID == iTexID)
            {
                
                TextureMap* pkMap = 
                    local->mtl[iMaterialID].FindTextureMap(pcMapName);
                
                // Check the Filter type of the Map                
                if(pkMap && (pkMap->m_cMapFilter >= 2))
                    return true;
                
            }
        }
    }

    return false;
}

#ifdef WIN32
#pragma warning(default: 4244)
#endif // WIN32
