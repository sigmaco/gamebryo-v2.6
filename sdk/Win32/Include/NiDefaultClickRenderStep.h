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

#ifndef NIDEFAULTCLICKRENDERSTEP_H
#define NIDEFAULTCLICKRENDERSTEP_H

#include "NiClickRenderStep.h"

class NIMAIN_ENTRY NiDefaultClickRenderStep : public NiClickRenderStep
{
    NiDeclareRTTI;

public:
    // Functions for accessing the list of render clicks.
    inline void AppendRenderClick(NiRenderClick* pkRenderClick);
    inline void PrependRenderClick(NiRenderClick* pkRenderClick);
    inline void RemoveRenderClick(NiRenderClick* pkRenderClick);
    inline void RemoveAllRenderClicks();
    inline NiTPointerList<NiRenderClickPtr>& GetRenderClickList();
    inline const NiTPointerList<NiRenderClickPtr>& GetRenderClickList() const;
    inline NiRenderClick* GetRenderClickByName(const NiFixedString& kName) const;
    inline NiTListIterator GetRenderClickPosByName(const NiFixedString& kName) const;
    inline NiRenderClick* GetRenderClickAndPosByName(
        const NiFixedString& kName, NiTListIterator& kIter) const;

protected:
    // Function that returns a list of render clicks.
    virtual const NiTPointerList<NiRenderClickPtr>& GetRenderClicks();

    // The list of render clicks to use for this step.
    NiTPointerList<NiRenderClickPtr> m_kRenderClicks;
};

NiSmartPointer(NiDefaultClickRenderStep);

#include "NiDefaultClickRenderStep.inl"

#endif  // #ifndef NIDEFAULTCLICKRENDERSTEP_H
