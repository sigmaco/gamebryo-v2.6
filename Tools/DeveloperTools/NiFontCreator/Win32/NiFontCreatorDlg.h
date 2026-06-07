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


// NiFontCreatorDlg.h : header file


#if !defined(NIFONTCREATORDLG_H)
#define NIFONTCREATORDLG_H

#define NIFONTCREATORDLG_NAMELEN 64
#define NIFONTCREATORDLG_PATHLEN 512

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "afxwin.h"

#include "NiUnicodeFontCFG.h"


class NiFontCreatorDlg : public CDialog
{
// Construction
public:
    NiFontCreatorDlg(CWnd* pParent = NULL); // standard constructor

    ~NiFontCreatorDlg();

// Dialog Data
    //{{AFX_DATA(NiFontCreatorDlg)
    enum { IDD = IDD_NIFONTCREATOR_DIALOG };

    CEdit m_CtrlEdit_FontCfgFile;
    CEdit m_CtrlEdit_FontName;
    CEdit m_CtrlEdit_FontSize;

    CEdit m_CtrlEdit_AtlasPadTop;
    CEdit m_CtrlEdit_AtlasPadBottom;
    CEdit m_CtrlEdit_AtlasPadLeft;
    CEdit m_CtrlEdit_AtlasPadRight;

    CButton m_CtrlCheck_Bold;
    CButton m_CtrlCheck_Italic;
    CButton m_CtrlCheck_StrikeOut;
    CButton m_CtrlCheck_Underline;
    CButton m_CtrlCheck_AntiAlias;
    CButton m_CtrlCheck_DefaultASCII;
    CButton m_CtrlCheck_ExternalAtlas;
    CButton m_CtrlCheck_AtlasGrid;

    CStatic m_CtrlStatic_Atlas;
    CStatic m_CtrlStatic_AtlasTop;
    CStatic m_CtrlStatic_AtlasLeft;
    CStatic m_CtrlStatic_AtlasBottom;
    CStatic m_CtrlStatic_AtlasRight;

    CString m_szFontName;
    CString m_szFontSize;
    CString m_szFontCfgFile;

    BOOL m_bBold;
    BOOL m_bItalic;
    BOOL m_bStrikeOut;
    BOOL m_bUnderline;
    BOOL m_bAntiAlias;
    BOOL m_bAddDefaultASCII;
    BOOL m_bWriteExternalAtlas;
    BOOL m_bAtlasGrid;

    COLORREF m_kFontColor;

    CButton m_CtrlBtn_Select;
    CButton m_CtrlBtn_Save;
    CButton m_CtrlBtn_SelectFontCfg;
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(NiFontCreatorDlg)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Generated message map functions
    //{{AFX_MSG(NiFontCreatorDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    virtual void OnOK();

    afx_msg void OnSelectFont();
    afx_msg void OnSaveNFF();
    afx_msg void OnBnClickedSelectFontCfg();
    afx_msg void OnBnClickedSelectFontColor();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

// Implementation
protected:

    enum
    {
        NIFONT_MAX_NUM_TEXTURES = 3
    };

    // Font flags
    // Italic fonts supported as of NFF version 5.
    enum
    {
        FONTSTYLE_NONE          = 0x00,
        FONTSTYLE_BOLD          = 0x01,
        FONTSTYLE_ITALIC        = 0x02,
        FONTSTYLE_STRIKEOUT     = 0x04,
        FONTSTYLE_UNDERLINE     = 0x08,
        FONTSTYLE_ANTIALIASED   = 0x1000
    };

    class UnicodeGlyphInfo
    {
    public:
        UnicodeGlyphInfo() { m_usChar=0; m_usGlyphIndex=0; m_usPageIndex=0; }
        UnicodeGlyphInfo(unsigned short usChar, unsigned short usGID,
            unsigned short usPID)
            {m_usChar= usChar; m_usGlyphIndex = usGID; m_usPageIndex = usPID;}

        unsigned short  m_usChar;       // unicode encoding value
        unsigned short  m_usGlyphIndex; // in a texture
        unsigned short  m_usPageIndex;  // in which texture
    };

    // Read the interface and create the glyph map of available glyphs.
    bool CreateDefaultGlyphMap();
    bool CreateGlyphMapFromCfg();

    // Create the HFONT used to create draw the unicode glyphs
    HFONT CreateUnicodeFontHandle(LOGFONT* pkLogFont);

    // Adjust the name for saving
    bool AdjustNameForSaving(LOGFONT* pkLogFont);

    /// Core of GetMaxFontSize corrected for TrueType fonts.
    void GetTTGlyphInfo(HDC & hDC, TCHAR ch, SIZE& size, int uiGlyphIndex);

