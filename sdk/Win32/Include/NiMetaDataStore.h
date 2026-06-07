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

#ifndef NIMETADATASTORE_H
#define NIMETADATASTORE_H

#include <NiTPointerMap.h>

#include "NiTerrainLibType.h"
#include "NiMetaData.h"

class NiFixedString;

/** 
    The metadata store is used to map key names to data-types. Use this class
    to manage the data-types for a group of metadata objects within a 
    particular scope. For example, A MetaDataStore at the Terrain level to 
    manage data-types for all MetaData objects used in the Terrain system.
 */
class NITERRAIN_ENTRY NiMetaDataStore : public NiMemObject
{
public:
    /// Convenience type for key types.
    typedef NiMetaData::KeyType KeyType;

public:

    /// Default constructor
    NiMetaDataStore();

    /// Destructor
    ~NiMetaDataStore();

    /// Retrieve a pointer to the default singleton meta data store.
    ///
    /// This  function will create a store if none exists so far.
    static NiMetaDataStore* GetInstance();    

    /// @name Key Data Types
    //@{
    /** 
        Map a KeyName to a particular KeyType.
        
        NiMetaData objects refering to this store will use this mapping to
        determine what data should be stored for this key.        

        @param kKey The key name to be mapped.
        @param kType The type associated with the given key
    */
    void SetKeyType(const NiFixedString& kKey, KeyType kType);

    /** 
        Get the KeyType for a particular KeyName.
        
        @param kKey The key name to get the type for.
        @param kType The type associated with the given key
    */
    bool GetKeyType(const NiFixedString& kKey, KeyType& kType) const;

    /** 
        Remove the key name to key type mapping from the store. 
        
        @note This function will iterate through all MetaData objects 
            referencing this store and remove any references to this key

        @param kKey The key to remove.
        @return true if a valid key name to key type mapping was found and 
            removed.
     */
    bool RemoveKey(const NiFixedString& kKey);

    /** 
        Remove all type mappings from this MetaDataStore. 
        
        @note This function will iterate through all MetaData objects 
            referencing this store and remove any references to these keys.
     */
    void RemoveAll();
    //@}
    
    /// @name Manage MetaData Instances
    //@{
    /** 
        Notify this store object of a metadata object that is referencing it
            for type information.        
        @param kMetaData a pointer to the metadata object referencing the store
     */
    void RegisterMetaData(NiMetaData* kMetaData);

    /** 
        Notify this store object of a metadata object that is no longer
            maintaining a reference to it.        

        @param kMetaData Metadata object to unregister.
     */
    void UnregisterMetaData(NiMetaData* kMetaData);

    /** 
        Remove all the key entries from all MetaData objects referencing this
            store. 
        
        @param kKey The name of the key to clear.
     */
    void ClearKey(const NiFixedString& kKey);

    /** 
        Force all the MetaData objects to validate the data stored in them. 
        
        Each metadata will check the keys and make sure that the right 
        type of data is stored for the key. If the key does not have valid 
        data stored, then that entry will be removed.
        
        @return false if an invalid entry was found in the process.
     */
    bool ValidateKeys();

    /** 
        Force all the MetaData objects to validate the data stored in them. 
        
        Each metadata will check the keys and make sure that the right 
        type of data is stored for the key. 
        
        @return false if an invalid entry was found in the process.
     */
    bool ValidateKeys() const;
    //@}
  
    /** 
        Create a list of key names for which this store has type mappings 
        assigned
        
        @param kKeySet The set to which the key names shall be added.
     */
    void GetAvailableKeys(NiTObjectSet<NiFixedString>& kKeySet) const;

    /// @cond EMERGENT_INTERNAL
 
    /// Creates the singleton instance of the data store.
    static void CreateInstance();

    /// Destroys the singleton instance of the data store.
    static void DestroyInstance();    

    /// @endcond

private:
    /// Set of MetaData objects using this MetaDataStore
    NiTPrimitiveSet<NiMetaData*> m_kMetaData;

    /// Mappings from key name to key type.
    NiTStringMap<int> m_kTypeMap;

    static NiMetaDataStore* ms_pkSingleton;
};

#endif // NIMETADATASTORE_H
