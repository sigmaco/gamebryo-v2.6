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

#ifndef NIMATERIALNODEXMLLIBRARYPARSER_H
#define NIMATERIALNODEXMLLIBRARYPARSER_H

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

class TiXmlElement;

class NiMaterialNodeXMLLibraryParser
{
public:
    bool Load(const char* pcXMLFilename, const char* pcOutputCPPFile, 
        const char* pcOutputHFile, const char* pcClassName,
        const char* pcModifier = NULL, const char* pcPCH = NULL);

protected:
    unsigned int uiFragmentCount;

    bool ProcessXML(FILE* pkOutputFile, TiXmlElement* pkElement);
    bool ProcessFragment(FILE* pkOutputFile, TiXmlElement* pkElement);
    bool ProcessResource(FILE* pkOutputFile, TiXmlElement* pkElement,
        bool bInput);
    bool ProcessCode(FILE* pkOutputFile, TiXmlElement* pkElement);
    bool ProcessDesc(FILE* pkOutputFile, TiXmlElement* pkElement);

    static void WriteHeaderFile(FILE* pkOutputFile, 
        const char* pcHeaderFileName, const char* pcClassName,
        const char* pcModifier);

    static void WriteFileHeader(FILE* pkOutputFile, 
        const char* pcHeaderFileName, const char* pcPCH);
    void WriteClassConstructor(FILE* pkOutputFile,
        const char* pcClassName, unsigned int uiVersion);

    void WriteFragmentHeader(FILE* pkOutputFile);
    static void WriteFragmentFooter(FILE* pkOutputFile);
    static void WriteFragmentType(FILE* pkOutputFile, const char* pcValue);
    static void WriteFragmentName(FILE* pkOutputFile, const char* pcValue);
    static void WriteFragmentDescription(FILE* pkOutputFile, 
        const char* pcValue);

    static void WriteResourceHeader(FILE* pkOutputFile, bool bInput);
    static void WriteResourceFooter(FILE* pkOutputFile, bool bInput);
    static void WriteResourceType(FILE* pkOutputFile, const char* pcValue);
    static void WriteResourceSemantic(FILE* pkOutputFile, const char* pcValue);
    static void WriteResourceLabel(FILE* pkOutputFile, const char* pcValue);
    static void WriteResourceVariable(FILE* pkOutputFile, const char* pcValue);
    static void WriteResourceDefaultValue(FILE* pkOutputFile, 
        const char* pcValue);
    static void WriteResourceCount(FILE* pkOutputFile, unsigned int uiCount);

    static void WriteCodeBlockHeader(FILE* pkOutputFile);
    static void WriteCodeBlockFooter(FILE* pkOutputFile);
    static void WriteCodeBlockLanguage(FILE* pkOutputFile, 
        const char* pcValue);
    static void WriteCodeBlockPlatform(FILE* pkOutputFile, 
        const char* pcValue);
    static void WriteCodeBlockTarget(FILE* pkOutputFile, const char* pcValue);
    static void WriteCodeBlockText(FILE* pkOutputFile, const char* pcValue);

    static bool WriteTextToFile(FILE* pkOutputFile, 
        const char* pcFormatString, const char* pcAdditionalString);
    static bool WriteTextToFile(FILE* pkOutputFile, 
        const char* pcFormatString, unsigned int uiNumber);
    static bool WriteTextToFile(FILE* pkOutputFile, 
        const char* pcText);

    static unsigned int CountInitialWhitespace(const char* pcString);
    static bool RemoveLineBreaks(const char* pcInput, unsigned int uiSpaces, 
        char* pcOutput, unsigned int& uiOutputSize);
};

#endif  //#ifndef NIMATERIALNODEXMLLIBRARYPARSER_H
