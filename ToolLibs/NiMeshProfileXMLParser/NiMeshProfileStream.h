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

#ifndef NIMESHPROFILESTREAM_H
#define NIMESHPROFILESTREAM_H

#include <NiString.h>
#include <NiTArray.h>

#include "NiMeshProfileProcessorLibType.h"
#include "NiMeshProfileElement.h"

class TiXmlElement;

/*
<Stream name="" renderer="" access="" usage="">
    <Elements>
</Stream>
*/

/// @cond EMERGENT_INTERNAL
/**
    This is the XML stream class.  It corresponds to the NiDataStream object.
    This object contains data storage that allows for conversion to the
    operational profile. The name is needed for reference within the XML scope.
    Access flags and usage are important when trying to set the different
    attributes for the NiDataStream.
*/
/// @endcond
class NIMESHPROFILEXMLPARSER_ENTRY NiMeshProfileStream : public NiRefObject
{
public:
    NiMeshProfileStream();
    virtual ~NiMeshProfileStream();

    // Static type name
    inline static const char* GetFinalizeTypeName() {return "FinalizeStream";}

    // Accessors
    inline const NiFixedString& GetName() const;
    inline NiSystemDesc::RendererID GetRenderer() const;
    inline NiUInt8 GetAccessFlags() const;
    inline NiDataStream::Usage GetUsage() const;
    inline unsigned int GetElementCount() const;
    inline const NiMeshProfileElement* GetElement(unsigned int uiIndex) inline const;
    inline const NiTObjectArray <NiMeshProfileElementPtr>* GetElements() inline const;
    inline const char* GetRefStream() const;
    void GetLinks(NiTObjectArray<NiString>& kLinkNames);

    // Other functions
    void Reset();
    inline void SetName(const char* pName);
    bool Load(const TiXmlElement* pkElement);
    void InsertElement(const NiMeshProfileElement* pkElement,
        NiSystemDesc::RendererID eRenderer = NiSystemDesc::RENDERER_GENERIC);
    void MergeElement(unsigned int uiIndex, 
        const NiMeshProfileElement* pkElement);
    void Merge(const NiMeshProfileStream* pkFrom);
    void Copy(const NiMeshProfileStream* pkFrom, 
        NiSystemDesc::RendererID eRenderer);
    void Copy(const NiMeshProfileStream* pkFrom);
    bool NiMeshProfileStream::Fixup(
        const NiTObjectArray <NiMeshProfileElementPtr>& kElements);
    bool Validate();
    bool IsUnique(const NiMeshProfileStream* pkStream) const;
    void PrintDescription();
    bool Save() const;

protected:

    

    NiFixedString m_strName;
    NiSystemDesc::RendererID m_eRenderer;
    NiUInt8 m_uiAccessFlags;
    NiDataStream::Usage m_eUsage;
    NiTObjectArray <NiMeshProfileElementPtr> m_kElements;

    // Not read data
    // Referenced stream object default is empty
    NiString m_strRefStream;

    // Set the modified enumeration
    enum Modified
    {
        MODIFIED_NAME           = 0x01,
        MODIFIED_RENDERER       = 0x02,
        MODIFIED_ACCESSFLAGS    = 0x04,
        MODIFIED_USAGE          = 0x08,
        MODIFIED_REFSTREAM      = 0x10,
    };

    NiUInt32 m_uiModifiedFlags;
};

NiSmartPointer(NiMeshProfileStream);

#include "NiMeshProfileStream.inl"

#endif  // #ifndef NIMESHPROFILESTREAM_H
