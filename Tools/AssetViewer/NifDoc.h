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

// NifDoc.h

#ifndef NIFDOC_H
#define NIFDOC_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NifStatisticsManager.h"
#include "NifCameraList.h"
#include "NiCollisionLabInfo.h"
#include <NiMeshScreenElements.h>

class CNifTimeManager;
class CNifAnimationInfo;
class NiCollisionGroup;

class CNifDoc : public CDocument
{
protected: // create from serialization only
    CNifDoc();
    DECLARE_DYNCREATE(CNifDoc)

// Attributes
public:
    static CNifDoc* GetDocument();
// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNifDoc)
    public:
    virtual void Serialize(CArchive& ar);
    virtual void DeleteContents();
    virtual BOOL OnNewDocument();
    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
    virtual void OnCloseDocument();
    virtual void UpdateAllViews( CView* pSender, LPARAM lHint = 0L, 
        CObject* pHint = NULL );
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CNifDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    // Thread access methods
    void Lock();
    // If you set the bSceneChanged value to true, it will broadcast
    // a NIF_SCENECHANGED update message to all views
    void UnLock(bool bSceneChanged = false);

    // Scene data accessors
    NiNode* GetSceneGraph();
    NiNode* GetDefaultNode();
    NiNode* GetRoot(unsigned int uiWhichRoot);
    unsigned int GetNumberOfRoots();
    void AddRoot(NiNode* pkRoot);
    void RemoveRoot(NiNode* pkRoot);

    // Removing files.
    void RemoveFileByName(char* pcName);
    void RemoveFile(NiNode* pkNode);

    void EnableDefaultLights(bool bEnable);
    float UpdateScene(bool bForceUpdate = false);
    bool IsAnimated(NiAVObject* pkObj);
    bool IsAvailableNow();

    unsigned int GetRootId(NiNode* pkRoot);
    unsigned int GetRecursiveRootId(NiAVObject* pkNode);

    CNifCameraList* GetCameraList();
    NiCamera* GetCamera(NiCameraInfoIndex kIndex);
    NiCameraInfo* GetCameraInfo(NiCameraInfoIndex kIndex);

    CNifTimeManager* GetTimeManager();
    float GetLastTimeUpdated();

    CNifAnimationInfo* GetAnimationInfo();

    void HighlightObject(NiAVObject* pkObject);
    bool GetSelectionParams(NiPoint3& kCenter, float& fRadius);

    NiCollisionGroup* m_pkCollisionLabGrp;
    NiCollisionLabInfo m_kCollisionLabInfo;

    void SetRendererCreated(bool bCreated);
    void SetRendererCreationFailed(bool bFailed);

    bool GetDataStreamInfo(NiDataStream* pkStream, NiUInt8& uiAccessMask);
    void SetDataStreamInfo(NiDataStream* pkStream, NiUInt8 uiAccessMask);
    void ClearDataStreamInfo();

protected:
    // Create and destroy the scene.
    void CreateScene();
    void DestroyScene();

    // Adding files.
    bool OpenNifFile(CString strPathName);
    bool AddNifFile(CString strPathName);
    NiNode* AddRootObject(NiAVObject* pkRootObject, const char* pcFilePath);
    NiNode* AddRootTextureObject(NiTexture* pkRootObject,
        const char* pcFilePath);

    // Helper functions.
    bool CreateBoundNode();
    bool FindLights(NiNode* pkNode);
    void AddDefaultLights(NiNode* pkRoot);
    void UpdateBoundShapes(float fTime);
    void ShowABVsForRoot(NiNode* pkNode, bool bShowABVs = true,
        bool bUpdate = false);
    void ShowABVs(NiAVObject* pkRoot, bool bShowABVs = true,
        bool bUpdate = false);
    void RunUpScene();

    static CNifDoc* ms_pkThis;
    CNifCameraList* m_pkCameraList;
    CNifTimeManager* m_pkTimeManager;
    CNifAnimationInfo* m_pkAnimInfo;

    bool m_bWorking;
    bool m_bRendererCreated;
    bool m_bRendererCreationFailed;
    unsigned long m_ulThreadId;
    CCriticalSection m_kCriticalSection;

    NiNodePtr m_spScene;
    NiNode* m_pkDefaultNode;
    unsigned int m_uiNumRoots;
    float m_fLastUpdateTime;

    NiAVObjectPtr m_spBoundShape;
    NiTMap<NiAVObject*, NiAVObject*> m_kBoundShapeMap;
    NiNode* m_pkBoundShapeRoot;
    NiStream m_kFileStream;

    float m_fMinStartTime;

    NiTMap<NiDataStream*, NiUInt8> m_kDataStreamToAccessMaskMap;

// Generated message map functions
protected:
    //{{AFX_MSG(CNifDoc)
    afx_msg void OnOpenFile();
    afx_msg void OnAddFile();
    afx_msg void OnRemoveFile();
    afx_msg void OnUpdateRemoveFile(CCmdUI* pCmdUI);
    afx_msg void OnPreferences();
    afx_msg void OnSetImageSubfolder();
    afx_msg void OnListOptions();
    afx_msg void OnToggleABVs();
    afx_msg void OnUpdateToggleABVs(CCmdUI* pCmdUI);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

// support for overdraw metering
public:
    NiMeshScreenElementsArray& GetScreenElementsArray();
private:
    NiMeshScreenElementsArray m_kScreenElements;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NIFDOC_H
