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

// Disable generation of yyunput method to prevent unused function warnings
#define YY_NO_UNPUT 1

#if defined(WIN32) || defined(_XENON)
#pragma warning( disable : 4100 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4244 )
#pragma warning( disable : 4267 )
#endif

#if defined(_XENON)
    #include <xtl.h>
#elif defined (WIN32)
    #include "windows.h"
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
#elif defined (_PS3)
    #include <NiSystem.h>
#endif  //#if defined(_XENON)
    #include <NiShaderFactory.h>
    #include "NSFTextFile.h"
    #include "NSFLexSymbol.h"

    extern int iErrors;
    extern YYSTYPE yylval;

    #define YY_SKIP_YYWRAP          1
    #define YY_ALWAYS_INTERACTIVE   1

    extern "C"
    {
        static int NSFParser_line_number=1;
        
        int yywrap() 
        {
            return 1;
        };
    }

    void skip0();
    void skip1();
    unsigned long HexConvert(char* buf);

    extern NSFTextFile* g_pkFile;
    
    void NSFParserResetLineNumber()
    {
        NSFParser_line_number = 1;
    }
    
    int NSFParserGetLineNumber()
    {
        return NSFParser_line_number;
    }
    
    void NSFParsererror(const char *s) 
    {
        iErrors++;
        char szErr[1024];
        NiSprintf(szErr, 1024, 
            "* ERROR: Invalid identifier \'%s\' at line %d (%s)\n",
            yytext, NSFParser_line_number, s);
        NILOG(szErr);
        if (g_pkFile)
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                "* ERROR: %s\nInvalid identifier \'%s\' at line %d (%s)\n",
                g_pkFile->GetFilename(), yytext, NSFParser_line_number, s);
        }
        else
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                "* ERROR: %s\nInvalid identifier \'%s\' at line %d (%s)\n",
                "*** Unknown File ***", yytext, NSFParser_line_number, s);
        }
    };

    void NSFParsererror2(const char *s) 
    {
        iErrors++;
        char szErr[1024];
        NiSprintf(szErr, 1024, 
             "* ERROR: %s at line %d\n",s,NSFParser_line_number);
        NILOG(szErr);
        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
            "* ERROR: %s at line %d\n",s,NSFParser_line_number);
        if (g_pkFile)
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                "* ERROR: %s\n%s at line %d\n",
                g_pkFile->GetFilename(), s, NSFParser_line_number);
        }
        else
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                "* ERROR: %s\n%s at line %d\n",
                "*** Unknown File ***", s, NSFParser_line_number);
        }
    };

    #ifndef D3DSHADER_VERSION_MAJOR
    #define D3DSHADER_VERSION_MAJOR(_Version) (((_Version)>>8)&0xFF)
    #endif  //#ifndef D3DSHADER_VERSION_MAJOR

    #ifndef D3DSHADER_VERSION_MINOR
    #define D3DSHADER_VERSION_MINOR(_Version) (((_Version)>>0)&0xFF)
    #endif  //#ifndef D3DSHADER_VERSION_MINOR

    #ifndef D3DVS_VERSION
    #define D3DVS_VERSION(_Major,_Minor) (0xFFFE0000|((_Major)<<8)|(_Minor))
    #endif  //#ifndef D3DVS_VERSION

    #ifndef D3DGS_VERSION
    #define D3DGS_VERSION(_Major,_Minor) (0xFFFD0000|((_Major)<<8)|(_Minor))
    #endif  //#ifndef D3DGS_VERSION

    #ifndef D3DPS_VERSION
    #define D3DPS_VERSION(_Major,_Minor) (0xFFFF0000|((_Major)<<8)|(_Minor))
    #endif  //#ifndef D3DPS_VERSION

    int my_yyinput (char * buf, int max_size);

    #undef YY_INPUT
    #define YY_INPUT(buf,result,max) (result = my_yyinput(buf,max))
    
    #define MAX_QUOTE_LENGTH    6 * 1024

//real            {integer}("."({digit}+))?{exponant}?
%}

white           [ \t]+
digit           [0-9]
integer         [+-]?{digit}+
hexidecimal     0x[_a-fA-F0-9_]+
exponant        [eE][+-]?({digit}+)
real            {integer}"."({digit}+)?{exponant}?
string          [_a-zA-Z0-9_]+
path_dir        ([/]?{string}[/]?)+
filename        {string}("."{string}?)?
path            ({path_dir}?){filename}
quotedstring    \"[^\"]*\"
version         [v]({digit}+"."{digit}+)
anything        .

