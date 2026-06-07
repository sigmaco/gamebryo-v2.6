using System;

namespace SourceGrid2.BehaviorModels
{
	/// <summary>
	/// A collection of elements of type IBehaviorModel
	/// </summary>
	public class BehaviorModelCollection: System.Collections.CollectionBase
	{
		/// <summary>
		/// Initializes a new empty instance of the BehaviorModelCollection class.
		/// </summary>
		public BehaviorModelCollection()
		{
			// empty
		}

		/// <summary>
		/// Initializes a new instance of the BehaviorModelCollection class, containing elements
		/// copied from an array.
		/// </summary>
		/// <param name="items">
		/// The array whose elements are to be added to the new BehaviorModelCollection.
		/// </param>
		public BehaviorModelCollection(IBehaviorModel[] items)
		{
			this.AddRange(items);
		}

		/// <summary>
		/// Initializes a new instance of the BehaviorModelCollection class, containing elements
		/// copied from another instance of BehaviorModelCollection
		/// </summary>
		/// <param name="items">
		/// The BehaviorModelCollection whose elements are to be added to the new BehaviorModelCollection.
		/// </param>
		public BehaviorModelCollection(BehaviorModelCollection items)
		{
			this.AddRange(items);
		}

		/// <summary>
		/// Adds the elements of an array to the end of this BehaviorModelCollection.
		/// </summary>
		/// <param name="items">
		/// The array whose elements are to be added to the end of this BehaviorModelCollection.
		/// </param>
		public virtual void AddRange(IBehaviorModel[] items)
		{
			foreach (IBehaviorModel item in items)
			{
				this.List.Add(item);
			}
		}

		/// <summary>
		/// Adds the elements of another BehaviorModelCollection to the end of this BehaviorModelCollection.
		/// </summary>
		/// <param name="items">
		/// The BehaviorModelCollection whose elements are to be added to the end of this BehaviorModelCollection.
		/// </param>
		public virtual void AddRange(BehaviorModelCollection items)
		{
			foreach (IBehaviorModel item in items)
			{
				this.List.Add(item);
			}
		}

		/// <summary>
		/// Adds an instance of type IBehaviorModel to the end of this BehaviorModelCollection. If null the item is not added
		/// </summary>
		/// <param name="value">
		/// The IBehaviorModel to be added to the end of this BehaviorModelCollection.
		/// </param>
		public virtual void Add(IBehaviorModel value)
		{
			if (value!=null)
				this.List.Add(value);
		}

		/// <summary>
		/// Determines whether a specfic IBehaviorModel value is in this BehaviorModelCollection.
		/// </summary>
		/// <param name="value">
		/// The IBehaviorModel value to locate in this BehaviorModelCollection.
		/// </param>
		/// <returns>
		/// true if value is found in this BehaviorModelCollection;
		/// false otherwise.
		/// </returns>
		public virtual bool Contains(IBehaviorModel value)
		{
			return this.List.Contains(value);
		}

		/// <summary>
		/// Return the zero-based index of the first occurrence of a specific value
		/// in this BehaviorModelCollection
		/// </summary>
		/// <param name="value">
		/// The IBehaviorModel value to locate in the BehaviorModelCollection.
		/// </param>
		/// <returns>
		/// The zero-based index of the first occurrence of the _ELEMENT value if found;
		/// -1 otherwise.
		/// </returns>
		public virtual int IndexOf(IBehaviorModel value)
		{
			return this.List.IndexOf(value);
		}

		/// <summary>
		/// Inserts an element into the BehaviorModelCollection at the specified index
		/// </summary>
		/// <param name="index">
		/// The index at which the IBehaviorModel is to be inserted.
		/// </param>
		/// <param name="value">
		/// The IBehaviorModel to insert.
		/// </param>
		public virtual void Insert(int index, IBehaviorModel value)
		{
			if (value!=null)
				this.List.Insert(index, value);
		}

		/// <summary>
		/// Gets or sets the IBehaviorModel at the given index in this BehaviorModelCollection.
		/// </summary>
		public virtual IBehaviorModel this[int index]
		{
			get
			{
				return (IBehaviorModel) this.List[index];
			}
			set
			{
				this.List[index] = value;
			}
		}

		/// <summary>
		/// Removes the first occurrence of a specific IBehaviorModel from this BehaviorModelCollection.
		/// </summary>
		/// <param name="value">
		/// The IBehaviorModel value to remove from this BehaviorModelCollection.
		/// </param>
		public virtual void Remove(IBehaviorModel value)
		{
			this.List.Remove(value);
		}

		/// <summary>
		/// Type-specific enumeration class, used by BehaviorModelCollection.GetEnumerator.
		/// </summary>
		public class Enumerator: System.Collections.IEnumerator
		{
			private System.Collections.IEnumerator wrapped;

			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="collection"></param>
			public Enumerator(BehaviorModelCollection collection)
			{
				this.wrapped = ((System.Collections.CollectionBase)collection).GetEnumerator();
			}

			/// <summary>
			/// 
			/// </summary>
			public IBehaviorModel Current
			{
				get
				{
					return (IBehaviorModel) (this.wrapped.Current);
				}
			}

			/// <summary>
			/// 
			/// </summary>
			object System.Collections.IEnumerator.Current
			{
				get
				{
					return (IBehaviorModel) (this.wrapped.Current);
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
		/// Returns an enumerator that can iterate through the elements of this BehaviorModelCollection.
		/// </summary>
		/// <returns>
		/// An object that implements System.Collections.IEnumerator.
		/// </returns>        
		public new virtual BehaviorModelCollection.Enumerator GetEnumerator()
		{
			return new BehaviorModelCollection.Enumerator(this);
		}
	}

}
