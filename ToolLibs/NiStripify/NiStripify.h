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

#ifndef NISTRIPIFY_H
#define NISTRIPIFY_H

#include "NiStripifyLibType.h"
#include <NiObjectNET.h>
#include <NiMemObject.h>
#include <NiTSet.h>
#include <NiTMap.h>
#include <NiDataStreamRef.h>

class NiAVObject;
class NiNode;
class NiMesh;
class NiDataStreamPrimitiveLock;
class NiBoneLODController;
class NiRoom;

class NISTRIPIFY_ENTRY NiStripify : public NiMemObject
{
public:

    // Used for gathering statistics.    
    class Stats : public NiMemObject
    {
    public:
        Stats();
        void Reset();

        // Number of NiMesh objects that were converted. Note, the OkayCallback
        // can be used to causes certain meshes to not be converted.
        unsigned int m_uiConvertedMeshes;

        // Sum of the triangle counts for the NiMesh objects that were
        // converted.
        unsigned int m_uiTriangles;

        unsigned int m_uiStripsTriangles;

        // The sum of the number of strips in the resulting converted objects.
        unsigned int m_uiStrips;        
    };    

    static void Stripify(NiNode* pkNode, unsigned int uiCacheSize);    

    // callback to indicate whether an object should be converted to strips
    typedef bool (*OkayCallback)(NiAVObject* pkObject, void* pvData);    
    static void SetOkayCallback(OkayCallback pfnCallback, void* pvData);
    static void GetOkayCallback(OkayCallback& pfnCallback, void*& pvData);

    // callback to indicate that an object has been converted to strips
    typedef bool (*FinishedCallback)(NiAVObject* pkObject);    
    static void SetFinishedCallback(FinishedCallback pfnCallback);
    static void GetFinishedCallback(FinishedCallback& pfnCallback);

    // Callback to indicate that an object has caused an error in
    // stripification.  This can be ignored and errors will simply
    // fail silently and some stripification may not occur.
    enum ErrorCode
    {
        STRIPPED_TRISHAPE_TOO_LARGE,
        STRIPPED_PARTITION_TOO_LARGE
    };
    typedef void (*ErrorCallback)(unsigned int uiCode, NiObject* pkObj,
        const char* pcErrorString);
    static void SetErrorCallback(ErrorCallback pfnCallback);
    static void GetErrorCallback(ErrorCallback& pfnCallback);    

    static void GetStats(Stats& kStats);
    static void GetnVStats(Stats& kStats);
    
    // Reinitialize all the values returned by GetStats to 0.
    static void ResetStats();
    static void ResetnVStats();

    static void ResetIndexSum(void);
    static unsigned int GetIndexSum(void);

    static void SetUse_NextGeneration(bool bUse_NextGeneration);
    
    static void Init();
    static void Shutdown();

protected:
    static bool NoStripifyExtraDataIsNotPresent(NiObjectNET* pkObject);

    // Map that holds a list of data streams that have already been converted
    // so cloned/shared data streams are not processed twice.
    typedef NiTPointerMap<NiDataStream*, NiDataStream*> DataStreamMap;

    static void Stripify(NiNode* pkNode, unsigned int uiCacheSize, 
        DataStreamMap& kMeshMap);
    static bool Stripify(NiMesh* pkMesh, unsigned int uiCacheSize, 
        DataStreamMap& kMeshMap);
    static bool Stripify(NiMesh* pkMesh, unsigned int uiCacheSize, 
        DataStreamMap& kMeshMap, NiDataStreamRef* pkIndexBuffer);
    static bool ConvertClone(NiMesh* pkMesh, DataStreamMap& kMeshMap);
    static bool RemapVertices(NiMesh* pkMesh, DataStreamMap& kMeshMap,
        NiUInt32** ppuiVertexRemappings);

    static FinishedCallback ms_pfnFinishedCallback;
    static OkayCallback ms_pfnOkayCallback;
    static ErrorCallback ms_pfnErrorCallback;
    static void* ms_pvCallbackData;
    static Stats ms_kStats;
    static Stats ms_knVStats;

    static unsigned int ms_uiIndexSum;

    // bone LOD controller static functions and variables
    //static NiTPrimitiveSet<NiBoneLODController*>* ms_pkBoneLODSet;
    //static void BuildBoneLODSet(NiAVObject* pkObject);
    //static void UpdateBoneLODSet(NiTriShape* pkShape, NiTriStrips* pkStrips);
    //static bool CheckForBoneLOD(NiTriBasedGeom* pkGeom);
    //static void StripifyBoneLODPartitions();
};

#endif // NISTRIPIFY_H
