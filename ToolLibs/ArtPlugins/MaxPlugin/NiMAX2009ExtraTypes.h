// NOTE: This file is a concatenation of files from the samples in the MAX
// sdk that are not located in the sdk include directory.
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4201) // nonstandard extension used : nameless struct/union
#pragma warning(disable: 4238) // nonstandard extension used : class rvalue used as lvalue
#pragma warning(disable: 4239) // nonstandard extension used : 'default argument' : conversion from '' to ' &'
#pragma warning(disable: 4245) // 'argument' : conversion from '' to '', signed/unsigned mismatch
#pragma warning(disable: 4512) // '' : assignment operator could not be generated
#include "max.h"
#include "istdplug.h"
#include "iparamm.h"
#include "iparamb2.h"
#include "simpobj.h"
#include "simpmod.h"
#include "shaders.h"
#include "CS/BIPEXP.H"
#include <MeshNormalSpec.h>
// Reinclude assert header to override assert definition from MAX headers
// (which asserts even in Release mode).
#ifdef NI_OLD_STYLE_HEADERS
#include "assert.h"
#else
#include "cassert"
#endif
#pragma warning(pop)
#ifndef __MAX80__
#define __MAX80__

//---------------------------------------------------------------------------
#define NiStdMat StdMat2
#define NiParamBlock IParamBlock2
#define NiStdMtl StdMtl2

TCHAR *GetString(int id);
//---------------------------------------------------------------------------
// Found in file: .\maxsdk\samples\modifiers\morpher\wm3.h

/*===========================================================================*\
 | 
 |  FILE:   wM3.h
 |          Weighted Morpher for MAX R3
 |          Main header file
 | 
 |  AUTH:   Harry Denholm
 |          Copyright(c) Kinetix 1999
 |          All Rights Reserved.
 |
 |  HIST:   Started 22-5-98
 | 
\*===========================================================================*/


#ifndef __MORPHR3__H
#define __MORPHR3__H

#define MorphExport __declspec( dllexport )

#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4201) // nonstandard extension used : nameless struct/union
#pragma warning(disable: 4238) // nonstandard extension used : class rvalue used as lvalue
#pragma warning(disable: 4239) // nonstandard extension used : 'default argument' : conversion from '' to ' &'
#pragma warning(disable: 4245) // 'argument' : conversion from '' to '', signed/unsigned mismatch
#pragma warning(disable: 4512) // '' : assignment operator could not be generated

#include <Max.h>
#include <istdplug.h>
#include <meshadj.h>
#include <modstack.h>
#include <imtl.h>
#include <texutil.h>
#include <stdmat.h>
#include <macrorec.h>
#include <vector>
#include <XRef\IXRefMaterial.h>

#pragma warning(pop)

#define MR3_CLASS_ID        Class_ID(0x17bb6854, 0xa5cba2a3)
#define MR3_NUM_CHANNELS    100
#define MAX_TARGS           25

// paramblock index table
#define PB_OV_USELIMITS     0
#define PB_OV_SPINMIN       1
#define PB_OV_SPINMAX       2
#define PB_OV_USESEL        3
#define PB_AD_VALUEINC      4
#define PB_CL_AUTOLOAD      5

class MorphR3;
class M3Mat;

static Point3 junkpoint(0,0,0);
class morphChannel;

class TargetCache
{
public:
    long mNumPoints;
    INode *mTargetINode;
    std::vector<Point3> mTargetPoints;
    float mTargetPercent;

    TargetCache(){
        mNumPoints=0;
        mTargetINode=NULL;
        mTargetPercent = 0.0f;
    }

    TargetCache(const TargetCache &tcache);

    ~TargetCache(){
        mTargetPoints.erase(mTargetPoints.begin(), mTargetPoints.end());
        mTargetPoints.resize(0);
    }

    void Clear(void){
        mTargetPoints.erase(mTargetPoints.begin(), mTargetPoints.end());
        mTargetPoints.resize(0);
        mNumPoints=0;
        mTargetINode=NULL;
    }

    void operator=(const TargetCache &tcache); 

    void operator=(const morphChannel &mchan);
    void Init( INode *nd);
    void Reset(void);
    const Point3 &GetPoint(const long &index) { 
        if(index>=0 && index<mNumPoints) return mTargetPoints[index]; 
        return junkpoint;
    }
    
    IOResult Save(ISave* isave);
    IOResult Load(ILoad* iload);
    INode *RefNode(void) { return mTargetINode; }
    
};


/*===========================================================================*\
 | Morph channel manager class
 | Storage for all the morph data needed
\*===========================================================================*/

class morphChannel
{
public:

    // Construct/Destruct
    ~morphChannel();
    morphChannel();
    morphChannel(const morphChannel & from);

    MorphR3     *mp;

    float mCurvature;

    // Number of points
    int         mNumPoints;

    int iTargetListSelection;

    // Actual morphable points
    std::vector<Point3>     mPoints;
    std::vector<Point3>     mDeltas;
    std::vector<double>     mWeights;
    
    std::vector<TargetCache>    mTargetCache;

        // BitArray to check against for point selection
    BitArray    mSel;
    // INode that we use to update and reload from, if possible
    INode*      mConnection;
    
    // Name for the morph channel
    TSTR        mName;
    //
    int mNumProgressiveTargs;
    float mTargetPercent;

    // Various, non-animatable stuff
    // mActive is TRUE if the channel has data in it in some form
    // mModded is TRUE if the channel has been changed in SOME form,
    //      ie, had its name changed or similar
    BOOL        mActive, mModded, mUseLimit, mUseSel;
    float       mSpinmin,mSpinmax;

    // TRUE if the channel has been marked as bad. It will not be 
    // considered when building the morph results.
    BOOL        mInvalid;

    // Channel enabled/disabled
    BOOL        mActiveOverride;

    void InitTargetCache(const int &targnum, INode *nd){ mTargetCache[targnum].Init( nd); }

    // paramblock for the morph channels values
    IParamBlock* cblock;
    
    // Delete and reset channel
    MorphExport void ResetMe();


    MorphExport void AllocBuffers( int sizeA, int sizeB );

    // Do some rough calculations about how much space this channel
    // takes up
    // This isn't meant to be fast or terribly accurate!
    MorphExport float getMemSize();


    // The rebuildChannel call will recalculate the optimization data
    // and refill the mSel selection array. This will be called each time a
    // targeted node is changed, or any of the 'Update Target' buttons is
    // pressed on the UI
    MorphExport void rebuildChannel();

    // Initialize a channel using a scene node
    MorphExport void buildFromNode( INode *node , BOOL resetTime=TRUE , TimeValue t=0, BOOL picked = FALSE );

    // Transfer data to another channel
    MorphExport void operator=(const morphChannel& from);
    void operator=(const TargetCache& tcache);

    // Load/Save channel to stream
    MorphExport IOResult Save(ISave* isave);
    MorphExport IOResult Load(ILoad* iload);
    void SetUpNewController();
    int  NumProgressiveTargets(void) { return mNumProgressiveTargs; }
    void ResetRefs(MorphR3 *, const int&);
    float GetTargetPercent(const int &which);
    void ReNormalize();
    void CopyTargetPercents(const morphChannel &chan);
};

// Internally-used local object morph cache
class morphCache
{

public:

    BOOL CacheValid;

    Point3*     oPoints;
    double*     oWeights;
    BitArray    sel;

    int     Count;

    morphCache ();
    ~morphCache () { NukeCache(); }

    MorphExport void MakeCache(Object *obj);
    MorphExport void NukeCache();

    MorphExport BOOL AreWeCached();
};

/*===========================================================================*\
 | Modifer class definition
\*===========================================================================*/
class MorphR3 : public Modifier, TimeChangeCallback {
    public:

        float mFileVersion;

        // Access to the interface
        static IObjParam *ip;
        
        // Pointer to the morph channels
        std::vector<morphChannel>   chanBank;

        // Pointer to the morph material bound to this morpher
        M3Mat *morphmaterial;
        
        // Currently selected channel (0-9)
        int                 chanSel;
        
        // Currently viewable channel banks (0-99)
        int                 chanNum;

        // Spinners from main page
        ISpinnerControl     *chanSpins[10];

        // Spinners from global settings page
        ISpinnerControl     *glSpinmin,*glSpinmax;

        // Spinners from the channel params dlg
        ISpinnerControl     *cSpinmin,*cSpinmax,*cCurvature,*cTargetPercent;

        // Global parameter block
        IParamBlock         *pblock;

        // The window handles for the 4 rollout pages
        HWND hwGlobalParams, hwChannelList, hwChannelParams, hwAdvanced, hwLegend;
        static HWND hMaxWnd;

        // For the namer dialog
        ICustEdit           *newname;

        // Morph Cache
        morphCache MC_Local;

        BOOL tccI;
        char trimD[50];

        // 'Save as Current' support
        BOOL recordModifications;
        int recordTarget;


        // Marker support
        Tab<int>            markerIndex;
        NameTab             markerName;
        int                 markerSel;


        // Channel operation flag for dialog use
        int                 cOp;
        int                 srcIdx;

        // storage variable for states between dialog procs
        bool hackUI;


        //Constructor/Destructor
        MorphR3();
        ~MorphR3();


        // TimeChangeCallback
        void TimeChanged(TimeValue t) {
            if(hwChannelList) Update_channelValues();

            Interval valid=FOREVER; int itmp; 
            Interface *Cip = GetCOREInterface();

            if(pblock&&Cip)
            {
                pblock->GetValue(PB_CL_AUTOLOAD, 0, itmp, valid);
                if(itmp==1) NotifyDependents(FOREVER, PART_OBJ, REFMSG_CHANGE);
            }

        }


        // From Animatable
        void DeleteThis() { delete this; }
        void GetClassName(TSTR& s);
        virtual Class_ID ClassID() { return MR3_CLASS_ID;}
        RefTargetHandle Clone(RemapDir& remap = DefaultRemapDir());
        TCHAR *GetObjectName() ;

        SvGraphNodeReference SvTraverseAnimGraph(IGraphObjectManager *gom, Animatable *owner, int id, DWORD flags);
        TSTR SvGetRelTip(IGraphObjectManager *gom, IGraphNode *gNodeTarger, int id, IGraphNode *gNodeMaker);

        IOResult Load(ILoad *iload);
        IOResult Save(ISave *isave);

        //From Modifier
        ChannelMask ChannelsUsed()  { return PART_GEOM|PART_TOPO|SELECT_CHANNEL; }
        ChannelMask ChannelsChanged() { return PART_GEOM; }

        void Bez3D(Point3 &b, const Point3 *p, const float &u);
        void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
        Class_ID InputType() {
            // removed the following
            // it was screwing with modstack evaluation severely!
            //Interface *ip = GetCOREInterface();
            //if(ip&&ip->GetSelNodeCount()>1) return Class_ID(0,0);
            return defObjectClassID;
        }
    
        Interval LocalValidity(TimeValue t);
        void NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext *mc);

        void DeleteChannel(const int &);
        // From BaseObject
        BOOL ChangeTopology() {return FALSE;}
        int GetParamBlockIndex(int id) {return id;}

        //From ReferenceMaker
        int NumRefs();
        RefTargetHandle GetReference(int i);
        void SetReference(int i, RefTargetHandle rtarg);
        
        int NumSubs();
        Animatable* SubAnim(int i);
        TSTR SubAnimName(int i);
        bool CheckMaterialDependency( void );
        bool CheckSubMaterialDependency( void );
        RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, 
           PartID& partID, RefMessage message);
        void TestMorphReferenceDependencies( const RefTargetHandle hTarget);
        
        CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;}
        void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
        void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);

        Interval GetValidity(TimeValue t);
        ParamDimension *GetParameterDim(int pbIndex);
        TSTR GetParameterName(int pbIndex);

        // Handles the scroll bar on the channel list UI
        MorphExport void VScroll(int code, short int cpos );
        // Clamps channel number to valid range
        MorphExport void Clamp_chanNum();

        
        MorphExport void ChannelOp(int src, int targ, int flags);


        MorphExport void Update_globalParams();
        MorphExport void Update_advancedParams();   
        MorphExport void Update_channelParams();


        // evaluate the value increments setting
        MorphExport float GetIncrements();
        // SetScale on the channel list spinners
        MorphExport void Update_SpinnerIncrements();

        // Functions to update the channel list dialog box:
        MorphExport void Update_colorIndicators();
        MorphExport void Update_channelNames();
        MorphExport void Update_channelValues();
        MorphExport void Update_channelLimits();
        MorphExport void Update_channelInfo();
        MorphExport void Update_channelMarkers();
        // Seperated cause this function is pretty expensive
        // Lots done, complete update - calls all functions above
        MorphExport void Update_channelFULL();
        
        // Used to trim fp values to a number of decimal points
        MorphExport float TrimDown(float value, int decimalpts);

        BOOL inRender;

        int RenderBegin(TimeValue t, ULONG flags) { 
            inRender = TRUE;
            return 1;   
            }
        int RenderEnd(TimeValue t) {    
            inRender = FALSE;   
            return 1;   
            }

        int CurrentChannelIndex(void) {return chanNum + chanSel; }
        morphChannel &CurrentChannel(void) { return chanBank[chanNum + chanSel]; }

        float GetCurrentTargetPercent(void); 
        void SetCurrentTargetPercent(const float &fval);
    
        void DeleteTarget(void);
        void Update_TargetListBoxNames(void);
        void SwapTargets(const int way);
        void SwapTargets(const int from, const int to, const bool isundo);

        int GetRefNumber(int chanNum, int targNum) { return (200 + targNum + (chanNum * MAX_TARGS)); }
        void DisplayMemoryUsage(void );
};
#endif

//---------------------------------------------------------------------------
// Found in file: .\maxsdk\samples\objects\morphobj.cpp

class PickTarget;

class MorphObject: public GeomObject {             
        
        RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
           PartID& partID, RefMessage message);
                
    public:
        MorphControl *morphCont;
        int sel;

        static IObjParam *ip;
        static HWND hParams1;
        static HWND hParams2;
        static PickTarget pickCB;
        static int addTargMethod;
        static MorphObject *editOb;
        static BOOL creating;

        MorphObject(BOOL loading=FALSE);
        ~MorphObject();
        void SetupTargetList();
        int AddTargMethod();
        void SetTargMethod(int m);
        void AddNewTarget(INode *node,TimeValue t,int m);

        // From BaseObject
        CreateMouseCallBack* GetCreateMouseCallBack();      
        TCHAR *GetObjectName() ;

        // From Object      
        void InitNodeName(TSTR& s);
        Interval ObjectValidity(TimeValue t);
        int CanConvertToType(Class_ID obtype);
        Object* ConvertToType(TimeValue t, Class_ID obtype);
        BOOL PolygonCount(TimeValue t, int& numFaces, int& numVerts);
        ObjectState Eval(TimeValue time);
        int NumPipeBranches(bool selected);
        Object *GetPipeBranch(int i, bool selected);
        BOOL HasUVW() {return morphCont->HasUVW();}
        void SetGenUVW(BOOL sw) {morphCont->SetGenUVW(sw);}

        // From GeomObject      
        ObjectHandle CreateTriObjRep(TimeValue t);  // for rendering, also for deformation
        int IntersectRay(TimeValue t, Ray& r, float& at) {return 0;}

        // Animatable methods
        Class_ID ClassID() {return Class_ID(MORPHOBJ_CLASS_ID,0);}  
        void GetClassName(TSTR& s);
        void DeleteThis() {delete this;}                
        void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
        void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);
        BOOL AssignController(Animatable *control,int subAnim);

        int NumSubs();
        Animatable* SubAnim(int i);
        TSTR SubAnimName(int i);

        // From ref
        RefTargetHandle Clone(RemapDir& remap);
        int NumRefs() {return 1;}
        RefTargetHandle GetReference(int i) {return morphCont;}
        void SetReference(int i, RefTargetHandle rtarg) {morphCont = (MorphControl*)rtarg;}
    };              

//---------------------------------------------------------------------------
// Found in file: .\maxsdk\samples\objects\materials\rgbmult.cpp

#define RGB_NSUBTEX 2    // number of texture map slots
#define RGB_NCOLS 2
#define IDS_DS_RGBMULT                  11488

static Class_ID rgbmultClassID(RGBMULT_CLASS_ID,0);

//--------------------------------------------------------------
// RGBMult: A Composite texture map
//--------------------------------------------------------------
class RGBMult: public Texmap { 
    public:
    BOOL Param1;
    IParamBlock2 *pblock;   // ref #0
    Texmap* subTex[RGB_NSUBTEX];  // refs 1,2;
    BOOL mapOn[RGB_NSUBTEX];
    Color col[RGB_NCOLS];
    Interval ivalid;
    int alphaFrom;
    BOOL rollScroll;
    BOOL loadingOld;
        RGBMult();
        ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
        void Update(TimeValue t, Interval& valid);
        void Init();
        void Reset();
        Interval Validity(TimeValue t) { Interval v; Update(t,v); return ivalid; }

