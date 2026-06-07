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

#ifndef NIVISUALTRACKER_H
#define NIVISUALTRACKER_H

#include <NiVisualTrackerLibType.h>
#include <NiRefObject.h>
#include <NiColor.h>
#include <NiObject.h>
#include <NiRect.h>
#include <NiMesh.h>
#include <NiPoint2.h>
#include <NiNode.h>
#include <NiPoint3.h>
#include <NiMeshScreenElements.h>

NiSmartPointer(NiTexture);


class NIVISUALTRACKER_ENTRY NiVisualTrackerBase : public NiRefObject
{
public:
    NiVisualTrackerBase(NiRect<float> kWindowRect,
        bool bShow,
        const char* pcName);
    virtual ~NiVisualTrackerBase();

    void SetShow(bool bShow);
    bool GetShow();

    virtual void Update();
    virtual void Draw();
protected:
    class ScreenText : public NiMemObject
    {
    public:
        ScreenText(unsigned int uiMaxChars,
            NiMeshScreenElementsArray* pkScreenTextures, 
            const NiColorA& kColor);
        ~ScreenText();

        // Will truncate the string to the pre-set max number of characters
        void SetString(const char* pcString);
        const char* GetString() const;

        // Works in screen space pixel coords: <0 - (m-1), 0 - (n-1)>,
        // where the screen size is (m,n), to set the position of the 
        // upper-left edge of the text.
        void SetTextOrigin(unsigned int uiX, unsigned int uiY);
        void GetTextOrigin(unsigned int& uiX, unsigned int& uiY) const;
        
        void SetScrollDown(bool bDown);
        bool GetScrollDown() const;

        void SetColor(NiColorA& kNewColor);
        const NiColorA& GetColor() const;

        void SetVisible(bool bVisible);

        short GetHeight() const;
        short GetWidth() const;
        short GetCharacterSpacing() const;

        // Sets the line length for word wrapping  This should always be 
        // less than the text size, or else truncation will occur.
        void SetMaxLineLength(unsigned int uiColumns);
        unsigned int GetMaxLineLength() const;

        // Will not append beyond the pre-set max number of characters.
        void AppendCharacter(char cChar);
        void DeleteLastCharacter();

        void RecreateText();

        static NiTexture* GetTexture();

    protected:
        void Init(unsigned int uiMaxChars, 
            NiMeshScreenElementsArray* pkScreenTextures, 
            const NiColorA& kColor);

        NiPixelData* CreateCourier16PixelData();

        NiMeshScreenElementsPtr m_spScreenElement;
        NiMeshScreenElementsArray* m_pkScreenElements;
        NiColorA m_kColor;

        unsigned int m_uiTextOriginX;
        unsigned int m_uiTextOriginY;
        
        char* m_pcString;
        unsigned int m_uiMaxChars;
        unsigned int m_uiNumChars;
        unsigned int m_uiMaxCols;

        unsigned int m_uiNumRects;
        unsigned int m_uiNumCurrentRows;
        unsigned int m_uiCurrentColumn;

        bool m_bScrollDown;

        static NiPixelDataPtr ms_spFontPixelData;
        static unsigned int ms_uiCharWidth;
        static unsigned int ms_uiCharHeight;
        static unsigned int ms_uiCharSpacingX;
        static unsigned int ms_uiCharSpacingY;
        static const unsigned int ms_uiCharBaseU;
        static const unsigned int ms_uiCharBaseV;
        static NiTexturePtr ms_spTextTexture;
        static const char* ms_pcTextImage;
        static const unsigned int ms_uiASCIIMin;
        static const unsigned int ms_uiASCIIMax;
        static unsigned int ms_uiASCIICols;
        static int ms_iCount;
    };

    NiRect<float> m_kDimensions;  
    bool m_bShow;
    NiMeshScreenElementsArray m_kScreenTextures;
    char m_acName[256];
};

NiSmartPointer(NiVisualTrackerBase);


