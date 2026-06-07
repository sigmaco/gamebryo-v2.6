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
#ifndef NIIMMEDIATEMODEMACRO_H
#define NIIMMEDIATEMODEMACRO_H

#include "NiImmediateModeAdapter.h"

class NiPointLight;
class NiSpotLight;
class NiDirectionalLight;
class NiCamera;
class NiMesh;

/**
    Contains methods for submitting pre-defined, immediate mode
    geometry to an NiImmediateModeAdapter.
*/
class NIMESH_ENTRY NiImmediateModeMacro
{
public:

    /// Constructor.
    /// @param kAdapter The NiImmediateModeAdapter to use.
    NiImmediateModeMacro(NiImmediateModeAdapter& kAdapter);

    /// Virtual destructor.
    virtual ~NiImmediateModeMacro();

    /// @name Sphere generation
    //@{
    /// Creates a solid sphere
    void SolidSphere(
        const NiPoint3& kCenter, 
        float fRadius, 
        unsigned int uiNumSlices, 
        unsigned int uiNumStacks);
    
    /// Creates a wireframe sphere
    void WireSphere(
        const NiPoint3& kCenter, 
        float fRadius, 
        unsigned int uiNumSlices, 
        unsigned int uiNumStacks);
    //@}
    
    /// @name Cone/box generation
    /// @note The cone length is along the x-axis. The vector from the tip of 
    /// the cone to the base is parallel to the positive x-axis. The tip 
    /// of the cone starts at the local-space origin.
    //@{
    
    /// Creates a solid cone
    void SolidCone(
        const NiTransform& kLocalTrans, 
        float fLength, 
        float fRadius, 
        unsigned int uiNumSlices);
    
    /// Creates a wireframe cone
    void WireCone(
        const NiTransform& kLocalTrans, 
        float fLength, 
        float fRadius, 
        unsigned int uiNumSlices);
    
    /// Creates a solid box
    void SolidBox(const NiTransform& kLocalTrans, 
        float fX, 
        float fY, 
        float fZ);

    /// Creates a wireframe box
    void WireBox(const NiTransform& kLocalTrans, 
        float fX, 
        float fY, 
        float fZ);
    //@}

    /// @name Camera, light, and rotational frame generation
    //@{
    // The camera, light, and rotational frame objects below all have a
    // default scale of 1.0 in world space.  You can scale them to fit your
    // scene more appropriately, or use the GetScreenScaleFactor function
    // to get a scale that will fit them to a particular screen size.

    /// Creates a wireframe camera.
    void WireCamera(NiCamera* pkCamera, float fScaleMult);
    
    /// Creates a wireframe frustum for a given camera.
    void WireFrustum(NiCamera* pkCamera);
    
    /// Creates a wireframe frustum for a frustum with a specified transform.
    void WireFrustum(const NiFrustum& kFrust, const NiTransform& kTrans);

    /// Creates a point light.
    void WirePointLight(NiPointLight* pkLight, float fScaleMult);
    
    /// Creates a spot light.
    void WireSpotLight(NiSpotLight* pkLight, float fScaleMult);
    
    /// Creates a directional light.
    void WireDirectionalLight(NiDirectionalLight* pkLight, float fScaleMult);
    
    /// Creates the three axes comprising a rotational frame at the
    /// world-space position of the transformation.  The scale of kTrans
    /// is ignored and fScaleMult is used instead.
    void RotationalFrame(NiTransform& kTrans, float fScaleMult);

    /// Creates a bone hierarchy rendering if the mesh has a skin modifier
    /// attached.
    void WireBoneHierarchy(NiMesh* pkMesh);
    //@}

    /// Utility function for calculating a view-dependent scale multiplier.
    /// @return Returns the scale factor needed to scale the NiAVObject to
    /// fit to the screen size of the NiImmediateModeAdapter object's camera.
    inline float GetScreenScaleFactor(const NiAVObject* pkObj) const;

    /// Static utility function for calculating a view-dependent scale
    /// multiplier.
    /// @return Returns the scale factor needed to scale the NiAVObject to
    /// fit to the screen size of the NiImmediateModeAdapter object's camera.
    static float GetScreenScaleFactor(const NiAVObject* pkObj,
        const NiImmediateModeAdapter* pkAdapter);

protected:
    inline void ForceAdapterIntoMode(NiPrimitiveType::Type eType);

    inline void CreateConeVerts(
        NiPoint3* pkPoints, 
        const NiTransform& kLocalTrans, 
        float fLength, 
        float fRadius, 
        unsigned int uiNumSlices);

    void CreateSphereVerts(
        NiPoint3* pkPoints, 
        const NiPoint3& kCenter, 
        float fRadius, 
        unsigned int uiNumSlices, 
        unsigned int uiNumStacks);

    void CreateBone(const NiTransform& kParent, const NiTransform& kChild,
        const NiColorA& kColor);

    NiImmediateModeAdapter* m_pkAdapter;
};

#include "NiImmediateModeMacro.inl"

#endif // #ifdef NIIMMEDIATEMODEMACRO_H