%%

{white}             { };
{integer}           {
                        NSFParserlval.ival=(int)atoi(yytext);
                        return N_INT;
                    }
{real}              { 
                        NSFParserlval.fval=(float)atof(yytext);
                        return N_FLOAT;
                    };
{hexidecimal}       {
                        NSFParserlval.dword=HexConvert(yytext);
                        return N_HEX;
                    };

{version}           {
                        // Parse out the actual numbers
                        char szNum[32];
                        
                        // Skip the v
                        unsigned int uiIndex = 1;
                        unsigned int uiNumIndex = 0;
                        bool bFoundDot = false;
                        unsigned int uiMaj = 0;
                        unsigned int uiMin = 0;
                        
                        while (yytext[uiIndex] != 0)
                        {
                            bool bEndPack = false;
                            
                            if (yytext[uiIndex] != '.')
                                szNum[uiNumIndex++] = yytext[uiIndex];
                            else
                                bEndPack = true;
                            
                            if (yytext[uiIndex+1] == 0)
                                bEndPack = true;
                                
                            if (bEndPack)
                            {
                                szNum[uiNumIndex] = 0;
                                unsigned int uiNum = atoi(szNum);
                                
                                if (!bFoundDot)
                                    uiMaj = uiNum;
                                else
                                    uiMin = uiNum;
                                bFoundDot = true;
                                uiNumIndex = 0;
                            }
                            uiIndex++;
                        }
                        
                        NSFParserlval.vers = D3DVS_VERSION(uiMaj, uiMin);
                        
                        return N_VERSION;
                    };
            
"//"                {   skip0();                };
"/*"                {   skip1();                };

"{"                 {   return L_ACCOLADE;      };
"}"                 {   return R_ACCOLADE;      };

"("                 {   return L_PARENTHESE;    };
")"                 {   return R_PARENTHESE;    };

"["                 {   return L_BRACKET;       };
"]"                 {   return R_BRACKET;       };

"<"                 {   return L_ANGLEBRACKET;  };
">"                 {   return R_ANGLEBRACKET;  };

"|"                 {   return OR;              };

"="                 {   return ASSIGN;          };
","                 {   return COMMA;           };

"_"                 {   return UNDERSCORE;      };

"*"                 {   return ASTERIK;         };
"/"                 {   return FORWARDSLASH;    };
"+"                 {   return PLUS;            };
"-"                 {   return MINUS;           };

"@"                 {   return NSF_AT_SYMBOL;   };
";"                 {   return NSF_SEMICOLON;   };
":"                 {   return NSF_COLON;       };

NSFShader           {   return NSFSHADER;       };

PackingDef          {   return PACKINGDEF;      };
PD_Stream           {   return PD_STREAM;       };
PD_FixedFunction    {   return PD_FIXEDFUNCTION;};

    /* Support for semantic adapter tables in NSF files */
SemanticAdapterTable { return SEMANTICADAPTERTABLE; }

    /* Based on D3DVSDE_ macros */
PDP_Position        {   return PDP_POSITION;    };
PDP_Position0        {   return PDP_POSITION0;    };
PDP_Position1        {   return PDP_POSITION1;    };
PDP_Position2        {   return PDP_POSITION2;    };
PDP_Position3        {   return PDP_POSITION3;    };
PDP_Position4        {   return PDP_POSITION4;    };
PDP_Position5        {   return PDP_POSITION5;    };
PDP_Position6        {   return PDP_POSITION6;    };
PDP_Position7        {   return PDP_POSITION7;    };

