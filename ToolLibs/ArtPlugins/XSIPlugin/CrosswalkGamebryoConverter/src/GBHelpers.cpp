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

#include "CrosswalkGamebryoHelpers.h"
#include "CrosswalkGamebryoImpl.h"
#include "CrosswalkGamebryoContext.h"

#include "Model.h"
#include "Visibility.h"
#include "CustomPSet.h"

#include "NiNode.h"
#include "NiColor.h"
#include "NiString.h"
#include "NiZBufferProperty.h"

namespace
{
    //---------------------------------------------------------------------------
    // Parse a Z-buffer function name.
    //---------------------------------------------------------------------------
    NiZBufferProperty::TestFunction ParseZBufferFunction(const NiString& in_Name)
    {
        struct ZBufferFunction
        {
            const char* name;
            NiZBufferProperty::TestFunction func;
        };

        static const ZBufferFunction functions[] =
        {
            {XSI_GAMEBRYO_ZBUFFER_ALWAYS_VALUE_NAME,        NiZBufferProperty::TEST_ALWAYS      },
            {XSI_GAMEBRYO_ZBUFFER_LESS_VALUE_NAME,          NiZBufferProperty::TEST_LESS        },
            {XSI_GAMEBRYO_ZBUFFER_EQUAL_VALUE_NAME,         NiZBufferProperty::TEST_EQUAL       },
            {XSI_GAMEBRYO_ZBUFFER_LESS_EQUAL_VALUE_NAME,    NiZBufferProperty::TEST_LESSEQUAL   },
            {XSI_GAMEBRYO_ZBUFFER_GREATER_VALUE_NAME,       NiZBufferProperty::TEST_GREATER     },
            {XSI_GAMEBRYO_ZBUFFER_NOT_EQUAL_VALUE_NAME,     NiZBufferProperty::TEST_NOTEQUAL    },
            {XSI_GAMEBRYO_ZBUFFER_GREATER_EQUAL_VALUE_NAME, NiZBufferProperty::TEST_GREATEREQUAL},
            {XSI_GAMEBRYO_ZBUFFER_NEVER_VALUE_NAME,         NiZBufferProperty::TEST_NEVER       },
            {0, NiZBufferProperty::TEST_MAX_MODES }
        };

        for (int i = 0; functions[i].name; ++i)
            if (in_Name.EqualsNoCase(functions[i].name))
                return functions[i].func;

        // Note: this is the default as specified in NiZBufferProperty documentation.
        return NiZBufferProperty::TEST_LESSEQUAL;
    }

    //---------------------------------------------------------------------------
}

namespace epg
{
    //---------------------------------------------------------------------------
    // Convert the color, support only alpha of 0 or 1.
    //---------------------------------------------------------------------------
    bool ConvertColor(
        Context & io_Context,
        const CSIBCColorf & in_XSIColor,
        NiColor & out_GBColor,
        const char * in_ObjName,
        const char * in_PropName)
    {
        // Transparent is returned as non-existent.
        if (in_XSIColor.m_fA == 0.0f)
            return false;

        // 
        if (in_XSIColor.m_fA != 1.0f)
        {
            io_Context.Logf(
                LOG_WARNING,
                "Object \"%s\" uses unsupported alpha transparency in \"%s\"",
                in_ObjName  ? in_ObjName  : "unknown object",
                in_PropName ? in_PropName : "unknown property");
        }

        out_GBColor.r = in_XSIColor.m_fR;
        out_GBColor.g = in_XSIColor.m_fG;
        out_GBColor.b = in_XSIColor.m_fB;

        return true;
    }

    //---------------------------------------------------------------------------
    // Transfer the XSI model name to the node.
    //---------------------------------------------------------------------------
    void CopyNodeName(CSIBCString & in_Name, NiAVObject& io_Object, bool in_FixABVTag)
    {
        // Unfortunately, the CreateABV pipeline plugin expect the node name
        // to begin with "NDLCD ", but XSI does not support object name with
        // spaces in them... it automatically replaces spaces with underscores.
        // So... revert that here for those nodes.
        static const char incorrectPrefix[] = "NDLCD_";
        static const char correctedPrefix[] = "NDLCD ";
        if (in_FixABVTag && 
            0 == strncmp(in_Name.GetText(), incorrectPrefix, strlen(incorrectPrefix)))
        {
            NiString correctedName(correctedPrefix);
            correctedName += in_Name.GetText() + strlen(incorrectPrefix);
            io_Object.SetName(NiFixedString(correctedName));
        }
        else
        {
            io_Object.SetName(in_Name.GetText());
        }
    }

