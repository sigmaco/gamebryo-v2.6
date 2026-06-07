// Copyright (C) 1998-2007 Alias | Wavefront, a division of Silicon 
// Graphics Limited.
//
// The information in this file is provided for the exclusive use of the
// licensees of Alias | Wavefront.  Such users have the right to use, modify,
// and incorporate this code into other products for purposes authorized
// by the Alias | Wavefront license agreement, without fee.
//
// ALIAS | WAVEFRONT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
// INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
// EVENT SHALL ALIAS | WAVEFRONT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
// CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OFUSE
// DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.


// Precompiled Headers
#include "MayaPluginPCH.h"
#include "iffreader.h"

// Need to work around some file problems on the NT platform with Maya 1.0



IFFimageReader::IFFimageReader ()
{
        fImage = NULL;
        fBuffer = NULL;
        fZBuffer = NULL;
}

IFFimageReader::~IFFimageReader ()
{
        close ();
}

MStatus IFFimageReader::open (MString filename)
{
    // Close any other files

    close ();

    fImage = ILopen (filename.asChar (), "r");

    // if not there then do the search path thing.

    if (NULL == fImage)
    {
            // Lets now try using the textureSearchPath that the user gave
            char *searchPath = DtExt_getTextureSearchPath();
            char *workCopy, *tPath;
            char trialFileName[1024];
            int  found = false;
    
            // If no search path to look thru then normal failure exit

            if ( !searchPath )
                return MS::kFailure;

            if ( strlen( searchPath ) == 0 )
                return MS::kFailure;

            // Else lets get a working copy of the path - since it going to
            // change, and start going thru it.  Since NT uses ":" character
            // as part of disk drive specification we will use "|" as the 
            // path separator

            workCopy = NiStrdup( searchPath );
            char* pcContext;
            tPath = NiStrtok( workCopy, "|", &pcContext );
            
            while ( tPath && !found )
            {
                NiStrcpy( trialFileName, 1024, tPath );
#ifdef WIN32
                NiStrcat( trialFileName, 1024, "\\" );
#else
                NiStrcat( trialFileName, 1024, "/" );
#endif
                NiStrcat( trialFileName, 1024, filename.asChar() );

                fImage = ILopen( trialFileName, "r" );

                if ( fImage )
                {
                    found = true;
                } else {
                    tPath = NiStrtok( NULL, "|", &pcContext );
                }
            }
            
            if ( workCopy )
                free( workCopy );

            if ( !found ) 
                return MS::kFailure;
        }

        // Read top-to-bottom, not bottom-to-top
        //int flag = ILctrl (fImage, ILF_Updown, 1);

        // Convert all data to RGBA, even if there's no alpha channel
        ILctrl (fImage, ILF_Pack, 0);
        // If the data is 16 bits, read in the full 16 bits. Otherwise,
        // truncate to 8 bits. Default behaviour is to truncate 16 bit
        // data to 8 bits.
        if (ILgetbpp (fImage) == 2)
                ILctrl (fImage, ILF_Full, 1);
        else
                ILctrl (fImage, ILF_Full, 0);
        // Generate a zero alpha mask if there's no alpha channel
        ILctrl (fImage, ILF_NoMask, 0);

        return MS::kSuccess;
}

MStatus IFFimageReader::close ()
{
        if (NULL == fImage)
                return MS::kFailure;

        if (ILclose (fImage))
        {
                fImage = NULL;
                return MS::kFailure;
        }

        fImage = NULL;

        if (NULL != fBuffer)
        {
                delete [] fBuffer;
                fBuffer = NULL;
        }
        if (NULL != fZBuffer)
        {
                delete [] fZBuffer;
                fZBuffer = NULL;
        }

        return MS::kSuccess;
}

MStatus IFFimageReader::getSize (int &x, int &y)
{
        if (NULL == fImage)
                return MS::kFailure;

        if (ILgetsize (fImage, &x, &y))
                return MS::kFailure;

        return MS::kSuccess;
}

int IFFimageReader::getBytesPerChannel ()
{
        return ILgetbpp (fImage);
}