PDP_BlendWeight     {   return PDP_BLENDWEIGHTS;};
PDP_BlendWeights    {   return PDP_BLENDWEIGHTS;};
PDP_BlendIndices    {   return PDP_BLENDINDICES;};
PDP_Normal          {   return PDP_NORMAL;      };
PDP_PSize           {   return PDP_POINTSIZE;   };
PDP_PointSize       {   return PDP_POINTSIZE;   };
PDP_Diffuse         {   return PDP_COLOR;       };
PDP_Color           {   return PDP_COLOR;       };
PDP_Specular        {   return PDP_COLOR2;      };
PDP_Color2          {   return PDP_COLOR2;      };
PDP_TexCoord0       {   return PDP_TEXCOORD0;   };
PDP_TexCoord1       {   return PDP_TEXCOORD1;   };
PDP_TexCoord2       {   return PDP_TEXCOORD2;   };
PDP_TexCoord3       {   return PDP_TEXCOORD3;   };
PDP_TexCoord4       {   return PDP_TEXCOORD4;   };
PDP_TexCoord5       {   return PDP_TEXCOORD5;   };
PDP_TexCoord6       {   return PDP_TEXCOORD6;   };
PDP_TexCoord7       {   return PDP_TEXCOORD7;   };
PDP_Normal2         {   return PDP_NORMAL2;     };
PDP_Tangent         {   return PDP_TANGENT;     };
PDP_Binormal        {   return PDP_BINORMAL;    };
PDP_ExtraData       {   return PDP_EXTRADATA;   };

    /* Based on D3DVSDT_ macros and D3DDECLTYPE_ enumerations */
PDT_Float1          {   return PDT_FLOAT1;      };
PDT_Float2          {   return PDT_FLOAT2;      };
PDT_Float3          {   return PDT_FLOAT3;      };
PDT_Float4          {   return PDT_FLOAT4;      };
PDT_D3DColor        {   return PDT_UBYTECOLOR;  };
PDT_UByteColor      {   return PDT_UBYTECOLOR;  };
PDT_Short1          {   return PDT_SHORT1;      };
PDT_Short2          {   return PDT_SHORT2;      };
PDT_Short3          {   return PDT_SHORT3;      };
PDT_Short4          {   return PDT_SHORT4;      };
PDT_UByte4          {   return PDT_UBYTE4;      };
PDT_NormShort1      {   return PDT_NORMSHORT1;  };
PDT_Short1N         {   return PDT_NORMSHORT1;  };
PDT_NormShort2      {   return PDT_NORMSHORT2;  };
PDT_Short2N         {   return PDT_NORMSHORT2;  };
PDT_NormShort3      {   return PDT_NORMSHORT3;  };
PDT_Short3N         {   return PDT_NORMSHORT3;  };
PDT_NormShort4      {   return PDT_NORMSHORT4;  };
PDT_Short4N         {   return PDT_NORMSHORT4;  };
PDT_NormPacked3     {   return PDT_NORMPACKED3; };
PDT_Packed3N        {   return PDT_NORMPACKED3; };
PDT_PByte1          {   return PDT_PBYTE1;      };
PDT_PByte2          {   return PDT_PBYTE2;      };
PDT_PByte3          {   return PDT_PBYTE3;      };
PDT_PByte4          {   return PDT_PBYTE4;      };
PDT_Float2H         {   return PDT_FLOAT2H;     };
PDT_UByte4N         {   return PDT_NORMUBYTE4;  };
PDT_NormUByte4      {   return PDT_NORMUBYTE4;  };
PDT_UShort2N        {   return PDT_NORMUSHORT2; };
PDT_NormUShort2     {   return PDT_NORMUSHORT2; };
PDT_UShort4N        {   return PDT_NORMUSHORT4; };
PDT_NormUShort4     {   return PDT_NORMUSHORT4; };
PDT_UDec3           {   return PDT_UDEC3;       };
PDT_Dec3N           {   return PDT_NORMDEC3;    };
PDT_NormDec3        {   return PDT_NORMDEC3;    };
PDT_Float16_2       {   return PDT_FLOAT16_2;   };
PDT_Float16_4       {   return PDT_FLOAT16_4;   };

    /* Based on D3DDECLMETHOD_ enumerations */
PDTESS_Default          {   return PDTESS_DEFAULT;          };
PDTESS_PartialU         {   return PDTESS_PARTIALU;         };
PDTESS_PartialV         {   return PDTESS_PARTIALV;         };
PDTESS_CrossUV          {   return PDTESS_CROSSUV;          };
PDTESS_UV               {   return PDTESS_UV;               };
PDTESS_LookUp           {   return PDTESS_LOOKUP;           };
PDTESS_LookUpPresampled {   return PDTESS_LOOKUPPRESAMPLED; };

    /* Based on D3DDECLUSAGE_ enumerations */
