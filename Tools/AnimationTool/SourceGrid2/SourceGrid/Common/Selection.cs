using System;
using System.Drawing;
using System.Collections;
using System.Windows.Forms;
using System.Runtime.Serialization;
using SourceLibrary.Windows.Forms;
using System.ComponentModel;

namespace SourceGrid2
{
	/// <summary>
	/// Represent the selected cells of the grid.
	/// </summary>
	public class Selection : ICollection
	{
		#region Member Variables
		private GridRangeCollection m_RangeList = new GridRangeCollection();
		private GridVirtual m_Grid;
		#endregion

		#region Constructor
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_Grid"></param>
		public Selection(GridVirtual p_Grid)
		{
			m_Grid = p_Grid;

#if !MINI
			m_iImageCut = m_MenuImageList.Images.Count;
			m_MenuImageList.Images.Add(CommonImages.Cut);

			m_iImageCopy = m_MenuImageList.Images.Count;
			m_MenuImageList.Images.Add(CommonImages.Copy);

			m_iImagePaste = m_MenuImageList.Images.Count;
			m_MenuImageList.Images.Add(CommonImages.Paste);

			m_iImageClear = m_MenuImageList.Images.Count;
			m_MenuImageList.Images.Add(CommonImages.Clear);

			m_iImageFormatCells = m_MenuImageList.Images.Count;
			m_MenuImageList.Images.Add(CommonImages.Properties);

			m_SelectionColor = Color.FromArgb(75, Color.FromKnownColor(KnownColor.Highlight));
#endif
		}

		#endregion

		#region Grid
		/// <summary>
		/// Linked grid
		/// </summary>
		public GridVirtual Grid
		{
			get{return m_Grid;}
		}

		#endregion

		#region GetCells
		/// <summary>
		/// Returns the union of all the selected range as Position collection. This method doesn't return span cells; if for example grid[0,0] has a Span Column = 2 and the range selected is [0,0] -> [0,1] this method returns only 0,0.
		/// </summary>
		/// <returns></returns>
		public virtual PositionCollection GetCellsPositions()
		{
			if (m_PositionListCache==null)
			{
				m_PositionListCache = new PositionCollection();
				for (int i = 0; i < m_RangeList.Count; i++)
				{
					PositionCollection l_tmp = m_RangeList[i].GetCellsPositions();
					for (int j = 0; j < l_tmp.Count; j++)
					{
						//search for the real position
						Position pos = Grid.PositionToStartPosition(l_tmp[j]);

						if (m_PositionListCache.Contains(pos)==false)
							m_PositionListCache.Add(pos);
					}
				}
			}

			return m_PositionListCache;
		}

		/// <summary>
		/// Returns the union of all the selected range as Position collection
		/// </summary>
		/// <returns></returns>
		public virtual CellVirtualCollection GetCells()
		{
			if (m_CellBaseListCache==null)
			{
				m_CellBaseListCache = new CellVirtualCollection();
				PositionCollection l_Positions = GetCellsPositions();
				for (int i = 0; i < l_Positions.Count; i++)
				{
					m_CellBaseListCache.Add(m_Grid.GetCell(l_Positions[i]));
				}
			}

			return m_CellBaseListCache;
		}

		#endregion

		#region Indexer
		/// <summary>
		/// Returns the cell at the specific position
		/// </summary>
		public Range this[int index]
		{
			get{return m_RangeList[index];}
		}


		/// <summary>
		/// Searches for the specified Cell and returns the zero-based index of the first occurrence that starts at the specified index and contains the specified number of elements.
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <returns></returns>
		public int IndexOf(Cells.ICellVirtual p_Cell)
		{
			return GetCells().IndexOf(p_Cell);
		}
		#endregion

		#region Contains
		/// <summary>
		/// Indicates if the specified cell is selected
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <returns></returns>
		public virtual bool Contains(Position p_Cell)
		{
			if (p_Cell.IsEmpty())
				return false;

			for (int r = 0; r < m_RangeList.Count; r++)
			{
				if ( this[r].Contains(p_Cell) )
					return true;
			}

			return false;
		}

		/// <summary>
		/// Indicates if the specified range of cells is selected
		/// </summary>
		/// <param name="p_Range"></param>
		/// <returns></returns>
		public virtual bool Contains(Range p_Range)
		{
			if (Count<=0)
				return false;

			//prima cerco se è presente un range esattamente come quello richiesto
			if (m_RangeList.Contains(p_Range))
				return true;

			//se non ho trovato uguale provo a cercare cella per cella
			PositionCollection l_SearchList = p_Range.GetCellsPositions();
			for (int i = 0; i < l_SearchList.Count; i++)
			{
				bool l_bFound = false;
				for (int r = 0; r < Count; r++)
				{
					if (this[r].Contains(l_SearchList[i]))
					{
						l_bFound = true;
						break;
					}
				}
				if (l_bFound==false)
					return false;
			}

			return true;
		}

		/// <summary>
		/// Indicates if the specified row is selected
		/// </summary>
		/// <param name="p_Row"></param>
		/// <returns></returns>
		public virtual bool ContainsRow(int p_Row)
		{
			for (int r = 0; r < m_RangeList.Count; r++)
			{
				if ( this[r].ContainsRow(p_Row) )
					return true;
			}

			return false;
		}
		/// <summary>
		/// Indicates if the specified column is selected
		/// </summary>
		/// <param name="p_Column"></param>
		/// <returns></returns>
		public virtual bool ContainsColumn(int p_Column)
		{
			for (int r = 0; r < m_RangeList.Count; r++)
			{
				if ( this[r].ContainsColumn(p_Column) )
					return true;
			}

			return false;
		}
		#endregion

