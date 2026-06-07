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

#ifndef NIFILENAME_H
#define NIFILENAME_H

#include "NiRTLib.h"
#include "NiSystem.h"
#include "NiSystemLibType.h"

#ifndef _MAX_DIR
#define _MAX_DIR 256
#endif
#ifndef _MAX_DRIVE
#define _MAX_DRIVE 3
#endif
#ifndef _MAX_EXT
#define _MAX_EXT 256
#endif
#ifndef _MAX_FNAME
#define _MAX_FNAME 256
#endif

class NISYSTEM_ENTRY NiFilename : public NiMemObject
{
public:
    NiFilename(const char* pcFullPath);

    inline void SetDir(const char* pcDir);
    inline void SetDrive(const char* pcDrive);
    inline void SetExt(const char* pcExt);
    inline void SetFilename(const char* pcFname);
    inline void SetPlatformSubDir(const char* pcSubDir);

    inline const char* GetDir() const;
    inline const char* GetDrive() const;
    inline const char* GetExt() const;
    inline const char* GetFilename() const;
    inline const char* GetPlatformSubDir() const;

    bool GetFullPath(char* pcFullPath, unsigned int uiStrLen) const;

private:
    void Splitpath(const char* pcStr);
    bool Makepath(char* pcStr, size_t stStrLen) const;

    char m_acDir[_MAX_DIR]; 
    char m_acDrive[_MAX_DRIVE]; 
    char m_acExt[_MAX_EXT];
    char m_acFname[_MAX_FNAME]; 
    char m_acSubDir[_MAX_DIR]; 
};

//---------------------------------------------------------------------------
inline void NiFilename::SetDir(const char* pcDir)
{
    NiStrcpy(m_acDir, _MAX_DIR, pcDir);
}
//---------------------------------------------------------------------------
inline void NiFilename::SetDrive(const char* pcDrive)
{
    NiStrcpy(m_acDrive, _MAX_DRIVE, pcDrive);
}
//---------------------------------------------------------------------------
inline void NiFilename::SetExt(const char* pcExt)
{
    NiStrcpy(m_acExt, _MAX_EXT, pcExt);
}
//---------------------------------------------------------------------------
inline void NiFilename::SetFilename(const char* pcFname)
{
    NiStrcpy(m_acFname, _MAX_FNAME, pcFname);
}
//---------------------------------------------------------------------------
inline void NiFilename::SetPlatformSubDir(const char* pcSubDir)
{
    NiStrcpy(m_acSubDir, _MAX_DIR, pcSubDir);
}
//---------------------------------------------------------------------------
inline const char* NiFilename::GetDir() const
{
    return m_acDir;
}
//---------------------------------------------------------------------------
inline const char* NiFilename::GetDrive() const
{
    return m_acDrive;
}
//---------------------------------------------------------------------------
inline const char* NiFilename::GetExt() const
{
    return m_acExt;
}
//---------------------------------------------------------------------------
inline const char* NiFilename::GetFilename() const
{
    return m_acFname;
}
//---------------------------------------------------------------------------
inline const char* NiFilename::GetPlatformSubDir() const
{
    return m_acSubDir;
}
//---------------------------------------------------------------------------

#endif // NIFILENAME_H
