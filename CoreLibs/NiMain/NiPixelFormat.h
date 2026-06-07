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
#ifndef NIPIXELFORMAT_H
#define NIPIXELFORMAT_H

#include "NiMainLibType.h"
#include "NiFlags.h"
#include <NiRTLib.h>
#include <NiMemObject.h>
class NiStream;

class NIMAIN_ENTRY NiPixelFormat : public NiMemObject
{
    NiDeclareFlags(unsigned char); 
public:
    // Specify the Meta-format of the image
    enum Format
    {
        FORMAT_RGB = 0,
        FORMAT_RGBA,
        FORMAT_PAL,
        FORMAT_PALALPHA,
        FORMAT_DXT1,
        FORMAT_DXT3,
        FORMAT_DXT5,
        FORMAT_RGB24NONINTERLEAVED,
        FORMAT_BUMP,
        FORMAT_BUMPLUMA,
        FORMAT_RENDERERSPECIFIC,
        FORMAT_ONE_CHANNEL,
        FORMAT_TWO_CHANNEL,
        FORMAT_THREE_CHANNEL,
        FORMAT_FOUR_CHANNEL,
        FORMAT_DEPTH_STENCIL,
        FORMAT_UNKNOWN,
        FORMAT_BUMPLUMA_WII,
        FORMAT_MAX
    };

    // Specify the semantic meaning of a channel
    // in the image
    enum Component
    {
        COMP_RED = 0,
        COMP_GREEN,
        COMP_BLUE,
        COMP_ALPHA,
        COMP_COMPRESSED,
        COMP_OFFSET_U,
        COMP_OFFSET_V,         
        COMP_OFFSET_W,
        COMP_OFFSET_Q,
        COMP_LUMA,
        COMP_HEIGHT,
        COMP_VECTOR_X,
        COMP_VECTOR_Y,
        COMP_VECTOR_Z,       
        COMP_PADDING,
        COMP_INTENSITY,
        COMP_INDEX,
        COMP_DEPTH,
        COMP_STENCIL,
        COMP_EMPTY,
        COMP_MAX,
        NUM_COMPS = 4 
    };
    // Specify the data storage convention
    enum Representation
    {
        REP_NORM_INT = 0,
        REP_HALF,
        REP_FLOAT,
        REP_INDEX,
        REP_COMPRESSED,
        REP_UNKNOWN,
        REP_INT,
        REP_MAX
    };

    // Specify the tiling support of the image
    enum Tiling
    {
        TILE_NONE = 0,
        TILE_XENON = 1,
        TILE_WII = 2,
        TILE_NV_SWIZZLED = 3,
        TILE_MAX
    };

    static const unsigned int INVALID_RENDERER_HINT = 0xFFFFFFFF;

    NiPixelFormat();

    NiPixelFormat(Format eFormat, 
        Component eComp, Representation eRep, unsigned char ucBPP);
    NiPixelFormat(Format eFormat, 
        Component eComp0, Representation eRep0, unsigned char ucBPC0,
        Component eComp1, Representation eRep1, unsigned char ucBPC1);
    NiPixelFormat(Format eFormat, 
        Component eComp0, Representation eRep0, unsigned char ucBPC0, 
        Component eComp1, Representation eRep1, unsigned char ucBPC1, 
        Component eComp2, Representation eRep2, unsigned char ucBPC2);
    NiPixelFormat(Format eFormat, 
        Component eComp0, Representation eRep0, unsigned char ucBPC0, 
        Component eComp1, Representation eRep1, unsigned char ucBPC1, 
        Component eComp2, Representation eRep2, unsigned char ucBPC2, 
        Component eComp3, Representation eRep3, unsigned char ucBPC3);
    NiPixelFormat(Format eFormat, unsigned char ucBPP, Tiling eTiled, 
        bool bLittleEndian, unsigned int uiRendererHint,
        Component eComp0, Representation eRep0, unsigned char ucBPC0,
        bool bSigned0, Component eComp1, Representation eRep1, 
        unsigned char ucBPC1, bool bSigned1, Component eComp2, 
        Representation eRep2, unsigned char ucBPC2, bool bSigned2,
        Component eComp3, Representation eRep3, unsigned char ucBPC3, 
        bool bSigned3);
    NiPixelFormat(Format eFormat, unsigned char ucBPP, Tiling eTiled, 
        bool bLittleEndian, unsigned int uiRendererHint, bool bSRGBSpace,
        Component eComp0, Representation eRep0, unsigned char ucBPC0,
        bool bSigned0, Component eComp1, Representation eRep1, 
        unsigned char ucBPC1, bool bSigned1, Component eComp2, 
        Representation eRep2, unsigned char ucBPC2, bool bSigned2,
        Component eComp3, Representation eRep3, unsigned char ucBPC3, 
        bool bSigned3);

