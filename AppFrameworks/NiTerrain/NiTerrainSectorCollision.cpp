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

#include"NiTerrainPCH.h"

#include "NiTerrainSector.h"
#include "NiTerrainUtils.h"

//---------------------------------------------------------------------------
bool NiTerrainSector::CollideWithRay(NiRay& kRay,
    NiUInt32 uiDetailLOD) const
{
    if (!m_pkQuadData)
        return false;

    if (kRay.GetDirection().SqrLength() == 0.0f)
        return false;

    NiPoint3 kOldOrigin = kRay.GetOrigin();
    NiPoint3 kOldDirection = kRay.GetDirection();
    float fOldLength = kRay.GetLength();
    NiTransform kTerrainTransform = GetWorldTransform();

    // Transform the ray to terrain space
    bool bModifiedZ = false;

    if (kOldOrigin.z == NI_INFINITY)
    {
        kOldOrigin.z = 0.0f;
        bModifiedZ = true;
    }

    NiPoint3 kNewOrigin = kTerrainTransform.m_Rotate.Inverse() * (
        (kOldOrigin - kTerrainTransform.m_Translate)
        / kTerrainTransform.m_fScale);

    NiPoint3 kNewDirection = kOldDirection * kTerrainTransform.m_Rotate;

    float fNewLength = fOldLength * kTerrainTransform.m_fScale;

    if (bModifiedZ)
    {
        kNewOrigin -= kNewDirection * 1000;
        kOldOrigin.z = NI_INFINITY;
    }

    kRay.SetRay(kNewOrigin, kNewDirection, fNewLength);

    bool bRes;

    // Test if the ray is normal to the X-Y Plane:
    if (NiAbs(kRay.GetDirection().z) > (1.0f - FLT_EPSILON) && 
        NiAbs(kRay.GetDirection().z) < (1.0f + FLT_EPSILON))
    {
        kRay.SetRay(NiPoint3(kRay.GetOrigin().x, kRay.GetOrigin().y, 
            NI_INFINITY), NiPoint3(0.0f, 0.0f, 
            NiSign(kRay.GetDirection().z)), kRay.GetLength());
        bRes = NiTerrainUtils::TestRay2D(kRay, m_pkQuadData, uiDetailLOD);
    }
    else
    {
        bRes =  NiTerrainUtils::TestRay(kRay, m_pkQuadData, uiDetailLOD);
    }

    kRay.SetRay(kOldOrigin, kOldDirection, fOldLength);
    return bRes;
}
//---------------------------------------------------------------------------
bool NiTerrainSector::GetAdjacentSurfaceOpacity(
    const NiSurfaceMaskPixel& kPixel, const NiSurface* pkSurface,
    NiTerrainDataLeaf::BORDERS eDirection, NiUInt8& ucValue) const
{
    NiUInt32 uiMaxCoord = kPixel.GetMask()->GetWidth() - 1;
    ucValue = 0;

    // Regular case, the borders don't overlap
    if (eDirection == NiTerrainDataLeaf::BORDER_TOP && 
        kPixel.GetY() < uiMaxCoord)
    {
        ucValue = kPixel.GetMask()->GetAt(kPixel.GetX(), kPixel.GetY() + 1);
        return true;
    }
    else if (eDirection == NiTerrainDataLeaf::BORDER_BOTTOM && 
        kPixel.GetY() > 0)
    {
        ucValue = kPixel.GetMask()->GetAt(kPixel.GetX(), kPixel.GetY() - 1);
        return true;
    }
    else if (eDirection == NiTerrainDataLeaf::BORDER_LEFT && 
        kPixel.GetX() > 0)
    {
        ucValue = kPixel.GetMask()->GetAt(kPixel.GetX() - 1, kPixel.GetY());
        return true;
    }
    else if (eDirection == NiTerrainDataLeaf::BORDER_RIGHT && 
        kPixel.GetX() < uiMaxCoord)
    {
        ucValue = kPixel.GetMask()->GetAt(kPixel.GetX() + 1, kPixel.GetY());
        return true;
    }

    // Special case, we are at an edge
    NiUInt32 uiHalfMaskSize = kPixel.GetMask()->GetWidth() >> 1;
    const NiSurfaceMask* pkMask;
    const NiTerrainDataLeaf* pkAdjacent;
    const NiTerrainDataLeaf* pkTestee;
    NiIndex kAdjacentCoord;
    NiUInt32 uiCoordX = kPixel.GetX();
    NiUInt32 uiCoordY = kPixel.GetY();

    if (eDirection == NiTerrainDataLeaf::BORDER_TOP)
    {
        kAdjacentCoord.x = kPixel.GetX();
        kAdjacentCoord.y = 0;
    }
    else if (eDirection == NiTerrainDataLeaf::BORDER_BOTTOM)
    {
        kAdjacentCoord.x = kPixel.GetX();
        kAdjacentCoord.y = uiMaxCoord;
    }
    else if (eDirection == NiTerrainDataLeaf::BORDER_LEFT)
    {
        kAdjacentCoord.x = uiMaxCoord;
        kAdjacentCoord.y = kPixel.GetY();
    }
    else if (eDirection == NiTerrainDataLeaf::BORDER_RIGHT)
    {
        kAdjacentCoord.x = 0;
        kAdjacentCoord.y = kPixel.GetY();
    }
    else
        return false;

    // Do Search, find a mask
    pkAdjacent = kPixel.GetFirstContainingLeaf()->GetAdjacent(
        eDirection);
    if (!pkAdjacent)
        return false;
    pkTestee = pkAdjacent;

    // Search down
    uiCoordX = kAdjacentCoord.x;
    uiCoordY = kAdjacentCoord.y;
    while (pkTestee)
    {
        pkMask = pkTestee->GetSurfaceMask(pkSurface);
        if (pkMask)
        {
            if (pkMask->GetWidth() > kPixel.GetMask()->GetWidth())
            {
                NiUInt32 uiFactor = 
                    pkMask->GetWidth() / kPixel.GetMask()->GetWidth();
                uiCoordX /= uiFactor;
                uiCoordY /= uiFactor;
            }
            else
            {
                NiUInt32 uiFactor = 
                    kPixel.GetMask()->GetWidth() / pkMask->GetWidth();
                uiCoordX *= uiFactor;
                uiCoordY *= uiFactor;
            }

            ucValue = pkMask->GetAt(
                (unsigned short)uiCoordX,
                (unsigned short)uiCoordY);
            return true;
        }

        // Which leaf to look into?
        NiUInt8 ucChildID = 0;
        if (uiCoordY > uiHalfMaskSize)
        {
            ucChildID += 2;
            uiCoordY = (uiCoordY - uiHalfMaskSize) << 1;
        }
        else
        {
            // ucChildID = 0
            uiCoordY = (uiCoordY << 1);
        }

        if (uiCoordX > uiHalfMaskSize)
        {
            if (ucChildID == 0)
                ucChildID = 1;
            uiCoordX = (uiCoordX - uiHalfMaskSize) << 1;
        }
        else
        {
            if (ucChildID == 2)
                ucChildID = 3;
            uiCoordX = uiCoordX << 1;
        }

        pkTestee = pkTestee->GetChildAt(ucChildID);
    }

    // Search up
    uiCoordX = kAdjacentCoord.x;
    uiCoordY = kAdjacentCoord.y;
    while (pkTestee)
    {
        pkMask = pkTestee->GetSurfaceMask(pkSurface);
        if (pkMask)
        {
            if (pkMask->GetWidth() > kPixel.GetMask()->GetWidth())
            {
                NiUInt32 uiFactor = 
                    pkMask->GetWidth() / kPixel.GetMask()->GetWidth();
                uiCoordX /= uiFactor;
                uiCoordY /= uiFactor;
            }
            else
            {
                NiUInt32 uiFactor = 
                    kPixel.GetMask()->GetWidth() / pkMask->GetWidth();
                uiCoordX *= uiFactor;
                uiCoordY *= uiFactor;
            }

            ucValue = pkMask->GetAt(
                (unsigned short)uiCoordX,
                (unsigned short)uiCoordY);
            return true;
        }

        // Which child are we?
        NiUInt8 ucChildID = 0;
        for (NiUInt8 uc = 0; uc < 4; ++uc)
        {
            if (pkTestee->GetParent()->GetChildAt(uc) == pkTestee)
            {
                ucChildID = uc;
                break;
            }
        }
        
        uiCoordX >>= 1;
        uiCoordY >>= 1;
        if (ucChildID >= 2)
        {
            uiCoordY += uiHalfMaskSize;
        }

        if (ucChildID == 1 || ucChildID == 2)
        {
            uiCoordX += uiHalfMaskSize;
        }
                
        pkTestee = pkAdjacent->GetParent();
    }

    return false;
}
