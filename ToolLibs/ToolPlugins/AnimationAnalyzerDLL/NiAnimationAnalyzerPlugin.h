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

#ifndef NIAnimationANALYZERPLUGIN_H
#define NIAnimationANALYZERPLUGIN_H

#include <NiAnimationKey.h>

class NiAnimationAnalyzerPlugin : public NiPlugin
{
public:
    NiDeclareRTTI;

    /// Default constructor.
    NiAnimationAnalyzerPlugin();

    /// Required virtual function overrides from NiPlugin.
    virtual bool CanExecute(NiPluginInfo* pkInfo);
    virtual NiExecutionResultPtr Execute(const NiPluginInfo* pkInfo);
    virtual NiPluginInfo* GetDefaultPluginInfo(); 
    virtual bool HasManagementDialog();
    virtual bool DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent);

protected:
    void  FindAnimations(NiAVObject* pkObj, NiString& strAnimationRes);

    static const char* GetKeyTypeEnumString(NiAnimationKey::KeyType eType);
    static NiString GetKeyAsString(NiAnimationKey* pkKey, 
        NiAnimationKey::KeyContent eContentType,
        NiAnimationKey::KeyType eType);
    static unsigned int GetKeySize(NiAnimationKey* pkKey, 
        NiAnimationKey::KeyContent eContentType,
        NiAnimationKey::KeyType eType);
    static NiString ToString(float fValue);
    static NiString ToString(NiPoint3 kValue);
    static NiString ToString(NiQuaternion kValue);

    NiString m_strMessage;
    unsigned int m_uiTotalNumKeys;
    unsigned int m_uiTotalNumControllers;
    unsigned int m_uiTotalSizeKeys;
    unsigned int m_uiTotalRotKeys;
    unsigned int m_uiTotalPosKeys;
    unsigned int m_uiTotalScaleKeys;
};

#endif  // #ifndef NIAnimationANALYZERPLUGIN_H