    static NiPixelFormat CreateFromRGBAMasks(unsigned char ucBitsPerPixel,
        unsigned int uiRedMask, unsigned int uiGreenMask, 
        unsigned int uiBlueMask, unsigned int uiAlphaMask);

    Format GetFormat() const;
    inline unsigned char GetBitsPerPixel() const;
    inline unsigned int GetRendererHint() const;
    inline unsigned int GetExtraData() const;
    Tiling GetTiling() const;
    inline bool GetLittleEndian() const;
    inline bool GetSRGBSpace() const;

    inline void SetFormat(Format eFormat);
    inline void SetBitsPerPixel(unsigned char ucBPP);
    inline void SetRendererHint(unsigned int uiRendererHint);
    inline void SetExtraData(unsigned int uiExtraData);
    inline void SetTiling(Tiling eTiling);
    inline void SetLittleEndian(bool bLittleEndian);
    inline void SetSRGBSpace(bool bSRGB);

    inline bool GetComponent(unsigned int uiWhichComponent, 
        Component& eComponent, Representation& eRep,
        unsigned char& ucBPC, bool& bSigned) const;
    inline void SetComponent(unsigned int uiWhichComponent, 
        Component eComponent, Representation eRep,
        unsigned char ucBPC, bool bSigned);

    inline unsigned int GetNumComponents() const;
        
    inline bool GetPalettized() const;
    inline bool GetPaletteHasAlpha() const;
    inline bool GetCompressed() const;

    inline unsigned int GetMask(Component eComp) const;
    inline unsigned char GetBits(Component eComp) const;
    inline unsigned char GetShift(Component eComp) const;       

    inline bool operator==(const NiPixelFormat& kPixDesc) const;
    inline bool operator!=(const NiPixelFormat& kPixDesc) const;

    // Represent the Gamebryo "standard" pixel formats.
    // Note that the naming convention for these variables is to 
    // list the channels from lowest value to highest value. Note
    // that all of these formats are explicitly little endian.
    const static NiPixelFormat PAL4;
    const static NiPixelFormat PALA4;
    const static NiPixelFormat PAL8;
    const static NiPixelFormat PALA8;
    const static NiPixelFormat RGB24;
    const static NiPixelFormat RGBA32;
    const static NiPixelFormat BUMP16;
    const static NiPixelFormat BUMPLUMA32;
    const static NiPixelFormat BUMPLUMA32_WII;
    const static NiPixelFormat RGBA16;
    const static NiPixelFormat BGRA16;
    const static NiPixelFormat BGRA4443;
    const static NiPixelFormat DXT1;
    const static NiPixelFormat DXT3;
    const static NiPixelFormat DXT5;
    const static NiPixelFormat R16;
    const static NiPixelFormat R32;
    const static NiPixelFormat RG32;
    const static NiPixelFormat RG64;
    const static NiPixelFormat RGBA64;
    const static NiPixelFormat RGBA128;

    // Represent the Gamebryo "standard" pixel formats on big-endian platforms.
    // There is only a distinction made for types that have individual 
    // components larger than 8 bits, or are packed into a single
    // 'super-component' of more than 8 bits (e.g., 16 bpp RGB/RGBA formats).
    const static NiPixelFormat R16_BE;
    const static NiPixelFormat R32_BE;
    const static NiPixelFormat RG32_BE;
    const static NiPixelFormat RG64_BE;
    const static NiPixelFormat RGBA64_BE;
    const static NiPixelFormat RGBA128_BE;
    const static NiPixelFormat RGBA5551_BE;
    const static NiPixelFormat BGRA5551_BE;
    const static NiPixelFormat RGBA32_BE;  
    const static NiPixelFormat RGBA16_BE;
    const static NiPixelFormat BGR565_BE;
    const static NiPixelFormat BGRA4444_BE;
    const static NiPixelFormat BGRA4443_BE;

