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

#ifndef NIMESHPROFILECOMPONENT_H
#define NIMESHPROFILECOMPONENT_H

#include <NiSystemDesc.h>
#include <NiString.h>
#include <NiCommonSemantics.h>
#include <NiCommonSemantics.h>

#include "NiMeshProfileXMLParserLibType.h"

class NiMeshProfileElement;
class TiXmlElement;

/// @cond EMERGENT_INTERNAL
/**
    This is the component class for the mesh profile XML parser library.  A
    component is owned by it's parent element class.  The data stored by a
    component is dependent on the type of the component.  It can store a given
    default value to be used litterally or it can store a link to another
    component in a different element.

    <Component value="" default="" remap="" clamp=""/>
*/
/// @endcond
class NIMESHPROFILEXMLPARSER_ENTRY NiMeshProfileComponent : public NiRefObject
{
public:
    enum Component
    {
        PD_X = 0,       // Index 0            
        PD_Y = 1,       // Index 1           
        PD_Z = 2,       // Index 2
        PD_W = 3,       // Index 3
        PD_R = PD_X,    // Same as X
        PD_G = PD_Y,    // Same as Y
        PD_B = PD_Z,    // Same as Z
        PD_A = PD_W,    // Same as W
        PD_0 = PD_X,    // Same as X
        PD_1 = PD_Y,    // Same as Y
        PD_2 = PD_Z,    // Same as Z
        PD_3 = PD_W,    // Same as W
        PD_U = PD_X,    // Same as X
        PD_V = PD_Y,    // Same as Y

        PD_COMPONENT_COUNT = 4,
        PD_MAX_COMPS = 4,
        PD_DISABLED,
    };

    NiMeshProfileComponent();
    virtual ~NiMeshProfileComponent();

    // Static type name
    inline static const char* GetTypeName() {return "Component";}

    // Static function to return the component type given the character
    // 'x' 'y' 'z' 'w' 'u' 'v' 'r' 'g' 'b' 'a' '0' '1' '2' '3'
    static Component GetComponentType(const char c);

    // Parse the link
    static bool IsLiteral(const NiString& strLink);
    static bool ParseComponentLink(const NiString& strLink, 
        NiFixedString* pkStream, NiFixedString* pkElement, int* piIndex,
        NiFixedString* pkComponent);
    static bool ParseElementLink(const NiString& strLink, 
        NiFixedString* pkStream, NiFixedString* pkElement, int* piIndex);

    /**
        This structure is used to store the data needed to perform a remap.
        This class does not do any of the remap funcationality but serves
        solely as the data storage to be used to convert to a NiOpRemap 
        operational node.

        remap="src-a=x src-b=y dst-a=z dst-b=w"
    */
    struct NiRemap {

        NiRemap();

        // Static type name
        inline static const char* GetTypeName() {return "Remap";}

        bool SetValue(const char* pValue);

        double m_dSrcStart;
        double m_dSrcEnd;
        double m_dDestStart;
        double m_dDestEnd;

        bool m_bIsValueSet;
    };

    /**
        This structure is used to store the data needed to perform a clamp.
        This class does not do any of the clamping functionality but serves
        solely as the data storage to be used to convert to a NiOpClamp 
        operational node.

        clamp="min=x max=y"
    */
    struct NiClamp {

        NiClamp();

        // Static type name
        inline static const char* GetTypeName() {return "Clamp";}

        bool SetValue(const char* pValue);

        double m_dMin;
        double m_dMax;

        bool m_bIsValueSet;
    };

    // Accessors
    inline NiMeshProfileComponent::Component GetType() const;
    inline const char * GetLinkedComponent() const;
    inline const NiRemap& GetRemap() const;
    inline const NiClamp& GetClamp() const;

    void GetPrepData(const NiMeshProfileElement* pkParentElement, 
        NiFixedString& kSemantic, 
        int& iIndex, Component& eComponent, 
        double & dLiteral) const;

    // Other functions
    inline bool NeedsClamp() const;
    inline bool NeedsRemap() const;
    inline bool IsLiteral() const;
    void Reset();
    inline void SetLinkedComponent(const char* pcLinkedComponent);
    bool Load(const TiXmlElement* pkElement, 
        const Component eComponent);
    void Copy(const NiMeshProfileComponent* pkFrom);
    void PrintDescription();
    bool Save() const;

protected:
    Component m_eType;
    NiString m_strLinkedComponent;
    NiRemap m_kRemap;
    NiClamp m_kClamp;
    
};

NiSmartPointer(NiMeshProfileComponent);

#include "NiMeshProfileComponent.inl"

#endif  // #ifndef NIMESHPROFILECOMPONENT_H
