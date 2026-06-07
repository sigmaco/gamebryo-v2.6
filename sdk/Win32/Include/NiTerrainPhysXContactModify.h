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

#ifndef NITERRAINPHYSXCONTACTMODIFY_H
#define NITERRAINPHYSXCONTACTMODIFY_H

// This PhysX header doesn't compile clean under warning level 4.
// However, the pragma warning(push, 3) will produce another warning
// on PS3 and must be ifdef'd.
#if defined(_XENON) || defined(WIN32)
#pragma warning(push, 3)
#endif

#include <NxUserContactReport.h>

#if defined(_XENON) || defined(WIN32)
#pragma warning(pop)
#endif

#include <NiFixedString.h>
#include <NiRefObject.h>
#include <NiSmartPointer.h>

class NiTerrainInteractor;

/**
    An implementation of PhysX's UserContactModify base class that allows for
    highly accurate readings of the PhysX meta data from terrain materials.
    Meta data from each material present at the collision point is weighted
    and merged to give a true reading.

    It is important that once an instance of this class has been created, the
    scale factor is set to match that of the PhysX scene via the SetScaleFactor
    function.

    @note Using any UserContactModify derived class will have a significant 
        impact on performance and is not recommended by the PhysX documentation.
 */
class NiTerrainPhysXContactModify : public NxUserContactModify, 
    public NiRefObject
{
public: 

    /// Default Constructor
    NiTerrainPhysXContactModify();
    ~NiTerrainPhysXContactModify();

    /// @cond EMERGENT_INTERNAL
    /**
        Called by PhysX when the contact constraint is generated. If either
        given shape corresponds to a terrain, the callback data is assigned
        accurate values from the terrain meta data.

        @param changeFlags Flags which members of data were set from the 
            terrain meta data at the collision point
        @param shape0 one of the two shapes in contact
        @param shape1 the other shape
        @param featureIndex0 feature on the first shape, which is in contact 
            with the other shape
        @param featureIndex1 feature on the second shape, which is in contact 
            with the other shape
        @param data Contact data at the collision point. Valid data members
            are dictated by the changeFlags parameter

        @internal Emergent Internal Use Only
     */
    virtual bool onContactConstraint(NxU32& changeFlags,
        const NxShape* shape0,
        const NxShape* shape1,
        const NxU32 featureIndex0,
        const NxU32 featureIndex1,
        NxContactCallbackData& data);

    /**
        Helper function that extracts terrain meta data for the given shape 
        at the contact point defined in the contact data.

        @param pkInteractor Terrain interactor to which meta data queries will
            be sent
        @param changeFlags Flags which members of data were set from the 
            terrain meta data at the collision point
        @param pkTerrainShape Collision shape that corresponds to the terrain
            to which the interactor belongs
        @param data Contact data at the collision point. Valid data members
        are dictated by the changeFlags parameter
        @param bTerrainIs0 Switch to dictate which of the shapes in the data
            parameter corresponds to the given terrain shape.

        @internal Emergent Internal Use Only
     */
    void OnContactTerrain(NiTerrainInteractor* pkInteractor, 
        NxU32& changeFlags,
        const NxShape* pkTerrainShape,
        NxContactCallbackData& data,
        bool bTerrainIs0);
    /// @endcond

    /**
        Scale factor of the PhysX scene that the terrain belongs too. It is
        important that this value is the same as the PhysX scene.
     */
    //@{
    void SetScaleFactor(float fScale);
    float GetScaleFactor();
    //@}
    
    float m_fScaleFactor;

};

NiSmartPointer(NiTerrainPhysXContactModify);

#endif
