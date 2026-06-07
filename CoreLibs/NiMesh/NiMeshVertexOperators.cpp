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

// Pre-compiled header
#include "NiMeshPCH.h"

#include "NiMeshVertexOperators.h"

NiMeshVertexOperators* NiMeshVertexOperators::ms_pkInstance(NULL);

//---------------------------------------------------------------------------
NiMeshVertexOperators::NiMeshVertexOperators()
{
    RegisterEqualsOp<float>(NiDataStreamElement::T_FLOAT32);
    RegisterEqualsOp<NiInt32>(NiDataStreamElement::T_INT32);
    RegisterEqualsOp<NiInt16>(NiDataStreamElement::T_INT16);
    RegisterEqualsOp<NiInt8>(NiDataStreamElement::T_INT8);
    RegisterEqualsOp<NiUInt32>(NiDataStreamElement::T_UINT32);
    RegisterEqualsOp<NiUInt16>(NiDataStreamElement::T_UINT16);
    RegisterEqualsOp<NiUInt8>(NiDataStreamElement::T_UINT8);
}
//---------------------------------------------------------------------------
NiMeshVertexOperators::~NiMeshVertexOperators()
{

}
//---------------------------------------------------------------------------
void NiMeshVertexOperators::_SDMInit()
{
    NIASSERT(NiMeshVertexOperators::ms_pkInstance == NULL);
    NiMeshVertexOperators::ms_pkInstance = NiNew NiMeshVertexOperators();
}
//---------------------------------------------------------------------------
void NiMeshVertexOperators::_SDMShutdown()
{
    NiDelete NiMeshVertexOperators::ms_pkInstance;
    NiMeshVertexOperators::ms_pkInstance = NULL;
}
//---------------------------------------------------------------------------