		#region Add/Remove/Clear

		private PositionCollection m_PositionListCache = null;
		private CellVirtualCollection m_CellBaseListCache = null;

		private void ClearCache()
		{
			m_CellBaseListCache = null;
			m_PositionListCache = null;
		}

		/// <summary>
		/// deseleziona tutte le celle tranne quella passata in input
		/// </summary>
		/// <param name="p_CellLeaveThisCellSelected"></param>
		public void Clear(Position p_CellLeaveThisCellSelected)
		{
			if (Count>0)
			{
				m_RangeList.Clear();
			
				m_RangeList.Add( new Range(p_CellLeaveThisCellSelected) );

				OnSelectionChange(new SelectionChangeEventArgs(SelectionChangeEventType.Clear, Range.Empty));
			}
		}

		/// <summary>
		/// Deselect all the cells
		/// </summary>
		public virtual void Clear()
		{
			if (Count>0)
			{
				m_RangeList.Clear();

				OnSelectionChange(new SelectionChangeEventArgs(SelectionChangeEventType.Clear, Range.Empty));
			}
		}

		/// <summary>
		/// Select the specified cell and add the cell to the collection.
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <returns></returns>
		public void Add(Position p_Cell)
		{
			AddRange( new Range(p_Cell) );
		}

		/// <summary>
		/// Select the specified Range of cells
		/// </summary>
		/// <param name="p_Range"></param>
		public void AddRange(Range p_Range)
		{
			if (p_Range.IsEmpty() == false)
			{
				Range l_RangeToSelect = p_Range;

				//Apply SelectionMode
				if (m_SelMode == GridSelectionMode.Row)
				{
					if (m_Grid.ColumnsCount>0)
						l_RangeToSelect = new Range(p_Range.Start.Row, 0, p_Range.End.Row, m_Grid.ColumnsCount-1);
				}
				else if (m_SelMode == GridSelectionMode.Col)
				{
					if (m_Grid.RowsCount>0)
						l_RangeToSelect = new Range(0, p_Range.Start.Column, m_Grid.RowsCount-1, p_Range.End.Column);
				}

				//I optimize the case of one cell selected because is the most common case
				if (Count == 1 && this[0].Contains(l_RangeToSelect) )
				{
					//do nothing, the range is already contained 
				}
				else if (Count == 1 && l_RangeToSelect.Contains(this[0])) //if the range to select contains the old selected cells
				{
					m_RangeList[0] = l_RangeToSelect;
					OnSelectionChange(new SelectionChangeEventArgs(SelectionChangeEventType.Add, l_RangeToSelect));
				}
				else if (Contains(l_RangeToSelect) == false)
				{
					m_RangeList.Add(l_RangeToSelect);
					OnSelectionChange(new SelectionChangeEventArgs(SelectionChangeEventType.Add, l_RangeToSelect));
				}
			}
		}
		
		/// <summary>
		/// Deselect and remove from the collection the specified range of cells
		/// </summary>
		/// <param name="p_Range"></param>
		public void RemoveRange(Range p_Range)
		{
			if (p_Range.IsEmpty() == false)
			{
				Range l_RangeToDeselect = p_Range;

				//Apply SelectionMode
				if (m_SelMode == GridSelectionMode.Row)
				{
					if (m_Grid.ColumnsCount>0)
						l_RangeToDeselect = new Range(p_Range.Start.Row, 0, p_Range.End.Row, m_Grid.ColumnsCount-1);
				}
				else if (m_SelMode == GridSelectionMode.Col)
				{
					if (m_Grid.RowsCount>0)
						l_RangeToDeselect = new Range(0, p_Range.Start.Column, m_Grid.RowsCount-1, p_Range.End.Column);
				}

				//TODO bisognerebbe ottimizzare questo metodo
				// per ora scompatto i range correnti in tante celle e poi le celle contenute nel range da deselezionare le rimuovo
				PositionCollection l_CurrentRanges = GetCellsPositions();
				m_RangeList.Clear();
				bool l_bFound = false;
				for (int i = 0; i < l_CurrentRanges.Count; i++)
				{
					if (l_RangeToDeselect.Contains(l_CurrentRanges[i])==false)
						m_RangeList.Add( new Range(l_CurrentRanges[i]) );
					else
						l_bFound = true;
				}

				if (l_bFound)
					OnSelectionChange(new SelectionChangeEventArgs(SelectionChangeEventType.Remove, l_RangeToDeselect));
			}
		}
		/// <summary>
		/// Deselect and remove from the collection the specified cell
		/// </summary>
		/// <param name="p_Cell"></param>
		public void Remove(Position p_Cell)
		{
			RemoveRange( new Range(p_Cell) );
		}

		#endregion

		#region Invalidate
		/// <summary>
		/// Invalidate all the selected cells
		/// </summary>
		public virtual void Invalidate()
		{
			for (int i = 0;i < Count; i++)
				m_Grid.InvalidateRange(this[i]);
		}

		#endregion

		#region SelectionChange event
		/// <summary>
		/// Fired when a cell is added from the selection or removed from the selection
		/// </summary>
		public event SelectionChangeEventHandler SelectionChange;

