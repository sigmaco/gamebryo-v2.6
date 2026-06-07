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

namespace SegmentedTrackBarDLL
{
    public class Collection: System.Collections.CollectionBase, IDisposable
    {
        private void SetItemCollection(CollectionItem item)
        {
            if (item.Collection != null && item.Collection != this)
                item.Collection.Remove(item);
            item.collection = this;
        }

        private void EnsureItemNotNull(CollectionItem item)
        {
            if (item == null)
                throw new ArgumentNullException("Item cannot be null");
        }

        //IDisposable
        protected virtual void Dispose(Boolean disposing)
        {
            foreach(CollectionItem item in List)
                ((IDisposable)item).Dispose();
        }

        void IDisposable.Dispose()
        {
            lock(this)
            {
                GC.SuppressFinalize(this);
                Dispose(true);
            }
        }

        ~Collection()
        {
            Dispose(false);
        }

        public Int32 Add(CollectionItem item)
        {
            EnsureItemNotNull(item);
            SetItemCollection(item);
            return List.Add(item);
        }

        public void Insert(Int32 index, CollectionItem item)
        {
            EnsureItemNotNull(item);
            SetItemCollection(item);
            List.Insert(index, item);
        }

        public void Remove(CollectionItem item)
        {
            EnsureItemNotNull(item);
            if (item.Collection == this)
                item.collection = null;
            List.Remove(item);
        }

        public bool Contains(CollectionItem item)
        {
            return List.Contains(item);
        }

        public Int32 IndexOf(CollectionItem item)
        {
            return List.IndexOf(item);
        }

        public void CopyTo(CollectionItem[] array, Int32 index)
        {
            List.CopyTo(array, index);
        }

        public CollectionItem this[Int32 index]
        {
            get
            {
                return (CollectionItem)List[index];
            }
            set
            {
                List[index] = value;
            }
        }
    }
}
