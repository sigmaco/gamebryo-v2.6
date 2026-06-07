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
//  NiTNodeTraversal inline functions

//---------------------------------------------------------------------------
template <class Functor>
void NiTNodeTraversal::DepthFirst_AllObjects(
    NiAVObject* pkAVObj,
    Functor &kFunctor)
{
    if (!pkAVObj)
        return;

    if (pkAVObj)
        kFunctor(pkAVObj);

    if (pkAVObj->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkAVObj;

        unsigned int uiChildCount = pkNode->GetArrayCount();
        for (unsigned int uiChild = 0; uiChild < uiChildCount; uiChild++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiChild);
            DepthFirst_AllObjects(pkChild, kFunctor);
        }
    }
}
//---------------------------------------------------------------------------
template <class Functor>
bool NiTNodeTraversal::DepthFirst_FirstStop(
    NiAVObject* pkAVObj,
    Functor &kFunctor)
{
    if (!pkAVObj)
        return true;

    if (!kFunctor(pkAVObj))
        return false;

    if (pkAVObj->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkAVObj;

        unsigned int uiChildCount = pkNode->GetArrayCount();
        for (unsigned int uiChild = 0; uiChild < uiChildCount; uiChild++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiChild);
            if (!DepthFirst_FirstStop(pkChild, kFunctor))
                return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
template <class Functor>
void NiTNodeTraversal::DepthFirst_PreRecurse(
    NiAVObject* pkAVObj,
    Functor &kFunctor)
{
    if (!pkAVObj)
        return;

    if (!kFunctor(pkAVObj))
        return;

    if (pkAVObj->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkAVObj;

        unsigned int uiChildCount = pkNode->GetArrayCount();
        for (unsigned int uiChild = 0; uiChild < uiChildCount; uiChild++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiChild);
            DepthFirst_PreRecurse(pkChild, kFunctor);
        }
    }
}
//---------------------------------------------------------------------------