		/// <summary>
		/// Fired when a cell is added from the selection or removed from the selection
		/// </summary>
		/// <param name="e"></param>
		protected virtual void OnSelectionChange(SelectionChangeEventArgs e)
		{
//			#warning Temporaneo
//			if (e.EventType == SelectionChangeEventType.Add)
//				System.Diagnostics.Debug.WriteLine("Selection.AddRange " + e.Range.ToString());
//			else if (e.EventType == SelectionChangeEventType.Remove)
//				System.Diagnostics.Debug.WriteLine("Selection.RemoveRange " + e.Range.ToString());
//			else if (e.EventType == SelectionChangeEventType.Clear)
//				System.Diagnostics.Debug.WriteLine("Selection.Clear");

			ClearCache();

			//If I add a selection range and the Count is equals to 2 I must remove the selection border (see also DrawSelectionMask method)
			if (e.EventType == SelectionChangeEventType.Add && Count >= 2)
			{
				Invalidate();
			}
			else if (e.EventType == SelectionChangeEventType.Add || e.EventType == SelectionChangeEventType.Remove)
			{
				m_Grid.InvalidateRange(e.Range);
			}
			else //clear
				m_Grid.InvalidateCells();

			if (SelectionChange!=null)
				SelectionChange(this,e);
		}

		#endregion

		#region SelectionMode
		private GridSelectionMode m_SelMode = GridSelectionMode.Cell;
		/// <summary>
		/// Selection type
		/// </summary>
		public GridSelectionMode SelectionMode
		{
			get{return m_SelMode;}
			set{m_SelMode = value;}
		}

		private bool m_bEnableMultiSelection = true;

		/// <summary>
		/// True=Enable multi selection with the Ctrl key or Shift Key or with mouse.
		/// </summary>
		public bool EnableMultiSelection
		{
			get{return m_bEnableMultiSelection;}
			set{m_bEnableMultiSelection = value;}
		}

		#endregion

		#region Range
		/// <summary>
		/// Returns the range of the current selection. If the user has selected non contiguous cells this method returns a range to contains all the selected cells.
		/// </summary>
		/// <returns></returns>
		public virtual Range GetRange()
		{
			if (Count > 0)
			{
				int l_row1 = int.MaxValue;
				int l_col1 = int.MaxValue;
				int l_row2 = int.MinValue;
				int l_col2 = int.MinValue;
				foreach ( Range r in this)
				{
					if (l_row1 > r.Start.Row)
						l_row1 = r.Start.Row;

					if (l_col1 > r.Start.Column)
						l_col1 = r.Start.Column;

					if (l_row2 < r.Start.Row)
						l_row2 = r.Start.Row;

					if (l_col2 < r.Start.Column)
						l_col2 = r.Start.Column;


					if (l_row1 > r.End.Row)
						l_row1 = r.End.Row;

					if (l_col1 > r.End.Column)
						l_col1 = r.End.Column;

					if (l_row2 < r.End.Row)
						l_row2 = r.End.Row;

					if (l_col2 < r.End.Column)
						l_col2 = r.End.Column;
				}

				return new Range(l_row1, l_col1, l_row2, l_col2);
			}
			else
				return Range.Empty;
		}

		#endregion

		#region Selected Rows/Columns
		/// <summary>
		/// Returns an array of the rows selected
		/// </summary>
		public RowInfo[] SelectedRows
		{
			get
			{
				ArrayList l_List = new ArrayList();
				Range l_Range = GetRange();
				if (l_Range.IsEmpty() == false)
				{
					for (int r = l_Range.Start.Row; r <= l_Range.End.Row; r++)
					{
						if (ContainsRow(r))
							l_List.Add(Grid.Rows[r]);
					}
				}
				RowInfo[] ret = new RowInfo[l_List.Count];
				for (int r = 0; r < ret.Length; r++)
					ret[r] = (RowInfo)(l_List[r]);

				return ret;
			}
		}
		/// <summary>
		/// Returns an array of the columns selected
		/// </summary>
		public ColumnInfo[] SelectedColumns
		{
			get
			{
				ArrayList l_List = new ArrayList();
				Range l_Range = GetRange();
				if (l_Range.IsEmpty() == false)
				{
					for (int c = l_Range.Start.Column; c <= l_Range.End.Column; c++)
					{
						if (ContainsColumn(c))
							l_List.Add(Grid.Columns[c]);
					}
				}
				ColumnInfo[] ret = new ColumnInfo[l_List.Count];
				for (int c = 0; c < ret.Length; c++)
					ret[c] = (ColumnInfo)(l_List[c]);

				return ret;
			}
		}
		#endregion

		#region ContextMenu
#if !MINI
		#region ImageContextMenu
		//image for menu
		private ImageList m_MenuImageList = new ImageList();

