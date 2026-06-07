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

#ifndef NIPSFIELDFORCE_H
#define NIPSFIELDFORCE_H

#include "NiPSForce.h"
#include <NiAVObject.h>

/**
    The abstract base class for all field force particle modifiers.

    NiPSFieldForce defines a force over a field of space centered at the world
    position of a field object in the scene graph. Options can be set
    affecting the magnitude, attenuation, and maximum distance of the force.
    
    In general, the final force of a field is equal to the magnitude divided
    by 1.0 plus the product of the attenuation and the distance from the
    field object to the particle:

    Force = Magnitude / (1.0 + Attenuation * Distance)
    
    This force and all derived forces most closely approximate the behavior of
    field modifiers in Maya.
*/
class NIPARTICLE_ENTRY NiPSFieldForce : public NiPSForce
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareAbstractClone(NiPSFieldForce);
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    //@{
    /// Accesses the object in the scene graph whose world-space
    /// transformations will be used to represent the position and orientation
    /// of the field.
    inline NiAVObject* GetFieldObj() const;
    inline void SetFieldObj(NiAVObject* pkFieldObj);
    //@}

    //@{
    /// Accesses the magnitude of the force.
    inline float GetMagnitude() const;
    inline void SetMagnitude(float fMagnitude);
    //@}
   
    //@{
    /// Accesses how the magnitude of the force diminishes with distance from
    /// the field object.
    inline float GetAttenuation() const;
    inline void SetAttenuation(float fAttenuation);
    //@}
   
    //@{
    /// Accesses whether or not to use a maximum distance from the filed object
    /// after which the force has no effect.
    inline bool GetUseMaxDistance() const;
    inline void SetUseMaxDistance(bool bUseMaxDistance);
    //@}

    //@{
    /**
        Accesses the maximum distance from the field object at which the force
        has any effect.

        This distance is only used if GetUseMaxDistance returns true.
    */
    inline float GetMaxDistance() const;
    inline void SetMaxDistance(float fMaxDistance);
    //@}

    /// @cond EMERGENT_INTERNAL

    /// @name Base Class Overrides
    //@{
    virtual void ProcessClone(NiCloningProcess& kCloning);
    //@}

    /// @endcond

protected:
    /// @name Construction and Destruction
    //@{
    /**
        Protected main constructor.

        @param kName The name of the force.
        @param eType The type of the force.
        @param pkInputData A pointer to a structure containing the data for
            the field force.
        @param pkFieldObj The object in the scene graph whose world-space
            transformations will be used to represent the position and
            orientation of the field.
        @param fMagnitude The magnitude of the force.
        @param fAttenuation How the magnitude of the force diminishes with
            distance from the field object.
        @param bUseMaxDistance Whether or not to use a maximum distance from
            the field object after which the force has no effect.
        @param fMaxDistance The maximum distance from the field object at
            which the force has any effect. This distance is only used if
            bUseMaxDistance is true.
    */
    NiPSFieldForce(
        const NiFixedString& kName,
        NiPSForceDefinitions::ForceType eType,
        NiPSForceDefinitions::FieldData* pkInputData,
        NiAVObject* pkFieldObj,
        float fMagnitude,
        float fAttenuation,
        bool bUseMaxDistance,
        float fMaxDistance);

    /// Protected default constructor for cloning and streaming only.
    NiPSFieldForce();
    //@}

    /**
        A helper function that uses the world transforms of the field object
        and the particle system to compute the field-to-particle system
        transformation.

        @param kPSys The world transformation of the particle system.
        @param kFieldToPSys Returns the computed transformation from the space
            of the field object to the space of the particle system.
    */
    inline void ComputeFieldToPSys(
        const NiTransform& kPSys,
        NiTransform& kFieldToPSys);

    /**
        Copies the data stored in the FieldData structure.

        This helper function should be called by derived classes during
        cloning to copy the FieldData structure.
    */
    void CopyFieldData(NiPSFieldForce* pkDest);

    /**
        Loads the data stored in the FieldData structure from a stream.

        This helper function should be called by derived classes to serialize
        the FieldData structure.
    */
    void LoadFieldData(NiStream& kStream);

    /**
        Saves the data stored in the FieldData structure to a stream.

        This helper function should be called by derived classes to serialize
        the FieldData structure.
    */
    void SaveFieldData(NiStream& kStream);

    /// A pointer to the data for the field force.
    NiPSForceDefinitions::FieldData* m_pkInputData;

    /// A pointer to the field object in the scene graph.
    NiAVObject* m_pkFieldObj;
};

NiSmartPointer(NiPSFieldForce);

#include "NiPSFieldForce.inl"

#endif  // #ifndef NIPSFIELDFORCE_H
