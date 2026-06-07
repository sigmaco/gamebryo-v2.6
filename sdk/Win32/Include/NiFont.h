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

#ifndef NIFONT_H
#define NIFONT_H

#include <NiRTTI.h>
#include <NiRefObject.h>
#include <NiStream.h>
#include <NiColor.h>
#include <NiRenderer.h>
#include <NiPixelData.h>
#include <NiSourceTexture.h>

#include "NiFontLibType.h"

class NIFONT_ENTRY NiFont : public NiRefObject
{
    NiDeclareRootRTTI(NiFont);
    NiDeclareViewerStrings;

protected:
    NiFont(NiRenderer* pkRenderer);

public:

    ~NiFont();

    static NiFont* Create(NiRenderer* pkRenderer, const char* pcFontFile);

    enum Error
    {
        ERR_OK,
        ERR_BADFILE = 0x80000001,
        ERR_INVALIDVERSION,
        ERR_INVALIDFONT,
        ERR_CREATEFAILED,
        ERR_PIXELDATAFAILED
    };

    enum TextDirection
    {
        NIFONT_LEFT_TO_RIGHT = 0,
        NIFONT_RIGHT_TO_LEFT,
        NIFONT_TOP_TO_BOTTOM,
        NIFONT_BOTTOM_TO_TOP
    };

    // Convert the ASCII string to Unicode using the static internal
    // buffer. THIS IS NOT THREAD SAFE AND MULTIPLE CALLS TO THIS
    // FUNCTION ARE DESTRUCTIVE. If you call this function again
    // before using the returned NiWChar* then the buffer will
    // be overwritten by the second call.
    static NiWChar* AsciiToUnicode(const char* pcBuffer, 
        unsigned int uiBufferSize);

    // Convert the ASCII string to Unicode. This version is thread 
    // safe and non-destructive. 
    static void AsciiToUnicode(const char* pcBuffer, unsigned int uiBufferSize,
        NiWChar* pkUnicodeBuffer, unsigned int uiUnicodeBufferSize);


    inline bool IsBold() const;
    /// Returns true if this is an italic font. Note that some fonts
    /// may look like "italic" fonts and have all the characteristic
    /// features - e.g. Lucida Handwriting.
    inline bool IsItalic() const;
    inline bool IsStrikeOut() const;
    inline bool IsUnderline() const;
    inline bool IsAntiAliased() const;


    //  Get Size, etc.
    // ASCII interfaces to get the extent of a charater or string
    void GetTextExtent(const char* pcText, float& fWidth, float& fHeight,
        unsigned char ucDirection = NIFONT_LEFT_TO_RIGHT) const;

    void GetCharExtent(char cChar, float& fWidth, float& fHeight) const;

    // Unicode interfaces to get the extent of a character or string
    void GetTextExtent(const NiWChar* pkText, float& fWidth, float& fHeight,
        unsigned char ucDirection = NIFONT_LEFT_TO_RIGHT) const;

    void GetCharExtent(NiWChar kChar, float& fWidth, float& fHeight) const;

    // Get the size up to the first newline
    void GetTextLineExtent(const char* pcText, float& fWidth, float& fHeight,
        unsigned char ucDirection = NIFONT_LEFT_TO_RIGHT) const;

    void GetTextLineExtent(const NiWChar* pkText, float& fWidth,
        float& fHeight, unsigned char ucDirection = NIFONT_LEFT_TO_RIGHT)
        const;

    // Get the size of the first N chars
    void GetTextRangeExtent(const char* pcText, unsigned int uiNumChars,
        float& fWidth, float& fHeight, 
        unsigned char ucDirection = NIFONT_LEFT_TO_RIGHT) const;

    void GetTextRangeExtent(const NiWChar* pkText, unsigned int uiNumChars,
        float& fWidth, float& fHeight,
        unsigned char ucDirection = NIFONT_LEFT_TO_RIGHT) const;

    /// Returns the length of the longest substring of the input that will
    /// fit in fMaxSize pixels (width or height as appropriate
    /// for direction); -1 on error.
    int FitTextRangeExtent(const char* pcText, unsigned int uiNumChars,
        float fMaxSize,
        unsigned char ucDirection = NIFONT_LEFT_TO_RIGHT) const;

