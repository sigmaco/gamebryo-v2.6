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
#ifndef NI2DBUFFER_H
#define NI2DBUFFER_H

#include "NiPixelFormat.h"
#include "NiMath.h"
#include "NiObject.h"
#include "NiRect.h"

class NiRenderer;

class NIMAIN_ENTRY Ni2DBuffer : public NiObject
{
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    virtual ~Ni2DBuffer();

    enum CopyFilterPreference
    {
        COPY_FILTER_NONE   = 0,   // No filtering
        COPY_FILTER_POINT  = 1,   // Point filtering
        COPY_FILTER_LINEAR = 2,   // Bilinear interpolation filtering 
        COPY_FILTER_MAX
    };

    // MultiSample Anti-Aliasing Preference 
    typedef unsigned short MultiSamplePreference;

    static void GetMSAACountAndQualityFromPref(MultiSamplePreference ePref, 
        unsigned int& uiCount, unsigned int& uiQuality);
    static MultiSamplePreference GetMSAAPrefFromCountAndQuality(
        unsigned int uiCount, unsigned int uiQuality);

    // Old-style MSAA enumerations for backwards compatibility
    static const MultiSamplePreference MULTISAMPLE_NONE = 0x0100;
    static const MultiSamplePreference MULTISAMPLE_2 = 0x0200;
    static const MultiSamplePreference MULTISAMPLE_4 = 0x0400;
    static const MultiSamplePreference MULTISAMPLE_4_ROTATE = 0x0401;

    inline unsigned int GetWidth() const;
    inline unsigned int GetHeight() const;

    inline const NiPixelFormat* GetPixelFormat() const;
    MultiSamplePreference GetMSAAPref() const;

    // Copy an arbitrary region from one 2D buffer to the specified 
    // coordinates of a different 2D buffer. Note that the NiPixelFormats
    // of both Ni2DBuffers must match for this method. This is the fastest 
    // path possible for copying pixel data. Also note that a NULL value for 
    // the NiRect will use the dimensions of the Ni2DBuffer.
    virtual bool FastCopy(Ni2DBuffer* pkDest, 
        const NiRect<unsigned int>* pkSrcRect = NULL,
        unsigned int uiDestX = 0, unsigned int uiDestY = 0) const;
    // Copy an arbitrary region from one 2D buffer to another arbitrary region
    // in a different 2D buffer. Note that the NiPixelFormats
    // of both Ni2DBuffers may differ for this method. Also note that a NULL 
    // value for either NiRect will use the dimensions of the Ni2DBuffer.
    // This path is slower than the previous FastCopy method
    virtual bool Copy(Ni2DBuffer* pkDest, 
        const NiRect<unsigned int>* pkSrcRect,
        const NiRect<unsigned int>* pkDestRect,
        Ni2DBuffer::CopyFilterPreference ePref) const;
    /*** begin Emergent internal use only ***/
    
    // Renderer specific data class
    class NIMAIN_ENTRY RendererData : public NiRefObject
    {
    public:
        virtual ~RendererData();

        inline unsigned int GetWidth() const;
        inline unsigned int GetHeight() const;
        inline MultiSamplePreference GetMSAAPref() const;
        inline const NiPixelFormat* GetPixelFormat() const;

    protected:
        RendererData();
        RendererData(Ni2DBuffer* pkBuffer);
        Ni2DBuffer* m_pkBuffer;
        const NiPixelFormat* m_pkPixelFormat;
        MultiSamplePreference m_eMSAAPref;
    };
    typedef NiPointer<RendererData> RendererDataPtr;

    // Create methods for Ni2DBuffer. These methods are Emergent internal use 
    // only. 

    // Create a 2D buffer from specified renderer data. 
    static Ni2DBuffer* Create(unsigned int uiWidth, unsigned int uiHeight,
        Ni2DBuffer::RendererData* pkData);
    // Create a 2D buffer, but do not initialize its renderer data. Before
    // this buffer can be used, its data must be set by the SetRendererData
    // method.
    static Ni2DBuffer* Create(unsigned int uiWidth, unsigned int uiHeight);

    inline void SetRendererData(RendererData* pkRendererData);
    RendererData* GetRendererData() const;
    
    void ResetDimensions(unsigned int uiWidth, unsigned int uiHeight);
    /*** end Emergent internal use only ***/

protected:
    Ni2DBuffer();

    virtual bool CreateRendererData(const NiPixelFormat& pkFormat,
        Ni2DBuffer::MultiSamplePreference eMSAAPref = 
        Ni2DBuffer::MULTISAMPLE_NONE);

    unsigned int m_uiWidth;
    unsigned int m_uiHeight;

    RendererDataPtr m_spRendererData;
};

NiSmartPointer(Ni2DBuffer);


#include "Ni2DBuffer.inl"
#endif