		private int m_iImageCut;
		private int m_iImageCopy;
		private int m_iImagePaste;
		private int m_iImageClear;
		private int m_iImageFormatCells;
		#endregion
#endif
		/// <summary>
		/// Returns the ContextMenu used when the user Right-Click on a selected cell.
		/// </summary>
		/// <returns></returns>
		public virtual MenuCollection GetContextMenus()
		{
			MenuCollection l_Array = new MenuCollection();

			bool l_EnableCopyPasteSelection = false;
			if ( (m_Grid.ContextMenuStyle & ContextMenuStyle.CopyPasteSelection) == ContextMenuStyle.CopyPasteSelection)
				l_EnableCopyPasteSelection = true;

			bool l_EnableClearSelection = false;
			if ( (m_Grid.ContextMenuStyle & ContextMenuStyle.ClearSelection) == ContextMenuStyle.ClearSelection)
				l_EnableClearSelection = true;

//			bool l_EnablePropertySelection = false;
//			if ( (m_Grid.ContextMenuStyle & ContextMenuStyle.PropertySelection) == ContextMenuStyle.PropertySelection)
//				l_EnablePropertySelection = true;

			if (m_ContextMenuItems!=null && m_ContextMenuItems.Count > 0)
			{
				foreach(MenuItem m in m_ContextMenuItems)
					l_Array.Add(m);

				if (l_EnableClearSelection || l_EnableCopyPasteSelection ) //|| l_EnablePropertySelection)
				{
					MenuItem l_MenuItem = new MenuItem();
					l_MenuItem.Text = "-";
					l_Array.Add(l_MenuItem);
				}
			}

#if !MINI
			if (l_EnableCopyPasteSelection)
			{
//				//CUT (not implemented)
//				MenuItem l_mnCut = new MenuItemImage("Cut", new EventHandler(Selection_Cut), m_MenuImageList, m_iImageCut);
//				l_mnCut.Enabled = false;
//				l_Array.Add(l_mnCut);

				//COPY 
				MenuItem l_mnCopy = new MenuItemImage("Copy", new EventHandler(Selection_Copy), m_MenuImageList, m_iImageCopy);
				l_Array.Add(l_mnCopy);

				//PASTE
				MenuItem l_mnPaste = new MenuItemImage("Paste", new EventHandler(Selection_Paste), m_MenuImageList, m_iImagePaste);
				l_mnPaste.Enabled = IsValidClipboardForPaste();
				l_Array.Add(l_mnPaste);
			}
#endif

			if (l_EnableClearSelection)
			{
				if (l_EnableCopyPasteSelection)// && l_EnablePropertySelection)
				{
					MenuItem l_MenuItem = new MenuItem();
					l_MenuItem.Text = "-";
					l_Array.Add(l_MenuItem);
				}

#if !MINI
				MenuItem l_mnClear = new MenuItemImage("Clear", new EventHandler(Selection_ClearValues), m_MenuImageList, m_iImageClear);
#else
				MenuItem l_mnClear = new MenuItem();
				l_mnClear.Text = "Clear";
				l_mnClear.Click += new EventHandler(Selection_ClearValues);
#endif
				l_Array.Add(l_mnClear);
			}
//			if (l_EnablePropertySelection)
//			{
//				MenuItem l_mnFormatCells = new MenuItem("Format Cells ...", new EventHandler(Selection_FormatCells));
//				m_Grid.SetMenuImage(l_mnFormatCells,m_iImageFormatCells);
//				l_Array.Add(l_mnFormatCells);
//			}

			return l_Array;
		}


		private MenuCollection m_ContextMenuItems = null;

		/// <summary>
		/// ContextMenu of the selected cells. Null if no contextmenu is active.
		/// </summary>
		public MenuCollection ContextMenuItems
		{
			get{return m_ContextMenuItems;}
			set{m_ContextMenuItems = value;}
		}

		#endregion

#if !MINI
		#region Clipboard
		private bool m_bAutoCopyPaste = true;
		/// <summary>
		/// True to enable the default copy/paste operations
		/// </summary>
		public bool AutoCopyPaste
		{
			get{return m_bAutoCopyPaste;}
			set{m_bAutoCopyPaste = value;}
		}

		/// <summary>
		/// Copy event
		/// </summary>
		public event EventHandler ClipboardCopy;
		/// <summary>
		/// Paste Event
		/// </summary>
		public event EventHandler ClipboardPaste;
		/// <summary>
		/// Cut event
		/// </summary>
		public event EventHandler ClipboardCut;

		/// <summary>
		/// Cut the content of the selected cells. NOT YET IMPLEMENTED.
		/// </summary>
		public virtual void OnClipboardCut()
		{
			try
			{
				if (ClipboardCut!=null)
					ClipboardCut(this,EventArgs.Empty);
			}
			catch(Exception err)
			{
				MessageBox.Show(err.Message,"Clipboard copy error");
			}		
		}
		/// <summary>
		/// Copy the content of the selected cells
		/// </summary>
		public virtual void OnClipboardCopy()
		{
			try
			{
				if (ClipboardCopy!=null)
					ClipboardCopy(this,EventArgs.Empty);

				if (m_bAutoCopyPaste)
				{
					if (Count>0)
					{
						//Clipboard text format
						Range l_Range = GetRange();
						System.Text.StringBuilder l_TabBuffer = new System.Text.StringBuilder();
						for (int r = l_Range.Start.Row; r <= l_Range.End.Row; r++)
						{
							for (int c = l_Range.Start.Column;c <= l_Range.End.Column; c++)
							{
								//devo controllare che la cella sia selezionata perchè la find trova soltanto gli estremi
								if ( m_Grid.GetCell(r,c) != null && m_Grid.Selection.Contains(new Position(r,c)) )
								{
									if ( m_Grid.GetCell(r,c).DataModel != null)
										l_TabBuffer.Append(m_Grid.GetCell(r,c).DataModel.ValueToString(m_Grid.GetCell(r,c).GetValue(new Position(r,c)) ));
									else
										l_TabBuffer.Append(m_Grid.GetCell(r,c).GetDisplayText(new Position(r,c)));
									//l_TabBuffer.Append(m_Grid[r,c].Value.ToString());
								}

								if (c<l_Range.End.Column)
								{
									l_TabBuffer.Append("\t");
								}
							}
							if (r<l_Range.End.Row)
							{
								l_TabBuffer.Append("\x0D\x0A");
							}
						}
						DataObject l_dataObj = new DataObject();
						l_dataObj.SetData(DataFormats.Text,l_TabBuffer.ToString());

						Clipboard.SetDataObject(l_dataObj,true);
					}
				}
			}
			catch(Exception err)
			{
				MessageBox.Show(err.Message,"Clipboard copy error");
			}
		}
		/// <summary>
		/// Paste the content of the selected cells
		/// </summary>
		public virtual void OnClipboardPaste()
		{
			try
			{
				if (ClipboardPaste!=null)
					ClipboardPaste(this,EventArgs.Empty);

				if (m_bAutoCopyPaste)
				{
					if (IsValidClipboardForPaste() && Count > 0)
					{
						IDataObject l_dtObj = Clipboard.GetDataObject();
						string l_buffer = (string)l_dtObj.GetData(DataFormats.Text,true);
						//tolgo uno dei due caratteri di a capo per usare lo split
						l_buffer = l_buffer.Replace("\x0D\x0A","\x0A");
						string[] l_buffRows = l_buffer.Split('\x0A','\x0D');

						Range l_Range = GetRange();
						for (int r = l_Range.Start.Row; r < Math.Min(l_Range.Start.Row+l_buffRows.Length,m_Grid.RowsCount); r++)
						{
							if (l_buffRows[r-l_Range.Start.Row].Length>0)
							{
								string[] l_buffCols = l_buffRows[r-l_Range.Start.Row].Split('\t');
								for (int c = l_Range.Start.Column; c < Math.Min(l_Range.Start.Column+l_buffCols.Length,m_Grid.ColumnsCount); c++)
								{
									Cells.ICellVirtual l_Cell = m_Grid.GetCell(r,c);
									if (l_Cell != null && l_Cell.DataModel != null)
									{
										l_Cell.DataModel.SetCellValue(l_Cell, new Position(r,c), l_buffCols[c-l_Range.Start.Column]);
									}
								}
							}
						}
					}
				}
			}
			catch(Exception err)
			{
				MessageBox.Show(err.Message,"Clipboard paste error");
			}
		}

