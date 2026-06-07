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

#ifndef NIMETADATA_H
#define NIMETADATA_H

#include <NiTStringMap.h>
#include <NiFixedString.h>
#include <NiTSet.h>

#include "NiTerrainLibType.h"

class NiDOMTool;
class NiMetaDataStore;

/** 
    The MetaData object is used to store extra data associated with an object.

    @note Currently only supports INTEGER, STRING and FLOAT data-types.
 */
class NITERRAIN_ENTRY NiMetaData : public NiMemObject
{
public:

    /**
        Meta data data-types.
        
        These are explicitly defined so the compiler does not change them.
        Remember, these are used during load/save operations.
    */
    enum KeyType
    {
        /// Single integer value.
        INTEGER = 2,

        /// Single floating point value.
        FLOAT = 3,

        /// Arbitrary string value.
        STRING = 4,        
        
        /// Single integer value that will blend with other 
        /// metadata objects
        INTEGER_BLENDED = 5,
        
        /// Single floating point value that will blend with other 
        /// metadata objects
        FLOAT_BLENDED = 6
    };

    /// @cond EMERGENT_INTERNAL
    class MetaDataValue : public NiMemObject
    {
    public:
        /// The type of the data.
        KeyType eType;
        
        /// The string value, if this is a string type.
        NiFixedString kStringValue;
        
        /// The weight for the data.
        float fWeight;
        
        /// Int or float values.
        union 
        {
            NiInt32 iValue;
            float fValue;
        };
    
        /// An operator to implement a "Zero-Element".
        const MetaDataValue& operator=(const int& iParam)
        {
            eType = INTEGER;
            iValue = iParam;
            return *this;
        }
    };
    /// @endcond

    /// @name Constructors
    //@{    
    /**
        Initialize a new meta data object using a specified meta data store.
        
        @param pkMetaDataStore A valid meta data store which this meta object
            shall use to manage its keys
     */
    NiMetaData(NiMetaDataStore* pkMetaDataStore);

    /**
        Copy constructor for storing meta data in a list/map. 
        
        @param kMetaData The source NiMetaData object from which we will
            construct the copy.
     */
    NiMetaData(const NiMetaData& kMetaData);

    /// Initialize a new meta data object using the static meta data store
    NiMetaData();
    //@}

    /// Default Destructor.
    ~NiMetaData();

    /// Assignment operator.
    NiMetaData& operator=(const NiMetaData& kMetaData);    

    /** 
        Set an int value of meta data corresponding to the given key.
        
        If no meta data previous existed with the given key, it will be
        created.
        
        @param kKey Key corresponding to the meta data to be set
        @param iValue Value to give the meta data.
        @param eType The type of the key as enumerated by the KeyType 
            enumeration.
        @param fWeight Weight assigned to the meta data entry.
        @return True if the value was successfully set.
     */
    bool Set(const NiFixedString& kKey, NiInt32 iValue, 
        KeyType eType = INTEGER, float fWeight = 1.0f);

    /** 
        Set a float value of meta data corresponding to the given key.
        
        If no meta data previous existed with the given key, it will be
        created.
        
        @param kKey Key corresponding to the meta data to be set
        @param fValue Value to give the meta data.
        @param eType The type of the key as enumerated by the KeyType 
            enumeration.
        @param fWeight Weight assigned to the meta data entry.
        @return True if the value was successfully set.
    */
    bool Set(const NiFixedString& kKey, float fValue, 
        KeyType eType = FLOAT, float fWeight = 1.0f);
    
    /** 
        Set a string value of meta data corresponding to the given key.
        
        If no meta data previous existed with the given key, it will be
        created.
        
        @param kKey Key corresponding to the meta data to be set
        @param kValue Value to give the meta data.
        @param eType The type of the key as enumerated by the KeyType 
            enumeration.
        @param fWeight Weight assigned to the meta data entry.
        @return True if the value was successfully set.
    */
    bool Set(const NiFixedString& kKey, const NiFixedString& kValue, 
        KeyType eType = STRING, float fWeight = 1.0f);

    /** 
        Retrieve int valued meta data stored within this object,
        specified by the given key.
        
        This method will fail if no meta data exists by the given key, or it
        is of the wrong type.
        
        @param kKey Key corresponding to the meta data to be set retrieved.
        @param iValue Variable within which to place the value of the 
            meta data.
        @param fWeight Current weight of the meta data object.
        @return true if the value was successfully retrieved.
     */
    bool Get(const NiFixedString& kKey, NiInt32& iValue, float& fWeight) const;
    
