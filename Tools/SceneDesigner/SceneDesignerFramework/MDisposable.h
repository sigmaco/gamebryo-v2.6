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

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __abstract __gc class MDisposable : public IDisposable
    {
    public:
        MDisposable();
        virtual ~MDisposable();

        // IDisposable overrides.
        void Dispose();

    protected:
        virtual void Do_Dispose(bool bDisposing) = 0;
        bool HasBeenDisposed();

    private:
        void Dispose(bool bFromDestructor);
        bool m_bDisposed;
    };
}}}}
