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
#include "NiMAXPlatformMaterial.h"
#include "NiMAXPlatformMaterialClassDesc.h"

#define NUM_PLATFORMS_SUPPORTED 5

// the one instance of the paramDlg
NiMAXPlatformMaterialDlgProc* NiMAXPlatformMaterial::ms_pParamDlg;

static NiMAXPlatformMaterialClassDesc _platformMaterialCD;

ClassDesc* GetNiMAXPlatformMaterialDesc()
{ 
    return &_platformMaterialCD; 
}

enum { platformMaterial_params };  // m_pkBlock ID
// NiMAXPlatformMaterial_params param IDs

enum 
{ 
    platformMaterial_vp_n_mtl, 
    platformMaterial_mtl_one,    // PC/Generic
    platformMaterial_mtl_two,    // Xbox 360
    platformMaterial_mtl_three,    // PS3
    platformMaterial_mtl_four,    // Wii
    platformMaterial_mtl_five    // D3D10
};

//---------------------------------------------------------------------------

class PlatformPBAccessor : public PBAccessor
{
public:
    inline void Set(PB2Value& v, ReferenceMaker* owner, ParamID id, int, 
        TimeValue)    // set from v
    {
        NiMAXPlatformMaterial* mtl = (NiMAXPlatformMaterial*)owner;
        switch (id)
        {
            // use item data to unscramble sorted lists
            case platformMaterial_mtl_one: 
                mtl->ReplaceReference(MTL_ONE_REF, v.r);
                break;
            case platformMaterial_mtl_two: 
                mtl->ReplaceReference(MTL_TWO_REF, v.r);
                break;
            case platformMaterial_mtl_three:
                mtl->ReplaceReference(MTL_THREE_REF, v.r);
                break;
            case platformMaterial_mtl_four:
                mtl->ReplaceReference(MTL_FOUR_REF, v.r);
                break;
            case platformMaterial_mtl_five:
                mtl->ReplaceReference(MTL_FIVE_REF, v.r);
                break;
        } // end, switch
    } // end, set accessor

};

//---------------------------------------------------------------------------
static PlatformPBAccessor platformPBAccessor;


//---------------------------------------------------------------------------
static ParamBlockDesc2 platformMaterial_param_blk ( platformMaterial_params,
    _T("parameters"),  0, &_platformMaterialCD, 
    P_AUTO_CONSTRUCT + P_AUTO_UI, PB_REF, 
    //rollout
    IDD_PLATFORM_MATERIAL, IDS_NI_PLATFORM_MTL_PARAMS, 0, 0, NULL, 
    // params
    platformMaterial_mtl_one, _T("PCMaterial"),   TYPE_MTL,  P_OWNERS_REF,
        IDS_NI_MTL_ONE,
        p_refno,        MTL_ONE_REF,
        p_submtlno,     0,      
        p_ui,           TYPE_MTLBUTTON, IDC_MTL_ONE,
        end,
    platformMaterial_mtl_two, _T("XenonMaterial"), TYPE_MTL, P_OWNERS_REF, 
        IDS_NI_MTL_TWO,
        p_refno,        MTL_TWO_REF,
        p_submtlno,     1,
        p_ui,           TYPE_MTLBUTTON, IDC_MTL_TWO,
        end,
    platformMaterial_mtl_three, _T("PS3Material"), TYPE_MTL, P_OWNERS_REF, 
        IDS_NI_MTL_THREE,
        p_refno,        MTL_THREE_REF,
        p_submtlno,     2,
        p_ui,           TYPE_MTLBUTTON, IDC_MTL_THREE,
        end,
    platformMaterial_mtl_four, _T("WiiMaterial"), TYPE_MTL, P_OWNERS_REF, 
        IDS_NI_MTL_FOUR,
        p_refno,        MTL_FOUR_REF,
        p_submtlno,     3,
        p_ui,           TYPE_MTLBUTTON, IDC_MTL_FOUR,
        end,
    platformMaterial_mtl_five, _T("D3D10Material"), TYPE_MTL, P_OWNERS_REF, 
        IDS_NI_MTL_FIVE,
        p_refno,        MTL_FIVE_REF,
        p_submtlno,     4,
        p_ui,           TYPE_MTLBUTTON, IDC_MTL_FIVE,
        end,
    platformMaterial_vp_n_mtl, _T("viewportMtlIndex"), TYPE_INT,    0, 
        IDS_NI_VP_MTL,
        p_default,      0,
        p_range,        0,  1,
        p_ui,           TYPE_RADIO, 5, 
                        IDC_PLATFORM_MATERIAL_VP_USE1, 
                        IDC_PLATFORM_MATERIAL_VP_USE2, 
                        IDC_PLATFORM_MATERIAL_VP_USE3, 
                        IDC_PLATFORM_MATERIAL_VP_USE4,
                        IDC_PLATFORM_MATERIAL_VP_USE5,
        end,

    end
);


