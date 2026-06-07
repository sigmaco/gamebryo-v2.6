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

#ifndef XSI_GAMEBRYO_CONVERTER_MESH_HELPERS_H
#define XSI_GAMEBRYO_CONVERTER_MESH_HELPERS_H

#include "SLTypes.h"
#include "SL_Array.h"

#include "NiMainLibType.h"
#include "NiMeshLibType.h"
#include "NiDataStream.h"

#include <map>
#include <vector>
#include <string>
#include <set>

//---------------------------------------------------------------------------
// External references.
//---------------------------------------------------------------------------

class XSIEXPORT CSLXSIShape;
class XSIEXPORT CSLXSITriangleList;
class XSIEXPORT CSLXSISubComponentAttributeList;
class XSIEXPORT CSLModel;

class NIMESH_ENTRY NiMesh;
class NIMAIN_ENTRY NiFixedString;
class NIMAIN_ENTRY NiNode;

//---------------------------------------------------------------------------
/// Define operator< for NiFiedString so it can be used in maps and sets.
//---------------------------------------------------------------------------
bool operator<(const NiFixedString& lhs, const NiFixedString& rhs);

namespace epg
{
    class Context;

    //---------------------------------------------------------------------------
    /// XSI Crosswalk container for indices.
    //---------------------------------------------------------------------------
	typedef CSLArrayProxy<SI_Int, SI_Int, 1> CSLIntArray;

    //---------------------------------------------------------------------------
    /// Stream semantic mapping interface.
    //---------------------------------------------------------------------------
    class SemanticMapper
    {
    public:
        /** Map the given XSI semantic to a Gamebryo semantic.
            @param in_XSISemantic the XSI semantic.
            @return the Gamebryo semantic. Rteurn an empty string
                    to exclude that semantic, its corresponding stream
                    will not be converted.
        */
        virtual NiFixedString TranslateSemantic(const char* in_XSISemantic) = 0;
    };

    //---------------------------------------------------------------------------
    /// The standard Gamebryo semantic mapper.
    //---------------------------------------------------------------------------
    class StandardSemanticMapper : public SemanticMapper
    {
    public:
        /// The base mode of the standard semantics: normal or skinning.
        enum Mode
        {
            STANDARD_MODE,
            SKINNING_MODE
        };

        StandardSemanticMapper(Mode in_Mode);

        NiFixedString TranslateSemantic(const char* in_XSISemantic);

    protected:
        const Mode          m_Mode;

    private:
        StandardSemanticMapper(const StandardSemanticMapper&);
        void operator=(const StandardSemanticMapper&);
    };

    //---------------------------------------------------------------------------
    /// Semantic mapper used for shape animation data.
    /// Records the semantic that were actually translated.
    //---------------------------------------------------------------------------
    class ShapeAnimSemanticMapper : public StandardSemanticMapper
    {
    public:
        ShapeAnimSemanticMapper(Mode in_Mode);

        NiFixedString TranslateSemantic(const char* in_XSISemantic);

        typedef std::set<NiFixedString> Semantics;

        const Semantics& GetStandardSemanticsFound() const;
        const Semantics& GetMorphSemanticsFound() const;

    private:
        Semantics               m_StandardSemantics;
        Semantics               m_MorphSemantics;

        ShapeAnimSemanticMapper(const ShapeAnimSemanticMapper&);
        void operator=(const ShapeAnimSemanticMapper&);
    };

    //---------------------------------------------------------------------------
    /// Semantic mapper used for shape animation data that restricts
    /// the semantic that are translated to those that were found by
    /// another shape anim semantic mapper.
    ///
    /// This is used to translate the base shape into a morph stream
    /// but limiting the streams created to those used by the shape animation.
    //---------------------------------------------------------------------------
    class ValidatedSemanticMapper : public ShapeAnimSemanticMapper
    {
    public:
        ValidatedSemanticMapper(Mode in_Mode, const ShapeAnimSemanticMapper& in_ValidSemanticMap);
        ValidatedSemanticMapper(Mode in_Mode, const Semantics& in_ValidSemantics);

        NiFixedString TranslateSemantic(const char* in_XSISemantic);

    private:
        // Note: we take a copy to avoid lifetime of provided valid semantic container.
        const Semantics         m_ValidSemantics;

        ValidatedSemanticMapper(const ValidatedSemanticMapper&);
        void operator=(const ValidatedSemanticMapper&);
    };

