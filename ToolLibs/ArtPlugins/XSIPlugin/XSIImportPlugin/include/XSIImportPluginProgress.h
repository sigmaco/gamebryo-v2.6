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

#ifndef XSI_IMPORT_PLUGIN_PROGRESS_H
#define XSI_IMPORT_PLUGIN_PROGRESS_H

#include "CrosswalkGamebryoProgress.h"

#include "NiPluginToolkitLibType.h"

//---------------------------------------------------------------------------
// External references.
//---------------------------------------------------------------------------

class NIPLUGINTOOLKIT_ENTRY NiProgressDialog;

namespace epg
{
    //---------------------------------------------------------------------------
    // XSI import plugin progress monitor.
    //---------------------------------------------------------------------------

    class XSIImportProgress : public epg::ProgressMonitor
    {
    public:
        /// Create the dialog.
        XSIImportProgress();

        /// Delete the dialog.
        ~XSIImportProgress();

        /// Start a phase.
        void StartPhase(ProgressPhase in_Phase);

        /// End a phase.
        void EndPhase(ProgressPhase in_Phase);

        /// Report progress of the current phase, from 0 to 1.
        void PhaseProgress(float in_CompletionFraction);

    private:
        NiProgressDialog*       m_pProgressDialog;
        ProgressPhase           m_CurrentPhase;
        float                   m_CompletionFraction;
    };

    //---------------------------------------------------------------------------
}

#endif /* XSI_IMPORT_PLUGIN_PROGRESS_H */
