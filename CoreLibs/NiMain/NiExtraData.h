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

#ifndef NIEXTRADATA_H
#define NIEXTRADATA_H

#include "NiObject.h"
#include "NiStream.h"
#include "NiFixedString.h"
NiSmartPointer(NiExtraData);


class NIMAIN_ENTRY NiExtraData : public NiObject
{
    NiDeclareRTTI;
    //NiDeclareClone(NiExtraData);
    NiDeclareAbstractClone(NiExtraData);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    // construction and destruction
    NiExtraData();  // support for streaming
    NiExtraData(const NiFixedString& kName);
    virtual ~NiExtraData ();

    const NiFixedString& GetName() const;
    void SetName(const NiFixedString& kName);   // Used for cloning.

    // The following members dictate whether or not the extra data should be
    // streamed or cloned which can be useful in the case of extra data that
    // holds temporary values during execution.
    virtual bool IsStreamable() const;
    virtual bool IsCloneable() const;

protected:
    // Every extra data instance has a unique name; 
    // if one is not provided, a unique name will be generated.
    NiFixedString m_kName; 
};

NiSmartPointer(NiExtraData);

#endif  // #ifndef NIEXTRADATA_H
