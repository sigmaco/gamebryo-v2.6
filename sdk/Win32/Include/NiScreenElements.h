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

#ifndef NISCREENELEMENTS_H
#define NISCREENELEMENTS_H

#include "NiTriShape.h"
#include "NiScreenElementsData.h"


class NIMAIN_ENTRY NiScreenElements : public NiTriShape
{
public:
    NiDeclareRTTI;
    NiDeclareClone(NiScreenElements);
    NiDeclareStream;

public:
    /*** begin Emergent internal use only ***/

    NiScreenElements(NiScreenElementsData* pkModelData);
    virtual ~NiScreenElements();

    inline NiScreenElementsData* Data();
    inline const NiScreenElementsData* Data() const;

    /*** end Emergent internal use only ***/

protected:
    // Streaming support.
    NiScreenElements();
};

NiSmartPointer(NiScreenElements);
typedef NiTObjectArray<NiScreenElementsPtr> NiScreenElementsArray;
#include "NiScreenElements.inl"

#endif //NISCREENELEMENTS_H
