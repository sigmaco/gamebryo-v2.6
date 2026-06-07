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

#ifndef NIBACKTOFRONTACCUMULATOR_H
#define NIBACKTOFRONTACCUMULATOR_H

#include "NiAccumulator.h"
#include "NiCamera.h"
#include "NiRenderObject.h"

class NIMAIN_ENTRY NiBackToFrontAccumulator : public NiAccumulator
{
    NiDeclareRTTI;
    NiDeclareClone(NiBackToFrontAccumulator);
    NiDeclareStream;

public:
    // creation and destruction
    NiBackToFrontAccumulator();
    virtual ~NiBackToFrontAccumulator();

    // *** begin Emergent internal use only ***

    virtual void RegisterObjectArray (NiVisibleArray& kArray);
    virtual void FinishAccumulating();

    // *** end Emergent internal use only ***

protected:
    virtual void Sort();

    void SortObjectsByDepth(int l, int r);
    float ChoosePivot(int l, int r) const;

    NiSortedObjectList m_kItems;

    int m_iNumItems;
    int m_iMaxItems;
    NiRenderObject** m_ppkItems;
    float* m_pfDepths;
    int m_iCurrItem;
};

NiSmartPointer(NiBackToFrontAccumulator);

#endif //NIBACKTOFRONTACCUMULATOR_H

