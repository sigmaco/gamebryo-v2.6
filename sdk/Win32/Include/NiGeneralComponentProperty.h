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

#ifndef NIGENERALCOMPONENTPROPERTY_H
#define NIGENERALCOMPONENTPROPERTY_H

#include "NiEntityLibType.h"
#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include <NiFixedString.h>
#include <NiBool.h>

class NiGeneralComponent;

class NIENTITY_ENTRY NiGeneralComponentProperty : public NiRefObject
{
public:
    NiGeneralComponentProperty(const NiFixedString& kPropertyName,
        const NiFixedString& kDisplayName,
        const NiFixedString& kPrimitiveType,
        const NiFixedString& kSemanticType,
        const NiFixedString& kDescription, bool IsCollection = false);
    virtual ~NiGeneralComponentProperty();

    inline const NiFixedString& GetPropertyName() const;
    inline void SetPropertyName(const NiFixedString& kPropertyName);

    inline const NiFixedString& GetDisplayName() const;
    inline void SetDisplayName(const NiFixedString& kDisplayName);

    inline const NiFixedString& GetPrimitiveType() const;
    inline void SetPrimitiveType(const NiFixedString& kPrimitiveType);

    inline const NiFixedString& GetSemanticType() const;
    inline void SetSemanticType(const NiFixedString& kSemanticType);

    inline const NiFixedString& GetDescription() const;
    inline void SetDescription(const NiFixedString& kDescription);

    //functions for managing collections
    inline NiBool GetIsCollection() const;
    inline void SetIsCollection(bool bIsCollection);

    inline unsigned int GetCollectionSize() const;
    inline void SetCollectionSize(unsigned int uiSize);

    // Functions for setting data.
    template<class T> void SetData(T* pvDataArray, size_t stArrayCount,
        unsigned int uiIndex = 0);

    // Functions for getting data.
    inline void* GetData(unsigned int uiIndex = 0) const;
    inline size_t GetDataSizeInBytes(unsigned int uiIndex = 0) const;


    inline NiGeneralComponentProperty* Clone();

private:
    NiFixedString m_kPropertyName;
    NiFixedString m_kDisplayName;
    NiFixedString m_kPrimitiveType;
    NiFixedString m_kSemanticType;
    NiFixedString m_kDescription;
    bool m_bIsCollection;
    NiTPrimitiveArray<void*> m_kDataPointers;
    NiTPrimitiveArray<size_t> m_kDataSizesInBytes;
};

NiSmartPointer(NiGeneralComponentProperty);

#include "NiGeneralComponentProperty.inl"

#endif // NIGENERALCOMPONENTPROPERTY_H
