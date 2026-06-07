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

#ifndef NIMESHVERTEXOPERATORS_H
#define NIMESHVERTEXOPERATORS_H

#include <NiMemObject.h>
#include <NiTMap.h>
#include "NiDataStreamElement.h"

/// Templated function to compare two vertices
template <typename T> 
bool DataTypeEquals(void* pvOne, void* pvTwo, NiUInt8 uiCount);

/**
    This class contains helper functions for comparing one vertex to another
    when the data type is known in advance.
*/
class NIMESH_ENTRY NiMeshVertexOperators : public NiMemObject
{
public:

    /// Typedef for a comparison function between two vertices
    typedef bool (*EQUALS_FUNCTION)(void* pvValue1, void* pvValue2, 
        NiUInt8 uiCount);

    /// Accessor for the singleton
    static NiMeshVertexOperators* GetInstance();

    /**
        Register a default equals function for type T to the corresponding
        NiDataStreamElement type.
    
        The default equals function compares the components of a type
        for exact equality. This is a templated function.
    */
    template <typename T>
    inline void RegisterEqualsOp(NiDataStreamElement::Type uiDataType);

    /// Retrieve the equals operator associated with the 
    /// NiDataStreamElement type.
    EQUALS_FUNCTION GetEqualsOp(NiDataStreamElement::Type uiDataType);

    /// @cond EMERGENT_INTERNAL

    /// Creates the singleton and registers default vertex operators.
    static void _SDMInit();

    /// Destroys the singleton
    static void _SDMShutdown();

    /// @endcond

protected:

    /// Protected constructor
    NiMeshVertexOperators();

    /// Protected destructor
    ~NiMeshVertexOperators();

    /// Typedef for the equals operator map
    typedef NiTMap<NiUInt32, EQUALS_FUNCTION> EqualsOperatorMap;

    /// The internal hash table of id's to EQUALS_FUNCTION's
    EqualsOperatorMap m_kEqualsOpMap;
    
    /// Singleton instance of the class.
    static NiMeshVertexOperators* ms_pkInstance;
};

#include "NiMeshVertexOperators.inl"

#endif