PDU_Position        {   return PDU_POSITION;    };
PDU_BlendWeight     {   return PDU_BLENDWEIGHT; };
PDU_BlendIndices    {   return PDU_BLENDINDICES;};
PDU_Normal          {   return PDU_NORMAL;      };
PDU_PSize           {   return PDU_PSIZE;       };
PDU_TexCoord        {   return PDU_TEXCOORD;    };
PDU_Tangent         {   return PDU_TANGENT;     };
PDU_BiNormal        {   return PDU_BINORMAL;    };
PDU_TessFactor      {   return PDU_TESSFACTOR;  };
PDU_PositionT       {   return PDU_POSITIONT;   };
PDU_Color           {   return PDU_COLOR;       };
PDU_Fog             {   return PDU_FOG;         };
PDU_Depth           {   return PDU_DEPTH;       };
PDU_Sample          {   return PDU_SAMPLE;      };

Artist              {   return ARTIST;          };
Hidden              {   return HIDDEN;          };
Save                {   return SAVE;            };

Attributes          {   return ATTRIBUTES;      };
Global_Attributes   {   return GLOBALATTRIBUTES;};
Attrib              {   return ATTRIB;          };
Attrib_Bool         {   return ATTRIB_BOOL;     };
Attrib_String       {   return ATTRIB_STRING;   };
Attrib_UInt         {   return ATTRIB_UINT;     };
Attrib_Float        {   return ATTRIB_FLOAT;    };
Attrib_Point2       {   return ATTRIB_POINT2;   };
Attrib_Point3       {   return ATTRIB_POINT3;   };
Attrib_Point4       {   return ATTRIB_POINT4;   };
Attrib_Matrix3      {   return ATTRIB_MATRIX3;  };
Attrib_Transform    {   return ATTRIB_TRANSFORM;};
Attrib_Color        {   return ATTRIB_COLOR;    };
Attrib_Texture      {   return ATTRIB_TEXTURE;  };

Objects                         {   return OBJECTS;                     };
Effect_GeneralLight             {   return EFFECT_GENERALLIGHT;         };
Effect_PointLight               {   return EFFECT_POINTLIGHT;           };
Effect_DirectionalLight         {   return EFFECT_DIRECTIONALLIGHT;     };
Effect_SpotLight                {   return EFFECT_SPOTLIGHT;            };
Effect_ShadowPointLight         {   return EFFECT_SHADOWPOINTLIGHT;           };
Effect_ShadowDirectionalLight   {   return EFFECT_SHADOWDIRECTIONALLIGHT;     };
Effect_ShadowSpotLight          {   return EFFECT_SHADOWSPOTLIGHT;            };
Effect_EnvironmentMap           {   return EFFECT_ENVIRONMENTMAP;       };
Effect_ProjectedShadowMap       {   return EFFECT_PROJECTEDSHADOWMAP;   };
Effect_ProjectedLightMap        {   return EFFECT_PROJECTEDLIGHTMAP;    };
Effect_FogMap                   {   return EFFECT_FOGMAP;               };

UserDefinedData     {   return USERDEFINEDDATA; };
Implementation      {   return IMPLEMENTATION;  };
OutputStream        {   return OUTPUTSTREAM;    };

ClassName           {   return CLASSNAME;       };

Pass                {   return PASS;            };

Stage               {   return STAGE;           };

    /* Based on D3DTSS_ enumerations */
