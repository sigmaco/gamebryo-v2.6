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

#ifndef NIFLIPCONTROLLER_H
#define NIFLIPCONTROLLER_H

#include <NiTexturingProperty.h>
#include <NiFloatInterpController.h>
#include "NiAnimationLibType.h"

NiSmartPointer(NiFlipController);

class NIANIMATION_ENTRY NiFlipController : public NiFloatInterpController
{
    NiDeclareRTTI;
    NiDeclareClone(NiFlipController);
    NiDeclareStream;
 
public:
    NiFlipController();
    ~NiFlipController();

    virtual void Update(float fTime);
    virtual bool UpdateValue(float fTime, float fFloat, 
        unsigned short usIndex = 0);
    virtual bool UpdateValue(float fTime, 
        NiControllerSequence* pkControllerSequence, 
        NiEvaluator* pkEvaluator, 
        NiEvaluatorSPData** ppkEvalSPData,
        unsigned short usIndex = 0);

    bool SetAffectedMap(NiTexturingProperty::Map* pkMap);
    NiTexturingProperty::Map* GetAffectedMap(void) const;

    void AttachTexture(NiTexture* pkTexture, unsigned int uiIndex);
    NiTexturePtr DetachTexture(unsigned int uiIndex);

    inline unsigned int GetIndex() const;
    inline unsigned int GetTextureArraySize() const;

    NiTexture* GetTextureAt(unsigned int uiIndex);
    inline NiTexture* GetTexture() const;

    // *** begin Emergent internal use only ***
    virtual const char* GetCtlrID();
    
    virtual NiBlendInterpolator* CreateBlendInterpolator(
        unsigned short usIndex = 0, bool bManagerControlled = false, 
        float fWeightThreshold = 0.0f, unsigned char ucArraySize = 2) 
        const;

    // *** begin Emergent internal use only ***

protected:
    // Virtual function overrides from base classes.
    virtual bool InterpTargetIsCorrectType(NiObjectNET* pkTarget) const;
    virtual void GetTargetFloatValue(float& fValue);
    
    NiTObjectArray<NiTexturePtr> m_kTextures;
    unsigned int m_uiIndex;
    unsigned int m_uiAffectedMap;
    char* m_pcCtlrID;

    void ResetCtlrID();
};

#include "NiFlipController.inl"

#endif

