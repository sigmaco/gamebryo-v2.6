%{

// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
// 
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
// 
// Copyright (c) 1996-2008 Emergent Game Technologies.
// All Rights Reserved.
// 
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

// Turn off warning in automatically generated grammar:
//  warning C4065: switch statement contains 'default' but no 'case' labels
#if defined(WIN32) || defined(_XENON)
#pragma warning( disable : 4065 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4244 )
#pragma warning( disable : 4245 )
#pragma warning( disable : 4267 )
#pragma warning( disable : 4706 )
#pragma warning( disable : 4702 )
#endif

#if defined(_XENON)
    #include <xtl.h>
    #include <malloc.h>
    #include <NiD3DDefines.h>
#elif defined (WIN32)
    #include "windows.h"
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <math.h>
    #include <malloc.h>
    #include <NiD3DDefines.h>
#elif defined (_PS3)
    #include <ctype.h>    
#endif  //#if defined(_XENON)

    #include "NSFTextFile.h"
    #include "NSFParsedShader.h"

    #include <NiTArray.h>
    #include <NiShaderFactory.h>

    #include <NiTextureStage.h>
    #include <NiSemanticAdapterTable.h>
    
    #include <NSBStateGroup.h>
    #include <NSBRenderStates.h>
    #include <NSBStageAndSamplerStates.h>

    #include <NSBShader.h>
    #include <NSBAttributeDesc.h>
    #include <NSBAttributeTable.h>
    #include <NSBObjectTable.h>
    #include <NSBPackingDef.h>
    #include <NSBImplementation.h>
    #include <NSBStateGroup.h>
    #include <NSBConstantMap.h>
    #include <NSBPass.h>
    #include <NSBTextureStage.h>
    #include <NSBTexture.h>
    #include <NiOutputStreamDescriptor.h>
    #include <NiStreamOutSettings.h>
    
    unsigned int F2DW(float fValue)
    {
        union FloatIntRep
        {
            float f;
            unsigned int ui;
        } kValue;
        
        kValue.f = fValue;
        
        return kValue.ui;
    }
    
    #include <NSBUserDefinedDataSet.h>
    
    
    void NSFParsererror2(const char *s);
    void NSFParsererror(const char *s);
    int  yylex    (void);

    NSFTextFile* g_pkFile = 0;
    
    extern int NSFParserGetLineNumber();

//    #define _ENABLE_DEBUG_STRING_OUT_
    char g_szDSO[1024];
    bool g_bFirstDSOFileAccess = true;
    int g_iDSOIndent = 0;
    FILE* g_pfDSOFile = 0;
    void DebugStringOut(const char* pszOut, bool bIndent = true);

    #define YYDEBUG                 1
    #define NSFParsererror_VERBOSE  1
    #define YYERROR_VERBOSE         1
    #define YYMALLOC NiExternalMalloc
    #define YYFREE NiExternalFree

    #define ERR_INVALID_ENUM    "Invalid enumerant"
    #define ERR_INVALID_COMMAND "Invalid command in block"

    // Gamebryo
        
    #define FLOAT_ARRAY_SIZE        64
    #define MAX_QUOTE_LENGTH    6 * 1024

    NiTPrimitiveArray<float>* g_afValues;
    
    unsigned int ResetFloatValueArray(void);
    unsigned int AddFloatToValueArray(float fValue);
    
    void ResetFloatRangeArrays(void);
    unsigned int AddFloatToLowArray(float fValue);
    unsigned int AddFloatToHighArray(float fValue);
    
    // Ranges
    bool g_bRanged;
    unsigned int g_uiLow, g_uiHigh;
    unsigned int g_uiLowFloatValues;
    float g_afLowValues[FLOAT_ARRAY_SIZE];
    unsigned int g_uiHighFloatValues;
    float g_afHighValues[FLOAT_ARRAY_SIZE];
    
    typedef enum
    {
        CONSTMAP_VERTEX = 0,
        CONSTMAP_PIXEL,
        CONSTMAP_GEOMETRY
    } NiConstantMapMode;
    
    NiConstantMapMode g_eConstMapMode;
    unsigned int g_uiCurrentPlatforms;
    bool g_bConstantMapPlatformBlock = false;
    
    void AddObjectToObjectTable(NiShaderAttributeDesc::ObjectType eType,
        unsigned int uiIndex, const char* pcName, const char* pcDebugString);
    unsigned int DecodeAttribTypeString(char* pszAttribType);
    unsigned int DecodePlatformString(char* pszPlatform);
    bool AddAttributeToConstantMap(char* pszName, 
        unsigned int uiRegisterStart, unsigned int uiRegisterCount,
        unsigned int uiExtraNumber, bool bIsGlobal);
    bool SetupOperatorEntry(char* pszName, int iRegStart, int iRegCount, 
        char* pszEntry1, int iOperation, char* pszEntry2, bool bInverse, 
        bool bTranspose);
    NiShaderAttributeDesc::AttributeType DetermineOperatorResult(
        int iOperation, NiShaderAttributeDesc::AttributeType eType1, 
        NiShaderAttributeDesc::AttributeType eType2);
    NiShaderAttributeDesc::AttributeType DetermineResultMultiply(
        NiShaderAttributeDesc::AttributeType eType1, 
        NiShaderAttributeDesc::AttributeType eType2);
    NiShaderAttributeDesc::AttributeType DetermineResultDivide(
        NiShaderAttributeDesc::AttributeType eType1, 
        NiShaderAttributeDesc::AttributeType eType2);
    NiShaderAttributeDesc::AttributeType DetermineResultAdd(
        NiShaderAttributeDesc::AttributeType eType1, 
        NiShaderAttributeDesc::AttributeType eType2);
    NiShaderAttributeDesc::AttributeType DetermineResultSubtract(
        NiShaderAttributeDesc::AttributeType eType1, 
        NiShaderAttributeDesc::AttributeType eType2);

    void SetShaderProgramFile(NSBPass* pkPass, const char* pcFile,
        unsigned int uiPlatforms, NiGPUProgram::ProgramType eType);
    void SetShaderProgramEntryPoint(NSBPass* pkPass,
        const char* pcEntryPoint,unsigned int uiPlatforms,
        NiGPUProgram::ProgramType eType);
    void SetShaderProgramShaderTarget(NSBPass* pkPass,
        const char* pcShaderTarget, unsigned int uiPlatforms,
        NiGPUProgram::ProgramType eType);

    NiTPointerList<NSFParsedShader*> g_kParsedShaderList;
    NSFParsedShader* g_pkCurrShader = 0;

    // Binary Shader
    NSBShader* g_pkCurrNSBShader = 0;

    // Attribute Table
    bool g_bGlobalAttributes = false;
    NSBAttributeTable* g_pkCurrAttribTable = 0;

    // Object Table
    NSBObjectTable* g_pkCurrObjectTable = 0;

    // Packing Definition
    unsigned int g_uiCurrPDStream = 0;    
    bool g_bCurrPDFixedFunction = false;
    NSBPackingDef* g_pkCurrPackingDef = 0;

    // Requirements
    NSBRequirements* g_pkCurrRequirements = 0;

    // Implementation
    unsigned int g_uiCurrImplementation = 0;
    NSBImplementation* g_pkCurrImplementation = 0;

    // OutputStream
    NiOutputStreamDescriptor* g_pkCurrentOutputStreamDescriptor = 0;
    NiOutputStreamDescriptor::DataType g_eDataType = 
        NiOutputStreamDescriptor::DATATYPE_MAX;

    // RenderState Group
    NSBStateGroup* g_pkCurrRSGroup = 0;

    // ConstantMap
    unsigned int g_uiCurrImplemVSConstantMap = 0;
    unsigned int g_uiCurrImplemGSConstantMap = 0;
    unsigned int g_uiCurrImplemPSConstantMap = 0;
    unsigned int g_uiCurrPassVSConstantMap = 0;
    unsigned int g_uiCurrPassGSConstantMap = 0;
    unsigned int g_uiCurrPassPSConstantMap = 0;
    NSBConstantMap* g_pkCurrConstantMap = 0;

    // Pass
    unsigned int g_uiCurrPassIndex = 0;
    NSBPass* g_pkCurrPass = 0;

    // TextureStage
    NSBTextureStage* g_pkCurrTextureStage = 0;
    unsigned int g_uiCurrTextureSlot = 0;
    NSBTexture* g_pkCurrTexture = 0;
    
    bool g_bCurrStateValid = false;
    unsigned int g_uiCurrStateState = 0;
    unsigned int g_uiCurrStateValue = 0;
    bool g_bUseMapValue = false;

    NSBUserDefinedDataSet* g_pkCurrUDDataSet = 0;
    NSBUserDefinedDataBlock* g_pkCurrUDDataBlock = 0;
    
%}

%union 
{
    float fval;
    int   ival;
    char* sval;
    unsigned int uival;
    unsigned long  dword;
    unsigned short word;
    unsigned char  byte;
    bool bval;
    unsigned int vers;
}

%start shader_file

%token EOLN PATH L_ACCOLADE R_ACCOLADE
%token L_PARENTHESE R_PARENTHESE
%token L_BRACKET R_BRACKET
%token L_ANGLEBRACKET R_ANGLEBRACKET
%token OR
%token ASSIGN
%token COMMA
%token NSF_AT_SYMBOL NSF_COLON NSF_SEMICOLON
%token UNDERSCORE
%token ASTERIK FORWARDSLASH PLUS MINUS

%token <dword> N_HEX
%token <fval>  N_FLOAT
%token <ival>  N_INT
%token <sval>  N_STRING
%token <sval>  PATH
%token <sval>  N_QUOTE
%token <bval>  N_BOOL
%token <vers>  N_VERSION

%token NSFSHADER

%token ARTIST HIDDEN SAVE

%token ATTRIBUTES GLOBALATTRIBUTES ATTRIB
%token ATTRIB_BOOL ATTRIB_STRING ATTRIB_UINT 
%token ATTRIB_FLOAT ATTRIB_POINT2 ATTRIB_POINT3 ATTRIB_POINT4
%token ATTRIB_MATRIX3 ATTRIB_TRANSFORM ATTRIB_COLOR
%token ATTRIB_TEXTURE

%token PACKINGDEF

%token PD_STREAM
%token PD_FIXEDFUNCTION

%token SEMANTICADAPTERTABLE

%token PDP_POSITION PDP_POSITION0 PDP_POSITION1 PDP_POSITION2 
%token PDP_POSITION3 PDP_POSITION4 PDP_POSITION5 PDP_POSITION6
%token PDP_POSITION7 PDP_BLENDWEIGHTS PDP_BLENDINDICES PDP_NORMAL
%token PDP_POINTSIZE PDP_COLOR PDP_COLOR2
%token PDP_TEXCOORD0 PDP_TEXCOORD1 PDP_TEXCOORD2 PDP_TEXCOORD3
%token PDP_TEXCOORD4 PDP_TEXCOORD5 PDP_TEXCOORD6 PDP_TEXCOORD7
%token PDP_POSITION2 PDP_NORMAL2
%token PDP_TANGENT PDP_BINORMAL
%token PDP_EXTRADATA

%token PDT_FLOAT1 PDT_FLOAT2 PDT_FLOAT3 PDT_FLOAT4
%token PDT_UBYTECOLOR
%token PDT_SHORT1 PDT_SHORT2 PDT_SHORT3 PDT_SHORT4
%token PDT_UBYTE4
%token PDT_NORMSHORT1 PDT_NORMSHORT2 PDT_NORMSHORT3 PDT_NORMSHORT4
%token PDT_NORMPACKED3
%token PDT_PBYTE1 PDT_PBYTE2 PDT_PBYTE3 PDT_PBYTE4
%token PDT_FLOAT2H
%token PDT_NORMUBYTE4 PDT_NORMUSHORT2 PDT_NORMUSHORT4
%token PDT_UDEC3 PDT_NORMDEC3
%token PDT_FLOAT16_2 PDT_FLOAT16_4

%token PDTESS_DEFAULT
%token PDTESS_PARTIALU PDTESS_PARTIALV
%token PDTESS_CROSSUV PDTESS_UV
%token PDTESS_LOOKUP PDTESS_LOOKUPPRESAMPLED

%token PDU_POSITION
%token PDU_BLENDWEIGHT PDU_BLENDINDICES
%token PDU_NORMAL
%token PDU_PSIZE
%token PDU_TEXCOORD
%token PDU_TANGENT PDU_BINORMAL
%token PDU_TESSFACTOR
%token PDU_POSITIONT
%token PDU_COLOR
%token PDU_FOG
%token PDU_DEPTH
%token PDU_SAMPLE

%token RENDERSTATES

%token CMDEFINED CMATTRIBUTE CMCONSTANT CMGLOBAL CMOPERATOR
%token VSCONSTANTMAP VSPROGRAM
%token GSCONSTANTMAP GSPROGRAM
%token PSCONSTANTMAP PSPROGRAM
%token PROGRAM ENTRYPOINT SHADERTARGET SOFTWAREVP
%token SKINBONEMATRIX3

%token REQUIREMENTS
%token VSVERSION
%token GSVERSION
%token PSVERSION
%token USERVERSION
%token PLATFORM
%token BONESPERPARTITION
%token BINORMALTANGENTMETHOD
%token BINORMALTANGENTUVSOURCE
%token NBTMETHOD_NONE
%token NBTMETHOD_NI
%token NBTMETHOD_MAX
%token NBTMETHOD_ATI

%token USERDEFINEDDATA

%token IMPLEMENTATION

%token OUTPUTSTREAM

%token STREAMOUTPUT
%token STREAMOUTTARGETS
%token STREAMOUTAPPEND

%token MAXVERTEXCOUNT
%token OUTPUTPRIMTYPE
%token _POINT
%token _LINE
%token _TRIANGLE
%token VERTEXFORMAT
%token FMT_FLOAT
%token FMT_INT
%token FMT_UINT

%token CLASSNAME

%token PASS

%token STAGE
%token TSS_TEXTURE
%token TSS_COLOROP 
%token TSS_COLORARG0
%token TSS_COLORARG1
%token TSS_COLORARG2
%token TSS_ALPHAOP
%token TSS_ALPHAARG0
%token TSS_ALPHAARG1
%token TSS_ALPHAARG2
%token TSS_RESULTARG
%token TSS_CONSTANT_DEPRECATED
%token TSS_BUMPENVMAT00
%token TSS_BUMPENVMAT01
%token TSS_BUMPENVMAT10
%token TSS_BUMPENVMAT11
%token TSS_BUMPENVLSCALE
%token TSS_BUMPENVLOFFSET
%token TSS_TEXCOORDINDEX
%token TSS_TEXTURETRANSFORMFLAGS
%token TSS_TEXTRANSMATRIX

%token TTFF_DISABLE 
%token TTFF_COUNT1 TTFF_COUNT2 TTFF_COUNT3 TTFF_COUNT4
%token TTFF_PROJECTED

%token PROJECTED
%token USEMAPINDEX
%token INVERSE TRANSPOSE

%token TTSRC_GLOBAL
%token TTSRC_CONSTANT

%token TT_WORLD_PARALLEL
%token TT_WORLD_PERSPECTIVE
%token TT_WORLD_SPHERE_MAP
%token TT_CAMERA_SPHERE_MAP
%token TT_SPECULAR_CUBE_MAP
%token TT_DIFFUSE_CUBE_MAP

%token TCI_PASSTHRU
%token TCI_CAMERASPACENORMAL
%token TCI_CAMERASPACEPOSITION
%token TCI_CAMERASPACEREFLECT 
%token TCI_SPHEREMAP

%token TOP_DISABLE
%token TOP_SELECTARG1
%token TOP_SELECTARG2
%token TOP_MODULATE
%token TOP_MODULATE2X
%token TOP_MODULATE4X
%token TOP_ADD
%token TOP_ADDSIGNED
%token TOP_ADDSIGNED2X
%token TOP_SUBTRACT
%token TOP_ADDSMOOTH
%token TOP_BLENDDIFFUSEALPHA
%token TOP_BLENDTEXTUREALPHA
%token TOP_BLENDFACTORALPHA
%token TOP_BLENDTEXTUREALPHAPM
%token TOP_BLENDCURRENTALPHA
%token TOP_PREMODULATE
%token TOP_MODULATEALPHA_ADDCOLOR
%token TOP_MODULATECOLOR_ADDALPHA
%token TOP_MODULATEINVALPHA_ADDCOLOR
%token TOP_MODULATEINVCOLOR_ADDALPHA
%token TOP_BUMPENVMAP
%token TOP_BUMPENVMAPLUMINANCE
%token TOP_DOTPRODUCT3
%token TOP_MULTIPLYADD
%token TOP_LERP

%token TA_CURRENT
%token TA_DIFFUSE
%token TA_SELECTMASK
%token TA_SPECULAR
%token TA_TEMP
%token TA_TEXTURE
%token TA_TFACTOR
%token TA_ALPHAREPLICATE
%token TA_COMPLEMENT

%token NTM_BASE NTM_DARK NTM_DETAIL NTM_GLOSS NTM_GLOW NTM_BUMP NTM_NORMAL NTM_PARALLAX NTM_DECAL

%token SAMPLER
%token TSAMP_ADDRESSU TSAMP_ADDRESSV TSAMP_ADDRESSW
%token TSAMP_BORDERCOLOR
%token TSAMP_MAGFILTER TSAMP_MINFILTER TSAMP_MIPFILTER
%token TSAMP_MIPMAPLODBIAS TSAMP_MAXMIPLEVEL TSAMP_MAXANISOTROPY
%token TSAMP_SRGBTEXTURE TSAMP_ELEMENTINDEX TSAMP_DMAPOFFSET
%token TSAMP_ALPHAKILL_DEPRECATED TSAMP_COLORKEYOP_DEPRECATED
%token TSAMP_COLORSIGN_DEPRECATED TSAMP_COLORKEYCOLOR_DEPRECATED

%token TADDR_WRAP TADDR_MIRROR TADDR_CLAMP
%token TADDR_BORDER TADDR_MIRRORONCE 
%token TADDR_CLAMPTOEDGE_DEPRECATED

%token TEXF_NONE TEXF_POINT TEXF_LINEAR TEXF_ANISOTROPIC
%token TEXF_PYRAMIDALQUAD TEXF_GAUSSIANQUAD 
%token TEXF_FLATCUBIC_DEPRECATED TEXF_GAUSSIANCUBIC_DEPRECATED 
%token TEXF_QUINCUNX_DEPRECATED TEXF_MAX_DEPRECATED

%token TAK_DISABLE_DEPRECATED TAK_ENABLE_DEPRECATED

%token TCKOP_DISABLE_DEPRECATED TCKOP_ALPHA_DEPRECATED 
%token TCKOP_RGBA_DEPRECATED TCKOP_KILL_DEPRECATED

%token TOKEN_TEXTURE
%token TEXTURE_SOURCE

%token OBJECTS
%token EFFECT_GENERALLIGHT EFFECT_POINTLIGHT EFFECT_DIRECTIONALLIGHT
%token EFFECT_SPOTLIGHT EFFECT_SHADOWPOINTLIGHT EFFECT_SHADOWDIRECTIONALLIGHT
%token EFFECT_SHADOWSPOTLIGHT EFFECT EFFECT_ENVIRONMENTMAP EFFECT_PROJECTEDSHADOWMAP
%token EFFECT_PROJECTEDLIGHTMAP EFFECT_FOGMAP
%token USEMAPVALUE
%token CMOBJECT


//note: %type declarations are only needed if the value is actually used 
//in the C++ code blocks (resulting from rule reductions) below.

%type    <sval>    optional_string
%type    <sval>    optional_description
%type    <ival>    optional_multiplicity
%type    <sval>    optional_filename
%type    <sval>    optional_multi_string
%type    <sval>    string_or_quote

%type    <sval>    attribute_name

%type    <bval>    artist_conditional

%type    <ival>    packing_definition_parameter
%type    <ival>    packing_definition_tesselator
%type    <ival>    packing_definition_usage
%type    <ival>    packing_definition_type

%type    <ival>    stage_texture_operation
%type    <ival>    stage_texture_argument
%type    <ival>    stage_texture_argument_modifiers_optional
%type    <ival>    stage_texture_argument_modifier
%type    <ival>    stage_texcoordindex_flags
%type    <ival>    stage_texturetransformflags_count 
%type    <ival>    stage_texturetransformflags_optional_projection

%type    <ival>    stage_texture_map_ndl
%type    <ival>    stage_texture_map_ndl_decal

%type    <ival>    sampler_texture_address
%type    <ival>    sampler_texture_filter
%type    <ival>    sampler_texture_alphakill
%type    <ival>    sampler_texture_colorkeyop

%type    <ival>    stage_textransmatrix_option

%type    <ival>    requirement_platform_list
%type    <ival>    binormaltanget_method

%type    <ival>    operator_type
%type    <bval>    operator_optional_inverse
%type    <bval>    operator_optional_transpose

%type    <sval>    vertexshader_program_name_only
%type    <sval>    geometryshader_program_name_only
%type    <sval>    pixelshader_program_name_only

%%

//---------------------------------------------------------------------------
// Shader file general
//---------------------------------------------------------------------------
shader_file:
        shader_file    shader
    |   shader
    ;

shader:
        NSFSHADER 
        N_STRING
        L_ACCOLADE
            {
                g_bGlobalAttributes = false;
                g_pkCurrAttribTable = 0;
                g_pkCurrObjectTable = 0;
                g_uiCurrPDStream = 0;    
                g_bCurrPDFixedFunction = false;
                g_pkCurrPackingDef = 0;
                g_pkCurrRequirements = 0;
                g_uiCurrImplementation = 0;
                g_pkCurrImplementation = 0;
                g_pkCurrRSGroup = 0;
                g_uiCurrImplemVSConstantMap = 0;
                g_uiCurrImplemGSConstantMap = 0;
                g_uiCurrImplemPSConstantMap = 0;
                g_uiCurrPassVSConstantMap = 0;
                g_uiCurrPassGSConstantMap = 0;
                g_uiCurrPassPSConstantMap = 0;
                g_pkCurrConstantMap = 0;
                g_uiCurrPassIndex = 0;
                g_pkCurrPass = 0;
                g_pkCurrTextureStage = 0;
                g_uiCurrTextureSlot = 0;
                g_pkCurrTexture = 0;
                
                g_pkCurrShader = NiNew NSFParsedShader();
                if (g_pkCurrShader)
                {
                    g_kParsedShaderList.AddTail(g_pkCurrShader);
                    g_pkCurrNSBShader = g_pkCurrShader->GetShader();
                    g_pkCurrNSBShader->SetName($2);
                    g_uiCurrImplementation = 0;
                }
                else
                {
                    DebugStringOut("Failed to create NSFParsedShader!\n");
                }
                
                g_iDSOIndent = 0;
                NiSprintf(g_szDSO, 1024, "\nNSF Shader - %s\n",$2);
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
            }
            optional_description
                {
                    NiSprintf(g_szDSO, 1024, "Description: %s\n", $5);
                    DebugStringOut(g_szDSO);
                    
                    if (g_pkCurrNSBShader)
                        g_pkCurrNSBShader->SetDescription($5);
                        
                    NiFree($5);
                }
            nsfshader_components_optional
        R_ACCOLADE
            {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Completed NSF Shader - %s\n",$2);
                DebugStringOut(g_szDSO);

                g_pkCurrShader = 0;
                g_pkCurrNSBShader = 0;
                
                NiFree($2);
            }
    |   error    '\n'
            {
                NSFParsererror("Syntax Error: shader");
                yyclearin;
            }
    ;

nsfshader_components_optional:
        /* empty */
    |   nsfshader_components_list
    ;
    
nsfshader_components_list:
        nsfshader_components_list    nsfshader_component
    |   nsfshader_component
    ;
    
nsfshader_component:
        attribute_list_with_brackets
    |   object_list_with_brackets
    |   global_attribute_list_with_brackets
    |   packing_definition_definition
    |   implementation
    |   outputstream
    |   userdefineddata_block
    ;
    
//---------------------------------------------------------------------------
// String helper functions
//---------------------------------------------------------------------------
optional_string:
        /* empty */     {   $$ = 0;    }
    |   N_STRING        {   $$ = $1;    }
    ;

optional_multi_string:
        N_QUOTE
            {
                $$ = $1;
            }
    |   optional_multi_string N_QUOTE
            {
                // Assumes $1 is a null-terminated string
                NIASSERT($1);
                size_t stLen = strlen($1) + 1;
                if (stLen < MAX_QUOTE_LENGTH)
                    stLen = MAX_QUOTE_LENGTH;
                $$ = NiStrcat($1, stLen, $2);
                NiFree($2);
            }
    ;
    
optional_description:
        /* empty */             {   $$ = 0;    }
    |   optional_multi_string   {   $$ = $1;    }
    ;

optional_filename:
        /* empty */     {   $$ = 0;    }
    |   N_QUOTE         {   $$ = $1;    }
    |   PATH            {   $$ = $1;    }
    ;

string_or_quote:
        N_STRING        {   $$ = $1;    }
    |   N_QUOTE         {   $$ = $1;    }
    ;

attribute_name:
        L_ANGLEBRACKET    N_STRING    R_ANGLEBRACKET    {   $$ = $2;    }
    ;

//---------------------------------------------------------------------------
// Range helper functions
//---------------------------------------------------------------------------
range_color_optional:
        /* empty */
            {
                g_bRanged = false;
            }
    |   L_BRACKET    
        L_PARENTHESE 
            N_FLOAT COMMA   N_FLOAT COMMA   N_FLOAT COMMA   N_FLOAT    
        R_PARENTHESE    
        L_PARENTHESE 
            N_FLOAT COMMA   N_FLOAT COMMA   N_FLOAT COMMA   N_FLOAT    
        R_PARENTHESE    
        R_BRACKET
            {
                g_bRanged    = true;
                AddFloatToLowArray($3);
                AddFloatToLowArray($5);
                AddFloatToLowArray($7);
                AddFloatToLowArray($9);
                AddFloatToHighArray($12);
                AddFloatToHighArray($14);
                AddFloatToHighArray($16);
                AddFloatToHighArray($18);
            }
    |   L_BRACKET    
        L_PARENTHESE 
            N_FLOAT    COMMA    N_FLOAT    COMMA    N_FLOAT    
        R_PARENTHESE
        L_PARENTHESE
            N_FLOAT    COMMA    N_FLOAT    COMMA    N_FLOAT    
        R_PARENTHESE
        R_BRACKET
            {
                g_bRanged    = true;
                AddFloatToLowArray($3);
                AddFloatToLowArray($5);
                AddFloatToLowArray($7);
                AddFloatToHighArray($10);
                AddFloatToHighArray($12);
                AddFloatToHighArray($14);
            }
    ;
    