//---------------------------------------------------------------------------
//dialog stuff to get the Set Ref button
class NiMAXPlatformMaterialDlgProc : public ParamMap2UserDlgProc {
//public ParamMapUserDlgProc {
    public:
        NiMAXPlatformMaterial *platformMaterial;        
        BOOL valid;
        HWND hPanel; 
        NiMAXPlatformMaterialDlgProc(NiMAXPlatformMaterial *m) {
            platformMaterial = m;
            valid   = FALSE;
        }       
        BOOL DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,
            WPARAM wParam,LPARAM lParam);       
        inline void DeleteThis() {NiDelete this;}

};


//---------------------------------------------------------------------------
BOOL NiMAXPlatformMaterialDlgProc::DlgProc(
        TimeValue,IParamMap2 *map,HWND,UINT msg,WPARAM,
        LPARAM)
    {
    Rect rect;
    platformMaterial = 
        (NiMAXPlatformMaterial*)map->GetParamBlock()->GetOwner(); 
    switch (msg) {
        case WM_PAINT: {
//          em->EnableAffectRegion (t);
//          PAINTSTRUCT ps;
//          HDC hdc = BeginPaint(hWnd,&ps);
//          DrawCurve(hWnd,hdc,platformMaterial);
//          EndPaint(hWnd,&ps);
            return FALSE;
            }

        case WM_COMMAND:  
//          switch (id) {
//              case IDC_NiMAXPlatformMaterial_MAP:
//                  platformMaterial->EnableStuff(); 
//                  break;
//              }
            break;                          
        case CC_SPINNER_CHANGE:
//          platformMaterial->Update(GetCOREInterface()->GetTime(),FOREVER);
//          GetClientRectP(GetDlgItem(hWnd,IDC_MIXCURVE),&rect);
//          InvalidateRect(hWnd,&rect,FALSE);
            return FALSE;
            break;

        default:
            return FALSE;
        }
    return FALSE;
    }



//---------------------------------------------------------------------------



NiMAXPlatformMaterial::NiMAXPlatformMaterial(BOOL loading) : m_kReshadeRQ(
    RR_None) // mjm - 06.02.00
{   
    m_pkBlock = NULL;
    m_pkMtlOne = m_pkMtlTwo = m_pkMtlThree = m_pkMtlFour = 
        m_pkMtlFive = NULL;
    m_kValidInt.SetEmpty();
    if (!loading) {
        _platformMaterialCD.MakeAutoParamBlocks(this);  
        // make and intialize paramblock2
        Init();
    }
}

