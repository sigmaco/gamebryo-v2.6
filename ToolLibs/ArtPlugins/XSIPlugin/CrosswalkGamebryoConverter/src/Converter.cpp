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

#include "CrosswalkGamebryoConverter.h"
#include "CrosswalkGamebryoConvMap.h"
#include "CrosswalkGamebryoSkinMap.h"
#include "CrosswalkGamebryoLightingMap.h"
#include "CrosswalkGamebryoContext.h"
#include "CrosswalkGamebryoSimpleLoggers.h"
#include "CrosswalkGamebryoSimpleProgress.h"
#include "CrosswalkGamebryoImpl.h"
#include "CrosswalkGamebryoSceneInfo.h"

#include "Scene.h"
#include "Material.h"

#include "NiAVObject.h"
#include "NiMaterialProperty.h"
#include "NiTexturingProperty.h"
#include "NiTransformController.h"
#include "NiAlphaProperty.h"
#include "NiSpecularProperty.h"
#include "NiShadeProperty.h"
#include "NiDynamicEffect.h"
#include "NiNode.h"
#include "NiSourceTexture.h"
#include "NiLODNode.h"
#include "NiMesh.h"
#include "NiLight.h"
#include "NiMorphWeightsController.h"
#include "NiSkinningUtilities.h"

#include <NiTimerSharedData.h>
#include <NiFramework.h>
#include <NiSharedDataList.h>

namespace epg
{
    //---------------------------------------------------------------------------
    // Local helper functions.
    //---------------------------------------------------------------------------

    namespace
    {
        //---------------------------------------------------------------------------
        // Convert the lighting groups to the affected list of the effetc map.
        //---------------------------------------------------------------------------
        bool ConvertLightingToEffect(Context& io_Context)
        {
            typedef LightingMap::LightingGroups Groups;
            const Groups& groups = io_Context.GetLightingMap().GetLightingGroups();

            typedef Groups::const_iterator iter;
            for (iter pos = groups.begin(); pos != groups.end(); ++pos)
            {
                if (!ConvertLightingGroup(io_Context, pos->second))
                    return false;
            }

            return true;
        }

        //---------------------------------------------------------------------------
        // Apply dynamic effects to their affected objects.
        //---------------------------------------------------------------------------
        void ApplyEffects(Context & io_Context)
        {
            EffectMap& effectMap = io_Context.GetEffectMap();
            typedef EffectMap::DynamicEffects DynEffects;
            DynEffects effects = effectMap.GetEffects();

            Phaser phase(io_Context, APPLY_EFFECTS_PHASE, effects.size());

            typedef DynEffects::iterator effect_iter;
            for (effect_iter effect = effects.begin(); effect != effects.end(); ++effect)
            {
                phase.Progress();

                NiDynamicEffectPtr pEffect = effect->second;

                typedef EffectMap::AffectedObjects Affected;
                const Affected* affected = effectMap.FindAffectedObjects(*pEffect);
                if (affected)
                {
                    // Affect only a sub-set of all objects.
                    typedef Affected::const_iterator affected_iter;
                    for (affected_iter node = affected->begin(); node != affected->end(); ++node)
                    {
                        if (NiNode* pNode = *node)
                        {
                            pEffect->AttachAffectedNode(pNode);
                            pNode->UpdateEffects();
                        }
                    }
                }
                else
                {
                    // No affected object list means all objects are affected.
                    typedef NodeMap::RootObjects Objects;
                    Objects objects = io_Context.GetNodeMap().GetRoots();

                    typedef Objects::iterator object_iter;
                    for (object_iter object = objects.begin(); object != objects.end(); ++object)
                    {
                        NiAVObject * pObj = *object;
                        if (NiNode * pNode = NiDynamicCast(NiNode,pObj))
                        {
                            pEffect->AttachAffectedNode(pNode);
                            pNode->UpdateEffects();
                        }
                    }
                }
            }
        }

        //---------------------------------------------------------------------------
        // Apply alpha blending on the scene.
        //---------------------------------------------------------------------------
        void ApplyAlphaBlending(Context & io_Context)
        {
        }

        //---------------------------------------------------------------------------
        // Apply smooth shading on the scene.
        //---------------------------------------------------------------------------
        void ApplySmoothShading(Context & io_Context)
        {
        }

        //---------------------------------------------------------------------------
        // Apply the necessary flags on the node for teh animations.
        //---------------------------------------------------------------------------
        void ApplyAnimationUpdates(
            NiAVObject& in_Object,
            const bool updateTransform,
            const bool updateProperty,
            const bool updateRigid)
        {
            in_Object.SetSelectiveUpdate(true);

            if (updateTransform)
                in_Object.SetSelectiveUpdateTransforms(true);
            if (updateProperty)
                in_Object.SetSelectiveUpdatePropertyControllers(true);
            if (!updateRigid)
                in_Object.SetSelectiveUpdateRigid(false);

            if (NiNode* pNode = NiDynamicCast(NiNode, &in_Object))
            {
                for (unsigned int i = 0; i < pNode->GetChildCount(); ++i)
                {
                    NiAVObject *pObject = pNode->GetAt(i);
                    if (pObject)
                    {
                        ApplyAnimationUpdates(
                            *pObject,
                            updateTransform,
                            updateProperty,
                            updateRigid);
                    }
                }
            }
        }