range_point4_optional:
        /* empty */
            {
                g_bRanged = false;
            }
    |   L_BRACKET    
        L_PARENTHESE 
            N_FLOAT COMMA   N_FLOAT COMMA   N_FLOAT COMMA   N_FLOAT    
        R_PARENTHESE    
        L_PARENTHESE 
            N_FLOAT COMMA   N_FLOAT COMMA   N_FLOAT COMMA   N_FLOAT    
        R_PARENTHESE    
        R_BRACKET
            {
                g_bRanged    = true;
                AddFloatToLowArray($3);
                AddFloatToLowArray($5);
                AddFloatToLowArray($7);
                AddFloatToLowArray($9);
                AddFloatToHighArray($12);
                AddFloatToHighArray($14);
                AddFloatToHighArray($16);
                AddFloatToHighArray($18);
            }
    ;
    
range_point3_optional:
        /* empty */
            {
                g_bRanged = false;
            }
    |   L_BRACKET    
        L_PARENTHESE 
            N_FLOAT    COMMA    N_FLOAT    COMMA    N_FLOAT    
        R_PARENTHESE    
        L_PARENTHESE 
            N_FLOAT    COMMA    N_FLOAT    COMMA    N_FLOAT    
        R_PARENTHESE    
        R_BRACKET
            {
                g_bRanged    = true;
                AddFloatToLowArray($3);
                AddFloatToLowArray($5);
                AddFloatToLowArray($7);
                AddFloatToHighArray($10);
                AddFloatToHighArray($12);
                AddFloatToHighArray($14);
            }
    ;
    
range_point2_optional:
        /* empty */
            {
                g_bRanged = false;
            }
    |   L_BRACKET    
        L_PARENTHESE N_FLOAT    COMMA    N_FLOAT    R_PARENTHESE    
        L_PARENTHESE N_FLOAT    COMMA    N_FLOAT    R_PARENTHESE    
        R_BRACKET
            {
                g_bRanged    = true;
                AddFloatToLowArray($3);
                AddFloatToLowArray($5);
                AddFloatToHighArray($8);
                AddFloatToHighArray($10);
            }
    ;
    
range_float_optional:
        /* empty */
            {
                g_bRanged = false;
            }
    |   L_BRACKET    N_FLOAT    N_FLOAT    R_BRACKET
            {
                g_bRanged    = true;
                AddFloatToLowArray($2);
                AddFloatToHighArray($3);
            }
    ;
    
range_int_optional:
        /* empty */
            {
                g_bRanged = false;
            }
    |   L_BRACKET    N_INT    N_INT    R_BRACKET
            {
                g_bRanged    = true;
                AddFloatToLowArray((float)$2);
                AddFloatToHighArray((float)$3);
            }
    |   L_BRACKET    N_FLOAT    N_FLOAT    R_BRACKET
            {
                // Allow floats for backwards compatibility
                g_bRanged    = true;
                AddFloatToLowArray($2);
                AddFloatToHighArray($3);
            }
    ;
    
//---------------------------------------------------------------------------
// Float value helper functions
//---------------------------------------------------------------------------
    
float_values_arbitrary_list:
        float_values_1
    |   float_values_1 COMMA float_values_arbitrary_list
    ;

float_values_1:
        N_FLOAT
            {
                AddFloatToValueArray($1);
            }
    ;
    
float_values_16:
        N_FLOAT COMMA    N_FLOAT COMMA    N_FLOAT COMMA    N_FLOAT COMMA    
        N_FLOAT COMMA    N_FLOAT COMMA    N_FLOAT COMMA    N_FLOAT COMMA    
        N_FLOAT COMMA    N_FLOAT COMMA    N_FLOAT COMMA    N_FLOAT COMMA    
        N_FLOAT COMMA    N_FLOAT COMMA    N_FLOAT COMMA    N_FLOAT 
            {
                AddFloatToValueArray($1);
                AddFloatToValueArray($3);
                AddFloatToValueArray($5);
                AddFloatToValueArray($7);
                AddFloatToValueArray($9);
                AddFloatToValueArray($11);
                AddFloatToValueArray($13);
                AddFloatToValueArray($15);
                AddFloatToValueArray($17);
                AddFloatToValueArray($19);
                AddFloatToValueArray($21);
                AddFloatToValueArray($23);
                AddFloatToValueArray($25);
                AddFloatToValueArray($27);
                AddFloatToValueArray($29);
                AddFloatToValueArray($31);
            }
    ;


//---------------------------------------------------------------------------
// Attributes
//---------------------------------------------------------------------------
// Represents a complete attribute declaration section
attribute_list_with_brackets:
        ATTRIBUTES 
        L_ACCOLADE
            {
                NiSprintf(g_szDSO, 1024, "Attribute Table Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrNSBShader)
                {
                    g_bGlobalAttributes = false;
                    g_pkCurrAttribTable = 
                        g_pkCurrNSBShader->GetAttributeTable();
                }
                else
                {
                    g_pkCurrAttribTable = 0;
                }
            }
            attribute_list
        R_ACCOLADE
            {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Attribute Table End\n");
                DebugStringOut(g_szDSO);
                g_pkCurrAttribTable = 0;
            }
    ;

global_attribute_list_with_brackets:
        GLOBALATTRIBUTES 
        L_ACCOLADE
            {
                NiSprintf(g_szDSO, 1024, "Global Attribute Table Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrNSBShader)
                {
                    g_bGlobalAttributes = true;
                    g_pkCurrAttribTable = 
                        g_pkCurrNSBShader->GetGlobalAttributeTable();
                }
                else
                {
                    g_pkCurrAttribTable = 0;
                }
            }
            attribute_list
        R_ACCOLADE
            {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Global Attribute Table End\n");
                DebugStringOut(g_szDSO);
                g_pkCurrAttribTable = 0;
            }
    ;

// allow array attributes
optional_multiplicity:
    /* empty */
    {
        $$ = 0;
    }
    |
    L_BRACKET N_INT R_BRACKET
    {
        $$ = $2;
    }
    ;

// List of attributes    
attribute_list:
        attribute_list    attribute_value
    |   attribute_value
    ;

// Individual attribute value    
attribute_value:
        attribute_bool
    |   attribute_uint
    |   attribute_float
    |   attribute_point2
    |   attribute_point3
    |   attribute_point4
    |   attribute_matrix3
    |   attribute_transform
    |   attribute_color
    |   attribute_texture
    ;

// Artist conditional - determines whether the artist can edit the
// attribute or not
artist_conditional:
        ARTIST      {   $$ = true;      }
    |   HIDDEN      {   $$ = false;     }
    ;

// Boolean attribute rule    
attribute_bool:
        ATTRIB_BOOL        
        N_STRING    
        artist_conditional    
        N_BOOL
            {
                if (g_pkCurrAttribTable)
                {
                    if (!g_pkCurrAttribTable->AddAttribDesc_Bool(
                        $2, 0, $3 ? false : true, 
                        $4))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    AddAttribDesc_Bool at line %d\n"
                            "    Desc name = %s\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), $2);
                    }
                }                
                NiSprintf(g_szDSO, 1024, "    Boolean: %16s - %6s - %s\n",
                    $2, $3 ? "ARTIST" : "HIDDEN", 
                    $4 ? "TRUE" : "FALSE");
                DebugStringOut(g_szDSO);

                NiFree($2);
            }
    ;

// unsigned int attribute rule    
attribute_uint:
        ATTRIB_UINT    
            {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            }
        N_STRING    
        artist_conditional    
        N_INT   
        range_int_optional
            {
                if (g_pkCurrAttribTable)
                {
                    if (g_bRanged && !g_bGlobalAttributes)
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_UnsignedInt(
                            $3, 0, $4 ? false : true, 
                            (unsigned int)$5,
                            (unsigned int)g_afLowValues[0], 
                            (unsigned int)g_afHighValues[0]))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_UnsignedInt at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), $3);
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_UnsignedInt(
                            $3, 0, $4 ? false : true, 
                            (unsigned int)$5))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_UnsignedInt at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), $3);
                        }
                    }
                }                

                NiSprintf(g_szDSO, 1024, "       uint: %16s - %6s - %d\n",
                    $3, $4 ? "ARTIST" : "HIDDEN", 
                    (int)$5);
                DebugStringOut(g_szDSO);
                if (g_bRanged && !g_bGlobalAttributes)
                {
                    NiSprintf(g_szDSO, 1024, "             Range: "
                        "[%4d..%4d]\n", 
                        (unsigned int)g_afLowValues[0], 
                        (unsigned int)g_afHighValues[0]);
                }

                NiFree($3);
            }
    ;

// float attribute rule    
attribute_float:
        ATTRIB_FLOAT    
            {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            }
        N_STRING    
        optional_multiplicity
        artist_conditional    
        float_values_arbitrary_list    
        range_float_optional
            {
                unsigned int uiExpectedLength = (($4 != 0) ? $4 : 1) * 1;
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < uiExpectedLength)
                {
                    // Pad out to the correct length with 0.0f's
                    for (; uiFoundLength < uiExpectedLength; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > uiExpectedLength)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected %d float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiExpectedLength, uiFoundLength);
                    
                    g_afValues->SetSize(uiExpectedLength);
                }

                if (g_pkCurrAttribTable)
                {
                    // test for array
                    if ($4)
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Array(
                            $3, 0, $5 ? false : true, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT,
                            $4,
                            g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Float at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), $3);
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Float(
                            $3, 0, $5 ? false : true, 
                            *(g_afValues->GetBase()),
                            g_afLowValues[0], g_afHighValues[0]))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Float at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), $3);
                        }
                    }
                }

                NiSprintf(g_szDSO, 1024, 
                    "      Float: %16s - %6s - %8.5f\n",
                    $3, $5 ? "ARTIST" : "HIDDEN", 
                    *(g_afValues->GetBase()));
                DebugStringOut(g_szDSO);

                if (g_bRanged && !g_bGlobalAttributes)
                {
                    NiSprintf(g_szDSO, 1024, "             Range: "
                        "[%8.5f..%8.5f]\n",
                        g_afLowValues[0], g_afHighValues[0]);
                    DebugStringOut(g_szDSO);
                }

                NiFree($3);
            }
    ;

// point2 attribute rule    
attribute_point2:
        ATTRIB_POINT2
            {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            }
        N_STRING    
        optional_multiplicity
        artist_conditional    
        float_values_arbitrary_list    
        range_point2_optional
            {
                unsigned int uiExpectedLength = (($4 != 0) ? $4 : 1) * 2;
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < uiExpectedLength)
                {
                    // Pad out to the correct length with 0.0f's
                    for (; uiFoundLength < uiExpectedLength; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > uiExpectedLength)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected %d float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiExpectedLength, uiFoundLength);
                    
                    g_afValues->SetSize(uiExpectedLength);
                }

                if (g_pkCurrAttribTable)
                {
                    // test for array
                    if ($4)
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Array(
                            $3, 0, $5 ? false : true, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_POINT2,
                            $4,
                            g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Point2 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), $3);
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Point2(
                            $3, 0, $5 ? false : true, 
                            g_afValues->GetBase(),
                            g_afLowValues, g_afHighValues))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Point2 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), $3);
                        }
                    }

                }

                NiSprintf(g_szDSO, 1024, "     Point2: %16s - %6s - "
                    "%8.5f,%8.5f\n",
                    $3, $5 ? "ARTIST" : "HIDDEN", 
                    g_afValues->GetAt(0), g_afValues->GetAt(1));
                DebugStringOut(g_szDSO);
                if (g_bRanged && !g_bGlobalAttributes)
                {
                    NiSprintf(g_szDSO, 1024, "             Range: "
                        "[(%8.5f,%8.5f)..(%8.5f,%8.5f)]\n",
                        g_afLowValues[0], g_afLowValues[1],
                        g_afHighValues[0], g_afHighValues[1]);
                    DebugStringOut(g_szDSO);
                }

                NiFree($3);
            }
    ;
    
// point3 attribute rule    
attribute_point3:
        ATTRIB_POINT3
            {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            }
        N_STRING    
        optional_multiplicity
        artist_conditional    
        float_values_arbitrary_list    
        range_point3_optional
            {
                unsigned int uiExpectedLength = (($4 != 0) ? $4 : 1) * 3;
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < uiExpectedLength)
                {
                    // Pad out to the correct length with 0.0f's
                    for (; uiFoundLength < uiExpectedLength; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > uiExpectedLength)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected %d float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiExpectedLength, uiFoundLength);
                    
                    g_afValues->SetSize(uiExpectedLength);
                }

                if (g_pkCurrAttribTable)
                {
                    if ($4)
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Array(
                            $3, 0, $5 ? false : true, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_POINT3,
                            $4,
                            g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Point3 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), $3);
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Point3(
                            $3, 0, $5 ? false : true, 
                            g_afValues->GetBase(),
                            g_afLowValues, g_afHighValues))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Point3 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), $3);
                        }
                    }
                }
            
                NiSprintf(g_szDSO, 1024, "     Point3: %16s - %6s - "
                    "%8.5f,%8.5f,%8.5f\n",
                    $3, $5 ? "ARTIST" : "HIDDEN", 
                    g_afValues->GetAt(0),
                    g_afValues->GetAt(1),
                    g_afValues->GetAt(2));
                DebugStringOut(g_szDSO);
                if (g_bRanged && !g_bGlobalAttributes)
                {
                    NiSprintf(g_szDSO, 1024, "             Range: "
                        "[(%8.5f,%8.5f,%8.5f)..(%8.5f,%8.5f,%8.5f)]"
                        "\n",
                        g_afLowValues[0], g_afLowValues[1], 
                        g_afLowValues[2], g_afHighValues[0],
                        g_afHighValues[1], g_afHighValues[2]);
                    DebugStringOut(g_szDSO);
                }

                NiFree($3);
            }
    ;
    
// point4 attribute rule    
attribute_point4:
        ATTRIB_POINT4
            {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            }
        N_STRING    
        optional_multiplicity
        artist_conditional    
        float_values_arbitrary_list    
        range_point4_optional
            {
                unsigned int uiExpectedLength = (($4 != 0) ? $4 : 1) * 4;
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < uiExpectedLength)
                {
                    // Pad out to the correct length with 0.0f's
                    for (; uiFoundLength < uiExpectedLength; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > uiExpectedLength)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected %d float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiExpectedLength, uiFoundLength);
                    
                    g_afValues->SetSize(uiExpectedLength);
                }

                if (g_pkCurrAttribTable)
                {
                    // test for array
                    if ($4)
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Array(
                            $3, 0, $5 ? false : true, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_POINT4,
                            $4,
                            g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Point4 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), $3);
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Point4(
                            $3, 0, $5 ? false : true, 
                            g_afValues->GetBase(),
                            g_afLowValues, g_afHighValues))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Point4 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), $3);
                        }
                    }

                }

                NiSprintf(g_szDSO, 1024, "     Point4: %16s - %6s - "
                    "%8.5f,%8.5f,%8.5f,%8.5f\n",
                    $3, $5 ? "ARTIST" : "HIDDEN", 
                    g_afValues->GetAt(0), g_afValues->GetAt(1), 
                    g_afValues->GetAt(2), g_afValues->GetAt(3));
                DebugStringOut(g_szDSO);
                if (g_bRanged && !g_bGlobalAttributes)
                {
                    NiSprintf(g_szDSO, 1024, "             Range: "
                        "[(%8.5f,%8.5f,%8.5f,%8.5f).."
                        "(%8.5f,%8.5f,%8.5f,%8.5f)]\n",
                        g_afLowValues[0], g_afLowValues[1], 
                        g_afLowValues[2], g_afLowValues[3],
                        g_afHighValues[0], g_afHighValues[1], 
                        g_afHighValues[2], g_afHighValues[3]);
                    DebugStringOut(g_szDSO);
                }

                NiFree($3);
            }
    ;

// matrix3 attribute rule    
attribute_matrix3:
        ATTRIB_MATRIX3
            {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            }
        N_STRING    
        optional_multiplicity
        artist_conditional    
        float_values_arbitrary_list
            {
                unsigned int uiExpectedLength = (($4 != 0) ? $4 : 1) * 9;
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < uiExpectedLength)
                {
                    // Pad out to the correct length with 0.0f's
                    for (; uiFoundLength < uiExpectedLength; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > uiExpectedLength)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected %d float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiExpectedLength, uiFoundLength);
                    
                    g_afValues->SetSize(uiExpectedLength);
                }

                if (g_pkCurrAttribTable)
                {
                    if ($4)
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Array(
                            $3, 0, $5 ? false : true, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3,
                            $4,
                            g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Matrix3 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), $3);
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Matrix3(
                                $3, 0, $5 ? false : true, 
                                g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Matrix3 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), $3);
                        }
                    }

                }

                NiSprintf(g_szDSO, 1024, 
                    "    Matrix3: %16s - %6s - %8.5f,%8.5f,%8.5f\n"
                    "             %16s   %6s   %8.5f,%8.5f,%8.5f\n"
                    "             %16s   %6s   %8.5f,%8.5f,%8.5f\n",
                    $3, $5 ? "ARTIST" : "HIDDEN", 
                    g_afValues->GetAt(0),
                    g_afValues->GetAt(1),
                    g_afValues->GetAt(2),
                    " ", " ",
                    g_afValues->GetAt(3),
                    g_afValues->GetAt(4),
                    g_afValues->GetAt(5),
                    " ", " ",
                    g_afValues->GetAt(6),
                    g_afValues->GetAt(7),
                    g_afValues->GetAt(8));
                DebugStringOut(g_szDSO);

                NiFree($3);
            }
    ;

// transform attribute rule    
attribute_transform:
        ATTRIB_TRANSFORM
            {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            }
        N_STRING    
        optional_multiplicity
        artist_conditional    
        float_values_arbitrary_list
            {
                unsigned int uiExpectedLength = (($4 != 0) ? $4 : 1) * 16;
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < uiExpectedLength)
                {
                    // Pad out to the correct length with 0.0f's
                    for (; uiFoundLength < uiExpectedLength; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > uiExpectedLength)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected %d float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiExpectedLength, uiFoundLength);
                    
                    g_afValues->SetSize(uiExpectedLength);
                }

                if (g_pkCurrAttribTable)
                {
                    if ($4)
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Array(
                            $3, 0, $5 ? false : true, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4,
                            $4,
                            g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Matrix4 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), $3);
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Matrix4(
                                $3, 0, $5 ? false : true, 
                                g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Matrix4 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), $3);
                        }
                    }
                }                

                NiSprintf(g_szDSO, 1024, 
                    "  Transform: %16s - %6s - %8.5f,%8.5f,%8.5f,%8.5f\n"
                    "             %16s   %6s   %8.5f,%8.5f,%8.5f,%8.5f\n"
                    "             %16s   %6s   %8.5f,%8.5f,%8.5f,%8.5f\n"
                    "             %16s   %6s   %8.5f,%8.5f,%8.5f,%8.5f\n",
                    $3, $5 ? "ARTIST" : "HIDDEN", 
                            g_afValues->GetAt( 0), g_afValues->GetAt( 1), 
                            g_afValues->GetAt( 2), g_afValues->GetAt( 3),
                    " ", " ", g_afValues->GetAt( 4), g_afValues->GetAt( 5),
                            g_afValues->GetAt( 6), g_afValues->GetAt( 7),
                    " ", " ", g_afValues->GetAt( 8), g_afValues->GetAt( 9),
                            g_afValues->GetAt(10), g_afValues->GetAt(11),
                    " ", " ", g_afValues->GetAt(12), g_afValues->GetAt(13),
                            g_afValues->GetAt(14), g_afValues->GetAt(15));
                DebugStringOut(g_szDSO);

                NiFree($3);
            }
    ;

// color attribute rule    
attribute_color:
        ATTRIB_COLOR
            {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            }
        N_STRING    
        artist_conditional    
        float_values_arbitrary_list
        range_color_optional
            {
                // This one is a bit special, the expected length is 3 *or* 4
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < 3)
                {
                    // Pad out to 3 if too short (with 0.0f's)
                    for (; uiFoundLength < 3; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > 4)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected 3 or 4 float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiFoundLength);

                    g_afValues->SetSize(4);
                }

                if (g_afValues->GetSize() == 3)
                {
                    if (g_pkCurrAttribTable)
                    {
                        if (g_bRanged && !g_bGlobalAttributes)
                        {
                            if (!g_pkCurrAttribTable->AddAttribDesc_Color(
                                $3, 0, 
                                $4 ? false : true, 
                                g_afValues->GetBase(),
                                g_afLowValues, g_afHighValues))
                            {
                                NiShaderFactory::ReportError(
                                    NISHADERERR_UNKNOWN, 
                                    true, "* PARSE ERROR: %s\n"
                                    "    AddAttribDesc_Color at line %d\n"
                                    "    Desc name = %s\n",
                                    g_pkFile->GetFilename(), 
                                    NSFParserGetLineNumber(), $3);
                            }
                        }
                        else
                        {
                            if (!g_pkCurrAttribTable->AddAttribDesc_Color(
                                $3, 0, 
                                $4 ? false : true, 
                                g_afValues->GetBase(),
                                g_afLowValues, g_afHighValues))
                            {
                                NiShaderFactory::ReportError(
                                    NISHADERERR_UNKNOWN, 
                                    true, "* PARSE ERROR: %s\n"
                                    "    AddAttribDesc_Color at line %d\n"
                                    "    Desc name = %s\n",
                                    g_pkFile->GetFilename(), 
                                    NSFParserGetLineNumber(), $3);
                            }
                        }
                    }                

                    NiSprintf(g_szDSO, 1024, "      Color: %16s - %6s - "
                        "%8.5f,%8.5f,%8.5f\n",
                        $3, $4 ? "ARTIST" : "HIDDEN", 
                        g_afValues->GetAt(0),
                        g_afValues->GetAt(1),
                        g_afValues->GetAt(2));
                    DebugStringOut(g_szDSO);
                    if (g_bRanged && !g_bGlobalAttributes)
                    {
                        NiSprintf(g_szDSO, 1024, "             Range: "
                            "[(%8.5f,%8.5f,%8.5f).."
                            "(%8.5f,%8.5f,%8.5f)]\n",
                            g_afLowValues[0], g_afLowValues[1], 
                            g_afLowValues[2],
                            g_afHighValues[0], g_afHighValues[1], 
                            g_afHighValues[2]);
                        DebugStringOut(g_szDSO);
                    }
                }
                else
                {
                    if (g_pkCurrAttribTable)
                    {
                        if (g_bRanged && !g_bGlobalAttributes)
                        {
                            if (!g_pkCurrAttribTable->AddAttribDesc_ColorA(
                                $3, 0, 
                                $4 ? false : true, 
                                g_afValues->GetBase(),
                                g_afLowValues, g_afHighValues))
                            {
                                NiShaderFactory::ReportError(
                                    NISHADERERR_UNKNOWN, 
                                    true, "* PARSE ERROR: %s\n"
                                    "    AddAttribDesc_ColorA at line %d\n"
                                    "    Desc name = %s\n",
                                    g_pkFile->GetFilename(), 
                                    NSFParserGetLineNumber(), $3);
                            }
                        }
                        else
                        {
                            if (!g_pkCurrAttribTable->AddAttribDesc_ColorA(
                                $3, 0, 
                                $4 ? false : true, 
                                g_afValues->GetBase(),
                                g_afLowValues, g_afHighValues))
                            {
                                NiShaderFactory::ReportError(
                                    NISHADERERR_UNKNOWN, 
                                    true, "* PARSE ERROR: %s\n"
                                    "    AddAttribDesc_ColorA at line %d\n"
                                    "    Desc name = %s\n",
                                    g_pkFile->GetFilename(), 
                                    NSFParserGetLineNumber(), $3);
                            }
                        }
                    }                

                    NiSprintf(g_szDSO, 1024, "     ColorA: %16s - %6s - "
                        "%8.5f,%8.5f,%8.5f,%8.5f\n",
                        $3, $4 ? "ARTIST" : "HIDDEN", 
                        g_afValues->GetAt(0), g_afValues->GetAt(1), 
                        g_afValues->GetAt(2), g_afValues->GetAt(3));
                    DebugStringOut(g_szDSO);
                    if (g_bRanged && !g_bGlobalAttributes)
                    {
                        NiSprintf(g_szDSO, 1024, "             Range: "
                            "[(%8.5f,%8.5f,%8.5f,%8.5f).."
                            "(%8.5f,%8.5f,%8.5f,%8.5f)]\n",
                            g_afLowValues[0], g_afLowValues[1], 
                            g_afLowValues[2], g_afLowValues[3],
                            g_afHighValues[0], g_afHighValues[1], 
                            g_afHighValues[2], g_afHighValues[3]);
                        DebugStringOut(g_szDSO);
                    }
                }

                NiFree($3);
            }
    ;

// Attribute Texture Rule
attribute_texture:
        ATTRIB_TEXTURE N_STRING artist_conditional N_INT optional_filename
            {
                if (g_pkCurrAttribTable)
                {
                    if (!g_pkCurrAttribTable->AddAttribDesc_Texture(
                        $2, 0, $3 ? false : true, 
                        (unsigned int)$4, $5))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    AddAttribDesc_Texture at line %d\n"
                            "    Desc name = %s\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), $3);
                    }
                }                
                NiSprintf(g_szDSO, 1024, "    Texture: %16s - %6s - Slot %d\n",
                    $2, $3 ? "ARTIST" : "HIDDEN", 
                    (int)$4);
                DebugStringOut(g_szDSO);

                NiFree($2);
                NiFree($5);
            }
    |       ATTRIB_TEXTURE N_STRING artist_conditional optional_filename
            {
                if (g_pkCurrAttribTable)
                {
                    if (!g_pkCurrAttribTable->AddAttribDesc_Texture(
                        $2, 0, $3 ? false : true, 
                        g_uiCurrTextureSlot, $4))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    AddAttribDesc_Texture at line %d\n"
                            "    Desc name = %s\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), $3);
                    }
                }                
                NiSprintf(g_szDSO, 1024, "    Texture: %16s - %6s - Slot %d\n",
                    $2, $3 ? "ARTIST" : "HIDDEN", 
                    g_uiCurrTextureSlot);
                DebugStringOut(g_szDSO);

                NiFree($2);
                NiFree($4);
                    
                g_uiCurrTextureSlot++;
            }
    ;