bool IFFimageReader::isRGB ()
{
        if (NULL == fImage)
                return false;
        int type = ILgettype (fImage);
        if (type == -1)
                return false;
        return (type&ILH_RGB) ? true : false;
}

bool IFFimageReader::isGrayscale ()
{
        if (NULL == fImage)
                return false;
        int type = ILgettype (fImage);
        if (type == -1)
                return false;
        return (type&ILH_Black) ? true : false;
}

bool IFFimageReader::hasAlpha ()
{
        if (NULL == fImage)
                return false;
        int type = ILgettype (fImage);
        if (type == -1)
                return false;
        return (type&ILH_Alpha) ? true : false;
}

bool IFFimageReader::hasDepthMap ()
{
        if (NULL == fImage)
                return false;
        int type = ILgettype (fImage);
        if (type == -1)
                return false;
        return (type&ILH_Zbuffer) ? true : false;
}

MStatus IFFimageReader::readImage ()
{
        if (NULL == fImage)
                return MS::kFailure;

        if (NULL != fBuffer || NULL != fZBuffer)
                return MS::kFailure;

        int width,height;
        ILgetsize (fImage, &width, &height);

        int bpp; // bytes per pixel, not bits per pixel
        bpp = ILgetbpp (fImage);

        int type;
        type = ILgettype (fImage);

        if ((type & ILH_RGB) || (type & ILH_Black))
                fBuffer = new byte [width * height * bpp * 4];

        if (type & ILH_Zbuffer)
                fZBuffer = new float [width * height];

        if (ILload (fImage, fBuffer, fZBuffer))
                return MS::kFailure;

        return MS::kSuccess;
}

MStatus IFFimageReader::getPixel (int x, int y, int *r, int *g, int *b, 
    int *a)
{
        if (NULL == fBuffer)
                return MS::kFailure;
        int width,height;
        ILgetsize (fImage, &width, &height);
        if (x >= width || y >= height || x < 0 || y < 0)
                return MS::kFailure;
        if (ILgetbpp (fImage) == 2) {
                int *ptr = (int *)&fBuffer [(y * width + x) * 4];
        // On IRIX pixels are stored as ABGR and on NT as BGRA
#ifdef _WIN32
                if (NULL != r)
                    *r = ptr [2];
                if (NULL != g)
                    *g = ptr [1];
                if (NULL != b)
                    *b = ptr [0];
                if (NULL != a)
                    *a = ptr [3];
#else
                if (NULL != r)
                        *r = ptr [3];
                if (NULL != g)
                        *g = ptr [2];
                if (NULL != b)
                        *b = ptr [1];
                if (NULL != a)
                        *a = ptr [0];
#endif
        } else {
                byte *ptr = &fBuffer [(y * width + x) * 4];
#ifdef _WIN32
                if (NULL != r)
                    *r = ptr [2];
                if (NULL != g)
                    *g = ptr [1];
                if (NULL != b)
                    *b = ptr [0];
                if (NULL != a)
                    *a = ptr [3];
#else
                if (NULL != r)
                        *r = ptr [3];
                if (NULL != g)
                        *g = ptr [2];
                if (NULL != b)
                        *b = ptr [1];
                if (NULL != a)
                        *a = ptr [0];
#endif
        }
        return MS::kSuccess;
}

MStatus IFFimageReader::getDepth (int x, int y, float *d)
{
        if (NULL == fZBuffer || NULL == d)
                return MS::kFailure;
        int width,height;
        ILgetsize (fImage, &width, &height);
        if (x >= width || y >= height || x < 0 || y < 0)
                return MS::kFailure;
        float depth = fZBuffer [y * width + x];
        if (depth == 0.)
                *d = 0.;
        else
                *d = -1.0f/depth;
        return MS::kSuccess;
}

MString IFFimageReader::errorString ()
{
        return FLstrerror (FLerror ());
}

const byte *IFFimageReader::getPixelMap () const
{
        return fBuffer;
}

const float *IFFimageReader::getDepthMap () const
{
        return fZBuffer;
}
