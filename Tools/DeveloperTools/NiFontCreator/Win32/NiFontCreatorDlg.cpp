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

#include "stdafx.h"
#include "NiFontCreator.h"
#include "NiFontCreatorDlg.h"
#include <NiDevImageQuantizer.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    //{{AFX_MSG(CAboutDlg)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
        // No message handlers
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(NiFontCreatorDlg, CDialog)
    //{{AFX_MSG_MAP(NiFontCreatorDlg)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()

    ON_BN_CLICKED(IDC_SELECT_FONT, OnSelectFont)
    ON_BN_CLICKED(IDC_SAVE_NFF, OnSaveNFF)
    ON_BN_CLICKED(IDC_SELECT_FONT_CFG, OnBnClickedSelectFontCfg)
    ON_BN_CLICKED(IDC_SELECT_FONT_COLOR, OnBnClickedSelectFontColor)

    ON_BN_CLICKED(IDC_CHECK_EXTERNAL, AdjustAtlasPadState)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------
// NiFontCreatorDlg dialog
//---------------------------------------------------------------------------
NiFontCreatorDlg::NiFontCreatorDlg(CWnd* pParent /*=NULL*/)
    : CDialog(NiFontCreatorDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(NiFontCreatorDlg)
    m_bBold = FALSE;
    m_bItalic = FALSE;
    m_bStrikeOut = FALSE;
    m_bUnderline = FALSE;
    m_bAntiAlias = FALSE;
    m_bAddDefaultASCII = TRUE;
    m_bWriteExternalAtlas = FALSE;
    m_bAtlasGrid = FALSE;
    m_szFontName = _T("");
    m_szFontSize = _T("");
    m_szFontCfgFile = _T("");
    m_uiAtlasPadTop = 0;
    m_uiAtlasPadBottom = 0;
    m_uiAtlasPadLeft = 0;
    m_uiAtlasPadRight = 0;
    // Default color of *white* maintains classic Gb behavior:
    // color specified by user is applied to font.
    m_kFontColor = RGB(255, 255, 255);
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
//---------------------------------------------------------------------------
NiFontCreatorDlg::~NiFontCreatorDlg()
{
    if (m_kSaveFileDlg)
        delete m_kSaveFileDlg;

    if (m_kOpenFileDlg)
        delete m_kOpenFileDlg;

    ResetGlyphDataMembers();
}
//---------------------------------------------------------------------------
void NiFontCreatorDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

    //{{AFX_DATA_MAP(NiFontCreatorDlg)
    DDX_Control(pDX, IDC_EDIT_FONTSIZE, m_CtrlEdit_FontSize);
    DDX_Control(pDX, IDC_EDIT_FONTNAME, m_CtrlEdit_FontName);
    DDX_Control(pDX, IDC_EDIT_FONTCFG_FILENAME, m_CtrlEdit_FontCfgFile);
    DDX_Control(pDX, IDC_EDIT_PAD_TOP, m_CtrlEdit_AtlasPadTop);
    DDX_Control(pDX, IDC_EDIT_PAD_BOTTOM, m_CtrlEdit_AtlasPadBottom);
    DDX_Control(pDX, IDC_EDIT_PAD_LEFT, m_CtrlEdit_AtlasPadLeft);
    DDX_Control(pDX, IDC_EDIT_PAD_RIGHT, m_CtrlEdit_AtlasPadRight);
    DDX_Control(pDX, IDC_CHECK_BOLD, m_CtrlCheck_Bold);
    DDX_Control(pDX, IDC_CHECK_ITALIC, m_CtrlCheck_Italic);
    DDX_Control(pDX, IDC_CHECK_UNDERLINE, m_CtrlCheck_Underline);
    DDX_Control(pDX, IDC_CHECK_STRIKEOUT, m_CtrlCheck_StrikeOut);
    DDX_Control(pDX, IDC_CHECK_ANTIALIAS, m_CtrlCheck_AntiAlias);
    DDX_Control(pDX, IDC_CHECK_DEFAULTASCII, m_CtrlCheck_DefaultASCII);
    DDX_Control(pDX, IDC_CHECK_EXTERNAL, m_CtrlCheck_ExternalAtlas);
    DDX_Control(pDX, IDC_CHECK_ATLASGRID, m_CtrlCheck_AtlasGrid);

    DDX_Control(pDX, IDC_STATIC_ATLAS, m_CtrlStatic_Atlas);
    DDX_Control(pDX, IDC_STATIC_ATLAS_TOP, m_CtrlStatic_AtlasTop);
    DDX_Control(pDX, IDC_STATIC_ATLAS_BOTTOM, m_CtrlStatic_AtlasBottom);
    DDX_Control(pDX, IDC_STATIC_ATLAS_LEFT, m_CtrlStatic_AtlasLeft);
    DDX_Control(pDX, IDC_STATIC_ATLAS_RIGHT, m_CtrlStatic_AtlasRight);

    DDX_Check(pDX, IDC_CHECK_BOLD, m_bBold);
    DDX_Check(pDX, IDC_CHECK_ITALIC, m_bItalic);
    DDX_Check(pDX, IDC_CHECK_STRIKEOUT, m_bStrikeOut);
    DDX_Check(pDX, IDC_CHECK_UNDERLINE, m_bUnderline);
    DDX_Check(pDX, IDC_CHECK_ANTIALIAS, m_bAntiAlias);
    DDX_Check(pDX, IDC_CHECK_DEFAULTASCII, m_bAddDefaultASCII);
    DDX_Check(pDX, IDC_CHECK_ATLASGRID, m_bAtlasGrid);

    DDX_Text(pDX, IDC_EDIT_FONTNAME, m_szFontName);
    DDX_Text(pDX, IDC_EDIT_FONTSIZE, m_szFontSize);
    DDX_Text(pDX, IDC_EDIT_FONTCFG_FILENAME, m_szFontCfgFile);
    DDX_Text(pDX, IDC_EDIT_PAD_TOP, m_uiAtlasPadTop);
    DDX_Text(pDX, IDC_EDIT_PAD_BOTTOM, m_uiAtlasPadBottom);
    DDX_Text(pDX, IDC_EDIT_PAD_LEFT, m_uiAtlasPadLeft);
    DDX_Text(pDX, IDC_EDIT_PAD_RIGHT, m_uiAtlasPadRight);

    DDX_Control(pDX, IDC_SAVE_NFF, m_CtrlBtn_Save);
    DDX_Control(pDX, IDC_SELECT_FONT_CFG, m_CtrlBtn_SelectFontCfg);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
// NiFontCreatorDlg message handlers
//---------------------------------------------------------------------------
BOOL NiFontCreatorDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    NIASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    NIASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);         // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    m_uiFlags = FONTSTYLE_NONE;
    m_bFontSelected = false;
    m_bFontCfgFileSelected = false;

    m_iGlyphCount = 0;
    m_uiSpacing = 3;
    m_uiHeight = 10;
    m_uiCharHeight = 0;

    strcpy_s(m_acName, NIFONTCREATORDLG_NAMELEN, "");
    strcpy_s(m_acNFFFileName, NIFONTCREATORDLG_PATHLEN, "");
    strcpy_s(m_acTGAFileName, NIFONTCREATORDLG_NAMELEN, "");
    strcpy_s(m_acFontCfgFileName, NIFONTCREATORDLG_PATHLEN, "");

    TCHAR kTChar[NIFONTCREATORDLG_PATHLEN];

    char *pcFilter = "Text Files (*.cfg)|*.cfg|(*.txt)|*.txt||";

    MultiByteToWideChar(CP_ACP, 0, pcFilter, (int)strlen(pcFilter) + 1,
        kTChar, NIFONTCREATORDLG_PATHLEN);
    m_kOpenFileDlg = new CFileDialog(true, NULL, NULL,
        OFN_OVERWRITEPROMPT, (LPCTSTR)kTChar);

    // OpenFileDialog for save NFF file
    char *pcSaveFilter = "Text Files (*.nff)|*.nff||";

    MultiByteToWideChar(CP_ACP, 0, pcSaveFilter, (int)strlen(pcSaveFilter) + 1,
        kTChar, NIFONTCREATORDLG_PATHLEN);
    m_kSaveFileDlg = new CFileDialog(false, NULL, NULL,
        OFN_OVERWRITEPROMPT, (LPCTSTR)kTChar);

    // Default max. texture size:  2048 x 2048.
    // NiFontCreator will use smaller power of 2 textures, if possible, or will
    // use up to three 2048 x 2048 textures for one font.
    // The user may override both the max. number of textures per font (default
    // value 3) and the max. texture size (default value 2048 x 2048) via the
    // NiFontCreator.ini file.
    m_uiBaseTextureWidth = 2048;
    m_uiBaseTextureHeight = 2048;

    m_iNumTextures = 0;
    m_pusTextureCoords = NULL;
    m_piOverhangRight = NULL;
    m_piOverhangLeft = NULL;

    m_pkGlyphMap = NULL;
    m_pusGlyphPixelWidthHeight = NULL;

    return TRUE;  // return TRUE  unless you set the focus to a control
}
//---------------------------------------------------------------------------
void NiFontCreatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialog::OnSysCommand(nID, lParam);
    }
}
//---------------------------------------------------------------------------
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
//---------------------------------------------------------------------------
void NiFontCreatorDlg::OnPaint() 
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}
//---------------------------------------------------------------------------
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
//---------------------------------------------------------------------------
HCURSOR NiFontCreatorDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}
//---------------------------------------------------------------------------
void NiFontCreatorDlg::OnOK() 
{
   CDialog::OnOK();
}
//---------------------------------------------------------------------------
void NiFontCreatorDlg::OnSelectFont() 
{
    UpdateData(true);

    // Removing CF_EFFECTS disables color selection, which would help
    // keep the user interface consistent (by only having one way to
    // choose color, the "Select Font Color" button), but also disables
    // strikeout and underlining...

    // Neither MS docs, MFC source, or "Programming Windows with MFC 2ndEd"
    // seem to give us enough information to subclass CFontDialog to
    // have those two checkboxes but not the color selector.

    // If this is a problem for our users, consider activating the strikeout
    // and underlining checkboxes on the main control panel, and writing
    // their values into m_kFontDlg.m_lf.lfUnderline and lfStrikeout?

    m_kFontDlg.m_cf.Flags = CF_INITTOLOGFONTSTRUCT | 
        CF_ENABLEHOOK | CF_FORCEFONTEXIST | CF_SCREENFONTS;
    
    if (m_kFontDlg.DoModal() == IDOK)
    {
        LPTSTR  pStr;

        m_kFontDlg.GetCurrentFont(&m_kLogFont);
        m_szFontName = m_kFontDlg.GetFaceName();
        m_uiHeight = m_kFontDlg.GetSize() / 10;
        pStr = m_szFontSize.GetBuffer(10);
        if (pStr)
        {
            _stprintf_s(pStr, 10, _T("%d"), m_uiHeight);
            m_szFontSize.ReleaseBuffer();
        }

        m_bBold = m_kFontDlg.IsBold();
        if (m_bBold)
            m_CtrlCheck_Bold.SetCheck(1);
        else
            m_CtrlCheck_Bold.SetCheck(0);

        m_bItalic = m_kFontDlg.IsItalic();
        if (m_bItalic)
            m_CtrlCheck_Italic.SetCheck(1);
        else
            m_CtrlCheck_Italic.SetCheck(0);

        m_bUnderline = m_kFontDlg.IsUnderline();
        if (m_bUnderline)
            m_CtrlCheck_Underline.SetCheck(1);
        else
            m_CtrlCheck_Underline.SetCheck(0);

        m_bStrikeOut = m_kFontDlg.IsStrikeOut();
        if (m_bStrikeOut)
            m_CtrlCheck_StrikeOut.SetCheck(1);
        else
            m_CtrlCheck_StrikeOut.SetCheck(0);

        m_bFontSelected = true;

        UpdateData(false);

        AdjustSaveButtonState();
    }
}
//---------------------------------------------------------------------------
void NiFontCreatorDlg::OnSaveNFF()
{
    UpdateData(true);

    if (!m_bFontSelected)
    {
        AfxMessageBox(_T("You must select a font!"));
        return;
    }

    LPTSTR pStr = m_szFontName.LockBuffer();
    NIASSERT(pStr);

#ifdef UNICODE
    int bUsedDefault;
    WideCharToMultiByte (CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR, pStr,
        -1, m_acName, 256, "?", &bUsedDefault);
#else
    strcpy(m_acName, pStr);
#endif

    m_szFontName.UnlockBuffer();

    if ( strcmp(m_acName, "") == 0 )
    {
        AfxMessageBox(_T("The font name is invalid!"));
        return;
    }

    CreateUnicodeFont(&m_kLogFont);
}
//---------------------------------------------------------------------------
void NiFontCreatorDlg::OnBnClickedSelectFontCfg() 
{
    UpdateData(true);

    // Open file browser
    if ( m_kOpenFileDlg->DoModal() == IDOK )
    {
        m_szFontCfgFile = m_kOpenFileDlg->GetPathName();

        LPTSTR pStr = m_szFontCfgFile.LockBuffer();
        NIASSERT(pStr);

#ifdef _UNICODE
        int bUsedDefault;
        WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR, pStr,
            -1, m_acFontCfgFileName, NIFONTCREATORDLG_PATHLEN, "?",
            &bUsedDefault);
#else
        strcpy(m_acFontCfgFileName, pStr);
#endif

        m_szFontCfgFile.UnlockBuffer();

        // Load the font configuration file.
        if ( m_kFontCfg.LoadFontCFG(m_acFontCfgFileName, m_bAddDefaultASCII) )
        {
            m_kFontCfg.ProcessFontCFG();
            m_bFontCfgFileSelected = true;
        }
    }

    if ( m_bFontCfgFileSelected )
    {
        UpdateData(false);

        AdjustSaveButtonState();
    }
}
//---------------------------------------------------------------------------
void NiFontCreatorDlg::OnBnClickedSelectFontColor()
{
    // Start with the custom-color panel open; if they chose a color in
    // the font selector, use that as the default.
    m_kColorDlg.m_cc.Flags |= CC_FULLOPEN | CC_RGBINIT;
    m_kColorDlg.m_cc.rgbResult = m_kFontColor;

    if (m_kColorDlg.DoModal() == IDOK)
    {
        m_kFontColor = m_kColorDlg.GetColor();
    }
}
//---------------------------------------------------------------------------
// This is used as default.
//---------------------------------------------------------------------------
bool NiFontCreatorDlg::CreateDefaultGlyphMap()
{
    // For now just create glyphs 32-126
    m_iGlyphCount = 95 + 1; // the first is for default glyph
    m_pkGlyphMap = new UnicodeGlyphInfo[m_iGlyphCount];

    // The first glyph is default: a rectangle
    m_pkGlyphMap[0].m_usChar = (unsigned int)40895; // 0x9FBF
    m_pkGlyphMap[0].m_usGlyphIndex = (unsigned short)0;
    m_pkGlyphMap[0].m_usPageIndex = (unsigned short)0;

    // Start with a space
    unsigned int uiFirstChar = 32;  // space
    unsigned int uiLastChar = uiFirstChar + (unsigned int)m_iGlyphCount - 1;
    for ( unsigned int i = uiFirstChar; i < uiLastChar; i++ )
    {
        unsigned int uiIdx = i - uiFirstChar + 1;   // skip the first one
        m_pkGlyphMap[uiIdx].m_usChar = (unsigned short)i;
        m_pkGlyphMap[uiIdx].m_usGlyphIndex = (unsigned short)0;
        m_pkGlyphMap[uiIdx].m_usPageIndex = (unsigned short)0;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiFontCreatorDlg::CreateGlyphMapFromCfg()
{
    int iUniqueGlyphs = m_kFontCfg.GetNumOfUniqueCharacters();
    m_iGlyphCount = iUniqueGlyphs + 1;      // the first is for default

    m_pkGlyphMap = new UnicodeGlyphInfo[m_iGlyphCount];

    // The first glyph is default: a rectangle
    m_pkGlyphMap[0].m_usChar = (unsigned int)40895; // 0x9FBF
    m_pkGlyphMap[0].m_usGlyphIndex = (unsigned short)0;
    m_pkGlyphMap[0].m_usPageIndex = (unsigned short)0;

    for ( int i = 0; i < iUniqueGlyphs; i++ )
    {
        m_pkGlyphMap[i+1].m_usChar = m_kFontCfg.GetCharEncoding(i);
        m_pkGlyphMap[i+1].m_usGlyphIndex = (unsigned short)0;
        m_pkGlyphMap[i+1].m_usPageIndex = (unsigned short)0;
    }

    return true;
}
//---------------------------------------------------------------------------
HFONT NiFontCreatorDlg::CreateUnicodeFontHandle(LOGFONT* pkLogFont)
{
    // Create the HFONT used to create draw the unicode glyphs
    TCHAR szName[128];

    // Convert the name to unicode if necessary
#ifdef UNICODE
    MultiByteToWideChar(CP_ACP, 0, m_acName, (int)strlen(m_acName) + 1, szName,
        128);
#else
    strcpy(szName, m_acName);
#endif

    m_uiFlags = FONTSTYLE_NONE;

    // Create the font (Windows font)
    unsigned int uiBold = pkLogFont->lfWeight;
    if (uiBold > FW_NORMAL)
        m_uiFlags |= FONTSTYLE_BOLD;

    unsigned int uiItalic = pkLogFont->lfItalic;
    if (uiItalic)
        m_uiFlags |= FONTSTYLE_ITALIC;

    unsigned int uiStrikeout = pkLogFont->lfStrikeOut;
    if (uiStrikeout)
        m_uiFlags |= FONTSTYLE_STRIKEOUT;
    unsigned int uiUnderline = pkLogFont->lfUnderline;
    if (uiUnderline)
        m_uiFlags |= FONTSTYLE_UNDERLINE;
    if (m_bAntiAlias)
        m_uiFlags |= FONTSTYLE_ANTIALIASED;

    unsigned int uiQuality = (m_uiFlags & FONTSTYLE_ANTIALIASED) ?
        ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY;

    return ::CreateFont(pkLogFont->lfHeight, 0, 0, 0, uiBold,
        uiItalic, uiUnderline, uiStrikeout, pkLogFont->lfCharSet, 
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, uiQuality, 
        VARIABLE_PITCH, szName);
}
//---------------------------------------------------------------------------
bool NiFontCreatorDlg::AdjustNameForSaving(LOGFONT* pkLogFont)
{
    // Adjust the name, for saving purposes...
    char acTemp1[10];
    char acTemp2[10];
    char acTemp3[10];
    char acTemp4[10];
    char acTemp5[10];
    char acTemp6[10];

    sprintf_s(acTemp1, 10, "%s", (pkLogFont->lfWeight == FW_BOLD) ? "B" : "");
    sprintf_s(acTemp2, 10, "%s", (pkLogFont->lfItalic) ? "I" : "");
    sprintf_s(acTemp3, 10, "%s", (pkLogFont->lfUnderline == TRUE) ? "U" : "");
    sprintf_s(acTemp4, 10, "%s", (pkLogFont->lfStrikeOut == TRUE) ? "S" : "");
    sprintf_s(acTemp5, 10, "%s",
        (m_uiFlags & FONTSTYLE_ANTIALIASED) ? "A" : "");
    if ((strcmp(acTemp1, "")) ||
        (strcmp(acTemp2, "")) ||
        (strcmp(acTemp3, "")) ||
        (strcmp(acTemp4, "")) ||
        (strcmp(acTemp5, "")))
    {
        strcat_s(m_acName, NIFONTCREATORDLG_NAMELEN, "_");
    }
    sprintf_s(acTemp6, 10, "_%d", m_uiHeight);
    strcat_s(m_acName, NIFONTCREATORDLG_NAMELEN, acTemp1);
    strcat_s(m_acName, NIFONTCREATORDLG_NAMELEN, acTemp2);
    strcat_s(m_acName, NIFONTCREATORDLG_NAMELEN, acTemp3);
    strcat_s(m_acName, NIFONTCREATORDLG_NAMELEN, acTemp4);
    strcat_s(m_acName, NIFONTCREATORDLG_NAMELEN, acTemp5);
    strcat_s(m_acName, NIFONTCREATORDLG_NAMELEN, acTemp6);

    return true;
}
//---------------------------------------------------------------------------
void NiFontCreatorDlg::GetTTGlyphInfo(HDC & hDC,
                                     TCHAR ch,
                                     SIZE& size,
                                     int uiGlyphIndex) {
    TCHAR str[2];
    ABC kChSpacing;
    str[0] = ch;
    str[1] = 0;
    // 'ABC' widths store blank padding on the left in 'A',
    // actual width of character in 'B',
    // and blank padding on the right in 'C'.
    // Handwriting-style fonts (Vivaldi, Lucida Handwriting) can have negative
    // A for j, g, y descenders extending left underneath the adjacent
    // character.
    // Italic fonts have negative C, non-italic fonts may have positive C.
    // (Some non-italic fonts will have negative C, particularly for
    // punctuation.)
    // Value returned by ::GetTextExtentPoint32 is typically A+B+C,
    // which means the right edge of italic characters is cut off.
    ::GetCharABCWidths(hDC, ch, ch, &kChSpacing);
    size.cx = kChSpacing.abcB;
    if (kChSpacing.abcA > 0)
    {
        size.cx += kChSpacing.abcA;
        m_piOverhangLeft[uiGlyphIndex] = 0;
    }
    else
    {
        m_piOverhangLeft[uiGlyphIndex] = kChSpacing.abcA;
    }
    if (kChSpacing.abcC > 0) 
    {
        size.cx += kChSpacing.abcC;
        m_piOverhangRight[uiGlyphIndex] = 0;
    }
    else
    {
        m_piOverhangRight[uiGlyphIndex] = kChSpacing.abcC;
    }
}

//---------------------------------------------------------------------------
void NiFontCreatorDlg::GetMaxFontSize(HDC& hDC, SIZE& kSize)
{
    NIASSERT(m_iGlyphCount > 0);

    TCHAR str[2];
    SIZE size;
    bool bIsTrueType;
    TEXTMETRIC tm;

    kSize.cx = 0;
    kSize.cy = 0;

    // Legacy Compute the CharHeight as the height of the letter 'X'
    str[0] = 'X';
    str[1] = 0;
    ::GetTextExtentPoint32(hDC, str, 1, &size);
    m_uiCharHeight = size.cy;

    // Is this a TrueType font?
    ::GetTextMetrics(hDC, &tm);
    bIsTrueType = ((tm.tmPitchAndFamily & TMPF_TRUETYPE) != 0);

    // Type 1 PostScript files don't detect as TrueType,
    // but they effectively are. Apparently, they can be recognized because
    // they have all the other bits set (this may only be true of PostScript
    // *printer device* fonts, in which case we should try just checking
    // TMPF_VECTOR, but we don't have any known good samples to test).
    if (!bIsTrueType)
    {
        bIsTrueType = ((tm.tmPitchAndFamily & 
        (TMPF_DEVICE | TMPF_VECTOR | TMPF_FIXED_PITCH)) != 0);
    }


    // Allocate the texture coordinate array.
    m_pusGlyphPixelWidthHeight = new unsigned short[m_iGlyphCount * 2];
    NIASSERT(m_pusGlyphPixelWidthHeight);

    m_pusTextureCoords = new unsigned short[m_iGlyphCount * 4];
    NIASSERT(m_pusTextureCoords);

    m_piOverhangRight = new int [m_iGlyphCount];
    NIASSERT(m_piOverhangRight);
    m_piOverhangLeft = new int [m_iGlyphCount];
    NIASSERT(m_piOverhangLeft);

    // Try each chacter in the glyph map and find the maximum size
    unsigned int uiLoop;
    for (uiLoop = 0; uiLoop < (unsigned int)m_iGlyphCount; uiLoop++)
    {
        str[0] = m_pkGlyphMap[uiLoop].m_usChar;

        ::GetTextExtentPoint32(hDC, str, 1, &size);
        if (bIsTrueType)
        {
            GetTTGlyphInfo(hDC, str[0], size, uiLoop);
        }
        else
        {
            ::GetTextMetrics(hDC, &tm);
            if (tm.tmOverhang > 0)
            {
                size.cx += tm.tmOverhang;
                m_piOverhangRight[uiLoop] = -tm.tmOverhang;
            }
            else
            {
                m_piOverhangRight[uiLoop] = 0;
            }
            m_piOverhangLeft[uiLoop] = 0;
        }
        kSize.cx = max(kSize.cx, size.cx);
        kSize.cy = max(kSize.cy, size.cy);

        // Determine the pixel size of each glyph
        m_pusGlyphPixelWidthHeight[uiLoop * 2] = (unsigned short)size.cx;
        m_pusGlyphPixelWidthHeight[(uiLoop * 2) + 1] = (unsigned short)size.cy;
    }

    m_usMaxGlyphPixelWidth = (unsigned short)kSize.cx;
    m_usMaxGlyphPixelHeight = (unsigned short)kSize.cy;

    // Compute the Glyph Constants
    FindPageSize(m_uiBaseTextureWidth, m_uiBaseTextureHeight);
}

//---------------------------------------------------------------------------
bool NiFontCreatorDlg::SavePages(HDC hDC)
{
    unsigned short usStartIndex = 0;
    unsigned short usEndIndex = 0;
    unsigned int uiPage = 0;
    while (usStartIndex < m_iGlyphCount)
    {
        usEndIndex = GetMinimumTextureSize(m_uiBaseTextureWidth, 
            m_uiBaseTextureHeight, usStartIndex);

        // Resize the buffers
        ResizeBMI(
            (unsigned short)m_uiBaseTextureWidth, 
            (unsigned short)m_uiBaseTextureHeight);

        if ( !SavePage(hDC, uiPage++, usStartIndex, usEndIndex) )
            return false;

        // Start on the next page.
        usStartIndex = usEndIndex;
    }


    // Create the NFF File
    if ( !SaveUnicodeFontNFF() )
        return false;

    return true;
}

//---------------------------------------------------------------------------
bool NiFontCreatorDlg::SavePage(HDC hDC, unsigned int uiPage,
    unsigned int uiFirstIndex, unsigned int uiLastIndex)
{
    TCHAR str[2] = _T("X");
    unsigned int uiX = 0;
    unsigned int uiY = 0;

    // Insure we don't overflow
    if (uiLastIndex > (unsigned int)m_iGlyphCount)
        uiLastIndex = (unsigned int)m_iGlyphCount;

    // If all glyphs have just been done, return.
    // The case is as (m_iGlyphCount % m_usGlyphsPerPage) == 0.
    if ( uiFirstIndex >= uiLastIndex )
        return true;

    m_iNumTextures += 1;

    // Warning if number of textures > NIFONT_MAX_NUM_TEXTURES
    if (m_iNumTextures > NIFONT_MAX_NUM_TEXTURES)
    {
        TCHAR akTemp[NIFONTCREATORDLG_NAMELEN];
        _stprintf_s(akTemp, NIFONTCREATORDLG_NAMELEN,
            TEXT("You are generating %d 2048X2048 textures!"), m_iNumTextures);
        MessageBox(akTemp, TEXT("WARNING"), MB_OK);
    }

    unsigned short* pfTexCoord = &(m_pusTextureCoords[uiFirstIndex * 4]);


    // Draw each character into the hDC
    for (unsigned int uiI = uiFirstIndex; uiI < uiLastIndex; uiI++)
    {

        // If there's left overhang, need to shift the character draw
        // location to the right so that our character ends up inside
        // the intended bounds.
        uiX = ((uiI - uiFirstIndex) % m_usGlyphsPerPageRow) *
            PaddedMaxGlyphPixelWidth() + m_uiAtlasPadLeft
            - m_piOverhangLeft[uiI];
        uiY = ((uiI - uiFirstIndex) / m_usGlyphsPerPageRow) *
            PaddedMaxGlyphPixelHeight()    + m_uiAtlasPadTop;

        m_pkGlyphMap[uiI].m_usGlyphIndex = (unsigned short)(uiI - uiFirstIndex);
        m_pkGlyphMap[uiI].m_usPageIndex = (unsigned short)uiPage;
        str[0] = m_pkGlyphMap[uiI].m_usChar;

        ExtTextOut(hDC, uiX + 0, uiY + 0,
            ETO_OPAQUE, NULL, str, 1, NULL);

        // Save texture coordinates for the glyph; extend the texture
        // if this font is padded or if the glyph has left overhang.
        // We CAN NOT contract the texture if this font has right overhang,
        // since that cuts off important pixels.
        pfTexCoord[0] = (unsigned short)(uiX - m_uiAtlasPadLeft
            + m_piOverhangLeft[uiI]);
        pfTexCoord[1] = (unsigned short)(uiY - m_uiAtlasPadTop);
        pfTexCoord[2] = (unsigned short)(pfTexCoord[0] + m_uiAtlasPadRight
            + m_uiAtlasPadLeft
            + m_pusGlyphPixelWidthHeight[uiI * 2]);
        pfTexCoord[3] = (unsigned short)(pfTexCoord[1] + m_uiAtlasPadBottom
            + m_uiAtlasPadTop
            + m_pusGlyphPixelWidthHeight[(uiI * 2) + 1]);
        pfTexCoord += 4;
    }

    if(m_bWriteExternalAtlas)
    {
        SavePageTGA(hDC, uiPage);   // Output TGA texture file for debugging.
    }
    else
    {
        SavePagePixelData(hDC);
    }

    return true;
}
//---------------------------------------------------------------------------
// Create a page TGA from an HDC
// This function has to be called before calling SavePagePixelData
//---------------------------------------------------------------------------
bool NiFontCreatorDlg::SavePageTGA(HDC, unsigned int uiPage)
{
    // Setup the TGA file header
    struct TargaHeader
    {
        BYTE IDLength;
        BYTE ColormapType;
        BYTE ImageType;
        BYTE ColormapSpecification[5];
        WORD XOrigin;
        WORD YOrigin;
        WORD ImageWidth;
        WORD ImageHeight;
        BYTE PixelDepth;
        BYTE ImageDescriptor;
    } tgaHeader;

    memset(&tgaHeader, 0, sizeof(tgaHeader));
    tgaHeader.IDLength        = 0;
    tgaHeader.ImageType       = 2;
    tgaHeader.ImageWidth      = (WORD)m_uiBaseTextureWidth;
    tgaHeader.ImageHeight     = (WORD)m_uiBaseTextureHeight;
    tgaHeader.PixelDepth      = 32;
    tgaHeader.ImageDescriptor = 0x28;

    char szFileName[_MAX_PATH];

    // Compute the correct name
    sprintf_s(szFileName, _MAX_PATH, "%s_%d.tga", m_acTGAFileName, uiPage);

    // Create a new file
    FILE* file;
    errno_t errnum = fopen_s(&file, szFileName, "wb");
    if ((errnum != 0) || (NULL == file))
        return false;

    // Write the Targa header and the surface pixels to the file
    fwrite(&tgaHeader, sizeof(TargaHeader), 1, file);

    unsigned int uiSize = m_uiBaseTextureWidth * m_uiBaseTextureHeight;
    
    // Come up with a high-contrast background color: compute luminance,
    // threshold off of that to determine whether uiBackgroundColor
    // should be white or black. Luminance constants from obscure MS
    // documentation; we could also try HSV or HLS here if it doesn't
    // give good results.
    unsigned int uiBackgroundColor;
    float fLuminance = GetRValue(m_kFontColor) * 0.2125f +
        GetGValue(m_kFontColor) * 0.7154f +
        GetBValue(m_kFontColor) * 0.0721f;
    if (fLuminance < 128.0f)
        uiBackgroundColor = 0x00ffffff;
    else
        uiBackgroundColor = 0x00000000;

    // Colors to show live area for each glyph in exported
    // texture atlas. Making these non-transparent can help
    // debug font alignment problems.
    unsigned int uiBoundsColor = 0x000066ff;
    unsigned int uiOverhangColor = 0x0033ff00;

    // Windows stores colors BGR, but TGA files are RGB,
    // so we need to construct a "correct"
    // foreground color by reversing the bytes.
    unsigned int uiForegroundColor =
        GetRValue(m_kFontColor) << 16 |
        GetGValue(m_kFontColor) << 8 |
        GetBValue(m_kFontColor);
 
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        // Convert the gray scale 32 bit image to an alpha image
        // by moving the blue bits to the alpha and or-ing in the
        // appropriate color.
        unsigned int uiPixel = m_pdwBitmapBits[ui] << 24;

        // Bounds pixel if coordinate equal to this glyph's texture coords.
        // Overhang pixel if coordinate equal to the edge of this glyph's
        // core rendered area.
        bool bIsBounds = false;
        bool bIsOverhang = false;

        unsigned int uiRow = ui / m_uiBaseTextureWidth;
        unsigned int uiColumn = ui % m_uiBaseTextureWidth;
        unsigned int uiGlyphNumber = uiColumn / PaddedMaxGlyphPixelWidth() +
            uiRow / PaddedMaxGlyphPixelHeight() * m_usGlyphsPerPageRow;

        if ((int)uiGlyphNumber < m_iGlyphCount) {
            unsigned int uiLeftCoord = m_pusTextureCoords[uiGlyphNumber*4];
            unsigned int uiRightCoord = m_pusTextureCoords[uiGlyphNumber*4+2];
            unsigned int uiTopCoord = m_pusTextureCoords[uiGlyphNumber*4+1];
            unsigned int uiBottomCoord = m_pusTextureCoords[uiGlyphNumber*4+3];

            unsigned int uiLeftOffset = uiLeftCoord + m_uiAtlasPadLeft
                - m_piOverhangLeft[uiGlyphNumber];
            unsigned int uiRightOffset = uiRightCoord - m_uiAtlasPadRight
                + m_piOverhangRight[uiGlyphNumber];
            unsigned int uiTopOffset = uiTopCoord + m_uiAtlasPadTop;
            unsigned int uiBottomOffset = uiBottomCoord - m_uiAtlasPadBottom;

            if ((uiColumn == uiLeftCoord ||    uiColumn == uiRightCoord) &&
                uiRow >= uiTopCoord && uiRow <= uiBottomCoord)
            {
                bIsBounds = true;
            }
            else if ((uiRow == uiTopCoord || uiRow == uiBottomCoord) &&
                uiColumn >= uiLeftCoord && uiColumn <= uiRightCoord)
            {
                bIsBounds = true;
            } 

            if ((uiColumn == uiLeftOffset || uiColumn == uiRightOffset)
                && uiRow >= uiTopOffset &&    uiRow <= uiBottomOffset)
            {
                bIsOverhang = true;
            }
            else if ((uiRow == uiTopOffset || uiRow == uiBottomOffset) &&
                uiColumn >= uiLeftOffset && uiColumn <= uiRightOffset)
            {
                bIsOverhang = true;
            }
        }

        // Set the pixel RGB to be either foreground, grid,
        // or background color
        if (uiPixel > 0)
            uiPixel |= uiForegroundColor;
        else if (bIsOverhang && m_bAtlasGrid)
            uiPixel |= uiOverhangColor;
        else if (bIsBounds && m_bAtlasGrid)
            uiPixel |= uiBoundsColor;
        else
            uiPixel |= uiBackgroundColor;

        fwrite(&uiPixel, 4, 1, file);
    }

    fclose (file);
    return true;
}
//---------------------------------------------------------------------------
bool NiFontCreatorDlg::SavePagePixelData(HDC)
{
    unsigned int uiSize = m_uiBaseTextureWidth * m_uiBaseTextureHeight;
    DWORD* pdwDest = new DWORD[uiSize];
    NIASSERT(pdwDest);

    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        // Convert the gray scale 32 bit image to an alpha image
        // by moving the blue bits to the alpha
        DWORD dwPixel = m_pdwBitmapBits[ui] << 24;

        // Set the pixel RGB to be while
        dwPixel |= 0x00ffffff;
        pdwDest[ui] = dwPixel;

        // Clear the pixels for the next draw
        m_pdwBitmapBits[ui] = 0x00000000;
    }

    // Pixel Data into array
    m_apkPixelData.Add(pdwDest);

    // Save the Width and Height of the texture
    m_aikTextureWidthHeight.Add(m_uiBaseTextureWidth);
    m_aikTextureWidthHeight.Add(m_uiBaseTextureHeight);


    return true;
}
//---------------------------------------------------------------------------
// Save NiFont-based NFF, which supports Unicode as well as Ascii.
bool NiFontCreatorDlg::SaveUnicodeFontNFF()
{
    // Create the root UFF File
    if (strlen(m_acNFFFileName) <= 0)
        return false;

    NiFile kFile(m_acNFFFileName, NiFile::WRITE_ONLY);
    if (!kFile)
    {
        return false;
    }
    
    WriteToStream(kFile);

    return true;
}
//---------------------------------------------------------------------------
void NiFontCreatorDlg::WriteToStream(NiBinaryStream& kStream)
{
    WriteFontHeader(kStream);

    if(m_bWriteExternalAtlas)
    {
        WriteExternalAltasPaths(kStream);
    }
    else
    {
        WritePixelData(kStream);
    }
}
//---------------------------------------------------------------------------
void NiFontCreatorDlg::WriteFontHeader(NiBinaryStream& kStream)
{
    // Write the magic number
    unsigned int uiMagicNumber = 0x0046464E;
    NiStreamSaveBinary(kStream, uiMagicNumber);

    // Write out the Version
    unsigned int uiVersion = 5;
    NiStreamSaveBinary(kStream, uiVersion);

    // Write out the length of the name;
    unsigned int uiNameLength = (unsigned int)strlen(m_acName);
    NiStreamSaveBinary(kStream, uiNameLength);

    // Write out the name
    NiStreamSaveBinary(kStream, m_acName, uiNameLength + 1);

    // Height (font size)
    NiStreamSaveBinary(kStream, m_uiHeight);

    // CharHeight
    NiStreamSaveBinary(kStream, m_uiCharHeight);

    // max pixel width
    NiStreamSaveBinary(kStream, m_usMaxGlyphPixelWidth);

    // max pixel height
    NiStreamSaveBinary(kStream, m_usMaxGlyphPixelHeight);

    // m_uiFlags
    NiStreamSaveBinary(kStream, m_uiFlags);

    // m_iGlyphCount
    NiStreamSaveBinary(kStream, m_iGlyphCount);

    // pusGlyphMap
    unsigned int uiLoop;
    unsigned short* pusGlyphMap = new unsigned short[m_iGlyphCount];
    for (uiLoop = 0; uiLoop < (unsigned int)m_iGlyphCount; uiLoop++)
    {
        pusGlyphMap[uiLoop] = m_pkGlyphMap[uiLoop].m_usChar;
    }

    NiStreamSaveBinary(kStream, pusGlyphMap, m_iGlyphCount);
    delete[] pusGlyphMap;

    // Build the texture location array and assume it is all on the first
    // texture.
    char* pcTextureLocation = new char[m_iGlyphCount];
    for (uiLoop = 0; uiLoop < (unsigned int)m_iGlyphCount; uiLoop++)
    {
        pcTextureLocation[uiLoop] =
            (unsigned char)m_pkGlyphMap[uiLoop].m_usPageIndex;
    }

    // m_pucTextureLocation
    NiStreamSaveBinary(kStream, pcTextureLocation, m_iGlyphCount);
    delete[] pcTextureLocation;

    // m_pustTextureCoordinates
    NiStreamSaveBinary(kStream, m_pusTextureCoords, m_iGlyphCount * 4);
    // Atlas padding (support for external font effects)
    NiStreamSaveBinary(kStream, m_uiAtlasPadTop);
    NiStreamSaveBinary(kStream, m_uiAtlasPadBottom);
    NiStreamSaveBinary(kStream, m_uiAtlasPadLeft);
    NiStreamSaveBinary(kStream, m_uiAtlasPadRight);

    // Overhang (support for italic fonts)
    NiStreamSaveBinary(kStream, m_piOverhangRight, m_iGlyphCount);
    NiStreamSaveBinary(kStream, m_piOverhangLeft, m_iGlyphCount);

    // Num Textures
    NiStreamSaveBinary(kStream, m_iNumTextures);
}
//---------------------------------------------------------------------------
void NiFontCreatorDlg::WriteExternalAltasPaths(NiBinaryStream& kStream)
{
    NiBool bSaveExternalAtlas = true;
    NiStreamSaveBinary(kStream, bSaveExternalAtlas);

    char szFileName[_MAX_PATH];

    for(int uiLoop = 0; uiLoop < m_iNumTextures; uiLoop++)
    {
        // Compute the correct name
        sprintf_s(szFileName, _MAX_PATH, "%s_%d.tga",
            m_acTGAFileName, uiLoop);

        // Write out the length of the name;
        unsigned int uiNameLength = (unsigned int)strlen(szFileName);
        NiStreamSaveBinary(kStream, uiNameLength);

        // Write out the name
        NiStreamSaveBinary(kStream, szFileName, uiNameLength + 1);
    }
}
//---------------------------------------------------------------------------
void NiFontCreatorDlg::WritePixelData(NiBinaryStream& kStream)
{
    NiBool bSaveExternalAtlas = false;
    NiStreamSaveBinary(kStream, bSaveExternalAtlas);

    unsigned int uiLoop;
    unsigned int uiWidth;
    unsigned int uiHeight;
    unsigned int uiBitsPerPixel = 32;
    unsigned int uiSize;
    unsigned int uiCount = m_apkPixelData.GetSize();
    char* pkPixelData = NULL;
    for (uiLoop = 0; uiLoop < uiCount; ++uiLoop)
    {
        pkPixelData = (char*)m_apkPixelData.GetAt(uiLoop);
        NIASSERT(pkPixelData);

        // Write pixel data width & height
        // If texture is dynamically changed, it should
        // be saved somewhere. Then get it here.
        uiWidth = m_aikTextureWidthHeight.GetAt(uiLoop * 2);
        uiHeight = m_aikTextureWidthHeight.GetAt((uiLoop * 2) + 1);
        NiStreamSaveBinary(kStream, uiWidth);
        NiStreamSaveBinary(kStream, uiHeight);

        // Write bit-per-pixel
        NiStreamSaveBinary(kStream, uiBitsPerPixel);

        // Write pixel data (bit-per-pixel is 32)
        uiSize = uiWidth * uiHeight * uiBitsPerPixel / 8;
        NiStreamSaveBinary(kStream, pkPixelData, uiSize);
    }
}
//---------------------------------------------------------------------------
void NiFontCreatorDlg::RemoveSpaces(char* pcString)
{
    for (char* pc0 = pcString; *pc0 != 0; pc0++)
    {
        if (*pc0 == ' ')
        {
            for (char* pc1 = pc0; pc1[0] != 0; pc1++)
                pc1[0] = pc1[1];
        }
    }
}
//---------------------------------------------------------------------------
unsigned short NiFontCreatorDlg::PaddedMaxGlyphPixelWidth()
{
    return (unsigned short)(m_usMaxGlyphPixelWidth + m_uiSpacing +
        m_uiAtlasPadLeft + m_uiAtlasPadRight);
}
//---------------------------------------------------------------------------
unsigned short NiFontCreatorDlg::PaddedMaxGlyphPixelHeight()
{
    return (unsigned short)(m_usMaxGlyphPixelHeight + m_uiSpacing +
        m_uiAtlasPadTop + m_uiAtlasPadBottom);
}
//---------------------------------------------------------------------------
void NiFontCreatorDlg::FindPageSize(unsigned int uiX, unsigned int uiY)
{
    m_usGlyphsPerPageRow = (unsigned short)(uiX /
        PaddedMaxGlyphPixelWidth());

    m_usGlyphsPerPage = (unsigned short)((uiY /
        PaddedMaxGlyphPixelHeight()) * m_usGlyphsPerPageRow);
}
//---------------------------------------------------------------------------
unsigned short NiFontCreatorDlg::GetMinimumTextureSize(unsigned int& uiX, 
    unsigned int& uiY, unsigned short usStartIndex)
{

    m_usGlyphsPerPageRow = 0;
    m_usGlyphsPerPage = 0;

    uiX = 128;
    uiY = 128;

    while ((uiX <= 2048) && (uiY <= 2048))
    {
        // Compute the Glyph Constants
        FindPageSize(uiX, uiY);

        if (m_usGlyphsPerPage >= (m_iGlyphCount - usStartIndex))
            return (unsigned short)m_iGlyphCount;

        // Determine the minimum texture size for the available glyphs
        if (uiX == uiY)
            uiX *= 2;
        else
            uiY *= 2;
    }

    // Don't overflow
    uiX = 2048;
    uiY = 2048;

    // Update where we will be starting next page
    return usStartIndex + m_usGlyphsPerPage;;
}
//---------------------------------------------------------------------------
bool NiFontCreatorDlg::ResizeBMI(unsigned short, unsigned short)
{
    // Create bitmap and DC for the bitmap
    BITMAPINFO bmi;
    memset ( (void*)&bmi, 0, sizeof(BITMAPINFOHEADER) );
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth =  (int)m_uiBaseTextureWidth;
    bmi.bmiHeader.biHeight = -(int)m_uiBaseTextureHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biBitCount = 32;

    m_hBitmap   = CreateDIBSection (m_hDC, &bmi, DIB_RGB_COLORS,
        (void**)&m_pdwBitmapBits, NULL, 0);
    if ( m_hBitmap == 0 )
        return false;

    HGDIOBJ hOldBmp = ::SelectObject(m_hDC, m_hBitmap);

    // Delete the old bit map
    if (hOldBmp)
        DeleteObject (hOldBmp);

    return true;
}
//---------------------------------------------------------------------------
bool NiFontCreatorDlg::CreateUnicodeFont(LOGFONT* pkLogFont)
{
    // ResetGlyphDataMembers
    ResetGlyphDataMembers();

    // Create the Glyph Map
    bool mapStatus;
    if ( m_bFontCfgFileSelected )
        mapStatus = CreateGlyphMapFromCfg();
    else
        mapStatus = CreateDefaultGlyphMap();

    if ( !mapStatus )
        return false;

    // Create the Font
    m_hFont = CreateUnicodeFontHandle(pkLogFont);
    if (m_hFont == 0)
        return false;

    m_hDC = CreateCompatibleDC(NULL);
    if (m_hDC == 0)
        return false;

    // Create bitmap and DC for the bitmap
    BITMAPINFO bmi;
    memset ( (void*)&bmi, 0, sizeof(BITMAPINFOHEADER) );
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth =  (int)m_uiBaseTextureWidth;
    bmi.bmiHeader.biHeight = -(int)m_uiBaseTextureHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biBitCount = 32;

    m_hBitmap   = CreateDIBSection (m_hDC, &bmi, DIB_RGB_COLORS,
        (void**)&m_pdwBitmapBits, NULL, 0);
    if ( m_hBitmap == 0 )
        return false;

    HGDIOBJ hOldBmp = ::SelectObject(m_hDC, m_hBitmap);
    HGDIOBJ hOldFont = ::SelectObject(m_hDC, m_hFont);

    // Set text properties
    ::SetTextColor (m_hDC, RGB(255,255,255));
    ::SetBkColor (m_hDC, 0x00000000);
    ::SetTextAlign (m_hDC, TA_TOP);

    // Loop through all printable chars & output them to the bitmap.
    // Track texture coords...
    SIZE size;
    GetMaxFontSize (m_hDC, size);

    // Adjust the name to include bold etc and font size
    if (!AdjustNameForSaving(pkLogFont))
        return false;

    // Get NFF full path name
    sprintf_s(m_acNFFFileName, NIFONTCREATORDLG_PATHLEN, "%s.nff", m_acName);
    sprintf_s(m_acTGAFileName, NIFONTCREATORDLG_NAMELEN, "%s", m_acName);

    // Remove space in name string
    RemoveSpaces(m_acNFFFileName);
    RemoveSpaces(m_acTGAFileName);

    MultiByteToWideChar(CP_ACP, 0, m_acNFFFileName,
        (int)strlen(m_acNFFFileName) + 1, m_kSaveFileDlg->GetOFN().lpstrFile,
        NIFONTCREATORDLG_PATHLEN);

    if (m_kSaveFileDlg->DoModal() == IDOK)
    {
        CString kFullFileName = m_kSaveFileDlg->GetPathName();
        CString kBaseFileName = m_kSaveFileDlg->GetFileTitle();
        LPTSTR pFullFileNameStr = kFullFileName.LockBuffer();
        LPTSTR pBaseFileNameStr = kBaseFileName.LockBuffer();
        NIASSERT(pFullFileNameStr);
        NIASSERT(pBaseFileNameStr);

#ifdef _UNICODE
        int bUsedDefault;
        WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR,
            pFullFileNameStr, -1, m_acNFFFileName, NIFONTCREATORDLG_PATHLEN,
            "?", &bUsedDefault);
        WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR,
            pBaseFileNameStr, -1, m_acTGAFileName, NIFONTCREATORDLG_PATHLEN,
            "?", &bUsedDefault);

        // it should be safe to over-write m_acName here.
        WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR,
            pBaseFileNameStr, -1, m_acName, NIFONTCREATORDLG_PATHLEN,
            "?", &bUsedDefault);
