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

#ifndef NIUNICODEFONTCFG_H
#define NIUNICODEFONTCFG_H

#include <NiRTTI.h>
#include <NiMemObject.h>
#include <NiTArray.h>

#define NIUNICODEFONTCFG_NAMELEN 64
#define NIUNICODEFONTCFG_PATHLEN 512

class NiUnicodeFontCFG : public NiMemObject
{
    //NiDeclareRootRTTI(NiUnicodeFontCFG);  // link error if add this!

public:
    NiUnicodeFontCFG();
    virtual ~NiUnicodeFontCFG();

    // definition used in thie class
    enum FontStyle
    {
        FONTSTYLE_NONE          = 0x00, // 0000 0000
        FONTSTYLE_BOLD          = 0x01, // 0000 0001
        FONTSTYLE_ITALIC        = 0x02, // 0000 0010
        FONTSTYLE_STRIKEOUT     = 0x04, // 0000 0100
        FONTSTYLE_UNDERLINE     = 0x08, // 0000 1000
        FONTSTYLE_ANTIALIASED   = 0x10, // 0001 0000
    };

    //class UnicodeFontCfgHeader : public NiMemObject
    class UnicodeFontCfgHeader
    {
    public:
        UnicodeFontCfgHeader();

        // Methods
        void    SetFontStyle(const char* pcStyle);
        void    SetFontStyle(unsigned short* pusStyle);

        unsigned int    m_uiVersion;            // font CFG file version number
        char            m_acFontName[NIUNICODEFONTCFG_NAMELEN];
        unsigned short  m_usFontSize;
        unsigned short  m_usFontStyle;
    };

    //class CharData : public NiMemObject
    class CharData
    {
    public:
        CharData();
        CharData(unsigned int p, unsigned short c)
            { m_uiPriority = p; m_usChar = c; }

        unsigned int    m_uiPriority;   // the number of usages
        unsigned short  m_usChar;       // Unicode encoding value
    };

    // inlines
    int         GetNumOfUniqueCharacters() const;
    const char* GetFontName() const;
    void        SetHeader(const UnicodeFontCfgHeader& header);
    const UnicodeFontCfgHeader& GetHeader() const;
    unsigned short  GetCharEncoding(unsigned int uiIdex) const;

    // methods
    void        Reset();
    bool        LoadFontCFG(const char* fileName, BOOL bAddDefaultASCII);
    bool        LoadFontCFG(FILE* pkFile);
    void        ProcessFontCFG();
    void        SortUniqueData();
    void        AccumulateCharacter(unsigned short charCoding);

protected:

    bool CharExists(unsigned short usChar) const;

    // This file name is going to be updated if load from a new file.
    // If the file is same, won't load from it.
    char m_pcCurrentFontCfgFileName[NIUNICODEFONTCFG_PATHLEN];
    UnicodeFontCfgHeader        m_kHeader;

    // lines of Unicode string
    NiTPrimitiveArray<unsigned short *> m_kRawData;

    // sorted unique characters.
    NiTPrimitiveArray<CharData*>    m_kSortedData;
};

typedef NiPointer<NiUnicodeFontCFG> NiUnicodeFontCFGPtr;

#include "NiUnicodeFontCFG.inl"

#endif //#ifndef NIUNICODEFONTCFG_H

