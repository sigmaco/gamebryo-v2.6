// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2008 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

using System;
using System.Collections;
using System.Text.RegularExpressions; 
using System.Windows.Forms;

namespace AnimationToolUI
{
    /// <summary>
    /// ColumnSort class is used to ensure that data gets sorted correctly.
    /// In particular, numerical data will not get sorted simply by ascii
    /// value which is the default sorter.
    /// </summary>
    public class ColumnSort : IComparer
    {
        public ColumnSort()
        {
            m_iColumnToSort = 0; // Initialize the column to '0'

            // Initialize the sort order to 'Ascending'
            m_kOrderOfSort = SortOrder.Ascending;
            m_kObjectCompare = new NumberCaseInsensitiveComparer();
            m_kFirstObjectCompare = new ImageTextComparer();
        }

        private int m_iColumnToSort;
        
        /// <summary>
        /// Specifies the order in which to sort (i.e. 'Ascending').
        /// </summary>
        private SortOrder m_kOrderOfSort;

        /// <summary>
        /// Case insensitive comparer object
        /// </summary> 
        private NumberCaseInsensitiveComparer m_kObjectCompare;
        
        private ImageTextComparer m_kFirstObjectCompare;

        /// <summary>
        /// This method is inherited from the IComparer interface.
        /// It compares the two objects passed using a 
        /// case insensitive comparison.
        /// </summary>
        /// <param name="x">First object to be compared</param>
        /// <param name="y">Second object to be compared</param>
        /// <returns>The result of the comparison. "0" if equal,
        /// negative if 'x' is less than 'y' and positive
        /// if 'x' is greater than 'y'</returns>
        public int Compare(object x, object y)
        {
            int iCompareResult;
            ListViewItem kListViewX, kListViewY;
            // Cast the objects to be compared to ListViewItem objects
            kListViewX = (ListViewItem)x;
            kListViewY = (ListViewItem)y;
            if (m_iColumnToSort == 0)
            {
                iCompareResult = m_kFirstObjectCompare.Compare(x,y);
            }
            else
            {
                // Compare the two items
                iCompareResult = 
                    m_kObjectCompare.Compare(
                    kListViewX.SubItems[m_iColumnToSort].Text,
                    kListViewY.SubItems[m_iColumnToSort].Text);
            }
            // Calculate correct return value based on object comparison
            if (m_kOrderOfSort == SortOrder.Ascending)
            {
                // Ascending sort is selected,
                // return normal result of compare operation
                return iCompareResult;
            }
            else if (m_kOrderOfSort == SortOrder.Descending)
            {
                // Descending sort is selected,
                // return negative result of compare operation
                return (-iCompareResult);
            }
            else
            {
                // Return '0' to indicate they are equal
                return 0;
            }
        }
    
        /// <summary>
        /// Gets or sets the number of the column to which
        /// to apply the sorting operation (Defaults to '0').
        /// </summary>
        public int SortColumn
        {
            set
            {
                m_iColumnToSort = value;
            }
            get
            {
                return m_iColumnToSort;
            }
        }
        /// <summary>
        /// Gets or sets the order of sorting to apply
        /// (for example, 'Ascending' or 'Descending').
        /// </summary>
        public SortOrder Order
        {
            set
            {
                m_kOrderOfSort = value;
            }
            get
            {
                return m_kOrderOfSort;
            }
        }

        public static void SortOnColumn(ListView myListView, 
            int iColumn, ColumnSort kThisColumnSort)
        {
            // Determine if clicked column 
            // is already the column that is being sorted.
            if ( iColumn == kThisColumnSort.SortColumn )
            {
                // Reverse the current sort direction for this column.
                if (kThisColumnSort.Order == SortOrder.Ascending)
                {
                    kThisColumnSort.Order = SortOrder.Descending;
                }
                else
                {
                    kThisColumnSort.Order = SortOrder.Ascending;
                }
            }
            else
            {
                // Set the column number that is to 
                //be sorted; default to ascending.
                kThisColumnSort.SortColumn = iColumn;
                kThisColumnSort.Order = SortOrder.Ascending;
            }

            // Perform the sort with these new sort options.
            myListView.Sort();
        }

        public static void ListView_ColumnClick(object sender, 
            System.Windows.Forms.ColumnClickEventArgs e,
            ColumnSort kThisColumnSort)
        {
            ListView myListView = (ListView)sender;
            SortOnColumn(myListView, e.Column, kThisColumnSort);
        }
    }
    public class ImageTextComparer : IComparer
    {
        //private CaseInsensitiveComparer m_kObjectCompare;
        private NumberCaseInsensitiveComparer m_kObjectCompare;
        
        public ImageTextComparer()
        {
            // Initialize the CaseInsensitiveComparer object
            m_kObjectCompare = new NumberCaseInsensitiveComparer();
        }
        public int Compare(object x, object y)
        {
            //int iCompareResult;
            int image1, image2;
            ListViewItem kListViewX, kListViewY;
            // Cast the objects to be compared to ListViewItem objects
            kListViewX = (ListViewItem)x;
            image1 = kListViewX.ImageIndex;
            kListViewY = (ListViewItem)y;
            image2 = kListViewY.ImageIndex;
            if (image1 < image2)
            {
                return -1;
            }
            else if (image1 == image2)
            {
                return 
                    m_kObjectCompare.Compare(kListViewX.Text,kListViewY.Text);
            }
            else
            {
                return 1;
            }
        }
    }
    public class NumberCaseInsensitiveComparer : CaseInsensitiveComparer
    {
        public NumberCaseInsensitiveComparer ()
        {
   
        }
        public new int Compare(object x, object y)
        { 
            // in case x,y are strings and actually number,
            // convert them to int and use the base.Compare for comparison
            if ((x is System.String) && IsWholeNumber((string)x) 
                && (y is System.String) && IsWholeNumber((string)y))
            {
                return base.Compare(System.Convert.ToInt32(x),
                    System.Convert.ToInt32(y));
            }
            else
            {
                return base.Compare(x,y);
            }
        }
        private bool IsWholeNumber(string strNumber)
        { // use a regular expression to find 
          //out if string is actually a number
            Regex objNotWholePattern=new Regex("[^0-9]");
            return !objNotWholePattern.IsMatch(strNumber);
        }  
    }
}