//---------------------------------------------------------------------------
// Object Table
//---------------------------------------------------------------------------
// Represents a complete object declaration section
object_list_with_brackets:
        OBJECTS
        L_ACCOLADE
            {
                NiSprintf(g_szDSO, 1024, "Object Table Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                
                if (g_pkCurrNSBShader)
                {
                    g_pkCurrObjectTable = g_pkCurrNSBShader->GetObjectTable();
                }
                else
                {
                    g_pkCurrObjectTable = 0;
                }
            }
            object_list
        R_ACCOLADE
            {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Object Table End\n");
                DebugStringOut(g_szDSO);
                g_pkCurrObjectTable = 0;
            }
    ;

// List of objects
object_list:
        object_list     object_value
    |   object_value
    ;

// Individual object value
object_value:
        object_effect_general_light
    |   object_effect_point_light
    |   object_effect_directional_light
    |   object_effect_spot_light
    |   object_effect_shadow_point_light
    |   object_effect_shadow_directional_light
    |   object_effect_shadow_spot_light
    |   object_effect_environment_map
    |   object_effect_projected_shadow_map
    |   object_effect_projected_light_map
    |   object_effect_fog_map
    ;

// Effect_GeneralLight rule
object_effect_general_light:
        EFFECT_GENERALLIGHT
        N_INT
        N_STRING
            {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_GENERALLIGHT,
                    $2, $3, "Effect_GeneralLight");
                NiFree($3);
            }
    ;

// Effect_PointLight rule
object_effect_point_light:
        EFFECT_POINTLIGHT
        N_INT
        N_STRING
            {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_POINTLIGHT,
                    $2, $3, "Effect_PointLight");
                NiFree($3);
            }
    ;

// Effect_DirectionalLight rule
object_effect_directional_light:
        EFFECT_DIRECTIONALLIGHT
        N_INT
        N_STRING
            {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT,
                    $2, $3, "Effect_DirectionalLight");
                NiFree($3);
            }
    ;

// Effect_SpotLight rule
object_effect_spot_light:
        EFFECT_SPOTLIGHT
        N_INT
        N_STRING
            {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_SPOTLIGHT,
                    $2, $3, "Effect_SpotLight");
                NiFree($3);
            }
    ;

// Effect_ShadowPointLight rule
object_effect_shadow_point_light:
        EFFECT_SHADOWPOINTLIGHT
        N_INT
        N_STRING
            {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_SHADOWPOINTLIGHT,
                    $2, $3, "Effect_ShadowPointLight");
                NiFree($3);
            }
    ;

// Effect_ShadowDirectionalLight rule
object_effect_shadow_directional_light:
        EFFECT_SHADOWDIRECTIONALLIGHT
        N_INT
        N_STRING
            {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_SHADOWDIRECTIONALLIGHT,
                    $2, $3, "Effect_ShadowDirectionalLight");
                NiFree($3);
            }
    ;

// Effect_ShadowSpotLight rule
object_effect_shadow_spot_light:
        EFFECT_SHADOWSPOTLIGHT
        N_INT
        N_STRING
            {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_SHADOWSPOTLIGHT,
                    $2, $3, "Effect_ShadowSpotLight");
                NiFree($3);
            }
    ;

// Effect_EnvironmentMap rule
object_effect_environment_map:
        EFFECT_ENVIRONMENTMAP
        N_INT
        N_STRING
            {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP,
                    $2, $3, "Effect_EnvironmentMap");
                NiFree($3);
            }
    ;
    
// Effect_ProjectedShadowMap rule
object_effect_projected_shadow_map:
        EFFECT_PROJECTEDSHADOWMAP
        N_INT
        N_STRING
            {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_PROJECTEDSHADOWMAP,
                    $2, $3, "Effect_ProjectedShadowMap");
                NiFree($3);
            }
    ;

// Effect_ProjectedLightMap rule
object_effect_projected_light_map:
        EFFECT_PROJECTEDLIGHTMAP
        N_INT
        N_STRING
            {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_PROJECTEDLIGHTMAP,
                    $2, $3, "Effect_ProjectedLightMap");
                NiFree($3);
            }
    ;

// Effect_FogMap rule
object_effect_fog_map:
        EFFECT_FOGMAP
        N_INT
        N_STRING
            {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_FOGMAP,
                    $2, $3, "Effect_FogMap");
                NiFree($3);
            }
    ;

//---------------------------------------------------------------------------
// Packing Definitions
//---------------------------------------------------------------------------
packing_definition_definition:
        PACKINGDEF
        N_STRING
        L_ACCOLADE
            {
                if (g_pkCurrNSBShader)
                {
                    g_pkCurrPackingDef = 
                        g_pkCurrNSBShader->GetPackingDef($2, true);
                }
                else
                {
                    g_pkCurrPackingDef = 0;
                }

                g_bCurrPDFixedFunction = false;
                
                NiSprintf(g_szDSO, 1024, "PackingDefinition Start %s\n", 
                    $2);
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
            }
        packing_definition_entries
        R_ACCOLADE
            {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "PackingDefinition End %s\n", $2);
                DebugStringOut(g_szDSO);

                g_pkCurrPackingDef = 0;
                
                NiFree($2);
            }
    ;
    
packing_definition_declaration:
        PACKINGDEF
        N_STRING
            {
                NiSprintf(g_szDSO, 1024, "Using PackingDefinition %s\n",$2);
                DebugStringOut(g_szDSO);
                if (g_pkCurrNSBShader)
                {
                    NSBPackingDef* pkPackingDef = 
                        g_pkCurrNSBShader->GetPackingDef($2, false);
                    if (!pkPackingDef)
                    {
                        NiSprintf(g_szDSO, 1024, "    WARNING: PackingDefinition %s "
                            "NOT FOUND\n",$2);
                        DebugStringOut(g_szDSO);
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    PackingDefinition %s\n"
                            "    at line %d\n"
                            "    NOT FOUND!\n",
                            g_pkFile->GetFilename(), $2, 
                            NSFParserGetLineNumber());
                    }
                    else
                    {
                        if (g_pkCurrImplementation)
                        {
                            g_pkCurrImplementation->SetPackingDef(
                                $2);
                        }
                    }
                }

                NiFree($2);
            }
    ;

packing_definition_type:
        /* empty */     {   $$ = 0x7fffffff;                            }
    |   PDT_FLOAT1      {   $$ = NSBPackingDef::NID3DPDT_FLOAT1;        }
    |   PDT_FLOAT2      {   $$ = NSBPackingDef::NID3DPDT_FLOAT2;        }
    |   PDT_FLOAT3      {   $$ = NSBPackingDef::NID3DPDT_FLOAT3;        }
    |   PDT_FLOAT4      {   $$ = NSBPackingDef::NID3DPDT_FLOAT4;        }
    |   PDT_UBYTECOLOR  {   $$ = NSBPackingDef::NID3DPDT_UBYTECOLOR;    }
    |   PDT_SHORT1      {   $$ = NSBPackingDef::NID3DPDT_SHORT1;        }
    |   PDT_SHORT2      {   $$ = NSBPackingDef::NID3DPDT_SHORT2;        }
    |   PDT_SHORT3      {   $$ = NSBPackingDef::NID3DPDT_SHORT3;        }
    |   PDT_SHORT4      {   $$ = NSBPackingDef::NID3DPDT_SHORT4;        }
    |   PDT_UBYTE4      {   $$ = NSBPackingDef::NID3DPDT_UBYTE4;        }
    |   PDT_NORMSHORT1  {   $$ = NSBPackingDef::NID3DPDT_NORMSHORT1;    }
    |   PDT_NORMSHORT2  {   $$ = NSBPackingDef::NID3DPDT_NORMSHORT2;    }
    |   PDT_NORMSHORT3  {   $$ = NSBPackingDef::NID3DPDT_NORMSHORT3;    }
    |   PDT_NORMSHORT4  {   $$ = NSBPackingDef::NID3DPDT_NORMSHORT4;    }
    |   PDT_NORMPACKED3 {   $$ = NSBPackingDef::NID3DPDT_NORMPACKED3;   }
    |   PDT_PBYTE1      {   $$ = NSBPackingDef::NID3DPDT_PBYTE1;        }
    |   PDT_PBYTE2      {   $$ = NSBPackingDef::NID3DPDT_PBYTE2;        }
    |   PDT_PBYTE3      {   $$ = NSBPackingDef::NID3DPDT_PBYTE3;        }
    |   PDT_PBYTE4      {   $$ = NSBPackingDef::NID3DPDT_PBYTE4;        }
    |   PDT_FLOAT2H     {   $$ = NSBPackingDef::NID3DPDT_FLOAT2H;       }
    |   PDT_NORMUBYTE4  {   $$ = NSBPackingDef::NID3DPDT_NORMUBYTE4;    }
    |   PDT_NORMUSHORT2 {   $$ = NSBPackingDef::NID3DPDT_NORMUSHORT2;   }
    |   PDT_NORMUSHORT4 {   $$ = NSBPackingDef::NID3DPDT_NORMUSHORT4;   }
    |   PDT_UDEC3       {   $$ = NSBPackingDef::NID3DPDT_UDEC3;         }
    |   PDT_NORMDEC3    {   $$ = NSBPackingDef::NID3DPDT_NORMDEC3;      }
    |   PDT_FLOAT16_2   {   $$ = NSBPackingDef::NID3DPDT_FLOAT16_2;     }
    |   PDT_FLOAT16_4   {   $$ = NSBPackingDef::NID3DPDT_FLOAT16_4;     }
    |   error    '\n'
            {
                NSFParsererror("Syntax Error: packing_definition_type");
                yyclearin;
            }
    ;

packing_definition_parameter:
        PDP_POSITION        
            {   $$ = NiShaderDeclaration::SHADERPARAM_NI_POSITION0;       }                                
    |   PDP_POSITION0
            {   $$ = NiShaderDeclaration::SHADERPARAM_NI_POSITION0;       }                
    |   PDP_POSITION1
            {   $$ = NiShaderDeclaration::SHADERPARAM_NI_POSITION1;       }                
    |   PDP_POSITION2
            {   $$ = NiShaderDeclaration::SHADERPARAM_NI_POSITION2;       }
    |   PDP_POSITION3
            {   $$ = NiShaderDeclaration::SHADERPARAM_NI_POSITION3;       }
    |   PDP_POSITION4
            {   $$ = NiShaderDeclaration::SHADERPARAM_NI_POSITION4;       }
    |   PDP_POSITION5
            {   $$ = NiShaderDeclaration::SHADERPARAM_NI_POSITION5;       }
    |   PDP_POSITION6
            {   $$ = NiShaderDeclaration::SHADERPARAM_NI_POSITION6;       }
    |   PDP_POSITION7
            {   $$ = NiShaderDeclaration::SHADERPARAM_NI_POSITION7;       }
    |   PDP_BLENDWEIGHTS    
            {   $$ = NiShaderDeclaration::SHADERPARAM_NI_BLENDWEIGHT;    }
    |   PDP_BLENDINDICES    
            {   $$ = NiShaderDeclaration::SHADERPARAM_NI_BLENDINDICES;   }
    |   PDP_NORMAL          
            {   $$ = NiShaderDeclaration::SHADERPARAM_NI_NORMAL;         }
    |   PDP_POINTSIZE       
            {   $$ =    0;                                                  }
    |   PDP_COLOR           
            {   $$ = NiShaderDeclaration::SHADERPARAM_NI_COLOR;          }
    |   PDP_COLOR2          
            {   $$ =    0;                                                  }
    |   PDP_TEXCOORD0       
            {   $$ = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD0;      }
    |   PDP_TEXCOORD1       
            {   $$ = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD1;      }
    |   PDP_TEXCOORD2       
            {   $$ = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD2;      }
    |   PDP_TEXCOORD3       
            {   $$ = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD3;      }
    |   PDP_TEXCOORD4       
            {   $$ = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD4;      }
    |   PDP_TEXCOORD5       
            {   $$ = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD5;      }
    |   PDP_TEXCOORD6       
            {   $$ = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD6;      }
    |   PDP_TEXCOORD7       
            {   $$ = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD7;      }
    |   PDP_NORMAL2         
            {   $$ =    0;                                                  }
    |   PDP_TANGENT         
            {   $$ = NiShaderDeclaration::SHADERPARAM_NI_TANGENT;        }
    |   PDP_BINORMAL        
            {   $$ = NiShaderDeclaration::SHADERPARAM_NI_BINORMAL;       }
    ;

packing_definition_tesselator:
        PDTESS_DEFAULT          
            {   $$ = NiShaderDeclaration::SPTESS_DEFAULT;            }
    |   PDTESS_PARTIALU         
            {   $$ = NiShaderDeclaration::SPTESS_PARTIALU;           }
    |   PDTESS_PARTIALV         
            {   $$ = NiShaderDeclaration::SPTESS_PARTIALV;           }
    |   PDTESS_CROSSUV          
            {   $$ = NiShaderDeclaration::SPTESS_CROSSUV;            }
    |   PDTESS_UV               
            {   $$ = NiShaderDeclaration::SPTESS_UV;                 }
    |   PDTESS_LOOKUP           
            {   $$ = NiShaderDeclaration::SPTESS_LOOKUP;             }
    |   PDTESS_LOOKUPPRESAMPLED 
            {   $$ = NiShaderDeclaration::SPTESS_LOOKUPPRESAMPLED;   }
    ;

packing_definition_usage:
        PDU_POSITION            
            {   $$ = NiShaderDeclaration::SPUSAGE_POSITION;      }
    |   PDU_BLENDWEIGHT         
            {   $$ = NiShaderDeclaration::SPUSAGE_BLENDWEIGHT;   }
    |   PDU_BLENDINDICES        
            {   $$ = NiShaderDeclaration::SPUSAGE_BLENDINDICES;  }
    |   PDU_NORMAL              
            {   $$ = NiShaderDeclaration::SPUSAGE_NORMAL;        }
    |   PDU_PSIZE               
            {   $$ = NiShaderDeclaration::SPUSAGE_PSIZE;         }
    |   PDU_TEXCOORD            
            {   $$ = NiShaderDeclaration::SPUSAGE_TEXCOORD;      }
    |   PDU_TANGENT             
            {   $$ = NiShaderDeclaration::SPUSAGE_TANGENT;       }
    |   PDU_BINORMAL            
            {   $$ = NiShaderDeclaration::SPUSAGE_BINORMAL;      }
    |   PDU_TESSFACTOR          
            {   $$ = NiShaderDeclaration::SPUSAGE_TESSFACTOR;    }
    |   PDU_POSITIONT           
            {   $$ = NiShaderDeclaration::SPUSAGE_POSITIONT;     }
    |   PDU_COLOR               
            {   $$ = NiShaderDeclaration::SPUSAGE_COLOR;         }
    |   PDU_FOG                 
            {   $$ = NiShaderDeclaration::SPUSAGE_FOG;           }
    |   PDU_DEPTH               
            {   $$ = NiShaderDeclaration::SPUSAGE_DEPTH;         }
    |   PDU_SAMPLE              
            {   $$ = NiShaderDeclaration::SPUSAGE_SAMPLE;        }
    ;
    
packing_definition_entries:
        packing_definition_entries    packing_definition_entry
    |   packing_definition_entry
    |   error    '\n'
            {
                NSFParsererror("Syntax Error: packing_definition_entries");
                yyclearin;
            }
    ;
    
packing_definition_entry:
        PD_STREAM    N_INT   
            {
                g_uiCurrPDStream    = (unsigned int)$2;
                NiSprintf(g_szDSO, 1024, "Stream %d\n", g_uiCurrPDStream);
                DebugStringOut(g_szDSO);
            }
    |   PD_FIXEDFUNCTION    N_BOOL
            {
                g_bCurrPDFixedFunction = $2;
                if (g_pkCurrPackingDef)
                    g_pkCurrPackingDef->SetFixedFunction($2);
            }
    |   PDP_EXTRADATA    
        N_INT   
        N_INT   
        packing_definition_type
        packing_definition_tesselator
        packing_definition_usage
        N_INT   
            {
                unsigned int uiParam = 
                    NiShaderDeclaration::SHADERPARAM_EXTRA_DATA_MASK;
                uiParam |= $2;
                unsigned int uiRegister = (unsigned int)$3;

                NSBPackingDef::NiD3DPackingDefType eType = 
                    (NSBPackingDef::NiD3DPackingDefType)$4;
                NiShaderDeclaration::ShaderParameterTesselator eTess = 
                    (NiShaderDeclaration::ShaderParameterTesselator)
                    $5;
                NiShaderDeclaration::ShaderParameterUsage eUsage = 
                    (NiShaderDeclaration::ShaderParameterUsage)
                    $6;
                unsigned int uiUsageIndex = (unsigned int)$7;

                // Add the entry to the current stream
                if (g_pkCurrPackingDef)
                {
                    if (!g_pkCurrPackingDef->AddPackingEntry(
                        g_uiCurrPDStream, uiRegister, uiParam, eType, 
                        eTess, eUsage, uiUsageIndex))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    PackingDefinition failed AddPackingEntry\n"
                            "    at line %d\n", 
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber());
                    }
                }

                const char* pszParam = NSBPackingDef::GetParameterName(
                    (NiShaderDeclaration::ShaderParameter)uiParam);
                const char* pszType = NSBPackingDef::GetTypeName(eType);

                NiSprintf(g_szDSO, 1024, "    %16s %2d - Reg %3d - %16s - "
                    "0x%08x, 0x%08x, 0x%08x\n", 
                    pszParam, (int)$2, uiRegister, pszType, eTess, 
                    eUsage, uiUsageIndex);
                DebugStringOut(g_szDSO);

                NiSprintf(g_szDSO, 1024, "    %16s %2d - Reg %3d - %16s\n", 
                    pszParam, (int)$2, uiRegister, pszType);
                DebugStringOut(g_szDSO);
            }
    |   PDP_EXTRADATA    N_INT   N_INT   packing_definition_type
            {
                unsigned int uiParam = 
                    NiShaderDeclaration::SHADERPARAM_EXTRA_DATA_MASK;
                uiParam |= $2;
                unsigned int uiRegister = (unsigned int)$3;
                NSBPackingDef::NiD3DPackingDefType eType = 
                    (NSBPackingDef::NiD3DPackingDefType)$4;
                
                // Add the entry to the current stream
                if (g_pkCurrPackingDef)
                {
                    if (!g_pkCurrPackingDef->AddPackingEntry(g_uiCurrPDStream, 
                        uiRegister, uiParam, eType,

                        NiShaderDeclaration::SPTESS_DEFAULT, 
                        NiShaderDeclaration::SPUSAGE_COUNT, 0))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    PackingDefinition failed AddPackingEntry\n"
                            "    at line %d\n", 
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber());
                    }
                }
                
                const char* pszParam = NSBPackingDef::GetParameterName(
                    (NiShaderDeclaration::ShaderParameter)uiParam);
                const char* pszType = NSBPackingDef::GetTypeName(eType);

                NiSprintf(g_szDSO, 1024, "    %16s %2d - Reg %3d - %16s\n", 
                    pszParam, (int)$2, uiRegister, pszType);
                DebugStringOut(g_szDSO);
            }
    |   packing_definition_parameter    
        N_INT   
        packing_definition_type
        packing_definition_tesselator
        packing_definition_usage
        N_INT   
            {
                unsigned int uiParam = $1;
                unsigned int uiRegister = (unsigned int)$2;
                NSBPackingDef::NiD3DPackingDefType eType = 
                    (NSBPackingDef::NiD3DPackingDefType)$3;
                NiShaderDeclaration::ShaderParameterTesselator eTess = 
                    (NiShaderDeclaration::ShaderParameterTesselator)
                    $4;
                NiShaderDeclaration::ShaderParameterUsage eUsage = 
                    (NiShaderDeclaration::ShaderParameterUsage)
                    $5;
                unsigned int uiUsageIndex = (unsigned int)$6;

                // Add the entry to the current stream
                if (g_pkCurrPackingDef)
                {
                    if (!g_pkCurrPackingDef->AddPackingEntry(
                        g_uiCurrPDStream, uiRegister, uiParam, eType, 
                        eTess, eUsage, uiUsageIndex))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    PackingDefinition failed AddPackingEntry\n"
                            "    at line %d\n", 
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber());
                    }
                }

                const char* pszParam = NSBPackingDef::GetParameterName(
                    (NiShaderDeclaration::ShaderParameter)uiParam);
                const char* pszType = NSBPackingDef::GetTypeName(eType);

                NiSprintf(g_szDSO, 1024, "    %16s    - Reg %3d - %16s - "
                    "0x%08x, 0x%08x, 0x%08x\n", 
                    pszParam, uiRegister, pszType, eTess, eUsage, 
                    uiUsageIndex);
                DebugStringOut(g_szDSO);
            }
    |   packing_definition_parameter    N_INT   packing_definition_type
            {
                unsigned int uiParam = $1;
                unsigned int uiRegister = (unsigned int)$2;
                NSBPackingDef::NiD3DPackingDefType eType = 
                    (NSBPackingDef::NiD3DPackingDefType)$3;

                // Add the entry to the current stream
                if (g_pkCurrPackingDef)
                {
                    if (!g_pkCurrPackingDef->AddPackingEntry(
                        g_uiCurrPDStream, uiRegister, uiParam, eType,
                        NiShaderDeclaration::SPTESS_DEFAULT, 
                        NiShaderDeclaration::SPUSAGE_COUNT, 0))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    PackingDefinition failed AddPackingEntry\n"
                            "    at line %d\n", 
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber());
                    }
                }

                const char* pszParam = NSBPackingDef::GetParameterName(
                    (NiShaderDeclaration::ShaderParameter)uiParam);
                const char* pszType = NSBPackingDef::GetTypeName(eType);

                NiSprintf(g_szDSO, 1024, "    %16s    - Reg %3d - %16s\n", 
                    pszParam, uiRegister, pszType);
                DebugStringOut(g_szDSO);
            }
    ;

//---------------------------------------------------------------------------
// SemanticAdapterTable
//---------------------------------------------------------------------------

semantic_adapter_table_declaration:
        SEMANTICADAPTERTABLE
        L_ACCOLADE
            {
                NiSprintf(g_szDSO, 1024, "SemanticAdapterTable List Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
            }
        semantic_adapter_list
        R_ACCOLADE
            {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "SemanticAdapterTable List End\n");
                DebugStringOut(g_szDSO);

                //g_pkCurrRSGroup = 0;
            }
    ;

// A list of one or more entries
semantic_adapter_list:
        semantic_adapter_list    semantic_adapter_entry
    |   semantic_adapter_entry
    ;

// A single semantic adapter table entry of the form:
//   <component count> <generic name> : <generic idx>
//   <renderer name> : <renderer idx> @ <renderer slot>
semantic_adapter_entry:
        N_INT                            /* ComponentCount = $1 */
        string_or_quote NSF_COLON N_INT  /* Generic Name : Index = $2 $3 $4 */
        string_or_quote NSF_COLON N_INT  /* Renderer Name : Index = $5 $6 $7 */
        NSF_AT_SYMBOL N_INT              /* @ Renderer Slot = $8 $9 */
        NSF_SEMICOLON                    /* ; = $10 */
            {
                NiSprintf(g_szDSO, 1024, "SAT entry [%d elements map from "
                    "%s,%d to %s,%d,%d]\n", $1, $2, $4, $5, $7, $9);
                DebugStringOut(g_szDSO);

                if (g_pkCurrImplementation)
                {
                    NiSemanticAdapterTable& kTable =
                        g_pkCurrImplementation->GetSemanticAdapterTable();
                    unsigned int uiEntryID = kTable.GetFreeEntry();
                    
                    kTable.SetComponentCount(uiEntryID, $1);
                    kTable.SetGenericSemantic(uiEntryID, $2, $4);
                    kTable.SetRendererSemantic(uiEntryID, $5, $7);
                    kTable.SetSharedRendererSemanticPosition(uiEntryID, $9);
                }

                NiFree($2);
                NiFree($5);
            }
    ;

//---------------------------------------------------------------------------
// RenderStates
//---------------------------------------------------------------------------
renderstate_list_with_brackets:
        RENDERSTATES
        L_ACCOLADE
            {
                NiSprintf(g_szDSO, 1024, "RenderState List Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
            
                // Check the pass first...    
                if (g_pkCurrPass)
                {
                    g_pkCurrRSGroup = g_pkCurrPass->GetRenderStateGroup();
                }
                else
                if (g_pkCurrImplementation)
                {
                    g_pkCurrRSGroup = 
                        g_pkCurrImplementation->GetRenderStateGroup();
                }
                else
                {
                    g_pkCurrRSGroup = 0;
                }
            }
            renderstate_list
        R_ACCOLADE
            {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "RenderState List End\n");
                DebugStringOut(g_szDSO);

                g_pkCurrRSGroup = 0;
            }
    ;

renderstate_list:
        renderstate_list    renderstate_entry_save_optional
    |   renderstate_entry_save_optional
    ;
    
renderstate_entry_save_optional:
        renderstate_entry_save
            {
                if (g_bCurrStateValid)
                {
                    g_pkCurrRSGroup->SetState(g_uiCurrStateState, 
                        g_uiCurrStateValue, true);

                    DebugStringOut("    SAVE\n", false);
                }
            }
    |   renderstate_entry
            {
                if (g_bCurrStateValid)
                {
                    g_pkCurrRSGroup->SetState(g_uiCurrStateState, 
                        g_uiCurrStateValue, false);

                    DebugStringOut("\n", false);
                }
            }
    ;

renderstate_entry_save:
        renderstate_entry    SAVE
    ;
    
renderstate_entry:
        renderstate_bool
    |   renderstate_hex
    |   renderstate_float
    |   renderstate_attribute
    |   renderstate_string
    |   error    '\n'
            {
                NSFParsererror("Syntax Error: renderstate_entry");
                yyclearin;
            }
    ;

renderstate_attribute:
        N_STRING    ASSIGN    attribute_name
            {
                NSBRenderStates::NiD3DRenderState eRS = 
                    NSBRenderStates::LookupRenderState($1);
                if (eRS == NSBRenderStates::NID3DRS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), $1, 
                        NSFParserGetLineNumber());
                }
                else if (eRS == (NSBRenderStates::NID3DRS_DEPRECATED))
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    DeprecatedRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), $1, 
                        NSFParserGetLineNumber());
                }
                
                NiSprintf(g_szDSO, 1024, "    %32s = ATTRIBUTE - %s", 
                    $1, $3);
                DebugStringOut(g_szDSO);

                NiFree($1);
                NiFree($3);
            }
    ;