//---------------------------------------------------------------------------
void NiMAXPlatformMaterial::DeleteThis()            
{ 
    NiDelete this; 
}
//---------------------------------------------------------------------------
void NiMAXPlatformMaterial::Init()
{
    _platformMaterialCD.Reset(this, TRUE);  // reset all pb2's
    ReplaceReference(MTL_ONE_REF,  NewDefaultStdMat());

    // this should be the hw material eventually
    ReplaceReference(MTL_TWO_REF, NewDefaultStdMat());
    ReplaceReference(MTL_THREE_REF, NewDefaultStdMat());
    ReplaceReference(MTL_FOUR_REF, NewDefaultStdMat());
    ReplaceReference(MTL_FIVE_REF, NewDefaultStdMat());
    
    for (unsigned int ui = 0; ui < NUM_PLATFORM_SUBMTLS; ui++)
    {
        Mtl* pkMtl = GetSubMtl(ui);
        if (pkMtl)
            GetCOREInterface()->AssignNewName(pkMtl);
    }
}

//---------------------------------------------------------------------------
void NiMAXPlatformMaterial::Reset()
{
    Init();
}

//---------------------------------------------------------------------------
void* NiMAXPlatformMaterial::GetInterface(ULONG id)
{
    if( id == IID_IReshading )
        return (IReshading*)( this );
    else if ( id == IID_IValidityToken )
        return (IValidityToken*)( this );
    else
        return Mtl::GetInterface(id);
}

//---------------------------------------------------------------------------
int NiMAXPlatformMaterial::VPDisplaySubMtl() 
{
    int m;
    Interval intForever = FOREVER;
    m_pkBlock->GetValue(platformMaterial_vp_n_mtl, 0, m, intForever);
    return m; 
} 

//---------------------------------------------------------------------------
Mtl * NiMAXPlatformMaterial::UseMtl() 
{
    int n;
    Interval intForever = FOREVER;
    m_pkBlock->GetValue(platformMaterial_vp_n_mtl,0, n, intForever);
    if (n >= NUM_PLATFORMS_SUPPORTED)
    {
        m_pkBlock->SetValue(platformMaterial_vp_n_mtl, 0, 0);
        n = 0;
    }
    if (n==0 && m_pkMtlOne) 
        return m_pkMtlOne;

    if (n==1 && m_pkMtlTwo)
        return m_pkMtlTwo;

    if (n==2 && m_pkMtlThree)
        return m_pkMtlThree;

    if (n==3 && m_pkMtlFour)
        return m_pkMtlFour;

    if (n==4 && m_pkMtlFive)
        return m_pkMtlFive;

    return m_pkMtlOne ? m_pkMtlOne : m_pkMtlTwo;
}

//---------------------------------------------------------------------------
Color NiMAXPlatformMaterial::GetAmbient(int mtlNum, BOOL backFace) 
{ 
    return UseMtl() ? UseMtl()->GetAmbient(mtlNum,backFace) : Color(0,0,0);
}       
//---------------------------------------------------------------------------
Color NiMAXPlatformMaterial::GetDiffuse(int mtlNum, BOOL backFace)
{ 
    return UseMtl() ? UseMtl()->GetDiffuse(mtlNum,backFace) : Color(0,0,0);
}               
//---------------------------------------------------------------------------
Color NiMAXPlatformMaterial::GetSpecular(int mtlNum, BOOL backFace)
{
    return UseMtl() ? UseMtl()->GetSpecular(mtlNum,backFace) : Color(0,0,0);
}       
//---------------------------------------------------------------------------
float NiMAXPlatformMaterial::GetXParency(int mtlNum, BOOL backFace) 
{
    return UseMtl() ? UseMtl()->GetXParency(mtlNum,backFace) : 0.0f;
}
//---------------------------------------------------------------------------
float NiMAXPlatformMaterial::GetShininess(int mtlNum, BOOL backFace) 
{
    return UseMtl() ? UseMtl()->GetXParency(mtlNum,backFace) : 0.0f;
}       
//---------------------------------------------------------------------------
float NiMAXPlatformMaterial::GetShinStr(int mtlNum, BOOL backFace) 
{
    return UseMtl() ? UseMtl()->GetXParency(mtlNum,backFace) : 0.0f;
}
//---------------------------------------------------------------------------
float NiMAXPlatformMaterial::WireSize(int mtlNum, BOOL backFace) 
{
    return UseMtl() ? UseMtl()->WireSize(mtlNum,backFace) : 0.0f;
}   
//---------------------------------------------------------------------------
ParamDlg* NiMAXPlatformMaterial::CreateParamDlg(HWND hwMtlEdit, 
    IMtlParams *imp)
{
// JBW: the main difference here is the automatic creation of a ParamDlg 
// by the new ClassDesc2 function CreateParamDlgs().  This mirrors the way
// BeginEditParams() can be redirected to the ClassDesc2 for automatic 
// ParamMap2 management.  In this case a special subclass of ParamDlg, 
// AutoMParamDlg, defined in IParamm2.h, is  created.  It can act as a 
//'master' ParamDlg to which you can add any number of 
// secondary dialogs and it will make sure all the secondary dialogs are
// kept  up-to-date and deleted as necessary.  

    // create the rollout dialogs
    IAutoMParamDlg* masterDlg = _platformMaterialCD.CreateParamDlgs(
        hwMtlEdit, imp, this);
    ms_pParamDlg = NiExternalNew NiMAXPlatformMaterialDlgProc(this);
    platformMaterial_param_blk.SetUserDlgProc( ms_pParamDlg );
    Interval intForever = FOREVER;
    Update(imp->GetTime(), intForever);
    return masterDlg;
}

