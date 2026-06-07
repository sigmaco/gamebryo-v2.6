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

#ifndef NICOMMONGRAPHCALLBACKOBJECTS_H
#define NICOMMONGRAPHCALLBACKOBJECTS_H

#include <NiVisualTrackerLibType.h>
#include <NiVisualTracker.h>
#include <NiMemTracker.h>
#include <NiVisibleArray.h>

//---------------------------------------------------------------------------
class NIVISUALTRACKER_ENTRY MemHighWaterMarkUpdate : 
    public NiVisualTracker::GraphCallbackObject
{
public:
    MemHighWaterMarkUpdate(float fScalar, NiMemTracker* pkTracker);
    virtual float TakeSample(float fTime);

    float m_fScalar;
    NiMemTracker* m_pkTracker;
};
//---------------------------------------------------------------------------
class NIVISUALTRACKER_ENTRY MemCurrentUpdate : 
    public NiVisualTracker::GraphCallbackObject
{
public:

    MemCurrentUpdate(float fScalar, NiMemTracker* pkTracker);
    virtual float TakeSample(float fTime);

    float m_fScalar;
    NiMemTracker* m_pkTracker;
};
//---------------------------------------------------------------------------
class NIVISUALTRACKER_ENTRY MemCurrentAllocCountUpdate : 
    public NiVisualTracker::GraphCallbackObject
{
public:
    MemCurrentAllocCountUpdate(float fScalar, NiMemTracker* pkTracker);
    virtual float TakeSample(float fTime);

    float m_fScalar;
    NiMemTracker* m_pkTracker;
};
//---------------------------------------------------------------------------
class NIVISUALTRACKER_ENTRY FrameRateUpdate : 
    public NiVisualTracker::GraphCallbackObject
{
public:
    FrameRateUpdate(float *pfFrameRate);

    virtual float TakeSample(float fTime);
    float *m_pfFrameRate;
};
//---------------------------------------------------------------------------
class NIVISUALTRACKER_ENTRY VisibleArrayUpdate : 
    public NiVisualTracker::GraphCallbackObject
{
public:
    VisibleArrayUpdate(NiVisibleArray& kVisArray);

    virtual float TakeSample(float fTime);
    NiVisibleArray& m_kVisArray;
private:
    VisibleArrayUpdate & operator=( const VisibleArrayUpdate & );
};
//---------------------------------------------------------------------------
class NIVISUALTRACKER_ENTRY GenericFloatUpdate : 
    public NiVisualTracker::GraphCallbackObject
{
public:

    GenericFloatUpdate(float fScalar, float* pfValue);
    virtual float TakeSample(float fTime);

    float m_fScalar;
    float* m_pfValue;
};
//---------------------------------------------------------------------------
class NIVISUALTRACKER_ENTRY GenericUnsignedIntUpdate : 
    public NiVisualTracker::GraphCallbackObject
{
public:

    GenericUnsignedIntUpdate(float fScalar, unsigned int* puiValue);
    virtual float TakeSample(float fTime);

    float m_fScalar;
    unsigned int* m_puiValue;
};
//---------------------------------------------------------------------------
#endif