renderstate_string:
        N_STRING    ASSIGN    N_STRING
            {
                NSBRenderStates::NiD3DRenderState eRS = 
                    NSBRenderStates::LookupRenderState($1);
                if (eRS == NSBRenderStates::NID3DRS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), $1, 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else if (eRS == (NSBRenderStates::NID3DRS_DEPRECATED))
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    DeprecatedRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), $1, 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    unsigned int uiValue; 
                    if (!NSBRenderStates::LookupRenderStateValue(eRS, 
                        $3, uiValue))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    Invalid RenderStateValue (%s)\n"
                            "    at line %d\n", 
                            g_pkFile->GetFilename(), $3, 
                            NSFParserGetLineNumber());
                        g_bCurrStateValid = false;
                    }
                    else
                    {
                        g_uiCurrStateState = (unsigned int)eRS;
                        g_uiCurrStateValue = uiValue;
                        g_bCurrStateValid = true;
                    }
                }

                NiSprintf(g_szDSO, 1024, "    %32s = %s", 
                    $1, $3);
                DebugStringOut(g_szDSO);

                NiFree($1);
                NiFree($3);
            }
    ;
    
renderstate_bool:
        N_STRING    ASSIGN    N_BOOL
            {
                NSBRenderStates::NiD3DRenderState eRS = 
                    NSBRenderStates::LookupRenderState($1);
                if (eRS == NSBRenderStates::NID3DRS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), $1, 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else if (eRS == (NSBRenderStates::NID3DRS_DEPRECATED))
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    DeprecatedRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), $1, 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    unsigned int uiValue = $3 ? 1 : 0;

                    g_uiCurrStateState = (unsigned int)eRS;
                    g_uiCurrStateValue = uiValue;
                    g_bCurrStateValid = true;
                }

                NiSprintf(g_szDSO, 1024, "    %32s = %s", $1, 
                    $3 ? "TRUE" : "FALSE");
                DebugStringOut(g_szDSO);

                NiFree($1);
            }
    ;
    
renderstate_float:
        N_STRING    ASSIGN    N_FLOAT
            {
                NSBRenderStates::NiD3DRenderState eRS = 
                    NSBRenderStates::LookupRenderState($1);
                if (eRS == NSBRenderStates::NID3DRS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), $1, 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else if (eRS == (NSBRenderStates::NID3DRS_DEPRECATED))
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    DeprecatedRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), $1, 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    unsigned int uiValue = F2DW($3);
                    
                    g_uiCurrStateState = (unsigned int)eRS;
                    g_uiCurrStateValue = uiValue;
                    g_bCurrStateValid = true;
                }

                NiSprintf(g_szDSO, 1024, "    %32s = %8.5f", 
                    $1, $3);
                DebugStringOut(g_szDSO);

                NiFree($1);
            }
    ;
    
renderstate_hex:
        N_STRING    ASSIGN    N_HEX
            {
                NSBRenderStates::NiD3DRenderState eRS = 
                    NSBRenderStates::LookupRenderState($1);
                if (eRS == NSBRenderStates::NID3DRS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), $1, 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else if (eRS == (NSBRenderStates::NID3DRS_DEPRECATED))
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    DeprecatedRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), $1, 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    unsigned int uiValue = $3;

                    g_uiCurrStateState = (unsigned int)eRS;
                    g_uiCurrStateValue = uiValue;
                    g_bCurrStateValid = true;
                }
                NiSprintf(g_szDSO, 1024, "    %32s = 0x%08x", 
                    $1, $3);
                DebugStringOut(g_szDSO);

                NiFree($1);
            }
    |   N_STRING    ASSIGN    N_INT
            {
                NSBRenderStates::NiD3DRenderState eRS = 
                    NSBRenderStates::LookupRenderState($1);
                if (eRS == NSBRenderStates::NID3DRS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), $1, 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else if (eRS == (NSBRenderStates::NID3DRS_DEPRECATED))
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    DeprecatedRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), $1, 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    unsigned int uiValue = $3;

                    g_uiCurrStateState = (unsigned int)eRS;
                    g_uiCurrStateValue = uiValue;
                    g_bCurrStateValid = true;
                }
                NiSprintf(g_szDSO, 1024, "    %32s = 0x%08x", 
                    $1, $3);
                DebugStringOut(g_szDSO);

                NiFree($1);
            }
    ;
    
//---------------------------------------------------------------------------
// ConstantMaps
//---------------------------------------------------------------------------
constantmap_list:
       constantmap_list    constantmap_entry
    |   constantmap_entry
    ;
    
constantmap_entry:
       constantmap_entry_defined
    |   constantmap_entry_object
    |   constantmap_entry_attribute
    |   constantmap_entry_constant
    |   constantmap_entry_global
    |   constantmap_entry_operator
    |   constantmap_platform_block
    ;

constantmap_platform_block:
        L_ACCOLADE
            {
                if (g_bConstantMapPlatformBlock)
                {
                    NSFParsererror("Syntax Error: "
                        "Embedded ConstantMap Platform-block");
                }
                NiSprintf(g_szDSO, 1024,"ConstantMap Platform-block Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                g_bConstantMapPlatformBlock = true;
            }
            PLATFORM    ASSIGN  requirement_platform_list
                {
                    g_uiCurrentPlatforms = $5;
                }
            constantmap_list
        R_ACCOLADE
            {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "ConstantMap Platform-block End\n");
                DebugStringOut(g_szDSO);
                g_bConstantMapPlatformBlock = false;
            }

constantmap_entry_defined:
    CMDEFINED    SKINBONEMATRIX3    N_INT   N_INT   N_INT   
            {
                // Name, RegStart, StartBoneIndex, BoneCount
                unsigned int uiFlags = 
                    NiShaderConstantMapEntry::SCME_MAP_DEFINED;
                unsigned int uiExtra = (unsigned int)$4 |
                    ((unsigned int)$5 << 16);
                unsigned int uiRegCount = 3 * (unsigned int)$5;
                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, "SkinBoneMatrix3", uiFlags, 
                        uiExtra, (unsigned int)$3, uiRegCount, 
                        NULL);
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry("SkinBoneMatrix3", uiFlags, 
                        uiExtra, (unsigned int)$3, uiRegCount, 
                        NULL);
                }

                if (g_pkCurrImplementation)
                {
                    g_pkCurrRequirements = 
                        g_pkCurrImplementation->GetRequirements();
                    if (g_pkCurrRequirements)
                    {
                        g_pkCurrRequirements->SetBoneMatrixRegisters(3);
                        g_pkCurrRequirements->SetBoneCalcMethod(
                            NiBinaryShader::BONECALC_SKIN);
                    }
                }
                    
                NiSprintf(g_szDSO, 1024,
                    "    Defined: SkinBoneMatrix3          "
                    "%3d %3d %3d\n", (int)$3, (int)$4, 
                    int($5));
                DebugStringOut(g_szDSO);
            }
    |   CMDEFINED    SKINBONEMATRIX3    string_or_quote   N_INT   N_INT   
            {
                // Name, RegStart, StartBoneIndex, BoneCount
                unsigned int uiFlags = 
                    NiShaderConstantMapEntry::SCME_MAP_DEFINED;
                unsigned int uiExtra = (unsigned int)$4 |
                    ((unsigned int)$5 << 16);
                unsigned int uiRegCount = 3 * (unsigned int)$5;
                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, "SkinBoneMatrix3", uiFlags, 
                        uiExtra, -1, uiRegCount, $3);
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry("SkinBoneMatrix3", uiFlags, 
                        uiExtra, -1, uiRegCount, $3);
                }

                if (g_pkCurrImplementation)
                {
                    g_pkCurrRequirements = 
                        g_pkCurrImplementation->GetRequirements();
                    if (g_pkCurrRequirements)
                    {
                        g_pkCurrRequirements->SetBoneMatrixRegisters(3);
                        g_pkCurrRequirements->SetBoneCalcMethod(
                            NiBinaryShader::BONECALC_SKIN);
                    }
                }
                    
                NiSprintf(g_szDSO, 1024,
                    "    Defined: SkinBoneMatrix3          "
                    "%24s %3d %3d\n", $3, (int)$4, 
                    int($5));
                DebugStringOut(g_szDSO);
                
                NiFree($3)
            }
    |   CMDEFINED string_or_quote    N_INT   N_INT   
            {
                // Name, RegStart, Extra
                if (NiShaderConstantMap::LookUpPredefinedMappingType(
                    $2) == NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED)
                {
                    char acBuffer[1024];
                    NiSprintf(acBuffer, sizeof(acBuffer),
                        "PARSE ERROR: %s (line %d)\n"
                        "Predefined mapping %s is deprecated or invalid.",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        $2);
                    NSFParsererror(acBuffer);
                    yyclearin;
                }
                else
                {
                    unsigned int uiFlags = 
                        NiShaderConstantMapEntry::SCME_MAP_DEFINED;
                    if (g_bConstantMapPlatformBlock)
                    {
                        g_pkCurrConstantMap->AddPlatformSpecificEntry(
                            g_uiCurrentPlatforms, $2, uiFlags, 
                            (unsigned int)$4, 
                            (unsigned int)$3, 0, 0);
                    }
                    else
                    {
                        g_pkCurrConstantMap->AddEntry($2, uiFlags, 
                            (unsigned int)$4, 
                            (unsigned int)$3, 0, 0);
                    }

                    NiSprintf(g_szDSO, 1024, "    Defined: %24s %3d %3d\n",
                        $2, (int)$3, (int)$4);
                    DebugStringOut(g_szDSO);
                }
                
                NiFree($2);
            }
    |   CMDEFINED string_or_quote    string_or_quote     N_INT   
            {
                // Name, VariableName, Extra
                if (NiShaderConstantMap::LookUpPredefinedMappingType(
                    $2) == NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED)
                {
                    char acBuffer[1024];
                    NiSprintf(acBuffer, sizeof(acBuffer),
                        "PARSE ERROR: %s (line %d)\n"
                        "Predefined mapping %s is deprecated or invalid.",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        $2);
                    NSFParsererror(acBuffer);
                    yyclearin;
                }
                else
                {
                    unsigned int uiFlags = 
                        NiShaderConstantMapEntry::SCME_MAP_DEFINED;
                    if (g_bConstantMapPlatformBlock)
                    {
                        g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, $2, uiFlags, 
                        (unsigned int)$4, -1, 0, $3);
                    }
                    else
                    {
                        g_pkCurrConstantMap->AddEntry($2, uiFlags, 
                            (unsigned int)$4, -1, 0, $3);
                    }

                    NiSprintf(g_szDSO, 1024,
                        "    Defined: %24s %3d %24s %3d\n",
                        $2, -1, $3, (int)$4);
                    DebugStringOut(g_szDSO);
                }
                
                NiFree($2);
                NiFree($3);
            }
    ;

constantmap_entry_object:
        CMOBJECT    N_STRING    N_STRING    N_INT
            {
                // CM_Object, NameOfObject, Parameter, RegStart
                
                NSBObjectTable::ObjectDesc* pkDesc = NULL;
                NSBObjectTable* pkTable = g_pkCurrNSBShader->GetObjectTable();
                if (pkTable)
                {
                    pkDesc = pkTable->GetObjectByName($2);
                }
                if (!pkDesc)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        false,
                        "* PARSE ERROR: %s\n"
                        "    GetObjectByName at line %d\n"
                        "    Local name = %s\n",
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber(),
                        $2);
                    break;
                }

                // Verify mapping is supported by object type.
                unsigned int uiMapping;
                bool bSuccess = NiShaderConstantMap::LookUpObjectMapping($3,
                    uiMapping);
                if (bSuccess)
                {
                    bSuccess = NiShaderConstantMap::IsObjectMappingValidForType(
                        (NiShaderConstantMap::ObjectMappings) uiMapping,
                        pkDesc->GetType());
                }
                if (!bSuccess)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        false,
                        "* PARSE ERROR: %s\n"
                        "    IsObjectMappingValidForType at line %d\n"
                        "    Object name = %s\n"
                        "    Object mapping = %s\n",
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber(),
                        $2,
                        $3);
                    break;
                }

                unsigned int uiFlags =
                    NiShaderConstantMapEntry::SCME_MAP_OBJECT |
                    NiShaderConstantMapEntry::GetObjectFlags(
                        pkDesc->GetType());
                
                size_t stBufSize = strlen($3) + strlen($2) + 3;
                char* pcKey = NiAlloc(char, stBufSize);
                NiSprintf(pcKey, stBufSize, "%s@@%s", $3, $2);
                
                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pcKey, uiFlags,
                        pkDesc->GetIndex(), (unsigned int) $4, 0, 0);
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry(pcKey, uiFlags,
                        pkDesc->GetIndex(), (unsigned int) $4, 0, 0);
                }
                
                NiSprintf(g_szDSO, 1024, "    Object: %24s %16s %3d\n",
                    $2, $3, $4);
                DebugStringOut(g_szDSO);
                
                NiFree(pcKey);
                NiFree($2);
                NiFree($3);
            }
    |
        CMOBJECT    N_STRING    N_STRING    N_STRING
            {
                // CM_Object, NameOfObject, Parameter, VariableName
                
                NSBObjectTable::ObjectDesc* pkDesc = NULL;
                NSBObjectTable* pkTable = g_pkCurrNSBShader->GetObjectTable();
                if (pkTable)
                {
                    pkDesc = pkTable->GetObjectByName($2);
                }
                if (!pkDesc)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        false,
                        "* PARSE ERROR: %s\n"
                        "    GetObjectByName at line %d\n"
                        "    Local name = %s\n",
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber(),
                        $2);
                    break;
                }
                
                // Verify mapping is supported by object type.
                unsigned int uiMapping;
                bool bSuccess = NiShaderConstantMap::LookUpObjectMapping($3,
                    uiMapping);
                if (bSuccess)
                {
                    bSuccess = NiShaderConstantMap::IsObjectMappingValidForType(
                        (NiShaderConstantMap::ObjectMappings) uiMapping,
                        pkDesc->GetType());
                }
                if (!bSuccess)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        false,
                        "* PARSE ERROR: %s\n"
                        "    IsObjectMappingValidForType at line %d\n"
                        "    Local name = %s\n"
                        "    Object mapping = %s\n",
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber(),
                        $2,
                        $3);
                    break;
                }
                
                unsigned int uiFlags =
                    NiShaderConstantMapEntry::SCME_MAP_OBJECT |
                    NiShaderConstantMapEntry::GetObjectFlags(
                        pkDesc->GetType());
                
                size_t stBufSize = strlen($3) + strlen($2) + 3;
                char* pcKey = NiAlloc(char, stBufSize);
                NiSprintf(pcKey, stBufSize, "%s@@%s", $3, $2);
                
                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pcKey, uiFlags,
                        pkDesc->GetIndex(), -1, 0, $4);
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry(pcKey, uiFlags,
                        pkDesc->GetIndex(), -1, 0, $4);
                }
                
                NiSprintf(g_szDSO, 1024, "    Object: %24s %16s %16s\n",
                    $2, $3, $4);
                DebugStringOut(g_szDSO);
                
                NiFree(pcKey);
                NiFree($2);
                NiFree($3);
                NiFree($4);
                
            }
    ;

constantmap_entry_attribute:
        CMATTRIBUTE
            {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            }
        string_or_quote N_INT   N_INT   N_INT
            {
                // Name, RegStart, RegCount
                if (!AddAttributeToConstantMap($3, $4, $5, $6, false))
                {
                    // Report the error
                    NSFParsererror(
                        "Syntax Error: AddAttributeToConstantMap!");
                    yyclearin;
                }

                NiSprintf(g_szDSO, 1024, "     Attrib: %24s %3d %3d %3d\n",
                    $3, (int)$4, (int)$5, (int)$6);
                DebugStringOut(g_szDSO);
                                
                NiFree($3);
            }
    ;

//note: constant maps don't support arrays at this time
constantmap_entry_constant:
        CMCONSTANT
            {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            }
        string_or_quote N_INT   N_INT   float_values_arbitrary_list 
            {
                // Name, RegStart, RegCount, Values

                unsigned int uiFlags = 
                    NiShaderConstantMapEntry::SCME_MAP_CONSTANT |
                    NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;

                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, $3, uiFlags, 
                        0, (unsigned int)$4, 
                        (unsigned int)$5, NULL,
                        g_afValues->GetSize() * sizeof(float),
                        sizeof(float), g_afValues->GetBase(), true);
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry($3, uiFlags, 
                        0, (unsigned int)$4, 
                        (unsigned int)$5, NULL,
                        g_afValues->GetSize() * sizeof(float),
                        sizeof(float), g_afValues->GetBase(), true);
                }
                
                NiSprintf(g_szDSO, 1024, "      Const: %24s %3d %3d - %3d values\n",
                    $3, (int)$4, 
                    (int)$5, g_afValues->GetSize());
                DebugStringOut(g_szDSO);
                
                NiSprintf(g_szDSO, 1024, "             %24s         - ", " ");
                DebugStringOut(g_szDSO);
                
                for (unsigned int ui = 0; ui < g_afValues->GetSize(); ui++)
                {
                    NiSprintf(g_szDSO, 1024, "%8.5f,", g_afValues->GetAt(ui));
                    if ((((ui + 1) % 4) == 0) ||
                        (ui == (g_afValues->GetSize() - 1)))
                    {
                        NiStrcat(g_szDSO, 1024, "\n");
                    }
                    DebugStringOut(g_szDSO, false);
                    if ((((ui + 1) % 4) == 0) &&
                        (ui < (g_afValues->GetSize() - 1)))
                    {
                        NiSprintf(g_szDSO, 1024, "             %24s         - ", " ");
                        DebugStringOut(g_szDSO);
                    }
                }
                
                NiFree($3);
            }
    ;

constantmap_entry_global:
        CMGLOBAL
            {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            }
        string_or_quote N_INT   N_INT   
            {
                // Name, RegStart, RegCount
                if (!AddAttributeToConstantMap($3, $4, $5, 0, true))
                {
                    // Report the error
                    NSFParsererror(
                        "Syntax Error: AddAttributeToConstantMap!");
                    yyclearin;
                }

                NiSprintf(g_szDSO, 1024, "     Global: %24s %3d %3d\n",
                    $3, (int)$4, 
                    (int)$5);
                DebugStringOut(g_szDSO);
                                
                NiFree($3);
            }
    ;

constantmap_entry_operator:
        CMOPERATOR    
        N_STRING    
        N_INT   
        N_INT   
        N_STRING    
        operator_type    
        N_STRING    
        operator_optional_inverse
        operator_optional_transpose
            {
                // Name, type, RegStart, RegCount, Entry1 * Entry2
                if (!SetupOperatorEntry($2, $3, 
                    $4, $5, (int)$6, 
                    $7, $8, $9))
                {
                    NiFree($2);
                    NiFree($5);
                    NiFree($7);
                    // Report the error
                    NSFParsererror("Syntax Error: SetupOperatorEntry!");
                    yyclearin;
                }
                else
                {
                    NiFree($2);
                    NiFree($5);
                    NiFree($7);
                }
            }
    ;

operator_type:
        ASTERIK
            {   $$ = NiShaderConstantMapEntry::SCME_OPERATOR_MULTIPLY;   }
    |   FORWARDSLASH
            {   $$ = NiShaderConstantMapEntry::SCME_OPERATOR_DIVIDE;     }
    |   PLUS
            {   $$ = NiShaderConstantMapEntry::SCME_OPERATOR_ADD;        }
    |   MINUS
            {   $$ = NiShaderConstantMapEntry::SCME_OPERATOR_SUBTRACT;   }
    |   error    '\n'
            {
                NSFParsererror("Syntax Error: operator_type");
                yyclearin;
            }
    ;
    
operator_optional_transpose:
        /* empty */     {   $$ = false;     }
    |   TRANSPOSE       {   $$ = true;      }
    ;
    
operator_optional_inverse:
        /* empty */     {   $$ = false;     }
    |   INVERSE         {   $$ = true;      }
    ;
    
//---------------------------------------------------------------------------
// VertexShader ConstantMaps
vs_constantmap_with_brackets:
        VSCONSTANTMAP
        L_ACCOLADE
            {
                g_eConstMapMode = CONSTMAP_VERTEX;
                NiSprintf(g_szDSO, 1024, "VertexShader ConstantMap Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                
                // Check the pass first...    
                if (g_pkCurrPass)
                {
                    g_pkCurrConstantMap = g_pkCurrPass->GetVertexConstantMap(
                        g_uiCurrPassVSConstantMap++);
                }
                else
                if (g_pkCurrImplementation)
                {
                    g_pkCurrConstantMap = g_pkCurrImplementation->
                        GetVertexConstantMap(g_uiCurrImplemVSConstantMap++);
                }
                else
                {
                    g_pkCurrConstantMap = 0;
                }
            }
            constantmap_list
        R_ACCOLADE
            {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "VertexShader ConstantMap End\n");
                DebugStringOut(g_szDSO);

                g_pkCurrConstantMap = 0;
            }
    ;

//---------------------------------------------------------------------------
// GeometryShader ConstantMaps
gs_constantmap_with_brackets:
        GSCONSTANTMAP
        L_ACCOLADE
            {
                g_eConstMapMode = CONSTMAP_GEOMETRY;
                NiSprintf(g_szDSO, 1024, "GeometryShader ConstantMap Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                
                // Check the pass first...    
                if (g_pkCurrPass)
                {
                    g_pkCurrConstantMap = g_pkCurrPass->GetGeometryConstantMap(
                        g_uiCurrPassGSConstantMap++);
                }
                else
                if (g_pkCurrImplementation)
                {
                    g_pkCurrConstantMap = g_pkCurrImplementation->
                        GetGeometryConstantMap(g_uiCurrImplemGSConstantMap++);
                }
                else
                {
                    g_pkCurrConstantMap = 0;
                }
            }
            constantmap_list
        R_ACCOLADE
            {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "GeometryShader ConstantMap End\n");
                DebugStringOut(g_szDSO);

                g_pkCurrConstantMap = 0;
            }
    ;

//---------------------------------------------------------------------------
// PixelShader ConstantMaps
ps_constantmap_with_brackets:
        PSCONSTANTMAP
        L_ACCOLADE
            {
                g_eConstMapMode = CONSTMAP_PIXEL;
                NiSprintf(g_szDSO, 1024, "PixelShader ConstantMap Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                // Check the pass first...    
                if (g_pkCurrPass)
                {
                    g_pkCurrConstantMap = g_pkCurrPass->GetPixelConstantMap(
                        g_uiCurrPassPSConstantMap++);
                }
                else
                if (g_pkCurrImplementation)
                {
                    g_pkCurrConstantMap = g_pkCurrImplementation->
                        GetPixelConstantMap(g_uiCurrImplemPSConstantMap++);
                }
                else
                {
                    g_pkCurrConstantMap = 0;
                }
            }
            constantmap_list
        R_ACCOLADE
            {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "PixelShader ConstantMap End\n");
                DebugStringOut(g_szDSO);

                g_pkCurrConstantMap = 0;
            }
    ;

//---------------------------------------------------------------------------
// User-defined classname
//---------------------------------------------------------------------------
userdefined_classname:
        CLASSNAME   ASSIGN  string_or_quote
            {
                if (g_pkCurrImplementation)
                    g_pkCurrImplementation->SetClassName($3);

                NiSprintf(g_szDSO, 1024, "ClassName = %s\n", $3);
                DebugStringOut(g_szDSO);
                NiFree($3);
            }
    ;

//---------------------------------------------------------------------------
// Vertex Shader programs
//---------------------------------------------------------------------------
vertexshader_program_name:
        VSPROGRAM   PATH
            {
                SetShaderProgramFile(g_pkCurrPass, $2,
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_VERTEX);
                NiFree($2);
            }
    |   VSPROGRAM   N_STRING
            {
                SetShaderProgramFile(g_pkCurrPass, $2,
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_VERTEX);
                NiFree($2);
            }
    |   VSPROGRAM   N_QUOTE
            {
                SetShaderProgramFile(g_pkCurrPass, $2,
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_VERTEX);
                NiFree($2);
            }
    ;

vertexshader_program:
        vertexshader_program_name   N_STRING    N_STRING
            {
                // shader, entry point, shader target
                NiSprintf(g_szDSO, 1024, "VertexShader File EP %s, ST %s\n", 
                    $2, $3);
                DebugStringOut(g_szDSO);

                SetShaderProgramEntryPoint(g_pkCurrPass, $2,
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_VERTEX);
                SetShaderProgramShaderTarget(g_pkCurrPass, $3,
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_VERTEX);
                    
                NiFree($2);
                NiFree($3);
            }
    |   vertexshader_program_name   N_STRING
            {
                // shader, entry point
                NiSprintf(g_szDSO, 1024, "VertexShader File EP %s\n", $2);
                DebugStringOut(g_szDSO);

                SetShaderProgramEntryPoint(g_pkCurrPass, $2,
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_VERTEX);
                    
                NiFree($2);
            }
    |   vertexshader_program_name
            {
                // shader
                NiSprintf(g_szDSO, 1024, "VertexShader File\n");
                DebugStringOut(g_szDSO);
            }
    ;

vertexshader_program_name_only:
        PROGRAM ASSIGN  PATH        {   $$ = $3;    }
    |   PROGRAM ASSIGN  N_STRING    {   $$ = $3;    }
    |   PROGRAM ASSIGN  N_QUOTE     {   $$ = $3;    }
    ;

vertexshader_program_bracket_contents_list:
        vertexshader_program_bracket_content
    |   vertexshader_program_bracket_content
        vertexshader_program_bracket_contents_list
    ;
    
vertexshader_program_bracket_content:
        vertexshader_program_name_only
            {
                SetShaderProgramFile(g_pkCurrPass, $1,
                    g_uiCurrentPlatforms,
                    NiGPUProgram::PROGRAM_VERTEX);
                NiFree($1);
            }
    |   ENTRYPOINT      ASSIGN  N_STRING
            {
                SetShaderProgramEntryPoint(g_pkCurrPass, $3,
                    g_uiCurrentPlatforms,
                    NiGPUProgram::PROGRAM_VERTEX);
                NiFree($3);
            }
    |   SHADERTARGET    ASSIGN  N_STRING
            {
                SetShaderProgramShaderTarget(g_pkCurrPass, $3,
                    g_uiCurrentPlatforms,
                    NiGPUProgram::PROGRAM_VERTEX);
                NiFree($3);
            }
    |   SOFTWAREVP      ASSIGN  N_BOOL
            {
                if (g_pkCurrPass)
                {
                    g_pkCurrPass->SetSoftwareVertexProcessing($3);
                }
            }
    ;
    
vertexshader_program_bracket:
        VSPROGRAM
        L_ACCOLADE
            {
                NiSprintf(g_szDSO, 1024, "VertexShader Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
            }
            PLATFORM    ASSIGN  requirement_platform_list
                {
                    g_uiCurrentPlatforms = $6;
                }
            vertexshader_program_bracket_contents_list
        R_ACCOLADE
            {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "VertexShader End\n");
                DebugStringOut(g_szDSO);
            }
    ;
    
