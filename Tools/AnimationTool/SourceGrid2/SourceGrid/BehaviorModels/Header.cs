using System;
using SourceGrid2.Cells;

namespace SourceGrid2.BehaviorModels
{
	/// <summary>
	/// Summary description for Header.
	/// </summary>
	public class Header : BehaviorModelGroup
	{
		/// <summary>
		/// Default column header behavior
		/// </summary>
		public readonly static Header Default = new Header();

		private Resize m_Resize;
		/// <summary>
		/// Constructor
		/// </summary>
		public Header():this(Resize.ResizeBoth, Button.Default)
		{
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_BehaviorResize"></param>
		/// <param name="p_BehaviorButton"></param>
		public Header(Resize p_BehaviorResize, Button p_BehaviorButton)
		{
			m_Resize = p_BehaviorResize;

			SubModels.Add(m_Resize);
			SubModels.Add(p_BehaviorButton);
		}
	}
}
