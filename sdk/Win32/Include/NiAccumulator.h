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

#ifndef NIACCUMULATOR_H
#define NIACCUMULATOR_H

#include "NiMainLibType.h"
#include "NiCullingProcess.h"
#include "NiObject.h"
#include "NiStream.h"
#include "NiTPointerList.h"

class NiRenderObject;
class NiCamera;


class NIMAIN_ENTRY NiAccumulator : public NiObject
{
    NiDeclareRTTI;
    NiDeclareAbstractClone(NiAccumulator);
    NiDeclareAbstractStream;

public:
    NiAccumulator ();
    inline virtual ~NiAccumulator () { /**/ }

    // *** begin Emergent internal use only ***

    virtual void StartAccumulating (const NiCamera* pkCamera); 
    virtual void FinishAccumulating (); 
    virtual void RegisterObjectArray (NiVisibleArray& kArray) = 0;
    bool IsAccumulating() const;

    // *** end Emergent internal use only ***

protected:
    const NiCamera* m_pkCamera;
};

// list of NiAVObjects - most likely to be sorted
typedef NiTPointerList<NiRenderObject*> NiSortedObjectList;

typedef NiPointer<NiAccumulator> NiAccumulatorPtr;

#endif

