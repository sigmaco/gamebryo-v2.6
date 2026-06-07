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

#ifndef NSBD3D10USERDEFINEDDATASET_H
#define NSBD3D10USERDEFINEDDATASET_H

#include "NSBD3D10UserDefinedDataBlock.h"

// NSBD3D10UserDefinedDataSet is a container for a collection of
// NSBD3D10UserDefinedDataBlock instances.  This class exists to minimize code
// duplication as NSBD3D10Shader, NSBD3D10Implementation, and NSBD3D10Pass may
// all have a collection of NSBD3D10UserDefinedDataBlock instances.

class NID3D10BINARYSHADERLIB_ENTRY NSBD3D10UserDefinedDataSet
    : public NiRefObject
{
public:
    NSBD3D10UserDefinedDataSet();
    ~NSBD3D10UserDefinedDataSet();

    // The user-defined data
    unsigned int GetBlockCount();
    NSBD3D10UserDefinedDataBlock* GetBlock(const char* pcName, 
        bool bCreate = false);
    NSBD3D10UserDefinedDataBlock* GetFirstBlock();
    NSBD3D10UserDefinedDataBlock* GetNextBlock();

    // *** begin Emergent internal use only
    virtual bool SaveBinary(NiBinaryStream& kStream);
    virtual bool LoadBinary(NiBinaryStream& kStream);

#if defined(NIDEBUG)
    virtual void Dump(FILE* pf);
#endif  //#if defined(NIDEBUG)
    // *** end Emergent internal use only

protected:
    NiTListIterator m_kBlockIter;
    NiTPointerList<NSBD3D10UserDefinedDataBlock*> m_kUserDefinedDataBlocks;
};

NiSmartPointer(NSBD3D10UserDefinedDataSet);

#endif  //NSBD3D10USERDEFINEDDATASET_H
