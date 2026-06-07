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

#ifndef NITERRAINDECALMANAGER_H
#define NITERRAINDECALMANAGER_H

#include <NiEntityErrorInterface.h>

#include "NiTerrainLibType.h"

class NiTerrainDecal;

/**
    This class is responsible for managing a set of decals, using the reference
    count of the decal as a flag for deletion by the containing sector.
 */
class NITERRAIN_ENTRY NiTerrainDecalManager : public NiRefObject
{
public:
    
    /** @name Constructors */
    //@{

    /// Default constructor
    NiTerrainDecalManager();
    //@}

    /// Destructor
    ~NiTerrainDecalManager();

    /** @name Decal access functions */
    //@{

    /** 
        Adds a new decal.

        @param pkDecal pointer to the decal to add    
    */
    void AddDecal(NiTerrainDecal* pkDecal);
    
    /** 
        Removes specified decal and decreases its reference count by 1. If the
        reference count reaches zero however, the decal is NOT deleted.

        @param pkDecal Pointer to the decal to remove    
    */
    void RemoveDecal(NiTerrainDecal* pkDecal);

    /** 
        Removes all decals from the manager, decreasing each ones reference
        count by 1.  If a decals reference count reaches zero, it is however 
        NOT deleted.
     */
    void RemoveAll();

    //@}
    /** @name Update, Rendering */
    //@{

    /** 
        Updates the all the listed decals. This is where decals will be removed
            if their time has come

        @param pkSector Sector that contains the sectors within this manager
        @param fDeltaTime Time since UpdateDecals was last called. If this is
            the first call to this function, fDeltaTime should be 0.0
    */
    void UpdateDecals(NiTerrainSector* pkSector, float fDeltaTime);

    /** 
        Adds the appropriate decals to the visible set. It also defines part
            of the alpha value of a decal depending on the camera distance.
    
        @param kCuller Culling process containing the camera and the
            visible set all decals should be added to.
    */
    void Cull(NiCullingProcess &kCuller);
    
    //@}

private:

    /// List of Decals
    NiTPointerList<NiTerrainDecal*> m_kDecalList;

    /// Map matching decal objects to their position in the decal list
    NiTMap<NiTerrainDecal*, NiTListIterator> m_kDecalMap;
};

NiSmartPointer(NiTerrainDecalManager);

#endif
