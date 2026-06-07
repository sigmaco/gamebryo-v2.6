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

#ifndef MULTINIFEXPORTPLUGINDEFINES_H
#define MULTINIFEXPORTPLUGINDEFINES_H

/// The following definitions are for constant character strings
/// used by SampleExportPlugin, its corresponding NiPluginInfo object,
/// and its management NiDialog. Using #defines is useful in avoiding
/// string typo mistakes that can be costly to track down when debugging.
#define MULTI_NIF_EXPROT_USE_OBJECT_NAME_AS_FILE_NAME \
    "MultiNifExportUseObjectNameAsFileName"
#define MULTI_NIF_EXPORT_DELETE_AFTER_EXPORT \
    "MultiNifExportDeleteAfterExport"
#define MULTI_NIF_EXPORT_TRANSLATE_TYPE \
    "MultiNifExportTranformType"
#define MULTI_NIF_EXPORT_ROTATE_TYPE \
    "MultiNifExportRotateType"
#define MULTI_NIF_EXPORT_SCALE_TYPE \
    "MultiNifExportScaleType"
#define MULTI_NIF_EXPORT_REPAIR_EMITTER_WITHOUT_PARTICLES \
    "MultiNifExportRepairEmitterWithoutParticles"

enum
{
    MULTI_NIF_EXPORT_KEEP_WORLD_TRANFORM,
    MULTI_NIF_EXPORT_KEEP_LOCAL_TRANFORM,
    MULTI_NIF_EXPORT_CLEAR_WORLD_TRANFORM
};

/// The following are definitions for constant strings the plug-in will 
/// use to define NiExtraData names it is expecting.
#define MULTI_NIF_EXPORT_EXTRA_DATA_USE_GLOBAL_SETTINGS \
    "MultiNifExportUseGlobalSettings"
#define MULTI_NIF_EXPORT_EXTRA_DATA_FILE_NAME \
    "MultiNifExportFileName"
#define MULTI_NIF_EXPORT_EXTRA_DATA_USE_OBJECT_NAME \
    "MultiNifExportUseObjectName"
#define MULTI_NIF_EXPORT_EXTRA_DATA_TRANSLATE_TYPE \
    "MultiNifExportTranslateType"
#define MULTI_NIF_EXPORT_EXTRA_DATA_ROTATE_TYPE \
    "MultiNifExportRotateType"
#define MULTI_NIF_EXPORT_EXTRA_DATA_SCALE_TYPE \
    "MultiNifExportScaleType"
#define MULTI_NIF_EXPORT_EXTRA_DATA_DELETE_AFTER_EXPORT \
    "MultiNifExportDeleteAfterExport"
#define MULTI_NIF_EXPORT_EXTRA_DATA_REPAIR_EMITTER_WITHOUT_PARTICLES \
    "MultiNifExportRepairEmitterWithoutParticles"

#endif  // #ifndef MULTINIFEXPORTPLUGINDEFINES_H
