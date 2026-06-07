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

#ifndef NSBUSERDEFINEDDATASET_H
#define NSBUSERDEFINEDDATASET_H

#include "NSBUserDefinedDataBlock.h"

// NSBUserDefinedDataSet is a container for a collection of 
// NSBUserDefinedDataBlock instances. This class exists to minimize
// code duplication as NSBShader, NSBImplementation, and NSBPass may
// all have a collection of NSBUserDefinedDataBlock instances.
//
class NIBINARYSHADERLIB_ENTRY NSBUserDefinedDataSet : public NiRefObject
{
public:
    NSBUserDefinedDataSet();
    ~NSBUserDefinedDataSet();

    // The user-defined data
    unsigned int GetBlockCount();
    NSBUserDefinedDataBlock* GetBlock(const char* pcName, 
        bool bCreate = false);
    NSBUserDefinedDataBlock* GetFirstBlock();
    NSBUserDefinedDataBlock* GetNextBlock();

    // *** begin Emergent internal use only
    virtual bool SaveBinary(NiBinaryStream& kStream);
    virtual bool LoadBinary(NiBinaryStream& kStream);

#if defined(NIDEBUG)
    virtual void Dump(FILE* pf);
#endif  //#if defined(NIDEBUG)
    // *** end Emergent internal use only

protected:
    NiTListIterator m_kBlockIter;
    NiTPointerList<NSBUserDefinedDataBlock*> m_kUserDefinedDataBlocks;
};

NiSmartPointer(NSBUserDefinedDataSet);

#endif  //NSBUSERDEFINEDDATASET_H
