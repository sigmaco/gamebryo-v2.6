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

#include "CrosswalkGamebryoMeshHelpers.h"

#include "XSIShape.h"
#include "XSISubComponentAttributeList.h"

#include "NiMorphMeshModifier.h"
#include "NiCommonSemantics.h"

//---------------------------------------------------------------------------
// Define operator< for NiFiedString so it can be used in maps and sets.
//---------------------------------------------------------------------------
bool operator<(const NiFixedString& lhs, const NiFixedString& rhs)
{
    return lhs < NiString(rhs);
}

namespace epg
{
    //---------------------------------------------------------------------------
    // The standard Gamebryo semantic mapper.
    //---------------------------------------------------------------------------
    StandardSemanticMapper::StandardSemanticMapper(Mode in_Mode)
        : m_Mode(in_Mode)
    {
    }

    //---------------------------------------------------------------------------
    NiFixedString StandardSemanticMapper::TranslateSemantic(const char* in_XSISemantic)
    {
        NiFixedString semantic(in_XSISemantic);

        if (semantic == "POSITION")
        {
            if (STANDARD_MODE == m_Mode)
                return NiCommonSemantics::POSITION();
            else
                return NiCommonSemantics::POSITION_BP();
        }

        if (semantic == "NORMAL")
        {
            if (STANDARD_MODE == m_Mode)
                return NiCommonSemantics::NORMAL();
            else
                return NiCommonSemantics::NORMAL_BP();
        }

        if (semantic == "BINORMAL")
        {
            if (STANDARD_MODE == m_Mode)
                return NiCommonSemantics::BINORMAL();
            else
                return NiCommonSemantics::BINORMAL_BP();
        }

        if (semantic == "TANGENT")
        {
            if (STANDARD_MODE == m_Mode)
                return NiCommonSemantics::TANGENT();
            else
                return NiCommonSemantics::TANGENT_BP();
        }

        if (semantic == "TEXCOORD")
            return NiCommonSemantics::TEXCOORD();

        if (semantic == "COLOR")
            return NiCommonSemantics::COLOR();

        NIASSERT("Unknown data stream semantic");
        return "";
    }

    //---------------------------------------------------------------------------
    // Semantic mapper used for shape animation data.
    //---------------------------------------------------------------------------
    ShapeAnimSemanticMapper::ShapeAnimSemanticMapper(Mode in_Mode)
        : StandardSemanticMapper(in_Mode)
    {
    }

    //---------------------------------------------------------------------------
    NiFixedString ShapeAnimSemanticMapper::TranslateSemantic(const char* in_XSISemantic)
    {
        NiFixedString standardSemantic =
            StandardSemanticMapper::TranslateSemantic(in_XSISemantic);
        NiFixedString morphSemantic =
            NiMorphMeshModifier::MorphElementSemantic(standardSemantic);
        if (standardSemantic.GetLength() && morphSemantic.GetLength())
        {
            m_StandardSemantics.insert(standardSemantic);
            m_MorphSemantics.insert(morphSemantic);
        }
        return morphSemantic;
    }
    
    //---------------------------------------------------------------------------
    const ShapeAnimSemanticMapper::Semantics&
    ShapeAnimSemanticMapper::GetStandardSemanticsFound() const
    {
        return m_StandardSemantics;
    }

    //---------------------------------------------------------------------------
    const ShapeAnimSemanticMapper::Semantics&
    ShapeAnimSemanticMapper::GetMorphSemanticsFound() const
    {
        return m_MorphSemantics;
    }

    //---------------------------------------------------------------------------
    // Semantic mapper used to convert the original shape stream that
    // are also present in the shape anim.
    //---------------------------------------------------------------------------
    ValidatedSemanticMapper::ValidatedSemanticMapper(
        Mode in_Mode,
        const ShapeAnimSemanticMapper& in_ValidSemanticMapper)
        : ShapeAnimSemanticMapper(in_Mode)
        , m_ValidSemantics(in_ValidSemanticMapper.GetMorphSemanticsFound())
    {
    }

    //---------------------------------------------------------------------------
    ValidatedSemanticMapper::ValidatedSemanticMapper(
        Mode in_Mode,
        const Semantics& in_ValidSemantics)
        : ShapeAnimSemanticMapper(in_Mode)
        , m_ValidSemantics(in_ValidSemantics)
    {
    }

    //---------------------------------------------------------------------------
    NiFixedString ValidatedSemanticMapper::TranslateSemantic(const char* in_XSISemantic)
    {
        NiFixedString semantic = ShapeAnimSemanticMapper::TranslateSemantic(in_XSISemantic);
        if (m_ValidSemantics.count(semantic) > 0)
            return semantic;
        else
            return "";
    }

    //---------------------------------------------------------------------------
    // Constructor for a descriptor for the attributes of a shape.
    //---------------------------------------------------------------------------
    ShapeAttributeMap::ShapeAttributeMap(CSLXSIShape& in_Shape, SemanticMapper& in_SemanticMapper)
    {
        // Note: vertex position attribute list is kept separate from others... go figure...
        AddAttributeList(in_Shape.GetVertexPositionList(), in_SemanticMapper);

        CSIBCArray<CSLXSISubComponentAttributeList*>* pkAttrListArray = in_Shape.AttributeLists();
        if (!pkAttrListArray)
            return;

        const SI_Int count = in_Shape.GetAttributeListCount();
        for (SI_Int i = 0; i < count; ++i)
        {
            CSLXSISubComponentAttributeList* pkAttrList = (*pkAttrListArray)[i];
            AddAttributeList(pkAttrList, in_SemanticMapper);
        }
    }

    //---------------------------------------------------------------------------
    // Add the specified attribute list to the maps.
    //---------------------------------------------------------------------------
    void ShapeAttributeMap::AddAttributeList(
        CSLXSISubComponentAttributeList* in_pkAttrList,
        SemanticMapper& in_SemanticMapper)
    {
        if (!in_pkAttrList)
            return;

        // Note: as per the SemanticMapper API, an empty semantic
        //       means the attribute should not be converted.
        NiFixedString semantic = in_SemanticMapper.TranslateSemantic(
                                     in_pkAttrList->GetSemantic());
        if (!semantic.GetLength())
            return;

        const char* pAttrListName = in_pkAttrList->GetName();
        NIASSERT(pAttrListName && pAttrListName[0]);
        if (!pAttrListName)
            return;

        NameMap[pAttrListName] = in_pkAttrList;
        SemanticMap[in_pkAttrList] = semantic;
    }

    //---------------------------------------------------------------------------
}