#else
        strcpy(m_acNFFFileName, pFullFileNameStr);
        strcpy(m_acTGAFileName, pBaseFileNameStr);

        // it should be safe to over-write m_acName here.
        strcpy(m_acName, pBaseFileNameStr);
#endif

        kFullFileName.UnlockBuffer();
        kBaseFileName.UnlockBuffer();
    }
    else
    {
        return false;
    }

    SavePages (m_hDC);

    ::SelectObject (m_hDC, hOldFont);
    ::SelectObject (m_hDC, hOldBmp);

    if (m_hBitmap)
        DeleteObject (m_hBitmap);
    if (m_hDC)
        DeleteDC (m_hDC);
    if (m_hFont)
        DeleteObject (m_hFont);

    return true;
}
//---------------------------------------------------------------------------
void NiFontCreatorDlg::AdjustSaveButtonState()
{
    // "Save NFF" enabled as long as a font cfg file selected.
    if( m_bFontSelected )
        m_CtrlBtn_Save.EnableWindow();
    else
        m_CtrlBtn_Save.EnableWindow(false);
}
//---------------------------------------------------------------------------
void NiFontCreatorDlg::AdjustAtlasPadState()
{
    // Once we have a handler here, member variable doesn't get automatically
    // set, so set it manually...
    m_bWriteExternalAtlas =
        (m_CtrlCheck_ExternalAtlas.GetCheck() == BST_CHECKED);

    // "Atlas Pad" enabled only if exporting to a TGA atlas
    if (m_bWriteExternalAtlas)
    {
        m_CtrlEdit_AtlasPadTop.EnableWindow();
        m_CtrlEdit_AtlasPadBottom.EnableWindow();
        m_CtrlEdit_AtlasPadLeft.EnableWindow();
        m_CtrlEdit_AtlasPadRight.EnableWindow();
        m_CtrlCheck_AtlasGrid.EnableWindow();
    }
    else
    {
        m_CtrlEdit_AtlasPadTop.EnableWindow(false);
        m_CtrlEdit_AtlasPadBottom.EnableWindow(false);
        m_CtrlEdit_AtlasPadLeft.EnableWindow(false);
        m_CtrlEdit_AtlasPadRight.EnableWindow(false);
        m_CtrlCheck_AtlasGrid.EnableWindow(false);
    }
    m_CtrlStatic_Atlas.EnableWindow(m_bWriteExternalAtlas);
    m_CtrlStatic_AtlasTop.EnableWindow(m_bWriteExternalAtlas);
    m_CtrlStatic_AtlasBottom.EnableWindow(m_bWriteExternalAtlas);
    m_CtrlStatic_AtlasLeft.EnableWindow(m_bWriteExternalAtlas);
    m_CtrlStatic_AtlasRight.EnableWindow(m_bWriteExternalAtlas);
}