    /// Iterate through characters to find the largest;
    /// stores overhang for each character as it goes through the list.
    void GetMaxFontSize(HDC& hDC, SIZE& kSize);

    // Save out the TGAs that represent the pages
    bool SavePages(HDC hDC);
    bool SavePage(HDC hDC, unsigned int uiPage, 
        unsigned int uiFirstIndex, unsigned int uiLastIndex);

    // This will be for debugging.
    // Create a page TGA from an HDC
    // It has to be called before SavePagePixelData.
    bool SavePageTGA(HDC hDC, unsigned int uiPage);

    // Create a NiPixelData for a page from an HDC
    bool SavePagePixelData(HDC hDC);

    // Create the root UFF File
    bool SaveUnicodeFontNFF();

    // New save function: stream.
    void WriteToStream(NiBinaryStream& kStream);
    void WriteFontHeader(NiBinaryStream& kStream);
    void WriteExternalAltasPaths(NiBinaryStream& kStream);
    void WritePixelData(NiBinaryStream& kStream);

    void RemoveSpaces(char* pcString);

    unsigned short PaddedMaxGlyphPixelWidth(void);
    unsigned short PaddedMaxGlyphPixelHeight(void);

    /// Computes m_usGlyphsPerPageRow and m_usGlyphsPerPage, given a
    /// page X x Y pixels.
    void FindPageSize(unsigned int uiX, unsigned int uiY);
    
    // Determine the minum texture size for the available glyphs
    unsigned short GetMinimumTextureSize(unsigned int& uiX, unsigned int& uiY, 
        unsigned short usStartIndex);

    bool ResizeBMI(unsigned short usX, unsigned short usY);

    bool CreateUnicodeFont(LOGFONT* pkLogFont);

    // Adjust save buttons state: enable|disable
    void AdjustSaveButtonState();

    /// Enable or disable atlas padding edit boxes as atlas is
    /// selected/deselected.
    void AdjustAtlasPadState();

    // Reset tool
    void ResetGlyphDataMembers();

    // Data members
    //----------------------------------------------
    // Select fontb
    HICON m_hIcon;
    LOGFONT m_kLogFont;
    CFontDialog m_kFontDlg;
    CColorDialog m_kColorDlg;
    bool m_bFontSelected;

    // Select the font configuration file
    CFileDialog *m_kOpenFileDlg;
    NiUnicodeFontCFG m_kFontCfg;
    UnicodeGlyphInfo* m_pkGlyphMap;
    char m_acFontCfgFileName[NIFONTCREATORDLG_PATHLEN];
    bool m_bFontCfgFileSelected;

    CFileDialog *m_kSaveFileDlg;
    char m_acNFFFileName[NIFONTCREATORDLG_PATHLEN];
    char m_acTGAFileName[NIFONTCREATORDLG_NAMELEN];
    char m_acName[NIFONTCREATORDLG_NAMELEN];

    // Glyph data member
    int m_iNumTextures;
    int m_iGlyphCount;

    unsigned int m_uiHeight;
    unsigned int m_uiCharHeight;
    unsigned int m_uiFlags;
    unsigned int m_uiSpacing;

    unsigned int m_uiAtlasPadTop;
    unsigned int m_uiAtlasPadBottom;
    unsigned int m_uiAtlasPadLeft;
    unsigned int m_uiAtlasPadRight;

    unsigned int m_uiBaseTextureWidth;
    unsigned int m_uiBaseTextureHeight;
    unsigned short m_usMaxGlyphPixelWidth;
    unsigned short m_usMaxGlyphPixelHeight;

    // Computed constants for easy glyph lookups
    unsigned short m_usGlyphsPerPage;
    unsigned short m_usGlyphsPerPageRow;

    // Glyph map to hold all of the glyph we will be using.
    unsigned short* m_pusGlyphPixelWidthHeight;

    // Texture coodinates
    unsigned short* m_pusTextureCoords;

    /// Overhang: adjustment to the width of each glyph to account for
    /// italic skew or other text effects (built-in tracking).
    int * m_piOverhangRight;
    int * m_piOverhangLeft;

    DWORD* m_pdwBitmapBits;

    HFONT m_hFont;
    HDC m_hDC;
    HBITMAP m_hBitmap;
    HGDIOBJ m_hOldBmp;

    // Pixel data array: NiPixelData pointer array.
    NiTPrimitiveArray<DWORD*> m_apkPixelData;
    NiTPrimitiveArray<unsigned int> m_aikTextureWidthHeight;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before
// the previous line.

#endif // !defined(NIFONTCREATORDLG_H)