        //---------------------------------------------------------------------------
        // Apply the necessary flags on the node for teh animations.
        //---------------------------------------------------------------------------
        void ApplyAnimationUpdates(Context& io_Context)
        {
            typedef AnimationMap::AnimatedObjects Anims;
            typedef Anims::iterator iter;
            Anims& anims = io_Context.GetAnimationMap().GetAnimatedObjects();
            Phaser phase(io_Context, APPLY_ANIMS_PHASE, anims.size());
            for (iter pos = anims.begin(); pos != anims.end(); ++pos)
            {
                phase.Progress();

                if (NiAVObject* pObject = NiDynamicCast(NiAVObject, *pos))
                {
                    const bool update = pObject->GetSelectiveUpdate();
                    if (!update)
                        continue;

                    // Retrieve which flags will need to be set.
                    const bool updateTransform = pObject->GetSelectiveUpdateTransforms();
                    const bool updateProperty  = pObject->GetSelectiveUpdatePropertyControllers();
                    const bool updateRigid     = pObject->GetSelectiveUpdateRigid();

                    // For parent, we only add the selective update flag so that
                    // the engine drill down to the node that actually need updating.
                    for (NiNode* pPar = pObject->GetParent(); pPar; pPar = pPar->GetParent())
                    {
                        pPar->SetSelectiveUpdate(true);
                        if (!updateRigid)
                            pPar->SetSelectiveUpdateRigid(false);
                    }

                    // If this is a node (i.e. it can have children), we propagate
                    // the updates that are needed to its chidlren so that they get
                    // animated.
                    ApplyAnimationUpdates(*pObject, updateTransform, updateProperty, updateRigid);
                }
            }
        }

        //---------------------------------------------------------------------------
        // Default do-nothing progress monitor.
        //---------------------------------------------------------------------------
        DoNothingProgressMonitor gDoNothingProgressMonitor;

        // Nixing the AutoImageConverter.  The NiDevImageConverter needs a longer
        // lifetime than what it has, so it would be better to create it in a different
        // location.
        // bsowers 9/9/08
    }

    //---------------------------------------------------------------------------
    // Constructor.
    //---------------------------------------------------------------------------

    Converter::Converter(const ConverterSettings& in_Settings)
        : m_Settings( in_Settings )
        , m_pLogger( 0 )
        , m_pProgress( &gDoNothingProgressMonitor )
    {
        SetDefaultLogger();
        SetDefaultProgressMonitor();
    }

    //---------------------------------------------------------------------------
    // Reset the converter. Keeps the settings.
    //---------------------------------------------------------------------------
    void Converter::Clear()
    {
        if (AccumulatingLogger* log = dynamic_cast<AccumulatingLogger*>(m_pLogger))
        {
            log->ClearLog();
        }

        m_NodeMap.Clear();
        m_MaterialMap.Clear();
        m_TextureMap.Clear();
        m_EffectMap.Clear();
        m_AnimationMap.Clear();
    }

    //---------------------------------------------------------------------------
    // Logger functions.
    //---------------------------------------------------------------------------

    void Converter::SetLogger(Logger& in_Logger)
    {
        m_pLogger = &in_Logger;
    }
    //---------------------------------------------------------------------------
    void Converter::SetDefaultLogger()
    {
        m_pLogger = & AccumulatingLogger::GetLogger();
    }
    //---------------------------------------------------------------------------
    Logger& Converter::GetLogger() const
    {
        return *m_pLogger;
    }

    //---------------------------------------------------------------------------
    // Set the progress monitor. The object must exists as long
    // as it is used by the converter.
    //---------------------------------------------------------------------------
    void Converter::SetProgressMonitor(ProgressMonitor& in_Monitor)
    {
        m_pProgress = &in_Monitor;
    }

    //---------------------------------------------------------------------------
    // Set the default (do-nothing) progress monitor.
    //---------------------------------------------------------------------------
    void Converter::SetDefaultProgressMonitor()
    {
        m_pProgress = &gDoNothingProgressMonitor;
    }

    //---------------------------------------------------------------------------
    // Main conversion entry-point.
    //---------------------------------------------------------------------------