        void NotifyChanged();

        void SetColor(int i, Color c, TimeValue t);

        // Evaluate the color of map for the context.
        AColor EvalColor(ShadeContext& sc);
        float EvalMono(ShadeContext& sc);
        AColor EvalFunction(ShadeContext& sc, float u, float v, float du, float dv);

        // For Bump mapping, need a perturbation to apply to a normal.
        // Leave it up to the Texmap to determine how to do this.
        Point3 EvalNormalPerturb(ShadeContext& sc);

        // Methods to access texture maps of material
        int NumSubTexmaps() { return RGB_NSUBTEX; }
        Texmap* GetSubTexmap(int i) { return subTex[i]; }
        void SetSubTexmap(int i, Texmap *m);
        TSTR GetSubTexmapSlotName(int i);

        Class_ID ClassID() {    return rgbmultClassID; }
        SClass_ID SuperClassID() { return TEXMAP_CLASS_ID; }
        void GetClassName(TSTR& s) { s= GetString(IDS_DS_RGBMULT); }  
        void DeleteThis() { delete this; }  

        int NumSubs() { return RGB_NSUBTEX+1; }  
        Animatable* SubAnim(int i);
        TSTR SubAnimName(int i);
        int SubNumToRefNum(int subNum) { return subNum; }

        // From ref
        int NumRefs() { return RGB_NSUBTEX+1; }
        RefTargetHandle GetReference(int i);
        void SetReference(int i, RefTargetHandle rtarg);
        int RemapRefOnLoad(int iref); 

        RefTargetHandle Clone(RemapDir &remap = DefaultRemapDir());
        RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
           PartID& partID, RefMessage message );

        // IO
        IOResult Save(ISave *isave);
        IOResult Load(ILoad *iload);

// JBW: direct ParamBlock access is added
        int NumParamBlocks() { return 1; }                  // return number of ParamBlocks in this instance
        IParamBlock2* GetParamBlock(int i) { return pblock; } // return i'th ParamBlock
        IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock

        // From Texmap
        bool IsLocalOutputMeaningful( ShadeContext& sc ) { return true; }

    };
//---------------------------------------------------------------------------
// Found in file: .\maxsdk\samples\materials\partage.cpp

#define PARTAGE_CLASSID Class_ID(0x8d618ea4,0x49bbe8cf)
#define IDS_RB_PARTICLEAGE              11521


#define PAGE_NSUBTEX    3

class PartAgeTex : public Texmap { 
    public:
        static ParamDlg* texoutDlg;
        IParamBlock2 *pblock;       // ref 0        
        TextureOutput *texout;      // ref 1
        Texmap* subTex[PAGE_NSUBTEX];   // ref 2-4


        // Caches
        Interval ivalid;
        CRITICAL_SECTION csect;
        Color col1, col2, col3;
        int usemap1, usemap2, usemap3;
        float p1, p2, p3;       

        PartAgeTex();
        ~PartAgeTex() {DeleteCriticalSection(&csect);}

        ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);      
        void Update(TimeValue t, Interval& valid);
        void Init();
        void Reset();
        Interval Validity(TimeValue t) {Update(t,FOREVER); return ivalid;}      

        // Evaluation
        AColor EvalColor(ShadeContext& sc);
        float EvalMono(ShadeContext& sc);
        AColor EvalFunction(ShadeContext& sc, float u, float v, float du, float dv);        
        Point3 EvalNormalPerturb(ShadeContext& sc);     

        // Methods to access texture maps of material
        int NumSubTexmaps() {return PAGE_NSUBTEX;}
        Texmap* GetSubTexmap(int i) {return subTex[i];}
        void SetSubTexmap(int i, Texmap *m);
        TSTR GetSubTexmapSlotName(int i);

        Class_ID ClassID() {return PARTAGE_CLASSID;}
        SClass_ID SuperClassID() {return TEXMAP_CLASS_ID;}
        void GetClassName(TSTR& s) { s = GetString(IDS_RB_PARTICLEAGE); } // mjm - 2.3.99
        void DeleteThis() {delete this;}    

        int NumSubs() {return 5;}
        Animatable* SubAnim(int i);
        TSTR SubAnimName(int i);
        int SubNumToRefNum(int subNum) {return subNum;}

        int NumRefs() {return 5;}
        RefTargetHandle GetReference(int i);
        void SetReference(int i, RefTargetHandle rtarg);

        IOResult Load(ILoad *iload);
        IOResult Save(ISave *isave);
        RefTargetHandle Clone(RemapDir &remap = DefaultRemapDir());
        RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
           PartID& partID, RefMessage message);

// JBW: direct ParamBlock access is added
        int NumParamBlocks() { return 1; }                  // return number of ParamBlocks in this instance
        IParamBlock2* GetParamBlock(int i) { return pblock; } // return i'th ParamBlock
        IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock
        BOOL SetDlgThing(ParamDlg* dlg);

    bool IsLocalOutputMeaningful( ShadeContext& sc );
    };
//---------------------------------------------------------------------------
// Found in file: .\maxsdk\samples\materials\texmaps.h

//---------------------------------------------------------------------------
//  Texmaps
//---------------------------------------------------------------------------

#define MAPACTIVE(i) ((*maps)[i].IsActive())

class TexmapSlot {
    public:
        float amount;
        Control *amtCtrl;  // ref to controller
        Texmap  *map;       // ref to map
        BOOL    mapOn;
        TSTR    name;

        TexmapSlot();
        RGBA Eval(ShadeContext& sc) { 
            return amount * map->EvalColor(sc); 
            }
        float EvalMono(ShadeContext& sc) { 
            return amount * map->EvalMono(sc); 
            }
        float LerpEvalMono(ShadeContext& sc, float v) {
            if( amount<0.0f ){
                float b = 1.0f + amount;
                if ( b < 0.0f ) b = 0.0f;
                return -amount * (1.0f - map->EvalMono(sc)) + b*v; 
            } else {
                float b = 1.0f - amount;
                if ( b < 0.0f ) b = 0.0f;
                return amount * map->EvalMono(sc) + b*v; 
            }
//          float b = 1.0f - (amount<0.0f ? -amount : amount);
//          if ( b < 0.0f ) b = 0.0f;
//          return amount * map->EvalMono(sc) + b*v; 
        }

// begin - ke/mjm - 03.16.00 - merge reshading code
        float LerpMono( float origVal, float texVal ) // for reShading, no eval
        {
            if( amount<0.0f )
            {
                float b = 1.0f + amount;
                if ( b < 0.0f ) b = 0.0f;
                return -amount * texVal + b * (1.0f - origVal); 
            } else {
                float b = 1.0f - amount;
                if ( b < 0.0f ) b = 0.0f;
                return amount * texVal + b * origVal;
            }
        }
// end - ke/mjm - 03.16.00 - merge reshading code

        Point3 EvalNormalPerturb(ShadeContext &sc) {
            return amount * map->EvalNormalPerturb(sc); 
            }
        BOOL IsActive() { 
            return (map&&mapOn&&(amtCtrl||amount!=0.0f)); 
        }
        void Update(TimeValue t, Interval &ivalid);             
        float GetAmount(TimeValue t);
        BOOL HasMap() { return map != 0 ? TRUE : FALSE; }

};


class Texmaps: public TexmapContainer {
    public:  
        MtlBase *client;
        TexmapSlot txmap[32];
        BOOL loadingOld;

        Texmaps();
        Texmaps(MtlBase *mb);
        void SetClientPtr(MtlBase *mb) { client = mb; }
        TexmapSlot& operator[](int i) { return txmap[i]; }

        Class_ID ClassID();

        void DeleteThis();
        RefTargetHandle Clone(RemapDir &remap = DefaultRemapDir()); 
        RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
           PartID& partID, RefMessage message );

        void RescaleWorldUnits(float f);

        BOOL AssignController(Animatable *control,int subAnim) {
            ReplaceReference(SubNumToRefNum(subAnim),(ReferenceTarget *)control);
            return TRUE;
            }

        int NumSubs();
        Animatable* SubAnim(int i);
        TSTR SubAnimName(int i);
        int SubNumToRefNum(int subNum) {return subNum; }
        BOOL InvisibleProperty() { return TRUE; }  // maps are made visible in scripter by pb_maps paramblock in the material so don't expose them as a subanim
        
//      int NumRefs() { return STD2_NMAX_TEXMAPS*2; }
        int NumRefs();
        RefTargetHandle GetReference(int i);
        void SetReference(int i, RefTargetHandle rtarg);

        // IO
        IOResult Save(ISave *isave);
        IOResult Load(ILoad *iload);

        void SetName( long i, TSTR& nm ){txmap[i].name = nm; } 
        TSTR& GetName( long i ){return txmap[i].name; } 
        SvGraphNodeReference SvTraverseAnimGraph(IGraphObjectManager *gom, Animatable *owner, int id, DWORD flags);
        BOOL HasSomeMaps(){
            for(int i=0; i<STD2_NMAX_TEXMAPS; ++i){
                if( txmap[i].map )
                    return TRUE;
            }
            return FALSE;
        }

    };

//---------------------------------------------------------------------------
// Found in file: .\maxsdk\samples\materials\stdmtl2.h

// StdMtl2 flags values
#define STDMTL_ADD_TRANSP   (1<<0)
#define STDMTL_FALLOFF_OUT  (1<<1)
#define STDMTL_WIRE         (1<<2)
#define STDMTL_2SIDE        (1<<3)
#define STDMTL_SOFTEN       (1<<4)
#define STDMTL_FILT_TRANSP  (1<<5)
#define STDMTL_WIRE_UNITS   (1<<6)
#define STDMTL_LOCK_AD      (1<<8)
#define STDMTL_LOCK_DS      (1<<9)
#define STDMTL_UNUSED1      (1<<10)
#define STDMTL_LOCK_ADTEX   (1<<11)
#define STDMTL_FACEMAP      (1<<12)
#define STDMTL_OLDSPEC      (1<<13)
#define STDMTL_SSAMP_ON     (1<<14)
#define STDMTL_COLOR_SI     (1<<15)
#define STDMTL_FACETED      (1<<16)

#define STDMTL_ROLLUP0_OPEN  (1<<27)    // shader
#define STDMTL_ROLLUP1_OPEN  (1<<28)    // basic
#define STDMTL_ROLLUP2_OPEN  (1<<29)    // extra
#define STDMTL_ROLLUP3_OPEN  (1<<30)    // maps
#define STDMTL_ROLLUP4_OPEN  (1<<26)    // sampling
#define STDMTL_ROLLUP5_OPEN  (1<<25)    // dynamics
#define STDMTL_ROLLUP6_OPEN  (1<<24)    // effects

// only needed if the constant shader is included in shaders
#define  CONSTClassID (STDSHADERS_CLASS_ID+1)

#define STDMTL_ROLLUP_FLAGS (STDMTL_ROLLUP0_OPEN|STDMTL_ROLLUP1_OPEN|STDMTL_ROLLUP2_OPEN|STDMTL_ROLLUP3_OPEN \
                            |STDMTL_ROLLUP4_OPEN|STDMTL_ROLLUP5_OPEN|STDMTL_ROLLUP6_OPEN)

// IDs for all the ParamBlocks and their parameters.  One block UI per rollout.
enum { std2_shader, std2_extended, std2_sampling, std_maps, std2_dynamics, };  // pblock IDs
// std2_shader param IDs
enum 
{ 
    std2_shader_type, std2_wire, std2_two_sided, std2_face_map, std2_faceted,
    std2_shader_by_name,  // virtual param for accessing shader type by name
};
// std2_extended param IDs
enum 
{ 
    std2_opacity_type, std2_opacity, 
    std2_filter_color, std2_ep_filter_map,
    std2_falloff_type, std2_falloff_amnt, 
    std2_ior,
    std2_wire_size, std2_wire_units,
    std2_apply_refl_dimming, std2_dim_lvl,
    std2_refl_lvl,
    std2_translucent_blur, 
    std2_ep_translucent_blur_map,
    std2_translucent_color, 
    std2_ep_translucent_color_map,
};

// std2_sampling param IDs
enum 
{ 
    std2_ssampler, std2_ssampler_qual, std2_ssampler_enable, 
        std2_ssampler_adapt_on, std2_ssampler_adapt_threshold, std2_ssampler_advanced,
        std2_ssampler_subsample_tex_on, std2_ssampler_by_name, 
        std2_ssampler_param0, std2_ssampler_param1,
};
// std_maps param IDs
enum 
{
    std2_map_enables, std2_maps, std2_map_amnts, std2_mp_ad_texlock, 
};
// std2_dynamics param IDs
enum 
{
    std2_bounce, std2_static_friction, std2_sliding_friction,
};


// paramblock2 block and parameter IDs for the standard shaders
// NB these are duplicated in shaders/stdShaders.cpp...
enum { shdr_params, };
// shdr_params param IDs
enum 
{ 
    shdr_ambient, shdr_diffuse, shdr_specular,
    shdr_ad_texlock, shdr_ad_lock, shdr_ds_lock, 
    shdr_use_self_illum_color, shdr_self_illum_amnt, shdr_self_illum_color, 
    shdr_spec_lvl, shdr_glossiness, shdr_soften,
};



#define NUM_REFS        9

// refs
#define OLD_PBLOCK_REF  0       // reference number assignments
#define TEXMAPS_REF     1
#define SHADER_REF      2
#define SHADER_PB_REF   3
#define EXTENDED_PB_REF 4
#define SAMPLING_PB_REF 5
#define MAPS_PB_REF     6
#define DYNMAICS_PB_REF 7
#define SAMPLER_REF     8


// sub anims
#define NUM_SUB_ANIMS   5
//#define OLD_PARAMS_SUB        0
#define TEXMAPS_SUB         0
#define SHADER_SUB          1
#define EXTRA_PB_SUB        2
#define SAMPLING_PB_SUB     3
#define DYNAMICS_PB_SUB     4

// these define the evalType parameter for the private
// EvalReflection & EvalRefraction calls
#define EVAL_CHANNEL    0
#define RAY_QUERY       1

#define IDS_KE_STANDARD2                11534

class RefmsgKillCounter {
private:
    friend class KillRefmsg;
    LONG    counter;

public:
    RefmsgKillCounter() : counter(-1) {}

    bool DistributeRefmsg() { return counter < 0; }
};

class KillRefmsg {
private:
    LONG&   counter;

public:
    KillRefmsg(RefmsgKillCounter& c) : counter(c.counter) { ++counter; }
    ~KillRefmsg() { --counter; }
};

class StdMtl2 : public StdMat2, public IReshading {
    // Animatable parameters
    public:
        // current UI if open
        static ShaderParamDlg* pShaderDlg;
        static IAutoMParamDlg* masterDlg;
        static IAutoMParamDlg* texmapDlg;
        static IAutoMParamDlg* extendedDlg;
        static IAutoMParamDlg* samplingDlg;
        static HWND            curHwmEdit;
        static IMtlParams*     curImp;
        static Tab<ClassDesc*> shaderList;
        static Tab<ClassDesc*> samplerList;

        IParamBlock *old_pblock;    // ref 0, for old version loading
        Texmaps* maps;              // ref 1
        Interval ivalid;
        ReshadeRequirements mReshadeRQ; // mjm - 06.02.00
        ReshadeRequirements mInRQ;      // ca - 12/7/00
        ULONG flags;
        int shaderId;
        Shader *pShader;            // ref 2
        // new PB2 paramblocks, one per rollout
        IParamBlock2 *pb_shader;    // ref 3, 4, ...
        IParamBlock2 *pb_extended;  
        IParamBlock2 *pb_sampling;  
        IParamBlock2 *pb_maps;  
        IParamBlock2 *pb_dynamics;  

        Color filter;
        float opacity;  
        float opfall;
        float wireSize;
        float ioRefract;
        float dimIntens;
        float dimMult;
        BOOL dimReflect;

        Color translucentColor;
        float translucentBlur;

        
        // sampling 
        int samplerId;
        Sampler* pixelSampler;  // ref 8

        // composite of shader/mtl channel types
        int channelTypes[ STD2_NMAX_TEXMAPS ];
        int stdIDToChannel[ N_ID_CHANNELS ];

        // experiment: override filter
//        BOOL    filterOverrideOn;
//        float   filterSz;

        // Kill REFMSG_CHANGE messages. This counter is used to
        // prevent these messages when things really aren't changing.
        // Use the class KillRefmsg
        RefmsgKillCounter   killRefmsg;

        void SetFlag(ULONG f, ULONG val);
        void EnableMap(int i, BOOL onoff);
        BOOL IsMapEnabled(int i) { return (*maps)[i].mapOn; }
        BOOL KeyAtTime(int id,TimeValue t) { return (id == OPACITY_PARAM) ? pb_extended->KeyFrameAtTime(std2_opacity, t) : FALSE; }
        BOOL AmtKeyAtTime(int i, TimeValue t);
        int  GetMapState( int indx ); //returns 0 = no map, 1 = disable, 2 = mapon
        TSTR  GetMapName( int indx ); 
        void SyncADTexLock( BOOL lockOn );

