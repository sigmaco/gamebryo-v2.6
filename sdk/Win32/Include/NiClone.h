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

#ifndef NICLONE_H
#define NICLONE_H

// The macro NiDeclareClone goes in each NiObject-derived class that
// supports cloning. The typical implementation of cloning is
//
//     NiObject* NiDerivedClass::CreateClone()
//     {
//         NiDerivedClass* pClone = new NiDerivedClass;
//         CopyMembers(pClone);
//         <any additional processing goes here>;
//         return pClone;
//     }
//
// CopyMembers does a member-by-member copy from 'this' to 'pDest'.

class NiCloningProcess;

#define NiDeclareClone(classname) \
    public: \
        virtual NiObject* CreateClone (NiCloningProcess& kCloning); \
    protected: \
        void CopyMembers (classname* pDest, NiCloningProcess& kCloning)

// This version is used by abstract classes. Only difference is absence of
// CreateClone declaration.

#define NiDeclareAbstractClone(classname) \
    protected: \
        void CopyMembers (classname* pDest, NiCloningProcess& kCloning)

// macros for creating a object
#define NiImplementCreateClone(classname) \
    NiObject* classname::CreateClone(NiCloningProcess& kCloning) \
    { \
        classname* pkObject = NiNew classname; \
        NIASSERT(pkObject != NULL); \
        CopyMembers(pkObject, kCloning); \
        return pkObject; \
    }     

#endif // NICLONE_H