//---------------------------------------------------------------------------
BOOL NiMAXPlatformMaterial::SetDlgThing(ParamDlg*)
{
    // JBW: set the appropriate 'thing' sub-object for each
    // secondary dialog
    return FALSE;
}
//---------------------------------------------------------------------------
void NiMAXPlatformMaterial::SetSubTexmap(int, Texmap *)
{
}
//---------------------------------------------------------------------------
void NiMAXPlatformMaterial::SetSubMtl(int i, Mtl *m)
{
    ReplaceReference(i+1, m);
    if (i==0) 
    {
        platformMaterial_param_blk.InvalidateUI(platformMaterial_mtl_one);
        m_kValidInt.SetEmpty();
    }   
    else if (i==1) 
    {
        platformMaterial_param_blk.InvalidateUI(platformMaterial_mtl_two);
        m_kValidInt.SetEmpty();
    }   
    else if (i==2) 
    {
        platformMaterial_param_blk.InvalidateUI(platformMaterial_mtl_three);
        m_kValidInt.SetEmpty();
    }   
    else if (i==3) 
    {
        platformMaterial_param_blk.InvalidateUI(platformMaterial_mtl_four);
        m_kValidInt.SetEmpty();
    }   
    else if (i==4) 
    {
        platformMaterial_param_blk.InvalidateUI(platformMaterial_mtl_five);
        m_kValidInt.SetEmpty();
    }   
}
//---------------------------------------------------------------------------
Mtl* NiMAXPlatformMaterial::GetSubMtl(int i)
{ 
    return GetPlatformMtl(i);
}
//---------------------------------------------------------------------------
TSTR NiMAXPlatformMaterial::GetSubMtlSlotName(int i) 
{
    switch (i)
    {
        default:
        case 0:
            return TSTR(GetString(IDS_NI_MTL_ONE));
        case 1:
            return TSTR(GetString(IDS_NI_MTL_TWO));
        case 2:
            return TSTR(GetString(IDS_NI_MTL_THREE));
        case 3:
            return TSTR(GetString(IDS_NI_MTL_FOUR));
        case 4:
            return TSTR(GetString(IDS_NI_MTL_FIVE));

    };
}
//---------------------------------------------------------------------------
void NiMAXPlatformMaterial::PreShade(ShadeContext& sc, 
    IReshadeFragment* pFrag)
{
    IReshading* pReshading = NULL;

    int lenChan = pFrag->NTextures(); // indx of len channel
    pFrag->AddIntChannel(0);//placeholder

    int mtlLength = 0;
    int useMtl;
    Interval intForever = FOREVER;
    m_pkBlock->GetValue(platformMaterial_vp_n_mtl, 0, useMtl, intForever);
    
    Mtl* mtl = GetPlatformMtl(useMtl);
    if ( mtl)
    {
        pReshading = (IReshading*)(mtl->GetInterface(IID_IReshading));
    }

    if( pReshading ) 
    {
        pReshading->PreShade(sc, pFrag);
        mtlLength = pFrag->NTextures() - lenChan - 1;
    }
    // save the length into the int channel
    pFrag->SetIntChannel(lenChan, mtlLength);
}

