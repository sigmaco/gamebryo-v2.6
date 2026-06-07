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

#ifndef NICONTROLLEREXTRACTORSHAREDDATA_H
#define NICONTROLLEREXTRACTORSHAREDDATA_H

#include "NiStandardSharedDataLibType.h"
#include "NiSharedData.h"
#include <NiControllerExtractor.h>

/// This shared data class contains information necessary for the
/// controller extractor plug-in.
class NISTANDARDSHAREDDATA_ENTRY NiControllerExtractorSharedData :
    public NiSharedData
{
public:
    NiDeclareRTTI;

    /// Default constructor.
    NiControllerExtractorSharedData();

    /// Destructor.
    ~NiControllerExtractorSharedData();

    /// Get the SequenceInfo array.
    NiControllerExtractor::InfoArray& GetInfoArray();

    /// Add a SequenceInfo object to the array.
    void AddInfo(NiControllerExtractor::SequenceInfo* pkInfo);

    /// Get a NiCCAGlobalInfo object to the array
    NiControllerExtractor::NiCCAGlobalInfoArray& GetCCAGlobalInfoArray();

    /// Add a NiCCAGlobalInfo object to the array
    void AddCCAGlobalInfo(NiControllerExtractor::NiCCAGlobalInfo* pkInfo);

    /// Get a NiCCAInfo object to the array
    NiControllerExtractor::NiCCAInfoArray& GetCCAOverrideInfoArray();

    // Add a NiCCAInfo object to the override array
    void NiAddCCAOverrideInfo(NiControllerExtractor::NiCCAInfo* pkInfo);

protected:
    /// The array of sequence info objects.
    NiControllerExtractor::InfoArray m_kInfoArray;

    /// The array of Character Compress Animation Global Values
    NiControllerExtractor::NiCCAGlobalInfoArray m_kCCAGlobalArray;

    /// The array of Character Compress Animation override values
    NiControllerExtractor::NiCCAInfoArray m_kCCAOverrideArray;
};

NiSmartPointer(NiControllerExtractorSharedData);

#endif  // #ifndef NICONTROLLEREXTRACTORSHAREDDATA_H
