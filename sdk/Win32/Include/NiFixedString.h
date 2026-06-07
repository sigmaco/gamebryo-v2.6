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

#ifndef NIFIXEDSTRINGTABLE_H
#define NIFIXEDSTRINGTABLE_H

#include "NiRTLib.h"
#include "NiGlobalStringTable.h"
#include "NiPath.h"
#include "NiBinaryLoadSave.h"

class NIMAIN_ENTRY NiFixedString : public NiMemObject
{
public:
    NiFixedString();
    NiFixedString(const char* pcString);
    NiFixedString(const NiFixedString& kString);
    ~NiFixedString();

    operator const char*() const;

    inline bool Exists() const;

    inline NiFixedString& operator=(const NiFixedString& kString);
    inline NiFixedString& operator=(const char* pcString);

    inline size_t GetLength() const;
    inline unsigned int GetRefCount() const;

    inline bool Equals(const char* pcStr) const;
    inline bool EqualsNoCase(const char* pcStr) const;

    inline bool Contains(const char* pcStr) const;
    inline bool ContainsNoCase(const char* pcStr) const;

    friend bool operator==(const NiFixedString& s1, const NiFixedString& s2);
    friend bool operator!=(const NiFixedString& s1, const NiFixedString& s2);
   
    friend bool operator==(const NiFixedString& s1, const char* s2);
    friend bool operator!=(const NiFixedString& s1, const char* s2);
   
    friend bool operator==(const char* s1, const NiFixedString& s2);
    friend bool operator!=(const char* s1, const NiFixedString& s2);
   
    // *** begin Emergent internal use only ***
    static void LoadCStringAsFixedString(NiBinaryStream& kStream,
        NiFixedString& kString);
    static void SaveFixedStringAsCString(NiBinaryStream& kStream,
        const NiFixedString& kString);
    // *** end Emergent internal use only ***
protected:
    NiGlobalStringTable::GlobalStringHandle m_kHandle;
};

NIMAIN_ENTRY void NiStandardizeFilePath(NiFixedString& kString);

#include "NiFixedString.inl"

#endif