        // from StdMat
        // these set Approximate colors into the plug in shader
        BOOL IsSelfIllumColorOn();
        void SetSelfIllumColorOn( BOOL on );
        void SetSelfIllumColor(Color c, TimeValue t);       
        void SetAmbient(Color c, TimeValue t);      
        void SetDiffuse(Color c, TimeValue t);      
        void SetSpecular(Color c, TimeValue t);
        void SetShininess(float v, TimeValue t);        
        void SetShinStr(float v, TimeValue t);      
        void SetSelfIllum(float v, TimeValue t);    
        void SetSoften(BOOL onoff) { SetFlag(STDMTL_SOFTEN,onoff); }
        
        void SetTexmapAmt(int imap, float amt, TimeValue t);
        void LockAmbDiffTex(BOOL onoff) { SetFlag(STDMTL_LOCK_ADTEX,onoff); }

        void SetWire(BOOL onoff){ pb_shader->SetValue(std2_wire,0, (onoff!=0) ); }//SetFlag(STDMTL_WIRE,onoff); }
        void SetWireSize(float s, TimeValue t);
        void SetWireUnits(BOOL onoff) { pb_extended->SetValue(std2_wire_units,0, (onoff!=0) ); } //SetFlag(STDMTL_WIRE_UNITS,onoff); }
        
        void SetFaceMap(BOOL onoff) { pb_shader->SetValue(std2_face_map,0, (onoff!=0) ); } //SetFlag(STDMTL_FACEMAP,onoff); }
        void SetTwoSided(BOOL onoff) { pb_shader->SetValue(std2_two_sided,0, (onoff!=0) ); } //SetFlag(STDMTL_2SIDE,onoff); }
        void SetFalloffOut(BOOL outOn) { pb_extended->SetValue(std2_falloff_type,0, (outOn!=0) ); } //SetFlag(STDMTL_FALLOFF_OUT,onoff); }
        void SetTransparencyType(int type);

        void SetFilter(Color c, TimeValue t);
        void SetOpacity(float v, TimeValue t);      
        void SetOpacFalloff(float v, TimeValue t);      
        void SetIOR(float v, TimeValue t);
        void SetDimIntens(float v, TimeValue t);
        void SetDimMult(float v, TimeValue t);
        
        // translucency
    void SetTranslucentColor(Color& c, TimeValue t);
        void SetTranslucentBlur(float v, TimeValue t);

        int GetFlag(ULONG f) { return (flags&f)?1:0; }

        // >>>Shaders

        // these 3 internal only
        void SetShaderIndx( long shaderId, BOOL update=TRUE );
        long GetShaderIndx(){ return shaderId; }
        void SetShader( Shader* pNewShader );
        void ShuffleTexMaps( Shader* newShader, Shader* oldShader );
        void ShuffleShaderParams( Shader* newShader, Shader* oldShader );

        Shader* GetShader(){ return pShader; }
        void SwitchShader(Shader* pNewShader, BOOL loadDlg = FALSE);
        void SwitchShader(ClassDesc* pNewCD);
        BOOL SwitchShader(Class_ID shaderId);
        int FindShader( Class_ID& findId, ClassDesc** ppCD=NULL );
        BOOL IsShaderInUI() { return pb_shader && pb_shader->GetMap() && pShader && pShader->GetParamDlg(); }

        static void StdMtl2::LoadShaderList();
        static int StdMtl2::NumShaders();
        static ClassDesc* StdMtl2::GetShaderCD(int i);
        static void StdMtl2::LoadSamplerList();
        static int StdMtl2::NumSamplers();
        static ClassDesc* StdMtl2::GetSamplerCD(int i);

        BOOL IsFaceted(){ return GetFlag(STDMTL_FACETED); }
        void SetFaceted( BOOL on ){ pb_shader->SetValue(std2_faceted,0, (on!=0) ); }

        // These utilitys provide R2.5 shaders, ONLY used for Translators
        // Does not & will not work for plug-in shaders
        void SetShading(int s);
        int GetShading();

        // from Mtl
        Color GetAmbient(int mtlNum=0, BOOL backFace=FALSE);        
        Color GetDiffuse(int mtlNum=0, BOOL backFace=FALSE);        
        Color GetSpecular(int mtlNum=0, BOOL backFace=FALSE);
        float GetShininess(int mtlNum=0, BOOL backFace=FALSE);  
        float GetShinStr(int mtlNum=0, BOOL backFace=FALSE) ;
        float GetXParency(int mtlNum=0, BOOL backFace=FALSE);
        float WireSize(int mtlNum=0, BOOL backFace=FALSE) { return wireSize; }

        // >>>> Self Illumination 
        float GetSelfIllum(int mtlNum, BOOL backFace) ;
        BOOL  GetSelfIllumColorOn(int mtlNum, BOOL backFace);
        Color GetSelfIllumColor(int mtlNum, BOOL backFace);
        
        // >>>> sampling
        void SetSamplingOn( BOOL on )
        {   
            pb_sampling->SetValue(std2_ssampler_enable, 0, on!=0 );
        }   
        BOOL GetSamplingOn()
        {   Interval iv; 
            BOOL on;
            pb_sampling->GetValue(std2_ssampler_enable, 0, on, iv );
            return on;
        }   
        void SetSamplingQuality( float quality )
        {    
            pb_sampling->SetValue(std2_ssampler_qual, 0, quality );
        }   
        float GetSamplingQuality()
        {   Interval iv; 
            float q;
            pb_sampling->GetValue(std2_ssampler_qual, 0, q, iv );
            return q;
        }

        void SwitchSampler(ClassDesc* pNewCD);
        void SwitchSampler(Sampler* pNewSampler);
        BOOL SwitchSampler(Class_ID samplerId);
        int FindSampler( Class_ID findId, ClassDesc** pNewCD=NULL );
        Sampler * GetPixelSampler(int mtlNum=0, BOOL backFace=FALSE){  return pixelSampler; }   

        // these 2 internal only
        void SetSamplerIndx( long indx, BOOL update=TRUE );
        long  GetSamplerIndx(){ return samplerId; }
        void SetPixelSampler( Sampler * sampler );

        // Dynamics properties
        float GetDynamicsProperty(TimeValue t, int mtlNum, int propID);
        void SetDynamicsProperty(TimeValue t, int mtlNum, int propID, float value);

        // from StdMat
        BOOL GetSoften() { return GetFlag(STDMTL_SOFTEN); }
        BOOL GetFaceMap() { return GetFlag(STDMTL_FACEMAP); }
        BOOL GetTwoSided() { return GetFlag(STDMTL_2SIDE); }
        BOOL GetWire() { return GetFlag(STDMTL_WIRE); }
        BOOL GetWireUnits() { return GetFlag(STDMTL_WIRE_UNITS); }
        BOOL GetFalloffOut() { return GetFlag(STDMTL_FALLOFF_OUT); }  // 1: out, 0: in
        BOOL GetAmbDiffTexLock(){ return GetFlag(STDMTL_LOCK_ADTEX); } 
        int GetTransparencyType() {
            return (flags&STDMTL_FILT_TRANSP)?TRANSP_FILTER:
                flags&STDMTL_ADD_TRANSP ? TRANSP_ADDITIVE: TRANSP_SUBTRACTIVE;
            }
        Color GetFilter(TimeValue t);

        // these are stubs till i figure out scripting
        Color GetAmbient(TimeValue t);      
        Color GetDiffuse(TimeValue t);      
        Color GetSpecular(TimeValue t);
        float GetShininess( TimeValue t);       
        float GetShinStr(TimeValue t);  
        float GetSelfIllum(TimeValue t);
        BOOL  GetSelfIllumColorOn();
        Color GetSelfIllumColor(TimeValue t); 

        float GetOpacity( TimeValue t);     
        float GetOpacFalloff(TimeValue t);      
        float GetWireSize(TimeValue t);
        float GetIOR( TimeValue t);
        float GetDimIntens( TimeValue t);
        float GetDimMult( TimeValue t);
        float GetSoftenLevel( TimeValue t);
        BOOL MapEnabled(int i);
        float GetTexmapAmt(int imap, TimeValue t);

        // translucency
    Color GetTranslucentColor(TimeValue t);
        float GetTranslucentBlur(TimeValue t);

        // internal
        float GetOpacity() { return opacity; }      
        float GetOpacFalloff() { return opfall; }       
        float GetTexmapAmt(int imap);
        Color GetFilter();
        float GetIOR() { return ioRefract; }

        StdMtl2(BOOL loading = FALSE);
        ~StdMtl2() {
            DiscardTexHandles();
            }
        BOOL ParamWndProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
        ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
        BOOL SetDlgThing(ParamDlg* dlg);
        void UpdateTexmaps();
        void UpdateMapButtons();
        void UpdateExtendedMapButtons();
        void UpdateMtlDisplay();
        void UpdateLockADTex( BOOL passOn );
        void UpdateExtraParams( ULONG stdParams );
        void UpdateSamplingParams();


        Color TranspColor(ShadeContext& sc, float opac, Color& diff);
        void Shade(ShadeContext& sc);
        float EvalDisplacement(ShadeContext& sc); 
        Interval DisplacementValidity(TimeValue t); 
        void Update(TimeValue t, Interval& validr);
        void Reset();
        void OldVerFix(int loadVer);
        void BumpFix();
        Interval Validity(TimeValue t);
        void NotifyChanged();

        // Requirements
        int BuildMaps(TimeValue t, RenderMapsContext &rmc);
        ULONG LocalRequirements(int subMtlNum);
        ULONG Requirements(int subMtlNum);
        void MappingsRequired(int subMtlNum, BitArray & mapreq, BitArray &bumpreq);

        // Methods to access texture maps of material
        int NumSubTexmaps() { return STD2_NMAX_TEXMAPS; }
        Texmap* GetSubTexmap(int i) { return (*maps)[i].map; }
        int MapSlotType(int i);
        void SetSubTexmap(int i, Texmap *m);
        TSTR GetSubTexmapSlotName(int i);
        int SubTexmapOn(int i) { return  MAPACTIVE(i); } 
        long StdIDToChannel( long id ){ return stdIDToChannel[id]; }

        Class_ID ClassID();
        SClass_ID SuperClassID() { return MATERIAL_CLASS_ID; }
        void GetClassName(TSTR& s) { s = GetString(IDS_KE_STANDARD2); }  

        void DeleteThis();

        int NumSubs() { return NUM_SUB_ANIMS; }  
        Animatable* SubAnim(int i);
        TSTR SubAnimName(int i);
        int SubNumToRefNum(int subNum);

        // JBW: add direct ParamBlock access
        int NumParamBlocks() { return 5; }
        IParamBlock2* GetParamBlock(int i);
        IParamBlock2* GetParamBlockByID(BlockID id);

        // From ref
        int NumRefs() { return NUM_REFS; }
        RefTargetHandle GetReference(int i);
        void SetReference(int i, RefTargetHandle rtarg);

        RefTargetHandle Clone(RemapDir &remap = DefaultRemapDir());
        RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
           PartID& partID, RefMessage message );

        // IO
        IOResult Save(ISave *isave);
        IOResult Load(ILoad *iload);

// begin - ke/mjm - 03.16.00 - merge reshading code
        BOOL SupportsRenderElements(){ return TRUE; }
//      BOOL SupportsReShading(ShadeContext& sc);
        void UpdateReshadeRequirements(RefTargetHandle hTarget, PartID partID); // mjm - 06.02.00
        ReshadeRequirements GetReshadeRequirements() { return mReshadeRQ; } // mjm - 06.02.00
        void PreShade(ShadeContext& sc, IReshadeFragment* pFrag);
        void PostShade(ShadeContext& sc, IReshadeFragment* pFrag, int& nextTexIndex, IllumParams* ip);
// end - ke/mjm - 03.16.00 - merge reshading code

// begin - dds- 04.27.00 - multiple map display support
#define NTEXHANDLES 2
        TexHandle *texHandle[NTEXHANDLES];
        short useSubForTex[NTEXHANDLES];
        short texOpsType[NTEXHANDLES];
        int numTexHandlesUsed;
        Interval texHandleValid;
        void SetTexOps(Material *mtl, int i, int type);
        void DiscardTexHandles();
        BOOL SupportTexDisplay() { return TRUE; }
        BOOL SupportsMultiMapsInViewport() { return TRUE; }
        void ActivateTexDisplay(BOOL onoff);
        void SetupGfxMultiMaps(TimeValue t, Material *mtl, MtlMakerCallback &cb);
// end - dds- 04.27.00 - multiple map display support

        // --- Material evaluation - from Mtl ---
        bool IsOutputConst( ShadeContext& sc, int stdID );
        bool EvalColorStdChannel( ShadeContext& sc, int stdID, Color& outClr );
        bool EvalMonoStdChannel( ShadeContext& sc, int stdID, float& outVal );

        void* GetInterface(ULONG id);

        float GetReflectionDim(float diffIllumIntensity ){
            if (dimReflect)
                return ((1.0f-dimIntens)*diffIllumIntensity*dimMult + dimIntens);
            else 
                return 1.0f;
        }
        Color TranspColor( float opac, Color filt, Color diff );

        float GetEffOpacity(ShadeContext& sc, float opac){
            if ( opac != 1.0f || opfall != 0.0f) {
                if (opfall != 0.0f) {   
                    Point3 N = (flags & STDMTL_FACETED) ? sc.GNormal() : sc.Normal();
                    float d = (float)fabs( DotProd( N, sc.V() ) );
                    if (flags & STDMTL_FALLOFF_OUT) d = 1.0f-d;
                    return opac * (1.0f - opfall * d);
                } else return opac;
            } else return 1.0f;
        }

    private:
        // --- Material evaluation ---
        bool EvalChannel( ShadeContext& sc, int channelID, Color& outClr);
        bool EvalBump( ShadeContext& sc, Color& outClr );
        bool EvalReflection( ShadeContext& sc, Color& outClr );
        bool EvalReflection( ShadeContext& sc, float& outVal );
        bool EvalRefraction( ShadeContext& sc, Color& outClr );
        bool EvalRefraction( ShadeContext& sc, float& outVal );
        bool EvalDisplacement( ShadeContext& sc, float& outVal );
        bool ShouldEvalSubTexmap( ShadeContext& sc, int id );

    };

//---------------------------------------------------------------------------
// Found in file: .\maxsdk\samples\objects\morphcnt.cpp

class MorphTarget {
    public:
        Object *obj;
        TSTR *name;
        Matrix3 tm;
        int refCount;
        MorphTarget(Object *o,TSTR &n,Matrix3 &m) {obj=0;name=new TSTR(n);refCount=1;tm = m;}
        MorphTarget() {obj=NULL;refCount=0;name=NULL;tm=Matrix3(1);}
    };

class MorphTargList : public Tab<MorphTarget> {
    public:
        int AddTarg(Object *obj,TSTR &name,Matrix3 &tm,ReferenceMaker *maker,int offset);
        BOOL DeleteTarg(int i,ReferenceMaker *maker);
        void ForceDeleteTarg(int i,int offset,ReferenceMaker *maker);
        void SetSize(int size);
        ~MorphTargList();
    };



// Key flags
#define KEY_SELECTED    (1<<0) 
#define KEY_FLAGGED     (1<<1)
#define MULTS_VALID     (1<<2)

// Track flags
#define TRACK_INVALID   (1<<0)
#define RANGE_UNLOCKED  (1<<1)


class MorphKey {
    public:         
        DWORD flags;
        TimeValue time; 

        void SetFlag(DWORD mask) {flags|=(mask);}
        void ClearFlag(DWORD mask) {flags &= ~(mask);}
        BOOL TestFlag(DWORD mask) {return(flags&(mask)?1:0);}
    };

static int __cdecl CompareMorphKeys(const MorphKey *k1, const MorphKey *k2)
    {
    if (k1->time<k2->time) return -1;
    if (k1->time>k2->time) return 1;
    return 0;
    }

/////////////////////////////////////////////////////////////////////
//
// A simple morph controller...

template <class KT>
class GenMorphCont : public MorphControl {
    public:
        MorphTargList targs;
        KT keys;
        ObjectState ob;
        Interval obValid;
        int version, flaggedTarg;
        BOOL lockCache;

        GenMorphCont();
        ~GenMorphCont();
        virtual void Update(TimeValue t,TimeValue realT)=0;
        virtual void HoldTrack()=0;
        void FreeCache();
        int FindKey(TimeValue t);
        void GetInterpVal(TimeValue t,int &n0, int &n1, float &u);
        void Invalidate();
        TimeValue ProcessORT(TimeValue t);
        
