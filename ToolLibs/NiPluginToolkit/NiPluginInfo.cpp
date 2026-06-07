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

#include "NiPluginInfo.h"
#include "NiParameterInfo.h"

NiImplementRootRTTI(NiPluginInfo);

//---------------------------------------------------------------------------
const NiString& NiPluginInfo::GetName() const
{
    return m_strPluginName;
}
//---------------------------------------------------------------------------
const NiString& NiPluginInfo::GetVersion() const
{
    return m_strPluginVersion;
}
//---------------------------------------------------------------------------
const NiString& NiPluginInfo::GetType() const
{
    return m_strPluginType;
}
//---------------------------------------------------------------------------
const NiString& NiPluginInfo::GetClassName() const
{
    return m_strClassName;
}
//---------------------------------------------------------------------------
unsigned int NiPluginInfo::GetParameterCount() const
{
    return m_kParameters.GetSize();
}
//---------------------------------------------------------------------------
unsigned int NiPluginInfo::GetParameterIndex(const char* pcKey,
    unsigned int uiStartIdx) const
{
    for (unsigned int ui = uiStartIdx; ui < m_kParameters.GetSize(); ui++)
    {
        NiParameterInfo* pkInfo = m_kParameters.GetAt(ui);
        if (pkInfo && pkInfo->GetKey().Equals(pcKey))
        {
            return ui;
        }
    }

    return NIPT_INVALID_INDEX;
}
//---------------------------------------------------------------------------
NiParameterInfo* NiPluginInfo::GetParameterAt(unsigned int ui) const
{
    return m_kParameters.GetAt(ui);
}
//---------------------------------------------------------------------------
void NiPluginInfo::SetName(const char* pcPluginName)
{
    MarkAsDirty(true);
    m_strPluginName = pcPluginName;
}
//---------------------------------------------------------------------------
void NiPluginInfo::SetVersion(const char* pcPluginVersion)
{
    MarkAsDirty(true);
    m_strPluginVersion = pcPluginVersion;
}
//---------------------------------------------------------------------------
void NiPluginInfo::SetType(const char* pcPluginType)
{
    MarkAsDirty(true);
    m_strPluginType = pcPluginType;
}
//---------------------------------------------------------------------------
void NiPluginInfo::SetClassName(const char* pcClassName)
{
    MarkAsDirty(true);
    m_strClassName = pcClassName;
}
//---------------------------------------------------------------------------
void NiPluginInfo::AddParameter(NiParameterInfo* pkParam)
{
    MarkAsDirty(true);
    m_kParameters.Add(pkParam);
    m_kParameters.Compact();
}
//---------------------------------------------------------------------------
void NiPluginInfo::AddParameter(const char* pcKey, const char* pcValue)
{
    NiParameterInfoPtr pkParam = NiNew NiParameterInfo();
    pkParam->SetKey(pcKey);
    pkParam->SetValue(pcValue);
    AddParameter(pkParam);
}
//---------------------------------------------------------------------------
NiString NiPluginInfo::GetValue(const char* pcKey, unsigned int uiStartIdx)
    const
{
    NiString strValue;

    unsigned int uiIdx = GetParameterIndex(pcKey, uiStartIdx);

    if(uiIdx == NIPT_INVALID_INDEX)
    {
        return strValue;
    }

    NiParameterInfo* pkParam = GetParameterAt(uiIdx);
    if (pkParam)
    {
        strValue = pkParam->GetValue();
    }

    return strValue;
}
//---------------------------------------------------------------------------
bool NiPluginInfo::GetBool(const char* pcKey, unsigned int uiStartIdx) const
{
    NiString strValue = GetValue(pcKey, uiStartIdx);
    NIASSERT(!strValue.IsEmpty());

    if (strValue == "1")
        return true;
    if (strValue == "0")
        return false;

    return IsTrue(strValue);
}
//---------------------------------------------------------------------------
int NiPluginInfo::GetInt(const char* pcKey, unsigned int uiStartIdx) const
{
    NiString strValue = GetValue(pcKey, uiStartIdx);
    NIASSERT(!strValue.IsEmpty());
    
    int iRet;
    if (strValue.ToInt(iRet))
        return iRet;
    else
    {
        NIASSERT(strValue.ToInt(iRet));
        return -1;
    }
}
//---------------------------------------------------------------------------
float NiPluginInfo::GetFloat(const char* pcKey, unsigned int uiStartIdx) const
{
    NiString strValue = GetValue(pcKey, uiStartIdx);
    NIASSERT(!strValue.IsEmpty());
    
    float iRet;
    if (strValue.ToFloat(iRet))
        return iRet;
    else
    {
        NIASSERT(strValue.ToFloat(iRet));
        return -1;
    }
}
//---------------------------------------------------------------------------
bool NiPluginInfo::SetValue(const char* pcKey, const char* pcValue, 
    unsigned int uiStartIdx)
{
    MarkAsDirty(true);
    unsigned int uiIdx = GetParameterIndex(pcKey, uiStartIdx);

    if(uiIdx == NIPT_INVALID_INDEX)
    {
        AddParameter(pcKey, pcValue);
        return true;
    }

    NiParameterInfo* pkParam = GetParameterAt(uiIdx);
    if (!pkParam)
    {
        return false;
    }

    pkParam->SetValue(pcValue);

    return true;
}