vertexshader_program_entry:
        vertexshader_program_bracket
    |   vertexshader_program
    ;
    
//---------------------------------------------------------------------------
// Geometry Shader programs
//---------------------------------------------------------------------------
geometryshader_program_name:
        GSPROGRAM   PATH
            {
                SetShaderProgramFile(g_pkCurrPass, $2,
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_GEOMETRY);
                NiFree($2);
            }
    |   GSPROGRAM   N_STRING
            {
                SetShaderProgramFile(g_pkCurrPass, $2,
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_GEOMETRY);
                NiFree($2);
            }
    |   GSPROGRAM   N_QUOTE
            {
                SetShaderProgramFile(g_pkCurrPass, $2,
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_GEOMETRY);
                NiFree($2);
            }
    ;

geometryshader_program:
        geometryshader_program_name N_STRING    N_STRING
            {
                // shader, entry point, shader target
                NiSprintf(g_szDSO, 1024, "GeometryShader File EP %s, ST %s\n", 
                    $2, $3);
                DebugStringOut(g_szDSO);

                SetShaderProgramEntryPoint(g_pkCurrPass, $2,
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_GEOMETRY);
                SetShaderProgramShaderTarget(g_pkCurrPass, $3,
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_GEOMETRY);
                    
                NiFree($2);
                NiFree($3);
            }
    |   geometryshader_program_name N_STRING
            {
                // shader, entry point
                NiSprintf(g_szDSO, 1024, "GeometryShader File EP %s\n", $2);
                DebugStringOut(g_szDSO);

                SetShaderProgramEntryPoint(g_pkCurrPass, $2,
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_GEOMETRY);
                NiFree($2);
            }
    |   geometryshader_program_name
            {
                // shader
                NiSprintf(g_szDSO, 1024, "GeometryShader File\n");
                DebugStringOut(g_szDSO);
            }
    ;

geometryshader_program_name_only:
        PROGRAM ASSIGN  PATH        {   $$ = $3;    }
    |   PROGRAM ASSIGN  N_STRING    {   $$ = $3;    }
    |   PROGRAM ASSIGN  N_QUOTE     {   $$ = $3;    }
    ;

geometryshader_program_bracket_contents_list:
        geometryshader_program_bracket_content
    |   geometryshader_program_bracket_content
        geometryshader_program_bracket_contents_list
    ;
    
geometryshader_program_bracket_content:
        geometryshader_program_name_only
            {
                SetShaderProgramFile(g_pkCurrPass, $1,
                    g_uiCurrentPlatforms,
                    NiGPUProgram::PROGRAM_GEOMETRY);
                NiFree($1);
            }
    |   ENTRYPOINT      ASSIGN  N_STRING
            {
                SetShaderProgramEntryPoint(g_pkCurrPass, $3,
                    g_uiCurrentPlatforms,
                    NiGPUProgram::PROGRAM_GEOMETRY);
                NiFree($3);
            }
    |   SHADERTARGET    ASSIGN  N_STRING
            {
                SetShaderProgramShaderTarget(g_pkCurrPass, $3,
                    g_uiCurrentPlatforms,
                    NiGPUProgram::PROGRAM_GEOMETRY);
                NiFree($3);
            }
    ;
    
geometryshader_program_bracket:
        GSPROGRAM
        L_ACCOLADE
            {
                NiSprintf(g_szDSO, 1024, "GeometryShader Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
            }
            PLATFORM    ASSIGN  requirement_platform_list
                {
                    g_uiCurrentPlatforms = $6;
                }
            geometryshader_program_bracket_contents_list
        R_ACCOLADE
            {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "GeometryShader End\n");
                DebugStringOut(g_szDSO);
            }
    ;
    
geometryshader_program_entry:
        geometryshader_program_bracket
    |   geometryshader_program
    ;
    
//---------------------------------------------------------------------------
// Pixel Shader programs
//---------------------------------------------------------------------------
pixelshader_program_name:
        PSPROGRAM   PATH
            {
                SetShaderProgramFile(g_pkCurrPass, $2,
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_PIXEL);
                NiFree($2);
            }
    |   PSPROGRAM   N_STRING
            {
                SetShaderProgramFile(g_pkCurrPass, $2,
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_PIXEL);
                NiFree($2);
            }
    |   PSPROGRAM   N_QUOTE
            {
                SetShaderProgramFile(g_pkCurrPass, $2,
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_PIXEL);
                NiFree($2);
            }
    ;

pixelshader_program:
        pixelshader_program_name    N_STRING    N_STRING
            {
                // shader, entry point, shader target
                NiSprintf(g_szDSO, 1024, "PixelShader File EP %s, ST %s\n", 
                    $2, $3);
                DebugStringOut(g_szDSO);

                SetShaderProgramEntryPoint(g_pkCurrPass, $2,
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_PIXEL);
                SetShaderProgramShaderTarget(g_pkCurrPass, $3,
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_PIXEL);
                    
                NiFree($2);
                NiFree($3);
            }
    |   pixelshader_program_name    N_STRING
            {
                // shader, entry point
                NiSprintf(g_szDSO, 1024, "PixelShader File EP %s\n", $2);
                DebugStringOut(g_szDSO);

                SetShaderProgramEntryPoint(g_pkCurrPass, $2,
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_PIXEL);
                NiFree($2);
            }
    |   pixelshader_program_name
            {
                // shader
                NiSprintf(g_szDSO, 1024, "PixelShader File\n");
                DebugStringOut(g_szDSO);
            }
    ;

pixelshader_program_name_only:
        PROGRAM ASSIGN  PATH        {   $$ = $3;    }
    |   PROGRAM ASSIGN  N_STRING    {   $$ = $3;    }
    |   PROGRAM ASSIGN  N_QUOTE     {   $$ = $3;    }
    ;

pixelshader_program_bracket_contents_list:
        pixelshader_program_bracket_content
    |   pixelshader_program_bracket_content
        pixelshader_program_bracket_contents_list
    ;
    
pixelshader_program_bracket_content:
        pixelshader_program_name_only
            {
                SetShaderProgramFile(g_pkCurrPass, $1,
                    g_uiCurrentPlatforms,
                    NiGPUProgram::PROGRAM_PIXEL);
                NiFree($1);
            }
    |   ENTRYPOINT      ASSIGN  N_STRING
            {
                SetShaderProgramEntryPoint(g_pkCurrPass, $3,
                    g_uiCurrentPlatforms,
                    NiGPUProgram::PROGRAM_PIXEL);
                NiFree($3);
            }
    |   SHADERTARGET    ASSIGN  N_STRING
            {
                SetShaderProgramShaderTarget(g_pkCurrPass, $3,
                    g_uiCurrentPlatforms,
                    NiGPUProgram::PROGRAM_PIXEL);
                NiFree($3);
            }
    ;
    
pixelshader_program_bracket:
        PSPROGRAM
        L_ACCOLADE
            {
                NiSprintf(g_szDSO, 1024, "PixelShader Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
            }
            PLATFORM    ASSIGN  requirement_platform_list
                {
                    g_uiCurrentPlatforms = $6;
                }
            pixelshader_program_bracket_contents_list
        R_ACCOLADE
            {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "PixelShader End\n");
                DebugStringOut(g_szDSO);
            }
    ;
    
pixelshader_program_entry:
        pixelshader_program_bracket
    |   pixelshader_program
    ;
    
//---------------------------------------------------------------------------
// Requirements
//---------------------------------------------------------------------------
requirement_list_with_brackets:
        REQUIREMENTS
        L_ACCOLADE
            {
                NiSprintf(g_szDSO, 1024, "Requirement List Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrImplementation)
                {
                    g_pkCurrRequirements = 
                        g_pkCurrImplementation->GetRequirements();
                }
            }
            requirement_list
        R_ACCOLADE
            {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Requirement List End\n");
                DebugStringOut(g_szDSO);

                if (g_pkCurrNSBShader && g_pkCurrRequirements)
                {
                    // Update the shader requirements
                    g_pkCurrNSBShader->AddVertexShaderVersion(
                        g_pkCurrRequirements->GetVSVersion());
                    g_pkCurrNSBShader->AddGeometryShaderVersion(
                        g_pkCurrRequirements->GetGSVersion());
                    g_pkCurrNSBShader->AddPixelShaderVersion(
                        g_pkCurrRequirements->GetPSVersion());
                    g_pkCurrNSBShader->AddUserVersion(
                        g_pkCurrRequirements->GetUserVersion());
                    g_pkCurrNSBShader->AddPlatform(
                        g_pkCurrRequirements->GetPlatformFlags());
                }
                g_pkCurrRequirements = 0;
            }
    ;

requirement_list:
        requirement_list    requirement_entry
    |   requirement_entry
    ;
    
requirement_entry:
        requirement_vsversion
    |   requirement_gsversion
    |   requirement_psversion
    |   requirement_userdefined
    |   requirement_platform
    |   requirement_bonesperpartition
    |   requirement_usesbinormaltangent
    |   requirement_usesbinormaltangentuvsource
    |   requirement_remaining
    |   error    '\n'
            {
                NSFParsererror("Syntax Error: requirement_entry");
                yyclearin;
            }
    ;

requirement_vsversion:
        VSVERSION    ASSIGN    N_VERSION
            {
                NiSprintf(g_szDSO, 1024, "    VSVersion   0x%08x\n", $3);
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetVSVersion($3);
            }
    ;

requirement_gsversion:
        GSVERSION    ASSIGN    N_VERSION
            {
                // The N_VERSION method uses the VS method to set the value.
                // We need to flip it back to the GS method here.
#ifndef _PS3
                unsigned int uiMaj = D3DSHADER_VERSION_MAJOR($3);
                unsigned int uiMin = D3DSHADER_VERSION_MINOR($3);
                unsigned int uiVers = D3DGS_VERSION(uiMaj, uiMin);
#else
                unsigned int uiVers = $3;
#endif                
            
                NiSprintf(g_szDSO, 1024, "    GSVersion   0x%08x\n", uiVers);
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetGSVersion(uiVers);
            }
    ;

requirement_psversion:
        PSVERSION    ASSIGN    N_VERSION
            {
                // The N_VERSION method uses the VS method to set the value.
                // We need to flip it back to the PS method here
#ifndef _PS3
                unsigned int uiMaj = D3DSHADER_VERSION_MAJOR($3);
                unsigned int uiMin = D3DSHADER_VERSION_MINOR($3);
                unsigned int uiVers = D3DPS_VERSION(uiMaj, uiMin);
#else
                unsigned int uiVers = $3;
#endif                
                NiSprintf(g_szDSO, 1024, "    PSVersion   0x%08x\n", uiVers);
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetPSVersion(uiVers);
            }
    ;

requirement_userdefined:
        USERVERSION    ASSIGN    N_VERSION
            {
                NiSprintf(g_szDSO, 1024, "    UserDefined 0x%08x\n", $3);
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetUserVersion($3);
            }
    ;

requirement_platform:
        PLATFORM    ASSIGN    requirement_platform_list
            {
                NiSprintf(g_szDSO, 1024, "       Platform 0x%08x\n", $3);
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetPlatformFlags($3);
            }
    ;
    
requirement_platform_list:
        N_STRING    OR    N_STRING    OR    N_STRING    OR  N_STRING
            {
                unsigned int uiPlatform = 0;
                unsigned int uiTemp;
                
                uiTemp = DecodePlatformString($1);
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", $1);
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;
                uiTemp = DecodePlatformString($3);
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", $3);
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;
                uiTemp = DecodePlatformString($5);
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", $5);
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;
                uiTemp = DecodePlatformString($7);
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", $7);
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;

                 NiFree($1);
                 NiFree($3);
                 NiFree($5);
                 NiFree($7);
                $$ = uiPlatform;
            }
    |   N_STRING    OR    N_STRING    OR    N_STRING
            {
                unsigned int uiPlatform = 0;
                unsigned int uiTemp;
                
                uiTemp = DecodePlatformString($1);
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", $1);
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;
                uiTemp = DecodePlatformString($3);
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", $3);
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;
                uiTemp = DecodePlatformString($5);
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", $5);
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;

                 NiFree($1);
                 NiFree($3);
                 NiFree($5);
                $$ = uiPlatform;
            }
    |   N_STRING    OR    N_STRING
            {
                unsigned int uiPlatform = 0;
                unsigned int uiTemp;
                
                uiTemp = DecodePlatformString($1);
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", $1);
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;
                uiTemp = DecodePlatformString($3);
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", $3);
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;
                
                 NiFree($1);
                 NiFree($3);

                $$ = uiPlatform;
            }
    |   N_STRING
            {
                unsigned int uiPlatform = 0;
                unsigned int uiTemp;
                
                uiTemp = DecodePlatformString($1);
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", $1);
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;
                
                 NiFree($1);

                $$ = uiPlatform;
            }
    ;

requirement_remaining:
        N_STRING    ASSIGN  N_BOOL
            {
                NiSprintf(g_szDSO, 1024, "%15s %s\n", $1,
                    $3 ? "true" : "false");
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                {
                    // Determine the requirement field
                    if (NiStricmp($1, "USESNIRENDERSTATE") == 0)
                    {
                        g_pkCurrRequirements->SetUsesNiRenderState(
                            $3);
                    }
                    else if (NiStricmp($1, "USESNILIGHTSTATE") == 0)
                    {
                        g_pkCurrRequirements->SetUsesNiLightState(
                            $3);
                    }
                    else if (NiStricmp($1, "SOFTWAREVPREQUIRED") == 
                        0)
                    {
                        g_pkCurrRequirements->SetSoftwareVPRequired(
                            $3);
                    }
                    else if (NiStricmp($1, "SOFTWAREVPFALLBACK") == 
                        0)
                    {
                        g_pkCurrRequirements->SetSoftwareVPAcceptable(
                            $3);
                    }
                }
                
                NiFree($1);
            }
    ;
    
requirement_bonesperpartition:
        BONESPERPARTITION    ASSIGN    N_INT   
            {
                NiSprintf(g_szDSO, 1024, "Bones/Partition %d\n", $3);
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetBonesPerPartition($3);
            }
    ;
    
requirement_usesbinormaltangentuvsource:
        BINORMALTANGENTUVSOURCE    ASSIGN    N_INT
            {
                NiSprintf(g_szDSO, 1024, "BinormalTangentUVSource %d\n", $3);
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                {
                    g_pkCurrRequirements->SetBinormalTangentUVSource($3);
                }
            }
    ;
    
requirement_usesbinormaltangent:
        BINORMALTANGENTMETHOD    ASSIGN    binormaltanget_method
            {
                NiSprintf(g_szDSO, 1024, "BinormalTangent %d\n", $3);
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                {
                    g_pkCurrRequirements->SetBinormalTangentMethod(
                        (NiShaderRequirementDesc::NBTFlags)$3);
                }
            }
    ;
    
    
binormaltanget_method:
        NBTMETHOD_NONE  {   $$ = NiShaderRequirementDesc::NBT_METHOD_NONE;   }
    |   NBTMETHOD_NI    {   $$ = NiShaderRequirementDesc::NBT_METHOD_NDL;    }
    |   NBTMETHOD_ATI   {   $$ = NiShaderRequirementDesc::NBT_METHOD_ATI;    }
    |   NBTMETHOD_MAX   
            {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    Deprecated value for BinormalTangentMethod "
                    "Requirement\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(),
                    NSFParserGetLineNumber());
                $$ = NiShaderRequirementDesc::NBT_METHOD_NDL;    
            }
    |   error    '\n'
            {
                NSFParsererror("Syntax Error: binormaltanget_method");
                yyclearin;
            }

//---------------------------------------------------------------------------
// Stage and Samplers
//---------------------------------------------------------------------------
stage_or_sampler_or_texture:
        stage
    |   sampler
    |   texture
    ;
    
//---------------------------------------------------------------------------
// Stages
//---------------------------------------------------------------------------
stage:
        STAGE    N_INT   optional_string
        L_ACCOLADE
            {
                NiSprintf(g_szDSO, 1024, "Stage Start %3d - %s\n", $2, 
                    $3);
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                
                if (g_pkCurrPass)
                {
                    g_pkCurrTextureStage = 
                        g_pkCurrPass->GetStage((int)$2);
                    if (g_pkCurrTextureStage)
                    {
                        g_pkCurrTextureStage->SetUseTextureTransformation(
                            false);
                    }
                }
            }
            stage_entry_list_optional
        R_ACCOLADE
            {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Stage End %3d - %s\n", (int)$2, 
                    $3);
                DebugStringOut(g_szDSO);

                g_pkCurrTextureStage = 0;

                NiFree($3);
            }
    ;

stage_entry_list_optional:
        /* empty */
    |   stage_entry_list
    ;
    
stage_entry_list:
        stage_entry_list    stage_entry_or_texture
    |   stage_entry_or_texture    
    ;
    
stage_entry_or_texture:
        stage_texture
    |   stage_textransmatrix
    |   stage_entry_save_optional
    ;
    
stage_entry_save_optional:
        stage_entry_save
            {
                if (g_bCurrStateValid)
                {
                    if (g_pkCurrTextureStage)
                    {
                        NSBStateGroup* pkGroup = 
                            g_pkCurrTextureStage->GetTextureStageGroup();
                        if (pkGroup)
                        {
                            pkGroup->SetState(g_uiCurrStateState, 
                                g_uiCurrStateValue, true);
                        }
                    }
                
                    DebugStringOut("    SAVE\n", false);
                }
            }
    |   stage_entry
            {
                if (g_bCurrStateValid)
                {
                    if (g_pkCurrTextureStage)
                    {
                        NSBStateGroup* pkGroup = 
                            g_pkCurrTextureStage->GetTextureStageGroup();
                        if (pkGroup)
                        {
                            pkGroup->SetState(g_uiCurrStateState, 
                                g_uiCurrStateValue, false);
                        }
                    }
                    
                    DebugStringOut("\n", false);
                }
            }
    ;

stage_entry_save:
        stage_entry    SAVE
    ;

stage_entry:
        stage_color_op
    |   stage_color_arg0
    |   stage_color_arg1
    |   stage_color_arg2
    |   stage_alpha_op
    |   stage_alpha_arg0
    |   stage_alpha_arg1
    |   stage_alpha_arg2
    |   stage_result_arg
    |   stage_constant_deprecated
    |   stage_bumpenvmat00
    |   stage_bumpenvmat01
    |   stage_bumpenvmat10
    |   stage_bumpenvmat11
    |   stage_bumpenvlscale
    |   stage_bumpenvloffset
    |   stage_texcoordindex
    |   stage_textransflags
    |   error    '\n'
            {
                NSFParsererror("Syntax Error: stage_entry");
                yyclearin;
            }
    ;

// This must NOT be grouped w/ the stage entries!
stage_texture:
        TSS_TEXTURE ASSIGN  stage_texture_map_ndl
            {
                if (g_pkCurrTextureStage)
                    g_pkCurrTextureStage->SetNDLMap($3);
            }
    |   TSS_TEXTURE ASSIGN  stage_texture_map_ndl_decal
            {
                if (g_pkCurrTextureStage)
                    g_pkCurrTextureStage->SetDecalMap($3);
            }
    |   TSS_TEXTURE ASSIGN  attribute_name
            {
                bool bFoundAttribute = false;
                NSBObjectTable* pkObjectTable = 0;
                if (g_pkCurrNSBShader)
                {
                    g_bGlobalAttributes = false;
                    g_pkCurrAttribTable = 
                        g_pkCurrNSBShader->GetAttributeTable();
                    pkObjectTable = g_pkCurrNSBShader->GetObjectTable();
                }
                if (g_pkCurrAttribTable)
                {
                    NSBAttributeDesc* pkAttrib = 
                        g_pkCurrAttribTable->GetAttributeByName($3);
                    if (pkAttrib)
                    {
                        bFoundAttribute = true;
                        
                        unsigned int uiValue;
                        const char* pszValue;
                        
                        if (pkAttrib->GetValue_Texture(uiValue, pszValue))
                        {
                            uiValue |= NiTextureStage::TSTF_MAP_SHADER;
                            if (g_pkCurrTextureStage)
                                g_pkCurrTextureStage->SetShaderMap(uiValue);
                        }
                        else
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, true,
                                "* PARSE ERROR: %s\n"
                                "    GetValue_Texture at line %d\n"
                                "    Attribute name = %s\n",
                                g_pkFile->GetFilename(),
                                NSFParserGetLineNumber(),
                                $3);
                        }
                    }
                    g_pkCurrAttribTable = 0;
                }
                
                if (!bFoundAttribute && pkObjectTable)
                {
                    NSBObjectTable::ObjectDesc* pkDesc =
                        pkObjectTable->GetObjectByName($3);
                    if (pkDesc)
                    {
                        NiShaderAttributeDesc::ObjectType eObjectType =
                            pkDesc->GetType();
                        if (eObjectType <
                            NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP ||
                            eObjectType >
                            NiShaderAttributeDesc::OT_EFFECT_FOGMAP)
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, true,
                                "* PARSE ERROR: %s\n"
                                "    InvalidObjectType at line %d\n"
                                "    Object name = %s\n",
                                g_pkFile->GetFilename(),
                                NSFParserGetLineNumber(),
                                $3);
                        }
                        else
                        {
                            if (g_pkCurrTextureStage)
                            {
                                g_pkCurrTextureStage->SetObjTextureSettings(
                                    eObjectType, pkDesc->GetIndex());
                            }
                        }
                    }
                    else
                    {
                        NiShaderFactory::ReportError(
                            NISHADERERR_UNKNOWN, true,
                            "* PARSE ERROR: %s\n"
                            "    TextureNotFound at line %d\n"
                            "    Attribute/Object name = %s\n",
                            g_pkFile->GetFilename(),
                            NSFParserGetLineNumber(),
                            $3);
                    }
                }
                
                NiFree($3);
            }
    |   TSS_TEXTURE ASSIGN  N_STRING
    {           
        NSBObjectTable::ObjectDesc* pkDesc = NULL;
        NSBObjectTable* pkTable = g_pkCurrNSBShader->GetObjectTable();
        if (pkTable)
        {
            pkDesc = pkTable->GetObjectByName($3);
        }
        if (!pkDesc)
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                false,
                "* PARSE ERROR: %s\n"
                "    GetObjectByName at line %d\n"
                "    Local name = %s\n",
                g_pkFile->GetFilename(),
                NSFParserGetLineNumber(),
                $3);
            NiFree($3);                             
            break;
        }
        if (g_pkCurrTextureStage)
        {
            g_pkCurrTextureStage->SetObjTextureSettings(
                pkDesc->GetType(), pkDesc->GetIndex());
        }
                
        NiFree($3);             
    }            
    ;

// Determine the 'pre-defined' Gamebryo map/slot to use
stage_texture_map_ndl:
        NTM_BASE    {   $$ = NiTextureStage::TSTF_NDL_BASE;      }
    |   NTM_DARK    {   $$ = NiTextureStage::TSTF_NDL_DARK;      }
    |   NTM_DETAIL  {   $$ = NiTextureStage::TSTF_NDL_DETAIL;    }
    |   NTM_GLOSS   {   $$ = NiTextureStage::TSTF_NDL_GLOSS;     }
    |   NTM_GLOW    {   $$ = NiTextureStage::TSTF_NDL_GLOW;      }
    |   NTM_BUMP    {   $$ = NiTextureStage::TSTF_NDL_BUMP;      }
    |   NTM_NORMAL    {   $$ = NiTextureStage::TSTF_NDL_NORMAL;      }
    |   NTM_PARALLAX    {   $$ = NiTextureStage::TSTF_NDL_PARALLAX;      }
    ;
    
// Determine the 'pre-defined' Gamebryo decal map/slot to use
stage_texture_map_ndl_decal:
        NTM_DECAL   N_INT   
            {
                $$ = NiTextureStage::TSTF_MAP_DECAL | $2;
            }
    ;

stage_color_op:
        TSS_COLOROP    ASSIGN    stage_texture_operation
            {
                NiSprintf(g_szDSO, 1024, "ColorOp            = 0x%08x", $3);
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_COLOROP;
                g_uiCurrStateValue = $3;  
                g_bCurrStateValid = true;  
            }
    ;

stage_color_arg0:
        TSS_COLORARG0    ASSIGN    stage_texture_argument
            {
                NiSprintf(g_szDSO, 1024, "ColorArg0          = 0x%08x", $3);
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_COLORARG0;
                g_uiCurrStateValue = $3;    
                g_bCurrStateValid = true;  
            }
    ;

stage_color_arg1:
        TSS_COLORARG1    ASSIGN    stage_texture_argument
            {
                NiSprintf(g_szDSO, 1024, "ColorArg1          = 0x%08x", $3);
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_COLORARG1;
                g_uiCurrStateValue = $3;    
                g_bCurrStateValid = true;  
            }
    ;

stage_color_arg2:
        TSS_COLORARG2    ASSIGN    stage_texture_argument
            {
                NiSprintf(g_szDSO, 1024, "ColorArg2          = 0x%08x", $3);
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_COLORARG2;
                g_uiCurrStateValue = $3;    
                g_bCurrStateValid = true;  
            }
    ;

stage_alpha_op:
        TSS_ALPHAOP        ASSIGN    stage_texture_operation
            {
                NiSprintf(g_szDSO, 1024, "AlphaOp            = 0x%08x", $3);
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_ALPHAOP;
                g_uiCurrStateValue = $3;    
                g_bCurrStateValid = true;  
            }
    ;

stage_alpha_arg0:
        TSS_ALPHAARG0    ASSIGN    stage_texture_argument
            {
                NiSprintf(g_szDSO, 1024, "AlphaArg0          = 0x%08x", $3);
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_ALPHAARG0;
                g_uiCurrStateValue = $3;    
                g_bCurrStateValid = true;  
            }
    ;

stage_alpha_arg1:
        TSS_ALPHAARG1    ASSIGN    stage_texture_argument
            {
                NiSprintf(g_szDSO, 1024, "AlphaArg1          = 0x%08x", $3);
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_ALPHAARG1;
                g_uiCurrStateValue = $3;    
                g_bCurrStateValid = true;  
            }
    ;

stage_alpha_arg2:
        TSS_ALPHAARG2    ASSIGN    stage_texture_argument
            {
                NiSprintf(g_szDSO, 1024, "AlphaArg2          = 0x%08x", $3);
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_ALPHAARG2;
                g_uiCurrStateValue = $3;    
                g_bCurrStateValid = true;  
            }
    ;