TSS_Texture                 {   return TSS_TEXTURE;                 };
TSS_ColorOp                 {   return TSS_COLOROP;                 };
TSS_ColorArg0               {   return TSS_COLORARG0;               };
TSS_ColorArg1               {   return TSS_COLORARG1;               };
TSS_ColorArg2               {   return TSS_COLORARG2;               };
TSS_AlphaOp                 {   return TSS_ALPHAOP;                 };
TSS_AlphaArg0               {   return TSS_ALPHAARG0;               };
TSS_AlphaArg1               {   return TSS_ALPHAARG1;               };
TSS_AlphaArg2               {   return TSS_ALPHAARG2;               };
TSS_ResultArg               {   return TSS_RESULTARG;               };
TSS_Constant                {   return TSS_CONSTANT_DEPRECATED;     };
TSS_BumpEnvMat00            {   return TSS_BUMPENVMAT00;            };
TSS_BumpEnvMat01            {   return TSS_BUMPENVMAT01;            };
TSS_BumpEnvMat10            {   return TSS_BUMPENVMAT10;            };
TSS_BumpEnvMat11            {   return TSS_BUMPENVMAT11;            };
TSS_BumpEnvLScale           {   return TSS_BUMPENVLSCALE;           };
TSS_BumpEnvLOffset          {   return TSS_BUMPENVLOFFSET;          };
TSS_TexCoordIndex           {   return TSS_TEXCOORDINDEX;           };
TSS_TexTransFlags           {   return TSS_TEXTURETRANSFORMFLAGS;   };
TSS_TextureTransformFlags   {   return TSS_TEXTURETRANSFORMFLAGS;   };
TSS_TexTransMatrix          {   return TSS_TEXTRANSMATRIX;          };
TSS_TextureTransformMatrix  {   return TSS_TEXTRANSMATRIX;          };

TTFF_Disable        {   return TTFF_DISABLE;        };
TTFF_Count1         {   return TTFF_COUNT1;         };
TTFF_Count2         {   return TTFF_COUNT2;         };
TTFF_Count3         {   return TTFF_COUNT3;         };
TTFF_Count4         {   return TTFF_COUNT4;         };
TTFF_Projected      {   return TTFF_PROJECTED;      };

Projected           {   return PROJECTED;           };
UseMapIndex         {   return USEMAPINDEX;         };
UseMapValue         {   return USEMAPVALUE;         };
Inverse             {   return INVERSE;             };
Transpose           {   return TRANSPOSE;           };

TTSrc_Global        {   return TTSRC_GLOBAL;        };
TTSrc_Constant      {   return TTSRC_CONSTANT;      };

TT_WorldParallel        {   return TT_WORLD_PARALLEL;       };
TT_WorldPerspective     {   return TT_WORLD_PERSPECTIVE;    };
TT_WorldSphereMap       {   return TT_WORLD_SPHERE_MAP;     };
TT_CameraSphereMap      {   return TT_CAMERA_SPHERE_MAP;    };
TT_SpecularCubeMap      {   return TT_SPECULAR_CUBE_MAP;    };
TT_DiffuseCubeMap       {   return TT_DIFFUSE_CUBE_MAP;     };

    /* Based on D3DTSS_TCI_ enumerations */
TCI_PassThru                    {   return TCI_PASSTHRU;            };
TCI_CameraSpaceNormal           {   return TCI_CAMERASPACENORMAL;   };
TCI_CameraSpacePosition         {   return TCI_CAMERASPACEPOSITION; };
TCI_CameraSpaceReflect          {   return TCI_CAMERASPACEREFLECT;  };
TCI_CameraSpaceReflectionVector {   return TCI_CAMERASPACEREFLECT;  };
TCI_SphereMap                   {   return TCI_SPHEREMAP;           };

    /* Based on D3DTOP_ enumerations */
TOP_Disable                     {   return TOP_DISABLE;                     };
TOP_SelectArg1                  {   return TOP_SELECTARG1;                  };
TOP_SelectArg2                  {   return TOP_SELECTARG2;                  };
TOP_Modulate                    {   return TOP_MODULATE;                    };
TOP_Modulate2x                  {   return TOP_MODULATE2X;                  };
TOP_Modulate4x                  {   return TOP_MODULATE4X;                  };
TOP_Add                         {   return TOP_ADD;                         };
TOP_AddSigned                   {   return TOP_ADDSIGNED;                   };
TOP_AddSigned2x                 {   return TOP_ADDSIGNED2X;                 };
TOP_Subtract                    {   return TOP_SUBTRACT;                    };
TOP_AddSmooth                   {   return TOP_ADDSMOOTH;                   };
TOP_BlendDiffuseAlpha           {   return TOP_BLENDDIFFUSEALPHA;           };
TOP_BlendTextureAlpha           {   return TOP_BLENDTEXTUREALPHA;           };
TOP_BlendFactoralAlpha          {   return TOP_BLENDFACTORALPHA;            };
TOP_BlendFactoraApha            {   return TOP_BLENDFACTORALPHA;            };
TOP_BlendFactorAlpha            {   return TOP_BLENDFACTORALPHA;            };
TOP_BlendTextureAlphaPM         {   return TOP_BLENDTEXTUREALPHAPM;         };
TOP_BlendCurrentAlpha           {   return TOP_BLENDCURRENTALPHA;           };
TOP_Premodulate                 {   return TOP_PREMODULATE;                 };
TOP_ModulateAlpha_AddColor      {   return TOP_MODULATEALPHA_ADDCOLOR;      };
TOP_ModulateColor_AddAlpha      {   return TOP_MODULATECOLOR_ADDALPHA;      };
TOP_ModulateInvAlpha_AddColor   {   return TOP_MODULATEINVALPHA_ADDCOLOR;   };
TOP_ModulateInvColor_AddAlpha   {   return TOP_MODULATEINVCOLOR_ADDALPHA;   };
TOP_BumpEnvMap                  {   return TOP_BUMPENVMAP;                  };
TOP_BumpEnvMapLuminance         {   return TOP_BUMPENVMAPLUMINANCE;         };
TOP_DotProduct3                 {   return TOP_DOTPRODUCT3;                 };
TOP_MultiplyAdd                 {   return TOP_MULTIPLYADD;                 };
TOP_Lerp                        {   return TOP_LERP;                        };

    /* Based on D3DTA_ macros */
