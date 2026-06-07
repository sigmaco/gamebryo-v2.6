using System;
using System.Windows.Forms;

namespace SourceGrid2.DataModels
{
	/// <summary>
	/// The base class for all the editor that have a control
	/// </summary>
	public abstract class EditorControlBase : DataModelBase
	{
		#region Constructor
		/// <summary>
		/// Construct a Model. Based on the Type specified the constructor populate AllowNull, DefaultValue, TypeConverter, StandardValues, StandardValueExclusive
		/// </summary>
		/// <param name="p_Type">The type of this model</param>
		public EditorControlBase(Type p_Type):base(p_Type)
		{
		}
		#endregion

		#region Editor Control
		/// <summary>
		/// Returns the control attached to a particular grid, using ScrollablePanel. This method cannot be used to retrive editor attached to the fixed panel. Use GetEditorControl(GridSubPanel p_GridPanel) if you want to attach the editor to a fixed panel.
		/// </summary>
		/// <param name="p_Grid">The grid where the control is attached.</param>
		/// <returns></returns>
		[Obsolete("Use GetEditorControl(GridSubPanel)")]
		public virtual System.Windows.Forms.Control GetEditorControl(GridVirtual p_Grid)
		{
			return GetEditorControl(p_Grid.ScrollablePanel);
		}

		/// <summary>
		/// Returns the control attached to a particular grid panel.
		/// </summary>
		/// <param name="p_GridPanel"></param>
		/// <returns></returns>
		public virtual System.Windows.Forms.Control GetEditorControl(GridSubPanel p_GridPanel)
		{
			if (IsAttached(p_GridPanel))
				return p_GridPanel.ControlsRepository[GetEditorControlGuid()];
			else
				throw new SourceGridException("Editor not attached to the grid, call AttachEditorControl first");
		}

		//ATTENZIONE: Ho dovuto fare un sistema di chiavi per i controlli dell'edit perchè altrimenti non riuscivo a fare uno share corretto degli editor.
		// questo perchè se ad esempio aggiungo un controllo di edito a una griglia e questa griglia viene distrutta, viene chiamato il
		// dispose su tutti gli oggetti contenuti, questo di fatto distruggeva l'editor che invece era magari condiviso da più griglie

		/// <summary>
		/// Returns true if the control is atteched to the grid. This method use IsAttached(GridSubPanel p_GridPanel) with ScrollablePanel property.
		/// </summary>
		/// <param name="p_Grid">The grid to check if the control is attached</param>
		/// <returns></returns>
		[Obsolete("Use IsAttached(GridSubPanel)")]
		public virtual bool IsAttached(GridVirtual p_Grid)
		{
			return IsAttached(p_Grid.ScrollablePanel);
		}

		/// <summary>
		/// Returns true if the control is atteched to the grid panel.
		/// </summary>
		/// <param name="p_GridPanel">The grid to check if the control is attached</param>
		/// <returns></returns>
		public virtual bool IsAttached(GridSubPanel p_GridPanel)
		{
			return p_GridPanel.ControlsRepository.ContainsKey(GetEditorControlGuid());
		}

		/// <summary>
		/// Add the current editor to the grid ScrollablePanel. If you want to attach the editor to another panel call AttachEditorControl(GridSubPanel p_GridPanel)
		/// </summary>
		/// <param name="p_Grid"></param>
		[Obsolete("Use AttachEditorControl(GridSubPanel)")]
		public virtual void AttachEditorControl(GridVirtual p_Grid)
		{
			AttachEditorControl(p_Grid.ScrollablePanel);
		}

		/// <summary>
		/// Add the current editor to the grid panel.
		/// </summary>
		/// <param name="p_GridPanel"></param>
		public virtual void AttachEditorControl(GridSubPanel p_GridPanel)
		{
			if (IsAttached(p_GridPanel) == false)
			{
#if !MINI
				p_GridPanel.SuspendLayout();
#endif

				try
				{
					Control l_EditorControl = CreateEditorControl();
					l_EditorControl.Visible = false;
					p_GridPanel.ControlsRepository.Add(GetEditorControlGuid(), l_EditorControl);
#if !MINI
					l_EditorControl.CreateControl();
#endif

					//m_Control.KeyDown += new KeyEventHandler(InnerControl_KeyDown);
					//l_EditorControl.Validating += new System.ComponentModel.CancelEventHandler(InnerControl_Validating);
					l_EditorControl.Validated += new EventHandler(InnerControl_Validated);

					OnEditorAttached(new ControlEventArgs(l_EditorControl));
				}
				finally
				{
#if !MINI
					p_GridPanel.ResumeLayout(true);
#endif
				}
			}
		}

