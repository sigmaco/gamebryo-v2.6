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

// Precompiled Header
#include "NiEntityPCH.h"

#include "NiDOMTool.h"

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
//#elif defined(_PS3)
//#pragma GCC system_header
#endif
#include "tinyxml.h"
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif


//---------------------------------------------------------------------------
#ifdef __GNUC__
#define POSSIBLY_UNUSED __attribute__ ((__unused__))
#else
#define POSSIBLY_UNUSED
#endif
//---------------------------------------------------------------------------
NiDOMTool::NiDOMTool() :
    m_pkXMLDocument(NULL),
    m_pkElementSet(NULL),
    m_uiVersion(0),
    m_bModified(false)
{
}
//---------------------------------------------------------------------------
NiDOMTool::NiDOMTool(const char* pcFileName) :
    m_pkXMLDocument(NULL),
    m_pkElementSet(NULL),
    m_uiVersion(0),
    m_bModified(false)
{
    Init(pcFileName);
}
//---------------------------------------------------------------------------
NiDOMTool::~NiDOMTool()
{
    Flush();
}
//---------------------------------------------------------------------------
void NiDOMTool::Init(const char* pcFileName)
{
    Flush();

    m_pkXMLDocument = NiExternalNew TiXmlDocument(pcFileName);
    m_pkElementSet = NiNew NiTPrimitiveSet<TiXmlElement*>;
    m_kFilename = pcFileName;

    // Strip filename from path.
    char acFilePath[NI_MAX_PATH];
    NiStrcpy(acFilePath, NI_MAX_PATH, pcFileName);
    char* pcLastSlash = strrchr(acFilePath, NI_PATH_DELIMITER_CHAR);
    if (pcLastSlash)
    {
        *pcLastSlash = '\0';
    }
    m_kFilePath = acFilePath;
}
//---------------------------------------------------------------------------
void NiDOMTool::Flush()
{
    if (m_pkElementSet)
        NiDelete m_pkElementSet;

    if (m_pkXMLDocument)
        NiExternalDelete m_pkXMLDocument;

    m_pkElementSet = NULL;
    m_pkXMLDocument = NULL;
}
//---------------------------------------------------------------------------
const NiFixedString& NiDOMTool::GetFilename() const
{
    return m_kFilename;
}
//---------------------------------------------------------------------------
const NiFixedString& NiDOMTool::GetFilePath() const
{
    return m_kFilePath;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::LoadFile()
{
    NIASSERT(m_pkXMLDocument);
#if defined(_WII)
    // TinyXML uses fopen to load files, which doesn't work
    // on the Wii.  Instead we need to use NiFile to open the file
    // and then pass the data to the TinyXML Parse function.
    NiFile* pkFile = NiFile::GetFile(m_pkXMLDocument->Value(), NiFile::READ_ONLY);
    if (!(*pkFile))
        return false;

    unsigned int uiSize = pkFile->GetFileSize();
    char* pBuffer = (char*)NiMalloc(uiSize);
    pkFile->Read(pBuffer, uiSize);
    bool bSuccess = (m_pkXMLDocument->Parse(pBuffer) != 0);
    NiFree(pBuffer);
    NiDelete pkFile;
    return bSuccess;
#else
    return m_pkXMLDocument->LoadFile();
#endif
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::SaveFile()
{
    NIASSERT(m_pkXMLDocument);
#if defined(_WII)
    // Saving entity data is not implemented on the Wii.
    return false;
#else
    return m_pkXMLDocument->SaveFile();
#endif
}
//---------------------------------------------------------------------------
TiXmlElement* NiDOMTool::SetSectionToNextSibling()
{
    TiXmlElement* pkElement = GetCurrentSection();
    pkElement = pkElement->NextSiblingElement();
    EndSection();
    m_pkElementSet->Add(pkElement);
    return pkElement;
}
//---------------------------------------------------------------------------
TiXmlElement* NiDOMTool::SetSectionToFirstChild()
{
    TiXmlElement* pkElement = GetCurrentSection();
    pkElement = pkElement->FirstChildElement();
    m_pkElementSet->Add(pkElement);
    return pkElement;
}
//---------------------------------------------------------------------------
TiXmlElement* NiDOMTool::GetCurrentSection()
{
    if (m_pkElementSet->GetSize() == 0)
    {
        return m_pkXMLDocument->FirstChildElement();
    }
    else
    {
        // Using array as a stack, should never have empty gaps
        int iIndex = m_pkElementSet->GetSize() - 1;
        NIASSERT(iIndex >= 0);
        return m_pkElementSet->GetAt(iIndex);
    }
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::IsCurrentSectionValid()
{
    if (GetCurrentSection())
        return true;

    return false;
}
//---------------------------------------------------------------------------
TiXmlElement* NiDOMTool::ResetSectionTo(const char* pcSection)
{
    if (!m_pkXMLDocument)
        return NULL;

    m_pkElementSet->RemoveAll();

    return SetSectionTo(pcSection);
}
//---------------------------------------------------------------------------
TiXmlElement* NiDOMTool::SetSectionTo(const char* pcSection)
{
    if (!m_pkXMLDocument)
        return NULL;

    TiXmlElement* pkElement;

    if (m_pkElementSet->GetSize() == 0)
    {
        pkElement = m_pkXMLDocument->FirstChildElement(pcSection);  
        m_pkElementSet->Add(pkElement);
    }
    else
    {
        unsigned int uiStackSize = m_pkElementSet->GetSize();;
        NIASSERT(uiStackSize >= 1);
        pkElement = m_pkElementSet->GetAt(uiStackSize-1);
        pkElement = pkElement->FirstChildElement(pcSection);
        m_pkElementSet->Add(pkElement);
    }

    return pkElement;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::EndSection()
{
    if (!m_pkXMLDocument)
        return false;

    int iIndex = m_pkElementSet->GetSize() - 1;
    NIASSERT(iIndex >= 0);

    m_pkElementSet->RemoveAt(iIndex);

    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::BeginSection(const char* pcName)
{
    if (!m_pkXMLDocument)
        return false;

    TiXmlElement* pkElement = NiExternalNew TiXmlElement(pcName);
    NIASSERT(pkElement); 

    if (m_pkElementSet->GetSize() == 0)
    {
        m_pkXMLDocument->LinkEndChild(pkElement);
    }
    else
    {
        // Using array as a stack, should never have empty gaps
        int iIndex = m_pkElementSet->GetSize() - 1;
        NIASSERT(iIndex >= 0);
        TiXmlElement* pkParent = m_pkElementSet->GetAt(iIndex);
        pkParent->LinkEndChild(pkElement);
    }

    m_pkElementSet->Add(pkElement);

    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::InsertComment(const char* pcComment)
{
    TiXmlElement* pkSection = GetCurrentSection();
    TiXmlComment kComment;

    size_t stLen = strlen(pcComment);

    if (stLen == 0)
        return false;

    stLen += 16;

    char* pcFormattedComment = NiExternalNew char[stLen];
    NiSprintf(pcFormattedComment, stLen, "<!--%s-->", pcComment);
    TiXmlComment* pkComment = NiExternalNew TiXmlComment();
    pkComment->Parse( pcFormattedComment, 0, TIXML_ENCODING_UTF8 );

    pkSection->LinkEndChild(pkComment);
    NiExternalDelete [] pcFormattedComment;

    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::AssignAttribute( const char* pcAttributeName, int iValue)
{
    TiXmlElement* pkParent = GetCurrentSection();

    if (!pkParent)
        return false;

    pkParent->SetAttribute(pcAttributeName, iValue);

    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::AssignAttribute(const char* pcAttributeName, 
    const char* pcValue)
{
    TiXmlElement* pkParent = GetCurrentSection();

    if (!pkParent)
        return false;

    pkParent->SetAttribute(pcAttributeName, pcValue);

    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::WriteHeader()
{
    if (!m_pkXMLDocument)
        return false;

    // Write Header
    TiXmlDeclaration* pkDecl = 
        NiExternalNew TiXmlDeclaration( "1.0", "", "" );  
    m_pkXMLDocument->LinkEndChild(pkDecl); 

    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::WritePrimitive(const float& fData)
{
    TiXmlElement* pkElement = GetCurrentSection();
    char acBuf[256];
    NiSprintf(acBuf, 256, "%.16f", fData);
    TiXmlText* pkText = NiExternalNew TiXmlText(acBuf);
    pkElement->LinkEndChild(pkText);
    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::WritePrimitive(const bool& bData)
{
    TiXmlElement* pkElement = GetCurrentSection();
    TiXmlText* pkText;
    if (bData)
        pkText = NiExternalNew TiXmlText("TRUE");
    else
        pkText = NiExternalNew TiXmlText("FALSE");

    pkElement->LinkEndChild(pkText);
    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::WritePrimitive(const int& iData)
{
    TiXmlElement* pkElement = GetCurrentSection();
    char acBuf[256];
    NiSprintf(acBuf, 256, "%d", iData);
    TiXmlText* pkText = NiExternalNew TiXmlText(acBuf);
    pkElement->LinkEndChild(pkText);
    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::WritePrimitive(const unsigned int& uiData)
{
    TiXmlElement* pkElement = GetCurrentSection();
    char acBuf[256];
    NiSprintf(acBuf, 256, "%u", uiData);
    TiXmlText* pkText = NiExternalNew TiXmlText(acBuf);
    pkElement->LinkEndChild(pkText);
    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::WritePrimitive(const short& sData)
{
    TiXmlElement* pkElement = GetCurrentSection();
    char acBuf[256];
    NiSprintf(acBuf, 256, "%i", sData);
    TiXmlText* pkText = NiExternalNew TiXmlText(acBuf);
    pkElement->LinkEndChild(pkText);
    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::WritePrimitive(const unsigned short& usData)
{
    TiXmlElement* pkElement = GetCurrentSection();
    char acBuf[256];
    NiSprintf(acBuf, 256, "%i", usData);
    TiXmlText* pkText = NiExternalNew TiXmlText(acBuf);
    pkElement->LinkEndChild(pkText);
    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::WritePrimitive(const NiFixedString& kData)
{
    TiXmlElement* pkElement = GetCurrentSection();
    TiXmlText* pkText = NiExternalNew TiXmlText(kData);
    pkElement->LinkEndChild(pkText);
    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::WritePrimitive(const NiPoint2& kData)
{
    TiXmlElement* pkElement = GetCurrentSection();
    char acBuf[256];
    NiSprintf(acBuf, 256, "%.16f, %.16f", kData.x, kData.y);
    TiXmlText* pkText = NiExternalNew TiXmlText(acBuf);
    pkElement->LinkEndChild( pkText );
    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::WritePrimitive(const NiPoint3& kData)
{
    TiXmlElement* pkElement = GetCurrentSection();
    char acBuf[256];
    NiSprintf(acBuf, 256, "%.16f, %.16f, %.16f", kData.x, kData.y, kData.z);
    TiXmlText* pkText = NiExternalNew TiXmlText(acBuf);
    pkElement->LinkEndChild( pkText );
    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::WritePrimitive(const NiQuaternion& kData)
{
    TiXmlElement* pkElement = GetCurrentSection();
    char acBuf[256];
    NiSprintf(acBuf, 256, "%.16f, %.16f, %.16f, %.16f", 
        kData.m_fW, kData.m_fX, kData.m_fY, kData.m_fZ);
    TiXmlText* pkText = NiExternalNew TiXmlText(acBuf);
    pkElement->LinkEndChild( pkText );
    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::WritePrimitive(const NiMatrix3& kData)
{
    TiXmlElement* pkElement = GetCurrentSection();
    char acBuf[256];
    int i=0;
    do
    {
        TiXmlElement* pkRow = NiExternalNew TiXmlElement("ROW");
        pkElement->LinkEndChild(pkRow);
        float f0, f1, f2;
        kData.GetRow(i, f0, f1, f2);
        NiSprintf(acBuf, 256, "%.16f, %.16f, %.16f", f0, f1, f2);
        TiXmlText* pkText = NiExternalNew TiXmlText(acBuf);
        pkRow->LinkEndChild( pkText );
        i++;
    } while (i<3);

    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::WritePrimitive(const NiColor& kData)
{
    TiXmlElement* pkElement = GetCurrentSection();
    char acBuf[256];
    NiSprintf(acBuf, 256, "%.16f, %.16f, %.16f", kData.r, kData.g, kData.b);
    TiXmlText* pkText = NiExternalNew TiXmlText(acBuf);
    pkElement->LinkEndChild( pkText );
    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::WritePrimitive(const NiColorA& kData)
{
    TiXmlElement* pkElement = GetCurrentSection();
    char acBuf[256];
    NiSprintf(acBuf, 256, "%.16f, %.16f, %.16f, %.16f", kData.r,
        kData.g, kData.b, kData.a);
    TiXmlText* pkText = NiExternalNew TiXmlText(acBuf);
    pkElement->LinkEndChild( pkText );
    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::WritePrimitive(const unsigned char* pucData, 
    size_t stDataSizeInBytes)
{
    TiXmlElement* pkElement = GetCurrentSection();
    char* pcBase64String = 
        NiBase64::EncodeToBase64(pucData, (int)stDataSizeInBytes);
    TiXmlText* pkText = NiExternalNew TiXmlText(pcBase64String);
    pkElement->LinkEndChild( pkText );
    delete [] pcBase64String;
    return true;
}
//---------------------------------------------------------------------------
int NiDOMTool::GetAttributeCountFromCurrent()
{
    int iCount = 0;
    TiXmlElement* pkElement = GetCurrentSection();
    TiXmlAttribute* pkAttrib = pkElement->FirstAttribute();
    while(pkAttrib != NULL)
    {
        iCount++;
        pkAttrib = pkAttrib->Next();
    }
    NIASSERT(pkElement);
    return iCount;
}
//---------------------------------------------------------------------------
const char* NiDOMTool::GetAttributeFromCurrent(const char* pcAttribute)
{
    TiXmlElement* pkElement = GetCurrentSection();
    NIASSERT(pkElement);
    return pkElement->Attribute(pcAttribute);
}
//---------------------------------------------------------------------------
const char* NiDOMTool::GetValueFromCurrent()
{
    TiXmlElement* pkElement = GetCurrentSection();
    NIASSERT(pkElement);
    return pkElement->Value();
}
//---------------------------------------------------------------------------
const char* NiDOMTool::GetTextFromCurrent()
{
    TiXmlElement* pkElement = GetCurrentSection();
    NIASSERT(pkElement);
    return pkElement->GetText();
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::IsCurrentLeaf()
{
    TiXmlElement* pkElement = GetCurrentSection();
    const TiXmlNode* pkChild = pkElement->FirstChild();
    if (pkChild)
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::ReadPrimitive(float& fData)
{
    TiXmlElement* pkElement = GetCurrentSection();
    const char* pcFloat = pkElement->GetText();
#if defined(_MSC_VER) && _MSC_VER >= 1400
    int iFieldsAssigned = sscanf_s(pcFloat, "%f", &fData);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    int iFieldsAssigned = sscanf(pcFloat, "%f", &fData);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400

    if (iFieldsAssigned != 1)
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::ReadPrimitive(bool& bData)
{
    TiXmlElement* pkElement = GetCurrentSection();

    const char* pcBool = pkElement->GetText();

    if (NiStricmp(pcBool,"TRUE") == 0)
    {
        bData = true;
    }
    else if (NiStricmp(pcBool,"FALSE") == 0)
    {
        bData = false;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::ReadPrimitive(int& iData)
{
    TiXmlElement* pkElement = GetCurrentSection();
    const char* pcInt = pkElement->GetText();
#if defined(_MSC_VER) && _MSC_VER >= 1400
    int POSSIBLY_UNUSED iFieldsAssigned = sscanf_s(pcInt, "%i", &iData);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    int POSSIBLY_UNUSED iFieldsAssigned = sscanf(pcInt, "%i", &iData);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (iFieldsAssigned != 1)
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::ReadPrimitive(unsigned int& uiData)
{
    TiXmlElement* pkElement = GetCurrentSection();
    const char* pcUnsignedInt = pkElement->GetText();
#if defined(_MSC_VER) && _MSC_VER >= 1400
    int POSSIBLY_UNUSED iFieldsAssigned =
        sscanf_s(pcUnsignedInt, "%u", &uiData);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    int POSSIBLY_UNUSED iFieldsAssigned = sscanf(pcUnsignedInt, "%u", &uiData);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (iFieldsAssigned != 1)
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::ReadPrimitive(short& sData)
{
    TiXmlElement* pkElement = GetCurrentSection();
    const char* pcShort = pkElement->GetText();
#if defined(_MSC_VER) && _MSC_VER >= 1400
    int POSSIBLY_UNUSED iFieldsAssigned = sscanf_s(pcShort, "%hd", &sData);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    int POSSIBLY_UNUSED iFieldsAssigned = sscanf(pcShort, "%hd", &sData);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (iFieldsAssigned != 1)
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::ReadPrimitive(unsigned short& usData)
{
    TiXmlElement* pkElement = GetCurrentSection();
    const char* pcUnsignedShort = pkElement->GetText();
#if defined(_MSC_VER) && _MSC_VER >= 1400
    int POSSIBLY_UNUSED iFieldsAssigned =
        sscanf_s(pcUnsignedShort, "%hu", &usData);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    int POSSIBLY_UNUSED iFieldsAssigned =
        sscanf(pcUnsignedShort, "%hu", &usData);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (iFieldsAssigned != 1)
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::ReadPrimitive(NiFixedString& kData)
{
    TiXmlElement* pkElement = GetCurrentSection();
    const char* pcNiFixedString = pkElement->GetText();
    kData = pcNiFixedString;
    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::ReadPrimitive(NiPoint2& kData)
{
    TiXmlElement* pkElement = GetCurrentSection();
    const char* pcRow = pkElement->GetText();
#if defined(_MSC_VER) && _MSC_VER >= 1400
    int POSSIBLY_UNUSED iFieldsAssigned =
        sscanf_s(pcRow, "%f, %f", &kData.x, &kData.y);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    int POSSIBLY_UNUSED iFieldsAssigned =
        sscanf(pcRow, "%f, %f", &kData.x, &kData.y);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (iFieldsAssigned != 2)
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::ReadPrimitive(NiPoint3& kData)
{
    TiXmlElement* pkElement = GetCurrentSection();
    const char* pcRow = pkElement->GetText();
#if defined(_MSC_VER) && _MSC_VER >= 1400
    int POSSIBLY_UNUSED iFieldsAssigned = sscanf_s(pcRow, "%f, %f, %f", 
        &kData.x, &kData.y, &kData.z);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    int POSSIBLY_UNUSED iFieldsAssigned = sscanf(pcRow, "%f, %f, %f", 
        &kData.x, &kData.y, &kData.z);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (iFieldsAssigned != 3)
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::ReadPrimitive(NiQuaternion& kData)
{
    TiXmlElement* pkElement = GetCurrentSection();
    const char* pcRow = pkElement->GetText();
#if defined(_MSC_VER) && _MSC_VER >= 1400
    int POSSIBLY_UNUSED iFieldsAssigned = sscanf_s(pcRow, "%f, %f, %f, %f", 
        &kData.m_fW, &kData.m_fX, &kData.m_fY, &kData.m_fZ);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    int POSSIBLY_UNUSED iFieldsAssigned = sscanf(pcRow, "%f, %f, %f, %f", 
        &kData.m_fW, &kData.m_fX, &kData.m_fY, &kData.m_fZ);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (iFieldsAssigned != 4)
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::ReadPrimitive(NiMatrix3& kData)
{
    TiXmlElement* pkElement = GetCurrentSection();

    TiXmlElement* pkChildElem =
        pkElement->FirstChildElement();

    int i=0;
    do
    {
        NIASSERT(NiStricmp(pkChildElem->Value(),"Row")==0);
        const char* pcRow = pkChildElem->GetText();

        float f1, f2, f3;
#if defined(_MSC_VER) && _MSC_VER >= 1400
        int iFieldsAssigned = 
            sscanf_s(pcRow, "%f, %f, %f", &f1, &f2, &f3);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
        int iFieldsAssigned = 
            sscanf(pcRow, "%f, %f, %f", &f1, &f2, &f3);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400

        if (iFieldsAssigned != 3)
        {
            return false;
        }

        kData.SetRow(i++, f1, f2, f3);

        pkChildElem = pkChildElem->NextSiblingElement();
    } while (i<3);

    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::ReadPrimitive(NiColor& kData)
{
    TiXmlElement* pkElement = GetCurrentSection();
    const char* pcRow = pkElement->GetText();
#if defined(_MSC_VER) && _MSC_VER >= 1400
    int POSSIBLY_UNUSED iFieldsAssigned = sscanf_s(pcRow, "%f, %f, %f", 
        &kData.r, &kData.g, &kData.b);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    int POSSIBLY_UNUSED iFieldsAssigned = sscanf(pcRow, "%f, %f, %f", 
        &kData.r, &kData.g, &kData.b);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (iFieldsAssigned != 3)
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::ReadPrimitive(NiColorA& kData)
{
    TiXmlElement* pkElement = GetCurrentSection();
    const char* pcRow = pkElement->GetText();
#if defined(_MSC_VER) && _MSC_VER >= 1400
    int POSSIBLY_UNUSED iFieldsAssigned = sscanf_s(pcRow, "%f, %f, %f, %f", 
        &kData.r, &kData.g, &kData.b, &kData.a);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    int POSSIBLY_UNUSED iFieldsAssigned = sscanf(pcRow, "%f, %f, %f, %f", 
        &kData.r, &kData.g, &kData.b, &kData.a);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (iFieldsAssigned != 4)
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::ReadPrimitive(unsigned char*& pucData,
    size_t& stDataSizeInBytes)
{
    // Calling function required to free pucData.
    NIASSERT(pucData == NULL);

    TiXmlElement* pkElement = GetCurrentSection();
    const char* pcData = pkElement->GetText();
    NIASSERT(pcData);
    size_t stLen = strlen(pcData);

    // The size of the text will always be greater than the actual size
    // of the buffer needed
    pucData = (unsigned char*)(NiMalloc(stLen));
    
    NIASSERT(pcData);
    NiBool bSuccess = 
        NiBase64::DecodeFromBase64(pucData, pcData, stDataSizeInBytes);
    return bSuccess;
}
//---------------------------------------------------------------------------
void NiDOMTool::PushElement(TiXmlElement* pkElement)
{
    NIASSERT(pkElement);
    m_pkElementSet->Add(pkElement);
}
//---------------------------------------------------------------------------
unsigned int NiDOMTool::GetVersion() const
{
    return m_uiVersion;
}
//---------------------------------------------------------------------------
void NiDOMTool::SetVersion(unsigned int uiVersion)
{
    m_uiVersion = uiVersion;
}
//---------------------------------------------------------------------------
NiBool NiDOMTool::GetModified() const
{
    return m_bModified;
}
//---------------------------------------------------------------------------
void NiDOMTool::SetModified(bool bModified)
{
    m_bModified = bModified;
}
//---------------------------------------------------------------------------
int NiDOMTool::GetErrorID()
{
    return m_pkXMLDocument->ErrorId();
}
//---------------------------------------------------------------------------
const char* NiDOMTool::GetErrorDesc()
{
    return m_pkXMLDocument->ErrorDesc();
}
//---------------------------------------------------------------------------
int NiDOMTool::GetErrorRow()
{
    return m_pkXMLDocument->ErrorRow();
}
//---------------------------------------------------------------------------
int NiDOMTool::GetErrorCol()
{
    return m_pkXMLDocument->ErrorCol();
}
//---------------------------------------------------------------------------