    //---------------------------------------------------------------------------
    // Convert the XSI visibility settings to the Gamebryo node.
    //---------------------------------------------------------------------------
    void CopyVisibility(CSLModel & in_Model, NiAVObject& io_Object)
    {
        CSLVisibility* visibility = in_Model.Visibility();
        if(visibility)
        {
            if (visibility->GetVisibility() == 0)
            {
                io_Object.SetAppCulled(true);
            }
        }
    }

    //---------------------------------------------------------------------------
    // Convert 
    //---------------------------------------------------------------------------
    void ConvertZBufferProperties(
        Context & io_Context,
        CSLModel & in_Model,
        NiAVObject& io_Object)
    {
        CSLCustomPSet* pZProps = 
            FindCustomProperties(in_Model, XSI_GAMEBRYO_ZBUFFER_PROPERTY_NAME);
        if (!pZProps)
            return;

        const bool zTest  = 
            GetBooleanParameter(*pZProps, XSI_GAMEBRYO_ZBUFFER_TEST_PARAM_NAME,  true);
        const bool zWrite = 
            GetBooleanParameter(*pZProps, XSI_GAMEBRYO_ZBUFFER_WRITE_PARAM_NAME, true);
        NiZBufferProperty::TestFunction zFunc = ParseZBufferFunction(
            GetStringParameter(*pZProps, XSI_GAMEBRYO_ZBUFFER_FUNCTION_PARAM_NAME));

        NiZBufferProperty* pNiZProp = NiNew NiZBufferProperty;
        pNiZProp->SetZBufferTest(zTest);
        pNiZProp->SetZBufferWrite(zWrite);
        pNiZProp->SetTestFunction(zFunc);

        io_Object.AttachProperty(pNiZProp);
    }

    //---------------------------------------------------------------------------
    // Convert common properties.
    //---------------------------------------------------------------------------
    bool ConvertCommonProperties(
        Context & io_Context,
        CSLModel & in_Model,
        NiAVObject& io_Object,
        CSIBCString & in_Name)
    {
        CopyNodeName(in_Name, io_Object);
        ConvertZBufferProperties(io_Context, in_Model, io_Object);

        if (!ConvertUnknownCustomProperties(io_Context, in_Model, io_Object))
            return false;

        // Note: animation clips must be converted before the model transform
        //       and the model itself because it can add new animation actions
        //       to the animation map that will be used and converted by
        //       the transform conversion and model conversion.
        if (!ConvertAnimationClips(io_Context, in_Model, io_Object))
            return false;

        // Note: ConvertTransform() can create intermediary nodes to support
        //       complex transforms and non-uniform scaling. It returns the
        //       top-node of the node created, if any. The actual object
        //       is always the bottom node of this mini-tree.
        return ConvertTransform(io_Context, in_Model, io_Object);
    }

    //---------------------------------------------------------------------------
    // Convert common properties and potemtially adds a node to handle
    // non-recursive visibility. Dummy node creation can also be forced
    // to handle other situations outside of the scope of this function's
    // knowledge.
    //---------------------------------------------------------------------------
    bool ConvertCommonProperties(
        Context & io_Context,
        CSLModel & in_Model,
        NiAVObject& io_Object,
        bool in_CreateDummyNode)
    {
        // Note: invisibility is per-node in XSI while it propagates to children
        //       in Gamebryo. So to support the XSI model, we insert a dummy
        //       parent node to connect children nodes and the invisibility
        //       is set on the Gamebryo node that request it without affecting
        //       children.
        if (in_CreateDummyNode || (CanBeVisible(in_Model) && !IsVisible(in_Model)))
        {
            NiNodePtr pkNode = NiNew NiNode;
            CSIBCString nodeName = in_Model.Name();
            nodeName.Concat("-DummyNode");
            if (!ConvertCommonProperties(io_Context, in_Model, *pkNode, nodeName))
                return false;
            pkNode->AttachChild(&io_Object);
            CopyNodeName(in_Model.Name(), io_Object);
            CopyVisibility(in_Model, io_Object);

            return true;
        }
        else
        {
            return ConvertCommonProperties(io_Context, in_Model, io_Object, in_Model.Name());
        }
    }

    //---------------------------------------------------------------------------
}
