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

#ifndef NIMAXUV_H
#define NIMAXUV_H

#include "NiMAX.h"
#include <NiTArray.h>
#include <NiTextureTransformController.h>

#define MAX_TEX_TRANSFORM_TYPE 5
class NiMAXUV : public NiMemObject
{
    // An NiMAXUV object keeps track of each MAX mapping channel
    // in use by some MAX material.  m_kChannels grows
    // as required.
    
    class Channel : public NiRefObject
    {
        // A Channel object keeps track of how a single mapping
        // channel is used. 
    public:
        Channel();
        Channel(
            Texmap* pkTm,
            UVGen* pkUVGen,
            TimeValue kAnimStart, 
            int iAttrId,
            int iMaxChannel);
        Channel(Texmap* pkTm,
            UVGen* pkUVGen,
            IParamBlock2 * pBlock, 
            TimeValue kAnimStart,
            int iAttrId, 
            int iMaxChannel);

        class TexmapInfo : public NiRefObject
        {
        public:
            NiPoint2 m_kUVScale;
            NiPoint2 m_kUVOffset;
            float m_fWAngle;
            Texmap* m_pkTexmap;
            NiTextureTransformControllerPtr 
                m_spControllers[MAX_TEX_TRANSFORM_TYPE];

            void GenerateTextureTransformControllers(StdUVGen* pkUVGen);

        };

        TexmapInfo* GetTexmapInfo(Texmap* pkMap);

        typedef NiPointer<TexmapInfo> TexmapInfoPtr;

        short m_sMAXtoNIchannel; // MAX channel index to NI index
        // Filled in by geometry converter.
        NiTObjectArray<TexmapInfoPtr> m_kTexmaps; // Texmap's for this channel.
        // Used to link Ni texture UV sets  to Channels

        int m_iMaxChannel; //a MAX map channel for which this UV is valid.
        //There can be multiple assignments to this variable, 
        // but we need only have one valid
        //value in this variable when we reach the mesh converter.

        
        unsigned short m_ausOrder[2];
        NiPoint2 m_kUVCropOffset;
        NiPoint2 m_kUVCropDimensions;

        bool IsEqual(Channel* pOther);
        bool CanShareChannelInfo(Channel* pOther);
        bool MergeChannelInfo(Channel* pOther);

        int m_iAttrId;    // multi/sub object id
    };
    
    typedef NiPointer<Channel> ChannelPtr;

public:
    NiMAXUV();
    NiMAXUV(TimeValue kAnimStart);

    void Convert(Texmap* pkTm);

    short GetMAXtoNI(Texmap* pkTexmap);
    short GetMAXChannel(Texmap* pkTexmap);

    inline void SetMAXtoNI(unsigned int uiMAXChannel, short sNIChannel);
    inline bool GetUseVCForUV();
    inline bool IsChannel(unsigned int uiIndex); 
    // Does this Ni Channel have the correct AttrId?
    void AssignUVControllers(
        NiTexturingProperty* pkProperty, 
        NiTexturingProperty::Map* pkMap);
    int GetEquivalentChannelIndex(NiMAXUV::Channel* pOther);
    inline int GetNumberOfChannels();
    inline int GetMAXChannelForNiChannelAt(unsigned int uiIndex);
    
    inline int GetAttrId() const;
    inline void SetAttrId(int iAttrId);

    bool HasAnimatedTransforms(unsigned int uiMAXChannel, Texmap* pkMap);
    bool HasIdentityTransforms(unsigned int uiMAXChannel, Texmap* pkMap);

    inline unsigned short* GetOrder(unsigned int uiMAXChannel);
    inline NiPoint2    GetTiling(unsigned int uiMAXChannel, Texmap* pkMap);
    inline NiPoint2    GetOffset(unsigned int uiMAXChannel, Texmap* pkMap);
    inline NiPoint2    GetCropOffset(unsigned int uiMAXChannel);
    inline NiPoint2    GetCropDimensions(unsigned int uiMAXChannel);
    inline float       GetWAngle( unsigned int uiMAXChannel, Texmap* pkMap);

    NiTextureTransform* CreateTextureTransform(
        unsigned int uiMAXChannel,
        Texmap* pkMap);

    void AssignMapChannel(
        NiTexturingProperty::Map*& pkMap,
        NiFlipController*& pkFlipper,
        NiTObjectArray<NiTextureTransformControllerPtr>& kTTControllers,
        Texmap* pkTexmap, bool bAutoDelete = true);

    void AssignTextureTransformControllers(
        NiTObjectArray<NiTextureTransformControllerPtr>& kTTControllers,
        NiTexturingProperty* pkProperty, 
        NiTexturingProperty::Map* pkTargetMap);

    void GetTextureTransformControllers(
        unsigned int uiMAXChannel,
        Texmap* pkMap,
        NiTObjectArray<NiTextureTransformControllerPtr>& kTTControllers);

    static void BakeTransformsIntoGeometry(
        NiMesh* pkMesh, 
        NiTexturingProperty* pkProp);

private:
    static bool HasAnimatedTextureTransform(
        NiTexturingProperty* pkProperty,
        NiTexturingProperty::Map* pkMap);

    NiTObjectArray<ChannelPtr> m_kChannels;
    bool m_bUseVCForUV;                 // use channel 0 as VCs or UVs
    TimeValue m_kAnimStart;
    int m_iAttrId;

};

#include "NiMAXUV.inl"

#endif // NIMAXUV_H
