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

#include "MaxImmerse.h"
#include "NiMAXCustAttribConverter.h"
#include "NiMAXAnimationConverter.h"
#include "NiMAXTextureConverter.h"

#include <custattrib.h>
#include <icustattribcontainer.h>

#define NUM_STANDARD_CAS 6
const char * gs_acStdCANames[NUM_STANDARD_CAS] =
{
    "PrecacheGeometrySettings",
    "PrecacheGeometryCA",
    "ParticleSystemCA",
    "ParticleSystemSettings",
    "MeshProfileCA",
    "MeshProfileSettings",
};

#define NUM_MAX_CAS 5
const char* gs_acMaxCANames[NUM_MAX_CAS] =
{
    "Viewport Manager",
    "DirectX Manager",
    "mental ray: material custom attribute",
    "mental ray: Indirect Illumination custom attribute",
    "mental ray: light shader custom attribute"
};

#define NI_NEAREST 0
#define NI_BILERP 1
#define NI_NEAREST_MIPNEAREST 2
#define NI_NEAREST_MIPLERP 3
#define NI_BILERP_MIPNEAREST 4
#define NI_TRILERP 5
#define NI_ANISOTROPIC 6
//---------------------------------------------------------------------------
bool IsValidName(TCHAR* pcStr)
{
    int iLen = (int)strlen(pcStr);
    if ( iLen == 0) 
        return false;

    for (int ui = 0; ui < iLen; ui++)
    {
        if (pcStr[ui] >= 'a' && pcStr[ui] <= 'z')
            continue;
        else if (pcStr[ui] >= 'A' && pcStr[ui] <= 'Z')
            continue;
        else if (pcStr[ui] >= '1' && pcStr[ui] <= '9')
            continue;
        else if (pcStr[ui] == '0')
            continue;
        else if (pcStr[ui] == ' ')
            continue;
        else if (pcStr[ui] == '\t')
            continue;
        else if (pcStr[ui] == '_')
            continue;
        else if (pcStr[ui] == '-')
            continue;
        else if (pcStr[ui] == '/')
            continue;
        else if (pcStr[ui] == '\\')
            continue; 
        else
            return false;
    }
    return true;
}
//---------------------------------------------------------------------------
void NiMAXCustAttribConverter::GetUVInfo(Mtl* pkMaterial, 
    TimeValue kAnimStart, NiMAXUV*& pkNiMAXUV, int iAttrId, bool)
{
    if (!pkNiMAXUV)
        pkNiMAXUV = NiNew NiMAXUV(kAnimStart);
    
    if (pkMaterial == NULL)
        return;

    pkNiMAXUV->SetAttrId(iAttrId);

    ICustAttribContainer* cc = pkMaterial->GetCustAttribContainer();
    if (cc) 
    {
        int num_attribs = cc->GetNumCustAttribs();
        CustAttrib * pkAttribute = NULL;
        IParamBlock2* pkParamBlock = NULL;

        bool bIgnoreStandardCAs = true;
        for (int attrib = 0; attrib < num_attribs; attrib++)
        {
            pkAttribute = (CustAttrib*) cc->GetCustAttrib(attrib);
            if (pkAttribute)
            {
                CHECK_MEMORY();
                if (IsMaxCA(pkAttribute->GetName()) || (bIgnoreStandardCAs && 
                    IsStandardCA(pkAttribute->GetName())))
                {
                    continue;
                }
                NiOutputDebugString("GetUVInfo Cust Attrib:");
                NiOutputDebugString(pkAttribute->GetName());
                NiOutputDebugString("\n");
                Class_ID cId = pkAttribute->ClassID();
                pkParamBlock = pkAttribute->GetParamBlock(0);

                pkParamBlock->GetDesc();
                int num_params = pkParamBlock->NumParams();

                int param_type;
                NiExtraDataPtr spExtraData = NULL;

                for (int i = 0; i < num_params; i++)
                {
                    ParamID kID = pkParamBlock->IndextoID(i);
                    param_type = pkParamBlock->GetParameterType(kID);

                    if (TYPE_TEXMAP == param_type)
                    {
                        ParamBlockDesc2 *pkParamDesc = pkParamBlock->GetDesc();

                        if (!pkParamDesc)
                            return;

                        pkParamDesc->GetParamDef(kID);

                        Texmap* Texmap_value;
                        Interval valid_interval;
                        BOOL retrievedTexmap = false;
                        retrievedTexmap = pkParamBlock->GetValue(kID, 0, 
                            Texmap_value, valid_interval);
                        if (retrievedTexmap)
                            pkNiMAXUV->Convert(Texmap_value);
                    }
                }
                CHECK_MEMORY();
            }
        }
    }

}
//---------------------------------------------------------------------------
void AssignFlipController(NiFlipController* pkFlipper, 
    NiTexturingProperty* pkTex, NiTexturingProperty::ShaderMap* pkMap)
{
    if (pkFlipper)
    {
        pkFlipper->SetTarget(pkTex);
        pkFlipper->SetAffectedMap(pkMap);
    }
}
//---------------------------------------------------------------------------
NiMAXCustAttribConverter::NiMAXCustAttribConverter(TimeValue animStart, 
    TimeValue animEnd)
{
    m_kAnimStart = animStart;
    m_kAnimEnd = animEnd;
}

//---------------------------------------------------------------------------
NiMAXCustAttribConverter::~NiMAXCustAttribConverter()
{
}
//---------------------------------------------------------------------------
void NiMAXCustAttribConverter::Init()
{
}

//---------------------------------------------------------------------------
void NiMAXCustAttribConverter::Shutdown()
{
}

//---------------------------------------------------------------------------
void NiMAXCustAttribConverter::Convert(Animatable *obj, NiObjectNET* pkNiObj,
    bool bIgnoreStandardCAs, NiTexturingProperty* pkTexProp, 
    NiMAXUV* pkNiMAXUV)
{

    if (obj && obj->SuperClassID() == GEN_DERIVOB_CLASS_ID)
    {
        IDerivedObject* pkDerived = (IDerivedObject*) obj;
        for (int i = 0; i < pkDerived->NumModifiers(); i++)
        {
            Modifier* pkMod = pkDerived->GetModifier(i);
            if (pkMod)
            {            
                Convert(pkMod, pkNiObj, bIgnoreStandardCAs, pkTexProp, 
                    pkNiMAXUV);
            }
        }

        Object* pkObjRef = pkDerived->GetObjRef();
        if (pkObjRef != obj)
        {            
            Convert(pkObjRef, pkNiObj, bIgnoreStandardCAs, pkTexProp, 
                pkNiMAXUV);
        }

        BaseObject* pkBaseObj = pkDerived->FindBaseObject();
        if (pkBaseObj != obj && pkBaseObj != pkObjRef)
        {            
            Convert(pkBaseObj, pkNiObj, bIgnoreStandardCAs, pkTexProp, 
                pkNiMAXUV);
        }
    }

    /*BaseObject* pkBaseObj = GetObjectInterface(obj);
    if (pkBaseObj)
    {
        pkBaseObj = pkBaseObj->FindBaseObject();
        if (pkBaseObj != obj)
        {            
            Convert(pkBaseObj, pkNiObj, bIgnoreStandardCAs, pkTexProp, 
                pkNiMAXUV);
        }
    }*/

    if (!obj || !pkNiObj)
        return;

    ICustAttribContainer* cc = obj->GetCustAttribContainer();
    if (cc) 
    {
        int num_attribs = cc->GetNumCustAttribs();
        CustAttrib * pkAttribute = NULL;

        for (int attrib = 0; attrib < num_attribs; attrib ++)
        {
            pkAttribute = (CustAttrib*) cc->GetCustAttrib(attrib);
            if (pkAttribute)
            {
                CHECK_MEMORY();
                if (IsMaxCA(pkAttribute->GetName()) || (bIgnoreStandardCAs && 
                    IsStandardCA(pkAttribute->GetName())))
                {
                    continue;
                }
                Class_ID cId = pkAttribute->ClassID();
                NiOutputDebugString("Converting Cust Attrib:");
                NiOutputDebugString(pkAttribute->GetName());
                NiOutputDebugString("\n");
                        
                for (int iWhichPB = 0; iWhichPB < 
                     pkAttribute->NumParamBlocks(); iWhichPB++)
                {
                    IParamBlock2* pkParamBlock = 
                        pkAttribute->GetParamBlock(iWhichPB);
                    if (pkParamBlock != NULL)
                    {
                        if (!ConvertParamBlock(pkParamBlock, pkNiObj,
                            pkTexProp, pkNiMAXUV))
                        {
                        }
                    }
                }
            }
        }
    }
}

