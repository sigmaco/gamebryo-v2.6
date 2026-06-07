using System;
using System.Drawing;
using System.Windows.Forms;

namespace SourceGrid2
{
	/// <summary>
	/// Class with some static methods for painting cells, grid and borders
	/// </summary>
	public class GridPaint
	{
		/// <summary>
		/// Constructor
		/// </summary>
		private GridPaint()
		{
		}

		#region DrawUtility
		/// <summary>
		/// Draw a border
		/// </summary>
		/// <param name="graphics"></param>
		/// <param name="rectangle"></param>
		/// <param name="border"></param>
		public static void DrawBorder(Graphics graphics, Rectangle rectangle, RectangleBorder border)
		{
			if (border.Left.Width > 0)
			{
				using (Pen leftPen = new Pen(border.Left.Color))
				{
					for (int i = 0; i < border.Left.Width; i++)
						graphics.DrawLine(leftPen, rectangle.X+i, rectangle.Y, rectangle.X+i, rectangle.Bottom-1);
				}
			}

			if (border.Bottom.Width > 0)
			{
				using (Pen bottomPen = new Pen(border.Bottom.Color))
				{
					for (int i = 1; i <= border.Bottom.Width; i++)
						graphics.DrawLine(bottomPen, rectangle.X, rectangle.Bottom-i, rectangle.Right-1, rectangle.Bottom-i);
				}
			}

			if (border.Right.Width > 0)
			{
				using (Pen rightPen = new Pen(border.Right.Color))
				{
					for (int i = 1; i <= border.Right.Width; i++)
						graphics.DrawLine(rightPen, rectangle.Right-i, rectangle.Y, rectangle.Right-i, rectangle.Bottom-1);
				}
			}

			if (border.Top.Width > 0)
			{
				using (Pen topPen = new Pen(border.Top.Color))
				{
					for (int i = 0; i < border.Top.Width; i++)
						graphics.DrawLine(topPen, rectangle.X, rectangle.Y+i, rectangle.Right-1, rectangle.Y+i);
				}
			}
		}
		#endregion

		/// <summary>
		/// Paint the Text and the Image passed
		/// </summary>
		/// <param name="g">Graphics device where you can render your image and text</param>
		/// <param name="p_displayRectangle">Relative rectangle based on the display area</param>
		/// <param name="p_Image">Image to draw. Can be null.</param>
		/// <param name="p_ImageAlignment">Alignment of the image</param>
		/// <param name="p_ImageStretch">True to make the draw the image with the same size of the cell</param>
		/// <param name="p_Text">Text to draw (can be null)</param>
		/// <param name="p_StringFormat">String format (can be null)</param>
		/// <param name="p_AlignTextToImage">True to align the text with the image</param>
		/// <param name="p_Border">Cell Border</param>
		/// <param name="p_TextColor">Text Color</param>
		/// <param name="p_TextFont">Text Font(can be null)</param>
		public static void DrawImageAndText(Graphics g, 
			Rectangle p_displayRectangle,
			Image p_Image,
			SourceLibrary.Drawing.ContentAlignment p_ImageAlignment,
			bool p_ImageStretch,
			string p_Text,
			StringFormat p_StringFormat,
			bool p_AlignTextToImage,
			RectangleBorder p_Border,
			Color p_TextColor,
			Font p_TextFont)
		{
			// Calculate Rectangle with no border
			Rectangle l_CellRectNoBorder = p_Border.RemoveBorderFromRectanlge(p_displayRectangle);

			#region Image
			//Image
			if (p_Image != null)
			{
				if (p_ImageStretch) //strech image
#if !MINI
					g.DrawImage(p_Image, l_CellRectNoBorder);
#else
					g.DrawImage(p_Image, l_CellRectNoBorder, new Rectangle(0, 0, p_Image.Width, p_Image.Height), GraphicsUnit.Pixel);
#endif
				else
				{
					PointF l_PointImage = Utility.CalculateObjAlignment(p_ImageAlignment, (int)l_CellRectNoBorder.Left, (int)l_CellRectNoBorder.Top, (int)l_CellRectNoBorder.Width, (int)l_CellRectNoBorder.Height, p_Image.Width, p_Image.Height);

					Rectangle l_RectDrawImage = new Rectangle((int)l_PointImage.X, (int)l_PointImage.Y,
						p_Image.Width, p_Image.Height);
					//RectangleF l_RectDrawImage = l_CellRectNoBorder;
					//l_RectDrawImage.Intersect(new RectangleF(l_PointImage,p_Image.PhysicalDimension));

					//Truncate the Rectangle for appreximation problem
#if !MINI
					g.DrawImage(p_Image, Rectangle.Truncate(l_RectDrawImage));
#else
					g.DrawImage(p_Image, Rectangle.Truncate(l_RectDrawImage), new Rectangle(0, 0, p_Image.Width, p_Image.Height), GraphicsUnit.Pixel);
#endif

					//g.DrawImage(p_Image,l_PointImage);
				}
			}
			#endregion

			#region Text
			//Text
			if (p_Text != null && p_Text.Length>0)
			{
				if (l_CellRectNoBorder.Width>0 && l_CellRectNoBorder.Height>0)
				{
					RectangleF l_RectDrawText = l_CellRectNoBorder;
					#region Align Text To Image
					if (p_Image != null && p_ImageStretch == false && p_AlignTextToImage)
					{
						if (Utility.IsBottom(p_ImageAlignment) && Utility.IsBottom(p_StringFormat))
						{
							l_RectDrawText.Height -= p_Image.Height;
						}
						if (Utility.IsTop(p_ImageAlignment) && Utility.IsTop(p_StringFormat))
						{
							l_RectDrawText.Y += p_Image.Height;
							l_RectDrawText.Height -= p_Image.Height;
						}
						if (Utility.IsLeft(p_ImageAlignment) && Utility.IsLeft(p_StringFormat))
						{
							l_RectDrawText.X += p_Image.Width;
							l_RectDrawText.Width -= p_Image.Width;
						}
						if (Utility.IsRight(p_ImageAlignment) && Utility.IsRight(p_StringFormat))
						{
							l_RectDrawText.Width -= p_Image.Width;
						}
					}
					#endregion

#if !MINI
					using (SolidBrush textBrush = new SolidBrush(p_TextColor))
					{
						g.DrawString(p_Text,
							p_TextFont,
							textBrush,
							l_RectDrawText,
							p_StringFormat);
					}
#else
					SolidBrush textBrush = new SolidBrush(p_TextColor);

					SizeF l_SizeText = g.MeasureString(p_Text, p_TextFont);

					PointF l_TextPoint = Utility.CalculateObjAlignment(Utility.StringFormatoToContentAlignment(p_StringFormat),
																		(int)l_RectDrawText.Left,
																		(int)l_RectDrawText.Top,
																		(int)l_RectDrawText.Width,
																		(int)l_RectDrawText.Height,
																		(int)l_SizeText.Width,
																		(int)l_SizeText.Height);

					g.DrawString(p_Text,
							p_TextFont,
							textBrush,
							new RectangleF(l_TextPoint.X, l_TextPoint.Y, l_SizeText.Width, l_SizeText.Height));
#endif
				}
			}
			#endregion
		}
	}
}
