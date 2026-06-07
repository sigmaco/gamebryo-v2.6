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

#ifndef NIGPUPROGRAM_H
#define NIGPUPROGRAM_H

#include "NiRTTI.h"
#include "NiRefObject.h"
#include "NiSmartPointer.h"

class NIMAIN_ENTRY NiGPUProgram : public NiRefObject
{
    NiDeclareRootRTTI(NiGPUProgram);
public:
    enum ProgramType
    {
        PROGRAM_VERTEX = 0,
        PROGRAM_PIXEL,
        PROGRAM_GEOMETRY,
        PROGRAM_MAX
    };

    NiGPUProgram(ProgramType eType);
    ProgramType GetProgramType();

    virtual unsigned int GetVariableCount();
    virtual const char* GetVariableName(unsigned int uiIndex);

protected:
    ProgramType m_eProgramType;
};
//---------------------------------------------------------------------------
NiSmartPointer(NiGPUProgram);
//---------------------------------------------------------------------------
#include "NiGPUProgram.inl"
#endif
