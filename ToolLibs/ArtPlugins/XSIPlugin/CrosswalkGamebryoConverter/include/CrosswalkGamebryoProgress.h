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

#ifndef XSI_GAMEBRYO_CONVERTER_PROGRESS_H
#define XSI_GAMEBRYO_CONVERTER_PROGRESS_H

namespace epg
{
    //---------------------------------------------------------------------------
    /// The different phase of the conversion.
    //---------------------------------------------------------------------------

    enum ProgressPhase
    {
        CONVERT_IMAGES_PHASE,
        CONVERT_MATERIALS_PHASE,
        CONVERT_ENVELOPES_PHASE,
        CONVERT_MODELS_PHASE,
        FILL_SKINS_PHASE,
        APPLY_EFFECTS_PHASE,
        APPLY_ANIMS_PHASE,
        PROGRESS_PHASE_COUNT
    };

    //---------------------------------------------------------------------------
    /// Progress monitor interface.
    //---------------------------------------------------------------------------

    class ProgressMonitor
    {
    public:
        /// Create an empty progress monitor.
        ProgressMonitor();

        /// Destroy a progress monitor.
        virtual ~ProgressMonitor();

        /// Start a phase.
        virtual void StartPhase(ProgressPhase in_Phase) = 0;

        /// End a phase.
        virtual void EndPhase(ProgressPhase in_Phase) = 0;

        /// Report progress of the current phase, from 0 to 1.
        virtual void PhaseProgress(float in_CompletionFraction) = 0;

    private:
        // Disable copying.
        ProgressMonitor(const ProgressMonitor &);
        void operator=(const ProgressMonitor &);
    };

    //---------------------------------------------------------------------------
    /// Phase helper: use RAII to start and end a phase.
    //---------------------------------------------------------------------------
    class Phaser
    {
    public:
        /// Start the given phase.
        Phaser(ProgressMonitor& in_Progress, ProgressPhase in_Phase, int in_Count)
            : m_Progress(in_Progress)
            , m_Phase(in_Phase)
            , m_Count(in_Count > 0 ? in_Count : 1)
            , m_Current(0)
        {
            m_Progress.StartPhase(m_Phase);
        }

        /// End the given phase.
        ~Phaser()
        {
            m_Progress.PhaseProgress(1.0f);
            m_Progress.EndPhase(m_Phase);
        }

        /// Progress.
        void Progress()
        {
            m_Progress.PhaseProgress(m_Current++ * 1.0f / m_Count);
        }

    private:
        ProgressMonitor&    m_Progress;
        ProgressPhase       m_Phase;
        int                 m_Count;
        int                 m_Current;
    };
}

#endif /* XSI_GAMEBRYO_CONVERTER_PROGRESS_H */
