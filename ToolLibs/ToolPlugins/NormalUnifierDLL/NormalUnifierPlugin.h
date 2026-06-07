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

#ifndef NORMALUNIFIERPLUGIN_H
#define NORMALUNIFIERPLUGIN_H

#include "NiTPointerList.h"

//-----------------------------------------------------------------------
class NiUnifyHashFunctor : public NiMemObject
{
public:
    static unsigned int KeyToHashIndex(
        NiPoint3* key, unsigned int uiTableSize);
};
//-----------------------------------------------------------------------
class NiUnifyEqualsFunctor : public NiMemObject
{
public:
    static bool IsKeysEqual(NiPoint3* key1, NiPoint3* key2);
    static void SetTolerance(const float fTolerance);
    
    static float m_fTolerance;
};
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
    /// This is our Remove Hidden Objects process plug-in. It is used to
    /// examine art during export and remove any objects which are app culled.
class NormalUnifierPlugin : public NiPlugin
{
public:
    /// RTTI declaration macro.
    NiDeclareRTTI;

    /// Default constructor.
    NormalUnifierPlugin();

    //-----------------------------------------------------------------------
    // Required virtual function overrides from NiPlugin.
    //-----------------------------------------------------------------------
    /// Returns a plug-in script with default parameters.
    virtual NiPluginInfo* GetDefaultPluginInfo(); 

    /// Does this plug-in have editable options?
    virtual bool HasManagementDialog();
    
    /// Bring up the options dialog and handle user interaction.
    virtual bool DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent);

    /// Determines whether or not this plug-in can handle this plug-in info
    /// object.
    virtual bool CanExecute(NiPluginInfo* pkInfo);
    
    /// Execute the NiPluginInfo script.
    virtual NiExecutionResultPtr Execute(const NiPluginInfo* pkInfo);
    
protected:
    class VertexCluster : public NiMemObject
    {
    public:
        struct Reference {
            NiMesh* m_pkMesh;
            NiUInt32 m_uiIndex;
        };
        
        VertexCluster();
        ~VertexCluster();
        
        void AddVertex(NiMesh* pkMesh, NiUInt32 uiIndex,
            NiPoint3& kNormal);
        
        NiTPrimitiveArray<Reference*> m_kReferences;
        NiPoint3 m_kNormal;
    };    
    
    void CollectMesh(NiAVObject* pkObject, bool bAutoAdd = false);

    void PrepareVertexMap();

    void CollectVertices();
    const NiFixedString& GetNormalSemantic(NiMesh* pkMesh);
    const NiFixedString& GetPositionSemantic(NiMesh* pkMesh);
    void AddVertex(NiMesh* pkMesh, NiUInt32 uiIndex, NiPoint3& kPoint,
        NiPoint3& kNormal);

    void ProcessClusters();
    void UnifyNormals(VertexCluster* pkCluster);

    void CleanMap();

    bool CheckForExtraDataTags(NiAVObject* pkObject);
    bool CheckForExtraDataTags(NiAVObject* pkObject, const char* ppcTags[], 
        NiUInt32 uiNumTags);

    // Array of meshes on which we are unifying normals
    NiTPrimitiveArray<NiMesh*> m_kMeshArray;

    // Map of all positions with info to find their matching normals
    NiTMap<NiPoint3*, VertexCluster*, NiUnifyHashFunctor,
        NiUnifyEqualsFunctor>* m_pkVertexMap;
};

#endif  // #ifndef NORMALUNIFIERPLUGIN_H