		/// <summary>
		/// Returns if the current content of the Clipboard is valid for Paste operations
		/// </summary>
		/// <returns></returns>
		public virtual bool IsValidClipboardForPaste()
		{
			IDataObject l_dtObj = Clipboard.GetDataObject();
			return l_dtObj.GetDataPresent(DataFormats.Text,true);
		}

		private void Selection_Cut(object sender, EventArgs e)
		{
			OnClipboardCut();
		}
		private void Selection_Paste(object sender, EventArgs e)
		{
			OnClipboardPaste();
		}
		private void Selection_Copy(object sender, EventArgs e)
		{
			OnClipboardCopy();
		}
		#endregion

		#region Draw Properties and Methods

		private Color m_FocusBackColor = Color.Transparent;

		/// <summary>
		/// BackColor of the cell with the Focus. Default is Color.Transparent.
		/// </summary>
		public Color FocusBackColor
		{
			get{return m_FocusBackColor;}
			set{m_FocusBackColor = value;Invalidate();}
		}

		private Color m_SelectionColor;

		/// <summary>
		/// Selection backcolor. Usually is a color with a transparent value so you can see the color of the cell. Default is: Color.FromArgb(75, Color.FromKnownColor(KnownColor.Highlight))
		/// </summary>
		public Color BackColor
		{
			get{return m_SelectionColor;}
			set{m_SelectionColor = value;Invalidate();}
		}

		private RectangleBorder m_SelectionBorder = new RectangleBorder(new Border(Color.Black, 2));

		/// <summary>
		/// Border of the selection. Default is new RectangleBorder(new Border(Color.Black, 2));
		/// </summary>
		public RectangleBorder Border
		{
			get{return m_SelectionBorder;}
			set{m_SelectionBorder = value;Invalidate();}
		}

		private SelectionBorderMode m_SelectionBorderMode = SelectionBorderMode.Auto;

		/// <summary>
		/// Style of the selection border. Default is Auto.
		/// </summary>
		public SelectionBorderMode BorderMode
		{
			get{return m_SelectionBorderMode;}
			set{m_SelectionBorderMode = value;Invalidate();}
		}

		private SelectionMaskStyle m_MaskStyle = SelectionMaskStyle.Default;

		public SelectionMaskStyle MaskStyle
		{
			get{return m_MaskStyle;}
			set{m_MaskStyle = value;Invalidate();}
		}

