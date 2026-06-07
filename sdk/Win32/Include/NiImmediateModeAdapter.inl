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
inline bool NiImmediateModeAdapter::IsValidAppend(
    unsigned int, unsigned int uiIndices)
{
    bool bVertexCountMatch;
    switch (GetPrimitiveType())
    {
    case NiPrimitiveType::PRIMITIVE_LINES:
        bVertexCountMatch = (0 == uiIndices % 2);
        break;

    case NiPrimitiveType::PRIMITIVE_LINESTRIPS:
    case NiPrimitiveType::PRIMITIVE_POINTS:
        bVertexCountMatch = true;
        break;

    case NiPrimitiveType::PRIMITIVE_QUADS:
        bVertexCountMatch = (0 == uiIndices % 4);
        break;

    case NiPrimitiveType::PRIMITIVE_TRIANGLES:
        bVertexCountMatch = (0 == uiIndices % 3);
        break;

    case NiPrimitiveType::PRIMITIVE_TRISTRIPS:
    default:
        // Tristrips and unknown types not supported
        bVertexCountMatch = false;
        break;
    }

    // Appends are only valid if the vertex count matches and we are
    // already in the middle of a draw state.
    bool bSuccess = (bVertexCountMatch && m_bInDrawState);

    if (!bSuccess)
        m_eLastError = NIIMERROR_ILLEGAL_APPEND;

    return bSuccess;
}
//---------------------------------------------------------------------------
inline bool NiImmediateModeAdapter::IsValidStateChange()
{
    if (m_bInDrawState)
    {
        m_eLastError = NIIMERROR_ILLEGAL_STATE_CHANGE;
    }

    // State changes are only valid outside of a draw state.
    return (!m_bInDrawState);
}
//---------------------------------------------------------------------------
inline bool NiImmediateModeAdapter::IsDrawing() const
{
    return m_bInDrawState;
}
//---------------------------------------------------------------------------
inline bool NiImmediateModeAdapter::IsIndexed() const
{
    return m_bIsIndexed;
}
//---------------------------------------------------------------------------
inline void NiImmediateModeAdapter::SubmitIfCantFit(unsigned int uiNumVerts,
    unsigned int uiNumIndices)
{
    NIASSERT(m_bInDrawState);

    if (m_uiVerts + uiNumVerts > GetVertexBufferSize() ||
        m_bIsIndexed && m_uiIndices + uiNumIndices > GetIndexBufferSize())
    {
        EndDrawing();

        // resize if too small!
        if (uiNumVerts > GetVertexBufferSize())
            SetNumMaxVertices(uiNumVerts);
        if (m_bIsIndexed && uiNumIndices > GetIndexBufferSize())
            SetNumMaxIndices(uiNumIndices);

        BeginDrawingInternal();
    }
}
//---------------------------------------------------------------------------
inline NiPrimitiveType::Type NiImmediateModeAdapter::GetPrimitiveType() const
{
    return m_spMesh->GetPrimitiveType();
}
//---------------------------------------------------------------------------
inline void NiImmediateModeAdapter::CheckAlphaBlending(
    const NiColorA& kColor)
{
    m_bIsOpaque &= (kColor.a >= 1.0f);
}
//---------------------------------------------------------------------------
inline NiImmediateModeAdapter::NiIMError
    NiImmediateModeAdapter::GetLastError() const
{
    return m_eLastError;
}
//---------------------------------------------------------------------------
inline void NiImmediateModeAdapter::ClearLastError()
{
    m_eLastError = NIIMERROR_OK;
}
//---------------------------------------------------------------------------
inline bool NiImmediateModeAdapter::GetZBufferCompare() const
{
    NiZBufferProperty* pkZ = 
        (NiZBufferProperty*)m_spMesh->GetProperty(NiProperty::ZBUFFER);
    NIASSERT(pkZ);

    return pkZ->GetZBufferTest();
}
//---------------------------------------------------------------------------
inline bool NiImmediateModeAdapter::GetZBufferWrite() const
{
    NiZBufferProperty* pkZ = 
        (NiZBufferProperty*)m_spMesh->GetProperty(NiProperty::ZBUFFER);
    NIASSERT(pkZ);

    return pkZ->GetZBufferWrite();
}
//---------------------------------------------------------------------------
inline const NiTransform& NiImmediateModeAdapter::GetWorldTransform() const
{
    return m_spMesh->GetWorldTransform();
}
//---------------------------------------------------------------------------
inline unsigned int NiImmediateModeAdapter::GetNumVerticesUsed() const
{
    return m_uiVerts + m_uiTotalVerts;
}
//---------------------------------------------------------------------------
inline unsigned int NiImmediateModeAdapter::GetNumIndicesUsed() const
{
    return m_uiIndices + m_uiTotalIndices;
}
//---------------------------------------------------------------------------
inline unsigned int NiImmediateModeAdapter::GetVertexBufferSize() const
{
    return m_spPositionStream != NULL ? m_spPositionStream->GetTotalCount() :
        m_uiInitialVertexBufferSize;
}
//---------------------------------------------------------------------------
inline unsigned int NiImmediateModeAdapter::GetIndexBufferSize() const
{
    return m_spIndexStream != NULL ? m_spIndexStream->GetTotalCount() :
        m_uiInitialIndexBufferSize;
}
//---------------------------------------------------------------------------
inline void NiImmediateModeAdapter::SetCurrentColor(const NiColorA& kColor)
{
    m_kDefaultColor = kColor;
}
//---------------------------------------------------------------------------
inline const NiColorA& NiImmediateModeAdapter::GetCurrentColor() const
{
    return m_kDefaultColor;
}
//---------------------------------------------------------------------------
inline const NiCamera* NiImmediateModeAdapter::GetCurrentCamera() const
{
    return m_spCamera;
}
//---------------------------------------------------------------------------
