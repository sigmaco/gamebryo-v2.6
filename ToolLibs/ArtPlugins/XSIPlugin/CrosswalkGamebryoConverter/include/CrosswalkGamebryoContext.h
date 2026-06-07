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

#ifndef XSI_GAMEBRYO_CONVERTER_CONTEXT_H
#define XSI_GAMEBRYO_CONVERTER_CONTEXT_H

#include "CrosswalkGamebryoLogger.h"
#include "CrosswalkGamebryoProgress.h"

#include "SLTypes.h"

#include "NiMainLibType.h"

//---------------------------------------------------------------------------
// External references.
//---------------------------------------------------------------------------

class XSIEXPORT CSLScene;
class XSIEXPORT CSLModel;
class XSIEXPORT CSLBaseMaterial;

class NIMAIN_ENTRY NiAVObject;
class NIMAIN_ENTRY NiNode;

namespace epg
{
    class Logger;
    class ConversionMap;
    class NodeMap;
    class MaterialMap;
    class TextureMap;
    class EffectMap;
    class AnimationMap;
    class SkinMap;
    class LightingMap;
    class ConverterSettings;
    class SceneInfo;
    enum LogLevel;

    //---------------------------------------------------------------------------
    /// Conversion context.
    ///
    /// For convenience, it is a logger that forwards calls to the actual logger.
    /// Same thing for the progress monitor.
    //---------------------------------------------------------------------------

    class Context : public Logger, public ProgressMonitor
    {
    public:
        /// Create a conversion context with all necessary info.
        Context(
            CSLScene& in_Scene,
            const SceneInfo& in_SceneInfo,
            const ConverterSettings& in_Settings,
            const ConversionMap& in_ConvMap,
            NodeMap& in_NodeMap,
            MaterialMap& in_MatMap,
            TextureMap& in_TexMap,
            EffectMap& in_EffectMap,
            AnimationMap& in_AnimMap,
            SkinMap& in_SkinMap,
            LightingMap& in_LightingMap,
            Logger& in_Logger,
            ProgressMonitor& in_Progress);

        /// Logger API.
        void StartConversion();
        void EndConversion();
        bool Log(LogLevel, const char * in_Msg);

        /// Progress monitor API.
        void StartPhase(ProgressPhase in_Phase);
        void EndPhase(ProgressPhase in_Phase);
        void PhaseProgress(float in_CompletionFraction);

        /** Convert the given material using the conversion map.
            @param CSLBaseMaterial& the material to be converted.
            @return true if successful.
        */
        bool Convert(CSLBaseMaterial& in_Material);

        /** Convert the given model using the conversion map.
            @param CSLModel& the model to be converted.
            @return true if successful.
        */
        bool Convert(CSLModel& in_Model);

        /** Converts the children of the given node.
            Convert() automatically calls this.
        */
        bool ConvertChildren(CSLModel& in_Model);

        /// Retrieve the node map.
        NodeMap& GetNodeMap();

        /// Retrieve the material map.
        MaterialMap& GetMaterialMap();

        /// Retrieve the texture map.
        TextureMap& GetTextureMap();

        /// Retrieve the effect map.
        EffectMap& GetEffectMap();

        /// Retrieve the animation map.
        AnimationMap& GetAnimationMap();

        /// Retrieve the skin map.
        SkinMap& GetSkinMap();

        /// Retrieve the lighting map.
        LightingMap& GetLightingMap();

        /// Retrieve the conversion settings.
        const ConverterSettings& GetSettings() const;

        /// Retrieve global information about the scene.
        const SceneInfo& GetSceneInfo() const;

        /// Retrieve the scene being converted.
        CSLScene& GetScene();

    private:
        CSLScene&                   m_Scene;
        const SceneInfo&            m_SceneInfo;
        const ConverterSettings&    m_Settings;
        const ConversionMap&        m_ConvMap;
        NodeMap&                    m_NodeMap;
        MaterialMap&                m_MaterialMap;
        TextureMap&                 m_TextureMap;
        EffectMap&                  m_EffectMap;
        AnimationMap&               m_AnimationMap;
        SkinMap&                    m_SkinMap;
        LightingMap&                m_LightingMap;
        Logger&                     m_Logger;
        ProgressMonitor&            m_Progress;
        int                         m_ExpectedModelCount;
        int                         m_CurrentModelCount;
    };
}

#endif /* XSI_GAMEBRYO_CONVERTER_CONTEXT_H */
