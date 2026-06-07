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

#include "CrosswalkGamebryoContext.h"
#include "CrosswalkGamebryoImpl.h"
#include "CrosswalkGamebryoTexMap.h"

#include "XSIImageLibrary.h"
#include "XSIImage.h"

#include "NiSourceTexture.h"
#include "NiTexturingProperty.h"

namespace epg
{
    //---------------------------------------------------------------------------
    // Converts all images in the given library.
    //---------------------------------------------------------------------------
    bool ConvertImages(Context & io_Context, CSLImageLibrary* in_ImageLib)
    {
        if (!in_ImageLib)
            return true;

        CSLImage** imageList = in_ImageLib->GetImageList();
        const SI_Int imageCount = in_ImageLib->GetImageCount();
        Phaser phase(io_Context, CONVERT_IMAGES_PHASE, imageCount);
        for (SI_Int i = 0; i < imageCount; ++i)
        {
            phase.Progress();

            if (imageList[i])
            {
                if (!ConvertImage(io_Context, imageList[i][0]))
                {
                    return false;
                }
            }
        }

        return true;
    }

    //---------------------------------------------------------------------------
    // Convert one image.
    //---------------------------------------------------------------------------
    bool ConvertImage(Context& io_Context, CSLImage& in_Image)
    {
        SI_Char* filename = in_Image.GetSourceFile();
        if (!filename || !filename[0])
        {
            return io_Context.Logf(
                LOG_ERROR,
                "Cannot convert image \"%s\" because it has no file name.",
                in_Image.Name().GetText());
        }

        if ( in_Image.CropMinX()->GetFloatValue() != 0.0f ||
             in_Image.CropMaxX()->GetFloatValue() != 1.0f ||
             in_Image.CropMinY()->GetFloatValue() != 0.0f ||
             in_Image.CropMaxY()->GetFloatValue() != 1.0f)
        {
            io_Context.Logf(
                LOG_WARNING,
                "Cropping in image \"%s\" is not supported.",
                filename);
        }

        io_Context.GetTextureMap().AddSourceTexture(
            in_Image.Name().GetText(), NiFixedString(filename));

        return true;
    }

    //---------------------------------------------------------------------------
}
