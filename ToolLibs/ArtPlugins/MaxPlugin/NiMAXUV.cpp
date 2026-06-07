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
#include "NiMAXAnimationConverter.h"
#include "NiMAXUV.h"


//---------------------------------------------------------------------------
NiMAXUV::NiMAXUV() : m_kChannels(1,5) 
{
    m_kAnimStart = 0;
    m_bUseVCForUV = false;
}
//---------------------------------------------------------------------------
NiMAXUV::NiMAXUV(TimeValue kAnimStart) : m_kChannels(1,5)
{
    m_kAnimStart = kAnimStart;
    m_bUseVCForUV = false;
    m_iAttrId = -1;
}
//---------------------------------------------------------------------------
NiMAXUV::Channel::Channel()
{
    CHECK_MEMORY();
    m_ausOrder[0] = 0;
    m_ausOrder[1] = 1;

    m_kUVCropOffset.x = 0.0f;
    m_kUVCropOffset.x = 0.0f;
    m_kUVCropDimensions.x = 1.0f;
    m_kUVCropDimensions.y = 1.0f;

    m_sMAXtoNIchannel = -1;
    m_iMaxChannel = -1;
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
NiMAXUV::Channel::Channel(
    Texmap* pkTm,
    UVGen* pkUVGen,
    TimeValue kAnimStart, 
    int iAttrId,
    int iMaxChannel)
{
    CHECK_MEMORY();
    int iAxis = pkUVGen->GetAxis();
    StdUVGen* pkStdUVGen = NULL;
    if (pkUVGen && pkUVGen->IsStdUVGen())
        pkStdUVGen = (StdUVGen*) pkUVGen;

    if (iAxis == AXIS_UV)
    {
        m_ausOrder[0] = 0;
        m_ausOrder[1] = 1;
    }
    else if (iAxis == AXIS_VW)
    {
        m_ausOrder[0] = 1;
        m_ausOrder[1] = 2;
    }
    else // (iAxis == AXIS_WU)
    {
        m_ausOrder[0] = 2;
        m_ausOrder[1] = 0;
    }

    TexmapInfo* pkTmInfo = NiNew TexmapInfo;

    if (pkStdUVGen)
    {
        pkTmInfo->m_kUVOffset.x = pkStdUVGen->GetUOffs(kAnimStart);
        pkTmInfo->m_kUVOffset.y = pkStdUVGen->GetVOffs(kAnimStart);
        pkTmInfo->m_kUVScale.x = pkStdUVGen->GetUScl(kAnimStart);
        pkTmInfo->m_kUVScale.y = pkStdUVGen->GetVScl(kAnimStart);
    }
    else
    {
        pkTmInfo->m_kUVOffset.x = 0.0f;
        pkTmInfo->m_kUVOffset.y = 0.0f;
        pkTmInfo->m_kUVScale.x = 1.0f;
        pkTmInfo->m_kUVScale.y = 1.0f;
    }

    pkTmInfo->m_fWAngle = 0.0f;
    pkTmInfo->m_pkTexmap = pkTm;

    m_kUVCropOffset.x = 0.0f;
    m_kUVCropOffset.y = 0.0f;
    m_kUVCropDimensions.x = 1.0f;
    m_kUVCropDimensions.y = 1.0f;

    if (pkStdUVGen)
    {
        pkTmInfo->GenerateTextureTransformControllers(pkStdUVGen);
    }
    else
    {
        for (unsigned int ui = 0; ui < MAX_TEX_TRANSFORM_TYPE; ui++)
            pkTmInfo->m_spControllers[ui] = NULL;
    }

    m_kTexmaps.Add(pkTmInfo);

    // This is filled in later by the geometry converter.
    m_sMAXtoNIchannel = -1;

    m_iAttrId = iAttrId; // multi/sub-object id

    m_iMaxChannel = iMaxChannel;
}
//---------------------------------------------------------------------------
NiMAXUV::Channel::Channel(Texmap* pkTm,
    UVGen* pkUVGen,
    IParamBlock2 * pBlock, 
    TimeValue kAnimStart,
    int iAttrId,
    int iMaxChannel)
{
    int iAxis = AXIS_UV;
    StdUVGen* pkStdUVGen = NULL;

    if (pkUVGen)
    {
        iAxis = pkUVGen->GetAxis();
        if (pkUVGen->IsStdUVGen())
            pkStdUVGen = (StdUVGen*) pkUVGen;
    }

    if (iAxis == AXIS_UV)
    {
        m_ausOrder[0] = 0;
        m_ausOrder[1] = 1;
    }
    else if (iAxis == AXIS_VW)
    {
        m_ausOrder[0] = 1;
        m_ausOrder[1] = 2;
    }
    else // (iAxis == AXIS_WU)
    {
        m_ausOrder[0] = 2;
        m_ausOrder[1] = 0;
    }

    TexmapInfo* pkTmInfo = NiNew TexmapInfo;
    if (pkStdUVGen)
    {
        pkTmInfo->m_kUVOffset.x = pkStdUVGen->GetUOffs(kAnimStart);
        pkTmInfo->m_kUVOffset.y = pkStdUVGen->GetVOffs(kAnimStart);
        pkTmInfo->m_kUVScale.x = pkStdUVGen->GetUScl(kAnimStart);
        pkTmInfo->m_kUVScale.y = pkStdUVGen->GetVScl(kAnimStart);
    }
    else
    {
        pkTmInfo->m_kUVOffset.x = 0.0f;
        pkTmInfo->m_kUVOffset.y = 0.0f;
        pkTmInfo->m_kUVScale.x = 1.0f;
        pkTmInfo->m_kUVScale.y = 1.0f;
    }
    pkTmInfo->m_fWAngle = 0.0f;
    pkTmInfo->m_pkTexmap = pkTm;

    m_kUVCropOffset.x = 0.0f;
    m_kUVCropOffset.y = 0.0f;
    m_kUVCropDimensions.x = 1.0f;
    m_kUVCropDimensions.y = 1.0f;

    m_kTexmaps.Add(pkTmInfo);

    // These enumerated values are taken directly from the 
    // bmtex.cpp from the Max SDK and are what is
    // needed to communicate with the current clipping values.
    enum
    {
        bmtex_clipu,
        bmtex_clipv,
        bmtex_clipw,
        bmtex_cliph, 
        bmtex_jitter,
        bmtex_usejitter,
        bmtex_apply,
        bmtex_crop_place, 
    };

    if (pkUVGen != NULL)
    {
        Interval valid;
        BOOL bApply;
        if (pBlock)
        {
            pBlock->GetValue(bmtex_apply, 0, bApply, valid);

            if (bApply)
            {
                pBlock->GetValue(bmtex_clipu, 0, m_kUVCropOffset.x, valid);
                pBlock->GetValue(bmtex_clipv, 0, m_kUVCropOffset.y, valid);
                pBlock->GetValue(bmtex_clipw, 0, m_kUVCropDimensions.x,
                    valid);
                pBlock->GetValue(bmtex_cliph, 0, m_kUVCropDimensions.y, 
                    valid);
            }
        }

        if (pkStdUVGen)
            pkTmInfo->m_fWAngle = pkStdUVGen->GetWAng(0);
    }

    if (pkStdUVGen)
    {
        pkTmInfo->GenerateTextureTransformControllers(pkStdUVGen);
    }
    else
    {
        for (unsigned int ui = 0; ui < MAX_TEX_TRANSFORM_TYPE; ui++)
            pkTmInfo->m_spControllers[ui] = NULL;
    }

    // This is filled in later by the geometry converter.
    m_sMAXtoNIchannel = -1;

    m_iAttrId = iAttrId; // multi/sub-object id

    m_iMaxChannel = iMaxChannel;
}
//---------------------------------------------------------------------------
void NiMAXUV::Convert(Texmap* pkTm)
{
    if (pkTm/* && pkTm->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)*/)
    {
        //BitmapTex       * pkBm = (BitmapTex*)pkTm;   
        IParamBlock2    * pBlock = pkTm->GetParamBlock( 0 );
        UVGen           * pkUVGen = pkTm->GetTheUVGen();
        StdUVGen        * pkStdUVGen = NULL;
        if (pkUVGen && pkUVGen->IsStdUVGen())
            pkStdUVGen = (StdUVGen*) pkUVGen;

        int iSource = pkTm->GetUVWSource();

        if (pkUVGen == NULL)
        {
            return;
        }
        else if (iSource == UVWSRC_EXPLICIT)
        {
            iSource = pkTm->GetMapChannel();
            NIASSERT(iSource != 0);
        }
        else if (iSource == UVWSRC_EXPLICIT2)
        {
            // MAX code for "use vertex colors"
            iSource = 0;
            m_bUseVCForUV = true;
        }
        else
        {
            // nothing really good to do if they use one of the 
            // other uv channels
            return;
        }

        NiMAXUV::ChannelPtr spChannel = NiNew NiMAXUV::Channel(pkTm, pkUVGen, 
            pBlock, m_kAnimStart, m_iAttrId, iSource);

        int iChannelID = GetEquivalentChannelIndex(spChannel);
        if(iChannelID > -1)
        {
            NiMAXUV::ChannelPtr spOldChannel = m_kChannels.GetAt(iChannelID);
            spOldChannel->MergeChannelInfo(spChannel);
        }
        else
        {
            m_kChannels.Add(spChannel);
        }
    }
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXUV::Channel::TexmapInfo::GenerateTextureTransformControllers(
    StdUVGen* pkUVGen)
{
    CHECK_MEMORY();
    NIASSERT(pkUVGen->NumSubs() >= 1);
    Animatable* pkSubAnim = pkUVGen->SubAnim(0);
    NIASSERT(pkSubAnim->SuperClassID() == PARAMETER_BLOCK_CLASS_ID);

    IParamBlock* pkPB = (IParamBlock*) pkSubAnim;

/*
  "Coordinates" Parameters 
    0  U Offset
    1  V Offset
    2  U Tiling
    3  V Tiling
    4  U Angle
    5  V Angle
    6  W Angle
    7  Blur
    8  Noise Amount
    9  Noise Size
    10 Noise Levels
    11 Phase
    12 Blur Offset
*/

    for (unsigned int ui = 0; ui < MAX_TEX_TRANSFORM_TYPE; ui++)
        m_spControllers[ui] = NULL;

/*    c=AnimatableName(map, "U Offset");
    c=AnimatableName(map, "V Offset");
    c=AnimatableName(map, "U Tiling");
    c=AnimatableName(map, "V Tiling");
    c=AnimatableName(map, "W Angle");
    */
    

    for (int j = 0; j < 12; j++)
    {
        Control* pkCont = pkPB->GetController(j);
        if (pkCont)
        {
            NiMAXAnimationConverter anim(0, 0);
            NiInterpolator* pkInterp = NULL;

            /*
            enum TransformMember {
                TT_TRANSLATE_U = 0,
                TT_TRANSLATE_V,
                TT_ROTATE,
                TT_SCALE_U,
                TT_SCALE_V
            };
            */
            unsigned int uiIndex = MAX_TEX_TRANSFORM_TYPE;
            switch (j)
            {
                case 0:
                    pkInterp = anim.ConvertFloatAnim(pkCont);
                    if (pkInterp)
                    {
                        uiIndex = (unsigned int) 
                            NiTextureTransformController::TT_TRANSLATE_U;
                        m_spControllers[uiIndex] = 
                            NiNew NiTextureTransformController;
                        m_spControllers[uiIndex]->SetAffectedMember(
                            (NiTextureTransformController::TransformMember)
                            uiIndex);
                
                        m_spControllers[uiIndex]->SetInterpolator(pkInterp);
                        m_spControllers[uiIndex]->ResetTimeExtrema();
                    }
                    break;
                case 1:
                    pkInterp = anim.ConvertFloatAnim(pkCont);
                    if (pkInterp)
                    {
                        uiIndex = (unsigned int) 
                            NiTextureTransformController::TT_TRANSLATE_V;
                        m_spControllers[uiIndex] = 
                            NiNew NiTextureTransformController;
                        m_spControllers[uiIndex]->SetAffectedMember(
                            (NiTextureTransformController::TransformMember)
                            uiIndex);
                
                        m_spControllers[uiIndex]->SetInterpolator(pkInterp);
                        m_spControllers[uiIndex]->ResetTimeExtrema();
                    }
                    break;
                case 2:
                    pkInterp = anim.ConvertFloatAnim(pkCont);
                    if (pkInterp)
                    {
                        uiIndex = (unsigned int) 
                            NiTextureTransformController::TT_SCALE_U;
                        m_spControllers[uiIndex] = 
                            NiNew NiTextureTransformController;
                        m_spControllers[uiIndex]->SetAffectedMember(
                            (NiTextureTransformController::TransformMember)
                            uiIndex);
                
                        m_spControllers[uiIndex]->SetInterpolator(pkInterp);
                        m_spControllers[uiIndex]->ResetTimeExtrema();
                    }
                    break;
                case 3:
                    pkInterp = anim.ConvertFloatAnim(pkCont);
                    if (pkInterp)
                    {
                        uiIndex = (unsigned int) 
                            NiTextureTransformController::TT_SCALE_V;
                        m_spControllers[uiIndex] = 
                            NiNew NiTextureTransformController;
                        m_spControllers[uiIndex]->SetAffectedMember(
                            (NiTextureTransformController::TransformMember)
                            uiIndex);
                
                        m_spControllers[uiIndex]->SetInterpolator(pkInterp);
                        m_spControllers[uiIndex]->ResetTimeExtrema();
                    }
                    break;

                /*case 4:
                case 5:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                    anim.ConvertFloatAnim(pkCont, &uiNumKeys, &pkKeys, &eType);
                    if (uiNumKeys > 1)
                    {
                        NiOutputDebugString("Found Keys\n");
                    }
                    break;*/
                case 6:
                    pkInterp = anim.ConvertFloatAnim(pkCont);
                    if (pkInterp)
                    {
                        uiIndex = (unsigned int) 
                            NiTextureTransformController::TT_ROTATE;
                        m_spControllers[uiIndex] = 
                            NiNew NiTextureTransformController;
                        m_spControllers[uiIndex]->SetAffectedMember(
                            (NiTextureTransformController::TransformMember)
                            uiIndex);
                
                        m_spControllers[uiIndex]->SetInterpolator(pkInterp);
                        m_spControllers[uiIndex]->ResetTimeExtrema();
                    }
                    break;
            }
            

            if (pkCont)
            {
                // if any of the tracks are not clamped, set the cycle type 
                // to that type (ie, loop or reverse).
                NiTimeController::CycleType eCycleType = 
                    NiMAXAnimationConverter::SetORT(pkCont, NULL);
        
                if (uiIndex != MAX_TEX_TRANSFORM_TYPE && 
                    m_spControllers[uiIndex] != NULL)
                {
                    if (eCycleType != NiTimeController::CLAMP)
                        m_spControllers[uiIndex]->SetCycleType(eCycleType);
                
                    if (NiMAXConverter::GetUseAppTime())
                    {
                        m_spControllers[uiIndex]->SetAnimType(
                            NiTimeController::APP_TIME);
                    }
                    else
                    {
                        m_spControllers[uiIndex]->SetAnimType(
                            NiTimeController::APP_INIT);
                    }
                }
            }
        }
    }
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXUV::AssignTextureTransformControllers(
    NiTObjectArray<NiTextureTransformControllerPtr>& kTTCont,
    NiTexturingProperty* pkProperty, 
    NiTexturingProperty::Map* pkMap)
{
    if (!pkProperty)
        return;

    if (!pkMap)
        return;

    for (unsigned int ui = 0; ui < kTTCont.GetSize(); ui++)
    {
        NiTextureTransformController* pkController = kTTCont.GetAt(ui);
        if (pkController)
        {
            pkController->SetTarget(pkProperty);
            pkController->SetMap(pkMap);
        }
    }
}
//---------------------------------------------------------------------------
short NiMAXUV::GetMAXChannel(Texmap* pkTexmap)
{
    CHECK_MEMORY();
    int uiSize = m_kChannels.GetEffectiveSize();
    for(int i = 0; i < uiSize; i++)
    {
        NiMAXUV::Channel* pkChannel = m_kChannels.GetAt(i);
        if (pkChannel->m_iAttrId == m_iAttrId)
        {
            int uiTexmapArraySize = pkChannel->m_kTexmaps.GetSize();
            for(int j = 0; j < uiTexmapArraySize; j++)
            {
                Channel::TexmapInfo* pkInfo = pkChannel->m_kTexmaps.GetAt(j);
                if (pkInfo &&  (pkTexmap == pkInfo->m_pkTexmap))
                {
                    return (short)i;
                }
            }
        }
    }
    CHECK_MEMORY();
    return -1;
}

//---------------------------------------------------------------------------
int NiMAXUV::GetEquivalentChannelIndex(NiMAXUV::Channel* pOther)
{
    CHECK_MEMORY();
    int uiSize = m_kChannels.GetEffectiveSize();
    for(int i = 0; i < uiSize; i++)
    {
        if(pOther->CanShareChannelInfo(m_kChannels.GetAt(i)))
        {
            return i;
        }
    }
    CHECK_MEMORY();
    return -1;
}
//---------------------------------------------------------------------------
short NiMAXUV::GetMAXtoNI(Texmap* pkTexmap)
{
    CHECK_MEMORY();
    int uiSize = m_kChannels.GetEffectiveSize();
    for(int i = 0; i < uiSize; i++)
    {
        NiMAXUV::Channel* pkChannel = m_kChannels.GetAt(i);
        if (pkChannel->m_iAttrId == m_iAttrId)
        {
            int uiTexmapArraySize = pkChannel->m_kTexmaps.GetSize();
            for(int j = 0; j < uiTexmapArraySize; j++)
            {
                Channel::TexmapInfo* pkInfo = pkChannel->m_kTexmaps.GetAt(j);
                if (pkInfo && (pkTexmap == pkInfo->m_pkTexmap))
                {
                    CHECK_MEMORY();
                    return pkChannel->m_sMAXtoNIchannel;
                }
            }
        }
    }
    CHECK_MEMORY();
    return -1;
}
//---------------------------------------------------------------------------
NiTextureTransform* 
NiMAXUV::CreateTextureTransform(unsigned int uiMAXChannel, Texmap* pkTexmap)
{
    NiPoint2 kScale = GetTiling(uiMAXChannel, pkTexmap);
    NiPoint2 kTranslate = GetOffset(uiMAXChannel, pkTexmap);
    float fWAngle = GetWAngle(uiMAXChannel, pkTexmap);

    if (!HasIdentityTransforms(uiMAXChannel, pkTexmap))
    {
        NiTextureTransform* pTransform = NiNew NiTextureTransform;
        pTransform->SetTransformMethod(NiTextureTransform::MAX_TRANSFORM);
        pTransform->SetScale(kScale);
        pTransform->SetTranslate(kTranslate);
        pTransform->SetRotate(fWAngle);
        NiPoint2 kCenter = GetCropOffset(uiMAXChannel) + 
            0.5f*(GetCropDimensions(uiMAXChannel));
        pTransform->SetCenter(kCenter);
        return pTransform;
    }
    else
    {
        return NULL;
    }
}
//---------------------------------------------------------------------------
bool NiMAXUV::HasAnimatedTransforms(
    unsigned int uiMAXChannel,
    Texmap* pkTexmap)
{
    Channel* pkChannel = m_kChannels.GetAt(uiMAXChannel);
    if (pkChannel == NULL)
        return false;

    Channel::TexmapInfo* pkInfo = pkChannel->GetTexmapInfo(pkTexmap);
    //NIASSERT(pkInfo);
    if (!pkInfo)
        return false;

    for (unsigned int ui = 0; ui < MAX_TEX_TRANSFORM_TYPE; ui++)
    {
        if (pkInfo->m_spControllers[ui] != NULL)
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiMAXUV::HasIdentityTransforms(
    unsigned int uiMAXChannel, 
    Texmap* pkTexmap)
{
    NiPoint2 kScale = GetTiling(uiMAXChannel, pkTexmap);
    NiPoint2 kTranslate = GetOffset(uiMAXChannel, pkTexmap);
    float fWAngle = GetWAngle(uiMAXChannel, pkTexmap);

    // We disregard the Cropping offset and dimensions here as 
    // they are only necessary if we have scale, rotation, or translation

    if (HasAnimatedTransforms(uiMAXChannel, pkTexmap))
        return false;

    if (kScale.x != 1.0f || kScale.y != 1.0f || kTranslate.x != 0.0f 
        || kTranslate.y != 0.0f || fWAngle != 0.0f)
    {
        return false;
    }
    else
    {
        return true;
    }
}
//---------------------------------------------------------------------------
void NiMAXUV::AssignMapChannel(
    NiTexturingProperty::Map*& pkMap,
    NiFlipController*& pkFlipper,
    NiTObjectArray<NiTextureTransformControllerPtr>& kTTCont, 
    Texmap* pkTexmap,
    bool bAutoDelete)
{
    CHECK_MEMORY();

    if (pkMap == NULL)
        return;

    NiTexture* pkTexture = pkMap->GetTexture();
    if (pkTexture == NULL)
        return;

    if (NiIsKindOf(NiSourceCubeMap, pkTexture))
        return;

    // delete map if it is assigned to an undefined map channel
    if (GetMAXtoNI(pkTexmap) == -1 && bAutoDelete)
    {
        if (NiIsKindOf(NiSourceTexture, pkTexture))
        {
            char acString[1024];
            NiSprintf(acString, 1024, "Deleting texture, \"%s\", map due "
                "to invalid UV channel assignment.\n", 
                ((NiSourceTexture*)pkTexture)->GetFilename());
            NILOGWARNING(acString);
        }

        NiDelete pkMap;
        pkMap = 0;
        NiDelete pkFlipper;
        pkFlipper = 0;
    }
    else
    {
        short sMAXIndex = GetMAXtoNI(pkTexmap);
        unsigned int uiMAXIndex = sMAXIndex;
        if (sMAXIndex == -1)
            uiMAXIndex = 0;
        unsigned int uiChannelIdx = GetMAXChannel(pkTexmap);

        pkMap->SetTextureIndex(uiMAXIndex);
        NiTextureTransform* pkTransform = 
            CreateTextureTransform(uiChannelIdx, pkTexmap);

        GetTextureTransformControllers(uiChannelIdx, pkTexmap, kTTCont);

        if (pkTransform)
            pkMap->SetTextureTransform(pkTransform);
    }
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXUV::GetTextureTransformControllers(
    unsigned int uiMAXChannel, 
    Texmap* pkTexmap,
    NiTObjectArray<NiTextureTransformControllerPtr>& kTTControllers)
{
    Channel* pkChannel = m_kChannels.GetAt(uiMAXChannel);
    if (pkChannel == NULL)
        return;

    Channel::TexmapInfo* pkInfo = pkChannel->GetTexmapInfo(pkTexmap);
    //NIASSERT(pkInfo);
    if (!pkInfo)
        return;

    for (unsigned int ui = 0; ui < MAX_TEX_TRANSFORM_TYPE; ui++)
    {
        if (pkInfo->m_spControllers[ui] != NULL)
        {
            kTTControllers.Add(pkInfo->m_spControllers[ui]);
        }
    }
}
//---------------------------------------------------------------------------
NiMAXUV::Channel::TexmapInfo* NiMAXUV::Channel::GetTexmapInfo(Texmap* pkMap)
{
    for (unsigned int ui = 0; ui < m_kTexmaps.GetSize(); ui++)
    {
        Channel::TexmapInfo* pkInfo = m_kTexmaps.GetAt(ui);
        if (pkInfo != NULL && pkInfo->m_pkTexmap == pkMap)
            return pkInfo;
    }
    return NULL;
}
//---------------------------------------------------------------------------
bool NiMAXUV::Channel::MergeChannelInfo(Channel* pOther)
{
    if (!pOther)
        return false;

    for (unsigned int ui = 0; ui < pOther->m_kTexmaps.GetSize();ui++)
    {
        m_kTexmaps.Add(pOther->m_kTexmaps.GetAt(ui));
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiMAXUV::HasAnimatedTextureTransform(
    NiTexturingProperty* pkProperty,
    NiTexturingProperty::Map* pkMap)
{
    if (!pkProperty || !pkMap)
        return false;

    NiTimeController* pkController = pkProperty->GetControllers();

    while (pkController)
    {
        if (NiIsKindOf(NiTextureTransformController, pkController))
        {
            NiTextureTransformController* pkTTCont = 
                (NiTextureTransformController*) pkController;

            NIASSERT(pkTTCont->GetMap() != NULL);

            if (pkTTCont->GetMap() == pkMap)
                return true;
        }
        pkController = pkController->GetNext();
    }

    return false;
}
//---------------------------------------------------------------------------
void NiMAXUV::BakeTransformsIntoGeometry(
    NiMesh* pkMesh, 
    NiTexturingProperty* pkTexProp)
{
    if (!pkTexProp || !pkMesh)
        return;

    // Build an array containing all maps in this property.
    NiTPrimitiveArray<NiTexturingProperty::Map*> kMapArray(10, 1);
    const NiTexturingProperty::NiMapArray& kTempMaps = pkTexProp->GetMaps();
    unsigned int uiTempMapCount = kTempMaps.GetSize();
    for (unsigned int ui = 0; ui < uiTempMapCount; ui++)
    {
        if (kTempMaps.GetAt(ui))
        {
            kMapArray.Add(kTempMaps.GetAt(ui));
        }
    }

    for (unsigned int uj = 0; uj < pkTexProp->GetShaderArrayCount(); uj++)
    {
        if (pkTexProp->GetShaderMap(uj))
            kMapArray.Add(pkTexProp->GetShaderMap(uj));
    }

    // For each map, check to see if it uniquely points to a UV coordinate set
    // or if the UV transform is the same for ALL of them.
    // If so and the texture transform is NOT animated, get its texture 
    // transform matrix and apply it to all UV's in that set and then remove 
    // the texture transform. Otherwise, we will need to keep the texture
    // transforms around.


    unsigned int uiMapSize = kMapArray.GetSize();
    for (unsigned int uiIdx = 0; uiIdx < uiMapSize; uiIdx++)
    {
        NiTexturingProperty::Map* pkMap = kMapArray.GetAt(uiIdx);
        if (pkMap)
        {
            unsigned int uiMapUVSet = pkMap->GetTextureIndex();
            NiTextureTransform* pkTexTransform = pkMap->GetTextureTransform();
            bool bAnimated = HasAnimatedTextureTransform(pkTexProp, pkMap);
            if (bAnimated)
                continue;

            bool bUnique = true;

            for (unsigned int uiCheckIdx = 0; uiCheckIdx < uiMapSize; 
                uiCheckIdx++)
            {
                // Don't compare against yourself
                if (uiCheckIdx != uiIdx)
                {
                    NiTexturingProperty::Map* pkOtherMap = 
                        kMapArray.GetAt(uiCheckIdx);
                    
                    if (!pkOtherMap)
                        continue;

                    NIASSERT(pkOtherMap != pkMap);
                    NiTextureTransform* pkOtherTexTransform = 
                        pkOtherMap->GetTextureTransform();
                    unsigned int uiOtherUVSet = pkOtherMap->GetTextureIndex();

                    if (uiMapUVSet == uiOtherUVSet)
                    {
                        if (HasAnimatedTextureTransform(pkTexProp, pkOtherMap))
                        {
                            bUnique = false;
                            break;
                        }
                        else if ((pkOtherTexTransform != NULL && 
                            pkTexTransform == NULL) ||
                            (pkOtherTexTransform == NULL && 
                            pkTexTransform != NULL))
                        {
                            bUnique = false;
                            break;
                        }
                        else if (pkOtherTexTransform != NULL && 
                          pkTexTransform != NULL && 
                          *(pkOtherTexTransform) != *(pkTexTransform))
                        {
                            bUnique = false;
                            break;
                        }
                    }
                }
            }

            if (!bUnique)
                continue;

            // We know that we have a unique mapping, bake the UV's
            if (pkTexTransform != NULL)
            {
                NiMatrix3 kMatrix = *(pkTexTransform->GetMatrix());
                NiDataStreamElementLock kLock(pkMesh, 
                    NiCommonSemantics::TEXCOORD(), uiMapUVSet,    
                    NiDataStreamElement::F_FLOAT32_2, 
                    NiDataStream::LOCK_TOOL_READ |  
                    NiDataStream::LOCK_TOOL_WRITE); 

                // If the lock is not valid, then texture coords 
                // were not present on the mesh.
                NiBool bIsLocked = kLock.IsLocked();
                NIASSERT(bIsLocked); 

                if(!bIsLocked)
                {
                    //probably the uiMapUVset is bad
                    //just try the next one
                    continue;
                }

                NiTStridedRandomAccessIterator<NiPoint2> kBegin = 
                    kLock.begin<NiPoint2>();            
                NiTStridedRandomAccessIterator<NiPoint2> kEnd = 
                    kLock.end<NiPoint2>();

                for (NiTStridedRandomAccessIterator<NiPoint2> kIter = 
                    kBegin; kIter != kEnd; ++kIter)
                {
                    NiPoint2 kUV = *kIter;

                    NiPoint3 kPt(kUV.x,
                        kUV.y, 1.0f);
                    kPt = kMatrix * kPt;
                    kUV.x = kPt.x;
                    kUV.y = kPt.y;

                    *kIter = kUV;
                }  

                // Because the TextureTransform may have been the
                // same on multiple maps, we need to set ALL back to NULL
                for (unsigned int uk = 0; uk < uiMapSize; uk++)
                {
                    NiTexturingProperty::Map* pkTestMap = kMapArray.GetAt(uk);
                    if (pkTestMap && pkTestMap->GetTextureIndex() == 
                        uiMapUVSet)
                    {
                        NiTextureTransform* pkOldTT = 
                            pkTestMap->GetTextureTransform();
                        pkTestMap->SetTextureTransform(NULL);
                        NiDelete pkOldTT;
                    }
                }
            }
        }
    }
}