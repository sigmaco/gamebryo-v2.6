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

#include "CrosswalkGamebryoContext.h"
#include "CrosswalkGamebryoImpl.h"
#include "CrosswalkGamebryoHelpers.h"
#include "CrosswalkGamebryoNodeMap.h"

#include "Template.h"
#include "CustomPSet.h"
#include "VariantParameter.h"

#include "NiAVObject.h"
#include "NiBooleanExtraData.h"
#include "NiIntegerExtraData.h"
#include "NiIntegersExtraData.h"
#include "NiFloatExtraData.h"
#include "NiFloatsExtraData.h"
#include "NiStringExtraData.h"
#include "NiString.h"

namespace
{
    //---------------------------------------------------------------------------
    // Convert array of values to the proper type for NiExtraData.
    //---------------------------------------------------------------------------
    template <class T>
    NiIntegersExtraData* ConvertIntegersCustomParameter(SI_Int in_Count, const T* in_Values)
    {
        std::vector<int> values;
        values.reserve(in_Count);
        for (SI_Int i = 0; i < in_Count; ++i)
            values.push_back((int) in_Values[i]);
        return NiNew NiIntegersExtraData(in_Count, &values[0]);
    }

    //---------------------------------------------------------------------------
    NiFloatsExtraData* ConvertFloatsCustomParameter(SI_Int in_Count, const double* in_Values)
    {
        std::vector<float> values;
        values.reserve(in_Count);
        for (SI_Int i = 0; i < in_Count; ++i)
            values.push_back((float) in_Values[i]);
        return NiNew NiFloatsExtraData(in_Count, &values[0]);
    }

    //---------------------------------------------------------------------------
}

namespace epg
{
    //---------------------------------------------------------------------------
    // Convert all custom properties to their proper equivalent.
    //---------------------------------------------------------------------------
    bool ConvertUnknownCustomProperties(
        Context& io_Context,
        CSLTemplate& in_Tmpl,
        NiAVObject& io_Object)
    {
        const SI_Int propSetCount = in_Tmpl.GetCustomPSetCount();
        CSLCustomPSet** propSetArray = in_Tmpl.GetCustomPSetList();
        for (SI_Int i = 0 ; i < propSetCount; ++i)
        {
            CSLCustomPSet* propSet = propSetArray[i];
            if (!propSet)
                continue;

            if (IsKnownCustomProperty(propSet->Name().GetText()))
                continue;

            if (!ConvertUnknownCustomPropertySet(io_Context, *propSet, io_Object))
                return false;
        }

        return true;
    }

    //---------------------------------------------------------------------------
    // Verify if a custom property name is a known cutom property used in Gamebryo.
    //---------------------------------------------------------------------------
    bool IsKnownCustomProperty(const char* in_Name)
    {
        if (!in_Name || !in_Name[0])
            return false;

        static const char* const knownCustomProperties[] =
        {
            XSI_GAMEBRYO_LOD_PROPERTY_NAME,
            XSI_GAMEBRYO_LIGHTNING_PROPERTY_NAME,
            XSI_GAMEBRYO_EXPORT_OPTIONS_NAME,
            XSI_GAMEBRYO_CROSSWALK_EXPORT_OPTIONS_NAME,
            XSI_GAMEBRYO_ZBUFFER_PROPERTY_NAME,
            XSI_GAMEBRYO_SHADOW_PROPERTY_NAME,
            0
        };

        const NiString name(in_Name);
        for (int i = 0; knownCustomProperties[i]; ++i)
            if (name.EqualsNoCase(knownCustomProperties[i]))
                return true;

        return false;
    }

    //---------------------------------------------------------------------------
    // Convert an XSI custom property set to NiExtraData on the specified object.
    //---------------------------------------------------------------------------
    bool ConvertUnknownCustomPropertySet(
        Context& io_Context,
        CSLCustomPSet& in_PSet,
        NiAVObject& io_Object)
    {
        const SI_Int count = in_PSet.GetParameterCount();
        if (!count)
            return true;

        CSLVariantParameter** pParamArray = in_PSet.GetParameterList();
        if (!pParamArray)
            return true;

        for (SI_Int i = 0; i < count; ++i)
        {
            CSLVariantParameter* pParam = pParamArray[i];
            if (!pParam)
                continue;

            if (!ConvertCustomParameter(io_Context, *pParam, io_Object))
                return false;
        }

        return true;
    }