        // Animatable methods
        SClass_ID SuperClassID() {return CTRL_MORPH_CLASS_ID;}
        int NumKeys() {return keys.Count();}
        TimeValue GetKeyTime(int index) {return keys[index].time;}
        int GetTrackVSpace( int lineHeight ) {return 1;}
        BOOL IsAnimated() {return keys.Count()>=1;}

        void MapKeys(TimeMap *map,DWORD flags );
        void DeleteKeys( DWORD flags );
        void CloneSelectedKeys(BOOL offset);
        void DeleteTime( Interval iv, DWORD flags );
        void ReverseTime( Interval iv, DWORD flags );
        void ScaleTime( Interval iv, float s);
        void InsertTime( TimeValue ins, TimeValue amount );
        void AddNewKey(TimeValue t,DWORD flags);
        int GetSelKeyCoords(TimeValue &t, float &val,DWORD flags);
        void SetSelKeyCoords(TimeValue t, float val,DWORD flags);               
        BOOL CanCopyTrack(Interval iv,DWORD flags) {return 0;} // {return 1;}
        BOOL CanPasteTrack(TrackClipObject *cobj,Interval iv,DWORD flags) {return FALSE;} //{return (cobj && (cobj->ClassID()==ClassID()));}
        TrackClipObject *CopyTrack(Interval iv,DWORD flags);
        void PasteTrack(TrackClipObject *cobj,Interval iv,DWORD flags);
        Interval GetTimeRange(DWORD flags);
        void EditTimeRange(Interval range,DWORD flags);
        int HitTestTrack(TrackHitTab& hits,Rect& rcHit,Rect& rcTrack,float zoom,int scroll,DWORD flags );
        int PaintTrack(ParamDimensionBase *dim,HDC hdc,Rect& rcTrack,Rect& rcPaint,float zoom,int scroll,DWORD flags );
        BOOL GetNextKeyTime(TimeValue t,DWORD flags,TimeValue &nt);
        void SelectKeys(TrackHitTab& sel, DWORD flags);
        void SelectKeyByIndex(int i,BOOL sel);
        BOOL IsKeySelected(int i);
        int NumSelKeys();
        void FlagKey(TrackHitRecord hit);
        int GetFlagKeyIndex();          
        int TrackParamsType() {return TRACKPARAMS_KEY;}
        BOOL SupportTimeOperations() {return TRUE;} // RB 3/26/99: This wasn't implemented (def. returned FALSE)

        // Control methods
        void Copy(Control *from) {}
        void CommitValue(TimeValue t) {}
        void RestoreValue(TimeValue t) {}
        BOOL IsLeaf() {return TRUE;}            
        void SetValue(TimeValue t, void *val, int commit=1, GetSetMethod method=CTRL_ABSOLUTE);
        void MouseCycleCompleted(TimeValue t);
        int GetFlaggedTarget() {return flaggedTarg;}

        // Morph control methods
        int NumMorphTargs() {return targs.Count();}
        Object *GetMorphTarg(int i) {return targs[i].obj;}
        void GetMorphTargName(int i,TSTR &name) {name = *targs[i].name;}
        void SetMorphTargName(int i,TSTR name) {*targs[i].name = name;NotifyDependents(FOREVER,0,REFMSG_NODE_NAMECHANGE);}
        Matrix3 GetMorphTargTM(int i) {return targs[i].tm;}
        BOOL ValidTarget(TimeValue t,Object *obj);
        void DeleteMorphTarg(int i);
        BOOL HasUVW();
        void SetGenUVW(BOOL sw);

        void DeleteThis() {delete this;}

        int NumSubs();
        Animatable* SubAnim(int i);
        TSTR SubAnimName(int i);
        
        // Ref methods
        int NumRefs();
        RefTargetHandle GetReference(int i);
        void SetReference(int i, RefTargetHandle rtarg);
        
        IOResult Save(ISave *isave);
        IOResult Load(ILoad *iload);

        RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
           PartID& partID, RefMessage message);

        void MergeAdditionalChannels(Object *from, int branchID);
    };


// Current version
#define MORPHCONT_VERSION       1


//------------------------------------------------------------------------
//
//  Cubic Spline Morph Controller
//

#define CUBICMORPHCONT_CLASS_ID 0x09923023
#define IDS_RB_CUBICMORPHCONTROL_CLASS  30537

class CubicMorphKey : public MorphKey {
    public:         
        int targ;
        float tens, cont, bias;
        float k, l, m, n;
        CubicMorphKey(TimeValue t,int targ) {
            time  = t;
            flags = 0;
            this->targ = targ;
            tens = cont = 0.0f;
            bias = k = l = m = n = 0.0f;
            }
        CubicMorphKey() {
            flags = 0;
            time  = 0;
            targ  = 0;
            tens = cont = 0.0f;
            bias = k = l = m = n = 0.0f;
            }
        
        void Invalidate();
        void ComputeMults();
    };

class CubicMorphKeyTab : public Tab<CubicMorphKey> {
    public:
        DWORD flags;
        Interval range;

        CubicMorphKeyTab() {
            flags = 0;
            }

        void SetFlag(DWORD mask) {flags|=(mask);}
        void ClearFlag(DWORD mask) {flags &= ~(mask);}
        BOOL TestFlag(DWORD mask) {return(flags&(mask)?1:0);}

        void AddNewKey(TimeValue t,int targ,BOOL sel=FALSE) {
            CubicMorphKey key(t,targ);
            if (sel) key.SetFlag(KEY_SELECTED);
            Append(1,&key,5);
            Invalidate();
            }
        
        void Clone(int i) {
            CubicMorphKey key = (*this)[i];
            Append(1,&key,5);
            }
        
        void Invalidate() {
            SetFlag(TRACK_INVALID);
            }

        void CheckForDups() {
            // Check for keys that landed on top of each other
            for (int i=0; i<Count()-1; i++) {
                if ((*this)[i].time==(*this)[i+1].time) {
                    if ((*this)[i].TestFlag(KEY_SELECTED)) {
                        Delete(i+1,1);
                    } else {
                        Delete(i,1);                        
                        }
                    i--;
                    }
                }                       
            }

        void Update();
        IOResult Save(ISave *isave);
        IOResult Load(ILoad *iload);
    };


class CubicMorphCont : public GenMorphCont<CubicMorphKeyTab> {
    public:                         
        void Update(TimeValue t,TimeValue realT);
        void HoldTrack();
        void CalcFirstCoef(float *v,float *c);
        void CalcLastCoef(float *v,float *c);
        void CalcMiddleCoef(float *v,int *knum,float *c);

        // Animatable methods
        Class_ID ClassID() {return Class_ID(CUBICMORPHCONT_CLASS_ID,0);}  
        void GetClassName(TSTR& s) {s =GetString(IDS_RB_CUBICMORPHCONTROL_CLASS);}
        void EditTrackParams(TimeValue t,ParamDimensionBase *dim,TCHAR *pname,HWND hParent,IObjParam *ip,DWORD flags);

        // Control methods
        void GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method);
        void DeleteMorphTarg(int i);

        RefTargetHandle Clone(RemapDir& remap);
        void Copy(Control *from);
    };

//------------------------------------------------------------------------
//
//  Barycentric Morph Controller
//

#define BARYMORPHCONT_CLASS_ID 0x09923022
#define IDS_RB_BARYMORPHCONTROL_CLASS   31338

class TargetWeight {
    public:
        int targ;
        float percent;
        TargetWeight() {targ=0;percent=0.0f;}
        TargetWeight(int t,float p) {targ=t;percent=p;}
    };

class BaryMorphKey : public MorphKey {
    public:                 
        float tens, cont, bias;
        float k, l, m, n;
        Tab<TargetWeight> *wts;

        BaryMorphKey(TimeValue t,int targ) {
            time  = t;
            flags = 0;          
            tens = cont = 0.0f;         
            bias = k = l = m = n = 0.0f;
            wts = new Tab<TargetWeight>;
            TargetWeight wt(targ,1.0f);
            WTS().Append(1,&wt);
            }
        BaryMorphKey() {
            flags = 0;
            time  = 0;          
            tens = cont = 0.0f;
            bias = k = l = m = n = 0.0f;
            wts = new Tab<TargetWeight>;
            }
        
        BaryMorphKey& operator=(BaryMorphKey &src) {
            MorphKey::operator=(src);
            tens = src.tens; cont = src.cont; bias = src.bias;
            k = src.k; l = src.l; m = src.m; n = src.n;
            wts = new Tab<TargetWeight>;
            *wts = *src.wts;
            return *this;
            }
            
        Tab<TargetWeight> &WTS() {return *wts;}
        void Invalidate();
        void ComputeMults();
        void TargetDeleted(int t);
        float GetWeight(int t);
        void SetWeight(int t,float w);
    };

class BaryMorphKeyTab : public Tab<BaryMorphKey> {
    public:
        DWORD flags;
        Interval range;

        BaryMorphKeyTab() {
            flags = 0;
            }

        void SetFlag(DWORD mask) {flags|=(mask);}
        void ClearFlag(DWORD mask) {flags &= ~(mask);}
        BOOL TestFlag(DWORD mask) {return(flags&(mask)?1:0);}

        void AddNewKey(TimeValue t,int targ,BOOL sel=FALSE) {
            BaryMorphKey key(t,targ);
            if (sel) key.SetFlag(KEY_SELECTED);
            Append(1,&key,5);
            Invalidate();
            }
        
        void Clone(int i) {
            BaryMorphKey key;
            key = (*this)[i];
            Append(1,&key,5);
            }
        
        void Invalidate() {
            SetFlag(TRACK_INVALID);
            }

        void CheckForDups() {
            // Check for keys that landed on top of each other
            for (int i=0; i<Count()-1; i++) {
                if ((*this)[i].time==(*this)[i+1].time) {
                    if ((*this)[i].TestFlag(KEY_SELECTED)) {
                        delete (*this)[i+1].wts;
                        Delete(i+1,1);
                    } else {
                        delete (*this)[i].wts;
                        Delete(i,1);                        
                        }
                    i--;
                    }
                }                       
            }

        void Update();
        IOResult Save(ISave *isave);
        IOResult Load(ILoad *iload);

        BaryMorphKeyTab& operator=(BaryMorphKeyTab &src) {
            Tab<BaryMorphKey>::operator=(src);
            for (int i=0; i<Count(); i++) {
                (*this)[i].wts = new Tab<TargetWeight>(*(*this)[i].wts);
                }
            range = src.range;
            flags = src.flags;
            return *this;
            }
    };


class BaryMorphCont : public GenMorphCont<BaryMorphKeyTab> {
    public:                         
        void Update(TimeValue t,TimeValue realT);
        void HoldTrack();
        void CalcFirstCoef(float *v,float *c);
        void CalcLastCoef(float *v,float *c);
        void CalcMiddleCoef(float *v,int *knum,float *c);

        // Animatable methods
        Class_ID ClassID() {return Class_ID(BARYMORPHCONT_CLASS_ID,0);}  
        void GetClassName(TSTR& s) {s =GetString(IDS_RB_BARYMORPHCONTROL_CLASS);}
        void EditTrackParams(TimeValue t,ParamDimensionBase *dim,TCHAR *pname,HWND hParent,IObjParam *ip,DWORD flags);
        void AddNewKey(TimeValue t,DWORD flags);

        // Control methods
        void GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method);
        void DeleteMorphTarg(int i);

        RefTargetHandle Clone(RemapDir& remap);
        void Copy(Control *from);
        
    };


//---------------------------------------------------------------------------
// Found in file: .\maxsdk\samples\controllers\lodctrl.cpp

#define IDS_RB_LODCONTROL               30746
#define IDS_RB_LODUTILITU               30747

#define LOD_CONTROL_CLASS_ID    Class_ID(0xbbe961a8,0xa0ee7b7f)
#define LOD_CONTROL_CNAME       GetString(IDS_RB_LODCONTROL)

class LODCtrl : public StdControl {
    public:
        float min, max, bmin, bmax;
        WORD grpID;
        int order;
        BOOL viewport, highest;

        LODCtrl();

        // Animatable methods       
        void DeleteThis() {delete this;}        
        int IsKeyable() {return 0;}     
        Class_ID ClassID() {return LOD_CONTROL_CLASS_ID;} 
        SClass_ID SuperClassID() {return CTRL_FLOAT_CLASS_ID;}
        void GetClassName(TSTR& s) {s = LOD_CONTROL_CNAME;}
        BOOL CanCopyAnim() {return FALSE;}
        BOOL CanMakeUnique() {return FALSE;}                

        // Reference methods
        RefResult NotifyRefChanged(Interval, RefTargetHandle, PartID&, RefMessage) {return REF_SUCCEED;}
        IOResult Save(ISave *isave);
        IOResult Load(ILoad *iload);
        RefTargetHandle Clone(RemapDir &remap = DefaultRemapDir());

        // Control methods              
        void Copy(Control *from) {}
        BOOL IsLeaf() {return TRUE;}
        void CommitValue(TimeValue t) {}
        void RestoreValue(TimeValue t) {}
        BOOL IsReplaceable() {return FALSE;}
        BOOL CanInstanceController() {return FALSE;}

        // StdControl methods
        void GetValueLocalTime(TimeValue t, void *val, Interval &valid, GetSetMethod method=CTRL_ABSOLUTE);         
        void SetValueLocalTime(TimeValue t, void *val, int commit, GetSetMethod method) {}
        void Extrapolate(Interval range,TimeValue t,void *val,Interval &valid,int type)
            {*((float*)val) = 1.0f;}
        void *CreateTempValue() {return new float;}
        void DeleteTempValue(void *val) {delete (float*)val;}
        void ApplyValue(void *val, void *delta) {*((float*)val) += *((float*)delta);}
        void MultiplyValue(void *val, float m) {*((float*)val) *= m;}

        float EvalVisibility(TimeValue t,View &view,Box3 pbox,Interval &valid);
        BOOL VisibleInViewports();
    };

class NullView: public View {
    public:
        Point2 ViewToScreen(Point3 p) { return Point2(p.x,p.y); }
        NullView() 
        { 
            worldToView.IdentityMatrix(); 
            screenW=640.0f; 
            screenH = 480.0f; 
        }
    };

//---------------------------------------------------------------------------
struct PCSavePt;
struct SavePt;
struct VelDir2;
struct VelDir;


typedef Tab<Mtl *>Mtllst;

typedef struct{
  Point3 spos1,svel1;
  Point3 spos2,svel2;
  int min,min2;
  TimeValue mintime;
  int collide;
} mindata;
typedef struct{
  int lastmin,lastcollide;
}oldipc;
typedef struct
{ Point3 vel;
  int K;
  TimeValue oneframe;
  BOOL inaxis;
}InDirInfo;
typedef struct{
 TimeValue tl;
 int gennum;
} TimeAndGen;
typedef struct{
  Tab<TimeAndGen> tl;
} TimeLst;
typedef Tab<Mtl *>Mtllst;
typedef struct
{ Point3 pts[8];
  Point3 Suboffset;}
ptlst;
typedef struct
{  Tab<ptlst> bpts;
   int numboxes;
} boxlst;
typedef struct
{ float dirchaos,spchaos,scchaos;
  int spsign,scsign,invel,spconst,scconst,axisentered;
  Point3 Axis;
  float axisvar;
} SpawnVars;
typedef struct{
 float val;
 int num;
} limdata;
typedef struct{
 int p1,p2;
} pairs;
typedef struct{
 Box3 bbox;
 int refnum;
}boxplus;
typedef struct{
  TimeValue stuckt;
  Tab<pairs> ppts;
} stuck;
const int NLEN=255;
typedef TCHAR AName[NLEN];

#include "RandGenerator.h"

class RandGeneratorParticles : public RandGenerator {
    public:
    // override for common rand functions for particle systems
        static const float IntMax;
        static const float IntMax1;
        static const float HalfIntMax;

        int RNDSign() { return RandSign(); }
        float RND01() { return Rand01(); }
        float RND11() { return Rand11(); }
        float RND55() { return Rand55(); }
        int RND0x(int maxnum) { return Rand0X(maxnum); }

        Point3 CalcSpread(float divangle,Point3 oldnorm);
        void VectorVar(Point3 *vel,float R,float MaxAngle);
        Point3 DoSpawnVars(SpawnVars spvars,Point3 pv,Point3 holdv,float *radius,Point3 *sW);
    };

static Class_ID PCLOUD_CLASS_ID(0x1c0f3d2f, 0x30310af9);

#define PB_PCLD_CREATEIN            0
#define PB_PCLD_SPEED           1
#define PB_PCLD_SPEEDVAR            2
#define PB_PCLD_SPEEDDIR            3
#define PB_PCLD_DIRX                4
#define PB_PCLD_DIRY                5
#define PB_PCLD_DIRZ                6
#define PB_PCLD_DIRVAR          7

