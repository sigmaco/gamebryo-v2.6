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

#ifndef NIPLUGININFO_H
#define NIPLUGININFO_H

#include <NiRefObject.h>
#include "NiPluginToolkitLibType.h"
#include "NiPluginToolkitDefinitions.h"
#include <NiRTTI.h>
#include <NiSmartPointer.h>
#include <NiTArray.h>
#include "NiParameterInfo.h"

/// The container for all script-relevant information about a specific plugin.
class NIPLUGINTOOLKIT_ENTRY NiPluginInfo : public NiRefObject
{
public:
    NiDeclareRootRTTI(NiPluginInfo);

    /// Virtual destructor.
    inline virtual ~NiPluginInfo(){};

    /// Gets the name of the plugin.
    const NiString& GetName() const;
    
    /// Gets the version of the plugin.
    const NiString& GetVersion() const;
    
    /// Gets the type of the plugin.
    const NiString& GetType() const;
    
    /// Gets the class name of the plugin.
    const NiString& GetClassName() const;
    
    /// Gets the number of specified parameters.
    unsigned int GetParameterCount() const;

    /// Gets the index of the ParameterInfo object pertaining to a specific 
    /// key starting from an optional index.
    unsigned int GetParameterIndex(const char* pcKey,
        unsigned int uiStartIdx = 0) const;
    
    /// Get the ParameterInfo object based off the index in the internal
    /// parameter array.
    NiParameterInfo* GetParameterAt(unsigned int ui) const;

    /// Get the value string for the input key
    NiString GetValue(const char* pcKey, unsigned int uiStartIdx = 0) const;
    bool GetBool(const char* pcKey, unsigned int uiStartIdx = 0) const;
    int GetInt(const char* pcKey, unsigned int uiStartIdx = 0) const;
    float GetFloat(const char* pcKey, unsigned int uiStartIdx = 0) const;

    /// Set the value string for the input key
    bool SetValue(const char* pcKey, const char* pcValue, 
        unsigned int uiStartIdx = 0);
    bool SetBool(const char* pcKey, bool bValue, unsigned int uiStartIdx = 0);
    bool SetInt(const char* pcKey, int iValue, unsigned int uiStartIdx = 0);
    bool SetFloat(const char* pcKey, float fValue, 
        unsigned int uiStartIdx = 0);

    /// Set the name of this plugin
    void SetName(const char* pcPluginName);

    /// Set the version # of the plugin
    void SetVersion(const char* pcPluginVersion);

    /// Set the type of this plugin
    void SetType(const char* pcPluginType);

    /// Set the classname of this plugin
    void SetClassName(const char* pcClassName);

    /// Add a parameter to this plugin
    void AddParameter(NiParameterInfo* pkParam);

    /// Add a parameter by key/value pair
    void AddParameter(const char* pcKey, const char* pcValue);

    /// Remove a parameter based off the index in the internal parameter array
    void RemoveParameterAt(unsigned int ui);

    /// Remove a parameter based off of its key string
    void RemoveParameter(const char* pcKey);

    /// Remove all parameters
    void RemoveAllParameters();

    /// Make a deep copy of the the plugin info
    virtual NiPluginInfo* Clone(NiPluginInfo* pkClone = NULL);

    /// Does the input plugin equal the current plugin
    virtual bool Equals(NiPluginInfo* pkInfo);

    /// Has this plugininfo been altered
    virtual bool IsDirty();

    /// Set the dirty bit for this object
    virtual void MarkAsDirty(bool bDirty);

protected:
    /// The name of the plugin.
    NiString m_strPluginName;
    
    /// The version of the plugin.
    NiString m_strPluginVersion;
    
    /// The type of the plugin.
    NiString m_strPluginType;
    
    /// The class id of the plugin.
    NiString m_strClassName;
    
    /// The array of Key/Value parameters.
    NiTObjectArray<NiParameterInfoPtr> m_kParameters;

    /// Is this plugin dirty?
    bool m_bDirty;

};

NiSmartPointer(NiPluginInfo);

#endif  // #ifndef NIPLUGININFO_H
