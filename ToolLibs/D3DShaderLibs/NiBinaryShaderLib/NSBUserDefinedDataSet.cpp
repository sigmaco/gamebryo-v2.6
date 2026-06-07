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
#include "NiBinaryShaderLibPCH.h"

#include <NiShaderFactory.h>

#include <NiShaderConstantMap.h>

#include "NSBUserDefinedDataSet.h"
#include "NSBUtility.h"

//---------------------------------------------------------------------------
NSBUserDefinedDataSet::NSBUserDefinedDataSet()
{
    m_kBlockIter = 0;
    m_kUserDefinedDataBlocks.RemoveAll();
}
//---------------------------------------------------------------------------
NSBUserDefinedDataSet::~NSBUserDefinedDataSet()
{
    NSBUserDefinedDataBlock* pkBlock = GetFirstBlock();
    while (pkBlock)
    {
        NiDelete pkBlock;
        pkBlock = GetNextBlock();
    }
    m_kUserDefinedDataBlocks.RemoveAll();
}
//---------------------------------------------------------------------------
unsigned int NSBUserDefinedDataSet::GetBlockCount()
{
    return m_kUserDefinedDataBlocks.GetSize();
}
//---------------------------------------------------------------------------
NSBUserDefinedDataBlock* NSBUserDefinedDataSet::GetBlock(const char* pcName, 
    bool bCreate)
{
    NiTListIterator kIter;
    NSBUserDefinedDataBlock* pkBlock;

    kIter = m_kUserDefinedDataBlocks.GetHeadPos();
    while (kIter)
    {
        pkBlock = m_kUserDefinedDataBlocks.GetNext(kIter);
        if (pkBlock && !NiStricmp(pkBlock->GetName(), pcName))
        {
            if (bCreate)
            {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* WARNING: NSBUserDefinedDataSet::GetBlock\n"
                    "    Block %s already in set\n"
                    "    Create was true - POTENTIAL OVERWRITE ISSUE!\n", 
                    pcName);
            }
            return pkBlock;
        }
    }

    if (bCreate)
    {
        pkBlock = NiNew NSBUserDefinedDataBlock(pcName);
        NIASSERT(pkBlock && 
            "NSBUserDefinedDataSet::GetBlock> Failed allocation!");

        // Add it to the list
        m_kUserDefinedDataBlocks.AddTail(pkBlock);

        return pkBlock;
    }
    return 0;
}
//---------------------------------------------------------------------------
NSBUserDefinedDataBlock* NSBUserDefinedDataSet::GetFirstBlock()
{
    m_kBlockIter = m_kUserDefinedDataBlocks.GetHeadPos();
    if (m_kBlockIter)
        return m_kUserDefinedDataBlocks.GetNext(m_kBlockIter);
    return 0;
}
//---------------------------------------------------------------------------
NSBUserDefinedDataBlock* NSBUserDefinedDataSet::GetNextBlock()
{
    if (m_kBlockIter)
        return m_kUserDefinedDataBlocks.GetNext(m_kBlockIter);
    return 0;
}
//---------------------------------------------------------------------------
bool NSBUserDefinedDataSet::SaveBinary(NiBinaryStream& kStream)
{
    unsigned int uiCount = GetBlockCount();
    NiStreamSaveBinary(kStream, uiCount);

    NSBUserDefinedDataBlock* pkBlock = GetFirstBlock();
    while (pkBlock)
    {
        if (!pkBlock->SaveBinary(kStream))
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                true, "* ERROR: NSBUserDefinedDataSet::SaveBinary\n"
                "    Failed to write block %s!\n",
                pkBlock->GetName());
            return false;
        }
        pkBlock = GetNextBlock();
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBUserDefinedDataSet::LoadBinary(NiBinaryStream& kStream)
{
    unsigned int uiCount;
    NiStreamLoadBinary(kStream, uiCount);

    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        NSBUserDefinedDataBlock* pkBlock = 
            NiNew NSBUserDefinedDataBlock(0);
        NIASSERT(pkBlock);

        if (!pkBlock->LoadBinary(kStream))
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                false, "* ERROR: NSBUserDefinedDataSet::LoadBinary\n"
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
void NSBUserDefinedDataSet::Dump(FILE* pf)
{
    NSBUtility::Dump(pf, true, "UserDefinedDataSet\n");
    NSBUtility::IndentInsert();

    NSBUtility::Dump(pf, true, "Count %d\n", GetBlockCount());

    NSBUserDefinedDataBlock* pkBlock;

    NiTListIterator kIter = m_kUserDefinedDataBlocks.GetHeadPos();
    while (kIter)
    {
        pkBlock = m_kUserDefinedDataBlocks.GetNext(kIter);
        if (pkBlock)
        {
            pkBlock->Dump(pf);
        }
    }
    NSBUtility::IndentRemove();
    NSBUtility::IndentRemove();
}
//---------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)
//---------------------------------------------------------------------------
