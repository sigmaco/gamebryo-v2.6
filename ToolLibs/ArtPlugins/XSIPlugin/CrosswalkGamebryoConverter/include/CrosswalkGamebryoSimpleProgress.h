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

#ifndef XSI_GAMEBRYO_CONVERTER_SIMPLE_PROGRESS_H
#define XSI_GAMEBRYO_CONVERTER_SIMPLE_PROGRESS_H

#include "CrosswalkGamebryoProgress.h"

namespace epg
{
    //---------------------------------------------------------------------------
    // Do-nothing progress monitor.
    //---------------------------------------------------------------------------
    class DoNothingProgressMonitor : public ProgressMonitor
    {
    public:
        void StartPhase(ProgressPhase);
        void EndPhase(ProgressPhase);
        void PhaseProgress(float);
    };

    //---------------------------------------------------------------------------
    /// Simple implementation of progress interface, prints to stdout.
    //---------------------------------------------------------------------------

    class StdoutProgress : public ProgressMonitor
    {
    public:
        StdoutProgress(bool in_BeQuiet);

        /// Progress API.
        void StartPhase(ProgressPhase in_Phase);
        void EndPhase(ProgressPhase in_Phase);
        void PhaseProgress(float in_CompletionFraction);

    private:
        bool                m_BeQuiet;
        ProgressPhase       m_Phase;
        int                 m_PercentDone;
        int                 m_TenthPercentDone;
    };
}

#endif /* XSI_GAMEBRYO_CONVERTER_SIMPLE_PROGRESS_H */
