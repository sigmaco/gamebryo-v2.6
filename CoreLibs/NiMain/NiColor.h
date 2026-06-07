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

#ifndef NICOLOR_H
#define NICOLOR_H

#include "NiMainLibType.h"
#include "NiMemObject.h"
#include "NiStream.h"
#include "NiMath.h"
#include <NiUniversalTypes.h>

class NiColorA;
class NiRGBA;

class NIMAIN_ENTRY NiColor : public NiMemObject
{
public:
    float r, g, b;

    inline NiColor (float fR = 0.0f, float fG = 0.0f, float fB = 0.0f);

    inline NiColor& operator= (float fScalar);
    inline bool operator== (const NiColor& c) const;
    inline bool operator!= (const NiColor& c) const;

    inline NiColor operator+ (const NiColor& c) const;
    inline NiColor operator- (const NiColor& c) const;
    inline NiColor operator* (float fScalar) const;
    inline NiColor operator* (const NiColor& c) const;
    inline NiColor operator/ (float fScalar) const;
    inline NiColor operator/ (const NiColor& c) const;
    inline NiColor operator- () const;
    NIMAIN_ENTRY friend NiColor operator* (float fScalar, const NiColor& c);

    inline NiColor& operator+= (const NiColor& c);
    inline NiColor& operator-= (const NiColor& c);
    inline NiColor& operator*= (float fScalar);
    inline NiColor& operator*= (const NiColor& c);
    inline NiColor& operator/= (float fScalar);
    inline NiColor& operator/= (const NiColor& c);

    // map to unit cube
    inline void Clamp();
    inline void Scale();

    // Function to return the color as an unsigned integer.
    inline void GetAs(NiRGBA& kColor) const;

    static const NiColor WHITE; // (1,1,1) 
    static const NiColor BLACK; // (0,0,0) 

    // *** begin Emergent internal use only ***
#ifndef __SPU__
    // streaming
    void LoadBinary (NiStream& stream);
    void SaveBinary (NiStream& stream);

    // Utility methods for streaming arrays of NiColor's.
    // Note that the array's must be pre-allocated for Loading, 
    // meaning that the external caller is responsible for
    // streaming the size.
    inline static void LoadBinary (NiStream& stream, NiColor* pkValues, 
        unsigned int uiNumValues);
    inline static void SaveBinary (NiStream& stream, NiColor* pkValues, 
        unsigned int uiNumValues);

    char* GetViewerString (const char* pPrefix) const;
#endif
    // *** end Emergent internal use only ***
};

class NIMAIN_ENTRY NiColorA : public NiMemObject
{
public:
    float r, g, b, a;

    inline NiColorA (float fR = 0.0f, float fG = 0.0f, 
              float fB = 0.0f, float fA = 0.0f);

    inline NiColorA& operator= (float fScalar);
    inline bool operator== (const NiColorA& c) const;
    inline bool operator!= (const NiColorA& c) const;

    inline NiColorA operator+ (const NiColorA& c) const;
    inline NiColorA operator- (const NiColorA& c) const;
    inline NiColorA operator* (float fScalar) const;
    inline NiColorA operator* (const NiColorA& c) const;
    inline NiColorA operator/ (float fScalar) const;
    inline NiColorA operator/ (const NiColorA& c) const;
    inline NiColorA operator- () const;
    NIMAIN_ENTRY friend NiColorA operator* (float fScalar, const NiColorA& c);

    inline NiColorA& operator+= (const NiColorA& c);
    inline NiColorA& operator-= (const NiColorA& c);
    inline NiColorA& operator*= (float fScalar);
    inline NiColorA& operator*= (const NiColorA& c);
    inline NiColorA& operator/= (float fScalar);
    inline NiColorA& operator/= (const NiColorA& c);

    // map (r,g,b,a) to unit hypercube
    inline void Clamp();
    inline void Scale();

    // Function to return the color as an unsigned integer.
    inline void GetAs(NiRGBA& kColor) const;