		/// <summary>
		/// Fired when the control is attached to a panel. Use this method to customize the control.
		/// </summary>
		/// <param name="e"></param>
		protected virtual void OnEditorAttached(ControlEventArgs e)
		{
		}

		/// <summary>
		/// Remove the current editor from the grid control
		/// </summary>
		/// <param name="p_Grid"></param>
		public virtual void DetachEditorControl(GridVirtual p_Grid)
		{
			DetachEditorControl(p_Grid.ScrollablePanel);
		}

		/// <summary>
		/// Remove the current editor from the grid panel.
		/// </summary>
		/// <param name="p_GridPanel"></param>
		public virtual void DetachEditorControl(GridSubPanel p_GridPanel)
		{
			if (IsAttached(p_GridPanel))
			{
				Control l_EditorControl = GetEditorControl(p_GridPanel);

				//m_Control.KeyDown -= new KeyEventHandler(InnerControl_KeyDown);
				//l_EditorControl.Validating -= new System.ComponentModel.CancelEventHandler(InnerControl_Validating);
				l_EditorControl.Validated -= new EventHandler(InnerControl_Validated);

#if !MINI
				//.Net bug : application doesn't close when a active control is removed from the control collection
				// change the focus first
				if (l_EditorControl.ContainsFocus)
					p_GridPanel.Grid.SetFocusOnCells();
#else
				if (l_EditorControl.Focused)
					p_GridPanel.Grid.SetFocusOnCells();
#endif

				l_EditorControl.Hide();
				p_GridPanel.ControlsRepository.Remove(GetEditorControlGuid());
			}
		}
		#endregion

		/// <summary>
		/// Start editing the cell passed. Do not call this method for start editing a cell, you must use Cell.StartEdit.
		/// </summary>
		/// <param name="p_Cell">Cell to start edit</param>
		/// <param name="p_Position">Editing position(Row/Col)</param>
		/// <param name="p_StartEditValue">Can be null(in this case use the p_cell.Value</param>
		public override void InternalStartEdit(Cells.ICellVirtual p_Cell, Position p_Position, object p_StartEditValue)
		{
			base.InternalStartEdit(p_Cell, p_Position, p_StartEditValue);

			if (p_Cell==null)
				throw new ArgumentNullException("p_Cell");
			if (p_Cell.Grid == null)
				throw new SourceGridException("Cell is not bounded to a grid");
			if (p_Cell.Grid.Selection.FocusPosition != p_Position)
				throw new SourceGridException("Cell must have the focus");

			if (IsEditing == false && EnableEdit)
			{
				//verifico di non avere ancora una cella associata
				if (EditCell!=null)
					throw new SourceGridException("There is already a Cell in edit state");

				GridSubPanel l_Panel = p_Cell.Grid.PanelAtPosition(p_Position);
				if (l_Panel==null)
					throw new SourceGridException("Invalid Cell Position, panel not found");
				AttachEditorControl(l_Panel);

				Control l_EditorControl = GetEditorControl(l_Panel);

				p_Cell.Grid.LinkedControls[l_EditorControl] = p_Position;

				//aggiorno la posizione
				p_Cell.Grid.RefreshLinkedControlsBounds();

				l_EditorControl.Show();
				l_EditorControl.BringToFront();
				l_EditorControl.Focus();

				SetEditCell(p_Cell, p_Position);//con questa chiamata inizia logicamente l'edit

				//p_Cell.Grid.InvalidateCell(p_Position);
			}
		}

#if !MINI
		private Guid m_Guid = Guid.NewGuid();//create a new key for each instance of the class
#else
		private static int s_Guid = 0;
		private static Guid NextStaticGuid()
		{
			s_Guid++;
			return new Guid(s_Guid,0,0,0,0,0,0,0,0,0,0);
		}
		private Guid m_Guid = NextStaticGuid();//create a new key for each instance of the class
#endif

