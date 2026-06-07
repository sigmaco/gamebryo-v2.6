using System;
using System.Drawing;
using System.Windows.Forms;

namespace SourceGrid2
{
	/// <summary>
	/// A struct that represents a single border line.
	/// If you have 2 adjacent cells and want to create a 1 pixel width border, you must set width 1 for one cell and width 0 for the other. Usually a cell has only Right and Bottom border.
	/// </summary>
	public struct Border
	{
		public readonly static Border NoBorder = new Border(Color.White,0);

		public Border(Color p_Color)
		{
			m_Color = p_Color;
			m_Width = 1;
		}
		public Border(Color p_Color, int p_Width)
		{
			m_Width = p_Width;
			m_Color = p_Color;
		}


		private int m_Width;
		private Color m_Color;

		public int Width
		{
			get{return m_Width;}
			set{m_Width = value;}
		}
		public Color Color
		{
			get{return m_Color;}
			set{m_Color = value;}
		}

		public override string ToString()
		{
			return Color.ToString() + ", Width= " + Width.ToString();
		}

		/// <summary>
		/// Compare to current border with another border.
		/// </summary>
		/// <param name="obj"></param>
		/// <returns></returns>
		public override bool Equals(object obj)
		{
			if (obj==null)
				return false;
			if (obj.GetType() != GetType())
				return false;
			Border l_Other = (Border)obj;
			if (l_Other.Width == Width && l_Other.Color == Color)
				return true;
			else
				return false;
		}

		public override int GetHashCode()
		{
			return m_Color.GetHashCode();
		}

		public static bool operator == (Border a, Border b)
		{
			return a.Equals(b);
		}

		public static bool operator != (Border a, Border b)
		{
			return a.Equals(b) == false;
		}
	}


	/// <summary>
	/// A struct that represents the borders of a cell. Contains 4 borders: Right, Left, Top, Bottom.
	/// If you have 2 adjacent cells and want to create a 1 pixel width border, you must set width 1 for one cell and width 0 for the other. Usually a cell has only Right and Bottom border.
	/// </summary>
	public struct RectangleBorder
	{
		public readonly static RectangleBorder Default = new RectangleBorder( new Border(Color.LightGray,1), new Border(Color.LightGray,1) );
		public readonly static RectangleBorder NoBorder = new RectangleBorder(Border.NoBorder);
		public readonly static RectangleBorder RectangleBlack1Width = new RectangleBorder(new Border(Color.Black,1));

		/// <summary>
		/// Construct a RectangleBorder with the same border on all the side
		/// </summary>
		/// <param name="p_Border"></param>
		public RectangleBorder(Border p_Border)
		{
			m_Top = p_Border;
			m_Bottom = p_Border;
			m_Left = p_Border;
			m_Right = p_Border;
		}

		/// <summary>
		/// Construct a RectangleBorder with the specified Right and Bottom border and a 0 Left and Top border
		/// </summary>
		/// <param name="p_Right"></param>
		/// <param name="p_Bottom"></param>
		public RectangleBorder(Border p_Right, Border p_Bottom)
		{
			m_Right = p_Right;
			m_Bottom = p_Bottom;
			m_Top = new Border(Color.White,0);
			m_Left = new Border(Color.White,0);
		}

		/// <summary>
		/// Construct a RectangleBorder with the specified borders
		/// </summary>
		/// <param name="p_Top"></param>
		/// <param name="p_Bottom"></param>
		/// <param name="p_Left"></param>
		/// <param name="p_Right"></param>
		public RectangleBorder(Border p_Top, Border p_Bottom, Border p_Left, Border p_Right)
		{
			m_Top = p_Top;
			m_Bottom = p_Bottom;
			m_Left = p_Left;
			m_Right = p_Right;
		}

		private Border m_Top;
		public Border Top
		{
			get{return m_Top;}
			set{m_Top = value;}
		}
		private Border m_Bottom;
		public Border Bottom
		{
			get{return m_Bottom;}
			set{m_Bottom = value;}
		}
		private Border m_Left;
		public Border Left
		{
			get{return m_Left;}
			set{m_Left = value;}
		}
		private Border m_Right;
		public Border Right
		{
			get{return m_Right;}
			set{m_Right = value;}
		}

