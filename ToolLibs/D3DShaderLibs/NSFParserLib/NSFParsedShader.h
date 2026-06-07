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

#ifndef NSFPARSEDSHADER_H
#define NSFPARSEDSHADER_H

#include "NSFParserLibLibType.h"
#include <NSBShader.h>

class NSFPARSERLIB_ENTRY NSFParsedShader : public NiMemObject
{
public:
    NSFParsedShader();
    ~NSFParsedShader();

    NSBShader* GetShader();

    // *** begin Emergent internal use only
    bool Save(char* pszFilename);
    bool SaveBinary(NiBinaryStream& kStream);
    bool Load(char* pszFilename);
    bool LoadBinary(NiBinaryStream& kStream);
    // *** end Emergent internal use only

protected:
    NSBShaderPtr m_spNSBShader;
};

#endif  //NSFPARSEDSHADER_H