//---------------------------------------------------------------------------
void NiMAXPlatformMaterial::PostShade(ShadeContext& sc, 
    IReshadeFragment* pFrag, int& nextTexIndex, IllumParams*)
{
    IReshading* pReshading;
    int useMtl;
    Interval intForever = FOREVER;
    m_pkBlock->GetValue(platformMaterial_vp_n_mtl, 0, useMtl, intForever);
    
    Mtl *mtl = GetPlatformMtl(useMtl);

    int mtlLength = pFrag->GetIntChannel(nextTexIndex++);
 
    ShadeOutput out1;
    if (mtl)
    {
        sc.ResetOutput();
        pReshading = (IReshading*)(mtl->GetInterface(IID_IReshading));
        if( pReshading ) 
            pReshading->PostShade(sc, pFrag, nextTexIndex);
    } 
    else 
    {
        // i think this is always an error
        nextTexIndex += mtlLength;
    }
}

//---------------------------------------------------------------------------
Sampler*  NiMAXPlatformMaterial::GetPixelSampler(int mtlNum, BOOL backFace )
{
    int useMtl;
    Interval intForever = FOREVER;
    m_pkBlock->GetValue(platformMaterial_vp_n_mtl, 0, useMtl, intForever);
    Mtl *mtl = GetPlatformMtl(useMtl);

    Sampler* pSampler = NULL;
    if( mtl )
        pSampler = mtl->GetPixelSampler( mtlNum, backFace );
    
    if ( !pSampler )
    {
        mtl = (useMtl == 1) ? m_pkMtlOne : m_pkMtlTwo; // other mtl
        pSampler = mtl->GetPixelSampler( mtlNum, backFace );
    }
    
    return pSampler;
}

//---------------------------------------------------------------------------
void NiMAXPlatformMaterial::Shade(ShadeContext& sc)
{
    if( gbufID )
        sc.SetGBufferID(gbufID);

    int useMtl;
    Interval intForever = FOREVER;
    m_pkBlock->GetValue(platformMaterial_vp_n_mtl, 0, useMtl, intForever);
    Mtl *mtl = GetPlatformMtl(useMtl);
    if (mtl)
        mtl->Shade(sc);
}

//---------------------------------------------------------------------------
void NiMAXPlatformMaterial::Update(TimeValue t, Interval& valid)
{   
    m_kValidInt = FOREVER;
    if (m_pkMtlOne) m_pkMtlOne->Update(t, valid);
    if (m_pkMtlTwo) m_pkMtlTwo->Update(t, valid);
    if (m_pkMtlThree) m_pkMtlThree->Update(t, valid);
    if (m_pkMtlFour) m_pkMtlFour->Update(t, valid);
    if (m_pkMtlFive) m_pkMtlFive->Update(t, valid);
}

//---------------------------------------------------------------------------
Interval NiMAXPlatformMaterial::Validity(TimeValue t)
{
    Interval valid = FOREVER;       
    if (m_pkMtlOne) valid &= m_pkMtlOne->Validity(t);
    if (m_pkMtlTwo) valid &= m_pkMtlTwo->Validity(t);
    if (m_pkMtlThree) valid &= m_pkMtlThree->Validity(t);
    if (m_pkMtlFour) valid &= m_pkMtlFour->Validity(t);
    if (m_pkMtlFive) valid &= m_pkMtlFive->Validity(t);
    return valid;
}