    bool Converter::Convert(CSLScene& in_Scene)
    {
        Logger& logger = *m_pLogger;
        logger.StartConversion();

        ConversionMap convMap;
        SkinMap skinMap;
        LightingMap lightingMap;
        SceneInfo sceneInfo(in_Scene);
        Context context(
            in_Scene,
            sceneInfo,
            m_Settings,
            convMap,
            m_NodeMap,
            m_MaterialMap,
            m_TextureMap,
            m_EffectMap,
            m_AnimationMap,
            skinMap,
            lightingMap,
            logger,
            *m_pProgress);

        BuildTimerSharedData(sceneInfo.GetStartTime(), sceneInfo.GetEndTime());

        m_AnimationMap.ReserveSceneAnimationTimeline(
            sceneInfo.GetEndTime() - sceneInfo.GetStartTime());

        if (!ConvertImages(context, in_Scene.GetImageLibrary()))
            return false;

        if (!ConvertMaterials(context, in_Scene.GetMaterialLibrary()))
            return false;

        // Note: the envelope conversion must be done before the node
        //       conversion because the node conversion must know that
        //       a mesh is a skin so that any mesh morph can morph the
        //       correct semantic. We could edit the morph modifier
        //       semantics instead, during skin conversion.
        if (!ConvertEnvelopes(context, in_Scene.EnvelopeList()))
            return false;
        
        // Convert all models starting from the root.
        m_pProgress->StartPhase(CONVERT_MODELS_PHASE);
        CSLModel * model = in_Scene.Root();
        if (model)
            if (!context.Convert(*model))
                return false;
        m_pProgress->EndPhase(CONVERT_MODELS_PHASE);

        // Note: the skin conversion must be done after the node
        //       conversion because it needs the skins and the bones
        //       at the same time, which we cannot garantee to be
        //       available when we convert the nodes.
        if (!FillSkinningModifiers(context))
            return false;
        
        // Note: we convert the ambient light after the models to be
        //       friendlier to the animation tool: it only gives
        //       access to the first root node of a NIF file, thus
        //       it would only give access to the ambient light if
        //       it were the first root!
        CSLAmbience * ambience = in_Scene.Ambience();
        if (ambience)
            if (!ConvertAmbientLight(context, *ambience))
                return false;

        ApplyAlphaBlending(context);
        ApplySmoothShading(context);

        // Note: ConvertLightingToEffect() must be done before ApplyEffects()
        //       since it may add affected objects.
        if (!ConvertLightingToEffect(context))
            return false;

        ApplyEffects(context);

        // Transplant all other objects under the root.
        //
        // Required because the Gamebryo conversion plugin pipeline
        // only support NiNode as root, so we need to add a top
        // node to support other free obejcts like lights and effects.
        if (model)
        {
            NiAVObjectPtr root = m_NodeMap.FindObject(*model);
            if (NiNode * rootNode = NiDynamicCast(NiNode,root))
            {
                m_NodeMap.Transplant(*rootNode);

                // Reorder bones. Magic!
                if (!NiSkinningUtilities::ReorderBonesForSkinnedMeshes(rootNode))
                {
                    context.Logf(
                        LOG_WARNING,
                        "Could not re-order the bones in skinned meshes in the scene \"%s\".",
                        in_Scene.FileName().GetText());
                }
            }
            else
            {
                context.Logf(
                    LOG_ERROR,
                    "No object in the scene \"%s\".",
                    in_Scene.FileName().GetText());
                return false;
            }
        }

        // Now apply animation flags appropriately.
        ApplyAnimationUpdates(context);

        logger.EndConversion();

        return true;
    }

    //---------------------------------------------------------------------------
    // Retrieve the node map.
    //---------------------------------------------------------------------------
    NodeMap& Converter::GetNodeMap()
    {
        return m_NodeMap;
    }

    //---------------------------------------------------------------------------
    // Retrieve the material map.
    //---------------------------------------------------------------------------
    MaterialMap& Converter::GetMaterialMap()
    {
        return m_MaterialMap;
    }

    //---------------------------------------------------------------------------
    // Retrieve the texture map.
    //---------------------------------------------------------------------------
    TextureMap& Converter::GetTextureMap()
    {
        return m_TextureMap;
    }

    //---------------------------------------------------------------------------
    // Retrieve the effect map.
    //---------------------------------------------------------------------------
    EffectMap& Converter::GetEffectMap()
    {
        return m_EffectMap;
    }

    //---------------------------------------------------------------------------
    // Retrieve the settings.
    //---------------------------------------------------------------------------
    ConverterSettings& Converter::GetSettings()
    {
        return m_Settings;
    }

    //---------------------------------------------------------------------------
    void Converter::BuildTimerSharedData(float fStartTime, float fEndTime)
    {
        NiFramework& kFramework = NiFramework::GetFramework();
        NiSharedDataList& kSharedDataList = kFramework.GetSharedDataList();

        // Lock the shared data list so other thread don't use it
        kSharedDataList.Lock();

        // Build the Shared Data
        NiTimerSharedDataPtr spTimer = NiNew NiTimerSharedData;

        spTimer->SetCurrentTime(0.0f);
        spTimer->SetScaleFactor(1.0f);
        spTimer->SetTimeMode(NiTimerSharedData::LOOP);

        spTimer->SetStartTime(fStartTime);
        spTimer->SetEndTime(fEndTime);

        kSharedDataList.Insert(spTimer);
        kSharedDataList.Unlock();
    }
}
