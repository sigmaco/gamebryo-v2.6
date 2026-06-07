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

#ifndef NILINES_H
#define NILINES_H

#include "NiGeometry.h"
#include "NiLinesData.h"

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiMesh in the NiMesh library.
*/

class NIMAIN_ENTRY NiLines : public NiGeometry
{
public:
    NiDeclareRTTI;
    NiDeclareClone(NiLines);
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***

    // The constructed object is given ownership of the input arrays and
    // has the responsibility for deleting them when finished with them.
    NiLines(unsigned short usVertices, NiPoint3* pkVertex, NiColorA* pkColor,
        NiPoint2* pkTexture, unsigned short usNumTextureSets, 
        NiShaderRequirementDesc::NBTFlags eNBTMethod, NiBool* pbFlags);

    // The constructed object shares the input data.
    NiLines(NiLinesData* pkModelData);

    // connection flags
    inline NiBool* GetFlags();
    inline const NiBool* GetFlags() const;

    // *** end Emergent internal use only ***
protected:
    // streaming support
    NiLines();
};

typedef NiPointer<NiLines> NiLinesPtr;

//---------------------------------------------------------------------------
//  Inline include
#include "NiLines.inl"

//---------------------------------------------------------------------------

#endif

