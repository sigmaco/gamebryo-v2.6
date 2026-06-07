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

#ifndef NSBD3D10STATEGROUP_H
#define NSBD3D10STATEGROUP_H

#include "NiD3D10BinaryShaderLibLibType.h"
#include "NSBD3D10StageAndSamplerStates.h"

#include <NiTPointerList.h>
#include <NiBinaryStream.h>

class NiD3D10RenderStateGroup;
class NiD3D10SamplerStateGroup;

//
class NID3D10BINARYSHADERLIB_ENTRY NSBD3D10StateGroup : public NiMemObject
{
public:
    NSBD3D10StateGroup();
    ~NSBD3D10StateGroup();

    class NID3D10BINARYSHADERLIB_ENTRY NSBD3D10SGEntry : public NiMemObject
    {
    public:
        enum
        {
            NSBSG_SAVE        = 0x00000001,
            NSBSG_ATTRIBUTE   = 0x00000002,
            NSBSG_USEMAPVALUE = 0x00000004
        };

        NSBD3D10SGEntry();
        ~NSBD3D10SGEntry();

        inline unsigned int GetFlags() const;
        inline bool IsSaved() const;
        inline bool UsesAttribute() const;
        inline bool UsesMapValue() const;
        inline unsigned int GetState() const;
        inline unsigned int GetValue() const;
        inline const char* GetAttribute() inline const;

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
    NSBD3D10SGEntry* GetFirstState();
    NSBD3D10SGEntry* GetNextState();

    bool SetupRenderStateGroup(NiD3D10RenderStateGroup& kRSGroup);
    bool SetupTextureSamplerGroup(NiD3D10RenderStateGroup& kRSGroup,
        unsigned int uiStage);

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
    void DumpEntryAsUnknown(FILE* pf, NSBD3D10SGEntry* pkEntry);
    void DumpEntryAsRenderState(FILE* pf, NSBD3D10SGEntry* pkEntry);
    void DumpEntryAsSamplerState(FILE* pf, NSBD3D10SGEntry* pkEntry);
#endif  //#if defined(NIDEBUG)
    // ***

protected:
    NSBD3D10SGEntry* FindStateInList(unsigned int uiState);

    bool SetupRenderStateGroup_D3D10(NiD3D10RenderStateGroup& kRSGroup);
    bool SetupTextureSamplerGroup_D3D10(NiD3D10RenderStateGroup& kRSGroup,
        unsigned int uiStage);

    NiTListIterator m_kStateIter;
    NiTPointerList<NSBD3D10SGEntry*> m_kStateList;
};

#include "NSBD3D10StateGroup.inl"

#endif  //NSBD3D10STATEGROUP_H