#define PB_PCLD_BIRTHMETHOD     8
#define PB_PCLD_PBIRTHRATE      9
#define PB_PCLD_PTOTALNUMBER        10
#define PB_PCLD_EMITSTART       11
#define PB_PCLD_EMITSTOP            12
#define PB_PCLD_DISPUNTIL       13
#define PB_PCLD_LIFE                14
#define PB_PCLD_LIFEVAR         15
#define PB_PCLD_CUSTOMMTL       16
#define PB_PCLD_SIZE                17
#define PB_PCLD_SIZEVAR         18
#define PB_PCLD_GROWTIME            19
#define PB_PCLD_FADETIME            20
#define PB_PCLD_RNDSEED         21
#define PB_PCLD_EMITRWID            22
#define PB_PCLD_EMITRHEIGHT     23
#define PB_PCLD_EMITRDEPTH      24

#define PB_PCLD_EMITRHID            25

#define PB_PCLD_PARTICLECLASS   26
#define PB_PCLD_PARTICLETYPE        27
#define PB_PCLD_METATENSION     28
#define PB_PCLD_METATENSIONVAR  29
#define PB_PCLD_METACOURSE      30
#define PB_PCLD_METAAUTOCOARSE  31
#define PB_PCLD_USESUBTREE      32
#define PB_PCLD_ANIMATIONOFFSET  33
#define PB_PCLD_OFFSETAMOUNT     34
#define PB_PCLD_VIEWPORTSHOWS   35
#define PB_PCLD_DISPLAYPORTION  36
#define PB_PCLD_MAPPINGTYPE     37
#define PB_PCLD_MAPPINGTIME     38
#define PB_PCLD_MAPPINGDIST     39
                        
#define PB_PCLD_SPINTIME            40
#define PB_PCLD_SPINTIMEVAR     41
#define PB_PCLD_SPINPHASE       42
#define PB_PCLD_SPINPHASEVAR        43
#define PB_PCLD_SPINAXISTYPE        44
#define PB_PCLD_SPINAXISX       45
#define PB_PCLD_SPINAXISY       46
#define PB_PCLD_SPINAXISZ       47
#define PB_PCLD_SPINAXISVAR     48

#define PB_PCLD_EMITVINFL       49
#define PB_PCLD_EMITVMULT       50
#define PB_PCLD_EMITVMULTVAR        51

#define PB_PCLD_BUBLAMP         52
#define PB_PCLD_BUBLAMPVAR      53
#define PB_PCLD_BUBLPER         54
#define PB_PCLD_BUBLPERVAR      55
#define PB_PCLD_BUBLPHAS            56
#define PB_PCLD_BUBLPHASVAR     57

#define PB_PCLD_STRETCH         58

#define PB_PCLD_SPAWNTYPE       59
#define PB_PCLD_SPAWNGENS       60
#define PB_PCLD_SPAWNCOUNT      61
#define PB_PCLD_SPAWNDIRCHAOS   62
#define PB_PCLD_SPAWNSPEEDCHAOS 63
#define PB_PCLD_SPAWNSPEEDSIGN  64
#define PB_PCLD_SPAWNINHERITV   65
#define PB_PCLD_SPAWNSCALECHAOS 66
#define PB_PCLD_SPAWNSCALESIGN  67
#define PB_PCLD_SPAWNLIFEVLUE   68
#define PB_PCLD_SPAWNSPEEDFIXED 69
#define PB_PCLD_SPAWNSCALEFIXED 70
#define PB_PCLD_METACOURSEV     71
#define PB_PCLD_SPAWNPERCENT        72
#define PB_PCLD_SPAWNMULTVAR        73
#define PB_PCLD_PCNOTDRAFT      74
#define PB_PCLD_SPAWNDIEAFTER   75
#define PB_PCLD_SPAWNDIEAFTERVAR    76

#define PB_PCLD_PCIPCOLLIDE_ON      77
#define PB_PCLD_PCIPCOLLIDE_STEPS   78
#define PB_PCLD_PCIPCOLLIDE_BOUNCE  79
#define PB_PCLD_PCIPCOLLIDE_BOUNCEVAR 80


class PCPickOperand;
class PCPickNorm;
class PCloudParticle;
class PCloudParticleDraw : public CustomParticleDisplay {
    public:
        BOOL firstpart;
        PCloudParticle *obj;
        int disptype,ptype,bb,anifr,anioff;
        boxlst *bboxpt;
        TimeValue t;
        InDirInfo indir;

        PCloudParticleDraw() {obj=NULL;}
        BOOL DrawParticle(GraphicsWindow *gw,ParticleSys &parts,int i);
    };
#define PCLD_BASER 4
class PCloudParticle : public SimpleParticle, IParamArray, RandGeneratorParticles {
    public:
        PCloudParticleDraw thePCloudDraw;
        PCloudParticle();
        ~PCloudParticle();
        static IParamMap *pmapParam;
        static IParamMap *pmapPGen;
        static IParamMap *pmapPType;
        static IParamMap *pmapPSpin;
        static IParamMap *pmapEmitV;
        static IParamMap *pmapBubl;
        static IParamMap *pmapSpawn;
        static int createmeth;
        static HWND hbubl;
        int stepSize,size,maincount,fragflags;
        static int custsettings;
        Mesh *cmesh,*dispmesh;
        Box3 *cmbb;
        TimeValue dispt;
        INode *custnode,*distnode,*cnode,*mbase;
        TSTR custname,distname,normname;
        DWORD flags;
        ULONG dflags;
        BOOL cancelled,wasmulti;
        static BOOL creating;
        static PCPickOperand pickCB;
        static PCPickNorm pickNorm;
        Mtl *origmtl;

        Point3 boxcenter;
        int CustMtls;
        Tab<int> nmtls;
        TimeLst times;
        void GetTimes(TimeLst &times,TimeValue t,int anifr,int ltype);
        void TreeDoGroup(INode *node,Matrix3 tspace,Mesh *cmesh,Box3 *cmbb,int *numV,int *numF,int *tvnum,int *ismapped,TimeValue t,int subtree,int custmtl);
        void CheckTree(INode *node,Matrix3 tspace,Mesh *cmesh,Box3 *cmbb,int *numV,int *numF,int *tvnum,int *ismapped,TimeValue t,int subtree,int custmtl);
        void GetMesh(TimeValue t,int subtree,int custmtl);
        void GetNextBB(INode *node,int subtree,int *count,int *tabmax,Point3 boxcenter,TimeValue t,int tcount,INode *onode);
        void DoGroupBB(INode *node,int subtree,int *count,int *tabmax,Point3 boxcenter,TimeValue t,int tcount,INode *onode);
        void GetallBB(INode *custnode,int subtree,TimeValue t);

        void SetUpList();
        void AddToList(INode *newnode,int i,BOOL add);
        void DeleteFromList(int nnum,BOOL all);
        Tab<INode*> nlist;
        Tab<int> llist;
        int deftime;
        int NumRefs() {return PCLD_BASER+nlist.Count();}
        RefTargetHandle GetReference(int i);
        void SetReference(int i, RefTargetHandle rtarg);        
        RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, 
           PartID& partID, RefMessage message);
        void SetUpLifeList();
        void AddToLifeList(int newlife);
        void DeleteFromLifeList(int nnum);
        void ShowName(int dist);
        static AName *NameLst;
        static HWND hexts,hParams2,hptype,hgen,hparam,hrot,spawn;
        static ICustEdit *custCtrlEdit;

        void GetWorldBoundBox(TimeValue t, INode *inode, ViewExp* vpt, Box3& box);
        int CountLive();
        int rcounter,vcounter;
        oldipc lc;
        static IObjParam *ip;
        // Animatable methods       
        void DeleteThis() {delete this;}
        void MapKeys(TimeMap *map,DWORD flags);
        int RenderBegin(TimeValue t, ULONG flags);      
        int RenderEnd(TimeValue t);

        // From SimpleParticle
        Interval GetValidity(TimeValue t);      
        BOOL EmitterVisible();      
        MarkerType GetMarkerType(); 
        PCSavePt *sdata;
//      unsigned int rseed; // don't need it anymore because of RandGenerator

        // From BaseObject
        int IsInstanceDependent() {return 1;}
        void GetFilename(TCHAR *filename);
        void SetupTargetList();
        int SaveSettings(int overwrite,TCHAR *newname);
        int GetSettings(int setnum,TCHAR *newname);
        int RemSettings(int setnum,TCHAR *newname);
        IOResult Load(ILoad *iload);
        IOResult Save(ISave *isave);
        void AssignMtl(INode *node,INode *topnode,int subtree,TimeValue t);
        void DoGroupMtls(INode *node,int subtree,int *numsubs,int *numtabs,int *tabmax,TimeValue t);
        void RetrieveMtls(INode *node,int subtree,int *numsubs,int *numtabs,int *tabmax,TimeValue t);
        void GetSubs(INode *node,INode *topnode,int subtree,TimeValue t);
        void CntDoGroupMtls(INode *node,int subtree,int *numsubs,int *numtabs,int *tabmax,TimeValue t);
        void CntRetrieveMtls(INode *node,int subtree,int *numsubs,int *numtabs,int *tabmax,TimeValue t);
        BOOL backpatch;
        Mtllst mttab;
        TimeValue ParticleLife(TimeValue t, int i);
        Point3 ParticlePosition(TimeValue t,int i);
        Point3 ParticleVelocity(TimeValue t,int i);     
        float ParticleSize(TimeValue t,int i);
        int ParticleCenter(TimeValue t,int i);
        void DoSpawn(int j,int spmult,SpawnVars spvars,TimeValue lvar,BOOL emits);

        CreateMouseCallBack* GetCreateMouseCallBack();

        TCHAR *GetObjectName();
        void BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev);     
        void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);       
        void BuildEmitter(TimeValue t, Mesh& amesh);
        void BirthParticle(INode *node,TimeValue bt,int num,VelDir *ptvel,Mesh* amesh,Point3* fnorms,Matrix3 disttm);
        BOOL ComputeParticleStart(TimeValue t0,int c);
        // From GeomObject
        Mesh* GetRenderMesh(TimeValue t, INode *inode, View& view, BOOL& needDelete);
        // Animatable methods       
        Class_ID ClassID() {return PCLOUD_CLASS_ID;} 
        // From ref
        RefTargetHandle Clone(RemapDir& remap = DefaultRemapDir());     

        // From Simple Particle
        void UpdateParticles(TimeValue t,INode *node);
        ParamDimension *GetParameterDim(int pbIndex);
        TSTR GetParameterName(int pbIndex);
        void InvalidateUI();
        int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
        int HitTest(TimeValue t, INode *inode, int type, int crossing, int flags, 
        IPoint2 *p, ViewExp *vpt);
        void MovePart(int j,TimeValue dt,BOOL fullframe,int tpf);
        void ResetSystem(TimeValue t,BOOL full=TRUE);
    };
//---------------------------------------------------------------------------
typedef struct 
{
    float Vsz,Ts,Ts0,LamTs,A,LamA,To;
    float M,Dis,Fo,Mltvar,pvar;
    int themtl,gennum,SpVar;
    TimeValue L,showframe,DL,persist;
    Point3  V,W,RV;
}   CSavePt;

static Class_ID SUPRSPRAY_CLASS_ID(0x74f811e3, 0x21fb7b57);
static Class_ID BLIZZARD_CLASS_ID(0x5835054d, 0x564b40ed);

#define PBBLIZ_OFFAXIS              0
#define PBBLIZ_AXISSPREAD           1
#define PBBLIZ_OFFPLANE             2
#define PBBLIZ_PLANESPREAD          3
#define PBBLIZ_SPEED                4
#define PBBLIZ_SPEEDVAR             5

#define PBBLIZ_BIRTHMETHOD          6
#define PBBLIZ_PBIRTHRATE           7
#define PBBLIZ_PTOTALNUMBER         8
#define PBBLIZ_EMITSTART            9
#define PBBLIZ_EMITSTOP             10
#define PBBLIZ_DISPUNTIL            11
#define PBBLIZ_LIFE                 12
#define PBBLIZ_LIFEVAR              13
#define PBBLIZ_SUBFRAMEMOVE         14
#define PBBLIZ_SUBFRAMETIME         15
#define PBBLIZ_SIZE                 16
#define PBBLIZ_SIZEVAR              17
#define PBBLIZ_GROWTIME             18
#define PBBLIZ_FADETIME             19
#define PBBLIZ_RNDSEED              20
#define PBBLIZ_EMITRWID             21
#define PBBLIZ_EMITRHID             22

#define PBBLIZ_PARTICLECLASS        23
#define PBBLIZ_PARTICLETYPE         24
#define PBBLIZ_METATENSION          25
#define PBBLIZ_METATENSIONVAR       26
#define PBBLIZ_METACOURSE           27
#define PBBLIZ_METAAUTOCOARSE       28
#define PBBLIZ_USESUBTREE           29
#define PBBLIZ_ANIMATIONOFFSET      30
#define PBBLIZ_OFFSETAMOUNT         31
#define PBBLIZ_VIEWPORTSHOWS        32
#define PBBLIZ_DISPLAYPORTION       33
#define PBBLIZ_MAPPINGTYPE          34
#define PBBLIZ_MAPPINGTIME          35
#define PBBLIZ_MAPPINGDIST          36
                        
#define PBBLIZ_SPINTIME             37
#define PBBLIZ_SPINTIMEVAR          38
#define PBBLIZ_SPINPHASE            39
#define PBBLIZ_SPINPHASEVAR         40
#define PBBLIZ_SPINAXISTYPE         41
#define PBBLIZ_SPINAXISX            42
#define PBBLIZ_SPINAXISY            43
#define PBBLIZ_SPINAXISZ            44
#define PBBLIZ_SPINAXISVAR          45

#define PBBLIZ_EMITVINFL            46
#define PBBLIZ_EMITVMULT            47
#define PBBLIZ_EMITVMULTVAR         48

#define PBBLIZ_SPAWNTYPE            49
#define PBBLIZ_SPAWNGENS            50
#define PBBLIZ_SPAWNCOUNT           51
#define PBBLIZ_SPAWNDIRCHAOS        52
#define PBBLIZ_SPAWNSPEEDCHAOS      53
#define PBBLIZ_SPAWNSPEEDSIGN       54
#define PBBLIZ_SPAWNINHERITV        55
#define PBBLIZ_SPAWNSCALECHAOS      56
#define PBBLIZ_SPAWNSCALESIGN       57
#define PBBLIZ_SPAWNLIFEVLUE        58
#define PBBLIZ_SPAWNSPEEDFIXED      59
#define PBBLIZ_SPAWNSCALEFIXED      60  

#define PBBLIZ_BUBLAMP              61
#define PBBLIZ_BUBLAMPVAR           62
#define PBBLIZ_BUBLPER              63
#define PBBLIZ_BUBLPERVAR           64
#define PBBLIZ_BUBLPHAS             65
#define PBBLIZ_BUBLPHASVAR          66

#define PBBLIZ_STRETCH              67
#define PBBLIZ_CUSTOMMTL            68
#define PBBLIZ_METACOURSEV          69
#define PBBLIZ_SUBFRAMEROT          70
#define PBBLIZ_SPAWNPERCENT         71
#define PBBLIZ_SPAWNMULTVAR         72
#define PBBLIZ_SSNOTDRAFT           73
#define PBBLIZ_SSSPAWNDIEAFTER      74
#define PBBLIZ_SSSPAWNDIEAFTERVAR   75

#define PBBLIZ_IPCOLLIDE_ON         76
#define PBBLIZ_IPCOLLIDE_STEPS      77
#define PBBLIZ_IPCOLLIDE_BOUNCE     78
#define PBBLIZ_IPCOLLIDE_BOUNCEVAR  79

#define PBBLIZ_TUMBLE                   0
#define PBBLIZ_TUMBLERATE               1
#define PBBLIZ_EMITMAP                  2
#define PBBLIZ_EMITRLENGTH              3
#define PBBLIZ_CUSTOMMTL2               61
#define PBBLIZ_METACOURSEVB             62
#define PBBLIZ_SUBFRAMEROT2             63
#define PBBLIZ_SPAWNPERCENT2            64
#define PBBLIZ_SPAWNMULTVAR2            65
#define PBBLIZ_BLNOTDRAFT               66
#define PBBLIZ_BLSPAWNDIEAFTER          67
#define PBBLIZ_BLSPAWNDIEAFTERVAR       68
#define PBBLIZ_BLIPCOLLIDE_ON           69
#define PBBLIZ_BLIPCOLLIDE_STEPS        70
#define PBBLIZ_BLIPCOLLIDE_BOUNCE       71
#define PBBLIZ_BLIPCOLLIDE_BOUNCEVAR    72

class CPickOperand;
class CommonParticle;
class CommonParticleDraw : public CustomParticleDisplay {
    public:
        float tumble,scale;
        BOOL firstpart;
        CommonParticle *obj;
        int disptype,ptype,bb,anifr,anioff;
        boxlst *bboxpt;
        TimeValue t;
        InDirInfo indir;