//---------------------------------------------------------------------------
bool NiPluginInfo::SetBool(const char* pcKey, bool bValue, 
                           unsigned int uiStartIdx)
{
    MarkAsDirty(true);
    NiString strBool = NiString::FromBool(bValue);
    return SetValue(pcKey, strBool, uiStartIdx);
}

//---------------------------------------------------------------------------
bool NiPluginInfo::SetInt(const char* pcKey, int iValue, 
                          unsigned int uiStartIdx)
{
    MarkAsDirty(true);
    NiString strInt = NiString::FromInt(iValue);
    return SetValue(pcKey, strInt, uiStartIdx);
}
//---------------------------------------------------------------------------
bool NiPluginInfo::SetFloat(const char* pcKey, float fValue, 
                          unsigned int uiStartIdx)
{
    MarkAsDirty(true);
    NiString strFloat = NiString::FromFloat(fValue);
    return SetValue(pcKey, strFloat, uiStartIdx);
}

//---------------------------------------------------------------------------
void NiPluginInfo::RemoveParameterAt(unsigned int ui)
{
    MarkAsDirty(true);
    NiParameterInfo* pkParam = GetParameterAt(ui);
    if (pkParam)
        m_kParameters.RemoveAt(ui);

    m_kParameters.Compact();

}
//---------------------------------------------------------------------------
void NiPluginInfo::RemoveParameter(const char* pcKey)
{
    MarkAsDirty(true);
    unsigned int uiIdx = GetParameterIndex(pcKey);
    NiParameterInfo* pkParam = GetParameterAt(uiIdx);
    if (pkParam)
        m_kParameters.RemoveAt(uiIdx);
    m_kParameters.Compact();
}
//---------------------------------------------------------------------------
void NiPluginInfo::RemoveAllParameters()
{
    MarkAsDirty(true);
    m_kParameters.RemoveAll();
    m_kParameters.Compact();
}
//---------------------------------------------------------------------------
NiPluginInfo* NiPluginInfo::Clone(NiPluginInfo* pkClone)
{
    if (!pkClone)
    {
        pkClone = NiNew NiPluginInfo;
    }
    pkClone->SetName(m_strPluginName);
    pkClone->SetVersion(m_strPluginVersion);
    pkClone->SetType(m_strPluginType);
    pkClone->SetClassName(m_strClassName);
    pkClone->RemoveAllParameters();

    for(unsigned int ui = 0; ui < GetParameterCount(); ui++)
    {
        NiParameterInfo* pkParam = GetParameterAt(ui);
        if (pkParam)
        {
            pkClone->AddParameter(pkParam->GetKey(), pkParam->GetValue());
        }
    }
    pkClone->MarkAsDirty(IsDirty());
    return pkClone;
}
//---------------------------------------------------------------------------
bool NiPluginInfo::Equals(NiPluginInfo* pkInfo)
{
    if (pkInfo->GetName() != m_strPluginName)
    {
        return false;
    }

    if (pkInfo->GetVersion() != m_strPluginVersion)
    {
        return false;
    }

    if (pkInfo->GetType() != m_strPluginType)
    {
        return false;
    }

    if (pkInfo->GetClassName() != m_strClassName)
    {
        return false;
    }

    if (pkInfo->GetParameterCount() != GetParameterCount())
    {
        return false;
    }

    for(unsigned int ui = 0; ui < GetParameterCount(); ui++)
    {
        NiParameterInfo* pkThisParam = GetParameterAt(ui);
        NiParameterInfo* pkOtherParam = pkInfo->GetParameterAt(ui);

        if (!pkThisParam || !pkOtherParam)
        {
            return false;
        }

        if (pkThisParam->GetKey() != pkOtherParam->GetKey())
        {
            return false;
        }

        if (pkThisParam->GetValue() != pkOtherParam->GetValue())
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
/// Has this plugininfo been altered
bool  NiPluginInfo::IsDirty()
{
    return m_bDirty;
}

//---------------------------------------------------------------------------
/// Set the dirty bit for this object
void  NiPluginInfo::MarkAsDirty(bool bDirty)
{
    m_bDirty = bDirty;
}
//---------------------------------------------------------------------------