		/// <summary>
		/// Returns the GUID of the control that the current editor use
		/// </summary>
		/// <returns></returns>
		public virtual Guid GetEditorControlGuid()
		{
			return m_Guid;
		}
		/// <summary>
		/// Create a new control used in this editor
		/// </summary>
		/// <returns></returns>
		public abstract Control CreateEditorControl();


		/// <summary>
		/// Apply edited value
		/// </summary>
		/// <returns></returns>
		public override bool InternalApplyEdit()
		{
			if (IsEditing == true && EnableEdit == true)
			{
				bool l_bSuccess;

				try
				{
					l_bSuccess = SetCellValue(EditCell, EditPosition, GetEditedValue());
				}
				catch(Exception err)
				{
					OnEditException(new EditExceptionEventArgs(err));
					l_bSuccess = false;
				}

				return l_bSuccess;
			}
			else
				return true;
		}

		/// <summary>
		/// Terminate the edit action
		/// </summary>
		/// <param name="p_Cancel">True to cancel the editing and return to normal mode, false to call automatically ApplyEdit and terminate editing</param>
		/// <returns>Returns true if the cell terminate the editing mode</returns>
		public override bool InternalEndEdit(bool p_Cancel)
		{
			if (IsEditing)
			{
				bool l_bSuccess = true;
				if (p_Cancel==false)
					l_bSuccess = InternalApplyEdit();

				if (l_bSuccess)
				{
					if (EditCell == null)
					{
						//This case must never be called !!!
						System.Diagnostics.Debug.Assert(false); //non dovrebbe succedere ma in debug un paio di volte è capitato ma non sono riuscito a riprodurlo
					}
					else
					{
						GridVirtual l_Grid = EditCell.Grid;
						GridSubPanel l_Panel = l_Grid.PanelAtPosition(EditPosition);
						//di fatto mettendo questa property a null termina logicamente l'edit
						// e è importante che venga fatto appena possibile (in particolare prima della chiamata a SetFocusOnGridSubPanel perchè altrimenti questa chiamerebbe nuovamente EndEdit(false)
						SetEditCell(null, Position.Empty);

						Control l_EditorControl = null;
						if (IsAttached(l_Panel))
							l_EditorControl = GetEditorControl(l_Panel);

						if (l_EditorControl!=null)
						{
#if !MINI
							//se il controllo ha il focus, metto il focus sulle cella in modo da forzare un eventuale validate, se il controllo non ha il fuoco il validate dovrebbe essere già stato chiamato
							if (l_Grid != null && l_EditorControl.ContainsFocus )
								l_Grid.SetFocusOnCells();
#else
							//se il controllo ha il focus, metto il focus sulle cella in modo da forzare un eventuale validate, se il controllo non ha il fuoco il validate dovrebbe essere già stato chiamato
							if (l_Grid != null && l_EditorControl.Focused)
								l_Grid.SetFocusOnCells();
#endif

							l_Grid.LinkedControls.Remove(l_EditorControl);
							l_EditorControl.Hide();
						}
						else
						{
							//This case must never be called !!!
							System.Diagnostics.Debug.Assert(false);
						}
					}
				}
				else //if the ApplyEdit failed
				{
					if (EditCell != null)
					{
						GridVirtual l_Grid = EditCell.Grid;
						GridSubPanel l_Panel = l_Grid.PanelAtPosition(EditPosition);
						if (IsAttached(l_Panel))
						{
							Control l_EditorControl = null;
							l_EditorControl = GetEditorControl(l_Panel);
#if !MINI
							if (l_EditorControl != null && l_EditorControl.ContainsFocus == false)
								l_EditorControl.Focus();
#else
							if (l_EditorControl != null && l_EditorControl.Focused == false)
								l_EditorControl.Focus();
#endif
						}
					}
				}

				return l_bSuccess;
			}
			else
				return true;
		}


		/// <summary>
		/// Validated of the editor control
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		protected virtual void InnerControl_Validated(object sender, EventArgs e)
		{
			try
			{
				if (IsEditing)
					EditCell.EndEdit(false);
			}
			catch(Exception err)
			{
				OnEditException(new EditExceptionEventArgs(err));
			}
		}

		/// <summary>
		/// Returns the value inserted with the current editor control
		/// </summary>
		/// <returns></returns>
		public override abstract object GetEditedValue();
	}

}
