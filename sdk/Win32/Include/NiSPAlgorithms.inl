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
template <class C, class T>
void NiSPAlgorithms::QuickSortObjects(C* pkCollection, NiInt32 iLeft, 
    NiInt32 iRight)
{
    if (iRight <= iLeft) 
        return;
    NiInt32 iPartition = PartitionObjects<C, T>(pkCollection, iLeft, iRight);
    QuickSortObjects<C, T>(pkCollection, iLeft, iPartition - 1);
    QuickSortObjects<C, T>(pkCollection, iPartition + 1, iRight);
}
//---------------------------------------------------------------------------
template <class C, class T>
void NiSPAlgorithms::QuickSortPointers(C* pkCollection, NiInt32 iLeft, 
    NiInt32 iRight)
{
    if (iRight <= iLeft) 
        return;
    NiInt32 iPartition = PartitionPointers<C, T>(pkCollection, iLeft, iRight);
    QuickSortPointers<C, T>(pkCollection, iLeft, iPartition - 1);
    QuickSortPointers<C, T>(pkCollection, iPartition + 1, iRight);
}
//---------------------------------------------------------------------------
template <class C, class T>
NiInt32 NiSPAlgorithms::PartitionObjects(C* pkCollection, NiInt32 iLeft, 
    NiInt32 iRight)
{
    NiInt32 i = iLeft - 1;
    NiInt32 j = iRight;
    const T& kElement = pkCollection->GetAt(iRight);

    for (;;)
    {
        while (pkCollection->GetAt(++i) < kElement) { }
        while (kElement < pkCollection->GetAt(--j))
            if (j == iLeft)
                break;
        if (i >= j) 
            break;
        SwapObjects<C, T>(pkCollection, i, j);
    }
    SwapObjects<C, T>(pkCollection, i, iRight);
    
    return i;
}
//---------------------------------------------------------------------------
template <class C, class T>
NiInt32 NiSPAlgorithms::PartitionPointers(C* pkCollection, NiInt32 iLeft, 
                                         NiInt32 iRight)
{
    NiInt32 i = iLeft - 1;
    NiInt32 j = iRight;
    T pkElement = pkCollection->GetAt(iRight);

    for (;;)
    {
        while (*pkCollection->GetAt(++i) < *pkElement) { }
        while (*pkElement < *pkCollection->GetAt(--j))
            if (j == iLeft)
                break;
        if (i >= j) 
            break;
        SwapPointers<C, T>(pkCollection, i, j);
    }
    SwapPointers<C, T>(pkCollection, i, iRight);

    return i;
}
//---------------------------------------------------------------------------
template <class C, class T>
void NiSPAlgorithms::SwapObjects(C* pkCollection, NiInt32 iAtIndex1, 
    NiInt32 iAtIndex2)
{
    const T& kTmp = pkCollection->GetAt(iAtIndex1);
    pkCollection->ReplaceAt(iAtIndex1, pkCollection->GetAt(iAtIndex2));
    pkCollection->ReplaceAt(iAtIndex2, kTmp);
}
//---------------------------------------------------------------------------
template <class C, class T>
void NiSPAlgorithms::SwapPointers(C* pkCollection, NiInt32 iAtIndex1, 
    NiInt32 iAtIndex2)
{
    T pkTmp = pkCollection->GetAt(iAtIndex1);
    pkCollection->ReplaceAt(iAtIndex1, pkCollection->GetAt(iAtIndex2));
    pkCollection->ReplaceAt(iAtIndex2, pkTmp);
}
//---------------------------------------------------------------------------
inline NiUInt64 NiSPAlgorithms::Align128(NiUInt64 uiSize)
{
    return (uiSize + 127) & ~127;
}
//---------------------------------------------------------------------------
inline void* NiSPAlgorithms::PreAlign128(void* uiSize)
{
    return (void*)(((NiUInt64)uiSize) & ~127);
}

//---------------------------------------------------------------------------
