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
// Precompiled Header
#include "NiD3D10BinaryShaderLibPCH.h"

#include "NSBD3D10UserDefinedDataSet.h"
#include "NSBD3D10Utility.h"

#include <NiD3D10ShaderConstantMap.h>
#include <NiD3D10ShaderFactory.h>

//---------------------------------------------------------------------------
NSBD3D10UserDefinedDataSet::NSBD3D10UserDefinedDataSet()
{
    m_kBlockIter = 0;
    m_kUserDefinedDataBlocks.RemoveAll();
}
//---------------------------------------------------------------------------
NSBD3D10UserDefinedDataSet::~NSBD3D10UserDefinedDataSet()
{
    NSBD3D10UserDefinedDataBlock* pkBlock = GetFirstBlock();
    while (pkBlock)
    {
        NiDelete pkBlock;
        pkBlock = GetNextBlock();
    }
    m_kUserDefinedDataBlocks.RemoveAll();
}
//---------------------------------------------------------------------------
unsigned int NSBD3D10UserDefinedDataSet::GetBlockCount()
{
    return m_kUserDefinedDataBlocks.GetSize();
}
//---------------------------------------------------------------------------
NSBD3D10UserDefinedDataBlock* NSBD3D10UserDefinedDataSet::GetBlock(
    const char* pcName, bool bCreate)
{
    NiTListIterator kIter;
    NSBD3D10UserDefinedDataBlock* pkBlock;

    kIter = m_kUserDefinedDataBlocks.GetHeadPos();
    while (kIter)
    {
        pkBlock = m_kUserDefinedDataBlocks.GetNext(kIter);
        if (pkBlock && !NiStricmp(pkBlock->GetName(), pcName))
        {
            if (bCreate)
            {
                NiD3D10ShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* WARNING: NSBD3D10UserDefinedDataSet::GetBlock\n"
                    "    Block %s already in set\n"
                    "    Create was true - POTENTIAL OVERWRITE ISSUE!\n", 
                    pcName);
            }
            return pkBlock;
        }
    }

    if (bCreate)
    {
        pkBlock = NiNew NSBD3D10UserDefinedDataBlock(pcName);
        NIASSERT(pkBlock && 
            "NSBD3D10UserDefinedDataSet::GetBlock> Failed allocation!");

        // Add it to the list
        m_kUserDefinedDataBlocks.AddTail(pkBlock);

        return pkBlock;
    }
    return 0;
}
//---------------------------------------------------------------------------
NSBD3D10UserDefinedDataBlock* NSBD3D10UserDefinedDataSet::GetFirstBlock()
{
    m_kBlockIter = m_kUserDefinedDataBlocks.GetHeadPos();
    if (m_kBlockIter)
        return m_kUserDefinedDataBlocks.GetNext(m_kBlockIter);
    return 0;
}
//---------------------------------------------------------------------------
NSBD3D10UserDefinedDataBlock* NSBD3D10UserDefinedDataSet::GetNextBlock()
{
    if (m_kBlockIter)
        return m_kUserDefinedDataBlocks.GetNext(m_kBlockIter);
    return 0;
}
//---------------------------------------------------------------------------
bool NSBD3D10UserDefinedDataSet::SaveBinary(NiBinaryStream& kStream)
{
    unsigned int uiCount = GetBlockCount();
    NiStreamSaveBinary(kStream, uiCount);

    NSBD3D10UserDefinedDataBlock* pkBlock = GetFirstBlock();
    while (pkBlock)
    {
        if (!pkBlock->SaveBinary(kStream))
        {
            NiD3D10ShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                true, "* ERROR: NSBD3D10UserDefinedDataSet::SaveBinary\n"
                "    Failed to write block %s!\n",
                pkBlock->GetName());
            return false;
        }
        pkBlock = GetNextBlock();
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10UserDefinedDataSet::LoadBinary(NiBinaryStream& kStream)
{
    unsigned int uiCount;
    NiStreamLoadBinary(kStream, uiCount);

    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        NSBD3D10UserDefinedDataBlock* pkBlock = 
            NiNew NSBD3D10UserDefinedDataBlock(0);
        NIASSERT(pkBlock);

        if (!pkBlock->LoadBinary(kStream))
        {
            NiD3D10ShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                false, "* ERROR: NSBD3D10UserDefinedDataSet::LoadBinary\n"
                "    Failed to read block %2d of %2d\n", 
                ui, uiCount);
            return false;
        }

        m_kUserDefinedDataBlocks.AddTail(pkBlock);
    }

    return true;
}
//---------------------------------------------------------------------------
#if defined(NIDEBUG)
//---------------------------------------------------------------------------
void NSBD3D10UserDefinedDataSet::Dump(FILE* pf)
{
    NSBD3D10Utility::Dump(pf, true, "UserDefinedDataSet\n");
    NSBD3D10Utility::IndentInsert();

    NSBD3D10Utility::Dump(pf, true, "Count %d\n", GetBlockCount());

    NSBD3D10UserDefinedDataBlock* pkBlock;

    NiTListIterator kIter = m_kUserDefinedDataBlocks.GetHeadPos();
    while (kIter)
    {
        pkBlock = m_kUserDefinedDataBlocks.GetNext(kIter);
        if (pkBlock)
        {
            pkBlock->Dump(pf);
        }
    }
    NSBD3D10Utility::IndentRemove();
    NSBD3D10Utility::IndentRemove();
}
//---------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)
//---------------------------------------------------------------------------
