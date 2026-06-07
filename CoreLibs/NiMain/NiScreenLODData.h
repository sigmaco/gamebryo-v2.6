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

#ifndef NISCREENLODDATA_H
#define NISCREENLODDATA_H

#include "NiLODData.h"
#include "NiBound.h"
class NiCamera;
class NiLODNode;

// NiScreenLODData Notes::
// 
//  There are a few assumptions made by NiScreenLODData that optimize it
// for size and speed.
//
//  Inherent Ordering.
//
//      It is assumed that the LOD node has it's children ordered from 
// Highest level of detail(child 0) to the lowest level of detail (Child N).
// It also assumes that only 1 child will ever be visible at any given time
// and one child will always be visible (aka. NiSwitchNode) Given these
// assumptions the NiScreenLODData needs to only save the far boundary for
// each LOD level. A particular LOD level will be drawn until it's screen
// space boundary is small then a "Far" proportion.
//
// 
//  Lowest Detail LOD has No Proportion
//
//      The lowest level of detail is assumed to have a far proportion of
// zero because it should always be show. For this reason lowest level of
// detail has no proportion. The number of proportions will be one less
// than the number of LOD levels. (m_uiNumProportions = (Num LOD -1)).
// You are only storing the bound conditions between LOD levels and the
// lowest LOD has no far boundary.
//
class NIMAIN_ENTRY NiScreenLODData : public NiLODData
{
    NiDeclareRTTI;
    NiDeclareClone(NiScreenLODData);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiScreenLODData();
    virtual ~NiScreenLODData();

    // Determine the LOD Level do be used based on this camera and Node.
    // NiScreenLODData will use the m_kWorldBound's screen size.
    virtual int GetLODLevel(const NiCamera* pkCamera, NiLODNode* pkLOD) const;

    // Update any internal world data base on this LOD node.  NiScreenLODData
    // will be updating the m_kWorldBound.
    virtual void UpdateWorldData(NiLODNode* pkLOD);

    // NiLODData are never shared so we need a duplication function for
    // cloning.
    virtual NiLODData* Duplicate();

    // Bounds used to determine the screen proportion
    inline void SetBound(const NiBound& kBound);
    inline const NiBound& GetBound() const;
    inline const NiBound& GetWorldBound() const;

    inline unsigned int GetNumProportions() const;
    void SetNumProportions(unsigned int uiNumProportions);

    inline float GetProportion(unsigned int uiIndex) const;
    inline void SetProportion(unsigned int uiIndex, float fProportion);

    // Merge the updated bounds for all children to determine the 
    // NiScreenLODData's bound
    void AutoSetBound(NiLODNode* pkLOD);

    virtual int GetLODIndex(int iLODLevel) const;


protected:
    NiBound m_kBound;
    NiBound m_kWorldBound;

    unsigned int m_uiNumProportions;
    float* m_pfProportions;
};


typedef NiPointer<NiScreenLODData> NiScreenLODDataPtr;

//---------------------------------------------------------------------------
//  Inline include
#include "NiScreenLODData.inl"

//---------------------------------------------------------------------------

#endif  // NISCREENLODDATA_H

