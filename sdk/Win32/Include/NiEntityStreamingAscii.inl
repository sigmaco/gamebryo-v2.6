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


//---------------------------------------------------------------------------
inline void NiEntityStreamingAscii::WriteEntityPointerToDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const NiFixedString& kPropertyName,
    unsigned int uiIndex)
{
    NiEntityInterface* pkPropIntfSubComp = NULL;
    pkPropIntf->GetPropertyData(
        kPropertyName, pkPropIntfSubComp, uiIndex);
    if (!pkPropIntfSubComp)
    {
        kDOM.AssignAttribute("RefLinkID", "NULL");
        return;
    }

    WriteUniqueIDToDOM(kDOM, "RefLinkID", pkPropIntfSubComp->GetID());
}
//---------------------------------------------------------------------------
inline void NiEntityStreamingAscii::WriteObjectPointerToDOM(
    NiDOMTool&,
    NiEntityPropertyInterface*,
    const NiFixedString&,
    unsigned int)
{
    NIASSERT(!"Streaming NiObject Pointer properties is not supported!");
    return;
}
//---------------------------------------------------------------------------
inline void NiEntityStreamingAscii::WriteFloatToDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const NiFixedString& kPropertyName,
    unsigned int uiIndex)
{
    float kData;
    NIVERIFY(NIBOOL_IS_TRUE(pkPropIntf->GetPropertyData(kPropertyName, kData, 
        uiIndex)));
    kDOM.WritePrimitive(kData);
}
//---------------------------------------------------------------------------
inline void NiEntityStreamingAscii::WriteBoolToDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const NiFixedString& kPropertyName,
    unsigned int uiIndex)
{
    bool kData;
    NIVERIFY(NIBOOL_IS_TRUE(pkPropIntf->GetPropertyData(kPropertyName, kData, 
        uiIndex)));
    kDOM.WritePrimitive(kData);
}
//---------------------------------------------------------------------------
inline void NiEntityStreamingAscii::WriteIntToDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const NiFixedString& kPropertyName,
    unsigned int uiIndex)
{
    int kData;
    NIVERIFY(NIBOOL_IS_TRUE(pkPropIntf->GetPropertyData(kPropertyName, kData, 
        uiIndex)));
    kDOM.WritePrimitive(kData);
}
//---------------------------------------------------------------------------
inline void NiEntityStreamingAscii::WriteUIntToDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const NiFixedString& kPropertyName,
    unsigned int uiIndex)
{
    unsigned int kData;
    NIVERIFY(NIBOOL_IS_TRUE(pkPropIntf->GetPropertyData(kPropertyName, kData, 
        uiIndex)));
    kDOM.WritePrimitive(kData);
}
//---------------------------------------------------------------------------
inline void NiEntityStreamingAscii::WriteShortToDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const NiFixedString& kPropertyName,
    unsigned int uiIndex)
{
    short kData;
    NIVERIFY(NIBOOL_IS_TRUE(pkPropIntf->GetPropertyData(kPropertyName, kData, 
        uiIndex)));
    kDOM.WritePrimitive(kData);
}
//---------------------------------------------------------------------------
inline void NiEntityStreamingAscii::WriteUShortToDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const NiFixedString& kPropertyName,
    unsigned int uiIndex)
{
    unsigned short kData;
    NIVERIFY(NIBOOL_IS_TRUE(pkPropIntf->GetPropertyData(kPropertyName, kData, 
        uiIndex)));
    kDOM.WritePrimitive(kData);
}
//---------------------------------------------------------------------------
inline void NiEntityStreamingAscii::WriteStringToDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const NiFixedString& kPropertyName,
    unsigned int uiIndex)
{
    NiFixedString kData;
    NIVERIFY(pkPropIntf->GetPropertyData(kPropertyName, kData, uiIndex));

    if (kData)
    {
        // Is this string a path and is it registered? If so, we need
        // to convert it to be file relative.
        bool bIsExternalAssetPath;
        NIVERIFY(pkPropIntf->IsExternalAssetPath(kPropertyName, uiIndex,
            bIsExternalAssetPath));
        if (bIsExternalAssetPath)
        {
            // This string is a path that has been registered.
            kDOM.AssignAttribute("Asset", "TRUE");
            WriteFilenameToDOM(kDOM, kData);
        }
        else
        {
            kDOM.WritePrimitive(kData);
        }
    }
}
//---------------------------------------------------------------------------
inline void NiEntityStreamingAscii::WritePoint2ToDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const NiFixedString& kPropertyName,
    unsigned int uiIndex)
{
    NiPoint2 kData;
    NIVERIFY(NIBOOL_IS_TRUE(pkPropIntf->GetPropertyData(kPropertyName, kData, 
        uiIndex)));
    kDOM.WritePrimitive(kData);
}
//---------------------------------------------------------------------------
inline void NiEntityStreamingAscii::WritePoint3ToDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const NiFixedString& kPropertyName,
    unsigned int uiIndex)
{
    NiPoint3 kData;
    NIVERIFY(NIBOOL_IS_TRUE(pkPropIntf->GetPropertyData(kPropertyName, kData, 
        uiIndex)));
    kDOM.WritePrimitive(kData);
}
//---------------------------------------------------------------------------
inline void NiEntityStreamingAscii::WriteQuaternionToDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const NiFixedString& kPropertyName,
    unsigned int uiIndex)
{
    NiQuaternion kData;
    NIVERIFY(NIBOOL_IS_TRUE(pkPropIntf->GetPropertyData(kPropertyName, kData, 
        uiIndex)));
    kDOM.WritePrimitive(kData);
}
//---------------------------------------------------------------------------
inline void NiEntityStreamingAscii::WriteMatrix3ToDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const NiFixedString& kPropertyName,
    unsigned int uiIndex)
{
    NiMatrix3 kData;
    NIVERIFY(NIBOOL_IS_TRUE(pkPropIntf->GetPropertyData(kPropertyName, kData, 
        uiIndex)));
    kDOM.WritePrimitive(kData);
}
//---------------------------------------------------------------------------
inline void NiEntityStreamingAscii::WriteColorToDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const NiFixedString& kPropertyName,
    unsigned int uiIndex)
{
    NiColor kData;
    NIVERIFY(NIBOOL_IS_TRUE(pkPropIntf->GetPropertyData(kPropertyName, kData, 
        uiIndex)));
    kDOM.WritePrimitive(kData);
}
//---------------------------------------------------------------------------
inline void NiEntityStreamingAscii::WriteColorAToDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const NiFixedString& kPropertyName,
    unsigned int uiIndex)
{
    NiColorA kData;
    NIVERIFY(NIBOOL_IS_TRUE(pkPropIntf->GetPropertyData(kPropertyName, kData, 
        uiIndex)));
    kDOM.WritePrimitive(kData);
}
//---------------------------------------------------------------------------
inline void NiEntityStreamingAscii::WriteUnknownTypeToDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const NiFixedString& kPropertyName,
    unsigned int uiIndex)
{
    void* pkData;
    size_t stDataSizeInBytes;

    NIVERIFY(NIBOOL_IS_TRUE(pkPropIntf->GetPropertyData(kPropertyName, 
        pkData, stDataSizeInBytes, uiIndex)));
    const unsigned char* pucData = (const unsigned char*)pkData;
    kDOM.WritePrimitive(pucData, stDataSizeInBytes);
}
//---------------------------------------------------------------------------
inline NiBool NiEntityStreamingAscii::ReadEntityPointerFromDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const char* pcName, 
    unsigned int uiIndex)
{
    NiUniqueID kRefLinkID;
    if (!ReadUniqueID(kDOM, "RefLinkID", kRefLinkID))
    {
        return false;
    }

    NiBool bSuccess;
    if (kRefLinkID == NiUniqueID())
    {
        bSuccess = pkPropIntf->SetPropertyData(pcName,
            (NiEntityInterface*) NULL, uiIndex);
        if (bSuccess)
        {
            bool bMadeUnique = false;
            NIVERIFY(pkPropIntf->MakePropertyUnique(pcName, bMadeUnique));
        }
        return bSuccess;
    }

    NiEntityPropertyInterface* pkRefEntity = GetFromID(kRefLinkID);
    if (!pkRefEntity)
    {
        char acMsg[512];
        NiSprintf(acMsg, 512, "CONFLICT: Cannot resolve entity with ID: "
            "\"%s\". The reference to that entity has been removed.",
            (const char*) kRefLinkID.ToString());
        ReportError(acMsg, m_acFilenameErrorMsg, pkPropIntf->GetName(),
            pcName);
        kDOM.SetModified(true);
    }
    NIASSERT(!pkRefEntity || pkRefEntity->GetInterfaceType() ==
        NiEntityInterface::IT_ENTITYINTERFACE);

    bSuccess = pkPropIntf->SetPropertyData(pcName, 
        (NiEntityInterface*) pkRefEntity, uiIndex);
    if (bSuccess)
    {
        bool bMadeUnique = false;
        pkPropIntf->MakePropertyUnique(pcName, bMadeUnique);
    }
    return bSuccess;
}
//---------------------------------------------------------------------------
inline NiBool NiEntityStreamingAscii::ReadObjectPointerFromDOM(
    NiDOMTool&,
    NiEntityPropertyInterface* pkPropIntf,
    const char* pcName, 
    unsigned int uiIndex)
{
    NiBool bSuccess = pkPropIntf->SetPropertyData(pcName, (NiObject*) NULL,
        uiIndex);
    if (bSuccess)
    {
        bool bMadeUnique = false;
        pkPropIntf->MakePropertyUnique(pcName, bMadeUnique);
    }
    return bSuccess;
}
//---------------------------------------------------------------------------
inline NiBool NiEntityStreamingAscii::ReadFloatFromDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const char* pcName, 
    unsigned int uiIndex)
{
    float kData;
    NiBool bSuccess = kDOM.ReadPrimitive(kData);
    if (bSuccess)
    {
        bSuccess = pkPropIntf->SetPropertyData(pcName, kData, uiIndex);
        if (bSuccess)
        {
            bool bMadeUnique = false;
            pkPropIntf->MakePropertyUnique(pcName, bMadeUnique);
        }
    }
    return bSuccess;
}
//---------------------------------------------------------------------------
inline NiBool NiEntityStreamingAscii::ReadBoolFromDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const char* pcName, 
    unsigned int uiIndex)
{       
    bool kData;
    NiBool bSuccess = kDOM.ReadPrimitive(kData);
    if (bSuccess)
    {
        bSuccess = pkPropIntf->SetPropertyData(pcName, kData, uiIndex);
        if (bSuccess)
        {
            bool bMadeUnique = false;
            pkPropIntf->MakePropertyUnique(pcName, bMadeUnique);
        }
    }
    return bSuccess;
}
//---------------------------------------------------------------------------
inline NiBool NiEntityStreamingAscii::ReadIntFromDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const char* pcName, 
    unsigned int uiIndex)
{
    int kData;
    NiBool bSuccess = kDOM.ReadPrimitive(kData);
    if (bSuccess)
    {
        bSuccess = pkPropIntf->SetPropertyData(pcName, kData, uiIndex);
        if (bSuccess)
        {
            bool bMadeUnique = false;
            pkPropIntf->MakePropertyUnique(pcName, bMadeUnique);
        }
    }
    return bSuccess;
}
//---------------------------------------------------------------------------
inline NiBool NiEntityStreamingAscii::ReadUIntFromDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const char* pcName, 
    unsigned int uiIndex)
{
    unsigned int kData;
    NiBool bSuccess = kDOM.ReadPrimitive(kData);
    if (bSuccess)
    {
        bSuccess = pkPropIntf->SetPropertyData(pcName, kData, uiIndex);
        if (bSuccess)
        {
            bool bMadeUnique = false;
            pkPropIntf->MakePropertyUnique(pcName, bMadeUnique);
        }
    }
    return bSuccess;
}
//---------------------------------------------------------------------------
inline NiBool NiEntityStreamingAscii::ReadShortFromDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const char* pcName, 
    unsigned int uiIndex)
{
    short kData;
    NiBool bSuccess = kDOM.ReadPrimitive(kData);
    if (bSuccess)
    {
        bSuccess = pkPropIntf->SetPropertyData(pcName, kData, uiIndex);
        if (bSuccess)
        {
            bool bMadeUnique = false;
            pkPropIntf->MakePropertyUnique(pcName, bMadeUnique);
        }
    }
    return bSuccess;
}
//---------------------------------------------------------------------------
inline NiBool NiEntityStreamingAscii::ReadUShortFromDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const char* pcName, 
    unsigned int uiIndex)
{
    unsigned short kData;
    NiBool bSuccess = kDOM.ReadPrimitive(kData);
    if (bSuccess)
    {
        bSuccess = pkPropIntf->SetPropertyData(pcName, kData, uiIndex);
        if (bSuccess)
        {
            bool bMadeUnique = false;
            pkPropIntf->MakePropertyUnique(pcName, bMadeUnique);
        }
    }
    return bSuccess;
}
//---------------------------------------------------------------------------
inline NiBool NiEntityStreamingAscii::ReadStringFromDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const char* pcName, 
    unsigned int uiIndex)
{
    NiFixedString kData;
    if (!kDOM.ReadPrimitive(kData))
    {
        return false;
    }

    // Is the string a path and does it need to be converted to an 
    // absolute path?
    const char* pkAsset = kDOM.GetAttributeFromCurrent("Asset");
    if (pkAsset && NiStricmp(pkAsset, "TRUE") == 0)
    {
        // This string is really a path that references an asset.
        // The path needs to be converted to an absolute path.
        if (!ReadFilenameFromDOM(kDOM, kData))
        {
            return false;
        }
    }

    NiBool bSuccess = pkPropIntf->SetPropertyData(pcName, kData, uiIndex);
    if (bSuccess)
    {
        bool bMadeUnique = false;
        pkPropIntf->MakePropertyUnique(pcName, bMadeUnique);
    }
    return bSuccess;
}
//---------------------------------------------------------------------------
inline NiBool NiEntityStreamingAscii::ReadPoint2FromDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const char* pcName, 
    unsigned int uiIndex)
{
    NiPoint2 kData;
    NiBool bSuccess = kDOM.ReadPrimitive(kData);
    if (bSuccess)
    {
        bSuccess = pkPropIntf->SetPropertyData(pcName, kData, uiIndex);
        if (bSuccess)
        {
            bool bMadeUnique = false;
            pkPropIntf->MakePropertyUnique(pcName, bMadeUnique);
        }
    }
    return bSuccess;
}
//---------------------------------------------------------------------------
inline NiBool NiEntityStreamingAscii::ReadPoint3FromDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const char* pcName, 
    unsigned int uiIndex)
{
    NiPoint3 kData;
    NiBool bSuccess = kDOM.ReadPrimitive(kData);
    if (bSuccess)
    {
        bSuccess = pkPropIntf->SetPropertyData(pcName, kData, uiIndex);
        if (bSuccess)
        {
            bool bMadeUnique = false;
            pkPropIntf->MakePropertyUnique(pcName, bMadeUnique);
        }
    }
    return bSuccess;
}
//---------------------------------------------------------------------------
inline NiBool NiEntityStreamingAscii::ReadQuaternionFromDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const char* pcName, 
    unsigned int uiIndex)
{
    NiQuaternion kData;
    NiBool bSuccess = kDOM.ReadPrimitive(kData);
    if (bSuccess)
    {
        bSuccess = pkPropIntf->SetPropertyData(pcName, kData, uiIndex);
        if (bSuccess)
        {
            bool bMadeUnique = false;
            pkPropIntf->MakePropertyUnique(pcName, bMadeUnique);
        }
    }
    return bSuccess;
}
//---------------------------------------------------------------------------
inline NiBool NiEntityStreamingAscii::ReadMatrix3FromDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const char* pcName, 
    unsigned int uiIndex)
{
    NiMatrix3 kData;
    NiBool bSuccess = kDOM.ReadPrimitive(kData);
    if (bSuccess)
    {
        bSuccess = pkPropIntf->SetPropertyData(pcName, kData, uiIndex);
        if (bSuccess)
        {
            bool bMadeUnique = false;
            pkPropIntf->MakePropertyUnique(pcName, bMadeUnique);
        }
    }
    return bSuccess;
}
//---------------------------------------------------------------------------
inline NiBool NiEntityStreamingAscii::ReadColorFromDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const char* pcName, 
    unsigned int uiIndex)
{
    NiColor kData;
    NiBool bSuccess = kDOM.ReadPrimitive(kData);
    if (bSuccess)
    {
        bSuccess = pkPropIntf->SetPropertyData(pcName, kData, uiIndex);
        if (bSuccess)
        {
            bool bMadeUnique = false;
            pkPropIntf->MakePropertyUnique(pcName, bMadeUnique);
        }
    }
    return bSuccess;
}
//---------------------------------------------------------------------------
inline NiBool NiEntityStreamingAscii::ReadColorAFromDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const char* pcName,
    unsigned int uiIndex)
{
    NiColorA kData;
    NiBool bSuccess = kDOM.ReadPrimitive(kData);
    if (bSuccess)
    {
        bSuccess = pkPropIntf->SetPropertyData(pcName, kData, uiIndex);
        if (bSuccess)
        {
            bool bMadeUnique = false;
            pkPropIntf->MakePropertyUnique(pcName, bMadeUnique);
        }
    }
    return bSuccess;
}
//---------------------------------------------------------------------------
