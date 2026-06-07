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

#ifndef NISTREAMOUTSETTINGS_H
#define NISTREAMOUTSETTINGS_H

#include "NiRefObject.h"
#include "NiTArray.h"
#include "NiSmartPointer.h"
#include "NiFixedString.h"

class NiBinaryStream;

class NIMAIN_ENTRY NiStreamOutSettings : public NiRefObject
{
public:
    NiStreamOutSettings();
    virtual ~NiStreamOutSettings();

    NiStreamOutSettings& operator=(
        const NiStreamOutSettings& kStreamOutSettings);

    inline void SetStreamOutAppend(bool bStreamOutAppend);
    inline bool GetStreamOutAppend() const;

    inline void SetStreamOutTargets(
        const NiTObjectArray<NiFixedString>& kStreamOutTargets);
    inline void ClearStreamOutTargets();
    inline void AppendStreamOutTargets(const char* pcStreamOutTarget);
        
    inline unsigned int GetStreamOutTargetCount() const;
    inline const NiFixedString& GetStreamOutTarget(int uiIndex) const;

    // Streaming support
    bool LoadBinary(NiBinaryStream& kStream);
    bool SaveBinary(NiBinaryStream& kStream) const;

private:
    NiTObjectArray<NiFixedString> m_kStreamOutTargets;
    bool m_bStreamOutAppend;
};

typedef NiPointer<NiStreamOutSettings> NiStreamOutSettingsPtr;

#include "NiStreamOutSettings.inl"

#endif  //NISTREAMOUTSETTINGS_H