stage_result_arg:
        TSS_RESULTARG    ASSIGN    stage_texture_argument
            {
                NiSprintf(g_szDSO, 1024, "ResultArg          = 0x%08x", $3);
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_RESULTARG;
                g_uiCurrStateValue = $3;    
                g_bCurrStateValid = true;  
            }
    ;

stage_constant_deprecated:
        TSS_CONSTANT_DEPRECATED    ASSIGN    N_HEX
            {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_INVALID;
                g_uiCurrStateValue = $3;
                    
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedTextureStageState (TSS_CONSTANT)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(),
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;  
            }
    ;

stage_bumpenvmat00:
        TSS_BUMPENVMAT00    ASSIGN    N_FLOAT
            {
                NiSprintf(g_szDSO, 1024, "BumpEnvMat00       = %8.5f", $3);
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_BUMPENVMAT00;
                g_uiCurrStateValue = F2DW($3);
                g_bCurrStateValid = true;  
            }
    |   TSS_BUMPENVMAT00    ASSIGN    attribute_name
            {
                NiSprintf(g_szDSO, 1024, "BumpEnvMat00       = %s", $3);
                DebugStringOut(g_szDSO);

                NiFree($3);
            }
    ;
    
stage_bumpenvmat01:
        TSS_BUMPENVMAT01    ASSIGN    N_FLOAT
            {
                NiSprintf(g_szDSO, 1024, "BumpEnvMat01       = %8.5f", $3);
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_BUMPENVMAT01;
                g_uiCurrStateValue = F2DW($3);
                g_bCurrStateValid = true;  
            }
    |   TSS_BUMPENVMAT01    ASSIGN    attribute_name
            {
                NiSprintf(g_szDSO, 1024, "BumpEnvMat01       = %s", $3);
                DebugStringOut(g_szDSO);

                NiFree($3);
            }
    ;
    
stage_bumpenvmat10:
        TSS_BUMPENVMAT10    ASSIGN    N_FLOAT
            {
                NiSprintf(g_szDSO, 1024, "BumpEnvMat10       = %8.5f", $3);
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_BUMPENVMAT10;
                g_uiCurrStateValue = F2DW($3);
                g_bCurrStateValid = true;  
            }
    |   TSS_BUMPENVMAT10    ASSIGN    attribute_name
            {
                NiSprintf(g_szDSO, 1024, "BumpEnvMat10       = %s", $3);
                DebugStringOut(g_szDSO);

                NiFree($3);
            }
    ;
    
stage_bumpenvmat11:
        TSS_BUMPENVMAT11    ASSIGN    N_FLOAT
            {
                NiSprintf(g_szDSO, 1024, "BumpEnvMat11       = %8.5f", $3);
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_BUMPENVMAT11;
                g_uiCurrStateValue = F2DW($3);
                g_bCurrStateValid = true;  
            }
    |   TSS_BUMPENVMAT11    ASSIGN    attribute_name
            {
                NiSprintf(g_szDSO, 1024, "BumpEnvMat11       = %s", $3);
                DebugStringOut(g_szDSO);

                NiFree($3);
            }
    ;
    
stage_bumpenvlscale:
        TSS_BUMPENVLSCALE    ASSIGN    N_FLOAT
            {
                NiSprintf(g_szDSO, 1024, "BumpEnvLScale      = %8.5f", $3);
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_BUMPENVLSCALE;
                g_uiCurrStateValue = F2DW($3);
                g_bCurrStateValid = true;  
            }
    |   TSS_BUMPENVLSCALE    ASSIGN    attribute_name
            {
                NiSprintf(g_szDSO, 1024, "BumpEnvLScale      = %s", $3);
                DebugStringOut(g_szDSO);

                NiFree($3);
            }
    ;
    
stage_bumpenvloffset:
        TSS_BUMPENVLOFFSET    ASSIGN    N_FLOAT
            {
                NiSprintf(g_szDSO, 1024, "BumpEnvLOffset     = %8.5f", $3);
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_BUMPENVLOFFSET;
                g_uiCurrStateValue = F2DW($3);
                g_bCurrStateValid = true;  
            }
    |   TSS_BUMPENVLOFFSET    ASSIGN    attribute_name
            {
                NiSprintf(g_szDSO, 1024, "BumpEnvLOffset     = %s", $3);
                DebugStringOut(g_szDSO);

                NiFree($3);
            }
    ;

stage_texcoordindex:
        TSS_TEXCOORDINDEX    ASSIGN    stage_texcoordindex_flags    N_INT   
            {
                int iValue = $3 | $4;
                NiSprintf(g_szDSO, 1024, "TexCoordIndex      = 0x%08x", iValue);
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_TEXCOORDINDEX;
                g_uiCurrStateValue = iValue;
                g_bCurrStateValid = true;  
            }
    |   TSS_TEXCOORDINDEX    ASSIGN    stage_texcoordindex_flags    USEMAPINDEX
            {
                int iValue = $3;
                NiSprintf(g_szDSO, 1024, "TexCoordIndex      = 0x%08x", iValue);
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_TEXCOORDINDEX;
                g_uiCurrStateValue = iValue;
                g_bCurrStateValid = true;  

                if (g_pkCurrTextureStage)
                    g_pkCurrTextureStage->SetUseIndexFromMap(true);
            }
    ;
    
stage_textransflags:
        TSS_TEXTURETRANSFORMFLAGS    
        ASSIGN    
        stage_texturetransformflags_count 
        stage_texturetransformflags_optional_projection
            {
                NiSprintf(g_szDSO, 1024, "TextureTransformFlags= COUNT%d", 
                    (int)$3);
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_TEXTURETRANSFORMFLAGS;
                g_uiCurrStateValue = (unsigned int)$3 | 
                    (unsigned int)$4;
                g_bCurrStateValid = true;  
                
                if (g_pkCurrTextureStage)
                    g_pkCurrTextureStage->SetUseTextureTransformation(true);
            }
    |   TSS_TEXTURETRANSFORMFLAGS   ASSIGN    TTFF_PROJECTED
            {
                NiSprintf(g_szDSO, 1024, "TextureTransformFlags= NID3DTTFF_PROJECTED");
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_TEXTURETRANSFORMFLAGS;
                g_uiCurrStateValue = (unsigned int)
                    NSBStageAndSamplerStates::NID3DTTFF_PROJECTED;
                g_bCurrStateValid = true;  
                
                if (g_pkCurrTextureStage)
                    g_pkCurrTextureStage->SetUseTextureTransformation(true);
            }
    |   TSS_TEXTURETRANSFORMFLAGS    ASSIGN    TTFF_DISABLE
            {
                NiSprintf(g_szDSO, 1024, "TextureTransformFlags= DISABLE");
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_TEXTURETRANSFORMFLAGS;
                g_uiCurrStateValue = 
                    NSBStageAndSamplerStates::NID3DTTFF_DISABLE;
                g_bCurrStateValid = true;  
                
                if (g_pkCurrTextureStage)
                    g_pkCurrTextureStage->SetUseTextureTransformation(true);
            }
    ;

stage_texturetransformflags_count:
        TTFF_COUNT1     {   $$ = NSBStageAndSamplerStates::NID3DTTFF_COUNT1; }
    |   TTFF_COUNT2     {   $$ = NSBStageAndSamplerStates::NID3DTTFF_COUNT2; }
    |   TTFF_COUNT3     {   $$ = NSBStageAndSamplerStates::NID3DTTFF_COUNT3; }
    |   TTFF_COUNT4     {   $$ = NSBStageAndSamplerStates::NID3DTTFF_COUNT4; }
    ;
    
stage_texturetransformflags_optional_projection:
        /* empty */     
            {   $$ = 0; }
    |   TTFF_PROJECTED
            {   $$ = NSBStageAndSamplerStates::NID3DTTFF_PROJECTED;  }
    ;

stage_texture_operation:
        TOP_DISABLE
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_DISABLE;                    
            }
    |   TOP_SELECTARG1
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_SELECTARG1;                 
            }
    |   TOP_SELECTARG2
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_SELECTARG2;                 
            }
    |   TOP_MODULATE
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_MODULATE;                   
            }
    |   TOP_MODULATE2X
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_MODULATE2X;                 
            }
    |   TOP_MODULATE4X
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_MODULATE4X;                 
            }
    |   TOP_ADD
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_ADD;                        
            }
    |   TOP_ADDSIGNED
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_ADDSIGNED;                  
            }
    |   TOP_ADDSIGNED2X
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_ADDSIGNED2X;                
            }
    |   TOP_SUBTRACT
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_SUBTRACT;                   
            }
    |   TOP_ADDSMOOTH
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_ADDSMOOTH;                  
            }
    |   TOP_BLENDDIFFUSEALPHA
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_BLENDDIFFUSEALPHA;          
            }
    |   TOP_BLENDTEXTUREALPHA
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_BLENDTEXTUREALPHA;          
            }
    |   TOP_BLENDFACTORALPHA
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_BLENDFACTORALPHA;           
            }
    |   TOP_BLENDTEXTUREALPHAPM
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_BLENDTEXTUREALPHAPM;        
            }
    |   TOP_BLENDCURRENTALPHA
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_BLENDCURRENTALPHA;          
            }
    |   TOP_PREMODULATE
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_PREMODULATE;                
            }
    |   TOP_MODULATEALPHA_ADDCOLOR
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_MODULATEALPHA_ADDCOLOR;     
            }
    |   TOP_MODULATECOLOR_ADDALPHA
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_MODULATECOLOR_ADDALPHA;     
            }
    |   TOP_MODULATEINVALPHA_ADDCOLOR
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_MODULATEINVALPHA_ADDCOLOR;
            }
    |   TOP_MODULATEINVCOLOR_ADDALPHA
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_MODULATEINVCOLOR_ADDALPHA;  
            }
    |   TOP_BUMPENVMAP
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_BUMPENVMAP;                 
            }
    |   TOP_BUMPENVMAPLUMINANCE
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_BUMPENVMAPLUMINANCE;        
            }
    |   TOP_DOTPRODUCT3
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_DOTPRODUCT3;                
            }
    |   TOP_MULTIPLYADD
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_MULTIPLYADD;                
            }
    |   TOP_LERP
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTOP_LERP;                       
            }
    |   error    '\n'
            {
                NSFParsererror("Syntax Error: stage_texture_operation");
                yyclearin;
            }
    ;
    
stage_texture_argument:
        TA_CURRENT        stage_texture_argument_modifiers_optional
            {   $$ = NSBStageAndSamplerStates::NID3DTA_CURRENT | 
                $2; }
    |   TA_DIFFUSE        stage_texture_argument_modifiers_optional
            {   $$ = NSBStageAndSamplerStates::NID3DTA_DIFFUSE | 
                $2; }
    |   TA_SELECTMASK    stage_texture_argument_modifiers_optional
            {   $$ = NSBStageAndSamplerStates::NID3DTA_SELECTMASK | 
                $2; }
    |   TA_SPECULAR        stage_texture_argument_modifiers_optional
            {   $$ = NSBStageAndSamplerStates::NID3DTA_SPECULAR | 
                $2; }
    |   TA_TEMP            stage_texture_argument_modifiers_optional
            {   $$ = NSBStageAndSamplerStates::NID3DTA_TEMP | 
                $2; }
    |   TA_TEXTURE        stage_texture_argument_modifiers_optional
            {   $$ = NSBStageAndSamplerStates::NID3DTA_TEXTURE | 
                $2; }
    |   TA_TFACTOR        stage_texture_argument_modifiers_optional
            {   $$ = NSBStageAndSamplerStates::NID3DTA_TFACTOR | 
                $2; }
    ;
    
stage_texture_argument_modifiers_optional:
        /* empty */                         {   $$ = 0;     }
    |   stage_texture_argument_modifier     {   $$ = $1;    }
    ;
    
stage_texture_argument_modifier:    
        TA_ALPHAREPLICATE    TA_COMPLEMENT
            {
                $$ = 
                    NSBStageAndSamplerStates::NID3DTA_ALPHAREPLICATE | 
                    NSBStageAndSamplerStates::NID3DTA_COMPLEMENT;
            }
    |   TA_COMPLEMENT    TA_ALPHAREPLICATE
            {
                $$ = NSBStageAndSamplerStates::NID3DTA_COMPLEMENT |
                    NSBStageAndSamplerStates::NID3DTA_ALPHAREPLICATE;
            }
    |   TA_ALPHAREPLICATE
            {   $$ = NSBStageAndSamplerStates::NID3DTA_ALPHAREPLICATE;    }
    |   TA_COMPLEMENT
            {   $$ = NSBStageAndSamplerStates::NID3DTA_COMPLEMENT;        }
    ;

stage_texcoordindex_flags:
        /* empty */
            {   $$ = 
                0;
            }
    |   TCI_PASSTHRU
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTSI_PASSTHRU;
            }
    |   TCI_CAMERASPACENORMAL
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTSI_CAMERASPACENORMAL;
            }
    |   TCI_CAMERASPACEPOSITION
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTSI_CAMERASPACEPOSITION;
            }
    |   TCI_CAMERASPACEREFLECT
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTSI_CAMERASPACEREFLECTIONVECTOR;
            }
    |   TCI_SPHEREMAP
            {   $$ = 
                NSBStageAndSamplerStates::NID3DTSI_SPHEREMAP;
            }
    |   error    '\n'
            {
                NSFParsererror("Syntax Error: stage_texcoordindex_flags");
                yyclearin;
            }
    ;

stage_textransmatrix:
        TSS_TEXTRANSMATRIX    ASSIGN    stage_textransmatrix_assignment
            {
                // The set will be performed in the assignment section!
            }
    ;

stage_textransmatrix_assignment:
        TTSRC_GLOBAL    stage_textransmatrix_option    N_STRING
            {
                unsigned int uiFlags = NiTextureStage::TSTTF_GLOBAL;
                
                uiFlags |= (int)$2;

                if (g_pkCurrTextureStage)
                {
                    const char* pszGlobalName = $3;
                    g_pkCurrTextureStage->SetTextureTransformFlags(uiFlags);
                    g_pkCurrTextureStage->SetGlobalName(pszGlobalName);
                    g_pkCurrTextureStage->SetUseTextureTransformation(true);
                }
                
                NiFree($3);
            }
    |   TTSRC_CONSTANT    
            {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            }
        stage_textransmatrix_option    float_values_16
            {
                unsigned int uiFlags = NiTextureStage::TSTTF_CONSTANT;
        
                uiFlags |= (int)$3;

#ifndef _PS3
                if (g_pkCurrTextureStage)
                {
                    D3DMATRIX kTrans;
                    
                    kTrans._11 = g_afValues->GetAt( 0);
                    kTrans._12 = g_afValues->GetAt( 1);
                    kTrans._13 = g_afValues->GetAt( 2);
                    kTrans._14 = g_afValues->GetAt( 3);
                    kTrans._21 = g_afValues->GetAt( 4);
                    kTrans._22 = g_afValues->GetAt( 5);
                    kTrans._23 = g_afValues->GetAt( 6);
                    kTrans._24 = g_afValues->GetAt( 7);
                    kTrans._31 = g_afValues->GetAt( 8);
                    kTrans._32 = g_afValues->GetAt( 9);
                    kTrans._33 = g_afValues->GetAt(10);
                    kTrans._34 = g_afValues->GetAt(11);
                    kTrans._41 = g_afValues->GetAt(12);
                    kTrans._42 = g_afValues->GetAt(13);
                    kTrans._43 = g_afValues->GetAt(14);
                    kTrans._44 = g_afValues->GetAt(15);
                    
                    g_pkCurrTextureStage->SetTextureTransformFlags(uiFlags);
                    g_pkCurrTextureStage->SetTextureTransformation(kTrans);
                    g_pkCurrTextureStage->SetUseTextureTransformation(true);
                }
#endif
                
            }
    ;

stage_textransmatrix_option:
        /* empty */             
            {   $$ = NiTextureStage::TSTTF_NI_NO_CALC;           }
    |   TT_WORLD_PARALLEL       
            {   $$ = NiTextureStage::TSTTF_NI_WORLD_PARALLEL;    }
    |   TT_WORLD_PERSPECTIVE    
            {   $$ = NiTextureStage::TSTTF_NI_WORLD_PERSPECTIVE; }
    |   TT_WORLD_SPHERE_MAP     
            {   $$ = NiTextureStage::TSTTF_NI_WORLD_SPHERE_MAP;  }
    |   TT_CAMERA_SPHERE_MAP    
            {   $$ = NiTextureStage::TSTTF_NI_CAMERA_SPHERE_MAP; }
    |   TT_SPECULAR_CUBE_MAP    
            {   $$ = NiTextureStage::TSTTF_NI_SPECULAR_CUBE_MAP; }
    |   TT_DIFFUSE_CUBE_MAP     
            {   $$ = NiTextureStage::TSTTF_NI_DIFFUSE_CUBE_MAP;  }
    |   error    '\n'
            {
                NSFParsererror("Syntax Error: stage_textransmatrix_option");
                yyclearin;
            }
    ;
        
//---------------------------------------------------------------------------
// Samplers
//---------------------------------------------------------------------------
sampler:
        SAMPLER    N_INT   optional_string
        L_ACCOLADE
            {
                NiSprintf(g_szDSO, 1024, "Sampler Start %3d - %s\n", 
                    $2, $3);
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrPass)
                {
                    g_pkCurrTextureStage = 
                        g_pkCurrPass->GetStage((int)$2);
                    g_pkCurrTextureStage->SetName($3);
                }
            }
            sampler_entry_list_optional
        R_ACCOLADE
            {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Sampler End %3d - %s\n", 
                    (int)$2, $3);
                DebugStringOut(g_szDSO);

                g_pkCurrTextureStage = 0;

                NiFree($3);
            }
    |   SAMPLER    optional_string
        L_ACCOLADE
            {
                NiSprintf(g_szDSO, 1024, "Sampler Start - %s\n", $2);
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrPass)
                {
                    g_pkCurrTextureStage = 
                        g_pkCurrPass->GetStage(g_pkCurrPass->GetStageCount());
                    g_pkCurrTextureStage->SetName($2);
                }
            }
            sampler_entry_list_optional
        R_ACCOLADE
            {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Sampler End %s\n", $2);
                DebugStringOut(g_szDSO);

                g_pkCurrTextureStage = 0;

                NiFree($2);
            }
    ;
    
sampler_entry_list_optional:
        /* empty */
    |   sampler_entry_list
    ;
    
sampler_entry_list:
        sampler_entry_list    sampler_entry_save_optional
    |   sampler_entry_save_optional
    ;

sampler_entry_save_optional:
        sampler_entry_save
            {
                if (g_bCurrStateValid)
                {
                    if (g_pkCurrTextureStage)
                    {
                        NSBStateGroup* pkGroup = 
                            g_pkCurrTextureStage->GetSamplerStageGroup();
                        if (pkGroup)
                        {
                            pkGroup->SetState(g_uiCurrStateState,
                                g_uiCurrStateValue, true, g_bUseMapValue);
                            g_bUseMapValue = false;
                        }
                    }
                    
                    DebugStringOut("    SAVE\n", false);
                }
            }
    |   sampler_entry
            {
                if (g_bCurrStateValid)
                {
                    if (g_pkCurrTextureStage)
                    {
                        NSBStateGroup* pkGroup = 
                            g_pkCurrTextureStage->GetSamplerStageGroup();
                        if (pkGroup)
                        {
                            pkGroup->SetState(g_uiCurrStateState, 
                                g_uiCurrStateValue, false, g_bUseMapValue);
                            g_bUseMapValue = false;
                        }
                    }
                    
                    DebugStringOut("\n", false);
                }
            }
    |   stage_texture
            {
                // Do nothing. It's handles in the stage_texture block!
                DebugStringOut("Sampler Texture!\n");
            }
    ;

sampler_entry_save:
        sampler_entry    SAVE
    ;

sampler_entry:
         sampler_addressu
    |   sampler_addressv
    |   sampler_addressw
    |   sampler_bordercolor
    |   sampler_magfilter
    |   sampler_minfilter
    |   sampler_mipfilter
    |   sampler_mipmaplodbias
    |   sampler_maxmiplevel
    |   sampler_maxanisotropy
    |   sampler_srgbtexture
    |   sampler_elementindex
    |   sampler_dmapoffset
    |   sampler_alphakill_deprecated
    |   sampler_colorkeyop_deprecated
    |   sampler_colorsign_deprecated
    |   sampler_colorkeycolor_deprecated
    |   error    '\n'
            {
                NSFParsererror("Syntax Error: sampler_entry");
                yyclearin;
            }
    ;

sampler_addressu:
        TSAMP_ADDRESSU    ASSIGN    sampler_texture_address
            {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_ADDRESSU;
                g_uiCurrStateValue = $3;
                if (!g_bUseMapValue && g_uiCurrStateValue == 
                    NSBStageAndSamplerStates::NID3DTADDRESS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    Deprecated value for TSAMP_ADDRESSU "
                        "SamplerState\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    g_bCurrStateValid = true;

                    NiSprintf(g_szDSO, 1024, "AddressU             = 0x%08x", 
                        (int)$3);
                    DebugStringOut(g_szDSO);
                }                
            }
    ;
    
sampler_addressv:
        TSAMP_ADDRESSV    ASSIGN    sampler_texture_address
            {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_ADDRESSV;
                g_uiCurrStateValue = $3;
                if (!g_bUseMapValue && g_uiCurrStateValue == 
                    NSBStageAndSamplerStates::NID3DTADDRESS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    Deprecated value for TSAMP_ADDRESSV "
                        "SamplerState\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    g_bCurrStateValid = true;

                    NiSprintf(g_szDSO, 1024, "AddressV             = 0x%08x", 
                        (int)$3);
                    DebugStringOut(g_szDSO);
                }
            }
    ;
    
sampler_addressw:
        TSAMP_ADDRESSW    ASSIGN    sampler_texture_address
            {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_ADDRESSW;
                g_uiCurrStateValue = $3;
                if (g_bUseMapValue)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        false, "* PARSE ERROR: %s\n"
                        "    UseMapValue not allowed for TSAMP_ADDRESSW "
                        "SamplerState\n"
                        "    at line %d\n",
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else if (g_uiCurrStateValue == 
                    NSBStageAndSamplerStates::NID3DTADDRESS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    Deprecated value for TSAMP_ADDRESSW "
                        "SamplerState\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    g_bCurrStateValid = true;

                    NiSprintf(g_szDSO, 1024, "AddressW             = 0x%08x", 
                        (int)$3);
                    DebugStringOut(g_szDSO);
                }                
            }
    ;
    
sampler_bordercolor:
        TSAMP_BORDERCOLOR    ASSIGN    N_HEX
            {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_BORDERCOLOR;
                g_uiCurrStateValue = $3;
                g_bCurrStateValid = true;
                
                NiSprintf(g_szDSO, 1024, "BorderColor         = 0x%08x", 
                    (int)$3);
                DebugStringOut(g_szDSO);
            }
    |   TSAMP_BORDERCOLOR    ASSIGN    attribute_name
            {
                NiSprintf(g_szDSO, 1024, "BorderColor         = %s", 
                    $3);
                DebugStringOut(g_szDSO);

                NiFree($3);
            }
    ;
    
sampler_magfilter:
        TSAMP_MAGFILTER    ASSIGN    sampler_texture_filter
            {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_MAGFILTER;
                g_uiCurrStateValue = $3;
                if (!g_bUseMapValue && g_uiCurrStateValue == 
                    NSBStageAndSamplerStates::NID3DTEXF_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    Deprecated value for TSAMP_MAGFILTER "
                        "SamplerState\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    g_bCurrStateValid = true;

                    NiSprintf(g_szDSO, 1024, "MagFilter          = 0x%08x", 
                        (int)$3);
                    DebugStringOut(g_szDSO);
                }
            }
    ;
            
sampler_minfilter:
        TSAMP_MINFILTER    ASSIGN    sampler_texture_filter
            {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_MINFILTER;
                g_uiCurrStateValue = $3;
                if (!g_bUseMapValue && g_uiCurrStateValue == 
                    NSBStageAndSamplerStates::NID3DTEXF_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    Deprecated value for TSAMP_MINFILTER "
                        "SamplerState\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    g_bCurrStateValid = true;
                    
                    NiSprintf(g_szDSO, 1024, "MinFilter          = 0x%08x", 
                        (int)$3);
                    DebugStringOut(g_szDSO);
                }
            }
    ;
    
sampler_mipfilter:
        TSAMP_MIPFILTER    ASSIGN    sampler_texture_filter
            {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_MIPFILTER;
                g_uiCurrStateValue = $3;
                if (!g_bUseMapValue && g_uiCurrStateValue == 
                    NSBStageAndSamplerStates::NID3DTEXF_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    Deprecated value for TSAMP_MIPFILTER "
                        "SamplerState\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    g_bCurrStateValid = true;
                    
                    NiSprintf(g_szDSO, 1024, "MipFilter          = 0x%08x", 
                        (int)$3);
                    DebugStringOut(g_szDSO);
                }
            }
    ;
    
sampler_mipmaplodbias:
        TSAMP_MIPMAPLODBIAS    ASSIGN    N_INT   
            {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_MIPMAPLODBIAS;
                g_uiCurrStateValue = (unsigned int)$3;
                g_bCurrStateValid = true;
                
                NiSprintf(g_szDSO, 1024, "MipMapLODBias      = %d", 
                    (int)$3);
                DebugStringOut(g_szDSO);
            }
    |   TSAMP_MIPMAPLODBIAS    ASSIGN    attribute_name
            {
                NiSprintf(g_szDSO, 1024, "MipMapLODBias      = %s", 
                    $3);
                DebugStringOut(g_szDSO);

                NiFree($3);
            }
    ;
    
sampler_maxmiplevel:
        TSAMP_MAXMIPLEVEL    ASSIGN    N_INT   
            {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_MAXMIPLEVEL;
                g_uiCurrStateValue = (unsigned int)$3;
                g_bCurrStateValid = true;
                
                NiSprintf(g_szDSO, 1024, "MaxMipLevel        = %d", 
                    (int)$3);
                DebugStringOut(g_szDSO);
            }
    |   TSAMP_MAXMIPLEVEL    ASSIGN    attribute_name
            {
                NiSprintf(g_szDSO, 1024, "MaxMipLevel        = %s", 
                    $3);
                DebugStringOut(g_szDSO);

                NiFree($3);
            }
    ;
    
