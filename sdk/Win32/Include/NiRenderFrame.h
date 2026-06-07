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

#ifndef NIRENDERFRAME_H
#define NIRENDERFRAME_H

#include "NiTPointerList.h"
#include "NiFixedString.h"
#include "NiRenderStep.h"

class NIMAIN_ENTRY NiRenderFrame : public NiRefObject
{
public:
    NiRenderFrame();

    // Functions for accessing the name of the render frame.
    inline void SetName(const NiFixedString& kName);
    inline const NiFixedString& GetName() const;

    // Primary rendering functionality.
    void Draw();
    void Display();

    // Functions for accessing the list of render steps.
    inline void AppendRenderStep(NiRenderStep* pkRenderStep);
    inline void PrependRenderStep(NiRenderStep* pkRenderStep);
    inline void RemoveRenderStep(NiRenderStep* pkRenderStep);
    inline void RemoveAllRenderSteps();
    inline NiTPointerList<NiRenderStepPtr>& GetRenderSteps();
    inline const NiTPointerList<NiRenderStepPtr>& GetRenderSteps() const;
    inline NiRenderStep* GetRenderStepByName(const NiFixedString& kName) const;
    inline NiTListIterator GetRenderStepPosByName(const NiFixedString& kName) const;
    inline NiRenderStep* GetRenderStepAndPosByName(
        const NiFixedString& kName, NiTListIterator& kIter) const;

    // Function for gathering statistics about the most recent frame.
    inline void GatherStatistics(unsigned int& uiNumObjectsDrawn, float& fCullTime,
        float& fRenderTime) const;

    // Callback definition.
    typedef bool (*Callback)(NiRenderFrame* pkCurrentFrame,
        void* pvCallbackData);

    // Pre-processing callback.
    inline void SetPreProcessingCallbackFunc(Callback pfnCallback,
        void* pvCallbackData = NULL);
    Callback GetPreProcessingCallbackFunc() const;
    inline void* GetPreProcessingCallbackFuncData() const;

    // Post-processing callback.
    inline void SetPostProcessingCallbackFunc(Callback pfnCallback,
        void* pvCallbackData = NULL);
    Callback GetPostProcessingCallbackFunc() const;
    inline void* GetPostProcessingCallbackFuncData() const;

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    // *** end Emergent internal use only ***

protected:
    // A list of render steps.
    NiTPointerList<NiRenderStepPtr> m_kRenderSteps;

    // Callback functions.
    Callback m_pfnPreProcessingCallback;
    Callback m_pfnPostProcessingCallback;

    // Callback data.
    void* m_pvPreProcessingCallbackData;
    void* m_pvPostProcessingCallbackData;

    // The name of the render frame.
    NiFixedString m_kName;

    // Default name for all render frames.
    static NiFixedString ms_kDefaultName;
};

NiSmartPointer(NiRenderFrame);

#include "NiRenderFrame.inl"

#endif  // #ifndef NIRENDERFRAME_H
