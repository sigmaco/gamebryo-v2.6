using System;
using System.Drawing;
using SourceGrid2.Cells;

namespace SourceGrid2.BehaviorModels
{
	/// <summary>
	/// Implement the mouse resize features of a cell. This behavior can be shared between multiple cells.
	/// </summary>
	public class Resize : BehaviorModelGroup
	{
		/// <summary>
		/// Resize both width nd height behavior
		/// </summary>
		public readonly static Resize ResizeBoth = new Resize(CellResizeMode.Both);
		/// <summary>
		/// Resize width behavior
		/// </summary>
		public readonly static Resize ResizeWidth = new Resize(CellResizeMode.Width);
		/// <summary>
		/// Resize height behavior
		/// </summary>
		public readonly static Resize ResizeHeight = new Resize(CellResizeMode.Height);

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_Mode"></param>
		public Resize(CellResizeMode p_Mode)
		{
			m_ResizeMode = p_Mode;
//			SubModels.Add(m_ModelCursor);
		}

		#region IBehaviorModel Members

		public override void OnMouseDown(PositionMouseEventArgs e)
		{
			base.OnMouseDown (e);

			m_IsHeightResize = false;
			m_IsWidthResize = false;

			Rectangle l_CellRect = e.Grid.PositionToDisplayRect(e.Position);
			Point l_MousePoint = new Point(e.MouseEventArgs.X, e.MouseEventArgs.Y);

			if (  ( (ResizeMode & CellResizeMode.Width) == CellResizeMode.Width) &&
						IsInResizeHorRegion(l_CellRect, l_MousePoint) )
				m_IsWidthResize = true;
			else if (  ( (ResizeMode & CellResizeMode.Height) == CellResizeMode.Height) &&
						IsInResizeVerRegion(l_CellRect, l_MousePoint) )
				m_IsHeightResize = true;
		}

		public override void OnMouseUp(PositionMouseEventArgs e)
		{
			base.OnMouseUp (e);

			m_IsWidthResize = false;
			m_IsHeightResize = false;
		}

		public override void OnMouseMove(PositionMouseEventArgs e)
		{
			base.OnMouseMove (e);

			Rectangle l_CellRect = e.Grid.PositionToDisplayRect(e.Position);
			Point l_MousePoint = new Point(e.MouseEventArgs.X, e.MouseEventArgs.Y);

			Point l_AbsPoint = e.Grid.PointRelativeToAbsolute(l_MousePoint);
			if (e.Position.Row < e.Grid.FixedRows)
				l_AbsPoint.Y = l_MousePoint.Y;
			if (e.Position.Column < e.Grid.FixedColumns)
				l_AbsPoint.X = l_MousePoint.X;

			//sono già in fase di resizing
			if (e.Grid.MouseDownPosition == e.Position)
			{
				if (m_IsWidthResize)
				{
					int l_NewWidth = l_AbsPoint.X - e.Grid.Columns[e.Position.Column].Left;

					if (l_NewWidth>0)
						e.Grid.Columns[e.Position.Column].Width = l_NewWidth+c_MouseDelta;
				}
				else if (m_IsHeightResize)
				{
					int l_NewHeight = l_AbsPoint.Y - e.Grid.Rows[e.Position.Row].Top;

					if (l_NewHeight>0)
						e.Grid.Rows[e.Position.Row].Height = l_NewHeight+c_MouseDelta;
				}
			}

#if !MINI
			//comunque aggiorno il cursore
			if ( IsInResizeHorRegion(l_CellRect, l_MousePoint) && (ResizeMode & CellResizeMode.Width) == CellResizeMode.Width )
				e.Cell.Grid.GridCursor = System.Windows.Forms.Cursors.VSplit;
			else if ( IsInResizeVerRegion(l_CellRect, l_MousePoint) && (ResizeMode & CellResizeMode.Height) == CellResizeMode.Height )
				e.Cell.Grid.GridCursor = System.Windows.Forms.Cursors.HSplit;
			else
				e.Cell.Grid.GridCursor = null;
#endif
		}

#if !MINI
		public override void OnMouseLeave(PositionEventArgs e)
		{
			base.OnMouseLeave(e);

			e.Cell.Grid.GridCursor = null;
			m_IsWidthResize = false;
			m_IsHeightResize = false;
		}

		public override void OnDoubleClick(PositionEventArgs e)
		{
			base.OnDoubleClick(e);

			Point l_Current = e.Grid.PointToClient(System.Windows.Forms.Control.MousePosition);
			Rectangle l_CellRect = e.Grid.PositionToDisplayRect(e.Position);

			if ( (ResizeMode & CellResizeMode.Width) == CellResizeMode.Width &&
				IsInResizeHorRegion(l_CellRect, l_Current))
			{
				e.Grid.Columns[e.Position.Column].AutoSize(true);
			}
			else if ( (ResizeMode & CellResizeMode.Height) == CellResizeMode.Height &&
				IsInResizeVerRegion(l_CellRect, l_Current))
			{
				e.Grid.Rows[e.Position.Row].AutoSize(true);
			}
		}
#endif

		#endregion

		private CellResizeMode m_ResizeMode = CellResizeMode.Both;

		/// <summary>
		/// Resize mode of the cell
		/// </summary>
		public CellResizeMode ResizeMode
		{
			get{return m_ResizeMode;}
		}

//		private Cursor m_ModelCursor = new Cursor();

		//Queste variabili indoco lo stato del resize (essendo usate però in un contesto di MouseEnter e MouseLeave possono essere tranquillamente condivise tra più cello o griglie, visto che il mouse in un dato momento sarà solo in una cella particolare, di un thread particolare, ...). Questo è un motivo in più per non poter usare questo controllo in multi thread (cosa che nessun controllo windows form può fare ...)
		private bool m_IsWidthResize = false;
		private bool m_IsHeightResize = false;

		/// <summary>
		/// Indicates if the behavior is currently resizing a cell width
		/// </summary>
		public bool IsWidthResizing
		{
			get{return m_IsWidthResize;}
		}

		/// <summary>
		/// Indicates if the behavior is currently resizing a cell height
		/// </summary>
		public bool IsHeightResizing
		{
			get{return m_IsHeightResize;}
		}

		#region Support Functions
		/// <summary>
		/// 
		/// </summary>
		/// <param name="p_CellRectangle">A grid relative rectangle</param>
		/// <param name="p"></param>
		/// <returns></returns>
		public static bool IsInResizeHorRegion(Rectangle p_CellRectangle, Point p)
		{
			if (p.X >= p_CellRectangle.Right-c_MouseDelta && p.X <= p_CellRectangle.Right)
				return true;
			else
				return false;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="p_CellRectangle">A grid relative rectangle</param>
		/// <param name="p"></param>
		/// <returns></returns>
		public static bool IsInResizeVerRegion(Rectangle p_CellRectangle, Point p)
		{
			if (p.Y >= p_CellRectangle.Bottom-c_MouseDelta && p.Y <= p_CellRectangle.Bottom)
				return true;
			else
				return false;
		}

		private const int c_MouseDelta = 4;

		#endregion
	}
}
