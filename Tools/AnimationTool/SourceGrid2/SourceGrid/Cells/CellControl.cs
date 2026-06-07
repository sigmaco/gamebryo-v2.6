using System;
using System.Windows.Forms;
using System.Reflection;
using System.Drawing;

namespace SourceGrid2.Cells.Real
{
//	/// <summary>
//	/// A cell that can contains a Control. Not recomanded.
//	/// </summary>
//	public class CellControl : Cell
//	{
//		private Control m_Control;
//		public Control InnerControl
//		{
//			get{return m_Control;}
//		}
//		public CellControl(Control p_Control)
//		{
//			m_Control = p_Control;
//		}
//		public CellControl(Control p_Control, PropertyInfo p_Property):this(p_Control)
//		{
//			BindValueAtProperty(p_Property,p_Control);
//		}
//
////		//proprietà per gestire il link di una cella ad una property
////		private PropertyInfo m_LinkPropertyInfo = null;
////		private object m_LinkObject = null;
////		public void BindValueAtProperty(PropertyInfo p_Property, object p_LinkObject)
////		{
////			m_LinkPropertyInfo = p_Property;
////			m_LinkObject = p_LinkObject;
////		}
////		public void UnBindValueAtProperty()
////		{
////			m_LinkPropertyInfo = null;
////			m_LinkObject = null;
////		}
//
//		protected override void OnAddToGrid(EventArgs e)
//		{
//			base.OnAddToGrid(e);
//			RecalcControlSize();
//			RecalcControlPosition();
//			Grid.Controls.Add(m_Control);
//			m_Control.BringToFront();
//		}
//
//		protected override void OnRemoveToGrid(EventArgs e)
//		{
//			base.OnRemoveToGrid(e);
//
//			//.Net bug : application doesn't close when a active control is removed from the control collection
//			// change the focus first
//			Grid.SetFocusOnGridSubPanel();
//			Grid.Controls.Remove(m_Control);
//		}
//
//		public virtual void RecalcControlPosition()
//		{
//			Rectangle l_rcClient = DisplayRectangle;
//			
//			if (m_Control.Location != l_rcClient.Location)
//				m_Control.Location = l_rcClient.Location;
//		}
//		public virtual void RecalcControlSize()
//		{
//			Rectangle l_rcClient = DisplayRectangle;
//			
//			if (m_Control.Size != l_rcClient.Size)
//				m_Control.Size = l_rcClient.Size;
//		}
//
//		public override void InvokePaint(PaintEventArgs e, Rectangle p_AbsoluteRectangle, bool p_bChekIfIsRegion)
//		{
//			//non chiamo il Paint della cella perchè il controllo si occupa di disegnare
//			RecalcControlPosition();
//			RecalcControlSize();
//		}
//
////		public override object Value
////		{
////			get
////			{
////				if (m_LinkPropertyInfo != null)
////					return m_LinkPropertyInfo.GetValue(m_LinkObject,null);
////				else
////					return base.Value;
////			}
////			set
////			{
////				if (m_LinkPropertyInfo != null)
////					m_LinkPropertyInfo.SetValue(m_LinkObject,value,null);
////				else
////					base.Value = value;
////			}
////		}
//	}
}
