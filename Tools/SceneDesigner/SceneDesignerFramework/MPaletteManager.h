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

#pragma once

#include "MDisposable.h"
#include "MPalette.h"

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MPaletteManager : public MDisposable
    {
    public:
        __property unsigned int get_PaletteCount();
        MPalette* GetPalettes()[];
        MPalette* GetPaletteByName(String* strName);
        bool AddPalette(MPalette* pmPalette);
        void RemovePalette(MPalette* pmPalette);
        void RemoveAllPalettes();

        __property MPalette* get_ActivePalette();
        __property void set_ActivePalette(MPalette* pmActivePalette);
        __property String* get_PaletteFolder();
        __property void set_PaletteFolder(String* strPath);
        __property String* get_PaletteFileExtension();

        MPalette* LoadPalette(String* strPath);
        void SavePalette(MPalette* pmPalette, String* strFilename,
            String* strFormat);

    private:
        ArrayList* m_pmPalettes;
        MPalette* m_pmActivePalette;
        String* m_strPaletteFolder;
        String* m_strPaletteFileExtension;

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // Singleton members.
    private public:
        static void Init();
        static void Shutdown();
        static bool InstanceIsValid();
        __property static MPaletteManager* get_Instance();
    private:
        static MPaletteManager* ms_pmThis = NULL;
        MPaletteManager();
    };
}}}}