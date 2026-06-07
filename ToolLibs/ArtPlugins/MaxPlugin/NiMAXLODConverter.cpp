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

#include "MaxImmerse.h"
#include "NiMAXLODConverter.h"
#include "NiMAXHierConverter.h"
#include <NiRangeLODData.h>
#include <NiScreenLODData.h>
void *NiMAXLODConverter::m_pUserData = NULL;


//---------------------------------------------------------------------------
NiMAXLODConverter::NiMAXLODConverter(
    TimeValue animStart,
    TimeValue animEnd)
{
    m_animStart = animStart;
    m_animEnd = animEnd;
}
//---------------------------------------------------------------------------
bool
NiMAXLODConverter::IsLODNode(INode *pMaxNode)
{
    CHECK_MEMORY();
    WORD id = 0;
    int i;

    if (pMaxNode->NumberOfChildren() == 0)
        return(false);

    BOOL bFound = false;
    if (pMaxNode->GetUserPropBool("LODGroup", bFound) && bFound)
    {
        return true;
    }

    for (i = 0; i < pMaxNode->NumberOfChildren(); i++) 
    {
        INode *pChild = pMaxNode->GetChildNode(i);        
        Control *pCont = pChild->GetVisController();

        if (pCont && pCont->ClassID()==LOD_CONTROL_CLASS_ID) 
        {
            LODCtrl *pLODCont = (LODCtrl*) pCont;

            if (i == 0)
                id = pLODCont->grpID;
            else if (pLODCont->grpID != id) 
                return(false);
        }           
        else
            return(false);
    }   

    return(true);
}

//---------------------------------------------------------------------------
int
NiMAXLODConverter::Convert(
    Interface *pIntf, 
    NiNode *pCurNode, 
    INode *pMaxNode, 
    ScaleInfo *pParentScale,
    bool bIsHidden)
{
    CHECK_MEMORY();
    NiLODNode *pLOD;    
    int i, iStatus;
    INode *pChild;
    NiBound sph;

    pLOD = NiNew NiLODNode();   
    if (pLOD == NULL)
        return(W3D_STAT_NO_MEMORY);
    

//    pLOD->UpdateOnlyActiveChild(false);
    pLOD->Update((float) m_animStart);
    sph = pLOD->GetWorldBound();
//    pLOD->UpdateOnlyActiveChild(true);

    BOOL bIsNewLODType = false;
    pMaxNode->GetUserPropBool("LODGroup", bIsNewLODType);

    if (bIsNewLODType)
    {
        int iChildCount = pMaxNode->NumberOfChildren();
        NiRangeLODData* pkLodData = NiNew NiRangeLODData();
        int iInitialChildCount = iChildCount;
        float* fDistances = NiAlloc(float, iChildCount);
        int* iDistancesIndex = NiAlloc(int, iChildCount);
        for (i = 0; i < iInitialChildCount; i++)
        {
            pChild = pMaxNode->GetChildNode(i);
            float fDist = 0.0f;
            if (pChild->GetUserPropFloat("LODDistance", fDist))
            {
                float fCurDist = fDist;
                int iCurIdx = i;
                
                for (int j = 0; j < i; j++)
                {
                    if (fDistances[j] > fCurDist)
                    {
                        float fTemp = fDistances[j];
                        int iTemp = iDistancesIndex[j];
                        fDistances[j] = fCurDist;
                        iDistancesIndex[j] = iCurIdx;
                        fCurDist = fTemp;
                        iCurIdx = iTemp;
                    }
                }

                fDistances[i]= fCurDist;
                iDistancesIndex[i] = iCurIdx;
            }
            else
            {
                char acString[256];
                NiSprintf(acString, 256, "LOD Root,\"%s\", contains a child,"
                    " \"%s\", with no LOD info. It will not be exported.", 
                    pMaxNode->GetName(), pChild->GetName());
                NILOGWARNING(acString);
                iChildCount--;
            }
        }

        pkLodData->SetNumRanges(iChildCount);

        for (int i = iChildCount - 1, iIndex = 0; i >= 0;iIndex++, i--)
        {
            NiMAXHierConverter hier(m_animStart, m_animEnd);
            NiNodePtr spResult;

            pChild = pMaxNode->GetChildNode(iDistancesIndex[i]);

            iStatus = hier.BuildNode(pIntf, pChild, &spResult, 
                                 pParentScale,true, 
                                 true);
            if (iStatus)
            {
                NiDelete pLOD;
                return(iStatus);
            }

            pLOD->SetAt(iIndex, spResult);

            float fNear = fDistances[i];
            float fFar = 99999999.0f;
            if (i+1 < iChildCount)
                fFar= fDistances[i+1];
            
            pkLodData->SetRange(iIndex, fNear, fFar);
        }

        NiFree(fDistances);
        NiFree(iDistancesIndex);
        pLOD->SetLODData(pkLodData);

    }
    else
    {
        int iChildCount = pMaxNode->NumberOfChildren();
        NiScreenLODData* pkLodData = NiNew NiScreenLODData();
        pkLodData->SetNumProportions(iChildCount-1);

        float* fProportions = NiAlloc(float, iChildCount);
        int* iProportionsIndex = NiAlloc(int, iChildCount);
        for (i = 0; i < iChildCount; i++)
        {
            float fFracFar, fFracNear;
            LODCtrl *pLODCont;

            pChild = pMaxNode->GetChildNode(i);
            pLODCont = (LODCtrl*) pChild->GetVisController();

            // MAX's values are multiplied by sqrt(640*640+480*480) == 800
            fFracNear = pLODCont->min / 800.0f;
            fFracFar = pLODCont->max / 800.0f;

            //pkLodData->SetProportion(i, fFracFar); 
            float fCurProportion = fFracNear;
            int iCurIdx = i;
                
            for (int j = 0; j < i; j++)
            {
                if (fProportions[j] > fCurProportion)
                {
                    float fTemp = fProportions[j];
                    int iTemp = iProportionsIndex[j];
                    fProportions[j] = fCurProportion;
                    iProportionsIndex[j] = iCurIdx;
                    fCurProportion = fTemp;
                    iCurIdx = iTemp;
                }
            }

            fProportions[i]= fCurProportion;
            iProportionsIndex[i] = iCurIdx;
        }

        for (int i = iChildCount - 1, iIndex = 0; i >= 0; iIndex++, i--)
        {
            NiMAXHierConverter hier(m_animStart, m_animEnd);
            NiNodePtr spResult;

            pChild = pMaxNode->GetChildNode(iProportionsIndex[i]);

            iStatus = hier.BuildNode(pIntf, pChild, &spResult, 
                                 pParentScale,true, 
                                 true);
            if (iStatus)
            {
                NiDelete pLOD;
                return(iStatus);
            }

            pLOD->SetAt(iIndex, spResult);

            float fFar = 1.0f;
            if (i+1 < iChildCount)
            {
                fFar= fProportions[i+1];        
                pkLodData->SetProportion(iIndex-1, fFar);
            }
            
        }

        NiFree(fProportions);
        NiFree(iProportionsIndex);
        
        pLOD->SetLODData(pkLodData);
    
    }   
    pLOD->SetAppCulled(bIsHidden);
    pLOD->SetIndex(0);

    pCurNode->AttachChild(pLOD);
    CHECK_MEMORY();

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------