    //---------------------------------------------------------------------------
    // Convert a custom property parameter to NiExtraData
    //---------------------------------------------------------------------------
    bool ConvertCustomParameter(
        Context& io_Context,
        CSLVariantParameter& in_Param,
        NiAVObject& io_Object)
    {
        SI_TinyVariant* pVariant = in_Param.GetValue();
        if (!pVariant)
            return true;

        const char* pName = in_Param.GetName();
        if (io_Object.GetExtraData(pName))
        {
            return io_Context.Logf(
                LOG_WARNING,
                "Extra data \"%s\" collides with other extra data"
                " with the same name on \"%s\". The second one is ignored.",
                pName,
                (const char*)io_Object.GetName());
        }

        NiExtraDataPtr pData;

        switch (pVariant->variantType)
        {
            case SI_VT_BOOL:
                pData = NiNew NiBooleanExtraData(pVariant->boolVal != 0);
                break;
            case SI_VT_BYTE:
                pData = NiNew NiIntegerExtraData(pVariant->bVal);
                break;
            case SI_VT_UBYTE:
                pData = NiNew NiIntegerExtraData(pVariant->ubVal);
                break;
            case SI_VT_SHORT:
                pData = NiNew NiIntegerExtraData(pVariant->sVal);
                break;
            case SI_VT_USHORT:
                pData = NiNew NiIntegerExtraData(pVariant->usVal);
                break;
            case SI_VT_INT:
                pData = NiNew NiIntegerExtraData(pVariant->nVal);
                break;
            case SI_VT_UINT:
                pData = NiNew NiIntegerExtraData(pVariant->unVal);
                break;
            case SI_VT_LONG:
                pData = NiNew NiIntegerExtraData(pVariant->lVal);
                break;
            case SI_VT_ULONG:
                pData = NiNew NiIntegerExtraData(pVariant->ulVal);
                break;
            case SI_VT_FLOAT:
                pData = NiNew NiFloatExtraData(pVariant->fVal);
                break;
            case SI_VT_DOUBLE:
                pData = NiNew NiFloatExtraData((float) pVariant->dVal);
                break;
            case SI_VT_PBOOL:
                pData = ConvertIntegersCustomParameter(pVariant->numElems, pVariant->p_boolVal);
                break;
            case SI_VT_PBYTE:
                pData = ConvertIntegersCustomParameter(pVariant->numElems, pVariant->p_bVal);
                break;
            case SI_VT_PUBYTE:
                pData = ConvertIntegersCustomParameter(pVariant->numElems, pVariant->p_ubVal);
                break;
            case SI_VT_PSHORT:
                pData = ConvertIntegersCustomParameter(pVariant->numElems, pVariant->p_sVal);
                break;
            case SI_VT_PUSHORT:
                pData = ConvertIntegersCustomParameter(pVariant->numElems, pVariant->p_usVal);
                break;
            case SI_VT_PINT:
                pData = ConvertIntegersCustomParameter(pVariant->numElems, pVariant->p_nVal);
                break;
            case SI_VT_PUINT:
                pData = ConvertIntegersCustomParameter(pVariant->numElems, pVariant->p_unVal);
                break;
            case SI_VT_PLONG:
                pData = ConvertIntegersCustomParameter(pVariant->numElems, pVariant->p_lVal);
                break;
            case SI_VT_PULONG:
                pData = ConvertIntegersCustomParameter(pVariant->numElems, pVariant->p_ulVal);
                break;
            case SI_VT_PFLOAT:
                pData = NiNew NiFloatsExtraData(pVariant->numElems, pVariant->p_fVal);
                break;
            case SI_VT_PDOUBLE:
                pData = ConvertFloatsCustomParameter(pVariant->numElems, pVariant->p_dVal);
                break;
            case SI_VT_PCHAR:
            case SI_VT_PPCHAR:
                pData = NiNew NiStringExtraData(pVariant->p_cVal);
                break;
            default:
                return io_Context.Logf(
                    LOG_WARNING,
                    "Invalid extra data type found in \"%s\".",
                    (const char*)io_Object.GetName());
        }

        if (pData)
        {
            if (!io_Object.AddExtraData(pName, pData))
            {
                // Note: AddExtraData() doesn't specify if the data is deleted
                //       on error. Some code paths delete it, others don't, so
                //       we don't delete it to avoid double-deletion crashes.
                return io_Context.Logf(
                    LOG_WARNING,
                    "Could not add \"%s\" as extra data on \"%s\".",
                    pName,
                    (const char*)io_Object.GetName());
            }
        }

        return true;
    }


    //---------------------------------------------------------------------------
}
