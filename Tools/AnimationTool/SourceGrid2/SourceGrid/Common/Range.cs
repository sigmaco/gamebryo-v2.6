using System;
using System.Drawing;

namespace SourceGrid2
{
	/// <summary>
	/// Represents range of cells. Once created connot be modified. This Range has always Start in the Top-Left, and End in the Bottom-Right (see Normalize method).
	/// </summary>
	public struct Range
	{
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_Start"></param>
		/// <param name="p_End"></param>
		public Range(Position p_Start, Position p_End):this(p_Start, p_End, true)
		{
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_StartRow"></param>
		/// <param name="p_StartCol"></param>
		/// <param name="p_EndRow"></param>
		/// <param name="p_EndCol"></param>
		public Range(int p_StartRow, int p_StartCol, int p_EndRow, int p_EndCol)
		{
			m_Start = new Position(p_StartRow, p_StartCol);
			m_End = new Position(p_EndRow, p_EndCol);

			Normalize();
		}

		private Range(Position p_Start, Position p_End, bool p_bCheck)
		{
			m_Start = p_Start;
			m_End = p_End;

			if (p_bCheck)
				Normalize();
		}

		static Range()
		{
			Empty = new Range(Position.Empty, Position.Empty, false);
		}

		private Position m_Start, m_End;

		public Position Start
		{
			get{return m_Start;}
			//set{m_Start = value; Normalize();}
		}

		public Position End
		{
			get{return m_End;}
			//set{m_End = value; Normalize();}
		}


		/// <summary>
		/// Move the current range to the specified position, leaving the current ColumnsCount and RowsCount
		/// </summary>
		/// <param name="p_StartPosition"></param>
		public void MoveTo(Position p_StartPosition)
		{
			int l_ColCount = ColumnsCount;
			int l_RowCount = RowsCount;
			m_Start = p_StartPosition;
			RowsCount = l_RowCount;
			ColumnsCount = l_ColCount;
		}

		/// <summary>
		/// Sets or Gets the columns count (End.Column-Start.Column)
		/// </summary>
		public int ColumnsCount
		{
			get
			{
				return (m_End.Column - m_Start.Column)+1;
			}
			set
			{
				if (value<=0) //facendo questo controllo non serve richiamare Normalize
					throw new SourceGridException("Invalid columns count");
				m_End = new Position(m_End.Row, m_Start.Column+value-1);
			}
		}

		/// <summary>
		/// Sets or Gets the rows count (End.Row-Start.Row)
		/// </summary>
		public int RowsCount
		{
			get
			{
				return (m_End.Row - m_Start.Row)+1;
			}
			set
			{
				if (value<=0) //facendo questo controllo non serve richiamare Normalize
					throw new SourceGridException("Invalid columns count");
				m_End = new Position(m_Start.Row+value-1, m_End.Column);
			}
		}

		/// <summary>
		/// Construct a Range of a single cell
		/// </summary>
		/// <param name="p_SinglePosition"></param>
		public Range(Position p_SinglePosition):this(p_SinglePosition, p_SinglePosition, false)
		{
		}

		/// <summary>
		/// Represents an empty range
		/// </summary>
		public readonly static Range Empty;

		/// <summary>
		/// Check and fix the range to always have Start smaller than End
		/// </summary>
		private void Normalize()
		{
			int l_MinRow, l_MinCol, l_MaxRow, l_MaxCol;
			
			if (m_Start.Row < m_End.Row)
				l_MinRow = m_Start.Row;
			else
				l_MinRow = m_End.Row;

			if (m_Start.Column < m_End.Column)
				l_MinCol = m_Start.Column;
			else
				l_MinCol = m_End.Column;


			if (m_Start.Row > m_End.Row)
				l_MaxRow = m_Start.Row;
			else
				l_MaxRow = m_End.Row;

			if (m_Start.Column > m_End.Column)
				l_MaxCol = m_Start.Column;
			else
				l_MaxCol = m_End.Column;

			m_Start = new Position(l_MinRow, l_MinCol);
			m_End = new Position(l_MaxRow, l_MaxCol);
		}
		/// <summary>
		/// Returns true if the specified row is present in the current range.
		/// </summary>
		/// <param name="p_Row"></param>
		/// <returns></returns>
		public bool ContainsRow(int p_Row)
		{
			return (p_Row >= m_Start.Row &&
				p_Row <= m_End.Row);
		}
		/// <summary>
		/// Returns true if the specified column is present in the current range.
		/// </summary>
		/// <param name="p_Col"></param>
		/// <returns></returns>
		public bool ContainsColumn(int p_Col)
		{
			return (p_Col >= m_Start.Column &&
				p_Col <= m_End.Column);
		}
		/// <summary>
		/// Returns true if the specified cell position is present in the current range.
		/// </summary>
		/// <param name="p_Position"></param>
		/// <returns></returns>
		public bool Contains(Position p_Position)
		{
			int l_Row = p_Position.Row;
			int l_Col = p_Position.Column;

			return (l_Row >= m_Start.Row &&
					l_Col >= m_Start.Column &&
					l_Row <= m_End.Row &&
					l_Col <= m_End.Column);
		}

		/// <summary>
		/// Returns true if the specified range is present in the current range.
		/// </summary>
		/// <param name="p_Range"></param>
		/// <returns></returns>
		public bool Contains(Range p_Range)
		{
			return (Contains(p_Range.Start) &&
					Contains(p_Range.End));
		}

		/// <summary>
		/// Determines if the current range is empty
		/// </summary>
		/// <returns></returns>
		public bool IsEmpty()
		{
			return (Start.IsEmpty() || End.IsEmpty());
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="Left"></param>
		/// <param name="Right"></param>
		/// <returns></returns>
		public static bool operator == (Range Left, Range Right)
		{
			return Left.Equals(Right);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="Left"></param>
		/// <param name="Right"></param>
		/// <returns></returns>
		public static bool operator != (Range Left, Range Right)
		{
			return !Left.Equals(Right);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		public override int GetHashCode()
		{
			return m_Start.Row;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="p_Range"></param>
		/// <returns></returns>
		public bool Equals(Range p_Range)
		{
			return (Start.Equals(p_Range.Start) && End.Equals(p_Range.End));
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="obj"></param>
		/// <returns></returns>
		public override bool Equals(object obj)
		{
			return Equals((Range)obj);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		public PositionCollection GetCellsPositions()
		{
			PositionCollection l_List = new PositionCollection();
			for (int r = Start.Row; r <= End.Row; r++)
				for (int c = Start.Column; c <= End.Column; c++)
					l_List.Add(new Position(r,c));

			return l_List;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		public override string ToString()
		{
			return Start.ToString() + " to " + End.ToString();
		}


		/// <summary>
		/// Returns a range with the smaller Start and the bigger End. The Union of the 2 Range. If one of the range is empty then the return is the other range.
		/// </summary>
		/// <param name="p_Range1"></param>
		/// <param name="p_Range2"></param>
		/// <returns></returns>
		public static Range Union(Range p_Range1, Range p_Range2)
		{
			if (p_Range1.IsEmpty())
				return p_Range2;
			else if (p_Range2.IsEmpty())
				return p_Range1;
			else
				return new Range(Position.MergeMinor(p_Range1.Start, p_Range2.Start),
							Position.MergeMajor(p_Range1.End, p_Range2.End), false);
		}

		/// <summary>
		/// Returns the intersection between the 2 Range. If one of the range is empty then the return is empty.
		/// </summary>
		/// <param name="p_Range1"></param>
		/// <param name="p_Range2"></param>
		/// <returns></returns>
		public static Range Intersect(Range p_Range1, Range p_Range2)
		{
			if (p_Range1.IsEmpty() || p_Range2.IsEmpty())
				return Range.Empty;

			Position startNew = Position.MergeMajor(p_Range1.Start, p_Range2.Start);
			Position endNew = Position.MergeMinor(p_Range1.End, p_Range2.End);

			if (startNew.Column > endNew.Column ||
				startNew.Row > endNew.Row)
				return Range.Empty;
			else
				return new Range(startNew, endNew, false);
		}

		/// <summary>
		/// Returns the intersection between the 2 Range. If one of the range is empty then the return is empty.
		/// </summary>
		/// <param name="p_Range"></param>
		/// <returns></returns>
		public Range Intersect(Range p_Range)
		{
			return Intersect(this, p_Range);
		}

		/// <summary>
		/// Returns true if the specified range intersects (one or more cells) with the current range.
		/// If one of the range is empty then the return is false.
		/// </summary>
		/// <param name="p_Range1"></param>
		/// <param name="p_Range2"></param>
		/// <returns></returns>
		public static bool IntersectsWith(Range p_Range1, Range p_Range2)
		{
			return Intersect(p_Range1, p_Range2).IsEmpty() == false;
		}

		/// <summary>
		/// Returns true if the specified range intersects (one or more cells) with the current range.
		/// If one of the range is empty then the return is false.
		/// </summary>
		/// <param name="p_Range"></param>
		/// <returns></returns>
		public bool IntersectsWith(Range p_Range)
		{
			return IntersectsWith(this, p_Range);
		}
	}

	/// <summary>
	/// Interface that rappresent a range of the grid. (RangeFullGridNoFixedRows, RangeFullGridNoFixedCols, RangeFixedRows, RangeFixedCols, Range)
	/// </summary>
	public interface IRangeLoader
	{
		/// <summary>
		/// Rectangle that contains the range.
		/// </summary>
		Range GetRange(GridVirtual p_grid);
	}

	/// <summary>
	/// Represents a range that contains all the grid
	/// </summary>
	public class RangeFullGrid : IRangeLoader 
	{
		/// <summary>
		/// Constructor
		/// </summary>
		public RangeFullGrid()
		{
		}

		/// <summary>
		/// Returns the Range struct from the specific instance
		/// </summary>
		/// <param name="p_Grid"></param>
		/// <returns></returns>
		public Range GetRange(GridVirtual p_Grid)
		{
			return p_Grid.CompleteRange;
		}
	}

	/// <summary>
	/// Represents a range that contains all the grid with no fixed rows
	/// </summary>
	public class RangeFullGridNoFixedRows : IRangeLoader 
	{
		/// <summary>
		/// Constructor
		/// </summary>
		public RangeFullGridNoFixedRows()
		{
		}

		/// <summary>
		/// Returns the Range struct from the specific instance
		/// </summary>
		/// <param name="p_Grid"></param>
		/// <returns></returns>
		public Range GetRange(GridVirtual p_Grid)
		{
			if (p_Grid.RowsCount>=p_Grid.FixedRows)
				return new Range(p_Grid.FixedRows,0,p_Grid.RowsCount-1,p_Grid.ColumnsCount-1);
			else
				return Range.Empty;
		}
	}
	/// <summary>
	/// Represents a range that contains all the grid with no fixed cols
	/// </summary>
	public class RangeFullGridNoFixedCols : IRangeLoader 
	{
		/// <summary>
		/// Constructor
		/// </summary>
		public RangeFullGridNoFixedCols()
		{
		}

		/// <summary>
		/// Returns the Range struct from the specific instance
		/// </summary>
		/// <param name="p_Grid"></param>
		/// <returns></returns>
		public Range GetRange(GridVirtual p_Grid)
		{
			if (p_Grid.ColumnsCount >= p_Grid.FixedColumns)
				return new Range(0,p_Grid.FixedColumns,p_Grid.RowsCount-1, p_Grid.ColumnsCount-1);
			else
				return Range.Empty;
		}
	}


	/// <summary>
	/// Represents a range that contains only fixed rows
	/// </summary>
	public class RangeFixedRows : IRangeLoader 
	{
		/// <summary>
		/// Constructor
		/// </summary>
		public RangeFixedRows()
		{
		}

		/// <summary>
		/// Returns the Range struct from the specific instance
		/// </summary>
		/// <param name="p_Grid"></param>
		/// <returns></returns>
		public Range GetRange(GridVirtual p_Grid)
		{
			if (p_Grid.RowsCount>=p_Grid.FixedRows)
				return new Range(0,0,p_Grid.FixedRows,p_Grid.ColumnsCount-1);
			else
				return Range.Empty;
		}
	}
	/// <summary>
	/// Represents a range that contains only fixed cols
	/// </summary>
	public class RangeFixedCols : IRangeLoader 
	{
		/// <summary>
		/// Constructor
		/// </summary>
		public RangeFixedCols()
		{
		}

		/// <summary>
		/// Returns the Range struct from the specific instance
		/// </summary>
		/// <param name="p_Grid"></param>
		/// <returns></returns>
		public Range GetRange(GridVirtual p_Grid)
		{
			if (p_Grid.ColumnsCount >= p_Grid.FixedColumns)
				return new Range(0, 0, p_Grid.RowsCount-1, p_Grid.FixedColumns);
			else
				return Range.Empty;
		}
	}

	/// <summary>
	/// Range custom
	/// </summary>
	public class RangeLoader : IRangeLoader
	{
		private Range m_GridRange;
		/// <summary>
		/// Constructor
		/// </summary>
		public RangeLoader(Range p_GridRange)
		{
			m_GridRange = p_GridRange;
		}

		/// <summary>
		/// Range
		/// </summary>
		public Range CellRange
		{
			get{return m_GridRange;}
			set{m_GridRange = value;}
		}

		/// <summary>
		/// Returns the Range struct from the specific instance
		/// </summary>
		/// <param name="p_Grid"></param>
		/// <returns></returns>
		public virtual Range GetRange(GridVirtual p_Grid)
		{
			return m_GridRange;
		}
	}



	/// <summary>
	/// A collection of elements of type Range
	/// </summary>
	public class GridRangeCollection: System.Collections.CollectionBase
	{
		/// <summary>
		/// Initializes a new empty instance of the GridRangeCollection class.
		/// </summary>
		public GridRangeCollection()
		{
			// empty
		}

		/// <summary>
		/// Initializes a new instance of the GridRangeCollection class, containing elements
		/// copied from an array.
		/// </summary>
		/// <param name="items">
		/// The array whose elements are to be added to the new GridRangeCollection.
		/// </param>
		public GridRangeCollection(Range[] items)
		{
			this.AddRange(items);
		}

		/// <summary>
		/// Initializes a new instance of the GridRangeCollection class, containing elements
		/// copied from another instance of GridRangeCollection
		/// </summary>
		/// <param name="items">
		/// The GridRangeCollection whose elements are to be added to the new GridRangeCollection.
		/// </param>
		public GridRangeCollection(GridRangeCollection items)
		{
			this.AddRange(items);
		}

		/// <summary>
		/// Adds the elements of an array to the end of this GridRangeCollection.
		/// </summary>
		/// <param name="items">
		/// The array whose elements are to be added to the end of this GridRangeCollection.
		/// </param>
		public virtual void AddRange(Range[] items)
		{
			foreach (Range item in items)
			{
				this.List.Add(item);
			}
		}

		/// <summary>
		/// Adds the elements of another GridRangeCollection to the end of this GridRangeCollection.
		/// </summary>
		/// <param name="items">
		/// The GridRangeCollection whose elements are to be added to the end of this GridRangeCollection.
		/// </param>
		public virtual void AddRange(GridRangeCollection items)
		{
			foreach (Range item in items)
			{
				this.List.Add(item);
			}
		}

		/// <summary>
		/// Adds an instance of type Range to the end of this GridRangeCollection.
		/// </summary>
		/// <param name="value">
		/// The Range to be added to the end of this GridRangeCollection.
		/// </param>
		public virtual void Add(Range value)
		{
			this.List.Add(value);
		}

		/// <summary>
		/// Determines whether a specfic Range value is in this GridRangeCollection.
		/// </summary>
		/// <param name="value">
		/// The Range value to locate in this GridRangeCollection.
		/// </param>
		/// <returns>
		/// true if value is found in this GridRangeCollection;
		/// false otherwise.
		/// </returns>
		public virtual bool Contains(Range value)
		{
			return this.List.Contains(value);
		}

		/// <summary>
		/// Return the zero-based index of the first occurrence of a specific value
		/// in this GridRangeCollection
		/// </summary>
		/// <param name="value">
		/// The Range value to locate in the GridRangeCollection.
		/// </param>
		/// <returns>
		/// The zero-based index of the first occurrence of the _ELEMENT value if found;
		/// -1 otherwise.
		/// </returns>
		public virtual int IndexOf(Range value)
		{
			return this.List.IndexOf(value);
		}

		/// <summary>
		/// Inserts an element into the GridRangeCollection at the specified index
		/// </summary>
		/// <param name="index">
		/// The index at which the Range is to be inserted.
		/// </param>
		/// <param name="value">
		/// The Range to insert.
		/// </param>
		public virtual void Insert(int index, Range value)
		{
			this.List.Insert(index, value);
		}

		/// <summary>
		/// Gets or sets the Range at the given index in this GridRangeCollection.
		/// </summary>
		public virtual Range this[int index]
		{
			get
			{
				return (Range) this.List[index];
			}
			set
			{
				this.List[index] = value;
			}
		}

		/// <summary>
		/// Removes the first occurrence of a specific Range from this GridRangeCollection.
		/// </summary>
		/// <param name="value">
		/// The Range value to remove from this GridRangeCollection.
		/// </param>
		public virtual void Remove(Range value)
		{
			this.List.Remove(value);
		}

		/// <summary>
		/// Type-specific enumeration class, used by GridRangeCollection.GetEnumerator.
		/// </summary>
		public class Enumerator: System.Collections.IEnumerator
		{
			private System.Collections.IEnumerator wrapped;

			public Enumerator(GridRangeCollection collection)
			{
				this.wrapped = ((System.Collections.CollectionBase)collection).GetEnumerator();
			}

			public Range Current
			{
				get
				{
					return (Range) (this.wrapped.Current);
				}
			}

			object System.Collections.IEnumerator.Current
			{
				get
				{
					return (Range) (this.wrapped.Current);
				}
			}

			public bool MoveNext()
			{
				return this.wrapped.MoveNext();
			}

			public void Reset()
			{
				this.wrapped.Reset();
			}
		}

		/// <summary>
		/// Returns an enumerator that can iterate through the elements of this GridRangeCollection.
		/// </summary>
		/// <returns>
		/// An object that implements System.Collections.IEnumerator.
		/// </returns>        
		public new virtual GridRangeCollection.Enumerator GetEnumerator()
		{
			return new GridRangeCollection.Enumerator(this);
		}
	}

}
