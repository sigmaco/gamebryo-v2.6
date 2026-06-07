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

#ifndef ASSOCIATEFILETYPES_H
#define ASSOCIATEFILETYPES_H

class AssociateFileTypes
{
public:
    static bool Associate(const char* pcExtension, const char* pcApplication, 
        const char* pcOptions, const char* pcIdentifier, 
        const char* pcDescription, const char* pcIcon);

private:
    // Don't allow this class to be instantiated since it is a container
    // for static methods
    AssociateFileTypes();

    // Simple class for getting/setting registry values
    class RegistryKey
    {
    public:
        RegistryKey();
        ~RegistryKey();

        operator HKEY&();

        bool IsValid();
        bool GetKey(HKEY kParent, const char* pcSubKey);
        bool SetValue(const char* pcName, const char* pcValue);

    private: 
        RegistryKey(const RegistryKey&); 
        HKEY m_kKey;
    };
};

#endif  // #ifndef ASSOCIATEFILETYPES_H
