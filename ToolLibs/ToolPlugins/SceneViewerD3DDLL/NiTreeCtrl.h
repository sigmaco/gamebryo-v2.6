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

#ifndef NITREECTRL_H
#define NITREECTRL_H

#include <windows.h>
#include <commctrl.h>

class NiAVObject;
class NiObject;

class NiTreeCtrl : public NiMemObject
{
public:
    NiTreeCtrl(HINSTANCE hInstance, HWND hParentWnd, NiAVObject* pkObject,
        char* pcTitle, int iXPos,  int iYPos, int iWidth, int iHeight);

    ~NiTreeCtrl();

    void OnIdle();

protected:
    void CreateWindows(int iXPos, int iYPos, int iWidth, int iHeight);
    void CreateImageList();
    void CreateTree();
    void CreateTreeRecursive(HTREEITEM hParent, NiAVObject* pkObject);
    void CreateTree(HTREEITEM hParent, NiMesh* pkData);
    void AddViewerStrings(NiObject* pkObject, TV_INSERTSTRUCT& tvs);

    void DestroyWindows();
    void DestroyImageList();
    void DestroyTree();

    static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uiMsg,
        WPARAM wParam, LPARAM lParam);

    HINSTANCE m_hInstance;
    HWND m_hParentWnd;
    static HWND m_hTreeWnd;
    static HWND m_hTreeView;
    HTREEITEM m_hTreeRoot;
    HIMAGELIST m_hImageList;

    NiAVObject* m_pkObject;
    char* m_pcName;

    enum
    {
        TI_WORLD,
        TI_CAMERA,
        TI_LIGHT,
        TI_NODE,
        TI_MESH,
        TI_ATTRIBUTE,
        TI_ATTRITEM,
        TI_MESHDATA,
        TI_NUMIMAGES
    };

    static int ms_aiImageIndex[];
    static int ms_aiImageID[];
    static char ms_acWindowClassName[];
};

#endif // NITREECTRL_H
