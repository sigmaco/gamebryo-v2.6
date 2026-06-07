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

#ifndef NIPHYSXMESHCONVERTERPLUGIN_H
#define NIPHYSXMESHCONVERTERPLUGIN_H


class NiPhysXMeshConverterPlugin : public NiPlugin
{
public:
    /// RTTI declaration macro.
    NiDeclareRTTI;

    /// Default constructor.
    NiPhysXMeshConverterPlugin();
    
    /// Destructor.
    ~NiPhysXMeshConverterPlugin();

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
    // Work through all the meshes in a single prop
    bool ProcessProp(NiPhysXProp* pkProp,
        NxCookingInterface* pkCookingInterface, NiXMLLogger* pkLogger);
    
    // Process an actor
    bool ProcessActor(NiPhysXActorDesc* pkActorDesc,
        NxCookingInterface* pkCookingInterface, NiXMLLogger* pkLogger);

    // Process a specific mesh
    bool ConvertMesh(NiPhysXMeshDesc* pkMeshDesc,
        NxCookingInterface* pkCookingInterface, NiXMLLogger* pkLogger);
    
    // Process a cloth mesh
    bool ConvertCloth(NiPhysXMeshDesc* pkMeshDesc, NiXMLLogger* pkLogger);
    
    // Map holding meshes that have already been converted
    NiTMap<NiPhysXMeshDesc*, bool> m_kConvertedMap;
    
};

#endif  // #ifndef NIPHYSXMESHCONVERTERPLUGIN_H