    //---------------------------------------------------------------------------
    /// Descriptor for the attributes of a shape.
    ///
    /// Creates maps to simplify attribute matching:
    ///
    ///    - To go from the attribute name to the attribute list itself.
    ///    - To go from the attribute list to its Gamebryo semantic.
    ///
    /// The reason we do this is that the Crosswalk file format keeps information
    /// about a shape in indirect indices arrays that reference the actual data
    /// by its name. Thus we need to easily go from a name to an attribute and
    /// then to its semantics. Thus we use this class.
    //---------------------------------------------------------------------------
    class ShapeAttributeMap
    {
    public:
        /// Create the semantic map.
        ShapeAttributeMap(CSLXSIShape& in_Shape, SemanticMapper& in_SemanticMapper);

        /// Goes from attribute name to attribute itelf.
        std::map<NiFixedString,CSLXSISubComponentAttributeList*> NameMap;

        /// Goes from attribute to its Gamebryo semantic.
        /// Note: one semantic can have more than one attribute associated with it!
        std::map<CSLXSISubComponentAttributeList*,NiFixedString> SemanticMap;

    private:
        void AddAttributeList(
            CSLXSISubComponentAttributeList* in_pkAttrList,
            SemanticMapper& in_SemanticMapper);

        // Prevent copies.
        ShapeAttributeMap(const ShapeAttributeMap&);
        void operator=(const ShapeAttributeMap&);
    };

    ///---------------------------------------------------------------------------
    /// Add the given float buffer as a stream in a Gamebryo mesh.
    ///
    /// @param io_Context the conversion context.
    /// @param io_Mesh the mesh to attach the stream to.
    /// @param in_Semantic the Gamebryo semantic of the stream.
    /// @param in_StreamIndex the stream index to use.
    /// @param in_PerElementCount the number of float value for each stream element.
    ///
    /// @return the stream index used or allocated.
    ///
    /// @note  The stream index will be chosen if a negative value is given.
    //---------------------------------------------------------------------------
    int AddStreamToMesh(
        Context&  io_Context,
        NiMesh& io_Mesh,
        const NiFixedString& in_Semantic,
        int in_StreamIndex,
        int in_PerElementCount,
        const std::vector<float>& in_Values);

    ///---------------------------------------------------------------------------
    /// Add the given NiUInt32 buffer as a stream in a Gamebryo mesh.
    ///
    /// @param io_Context the conversion context.
    /// @param io_Mesh the mesh to attach the stream to.
    /// @param in_Semantic the Gamebryo semantic of the stream.
    /// @param in_StreamIndex the stream index to use.
    /// @param in_PerElementCount the number of float value for each stream element.
    ///
    /// @return the stream index used or allocated.
    ///
    /// @note  The stream index will be chosen if a negative value is given.
    //---------------------------------------------------------------------------
    int AddIndexStreamToMesh(
        Context&  io_Context,
        NiMesh& io_Mesh,
        const NiFixedString& in_Semantic,
        int in_StreamIndex,
        int in_PerElementCount,
        const std::vector<NiUInt32>& in_Values);

    //---------------------------------------------------------------------------
    /// Type-safe or'ing of stream access flags.
    //---------------------------------------------------------------------------
    NiDataStream::Access operator|(NiDataStream::Access lhs, NiDataStream::Access rhs);

    //---------------------------------------------------------------------------
    /// Modify a data stream access and usage flags.
    //---------------------------------------------------------------------------
    bool SetStreamFlags(
        Context&  io_Context,
        CSLModel& in_Model,
        NiMesh& io_Mesh,
        const NiFixedString& in_Semantic,
        NiDataStream::Access in_NewAccess,
        NiDataStream::Usage  in_NewUsage);

    //---------------------------------------------------------------------------
    /// Convert a triangle list and its attributes to a mesh, including
    /// an optional shape animation. The first version creates the mesh
    /// the other use an existing mesh.
    //---------------------------------------------------------------------------
    NiMesh* ConvertTriangleListToMesh(
        Context&  io_Context,
        CSLModel& in_Model,
        CSLXSIShape& in_Shape,
        ShapeAttributeMap& in_ShapeAttrMap,
        CSLXSITriangleList& in_Triangles,
        NiNode&   io_Node);

    bool ConvertTriangleListToMesh(
        Context&  io_Context,
        CSLModel& in_Model,
        CSLXSIShape& in_Shape,
        ShapeAttributeMap& in_ShapeAttrMap,
        CSLXSITriangleList& in_Triangles,
        NiMesh& io_Mesh);
}

#endif /* XSI_GAMEBRYO_CONVERTER_MESH_HELPERS_H */
