// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
// 
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
// 
// Copyright (c) 1996-2008 Emergent Game Technologies.
// All Rights Reserved.
// 
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

using System;
using System.ComponentModel;
using System.ComponentModel.Design.Serialization;

namespace SegmentedTrackBarDLL
{
    [ToolboxItem(false)]
    [DesignTimeVisible(false)]
    [TypeConverter(typeof(SegmentedTrackBarDLL.CollectionItemConverter))]
    public abstract class CollectionItem: System.ComponentModel.Component, IDisposable
    {
        internal Collection collection;

        ~CollectionItem()
        {
            Dispose(false);
        }

        [Browsable(false)]
        public Collection Collection
        {
            get {return collection;}
        }
    }
}
