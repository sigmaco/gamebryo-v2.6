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

#ifndef XSI_GAMEBRYO_CONVERTER_IMPL_H
#define XSI_GAMEBRYO_CONVERTER_IMPL_H

#include "SLTypes.h"
#include "Template.h"

#include "NiMainLibType.h"

#include <vector>

//---------------------------------------------------------------------------
// External references.
//---------------------------------------------------------------------------

class XSIEXPORT CSLModel;
class XSIEXPORT CSLXSITransform;
class XSICOREEXPORT CSIBCVector3D;
class XSIEXPORT CSIBCColorf;
class XSIEXPORT CSLBaseMaterial;
class XSIEXPORT CSLMaterialLibrary;
class XSIEXPORT CSLImageLibrary;
class XSIEXPORT CSLImage;
class XSIEXPORT CSLAmbience;
class XSIEXPORT CSLXSIMesh;
class XSIEXPORT CSLXSIShape;
class XSIEXPORT CSLXSIShapeAnimation;
class XSIEXPORT CSLXSITriangleList;
class XSIEXPORT CSLEnvelopeList;
class XSIEXPORT CSLEnvelope;

class NIMAIN_ENTRY NiTransform;
class NIMAIN_ENTRY NiTransformController;
class NIMAIN_ENTRY NiFloatInterpolator;
class NIMAIN_ENTRY NiMatrix3;
class NIMAIN_ENTRY NiPoint3;
class NIMAIN_ENTRY NiAVObject;
class NIMAIN_ENTRY NiObjectNET;
class NIMAIN_ENTRY NiColor;
class NIMAIN_ENTRY NiMesh;

namespace epg
{
    class Context;
    class AnimCurves;
    class SceneInfo;

    //---------------------------------------------------------------------------
    /// The conversion of transform from XSI to Gamebryo requires creating
    /// intermediary nodes to support all the features of XSI while keeping
    /// the conversion code clear. We need to identify those various nodes
    /// in the code so we use the following enumeration for this purpose.
    //---------------------------------------------------------------------------

    enum DummyNodeType
    {
        NEUTRAL_POSE_DUMMY_NODE,
        PIVOT_COMPENSATION_DUMMY_NODE,
        TRANSLATE_DUMMY_NODE,
        PIVOT_DUMMY_NODE,
        SCALE_ROTATE_DUMMY_NODE,
        ANTI_PIVOT_DUMMY_NODE,
        DUMMY_NODE_TYPE_COUNT
    };

    /// Retrieve a printable version of a dummy node type.
    const char * GetDummyNodeTypeName(DummyNodeType in_Type);

    //---------------------------------------------------------------------------
    // Transform conversion.
    //---------------------------------------------------------------------------

    /// Convert angles from degrees to radians.
    SI_Float DegreeToRadian(SI_Float rot);

    /// Convert XSI rotation to Gamebryo format.
    /// @note handle some axis re-ordering and angle adjustment
    ///       to compensate from different axis standard.
    bool ConvertRotation(
        Context & io_Context,
        CSLModel & in_Model,
        const CSIBCVector3D & in_Rotation,
        CSLTemplate::ERotationOrder in_RotOrder,
        NiMatrix3 & io_Rotation);

    /// Convert XSI translation to Gamebryo format.
    void ConvertTranslation(
        const CSIBCVector3D & in_Translation,
        NiPoint3 & io_Translation,
        const CSIBCVector3D* in_Scale);

    /// Convert XSI scaling to Gamebryo format, detecting non-uniform scales.
    const CSIBCVector3D* ConvertScale(
        Context & io_Context,
        CSLModel & in_Model,
        const CSIBCVector3D & in_Scale,
        float & io_Scale);

    /// Convert XSI non-uniform scale to an average.
    const CSIBCVector3D* ConvertAverageScale(
        Context & io_Context,
        CSLModel & in_Model,
        const CSIBCVector3D & in_Scale,
        float & io_Scale);

    /// Scale points to conform to a non-uniform scale.
    void ScalePoints(
        const CSIBCVector3D & in_Scale,
        std::vector<float> & io_Vertices);

    /// Convert XSI transfrom to Gamebryo format.
    bool ConvertTransform(
        Context & io_Context,
        CSLModel & in_Model,
        NiAVObject & io_Object);

    //---------------------------------------------------------------------------
    // Color conversion.
    //---------------------------------------------------------------------------

    /// Convert color, taking into account alpha.
    bool ConvertColor(
        Context & io_Context,
        const CSIBCColorf & in_XSIColor,
        NiColor & out_GBColor,
        const char * in_ObjName,
        const char * in_PropName);

    //---------------------------------------------------------------------------
    // Image conversion.
    //---------------------------------------------------------------------------

    /// Convert an image library and registers it in the context image map.
    bool ConvertImages(Context& io_Context, CSLImageLibrary* in_ImageLib);
    /// Convert an image and registers it in the context image map.
    bool ConvertImage(Context&  io_Context, CSLImage& in_Image);

    //---------------------------------------------------------------------------
    // Skin conversion.
    //---------------------------------------------------------------------------