//---------------------------------------------------------------------------
Animatable* NiMAXPlatformMaterial::SubAnim(int i)
{
    switch (i) 
    {
        case 0: return m_pkBlock;
        case 1: return GetPlatformMtl(0);
        case 2: return GetPlatformMtl(1);
        case 3: return GetPlatformMtl(2);
        case 4: return GetPlatformMtl(3);
        case 5: return GetPlatformMtl(4);
        default: return NULL;
    }
}

//---------------------------------------------------------------------------
TSTR NiMAXPlatformMaterial::SubAnimName(int i)
{
    switch (i) 
    {
        case 0: return GetString(IDS_NI_PLATFORM_MTL_BASE_PARAMETERS);
        case 1: return GetSubMtlTVName(0); 
        case 2: return GetSubMtlTVName(1); 
        case 3: return GetSubMtlTVName(2);
        case 4: return GetSubMtlTVName(3);
        case 5: return GetSubMtlTVName(4);
        default: return _T("");
    }
}

//---------------------------------------------------------------------------
Mtl* NiMAXPlatformMaterial::GetPlatformMtl(unsigned int uiWhichMtl)
{
    switch(uiWhichMtl)
    {
        default:
            return NULL;
        case 0: 
            return m_pkMtlOne;
        case 1: 
            return m_pkMtlTwo;
        case 2:
            return m_pkMtlThree;
        case 3:
            return m_pkMtlFour;
        case 4:
            return m_pkMtlFive;
    }
}

//---------------------------------------------------------------------------
void NiMAXPlatformMaterial::SetPlatformMtl(Mtl* pkMtl, 
    unsigned int uiWhichMtl)
{
    switch(uiWhichMtl)
    {
        case 0: 
            m_pkMtlOne = pkMtl;
            break;
        case 1: 
            m_pkMtlTwo = pkMtl;
            break;
        case 2:
            m_pkMtlThree = pkMtl;
            break;
        case 3:
            m_pkMtlFour = pkMtl;
            break;
        case 4:
            m_pkMtlFive = pkMtl;
            break;
    }
}
//---------------------------------------------------------------------------
RefTargetHandle NiMAXPlatformMaterial::GetReference(int i)
{
    switch (i) 
    {
        case 0: return m_pkBlock;
        case 1: return GetPlatformMtl(0);
        case 2: return GetPlatformMtl(1);
        case 3: return GetPlatformMtl(2);
        case 4: return GetPlatformMtl(3);
        case 5: return GetPlatformMtl(4);
        default: return NULL;
    }
}

//---------------------------------------------------------------------------
void NiMAXPlatformMaterial::SetReference(int i, RefTargetHandle rtarg)
{
    switch (i) 
    {
        case 0: m_pkBlock = (IParamBlock2*)rtarg; break;
        case 1: SetPlatformMtl((Mtl*)rtarg, 0); break;
        case 2: SetPlatformMtl((Mtl*)rtarg, 1); break;
        case 3: SetPlatformMtl((Mtl*)rtarg, 2); break;
        case 4: SetPlatformMtl((Mtl*)rtarg, 3); break;
        case 5: SetPlatformMtl((Mtl*)rtarg, 4); break;
    }
}

//---------------------------------------------------------------------------
RefTargetHandle NiMAXPlatformMaterial::Clone(RemapDir &remap)
{
    NiMAXPlatformMaterial *mtl = NiExternalNew NiMAXPlatformMaterial(FALSE);
    *((MtlBase*)mtl) = *((MtlBase*)this);  // copy superclass stuff
    mtl->ReplaceReference(PB_REF, remap.CloneRef(m_pkBlock));

    if (m_pkMtlOne) 
        mtl->ReplaceReference(MTL_ONE_REF, remap.CloneRef(m_pkMtlOne));

    if (m_pkMtlTwo) 
        mtl->ReplaceReference(MTL_TWO_REF, remap.CloneRef(m_pkMtlTwo));

    if (m_pkMtlThree) 
        mtl->ReplaceReference(MTL_THREE_REF, remap.CloneRef(m_pkMtlThree));

    if (m_pkMtlFour) 
        mtl->ReplaceReference(MTL_FOUR_REF, remap.CloneRef(m_pkMtlFour));

    if (m_pkMtlFive) 
        mtl->ReplaceReference(MTL_FIVE_REF, remap.CloneRef(m_pkMtlFive));

    BaseClone(this, mtl, remap);    
    return mtl;
}