TA_Current                      {   return TA_CURRENT;                      };
TA_Diffuse                      {   return TA_DIFFUSE;                      };
TA_SelectMask                   {   return TA_SELECTMASK;                   };
TA_Specular                     {   return TA_SPECULAR;                     };
TA_Temp                         {   return TA_TEMP;                         };
TA_Texture                      {   return TA_TEXTURE;                      };
TA_TFactor                      {   return TA_TFACTOR;                      };
TA_AlphaReplicate               {   return TA_ALPHAREPLICATE;               };
TA_Complement                   {   return TA_COMPLEMENT;                   };

NTM_Base            {   return NTM_BASE;    };
NTM_Dark            {   return NTM_DARK;    };
NTM_Detail          {   return NTM_DETAIL;  };
NTM_Gloss           {   return NTM_GLOSS;   };
NTM_Glow            {   return NTM_GLOW;    };
NTM_Bump            {   return NTM_BUMP;    };
NTM_Decal           {   return NTM_DECAL;   };
NTM_Normal          {   return NTM_NORMAL;   };
NTM_Parallax        {   return NTM_PARALLAX;   };

Sampler             {   return SAMPLER;             };

    /* Based on D3DSAMP_ enumerations */
TSAMP_AddressU      {   return TSAMP_ADDRESSU;                  };
TSAMP_AddressV      {   return TSAMP_ADDRESSV;                  };
TSAMP_AddressW      {   return TSAMP_ADDRESSW;                  };
TSAMP_BorderColor   {   return TSAMP_BORDERCOLOR;               };
TSAMP_MagFilter     {   return TSAMP_MAGFILTER;                 };
TSAMP_MinFilter     {   return TSAMP_MINFILTER;                 };
TSAMP_MipFilter     {   return TSAMP_MIPFILTER;                 };
TSAMP_MipMapLODBias {   return TSAMP_MIPMAPLODBIAS;             };
TSAMP_MaxMipLevel   {   return TSAMP_MAXMIPLEVEL;               };
TSAMP_MaxAnisotropy {   return TSAMP_MAXANISOTROPY;             };
TSAMP_SRGBTexture   {   return TSAMP_SRGBTEXTURE;               };
TSAMP_ElementIndex  {   return TSAMP_ELEMENTINDEX;              };
TSAMP_DMapOffset    {   return TSAMP_DMAPOFFSET;                };
TSAMP_AlphaKill     {   return TSAMP_ALPHAKILL_DEPRECATED;      };
TSAMP_ColorKeyOp    {   return TSAMP_COLORKEYOP_DEPRECATED;     };
TSAMP_ColorSign     {   return TSAMP_COLORSIGN_DEPRECATED;      };
TSAMP_ColorKeyColor {   return TSAMP_COLORKEYCOLOR_DEPRECATED;  };

    /* Based on D3DSAMP_ enumerations */
