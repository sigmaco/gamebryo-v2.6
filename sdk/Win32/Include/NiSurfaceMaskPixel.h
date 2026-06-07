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

#ifndef NITERRAINPIXEL_H
#define NITERRAINPIXEL_H

#include <NiMemManager.h>

#include "NiTerrainLibType.h"
#include "NiIndex.h"
#include "NiSurfaceMask.h"

NiSmartPointer(NiSurfaceMaskPixel);

/**
    A wrapper class around a pixel inside a surface mask that is used to
    retrieve the value of the mask at the corresponding point and to store
    quadtree information to be used by deformation.

    A pixel object will only ever be created for a pixel that has been 
    requested for deformation, the mask will not have pixel objects created for
    it unless they have been specifically requested.
 */
class NITERRAIN_ENTRY NiSurfaceMaskPixel : public NiRefObject 
{

public:

    /// Default constructor.
    NiSurfaceMaskPixel();

    /**
        Parameterized constructor, initializes member variables.

        @param pkFirstContainingLeaf The first data leaf in the quadtree 
            that contains this pixel
        @param pkMask The surface mask that this pixel refers to
        @param uiX The X component of the position in the referred mask
        @param uiY The Y component of the position in the referred mask
     */
    NiSurfaceMaskPixel(
        NiTerrainDataLeaf* pkFirstContainingLeaf,
        const NiSurfaceMask* pkMask, NiUInt32 uiX, NiUInt32 uiY);

    /**
        Parameterized constructor, initializes member variables

        @param pkFirstContainingLeaf The first data leaf in the quadtree 
            that contains this pixel
        @param pkMask The surface mask that this pixel refers to
        @param kPosition The position in the referred mask
     */
    NiSurfaceMaskPixel(
        NiTerrainDataLeaf* pkFirstContainingLeaf, 
        const NiSurfaceMask* pkMask, NiIndex kPosition);
    
    /// Destructor
    ~NiSurfaceMaskPixel();

    /// @name Member Accessors
    //@{

    /**
        Get and set the X and Y components of the position in the mask that 
        this pixel refers to.
     */
    //@{
    /// Get the X component of the position in the mask that this pixel
    /// refers to.
    NiUInt32 GetX() const;

    /// Get the Y component of the position in the mask that this pixel
    /// refers to.
    NiUInt32 GetY() const;

    /// Set the X component of the position in the mask that this pixel
    /// refers to.
    void SetX(NiUInt32 uiX);

    /// Set the Y component of the position in the mask that this pixel
    /// refers to.
    void SetY(NiUInt32 uiY);

    /// Get the value of the mask at the position marked by this pixel.
    NiUInt8 GetValue() const;

    /// Get the surface mask that this pixel refers to.
    inline const NiSurfaceMask* GetMask() const;

    /// Set the surface mask that this pixel refers to.
    inline void SetMask(const NiSurfaceMask* pkMask);

    /// Get the first leaf in the quad data tree that contains this pixel.
    inline NiTerrainDataLeaf* GetFirstContainingLeaf();

    /// Get a constant pointer to the first leaf in the quad data tree that
    /// contains this pixel.
    inline const NiTerrainDataLeaf* GetFirstContainingLeaf() const;

    /// Set the first leaf in the quad data tree that contains this pixel.
    inline void SetFirstContainingLeaf(NiTerrainDataLeaf* pkLeaf);

    //@}

private:

    NiIndex m_kPosition;
    const NiSurfaceMask* m_pkMask;
    NiTerrainDataLeaf* m_pkFirstContainingLeaf;
};

#include "NiSurfaceMaskPixel.inl"

#endif
