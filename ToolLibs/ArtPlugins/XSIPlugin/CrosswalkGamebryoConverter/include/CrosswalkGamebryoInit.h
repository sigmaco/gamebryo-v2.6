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

#ifndef XSI_GAMEBRYO_CONVERTER_INIT_H
#define XSI_GAMEBRYO_CONVERTER_INIT_H

namespace epg
{
    //---------------------------------------------------------------------------
    /// Initialize the Gamebryo engine if needed and register the initializer
    /// as using it. Each initializer name should be unique.
    //---------------------------------------------------------------------------
    void InitializeGamebryo(const char * in_InitializerName);

    //---------------------------------------------------------------------------
    /// Load all Gamebryo pipelin plugins.
    ///
    /// The initialization must already be done.
    //---------------------------------------------------------------------------
    bool LoadAllGamebryoPlugins();

    //---------------------------------------------------------------------------
    /// Shut down the Gamebryo engine when last user is done.
    ///
    /// Must be called with the same initializer name as InitializeGamebryo()
    /// was called.
    //---------------------------------------------------------------------------
    void ShutdownGamebryo(const char * in_InitializerName, bool avoidXSICrash = false);
}

#endif /* XSI_GAMEBRYO_CONVERTER_INIT_H */
