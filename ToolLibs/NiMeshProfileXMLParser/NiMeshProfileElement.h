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

#ifndef NIMESHPROFILEELEMENT_H
#define NIMESHPROFILEELEMENT_H

#include "NiMeshProfileProcessorLibType.h"

#include <NiTArray.h>
#include <NiString.h>
#include <NiCommonSemantics.h>
#include <NiDataStreamElement.h>
#include "NiMeshProfileOperations.h"
#include "NiMeshProfileComponent.h"

class TiXmlElement;

/*
<Element
    name=""
    type=""
    index=""
    convert=""
    converttype=""
    required=""
    renderer=""
    strict="">
    <Components>
</Element>
*/

/// @cond EMERGENT_INTERNAL
/**
    This is the element class that represents a NiDataStreamElement.  It
    parses the XML nodes and fills this data structure in with the
    corresponding values.  It is required that a semantic name is generated
    the semantic name is built from either the "m_strName" variable or the 
    "m_eType" variable.  If the "m_eType" variable has be set it has a higher
    priority over the "m_strName".  The "m_strName" is used within the internal
    XML scope for referencing.  This class contains an array of components at
    at least one to up to four components.
*/
/// @endcond
class NIMESHPROFILEXMLPARSER_ENTRY NiMeshProfileElement : public NiRefObject
{
public:
    NiMeshProfileElement();
    virtual ~NiMeshProfileElement();

    // Static type name
    inline static const char* GetTypeName() {return "Element";}

    /// The required enumeration
    enum RequiredCode 
    {
        NOT_REQUIRED,       // Is not required (default)
        REQUIRED_FAIL,      // Is required and will abort
    };

    // Accessors
    inline const NiFixedString& GetName() const;
    inline NiCommonSemantics::Enumeration GetType() const;
    inline NiUInt8 GetIndex() const;
    inline NiUInt8 GetStrictFlags() const;
    inline NiMeshProfileOperations::CastConvertType GetConvertType() const;
    inline NiDataStreamElement::Type GetConvertFormat() const;
    inline NiDataStreamElement::Format GetSpecialFormat() const;
    NiDataStreamElement::Format GetFormat() const;
    inline NiSystemDesc::RendererID GetRenderer() const;
    RequiredCode GetRequired() const;
    inline unsigned int GetComponentCount() const;
    inline const NiMeshProfileComponent* GetComponent(unsigned int uiIndex) const;
    inline const NiTObjectArray <NiMeshProfileComponentPtr>* GetComponents() const;
    inline bool NeedsConverting() const;
    inline bool NeedsSpecialConverting() const;

    inline const char* GetRefElement() const;

    inline const NiFixedString& GetSemanticName() const;

    // Other functions
    void Reset();
    inline void SetName(const char* pName);
    void FixupForRange(const int iIndex);
    bool Load(const TiXmlElement* pkElement, int& iRangeMin, int& iRangeMax);
    void InsertComponent(const NiMeshProfileComponent* pkComponent);
    void Merge(const NiMeshProfileElement* pkFrom);
    void Copy(const NiMeshProfileElement* pkFrom, 
        NiSystemDesc::RendererID eRenderer);
    void Copy(const NiMeshProfileElement* pkFrom);
    bool Validate();
    void PrintDescription();
    bool Save() const;

protected:
    // Conversion of CastConvertType
    static NiMeshProfileOperations::CastConvertType FromString(const
        NiFixedString& kName);
    static NiFixedString ToString(NiMeshProfileOperations::CastConvertType
        eType);

    NiFixedString m_strName;                    // Unique name
    NiCommonSemantics::Enumeration m_eType;     // Common type (pos,nor,uv...)
    NiUInt8 m_uiIndex;
    NiUInt8 m_uiStrictFlags;

    // m_eConvertFormat and m_eConvertType are not compatable with 
    // m_eSpecialFormat
    NiMeshProfileOperations::CastConvertType m_eConvertType;
    NiDataStreamElement::Type m_eConvertFormat;
    NiDataStreamElement::Format m_eSpecialFormat;

    NiSystemDesc::RendererID m_eRenderer;
    RequiredCode m_eRequired;
    NiTObjectArray<NiMeshProfileComponentPtr> m_kComponents;

    // Not read data
    // Referenced element default is empty
    NiString m_strRefElement;

    // Set the modified enumeration
    enum Modified
    {
        MODIFIED_NAME           = 0x00000001,
        MODIFIED_TYPE           = 0x00000002,
        MODIFIED_INDEX          = 0x00000004,
        MODIFIED_STRICTFLAGS    = 0x00000008,
        MODIFIED_CONVERTTYPE    = 0x00000010,
        MODIFIED_CONVERTFORMAT  = 0x00000020,
        MODIFIED_SPECIALFORMAT  = 0x00000040,
        MODIFIED_RENDERER       = 0x00000080,
        MODIFIED_REQUIRED       = 0x00000100,
        MODIFIED_REFELEMENT     = 0x00000200,
    };

    NiUInt32 m_uiModifiedFlags;
 
};

NiSmartPointer(NiMeshProfileElement);

#include "NiMeshProfileElement.inl"

#endif  // #ifndef NIMESHPROFILEELEMENT_H
