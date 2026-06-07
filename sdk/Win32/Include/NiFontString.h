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

#ifndef NIFONTSTRING_H
#define NIFONTSTRING_H

#include "NiFontLibType.h"
#include <NiRefObject.h>
#include <NiRTTI.h>
#include <NiColor.h>
#include "NiFont.h"

class NIFONT_ENTRY NiFontString : public NiRefObject
{
    NiDeclareRootRTTI(NiFontString);

public:
    enum Flags
    {
        COLORED     = 0x00000001,
        CENTERED    = 0x00000002
    };

    enum 
    {
        NIFONTSTRING_LEFT_TO_RIGHT = 0,
        NIFONTSTRING_RIGHT_TO_LEFT,
        NIFONTSTRING_TOP_TO_BOTTOM,
        NIFONTSTRING_BOTTOM_TO_TOP
    };


protected:
    // ASCII Constructor
    NiFontString(NiFont* pkFont, 
        unsigned int uiFlags,
        unsigned int uiMaxStringLen, 
        const char* pcString,
        const NiColorA& kColor,
        unsigned char ucDirection = NIFONTSTRING_LEFT_TO_RIGHT,
        bool bStatic = false);

    // Unicode Constructor
    NiFontString(NiFont* pkFont, 
        unsigned int uiFlags,
        unsigned int uiMaxStringLen, 
        const NiWChar* pkString,
        const NiColorA& kColor,
        unsigned char ucDirection = NIFONTSTRING_LEFT_TO_RIGHT,
        bool bStatic = false);

public:
    virtual ~NiFontString();

    // Text
    void SetText(const char* pcText, unsigned int uiFlags = 0xffffffff);
    inline const char* GetText();

    void SetText(const NiWChar* pkText, unsigned int uiFlags = 0xffffffff);
    inline const NiWChar* GetUnicodeText();

    // Color
    virtual void SetColor(const NiColorA& kColor);
    inline const NiColorA& GetColor() const;

    // Get and set the direction the string will be drawn
    inline unsigned char GetDirection() const;
    inline void SetDirection(unsigned char ucDirection);
    /// Returns the leading (inter-line spacing) used in this string,
    /// in pixels.
    inline float GetLeading() const;
    /// Specifies the leading (inter-line spacing) to be used in this
    /// string, in pixels.
    inline void SetLeading(float fLeading);

    /// Returns the tracking (constant inter-character spacing) used
    /// in this string, in pixels.
    inline float GetTracking() const;
    /// Specifies the tracking (constant inter-character spacing) used
    /// in this string, in pixels.
    inline void SetTracking(float fTracking);

    //*** begin Emergent internal use only
    /// Divides string into series of lines separated by \n; caches
    /// line length and size. Must be called if tracking or leading
    /// changes to update that cache.
    void SplitString();
    void ShutdownString();
    //*** end Emergent internal use only

protected:
    NiFontPtr m_spFont;
    unsigned int m_uiFlags;
    unsigned int m_uiMaxStringLen;
    unsigned int m_uiStringLen;

    NiWChar* m_pkString;
    char* m_pcString;
    NiColorA m_kColor;
    unsigned char m_ucDirection;
    /// extra space between lines, in pixels - defaults to 0
    float m_fLeading;
    /// extra space between characters, in pixels - defaults to 0
    float m_fTracking;

    // *** begin Emergent internal use only *** //
    /// number of lines ('\n'-separated)
    unsigned int m_uiStringCount;
    /// width of each line, in pixels
    float* m_pfStringWidth;
    /// length of each line, in char/wchar
    unsigned int* m_puiStringLength;
    /// width of widest line, in pixels
    float m_fStringWidth;
    /// height of string (all lines), in pixels
    float m_fStringHeight;
    // Pixel measurements are at the scale that the font was loaded,
    // *before* any scaling that may be applied by NiFontString::m_usPointSize

    bool m_bModified;
    bool m_bStatic;

    // Be careful with threading when using the working string!
    static NiWChar ms_akWorkingString[NI_MAX_PATH];

    // *** end Emergent internal use only *** //
};

typedef NiPointer<NiFontString> NiFontStringPtr;

#include "NiFontString.inl"

#endif //#ifndef NIFONTSTRING_H
