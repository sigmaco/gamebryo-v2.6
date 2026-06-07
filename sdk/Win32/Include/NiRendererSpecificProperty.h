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

#ifndef NIRENDERERSPECIFICPROPERTY_H
#define NIRENDERERSPECIFICPROPERTY_H

#include "NiProperty.h"

NiSmartPointer(NiRendererSpecificProperty);

class NIMAIN_ENTRY NiRendererSpecificProperty : public NiProperty
{
    NiDeclareRTTI;
    NiDeclareClone(NiRendererSpecificProperty);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    inline NiRendererSpecificProperty();
    ~NiRendererSpecificProperty();

    
    inline bool IsEqualFast(const NiRendererSpecificProperty& kProp) const;

    virtual int Type () const;
    static int GetType ();
    static NiRendererSpecificProperty *GetDefault ();

    // *** begin Emergent internal use only ***

    static void _SDMInit();
    static void _SDMShutdown();

    // *** end Emergent internal use only ***

protected:
    static NiRendererSpecificPropertyPtr ms_spDefault;
};

typedef NiPointer<NiRendererSpecificProperty> NiRendererSpecificPropertyPtr;

#include "NiRendererSpecificProperty.inl"

#endif

