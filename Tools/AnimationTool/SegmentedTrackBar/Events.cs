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

namespace SegmentedTrackBarDLL
{
    /// <summary>
    /// Summary description for Events.
    /// </summary>
    public class Events
    {
        public Events(){}

        #region Delegate Definitions
        public delegate void EventHandler_ScrubberPosChanged();        
        public delegate void EventHandler_MarkerPosChanged(MarkerItem marker);        
        public delegate void EventHandler_MarkerDragEnded(MarkerItem marker);
        public delegate void EventHandler_MarkerDragStarted(MarkerItem marker);
        public delegate void EventHandler_ScrubberChangedRegion(RegionItem oldRegion,
            RegionItem newRegion);
        public delegate void EventHandler_ScrubberRepositioned();

        #endregion

        #region Event Raising and Handlers
        static public event EventHandler_ScrubberPosChanged ScrubberPosChanged;       
        static public void RaiseScrubberPosChanged()
        {
            if (ScrubberPosChanged != null)
                ScrubberPosChanged();
        }
        
        static public event EventHandler_MarkerPosChanged MarkerPosChanged;       
        static public void RaiseMarkerPosChanged(MarkerItem marker)
        {
            if (MarkerPosChanged != null)
                MarkerPosChanged(marker);
        }

        static public event EventHandler_MarkerDragEnded MarkerDragEnded;       
        static public void RaiseMarkerDragEnded(MarkerItem marker)
        {
            if (MarkerDragEnded != null)
                MarkerDragEnded(marker);
        }

        static public event EventHandler_MarkerDragStarted MarkerDragStarted;       
        static public void RaiseMarkerDragStarted(MarkerItem marker)
        {
            if (MarkerDragStarted != null)
                MarkerDragStarted(marker);
        }

        static public event EventHandler_ScrubberRepositioned ScrubberRepositioned;       
        static public void RaiseScrubberRepositioned()
        {
            if (ScrubberRepositioned != null)
                ScrubberRepositioned();
        }
        #endregion
    }
}
