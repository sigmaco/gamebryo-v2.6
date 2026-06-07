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
inline NiMeshVertexOperators* NiMeshVertexOperators::GetInstance()
{
    NIASSERT(ms_pkInstance);
    return ms_pkInstance;
}
//---------------------------------------------------------------------------
template <typename T>
inline void NiMeshVertexOperators::RegisterEqualsOp(
    NiDataStreamElement::Type uiDataType)
{
    EQUALS_FUNCTION pEqualsFunc = &DataTypeEquals<T>;
    m_kEqualsOpMap.SetAt(uiDataType, pEqualsFunc);
}
//---------------------------------------------------------------------------
inline NiMeshVertexOperators::EQUALS_FUNCTION 
NiMeshVertexOperators::GetEqualsOp(NiDataStreamElement::Type uiDataType)
{
    EQUALS_FUNCTION pFunc = NULL;
    m_kEqualsOpMap.GetAt(uiDataType, pFunc);
    NIASSERT(pFunc != NULL);
    return pFunc;
}
//---------------------------------------------------------------------------
template <typename T> 
inline bool DataTypeEquals(void* pvOne, void* pvTwo, NiUInt8 uiCount)
{
    T* pFirstValues = (T*)pvOne;
    T* pSecondValues = (T*)pvTwo;

    for (NiUInt8 ui = 0; ui < uiCount; ui++, pFirstValues++, pSecondValues++)
    {     
        if (*pFirstValues != *pSecondValues)
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
