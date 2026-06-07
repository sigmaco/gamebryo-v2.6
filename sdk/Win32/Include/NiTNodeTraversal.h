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

#ifndef NITNODETRAVERSAL_H
#define NITNODETRAVERSAL_H

#include "NiNode.h"

class NiTNodeTraversal
{
public:
    // For all traversal types:
    // The pointer passed to the functor is never NULL.

    // DepthFirst_AllObjects traverses an NiNode scene graph rooted at pkAVObj
    // The traversal is depth first, with storage on the stack.
    // kFunctor is called on every object.
    template <class Functor> 
        static void 
        DepthFirst_AllObjects(NiAVObject* pkAVObj, Functor &kFunctor);

    // DepthFirst_FirstStop traverses an NiNode scene graph rooted at pkAVObj
    // The traversal is depth first, with storage on the stack.
    // kFunctor is called prior to recursing on the children.
    // If kFunctor returns false, traversal stops.
    template <class Functor> 
        static bool
        DepthFirst_FirstStop(NiAVObject* pkAVObj, Functor &kFunctor);

    // DepthFirst_PreRecurse traverses an NiNode scene graph rooted at pkAVObj
    // The traversal is depth first, with storage on the stack.
    // kFunctor is called prior to recursing on the children.
    // If kFunctor returns false, children not recursed, traversal continues.
    template <class Functor> 
        static void 
        DepthFirst_PreRecurse(NiAVObject* pkAVObj, Functor &kFunctor);
};

#include "NiTNodeTraversal.inl"

#endif // NITNODETRAVERSAL_H
