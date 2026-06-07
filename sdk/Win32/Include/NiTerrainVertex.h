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

#ifndef NITERRAINVERTEX_H
#define NITERRAINVERTEX_H

#include <NiMemManager.h>
#include <NiPoint3.h>

#include "NiTerrainLibType.h"
#include "NiIndex.h"

class NiTerrainDataLeaf;

NiSmartPointer(NiTerrainVertex);

class NITERRAIN_ENTRY NiTerrainVertex : public NiRefObject 
{
public:

    /// Default Constructor
    NiTerrainVertex();

    /// Destructor
    ~NiTerrainVertex();
    
    /// Position vertically in terrain space
    //@{
    void SetHeight(float fHeight);
    const float GetHeight() const;
    //@}
    
    /// Sector scope index of this vertex
    //@{
    void SetIndex(const NiIndex& kIndex);
    const NiIndex& GetIndex() const;
    //@}

    /// The leaf in the quad tree that this vertex actually belongs to
    //@{
    inline NiTerrainDataLeaf* GetParentLeaf() const;
    inline void SetParentLeaf(NiTerrainDataLeaf* pkParentLeaf);
    //@}

    /// The first leaf in the quad tree, at any level, that contains a vertex 
    /// at this index
    //@{
    inline NiTerrainDataLeaf* GetFirstContainingLeaf() const;
    inline void SetFirstContainingLeaf(NiTerrainDataLeaf* pkFirstContainingLeaf);
    //@}

    /// @cond EMERGENT_INTERNAL
    /**
        Location of the vertex in the world space.

        @internal
        @note intended for emergent internal use only
    */
    NiPoint3 m_kWorldLocation;
    /// @endcond

private:

    /// Position vertically in terrain space
    float m_fHeight;

    /// The first leaf in the quad tree, at any level, that contains a vertex 
    /// at this index
    NiTerrainDataLeaf* m_pkFirstContainingLeaf;

    /// The leaf in the quad tree that this vertex actually belongs to
    NiTerrainDataLeaf* m_pkParentLeaf;

    /// Our sector scope index coordinate
    NiIndex m_kIndex;

};

#include "NiTerrainVertex.inl"

#endif // NITERRAINVERTEX_H