sampler_maxanisotropy:
        TSAMP_MAXANISOTROPY    ASSIGN    N_INT   
            {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_MAXANISOTROPY;
                g_uiCurrStateValue = (unsigned int)$3;
                g_bCurrStateValid = true;
                
                NiSprintf(g_szDSO, 1024, "MaxAnisotropy      = %d", 
                    (int)$3);
                DebugStringOut(g_szDSO);
            }
    |   TSAMP_MAXANISOTROPY    ASSIGN    attribute_name
            {
                NiSprintf(g_szDSO, 1024, "MaxAnisotropy      = %s", 
                    $3);
                DebugStringOut(g_szDSO);

                NiFree($3);
            }
    ;
    
sampler_srgbtexture:
        TSAMP_SRGBTEXTURE    ASSIGN    N_INT   
            {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_SRGBTEXTURE;
                g_uiCurrStateValue = (unsigned int)$3;
                g_bCurrStateValid = true;
                
                NiSprintf(g_szDSO, 1024, "SRGBTexture        = %d", 
                    (int)$3);
                DebugStringOut(g_szDSO);
            }
    |   TSAMP_SRGBTEXTURE    ASSIGN    attribute_name
            {
                NiSprintf(g_szDSO, 1024, "SRGBTexture        = %s", 
                    $3);
                DebugStringOut(g_szDSO);

                NiFree($3);
            }
    ;
    
sampler_elementindex:
        TSAMP_ELEMENTINDEX    ASSIGN    N_INT   
            {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_ELEMENTINDEX;
                g_uiCurrStateValue = (unsigned int)$3;
                g_bCurrStateValid = true;
                
                NiSprintf(g_szDSO, 1024, "ElementIndex       = %d", 
                    (int)$3);
                DebugStringOut(g_szDSO);
            }
    |   TSAMP_ELEMENTINDEX    ASSIGN    attribute_name
            {
                NiSprintf(g_szDSO, 1024, "ElementIndex       = %s", 
                    $3);
                DebugStringOut(g_szDSO);

                NiFree($3);
            }
    ;
    
sampler_dmapoffset:
        TSAMP_DMAPOFFSET    ASSIGN    N_INT   
            {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_DMAPOFFSET;
                g_uiCurrStateValue = (unsigned int)$3;
                g_bCurrStateValid = true;
                
                NiSprintf(g_szDSO, 1024, "DMapOffset         = %d", 
                    (int)$3);
                DebugStringOut(g_szDSO);
            }
    |   TSAMP_DMAPOFFSET    ASSIGN    attribute_name
            {
                NiSprintf(g_szDSO, 1024, "DMapOffset         = %s", 
                    $3);
                DebugStringOut(g_szDSO);

                NiFree($3);
            }
    ;

sampler_alphakill_deprecated:    
        TSAMP_ALPHAKILL_DEPRECATED    ASSIGN    sampler_texture_alphakill
            {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_ALPHAKILL)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;
            }
    |   TSAMP_ALPHAKILL_DEPRECATED    ASSIGN    attribute_name
            {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_ALPHAKILL)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;

                NiFree($3);
            }
    ;

sampler_colorkeyop_deprecated:
        TSAMP_COLORKEYOP_DEPRECATED    ASSIGN    sampler_texture_colorkeyop
            {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_COLORKEYOP)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;
            }
    ;

sampler_colorsign_deprecated:
        TSAMP_COLORSIGN_DEPRECATED    ASSIGN    N_STRING
            {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_COLORSIGN)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;

                NiFree($3);
            }
    |   TSAMP_COLORSIGN_DEPRECATED    ASSIGN    '0'
            {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_COLORSIGN)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;
            }
    ;

sampler_colorkeycolor_deprecated:
        TSAMP_COLORKEYCOLOR_DEPRECATED    ASSIGN    N_HEX
            {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_COLORKEYCOLOR)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;
            }
    |   TSAMP_COLORKEYCOLOR_DEPRECATED    ASSIGN    attribute_name
            {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_COLORKEYCOLOR)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;

                NiFree($3);
            }
    ;

sampler_texture_address:
        TADDR_WRAP
            {   $$ = NSBStageAndSamplerStates::NID3DTADDRESS_WRAP;          }
    |   TADDR_MIRROR
            {   $$ = NSBStageAndSamplerStates::NID3DTADDRESS_MIRROR;        }
    |   TADDR_CLAMP
            {   $$ = NSBStageAndSamplerStates::NID3DTADDRESS_CLAMP;         }
    |   TADDR_BORDER
            {   $$ = NSBStageAndSamplerStates::NID3DTADDRESS_BORDER;        }
    |   TADDR_MIRRORONCE
            {   $$ = NSBStageAndSamplerStates::NID3DTADDRESS_MIRRORONCE;    }
    |   TADDR_CLAMPTOEDGE_DEPRECATED
            {   $$ = NSBStageAndSamplerStates::NID3DTADDRESS_INVALID;       }
    |   USEMAPVALUE
            {
                g_bUseMapValue = true;
                $$ = NSBStageAndSamplerStates::NID3DTADDRESS_INVALID;
            }
    |   error    '\n'
            {
                NSFParsererror("Syntax Error: sampler_texture_address");
                yyclearin;
            }
    ;
    
sampler_texture_filter:
        TEXF_NONE
            {   $$ = NSBStageAndSamplerStates::NID3DTEXF_NONE;              }
    |   TEXF_POINT
            {   $$ = NSBStageAndSamplerStates::NID3DTEXF_POINT;             }
    |   TEXF_LINEAR
            {   $$ = NSBStageAndSamplerStates::NID3DTEXF_LINEAR;            }
    |   TEXF_ANISOTROPIC
            {   $$ = NSBStageAndSamplerStates::NID3DTEXF_ANISOTROPIC;       }
    |   TEXF_PYRAMIDALQUAD
            {   $$ = NSBStageAndSamplerStates::NID3DTEXF_PYRAMIDALQUAD;     }
    |   TEXF_GAUSSIANQUAD
            {   $$ = NSBStageAndSamplerStates::NID3DTEXF_GAUSSIANQUAD;      }
    |   TEXF_QUINCUNX_DEPRECATED
            {   $$ = NSBStageAndSamplerStates::NID3DTEXF_INVALID;           }
    |   TEXF_FLATCUBIC_DEPRECATED
            {   $$ = NSBStageAndSamplerStates::NID3DTEXF_INVALID;           }
    |   TEXF_GAUSSIANCUBIC_DEPRECATED
            {   $$ = NSBStageAndSamplerStates::NID3DTEXF_INVALID;           }
    |   TEXF_MAX_DEPRECATED
            {   $$ = NSBStageAndSamplerStates::NID3DTEXF_INVALID;           }
    |   USEMAPVALUE
            {
                g_bUseMapValue = true;
                $$ = NSBStageAndSamplerStates::NID3DTEXF_INVALID;
            }
    |   error    '\n'
            {
                NSFParsererror("Syntax Error: sampler_texture_filter");
                yyclearin;
            }
    ;

sampler_texture_alphakill:
        TAK_DISABLE_DEPRECATED  {   $$ = 0x7fffffff;    }
    |   TAK_ENABLE_DEPRECATED   {   $$ = 0x7fffffff;    }
    |   error    '\n'
            {
                NSFParsererror("Syntax Error: sampler_texture_alphakill");
                yyclearin;
            }
    ;

sampler_texture_colorkeyop:
        TCKOP_DISABLE_DEPRECATED    {   $$ = 0x7fffffff;    }
    |   TCKOP_ALPHA_DEPRECATED      {   $$ = 0x7fffffff;    }
    |   TCKOP_RGBA_DEPRECATED       {   $$ = 0x7fffffff;    }
    |   TCKOP_KILL_DEPRECATED       {   $$ = 0x7fffffff;    }
    |   error    '\n'
            {
                NSFParsererror("Syntax Error: sampler_texture_colorkeyop");
                yyclearin;
            }
    ;

//---------------------------------------------------------------------------
// Textures
//---------------------------------------------------------------------------
texture:
        TOKEN_TEXTURE    N_INT   optional_string
        L_ACCOLADE
            {
                NiSprintf(g_szDSO, 1024, "Texture Start %3d - %s\n", 
                    $2, $3);
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrPass)
                {
                    g_pkCurrTexture = g_pkCurrPass->GetTexture((int)$2);
                    g_pkCurrTexture->SetName($3);
                }
            }
            texture_entry_list_optional
        R_ACCOLADE
            {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Texture End %3d - %s\n", 
                    (int)$2, $3);
                DebugStringOut(g_szDSO);

                g_pkCurrTexture = 0;

                NiFree($3);
            }
    |   TOKEN_TEXTURE    optional_string
        L_ACCOLADE
            {
                NiSprintf(g_szDSO, 1024, "Texture Start - %s\n", $2);
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrPass)
                {
                    g_pkCurrTexture = 
                        g_pkCurrPass->GetTexture(g_pkCurrPass->GetTextureCount());
                    g_pkCurrTexture->SetName($2);
                }
            }
            texture_entry_list_optional
        R_ACCOLADE
            {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Texture End %s\n", $2);
                DebugStringOut(g_szDSO);

                g_pkCurrTexture = 0;

                NiFree($2);
            }
    ;

texture_entry_list_optional:
        /* empty */
    |   texture_entry_list
    ;
    
texture_entry_list:
        texture_entry_list    texture_entry
    |   texture_entry
    ;

texture_entry:
    texture_source
    |   error    '\n'
            {
                NSFParsererror("Syntax Error: texture_entry");
                yyclearin;
            }
    ;
    
texture_source:
        TEXTURE_SOURCE  ASSIGN  stage_texture_map_ndl
            {
                if (g_pkCurrTexture)
                    g_pkCurrTexture->SetSource($3);
            }
    |   TEXTURE_SOURCE  ASSIGN  stage_texture_map_ndl_decal
            {
                if (g_pkCurrTexture)
                    g_pkCurrTexture->SetSourceDecal($3);
            }
    |   TEXTURE_SOURCE ASSIGN  attribute_name
            {
                bool bFoundAttribute = false;
                NSBObjectTable* pkObjectTable = 0;
                if (g_pkCurrNSBShader)
                {
                    g_bGlobalAttributes = false;
                    g_pkCurrAttribTable = 
                        g_pkCurrNSBShader->GetAttributeTable();
                    pkObjectTable = g_pkCurrNSBShader->GetObjectTable();
                }
                if (g_pkCurrAttribTable)
                {
                    NSBAttributeDesc* pkAttrib = 
                        g_pkCurrAttribTable->GetAttributeByName($3);
                    if (pkAttrib)
                    {
                        bFoundAttribute = true;
                        
                        unsigned int uiValue;
                        const char* pszValue;
                        
                        if (pkAttrib->GetValue_Texture(uiValue, pszValue))
                        {
                            uiValue |= NiTextureStage::TSTF_MAP_SHADER;
                            if (g_pkCurrTexture)
                                g_pkCurrTexture->SetSourceShader(uiValue);
                        }
                        else
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, true,
                                "* PARSE ERROR: %s\n"
                                "    GetValue_Texture at line %d\n"
                                "    Attribute name = %s\n",
                                g_pkFile->GetFilename(),
                                NSFParserGetLineNumber(),
                                $3);
                        }
                    }
                    g_pkCurrAttribTable = 0;
                }
                
                if (!bFoundAttribute && pkObjectTable)
                {
                    NSBObjectTable::ObjectDesc* pkDesc =
                        pkObjectTable->GetObjectByName($3);
                    if (pkDesc)
                    {
                        NiShaderAttributeDesc::ObjectType eObjectType =
                            pkDesc->GetType();
                        if (eObjectType <
                            NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP ||
                            eObjectType >
                            NiShaderAttributeDesc::OT_EFFECT_FOGMAP)
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, true,
                                "* PARSE ERROR: %s\n"
                                "    InvalidObjectType at line %d\n"
                                "    Object name = %s\n",
                                g_pkFile->GetFilename(),
                                NSFParserGetLineNumber(),
                                $3);
                        }
                        else
                        {
                            if (g_pkCurrTexture)
                            {
                                g_pkCurrTexture->SetSourceObject(
                                    eObjectType, pkDesc->GetIndex());
                            }
                        }
                    }
                    else
                    {
                        NiShaderFactory::ReportError(
                            NISHADERERR_UNKNOWN, true,
                            "* PARSE ERROR: %s\n"
                            "    TextureNotFound at line %d\n"
                            "    Attribute/Object name = %s\n",
                            g_pkFile->GetFilename(),
                            NSFParserGetLineNumber(),
                            $3);
                    }
                }
                
                NiFree($3);
            }
    |   TEXTURE_SOURCE ASSIGN  N_STRING
            {           
                NSBObjectTable::ObjectDesc* pkDesc = NULL;
                NSBObjectTable* pkTable = g_pkCurrNSBShader->GetObjectTable();
                if (pkTable)
                {
                    pkDesc = pkTable->GetObjectByName($3);
                }
                if (!pkDesc)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        false,
                        "* PARSE ERROR: %s\n"
                        "    GetObjectByName at line %d\n"
                        "    Local name = %s\n",
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber(),
                        $3);
                    NiFree($3);                             
                    break;
                }
                if (g_pkCurrTexture)
                {
                    g_pkCurrTexture->SetSourceObject(
                        pkDesc->GetType(), pkDesc->GetIndex());
                }
                        
                NiFree($3);             
            }            
    ;

//---------------------------------------------------------------------------
// Passes
//---------------------------------------------------------------------------
pass_component_list:
        pass_component_list    pass_component
    |   pass_component
    ;
    
pass_component:
        renderstate_list_with_brackets
    |   vertexshader_program_entry
    |   vs_constantmap_with_brackets
    |   geometryshader_program_entry
    |   gs_constantmap_with_brackets
    |   pixelshader_program_entry
    |   ps_constantmap_with_brackets
    |   stage_or_sampler_or_texture
    |   userdefineddata_block
    |   streamoutput
    ;

pass:
        PASS    optional_string
        L_ACCOLADE
            {
                NiSprintf(g_szDSO, 1024, "Pass Start %s\n", $2);
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                
                if (g_pkCurrImplementation)
                {
                    g_pkCurrPass = 
                        g_pkCurrImplementation->GetPass(g_uiCurrPassIndex);
                }
            }
            pass_component_list
        R_ACCOLADE
            {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Pass End %s\n", $2);
                DebugStringOut(g_szDSO);

                g_pkCurrPass = 0;
                g_uiCurrPassVSConstantMap = 0;
                g_uiCurrPassGSConstantMap = 0;
                g_uiCurrPassPSConstantMap = 0;
                g_uiCurrPassIndex++;

                NiFree($2);
            }
    ;

//---------------------------------------------------------------------------
// Implementations
//---------------------------------------------------------------------------
implementation_component_list_optional:
        /* empty */
    |   implementation_component_list
    ;
    
implementation_component_list:
        implementation_component_list    implementation_component
    |   implementation_component
    ;
    
implementation_component:
        renderstate_list_with_brackets
    |   packing_definition_declaration
    |   pass
    |   requirement_list_with_brackets
    |   vs_constantmap_with_brackets
    |   gs_constantmap_with_brackets
    |   ps_constantmap_with_brackets
    |   userdefined_classname
    |   userdefineddata_block
    |   semantic_adapter_table_declaration
    ;
    
implementation:
        IMPLEMENTATION    N_STRING
        L_ACCOLADE
            {
                NiSprintf(g_szDSO, 1024, "Implementation Start %s\n", $2);
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrNSBShader)
                {
                    g_pkCurrImplementation = 
                        g_pkCurrNSBShader->GetImplementation($2, 
                            true, g_uiCurrImplementation);
                    if (g_pkCurrImplementation->GetIndex() == 
                        g_uiCurrImplementation)
                    {
                        g_uiCurrImplementation++;
                    }
                    g_uiCurrPassIndex = 0;
                }                    
            }
            optional_description
                {
                    NiSprintf(g_szDSO, 1024, "Description: %s\n", $5);
                    DebugStringOut(g_szDSO);
                    
                    if (g_pkCurrImplementation)
                        g_pkCurrImplementation->SetDesc($5);
                    NiFree($5);
                }
            implementation_component_list_optional
        R_ACCOLADE
            {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Implementation End %s\n", $2);
                DebugStringOut(g_szDSO);

                g_pkCurrImplementation = 0;
                g_uiCurrImplemVSConstantMap = 0;
                g_uiCurrImplemGSConstantMap = 0;
                g_uiCurrImplemPSConstantMap = 0;
                
                NiFree($2);
            }
    ;

//---------------------------------------------------------------------------
// StreamOutput Descriptor (exists on NSBImplementation)
//---------------------------------------------------------------------------
streamoutput:
        STREAMOUTPUT
        L_ACCOLADE
        {
            NIASSERT(g_pkCurrPass);
        }
        streamoutput_component_list
        R_ACCOLADE
    ;
streamoutput_component_list:
        streamoutput_component_list streamoutput_component
    |   streamoutput_component
    ;
streamoutput_component:
        streamouttargets
    |   streamoutappend
    ;
streamoutappend:
        STREAMOUTAPPEND
        ASSIGN 
        N_BOOL
            {
                NIASSERT(g_pkCurrPass);
                NiStreamOutSettings& kStreamOutSettings = 
                    g_pkCurrPass->GetStreamOutSettings();
                kStreamOutSettings.SetStreamOutAppend($3);
            }
        ;
streamouttargets:
        STREAMOUTTARGETS        
        L_ACCOLADE
        streamoutputtarget_list
        R_ACCOLADE
        ;
streamoutputtarget_list:
        streamoutputtarget_list streamoutputtarget
    |   streamoutputtarget
        ;
streamoutputtarget:
        N_STRING
            {
                NIASSERT(g_pkCurrPass);
                NiStreamOutSettings& kStreamOutSettings = 
                    g_pkCurrPass->GetStreamOutSettings();
                kStreamOutSettings.AppendStreamOutTargets($1);
                NiFree($1);
            }
        ;
        

//---------------------------------------------------------------------------
// OutputStream Descriptors (exist on NSBShader)
//---------------------------------------------------------------------------
outputstream:
        OUTPUTSTREAM
        N_STRING
        L_ACCOLADE
            {
                // Create a new OSD and set its name
                g_pkCurrentOutputStreamDescriptor = 
                NiNew NiOutputStreamDescriptor;
                g_pkCurrentOutputStreamDescriptor->SetName($2);
                
                NiFree($2);
            }
        outputstream_component_list
        R_ACCOLADE
            {
                // Add the new OSD
                g_pkCurrNSBShader->AddOutputStreamDescriptor(
                    *g_pkCurrentOutputStreamDescriptor);
            
                NiDelete g_pkCurrentOutputStreamDescriptor;
                g_pkCurrentOutputStreamDescriptor = NULL;
            }
        ;
outputstream_component_list:
        outputstream_component_list outputstream_component
    |   outputstream_component
    ;
outputstream_component:
        maxvertexcount
    |   outputprimtype
    |   vertexformat
        ;    
maxvertexcount:
        MAXVERTEXCOUNT ASSIGN N_INT
            {
                // note: 0 = same # of verts as in source mesh.
                g_pkCurrentOutputStreamDescriptor->SetMaxVertexCount($3);
            }
        ;

vertexformat:
        VERTEXFORMAT L_ACCOLADE    
            {
                g_eDataType = NiOutputStreamDescriptor::DATATYPE_MAX;
            }        
        vertexformat_entry_list
        R_ACCOLADE
            {
                g_eDataType = NiOutputStreamDescriptor::DATATYPE_MAX;
            }        
        ;
vertexformat_entry_list:
        vertexformat_entry_list vertexformat_entry
    |   vertexformat_entry
    ;
vertexformat_entry:
        // A single vertex format entry of the form:
        //      <data type> <component count> <semantic name> : <semantic idx>
        vertexformat_entry_datatype      /* Data type = $1 */
        N_INT                            /* ComponentCount = $2 */
        string_or_quote NSF_COLON N_INT  /* Semantic Name : Index = $3 $4 $5 */
        NSF_SEMICOLON                    /* ; = $6 */
            {
                NiSprintf(g_szDSO, 1024, "vertex format entry [%d elements "
                    "using %s,%d]\n", $2, $3, $5);
                DebugStringOut(g_szDSO);

                NiOutputStreamDescriptor::VertexFormatEntry kEntry;
                
                NIASSERT(
                    g_eDataType != NiOutputStreamDescriptor::DATATYPE_MAX);
                kEntry.m_eDataType = g_eDataType;
                kEntry.m_uiComponentCount = ($2);
                kEntry.m_kSemanticName = ($3);
                kEntry.m_uiSemanticIndex = ($5);
                g_pkCurrentOutputStreamDescriptor->AppendVertexFormat(kEntry);

                NiFree($3);
            }
    ;

vertexformat_entry_datatype:
        vertexformat_entry_datatype_float 
    |   vertexformat_entry_datatype_int
    |   vertexformat_entry_datatype_uint
        ;    
vertexformat_entry_datatype_float:
        FMT_FLOAT
            {
                g_eDataType = NiOutputStreamDescriptor::DATATYPE_FLOAT;
            }
        ;
vertexformat_entry_datatype_int:
        FMT_INT
            {
                g_eDataType = NiOutputStreamDescriptor::DATATYPE_INT;
            }
        ;
vertexformat_entry_datatype_uint:
        FMT_UINT
            {
                g_eDataType = NiOutputStreamDescriptor::DATATYPE_UINT;
            }
        ;

outputprimtype:        
        OUTPUTPRIMTYPE ASSIGN outputprimtype2
        ;
outputprimtype2:
        outputprimtype2_POINT
    |   outputprimtype2_LINE
    |   outputprimtype2_TRIANGLE
    ;
outputprimtype2_POINT:
    _POINT
            {
                g_pkCurrentOutputStreamDescriptor->SetPrimType(
                    NiPrimitiveType::PRIMITIVE_POINTS);
            }
    ;
outputprimtype2_LINE:
    _LINE
            {
                g_pkCurrentOutputStreamDescriptor->SetPrimType(
                    NiPrimitiveType::PRIMITIVE_LINES);
            }
    ;
outputprimtype2_TRIANGLE:
    _TRIANGLE
            {
                g_pkCurrentOutputStreamDescriptor->SetPrimType(
                    NiPrimitiveType::PRIMITIVE_TRIANGLES);
            }
    ;
    

//---------------------------------------------------------------------------
// UserDefinedData
//---------------------------------------------------------------------------
userdefineddata:
        string_or_quote ASSIGN  N_INT
            {
                NiSprintf(g_szDSO, 1024, "Integer............%s\n", $1);
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                NiSprintf(g_szDSO, 1024, "%d\n", $3);
                DebugStringOut(g_szDSO);
                g_iDSOIndent -= 4;
                
                if (g_pkCurrUDDataBlock)
                {
                    unsigned int uiValue = (unsigned int)$3;
                    unsigned int uiFlags = 
                        NiShaderConstantMapEntry::SCME_MAP_ATTRIBUTE | 
                        NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
                    if (!g_pkCurrUDDataBlock->AddEntry($1, 
                        uiFlags, sizeof(unsigned int), sizeof(unsigned int), 
                        (void*)&uiValue, true))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    UserDefinedData at line %d\n"
                            "    Name = %s\n"
                            "    Failed to add!\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), $1);
                    }
                }                

                NiFree($1);
            }
    |   string_or_quote ASSIGN  N_BOOL
            {
                NiSprintf(g_szDSO, 1024, "Boolean............%s\n", $1);
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                NiSprintf(g_szDSO, 1024, "%s\n", $3 ? "TRUE" : "FALSE");
                DebugStringOut(g_szDSO);
                g_iDSOIndent -= 4;

                if (g_pkCurrUDDataBlock)
                {
                    bool bValue = $3 ? true : false;
                    unsigned int uiFlags = 
                        NiShaderConstantMapEntry::SCME_MAP_ATTRIBUTE | 
                        NiShaderAttributeDesc::ATTRIB_TYPE_BOOL;
                    if (!g_pkCurrUDDataBlock->AddEntry($1, 
                        uiFlags, sizeof(bool), sizeof(bool), (void*)&bValue, 
                        true))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    UserDefinedData at line %d\n"
                            "    Name = %s\n"
                            "    Failed to add!\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), $1);
                    }
                }                

                NiFree($1);
            }
    |   string_or_quote ASSIGN  float_values_arbitrary_list
            {
                NiSprintf(g_szDSO, 1024, "Floats (%2d)........%s\n", 
                    g_afValues->GetSize(), $1);
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                for (unsigned int ui = 0; ui < g_afValues->GetSize(); ui++)
                {
                    if ((ui % 4) == 0)
                        DebugStringOut("");
                    NiSprintf(g_szDSO, 1024, "%-8.5f", g_afValues->GetAt(ui));
                    if ((((ui + 1) % 4) == 0) ||
                        (ui + 1 == g_afValues->GetSize()))
                    {
                        NiStrcat(g_szDSO, 1024, "\n");
                    }
                    DebugStringOut(g_szDSO, false);
                }
                g_iDSOIndent -= 4;
                
                if (g_pkCurrUDDataBlock)
                {
                    unsigned int uiFlags = 0;
                    
                    switch (g_afValues->GetSize())
                    {
                    case 1:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
                        break;
                    case 2:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
                        break;
                    case 3:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
                        break;
                    case 4:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
                        break;
                    case 8:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT8;
                        break;
                    case 9:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
                        break;
                    case 12:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT12;
                        break;
                    case 16:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
                        break;
                    default:
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            false, "* %s(%d): Unsupported or unexpected "
                            "attribute size\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber());
                        break;
                    }
                    uiFlags |= 
                        NiShaderConstantMapEntry::SCME_MAP_ATTRIBUTE;
                    
                    if (uiFlags == 0)
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    UserDefinedData at line %d\n"
                            "    Name = %s\n"
                            "    Invalid number of floats!\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), $1);
                    }
                    else if (!g_pkCurrUDDataBlock->AddEntry($1, 
                        uiFlags, sizeof(float) * g_afValues->GetSize(), 
                        sizeof(float), (void*)g_afValues->GetBase(), true))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    UserDefinedData at line %d\n"
                            "    Name = %s\n"
                            "    Failed to add!\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), $1);
                    }
                }                

                // Reset the float arrays, so any entries that follow this
                // one will be handled correctly
                ResetFloatValueArray();
                ResetFloatRangeArrays();

                NiFree($1);
            }
    |   string_or_quote ASSIGN  string_or_quote
            {
                NiSprintf(g_szDSO, 1024, "String.............%s\n", $1);
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                NiSprintf(g_szDSO, 1024, "%s\n", $3);
                DebugStringOut(g_szDSO);
                g_iDSOIndent -= 4;
                
                if (g_pkCurrUDDataBlock)
                {
                    unsigned int uiFlags = 
                        NiShaderConstantMapEntry::SCME_MAP_ATTRIBUTE | 
                        NiShaderAttributeDesc::ATTRIB_TYPE_STRING;
                    if (!g_pkCurrUDDataBlock->AddEntry($1, 
                        uiFlags, sizeof(char) * strlen($3), 
                        sizeof(char) * strlen($3), 
                        (void*)$3, true))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    UserDefinedData at line %d\n"
                            "    Name = %s\n"
                            "    Failed to add!\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), $1);
                    }
                }                

                NiFree($1);
                NiFree($3);
            }
    ;
    