    /// Returns the length of the longest substring of the input that will
    /// fit in fMaxSize pixels (width or height as appropriate
    /// for direction); -1 on error.
    int FitTextRangeExtent(const NiWChar* pkText, unsigned int uiNumChars,
        float fMaxSize,
        unsigned char ucDirection = NIFONT_LEFT_TO_RIGHT) const;

    // Get the name of this font
    inline const char* GetName() const;

    // Get the points size of the font
    inline unsigned int GetHeight() const;

    // Get the pixel height of the 'X' character.
    inline unsigned int GetCharHeight() const;

    // Get the number of textures
    inline unsigned int GetNumTextures() const;

    // Get the texture associated with this character. If no character is
    // provided, assume you want the first texture.
    inline NiSourceTexture* GetTexture(NiWChar kChar = 0) const;

    // Get the texture index associated with this character. 
    inline unsigned char GetTextureIndex(NiWChar kChar = 0) const;

    // Get the texture at at this index.
    inline NiSourceTexture* GetTextureAtIndex(unsigned char ucIndex) const;

    // Get the width and height of the texture associated with this character.
    // If no character is provided assume you want the first texture.
    inline unsigned int GetTextureWidth(NiWChar kChar = 0) const;
    inline unsigned int GetTextureHeight(NiWChar kChar = 0) const;

    // These functions have been deprecated and exist for legacy reasons only.
    // GetPixelData() returns the pixel data from the texture, but 
    // SetPixelData() has been fully deprecated.
    inline const NiPixelData* GetPixelData(unsigned char ucIndex = 0) const;
    inline void SetPixelData(NiPixelData* pkPixelData);

    /**
        Returns the right overhang of this character.
        Following convention, overhang is *negative*.
        Includes both italic compensation ("overhang" in Microsoft
        font terminology, "C width" in TrueType terms) and any
        padding added to the texture atlas for art effects.
        Measured in pixels at the font default point size.
    */
    inline int GetRightOverhang(char cChar) const;
    /**
        Returns the right overhang of this character.
        Following convention, overhang is *negative*.
        Includes both italic compensation ("overhang" in Microsoft
        font terminology, "C width" in TrueType terms) and any
        padding added to the texture atlas for art effects.
        Measured in pixels at the font default point size.
    */
    inline int GetRightOverhang(NiWChar kChar) const;
    /**
        Returns the left overhang of this character.
        Following convention, overhang is *negative*.
        Includes both italic compensation ("A width" in TrueType terms)
        and any padding added to the texture atlas for art effects.
        Measured in pixels at the font default point size.
    */
    inline int GetLeftOverhang(char cChar) const;
    /**
        Returns the left overhang of this character.
        Following convention, overhang is *negative*.
        Includes both italic compensation ("A width" in TrueType terms)
        and any padding added to the texture atlas for art effects.
        Measured in pixels at the font default point size.
    */
    inline int GetLeftOverhang(NiWChar kChar) const;

    inline int GetTopOverhang(void) const;
    inline int GetBottomOverhang(void) const;

    inline unsigned int GetAtlasPadRight(void) const;
    inline unsigned int GetAtlasPadLeft(void) const;
    inline unsigned int GetAtlasPadTop(void) const;
    inline unsigned int GetAtlasPadBottom(void) const;

    //  Error
    Error GetLastError();

    //*** begin Emergent internal use only
    inline unsigned int GetGlyphCount() const;
    inline unsigned short GetMaxPixelHeight() const;
    inline unsigned short GetMaxPixelWidth() const;

    void GetCharUVPair(const char cChar, float* pkUVs) const;
    void GetCharUVPair(const NiWChar kChar, float* pkUVs) const;

    void GetCharUVPair(const char cChar, unsigned short* pusPixelLocation)
        const;
    void GetCharUVPair(const NiWChar kChar, unsigned short* pusPixelLocation)
        const;
    //*** end Emergent internal use only

protected:

    enum
    {
        MAGIC_NUMBER    = 0x0046464E,
        NAME_LEN        = 128
    };

    // Font flags
    enum
    {
        BOLD        = 0x00000001,
        ITALIC      = 0x00000002,
        STRIKEOUT   = 0x00000004,
        UNDERLINE   = 0x00000008,
        ANTIALIASED = 0x00001000
    };

    // streaming support
    NiFont();

    // Read the Glyph map and return the glyph map index for this character.
    // If the character is not found the default character will be used
    inline unsigned short GetGlyphMapIndex(NiWChar kChar) const;

