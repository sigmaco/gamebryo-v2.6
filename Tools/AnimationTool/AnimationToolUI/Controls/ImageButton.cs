//
// Copyright (c) 2004, O&O Services GmbH.
// Am Borsigturm 48
// 13507 Berlin
// GERMANY
// Tel: +49 30 43 03 43-03, Fax: +49 30 43 03 43-99
// E-mail: info@oo-services.com
// Web: http://www.oo-services.com
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice, 
//   this list of conditions and the following disclaimer in the documentation 
//   and/or other materials provided with the distribution.
// * Neither the name of O&O Services GmbH nor the names of its contributors 
//   may be used to endorse or promote products derived from this software
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

using System;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.ComponentModel;

namespace OOGroup.Windows.Forms
{
    /// <summary>
    /// A System.Windows.Forms.Button that can have an image on systems that support visual styles.
    /// Uses the BCM_SETIMAGE message that is provided by comctl32.dll, version 6.
    /// On systems that have a lower version of comctl32.dll, i.e. earlier than Windows XP,
    /// falls back to FlatStyle.Standard.
    /// </summary>
    public class ImageButton: Button
    {
        /// <summary>
        /// Constructor.
        /// </summary>
        public ImageButton()
        {
            FlatStyle = FlatStyle.System;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct RECT
        {
            public int left;
            public int top;
            public int right;
            public int bottom;
        }

        /// <summary>
        /// The alignment of an image within a button.
        /// </summary>
        public enum Alignment { Left, Right, Top, Bottom, Center };

        private const int BCM_SETIMAGELIST = 0x1600 + 2;

        [StructLayout(LayoutKind.Sequential)]
        private struct BUTTON_IMAGELIST
        {
            public IntPtr himl;
            public RECT margin;
            public int uAlign;
        }

        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        private static extern int SendMessage(IntPtr hWnd, int msg, int wParam, ref BUTTON_IMAGELIST lParam);

        [StructLayout(LayoutKind.Sequential)]
        private struct DLLVERSIONINFO
        {
            public int cbSize;
            public int dwMajorVersion;
            public int dwMinorVersion;
            public int dwBuildNumber;
            public int dwPlatformID;
        }

        [DllImport("comctl32.dll", EntryPoint="DllGetVersion")]
        private static extern int GetCommonControlDLLVersion(ref DLLVERSIONINFO dvi);

        private int ComCtlMajorVersion = -1;

        private Bitmap themedImage;
        [Description("The image on the face of the button."), Category("Appearance"), DefaultValue(null)]
        public Bitmap ThemedImage
        {
            get
            {
                return themedImage;
            }

            set
            {
                if (value != null)
                {
                    SetImage(value);
                }

                themedImage = value;
            }
        }

        /// <summary>
        /// Sets the image of the button to the specified value.
        /// The image is aligned at the center with no margin.
        /// All states of the button show the same image.
        /// </summary>
        /// <param name="image">The images for the button</param>
        public void SetImage(Bitmap image)
        {
            SetImage(new Bitmap[] { image }, Alignment.Center, 0, 0, 0, 0);
        }
        
        /// <summary>
        /// Sets the image of the button to the specified value with the specified alignment.
        /// All states of the button show the same image.
        /// </summary>
        /// <param name="image">The images for the button</param>
        /// <param name="align">The alignment of the image</param>
        public void SetImage(Bitmap image, Alignment align)
        {
            SetImage(new Bitmap[] { image }, align, 0, 0, 0, 0);
        }

        /// <summary>
        /// Sets the image of the button to the specified value with the specified alignment and margins.
        /// All states of the button show the same image.
        /// </summary>
        /// <param name="image">The images for the button</param>
        /// <param name="align">The alignment of the image</param>
        /// <param name="leftMargin">The left margin of the image in pixels</param>
        /// <param name="topMargin">The top margin of the image in pixels</param>
        /// <param name="rightMargin">The right margin of the image in pixels</param>
        /// <param name="bottomMargin">The bottom margin of the image in pixels</param>
        public void SetImage(Bitmap image, Alignment align, int leftMargin, int topMargin, int rightMargin,
            int bottomMargin)
        {
            SetImage(new Bitmap[] { image }, align, leftMargin, topMargin, rightMargin, bottomMargin);
        }

        /// <summary>
        /// Sets the images of the button to the specified values.
        /// The images are aligned at the center with no margin.
        /// </summary>
        /// <param name="image">The images for the button</param>
        /// <param name="normalImage">The normal state image for the button.</param>
        /// <param name="hoverImage">The hover state image for the button.</param>
        /// <param name="pressedImage">The pressed state image for the button.</param>
        /// <param name="disabledImage">The disabled state image for the button.</param>
        /// <param name="focusedImage">The focused state image for the button.</param>
        public void SetImage(Bitmap normalImage, Bitmap hoverImage, Bitmap pressedImage,
            Bitmap disabledImage, Bitmap focusedImage)
        {
            SetImage(new Bitmap[] { normalImage, hoverImage, pressedImage,
                                      disabledImage, focusedImage },
                Alignment.Center, 0, 0, 0, 0);
        }
        
        /// <summary>
        /// Sets the images of the button to the specified values with the specified alignment.
        /// </summary>
        /// <param name="image">The images for the button</param>
        /// <param name="normalImage">The normal state image for the button.</param>
        /// <param name="hoverImage">The hover state image for the button.</param>
        /// <param name="pressedImage">The pressed state image for the button.</param>
        /// <param name="disabledImage">The disabled state image for the button.</param>
        /// <param name="focusedImage">The focused state image for the button.</param>
        /// <param name="align">The alignment of the image</param>
        public void SetImage(Bitmap normalImage, Bitmap hoverImage, Bitmap pressedImage,
            Bitmap disabledImage, Bitmap focusedImage,
            Alignment align)
        {
            SetImage(new Bitmap[] { normalImage, hoverImage, pressedImage,
                                      disabledImage, focusedImage },
                align, 0, 0, 0, 0);
        }

        /// <summary>
        /// Sets the images of the button to the specified values with the specified alignment and margins.
        /// </summary>
        /// <param name="normalImage">The normal state image for the button.</param>
        /// <param name="hoverImage">The hover state image for the button.</param>
        /// <param name="pressedImage">The pressed state image for the button.</param>
        /// <param name="disabledImage">The disabled state image for the button.</param>
        /// <param name="focusedImage">The focused state image for the button.</param>
        /// <param name="align">The alignment of the image</param>
        /// <param name="leftMargin">The left margin of the image in pixels</param>
        /// <param name="topMargin">The top margin of the image in pixels</param>
        /// <param name="rightMargin">The right margin of the image in pixels</param>
        /// <param name="bottomMargin">The bottom margin of the image in pixels</param>
        public void SetImage(Bitmap normalImage, Bitmap hoverImage, Bitmap pressedImage,
            Bitmap disabledImage, Bitmap focusedImage,
            Alignment align, int leftMargin, int topMargin, int rightMargin,
            int bottomMargin)
        {
            SetImage(new Bitmap[] { normalImage, hoverImage, pressedImage,
                                      disabledImage, focusedImage },
                align, leftMargin, topMargin, rightMargin, bottomMargin);
        }
    
        private bool generateDisabledImage = false;
        [Description("Determines whether the image for the disabled state will be generated automatically from the normal one."), Category("Appearance"), DefaultValue(false)]
        public bool GenerateDisabledImage
        {
            get
            {
                return generateDisabledImage;
            }

            set
            {
                generateDisabledImage = value;
            }
        }

        /// <summary>
        /// Sets the images of the button to the specified value with the specified alignment and margins.
        /// </summary>
        /// <param name="images">The images for the button.</param>
        /// <param name="align">The alignment of the image</param>
        /// <param name="leftMargin">The left margin of the image in pixels</param>
        /// <param name="topMargin">The top margin of the image in pixels</param>
        /// <param name="rightMargin">The right margin of the image in pixels</param>
        /// <param name="bottomMargin">The bottom margin of the image in pixels</param>
        public void SetImage(Bitmap[] images,
            Alignment align, int leftMargin, int topMargin, int rightMargin,
            int bottomMargin)
        {
            if (GenerateDisabledImage)
            {
                if (images.Length == 1)
                {
                    Bitmap image = images[0];
                    images = new Bitmap[] { image, image, image, image, image };
                }

                images[3] = DrawImageDisabled(images[3]);
            }

            if (ComCtlMajorVersion < 0)
            {
                DLLVERSIONINFO dllVersion = new DLLVERSIONINFO();
                dllVersion.cbSize = Marshal.SizeOf(typeof(DLLVERSIONINFO));
                GetCommonControlDLLVersion(ref dllVersion);
                ComCtlMajorVersion = dllVersion.dwMajorVersion;
            }

            if (ComCtlMajorVersion >= 6 && FlatStyle == FlatStyle.System)
            {
                RECT rect = new RECT();
                rect.left = leftMargin;
                rect.top = topMargin;
                rect.right = rightMargin;
                rect.bottom = bottomMargin;

                BUTTON_IMAGELIST buttonImageList = new BUTTON_IMAGELIST();
                buttonImageList.margin = rect;
                buttonImageList.uAlign = (int)align;

                ImageList = GenerateImageList(images);
                buttonImageList.himl = ImageList.Handle;

                SendMessage(this.Handle, BCM_SETIMAGELIST, 0, ref buttonImageList);
            }
            else
            {
                FlatStyle = FlatStyle.Standard;

                if (images.Length > 0)
                {
                    Image = images[0];
                }

                switch (align)
                {
                    case Alignment.Bottom:
                        ImageAlign = ContentAlignment.BottomCenter;
                        break;
                    case Alignment.Left:
                        ImageAlign = ContentAlignment.MiddleLeft;
                        break;
                    case Alignment.Right:
                        ImageAlign = ContentAlignment.MiddleRight;
                        break;
                    case Alignment.Top:
                        ImageAlign = ContentAlignment.TopCenter;
                        break;
                    case Alignment.Center:
                        ImageAlign = ContentAlignment.MiddleCenter;
                        break;
                }
            }
        }

        private System.Drawing.Bitmap DrawImageDisabled(System.Drawing.Image image)
        {
            System.Drawing.Bitmap active = new Bitmap(image);
            System.Drawing.Bitmap disable = new Bitmap(active.Width, active.Height);
            System.Drawing.Graphics g = Graphics.FromImage(disable);

            g.DrawImage(active, 0, 0);
            ControlPaint.DrawImageDisabled(g, active, 0, 0, Color.Empty);
            g.Dispose();   
            return disable;
        }

        private ImageList GenerateImageList(Bitmap[] images)
        {
            ImageList il = new ImageList();
            il.ColorDepth = ColorDepth.Depth32Bit;

            if (images.Length > 0)
            {
                for (int i = 0; i < images.Length; i++)
                {
                    int ReSizeHeight = this.Height;
                    int ReSizeWidth = this.Width;

                    if (ReSizeHeight > 256)
                    {
                        ReSizeHeight = 256;
                    }
                    
                    if (ReSizeWidth > 256)
                    {
                        ReSizeWidth = 256;
                    }

                    if (images[i].Width > ReSizeWidth || images[i].Height > ReSizeHeight)
                    {
                        double Scaling;
                        double WidthScaling = ReSizeWidth / (double)images[i].Width;
                        double HeightScaling = ReSizeHeight / (double)images[i].Height;

                        if (WidthScaling < HeightScaling)
                        {
                            Scaling = WidthScaling;
                        }
                        else
                        {
                            Scaling = HeightScaling;
                        }

                        int newWidth = (int)(images[i].Width * Scaling);
                        int newHeight = (int)(images[i].Height * Scaling);
                        Bitmap bm = new Bitmap(newWidth, newHeight);
                        Graphics graphics = Graphics.FromImage(bm);
                        graphics.InterpolationMode = InterpolationMode.HighQualityBicubic;
                        graphics.DrawImage(images[i], 0, 0, newWidth, newHeight);
                        images[i] = bm;
                    }
                }

                il.ImageSize = new Size(images[0].Width, images[0].Height);

                foreach (Bitmap image in images)
                {
                    il.Images.Add(image);
                    Bitmap bm = (Bitmap)il.Images[il.Images.Count - 1];

                    // copy pixel data from original Bitmap into ImageList
                    // to work around a bug in ImageList:
                    // adding an image to an ImageList destroys the alpha channel

//                    for (int x = 0; x < bm.Width; x++)
//                    {
//                        for (int y = 0; y < bm.Height; y++)
//                        {
//                            bm.SetPixel(x, y, image.GetPixel(x, y));
//                        }
//                    }

                    // code below contributed by Richard Deeming
                    // requires /unsafe compiler flag
                    // does the same as the code above, albeit a lot faster
                    Rectangle rc = new Rectangle(new Point(0, 0), image.Size);
                    BitmapData src = image.LockBits(rc, ImageLockMode.ReadOnly, image.PixelFormat);
                    BitmapData dst = bm.LockBits(rc, ImageLockMode.WriteOnly, image.PixelFormat);
            
                    try
                    {
                        unsafe
                        {
                            int* pSrc = (int*)src.Scan0;
                            int* pDst = (int*)dst.Scan0;
                    
                            for(int row=0; row < bm.Height; row++)
                            {
                                for(int col=0; col < bm.Width; col++)
                                {
                                    pDst[col] = pSrc[col];
                                }
                        
                                pSrc += src.Stride >> 2;
                                pDst += dst.Stride >> 2;
                            }
                        }
                    }
                    finally
                    {
                        bm.UnlockBits(dst);
                        image.UnlockBits(src);
                    }
                }
            }

            return il;
        }

        private bool dropDown = false;
        [Description("Determines whether a small dropdown arrow will painted."), Category("Appearance"), DefaultValue(false)]
        public bool DropDown
        {
            get
            {
                return dropDown;
            }

            set
            {
                dropDown = value;
            }
        }

        public const int WM_PAINT = 0x000f;
        public const int WM_ENABLE = 0x000a;

        protected override void WndProc(ref Message m)
        {
            base.WndProc (ref m);

            if (DropDown && (m.Msg == WM_PAINT || m.Msg == WM_ENABLE))
            {
                try
                {
                    Graphics g = CreateGraphics();
                    Font font = new Font("Marlett", Font.Size);
                    SizeF sizeF = g.MeasureString("6", font);
                    Brush brush = Enabled ? SystemBrushes.ControlText : new SolidBrush(SystemColors.GrayText);
                    g.DrawString("6", font, brush, 
                        Width - 4 - sizeF.Width, (Height - sizeF.Height) / 2.0f);
                }
                catch
                {
                    Trace.WriteLine("Problem drawing dropdown arrow");
                }
            }
        }
    }
}
