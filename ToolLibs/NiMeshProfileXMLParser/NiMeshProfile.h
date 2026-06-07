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

#ifndef NIMESHPROFILE_H
#define NIMESHPROFILE_H

#include <NiTArray.h>
#include <NiString.h>

#include "NiMeshProfileProcessorLibType.h"
#include "NiMeshProfileStream.h"

class TiXmlElement;

/// @cond EMERGENT_INTERNAL
/**
    This class represents the class that parses mesh profiles. The XML is
    converted into this class one per renderer type. This class is used to
    convert into the operational mesh profile that does all the
    finalization of the data. This class owns two stream arrays, one for the
    preparation streams and another for the finalize streams. The "name"
    represents the name that is used for referencing throughout the mesh
    profile system. The description is a brief explanation of the usage and
    other information the profile author wishes to convey to the user.  During
    both the preparation and finalize phases the data streams that are not
    present in the mesh profile will be removed unless the flag to keep them is
    set. The preparation phase by default will keep unknown streams and the 
    finalize phase will remove them by default. There is an option to recompute
    the normals if you have done any modification of the positions that would
    result in not correct normals.

    <MeshProfile name="" description="" keepUnspecifiedFinalizeStreams="" 
        recomputenormals="">
        <FinalizeStream>
    </MeshProfile>
*/
/// @endcond
class NIMESHPROFILEXMLPARSER_ENTRY NiMeshProfile : public NiRefObject
{
public:
    NiMeshProfile();
    virtual ~NiMeshProfile();

    // Static type name
    inline static const char* GetTypeName() {return "MeshProfile";}

    // Accessors
    inline const NiFixedString& GetName() const;
    inline const char* GetDescription() const;
    inline bool GetRecomputeNormals() const;
    inline unsigned int GetFinalizeStreamCount() const;
    inline const NiMeshProfileStream* GetFinalizeStream(unsigned int uiIndex) const;
    inline const NiTObjectArray <NiMeshProfileStreamPtr>* GetFinalizeStreams() const;

    inline bool GetKeepUnspecifiedFinalizeStreams() const;

    void GetFinalizeData(const NiMeshProfileElement* pkParentElement, 
        const char* pcInput, const NiMeshProfileStream** pkStream,
        const NiMeshProfileElement** ppkElement,
        const NiMeshProfileComponent** ppkComponent) const;

    // Other functions
    void Reset();

    bool Load(const TiXmlElement* pkElement);
    bool Link(
        const NiTObjectArray <NiMeshProfileElementPtr>& kElements);
    void InsertFinalizeStream(const NiMeshProfileStream* pkFinalizeStream,
        NiSystemDesc::RendererID eRenderer);
    void MergeFinalizeStream(unsigned int uiIndex, 
        const NiMeshProfileStream* pkFinalizeStream);
    void CopyToRenderer(NiSystemDesc::RendererID eRenderer, 
        const NiMeshProfile* pkInputMeshProfile);
    void Copy(const NiMeshProfile* pkFrom, 
        const bool bCopyFinalizeStream = true);

    bool Fixup(
        const NiTObjectArray <NiMeshProfileElementPtr>& kElements,
        const NiTObjectArray <NiMeshProfileStreamPtr>& kFinalizeStreams);

    bool Validate();

    void PrintDescription();
    
    bool Save() const;

protected:

    void GetData(const NiMeshProfileElement* pkParentElement, 
        const NiTObjectArray <NiMeshProfileStreamPtr>& kStreams,
        const char* pcInput, const NiMeshProfileStream** ppkStream,
        const NiMeshProfileElement** ppkElement,
        const NiMeshProfileComponent** ppkComponent) const;

    NiFixedString m_strName;
    NiString m_strDescription;
    bool m_bRecomputeNormals;

    bool m_bKeepUnspecifiedFinalizeStreams;

    NiTObjectArray <NiMeshProfileStreamPtr> m_kFinalizeStreams;
    
};

NiSmartPointer(NiMeshProfile);

#include "NiMeshProfile.inl"

#endif  // #ifndef NIMESHPROFILE_H
