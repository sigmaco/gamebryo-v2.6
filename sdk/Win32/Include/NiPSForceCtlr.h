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

#ifndef NIPSFORCECTLR_H
#define NIPSFORCECTLR_H

#include "NiParticleLibType.h"
#include <NiSingleInterpController.h>

class NiPSForce;

/**
    Abstract base class for all particle force time controllers.

    Contains functions for resolving a force name to an NiPSForce pointer.
*/
class NIPARTICLE_ENTRY NiPSForceCtlr : public NiSingleInterpController
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareAbstractClone(NiPSForceCtlr);
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// Overridden from the base class to resolve the force name into an
    /// NiPSForce pointer.
    virtual void SetTarget(NiObjectNET* pkTarget);

    /// Returns the force name (used to find the force pointer).
    inline const NiFixedString& GetForceName() const;

    /**
        Sets the force name.

        @note If the force pointer has already been resolved from the name,
            changing the name with this function will have no effect. This
            function should only be called on a controller before its target
            has been set.
    */
    inline void SetForceName(const NiFixedString& kForceName);

    /**
        Returns a pointer to the NiPSForce object that this controller
        animates.
        
        If the controller has no target, NULL is returned.
    */
    inline NiPSForce* GetForcePointer() const;

    /// @cond EMERGENT_INTERNAL

    /// @name Base Class Overrides
    //@{
    virtual const char* GetCtlrID();
    virtual void ProcessClone(NiCloningProcess& kCloning);
    virtual void PostLinkObject(NiStream& kStream);
    //@}

    /// @endcond

protected:
    /// @name Construction and Destruction
    //@{
    /**
        Main constructor.

        @param kForceName The name of the NiPSForce object that should be
            animated.
    */
    NiPSForceCtlr(const NiFixedString& kForceName);

    /// Protected default constructor for cloning and streaming only.
    NiPSForceCtlr();
    //@}

    /// @name Base Class Overrides
    //@{
    virtual bool InterpTargetIsCorrectType(NiObjectNET* pkTarget) const;
    //@}

    /// The name of the NiPSForce object.
    NiFixedString m_kForceName;

    /// A pointer to the NiPSForce object to animate (resolved from the
    /// force name).
    NiPSForce* m_pkForce;

private:
    /// Sets m_pkForce to point to the correct force, using the name stored in
    /// m_kForceName.
    void GetForcePointerFromName();
};

NiSmartPointer(NiPSForceCtlr);

#include "NiPSForceCtlr.inl"

#endif  // #ifndef NIPSFORCECTLR_H
