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

#ifndef MIVIEWERLOADERDIALOG_H 
#define MIVIEWERLOADERDIALOG_H
#include <windows.h>

class MiViewerLoaderDialog
{
    public:
        MiViewerLoaderDialog(HINSTANCE hParent, char* pcTitle);
        ~MiViewerLoaderDialog();

        void EndWindow();
        void UpdateLoadingString(char* pcString);
        void UpdateSpecificString(char* pcString);
        void StepIt();
        void SetRangeSpan(unsigned int uiSpan);
        void SetPosition(unsigned int uiPos);

    protected:
        HWND hWnd;
        HWND hwndPB;
        float m_fPercentageComplete;
        char* m_pcLoadingString;
        float m_fPercentage;
        
};

#endif