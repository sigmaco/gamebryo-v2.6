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

#ifndef NIENTITYPROPERTYINTERFACE_H
#define NIENTITYPROPERTYINTERFACE_H

#include "NiEntityLibType.h"
#include "NiInterfaceSmartPointer.h"
#include <NiFixedString.h>
#include "NiEntityErrorInterface.h"
#include "NiEntityRenderingContext.h"
#include "NiUniqueID.h"
#include <NiBool.h>
#include <NiTSet.h>
#include <NiPoint2.h>
#include <NiPoint3.h>
#include <NiMatrix3.h>
#include <NiQuaternion.h>
#include <NiColor.h>
#include <NiObject.h>

class NiEntityInterface;
class NiExternalAssetManager;

//The following is to resolve a naming conflict with the Win32 API 
//function GetClassName. WinUser.h redefines that token and causes
//conflicts for any App that include both this header and Winuser.h
#ifdef GetClassName
#undef GetClassName
#endif
// A pure virtual class that provides an interface for accessing entity
// properties.
class NIENTITY_ENTRY NiEntityPropertyInterface
{
public:
    // Primitive types.
    static NiFixedString PT_FLOAT;
    static NiFixedString PT_BOOL;
    static NiFixedString PT_INT;
    static NiFixedString PT_UINT;
    static NiFixedString PT_SHORT;
    static NiFixedString PT_USHORT;
    static NiFixedString PT_STRING;
    static NiFixedString PT_POINT2;
    static NiFixedString PT_POINT3;
    static NiFixedString PT_QUATERNION;
    static NiFixedString PT_MATRIX3;
    static NiFixedString PT_COLOR;
    static NiFixedString PT_COLORA;
    static NiFixedString PT_NIOBJECTPOINTER;
    static NiFixedString PT_ENTITYPOINTER;

    // Constructor.
    NiEntityPropertyInterface();

    // This function should be called whenever a reference is obtained to this
    // interface.
    virtual void AddReference() = 0;

    // This function should be called whenever a reference to this interface
    // is no longer needed.
    virtual void RemoveReference() = 0;

    // Returns the type of this interface.
    virtual NiFixedString GetInterfaceType() = 0;

    // Returns the class name for the implementing class for this interface.
    virtual NiFixedString GetClassName() const = 0;

    // Gets the template ID for this interface.
    virtual NiUniqueID GetTemplateID() = 0;

    // Sets the template ID for this interface. Implementing classes are not
    // required to allow setting of their template ID and should return false
    // if the template ID was not set.
    virtual NiBool SetTemplateID(const NiUniqueID& kID) = 0;

    // Returns the name of this object.
    virtual NiFixedString GetName() const = 0;

    // Sets the name of this object, if allowed. The return value indicates
    // whether or not the name was set.
    virtual NiBool SetName(const NiFixedString& kName) = 0;

    // Returns whether or not this object is animated and should be updated
    // each frame.
    virtual NiBool IsAnimated() const;

    // Updates the object. Errors are reported to the provided error
    // interface. External assets are loaded using the provided external
    // asset interface.
    virtual void Update(NiEntityPropertyInterface* pkParentEntity,
        float fTime, NiEntityErrorInterface* pkErrors,
        NiExternalAssetManager* pkAssetManager) = 0;

    // Builds the Visible Set on the object using the provided rendering
    // context. Errors are reported to the provided error interface.
    virtual void BuildVisibleSet(NiEntityRenderingContext* pkRenderingContext,
        NiEntityErrorInterface* pkErrors) = 0;

    // Gets all the property names for this object. Property names are
    // added to the array. No names are removed from the array.
    virtual void GetPropertyNames(
        NiTObjectSet<NiFixedString>& kPropertyNames) const = 0;

    // Returns whether or not the named property can be reset to a default
    // value. If the property is not found, the function returns false.
    virtual NiBool CanResetProperty(const NiFixedString& kPropertyName,
        bool& bCanReset) const = 0;

    // Resets the property to its default value if it can be reset. The return
    // value indicates whether or not the property was reset.
    virtual NiBool ResetProperty(const NiFixedString& kPropertyName) = 0;