TADDR_Wrap          {   return TADDR_WRAP;                      };
TADDR_Mirror        {   return TADDR_MIRROR;                    };
TADDR_Clamp         {   return TADDR_CLAMP;                     };
TADDR_Border        {   return TADDR_BORDER;                    };
TADDR_MirrorOnce    {   return TADDR_MIRRORONCE;                };
TADDR_ClampToEdge   {   return TADDR_CLAMPTOEDGE_DEPRECATED;    };

    /* Based on D3DTEXF_ enumerations */
TEXF_None           {   return TEXF_NONE;                       };
TEXF_Point          {   return TEXF_POINT;                      };
TEXF_Linear         {   return TEXF_LINEAR;                     };
TEXF_Anisotropic    {   return TEXF_ANISOTROPIC;                };
TEXF_PyramidalQuad  {   return TEXF_PYRAMIDALQUAD;              };
TEXF_QuassianQuad   {   return TEXF_GAUSSIANQUAD;               };
TEXF_Quincunx       {   return TEXF_QUINCUNX_DEPRECATED;        };
TEXF_FlatCubic      {   return TEXF_FLATCUBIC_DEPRECATED;       };
TEXF_GaussianCubic  {   return TEXF_GAUSSIANCUBIC_DEPRECATED;   };
TEXF_Max            {   return TEXF_MAX_DEPRECATED;             };

TAK_Disable         {   return TAK_DISABLE_DEPRECATED;  };
TAK_Enable          {   return TAK_ENABLE_DEPRECATED;   };

TCKOP_Disable       {   return TCKOP_DISABLE_DEPRECATED;    };
TCKOP_Alpha         {   return TCKOP_ALPHA_DEPRECATED;      };
TCKOP_RGBA          {   return TCKOP_RGBA_DEPRECATED;       };
TCKOP_Kill          {   return TCKOP_KILL_DEPRECATED;       };

Texture             {   return TOKEN_TEXTURE;         };

Source              {   return TEXTURE_SOURCE; };


RenderStates        {   return RENDERSTATES;    };

CM_Defined          {   return CMDEFINED;       };
CM_Attribute        {   return CMATTRIBUTE;     };
CM_Constant         {   return CMCONSTANT;      };
CM_Global           {   return CMGLOBAL;        };
CM_Operator         {   return CMOPERATOR;      };
CM_Object           {   return CMOBJECT;        };
VS_Constantmap      {   return VSCONSTANTMAP;   };
GS_Constantmap      {   return GSCONSTANTMAP;   };
PS_Constantmap      {   return PSCONSTANTMAP;   };

SkinBoneMatrix3     {   return SKINBONEMATRIX3; };

VSProgram           {   return VSPROGRAM;       };
GSProgram           {   return GSPROGRAM;       };
PSProgram           {   return PSPROGRAM;       };
Program             {   return PROGRAM;         };
EntryPoint          {   return ENTRYPOINT;      };
ShaderTarget        {   return SHADERTARGET;    };
SoftwareVP          {   return SOFTWAREVP;      };

StreamOutput        {   return STREAMOUTPUT;        };
StreamOutTargets    {   return STREAMOUTTARGETS;    };
StreamOutAppend     {   return STREAMOUTAPPEND;     };

MaxVertexCount      {   return MAXVERTEXCOUNT;  };
OutputPrimType      {   return OUTPUTPRIMTYPE;  };
Point               {   return _POINT;          };
Line                {   return _LINE;           };
Triangle            {   return _TRIANGLE;       };
VertexFormat        {   return VERTEXFORMAT;    };
Float               {   return FMT_FLOAT;       };
Int                 {   return FMT_INT;         };
Uint                {   return FMT_UINT;        };

Requirements            {   return REQUIREMENTS;            };
VSVersion               {   return VSVERSION;               };
GSVersion               {   return GSVERSION;               };
PSVersion               {   return PSVERSION;               };
UserVersion             {   return USERVERSION;             };
Platform                {   return PLATFORM;                };
BonesPerPartition       {   return BONESPERPARTITION;       };
BinormalTangentMethod   {   return BINORMALTANGENTMETHOD;   };
BinormalTangentUVSource {   return BINORMALTANGENTUVSOURCE;   };
NBTMethod_None          {   return NBTMETHOD_NONE;          };
NBTMethod_NI            {   return NBTMETHOD_NI;            };
NBTMethod_MAX           {   return NBTMETHOD_MAX;           };
NBTMethod_ATI           {   return NBTMETHOD_ATI;           };