    /// Convert an envelope library and registers it in the context skin map.
    bool ConvertEnvelopes(Context&  io_Context, CSLEnvelopeList* in_pEnvelopes);
    /// Convert an envelope and registers it in the context skin map.
    bool ConvertEnvelope(Context& io_Context, CSLEnvelope& in_Envelope);

    /// Convert a skinand registers it in the context skin map.
    bool ConvertSkin(
        Context& io_Context,
        CSLModel& in_Model,
        CSLXSIShape& in_SkinShape,
        CSLXSITriangleList& in_Triangles,
        NiMesh& io_Mesh);

    /// Fill the missing information of all skins in the skin map.
    bool FillSkinningModifiers(Context& io_Context);

    //---------------------------------------------------------------------------
    /// Convert all animation clips found in the specified model.
    /// Also adds the necessary extra data for the controller extractor plugin.
    //---------------------------------------------------------------------------

    bool ConvertAnimationClips(
        Context& io_Context,
        CSLModel& in_Model,
        NiAVObject& io_Object);

    //---------------------------------------------------------------------------
    /// Light conversion.
    //---------------------------------------------------------------------------

    bool ConvertAmbientLight(Context& io_Context, CSLAmbience& in_Model);

    //---------------------------------------------------------------------------
    /// Material conversion.
    //---------------------------------------------------------------------------

    /// Convert a material library and registers it in the context material map.
    bool ConvertMaterials(Context&   io_Context, CSLMaterialLibrary* in_MatLib);
    /// Convert a material and registers it in the context material map.
    bool ConvertMaterial(Context&    io_Context, CSLBaseMaterial& in_Material);
    /// Convert a material and registers it in the context material map.
    bool ConvertXSIMaterial(Context& io_Context, CSLBaseMaterial& in_Material);

    //---------------------------------------------------------------------------
    // Transform animation conversion.
    //---------------------------------------------------------------------------

    /** The various curves supported: position, rotation, scale.
    */
    enum TransformCurveType
    {
        TRANSFORM_CURVE_POS,
        TRANSFORM_CURVE_ROT,
        TRANSFORM_CURVE_SCALE,
        TRANSFORM_CURVE_TYPE_COUNT
    };

    /// Convert a transform animation into Gamebryo controller and interpolator.
    bool ConvertTransformAnimation(
        Context& io_Context,
        CSLModel& in_Model,
        NiAVObject& in_Object,
        CSLTemplate& in_Transform,
        DummyNodeType in_DummyNodeType,
        const CSIBCVector3D& in_DefaultScale,
        const CSIBCVector3D& in_DefaultTranslation,
        const CSIBCVector3D& in_DefaultRotation,
        CSLTemplate::ERotationOrder in_RotOrder);

    /// Detect if a transform is animated.
    bool IsTransformAnimated(
        Context& io_Context,
        CSLModel& in_Model,
        CSLXSITransform& in_TransformXSI,
        DummyNodeType in_DummyNodeType);

    //---------------------------------------------------------------------------
    // Animation conversion.
    //---------------------------------------------------------------------------

    /// Convert the 1-dimensional float values in the anim curve into a Gamebryo interpolator.
    NiFloatInterpolator* ConvertFloatCurves(
        Context& io_Context,
        const char* in_ModelName,
        AnimCurves& in_Curves,
        float& io_InitialValue);

    /// Convert an XSI shape animation into a Gamebryo morph controller and weight interpolator.
    bool ConvertShapeAnimation(
        Context& io_Context,
        CSLModel& in_Model,
        CSLXSIShape& in_BaseShape,
        CSLXSIShapeAnimation& in_ShapeAnim,
        CSLXSITriangleList& in_Triangles,
        NiMesh& io_Mesh);

    //---------------------------------------------------------------------------
    // Gamebryo common properties conversion.
    //---------------------------------------------------------------------------

    /// Connect the Gamebryo node into the Gamebryo scene.
    bool ConnectNodes(
        Context & io_Context,
        CSLModel & in_Model,
        NiAVObject& io_TopChild,
        NiAVObject & io_BottomChild);

    /// Copy the XSI model name into the Gamebryo node.
    void CopyNodeName(
        CSIBCString& in_Name,
        NiAVObject& io_Object,
        bool in_FixABVTag = true);

    /// Copy the visibility flag.
    void CopyVisibility(
        CSLModel & in_Model,
        NiAVObject& io_Object);

    /// Convert the common properties: name, visibility, connection, etc.
    bool ConvertCommonProperties(
        Context & io_Context,
        CSLModel & in_Model,
        NiAVObject& io_Object,
        CSIBCString & in_Name);

    /// Convert the common properties: name, visibility, connection, z-buffer, etc.
    bool ConvertCommonProperties(
        Context & io_Context,
        CSLModel & in_Model,
        NiAVObject& io_Object,
        bool in_CreateDummyNode = false);

    /// Convert the Z-buffer property.
    void ConvertZBufferProperties(
        Context & io_Context,
        CSLModel & in_Model,
        NiAVObject& io_Object);
}

#endif /* XSI_GAMEBRYO_CONVERTER_IMPL_H */
