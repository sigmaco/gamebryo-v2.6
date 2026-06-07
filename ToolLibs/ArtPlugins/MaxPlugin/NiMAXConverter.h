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

#ifndef NIMAXCONVERTER_H
#define NIMAXCONVERTER_H

#include "NiMAX.h"
#include <NiProgressDialog.h>
#include <NiInfoDialog.h>
#include <NiXMLLogger.h>

class ScaleInfo : public NiMemObject
{
public:
    inline bool IsAnisotropic()
    {
        return (!bIsIdent && 
            !(NiOptimize::CloseTo(scale.s.x, scale.s.y) &&
            NiOptimize::CloseTo(scale.s.x, scale.s.z)) ||
            scale.s.x <= 0.0f);
    }
    ScaleValue scale;
    bool bIsIdent;
};

//---------------------------------------------------------------------------
class NiMAXConverter : public NiMemObject
{
public:
    NiMAXConverter();
    ~NiMAXConverter();
    
    inline void SetAnimEnd(TimeValue animEnd) { m_animEnd = animEnd; }
    inline TimeValue GetAnimEnd() { return m_animEnd; }

    inline void SetAnimStart(TimeValue animStart) { m_animStart = animStart; }
    inline TimeValue GetAnimStart() { return m_animStart; }

    inline void SetInvTicksPerSecond(float fInvTicksPerSecond)
                { m_fInvTicksPerSecond = fInvTicksPerSecond; }
    inline float GetInvTicksPerSecond() { return m_fInvTicksPerSecond; }

    inline NiColor GetBackGround() { return m_backGnd; }

    int Convert(Interface *pIntf, bool bExportSelected);

    unsigned int CalculateTotalNodeCount(INode* pkNode);
    NiNode *GetRoot() { return m_spScene; }

    inline static void SetUseAppTime(bool bUseAppTime) 
                    { m_bUseAppTime = bUseAppTime; }
    inline static bool GetUseAppTime() { return m_bUseAppTime; }

    static void SetViewerRun(bool bViewerRun);
    static bool GetViewerRun();

    static bool GetExportSelected();

    static void Init();
    static void Shutdown();

    static NiProgressDialog* GetDialog();
    
    static void LogError(const char* pcError);
    static void LogWarning(const char* pcError);
    static void ClearLog();
    static bool AreAnyErrors();
    static void ShowLog();
    static unsigned int GetErrorCount();

    static NiXMLLogger* GetXMLLogger();

private:
    static NiProgressDialog* ms_pkViewerLoaderDlg;
    static NiInfoDialog* ms_pkInfoDialog;
    static unsigned int ms_uiNumErrors;

    void ConvertBackGround(Interface *pIntf);

    void ConvertAtmospherics(Interface* pIntf, NiNode* pkScene);

    NiNodePtr m_spScene;
    NiColor m_backGnd;

    TimeValue m_animStart, m_animEnd;
    float m_fInvTicksPerSecond;

    static bool m_bUseAppTime;
    static bool ms_bViewerRun;
    static bool ms_bExportSelected;

    static void *m_pUserData;
    static NiXMLLogger ms_kLogger;
};

//---------------------------------------------------------------------------

// error codes the converter can return
#define W3D_STAT_OK             0x0000
#define W3D_STAT_FAILED         0x0001
#define W3D_STAT_NO_MEMORY      0x0002

#define W3D_STAT_MISSING_DEMO   0X0003
#define W3D_STAT_NO_RENDERER    0x0004


//---------------------------------------------------------------------------
#define ND_CB_OK 0
#define ND_CB_DONE 1
#define ND_CB_CONTINUE 2
#define ND_CB_ERROR 3

//---------------------------------------------------------------------------

#define SECONDSPERTICK (1.0f/4800.0f)
#define TICKSPERSECOND (4800.0f)
//---------------------------------------------------------------------------

#include "NiMAXConverter.inl"

#endif
