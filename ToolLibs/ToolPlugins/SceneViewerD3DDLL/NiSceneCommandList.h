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

#ifndef NISCENECOMMANDLIST_H
#define NISCENECOMMANDLIST_H

#include "NiSceneCommandInfo.h"

class NiConfigurableUIMapWriter;

class NiSceneCommandList : public NiMemObject
{
public:
    NiSceneCommandList();
    ~NiSceneCommandList();
    unsigned int GetSize();
    int Add(NiSceneCommandInfo* pkInfo);
    bool Remove(unsigned int uiIndex);
    bool RemoveAll();
    NiSceneCommandInfo* GetAt(unsigned int Index);
    void SetAt(unsigned int uiIndex, NiSceneCommandInfo* pkInfo);
    NiTList<NiSceneCommandInfo*>* GetList();
    void PrintDebug();
protected:
    NiTList<NiSceneCommandInfo*>* m_pkList;
};


#endif