		/// <summary>
		/// Draw the selection using the SelectionColor property over the selected cells. Draw a Border around the selection using Border and BorderMode properties.
		/// </summary>
		/// <param name="p_Panel"></param>
		/// <param name="e"></param>
		/// <param name="pRangeToRedraw">The range of cells that must be redrawed. Consider that can contains also not selected cells.</param>
		public virtual void DrawSelectionMask(GridSubPanel p_Panel, PaintEventArgs e, Range pRangeToRedraw)
		{
			if (Count == 0)
				return;

			Region oldClip = e.Graphics.Clip;
			SolidBrush brushFillMask = new SolidBrush(BackColor);
			try
			{
				e.Graphics.Clip = new Region(e.ClipRectangle);

				Rectangle rectFocus = Rectangle.Empty;
				if (m_FocusPosition.IsEmpty() == false)
					rectFocus = p_Panel.RectangleGridToPanel( Grid.PositionToDisplayRect(m_FocusPosition) );
				Cells.ICellVirtual cellFocus = Grid.GetCell(m_FocusPosition);

				//Draw selection mask and border
				//Draw each cell separately
				if ( (m_MaskStyle & SelectionMaskStyle.DrawOnlyInitializedCells) == SelectionMaskStyle.DrawOnlyInitializedCells)
				{
					PositionCollection selectedCells = GetCellsPositions();
					for (int i = 0; i < selectedCells.Count; i++)
					{
						//if must be redrawed, is is not the cell with the focus and contains a cell
						if (pRangeToRedraw.Contains(selectedCells[i]) && selectedCells[i] != m_FocusPosition &&
							Grid.GetCell(selectedCells[i]) != null)
						{
							Rectangle rect = p_Panel.RectangleGridToPanel( Grid.PositionToDisplayRect(selectedCells[i]) );
							e.Graphics.FillRectangle(brushFillMask, rect);
						}
					}
				}
				else //draw all the selected ranges (Default)
				{
					for (int i = 0; i < Count; i++)
					{
						Range range = this[i];
						if (range.IntersectsWith(pRangeToRedraw))
						{
							Rectangle rect = p_Panel.RectangleGridToPanel( Grid.RangeToDisplayRect(range) );

							if (range.Contains(m_FocusPosition))
							{
								Region region = new Region(rect);
								region.Exclude( rectFocus );
								e.Graphics.FillRegion(brushFillMask, region);
							}
							else
								e.Graphics.FillRectangle(brushFillMask, rect);

							//Draw the selection border of the range
							if (  m_SelectionBorderMode == SelectionBorderMode.Selection)
								GridPaint.DrawBorder(e.Graphics, rect, m_SelectionBorder);
							else if (m_SelectionBorderMode == SelectionBorderMode.Auto)
							{
								//only one range or the range with the focus and not when in editing mode
								if ( (Count == 1 || range.Contains(m_FocusPosition) ) && 
									(cellFocus == null || cellFocus.IsEditing(m_FocusPosition) == false) )
									GridPaint.DrawBorder(e.Graphics, rect, m_SelectionBorder);
							}
						}
					}
				}

				
				//Draw focus mask and focus border (only if there is a fucus cell and is not in editng mode)
				if (cellFocus != null && cellFocus.IsEditing(m_FocusPosition) == false &&
					pRangeToRedraw.Contains(m_FocusPosition))
				{
					if (m_FocusBackColor != Color.Transparent)
					{
						using (Brush focusBrush = new SolidBrush(m_FocusBackColor))
						{
							e.Graphics.FillRectangle(focusBrush, rectFocus);
						}
					}

					if (m_SelectionBorderMode == SelectionBorderMode.FocusCell)
					{
						GridPaint.DrawBorder(e.Graphics, rectFocus, m_SelectionBorder);
					}
				}

			}
			finally
			{
				brushFillMask.Dispose();
				e.Graphics.Clip = oldClip;
			}
		}

		#endregion
#endif

        #region FinishOccurred event
        /// <summary>
        /// Fired when Finish of selection occurs
        /// </summary>
        public event FinishOccurredEventHandler FinishOccurred;

        /// <summary>
        /// Fired when finish of selection occurrs
        /// </summary>
        /// <param name="e"></param>
        protected virtual void OnFinishOccurred(FinishOccurredEventArgs e)
        {
            if (FinishOccurred!=null)
                FinishOccurred(this,e);
        }

        public void Finish()
        {
            OnFinishOccurred(new FinishOccurredEventArgs(m_RangeList));
        }
        #endregion


		#region Focus Cell
		private Position m_FocusPosition = Position.Empty;

		/// <summary>
		/// Returns the cell with the focus.
		/// </summary>
		public Position FocusPosition
		{
			get{return m_FocusPosition;}
		}

		/// <summary>
		/// Change the focus of the grid. 
		/// The calls order is: 
		/// 
		/// (the user select CellX) 
		/// CellX.FocusEntering
		/// Grid.CellGotFocus(CellX), 
		/// CellX.FocusEntered, 
		/// [OnFocusRowEntered],
		/// [OnFocusColumnEntered]
		/// 
		/// (the user select CellY), 
		/// CellY.FocusEntering 
		/// CellX.FocusLeaving
		/// Grid.CellLostFocus(CellX), 
		/// [OnFocusRowLeaving],
		/// [OnFocusColumnLeaving],
		/// CellX.FocusLeft,
		/// Grid.CellGotFocus(CellY), 
		/// CellY.FocusEntered,
		/// [OnFocusRowEntered],
		/// [OnFocusColumnEntered]
		/// 
		/// Use Position.Empty to remove the focus cell.
		/// </summary>
		/// <param name="cellToSetFocus"></param>
		/// <returns></returns>
		public virtual bool Focus(Position pCellToSetFocus)
		{
			//Check the control key status
#if !MINI
			bool l_bControlPress = ((Control.ModifierKeys & Keys.Control) == Keys.Control &&
				(Grid.SpecialKeys & GridSpecialKeys.Control) == GridSpecialKeys.Control);
#else
			bool l_bControlPress = false;
#endif

			//If control key is pressed, enableMultiSelection is true and the cell that will receive the focus is not empty leave the cell selected otherwise deselect other cells
			bool deselectOtherCells = false;
			if (pCellToSetFocus.IsEmpty() == false && (l_bControlPress == false || EnableMultiSelection == false) ) 
				deselectOtherCells = true;

			pCellToSetFocus = Grid.PositionToStartPosition( pCellToSetFocus );

            if (l_bControlPress && Contains(pCellToSetFocus))
            {
                // Has this cell already been selected...?
                Range r = new Range(pCellToSetFocus);
                Remove(pCellToSetFocus);
                Invalidate();
                OnSelectionChange(new SelectionChangeEventArgs(
                    SelectionChangeEventType.Remove, r));

                //LostFocus Event Arguments
                Cells.ICellVirtual oldCellFocus = Grid.GetCell(FocusPosition);
                CellLostFocusEventArgs lostFocusEventArgs = new CellLostFocusEventArgs(FocusPosition, oldCellFocus, pCellToSetFocus);

                if (oldCellFocus != null)
                {
                    //Cell Focus Leaving
                    oldCellFocus.OnFocusLeaving(lostFocusEventArgs);
                    if (lostFocusEventArgs.Cancel)
                        return false;

                    //Cell Lost Focus
                    OnCellLostFocus(lostFocusEventArgs);
                    if (lostFocusEventArgs.Cancel)
                        return false;
                }

                return false;
            }

            if (pCellToSetFocus != FocusPosition)
			{
				//GotFocus Event Arguments
				Cells.ICellVirtual newCellToFocus = Grid.GetCell(pCellToSetFocus);
				CellGotFocusEventArgs gotFocusEventArgs = new CellGotFocusEventArgs(pCellToSetFocus, newCellToFocus, FocusPosition);

				//LostFocus Event Arguments
				Cells.ICellVirtual oldCellFocus = Grid.GetCell(FocusPosition);
				CellLostFocusEventArgs lostFocusEventArgs = new CellLostFocusEventArgs(FocusPosition, oldCellFocus, pCellToSetFocus);

				if (newCellToFocus != null )
				{
					//Cell Focus Entering
					newCellToFocus.OnFocusEntering(gotFocusEventArgs);
					if (gotFocusEventArgs.Cancel)
						return false;

					//If the cell can't receive the focus stop the focus operation
					if (newCellToFocus.CanReceiveFocus == false)
						return false;
				}

				if (oldCellFocus != null)
				{
					//Cell Focus Leaving
					oldCellFocus.OnFocusLeaving(lostFocusEventArgs);
					if (lostFocusEventArgs.Cancel)
						return false;

					//Cell Lost Focus
					OnCellLostFocus(lostFocusEventArgs);
					if (lostFocusEventArgs.Cancel)
						return false;
				}

				//Deselect previous selected cells
				if (deselectOtherCells)
					Clear();

				if (newCellToFocus != null )
				{
					//Cell Got Focus
					OnCellGotFocus(gotFocusEventArgs);

					return (!gotFocusEventArgs.Cancel);
				}
				else
					return true;
			}
			else
			{
				//Deselect previous selected cells
				if (deselectOtherCells)
					Clear( pCellToSetFocus );

				//put anyway the focus on the cells and force the selection of the focused cell (to reapply selection rule, SelectionMode = Row/Column)
				if (pCellToSetFocus.IsEmpty() == false)
				{
					Add(pCellToSetFocus);
					Grid.SetFocusOnCells();
				}

				return true;
			}
		}

