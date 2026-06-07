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
#include "MEntity.h"

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MCollectionPropertyEditor : 
        public System::ComponentModel::Design::CollectionEditor
    {
    public:
        MCollectionPropertyEditor(Type* pmCollectionType, Type* pmItemType);
        Object* EditValue(
            System::ComponentModel::ITypeDescriptorContext* pmContext, 
            System::IServiceProvider* sp, Object* pmValue);

    protected:
        virtual Type* CreateCollectionItemType();
        virtual Object* CreateInstance(System::Type* type);
    private:
        MComponent* GetComponentContainingProperty(MEntity* pmEntity,
            String* pmPropertyName);
        Type* m_pmItemType;

    };

}}}}