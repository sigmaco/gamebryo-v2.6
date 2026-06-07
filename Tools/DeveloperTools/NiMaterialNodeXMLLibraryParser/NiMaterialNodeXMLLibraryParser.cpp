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
#include "NiMaterialNodeXMLLibraryParser.h"

#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#include <TinyXML.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

//---------------------------------------------------------------------------
bool NiMaterialNodeXMLLibraryParser::Load(const char* pcXMLFilename, 
    const char* pcOutputCPPFile, const char* pcOutputHFile, 
    const char* pcClassName, const char* pcModifier,
    const char* pcPCH)
{
    uiFragmentCount = 0;


    TiXmlBase::SetCondenseWhiteSpace(false);
    TiXmlDocument kDoc(pcXMLFilename);

    if (kDoc.LoadFile() == true)
    {
        // Figure out the name of the header file without the entire path
        const char* pcFilename = pcOutputHFile + strlen(pcOutputHFile) - 1;
        while (pcFilename > pcOutputHFile)
        {
            if (*pcFilename == '\\' || *pcFilename == '/')
            {
                assert (pcFilename != 
                    pcOutputHFile + strlen(pcOutputHFile) - 1);
                pcFilename++;
                break;
            }
            pcFilename--;
        }

        // Create the header file
        FILE* pkOutHFile = fopen(pcOutputHFile, "wt");
        if (pkOutHFile == NULL)
        {
            fprintf(stderr, "Error: file %s failed to be created.\n\n", 
                pcOutputHFile);
            return false;
        }
        WriteHeaderFile(pkOutHFile, pcFilename, pcClassName, pcModifier);
        fclose(pkOutHFile);

        // Create the source file
        FILE* pkOutCPPFile = fopen(pcOutputCPPFile, "wt");
        if (pkOutCPPFile == NULL)
        {
            fprintf(stderr, "Error: file %s failed to be created.\n\n", 
                pcOutputCPPFile);
            return false;
        }

        TiXmlElement* pkRoot = kDoc.RootElement();
        TiXmlAttribute* pkAttrib = pkRoot->FirstAttribute();
        unsigned int uiVersion = 0;

        if (pkAttrib && _stricmp(pkAttrib->Name(), "version") == 0)
            uiVersion = (unsigned int) pkAttrib->IntValue();

        WriteFileHeader(pkOutCPPFile, pcFilename, pcPCH);

        if (!ProcessXML(pkOutCPPFile, pkRoot))
            return false;

        WriteClassConstructor(pkOutCPPFile, pcClassName, uiVersion);
    
        fclose(pkOutCPPFile);
    }
    else if (kDoc.Error())
    {
        fputs("\nERROR NiMaterialNodeXMLLibraryParser>\n", stderr);
        fprintf(stderr, "\tFile:%s\n", pcXMLFilename);
        fprintf(stderr, "\tError ID: %d\n", kDoc.ErrorId());
        fprintf(stderr, "\tDesc:%s\n", kDoc.ErrorDesc());
        fprintf(stderr, "\tRow:%d    Column: %d\n", kDoc.ErrorRow(),
            kDoc.ErrorCol());

        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiMaterialNodeXMLLibraryParser::ProcessXML(FILE* pkOutputFile,
    TiXmlElement* pkElement)
{
    if (pkElement == NULL)
        return true;

    const char* pcElementName = pkElement->Value();
    if (_stricmp(pcElementName, "library") == 0)
    {
        TiXmlNode* pkChild = pkElement->IterateChildren( 0 );
        while( pkChild )
        {
            if (!ProcessXML(pkOutputFile, pkChild->ToElement()))
                return false;
            pkChild = pkElement->IterateChildren( pkChild );
        }
    }
    else if (_stricmp(pcElementName, "fragment") == 0)
    {
        if (!ProcessFragment(pkOutputFile, pkElement))
            return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiMaterialNodeXMLLibraryParser::ProcessFragment(FILE* pkOutputFile,
    TiXmlElement* pkElement)
{
    WriteFragmentHeader(pkOutputFile);

    TiXmlAttribute* pkAttrib = pkElement->FirstAttribute();
    while (pkAttrib != NULL)
    {
        const char* pcName = pkAttrib->Name();
        const char* pcValue = pkAttrib->Value();

        if (_stricmp(pcName, "type") == 0)
        {
            WriteFragmentType(pkOutputFile, pcValue);
        }
        else if (_stricmp(pcName, "name") == 0)
        {
            WriteFragmentName(pkOutputFile, pcValue);
        }

        pkAttrib = pkAttrib->Next();
    }

    if (pkElement->NoChildren())
        return false;

    TiXmlNode* pkChild = pkElement->IterateChildren( 0 );
    while( pkChild )
    {
        if (_stricmp(pkChild->Value(), "inputs") == 0)
        {
            if (!ProcessResource(pkOutputFile, pkChild->ToElement(), true))
                return false;
        }
        if (_stricmp(pkChild->Value(), "outputs") == 0)
        {
            if (!ProcessResource(pkOutputFile, pkChild->ToElement(), false))
                return false;
        }
        if (_stricmp(pkChild->Value(), "code") == 0)
        {
            if (!ProcessCode(pkOutputFile, pkChild->ToElement()))
                return false;
        }
        if (_stricmp(pkChild->Value(), "desc") == 0)
        {
            if (!ProcessDesc(pkOutputFile, pkChild->ToElement()))
                return false;
        }

        pkChild = pkElement->IterateChildren( pkChild );
    }

    WriteFragmentFooter(pkOutputFile);

    return true;
}
//---------------------------------------------------------------------------
bool NiMaterialNodeXMLLibraryParser::ProcessResource(FILE* pkOutputFile,
    TiXmlElement* pkElement, bool bInput)
{
    if (pkElement->NoChildren())
        return false;

    TiXmlNode* pkChild = pkElement->IterateChildren( 0 );
    while( pkChild )
    {
        if (_stricmp(pkChild->Value(), "resource") == 0)
        {
            WriteResourceHeader(pkOutputFile, bInput);

            TiXmlElement* pkChildElement = pkChild->ToElement();
            TiXmlAttribute* pkAttrib = pkChildElement->FirstAttribute();
            while (pkAttrib != NULL)
            {
                const char* pcName = pkAttrib->Name();
                const char* pcValue = pkAttrib->Value();

                if (_stricmp(pcName, "type")==0)
                {
                    WriteResourceType(pkOutputFile, pcValue);
                }
                else if (_stricmp(pcName, "semantic") == 0)
                {
                    WriteResourceSemantic(pkOutputFile, pcValue);
                }
                else if (_stricmp(pcName, "label") == 0)
                {
                    WriteResourceLabel(pkOutputFile, pcValue);
                }
                else if (_stricmp(pcName, "variable") == 0)
                {
                    WriteResourceVariable(pkOutputFile, pcValue);
                }
                else if (_stricmp(pcName, "default") == 0)
                {
                    WriteResourceDefaultValue(pkOutputFile, pcValue);
                }
                else if (_stricmp(pcName, "count") == 0)
                {
                    if (pcValue != NULL && strlen(pcValue) != 0)
                        WriteResourceCount(pkOutputFile, pkAttrib->IntValue());
                }

                pkAttrib = pkAttrib->Next();
            }
        
            WriteResourceFooter(pkOutputFile, bInput);
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
bool NiMaterialNodeXMLLibraryParser::ProcessCode(FILE* pkOutputFile,
    TiXmlElement* pkElement)
{
    WriteCodeBlockHeader(pkOutputFile);

    TiXmlAttribute* pkAttrib = pkElement->FirstAttribute();

    while (pkAttrib != NULL)
    {
        const char* pcName = pkAttrib->Name();
        const char* pcValue = pkAttrib->Value();

        if (_stricmp(pcName, "lang") == 0)
        {
            WriteCodeBlockLanguage(pkOutputFile, pcValue);
        }
        else if (_stricmp(pcName, "platform") == 0)
        {
            WriteCodeBlockPlatform(pkOutputFile, pcValue);
        }
        else if (_stricmp(pcName, "target") == 0)
        {
            WriteCodeBlockTarget(pkOutputFile, pcValue);
        }

        pkAttrib = pkAttrib->Next();
    }

    if (pkElement->NoChildren())
        return false;

    TiXmlNode* pkCodeElement = pkElement->FirstChild();
    if (!pkCodeElement)
        return false;

    WriteCodeBlockText(pkOutputFile, pkCodeElement->Value());

    WriteCodeBlockFooter(pkOutputFile);

    return true;
}
//---------------------------------------------------------------------------
bool NiMaterialNodeXMLLibraryParser::ProcessDesc(FILE* pkOutputFile, 
    TiXmlElement* pkElement)
{
    if (pkElement->NoChildren())
        return false;

    TiXmlNode* pkTextElement = pkElement->FirstChild();
    if (!pkTextElement)
        return false;

    WriteFragmentDescription(pkOutputFile, pkTextElement->Value());
    return true;
}
//---------------------------------------------------------------------------
void NiMaterialNodeXMLLibraryParser::WriteHeaderFile(FILE* pkOutputFile, 
    const char* pcHeaderFileName, const char* pcClassName,
    const char* pcModifier)
{
    // Create a capitalized version of the header name
    assert (pcHeaderFileName && *pcHeaderFileName);
    size_t stStrLen = strlen(pcHeaderFileName) + 1;
    char* pcCapHeaderFileName = new char[stStrLen];
    for (unsigned int i = 0; i < stStrLen; i++)
    {
        if (pcHeaderFileName[i] == '.')
            pcCapHeaderFileName[i] = '_';
        else if (pcHeaderFileName[i] == '\0')
            pcCapHeaderFileName[i] = '\0';
        else
            pcCapHeaderFileName[i] = (char)toupper(pcHeaderFileName[i]);
    }

    WriteTextToFile(pkOutputFile,
        "// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION\n"
        "//\n"
        "// This software is supplied under the terms of a license agreement "
        "or\n"
        "// nondisclosure agreement with Emergent Game Technologies and may "
        "not \n"
        "// be copied or disclosed except in accordance with the terms of "
        "that \n"
        "// agreement.\n"
        "//\n"
        "//      Copyright (c) 1996-2008 Emergent Game Technologies.\n"
        "//      All Rights Reserved.\n"
        "//\n"
        "// Emergent Game Technologies, Chapel Hill, North Carolina 27517\n"
        "// http://www.emergent.net\n"
        "\n"
        "//-------------------------------------------------------------------"
        "--------\n"
        "// This file has been automatically generated using the\n"
        "// NiMaterialNodeXMLLibraryParser tool. It should not be directly "
        "edited.\n"
        "//-------------------------------------------------------------------"
        "--------\n"
        "\n"
        "#ifndef %s\n", pcCapHeaderFileName);

    WriteTextToFile(pkOutputFile,
        "#define %s\n"
        "\n", pcCapHeaderFileName);

    WriteTextToFile(pkOutputFile,
        "class NiMaterialNodeLibrary;\n"
        "\n"
        "class ");

    if (pcModifier)
    {
        WriteTextToFile(pkOutputFile, "%s ", pcModifier);
    }

    WriteTextToFile(pkOutputFile,
        "%s\n"
        "{\n"
        "public:\n"
        "    static NiMaterialNodeLibrary* CreateMaterialNodeLibrary();\n"
        "};\n"
        "\n"
        "#endif\n", pcClassName);
}
//---------------------------------------------------------------------------
void NiMaterialNodeXMLLibraryParser::WriteFileHeader(FILE* pkOutputFile, 
    const char* pcHeaderFileName, const char* pcPCH)
{
    // Find just the file name for the header file

    WriteTextToFile(pkOutputFile, 
        "// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION\n"
        "//\n"
        "// This software is supplied under the terms of a license agreement "
        "or\n"
        "// nondisclosure agreement with Emergent Game Technologies and may "
        "not \n"
        "// be copied or disclosed except in accordance with the terms of "
        "that \n"
        "// agreement.\n"
        "//\n"
        "//      Copyright (c) 1996-2008 Emergent Game Technologies.\n"
        "//      All Rights Reserved.\n"
        "//\n"
        "// Emergent Game Technologies, Chapel Hill, North Carolina 27517\n"
        "// http://www.emergent.net\n"
        "\n"
        "//-------------------------------------------------------------------"
        "--------\n"
        "// This file has been automatically generated using the\n"
        "// NiMaterialNodeXMLLibraryParser tool. It should not be directly "
        "edited.\n"
        "//-------------------------------------------------------------------"
        "--------\n"
        "\n");

    if (pcPCH)
    {
        WriteTextToFile(pkOutputFile,
            "#include \"%s\"\n"
            "\n", pcPCH);
    }

    WriteTextToFile(pkOutputFile,
        "#include <NiMaterialFragmentNode.h>\n"
        "#include <NiMaterialNodeLibrary.h>\n"
        "#include <NiMaterialResource.h>\n"
        "#include <NiCodeBlock.h>\n"
        "#include \"%s\"\n"
        "\n", pcHeaderFileName);
}
//---------------------------------------------------------------------------
void NiMaterialNodeXMLLibraryParser::WriteClassConstructor(FILE* pkOutputFile, 
    const char* pcClassName, unsigned int uiVersion)
{
    WriteTextToFile(pkOutputFile, "//-----------------------------------------"
        "----------------------------------\n");

    if (strlen(pcClassName) < 28)
    {
        WriteTextToFile(pkOutputFile, 
            "NiMaterialNodeLibrary* %s::CreateMaterialNodeLibrary()\n", 
            pcClassName);
    }
    else
    {
        WriteTextToFile(pkOutputFile, 
            "NiMaterialNodeLibrary* \n"
            "    %s::CreateMaterialNodeLibrary()\n", pcClassName);
    } 
    WriteTextToFile(pkOutputFile,
        "{\n"
        "    // Create a new NiMaterialNodeLibrary\n"
        "    NiMaterialNodeLibrary* pkLib = NiNew NiMaterialNodeLibrary(");

    char acString[64];
    sprintf(acString, "%d", uiVersion);
    
    WriteTextToFile(pkOutputFile, acString);
    WriteTextToFile(pkOutputFile, 
        ");\n"
        "\n");


    // Call each of the fragment creators
    for (unsigned int ui = 0; ui < uiFragmentCount; ++ui)
    {
        WriteTextToFile(pkOutputFile, "    CreateFragment%d(pkLib);\n", ui);
    }

    WriteTextToFile(pkOutputFile, 
        "\n"
        "    return pkLib;\n"
        "}\n"
        "//-------------------------------------------------------------------"
        "--------\n"
        "\n");
}
//---------------------------------------------------------------------------
void NiMaterialNodeXMLLibraryParser::WriteFragmentHeader(FILE* pkOutputFile)
{
    // NI_NOINLINE is needed on PS3, or else the compiler's optimizer gets
    // overwhelmed by large fragment material source files.
    WriteTextToFile(pkOutputFile, 
        "//-------------------------------------------------------------------"
        "--------\n"
        "NI_NOINLINE static void CreateFragment%d(NiMaterialNodeLibrary* pkLib)\n"
        "{\n"
        "    NiMaterialFragmentNode* pkFrag = "
            "NiNew NiMaterialFragmentNode();\n"
        "\n", uiFragmentCount);
    ++uiFragmentCount;
}
//---------------------------------------------------------------------------
void NiMaterialNodeXMLLibraryParser::WriteFragmentFooter(FILE* pkOutputFile)
{
    WriteTextToFile(pkOutputFile, 
        "    pkLib->AddNode(pkFrag);\n"
        "}\n");
}
//---------------------------------------------------------------------------
void NiMaterialNodeXMLLibraryParser::WriteFragmentType(FILE* pkOutputFile, 
    const char* pcValue)
{
    WriteTextToFile(pkOutputFile,
        "    pkFrag->SetType(\"%s\");\n", pcValue);
}
//---------------------------------------------------------------------------
void NiMaterialNodeXMLLibraryParser::WriteFragmentName(FILE* pkOutputFile, 
    const char* pcValue)
{
    WriteTextToFile(pkOutputFile,
        "    pkFrag->SetName(\"%s\");\n", pcValue);
}
//---------------------------------------------------------------------------
void NiMaterialNodeXMLLibraryParser::WriteFragmentDescription(
    FILE* pkOutputFile, const char* pcValue)
{
    WriteTextToFile(pkOutputFile,
        "    pkFrag->SetDescription(\"%s\");\n", pcValue);
}
//---------------------------------------------------------------------------
void NiMaterialNodeXMLLibraryParser::WriteResourceHeader(FILE* pkOutputFile, 
    bool bInput)
{
    if (bInput)
    {
        WriteTextToFile(pkOutputFile, 
            "    // Insert an input resource\n"
            "    {\n"
            "        NiMaterialResource* pkRes = NiNew "
            "NiMaterialResource();\n"
            "\n");
    }
    else
    {
        WriteTextToFile(pkOutputFile, 
            "    // Insert an output resource\n"
            "    {\n"
            "        NiMaterialResource* pkRes = NiNew "
            "NiMaterialResource();\n"
            "\n");
    }
}
//---------------------------------------------------------------------------
void NiMaterialNodeXMLLibraryParser::WriteResourceFooter(FILE* pkOutputFile, 
    bool bInput)
{
    if (bInput)
    {
        WriteTextToFile(pkOutputFile, 
            "\n"
            "        pkFrag->AddInputResource(pkRes);\n"
            "    }\n"
            "\n");
    }
    else
    {
        WriteTextToFile(pkOutputFile, 
            "\n"
            "        pkFrag->AddOutputResource(pkRes);\n"
            "    }\n"
            "\n");
    }
}
//---------------------------------------------------------------------------
void NiMaterialNodeXMLLibraryParser::WriteResourceType(FILE* pkOutputFile, 
    const char* pcValue)
{
    WriteTextToFile(pkOutputFile,
        "        pkRes->SetType(\"%s\");\n", pcValue);
}
//---------------------------------------------------------------------------
void NiMaterialNodeXMLLibraryParser::WriteResourceSemantic(FILE* pkOutputFile, 
    const char* pcValue)
{
    WriteTextToFile(pkOutputFile,
        "        pkRes->SetSemantic(\"%s\");\n", pcValue);
}
//---------------------------------------------------------------------------
void NiMaterialNodeXMLLibraryParser::WriteResourceLabel(FILE* pkOutputFile, 
    const char* pcValue)
{
    WriteTextToFile(pkOutputFile,
        "        pkRes->SetLabel(\"%s\");\n", pcValue);
}
//---------------------------------------------------------------------------
void NiMaterialNodeXMLLibraryParser::WriteResourceVariable(FILE* pkOutputFile, 
    const char* pcValue)
{
    WriteTextToFile(pkOutputFile,
        "        pkRes->SetVariable(\"%s\");\n", pcValue);
}
//---------------------------------------------------------------------------
void NiMaterialNodeXMLLibraryParser::WriteResourceDefaultValue(
    FILE* pkOutputFile, const char* pcValue)
{
    WriteTextToFile(pkOutputFile,
        "        pkRes->SetDefaultValue(\"%s\");\n", pcValue);
}
//---------------------------------------------------------------------------
void NiMaterialNodeXMLLibraryParser::WriteResourceCount(FILE* pkOutputFile, 
    unsigned int uiCount)
{
    WriteTextToFile(pkOutputFile,
        "        pkRes->SetCount(%d);\n", uiCount);
}
//---------------------------------------------------------------------------
void NiMaterialNodeXMLLibraryParser::WriteCodeBlockHeader(FILE* pkOutputFile)
{
    WriteTextToFile(pkOutputFile,
        "    // Insert a code block\n"
        "    {\n"
        "        NiCodeBlock* pkBlock = NiNew NiCodeBlock();\n"
        "\n");
}
//---------------------------------------------------------------------------
void NiMaterialNodeXMLLibraryParser::WriteCodeBlockFooter(FILE* pkOutputFile)
{
    WriteTextToFile(pkOutputFile,
        "\n"
        "        pkFrag->AddCodeBlock(pkBlock);\n"
        "    }\n"
        "\n");
}
//---------------------------------------------------------------------------
void NiMaterialNodeXMLLibraryParser::WriteCodeBlockLanguage(FILE* pkOutputFile,
    const char* pcValue)
{
    WriteTextToFile(pkOutputFile,
        "        pkBlock->SetLanguage(\"%s\");\n", pcValue);
}
//---------------------------------------------------------------------------
void NiMaterialNodeXMLLibraryParser::WriteCodeBlockPlatform(FILE* pkOutputFile,
    const char* pcValue)
{
    WriteTextToFile(pkOutputFile,
        "        pkBlock->SetPlatform(\"%s\");\n", pcValue);
}
//---------------------------------------------------------------------------
void NiMaterialNodeXMLLibraryParser::WriteCodeBlockTarget(FILE* pkOutputFile, 
    const char* pcValue)
{
    WriteTextToFile(pkOutputFile,
        "        pkBlock->SetTarget(\"%s\");\n", pcValue);
}
//---------------------------------------------------------------------------
void NiMaterialNodeXMLLibraryParser::WriteCodeBlockText(FILE* pkOutputFile, 
    const char* pcValue)
{
    WriteTextToFile(pkOutputFile,
        "\n"
        "        pkBlock->SetText(\"%s\");\n"
        "\n", pcValue);
}
//---------------------------------------------------------------------------
bool NiMaterialNodeXMLLibraryParser::WriteTextToFile(FILE* pkOutputFile, 
    const char* pcFormatString, const char* pcAdditionalString)
{
    if (pcAdditionalString == NULL || *pcAdditionalString == '\0')
        return false;

    unsigned int uiSpaces = CountInitialWhitespace(pcFormatString);

    unsigned int uiSize = 0;
    bool bSuccess = RemoveLineBreaks(pcAdditionalString, uiSpaces, NULL, 
        uiSize);
    if (!bSuccess || uiSize == 0)
        return false;

    char* pcNewValue = new char[uiSize];
    bSuccess = RemoveLineBreaks(pcAdditionalString, uiSpaces, pcNewValue, 
        uiSize);
    assert (bSuccess);

    fprintf(pkOutputFile, pcFormatString, pcNewValue);

    delete[] pcNewValue;
    return true;
}
//---------------------------------------------------------------------------
bool NiMaterialNodeXMLLibraryParser::WriteTextToFile(FILE* pkOutputFile, 
     const char* pcFormatString, unsigned int uiNumber)
{
    fprintf(pkOutputFile, pcFormatString, uiNumber);
    return true;
}
//---------------------------------------------------------------------------
bool NiMaterialNodeXMLLibraryParser::WriteTextToFile(FILE* pkOutputFile, 
    const char* pcText)
{
    fputs(pcText, pkOutputFile);
    return true;
}
//---------------------------------------------------------------------------
unsigned int NiMaterialNodeXMLLibraryParser::CountInitialWhitespace(
    const char* pcString)
{
    unsigned int uiWhitespace = 0;
    const char* pcIterator = pcString;
    while (*pcIterator != '\0' && isspace(*pcIterator))
    {
        if (*pcIterator == '\t')
            uiWhitespace += 4;
        else
            uiWhitespace++;
        pcIterator++;
    }
    return uiWhitespace;
}
//---------------------------------------------------------------------------
bool NiMaterialNodeXMLLibraryParser::RemoveLineBreaks(const char* pcInput, 
    unsigned int uiSpaces, char* pcOutput, unsigned int& uiOutputSize)
{
    // Initialize uiOutputSize to 0 if it needs to be filled in 
    if (pcOutput == NULL)
        uiOutputSize = 0;

    if (pcInput == NULL || *pcInput == '\0')
        return false;

    unsigned int uiFilledInSize = 0;

    unsigned int uiIndentedSpaces = uiSpaces + 4;

    const char* pcInIterator = pcInput;
    char* pcOutIterator = pcOutput;
    unsigned int uiLineLength = 0;
    while (*pcInIterator != '\0')
    {
        bool bNewlineCharacter = (*pcInIterator == '\n');
        if (bNewlineCharacter || uiLineLength > 74)
        {
            if (bNewlineCharacter)
            {
                // Increase size by 2 for adding in a newline character
                uiFilledInSize += 2;
            }

            // Increase size by 3 + uiIndentedSpaces: 
            //   1 for adding " before the line break
            //   1 for line break itself
            //   (uiIndentedSpaces) for indentation
            //   1 for adding initial " on the new line
            uiFilledInSize += 3 + uiIndentedSpaces;

            // Re-initialize the new line length to 1 + uiIndentedSpaces:
            //   (uiIndentedSpaces) for indentation
            //   1 for adding initial " on the new line
            uiLineLength = 1 + uiIndentedSpaces;
            if (pcOutput)
            {
                // Check for overrunning the input buffer
                if (uiFilledInSize > uiOutputSize)
                    return false;

                if (bNewlineCharacter)
                {
                    *pcOutIterator++ = '\\';
                    *pcOutIterator++ = 'n';
                }
                *pcOutIterator++ = '"';
                *pcOutIterator++ = '\n';
                for (unsigned int j = 0; j < uiIndentedSpaces; j++)
                    *pcOutIterator++ = ' ';
                *pcOutIterator++ = '"';
            }

            // If we did a newline because of an actual newline and not
            // because of an end-of-line issue, then advance the input
            if (bNewlineCharacter)
                pcInIterator++;
        }
        else if (*pcInIterator == '\t')
        {
            // Increase size by 4:
            //   4 spaces in a tab
            uiFilledInSize += 4;
            uiLineLength += 4;
            if (pcOutput)
            {
                // Check for overrunning the input buffer
                if (uiFilledInSize > uiOutputSize)
                    return false;
                *pcOutIterator++ = ' ';
                *pcOutIterator++ = ' ';
                *pcOutIterator++ = ' ';
                *pcOutIterator++ = ' ';
            }
            *pcInIterator++;
        }
        else if (*pcInIterator == '"')
        {
            // Increase size by 2:
            //   1 for \ before quote
            //   1 for quote itself
            uiFilledInSize += 2;
            uiLineLength += 2;
            if (pcOutput)
            {
                // Check for overrunning the input buffer
                if (uiFilledInSize > uiOutputSize)
                    return false;
                *pcOutIterator++ = '\\';
                *pcOutIterator++ = '"';
            }
            *pcInIterator++;
        }
        else if (*pcInIterator == '\\')
        {
            // Increase size by 2:
            //   1 for \ before backslash
            //   1 for backslash itself
            uiFilledInSize += 2;
            uiLineLength += 2;
            if (pcOutput)
            {
                // Check for overrunning the input buffer
                if (uiFilledInSize > uiOutputSize)
                    return false;
                *pcOutIterator++ = '\\';
                *pcOutIterator++ = '\\';
            }
            *pcInIterator++;
        }
        else
        {
            assert (*pcInIterator != '\0');

            uiFilledInSize++;
            uiLineLength++;
            if (pcOutput)
            {
                // Check for overrunning the input buffer
                if (uiFilledInSize > uiOutputSize)
                    return false;

                *pcOutIterator++ = *pcInIterator++;
            }
            else
            {
                pcInIterator++;
            }
        }
    }

    // Null terminate
    uiFilledInSize++;

    if (pcOutput)
    {
        // Check for overrunning the input buffer
        if (uiFilledInSize > uiOutputSize)
            return false;

        *pcOutIterator = '\0';
    }
    else
    {
        uiOutputSize = uiFilledInSize;
    }

    return true;
}
//---------------------------------------------------------------------------
