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

#ifndef NID3DTEXTURESTAGEGROUP_H
#define NID3DTEXTURESTAGEGROUP_H

#include "NiD3DDefines.h"
#include <NiTPool.h>

class NID3D_ENTRY NiD3DTextureStageGroup : public NiMemObject
{
    // *** begin Emergent internal use only ***
protected:
    static NiD3DRenderState* ms_pkD3DRenderState;

public:
    static void SetD3DRenderState(NiD3DRenderState* pkRS);

    static void InitializePools();
    static void ShutdownPools();

    // *** end Emergent internal use only ***

public:
    NiD3DTextureStageGroup();
    ~NiD3DTextureStageGroup();

    // Add and remove states
    void SetStageState(unsigned int uiState, unsigned int uiValue,
        bool bSave = false);
    void RemoveStageState(unsigned int uiState);
    bool GetStageState(unsigned int uiState, unsigned int& uiValue,
        bool& bSave);

    void SetSamplerState(unsigned int uiState, unsigned int uiValue,
        bool bSave = false, bool bUseMapValue = false);
    void RemoveSamplerState(unsigned int uiState);
    bool GetSamplerState(unsigned int uiState, unsigned int& uiValue,
        bool& bSave, bool& bUseMapValue);

    void RemoveAllStageStates();
    void RemoveAllSamplerStates();

    // Set and restore the states
    NiD3DError SetTextureStage(unsigned int uiStage);
    NiD3DError RestoreTextureStage(unsigned int uiStage);
    
    NiD3DError SetAllStageStates(unsigned int uiStage);
    NiD3DError RestoreAllStageStates(unsigned int uiStage);
    NiD3DError SetAllStageSamplers(unsigned int uiStage);
    NiD3DError RestoreAllStageSamplers(unsigned int uiStage);

    // Access to pools
    static NiD3DTextureStageGroup* GetFreeTextureStageGroup();
    static void ReleaseTextureStageGroup(NiD3DTextureStageGroup* pkGroup);

    // *** begin Emergent internal use only ***
    inline unsigned int GetTotalStageStateCount() const;
    inline unsigned int GetNoSaveStageStateCount() const;
    inline unsigned int GetSavedStageStateCount() const;

    inline unsigned int GetTotalSamplerStateCount() const;
    inline unsigned int GetNoSaveSamplerStateCount() const;
    inline unsigned int GetSavedSamplerStateCount() const;
    inline unsigned int GetUseMapValueCount() const;

    inline bool GetFirstNoSaveStageState(unsigned int& uiState, 
        unsigned int& uiValue);
    inline bool GetNextNoSaveStageState(unsigned int& uiState, 
        unsigned int& uiValue);
    inline bool GetFirstSavedStageState(unsigned int& uiState, 
        unsigned int& uiValue);
    inline bool GetNextSavedStageState(unsigned int& uiState, 
        unsigned int& uiValue);

    inline bool GetFirstNoSaveSamplerState(unsigned int& uiState, 
        unsigned int& uiValue, bool& bUseMapValue);
    inline bool GetNextNoSaveSamplerState(unsigned int& uiState, 
        unsigned int& uiValue, bool& bUseMapValue);
    inline bool GetFirstSavedSamplerState(unsigned int& uiState, 
        unsigned int& uiValue, bool& bUseMapValue);
    inline bool GetNextSavedSamplerState(unsigned int& uiState, 
        unsigned int& uiValue, bool& bUseMapValue);

    // Utility function to release all entries in the free pool.
    // This is useful for when several objects have been freed, a
    // level change has occured, etc.
    static void PurgeFreePool();
    // *** end Emergent internal use only ***

protected:
    bool m_bRendererOwned;

    static NiTObjectPool<NiD3DTextureStageGroup>* ms_pkTextureStageGroupPool;

    enum 
    {
#if defined(_XENON)
        STAGEARRAYSIZE      = D3DTSS_MAX,
//        SAMPLERARRAYSIZE    = D3DSAMP_MAX
        SAMPLERARRAYSIZE    = D3DSAMP_MAX + 2   // RGBINDEX and DMAPOFFSET
#elif defined(WIN32)
        STAGEARRAYSIZE = 33,
        SAMPLERARRAYSIZE = 14
#endif
    };

    unsigned int m_uiNumSavedStages;
    unsigned int m_uiSavedStageArrayIter;
    unsigned int m_auiSavedStageArray[STAGEARRAYSIZE];
    bool m_abSavedStageValid[STAGEARRAYSIZE];
    unsigned int m_uiNumStages;
    unsigned int m_uiStageArrayIter;
    unsigned int m_auiStageArray[STAGEARRAYSIZE];
    bool m_abStageValid[STAGEARRAYSIZE];

    unsigned int m_uiNumSavedSamplers;
    unsigned int m_uiSavedSamplerArrayIter;
    unsigned int m_auiSavedSamplerArray[SAMPLERARRAYSIZE];
    bool m_abSavedSamplerValid[SAMPLERARRAYSIZE];
    unsigned int m_uiNumSamplers;
    unsigned int m_uiSamplerArrayIter;
    unsigned int m_auiSamplerArray[SAMPLERARRAYSIZE];
    bool m_abSamplerValid[SAMPLERARRAYSIZE];
    unsigned int m_uiNumUseMapValue;
    bool m_abSamplerUseMapValue[SAMPLERARRAYSIZE];
};

#include "NiD3DTextureStageGroup.inl"

#endif  //#ifndef NID3DTEXTURESTAGEGROUP_H