        CommonParticleDraw() {obj=NULL;}
        BOOL DrawParticle(GraphicsWindow *gw,ParticleSys &parts,int i);
    };
#define COMMON_BASER 2
class CommonParticle : public SimpleParticle, IParamArray {
    public:
        CommonParticleDraw theSuprSprayDraw;
        static IParamMap *pmapParam;
        static IParamMap *pmapPGen;
        static IParamMap *pmapPType;
        static IParamMap *pmapPSpin;
        static IParamMap *pmapEmitV;
        static IParamMap *pmapSpawn;
        int stepSize,size;
        static int custsettings;
        ULONG dflags;
        Mesh *cmesh,*dispmesh;
        INode *custnode,*cnode;
        TSTR custname;
        DWORD flags;
        BOOL cancelled,wasmulti;
        static BOOL creating;
        static CPickOperand pickCB;
        Mtl *origmtl;

        Point3 boxcenter;
        int CustMtls;
        TimeLst times;
        void GetTimes(TimeLst &times,TimeValue t,int anifr,int ltype);
        void TreeDoGroup(INode *node,Matrix3 tspace,Mesh *cmesh,int *numV,int *numF,int *tvnum,int *ismapped,TimeValue t,int subtree,int custmtl);
        void CheckTree(INode *node,Matrix3 tspace,Mesh *cmesh,int *numV,int *numF,int *tvnum,int *ismapped,TimeValue t,int subtree,int custmtl);
        void GetMesh(TimeValue t,int subtree,int custmtl);
        void GetNextBB(INode *node,int subtree,int *count,int *tabmax,Point3 boxcenter,TimeValue t,int tcount,INode *onode);
        void DoGroupBB(INode *node,int subtree,int *count,int *tabmax,Point3 boxcenter,TimeValue t,int tcount,INode *onode);
        void GetallBB(INode *custnode,int subtree,TimeValue t);

        Tab<int> nmtls;
        void DoSpawn(int j,int spmult,SpawnVars spvars,TimeValue lvar,BOOL emit);
        void SetUpList();
        void AddToList(INode *newnode,int i,BOOL add);
        void DeleteFromList(int nnum,BOOL all);
        Tab<INode*> nlist;
        Tab<int> llist;
        int deftime;
        int maincount,rseed;
        int NumRefs() {return COMMON_BASER+nlist.Count();}
        RefTargetHandle GetReference(int i);
        void SetReference(int i, RefTargetHandle rtarg);        
        RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, 
           PartID& partID, RefMessage message);
        void SetUpLifeList();
        void AddToLifeList(int newlife);
        void DeleteFromLifeList(int nnum);
        void ShowName();
        static AName *NameLst;
        static HWND hParams2,hptype,hgen,hparam,hrot,spawn;
        static ICustEdit *custCtrlEdit;
        void ResetSystem(TimeValue t,BOOL full=TRUE);

        void GetWorldBoundBox(TimeValue t, INode *inode, ViewExp* vpt, Box3& box);
        int CountLive();
        int rcounter,vcounter;
        oldipc lc;
        static IObjParam *ip;
        // Animatable methods       
        void DeleteThis() {delete this;}
        void MapKeys(TimeMap *map,DWORD flags);
        int RenderBegin(TimeValue t, ULONG flags);      
        int RenderEnd(TimeValue t);

        // From SimpleParticle
        Interval GetValidity(TimeValue t);      
        BOOL EmitterVisible();      
        MarkerType GetMarkerType(); 
        CSavePt *sdata;

        // From BaseObject
        int IsInstanceDependent() {return 1;}
        Matrix3 TumbleMat(int index,float amount, float scale);
        void GetFilename(TCHAR *filename);
        void SetupTargetList();
        int SaveSettings(int overwrite,TCHAR *newname);
        int GetSettings(int setnum,TCHAR *newname);
        int RemSettings(int setnum,TCHAR *newname);
        IOResult Load(ILoad *iload);
        IOResult Save(ISave *isave);
        void AssignMtl(INode *node,INode *topnode,int subtree,TimeValue t);
        void DoGroupMtls(INode *node,int subtree,int *numsubs,int *numtabs,int *tabmax,TimeValue t);
        void RetrieveMtls(INode *node,int subtree,int *numsubs,int *numtabs,int *tabmax,TimeValue t);
        void GetSubs(INode *node,INode *topnode,int subtree,TimeValue t);
        void CntDoGroupMtls(INode *node,int subtree,int *numsubs,int *numtabs,int *tabmax,TimeValue t);
        void CntRetrieveMtls(INode *node,int subtree,int *numsubs,int *numtabs,int *tabmax,TimeValue t);
        BOOL backpatch;
        Mtllst mttab;
        TimeValue ParticleLife(TimeValue t, int i);
        int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
        int HitTest(TimeValue t, INode *inode, int type, int crossing, int flags, 
        IPoint2 *p, ViewExp *vpt);
        TimeValue dispt;
    };

typedef Tab<Mtl *>Mtllst;

static Class_ID PArray_CLASS_ID(0xe3c25b5, 0x109d1659);

#define PBPARR_DISTRIBUTION     0
#define PBPARR_EMITTERCOUNT     1
#define PBPARR_SPEED            2
#define PBPARR_SPEEDVAR         3
#define PBPARR_ANGLEDIV         4

#define PBPARR_BIRTHMETHOD      5
#define PBPARR_PBIRTHRATE       6
#define PBPARR_PTOTALNUMBER     7
#define PBPARR_EMITSTART        8
#define PBPARR_EMITSTOP         9
#define PBPARR_DISPUNTIL        10
#define PBPARR_LIFE             11
#define PBPARR_LIFEVAR          12
#define PBPARR_SUBFRAMEMOVE     13
#define PBPARR_SUBFRAMETIME     14
#define PBPARR_SIZE             15
#define PBPARR_SIZEVAR          16
#define PBPARR_GROWTIME         17
#define PBPARR_FADETIME         18
#define PBPARR_RNDSEED          19
#define PBPARR_EMITRWID         20
#define PBPARR_EMITRHID         21

#define PBPARR_PARTICLECLASS    22
#define PBPARR_PARTICLETYPE     23
#define PBPARR_METATENSION      24
#define PBPARR_METATENSIONVAR   25
#define PBPARR_METACOURSE       26
#define PBPARR_METAAUTOCOARSE   27
#define PBPARR_FRAGTHICKNESS    28
#define PBPARR_FRAGMETHOD       29
#define PBPARR_FRAGCOUNT        30
#define PBPARR_SMOOTHANG        31
#define PBPARR_USESUBTREE       32
#define PBPARR_ANIMATIONOFFSET  33
#define PBPARR_OFFSETAMOUNT     34
#define PBPARR_VIEWPORTSHOWS    35
#define PBPARR_DISPLAYPORTION   36
#define PBPARR_MAPPINGTYPE      37
#define PBPARR_MAPPINGTIME      38
#define PBPARR_MAPPINGDIST      39
#define PBPARR_CUSTOMMATERIAL   40
#define PBPARR_EMAT             41
#define PBPARR_BMAT             42
#define PBPARR_FMAT             43

#define PBPARR_SPINTIME         44
#define PBPARR_SPINTIMEVAR      45
#define PBPARR_SPINPHASE        46
#define PBPARR_SPINPHASEVAR     47
#define PBPARR_SPINAXISTYPE     48
#define PBPARR_SPINAXISX        49
#define PBPARR_SPINAXISY        50
#define PBPARR_SPINAXISZ        51
#define PBPARR_SPINAXISVAR      52

#define PBPARR_EMITVINFL        53
#define PBPARR_EMITVMULT        54
#define PBPARR_EMITVMULTVAR     55

#define PBPARR_BUBLAMP          56
#define PBPARR_BUBLAMPVAR       57
#define PBPARR_BUBLPER          58
#define PBPARR_BUBLPERVAR       59
#define PBPARR_BUBLPHAS         60
#define PBPARR_BUBLPHASVAR      61

#define PBPARR_STRETCH          62

#define PBPARR_SPAWNTYPE        63
#define PBPARR_SPAWNGENS        64
#define PBPARR_SPAWNCOUNT       65
#define PBPARR_SPAWNDIRCHAOS    66
#define PBPARR_SPAWNSPEEDCHAOS  67
#define PBPARR_SPAWNSPEEDSIGN   68
#define PBPARR_SPAWNINHERITV    69
#define PBPARR_SPAWNSCALECHAOS  70
#define PBPARR_SPAWNSCALESIGN   71
#define PBPARR_SPAWNLIFEVLUE    72
#define PBPARR_SPAWNSPEEDFIXED  73
#define PBPARR_SPAWNSCALEFIXED  74
#define PBPARR_METACOURSEV          75
#define PBPARR_SUBFRAMEROT          76
#define PBPARR_SPAWNPERCENT         77
#define PBPARR_SPAWNMULTVAR         78
#define PBPARR_PANOTDRAFT           79
#define PBPARR_USESELECTED              80
#define PBPARR_PASPAWNDIEAFTER          81
#define PBPARR_PASPAWNDIEAFTERVAR       82
    
#define PBPARR_PAIPCOLLIDE_ON           83
#define PBPARR_PAIPCOLLIDE_STEPS        84
#define PBPARR_PAIPCOLLIDE_BOUNCE       85
#define PBPARR_PAIPCOLLIDE_BOUNCEVAR    86


class PickOperand;
class PArrayParticle;
class PArrayParticleDraw : public CustomParticleDisplay {
    public:
//      float size,VSz,
        BOOL firstpart;
        PArrayParticle *obj;
        int disptype,ptype,bb,anifr,anioff;
        boxlst *bboxpt;
        TimeValue t;
        InDirInfo indir;

        PArrayParticleDraw() {obj=NULL;}
        BOOL DrawParticle(GraphicsWindow *gw,ParticleSys &parts,int i);
    };
typedef struct{
float M,Vsz;
Point3 Ve,vel,pos;
Point3 wbb[8];
}CacheDataType;
#define PARR_BASER 3
class PArrayParticle : public SimpleParticle, IParamArray {
    public:
        PArrayParticleDraw thePArrayDraw;
        PArrayParticle();
        ~PArrayParticle();
        static IParamMap *pmapParam;
        static IParamMap *pmapPGen;
        static IParamMap *pmapPType;
        static IParamMap *pmapPSpin;
        static IParamMap *pmapEmitV;
        static IParamMap *pmapBubl;
        static IParamMap *pmapSpawn;
        int stepSize,size,oldtvnum,lastrnd,emitrnd;
        static int custsettings;
        BOOL cancelled,wasmulti,storernd;
        static BOOL creating;
        BOOL fromgeom;
        INode *distnode,*custnode,*cnode;
        CacheDataType *storefrag;
        TSTR distname,custname;
        ULONG dflags;
        int fragflags;
        BOOL doTVs;
        static AName *NameLst;
        static HWND hParams2,hptype,hgen,hparam,hrot,hbubl,spawn;
        static PickOperand pickCB;
        static ICustEdit *custCtrlEdit;
        Mtl *origmtl;

        // to fix #182223 & 270224, add these members
        Matrix3 lastTM;
        TimeValue lastTMTime;

        BOOL GenerateNotGeom(TimeValue t,TimeValue lastt,int c,int counter,INode *distnode,int type,Matrix3 tm,Matrix3 nxttm);
        void GetInfoFromObject(float thick,int *c,INode *distnode,TimeValue t,TimeValue lastt);
        void GetLocalBoundBox(TimeValue t, INode *inode,ViewExp* vpt,  Box3& box); 
        void GetWorldBoundBox(TimeValue t, INode *inode, ViewExp* vpt, Box3& box);
        void RendGeom(Mesh *pm,Matrix3 itm,int maxmtl,int maptype,BOOL eitmtl,float mval,PArrayParticleDraw thePArrayDraw,TVFace defface,BOOL notrend);

        Tab<int> nmtls;
        void DoSpawn(int j,int spmult,SpawnVars spvars,TimeValue lvar,BOOL emits,int &oldcnt);
        void SetUpList();
        void AddToList(INode *newnode,int i,BOOL add);
        void DeleteFromList(int nnum,BOOL all);
        Tab<INode*> nlist;
        Tab<int> llist;
        int deftime;
        int maincount;
        int NumRefs() {return PARR_BASER+nlist.Count();}
        RefTargetHandle GetReference(int i);
        void SetReference(int i, RefTargetHandle rtarg);        
        RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, 
           PartID& partID, RefMessage message);
        void SetUpLifeList();
        void AddToLifeList(int newlife);
        void DeleteFromLifeList(int nnum);
        void ShowName(int dist);
        int CountLive();
        int rcounter,vcounter;
        oldipc lc;
        static IObjParam *ip;
        CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;}

        void BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev);     
        void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);       
        TimeValue ParticleLife(TimeValue t, int i);
        // Animatable methods       
        void DeleteThis() {delete this;}
        IOResult Load(ILoad *iload);
        IOResult Save(ISave *isave);
        void MapKeys(TimeMap *map,DWORD flags);
        int RenderBegin(TimeValue t, ULONG flags);      
        int RenderEnd(TimeValue t);
        
        // From SimpleParticle
        void BuildEmitter(TimeValue t, Mesh& amesh);
        Interval GetValidity(TimeValue t);      
        void InvalidateUI();
        BOOL EmitterVisible();      
        MarkerType GetMarkerType(); 
        SavePt *sdata;
        Mesh *pmesh;
        // From BaseObject
        TCHAR *GetObjectName();
        void BirthParticle(INode *node,TimeValue bt,int index,VelDir2 *ptvel2,VelDir ptvel,Matrix3 tmlast);
        BOOL ComputeParticleStart(TimeValue t0,TimeValue lastt,INode *node,int c);
        int IsInstanceDependent() {return 1;}

        // From GeomObject
        Mesh* GetRenderMesh(TimeValue t, INode *inode, View& view, BOOL& needDelete);

        // Animatable methods       
        Class_ID ClassID() {return PArray_CLASS_ID;} 

        // From ref
        RefTargetHandle Clone(RemapDir& remap = DefaultRemapDir());     

        // From Simple Particle
        void UpdateParticles(TimeValue t,INode *node);
        ParamDimension *GetParameterDim(int pbIndex);
        TSTR GetParameterName(int pbIndex);
        void GetFilename(TCHAR *filename);
        void SetupTargetList();
        int SaveSettings(int overwrite,TCHAR *newname);
        int GetSettings(int setnum,TCHAR *newname);
        int RemSettings(int setnum,TCHAR *newname);
        Point3 ParticlePosition(TimeValue t,int i);
        Point3 ParticleVelocity(TimeValue t,int i);     
        float ParticleSize(TimeValue t,int i);
        int ParticleCenter(TimeValue t,int i);

        Mesh *cmesh,*dispmesh;
        Box3 *cmbb;
        Point3 boxcenter;
        int CustMtls;
        TimeLst times;
        void GetTimes(TimeLst &times,TimeValue t,int anifr,int ltype,int fragflags);
        void TreeDoGroup(INode *node,Matrix3 tspace,Mesh *cmesh,Box3 *cmbb,int *numV,int *numF,int *tvnum,int *ismapped,TimeValue t,int subtree,int custmtl);
        void CheckTree(INode *node,Matrix3 tspace,Mesh *cmesh,Box3 *cmbb,int *numV,int *numF,int *tvnum,int *ismapped,TimeValue t,int subtree,int custmtl);
        void GetMesh(TimeValue t,int subtree,int custmtl,int fragflags);
        void GetNextBB(INode *node,int subtree,int *count,int *tabmax,Point3 boxcenter,TimeValue t,int tcount,INode *onode);
        void DoGroupBB(INode *node,int subtree,int *count,int *tabmax,Point3 boxcenter,TimeValue t,int tcount,INode *onode);
        void GetallBB(INode *custnode,int subtree,TimeValue t,int fragflags);
        void AssignMtl(INode *node,INode *topnode,int subtree,TimeValue t,int fragflags);
        void DoGroupMtls(INode *node,int subtree,int *numsubs,int *numtabs,int *tabmax,TimeValue t);
        void RetrieveMtls(INode *node,int subtree,int *numsubs,int *numtabs,int *tabmax,TimeValue t);
        void GetSubs(INode *node,INode *topnode,int subtree,TimeValue t,int fragflags);
        void CntDoGroupMtls(INode *node,int subtree,int *numsubs,int *numtabs,int *tabmax,TimeValue t);
        void CntRetrieveMtls(INode *node,int subtree,int *numsubs,int *numtabs,int *tabmax,TimeValue t);
        BOOL backpatch;
        Mtllst mttab;
        int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
        int HitTest(TimeValue t, INode *inode, int type, int crossing, int flags, 
        IPoint2 *p, ViewExp *vpt);
        TimeValue dispt;
        void MovePart(int j,TimeValue dt,BOOL fullframe,int tpf);
        void ResetSystem(TimeValue t,BOOL full=TRUE);
    };

