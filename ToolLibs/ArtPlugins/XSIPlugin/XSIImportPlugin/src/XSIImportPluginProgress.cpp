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

#include "XSIImportPluginProgress.h"

#include "NiProgressDialog.h"

//---------------------------------------------------------------------------
// External references.
//---------------------------------------------------------------------------

class NIPLUGINTOOLKIT_ENTRY NiProgressDialog;

namespace
{
    static const int PROGRESS_RANGE_SPAN = 1000000;
}

namespace epg
{
    //---------------------------------------------------------------------------
    // XSI import plugin progress monitor.
    //---------------------------------------------------------------------------
    XSIImportProgress::XSIImportProgress()
        : m_pProgressDialog(0)
        , m_CurrentPhase(CONVERT_IMAGES_PHASE)
        , m_CompletionFraction(-1.0f)
    {
        m_pProgressDialog = NiNew NiProgressDialog("XSI Conversion to Gamebryo");
        m_pProgressDialog->Create();
        m_pProgressDialog->SetLineOne("Converting Crosswalk file to Gamebryo format...");
        m_pProgressDialog->SetRangeSpan(PROGRESS_RANGE_SPAN);
        m_pProgressDialog->SetPosition(0);
    }

    //---------------------------------------------------------------------------
    // Delete the dialog.
    //---------------------------------------------------------------------------
    XSIImportProgress::~XSIImportProgress()
    {
        m_pProgressDialog->Destroy();
        NiDelete m_pProgressDialog;
        m_pProgressDialog = 0;
    }

    //---------------------------------------------------------------------------
    // Start a phase.
    //---------------------------------------------------------------------------
    void XSIImportProgress::StartPhase(ProgressPhase in_Phase)
    {
        m_CurrentPhase = in_Phase;

        const char* phaseName;
        switch (in_Phase)
        {
            case CONVERT_IMAGES_PHASE:      phaseName = "Converting images..."; break;
            case CONVERT_MATERIALS_PHASE:   phaseName = "Converting materials..."; break;
            case CONVERT_ENVELOPES_PHASE:   phaseName = "Converting envelopes..."; break;
            case CONVERT_MODELS_PHASE:      phaseName = "Converting models..."; break;
            case FILL_SKINS_PHASE:          phaseName = "Filling skins..."; break;
            case APPLY_EFFECTS_PHASE:       phaseName = "Applying dynamic effects..."; break;
            case APPLY_ANIMS_PHASE:         phaseName = "Applying animations..."; break;
            default:                        phaseName = "Converting..."; break;
        }

        m_pProgressDialog->SetLineTwo(phaseName);
        m_CompletionFraction = -1.0f;
        PhaseProgress(0.0f);
    }

    //---------------------------------------------------------------------------
    // End a phase.
    //---------------------------------------------------------------------------
    void XSIImportProgress::EndPhase(ProgressPhase in_Phase)
    {
        // Make sure the progrss bar always completes.
        PhaseProgress(1.0f);
    }

    //---------------------------------------------------------------------------
    // Report progress of the current phase, from 0 to 1.
    //---------------------------------------------------------------------------
    void XSIImportProgress::PhaseProgress(float in_CompletionFraction)
    {
        if (in_CompletionFraction > 1.0f)
            in_CompletionFraction = 1.0f;
        if (in_CompletionFraction < 0.0f)
            in_CompletionFraction = 0.0f;

        // Avoid too-frequent updates, might slow down processing.
        static const float MINIMUM_DELTA_FOR_UPDATE = 0.001f;
        if (fabs(m_CompletionFraction - in_CompletionFraction) > MINIMUM_DELTA_FOR_UPDATE)
        {
            m_CompletionFraction = in_CompletionFraction;

            // The total progress is equal to the number of completed phase
            // plus the currnt progress in the current phase.
            m_pProgressDialog->SetPosition((unsigned int)
                ((m_CurrentPhase + in_CompletionFraction)
                * PROGRESS_RANGE_SPAN / PROGRESS_PHASE_COUNT));
        }
    }

    //---------------------------------------------------------------------------
}