    // Legacy Load functions
    bool LegacyLoadFontHeader(NiBinaryStream& kStream, unsigned int uiVersion);
    void LegacyLoadPixelData(NiBinaryStream& kStream);

    bool Load(const char* pacFilename);
    bool LoadFromStream(NiBinaryStream& kStream, const char* pacFilename);
    bool LoadFontHeader(NiBinaryStream& kStream, unsigned int& uiVersion);
    bool LoadExternalAtlas(NiBinaryStream& kStream, const char* pacFilename);
    bool LoadPixelData(NiBinaryStream& kStream);

    void CreateTexture(NiPixelDataPtr spPixelData);
    float TexCoord(unsigned int i, unsigned int j) const;

    /** This function will accumulate the extent of characters in a string,
        correcting for the direction they will be drawn.
        If bIncludeLeftOverhang is set, the result includes overhang, which
        adjusts character spacing for italic fonts and needs to be applied
        for all but the first character in horizontal strings
        (right-to-left or left-to-right); likewise bIncludeRightOverhang
        should be set for all but the last character.
    */
    void SumGlyphExtent(NiWChar kChar, unsigned short& usRowWidth, 
        unsigned short& usRowHeight, unsigned short& usWidth, 
        unsigned short& usHeight, unsigned char ucDirection,
        bool bIncludeLeftOverhang, bool bIncludeRightOverhang) const;

    NiRendererPtr m_spRenderer;

    // Name of the Font.
    char m_acName[NAME_LEN];
    unsigned int m_uiHeight;
    unsigned int m_uiCharHeight;
    unsigned short m_usMaxPixelHeight;
    unsigned short m_usMaxPixelWidth;
    unsigned int m_uiFlags;
    unsigned int m_uiNumTextures;
    NiBool m_bUseExternalAtlas;

    // The glyph map represents a very fast way of converting from a Unicode
    // character to a glyph index.  The glyph index will be used to access
    // NiFonts internal structures for texture location and UV coordinates.
    unsigned int m_uiGlyphCount;
    NiTMap<NiWChar, unsigned short>* m_pkGlyphMap;
    
    /**
        The texture location indicates in which of the 1-3 textures a
        particular glyph will be found.  One entry in this array will exist
        for each possible glyph.  Using the glyph map you can convert from
        the character to the needed glyph index.  To save memory, if the
        m_pcTextureLocation is NULL, all glyphs are assumed to use the first
        texture.
    */
    unsigned char* m_pucTextureLocation;

    /**
        The texture coordinates are four pixel values for the Left,
        Top, Right, and Bottom of each glyph.  Using the glyph map, you may
        convert from the character to the needed glyph index to access this
        array.  It is important to remember that you will need to multiply the
        glyph index by 4 to get the start of a character�s texture coordinates
        because each character is represented by 4 shorts, i.e., left, top,
        right, bottom.
    */
    unsigned short* m_pusTextureCoordinates;

    /// An offset subtracted from the top of every character in the
    /// font to support text effects.
    unsigned int m_uiAtlasPadTop;
    /// An offset subtracted from the bottom of every character in the
    /// font to support text effects.
    unsigned int m_uiAtlasPadBottom;
    /// An offset subtracted from the left of every character in the
    /// font to support text effects.
    unsigned int m_uiAtlasPadLeft;
    /// An offset subtracted from the right of every character in the
    /// font to support text effects.
    unsigned int m_uiAtlasPadRight;

    /**
        A character-dependent offset applied to the current
        rendering position before we begin rendering the next character,
        to support italic fonts. The value is based on "overhang" in older
        MS documentation, "C" width in TrueType and other modern fonts.
        The value is negative or zero.
    */
    int* m_piOverhangRight;

    /**
        A character-dependent offset applied to the current
        rendering position before we begin rendering the a character,
        to support italic fonts. The value is based on "A" width in
        TrueType and other modern fonts.
        The value is negative or zero.
    */
    int* m_piOverhangLeft;

    NiTObjectArray<NiSourceTexturePtr> m_spTextures;

    Error m_eLastError;
    
    static unsigned int ms_uiCurrentVersion;
    static NiWChar ms_akConversionBuffer[2048];

};

typedef NiPointer<NiFont> NiFontPtr;

#include "NiFont.inl"

#endif  // NIFONT_H