#define RAINSIZEFACTOR (float(TIME_TICKSPERSEC)/120.0f)
#define PARTICLE_SEED   0x8d6a65bc


#define PB_VPTPARTICLES     0
#define PB_RNDPARTICLES     1
#define PB_DROPSIZE         2
#define PB_SPEED            3
#define PB_VARIATION        4
#define PB_DISPTYPE         5
#define PB_STARTTIME        6
#define PB_LIFETIME         7
#define PB_EMITTERWIDTH     8
#define PB_EMITTERHEIGHT    9
#define PB_HIDEEMITTER      10
#define PB_BIRTHRATE        11
#define PB_CONSTANT         12
#define PB_RENDER           13
#define PB_TUMBLE           14
#define PB_SCALE            15


//---------------------------------------------------------------------------
#define STDMTL_PB_OPAC      std2_opacity
#define STDMTL_PB_AMBIENT   shdr_ambient
#define STDMTL_PB_DIFFUSE   shdr_diffuse
#define STDMTL_PB_SELFI     shdr_self_illum_amnt
#define STDMTL_PB_SPECULAR  shdr_specular
//---------------------------------------------------------------------------
#define DEFLECTOBJECT_CLASS_ID      0xe5242
#define DEFLECTMOD_CLASS_ID         0xe5243
#define IDS_RB_DEFLECTOR_CLASS          30288
#define IDS_RB_DEFLECTOR                30553
#define IDS_RB_DEFLECTMOD               30554
#define IDS_RB_DEFLECTORBINDING         30555
// block IDs
enum { deflect_params, };

// geo_param param IDs
enum { deflect_bounce, deflect_width, deflect_height, deflect_variation, deflect_chaos,deflect_friction, 
       deflect_inherit_vel,deflect_quality,deflect_collider};

class DeflectObject : public SimpleWSMObject2 { 
    public:     
//      static IParamMap *pmapParam;
        static IObjParam *ip;
        static HWND hSot;
                    
        DeflectObject();
        ~DeflectObject();

        // From Animatable      
        void DeleteThis() {delete this;}        
        void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
        void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);       
        Class_ID ClassID() {return Class_ID(DEFLECTOBJECT_CLASS_ID,0);}     
        RefTargetHandle Clone(RemapDir& remap = DefaultRemapDir());
        TCHAR *GetObjectName() {return GetString(IDS_RB_DEFLECTOR);}
                
        // from object      
        CreateMouseCallBack* GetCreateMouseCallBack();      
        
        // From SimpleWSMObject     
        void InvalidateUI();        
        void BuildMesh(TimeValue t);
        ParamDimension *GetParameterDim(int pbIndex);
//      TSTR GetParameterName(int pbIndex);     
        
        // From WSMObject
        Modifier *CreateWSMMod(INode *node);    
        
        // Direct paramblock access
        int NumParamBlocks() { return 1; }
        IParamBlock2* GetParamBlock(int i) { return pblock2; }
        IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock2->ID() == id) ? pblock2 : NULL; }

        IOResult Load(ILoad *iload);
//watje
        CollisionObject *GetCollisionObject(INode *node);

    };

//--- DeflectMod -----------------------------------------------------
#include "ICollision.h"
class DeflectorField : public CollisionObject {
    public: 
        DeflectorField()
        {
            srand(1264873);
            for (int i =0;i < 500; i++)
                {
                randomFloat[i] = (float)rand()/(float)RAND_MAX;
                }
            colp=NULL;
        }

        ~DeflectorField()
        {
            if (colp) colp->DeleteThis();
            colp=NULL;
        }

        DeflectObject *obj;
        INode *node;
        Matrix3 tm, invtm;
        Interval tmValid;       
        CollisionPlane *colp;

        float randomFloat[500];

        BOOL CheckCollision(TimeValue t,Point3 &pos, Point3 &vel, float dt, int index, float *ct,BOOL UpdatePastCollide);
        Object *GetSWObject();

//hmm moved these so they are only gotten at one tick in time
        float width, height, at, bounce;
        float friction, chaos, variation, inherit;
        int quality;

    };
class DeflectMod : public SimpleWSMMod {
    public:             
        DeflectorField deflect;

        DeflectMod() {}
        DeflectMod(INode *node,DeflectObject *obj);     

        // From Animatable
        void GetClassName(TSTR& s) {s= GetString(IDS_RB_DEFLECTMOD);}
        SClass_ID SuperClassID() {return WSM_CLASS_ID;}
        void DeleteThis() {delete this;}
        Class_ID ClassID() { return Class_ID(DEFLECTMOD_CLASS_ID,0);}
        RefTargetHandle Clone(RemapDir& remap = DefaultRemapDir());
        TCHAR *GetObjectName() {return GetString(IDS_RB_DEFLECTORBINDING);}

        void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);

        // From SimpleWSMMod        
        Interval GetValidity(TimeValue t);      
        Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat);


    };
//---------------------------------------------------------------------------
enum 
{   sdeflectrobj_params, 
}; 

enum 
{   sdeflectrobj_bounce, 
    sdeflectrobj_bouncevar,
    sdeflectrobj_chaos,
    sdeflectrobj_radius,
    sdeflectrobj_velocity,
    sdeflectrobj_friction,
//watje ref to hold the collision engine
    sdeflectrobj_collider
};


//---------------------------------------------------------------------------
static Class_ID SPHEREDEF_CLASS_ID(0x6cbd289d, 0x3fef6656);
static Class_ID SPHEREDEFMOD_CLASS_ID(0x5cdf4181, 0x4c5b42f9);
#define IDS_RB_SPHDEFLECTOR             74
#define IDS_RB_SPHDEFLECTMOD            75
#define IDS_RB_SPHDEFBINDING            76

class SphereDefObject : public SimpleWSMObject2 {   
    public:     
//      static IParamMap *pmapParam;
        static IObjParam *ip;
        static HWND hSot;
        int lastrnd;
        TimeValue t;
                    
        SphereDefObject();
        ~SphereDefObject();

        // From Animatable      
        void DeleteThis() {delete this;}        
        void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
        void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);       
        Class_ID ClassID() {return SPHEREDEF_CLASS_ID;}     
        RefTargetHandle Clone(RemapDir& remap = DefaultRemapDir());
        TCHAR *GetObjectName() {return GetString(IDS_RB_SPHDEFLECTOR);}
                
        // from object      
        CreateMouseCallBack* GetCreateMouseCallBack();      
        
        // From SimpleWSMObject     
        void InvalidateUI();        
        void BuildMesh(TimeValue t);
        ParamDimension *GetParameterDim(int pbIndex);
//      TSTR GetParameterName(int pbIndex);     
        
        IOResult Load(ILoad *iload);
        CollisionObject *GetCollisionObject(INode *node);

        // From WSMObject
        Modifier *CreateWSMMod(INode *node);        

        // Direct paramblock access
        int NumParamBlocks() { return 1; }
        IParamBlock2* GetParamBlock(int i) { return pblock2; }
        IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock2->ID() == id) ? pblock2 : NULL; }
    };


class SphereDeflectorField : public CollisionObject {
    public:     
//watje
        SphereDeflectorField()
            {
            srand(7896764);
            for (int i =0;i < 500; i++)
                {
                randomFloat[i] = (float)rand()/(float)RAND_MAX;
                }
            cols=NULL;
            }

        ~SphereDeflectorField()
        {
            if (cols) cols->DeleteThis();
            cols=NULL;
        }

//watje moved these here since they don't need to be accessed for every particle
        float radius,chaos,bounce,bvar,vinher,friction;
//using a table of rnadmom floats now
        float randomFloat[500];

        SphereDefObject *obj;
        INode *node;
        Matrix3 tm, invtm,tp;
        Interval tmValid;
        Point3 Vc,Vcp;
        BOOL CheckCollision(TimeValue t,Point3 &pos, Point3 &vel, float dt, int index,float *ct,BOOL UpdatePastCollide);
        Object *GetSWObject();
        CollisionSphere *cols;
    };


class SphereDeflectMod : public SimpleWSMMod {
    public:             
        SphereDeflectorField deflect;

        SphereDeflectMod() {}
        SphereDeflectMod(INode *node,SphereDefObject *obj);     

        // From Animatable
        void GetClassName(TSTR& s) {s= GetString(IDS_RB_SPHDEFLECTMOD);}
        SClass_ID SuperClassID() {return WSM_CLASS_ID;}
        void DeleteThis() {delete this;}
        Class_ID ClassID() { return SPHEREDEFMOD_CLASS_ID;}
        RefTargetHandle Clone(RemapDir& remap = DefaultRemapDir());
        TCHAR *GetObjectName() {return GetString(IDS_RB_SPHDEFBINDING);}

        void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);

        // From SimpleWSMMod        
        Interval GetValidity(TimeValue t);      
        Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat);
    };


//---------------------------------------------------------------------------
#define IDS_RB_GRAVITY                  30569
#define IDS_RB_WIND                     30570
#define IDS_RB_GRAVITYMOD               30571
#define IDS_RB_GRAVITYBINDING           30572
#define IDS_RB_WINDMOD                  30573
#define IDS_RB_WINDBINDING              30574
#define IDD_WINDPARAM                   138
#define IDD_GRAVITYPARAM                139
#define IDD_DEFLECTORPARAM              140

enum { gravity_params, };

// geo_param param IDs
enum { PB_GRAV_STRENGTH, PB_GRAV_DECAY, PB_GRAV_TYPE, PB_GRAV_DISPLENGTH, PB_GRAV_HOOPSON };

class ForceObject : public SimpleWSMObject2 
{   
    public:     
//      static IParamMap *pmapParam;
        static IObjParam *ip;
        static HWND hSot;
                    
        ForceObject() {}
        BOOL SupportsDynamics() {return TRUE;}

        // From Animatable      
        void DeleteThis() {delete this;}        
        void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev );
        void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);       
                
        // from object      
        CreateMouseCallBack* GetCreateMouseCallBack();      
        
        // From SimpleWSMObject     
        void InvalidateUI();        
        void BuildMesh(TimeValue t);

        virtual int DialogID()=0;
//      virtual ParamUIDesc *UIDesc()=0;
//      virtual int UIDescLength()=0;
        int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
        void GetWorldBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box );
};

#include "pod.h"

class GravityObject;

class GravityField : public ForceField 
{
    public:
        GravityObject *obj;
        INode *node;
        Matrix3 tm;
        Interval tmValid;
        Point3 force;
        Interval fValid;
        int type;
        Point3 Force(TimeValue t,const Point3 &pos, const Point3 &vel,int index);
        void DeleteThis() {delete this;} // RB 5/12/99
};

class GravityMod : public SimpleWSMMod 
{
    public:             
        GravityField force;

        GravityMod() {}
        GravityMod(INode *node,GravityObject *obj);     

        // From Animatable
        void GetClassName(TSTR& s) {s= GetString(IDS_RB_GRAVITYMOD);}
        SClass_ID SuperClassID() {return WSM_CLASS_ID;}
        void DeleteThis() {delete this;}
        Class_ID ClassID() { return Class_ID(GRAVITYMOD_CLASS_ID,0);}
        RefTargetHandle Clone(RemapDir& remap = DefaultRemapDir());
        TCHAR *GetObjectName() {return GetString(IDS_RB_GRAVITYBINDING);}

        void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);

        // From SimpleWSMMod        
        Interval GetValidity(TimeValue t);      
        Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat);
};

class ParticleData;

class GravityObject : public ForceObject,IOperatorInterface 
{   
    public:                                 
        GravityObject();        
        ~GravityObject();       

        // From Animatable      
        void DeleteThis() {delete this;}        
        Class_ID ClassID() {return Class_ID(GRAVITYOBJECT_CLASS_ID,0);}     
        RefTargetHandle Clone(RemapDir& remap = DefaultRemapDir());
        TCHAR *GetObjectName() {return GetString(IDS_RB_GRAVITY);}
                        
        // From WSMObject
        Modifier *CreateWSMMod(INode *node);
        ForceField *GetForceField(INode *node);

        // From SimpleWSMObject             
        ParamDimension *GetParameterDim(int pbIndex);
//      TSTR GetParameterName(int pbIndex);

        int DialogID() {return IDD_GRAVITYPARAM;}
//      ParamUIDesc *UIDesc();
//      int UIDescLength();

        // Direct paramblock access
        int NumParamBlocks() { return 1; }
        IParamBlock2* GetParamBlock(int i) { return pblock2; }
        IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock2->ID() == id) ? pblock2 : NULL; }

        // from ref
        IOResult Load(ILoad *iload);

        int NPOpInterface(TimeValue t,ParticleData *part,float dt,INode *node,int index);
        void* GetInterface(ULONG id);
        ForceField *ff;
        void SetUpModifier(TimeValue t,INode *node);
        GravityMod *mf;
};


//---------------------------------------------------------------------------
class WindMod;
#define IDS_RB_WIND_CLASS               40036
#define IDS_RB_WINDMOD                  30573
#define IDS_RB_WINDBINDING              30574
#define IDS_RB_TURBULENCE               30575
#define IDS_RB_FREQUENCY                30576

class WindObject : public SimpleWSMObject2 // ,IOperatorInterface // Bayboro 9/18/01
{   
    public:     
//      static IParamMap *pmapParam;
        static IObjParam *ip;
        static HWND hSot;

        WindObject();       
        ~WindObject();      
        TCHAR *GetObjectName();
                    
        BOOL SupportsDynamics() {return TRUE;}

        // From Animatable      
        void DeleteThis() {delete this;}        
        Class_ID ClassID() ;    
        RefTargetHandle Clone(RemapDir& remap = DefaultRemapDir());
        void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev );
        void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);       
                
        // from object      
        CreateMouseCallBack* GetCreateMouseCallBack();  
        
        // From WSMObject
        Modifier *CreateWSMMod(INode *node);
        ForceField *GetForceField(INode *node);
        
        // From SimpleWSMObject     
        ParamDimension *GetParameterDim(int pbIndex);
//      TSTR GetParameterName(int pbIndex);
        void InvalidateUI();        
        void BuildMesh(TimeValue t);

        int DialogID();
//      ParamUIDesc *UIDesc();
//      int UIDescLength();

        // Direct paramblock access
        int NumParamBlocks() { return 1; }
        IParamBlock2* GetParamBlock(int i) { return pblock2; }
        IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock2->ID() == id) ? pblock2 : NULL; }

        // from ref
        IOResult Load(ILoad *iload);
        int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
        void GetWorldBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box );

//      int NPOpInterface(TimeValue t,ParticleData *part,float dt,INode *node,int index); // Bayboro 9/18/01
//      void* GetInterface(ULONG id); // Bayboro 9/18/01
        ForceField *ff;
        void SetUpModifier(TimeValue t,INode *node);
        WindMod *mf;
};


class WindField : public ForceField 
{
    public:
        WindObject *obj;
        INode *node;
        Matrix3 tm;
        Interval tmValid;
        Point3 force;
        Interval fValid;
        int type;
        Point3 Force(TimeValue t,const Point3 &pos, const Point3 &vel,int index);
        void DeleteThis() {delete this;} // RB 5/12/99
};

class WindMod : public SimpleWSMMod 
{
    public:             
        WindField force;

        WindMod() {}
        WindMod(INode *node,WindObject *obj);       

        // From Animatable
        void GetClassName(TSTR& s) {s= GetString(IDS_RB_WINDMOD);}
        SClass_ID SuperClassID() {return WSM_CLASS_ID;}
        void DeleteThis() {delete this;}
        Class_ID ClassID() { return Class_ID(WINDMOD_CLASS_ID,0);}
        RefTargetHandle Clone(RemapDir& remap = DefaultRemapDir());
        TCHAR *GetObjectName() {return GetString(IDS_RB_WINDBINDING);}

        void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);

        // From SimpleWSMMod        
        Interval GetValidity(TimeValue t);      
        Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat);
};



// block IDs
enum { wind_params, };

// geo_param param IDs
enum { PB_WIND_STRENGTH,PB_WIND_DECAY,PB_WIND_TYPE,PB_WIND_DISPLENGTH,
       PB_WIND_TURBULENCE,PB_WIND_FREQUENCY,PB_WIND_SCALE,PB_WIND_HOOPSON };
//
//
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static Class_ID PBOMB_CLASS_ID(0x4c200df3, 0x1a347a77);
static Class_ID PBOMBMOD_CLASS_ID(0xc0609ea, 0x1300b3d);

#define IDS_RB_PBOMB                    62
#define IDS_RB_PBOMBMOD                 63
#define IDS_RB_PBOMBBINDING             64

