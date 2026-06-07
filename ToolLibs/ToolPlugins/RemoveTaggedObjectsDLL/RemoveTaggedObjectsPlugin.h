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

#ifndef REMOVETAGGEDOBJECTSPLUGIN_H
#define REMOVETAGGEDOBJECTSPLUGIN_H

class RemoveTaggedObjectsPlugin : public NiPlugin
{
public:
    /// RTTI declaration macro.
    NiDeclareRTTI;

    /// Default constructor.
    RemoveTaggedObjectsPlugin();

    //-----------------------------------------------------------------------
    // Required virtual function overrides from NiPlugin.
    //-----------------------------------------------------------------------
    /// Returns a plug-in script with default parameters.
    virtual NiPluginInfo* GetDefaultPluginInfo(); 

    /// Does this plug-in have editable options?
    virtual bool HasManagementDialog();
    
    /// Bring up the options dialog and handle user interaction.
    virtual bool DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent);

    /// Determines whether or not this plug-in can handle this plug-in info
    /// object.
    virtual bool CanExecute(NiPluginInfo* pkInfo);
    
    /// Execute the NiPluginInfo script.
    virtual NiExecutionResultPtr Execute(const NiPluginInfo* pkInfo);

protected:
    void RemoveTaggedObjects(NiAVObject* pkObject);
    bool RemoveTaggedObjects(NiNode* pkSceneRoot, 
        NiTPointerList<NiAVObject*>& kList);
    void BuildRemoveTaggedObjectsList(NiAVObject* pkObject,
        NiTPointerList<NiAVObject*>& kList, 
        const NiTPointerMap<NiAVObject*, bool>& kMap);

    void BakeKeys(NiNode* pkSceneRoot, NiNode* pkCurrentParent, 
        NiNode* pkNewParent, NiAVObject* pkChild);
};

#endif  // #ifndef REMOVEOBJECTSPLUGIN_H
