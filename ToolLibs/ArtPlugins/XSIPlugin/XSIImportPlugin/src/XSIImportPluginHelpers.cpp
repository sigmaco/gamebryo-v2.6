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

#include "XSIImportPluginHelpers.h"

#include "NiSharedDataList.h"
#include "NiExporterOptionsSharedData.h"

namespace epg
{
    //---------------------------------------------------------------------------
    // Try to find the XML logger in the shared data.
    //---------------------------------------------------------------------------
    NiXMLLogger* FindXMLLogger(NiSharedDataList* pkDataList)
    {
        NiXMLLogger* logger = 0;

        pkDataList->Lock();
        unsigned int count = pkDataList->GetSharedDataCount();
        for (unsigned int i = 0; i < count; ++i)
        {
            NiSharedData* data = pkDataList->GetSharedDataAt(i);
            NiExporterOptionsSharedDataPtr pkExportData =
                NiDynamicCast(NiExporterOptionsSharedData, data);
            if (pkExportData)
            {
                logger = pkExportData->GetXMLLogger();
                if (logger)
                {
                    break;
                }
            }
        }
        pkDataList->Unlock();

        return logger;
    }
}