    static const NiColorA WHITE;  // (1,1,1,1) 
    static const NiColorA BLACK;  // (0,0,0,1) 

    // *** begin Emergent internal use only ***
#ifndef __SPU__
    // streaming
    void LoadBinary (NiStream& stream);
    void SaveBinary (NiStream& stream);

    // Utility methods for streaming arrays of NiColorA's.
    // Note that the array's must be pre-allocated for Loading, 
    // meaning that the external caller is responsible for
    // streaming the size.
    inline static void LoadBinary (NiStream& stream, NiColorA* pkValues, 
        unsigned int uiNumValues);
    inline static void SaveBinary (NiStream& stream, NiColorA* pkValues, 
        unsigned int uiNumValues);

    char* GetViewerString (const char* pPrefix) const;
#endif

    // *** end Emergent internal use only ***
};

class NIMAIN_ENTRY NiRGBA : public NiMemObject
{
public:

    inline NiRGBA (NiUInt8 uiR = 0, NiUInt8 uiG = 0, NiUInt8 uiB = 0, 
        NiUInt8 uiA = 0);

    inline NiUInt8& r();
    inline const NiUInt8& r() const;
    inline NiUInt8& g();
    inline const NiUInt8& g() const;
    inline NiUInt8& b();
    inline const NiUInt8& b() const;
    inline NiUInt8& a();
    inline const NiUInt8& a() const;

    inline NiRGBA& operator= (NiUInt8 uiValue);
    inline bool operator== (const NiRGBA& c) const;
    inline bool operator!= (const NiRGBA& c) const;

    inline NiRGBA operator+ (const NiRGBA& c) const;
    inline NiRGBA operator- (const NiRGBA& c) const;
    inline NiRGBA operator* (float fScalar) const;
    inline NiRGBA operator* (const NiRGBA& c) const;
    inline NiRGBA operator/ (float fScalar) const;
    inline NiRGBA operator/ (const NiRGBA& c) const;
    inline NiRGBA operator- () const;
    NIMAIN_ENTRY friend NiRGBA operator* (float fScalar, const NiRGBA& c);

    inline NiRGBA& operator+= (const NiRGBA& c);
    inline NiRGBA& operator-= (const NiRGBA& c);
    inline NiRGBA& operator*= (float fScalar);
    inline NiRGBA& operator*= (const NiRGBA& c);
    inline NiRGBA& operator/= (float fScalar);
    inline NiRGBA& operator/= (const NiRGBA& c);

    // Function to return the color as an unsigned integer.
    inline void GetAs(NiUInt32& uiColor) const;
    inline void GetAsBGRA(NiUInt32& uiColor) const;

    // Function to return the color as floats between 0 - 1.
    inline void GetAs(NiColor& kColor) const;
    inline void GetAs(NiColorA& kColor) const;

    static const NiRGBA WHITE; // (1,1,1,1) 
    static const NiRGBA BLACK; // (0,0,0,1) 

    // *** begin Emergent internal use only ***
#ifndef __SPU__
    // streaming
    void LoadBinary(NiStream& kStream);
    void SaveBinary(NiStream& kStream);

    // Utility methods for streaming arrays of NiRGBA objects.
    // Note that the arrays must be pre-allocated for Loading, 
    // meaning that the external caller is responsible for
    // streaming the size.
    inline static void LoadBinary(NiStream& stream, NiRGBA* pkValues, 
        unsigned int uiNumValues);
    inline static void SaveBinary(NiStream& stream, NiRGBA* pkValues, 
        unsigned int uiNumValues);

    char* GetViewerString(const char* pcPrefix) const;
#endif
    // *** end Emergent internal use only ***

private:

    struct Color
    {
        NiUInt8 r, g, b, a;
    };

    union 
    {
        Color m_kColor;
        NiUInt32 m_uiColor;
    }; 
};

//---------------------------------------------------------------------------
//  Inline include
#include "NiColor.inl"

//---------------------------------------------------------------------------

#endif
