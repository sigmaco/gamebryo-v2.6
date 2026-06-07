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
//
// THIS CLASS HAS BEEN DEPRECATED.
// Functionality here exists only to enable streaming and conversion to the
// NiMorphMeshModifier class.
//
//---------------------------------------------------------------------------

#ifndef NIMORPHDATA_H
#define NIMORPHDATA_H

#include <NiObject.h>
#include <NiPoint3.h>
#include "NiAnimationLibType.h"
#include "NiInterpolator.h"

NiSmartPointer(NiMorphData);

class NIANIMATION_ENTRY NiMorphData : public NiObject
{
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    // *** begin Emergent internal use only ***
    virtual ~NiMorphData ();

    class NIANIMATION_ENTRY MorphTarget : public NiMemObject
    {
    public:
        MorphTarget();
        ~MorphTarget();

        inline NiPoint3* GetTargetVerts () const;

        void ReplaceTargetVerts(NiPoint3* aTargetVerts);

        void SetName(const NiFixedString& kName);
        const NiFixedString& GetName();

        void LoadBinary(NiStream& kStream, unsigned int uiNumVerts);
        void SaveBinary(NiStream& kStream, unsigned int uiNumVerts);
        void RegisterStreamables(NiStream& kStream);

        // *** begin Emergent internal use only ***
        NiInterpolator* GetLegacyInterpolator();
        void SetLegacyInterpolator(NiInterpolator* pkInterp);
        inline float GetLegacyWeight() const;
        bool IsEqual(MorphTarget* pkOther, unsigned int uiNumVerts);
        // *** end Emergent internal use only ***
    protected:
        NiPoint3* m_aTargetVerts;
        NiFixedString m_kName;
        float m_fLegacyWeight;
        NiInterpolatorPtr m_spLegacyInterpolator;
    };

    // target access
    inline unsigned int GetNumTargets () const;
    inline unsigned int GetNumVertsPerTarget () const;
    MorphTarget* GetTarget (unsigned int uiIndex) const;

    inline NiPoint3* GetTargetVerts (unsigned int uiIndex) const;

    inline bool GetRelativeTargets () const;
    // *** end Emergent internal use only ***

protected:
    NiMorphData ();

    unsigned int m_uiNumTargets;
    unsigned int m_uiNumVertsPerTarget;
    MorphTarget* m_aMorphTargets;

    bool m_bRelativeTargets;
};

#include "NiMorphData.inl"

#endif