    // Represent additional Gamebryo pixel formats
    // that are used by the renderers but not one of the
    // standard convertable formats
    // Note that the naming convention for these variables is to 
    // list the channels from lowest value to highest value,
    // followed by the bit counts of the channels in order. Note
    // that all of these formats are explicitly little endian.
    const static NiPixelFormat I8;
    const static NiPixelFormat A8;
    const static NiPixelFormat IA88;
    const static NiPixelFormat I4;
    const static NiPixelFormat IA44;
    const static NiPixelFormat BGR233;
    const static NiPixelFormat BGRA2338;
    const static NiPixelFormat RGB555;
    const static NiPixelFormat BGR555;
    const static NiPixelFormat BGR565;
    const static NiPixelFormat RGBA5551;
    const static NiPixelFormat BGRA5551;
    const static NiPixelFormat BGRX5551;
    const static NiPixelFormat BGRA4444;
    const static NiPixelFormat BGRX4444;
    const static NiPixelFormat RGBA1010102;
    const static NiPixelFormat BGRA1010102;
    const static NiPixelFormat RGBA1010102F;
    const static NiPixelFormat GR1616;
    const static NiPixelFormat BGR888;
    const static NiPixelFormat BGRA8888;
    const static NiPixelFormat BGRX8888;
    const static NiPixelFormat RGBX8888;
    const static NiPixelFormat ARGB8888;
    const static NiPixelFormat BUMPLUMA556;
    const static NiPixelFormat PA88;
    const static NiPixelFormat L8;
    const static NiPixelFormat L16;
    const static NiPixelFormat LA88;
    const static NiPixelFormat LA44;
    const static NiPixelFormat BUMPWQ8888;
    const static NiPixelFormat BUMP1616;
    const static NiPixelFormat BUMPW101111;
    const static NiPixelFormat BUMPLUMAX8888;
    const static NiPixelFormat BUMPWA1010102;
    const static NiPixelFormat RENDERERSPECIFICCOMPRESSED;
    const static NiPixelFormat RENDERERSPECIFIC16;
    const static NiPixelFormat RENDERERSPECIFIC32;
    const static NiPixelFormat RENDERERSPECIFIC64;

    // Depth/Stencil buffer formats
    const static NiPixelFormat DEPTH8;
    const static NiPixelFormat DEPTH16;
    const static NiPixelFormat DEPTH32;
    const static NiPixelFormat STENCILDEPTH115;
    const static NiPixelFormat STENCILDEPTH824;
    const static NiPixelFormat STENCILDEPTH824FLOAT;
    const static NiPixelFormat XDEPTH824;
    const static NiPixelFormat STENCILXDEPTH4424;

    // *** begin Emergent internal use only ***

    // streaming
    void LoadBinary(NiStream& kStream);
    void SaveBinary(NiStream& kStream);

    enum EComparisonAxes
    {
        // OR these values together for axes that count as mismatches
        CMP_BASIC =         (1<<0),   // bpp, components
        CMP_SRGB =          (1<<1),   // sRGB
        CMP_ENDIAN =        (1<<2),
        CMP_TILING =        (1<<3),
        CMP_RENDERER_HINT = (1<<4),
        CMP_FORMAT =        (1<<5),   // format
        
        // AND these values together for axes that are ignored (can be anded
        // with ORs above)
        CMP_DONTCARE_ENDIAN = ~CMP_ENDIAN,
        CMP_DONTCARE_TILING = ~CMP_TILING
    };

    bool FunctionallyIdentical(const NiPixelFormat& kPixDesc,
        unsigned int uiComparisonMode) const;

    bool ComponentCrossesByteBoundary() const;
    // *** end Emergent internal use only ***
    
protected:

    enum
    {
        ENDIAN_MASK = 0x01
    };

    unsigned char m_ucBitsPerPixel;
    bool m_bSRGBSpace;
    Format m_eFormat;
    Tiling m_eTiling;
    unsigned int m_uiRendererHint;
    unsigned int m_uiExtraData;

    class NiComponentSpec : public NiMemObject
    {
    public:
        Component m_eComponent;
        Representation m_eRepresentation;
        unsigned char m_ucBitsPerComponent;
        bool m_bSigned;
    };
    
    NiComponentSpec m_akComponents[NUM_COMPS];

    void InitPixelFormat(Format eFormat, unsigned char ucBPP,  Tiling eTiled,
        bool bLittleEndian, unsigned int uiRendererHint, bool bSRGBSpace,
        Component eComp0, Representation eRep0, unsigned char ucBPC0, 
        bool bSigned0, Component eComp1, Representation eRep1, 
        unsigned char ucBPC1, bool bSigned1, Component eComp2, 
        Representation eRep2, unsigned char ucBPC2, bool bSigned2,
        Component eComp3, Representation eRep3, unsigned char ucBPC3,
        bool bSigned3);

    void ConvertOldPixelFormat(Format eFormat, unsigned char ucBPP, 
        unsigned int auiColorMasks[4]);

    unsigned char RoundBitsUpToNextByte(unsigned char ucNumOfBits) const;

    static unsigned int CalcMaskSize(unsigned int uiMask);
};

#include "NiPixelFormat.inl"

#endif