//---------------------------------------------------------------------------
void NiFontCreatorDlg::ResetGlyphDataMembers()
{
    m_iGlyphCount = 0;
    m_iNumTextures = 0;

    if (m_pkGlyphMap)
    {
        delete [] m_pkGlyphMap;
        m_pkGlyphMap = NULL;
    }

    if (m_pusGlyphPixelWidthHeight)
    {
        delete [] m_pusGlyphPixelWidthHeight;
        m_pusGlyphPixelWidthHeight = NULL;
    }

    if (m_pusTextureCoords)
    {
        delete [] m_pusTextureCoords;
        m_pusTextureCoords = NULL;
    }
    if (m_piOverhangRight)
    {
        delete [] m_piOverhangRight;
        m_piOverhangRight = NULL;
    }
    if (m_piOverhangLeft)
    {
        delete [] m_piOverhangLeft;
        m_piOverhangLeft = NULL;
    }

    NiUInt32 uiPixelDataCnt = m_apkPixelData.GetSize();
    for (NiUInt32 uiData=0; uiData < uiPixelDataCnt; uiData++)
    {
        DWORD* pdwData = NULL;
        pdwData = m_apkPixelData.GetAt(uiData);
        delete [] pdwData;
    }

    m_apkPixelData.RemoveAll();
    m_aikTextureWidthHeight.RemoveAll();
}
//---------------------------------------------------------------------------
