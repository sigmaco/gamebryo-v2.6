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

#ifndef NIMATERIALNODEXMLLIBRARYREADER_H
#define NIMATERIALNODEXMLLIBRARYREADER_H

#include "NiMaterialNodeXMLLibraryReaderLibType.h"
#include <NiMaterialNodeLibraryReader.h>
class TiXmlElement;
class NiMaterialFragmentNode;

class NIMATERIALNODEXMLLIBRARYREADER_ENTRY NiMaterialNodeXMLLibraryReader : 
    public NiMaterialNodeLibraryReader
{
public:
    NiMaterialNodeXMLLibraryReader();
    virtual NiMaterialNodeLibrary* Load(const char* pcFilename);
protected:
    void ProcessXML(NiMaterialNodeLibrary*& pkLib, TiXmlElement* pkElement);
    bool ProcessFragment(TiXmlElement* pkElement, 
        NiMaterialFragmentNode* pkFrag);
    bool ProcessResource(TiXmlElement* pkElement,
        NiMaterialNode* pkNode, bool bInput);
    bool ProcessCode(TiXmlElement* pkElement, NiMaterialFragmentNode* pkNode);
    bool ProcessDesc(TiXmlElement* pkElement, NiMaterialFragmentNode* pkNode);
};

typedef NiPointer<NiMaterialNodeXMLLibraryReader> 
    NiMaterialNodeXMLLibraryReaderPtr;

#endif  //#ifndef NIMATERIALNODEXMLLIBRARYREADER_H