#define PBOMB_SYMMETRY          0
#define PBOMB_CHAOS         1
#define PBOMB_STARTTIME     2
#define PBOMB_LASTSFOR          3
#define PBOMB_DELTA_V           4 
#define PBOMB_DECAY         5
#define PBOMB_DECAYTYPE     6
#define PBOMB_ICONSIZE          7
#define PBOMB_RANGEON           8

class PBombObject;

class PBombField : public ForceField {
    public:
        PBombObject *obj;
        TimeValue dtsq,dt;
        INode *node;
        int count;
        Matrix3 tm,invtm;
        Interval tmValid;
        Point3 force;
        Interval fValid;
        Point3 Force(TimeValue t,const Point3 &pos, const Point3 &vel, int index);
    };

class PBombMod : public SimpleWSMMod {
    public:             
        PBombField force;
        int seed;

        PBombMod() {}
        PBombMod(INode *node,PBombObject *obj);     
        // From Animatable
        void GetClassName(TSTR& s) {s= GetString(IDS_RB_PBOMBMOD);}
        SClass_ID SuperClassID() {return WSM_CLASS_ID;}
        void DeleteThis() {delete this;}
        Class_ID ClassID() { return PBOMBMOD_CLASS_ID;}
        RefTargetHandle Clone(RemapDir& remap = DefaultRemapDir());
        TCHAR *GetObjectName() {return GetString(IDS_RB_PBOMBBINDING);}

        void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);

        // From SimpleWSMMod        
        Interval GetValidity(TimeValue t);      
        Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat);
    };

#define IDD_SW_PARTICLEBOMB             110

class PBombObject : public SimpleWSMObject,IOperatorInterface 
{   
    public:                                 
        PBombObject();      
        ~PBombObject();     
        static IParamMap *pmapParam;
        static IObjParam *ip;
        static HWND hSot;
        ForceField *GetForceField(INode *node);
        BOOL SupportsDynamics() {return TRUE;}
        IOResult Load(ILoad *iload);

        // From Animatable      
        void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev );
        void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);       
        void MapKeys(TimeMap *map,DWORD flags);
                
        // from object      
        CreateMouseCallBack* GetCreateMouseCallBack();      
        
        // From SimpleWSMObject     
        void InvalidateUI();        
        // From Animatable      
        void DeleteThis() {delete this;}        
        Class_ID ClassID() {return PBOMB_CLASS_ID;}     
        RefTargetHandle Clone(RemapDir& remap = DefaultRemapDir());
        TCHAR *GetObjectName() {return GetString(IDS_RB_PBOMB);}
                        
        // From WSMObject
        Modifier *CreateWSMMod(INode *node);
        
        // From SimpleWSMObject             
        ParamDimension *GetParameterDim(int pbIndex);
        TSTR GetParameterName(int pbIndex);
        void BuildMesh(TimeValue t);
        int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);

        int DialogID() {return IDD_SW_PARTICLEBOMB;}
        ParamUIDesc *UIDesc();
        int UIDescLength();
        TSTR UIStrName();
        void GetWorldBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box );

        int NPOpInterface(TimeValue t,ParticleData *part,float dt,INode *node,int index);
        void* GetInterface(ULONG id);
        ForceField *ff;
        void SetUpModifier(TimeValue t,INode *node);
        PBombMod *mf;
    };

//---------------------------------------------------------------------------
// Found in file: .\maxsdk\samples\mesh\editablepoly\EditNormals.cpp

#include "iEditNormals.h"


#define EDIT_NORMALS_CLASS_ID Class_ID(0x4aa52ae3, 0x35ca1cde)

//---------------------------------------------------------------------------
// Found in file: .\maxsdk\samples\materials\acubic.cpp

/**********************************************************************
 *<
    FILE: ACUBIC.CPP

    DESCRIPTION: Cubic Reflection/Refraction map.

    CREATED BY: Dan Silva

    HISTORY:
                Update 11/17 to param block2 by Peter Watje

 *> Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include <bmmlib.h>
#include "render.h"
#include "stdmat.h"
#include "iparamm2.h"


extern HINSTANCE hInstance;

static Class_ID acubicClassID(ACUBIC_CLASS_ID,0);

//#define PB_SIZE       0
//#define PB_BLUR       1
//#define PB_BLUROFF  2
//#define PB_NEAR     3
//#define PB_FAR      4

enum { acubic_params };  // pblock ID
// grad_params param IDs
enum 
{ 
    acubic_size, acubic_blur, acubic_bluroffset, acubic_near, acubic_far,
    acubic_source, acubic_useatmospheric, acubic_applyblur,
    acubic_frametype, acubic_nthframe,
    acubic_bitmap_names,
    acubic_outputname,
//  acubic_camera,
//  acubic_node

};

//--------------------------------------------------------------
// CubicMap 
//--------------------------------------------------------------
#define VIEW_UP 0
#define VIEW_DN 1
#define VIEW_LF 2
#define VIEW_RT 3
#define VIEW_FR 4
#define VIEW_BK 5

class CubicMap {
    public:
        Bitmap *bitmap[6];
        Bitmap *blurmap;
        int size;
        CubicMap *next;
        int nodeID;
        BOOL tossMapsWhenBlur;
        TimeValue mapTime; // when maps were last rendered
    // Methods
        CubicMap();
        ~CubicMap();
        void FreeMaps();
        void FreeCubicMaps();
        int AllocMaps(int sz);
        BMM_Color_64 Sample(Point3 dir);
        int BuildBlurMap();
        void FreeBlurMap();
    };    


//--------------------------------------------------------------
// ACubic: 
//--------------------------------------------------------------
#define NUM_SRC_FILES 6 // mjm - 2.3.99

class ACubic: public StdCubic { 
    friend class ACubicPostLoad;
    friend class ACubicDlgProc;
    int size;
    int nth;
    float blur;
    float blurOff;
    float nearRange,farRange;
//  CRITICAL_SECTION csect;
    BOOL applyBlur;
    BOOL do_nth;
    BOOL useEnvMap;
    BOOL inMedit;
    BitmapInfo biInFile;            // for reading cubic map files
    BitmapInfo biOutFile;           // for rendering cubic map.
    TSTR fileNames[NUM_SRC_FILES];  // source files
    CubicMap *maps;                 // when Automatic mode
    Interval ivalid;
    int rollScroll;



//  ACubicDlg *paramDlg;
    public:
        BOOL useFile;
        IMtlParams *ip;
        IParamBlock2 *pblock;  // ref #1
        ACubic();
        ~ACubic() ;
        ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
        void Update(TimeValue t, Interval& valid);
        void Init();
        void Reset();
        Interval Validity(TimeValue t) ;

        // From StdCubic
        void SetSize(int n, TimeValue t);
        void SetDoNth(BOOL onoff) { do_nth = onoff; }
        void SetNth(int n) { nth = n; }
        void SetApplyBlur(BOOL onoff);
        void SetBlur(float b, TimeValue t);
        void SetBlurOffset(float b, TimeValue t);
        void UseHighDynamicRange(BOOL onoff);
        int IsHighDynamicRange( ) const;
        int GetSize(TimeValue t){ return pblock->GetInt(acubic_size,t); }
        BOOL GetDoNth() { return do_nth; }
        int GetNth() { return nth; }
        BOOL GetApplyBlur() { return applyBlur; }
        float GetBlur(TimeValue t) { return pblock->GetFloat(acubic_blur,t); }
        float GetBlurOffset(TimeValue t){ return pblock->GetFloat(acubic_bluroffset,t); }
        int  GetOutFileName(TSTR& fullname, TSTR &fname, int i);
        int WriteBM(Bitmap *bm, TCHAR *name);
        void RenderCubicMap(INode *node);
        void SetUseFile(BOOL onOff);
        float GetNearRange(TimeValue t) { return pblock->GetFloat(acubic_near,t); }
        float GetFarRange(TimeValue t) { return pblock->GetFloat(acubic_far,t); }
        void SetNearRange(float v, TimeValue t);
        void SetFarRange(float v, TimeValue t);

        void NotifyChanged();
    
        // Evaluate the color of map for the context.
        RGBA EvalColor(ShadeContext& sc);

        // optimized evaluation for monochrome use
        float EvalMono(ShadeContext& sc);

        // For Bump mapping, need a perturbation to apply to a normal.
        // Leave it up to the Texmap to determine how to do this.
        Point3 EvalNormalPerturb(ShadeContext& sc);

        ULONG LocalRequirements(int subMtlNum) ;

        int LoadMapFiles(TimeValue t);
        int BuildMaps(TimeValue t, RenderMapsContext &rmc);
        int DoThisFrame(TimeValue t, BOOL fieldRender, TimeValue mapTime);
        CubicMap *FindMap(int nodeNum);
        void FreeMaps();
        int RenderBegin(TimeValue t, ULONG flags) {  return 1;}
        int RenderEnd(TimeValue t) { if (!useFile) FreeMaps(); return 1; }

        Class_ID ClassID();
        SClass_ID SuperClassID() ;
        void GetClassName(TSTR& s);  
        void DeleteThis() { delete this; }  

        int NumSubs() {return 1; }  
        Animatable* SubAnim(int i);
        TSTR SubAnimName(int i);
        int SubNumToRefNum(int subNum) { return subNum; }

        // From ref
        int NumRefs() { return 1; }
        RefTargetHandle GetReference(int i);
        void SetReference(int i, RefTargetHandle rtarg);

        void EnumAuxFiles(NameEnumCallback& nameEnum, DWORD flags);
        RefTargetHandle Clone(RemapDir &remap = DefaultRemapDir());
        RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
           PartID& partID, RefMessage message );

        // IO
        IOResult Save(ISave *isave);
        IOResult Load(ILoad *iload);

// JBW: direct ParamBlock access is added
        int NumParamBlocks() { return 1; }                  // return number of ParamBlocks in this instance
        IParamBlock2* GetParamBlock(int i) { return pblock; } // return i'th ParamBlock
        IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock

        void UpdateInterface(BOOL on);  

        // From Texmap
        bool IsLocalOutputMeaningful( ShadeContext& sc );
};

//---------------------------------------------------------------------------
//  ListControl
//---------------------------------------------------------------------------

extern HINSTANCE hInstance;

#define FLOATLIST_CNAME     GetString(IDS_RB_FLOATLIST)
#define POINT3LIST_CNAME    GetString(IDS_RB_POINT3LIST)
#define POSLIST_CNAME       GetString(IDS_RB_POSITIONLIST)
#define ROTLIST_CNAME       GetString(IDS_RB_ROTATIONLIST)
#define SCALELIST_CNAME     GetString(IDS_RB_SCALELIST)
#define MASTERLIST_CNAME    GetString(IDS_PW_MASTERBLOCK)
#define POINT4LIST_CNAME    GetString(IDS_RB_POINT4LIST)

#define PBLOCK_INIT_REF_ID 2

static void UnRegisterListCtrlWindow(HWND hWnd);

class ListControl;

typedef Tab<Control*> ControlTab;

class NameList : public Tab<TSTR*> {
    public:
        void Free() {
            for (int i=0; i<Count(); i++) {
                delete (*this)[i];
                (*this)[i] = NULL;
                }
            }
        void Duplicate() {
            for (int i=0; i<Count(); i++) {
                if ((*this)[i]) (*this)[i] = new TSTR(*(*this)[i]);
                }
            }
    };

class ListDummyEntry : public Control {
    public:
        ListControl *lc;
        void Init(ListControl *l);
        Class_ID ClassID() {return Class_ID(DUMMY_CONTROL_CLASS_ID,0);}
        SClass_ID SuperClassID();
        void GetClassName(TSTR& s) {s=_T("");}
        RefResult AutoDelete() {return REF_SUCCEED;}
        RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
             PartID& partID,  RefMessage message) {return REF_SUCCEED;}
        RefTargetHandle Clone(RemapDir &remap) {BaseClone(this,this,remap); return this;}

        void Copy(Control *from) {}
        void CommitValue(TimeValue t) {}
        void RestoreValue(TimeValue t) {}
        void GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method=CTRL_ABSOLUTE);
        void SetValue(TimeValue t, void *val, int commit=1, GetSetMethod method=CTRL_ABSOLUTE) {}

        BOOL CanCopyAnim() {return FALSE;}
        int IsKeyable() {return FALSE;}
    };


class ListControlMotionDlg;

enum { kListCtrlWeightParams };                 // paramblock IDs
enum { kListCtrlWeight, kListCtrlAverage };     // parameter IDs

class ListControl : public IListControl {
    public:
        ListDummyEntry dummy;
        Tab<Control*> conts;    //ControlTab 
        Control *clip;          //ListControl
        NameList names;
        TSTR nameClip;
        float weightClip;
        int active; 
        
        IParamBlock2* pblock;
    
        ListControlMotionDlg *dlg;
        DWORD paramFlags;

        ListControl(BOOL loading=FALSE);
        ListControl(const ListControl& ctrl);
        virtual ~ListControl(); 
        
        ListControl& operator=(const ListControl& ctrl);
        void Resize(int c);
        float AverageWeight(float weight) 
        {
            BOOL average = FALSE;
            pblock->GetValue(kListCtrlAverage, 0, average, FOREVER);
            if (average) {
                float tempWeight = 0.0f;
                float totalWeight = 0.0f;;
                for (int i=0;i<conts.Count();i++) {
                    pblock->GetValue(kListCtrlWeight, 0, tempWeight, FOREVER, i);
                    totalWeight += tempWeight;
                    }
                return totalWeight!=0.0f?(average?(weight/totalWeight):weight):0.0f;
                }
            return weight;
        }

        
        //virtual ClassDesc2 *GetListDesc()=0;
        virtual ListControl *DerivedClone()=0;

        //From IListControl
        int  GetListCount() {return conts.Count();}
        void SetActive(int index);
        int  GetActive() { return active; }
        void DeleteItem(int index);
        void CutItem(int index);
        void PasteItem(int index);
        void SetName(int index, TSTR name);
        TSTR GetName(int index);
        
        //From FPMixinInterface
        BaseInterface* GetInterface(Interface_ID id) 
        { 
            if (id == LIST_CONTROLLER_INTERFACE) 
                return (IListControl*)this; 
            else 
                return FPMixinInterface::GetInterface(id);
        } 


        // From Control
        void Copy(Control *from);
        void CommitValue(TimeValue t);
        void RestoreValue(TimeValue t);
        BOOL IsLeaf() {return FALSE;}       
        int IsKeyable();
        void SetValue(TimeValue t, void *val, int commit=1, GetSetMethod method=CTRL_ABSOLUTE);
        void EnumIKParams(IKEnumCallback &callback);
        BOOL CompDeriv(TimeValue t,Matrix3& ptm,IKDeriv& derivs,DWORD flags);
        void MouseCycleCompleted(TimeValue t);
        BOOL InheritsParentTransform();

        // From Animatable
        virtual int NumSubs()  {return conts.Count()+2;} //numControllers+dummyController+pblock
        virtual Animatable* SubAnim(int i);
        virtual TSTR SubAnimName(int i);        
        void DeleteThis() {delete this;}        
        void AddNewKey(TimeValue t,DWORD flags);
        void CloneSelectedKeys(BOOL offset);
        void DeleteKeys(DWORD flags);
        void SelectKeys(TrackHitTab& sel, DWORD flags);
        BOOL IsKeySelected(int index);
        void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
        void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);
        ParamDimension* GetParamDimension(int i);
        BOOL AssignController(Animatable *control,int subAnim);
        void CopyKeysFromTime(TimeValue src,TimeValue dst,DWORD flags);
        void DeleteKeyAtTime(TimeValue t);
        BOOL IsKeyAtTime(TimeValue t,DWORD flags);
        BOOL GetNextKeyTime(TimeValue t,DWORD flags,TimeValue &nt);
        int GetKeyTimes(Tab<TimeValue> &times,Interval range,DWORD flags);
        int GetKeySelState(BitArray &sel,Interval range,DWORD flags);
        void EditTrackParams(
            TimeValue t,
            ParamDimensionBase *dim,
            TCHAR *pname,
            HWND hParent,
            IObjParam *ip,
            DWORD flags);
        int TrackParamsType() {return TRACKPARAMS_WHOLE;}       
        int SubNumToRefNum(int subNum);
        Interval GetTimeRange(DWORD flags);

        // From ReferenceTarget
        int NumParamBlocks() { return 1; }                  // return number of ParamBlocks in this instance
        IParamBlock2* GetParamBlock(int i) { return pblock; } // return i'th ParamBlock
        IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock

        int NumRefs() {return conts.Count()+3;};    //numControllers+dummyController+controllerInBuffer+pblock
        RefTargetHandle GetReference(int i);
        void SetReference(int i, RefTargetHandle rtarg);        
        RefTargetHandle Clone(RemapDir& remap = DefaultRemapDir());
        RefResult NotifyRefChanged(Interval, RefTargetHandle, PartID&, RefMessage);
        void NotifyForeground(TimeValue t);
        IOResult Save(ISave *isave);
        IOResult Load(ILoad *iload);

    };

#endif
