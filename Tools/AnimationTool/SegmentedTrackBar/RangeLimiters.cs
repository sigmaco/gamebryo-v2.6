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
using System.Drawing;
using System.Windows.Forms;
using System.ComponentModel;
using System.ComponentModel.Design;

namespace SegmentedTrackBarDLL
{
    [TypeConverter(typeof(ExpandableObjectConverter))]
    public class RangeLimiter
    {
        private int m_iPosition;
        private bool m_bEnabled;
        private bool m_bVisible;

        public RangeLimiter()
        {
            m_iPosition = 0;
            m_bEnabled = true;
            m_bVisible = true;
        }
        
        public int Position
        {
            get { return m_iPosition; }
            set { m_iPosition = value; }
        }
        
        public bool Enabled
        {
            get { return m_bEnabled; }
            set { m_bEnabled = value; }
        }

        public bool Visible
        {
            get { return m_bVisible; }
            set { m_bVisible = value; }
        }
    }
}
