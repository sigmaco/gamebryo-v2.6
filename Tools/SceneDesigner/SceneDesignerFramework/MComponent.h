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

#pragma once

#include "MDisposable.h"
#include "MPropertyContainer.h"

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MComponent : public MPropertyContainer
    {
    public:
        MComponent(NiEntityComponentInterface* pkComponent);

        static MComponent* CreateGeneralComponent(String* strName);

        NiEntityComponentInterface* GetNiEntityComponentInterface();
        
        __property String* get_Name();
        __property Guid get_TemplateID();
        __property void set_TemplateID(Guid mID);
        __property virtual NiEntityPropertyInterface* get_PropertyInterface();
        MComponent* Clone(bool bInheritProperties);

        __property MComponent* get_MasterComponent();
        __property void set_MasterComponent(MComponent* pmComponent);
        __property MComponent* get_RootMasterComponent();

        bool IsAddOrRemovePropertySupported();
        void AddProperty(String* strPropertyName, String* strDisplayName,
            String* strPrimitiveType, String* strSemanticType,
            String* strDescription, bool bUndoable);
        void AddProperty(String* strPropertyName, String* strDisplayName,
            String* strPrimitiveType, String* strSemanticType, 
            bool bCollection, String* strDescription, bool bUndoable);
        void RemoveProperty(String* strPropertyName, bool bUndoable);
        String* GetDependentPropertyNames()[];

        String* GetDescription(String* strPropertyName);
        bool SetDescription(String* strPropertyName, String* strDescription);

    private:
        NiEntityComponentInterface* m_pkComponent;
        void AddRemoveProperty(String* strPropertyName, String* strDisplayName,
            String* strPrimitiveType, String* strSemanticType, 
            bool bCollection, String* strDescription, bool bUndoable, 
            bool bAdd);

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);
    };
}}}}