class NIVISUALTRACKER_ENTRY NiVisualTracker : public NiVisualTrackerBase
{
    public:
        NiVisualTracker(float fMaxValue, unsigned int uiNumDecimalPlaces,
            NiRect<float> kWindowRect, const char* pcName, 
            bool bShow = true, 
            unsigned int uiNumGraphs = 5);

        virtual ~NiVisualTracker();

        class NIVISUALTRACKER_ENTRY GraphCallbackObject : public NiMemObject
        {
            public:
                inline virtual ~GraphCallbackObject() {};
                virtual float TakeSample(float fTime) = 0;
        };

        // Insert a tracker into the list. Returns the tracker index.
        unsigned int AddGraph(GraphCallbackObject* pkObject, 
            const char* pcName, const NiColor& kColor, 
            unsigned int uiNumSamplesToKeep, float fMinSampleTime, bool bShow);
        void RemoveGraph(const char* pcName);
        void RemoveAll();
        
        // Accessors
        const char* GetName();
        unsigned int GetGraphCount();
        void SetName(const char* pcName, unsigned int uiWhichGraph);
        const char* GetName(unsigned int uiWhichGraph);
        unsigned int GetGraphIndexByName(const char* pcName);

        // Toggle the visibility of the graph. This determines if the tracker 
        // is currently visible, but does not affect its sampling.
        inline void ShowGraph(bool bShow, unsigned int uiWhichGraph);
        inline bool GetGraphShow(unsigned int uiWhichGraph);

        virtual void Update();

        virtual void Draw();

    protected:
        class GraphCallbackObjectData : public NiMemObject
        {
            public:
                GraphCallbackObjectData(GraphCallbackObject* pkObject, 
                    const char* pcName, const NiColor& kColor, 
                    unsigned int uiNumSamplesToKeep, float fMaxValue, 
                    float fSamplingTime, bool bShow, 
                    const NiRect<float>& kDimensions, NiNode* pkParentNode,
                    unsigned int& uiLegendX, unsigned int& uiLegendY,
                    NiMeshScreenElementsArray& kTextures);

                ~GraphCallbackObjectData();

                void Update(float fTime);
                void Draw();

                bool GetShow();
                void SetShow(bool bShow);
                static NiMeshPtr CreateLines(unsigned int uiNumVertices,
                    const NiColor& kColor, const NiRect<float>& kDimensions);

                const char* GetName();
                void SetName(const char* pcName);

                void AdjustLegend(int iX, int iY);
                unsigned int GetLegendWidth();

            protected:  

                // BUFFER_COUNT dictates the number of times the 
                // GraphCallbackObjectData mesh data will be duplicated. The 
                // duplicated buffers are updated and rendered in a round robin
                // fashion where the rendered mesh is always one frame behind. 
                // This prevents any GPU stalling that may occur from trying to
                // update a mesh that we are currently rendering.
                enum
                {
                    BUFFER_COUNT  = 3
                };

                GraphCallbackObject* m_pkCallbackObj;
                bool m_bShow;
                unsigned int m_uiNumSamplesToKeep;
                float m_fSamplingTime;
                NiMeshPtr m_aspLines[BUFFER_COUNT];
                float m_afPrevFramesValues[BUFFER_COUNT];
                NiColor m_kColor;
                float m_fLastTime;
                char m_acName[256];
                float m_fMaxValue;
                unsigned int m_uiNextVertex;
                NiRect<float> m_kDimensions;
                ScreenText* m_pkText;
                unsigned int m_uiBufferID;
        };

        NiTPrimitiveArray<GraphCallbackObjectData*> m_kCallbackData;
        NiRect<float> m_kGraphLineDimensions;
        NiNodePtr m_spWindowRoot;
        NiMeshPtr m_spBorders;
        float m_fMaxValue;

        ScreenText* m_pkTextTitle;
        ScreenText* m_pkTextZero;
        ScreenText* m_pkTextTwentyFive;
        ScreenText* m_pkTextFifty;
        ScreenText* m_pkTextSeventyFive;
        ScreenText* m_pkTextOneHundred;

        unsigned int m_uiLegendX;
        unsigned int m_uiLegendY;
};

NiSmartPointer(NiVisualTracker);

#include "NiVisualTracker.inl"

#endif