true                {   
                        NSFParserlval.bval = true;      
                        return N_BOOL;
                    };
false               {   
                        NSFParserlval.bval = false;
                        return N_BOOL;
                    };

"\n"                {   NSFParser_line_number++;          }
"\r"                {}

{string}            {
                        int iLength = (strlen(yytext));
                        NSFParserlval.sval = NiAlloc(char, iLength+1);
                        NiStrncpy(NSFParserlval.sval, iLength+1, yytext, 
                            iLength);
                        return N_STRING;
                    };
{path}              {
                        int iLength = (strlen(yytext));
                        NSFParserlval.sval = NiAlloc(char, iLength+1);
                        NiStrncpy(NSFParserlval.sval, iLength+1, yytext, 
                            iLength);
                        return PATH;
                    };

{quotedstring}      {
                        int iAllocSize;
                        int l = (strlen(yytext) - 2);
                        if (l > MAX_QUOTE_LENGTH)
                        {
                            NIASSERT("Quote too long!");
                            iAllocSize = l;
                        }
                        else
                        {
                            iAllocSize = MAX_QUOTE_LENGTH;
                        }
                        NSFParserlval.sval = NiAlloc(char, iAllocSize+1);
                        NiStrncpy(NSFParserlval.sval, l+1, &yytext[1], l);
                        return N_QUOTE;
                    }

{anything}          {
                        NSFParsererror("Syntax Error");
                    }

%%

void skip0()
{
    char c;
    while (((c = g_pkFile->GetData()[g_pkFile->GetPos()++]) !='\n') && 
        (c != EOF));
    NSFParser_line_number++;
}

void skip1()
{
    char c0,c1;
    while (1)
    {
        c0 = g_pkFile->GetData()[g_pkFile->GetPos()++]; 
        if (c0==EOF)
        {
            NILOG("err: EOF in comment!\n");
            exit(-1);
        }

        if (c0 == '\n')
            NSFParser_line_number++;

        c1 = g_pkFile->GetData()[g_pkFile->GetPos()++];
        if (c1 == EOF)
        {
            NILOG("err: EOF in comment!\n");
            exit(-1);
        }

        if ((c0 == '*') && (c1 == '/'))
            break;  

        g_pkFile->GetPos()--;
    }
}

/*********************************************
  This routine is replaces the Flex 
  "internal" YY_INPUT macro
  that it uses to get input
*********************************************/

int my_yyinput (char * buf, int max_size)
{

    if (g_pkFile->GetSize() <= g_pkFile->GetPos()) 
        return YY_NULL;

    char c = g_pkFile->GetData()[g_pkFile->GetPos()++];
    
    if (c == EOF)
        return YY_NULL;

    buf[0] = c;
    buf[1] = 0;

    return 1;
}

unsigned long HexConvert(char* buf)
{
    if (!buf || (strcmp(buf, "") == 0))
        return 0;

    int iLen = strlen(buf);
    int iDigitVal = 1;
    int iValue = 0;
    
    // Skip the terminating 0
    iLen--;
    char* pchCurr = &buf[iLen];
    while ((*pchCurr != 'x') && (*pchCurr != 'X'))
    {
        if ((*pchCurr == 'a') || (*pchCurr == 'A'))
            iValue += iDigitVal * 10;
        else
        if ((*pchCurr == 'b') || (*pchCurr == 'B'))
            iValue += iDigitVal * 11;
        else
        if ((*pchCurr == 'c') || (*pchCurr == 'C'))
            iValue += iDigitVal * 12;
        else
        if ((*pchCurr == 'd') || (*pchCurr == 'D'))
            iValue += iDigitVal * 13;
        else
        if ((*pchCurr == 'e') || (*pchCurr == 'E'))
            iValue += iDigitVal * 14;
        else
        if ((*pchCurr == 'f') || (*pchCurr == 'F'))
            iValue += iDigitVal * 15;
        else
            iValue += iDigitVal * (*pchCurr - '0');
            
        iDigitVal *= 16;
        iLen--;
        pchCurr = &buf[iLen];
    }
    
    return iValue;
}

void NSFParserReset()
{
    YY_NEW_FILE;
} 

void NSFParserReleaseBuffer()
{
    yy_delete_buffer(yy_current_buffer);
}
    