		/// <summary>
		/// Fired before a cell receive the focus (FocusCell is populated after this event, use e.Cell to read the cell that will receive the focus)
		/// </summary>
		public event CellGotFocusEventHandler CellGotFocus;

		/// <summary>
		/// Fired before a cell lost the focus
		/// </summary>
		public event CellLostFocusEventHandler CellLostFocus;

		#region Focus Column/Row event and properties
		/// <summary>
		/// Fired before a row lost the focus
		/// </summary>
		public event RowCancelEventHandler FocusRowLeaving;
		/// <summary>
		/// Fired after a row receive the focus
		/// </summary>
		public event RowEventHandler FocusRowEntered;
		/// <summary>
		/// Fired before a column lost the focus
		/// </summary>
		public event ColumnCancelEventHandler FocusColumnLeaving;
		/// <summary>
		/// Fired after a column receive the focus
		/// </summary>
		public event ColumnEventHandler FocusColumnEntered;

		/// <summary>
		/// Fired before a row lost the focus
		/// </summary>
		/// <param name="e"></param>
		protected virtual void OnFocusRowLeaving(RowCancelEventArgs e)
		{
			if (FocusRowLeaving != null)
				FocusRowLeaving(this, e);
		}
		/// <summary>
		/// Fired after a row receive the focus
		/// </summary>
		/// <param name="e"></param>
		protected virtual void OnFocusRowEntered(RowEventArgs e)
		{
			if (FocusRowEntered != null)
				FocusRowEntered(this, e);
		}
		/// <summary>
		/// Fired before a column lost the focus
		/// </summary>
		/// <param name="e"></param>
		protected virtual void OnFocusColumnLeaving(ColumnCancelEventArgs e)
		{
			if (FocusColumnLeaving != null)
				FocusColumnLeaving(this, e);
		}
		/// <summary>
		/// Fired after a column receive the focus
		/// </summary>
		/// <param name="e"></param>
		protected virtual void OnFocusColumnEntered(ColumnEventArgs e)
		{
			if (FocusColumnEntered != null)
				FocusColumnEntered(this, e);
		}

