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

#ifndef NISCRIPTINFOSET_H
#define NISCRIPTINFOSET_H

#include "NiPluginToolkitLibType.h"
#include <NiRefObject.h>
#include "NiPluginToolkitDefinitions.h"
#include <NiRTTI.h>
#include <NiSmartPointer.h>
#include <NiTPtrSet.h>
#include "NiScriptInfo.h"

class NIPLUGINTOOLKIT_ENTRY NiScriptInfoSet : public NiRefObject
{
    public:
        /// Default constructor. Set bIgnoreMissingPlugins to false
        /// to allow scripts that reference plugins that the PluginManager
        /// knows nothing about
        NiScriptInfoSet(bool bIgnoreMissingPlugins = true);

        /// Add a NiPlugin class to the ignore list. This is useful
        /// for not adding scripts of a particular class to the list
        void IgnorePluginClass(const NiRTTI* pkRTTI);
        void IgnorePluginClass(const char* pcClassName);

        void IgnoreMissingPlugins(bool bIgnore = true);

        bool IsIgnoringMissingPlugins();

        /// Add a script to the list of known scripts, 
        ReturnCode AddScript(NiScriptInfo* pkInfo, bool bTemplate = false, 
            bool bSilentRunning = false);

        /// Determine if this script is a template
        /// Remove a script by name
        NiScriptInfoPtr RemoveScript(const char* pcScript);

        /// Remove All Scripts from this set
        void RemoveAllScripts();

        /// Get a script info by name
        NiScriptInfo* GetScript(const char* pcName);

        /// Get the number of known scripts
        unsigned int GetScriptCount();

        /// Get the script at the position ui
        NiScriptInfo* GetScriptAt(unsigned int ui);

        /// Get the index of the script by the given name
        unsigned int GetScriptIndex(const char* pcName);

        /// Copy the members of the input NiScriptInfoSet to
        /// this NiScriptInfoSet
        void Copy(NiScriptInfoSet* pkSet);

        /// Clone this NiScriptInfoSet
        NiScriptInfoSet* Clone();

        /// Is this script a template?
        bool IsTemplate(NiScriptInfo* pkInfo);

        void SetTemplate(NiScriptInfo* pkInfo, bool bTemplate);

        /// This method uses the name of the input script to locate
        /// the script it needs to replace. If a script of the same name 
        /// is not present, it adds to the end of this set. 
        /// Otherwise, it copies over the script in the set.
        NiScriptInfoPtr ReplaceScript(NiScriptInfo* pkNewScript);

        /// Does this script use unknown plugins or 
        /// classes ignored by this set?
        bool IgnoreScript(NiScriptInfo* pkInfo);
    
    protected:
        NiScriptInfoSet(NiScriptInfoSet&){};

        NiTPtrSet<NiScriptInfoPtr, NiTNewInterface<NiScriptInfoPtr> >
            m_kAllScripts;
        NiTPtrSet<bool, NiTMallocInterface<bool> > m_kAllScriptsTemplateFlags;
        NiTPtrSet<const char*, NiTMallocInterface<const char*> >
            m_kIgnoreClasses;
        bool m_bIgnoreMissingPlugins;
};

NiSmartPointer(NiScriptInfoSet);

#endif
