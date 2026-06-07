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

#ifndef NIPSEMITTERCTLR_H
#define NIPSEMITTERCTLR_H

#include "NiParticleLibType.h"
#include <NiSingleInterpController.h>
#include "NiPSParticleSystem.h"

/**
    Abstract base class for all particle emitter time controllers.

    Contains functions for resolving an emitter name to an NiPSEmitter pointer.
*/
class NIPARTICLE_ENTRY NiPSEmitterCtlr : public NiSingleInterpController
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareAbstractClone(NiPSEmitterCtlr);
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// Overridden from the base class to resolve the emitter name into an
    /// NiPSEmitter pointer.
    virtual void SetTarget(NiObjectNET* pkTarget);

    /// Returns the emitter name (used to find the emitter pointer).
    inline const NiFixedString& GetEmitterName() const;

    /**
        Sets the emitter name.

        @note If the emitter pointer has already been resolved from the name,
            changing the name with this function will have no effect. This
            function should only be called on a controller before its target
            has been set.
    */
    inline void SetEmitterName(const NiFixedString& kEmitterName);

    /**
        Returns a pointer to the NiPSEmitter object that this controller
        animates.
        
        If the controller has no target, NULL is returned.
    */
    inline NiPSEmitter* GetEmitterPointer() const;

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

        @param kEmitterName The name of the NiPSEmitter object that should be
            animated.
    */
    NiPSEmitterCtlr(const NiFixedString& kEmitterName);

    /// Protected default constructor for cloning and streaming only.
    NiPSEmitterCtlr();
    //@}

    /// @name Base Class Overrides
    //@{
    virtual bool InterpTargetIsCorrectType(NiObjectNET* pkTarget) const;
    //@}

    /// The name of the NiPSEmitter object.
    NiFixedString m_kEmitterName;

    /// A pointer to the NiPSEmitter object to animate (resolved from the
    /// emitter name).
    NiPSEmitter* m_pkEmitter;

private:
    /// Sets m_pkEmitter to point to the correct emitter, using the name
    /// stored in m_kEmitterName.
    inline void GetEmitterPointerFromName();
};

NiSmartPointer(NiPSEmitterCtlr);

#include "NiPSEmitterCtlr.inl"

#endif  // #ifndef NIPSEMITTERCTLR_H