    // Makes the specified property unique, if it is not already. This will
    // not change the property value but will instead prevent the property
    // from inheriting its value from a default value. The returned
    // bMadeUnique value indicates whether or not the property was made
    // unique. If the property was already unique, this value will be false.
    virtual NiBool MakePropertyUnique(const NiFixedString& kPropertyName,
        bool& bMadeUnique) = 0;

    // Indicates whether or not this object supports adding and removing
    // properties.
    virtual NiBool IsAddPropertySupported();

    // Adds a property with the specified information to this object. The new
    // property is assigned a default value. If adding properties is not
    // supported or an error occurs, this function returns false.
    virtual NiBool AddProperty(const NiFixedString& kPropertyName,
        const NiFixedString& kDisplayName,
        const NiFixedString& kPrimitiveType,
        const NiFixedString& kSemanticType,
        const NiFixedString& kDescription);

    // Changes a property to make it act as a collection rather than a single 
    // value. This is only supported on properties that support adding or 
    // removing properties in general. 
    virtual NiBool MakeCollection(const NiFixedString& kPropertyName,
        bool bCollection);

    // Removes the named property. If removing properties is not supported or,
    // an error occurs, this function returns false.
    virtual NiBool RemoveProperty(const NiFixedString& kPropertyName);

    // Gets and sets the display name for the named property. If the property
    // is not found, these functions return false. If the display name is set
    // to NULL, the property should not be displayed.
    virtual NiBool GetDisplayName(const NiFixedString& kPropertyName,
        NiFixedString& kDisplayName) const = 0;
    virtual NiBool SetDisplayName(const NiFixedString& kPropertyName,
        const NiFixedString& kDisplayName) = 0;

    // Gets and sets the primitive type for the named property. If the
    // property is not found, these functions return false.
    virtual NiBool GetPrimitiveType(const NiFixedString& kPropertyName,
        NiFixedString& kPrimitiveType) const = 0;
    virtual NiBool SetPrimitiveType(const NiFixedString& kPropertyName,
        const NiFixedString& kPrimitiveType) = 0;

    // Gets and sets the semantic type for the named property. If the property
    // is not found, these functions return false.
    virtual NiBool GetSemanticType(const NiFixedString& kPropertyName,
        NiFixedString& kSemanticType) const = 0;
    virtual NiBool SetSemanticType(const NiFixedString& kPropertyName,
        const NiFixedString& kSemanticType) = 0;

    // Gets and sets the description for the named property. If the property
    // is not found, these functions return false.
    virtual NiBool GetDescription(const NiFixedString& kPropertyName,
        NiFixedString& kDescription) const = 0;
    virtual NiBool SetDescription(const NiFixedString& kPropertyName,
        const NiFixedString& kDescription) = 0;

    // Returns the category for the named property. Properties are grouped by
    // category when displaying in a user interface.
    virtual NiBool GetCategory(const NiFixedString& kPropertyName,
        NiFixedString& kCategory) const = 0;

    // Retrieves whether or not the named property is read-only. If the
    // property is not found, this function returns false.
    virtual NiBool IsPropertyReadOnly(const NiFixedString& kPropertyName,
        bool& bIsReadOnly) = 0;

    // Retrieves whether or not the named property is unique. If the
    // property is not found, this function returns false.
    virtual NiBool IsPropertyUnique(const NiFixedString& kPropertyName,
        bool& bIsUnique) = 0;

    // Retrieves whether or not the named property should be serialized to
    // disk. If the property is not found, this function returns false.
    virtual NiBool IsPropertySerializable(const NiFixedString& kPropertyName,
        bool& bIsSerializable) = 0;

    // Retrieves whether or not the named property has the ability to be 
    // inherited. If the property is not found, this function returns false
    virtual NiBool IsPropertyInheritable(const NiFixedString& kPropertyName,
        bool& bIsInheritable) = 0;

    // Determines whether or not the property represents the path to an
    // external asset file. If the property is not found, this function
    // returns false.
    virtual NiBool IsExternalAssetPath(const NiFixedString& kPropertyName,
        unsigned int uiIndex, bool& bIsExternalAssetPath) const = 0;

