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

#include "CrosswalkGamebryoSimpleProgress.h"

#include <stdio.h>
#include <math.h>

namespace epg
{
    //---------------------------------------------------------------------------
    // Progress Monitor.
    //---------------------------------------------------------------------------
    ProgressMonitor::ProgressMonitor()
    {
    }

    ProgressMonitor::~ProgressMonitor()
    {
    }

    //---------------------------------------------------------------------------
    // Do-nothing progress monitor.
    //---------------------------------------------------------------------------
    void DoNothingProgressMonitor::StartPhase(ProgressPhase)
    {
        // Do nothing.
    }

    void DoNothingProgressMonitor::EndPhase(ProgressPhase)
    {
        // Do nothing.
    }

    void DoNothingProgressMonitor::PhaseProgress(float)
    {
        // Do nothing.
    }

    //---------------------------------------------------------------------------
    /// Simple implementation of progress interface, prints to stdout.
    //---------------------------------------------------------------------------

    StdoutProgress::StdoutProgress(bool in_BeQuiet)
        : m_BeQuiet(in_BeQuiet)
        , m_Phase(PROGRESS_PHASE_COUNT)
        , m_PercentDone(-1)
        , m_TenthPercentDone(-1)
    {
    }

    void StdoutProgress::StartPhase(ProgressPhase in_Phase)
    {
        if (m_BeQuiet)
            return;

        m_Phase            = in_Phase;
        m_PercentDone      = -1;
        m_TenthPercentDone = -1;

        PhaseProgress(0.0f);
    }

    void StdoutProgress::EndPhase(ProgressPhase in_Phase)
    {
        if (m_BeQuiet)
            return;

        if (in_Phase == m_Phase)
        {
            PhaseProgress(1.0f);
            puts("");

            m_Phase = PROGRESS_PHASE_COUNT;
        }
    }

    void StdoutProgress::PhaseProgress(float in_CompletionFraction)
    {
        if (m_BeQuiet)
            return;

        if (in_CompletionFraction > 1.0f)
            in_CompletionFraction = 1.0f;
        if (in_CompletionFraction < 0.0f)
            in_CompletionFraction = 0.0f;

        int percentDone      = (int) ::floor(in_CompletionFraction * 100);
        int tenthPercentDone = (int) ::floor(in_CompletionFraction * 1000) - percentDone * 10;
        if (percentDone != m_PercentDone || tenthPercentDone != m_TenthPercentDone)
        {
            m_PercentDone      = percentDone;
            m_TenthPercentDone = tenthPercentDone;

            const char* phaseName;
            switch (m_Phase)
            {
                case CONVERT_IMAGES_PHASE:      phaseName = "Converting images"; break;
                case CONVERT_MATERIALS_PHASE:   phaseName = "Converting materials"; break;
                case CONVERT_ENVELOPES_PHASE:   phaseName = "Converting envelopes"; break;
                case CONVERT_MODELS_PHASE:      phaseName = "Converting models"; break;
                case FILL_SKINS_PHASE:          phaseName = "Filling skins"; break;
                case APPLY_EFFECTS_PHASE:       phaseName = "Applying dynamic effects"; break;
                case APPLY_ANIMS_PHASE:         phaseName = "Applying animations"; break;
                default:                        phaseName = "Converting"; break;
            }
            printf("%s: %d.%d%%\r", phaseName, percentDone, tenthPercentDone);
        }
    }

    //---------------------------------------------------------------------------
}
