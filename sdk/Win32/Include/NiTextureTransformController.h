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

#ifndef NITEXTURETRANSFORMCONTROLLER_H
#define NITEXTURETRANSFORMCONTROLLER_H

#include <NiTexturingProperty.h>
#include <NiFloatInterpController.h>

NiSmartPointer(NiTextureTransformController);

class NIANIMATION_ENTRY NiTextureTransformController :
    public NiFloatInterpController
{
    NiDeclareRTTI;
    NiDeclareClone(NiTextureTransformController);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:

    enum TransformMember {
        TT_TRANSLATE_U = 0,
        TT_TRANSLATE_V,
        TT_ROTATE,
        TT_SCALE_U,
        TT_SCALE_V
    };

    NiTextureTransformController( NiTexturingProperty* pkTarget = 0,
        NiTexturingProperty::Map* pkMap = 0, 
        TransformMember eMember = TT_TRANSLATE_U);

    virtual ~NiTextureTransformController();
    
    virtual void Update(float fTime);
    virtual bool UpdateValue(float fTime, float fFloat, 
        unsigned short usIndex = 0);

    inline NiTexturingProperty::Map* GetMap();
    bool SetMap(NiTexturingProperty::Map* pkMap,
        NiTexturingProperty* pkTarget = 0);


    TransformMember GetAffectedMember();
    inline void SetAffectedMember(TransformMember eMember);

    // Override default
    virtual void SetTarget(NiObjectNET* pkTarget);

    // *** begin Emergent internal use only ***
    virtual const char* GetCtlrID();
    // *** begin Emergent internal use only ***

protected:

    unsigned int m_uiLastIdx;
    
    NiTexturingProperty::Map* m_pkMap;
    bool m_bShaderMap;
    unsigned int m_uiMapIndex;
    TransformMember m_eMember;
    char* m_pcCtlrID;
    
    // Set and internal map pointer for performance
    inline bool SetInternalMap();

    // Virtual function overrides from base classes.
    virtual bool InterpTargetIsCorrectType(NiObjectNET* pkTarget) const;
    virtual void GetTargetFloatValue(float& fValue);

    void ResetCtlrID();
};

#include "NiTextureTransformController.inl"

#endif  // #ifndef NITEXTURETRANSFORMCONTROLLER_H