//---------------------------------------------------------------------------

RefResult NiMAXPlatformMaterial::NotifyRefChanged(Interval, 
    RefTargetHandle hTarget, PartID&, RefMessage message)
{
    switch (message) 
    {
        case REFMSG_CHANGE:
        {
            m_kValidInt.SetEmpty();
            if (hTarget == m_pkBlock)
            {
                ParamID changing_param = m_pkBlock->LastNotifyParamID();
                platformMaterial_param_blk.InvalidateUI(changing_param);

                if((changing_param == platformMaterial_vp_n_mtl)
                    ||(changing_param == platformMaterial_mtl_one)
                    ||(changing_param == platformMaterial_mtl_two)
                    ||(changing_param == platformMaterial_mtl_three)
                    ||(changing_param == platformMaterial_mtl_four)
                    ||(changing_param == platformMaterial_mtl_five))
                    m_kReshadeRQ = RR_NeedReshade;
            }

//          if (hTarget==m_pkBlock) m_kValidInt.SetEmpty();

            if (hTarget != NULL) 
            {
                switch (hTarget->SuperClassID()) 
                {
                    case MATERIAL_CLASS_ID: 
                    {
                        IReshading* r = static_cast<IReshading*>(
                            hTarget->GetInterface(IID_IReshading));
                        m_kReshadeRQ = (r == NULL)? RR_None : 
                            r->GetReshadeRequirements();
                    } break;
                }
            }
            break;
        }
        case REFMSG_SUBANIM_STRUCTURE_CHANGED:
        {
            m_kReshadeRQ = RR_NeedPreshade;
            NotifyChanged();
            break;
        }
    }// end, switch
    return REF_SUCCEED;
}

//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////
//
//  Save & Load
//
// Note: ALL Materials and texmaps must have a Save and Load to save 
// and load the MtlBase info.
#define HEADER_CHUNK 0x4000
#define VERSION_CHUNK 0x1000

#define CURRENT_PLATFORM_MATERIAL_VERSION 3

IOResult NiMAXPlatformMaterial::Save(ISave *isave) 
{ 
    IOResult res;

    // Save common stuff
    isave->BeginChunk(HEADER_CHUNK);
    res = MtlBase::Save(isave);
    if (res!=IO_OK) return res;
    isave->EndChunk();

    isave->BeginChunk(VERSION_CHUNK);
    int version = CURRENT_PLATFORM_MATERIAL_VERSION;
    ULONG   nBytes;
    isave->Write( &version, sizeof(version), &nBytes );
    isave->EndChunk();

    return IO_OK;
}
//---------------------------------------------------------------------------
class NiMAXPlatformMaterialPostLoad : public PostLoadCallback {
    public:
        NiMAXPlatformMaterial *n;
        NiMAXPlatformMaterialPostLoad(NiMAXPlatformMaterial *ns) {n = ns;}
        inline void proc(ILoad *) { 
            // your goop here
            NiDelete this; 
        } 
};


//---------------------------------------------------------------------------
IOResult NiMAXPlatformMaterial::Load(ILoad *iload) 
{ 
    IOResult res;
    int version;

    while (IO_OK==(res=iload->OpenChunk()))
    {
        switch( iload->CurChunkID() ){
            case HEADER_CHUNK:
                res = MtlBase::Load(iload);
                break;
            case VERSION_CHUNK:
                ULONG nb;
                res = iload->Read(&version, sizeof(version), &nb);
                break;
        } // end switch
        
        iload->CloseChunk();
    
        if( res != IO_OK ) 
            return res;
    
    } // end, while still chunks

    // if need be register version converter
    // if( version < CURRENT_PLATFORM_MATERIAL_VERSION )
    //      iload->RegisterPostLoadCallback( 
    // new NiMAXPlatformMaterialPostLoad(this) );

    return IO_OK;
}

