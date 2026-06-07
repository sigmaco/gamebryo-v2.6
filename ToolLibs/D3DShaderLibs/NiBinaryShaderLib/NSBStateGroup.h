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

#ifndef NSBSTATEGROUP_H
#define NSBSTATEGROUP_H

#include "NiBinaryShaderLibLibType.h"
#include "NSBStageAndSamplerStates.h"
#include <NiTPointerList.h>
#include <NiBinaryStream.h>

class NiD3DRenderStateGroup;
class NiD3DTextureStageGroup;

//
class NIBINARYSHADERLIB_ENTRY NSBStateGroup : public NiMemObject
{
public:
    NSBStateGroup();
    ~NSBStateGroup();

    class NIBINARYSHADERLIB_ENTRY NSBSGEntry : public NiMemObject
    {
    public:
        enum
        {
            NSBSG_SAVE        = 0x00000001,
            NSBSG_ATTRIBUTE   = 0x00000002,
            NSBSG_USEMAPVALUE = 0x00000004
        };

        NSBSGEntry();
        ~NSBSGEntry();

        inline unsigned int GetFlags() const;
        inline bool IsSaved() const;
        inline bool UsesAttribute() const;
        inline bool UsesMapValue() const;
        inline unsigned int GetState() const;
        inline unsigned int GetValue() const;
        inline const char* GetAttribute() const;

        inline void SetFlags(unsigned int uiFlags);
        inline void SetSaved(bool bSave);
        inline void SetUseAttribute(bool bUse);
        inline void SetUseMapValue(bool bUse);
        inline void SetState(unsigned int uiState);
        inline void SetValue(unsigned int uiValue);
        void SetAttribute(const char* pcAttribute);

        bool SaveBinary(NiBinaryStream& kStream);
        bool LoadBinary(NiBinaryStream& kStream);

    protected:
        unsigned int m_uiFlags;
        unsigned int m_uiState;
        unsigned int m_uiValue;
        char* m_pcAttribute;
    };

    void SetState(unsigned int uiState, unsigned int uiValue, bool bSave,
        bool bUseMapValue = false);
    void SetState(unsigned int uiState, const char* pcAttribute, 
        bool bSave, bool bUseMapValue = false);

    unsigned int GetStateCount();
    NSBSGEntry* GetFirstState();
    NSBSGEntry* GetNextState();

    bool SetupRenderStateGroup(NiD3DRenderStateGroup& kRSGroup);
    bool SetupTextureStageGroup(NiD3DTextureStageGroup& kTSGroup);
    bool SetupTextureSamplerGroup(NiD3DTextureStageGroup& kTSGroup);

    // ***
    bool SaveBinary(NiBinaryStream& kStream);
    bool LoadBinary(NiBinaryStream& kStream);

#if defined(NIDEBUG)
    enum DumpMode
    {
        DUMP_RENDERSTATES = 0,
        DUMP_STAGESTATES,
        DUMP_SAMPLERSTATES
    };
    void Dump(FILE* pf, DumpMode eMode);
    void DumpEntryAsUnknown(FILE* pf, NSBSGEntry* pkEntry);
    void DumpEntryAsRenderState(FILE* pf, NSBSGEntry* pkEntry);
    void DumpEntryAsStageState(FILE* pf, NSBSGEntry* pkEntry);
    void DumpEntryAsSamplerState(FILE* pf, NSBSGEntry* pkEntry);
#endif  //#if defined(NIDEBUG)
    // ***

protected:
    NSBSGEntry* FindStateInList(unsigned int uiState);

    NiTListIterator m_kStateIter;
    NiTPointerList<NSBSGEntry*> m_kStateList;
};

#include "NSBStateGroup.inl"

#endif  //NSBSTATEGROUP_H
