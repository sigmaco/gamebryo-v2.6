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
	/// <summary>
	/// Summary description for Track.
	/// </summary>
	[TypeConverter(typeof(ExpandableObjectConverter))]
	public class Track
	{
        private Color m_InRangeColor;
        private Color m_OutOfRangeColor;

        public Color InRangeColor
        {
            get
            {
                return m_InRangeColor;
            }
            set
            {
                m_InRangeColor = value;
            }
        }

        public Color OutOfRangeColor
        {
            get
            {
                return m_OutOfRangeColor;
            }
            set
            {
                m_OutOfRangeColor = value;
            }
        }
	}
}
