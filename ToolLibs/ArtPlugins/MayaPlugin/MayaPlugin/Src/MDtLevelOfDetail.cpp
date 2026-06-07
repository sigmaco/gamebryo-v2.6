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

// Precompiled Headers
#include "MayaPluginPCH.h"


// Declare a global
kMDtLevelOfDetailManager gLevelOfDetailManager;



//---------------------------------------------------------------------------
kMDtLevelOfDetail::kMDtLevelOfDetail()
{
    m_lShapeIndex = -1;
    m_szName = NULL;

    m_MObjectLOD = MObject::kNullObj;
    m_MObjectParent = MObject::kNullObj;

    // Translate Attributes
    m_fTranslateX = 0.0f;
    m_fTranslateY = 0.0f;
    m_fTranslateZ = 0.0f;
    m_fRotateX = 0.0f;
    m_fRotateY = 0.0f;
    m_fRotateZ = 0.0f;
    m_fScaleX = 0.0f;
    m_fScaleY = 0.0f;
    m_fScaleZ = 0.0f;

    m_iNumThresholds;
    m_pfThresholds = NULL;

    m_bUseScreenLOD = false;
    m_iNumScreenThresholds = 0;
}
//---------------------------------------------------------------------------
kMDtLevelOfDetail::~kMDtLevelOfDetail()
{
    // Free the allocated name
    if(m_szName)
    {
        NiFree(m_szName);
        m_szName = NULL;
    }

    if(m_pfThresholds)
    {
        NiFree(m_pfThresholds);
        m_pfThresholds = NULL;
    }

}
//---------------------------------------------------------------------------
bool kMDtLevelOfDetail::Load(MObject LOD, MObject Parent)
{
    MStatus stat = MS::kSuccess;
    MFnDependencyNode dgNode;
    stat = dgNode.setObject(LOD);

    m_MObjectLOD = LOD;
    m_MObjectParent = Parent;


    // Create the Name buffer
    size_t stNameLength = strlen( dgNode.name().asChar() ) + 1;

    m_szName = NiAlloc(char, stNameLength);
    
    NiStrcpy( m_szName, stNameLength, dgNode.name().asChar() );  

    // ----------------------------------------------------------
    MPlug Plug = dgNode.findPlug("translateX");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fTranslateX ) != MS::kSuccess )
        printf("Error Reading translateX\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("translateY");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fTranslateY ) != MS::kSuccess )
        printf("Error Reading translateY\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("translateZ");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fTranslateZ ) != MS::kSuccess )
        printf("Error Reading translateZ\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("rotateX");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fRotateX ) != MS::kSuccess )
        printf("Error Reading rotateX\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("rotateY");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fRotateY ) != MS::kSuccess )
        printf("Error Reading rotateY\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("rotateZ");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fRotateZ ) != MS::kSuccess )
        printf("Error Reading rotateZ\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("scaleX");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fScaleX ) != MS::kSuccess )
        printf("Error Reading scaleX\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("scaleY");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fScaleY ) != MS::kSuccess )
        printf("Error Reading scaleY\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("scaleZ");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fScaleZ ) != MS::kSuccess )
        printf("Error Reading scaleZ\n");

    Plug = dgNode.findPlug("threshold");
    NIASSERT( !Plug.isNull() );

    NIASSERT(Plug.numElements() > 0);

    m_iNumThresholds = Plug.numElements();
    
    m_pfThresholds = NiAlloc(float, m_iNumThresholds);

    int iLoop = 0;
    for(; iLoop < m_iNumThresholds; iLoop++)
    {
        MString command = MString("getAttr ") + MString(m_szName) + 
            MString(".threshold[") + iLoop + MString("];");

        double dResult;

        stat = MGlobal::executeCommand(command, dResult);
        NIASSERT(stat == MS::kSuccess);

        m_pfThresholds[iLoop] = (float)dResult;
    }


    // Check for Extra Screen Attributes
    Plug = dgNode.findPlug("Ni_Use_ScreenLOD", &stat);
    if(stat != MS::kSuccess)
        return true;


    if(Plug.getValue(m_bUseScreenLOD) != MS::kSuccess)
        return true;
    
    // Don't bother collecting info if we arn't going to use it.
    if(!m_bUseScreenLOD)
        return true;

    char szBuffer[256];

    for(iLoop = 0; iLoop < 10; iLoop++)
    {
        NiSprintf(szBuffer, 256, "Ni_ScreenLOD_%d", iLoop + 1);
        Plug = dgNode.findPlug(szBuffer, &stat);
        if(stat != MS::kSuccess)
        {
            // Disable screen LOD if we fail
            m_bUseScreenLOD = false;
            return true;
        }

        if(Plug.getValue(m_afScreenThresholds[iLoop]) != MS::kSuccess)
        {
            // Disable screen LOD if we fail
            m_bUseScreenLOD = false;
            return true;
        }
    }

    // Bubble short the values from High to low
    bool bFound = true;

    while(bFound)
    {
        bFound = false;
        
        for(iLoop = 0; iLoop < (10 - 1); iLoop++)
        {
            if(m_afScreenThresholds[iLoop] < m_afScreenThresholds[iLoop+1])
            {
                float fTemp = m_afScreenThresholds[iLoop];
                m_afScreenThresholds[iLoop] = m_afScreenThresholds[iLoop+1];
                m_afScreenThresholds[iLoop+1] = fTemp;
                bFound = true;
            }
        }
    }

    // Count the values that are greater the 0.0
    m_iNumScreenThresholds = 0;
    for(iLoop = 0; iLoop < 10; iLoop++)
    {
        if(m_afScreenThresholds[iLoop] >= 0.0f)
            m_iNumScreenThresholds++;
    }

    // Disable Screen LOD if there are no thresholds
    if(m_iNumScreenThresholds == 0)
    {
        m_bUseScreenLOD = false;
    }
    return true;
}




//---------------------------------------------------------------------------
//
//   LEVEL OF DETAIL MANAGER
//
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
kMDtLevelOfDetailManager::kMDtLevelOfDetailManager()
{
    m_iNumLevelOfDetail = 0;
    m_ppLevelOfDetail = NULL;

}
//---------------------------------------------------------------------------
kMDtLevelOfDetailManager::~kMDtLevelOfDetailManager()
{
    Reset();
}
//---------------------------------------------------------------------------
void kMDtLevelOfDetailManager::Reset()
{
    if( m_ppLevelOfDetail )
    {
        unsigned int uiLoop;
        for (uiLoop = 0; uiLoop < (unsigned int)m_iNumLevelOfDetail; uiLoop++)
            NiExternalDelete m_ppLevelOfDetail[uiLoop];

        NiFree(m_ppLevelOfDetail);
        m_ppLevelOfDetail = NULL;

        m_iNumLevelOfDetail = 0;
    }

}
//---------------------------------------------------------------------------
int kMDtLevelOfDetailManager::GetNumLevelOfDetail()
{
    return m_iNumLevelOfDetail;
}
//---------------------------------------------------------------------------
int kMDtLevelOfDetailManager::AddLevelOfDetail( 
    kMDtLevelOfDetail* pNewLevelOfDetail)
{
    // INCREASE THE SIZE OF THE ARRAY
    kMDtLevelOfDetail** ppExpandedArray = 
        NiAlloc(kMDtLevelOfDetail*, m_iNumLevelOfDetail+1);

    // Copy over the old array;
    for(int iLoop = 0; iLoop < m_iNumLevelOfDetail; iLoop++)
    {
        ppExpandedArray[iLoop] = m_ppLevelOfDetail[iLoop];
    }

    // Add the new particle System
    ppExpandedArray[m_iNumLevelOfDetail] = pNewLevelOfDetail;

    // DELETE THE OLD ARRAY
    NiFree(m_ppLevelOfDetail);

    // Assign the newly created array
    m_ppLevelOfDetail = ppExpandedArray;

    return m_iNumLevelOfDetail++;
}
//---------------------------------------------------------------------------
kMDtLevelOfDetail* kMDtLevelOfDetailManager::GetLevelOfDetail( int iIndex )
{
    NIASSERT(iIndex >= 0);
    NIASSERT(iIndex < m_iNumLevelOfDetail);

    return m_ppLevelOfDetail[iIndex];
}
//---------------------------------------------------------------------------
int kMDtLevelOfDetailManager::GetLevelOfDetail( MObject mobj )
{
    // Scan the Array of Particle Systems looking for the match
    for(int iLoop = 0; iLoop < m_iNumLevelOfDetail; iLoop++)
    {
        if(m_ppLevelOfDetail[iLoop]->m_MObjectLOD == mobj)
            return iLoop;
    }

    return -1;
}

//---------------------------------------------------------------------------
//
//  BASIC ADDING FUNCTIONS
//
//---------------------------------------------------------------------------
int addLevelOfDetail(MObject LOD, MObject Parent)
{
    // Don't add if it was already added
    if (gLevelOfDetailManager.GetLevelOfDetail(LOD) != -1)
        return -1;

    kMDtLevelOfDetail* pNewLevelOfDetail = NiExternalNew kMDtLevelOfDetail();

    if(!pNewLevelOfDetail->Load(LOD, Parent))
    {
        NiDelete pNewLevelOfDetail;
        return -1;
    }

    return gLevelOfDetailManager.AddLevelOfDetail(pNewLevelOfDetail);
}
//---------------------------------------------------------------------------
void gLevelOfDetailNew()
{
    // INITIALIZE AT THE START OF THE EXPORTER

    gLevelOfDetailManager.Reset();
}
//---------------------------------------------------------------------------
int DtExt_LevelOfDetailGetTransform(int LevelOfDetailID, MObject &obj )
{
    kMDtLevelOfDetail* pkLOD = 
        gLevelOfDetailManager.GetLevelOfDetail(LevelOfDetailID);

    if(pkLOD == NULL)
        return 0;

    obj = pkLOD->m_MObjectLOD;
    return 1;
}
//---------------------------------------------------------------------------