//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////
//
//  Channel Evaluations
//

float NiMAXPlatformMaterial::EvalDisplacement(ShadeContext& sc) 
{
    int useMtl;
    Interval intForever = FOREVER;
    m_pkBlock->GetValue(platformMaterial_vp_n_mtl, 0, useMtl, intForever);
    Mtl* mtl = GetPlatformMtl(useMtl);

    if( mtl )
    {
        return mtl->EvalDisplacement(sc);
    }
    else 
    {
        return 0.0f;
    }
}

//---------------------------------------------------------------------------
Interval NiMAXPlatformMaterial::DisplacementValidity(TimeValue t)
{
    int useMtl;
    Interval intForever = FOREVER;
    m_pkBlock->GetValue(platformMaterial_vp_n_mtl, 0, useMtl, intForever);
    Mtl* mtl = GetPlatformMtl(useMtl);

    Interval iv;
    iv.SetInfinite();
    
    if (mtl)
    {
        iv &= mtl->DisplacementValidity(t);
    }
    return iv;
} 

//---------------------------------------------------------------------------
bool NiMAXPlatformMaterial::IsOutputConst(ShadeContext& sc, int stdID)
{
    int useMtl;
    Interval intForever = FOREVER;
    m_pkBlock->GetValue(platformMaterial_vp_n_mtl, 0, useMtl, intForever);
    Mtl *mtl = GetPlatformMtl(useMtl);

    if (mtl && !mtl->IsOutputConst(sc, stdID)) 
        return false;

    return true;
}

//---------------------------------------------------------------------------
// Evaluates the material on a single texmap channel. 
// For a mono channel, the value is copied in all 3 components of the 
// output color.
// 
bool NiMAXPlatformMaterial::EvalColorStdChannel(ShadeContext& sc, 
    int stdID, Color& outClr)
{
    int useMtl;
    Interval intForever = FOREVER;
    m_pkBlock->GetValue(platformMaterial_vp_n_mtl, 0, useMtl, intForever);
    Mtl *mtl = GetPlatformMtl(useMtl);

    bool bRes = false;

    if (mtl)
    {
        bRes = mtl->EvalColorStdChannel(sc, stdID, outClr);
    } 
    
    return bRes;
}

//---------------------------------------------------------------------------
// Evaluates the material on a single texmap channel. 
//
bool NiMAXPlatformMaterial::EvalMonoStdChannel(ShadeContext& sc, int stdID, 
    float& outVal)
{
    int useMtl;
    Interval intForever = FOREVER;
    m_pkBlock->GetValue(platformMaterial_vp_n_mtl, 0, useMtl, intForever);
    Mtl *mtl = GetPlatformMtl(useMtl);

    bool bRes = false;

    if ( mtl )
    {
        bRes = mtl->EvalMonoStdChannel(sc, stdID, outVal);
    } 
    
    return bRes;
}

//---------------------------------------------------------------------------
Mtl* NiMAXPlatformMaterial::GetDevImagePlatformMtl(
    NiDevImageConverter::Platform ePlatform)
{
    Mtl* pSub = NULL;
    switch(ePlatform)
    {
        
        case NiDevImageConverter::D3D10 :
            pSub = GetPlatformMtl(PLATFORM_PC_D3D10_ID);
            break;
        case NiDevImageConverter::DX9 :
        case NiDevImageConverter::ANY :
            pSub = GetPlatformMtl(PLATFORM_PC_ID);
            break;
        case NiDevImageConverter::XENON :
            pSub = GetPlatformMtl(PLATFORM_XENON_ID);
            break;
        case NiDevImageConverter::PLAYSTATION3:
            pSub = GetPlatformMtl(PLATFORM_PS3_ID);
            break;
        case NiDevImageConverter::WII:
            pSub = GetPlatformMtl(PLATFORM_WII_ID);
            break;
    }
    return pSub;
}
