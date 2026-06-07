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
#include "NiMaterialNodeXMLLibraryReader.h"
#include <NiMaterialFragmentNode.h>

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#endif
#include <tinyxml.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

//---------------------------------------------------------------------------
NiMaterialNodeXMLLibraryReader::NiMaterialNodeXMLLibraryReader()
{
}
//---------------------------------------------------------------------------
NiMaterialNodeLibrary* NiMaterialNodeXMLLibraryReader::Load(
    const char* pcFilename)
{
    TiXmlBase::SetCondenseWhiteSpace(false);
    TiXmlDocument doc(pcFilename );
    NiMaterialNodeLibrary* pkLibrary = NULL;

    if (doc.LoadFile() == true)
    {
        TiXmlElement* pkRoot = doc.RootElement();
        ProcessXML(pkLibrary, pkRoot);
    }
    else if (doc.Error())
    {
        char acString[1024];
        NiOutputDebugString("\nERROR NiMaterialNodeXMLLibraryReader>\n");
        NiOutputDebugString("\tFile:");
        NiOutputDebugString(pcFilename);
        NiOutputDebugString("\n");
        NiSprintf(acString, 1024, "\tError ID: %d\n", doc.ErrorId());
        NiOutputDebugString(acString);
        NiOutputDebugString("\tDesc:");
        NiOutputDebugString(doc.ErrorDesc());
        NiOutputDebugString("\n");
        NiSprintf(acString, 1024, "\tRow:%d    Column: %d\n", doc.ErrorRow(),
            doc.ErrorCol());
        NiOutputDebugString(acString);
    }

    return pkLibrary;
}
//---------------------------------------------------------------------------
void NiMaterialNodeXMLLibraryReader::ProcessXML(NiMaterialNodeLibrary*& pkLib,
    TiXmlElement* pkElement)
{
    if (pkElement == NULL)
        return;

    const char* pcElementName = pkElement->Value();
    if (NiStricmp(pcElementName, "library") == 0)
    {
        unsigned int uiVersion = 0;

        TiXmlAttribute* pkAttrib = pkElement->FirstAttribute();
        if (pkAttrib && NiStricmp(pkAttrib->Name(), "version") == 0)
            uiVersion = (unsigned int) pkAttrib->IntValue();
        pkLib = NiNew NiMaterialNodeLibrary((unsigned short)uiVersion);

        TiXmlNode* pkChild = pkElement->IterateChildren( 0 );
        while( pkChild )
        {
            ProcessXML(pkLib, pkChild->ToElement());
            pkChild = pkElement->IterateChildren( pkChild );
        }
    }
    else if (NiStricmp(pcElementName, "fragment") == 0)
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();
        if (ProcessFragment(pkElement, pkFrag))
        {
            pkLib->AddNode(pkFrag);
        }
        return;
    }
}
//---------------------------------------------------------------------------
bool NiMaterialNodeXMLLibraryReader::ProcessFragment(TiXmlElement* pkElement, 
    NiMaterialFragmentNode* pkFrag)
{
    TiXmlAttribute* pkAttrib = pkElement->FirstAttribute();
    while (pkAttrib != NULL)
    {
        const char* pcName = pkAttrib->Name();
        const char* pcValue = pkAttrib->Value();

        char acString[256];
        NIASSERT(strlen(pcValue) < 256);
        NiStrcpy(acString, 256, pcValue);

        if (NiStricmp(pcName, "type") == 0)
        {
            pkFrag->SetType(pcValue);
        }
        else if (NiStricmp(pcName, "name") == 0)
        {
            pkFrag->SetName(pcValue);
        }

        pkAttrib = pkAttrib->Next();
    }

    if (pkElement->NoChildren())
        return false;

    
    TiXmlNode* pkChild = pkElement->IterateChildren( 0 );
    while( pkChild )
    {
        if (NiStricmp(pkChild->Value(), "inputs") == 0)
        {
            if (!ProcessResource(pkChild->ToElement(), pkFrag, true))
                return false;
        }
        if (NiStricmp(pkChild->Value(), "outputs") == 0)
        {
            if (!ProcessResource(pkChild->ToElement(), pkFrag, false))
                return false;
        }
        if (NiStricmp(pkChild->Value(), "code") == 0)
        {
            if (!ProcessCode(pkChild->ToElement(), pkFrag))
                return false;
        }
        if (NiStricmp(pkChild->Value(), "desc") == 0)
        {
            if (!ProcessDesc(pkChild->ToElement(), pkFrag))
                return false;
        }

        pkChild = pkElement->IterateChildren( pkChild );
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiMaterialNodeXMLLibraryReader::ProcessResource(TiXmlElement* pkElement, 
    NiMaterialNode* pkFrag, bool bInput)
{
    NiMaterialResourcePtr spRes = NULL;
    
    if (pkElement->NoChildren())
        return false;

    TiXmlNode* pkChild = pkElement->IterateChildren( 0 );
    while( pkChild )
    {
        if (NiStricmp(pkChild->Value(), "resource") == 0)
        {
            spRes = NiNew NiMaterialResource();
            TiXmlElement* pkChildElement = pkChild->ToElement();
            TiXmlAttribute* pkAttrib = pkChildElement->FirstAttribute();
            while (pkAttrib != NULL)
            {
                const char* pcName = pkAttrib->Name();
                const char* pcValue = pkAttrib->Value();
                char acString[256];
                NIASSERT(strlen(pcValue) < 256);
                NiStrcpy(acString, 256, pcValue);
                
                if (NiStricmp(pcName, "type")==0)
                {
                    spRes->SetType(pcValue);
                }
                else if (NiStricmp(pcName, "semantic") == 0)
                {
                    spRes->SetSemantic(pcValue);
                }
                else if (NiStricmp(pcName, "label") == 0)
                {
                    spRes->SetLabel(pcValue);
                }
                else if (NiStricmp(pcName, "variable") == 0)
                {
                    spRes->SetVariable(pcValue);
                }
                else if (NiStricmp(pcName, "default") == 0)
                {
                    spRes->SetDefaultValue(pcValue);
                }
                else if (NiStricmp(pcName, "count") == 0)
                {
                    if (pcValue != NULL && strlen(pcValue) != 0)
                        spRes->SetCount(pkAttrib->IntValue());
                }

                pkAttrib = pkAttrib->Next();
            }


            if (bInput)
                pkFrag->AddInputResource(spRes);
            else
                pkFrag->AddOutputResource(spRes);
        }
        else
        {
            return false;
        }

        pkChild = pkElement->IterateChildren( pkChild );
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiMaterialNodeXMLLibraryReader::ProcessCode(TiXmlElement* pkElement,
    NiMaterialFragmentNode* pkFrag)
{
    TiXmlAttribute* pkAttrib = pkElement->FirstAttribute();
    NiCodeBlock* pkBlock = NiNew NiCodeBlock();

    while (pkAttrib != NULL)
    {
        const char* pcName = pkAttrib->Name();
        const char* pcValue = pkAttrib->Value();

        char acString[256];
        NIASSERT(strlen(pcValue) < 256);
        NiStrcpy(acString, 256, pcValue);

        if (NiStricmp(pcName, "lang") == 0)
        {
            pkBlock->SetLanguage(pcValue);
        }
        else if (NiStricmp(pcName, "platform") == 0)
        {
            pkBlock->SetPlatform(pcValue);
        }
        else if (NiStricmp(pcName, "target") == 0)
        {
            pkBlock->SetTarget(pcValue);
        }

        pkAttrib = pkAttrib->Next();
    }

    if (pkElement->NoChildren())
        return false;

    TiXmlNode* pkCodeElement = pkElement->FirstChild();
    if (!pkCodeElement)
        return false;

    pkBlock->SetText(pkCodeElement->Value());
    pkFrag->AddCodeBlock(pkBlock);
    return true;
}
//---------------------------------------------------------------------------
bool NiMaterialNodeXMLLibraryReader::ProcessDesc(TiXmlElement* pkElement,
    NiMaterialFragmentNode* pkFrag)
{
    if (pkElement->NoChildren())
        return false;

    TiXmlNode* pkTextElement = pkElement->FirstChild();
    if (!pkTextElement)
        return false;

    pkFrag->SetDescription(pkTextElement->Value());
    return true;
}
//---------------------------------------------------------------------------
