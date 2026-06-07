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

#ifndef NIUIATLASMAP_H
#define NIUIATLASMAP_H

#include "NiUserInterfaceLibType.h"
#include <NiSourceTexture.h>
#include <NiString.h>
#include <NiPoint2.h>
#include <NiRect.h>

class NIUSERINTERFACE_ENTRY NiUIAtlasMap
{
public:
    NiUIAtlasMap();
    virtual ~NiUIAtlasMap();
    bool ReadFile(const char* pcAtlasFilename);

    enum UIElement
    {
        GAMEPAD_BUTTON_RRIGHT = 0,
        GAMEPAD_BUTTON_RLEFT,
        GAMEPAD_BUTTON_RUP,
        GAMEPAD_BUTTON_RDOWN,
        GAMEPAD_BUTTON_R2,
        GAMEPAD_BUTTON_L2,
        GAMEPAD_BUTTON_R1,
        GAMEPAD_BUTTON_L1,
        GAMEPAD_BUTTON_SELECT,
        GAMEPAD_BUTTON_START,
        GAMEPAD_CONTROL_STICK_HORIZONTAL,
        GAMEPAD_CONTROL_STICK_VERTICAL,
        GAMEPAD_CONTROL_STICK_OMNI,
        GAMEPAD_CONTROL_STICK,
        GAMEPAD_DPAD_HORIZONTAL,
        GAMEPAD_DPAD_VERTICAL,
        GAMEPAD_DPAD_OMNI,
        GAMEPAD_DPAD,
        MOUSE_BUTTON_LEFT,
        MOUSE_BUTTON_RIGHT,
        MOUSE_BUTTON_SCROLL,
        MOUSE_MOVE,
        MOUSE_NULL,
        KEYBOARD_BUTTON_WIDE,
        KEYBOARD_BUTTON,
        UI_LABEL,
        UI_BUTTON_DEFAULT,
        UI_BUTTON_DOWN,
        UI_BUTTON_HIGH,
        UI_CHECK_BOX_DEFAULT,
        UI_CHECK_BOX_HIGH,
        UI_CHECK_MARK,
        UI_SLIDER_TRACK_DEFAULT,
        UI_SLIDER_TRACK_HIGH,
        UI_SLIDER_MARK_DEFAULT,
        UI_SLIDER_MARK_HIGH,
        UI_GROUP,
        UI_MIN_DEFAULT, 
        UI_MIN_DOWN,    
        UI_MIN_HIGH,        
        UI_MAX_DEFAULT, 
        UI_MAX_DOWN,        
        UI_MAX_HIGH,
#if defined(_WII)
        WII_REMOTE_WAGGLE,
        WII_REMOTE_HOME,
#endif
        NUM_ELEMENTS
    };

    enum TextColor
    {
        TEXT_ON_KEYBOARD_BUTTON = 0,
        TEXT_ON_UI_DEFAULT,
        TEXT_ON_UI_DOWN,
        TEXT_ON_UI_HIGH,
        TEXT_ON_GAMEPAD_CONTROL_STICK,

        NUM_TEXT_COLORS
    };

    enum TextSize
    {
        TEXT_FOR_LOW_RESOLUTION = 0,
        TEXT_FOR_MED_RESOLUTION,
        TEXT_FOR_HIGH_RESOLUTION,

        NUM_TEXT_SIZES
    };

    enum CoordinateSystem
    {
        ATLAS_UV_SPACE = 0,
        ATLAS_PIXEL_SPACE,
        DEVICE_NDC_SPACE
    };

    inline const NiColor GetTextColor(TextColor eUITextColor) const;
    inline const NiString GetTextFilename(TextSize eTextSize) const;
    inline const NiRect<float> GetOuterBounds(UIElement eElement,
        CoordinateSystem eSystem = ATLAS_UV_SPACE) const;
    inline const NiRect<float> GetInnerBounds(UIElement eElement,
        CoordinateSystem eSystem = ATLAS_UV_SPACE) const;
    const NiRect<float> GetOuterBoundsDifference(UIElement eElement,
        CoordinateSystem eSystem = ATLAS_UV_SPACE) const;
    inline NiSourceTexture* GetAtlas() const;

protected:
    enum Groups
    {
        FILES = 0,
        IMAGERECTS,
        COLORS,
        NUM_GROUPS
    };
    inline NiPoint2 Translate(unsigned int eElement, const NiPoint2& kOrigUV) 
        const;


    NiSourceTexturePtr m_spAtlas;
    NiRect<float> m_akBounds[NUM_ELEMENTS];
    NiRect<float> m_akInternalBounds[NUM_ELEMENTS];
    NiString m_akTextFilenames[NUM_TEXT_SIZES];
    NiColorA m_akTextColors[NUM_TEXT_COLORS];

    static const char* ms_apcTextColorNames[NUM_TEXT_COLORS];
    static const char* ms_apcTextSizeNames[NUM_TEXT_SIZES];
    static const char* ms_apcGroupNames[NUM_GROUPS];
    static const char* ms_apcElementNames[NUM_ELEMENTS];
    inline int ParseGroup(char*& pcGroup) const;
    inline int ParseElement(char*& pcElementName) const;
    inline int ParseTextColor(char*& pcTextColorName) const;
    inline int ParseTextSize(char*& pcTextSizeName) const;
    inline bool ParseFilename(char*& pcFilename, NiString& kValue) const;
    inline bool ParseInt(char*& pcFilename, int& iValue) const;
    inline bool ParseFloat(char*& pcFilename, float& fValue) const;

    NiRect<float> ConvertAtlasUVToPixels(const NiRect<float>& kCoords) const;
    NiRect<float> ConvertAtlasUVToNDC(
        const NiRect<float>& kCoords) const;

    NiRect<float> m_kLabelCoords;

    NiPoint2 m_kPixelToNDC;
};

#include "NiUIAtlasMap.inl"

#endif