		public override string ToString()
		{
			return "Top:" + Top.ToString() + " Bottom:" + Bottom.ToString() + " Left:" + Left.ToString() + " Right:" + Right.ToString();
		}
 
		/// <summary>
		/// Change the color of the current struct instance and return a copy of the modified struct.
		/// </summary>
		/// <param name="p_Color"></param>
		/// <returns></returns>
		public RectangleBorder SetColor(Color p_Color)
		{
			Top = new Border(p_Color,Top.Width);
			Bottom = new Border(p_Color,Bottom.Width);
			Left = new Border(p_Color,Left.Width);
			Right = new Border(p_Color,Right.Width);

			return this;
		}

		/// <summary>
		/// Change the width of the current struct instance and return a copy of the modified struct.
		/// </summary>
		/// <param name="p_Width"></param>
		/// <returns></returns>
		public RectangleBorder SetWidth(int p_Width)
		{
			Top = new Border(Top.Color,p_Width);
			Bottom = new Border(Bottom.Color,p_Width);
			Left = new Border(Left.Color,p_Width);
			Right = new Border(Right.Color,p_Width);

			return this;
		}

		/// <summary>
		/// Compare to current border with another border.
		/// </summary>
		/// <param name="obj"></param>
		/// <returns></returns>
		public override bool Equals(object obj)
		{
			if (obj==null)
				return false;
			if (obj.GetType() != GetType())
				return false;
			RectangleBorder l_Other = (RectangleBorder)obj;
			if (l_Other.Left == Left && l_Other.Bottom == Bottom &&
				l_Other.Top == Top && l_Other.Right == Right)
				return true;
			else
				return false;
		}

		public override int GetHashCode()
		{
			return Left.GetHashCode();
		}

		public static bool operator == (RectangleBorder a, RectangleBorder b)
		{
			return a.Equals(b);
		}

		public static bool operator != (RectangleBorder a, RectangleBorder b)
		{
			return a.Equals(b) == false;
		}

		/// <summary>
		/// Remove the width of all the borders (top, bottom, left, right) from a specified rectangle
		/// </summary>
		/// <param name="p_Input"></param>
		/// <returns></returns>
		public Rectangle RemoveBorderFromRectanlge(Rectangle p_Input)
		{
			p_Input.Y += Top.Width;
			p_Input.X += Left.Width;
			p_Input.Width -= Left.Width + Right.Width;
			p_Input.Height -= Top.Width + Bottom.Width;

			return p_Input;
		}

		#region utility
		/// <summary>
		/// Create an Inset border
		/// </summary>
		/// <param name="p_width"></param>
		/// <param name="p_ShadowColor"></param>
		/// <param name="p_LightColor"></param>
		public static RectangleBorder CreateInsetBorder(int p_width, Color p_ShadowColor, Color p_LightColor)
		{
			RectangleBorder l_Border = new RectangleBorder(new Border(Color.White));;

			l_Border.Top = new Border(p_ShadowColor,p_width);
			l_Border.Left = new Border(p_ShadowColor,p_width);
			l_Border.Bottom = new Border(p_LightColor,p_width);
			l_Border.Right = new Border(p_LightColor,p_width);
			
			return l_Border;
		}

		/// <summary>
		/// Create a Raised border
		/// </summary>
		/// <param name="p_width"></param>
		/// <param name="p_ShadowColor"></param>
		/// <param name="p_LightColor"></param>
		public static RectangleBorder CreateRaisedBorder(int p_width, Color p_ShadowColor, Color p_LightColor)
		{
			RectangleBorder l_Border = new RectangleBorder(new Border(Color.White));;

			l_Border.Top = new Border(p_LightColor,p_width);
			l_Border.Left = new Border(p_LightColor,p_width);
			l_Border.Bottom = new Border(p_ShadowColor,p_width);
			l_Border.Right = new Border(p_ShadowColor,p_width);
		
			return l_Border;
		}

		#endregion
	}


}
