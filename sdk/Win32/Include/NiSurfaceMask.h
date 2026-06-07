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

#ifndef NISURFACEMASK
#define NISURFACEMASK

#include <NiMemManager.h>

#include "NiTerrainLibType.h"

/** 
    Masks, like blocks, have their internal coordinates system origin at the
    BOTTOM LEFT of the data set.
 */
class NITERRAIN_ENTRY NiSurfaceMask : public NiMemObject
{
public:

    /// Default constructor
    NiSurfaceMask();
    
    /** 
        Get the width of the texture.
        
        Note that since we are a square power of 2 texture, the width and
        height should always be equal.
        
        @return the width (and height) of the mask in pixels.
     */
    NiUInt32 GetWidth() const;

    /** 
        Get the mask value at a particular coordinate.

        @note No bounds checking is performed by this function.

        @param uiX The X coordinate to retrieve.
        @param uiY The Y coordinate to retrieve.
        @return The mask value at the given index.
     */
    NiUInt8 GetAt(NiUInt32 uiX, NiUInt32 uiY) const;

    /** 
        Set the mask value at a particular coordinate.
        
        @note No bounds checking is performed by this function.

        @param uiX the X coordinate to modif.y
        @param uiY the Y coordinate to modify.
        @param ucValue the value to set.
     */
    void SetAt(NiUInt32 uiX, NiUInt32 uiY, NiUInt8 ucValue);

    /// @cond EMERGENT_INTERNAL
    
    /// The mask itself, 8 bit per channel gray-scale.
    ///
    /// The mask is made up of pointers to values, not the values themselves.
    NiUInt8* m_pucMask;

    /// The index of this mask, in the internal memory mask stack.
    NiUInt32 m_uiMaskIndex;

    /// The width should always equal the height, since we are a square power 
    /// of 2 texture.
    NiUInt32 m_uiWidth;
    
    /// @endcond
};

#include "NiSurfaceMask.inl"

#endif