		/// <summary>
		/// Returns the row that have the focus. If no row is selected return null.
		/// </summary>
		[Browsable(false),DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public RowInfo FocusRow
		{
			get
			{
				if (FocusPosition.IsEmpty() || FocusPosition.Row >= Grid.RowsCount)
					return null;
				return Grid.Rows[FocusPosition.Row];
			}
		}
		/// <summary>
		/// Returns the column that have the focus. If no column is selected return null.
		/// </summary>
		[Browsable(false),DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public ColumnInfo FocusColumn
		{
			get
			{
				if (FocusPosition.IsEmpty() || FocusPosition.Column >= Grid.ColumnsCount)
					return null;
				return Grid.Columns[FocusPosition.Column];
			}
		}
		#endregion

		/// <summary>
		/// Fired when a cell receive the focus
		/// </summary>
		/// <param name="e"></param>
		protected virtual void OnCellGotFocus(CellGotFocusEventArgs e)
		{
			if (e.Cancel)
				return;

			//Evento Got Focus
			if (CellGotFocus != null)
				CellGotFocus(this,e);
			if (e.Cancel)
				return;

			e.Cancel = !(Grid.SetFocusOnCells());
			if (e.Cancel)
				return;

			//N.B. E' importante impostare prima la variabile m_FocusCell e dopo chiamare l'evento OnEnter, altrimenti nel caso in cui la cella sia impostata in edit sul focus, l'eseguzione va in loop (cerca di fare l'edit ma per far questo è necessario avere il focus ...)
			m_FocusPosition = e.Position; //Set the focus on the cell

			//Select the cell
            Add(m_FocusPosition);

			//Invalidate the selection
			Invalidate();

			//Cell Focus Entered
			e.Cell.OnFocusEntered(e);

			//Column/Row Focus Enter
			//If the row is different from the previous row, fire a row focus entered
			if (e.Position.Row != e.OldFocusPosition.Row)
				OnFocusRowEntered(new RowEventArgs(FocusRow));
			//If the column is different from the previous column, fire a column focus entered
			if (e.Position.Column != e.OldFocusPosition.Column)
				OnFocusColumnEntered(new ColumnEventArgs(FocusColumn));
		}

		/// <summary>
		/// Fired when a cell lost the focus
		/// </summary>
		/// <param name="e"></param>
		protected virtual void OnCellLostFocus(CellLostFocusEventArgs e)
		{
			if (e.Cancel)
				return;

			bool l_FocusContainer = true;
#if !MINI
			if (Grid.ContainsFocus) // se la griglia ha il Focus cerco di impostare il focus sulle celle (il codice in realtà lo imposta su un pannello) in modo da forzare un eventuale Validated di qualche controllo figlio. Questo bisogna farlo però solo se la griglia ha il focus altrimenti sposto il focus senza motivo sulla griglia.
				l_FocusContainer = Grid.SetFocusOnCells(); //questo scatena un EndEdit sul Validate dell'editor (anche se successivamente io chiamo un endEdit forzatamente)
#endif

			if (l_FocusContainer)
			{
				//Stop the Edit operation
				if (e.Cell.EndEdit(false) == false)
					e.Cancel = true;
			}
			else
				e.Cancel = true;
			
			if (e.Cancel)
				return;

			//evento Lost Focus
			if (CellLostFocus != null)
				CellLostFocus(this,e);
			if (e.Cancel)
				return;

			//Row/Column leaving
			//If the new Row is different from the current focus row calls a Row Leaving event
			RowInfo focusRow = FocusRow;
			if (focusRow.Index != e.NewFocusPosition.Row)
			{
				RowCancelEventArgs rowArgs = new RowCancelEventArgs(focusRow);
				OnFocusRowLeaving(rowArgs);
				if (rowArgs.Cancel)
				{
					e.Cancel = true;
					return;
				}
			}
			//If the new Row is different from the current focus row calls a Row Leaving event
			ColumnInfo focusColumn = FocusColumn;
			if (focusColumn.Index != e.NewFocusPosition.Column)
			{
				ColumnCancelEventArgs columnArgs = new ColumnCancelEventArgs(focusColumn);
				OnFocusColumnLeaving(columnArgs);
				if (columnArgs.Cancel)
				{
					e.Cancel = true;
					return;
				}
			}

			//Change the focus cell to Empty
			m_FocusPosition = Position.Empty; //from now the cell doesn't have the focus
			//Cell Focus Left
			e.Cell.OnFocusLeft(e);
		}

		private FocusStyle m_FocusStyle = FocusStyle.Default;

		/// <summary>
		/// Specify the behavior of the focus and selection. Default is FocusStyle.None.
		/// </summary>
		public FocusStyle FocusStyle
		{
			get{return m_FocusStyle;}
			set{m_FocusStyle = value;}
		}
		#endregion

		#region ClearValues
		private void Selection_ClearValues(object sender, EventArgs e)
		{
			ClearValues();
		}
		/// <summary>
		/// Clear all the selected cells with a valid Model.
		/// </summary>
		public virtual void ClearValues()
		{
			try
			{
				if (ClearCells!=null)
					ClearCells(this, EventArgs.Empty);

				if (AutoClear)
				{
					PositionCollection l_Cells = GetCellsPositions();
					foreach(Position c in l_Cells)
					{
						if (Grid.GetCell(c)!=null)
						{
							if (Grid.GetCell(c).DataModel != null)
								Grid.GetCell(c).DataModel.ClearCell(Grid.GetCell(c), c);
						}
					}
				}
			}
			catch(Exception err)
			{
				MessageBox.Show(err.Message,"Clear error");
			}
		}
	
		private bool m_bAutoClear = true;
		/// <summary>
		/// True to enable the default clear operation
		/// </summary>
		public bool AutoClear
		{
			get{return m_bAutoClear;}
			set{m_bAutoClear = value;}
		}


		/// <summary>
		/// Clear event
		/// </summary>
		public event EventHandler ClearCells;

		#endregion

		#region ICollection Members

		/// <summary>
		/// 
		/// </summary>
		public bool IsSynchronized
		{
			get
			{
				return ((ICollection)m_RangeList).IsSynchronized;
			}
		}

		/// <summary>
		/// 
		/// </summary>
		public int Count
		{
			get
			{
				return m_RangeList.Count;
			}
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="array"></param>
		/// <param name="index"></param>
		public void CopyTo(Array array, int index)
		{
			((ICollection)m_RangeList).CopyTo(array,index);
		}

		/// <summary>
		/// 
		/// </summary>
		public object SyncRoot
		{
			get
			{
				return ((ICollection)m_RangeList).SyncRoot;
			}
		}

		#endregion

		#region IEnumerable Members

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		public IEnumerator GetEnumerator()
		{
			return m_RangeList.GetEnumerator();
		}

		#endregion
	}

}
