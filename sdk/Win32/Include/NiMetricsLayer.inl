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
inline void NiMetricsLayer::AddOutputModule(NiMetricsOutputModule* pkMod)
{
    ms_kMutex.Lock();

    ms_kOutputs.Add(pkMod);

    ms_kMutex.Unlock();
}
//---------------------------------------------------------------------------
inline void NiMetricsLayer::RemoveOutputModule(NiMetricsOutputModule* pkMod)
{
    ms_kMutex.Lock();

    for (unsigned int i = 0; i < ms_kOutputs.GetSize(); i++)
    {
        if (ms_kOutputs.GetAt(i) == pkMod)
        {
            ms_kOutputs.RemoveAtAndFill(i);

            ms_kMutex.Unlock();
            return;
        }
    }

    ms_kMutex.Unlock();

    NIASSERT(!"Attempting to remove output module not in list.");
}
//---------------------------------------------------------------------------
inline void NiMetricsLayer::AddValue(const char* pcName, float fValue)
{
    ms_kMutex.Lock();

    NIASSERT(pcName);
    for (unsigned int i = 0; i < ms_kOutputs.GetSize(); i++)
    {
        ms_kOutputs.GetAt(i)->AddValue(pcName, fValue);
    }

    ms_kMutex.Unlock();
}
//---------------------------------------------------------------------------
inline void NiMetricsLayer::StartTimer(NiMetricsTimer& kTimer)
{
    ms_kMutex.Lock();

    for (unsigned int i = 0; i < ms_kOutputs.GetSize(); i++)
    {
        ms_kOutputs.GetAt(i)->StartTimer(kTimer);
    }

    ms_kMutex.Unlock();
}
//---------------------------------------------------------------------------
inline void NiMetricsLayer::EndTimer(NiMetricsTimer& kTimer)
{
    ms_kMutex.Lock();

    for (unsigned int i = 0; i < ms_kOutputs.GetSize(); i++)
    {
        ms_kOutputs.GetAt(i)->EndTimer(kTimer);
    }

    ms_kMutex.Unlock();
}
//---------------------------------------------------------------------------
inline void NiMetricsLayer::AddImmediateEvent(const char* pcName, 
    float fValue)
{
    ms_kMutex.Lock();

    NIASSERT(pcName);
    for (unsigned int i = 0; i < ms_kOutputs.GetSize(); i++)
    {
        ms_kOutputs.GetAt(i)->AddImmediateEvent(pcName, fValue);
    }

    ms_kMutex.Unlock();
}
//---------------------------------------------------------------------------
inline void NiMetricsLayer::Update()
{
    ms_kMutex.Lock();

    for (unsigned int i = 0; i < ms_kOutputs.GetSize(); i++)
    {
        ms_kOutputs.GetAt(i)->Update();
    }

    ms_kMutex.Unlock();
}
//---------------------------------------------------------------------------