userdefineddata_list:
        userdefineddata
    |   userdefineddata userdefineddata_list
    ;
    
userdefineddata_block:
        USERDEFINEDDATA N_STRING
        L_ACCOLADE
            {
                NiSprintf(g_szDSO, 1024, "UserDefinedDataBlock Start %s\n", 
                    $2);
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                // Reset the float arrays, then each one will reset them
                // when they are done being processed
                ResetFloatValueArray();
                ResetFloatRangeArrays();

                if (g_pkCurrPass)
                {
                    g_pkCurrUDDataSet = 
                        g_pkCurrPass->GetUserDefinedDataSet();
                }
                else
                if (g_pkCurrImplementation)
                {
                    g_pkCurrUDDataSet = 
                        g_pkCurrImplementation->GetUserDefinedDataSet();
                }
                else
                if (g_pkCurrNSBShader)
                {
                    g_pkCurrUDDataSet = 
                        g_pkCurrNSBShader->GetUserDefinedDataSet();
                }

                if (!g_pkCurrUDDataSet)
                {
                    g_pkCurrUDDataSet = NiNew NSBUserDefinedDataSet();
                }

                if (!g_pkCurrUDDataSet)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* ALLOCATION ERROR: %s\n"
                        "    UserDefinedDataSet creation at line %d\n", 
                        g_pkFile->GetFilename(), 
                        NSFParserGetLineNumber());
                }
                else
                {
                    if (g_pkCurrPass)
                    {
                        g_pkCurrPass->SetUserDefinedDataSet(
                            g_pkCurrUDDataSet);
                    }
                    else
                    if (g_pkCurrImplementation)
                    {
                        g_pkCurrImplementation->SetUserDefinedDataSet(
                            g_pkCurrUDDataSet);
                    }
                    else
                    if (g_pkCurrNSBShader)
                    {
                        g_pkCurrNSBShader->SetUserDefinedDataSet(
                            g_pkCurrUDDataSet);
                    }
                }
                
                if (g_pkCurrUDDataSet)
                {
                    g_pkCurrUDDataBlock = 
                        g_pkCurrUDDataSet->GetBlock($2, false);
                    if (g_pkCurrUDDataBlock)
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    UserDefinedDataBlock at line %d\n"
                            "    Name = %s\n"
                            "    ALREADY EXISTS!\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), $2);
                    }
                    else
                    {
                        g_pkCurrUDDataBlock = 
                            g_pkCurrUDDataSet->GetBlock($2, true);
                    }
                    NIASSERT(g_pkCurrUDDataBlock);
                }
            }
        userdefineddata_list
        R_ACCOLADE
            {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "UserDefinedDataBlock End %s\n", 
                    $2);
                DebugStringOut(g_szDSO);

                g_pkCurrUDDataSet = 0;
                g_pkCurrUDDataBlock = 0;

                NiFree($2);
            }
    ;

//---------------------------------------------------------------------------

%%

//---------------------------------------------------------------------------
int iErrors = 0;
extern void NSFParserResetLineNumber();
extern void NSFParserReset(); 
extern void NSFParserReleaseBuffer();
//---------------------------------------------------------------------------
int ParseShader(const char* pszFileName)
{ 
    NSFParserResetLineNumber();
    g_pkFile = NiNew NSFTextFile();

    NSFParserlval.fval = 0.0f;
    NSFParserlval.ival = 0;
    NSFParserlval.sval = 0;
    NSFParserlval.dword = 0;
    NSFParserlval.word = 0;
    NSFParserlval.byte = 0;
    NSFParserlval.bval = false;

    if (!pszFileName)
    {
        NiDelete g_pkFile;
        NiSprintf(g_szDSO, 1024, "err: need input file\n");
        DebugStringOut(g_szDSO);
        return -1;
    };

    NSFParsedShader* pkParsedShader;
    NiTListIterator    pos = g_kParsedShaderList.GetHeadPos();
    while (pos)
    {
        pkParsedShader = g_kParsedShaderList.GetNext(pos);
        NiDelete pkParsedShader;
    }
    g_kParsedShaderList.RemoveAll();
    g_pkCurrShader = 0;

    if (g_pkFile->Load(pszFileName) != 0)
    {
        NiDelete g_pkFile;
        NiSprintf(g_szDSO, 1024, "err: file not found!\n");
        DebugStringOut(g_szDSO);
        return -1;
    }

    // create array
    g_afValues = NiNew NiTPrimitiveArray<float>;

    iErrors = -1;
    yyparse ();
    if (iErrors != -1)
    {
        iErrors++;
        printf ( "*FAILURE! %d errors found.\n" , iErrors);
        NiSprintf(g_szDSO, 1024, "*FAILURE! %d errors found.\n" , iErrors);
        DebugStringOut(g_szDSO);

        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
            false, "* PARSE ERROR: %s\n"
            "    FAILED - %d errors found\n",
            g_pkFile->GetFilename(), iErrors);
    }
        
    NiDelete g_afValues;
    NiDelete g_pkFile;
    g_pkFile = 0;

    if (iErrors == -1)    
        return 0;

    return iErrors;
}
//---------------------------------------------------------------------------
void ResetParser()
{
    NSFParserReset();
} 
//---------------------------------------------------------------------------
void CleanupParser()
{
    NSFParserReleaseBuffer();
}
//---------------------------------------------------------------------------
void DebugStringOut(const char* pszOut, bool bIndent)
{
    NI_UNUSED_ARG(bIndent);
    NI_UNUSED_ARG(pszOut);
#if defined(_ENABLE_DEBUG_STRING_OUT_)
    if (g_bFirstDSOFileAccess)
    {
        g_pfDSOFile = fopen("NSFShaderParser.out", "wt");
        g_bFirstDSOFileAccess = false;
    }
    else
    {
        g_pfDSOFile = fopen("NSFShaderParser.out", "at");
    }

    if (bIndent)
    {
        for (int ii = 0; ii < g_iDSOIndent; ii++)
        {
            printf(" ");
            NiOutputDebugString(" ");
            if (g_pfDSOFile)
                fprintf(g_pfDSOFile, " ");
        }
    }
    printf(pszOut);
    NiOutputDebugString(pszOut);
    if (g_pfDSOFile)
    {
        fprintf(g_pfDSOFile, pszOut);
        fclose(g_pfDSOFile);
    }
#endif    //#if defined(_ENABLE_DEBUG_STRING_OUT_)
}
//---------------------------------------------------------------------------
unsigned int ResetFloatValueArray(void)
{
    g_afValues->RemoveAll();
    return g_afValues->GetSize();
}
//---------------------------------------------------------------------------
unsigned int AddFloatToValueArray(float fValue)
{
    g_afValues->Add(fValue);
    return g_afValues->GetSize();
}
//---------------------------------------------------------------------------
void ResetFloatRangeArrays(void)
{
    g_uiLowFloatValues    = 0;
    g_uiHighFloatValues    = 0;

    g_afLowValues[0] = 0.0f;
    g_afLowValues[1] = 0.0f;
    g_afLowValues[2] = 0.0f;
    g_afLowValues[3] = 0.0f;
    g_afHighValues[0] = 0.0f;
    g_afHighValues[1] = 0.0f;
    g_afHighValues[2] = 0.0f;
    g_afHighValues[3] = 0.0f;
}
//---------------------------------------------------------------------------
unsigned int AddFloatToLowArray(float fValue)
{
    if ((g_uiLowFloatValues + 1) == FLOAT_ARRAY_SIZE)
    {
        DebugStringOut("Low Float array overflow!");
        return 0;
    }
    g_afLowValues[g_uiLowFloatValues++] = fValue;
    return g_uiLowFloatValues;
}
//---------------------------------------------------------------------------
unsigned int AddFloatToHighArray(float fValue)
{
    if ((g_uiHighFloatValues + 1) == FLOAT_ARRAY_SIZE)
    {
        DebugStringOut("High Float array overfHigh!");
        return 0;
    }
    g_afHighValues[g_uiHighFloatValues++] = fValue;
    return g_uiHighFloatValues;
}
//---------------------------------------------------------------------------
void AddObjectToObjectTable(NiShaderAttributeDesc::ObjectType eType,
    unsigned int uiIndex, const char* pcName, const char* pcDebugString)
{
    if (g_pkCurrObjectTable)
    {
        if (!g_pkCurrObjectTable->AddObject(pcName, eType, uiIndex))
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                true, "* PARSE ERROR: %s\n"
                "    AddObject at line %d\n"
                "    Object name = %s\n",
                g_pkFile->GetFilename(),
                NSFParserGetLineNumber(), pcName);
        }
    }
    NiSprintf(g_szDSO, 1024, "    %24s: %d - %16s\n", pcDebugString, uiIndex,
        pcName);
    DebugStringOut(g_szDSO);
}
//---------------------------------------------------------------------------
unsigned int DecodeAttribTypeString(char* pszAttribType)
{
    if (!pszAttribType || pszAttribType[0] == '\0')
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
        
    // We need to look up the attribute in the attribute map, and then
    // return the type
    if (!g_pkCurrNSBShader)
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;

    // Local attributes override global!
    g_pkCurrAttribTable = g_pkCurrNSBShader->GetAttributeTable();
    if (!g_pkCurrAttribTable)
    {
        NIASSERT(!"Invalid attribute table during parse!");
        return 0;
    }
    
    NSBAttributeDesc* pkAttribDesc = 
        g_pkCurrAttribTable->GetAttributeByName(pszAttribType);
    if (!pkAttribDesc)
    {
        g_pkCurrAttribTable = g_pkCurrNSBShader->GetGlobalAttributeTable();
        if (!g_pkCurrAttribTable)
        {
            NIASSERT(!"Invalid attribute table during parse!");
            return 0;
        }
        pkAttribDesc = g_pkCurrAttribTable->GetAttributeByName(pszAttribType);
        if (!pkAttribDesc)
        {
            NIASSERT(!"Attribute not found!");
            return 0;
        }
    }
    
    g_pkCurrAttribTable = 0;
    
    return (unsigned int)(pkAttribDesc->GetType());
}
//---------------------------------------------------------------------------
unsigned int DecodePlatformString(char* pszPlatform)
{
    if (!pszPlatform || pszPlatform[0] == '\0')
        return 0;
        
    char acTemp[64];

    unsigned int ui = 0;
    for (; ui < strlen(pszPlatform); ui++)
        acTemp[ui] = toupper(pszPlatform[ui]);
    acTemp[ui] = 0;
    
    if (NiStricmp(acTemp, "DX9") == 0)
        return NiShader::NISHADER_DX9;
    if (NiStricmp(acTemp, "XENON") == 0)
        return NiShader::NISHADER_XENON;
    if (NiStricmp(acTemp, "PS3") == 0)
        return NiShader::NISHADER_PS3;
    if (NiStricmp(acTemp, "D3D10") == 0)
        return NiShader::NISHADER_D3D10;

    return 0;
}
//---------------------------------------------------------------------------
void SetShaderProgramFile(NSBPass* pkPass, const char* pcFile,
    unsigned int uiPlatforms, NiGPUProgram::ProgramType eType)
{
    if (!pkPass)
        return;

    if (uiPlatforms & NiShader::NISHADER_DX9)
    {
        pkPass->SetShaderProgramFile(pcFile,
            NiSystemDesc::RENDERER_DX9, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_D3D10)
    {
        pkPass->SetShaderProgramFile(pcFile,
            NiSystemDesc::RENDERER_D3D10, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_PS3)
    {
        pkPass->SetShaderProgramFile(pcFile,
            NiSystemDesc::RENDERER_PS3, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_XENON)
    {
        pkPass->SetShaderProgramFile(pcFile,
            NiSystemDesc::RENDERER_XENON, eType);
    }
}
//---------------------------------------------------------------------------
void SetShaderProgramEntryPoint(NSBPass* pkPass, const char* pcEntryPoint,
    unsigned int uiPlatforms, NiGPUProgram::ProgramType eType)
{
    if (!pkPass)
        return;

    if (uiPlatforms & NiShader::NISHADER_DX9)
    {
        pkPass->SetShaderProgramEntryPoint(pcEntryPoint,
            NiSystemDesc::RENDERER_DX9, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_D3D10)
    {
        pkPass->SetShaderProgramEntryPoint(pcEntryPoint,
            NiSystemDesc::RENDERER_D3D10, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_PS3)
    {
        pkPass->SetShaderProgramEntryPoint(pcEntryPoint,
            NiSystemDesc::RENDERER_PS3, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_XENON)
    {
        pkPass->SetShaderProgramEntryPoint(pcEntryPoint,
            NiSystemDesc::RENDERER_XENON, eType);
    }
}
//---------------------------------------------------------------------------
void SetShaderProgramShaderTarget(NSBPass* pkPass,
    const char* pcShaderTarget, unsigned int uiPlatforms,
    NiGPUProgram::ProgramType eType)
{
    if (!pkPass)
        return;

    if (uiPlatforms & NiShader::NISHADER_DX9)
    {
        pkPass->SetShaderProgramShaderTarget(pcShaderTarget,
            NiSystemDesc::RENDERER_DX9, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_D3D10)
    {
        pkPass->SetShaderProgramShaderTarget(pcShaderTarget,
            NiSystemDesc::RENDERER_D3D10, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_PS3)
    {
        pkPass->SetShaderProgramShaderTarget(pcShaderTarget,
            NiSystemDesc::RENDERER_PS3, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_XENON)
    {
        pkPass->SetShaderProgramShaderTarget(pcShaderTarget,
            NiSystemDesc::RENDERER_XENON, eType);
    }
}
//---------------------------------------------------------------------------
bool AddAttributeToConstantMap(char* pszName, 
    unsigned int uiRegisterStart, unsigned int uiRegisterCount, 
    unsigned int uiExtraNumber, bool bIsGlobal)
{
    if (!g_pkCurrConstantMap)
        return false;
    
    // Cheat to force a copy of the data
    unsigned int uiFlags = bIsGlobal
        ? NiShaderConstantMapEntry::SCME_MAP_GLOBAL
        : NiShaderConstantMapEntry::SCME_MAP_ATTRIBUTE;

    unsigned int uiSize;

    // Look up the global attribute
    if (!g_pkCurrNSBShader)
    {
        return false;
    }
    else
    {
        if (bIsGlobal)
        {
            g_pkCurrAttribTable = 
                g_pkCurrNSBShader->GetGlobalAttributeTable();
        }
        else
        {
            g_pkCurrAttribTable = 
                g_pkCurrNSBShader->GetAttributeTable();
        }

        NSBAttributeDesc* pkAttribDesc = 
            g_pkCurrAttribTable->GetAttributeByName(pszName);

        if (!pkAttribDesc)
        {
            return false;
        }
        else
        {
            if (NiShaderConstantMapEntry::IsBool(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_BOOL;
                uiSize = sizeof(bool);
                bool bValue;
                
                pkAttribDesc->GetValue_Bool(bValue);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber, 
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)&bValue, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags, 
                        uiExtraNumber, uiRegisterStart, uiRegisterCount, 
                        (char*)0, uiSize, uiSize, (void*)&bValue, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsUnsignedInt(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
                uiSize = sizeof(unsigned int);
                unsigned int uiValue;
                
                pkAttribDesc->GetValue_UnsignedInt(uiValue);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber, 
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)&uiValue, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags, 
                        uiExtraNumber, uiRegisterStart, uiRegisterCount, 
                        (char*)0, uiSize, uiSize, (void*)&uiValue, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsFloat(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
                uiSize = sizeof(float);
                float fValue;
                
                pkAttribDesc->GetValue_Float(fValue);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber, 
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)&fValue, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags, 
                        uiExtraNumber, uiRegisterStart, uiRegisterCount, 
                        (char*)0, uiSize, uiSize, (void*)&fValue, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsPoint2(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
                uiSize = sizeof(NiPoint2);
                NiPoint2 kPt2Value;
                
                pkAttribDesc->GetValue_Point2(kPt2Value);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber, 
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)&kPt2Value, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags, 
                        uiExtraNumber, uiRegisterStart, uiRegisterCount, 
                        (char*)0, uiSize, uiSize, (void*)&kPt2Value, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsPoint3(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
                uiSize = sizeof(NiPoint3);
                NiPoint3 kPt3Value;
                
                pkAttribDesc->GetValue_Point3(kPt3Value);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber,
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)&kPt3Value, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags,
                        uiExtraNumber, uiRegisterStart, uiRegisterCount, 
                        (char*)0, uiSize, uiSize, (void*)&kPt3Value, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsPoint4(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
                uiSize = sizeof(float) * 4;
                float afValue[4];
                float* pfValue = &afValue[0];
                
                pkAttribDesc->GetValue_Point4(pfValue);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber, 
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)afValue, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags,
                        uiExtraNumber, uiRegisterStart, uiRegisterCount,
                        (char*)0, uiSize, uiSize, (void*)afValue, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsMatrix3(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
                uiSize = sizeof(NiMatrix3);
                NiMatrix3 kMat3Value;
                
                pkAttribDesc->GetValue_Matrix3(kMat3Value);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber, 
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)&kMat3Value, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags,
                        uiExtraNumber, uiRegisterStart, uiRegisterCount,
                        (char*)0, uiSize, uiSize, (void*)&kMat3Value, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsMatrix4(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
                uiSize = sizeof(float) * 16;
                float afValue[16];
                float* pfValue = &afValue[0];
                
                pkAttribDesc->GetValue_Matrix4(pfValue, uiSize);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber,
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)afValue, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags,
                        uiExtraNumber, uiRegisterStart, uiRegisterCount,
                        (char*)0, uiSize, uiSize, (void*)afValue, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsColor(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
                uiSize = sizeof(NiColorA);
                NiColorA kClrValue;
                
                pkAttribDesc->GetValue_ColorA(kClrValue);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber,
                        uiRegisterStart, uiRegisterCount, (char*)0,
                        uiSize, uiSize, (void*)&kClrValue, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags,
                        uiExtraNumber, uiRegisterStart, uiRegisterCount,
                        (char*)0, uiSize, uiSize, (void*)&kClrValue, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsArray(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY;

                // get description of array data
                NiShaderAttributeDesc::AttributeType eType;
                unsigned int uiElementSize;
                unsigned int uiNumElements;
                pkAttribDesc->GetArrayParams(
                    eType,
                    uiElementSize,
                    uiNumElements);

                // get copy of data
                uiSize = uiElementSize*uiNumElements;
                float* pfValues = NiAlloc(float,uiSize/sizeof(float));
                pkAttribDesc->GetValue_Array(pfValues,uiSize);

                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber,
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiElementSize, pfValues, true))
                    {
                        // PROBLEM
                        NiFree(pfValues);
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags, 
                        uiExtraNumber, uiRegisterStart, uiRegisterCount,
                        (char*)0, uiSize, uiElementSize, pfValues, true))
                    {
                        // PROBLEM!
                        NiFree(pfValues);
                        return false;
                    }
                }
                
                NiFree(pfValues);
                return true;
            }
            else
            {
                NIASSERT(!"Invalid Attribute Type");
                return false;
            }
        }
    }            
}
//---------------------------------------------------------------------------
bool SetupOperatorEntry(char* pszName, int iRegStart, int iRegCount, 
    char* pszEntry1, int iOperation, char* pszEntry2, bool bInverse, 
    bool bTranspose)
{
    if (!g_pkCurrConstantMap)
        return false;

    // Look up the 2 entries
    NSBConstantMap::NSBCM_Entry* pkEntry1;
    NSBConstantMap::NSBCM_Entry* pkEntry2;
    
    pkEntry1 = g_pkCurrConstantMap->GetEntryByKey(pszEntry1);
    pkEntry2 = g_pkCurrConstantMap->GetEntryByKey(pszEntry2);

    if (!pkEntry1 || !pkEntry2)    
    {
        NSFParsererror("CM_Operator operand not found\n");
        return false;
    }
    if (!(pkEntry1->IsDefined() || pkEntry1->IsGlobal() ||
          pkEntry1->IsAttribute() || pkEntry1->IsConstant()))
    {
        NSFParsererror("CM_Operator operand INVALID TYPE\n");
        return false;
    }
    if (!(pkEntry2->IsDefined() || pkEntry2->IsGlobal() ||
          pkEntry2->IsAttribute() || pkEntry2->IsConstant()))
    {
        NSFParsererror("CM_Operator operand INVALID TYPE\n");
        return false;
    }

    unsigned int uiEntry1;
    unsigned int uiEntry2;
    
    uiEntry1 = g_pkCurrConstantMap->GetEntryIndexByKey(pszEntry1);
    uiEntry2 = g_pkCurrConstantMap->GetEntryIndexByKey(pszEntry2);

    if ((uiEntry1 == 0xffffffff) || (uiEntry2 == 0xffffffff))    
    {
        NSFParsererror("CM_Operator operand INVALID INDEX\n");
        return false;
    }

    // Determine the results data type and set it in the flags
    NiShaderAttributeDesc::AttributeType eType1 = 
        pkEntry1->GetAttributeType();
    NiShaderAttributeDesc::AttributeType eType2 = 
        pkEntry2->GetAttributeType();

    if (pkEntry1->IsDefined())
    {
        // We have to look-up the type
        if (g_eConstMapMode == CONSTMAP_VERTEX)
        {
            eType1 = NiShaderConstantMap::LookUpPredefinedMappingType(
                pkEntry1->GetKey());
        }
        else
        if (g_eConstMapMode == CONSTMAP_GEOMETRY)
        {
            eType1 = NiShaderConstantMap::LookUpPredefinedMappingType(
                pkEntry1->GetKey());
        }
        else
        if (g_eConstMapMode == CONSTMAP_PIXEL)
        {
            eType1 = NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
        }
        else
        {
            NIASSERT(!"Invalid constant map mode!");
            return false;
        }
    }
    if (pkEntry2->IsDefined())
    {
        // We have to look-up the type
        if (g_eConstMapMode == CONSTMAP_VERTEX)
        {
            eType2 = NiShaderConstantMap::LookUpPredefinedMappingType(
                pkEntry2->GetKey());
        }
        else
        if (g_eConstMapMode == CONSTMAP_GEOMETRY)
        {
            eType2 = NiShaderConstantMap::LookUpPredefinedMappingType(
                pkEntry2->GetKey());
        }
        else
        if (g_eConstMapMode == CONSTMAP_PIXEL)
        {
            eType2 = NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
        }
        else
        {
            NIASSERT(!"Invalid constant map mode!");
            return false;
        }
    }
        
    if ((eType1 == NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED) ||
        NiShaderConstantMapEntry::IsBool(eType1) ||
        NiShaderConstantMapEntry::IsString(eType1) ||
        NiShaderConstantMapEntry::IsTexture(eType1) ||
        (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED) ||
        NiShaderConstantMapEntry::IsBool(eType2) ||
        NiShaderConstantMapEntry::IsString(eType2) ||
        NiShaderConstantMapEntry::IsTexture(eType2))
    {
        NSFParsererror("Invalid Operator Type");
        return false;
    }

    NiShaderAttributeDesc::AttributeType eResultType = 
        DetermineOperatorResult(iOperation, eType1, eType2);
    if (eResultType == NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED)
    {
        NSFParsererror("Invalid Operator - Result was invalid");
        return false;
    }
    
    // We have two valid entries, so let's setup the ConstantMapEntry
    // NOTE: If NSBConstantMap did not store it's list the proper way,
    // ie, via AddTail, this method would not work!
    unsigned int uiExtra = iOperation | uiEntry1 | 
        (uiEntry2 << NiShaderConstantMapEntry::SCME_OPERATOR_ENTRY2_SHIFT);
    if (bTranspose)
        uiExtra |= NiShaderConstantMapEntry::SCME_OPERATOR_RESULT_TRANSPOSE;
    if (bInverse)
        uiExtra |= NiShaderConstantMapEntry::SCME_OPERATOR_RESULT_INVERSE;

    unsigned int uiFlags = NiShaderConstantMapEntry::SCME_MAP_OPERATOR | 
        NiShaderConstantMapEntry::GetAttributeFlags(eResultType);

    if (g_bConstantMapPlatformBlock)
    {
        if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
            g_uiCurrentPlatforms, pszName, uiFlags, uiExtra, 
            iRegStart, iRegCount, (char*)0))
        {
            // PROBLEM
            return false;
        }
    }
    else
    {
        if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags, uiExtra, 
            iRegStart, iRegCount, (char*)0))
        {
            // PROBLEM
            return false;
        }
    }
        
    return true;
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType DetermineOperatorResult(int iOperation, 
    NiShaderAttributeDesc::AttributeType eType1, 
    NiShaderAttributeDesc::AttributeType eType2)
{
    switch (iOperation)
    {
    case NiShaderConstantMapEntry::SCME_OPERATOR_MULTIPLY:
        return DetermineResultMultiply(eType1, eType2);
    case NiShaderConstantMapEntry::SCME_OPERATOR_DIVIDE:
        return DetermineResultDivide(eType1, eType2);
    case NiShaderConstantMapEntry::SCME_OPERATOR_ADD:
        return DetermineResultAdd(eType1, eType2);
    case NiShaderConstantMapEntry::SCME_OPERATOR_SUBTRACT:
        return DetermineResultSubtract(eType1, eType2);
    default:
        break;
    }
    return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType DetermineResultMultiply(
    NiShaderAttributeDesc::AttributeType eType1, 
    NiShaderAttributeDesc::AttributeType eType2)
{
    switch (eType1)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
    default:
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    }
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType DetermineResultDivide(
    NiShaderAttributeDesc::AttributeType eType1, 
    NiShaderAttributeDesc::AttributeType eType2)
{
    switch (eType1)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
    default:
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        break;
    }
    return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType DetermineResultAdd(
    NiShaderAttributeDesc::AttributeType eType1, 
    NiShaderAttributeDesc::AttributeType eType2)
{
    switch (eType1)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
    default:
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    }
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType DetermineResultSubtract(
    NiShaderAttributeDesc::AttributeType eType1, 
    NiShaderAttributeDesc::AttributeType eType2)
{
    switch (eType1)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
    default:
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    }
}
//---------------------------------------------------------------------------
