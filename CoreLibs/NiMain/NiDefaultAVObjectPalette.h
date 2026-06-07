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

#ifndef NIDEFAULTAVOBJECTPALETTE_H
#define NIDEFAULTAVOBJECTPALETTE_H

#include "NiAVObjectPalette.h"

class NIMAIN_ENTRY NiDefaultAVObjectPalette : public NiAVObjectPalette
{
    NiDeclareRTTI;
    NiDeclareClone(NiDefaultAVObjectPalette);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiDefaultAVObjectPalette(NiAVObject* pkScene = NULL);
    inline ~NiDefaultAVObjectPalette() { /* */ }

    virtual NiAVObject* GetAVObject(const char* pcName);
    virtual void SetAVObject(const char* pcName, NiAVObject* pkObj);

    virtual void SetScene(NiAVObject* pkScene);
    virtual NiAVObject* GetScene();

    // *** begin Emergent internal use only ***
    virtual void ProcessClone(NiCloningProcess& kCloning);
    // *** end Emergent internal use only ***

protected:
    NiTStringPointerMap<NiAVObject*> m_kHash;
    NiAVObject* m_pkScene;
};

#include "NiDefaultAVObjectPalette.inl"

#endif