    /** 
        Retrieve float valued meta data stored within this object,
        specified by the given key.
        
        This method will fail if no meta data exists by the given key, or
        it is of the wrong type.

        @param kKey Key corresponding to the meta data to be set retrieved.
        @param fValue Variable within which to place the value of the 
            meta data.
        @param fWeight Current weight of the meta data object.
        @return true if the value was successfully retrieved.
    */
    bool Get(const NiFixedString& kKey, float& fValue, float& fWeight) const;
    
    /** 
        Retrieve string valued meta data stored within this object, specified
        by the given key.
        
        This method will fail if no meta data exists by the 
        given key, or it is of the wrong type.

        @param kKey Key corresponding to the meta data to be set retrieved.
        @param kValue Variable within which to place the value of the 
            meta data.
        @param fWeight Current weight of the meta data object.
        @return true if the value was successfully retrieved.
    */
    bool Get(const NiFixedString& kKey, NiFixedString& kValue, 
        float& fWeight) const;
  
    /// @name Key Removal
    //@{

    /** 
        Remove the value stored for a key.
        
        @param kKey the key to remove.
        @return true if the value was found and removed, false otherwise.
     */
    inline bool RemoveKey(const NiFixedString &kKey);

    /// Remove all keys stored by this metadata object.
    inline void RemoveAllKeys();

    /**
        Function that checks if a given key is present in the meta data.
        It returns true if a key was found and false otherwise.
    */
    inline bool HasKey(const NiFixedString& kMetaDataKey);
    //@}

    /// @name Key Information
    //@{

    /** 
        Retrieve the expected type for a particular key.

        @note This is a delegate function to the MetaDataStore object 
            referenced by this MetaData object.
        
        @param kKey the key to retrieve the type for
        @param kKeyType the variable to store the type inside.
        @return true if the type was successfully retrieved.
     */
    bool GetKeyType(const NiFixedString& kKey, KeyType& kKeyType) const;
    //@}

    /// @name Iteration
    //@{

    /** 
        Populate an ObjectSet with the sting names of all the keys stored 
        in this metadata object.
        
        This function is useful for iteration of all the data stored in this
        object.
               
        @param kKeyNames The list with which to insert the current keys.
     */
    void GetKeys(NiTObjectSet<NiFixedString>& kKeyNames) const;

    /** 
        Populate an ObjectSet with the string names of all the keys that have
        been assigned types in the parent MetaDataStore object. 
        
        @param kKeyNames the list to add the key names to
        @note This is a delegate function to the MetaDataStore object.
     */
    void GetAvailableKeys(NiTObjectSet<NiFixedString>& kKeyNames) const;
    //@}

    /// @name Serialization
    //@{

    /** 
        Force an iteration over all the values stored in this object and 
        cross-reference their types with those expected by the MetaDataStore. 
        
        Any keys with discrepancies shall be removed from the MetaData. Any 
        keys that were removed will have their names added to the kKeyNames 
        list.
        
        @param kKeyNames A list to be populated with the keys that are removed.
        @return True if no discrepancies were found.
     */
    bool ValidateEntries(NiTObjectSet<NiFixedString>& kKeyNames);

    /** 
        Force an iteration over all the values stored in this object and 
        cross-reference their types with those expected by the MetaDataStore.
         
        Any keys that should be removed will have their names added to the 
        kKeyNames list.        
       
        @param kKeyNames a list to be populated with the keys that should
            be removed.
        @return true if no discrepancies were found
        @note This version of the function does not remove invalid entries.
     */
    bool ValidateEntries(NiTObjectSet<NiFixedString>& kKeyNames) const;

    /// Set the weights of all the meta-data values.
    ///
    /// @param fWeight New weight.
    void UpdateWeights(float fWeight) const;

    /**
        Returns the weight of the Meta Data
    */
    float GetWeight() const;
    
    /**
        Blend this metadata object with another metadata object.
        
        Any keys that do not already exist will be added. 
        Any keys that already exist will be blended according to
        their type. INTEGER_BLENDED and FLOAT_BLENDED will be
        blended together using the weighting provided. For all
        other types the value will remain the value of the 
        metadata with the highest weight.
        
        @param pkMetaData A pointer to the metadata to blend with.
        @param fWeight The weighting of this metadata's values. 
     */
    void Blend(const NiMetaData* pkMetaData, float fWeight);   
     
    /// Save the data in this MetaData object into a DOM format.    
    void Save(NiDOMTool& kDom) const;

    /// Load the date stored in the NiDOMTool into this MetaData object.
    ///
    /// Any existing entries in this object shall be removed.
    void Load(NiDOMTool& kDom);
    //@}

private:

    /// Pointer to the store object overseeing the data types
    NiMetaDataStore* m_pkMetaDataStore;

    /// A StringMap mapping keys to their values
    NiTStringPointerMap<MetaDataValue*>* m_pkValueMap;
};

#include "NiMetaData.inl"

#endif // NIMETADATA_H