//---------------------------------------------------------------------------
bool NiMAXCustAttribConverter::ConvertParamBlock(IParamBlock2* pkParamBlock, 
        NiObjectNET* pkNiObj, NiTexturingProperty* pkTexProp , 
        NiMAXUV* pkNiMAXUV )
{
    pkParamBlock->GetDesc();
    int num_params = pkParamBlock->NumParams();

    int param_type;
    NiExtraData* pkExtraData = NULL;

    for (int i = 0; i < num_params; i++)
    {
        param_type = 
            pkParamBlock->GetParameterType(pkParamBlock->IndextoID(i));
        
        switch (param_type) 
        {
            case TYPE_INODE:
            {
                pkExtraData = ConvertNodeAttrib(pkParamBlock, i, pkNiObj, i);
                break;
            }
            case TYPE_STRING:
            {
                
                pkExtraData = ConvertStringAttrib(pkParamBlock, i, pkNiObj, i);
                break;
            }
            case TYPE_FLOAT:
            {
                pkExtraData = ConvertFloatAttrib(pkParamBlock, i, pkNiObj, i);
                break;
            }
            case TYPE_INT:
            {
                pkExtraData = ConvertIntAttrib(pkParamBlock, i, pkNiObj, i);
                break;
            }
            case TYPE_RGBA:
            {
                pkExtraData = ConvertColorAttrib(pkParamBlock, i, pkNiObj, i);
                break;
            }
            case TYPE_POINT3:
            {
                pkExtraData = ConvertPoint3Attrib(pkParamBlock, i, pkNiObj, i);
                break;
            }
            case TYPE_POINT4:
            {
                pkExtraData = ConvertPoint4Attrib(pkParamBlock, i, pkNiObj, i);
                break;
            }
            case TYPE_FRGBA:
            {
                pkExtraData = ConvertFRGBAAttrib(pkParamBlock, i, pkNiObj, i);
                break;
            }
            case TYPE_BOOL:
            {
                pkExtraData = ConvertBoolAttrib(pkParamBlock, i, pkNiObj, i);
                break;
            }
            case TYPE_FLOAT_TAB:
            {
                pkExtraData = ConvertFloatTabAttrib(pkParamBlock, i, pkNiObj, 
                    i);
                break;
            }
            case TYPE_RGBA_TAB:
            {
                pkExtraData = ConvertColorTabAttrib(pkParamBlock, i, pkNiObj,
                    i);
                break;
            }
            case TYPE_INT_TAB:
            {
                pkExtraData = ConvertIntTabAttrib(pkParamBlock, i, pkNiObj, i);
                break;
            }
            case TYPE_STRING_TAB:
            {
                pkExtraData = ConvertStringTabAttrib(pkParamBlock, i, 
                    pkNiObj, i);
                break;
            }
            case TYPE_TEXMAP:
            {
                pkExtraData = ConvertTexMapAttrib(pkParamBlock, i, 
                    pkNiObj, pkTexProp, pkNiMAXUV, i);
                break;
            }
            default:
            {
                ParamBlockDesc2 *pkParamDesc = pkParamBlock->GetDesc();
                TCHAR* pcAttribName = NULL;
                if (pkParamDesc)
                {
                    ParamDef& kParamDef = 
                        pkParamDesc->GetParamDef(pkParamDesc->IndextoID(i));
                    pcAttribName = kParamDef.int_name;
                }

                char acString[1024];
                NiSprintf(acString, 1024, "Unknown Custom Attribute: "
                    "Attribute \"%s\"is of an unknown custom "
                    "attribute type %d.\n", pcAttribName, param_type);
                NILOGWARNING(acString);
                break;
            }
        }
    }
    CHECK_MEMORY();

    return true;
}
//---------------------------------------------------------------------------
bool NiMAXCustAttribConverter::ConvertTexmap(
        Texmap* pkTm, 
        NiTexture::FormatPrefs& kPrefs, 
        unsigned int& uiSizeDivideBy,
        NiTObjectArray<NiString>*& pkFilenames, 
        bool& bExternalImages, 
        bool& bOriginalImages, 
        bool& bOriginalImagesVRAM,
        NiTexturingProperty::FilterMode& eFilterMode, 
        NiPixelFormat& ePixelFormat, 
        bool bImageHasAlpha, 
        unsigned short& usMaxAnisotropy)
{
    if (!pkTm)
        return false;

    ICustAttribContainer* cc = pkTm->GetCustAttribContainer();
    if (cc)
    {
        int num_attribs = cc->GetNumCustAttribs();
        CustAttrib * pkAttribute = NULL;

        for (int attrib = 0; attrib < num_attribs; attrib ++ )
        {
            pkAttribute = (CustAttrib *) cc->GetCustAttrib( attrib );
            if (pkAttribute)
            {
                CHECK_MEMORY();
                Class_ID cId = pkAttribute->ClassID();

                if (NiStricmp(pkAttribute->GetName(), "TextureExport") == 0 || 
                    NiStricmp(pkAttribute->GetName(), 
                    "TextureExportSettings") == 0)
                {
                    if (!ConvertTextureExportCA(
                        pkAttribute, 
                        kPrefs, 
                        uiSizeDivideBy, 
                        bExternalImages, 
                        bOriginalImages,
                        bOriginalImagesVRAM, 
                        eFilterMode, 
                        ePixelFormat,
                        bImageHasAlpha,
                        usMaxAnisotropy))
                        return false;
                }
                else if (NiStricmp(pkAttribute->GetName(), 
                    "MipMapPyramid") == 0 || NiStricmp(pkAttribute->GetName(), 
                    "MipMapPyramidSettings") == 0)
                {
                    if (!ConvertMipMapPyramidCA(pkAttribute, kPrefs, 
                        pkFilenames))
                    {
                        return false;
                    }
                }
                else 
                {
                    return false;
                }
            }
        }
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiMAXCustAttribConverter::ConvertTextureExportCA(
    CustAttrib * pkAttribute, 
    NiTexture::FormatPrefs& kPrefs, 
    unsigned int& uiSizeDivideBy, 
    bool& bExternalImages, 
    bool& bOriginalImages, 
    bool& bOriginalImagesVRAM,
    NiTexturingProperty::FilterMode& eFilterMode, 
    NiPixelFormat& ePixelFormat, 
    bool bImageHasAlpha,
    unsigned short& uiMaxAnisotropy )
{
    NiDevImageConverter::Platform ePlatform = 
        NiMAXTextureConverter::GetPlatform();
    IParamBlock2* pkParamBlock = pkAttribute->GetParamBlock(0);
    pkParamBlock->GetDesc();
    int num_params = pkParamBlock->NumParams();

    int param_type;
    int iFilterMode = 0;
    int iMaxAnisotropy = 1;
    int iGlobalMasterIndex = 0;


    // These indexes correspond to the MasterIndex in 
    // TextureExportCustAttribs.ms (+1 since MAXScript uses 1-based counting.
    const int iGeneric = 0;
    const int iXenon = 1;
    const int iPS3 = 2;
    const int iDX9 = 3;
    const int iWii = 4;
    const int iD3D10 = 5;

    bool bUseMasterIndexForAll = false;

    switch(ePlatform)
    {
        case NiDevImageConverter::ANY:    
            iGlobalMasterIndex = iGeneric;
            break;
        case NiDevImageConverter::XENON:
            iGlobalMasterIndex = iXenon;
            break;
        case NiDevImageConverter::PLAYSTATION3:
            iGlobalMasterIndex = iPS3;
            break;
        case NiDevImageConverter::WII:
            iGlobalMasterIndex = iWii;
            break;
        case NiDevImageConverter::DX9:
            iGlobalMasterIndex = iDX9;
            break;
        case NiDevImageConverter::D3D10:
            iGlobalMasterIndex = iD3D10;
            break;
        default:
            iGlobalMasterIndex = iGeneric;
    }
    
    for (int iIndex = 0; iIndex < num_params; iIndex++)
    {
        int iMasterIndex = iGlobalMasterIndex;
        ParamID kID = pkParamBlock->IndextoID(iIndex);
        param_type = pkParamBlock->GetParameterType(kID);

        switch (param_type)
        {
            case TYPE_INT:
            case TYPE_BOOL:
                iMasterIndex = 0;
            case TYPE_INT_TAB:
            {
                ParamBlockDesc2 *pkParamDesc = 
                    pkParamBlock->GetDesc();

                if (!pkParamDesc)
                    return NULL;

                ParamDef& kParamDef = 
                    pkParamDesc->GetParamDef(kID);

                int int_value;
                Interval valid_interval;
                BOOL retrieved = false;
                retrieved = pkParamBlock->GetValue(kID, 0, int_value, 
                    valid_interval, iMasterIndex);
                if (retrieved)
                {
                    TSTR value_name = kParamDef.int_name;
                    if (!IsValidName(value_name))
                        value_name = pkParamBlock->GetLocalName(kID);
                    NIASSERT(IsValidName(value_name));

                    if (NiStricmp("UseMasterIndexForAll", value_name) == 0)
                    {
                        bUseMasterIndexForAll = int_value != 0; 
                        // true or false
                    }
                    else if (NiStricmp("MasterIndex", value_name) == 0
                        && bUseMasterIndexForAll)
                    {
                        iGlobalMasterIndex = int_value - 1;
                    }
                    else if (NiStricmp("FinalImageSize", value_name) == 0)
                    {
                        if (int_value == 1)
                            uiSizeDivideBy = 1;
                        else if (int_value == 2) // Half size
                            uiSizeDivideBy = 2;
                        else if (int_value == 3) // Quarter size
                            uiSizeDivideBy = 4;
                        else
                            uiSizeDivideBy = 1;
                    }
                    else if (NiStricmp("FinalImageQuality", value_name) == 0 &&
                        kPrefs.m_ePixelLayout != 
                        NiTexture::FormatPrefs::BUMPMAP)
                    {
                        if (int_value == 1) // "Image Default"
                        {                            
                            kPrefs.m_ePixelLayout =
                                NiTexture::FormatPrefs::PIX_DEFAULT;
                        }
                        else if (int_value == 3) //"16 bit (All)",
                        {
                            kPrefs.m_ePixelLayout = 
                                NiTexture::FormatPrefs::TRUE_COLOR_32;
                            if (bImageHasAlpha)
                                ePixelFormat = NiPixelFormat::RGBA32;
                            else
                                ePixelFormat = NiPixelFormat::RGB24;
                        }
                        else if (int_value == 4) //"16 bit (All)",
                        {
                            kPrefs.m_ePixelLayout = 
                                NiTexture::FormatPrefs::HIGH_COLOR_16;
                            ePixelFormat = NiPixelFormat::RGBA16;
                        }
                        else if (int_value == 5 && 
                          NiMAXTextureConverter::GetDevImageQuantizerFound()) 
                          //"8 bit Palettized (All)",
                        {
                            kPrefs.m_ePixelLayout = 
                                NiTexture::FormatPrefs::PALETTIZED_8;
                            if (bImageHasAlpha)
                                ePixelFormat = NiPixelFormat::PALA8;
                            else
                                ePixelFormat = NiPixelFormat::PAL8;
                        }
                        else if (int_value == 6 && 
                          NiMAXTextureConverter::GetDevImageQuantizerFound()) 
                          // "DXT1 (All)", 
                        {
                            kPrefs.m_ePixelLayout = 
                                NiTexture::FormatPrefs::COMPRESSED;
                            ePixelFormat = 
                                NiPixelFormat(NiPixelFormat::DXT1);
                        }
                        else if (int_value == 7 && 
                          NiMAXTextureConverter::GetDevImageQuantizerFound()) 
                          //"DXT3 (All)"
                        {
                            kPrefs.m_ePixelLayout = 
                                NiTexture::FormatPrefs::COMPRESSED;
                            ePixelFormat = 
                                NiPixelFormat(NiPixelFormat::DXT3);
                        }
                        else if (int_value == 8 && 
                          NiMAXTextureConverter::GetDevImageQuantizerFound()) 
                          //"DXT5 (All)"
                        {
                            kPrefs.m_ePixelLayout = 
                                NiTexture::FormatPrefs::COMPRESSED;
                            ePixelFormat = 
                                NiPixelFormat(NiPixelFormat::DXT5);
                        }
                        else //"Use Global Settings"
                        {
                            kPrefs.m_ePixelLayout = 
                                NiMAXTextureConverter::GetPixelLayout();
                        }
                    }
                    else if (NiStricmp("FilterModeEx", value_name) == 0)
                    {
                        iFilterMode = int_value - 1;
                    }
                    else if (NiStricmp("MaxAnisotropy", value_name) == 0)
                    {
                        iMaxAnisotropy = int_value;
                    }
                    else if (NiStricmp("FileSettings", value_name) == 0)
                    {
                        if (int_value == 1) // "Use Global Settings"
                        {
                            bExternalImages = 
                          NiMAXTextureConverter::GetSaveTexturesSeparately();
                            bOriginalImages = 
                                NiMAXTextureConverter::GetUseOrigExt();
                            bOriginalImagesVRAM =
                                NiMAXTextureConverter::GetUseOrigExtVRAM();
                        }
                        else if (int_value == 2) // "Internal To NIF"
                        {
                            bExternalImages = false;
                            bOriginalImages = false;
                            bOriginalImagesVRAM = false;
                        }
                        else if (int_value == 3) // "External NIF texture"
                        {
                            bExternalImages = true;
                            bOriginalImages = false;
                            bOriginalImagesVRAM = false;
                        }
                        else if (int_value == 4) // "Original Images"
                        {
                            bExternalImages = true;
                            bOriginalImages = true;
                            bOriginalImagesVRAM = false;
                        }
                        else if (int_value == 5) // "Original Images VRAM"
                        {
                            bExternalImages = true;
                            bOriginalImages = false;
                            bOriginalImagesVRAM = true;
                        }
                    }
                }
            }

        }
    }

    // If size is divided by one, we can only have internal 
    // textures or external nif textures.
    if (uiSizeDivideBy != 1)
    {
        if (bExternalImages == true && bOriginalImages == true)
        {
            // Use the global setting to determine what is best here
            if (NiMAXTextureConverter::GetSaveTexturesSeparately())
            {
                bExternalImages = true; 
                bOriginalImages = false;
            }
            else
            {
                bExternalImages = false;
                bOriginalImages = false;
            }
        }
    }

    // Now that we have the filtermode settings, we can create
    // the appropriate composited NiTexturingProperty::FilterMode
    switch (iFilterMode)
    {
        case NI_NEAREST:
        {
            kPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
            eFilterMode = NiTexturingProperty::FILTER_NEAREST;
            break;
        }
        case NI_BILERP:
        {
            kPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
            eFilterMode = NiTexturingProperty::FILTER_BILERP;
            break;
        }
        case NI_NEAREST_MIPNEAREST:            
        {
            kPrefs.m_eMipMapped = NiTexture::FormatPrefs::YES;
            eFilterMode = NiTexturingProperty::FILTER_NEAREST_MIPNEAREST;
            break;
        }
        case NI_NEAREST_MIPLERP:
        {
            kPrefs.m_eMipMapped = NiTexture::FormatPrefs::YES;
            eFilterMode = NiTexturingProperty::FILTER_NEAREST_MIPLERP;
            break;
        }    
        case NI_BILERP_MIPNEAREST:
        {
            kPrefs.m_eMipMapped = NiTexture::FormatPrefs::YES;
            eFilterMode = NiTexturingProperty::FILTER_BILERP_MIPNEAREST;
            break;
        }
        case NI_TRILERP:
        {
            kPrefs.m_eMipMapped = NiTexture::FormatPrefs::YES;
            eFilterMode = NiTexturingProperty::FILTER_TRILERP;
            break;
        }
        case NI_ANISOTROPIC:
        {
            kPrefs.m_eMipMapped = NiTexture::FormatPrefs::YES;
            eFilterMode = NiTexturingProperty::FILTER_ANISOTROPIC;
            uiMaxAnisotropy = iMaxAnisotropy;
            break;
        }          
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiMAXCustAttribConverter::ConvertMipMapPyramidCA(
    CustAttrib * pkAttribute, NiTexture::FormatPrefs&, 
    NiTObjectArray<NiString>*& pkFilenames)
{
    IParamBlock2* pkParamBlock = pkAttribute->GetParamBlock(0);
    pkParamBlock->GetDesc();
    int num_params = pkParamBlock->NumParams();

    int param_type;
    bool bAutoGenMipMaps = true;

    for (int iIndex = 0; iIndex < num_params; iIndex++)
    {
        ParamID kID = pkParamBlock->IndextoID(iIndex);
        param_type = pkParamBlock->GetParameterType(kID);
        
        switch (param_type)
        {
            case TYPE_BOOL:
            {
                ParamBlockDesc2 *pkParamDesc = 
                    pkParamBlock->GetDesc();

                if (!pkParamDesc)
                    return NULL;

                ParamDef& kParamDef = pkParamDesc->GetParamDef(kID);
                
                int int_value;
                Interval valid_interval;
                BOOL retrieved = false;
                retrieved = pkParamBlock->GetValue(kID, 0, int_value, 
                    valid_interval);

                if (retrieved)
                {
                    TSTR value_name = kParamDef.int_name;
                    if (!IsValidName(value_name))
                        value_name = pkParamBlock->GetLocalName(kID);
                    NIASSERT(IsValidName(value_name));

                    bAutoGenMipMaps = int_value ? true : false;
                }
                break;
            }
            case TYPE_BITMAP_TAB:
            {
                ParamBlockDesc2 *pkParamDesc = 
                    pkParamBlock->GetDesc();
                
                if (!pkParamDesc)
                    return NULL;

                pkParamDesc->GetParamDef(kID);
                
                int iCount = pkParamBlock->Count(kID);

                if (iCount == 0)
                    break;

                pkFilenames = NiNew NiTObjectArray<NiString>;
                pkFilenames->SetSize(iCount);
                
                IParamBlock2* pkParams = pkParamBlock;
                bool bCompleteChain = false;

                for (int i = 0; i < iCount; i++)
                {
                    PBBitmap* pkBitmap = 
                        pkParams->GetBitmap(kID, 0, i);
                    if (pkBitmap)
                    {
                        if (BMMGetFullFilename(&pkBitmap->bi) == TRUE)
                        {
                            const TCHAR* pcFilename = pkBitmap->bi.Name();
                            if (pkBitmap->bi.Width() == 1 && 
                                pkBitmap->bi.Height() == 1)
                            {
                                bCompleteChain = true;
                            }
                            pkFilenames->Add(pcFilename);
                        }
                        else
                        {
                            NiOutputDebugString("Failed to load file in "
                                "map-map pyramid.\n");
                            bAutoGenMipMaps = true;
                            break;
                        }
                    }
                    else if (!bCompleteChain)
                    {
                        NiOutputDebugString("Incomplete mip-map pyramid"
                            " found!\n");
                        bAutoGenMipMaps = true;
                        break;
                    }
                    else
                    {
                        break;
                    }
                }

                break;
            }
            default:
                return false;
        }
    }

    if (bAutoGenMipMaps && pkFilenames != NULL)
    {
        NiDelete pkFilenames;
        pkFilenames = NULL;
    }

    return true;
}
//---------------------------------------------------------------------------
NiExtraData* NiMAXCustAttribConverter::ConvertFloatAttrib(
    IParamBlock2* pkParamBlock, int iIndex, NiObjectNET* pkObj, 
    int&)
{
    ParamBlockDesc2 *pkParamDesc = pkParamBlock->GetDesc();
    ParamID kID = pkParamDesc->IndextoID(iIndex);
    
    if (!pkParamDesc)
        return NULL;

    ParamDef& kParamDef = pkParamDesc->GetParamDef(kID);

    float float_value;
    Interval valid_interval;
    BOOL retrieved = false;
    retrieved = pkParamBlock->GetValue(kID, 0, float_value, valid_interval);

    if (retrieved)
    {
        TSTR value_name = kParamDef.int_name;
        if (!IsValidName(value_name))
            value_name = pkParamBlock->GetLocalName(kID);
        NIASSERT(IsValidName(value_name));

        NiFloatExtraDataPtr spFloatAttrib = NiNew NiFloatExtraData(
            float_value);
        spFloatAttrib->SetName(value_name.data());
        pkObj->AddExtraData(spFloatAttrib);

        Control * pkControl = pkParamBlock->GetController(kID, 0);
        if (pkControl)
        {
            NiMAXAnimationConverter anim(m_kAnimStart, m_kAnimEnd);
                       
            // convert a float animation
            NiInterpolatorPtr spInterp = anim.ConvertFloatAnim(pkControl);

            if (!spInterp)
                return W3D_STAT_OK;
        
            NiKeyBasedInterpolator* pkKeyInterp = NiDynamicCast(
                NiKeyBasedInterpolator, spInterp);

            if (pkKeyInterp && pkKeyInterp->GetKeyCount(0) != 0)
            {
                // Set the ExtraDataName instead of trying to preserve the
                //    ExtraData pointer reference across multiple streams.
                NiFloatExtraDataController* pkController = 
                    NiNew NiFloatExtraDataController(spFloatAttrib->GetName());

                pkKeyInterp->FillDerivedValues(0);
                pkController->SetInterpolator(spInterp);
                pkController->ResetTimeExtrema();
                pkController->SetTarget(pkObj);
    
                NiMAXAnimationConverter::SetORT(pkControl, pkController);
    
                if (NiMAXConverter::GetUseAppTime())
                    pkController->SetAnimType(NiTimeController::APP_TIME);
                else
                    pkController->SetAnimType(NiTimeController::APP_INIT);
   
            }

        }

        return spFloatAttrib;
    }
    else
        return NULL;
}
//---------------------------------------------------------------------------
NiExtraData* NiMAXCustAttribConverter::ConvertStringAttrib(
    IParamBlock2* pkParamBlock, int iIndex, NiObjectNET* pkObj, 
    int&)
{
    ParamBlockDesc2 *pkParamDesc = pkParamBlock->GetDesc();
    ParamID kID = pkParamDesc->IndextoID(iIndex);
    
    if (!pkParamDesc)
        return NULL;

    ParamDef& kParamDef = pkParamDesc->GetParamDef(kID);
    
    TCHAR* string_value;
    Interval valid_interval;
    BOOL retrieved = false;
    retrieved = pkParamBlock->GetValue(kID, 0, string_value, valid_interval);
    
    if (retrieved)
    {
        TSTR value_name = kParamDef.int_name;
        if (!IsValidName(value_name))
            value_name = pkParamBlock->GetLocalName(kID);
        NIASSERT(IsValidName(value_name));

        NiStringExtraDataPtr spStringAttrib = NiNew NiStringExtraData(
            string_value);
        spStringAttrib->SetName(value_name.data());
        pkObj->AddExtraData(spStringAttrib);
        return spStringAttrib;
    }
    else
        return NULL;

}
//---------------------------------------------------------------------------
NiExtraData* NiMAXCustAttribConverter::ConvertNodeAttrib(
    IParamBlock2* pkParamBlock, int iIndex, NiObjectNET* pkObj, 
    int&)
{
    ParamBlockDesc2 *pkParamDesc = pkParamBlock->GetDesc();
    ParamID kID = pkParamDesc->IndextoID(iIndex);

    if (!pkParamDesc)
        return NULL;

    ParamDef& kParamDef = pkParamDesc->GetParamDef(kID);

    
    INode* node_value;
    Interval valid_interval;
    BOOL retrieved = false;
    retrieved = pkParamBlock->GetValue (kID, 0, node_value, valid_interval);
    
    if (retrieved)
    {
        TSTR value_name = kParamDef.int_name;
        if (!IsValidName(value_name))
            value_name = pkParamBlock->GetLocalName(kID);
        NIASSERT(IsValidName(value_name));

        unsigned long* pkPointer = NiAlloc(unsigned long, 1);
        pkPointer[0] = (unsigned long) PtrToUlong(node_value);
        NiBinaryExtraDataPtr spBinaryAttrib = 
            NiNew NiBinaryExtraData(sizeof(unsigned long), (char*)(pkPointer));
        spBinaryAttrib->SetName(value_name.data());
        pkObj->AddExtraData(spBinaryAttrib);
        return spBinaryAttrib;
    }
    else
        return NULL;

}
//---------------------------------------------------------------------------
NiExtraData* NiMAXCustAttribConverter::ConvertColorAttrib(
    IParamBlock2* pkParamBlock, int iIndex, NiObjectNET* pkObj,
    int& iIncrement)
{
    ParamBlockDesc2 *pkParamDesc = pkParamBlock->GetDesc();
    int num_params = pkParamBlock->NumParams();
    ParamID kColorID = pkParamDesc->IndextoID(iIndex);
    ParamID kAlphaID = pkParamDesc->IndextoID(iIndex + 1);

    if (!pkParamDesc)
        return NULL;

    ParamDef& kColorParamDef = pkParamDesc->GetParamDef(kColorID);

    Color color_value;
    float alpha_value = 255.0f;
    Interval valid_interval;
    BOOL retrievedColor = false;
    BOOL retrievedAlpha = false;
    
    int iAlphaIndex = iIndex + 1;
    retrievedColor = pkParamBlock->GetValue(kColorID, 0, color_value, 
        valid_interval);

    if (iAlphaIndex < num_params)
    {
        int param_type = pkParamBlock->GetParameterType(kAlphaID);
        if (param_type == TYPE_FLOAT)
        {
            retrievedAlpha = pkParamBlock->GetValue(kAlphaID, 
                0, alpha_value, valid_interval);
        }
    }

    if (retrievedColor)
    {
        TSTR value_name = kColorParamDef.int_name;
        if (!IsValidName(value_name))
            value_name = pkParamBlock->GetLocalName(kColorID);
        NIASSERT(IsValidName(value_name));

        Control * pkColorControl = pkParamBlock->GetController(kColorID, 0);

        bool bHasAlpha = false;
        Control* pkAlphaControl = NULL;
        if (retrievedAlpha)
        {
            ParamDef& kAlphaParamDef = pkParamDesc->GetParamDef(kAlphaID);
            TSTR value_alpha_name = kAlphaParamDef.int_name;
            if (!IsValidName(value_alpha_name))
                value_alpha_name = pkParamBlock->GetLocalName(kAlphaID);
            pkAlphaControl = pkParamBlock->GetController(kAlphaID, 0);
            NIASSERT(IsValidName(value_alpha_name));

            NiString strColorName = value_name;
            strColorName += "Alpha";
            if (strColorName == value_alpha_name)
            {
                bHasAlpha = true;
                iIncrement++;
            }
            else
            {
                pkAlphaControl = NULL;
            }
        }

        NiColorExtraDataPtr spColorAttrib;
        if (bHasAlpha)
        {
            NiColorA kColor(color_value.r, color_value.g, 
                color_value.b, alpha_value);
            spColorAttrib = NiNew NiColorExtraData(kColor);
        }
        else
        {
            NiColor kColor(color_value.r, color_value.g, 
                color_value.b);
            spColorAttrib = NiNew NiColorExtraData(kColor);
        }
        spColorAttrib->SetName(value_name.data());
        pkObj->AddExtraData(spColorAttrib);

        if (pkColorControl || pkAlphaControl)
        {
            CreateColorCustAttribAnimator(pkColorControl, pkAlphaControl, 
                pkObj, spColorAttrib);
        }

        return spColorAttrib;
    }
    else
        return NULL;
}
//---------------------------------------------------------------------------
NiExtraData* NiMAXCustAttribConverter::ConvertPoint3Attrib(
    IParamBlock2* pkParamBlock, int iIndex, NiObjectNET* pkObj, 
    int&)
{
    ParamBlockDesc2 *pkParamDesc = pkParamBlock->GetDesc();
    ParamID kID = pkParamDesc->IndextoID(iIndex);
    if (!pkParamDesc)
        return NULL;

    ParamDef& kParamDef = pkParamDesc->GetParamDef(kID);

    
    Point3 vector_value;
    Interval valid_interval;
    BOOL retrieved = false;
    retrieved = pkParamBlock->GetValue(kID, 0, vector_value, valid_interval);

    if (retrieved)
    {
        TSTR value_name = kParamDef.int_name;
        if (!IsValidName(value_name))
            value_name = pkParamBlock->GetLocalName(kID);
        NIASSERT(IsValidName(value_name));

        NiPoint3 kVector(vector_value.x, vector_value.y, vector_value.z);
        NiVectorExtraDataPtr spVectorAttrib = NiNew NiVectorExtraData(kVector);
        spVectorAttrib->SetName(value_name.data());
        pkObj->AddExtraData(spVectorAttrib);
        return spVectorAttrib;
    }
    else
        return NULL;
}
//---------------------------------------------------------------------------
NiExtraData* NiMAXCustAttribConverter::ConvertPoint4Attrib(
    IParamBlock2* pkParamBlock, int iIndex, NiObjectNET* pkObj, 
    int&)
{
    ParamBlockDesc2 *pkParamDesc = pkParamBlock->GetDesc();
    ParamID kID = pkParamDesc->IndextoID(iIndex);
    if (!pkParamDesc)
        return NULL;

    ParamDef& kParamDef = pkParamDesc->GetParamDef(kID);
    
    Point4 vector_value;
    Interval valid_interval;
    BOOL retrieved = false;
    retrieved = pkParamBlock->GetValue(kID, 0, vector_value, valid_interval);

    if (retrieved)
    {
        TSTR value_name = kParamDef.int_name;
        if (!IsValidName(value_name))
            value_name = pkParamBlock->GetLocalName(kID);
        NIASSERT(IsValidName(value_name));

        float fArray[4];
        fArray[0] = vector_value.x;
        fArray[1] = vector_value.y;
        fArray[2] = vector_value.z;
        fArray[3] = vector_value.w;

        NiFloatsExtraDataPtr spFloatsAttrib = NiNew NiFloatsExtraData(4, 
            (const float*) &fArray);
        spFloatsAttrib->SetName(value_name.data());
        pkObj->AddExtraData(spFloatsAttrib);
        return spFloatsAttrib;
    }
    else
        return NULL;
}
//---------------------------------------------------------------------------
NiExtraData* NiMAXCustAttribConverter::ConvertFRGBAAttrib(
    IParamBlock2* pkParamBlock, int iIndex, NiObjectNET* pkObj, 
    int&)
{
    ParamBlockDesc2 *pkParamDesc = pkParamBlock->GetDesc();
    ParamID kID = pkParamDesc->IndextoID(iIndex);
    if (!pkParamDesc)
        return NULL;

    ParamDef& kParamDef = pkParamDesc->GetParamDef(kID);
    
    Point4 vector_value;
    Interval valid_interval;
    BOOL retrieved = false;
    retrieved = pkParamBlock->GetValue(kID, 0, vector_value, valid_interval);

    if (retrieved)
    {
        TSTR value_name = kParamDef.int_name;
        if (!IsValidName(value_name))
            value_name = pkParamBlock->GetLocalName(kID);
        NIASSERT(IsValidName(value_name));

        NiColorA kColor(vector_value.x, vector_value.y, 
                vector_value.z, vector_value.w);
        NiColorExtraDataPtr spColorAttrib = NiNew NiColorExtraData(kColor);
        spColorAttrib->SetName(value_name.data());
        pkObj->AddExtraData(spColorAttrib);
        return spColorAttrib;
    }
    else
        return NULL;
}
//---------------------------------------------------------------------------
NiExtraData* NiMAXCustAttribConverter::ConvertMatrix3Attrib(
    IParamBlock2* pkParamBlock, int iIndex, NiObjectNET*, 
    int&)
{
    ParamBlockDesc2 *pkParamDesc = pkParamBlock->GetDesc();
    ParamID kID = pkParamDesc->IndextoID(iIndex);
    if (!pkParamDesc)
        return NULL;

    pkParamDesc->GetParamDef(kID);

    Matrix3 matrix_value;
    Interval valid_interval;
    BOOL retrieved = false;
    retrieved = pkParamBlock->GetValue(kID, 0, matrix_value, valid_interval);
    return NULL;
}
//---------------------------------------------------------------------------
NiExtraData* NiMAXCustAttribConverter::ConvertIntAttrib(
    IParamBlock2* pkParamBlock, int iIndex, NiObjectNET* pkObj, 
    int&)
{
    ParamBlockDesc2 *pkParamDesc = pkParamBlock->GetDesc();
    ParamID kID = pkParamDesc->IndextoID(iIndex);
    if (!pkParamDesc)
        return NULL;

    ParamDef& kParamDef = pkParamDesc->GetParamDef(kID);
    
    int int_value;
    Interval valid_interval;
    BOOL retrieved = false;
    retrieved = pkParamBlock->GetValue(kID, 0, int_value, valid_interval);
    if (retrieved)
    {
        TSTR value_name = kParamDef.int_name;
        if (!IsValidName(value_name))
            value_name = pkParamBlock->GetLocalName(kID);
        NIASSERT(IsValidName(value_name));

        int i = int_value;
        NiIntegerExtraData* pkIntAttrib = NiNew NiIntegerExtraData(i);
        pkIntAttrib->SetName(value_name.data());
        pkObj->AddExtraData(pkIntAttrib);
        return pkIntAttrib;
    }
    else
        return NULL;
}
//---------------------------------------------------------------------------
NiExtraData* NiMAXCustAttribConverter::ConvertBoolAttrib(
    IParamBlock2* pkParamBlock, int iIndex, NiObjectNET* pkObj, 
    int&)
{
    ParamBlockDesc2 *pkParamDesc = pkParamBlock->GetDesc();
    ParamID kID = pkParamDesc->IndextoID(iIndex);

    if (!pkParamDesc)
        return NULL;

    ParamDef& kParamDef = pkParamDesc->GetParamDef(kID);

    int int_value;
    Interval valid_interval;
    BOOL retrieved = false;
    retrieved = pkParamBlock->GetValue(kID, 0, int_value, valid_interval);

    if (retrieved)
    {
        TSTR value_name = kParamDef.int_name;
        if (!IsValidName(value_name))
            value_name = pkParamBlock->GetLocalName(kID);
        NIASSERT(IsValidName(value_name));

        bool b = int_value ? true : false;
        NiBooleanExtraDataPtr spIntAttrib = NiNew NiBooleanExtraData(b);
        spIntAttrib->SetName(value_name.data());
        pkObj->AddExtraData(spIntAttrib);
        return spIntAttrib;
    }
    else
        return NULL;
}
//---------------------------------------------------------------------------
NiExtraData* NiMAXCustAttribConverter::ConvertFloatTabAttrib(IParamBlock2* 
    pkParamBlock, int iIndex, NiObjectNET* pkObj, int&)
{
    ParamBlockDesc2 *pkParamDesc = pkParamBlock->GetDesc();
    ParamID kID = pkParamDesc->IndextoID(iIndex);
    if (!pkParamDesc)
        return NULL;

    ParamDef& kParamDef = pkParamDesc->GetParamDef(kID);

    Interval valid_interval;
    BOOL retrieved = true;
    
    int iCount = pkParamBlock->Count(kID);

    if (iCount == 0)
        return NULL;

    float* pkFloats = NiAlloc(float, iCount);
    IParamBlock2* pkParams = pkParamBlock;

    for (int i = 0; i < iCount; i++)
    {
        retrieved &= pkParams->GetValue(kID, 0, pkFloats[i], 
            valid_interval, i);
    }

    NiFloatsExtraDataPtr spFloatsAttrib;
    if (retrieved && iCount > 0)
    {
        TSTR value_name = kParamDef.int_name;
        if (!IsValidName(value_name))
            value_name = pkParamBlock->GetLocalName(kID);
        NIASSERT(IsValidName(value_name));

        spFloatsAttrib = NiNew NiFloatsExtraData(iCount, pkFloats);
        spFloatsAttrib->SetName(value_name.data());
        pkObj->AddExtraData(spFloatsAttrib);

        for (int i = 0; i < iCount; i++)
        {
            Control * pkControl = pkParamBlock->GetController(kID, i);
            if (pkControl)
            {
                NiMAXAnimationConverter anim(m_kAnimStart, m_kAnimEnd);
                
                // convert a float animation
                NiInterpolatorPtr spInterp = 
                    anim.ConvertFloatAnim(pkControl);

                if (!spInterp)
                    return W3D_STAT_OK;
            
                NiKeyBasedInterpolator* pkKeyInterp = NiDynamicCast(
                    NiKeyBasedInterpolator, spInterp);

                if (pkKeyInterp && pkKeyInterp->GetKeyCount(0) != 0)
                {
                    // Set the ExtraDataName instead of trying to preserve 
                    // the ExtraData pointer reference across multiple
                    // streams.
                    NiFloatsExtraDataController* pkController = 
                        NiNew NiFloatsExtraDataController(
                        spFloatsAttrib->GetName());

                    pkKeyInterp->FillDerivedValues(0);
                    pkController->SetFloatsExtraDataIndex(i);
                    pkController->SetInterpolator(spInterp);
                    pkController->ResetTimeExtrema();
                    pkController->SetTarget(pkObj);
    
                    NiMAXAnimationConverter::SetORT(pkControl, pkController);
    
                    if (NiMAXConverter::GetUseAppTime())
                       pkController->SetAnimType(NiTimeController::APP_TIME);
                    else
                       pkController->SetAnimType(NiTimeController::APP_INIT);
   
                }
            }
        }
    }

    NiFree(pkFloats);
    return spFloatsAttrib;
}
//---------------------------------------------------------------------------
NiExtraData* NiMAXCustAttribConverter::ConvertColorTabAttrib(IParamBlock2* 
    pkParamBlock, int iIndex, NiObjectNET* pkObj, int&)
{
    ParamBlockDesc2 *pkParamDesc = pkParamBlock->GetDesc();
    ParamID kColorID = pkParamDesc->IndextoID(iIndex);
    if (!pkParamDesc)
        return NULL;

    ParamDef& kParamDef = pkParamDesc->GetParamDef(kColorID);

    Interval valid_interval;
    BOOL retrieved = true;
    
    int iCount = pkParamBlock->Count(kColorID);

    if (iCount == 0)
        return NULL;

    Color* pkColors = NiExternalNew Color[iCount];
    IParamBlock2* pkParams = pkParamBlock;

    for (int i = 0; i < iCount; i++)
    {
        retrieved &= pkParams->GetValue(kColorID, 0, pkColors[i], 
            valid_interval, i);
    }
    // Check if there is a valid alpha table
    BOOL bHasAlpha = false;
    ParamID kAlphaID = pkParamDesc->IndextoID(iIndex + 1);
    float* pfAlphaValue = NULL;

    if (kAlphaID != -1)
    {
        // check if there is a float table next - the alpha values table
        // and if that float table's name corresponds to the current 
        // table name + "Alpha"
        TCHAR* pcBaseName = kParamDef.int_name;
        ParamDef& kAlphaParamDef = pkParamDesc->GetParamDef(kAlphaID);
        TCHAR* pcAlphaName = kAlphaParamDef.int_name;
        NiString kNameCheck = pcBaseName;
        kNameCheck += "Alpha";
        if (kNameCheck == pcAlphaName)
        {
            pfAlphaValue = NiAlloc(float, iCount);
            int param_type = pkParamBlock->GetParameterType(kAlphaID);
            if (param_type == TYPE_FLOAT_TAB)
            {
                bHasAlpha = true;
                for (int i = 0; i < iCount; i++)
                {
                    bHasAlpha &= pkParamBlock->GetValue(kAlphaID, 
                        0, pfAlphaValue[i], valid_interval);
                }
            }
        }
    }
    NiFloatsExtraDataPtr spFloatsAttrib;
    if (retrieved && iCount > 0)
    {
        TSTR value_name = kParamDef.int_name;
        if (!IsValidName(value_name))
            value_name = pkParamBlock->GetLocalName(kColorID);

        NIASSERT(IsValidName(value_name));
        unsigned int uiNumComponents = 3;
        if (bHasAlpha)
            uiNumComponents = 4;

        float* pkFloats = NiAlloc(float, iCount * uiNumComponents);
        
        // Declare a float array - four floats for each color
        for (int i = 0; i < iCount; i++)
        {
            // Copy the color values to the float array
            pkFloats[i * uiNumComponents] = pkColors[i].r;
            pkFloats[i * uiNumComponents + 1] = pkColors[i].g;
            pkFloats[i * uiNumComponents + 2] = pkColors[i].b;
            if (bHasAlpha)
            {
                pkFloats[i * uiNumComponents + 3] = pfAlphaValue[i];
            }
        }
    
        spFloatsAttrib = NiNew NiFloatsExtraData(iCount * uiNumComponents, 
            pkFloats);
        spFloatsAttrib->SetName(value_name.data());
        pkObj->AddExtraData(spFloatsAttrib);

        // Convert any animated values
        for (int i = 0; i < iCount; i++)
        {
            Control * pkColorControl = 
                pkParamBlock->GetController(kColorID, i);
            Control * pkAlphaControl = NULL;
            if (bHasAlpha)
                pkAlphaControl = pkParamBlock->GetController(kAlphaID, i);

            if (pkColorControl || pkAlphaControl)
            {
                CreateColorArrayCustAttribAnimator(pkColorControl, 
                    pkAlphaControl, pkObj, spFloatsAttrib,
                    i * uiNumComponents);
            }
        }
    }

    if (pfAlphaValue)
        NiFree(pfAlphaValue);
    NiExternalDelete [] pkColors;
    return spFloatsAttrib;
}
//---------------------------------------------------------------------------
NiExtraData* NiMAXCustAttribConverter::ConvertIntTabAttrib(IParamBlock2* 
    pkParamBlock, int iIndex, NiObjectNET* pkObj, int&)
{
    ParamBlockDesc2 *pkParamDesc = pkParamBlock->GetDesc();
    ParamID kID = pkParamDesc->IndextoID(iIndex);
    if (!pkParamDesc)
        return NULL;

    ParamDef& kParamDef = pkParamDesc->GetParamDef(kID);

    Interval valid_interval;
    BOOL retrieved = true;
    
    int iCount = pkParamBlock->Count(kID);

    if (iCount == 0)
        return NULL;

    int* piInts = NiAlloc(int, iCount);
    IParamBlock2* pkParams = pkParamBlock;

    for (int i = 0; i < iCount; i++)
    {
        retrieved &= pkParams->GetValue(kID, 0, piInts[i], valid_interval, i);
    }

    NiIntegersExtraDataPtr spIntsAttrib;
    if (retrieved && iCount > 0)
    {
        TSTR value_name = kParamDef.int_name;
        if (!IsValidName(value_name))
            value_name = pkParamBlock->GetLocalName(kID);
        NIASSERT(IsValidName(value_name));

        spIntsAttrib = NiNew NiIntegersExtraData(iCount, piInts);
        spIntsAttrib->SetName(value_name.data());
        pkObj->AddExtraData(spIntsAttrib);
    }

    NiFree(piInts);
    return spIntsAttrib;
}
//---------------------------------------------------------------------------
NiExtraData* NiMAXCustAttribConverter::ConvertStringTabAttrib(IParamBlock2* 
    pkParamBlock, int iIndex, NiObjectNET* pkObj, int&)
{
    ParamBlockDesc2 *pkParamDesc = pkParamBlock->GetDesc();
    ParamID kID = pkParamDesc->IndextoID(iIndex);
    if (!pkParamDesc)
        return NULL;

    ParamDef& kParamDef = pkParamDesc->GetParamDef(kID);

    Interval valid_interval;
    BOOL retrieved = true;
    
    int iCount = pkParamBlock->Count(kID);

    if (iCount == 0)
        return NULL;

    TCHAR** pkStrings = NiAlloc(TCHAR*, iCount);
    IParamBlock2* pkParams = pkParamBlock;

    for (int i = 0; i < iCount; i++)
    {
        retrieved &= pkParams->GetValue(kID, 0, pkStrings[i], 
            valid_interval, i);
    }

    NiStringsExtraDataPtr spStringsAttrib;
    if (retrieved && iCount > 0)
    {
        TSTR value_name = kParamDef.int_name;
        if (!IsValidName(value_name))
            value_name = pkParamBlock->GetLocalName(kID);
        NIASSERT(IsValidName(value_name));

        spStringsAttrib = NiNew NiStringsExtraData(iCount, 
            const_cast<const char**>(pkStrings));
        spStringsAttrib->SetName(value_name.data());
        pkObj->AddExtraData(spStringsAttrib);

    }

    NiDelete [] pkStrings;
    return spStringsAttrib;
}
//---------------------------------------------------------------------------
NiExtraData* NiMAXCustAttribConverter::ConvertTexMapAttrib(IParamBlock2* 
    pkParamBlock, int iIndex, NiObjectNET* pkObj, 
    NiTexturingProperty*& pkTexProp, NiMAXUV* pkNiMAXUV, int&)
{
    ParamBlockDesc2 *pkParamDesc = pkParamBlock->GetDesc();
    ParamID kTexmapID = pkParamDesc->IndextoID(iIndex);
    ParamID kSlotID = pkParamDesc->IndextoID(iIndex + 1);
    int num_params = pkParamBlock->NumParams();
 
    if (!pkParamDesc)
        return NULL;
 
    ParamDef& kTexmapParamDef = pkParamDesc->GetParamDef(kTexmapID);
    ParamDef& kTexSlotParamDef = pkParamDesc->GetParamDef(kSlotID);

    Texmap* Texmap_value;
    int index_value = 0;
    Interval valid_interval;
    BOOL retrievedTexmap = false;
    BOOL retrievedTexSlot = false;
    TSTR value_name;
    int iIndexTexSlot = iIndex + 1;
    retrievedTexmap = pkParamBlock->GetValue(kTexmapID, 0, Texmap_value, 
        valid_interval);

    if (retrievedTexmap)
    {
        value_name = kTexmapParamDef.int_name;
        if (!IsValidName(value_name))
            value_name = pkParamBlock->GetLocalName(kTexmapID);
        NIASSERT(IsValidName(value_name));
    }
    
    if (iIndexTexSlot < num_params)
    {
        int param_type = 
            pkParamBlock->GetParameterType((ParamID)iIndexTexSlot);
        if (param_type == TYPE_INT)
        {
            TCHAR* value_index_name = kTexSlotParamDef.int_name;
            if (!IsValidName(value_index_name))
                value_name = pkParamBlock->GetLocalName(kSlotID);
            NIASSERT(IsValidName(value_index_name));

            NiString strTexmapName = value_name;
            strTexmapName+= "Index";
            if (strTexmapName == value_index_name)
            {
                retrievedTexSlot = pkParamBlock->GetValue(kSlotID, 0, 
                    index_value, valid_interval);
            }
        }
    }
    
    if (retrievedTexmap)
    {
        NiTexturingProperty::Map* pkMap = NULL;
        NiFlipController* pkFlipper = NULL;
        NiTObjectArray<NiTextureTransformControllerPtr> kUVControllers;
        NiTexture::FormatPrefs kPrefs;
        unsigned int uiFlags;
        unsigned int uiWidth;
        unsigned int uiHeight;
 
        NiMAXTextureConverter kTexConvert(m_kAnimStart, m_kAnimEnd);
        kTexConvert.Convert(Texmap_value, pkMap,
            NiTexturingProperty::MAP_CLASS_SHADER, pkFlipper,
            kPrefs, uiFlags, uiWidth, uiHeight);
 
        if (pkMap != NULL && Texmap_value != NULL && pkNiMAXUV != NULL)
        {
            pkNiMAXUV->AssignMapChannel(pkMap, pkFlipper, kUVControllers,
                Texmap_value, false);
        }
        
        if (pkMap && pkNiMAXUV) // may have been deleted by AssignMapChannel
        {
            unsigned int uiID = index_value;
 
            if (!pkTexProp)
            {
                pkTexProp = NiNew NiTexturingProperty;
                if (NiIsKindOf(NiAVObject, pkObj))
                {
                    ((NiAVObject*)pkObj)->AttachProperty(pkTexProp);
                }
            }

            NIASSERT(pkMap->GetClassID() == 
                NiTexturingProperty::MAP_CLASS_SHADER);
            NiTexturingProperty::ShaderMap* pkShaderMap = 
                (NiTexturingProperty::ShaderMap*)pkMap;
            pkShaderMap->SetID(uiID);

            if (index_value < 0)
                index_value = 10;

            pkTexProp->SetShaderMap(index_value, pkShaderMap);
            AssignFlipController(pkFlipper, pkTexProp, pkShaderMap);
            pkNiMAXUV->AssignTextureTransformControllers(kUVControllers, 
                pkTexProp, pkShaderMap);
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------
void NiMAXCustAttribConverter::CreateColorCustAttribAnimator(
    Control *pColorCont, Control *pAlphaCont, NiObjectNET* pkObj,
    NiColorExtraData* pkColorED)
{
    NiPosKey *pMaxPosKeys = NULL;
    unsigned int uiNumMaxPosKeys = 0;
    NiPosKey::KeyType eMaxPosType = NiPosKey::NOINTERP;
    unsigned char ucMaxPosSize=0;

    unsigned int uiNumAlphaKeys = 0;
    NiFloatKey *pAlphaKeys = NULL;
    NiAnimationKey::KeyType eAlphaType = NiAnimationKey::NOINTERP;
    unsigned char ucAlphaSize = 0;

    NiColorA kDefaultColor = pkColorED->GetValue();

    NiMAXAnimationConverter anim(m_kAnimStart, m_kAnimEnd);
    
    if (pColorCont)
    {
        NiInterpolatorPtr spInterp = anim.ConvertPoint3Anim(pColorCont);
        
        NiPoint3Interpolator* pkKeyInterp = NiDynamicCast(
            NiPoint3Interpolator, spInterp);
        
        if (!pkKeyInterp)
            return;
        
        pMaxPosKeys = (NiPosKey*) pkKeyInterp->GetAnim(uiNumMaxPosKeys,
            eMaxPosType, ucMaxPosSize);

        // transfer ownership of keys
        pkKeyInterp->SetKeys(NULL, 0, NiAnimationKey::NOINTERP);
    }

    if (pAlphaCont)
    {
        // convert a float animation
        NiInterpolatorPtr spInterp = anim.ConvertFloatAnim(pAlphaCont);
        
        NiFloatInterpolator* pkKeyInterp = NiDynamicCast(NiFloatInterpolator,
            spInterp);
        
        if (!pkKeyInterp)
            return;
        
        pAlphaKeys = (NiFloatKey*) pkKeyInterp->GetAnim(uiNumAlphaKeys,
            eAlphaType, ucAlphaSize);

        // transfer ownership of keys
        pkKeyInterp->SetKeys(NULL, 0, NiAnimationKey::NOINTERP);
    }

    if (uiNumMaxPosKeys == 0 && uiNumAlphaKeys == 0)
        return;

    // Now we merge keys together
    unsigned int uiNumKeys = 0;
    unsigned int uiMaximumNumKeys = uiNumMaxPosKeys + uiNumAlphaKeys;
    float* pkPossibleKeyTimes = NiAlloc(float, uiMaximumNumKeys);
    memset(pkPossibleKeyTimes, 0, uiMaximumNumKeys*sizeof(float));
    unsigned int uiIdx = 0;
    unsigned int uiAlphaIdx = 0;  

    for (unsigned int ui = 0; ui < uiNumMaxPosKeys; ui++)
    {
        float fTime = pMaxPosKeys->GetKeyAt(ui, ucMaxPosSize)->GetTime();
        float fAlphaTime = 0.0;
        for (unsigned int uiAlpha = uiAlphaIdx; uiAlpha < uiNumAlphaKeys && 
            fAlphaTime < fTime; uiAlpha++)
        {
            fAlphaTime = pAlphaKeys->GetKeyAt(uiAlpha, 
                ucAlphaSize)->GetTime();
            if (fAlphaTime < fTime )
            {
                bool bAdd = false;
                if (uiIdx != 0 && pkPossibleKeyTimes[uiIdx-1] < fAlphaTime)
                    bAdd = true;
                else if (uiIdx == 0)
                    bAdd = true;

                if (bAdd)
                {
                    pkPossibleKeyTimes[uiIdx] = fAlphaTime;
                    uiIdx++;
                }
                uiAlphaIdx++;
            }
            else if (fAlphaTime == fTime)
                uiAlphaIdx++;
        }
        pkPossibleKeyTimes[uiIdx] = fTime;
        uiIdx++;
    }

    for (unsigned int uiAlpha = uiAlphaIdx; uiAlpha < uiNumAlphaKeys; 
        uiAlpha++)
    {
        bool bAdd = false;
        float fAlphaTime = pAlphaKeys->GetKeyAt(uiAlpha, 
            ucAlphaSize)->GetTime();
        
        if (uiIdx != 0 && pkPossibleKeyTimes[uiIdx-1] < fAlphaTime)
            bAdd = true;
        else if (uiIdx == 0)
            bAdd = true;

        if (bAdd)
        {
            pkPossibleKeyTimes[uiIdx] = fAlphaTime;
            uiIdx++;
        }
        uiAlphaIdx++;
    }

    uiNumKeys = uiIdx;

    if (uiNumKeys != 0)
    {
        NiLinColorKey* pkColorKeys = NiNew NiLinColorKey[uiNumKeys];
        NiColorExtraDataController *pNiColorController;

        // Set the ExtraDataName instead of trying to preserve the
        //    ExtraData pointer reference across multiple streams.
        pNiColorController = NiNew NiColorExtraDataController(
                pkColorED->GetName());

        NIASSERT(pNiColorController);

        if (uiNumMaxPosKeys != 0)
        {
            NiPosKey::InsertFunction pInsert;
            pInsert = NiPosKey::GetInsertFunction(eMaxPosType);
            NIASSERT(pInsert);
            NiAnimationKey* pAnimKeys = pMaxPosKeys; 
            (*pInsert)(0.0, pAnimKeys, uiNumMaxPosKeys);
            (*pInsert)(pkPossibleKeyTimes[uiNumKeys-1], pAnimKeys, 
                uiNumMaxPosKeys);
            pMaxPosKeys = (NiPosKey*) pAnimKeys;
            if (uiNumMaxPosKeys > 1)
            {
                // precalulate values for the splines
                NiPosKey::FillDerivedValsFunction pDeriv;
    
                pDeriv = NiPosKey::GetFillDerivedFunction(eMaxPosType);
                NIASSERT(pDeriv);

                (*pDeriv)((NiAnimationKey*)pMaxPosKeys, uiNumMaxPosKeys,
                    ucMaxPosSize);
            }
            
        }

        if (uiNumAlphaKeys != 0)
        {
            NiFloatKey::InsertFunction pInsert;
            pInsert = NiFloatKey::GetInsertFunction(eAlphaType);
            NIASSERT(pInsert);
            NiAnimationKey* pAnimKeys = pAlphaKeys; 
            (*pInsert)(0.0, pAnimKeys, uiNumAlphaKeys);
            (*pInsert)(pkPossibleKeyTimes[uiNumKeys-1], pAnimKeys, 
                uiNumAlphaKeys);
            pAlphaKeys = (NiFloatKey*) pAnimKeys;
            if (uiNumAlphaKeys > 1)
            {
                NiFloatKey::FillDerivedValsFunction pDerived;
    
                pDerived = NiFloatKey::GetFillDerivedFunction(eAlphaType);
                NIASSERT(pDerived);

                (*pDerived)((NiAnimationKey*)pAlphaKeys, uiNumAlphaKeys,
                     ucAlphaSize);

            }
        }

        for (unsigned int uiTimeIdx = 0; uiTimeIdx < uiNumKeys; uiTimeIdx++)
        {
            float fTime = pkPossibleKeyTimes[uiTimeIdx];
            NiPoint3 kPt(kDefaultColor.r, kDefaultColor.g, kDefaultColor.b);
            float fAlpha = kDefaultColor.a;
            
            unsigned int uiLastAlpha = 0;
            unsigned int uiLastPos = 0;
            if (uiNumMaxPosKeys != 0)
            {
                kPt = NiPosKey::GenInterp(fTime, pMaxPosKeys, eMaxPosType, 
                    uiNumMaxPosKeys, uiLastPos, ucMaxPosSize);
            }

            if (uiNumAlphaKeys != 0)
            {
                fAlpha = NiFloatKey::GenInterp(fTime, pAlphaKeys, eAlphaType, 
                    uiNumAlphaKeys, uiLastAlpha, ucAlphaSize);
            }

            NiColorA kColor(kPt.x, kPt.y, kPt.z, fAlpha);
            pkColorKeys[uiTimeIdx].SetTime(fTime);
            pkColorKeys[uiTimeIdx].SetColor(kColor);
        }

        NiColorInterpolator* pkInterp = NiNew NiColorInterpolator;
        pkInterp->SetKeys(pkColorKeys, uiNumKeys,  NiAnimationKey::LINKEY);
        pNiColorController->SetInterpolator(pkInterp);
        pNiColorController->ResetTimeExtrema();
        pNiColorController->SetTarget(pkObj);

        if (pColorCont)
            NiMAXAnimationConverter::SetORT(pColorCont, pNiColorController);
        else if (pAlphaCont)
            NiMAXAnimationConverter::SetORT(pAlphaCont, pNiColorController);
        
        if (NiMAXConverter::GetUseAppTime())
            pNiColorController->SetAnimType(NiTimeController::APP_TIME);
        else
            pNiColorController->SetAnimType(NiTimeController::APP_INIT);

        if (uiNumMaxPosKeys != 0)
        {
            NiAnimationKey::DeleteFunction pfnDeleteFn = 
                NiPosKey::GetDeleteFunction(eMaxPosType);
            pfnDeleteFn(pMaxPosKeys);
        }

        if (uiNumAlphaKeys != 0)
        {
            NiAnimationKey::DeleteFunction pfnDeleteFn = 
                NiFloatKey::GetDeleteFunction(eAlphaType);
            pfnDeleteFn(pAlphaKeys);
        }

        NiFree(pkPossibleKeyTimes);
    }
}
//---------------------------------------------------------------------------
void NiMAXCustAttribConverter::CreateColorArrayCustAttribAnimator(
    Control *pColorCont, Control *pAlphaCont, NiObjectNET* pkObj,
    NiFloatsExtraData* pkColorED, unsigned int uiStartIdx)
{   
    NiMAXAnimationConverter anim(m_kAnimStart, m_kAnimEnd);

    if (pColorCont)
    {
        NiInterpolatorPtr spInterp = anim.ConvertPoint3Anim(pColorCont);
        
        NiPoint3Interpolator* pkKeyInterp = NiDynamicCast(
            NiPoint3Interpolator, spInterp);
        
        if (pkKeyInterp)
        {
            NiFloatsExtraDataPoint3Controller* pkColorController = 
                NiNew NiFloatsExtraDataPoint3Controller(pkColorED->GetName(),
                uiStartIdx);
            pkColorController->SetInterpolator(pkKeyInterp);

            NiMAXAnimationConverter::SetORT(pColorCont, pkColorController);
            
            if (NiMAXConverter::GetUseAppTime())
                pkColorController->SetAnimType(NiTimeController::APP_TIME);
            else
                pkColorController->SetAnimType(NiTimeController::APP_INIT);

            pkColorController->ResetTimeExtrema();
            pkColorController->SetTarget(pkObj);
        }
    }

    if (pAlphaCont)
    {
        // convert a float animation
        NiInterpolatorPtr spInterp = anim.ConvertFloatAnim(pAlphaCont);
        
        NiFloatInterpolator* pkKeyInterp = NiDynamicCast(NiFloatInterpolator,
            spInterp);
        
        if (pkKeyInterp)
        {
            NiFloatsExtraDataController* pkAlphaController = 
                NiNew NiFloatsExtraDataController(pkColorED->GetName(),
                uiStartIdx + 3);
            pkAlphaController->SetInterpolator(pkKeyInterp);

            NiMAXAnimationConverter::SetORT(pAlphaCont, pkAlphaController);
            
            if (NiMAXConverter::GetUseAppTime())
                pkAlphaController->SetAnimType(NiTimeController::APP_TIME);
            else
                pkAlphaController->SetAnimType(NiTimeController::APP_INIT);

            pkAlphaController->ResetTimeExtrema();
            pkAlphaController->SetTarget(pkObj);
        }
    }
}
//---------------------------------------------------------------------------
bool NiMAXCustAttribConverter::IsStandardCA(const char* pcString)
{
    for (unsigned int ui = 0; ui < NUM_STANDARD_CAS; ui++)
    {
        if (NiStricmp(pcString, gs_acStdCANames[ui]) == 0)
            return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiMAXCustAttribConverter::IsMaxCA(const char* pcString)
{
    for (unsigned int ui = 0; ui < NUM_MAX_CAS; ui++)
    {
        if (NiStricmp(pcString, gs_acMaxCANames[ui]) == 0)
            return true;
    }

    return false;
}
//---------------------------------------------------------------------------