    // Retrieves the number of data elements for this property. This number
    // should be used to limit the index value that is passed into the
    // GetPropertyData and SetPropertyData functions. If the property is not
    // found, this function returns false.
    virtual NiBool GetElementCount(const NiFixedString& kPropertyName,
        unsigned int& uiCount) const = 0;

    // Sets an upper bound on the number of elements for this property. If
    // the count is increased, appropriate default values are provided for the
    // new elements. If the count is decreased, elements with the highest
    // indices will be lost. This function should not be called for properties
    // that are not collections. bCountSet returns whether or not the count
    // was set (will be false for non-collection properties). If the
    // property is not found, this function returns false.
    virtual NiBool SetElementCount(const NiFixedString& kPropertyName,
        unsigned int uiCount, bool& bCountSet) = 0;

    // Returns whether or not the specified property is a collection of
    // elements or has just a single element. This has a direct bearing on
    // how such a property is edited in a user interface. This function
    // returns false if the property is not found. bIsCollection will be true
    // if the property is a collection and false if the property will only
    // ever have a single element.
    virtual NiBool IsCollection(const NiFixedString& kPropertyName,
        bool& bIsCollection) const = 0;

    // Functions for getting property data. These functions return false if
    // the property is not found, in which case the data arguments passed in
    // have not been modified.
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        float& fData, unsigned int uiIndex = 0) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        bool& bData, unsigned int uiIndex = 0) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        int& iData, unsigned int uiIndex = 0) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        unsigned int& uiData, unsigned int uiIndex = 0) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        short& sData, unsigned int uiIndex = 0) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        unsigned short& usData, unsigned int uiIndex = 0) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiFixedString& kData, unsigned int uiIndex = 0) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiPoint2& kData, unsigned int uiIndex = 0) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiPoint3& kData, unsigned int uiIndex = 0) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiQuaternion& kData, unsigned int uiIndex = 0) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiMatrix3& kData, unsigned int uiIndex = 0) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiColor& kData, unsigned int uiIndex = 0) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiColorA& kData, unsigned int uiIndex = 0) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiObject*& pkData, unsigned int uiIndex = 0) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiEntityInterface*& pkData, unsigned int uiIndex = 0) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        void*& pvData, size_t& stDataSizeInBytes, unsigned int uiIndex = 0)
        const;

    // Functions for setting property data. These functions return false if
    // the property is not found.
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        float fData, unsigned int uiIndex = 0);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        bool bData, unsigned int uiIndex = 0);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        int iData, unsigned int uiIndex = 0);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        unsigned int uiData, unsigned int uiIndex = 0);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        short sData, unsigned int uiIndex = 0);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        unsigned short usData, unsigned int uiIndex = 0);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiFixedString& kData, unsigned int uiIndex = 0);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiPoint2& kData, unsigned int uiIndex = 0);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiPoint3& kData, unsigned int uiIndex = 0);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiQuaternion& kData, unsigned int uiIndex = 0);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiMatrix3& kData, unsigned int uiIndex = 0);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiColor& kData, unsigned int uiIndex = 0);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiColorA& kData, unsigned int uiIndex = 0);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        NiObject* pkData, unsigned int uiIndex = 0);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        NiEntityInterface* pkData, unsigned int uiIndex = 0);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const void* pvData, size_t stDataSizeInBytes,
        unsigned int uiIndex = 0);

    // Returns a unique ID for this instance. This ID is generated in the
    // constructor for this class.
    const NiUniqueID& GetID() const;

    // Functions for accessing the filename in which this instance is stored.
    const NiFixedString& GetSourceFilename() const;
    virtual void SetSourceFilename(const NiFixedString& kSourceFilename);

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    inline virtual ~NiEntityPropertyInterface() {}

    // Sets a unique ID for this instance. This ID should not be set to the
    // same value for any two instances. This function is only provided for
    // streaming purposes.
    void SetID(const NiUniqueID& kID);
    // *** end Emergent internal use only ***

private:
    NiUniqueID m_kInstanceID;
    NiFixedString m_kSourceFilename;
};

NiInterfaceSmartPointer(NiEntityPropertyInterface);

#endif // NIENTITYPROPERTYINTERFACE_H
