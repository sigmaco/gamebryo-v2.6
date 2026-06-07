using System;
using SourceGrid2.Cells;

namespace SourceGrid2
{
	/// <summary>
	/// A collection of elements of type ICellVirtual
	/// </summary>
	public class CellVirtualCollection: System.Collections.CollectionBase
	{
		/// <summary>
		/// Initializes a new empty instance of the CellBaseCollection class.
		/// </summary>
		public CellVirtualCollection()
		{
			// empty
		}

		/// <summary>
		/// Initializes a new instance of the CellBaseCollection class, containing elements
		/// copied from an array.
		/// </summary>
		/// <param name="items">
		/// The array whose elements are to be added to the new CellBaseCollection.
		/// </param>
		public CellVirtualCollection(ICellVirtual[] items)
		{
			this.AddRange(items);
		}

		/// <summary>
		/// Initializes a new instance of the CellBaseCollection class, containing elements
		/// copied from another instance of CellBaseCollection
		/// </summary>
		/// <param name="items">
		/// The CellBaseCollection whose elements are to be added to the new CellBaseCollection.
		/// </param>
		public CellVirtualCollection(CellVirtualCollection items)
		{
			this.AddRange(items);
		}

		/// <summary>
		/// Adds the elements of an array to the end of this CellBaseCollection.
		/// </summary>
		/// <param name="items">
		/// The array whose elements are to be added to the end of this CellBaseCollection.
		/// </param>
		public virtual void AddRange(ICellVirtual[] items)
		{
			foreach (ICellVirtual item in items)
			{
				this.List.Add(item);
			}
		}

		/// <summary>
		/// Adds the elements of another CellBaseCollection to the end of this CellBaseCollection.
		/// </summary>
		/// <param name="items">
		/// The CellBaseCollection whose elements are to be added to the end of this CellBaseCollection.
		/// </param>
		public virtual void AddRange(CellVirtualCollection items)
		{
			foreach (ICellVirtual item in items)
			{
				this.List.Add(item);
			}
		}

		/// <summary>
		/// Adds an instance of type ICellVirtual to the end of this CellBaseCollection.
		/// </summary>
		/// <param name="value">
		/// The ICellVirtual to be added to the end of this CellBaseCollection.
		/// </param>
		public virtual void Add(ICellVirtual value)
		{
			this.List.Add(value);
		}

		/// <summary>
		/// Determines whether a specfic ICellVirtual value is in this CellBaseCollection.
		/// </summary>
		/// <param name="value">
		/// The ICellVirtual value to locate in this CellBaseCollection.
		/// </param>
		/// <returns>
		/// true if value is found in this CellBaseCollection;
		/// false otherwise.
		/// </returns>
		public virtual bool Contains(ICellVirtual value)
		{
			return this.List.Contains(value);
		}

		/// <summary>
		/// Return the zero-based index of the first occurrence of a specific value
		/// in this CellBaseCollection
		/// </summary>
		/// <param name="value">
		/// The ICellVirtual value to locate in the CellBaseCollection.
		/// </param>
		/// <returns>
		/// The zero-based index of the first occurrence of the _ELEMENT value if found;
		/// -1 otherwise.
		/// </returns>
		public virtual int IndexOf(ICellVirtual value)
		{
			return this.List.IndexOf(value);
		}

		/// <summary>
		/// Inserts an element into the CellBaseCollection at the specified index
		/// </summary>
		/// <param name="index">
		/// The index at which the ICellVirtual is to be inserted.
		/// </param>
		/// <param name="value">
		/// The ICellVirtual to insert.
		/// </param>
		public virtual void Insert(int index, ICellVirtual value)
		{
			this.List.Insert(index, value);
		}

		/// <summary>
		/// Gets or sets the ICellVirtual at the given index in this CellBaseCollection.
		/// </summary>
		public virtual ICellVirtual this[int index]
		{
			get
			{
				return (ICellVirtual) this.List[index];
			}
			set
			{
				this.List[index] = value;
			}
		}

		/// <summary>
		/// Removes the first occurrence of a specific ICellVirtual from this CellBaseCollection.
		/// </summary>
		/// <param name="value">
		/// The ICellVirtual value to remove from this CellBaseCollection.
		/// </param>
		public virtual void Remove(ICellVirtual value)
		{
			this.List.Remove(value);
		}

		/// <summary>
		/// Type-specific enumeration class, used by CellBaseCollection.GetEnumerator.
		/// </summary>
		public class Enumerator: System.Collections.IEnumerator
		{
			private System.Collections.IEnumerator wrapped;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="collection"></param>
			public Enumerator(CellVirtualCollection collection)
			{
				this.wrapped = ((System.Collections.CollectionBase)collection).GetEnumerator();
			}

			/// <summary>
			/// 
			/// </summary>
			public ICellVirtual Current
			{
				get
				{
					return (ICellVirtual) (this.wrapped.Current);
				}
			}

			/// <summary>
			/// 
			/// </summary>
			object System.Collections.IEnumerator.Current
			{
				get
				{
					return (ICellVirtual) (this.wrapped.Current);
				}
			}

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			public bool MoveNext()
			{
				return this.wrapped.MoveNext();
			}

			/// <summary>
			/// 
			/// </summary>
			public void Reset()
			{
				this.wrapped.Reset();
			}
		}

		/// <summary>
		/// Returns an enumerator that can iterate through the elements of this CellBaseCollection.
		/// </summary>
		/// <returns>
		/// An object that implements System.Collections.IEnumerator.
		/// </returns>        
		public new virtual CellVirtualCollection.Enumerator GetEnumerator()
		{
			return new CellVirtualCollection.Enumerator(this);
		}
	}
}
