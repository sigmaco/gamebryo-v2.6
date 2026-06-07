using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace SourceGrid2.Cells.Virtual
{
	/// <summary>
	/// Represents a CellVirtual in a grid.
	/// </summary>
	public abstract class CellVirtual : ICellVirtual
	{
		#region Constructor
		/// <summary>
		/// Constructor
		/// </summary>
		public CellVirtual():this(null, VisualModels.Common.Default)
		{
		}

		/// <summary>
		/// Create a cell with an editor using the type specified. (using Utility.CreateCellModel).
		/// </summary>
		/// <param name="p_Type">Type of the cell</param>
		public CellVirtual(Type p_Type):this(Utility.CreateDataModel(p_Type))
		{
		}

		/// <summary>
		/// Cell constructor
		/// </summary>
		/// <param name="p_Editor">The editor of this cell</param>
		public CellVirtual(DataModels.IDataModel p_Editor):this(p_Editor, VisualModels.Common.Default)
		{
		}

		/// <summary>
		/// Create a new instance of the cell. Using BehaviorModels.Common.Default
		/// </summary>
		/// <param name="p_Editor">Formatters used for string conversion, if null is used a shared default formatter.</param>
		/// <param name="p_VisualModel">Visual properties of the current cell, if null is used a shared default model VisualModels.Common.Default.</param>
		public CellVirtual(DataModels.IDataModel p_Editor, VisualModels.IVisualModel p_VisualModel)
			:this(p_Editor, p_VisualModel, BehaviorModels.Common.Default)
		{

		}

		/// <summary>
		/// Create a new instance of the cell.
		/// </summary>
		/// <param name="p_Editor">Formatters used for string conversion, if null is used a shared default formatter.</param>
		/// <param name="p_VisualModel">Visual properties of the current cell, if null is used a shared default model VisualModels.Common.Default.</param>
		/// <param name="p_BehaviorCommon"></param>
		public CellVirtual(DataModels.IDataModel p_Editor, VisualModels.IVisualModel p_VisualModel, BehaviorModels.IBehaviorModel p_BehaviorCommon)
		{
			DataModel = p_Editor;

			if (p_VisualModel!=null)
				VisualModel = p_VisualModel;
			else
				VisualModel = VisualModels.Common.Default;

			Behaviors.Add(p_BehaviorCommon);
		}

		#endregion

		#region Format

		//these methods clone the VisualModel and then change the specified property

		/// <summary>
		/// If null the default font is used
		/// </summary>
		public Font Font
		{
			get{return m_VisualModel.Font;} 
			set
			{
				VisualModels.IVisualModel l_Model = (VisualModels.IVisualModel)m_VisualModel.Clone(false);
				l_Model.Font = value;
				l_Model.MakeReadOnly();
				VisualModel = l_Model;
			}
		}

		/// <summary>
		/// BackColor of the cell
		/// </summary>
		public Color BackColor
		{
			get{return m_VisualModel.BackColor;}
			set
			{
				VisualModels.IVisualModel l_Model = (VisualModels.IVisualModel)m_VisualModel.Clone(false);
				l_Model.BackColor = value;
				l_Model.MakeReadOnly();
				VisualModel = l_Model;
			}
		}

		/// <summary>
		/// ForeColor of the cell
		/// </summary>
		public Color ForeColor
		{
			get{return m_VisualModel.ForeColor;}
			set
			{
				VisualModels.IVisualModel l_Model = (VisualModels.IVisualModel)m_VisualModel.Clone(false);
				l_Model.ForeColor = value;
				l_Model.MakeReadOnly();
				VisualModel = l_Model;
			}
		}

		/// <summary>
		/// The normal border of a cell
		/// </summary>
		public RectangleBorder Border
		{
			get{return m_VisualModel.Border;}
			set
			{
				VisualModels.IVisualModel l_Model = (VisualModels.IVisualModel)m_VisualModel.Clone(false);
				l_Model.Border = value;
				l_Model.MakeReadOnly();
				VisualModel = l_Model;
			}
		}

		/// <summary>
		/// Word Wrap.  This property is only a wrapper around StringFormat
		/// </summary>
		public bool WordWrap
		{
			get{return m_VisualModel.WordWrap;}
			set
			{
				VisualModels.IVisualModel l_Model = (VisualModels.IVisualModel)m_VisualModel.Clone(false);
				l_Model.WordWrap = value;
				l_Model.MakeReadOnly();
				VisualModel = l_Model;
			}
		}

		/// <summary>
		/// Text Alignment. This property is only a wrapper around StringFormat
		/// </summary>
		public SourceLibrary.Drawing.ContentAlignment TextAlignment
		{
			get{return m_VisualModel.TextAlignment;}
			set
			{
				VisualModels.IVisualModel l_Model = (VisualModels.IVisualModel)m_VisualModel.Clone(false);
				l_Model.TextAlignment = value;
				l_Model.MakeReadOnly();
				VisualModel = l_Model;
			}
		}
		#endregion

		#region LinkToGrid
		private GridVirtual m_Grid;
		/// <summary>
		/// The Grid object
		/// </summary>
		public GridVirtual Grid
		{
			get{return m_Grid;}
		}

		/// <summary>
		/// Link the cell at the specified grid.
		/// </summary>
		/// <param name="p_grid"></param>
		public virtual void BindToGrid(GridVirtual p_grid)
		{
			m_Grid = p_grid;
			OnAddToGrid(EventArgs.Empty);
		}
		/// <summary>
		/// Remove the link of the cell from the previous grid.
		/// </summary>
		public virtual void UnBindToGrid()
		{
			if (m_Grid!=null) //tolgo la cella dalla griglia precedentemente selezionata
				OnRemoveToGrid(EventArgs.Empty);

			m_Grid = null;
		}

		/// <summary>
		/// Fired when the cell is added to a grid
		/// </summary>
		/// <param name="e"></param>
		protected virtual void OnAddToGrid(EventArgs e)
		{
		}
		/// <summary>
		/// Fired before a cell is removed from a grid
		/// </summary>
		/// <param name="e"></param>
		protected virtual void OnRemoveToGrid(EventArgs e)
		{
		}
		#endregion

		#region GetValue, SetValue (ABSTRACT)

		/// <summary>
		/// Get the value of the cell at the specified position
		/// </summary>
		/// <param name="p_Position"></param>
		/// <returns></returns>
		public abstract object GetValue(Position p_Position);

		/// <summary>
		/// Set the value of the cell at the specified position. This method must call OnValueChanged() event.
		/// </summary>
		/// <param name="p_Position"></param>
		/// <param name="p_Value"></param>
		public abstract void SetValue(Position p_Position, object p_Value);

		#endregion

		#region GetDisplayString
		/// <summary>
		/// The string representation of the Cell.GetValue method (default Value.ToString())
		/// </summary>
		/// <param name="p_Position"></param>
		/// <returns></returns>
		public virtual string GetDisplayText(Position p_Position)
		{
			try
			{
				object l_TmpVal = GetValue(p_Position);
				if (m_CellModel==null)
				{
					if (l_TmpVal==null)
						return "";
					else
						return l_TmpVal.ToString();
				}
				else
				{
					return m_CellModel.ValueToDisplayString(l_TmpVal);
				}
			}
			catch(Exception err)
			{
				return "Error:"+err.Message;
			}
		}

		#endregion

		#region VisualModel

		private VisualModels.IVisualModel m_VisualModel;

		/// <summary>
		/// Visual properties of this cell and other cell. You can share the VisualProperties between many cell to optimize memory size.
		/// Warning Changing this property can affect many cells
		/// </summary>
		[Browsable(false)]
		public virtual VisualModels.IVisualModel VisualModel
		{
			get{return m_VisualModel;}
			set
			{
				if (value==null)
					throw new ArgumentNullException("VisualModel");

				m_VisualModel = value;

				Invalidate();
			}
		}
		#endregion

		#region Selection
		/// <summary>
		/// True if the cell can have the focus otherwise false.
		/// </summary>
		/// <returns></returns>
		public virtual bool CanReceiveFocus
		{
			get
			{
				bool ret = true;
				for (int i = 0; i < m_BehaviorModels.Count; i++)
					ret = ret && m_BehaviorModels[i].CanReceiveFocus;

				return ret;
			}
		}
		#endregion

		#region GetRequiredSize
		/// <summary>
		/// If the cell is not linked to a grid the result is not accurate (Font can be null). Call InternalMeasure with RowSpan and ColSpan = 1.
		/// </summary>
		/// <param name="p_Position">Position of the current cell</param>
		/// <param name="g"></param>
		/// <param name="maxLayoutArea">SizeF structure that specifies the maximum layout area for the text. If width or height are zero the value is set to a default maximum value.</param>
		/// <returns></returns>
		public virtual Size Measure(Position p_Position, Graphics g, SizeF maxLayoutArea)
		{
			//Per gestire RowSpan/ColSpan bisogna fare l'override di questo metodo.
			return InternalMeasure(p_Position, g, maxLayoutArea, 1, 1);
		}

		/// <summary>
		/// If the cell is not linked to a grid the result is not accurate (Font can be null)
		/// </summary>
		/// <param name="p_Position">Position of the current cell</param>
		/// <param name="g"></param>
		/// <param name="maxLayoutArea">SizeF structure that specifies the maximum layout area for the text. If width or height are zero the value is set to a default maximum value.</param>
		/// <param name="p_RowSpan"></param>
		/// <param name="p_ColSpan"></param>
		/// <returns></returns>
		protected virtual Size InternalMeasure(Position p_Position, Graphics g, SizeF maxLayoutArea, int p_RowSpan, int p_ColSpan)
		{
			SizeF l_ReqSize = VisualModel.Measure(g, this, p_Position, maxLayoutArea);

			//Approximate the width and Height value if ColSpan or RowSpan are grater than 1
			l_ReqSize.Width = l_ReqSize.Width / (float)p_ColSpan;
			l_ReqSize.Height = l_ReqSize.Height / (float)p_RowSpan;

			return l_ReqSize.ToSize();
		}
		#endregion

		#region Editing

		private DataModels.IDataModel m_CellModel = null;
		/// <summary>
		/// Editor of this cell and others cells. If null no edit is supported. 
		///  You can share the same model between many cells to optimize memory size. Warning Changing this property can affect many cells
		/// </summary>
		public DataModels.IDataModel DataModel
		{
			get{return m_CellModel;}
			set{m_CellModel = value;}
		}

		/// <summary>
		/// Start the edit operation with the current editor specified in the Model property.
		/// </summary>
		/// <param name="p_Position"></param>
		/// <param name="p_NewStartEditValue">The value that the editor receive</param>
		public virtual void StartEdit(Position p_Position, object p_NewStartEditValue)
		{
			if (m_Grid!=null && 
				m_CellModel != null && 
				IsEditing(p_Position) == false &&  //se la cella non è già in stato edit
				m_CellModel.EnableEdit && 
				m_Grid.Selection.Focus(p_Position)) //per finire eventuali altri edit e posizionare il focus su questa cella
			{
				PositionCancelEventArgs l_EventArgs = new PositionCancelEventArgs(p_Position, this);
				OnEditStarting(l_EventArgs);
				if (l_EventArgs.Cancel == false)
					m_CellModel.InternalStartEdit(this, p_Position, p_NewStartEditValue);
			}
		}

		/// <summary>
		/// Start the edit operation with the current editor specified in the Model property.
		/// </summary>
		/// <param name="p_Position"></param>
		public void StartEdit(Position p_Position)
		{
			StartEdit(p_Position, null);
		}

		/// <summary>
		/// Terminate the edit operation
		/// </summary>
		/// <param name="p_bCancel">If true undo all the changes</param>
		/// <returns>Returns true if the edit operation is successfully terminated, otherwise false</returns>
		public bool EndEdit(bool p_bCancel)
		{
			if (m_CellModel != null && m_CellModel.IsEditing)
			{
				Position l_Position = m_CellModel.EditPosition;
				bool l_Success = m_CellModel.InternalEndEdit(p_bCancel);
				if (l_Success)
				{
					PositionCancelEventArgs l_EventArgs = new PositionCancelEventArgs(l_Position, this);
					l_EventArgs.Cancel = p_bCancel;
					OnEditEnded(l_EventArgs);
				}
				return l_Success;
			}
			else
				return true;
		}

		/// <summary>
		/// True if this cell is currently in edit state, otherwise false.
		/// </summary>
		/// <param name="p_Position"></param>
		/// <returns></returns>
		public virtual bool IsEditing(Position p_Position)
		{
			if (m_CellModel!=null)
				return (m_CellModel.IsEditing && m_CellModel.EditCell == this && m_CellModel.EditPosition == p_Position);
			else
				return false;
		}
		#endregion

		#region Events
		/// <summary>
		/// Fired when a context menu is showed
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnContextMenuPopUp(PositionContextMenuEventArgs e)
		{
			for (int i = 0; i < m_BehaviorModels.Count; i++)
				m_BehaviorModels[i].OnContextMenuPopUp(e);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnMouseDown(PositionMouseEventArgs e)
		{
			for (int i = 0; i < m_BehaviorModels.Count; i++)
				m_BehaviorModels[i].OnMouseDown(e);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnMouseUp(PositionMouseEventArgs e)
		{
			for (int i = 0; i < m_BehaviorModels.Count; i++)
				m_BehaviorModels[i].OnMouseUp(e);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnMouseMove(PositionMouseEventArgs e)
		{
			for (int i = 0; i < m_BehaviorModels.Count; i++)
				m_BehaviorModels[i].OnMouseMove(e);
		}
#if !MINI
		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnMouseEnter(PositionEventArgs e)
		{
			for (int i = 0; i < m_BehaviorModels.Count; i++)
				m_BehaviorModels[i].OnMouseEnter(e);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnMouseLeave(PositionEventArgs e)
		{
			for (int i = 0; i < m_BehaviorModels.Count; i++)
				m_BehaviorModels[i].OnMouseLeave(e);
		}
#endif

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnKeyUp ( PositionKeyEventArgs e)
		{
			for (int i = 0; i < m_BehaviorModels.Count; i++)
				m_BehaviorModels[i].OnKeyUp(e);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnKeyDown ( PositionKeyEventArgs e)
		{
			for (int i = 0; i < m_BehaviorModels.Count; i++)
				m_BehaviorModels[i].OnKeyDown(e);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnKeyPress ( PositionKeyPressEventArgs e)
		{
			for (int i = 0; i < m_BehaviorModels.Count; i++)
				m_BehaviorModels[i].OnKeyPress(e);
		}

#if !MINI
		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnDoubleClick (PositionEventArgs e)
		{
			for (int i = 0; i < m_BehaviorModels.Count; i++)
				m_BehaviorModels[i].OnDoubleClick(e);
		}
#endif

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnClick (PositionEventArgs e)
		{
			for (int i = 0; i < m_BehaviorModels.Count; i++)
				m_BehaviorModels[i].OnClick(e);
		}


		/// <summary>
		/// Fired before the cell leave the focus, you can put the e.Cancel = true to cancel the leave operation.
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnFocusLeaving(PositionCancelEventArgs e)
		{
			for (int i = 0; i < m_BehaviorModels.Count; i++)
				m_BehaviorModels[i].OnFocusLeaving(e);
		}

		/// <summary>
		/// Fired when the cell has left the focus.
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnFocusLeft(PositionEventArgs e)
		{
			for (int i = 0; i < m_BehaviorModels.Count; i++)
				m_BehaviorModels[i].OnFocusLeft(e);
		}

		/// <summary>
		/// Fired when the focus is entering in the specified cell. You can put the e.Cancel = true to cancel the focus operation.
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnFocusEntering(PositionCancelEventArgs e)
		{
			for (int i = 0; i < m_BehaviorModels.Count; i++)
				m_BehaviorModels[i].OnFocusEntering(e);
		}

		/// <summary>
		/// Fired when the focus enter in the specified cell.
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnFocusEntered(PositionEventArgs e)
		{
			for (int i = 0; i < m_BehaviorModels.Count; i++)
				m_BehaviorModels[i].OnFocusEntered(e);
		}


		/// <summary>
		/// Fired when the SetValue method is called.
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnValueChanged(PositionEventArgs e)
		{
			for (int i = 0; i < m_BehaviorModels.Count; i++)
				m_BehaviorModels[i].OnValueChanged(e);
		}

		/// <summary>
		/// Fired when the StartEdit is called. You can set the Cancel = true to stop editing.
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnEditStarting(PositionCancelEventArgs e)
		{
			for (int i = 0; i < m_BehaviorModels.Count; i++)
				m_BehaviorModels[i].OnEditStarting(e);
		}
		/// <summary>
		/// Fired when the EndEdit is called. You can read the Cancel property to determine if the edit is completed. If you change the cancel property there is no effect.
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnEditEnded(PositionCancelEventArgs e)
		{
			for (int i = 0; i < m_BehaviorModels.Count; i++)
				m_BehaviorModels[i].OnEditEnded(e);
		}
		#endregion

		#region BehaviorModel
		private BehaviorModels.BehaviorModelCollection m_BehaviorModels = new SourceGrid2.BehaviorModels.BehaviorModelCollection();
		/// <summary>
		/// Collection of BehaviorModel. This represents the action that a cell can do.
		/// </summary>
		public BehaviorModels.BehaviorModelCollection Behaviors
		{
			get{return m_BehaviorModels;}
		}
		#endregion

		#region Invalidate

		/// <summary>
		/// Invalidate this cell. For this type of class I must invalidate the whole grid, because I don't known the current cell position.
		/// </summary>
		public virtual void Invalidate()
		{
			if (m_Grid != null)
				m_Grid.InvalidateCells();
		}

		/// <summary>
		/// Invalidate this cell
		/// </summary>
		/// <param name="p_Position"></param>
		public virtual void Invalidate(Position p_Position)
		{
			if (m_Grid != null)
			{
				//Non avendo la posizione della cella non conosco quale area invalidare. Ma visto che questa cella può essere usata per più posizioni credo sia giusto.
				m_Grid.InvalidateCell(p_Position);
			}
		}
		#endregion
	}
}
