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


// Declare a global Joint Manager.
kMDtParticleSystemManager gParticleSystemManager;


extern int shader_texture2( MObject alShader, MObject object, MDagPath 
    &dagPath );


//---------------------------------------------------------------------------
kMDtEmitter::kMDtEmitter()
{
    m_lShapeIndex = -1;
    m_szName = NULL;

    m_MObjectEmitter = MObject::kNullObj;
    m_MObjectParent = MObject::kNullObj;
    m_MObjectParticles = MObject::kNullObj;
    m_MObjectEmitterSurface = MObject::kNullObj;

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

    // Basic Emitter Attributes
    m_lEmitterType = -1;
    m_fParticlesPerSecond = -1.0;
    m_bScaleRateByObjectSize = false;
    m_bUsePointPerRates = false;
    m_bNeedParentUV = false;            // (Nurbs Surfaces Only)
    m_lCycleEmissionType = -1;
    m_fCycleInterval = 0.0f;
    m_fDeltaTimeCycle = 0.0f;


    // Distance / Direction Attributes
    m_fMinDistance = 0.0f;
    m_fMaxDistance = 0.0f;
    m_fDirectionX = 0.0f;
    m_fDirectionY = 0.0f;
    m_fDirectionZ = 0.0f;
    m_fSpread = 0.0f;

    // Basic Emission Speed Attributes
    m_fSpeed = 0.0f;
    m_fSpeedRandom = 0.0f;
    m_fTangentSpeed = 0.0f;
    m_fNormalSpeed = 0.0f;

    // Volume Emitter Attributes
    m_lVolumeShapeType = -1;
    m_fVolumeOffsetX = 0.0f;
    m_fVolumeOffsetY = 0.0f;
    m_fVolumeOffsetZ = 0.0f;
    m_fVolumeSweep = 0.0f;
    m_fSectionRadius = 0.0f;

    // Volume Speed Attributes
    m_fAwayFromCenter = 0.0f;
    m_fAwayFromAxis = 0.0f;
    m_fAlongAxis = 0.0f;
    m_fAroundAxis = 0.0f;
    m_fRandomDirection = 0.0f;
    m_fDirectionalSpeed = 0.0f;
    m_bScaleSpeedBySize = false;
    m_bDisplaySpeed = false;

    // Texture Emission Attributes (Nurbs Surfaces Only)
    //          m_ParticleColor;
    m_MObjectTextureName = MObject::kNullObj;
    m_szParticleTextureFileName = NULL;
    m_iTextureID = -1;

    m_fParticleColorR = 0.0f;
    m_fParticleColorG = 0.0f;
    m_fParticleColorB = 0.0f;
    m_bInheritColor = false;
    m_bInheritOpacity = false;
    m_bUseLuminance = false;
    m_bInvertOpacity = false;

    //          m_TextureRate
    m_fTextureRateR = 0.0f;
    m_fTextureRateG = 0.0f;
    m_fTextureRateB = 0.0f;
    m_bEnableTextureRate = false;
    m_bEmitFromDark = false;

}
//---------------------------------------------------------------------------
kMDtEmitter::~kMDtEmitter()
{
    // Free the allocated name
    if(m_szName)
    {
        NiFree(m_szName);
        m_szName = NULL;
    }

    // Free the allocated file name
    if(m_szParticleTextureFileName)
    {
        NiFree(m_szParticleTextureFileName);
        m_szParticleTextureFileName = NULL;
    }
}
//---------------------------------------------------------------------------
bool kMDtEmitter::Load(MObject PSystem, MObject Parent)
{
    MStatus stat = MS::kSuccess;
    MFnDependencyNode dgNode;
    stat = dgNode.setObject(PSystem);

    m_MObjectEmitter = PSystem;
    m_MObjectParent = Parent;

    int kValue;


    // Create the Name buffer
    size_t stNameLength = strlen( dgNode.name().asChar() ) + 1;

    if (m_szName)
    {
        NiFree(m_szName);
    }

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

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("emitterType");


    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( kValue ) == MS::kSuccess )
        m_lEmitterType = kValue;
    else
        printf("Error Reading EmitterType\n");;

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("rate");

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fParticlesPerSecond ) != MS::kSuccess )
        printf("Error Reading rate\n");

    // Check for a negative Rate
    if (m_fParticlesPerSecond < 0.0f)
        m_fParticlesPerSecond = 0.0f;

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("scaleRateByObjectSize");

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bScaleRateByObjectSize ) != MS::kSuccess )
        printf("Error Reading scaleRateByObjectSize\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("useRatePP");

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bUsePointPerRates ) != MS::kSuccess )
        printf("Error Reading useRatePP\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("needParentUV");

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bNeedParentUV ) != MS::kSuccess )
        printf("Error Reading needParentUV\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("cycleEmission");

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( kValue ) == MS::kSuccess )
        m_lCycleEmissionType = kValue;
    else
        printf("Error Reading cycle Emissions\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("cycleInterval");

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fCycleInterval ) != MS::kSuccess )
        printf("Error Reading cycle Interval\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("deltaTimeCycle");

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fDeltaTimeCycle ) != MS::kSuccess )
        printf("Error Reading deltaTimeCycle\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("maxDistance");

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fMaxDistance ) != MS::kSuccess )
        printf("Error Reading max Distance\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("minDistance");

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fMinDistance ) != MS::kSuccess )
        printf("Error Reading minDistance\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("direction");

    NIASSERT( !Plug.isNull() );
//  if( Plug.getValue( m_ ) != MS::kSuccess )
//      printf("Error Reading direction\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("directionX");

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fDirectionX ) != MS::kSuccess )
        printf("Error Reading directionX\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("directionY");

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fDirectionY ) != MS::kSuccess )
        printf("Error Reading directionY\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("directionZ");

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fDirectionZ ) != MS::kSuccess )
        printf("Error Reading directionZ\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("spread");

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fSpread ) != MS::kSuccess )
        printf("Error Reading spread\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("speed");

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fSpeed ) != MS::kSuccess )
        printf("Error Reading speed\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("speedRandom");

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fSpeedRandom ) != MS::kSuccess )
        printf("Error Reading speedRandom\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("tangentSpeed");

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fTangentSpeed ) != MS::kSuccess )
        printf("Error Reading tangentSpeed\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("normalSpeed");

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fNormalSpeed ) != MS::kSuccess )
        printf("Error Reading normalSpeed\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("enableTextureRate");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bEnableTextureRate ) != MS::kSuccess )
        printf("Error Reading enableTextureRate\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("textureRate");  

    NIASSERT( !Plug.isNull() );
//  if( Plug.getValue( m_ ) != MS::kSuccess )
//      printf("Error Reading textureRate\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("textureRateR");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fTextureRateR ) != MS::kSuccess )
        printf("Error Reading textureRateR\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("textureRateG");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fTextureRateG ) != MS::kSuccess )
        printf("Error Reading textureRateG\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("textureRateB");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fTextureRateB ) != MS::kSuccess )
        printf("Error Reading texturRateB\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("emitFromDark");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bEmitFromDark ) != MS::kSuccess )
        printf("Error Reading emitFromDark\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("inheritColor");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bInheritColor ) != MS::kSuccess )
        printf("Error Reading inheritColor\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("inheritOpacity");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bInheritOpacity ) != MS::kSuccess )
        printf("Error Reading inheritOpacity\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("invertOpacity");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bInvertOpacity ) != MS::kSuccess )
        printf("Error Reading inveritOpacity\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("useLuminance");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bUseLuminance ) != MS::kSuccess )
        printf("Error Reading useLuminance\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("particleColor");

    NIASSERT(!Plug.isNull());

    MPlugArray ParticleColorConnections;

    Plug.connectedTo(ParticleColorConnections, true, false, &stat);
    NIASSERT(stat == MS::kSuccess);

    if(ParticleColorConnections.length() == 1)
    {
        // FIND THE TEXTURE WE ARE CONNECTED TO 
        MPlug IsFullPlug = ParticleColorConnections[0];
        m_MObjectTextureName = IsFullPlug.node(&stat);


        MFnDependencyNode dgTextureNode;
        dgTextureNode.setObject(m_MObjectTextureName);

        MPlug TextureFileName = dgTextureNode.findPlug("fileTextureName", 
            &stat);

        if(stat != MS::kSuccess)
        {
            printf("Error Reading fileTextureName for Emitter: %s  Only TGA "
                "file textures are supported.\n",m_szName);
            return false;
        }
        else
        {
            MString StringFileName;
            stat = TextureFileName.getValue(StringFileName);

            NIASSERT(stat == MS::kSuccess);
    
            size_t stLen = strlen(StringFileName.asChar()) + 1;
            m_szParticleTextureFileName = NiAlloc(char, stLen);
            NiStrcpy(m_szParticleTextureFileName, stLen, 
                StringFileName.asChar());
        }
    }


    // ----------------------------------------------------------
    Plug = dgNode.findPlug("particleColorR");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fParticleColorR ) != MS::kSuccess )
        printf("Error Reading particleColorR\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("particleColorG");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fParticleColorG ) != MS::kSuccess )
        printf("Error Reading particleColorG\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("particleColorB");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fParticleColorB ) != MS::kSuccess )
        printf("Error Reading particleColorB\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("volumeShape");

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( kValue ) == MS::kSuccess )
        m_lVolumeShapeType = kValue;
    else
        printf("Error Reading volumeShape\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("volumeOffset");

    NIASSERT( !Plug.isNull() );
//  if( Plug.getValue( m_ ) != MS::kSuccess )
//      printf("Error Reading volumeOffset\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("volumeOffsetX");    

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fVolumeOffsetX ) != MS::kSuccess )
        printf("Error Reading volumeOffsetX\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("volumeOffsetY");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fVolumeOffsetY ) != MS::kSuccess )
        printf("Error Reading volumeOffsetY\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("volumeOffsetZ");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fVolumeOffsetZ ) != MS::kSuccess )
        printf("Error Reading volumenOffsetZ\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("volumeEfficiency");  

    NIASSERT( !Plug.isNull() );
//  if( Plug.getValue( m_ ) != MS::kSuccess )
//      printf("Error Reading volumeEfficiency\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("volumeSweep");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fVolumeSweep ) != MS::kSuccess )
        printf("Error Reading volumeSweep\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("sectionRadius");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fSectionRadius ) != MS::kSuccess )
        printf("Error Reading sectionRadius\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("awayFromCenter");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fAwayFromCenter ) != MS::kSuccess )
        printf("Error Reading awayFromCenter\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("awayFromAxis");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fAwayFromAxis ) != MS::kSuccess )
        printf("Error Reading awayFromAxis\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("alongAxis");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fAlongAxis ) != MS::kSuccess )
        printf("Error Reading alongAxis\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("aroundAxis");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fAroundAxis ) != MS::kSuccess )
        printf("Error Reading aroundAxis\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("randomDirection");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fRandomDirection ) != MS::kSuccess )
        printf("Error Reading randomDirection\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("directionalSpeed");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fDirectionalSpeed ) != MS::kSuccess )
        printf("Error Reading directionalSpeed\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("scaleSpeedBySize");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bScaleSpeedBySize ) != MS::kSuccess )
        printf("Error Reading scaleSpeedBySize\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("displaySpeed");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bDisplaySpeed ) != MS::kSuccess )
        printf("Error Reading displaySpeed\n");



    // FIND WHAT PARTICLES WE ARE CONNECTED TO 
    char szBuffer[256] = "output";

    Plug = dgNode.findPlug(szBuffer);

    NIASSERT(!Plug.isNull());

    MPlug FirstElementPlug = Plug.elementByLogicalIndex(0,&stat);
    NIASSERT(stat == MS::kSuccess);

    MPlugArray Connections;

    FirstElementPlug.connectedTo(Connections, false, true, &stat);
    NIASSERT(stat == MS::kSuccess);

    unsigned int uiConnections = Connections.length();
    if(uiConnections == 1)
    {
        MPlug IsFullPlug = Connections[0];
        m_MObjectParticles = IsFullPlug.node(&stat);
    }

    // Check to see if the emitter is connected to a surface object
    MObject MGeoConnector = FollowInputAttribute(m_MObjectEmitter, 
        MString("sweptGeometry"));

    if (MGeoConnector != MObject::kNullObj)
    {
        m_MObjectEmitterSurface = FollowInputAttribute(MGeoConnector, 
            MString("localGeometry"));

        if (m_MObjectEmitterSurface != MObject::kNullObj)
        {
            MFnDependencyNode kNode(m_MObjectEmitterSurface);

// Debugging
//            char szBuffer[256];
//            strcpy(szBuffer, kNode.typeName().asChar());
//
//            strcpy(szBuffer, kNode.name().asChar());
        }
    }
    return true;
}



//---------------------------------------------------------------------------
//
//   PARTICLES
//
//---------------------------------------------------------------------------

kMDtParticles::kMDtParticles()
{
    m_szName = NULL;

    m_lRenderType = 0;;
    m_bVisibleInReflections = false;
    m_bVisibleInRefractions = false;
    m_bCastsShadows = false;
    m_bPrimaryVisibility = false;
    m_lLifespanMode = 0;
    m_fLifespanRandom = 0.0;
    m_fGeneralSeed = 0.0;
    m_bIsDynamic = false;
    m_fDynamicsWeight = 0.0f;
    m_bForcesInWorld = false;
    m_fConserve = 0.0f;
    m_bEmissionInWorld = false;
    m_fMaxCount = 0.0f;
    m_fLevelOfDetail = 0.0f;
    m_fInheritFactor = 0.0f;
    m_fSeed = 0.0f;
    m_fCurrentTime = 0.0f;
    m_fStartFrame = 0.0f;
    m_fStartTime = 0.0f;
    m_fGoalSmoothness = 0.0f;
    m_bCollisions = false;
    m_fTraceDepth = 0.0f;
    m_bDepthSort = false;
    m_lParticleRenderType = 1;
    m_fLifespanPP = 0.0f;
    m_fLifespanPP0 = 0.0f;
    m_fLifespan = 0.0f;

        // Points Particle Render Type
    m_bColorAccum = false;
    m_bUseLighting = false;
    m_lPointSize = 0;
    m_lNormalDir = 0;

        // Sprites Particle Render Type
    m_fSpriteTwist = 0.0f;
    m_fSpriteScaleX = 0.0f;
    m_fSpriteScaleY = 0.0f;
    m_fSpriteNum = 0.0f;

    m_iMaterialID = -1;
    m_iTextureID = -1;


}
//---------------------------------------------------------------------------
kMDtParticles::~kMDtParticles()
{
    if(m_szName != NULL)
    {
        NiFree( m_szName);
        m_szName = NULL;
    }
}
//---------------------------------------------------------------------------
bool kMDtParticles::Load(MObject PSystem, MObject Parent, MDagPath shapePath)
{

    int kValue;


    m_MObjectParticles = PSystem;
    m_MObjectParent = Parent;
    MStatus stat = MS::kSuccess;
    MFnDependencyNode dgNode;
    stat = dgNode.setObject(PSystem);

    m_ShapePath = shapePath;

    // Create the Name buffer
    size_t stNameLength = strlen( dgNode.name().asChar() ) + 1;

    if (m_szName)
    {
        NiFree(m_szName);
    }

    m_szName = NiAlloc(char, stNameLength);
    
    NiStrcpy( m_szName, stNameLength, dgNode.name().asChar() );

    // ----------------------------------------------------------
    MPlug Plug = dgNode.findPlug("renderType");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( kValue ) == MS::kSuccess )
        m_lRenderType = kValue;
    else
        printf("Error Reading renderType\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("visibleInReflections");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bVisibleInReflections ) != MS::kSuccess )
        printf("Error Reading visibleInReflections\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("visibleInRefractions");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bVisibleInRefractions ) != MS::kSuccess )
        printf("Error Reading visibleInRefractions\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("castsShadows");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bCastsShadows ) != MS::kSuccess )
        printf("Error Reading castsShadows\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("primaryVisibility");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bPrimaryVisibility ) != MS::kSuccess )
        printf("Error Reading primaryVisibility\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("lifespanMode");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( kValue ) == MS::kSuccess )
        m_lLifespanMode = kValue;
    else
        printf("Error Reading lifespanMode\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("lifespanRandom");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fLifespanRandom ) != MS::kSuccess )
        printf("Error Reading lifespanRandom\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("generalSeed");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fGeneralSeed ) != MS::kSuccess )
        printf("Error Reading generalSeed\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("isDynamic");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bIsDynamic ) != MS::kSuccess )
        printf("Error Reading isDynamic\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("dynamicsWeight");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fDynamicsWeight ) != MS::kSuccess )
        printf("Error Reading dynamicsWeight\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("forcesInWorld");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bForcesInWorld ) != MS::kSuccess )
        printf("Error Reading forcesInWorld\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("conserve");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fConserve ) != MS::kSuccess )
        printf("Error Reading conserve\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("emissionInWorld");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bEmissionInWorld ) != MS::kSuccess )
        printf("Error Reading emmisionInWorld\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("maxCount");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fMaxCount ) != MS::kSuccess )
        printf("Error Reading maxCount\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("levelOfDetail");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fLevelOfDetail ) != MS::kSuccess )
        printf("Error Reading levelOfDetail\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("inheritFactor");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fInheritFactor ) != MS::kSuccess )
        printf("Error Reading inheritFactor\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("seed");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fSeed ) != MS::kSuccess )
        printf("Error Reading seed\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("currentTime");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fCurrentTime ) != MS::kSuccess )
        printf("Error Reading currentTime\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("startFrame");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fStartFrame ) != MS::kSuccess )
        printf("Error Reading startFrame\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("startTime");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fStartTime ) != MS::kSuccess )
        printf("Error Reading startTime\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("goalSmoothness");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fGoalSmoothness ) != MS::kSuccess )
        printf("Error Reading goalSmoothness\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("collisions");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bCollisions ) != MS::kSuccess )
        printf("Error Reading collisions\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("traceDepth");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fTraceDepth ) != MS::kSuccess )
        printf("Error Reading traceDepth\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("depthSort");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_bDepthSort ) != MS::kSuccess )
        printf("Error Reading depthSort\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("particleRenderType");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( kValue ) == MS::kSuccess )
        m_lParticleRenderType = kValue;
    else
        printf("Error Reading particleRenderType\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("lifespanPP");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fLifespanPP ) != MS::kSuccess )
        printf("Error Reading lifespanPP\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("lifespanPP0");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fLifespanPP0 ) != MS::kSuccess )
        printf("Error Reading lifespanPP0\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("lifespan");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fLifespan ) != MS::kSuccess )
        printf("Error Reading lifespan\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("colorAccum", &stat);  
    
    if(stat == MS::kSuccess)
    {
        NIASSERT( !Plug.isNull() );
        if( Plug.getValue( m_bColorAccum ) != MS::kSuccess )
            printf("Error Reading colorAccum\n");
    }
    
    // ----------------------------------------------------------
    Plug = dgNode.findPlug("useLighting", &stat);  
    
    if(stat == MS::kSuccess)
    {
        NIASSERT( !Plug.isNull() );
        if( Plug.getValue( m_bUseLighting ) != MS::kSuccess )
            printf("Error Reading useLighting\n");
    }
    
    // ----------------------------------------------------------
    Plug = dgNode.findPlug("pointSize", &stat);  
    
    if(stat == MS::kSuccess)
    {
        NIASSERT( !Plug.isNull() );
        if( Plug.getValue( m_lPointSize ) != MS::kSuccess )
            printf("Error Reading pointSize\n");
    }
    
    // ----------------------------------------------------------
    Plug = dgNode.findPlug("spriteTwist", &stat);  
    
    if(stat == MS::kSuccess)
    {
        NIASSERT( !Plug.isNull() );
        if( Plug.getValue( m_fSpriteTwist ) != MS::kSuccess )
            printf("Error Reading spriteTwist\n");
    }
    
    // ----------------------------------------------------------
    Plug = dgNode.findPlug("spriteScaleX", &stat);  
    
    if(stat == MS::kSuccess)
    {
        NIASSERT( !Plug.isNull() );
        if( Plug.getValue( m_fSpriteScaleX ) != MS::kSuccess )
            printf("Error Reading spriteScaleX\n");
    }
    
    // ----------------------------------------------------------
    Plug = dgNode.findPlug("spriteScaleY", &stat);  
    
    if(stat == MS::kSuccess)
    {
        NIASSERT( !Plug.isNull() );
        if( Plug.getValue( m_fSpriteScaleY ) != MS::kSuccess )
            printf("Error Reading \n");
    }
    
    // ----------------------------------------------------------
    Plug = dgNode.findPlug("spriteNum", &stat);  
    
    if(stat == MS::kSuccess)
    {
        NIASSERT( !Plug.isNull() );
        if( Plug.getValue( m_fSpriteNum ) != MS::kSuccess )
            printf("Error Reading spriteNum\n");
    }

    // ----------------------------------------------------------
    // FIND WHAT EMITTER WE ARE CONNECTED TO 
    Plug = dgNode.findPlug("isFull");

    MPlugArray Connections;
    Plug.connectedTo(Connections, false, true, &stat);
    if (Connections.length() != 1)
        return false;

    MPlug IsFullPlug = Connections[0];
    m_MObjectEmitter = IsFullPlug.node(&stat);

    // Set the Shader for this Particles
    SetShader();

    return true;

}
//---------------------------------------------------------------------------
int kMDtParticles::GetNumFields()
{
    MStatus stat = MS::kSuccess;
    MFnDependencyNode dgNode(m_MObjectParticles,&stat);

    NIASSERT(stat == MS::kSuccess);

    // Get the input force
    MPlug Plug = dgNode.findPlug("inputForce");  

    NIASSERT( !Plug.isNull() );

    int iNumConnectedElements = Plug.numConnectedElements(&stat);

    return iNumConnectedElements;
}
//---------------------------------------------------------------------------
MObject kMDtParticles::GetField(int iFieldNum)
{
    NIASSERT(iFieldNum < GetNumFields());

    MStatus stat = MS::kSuccess;
    MFnDependencyNode dgNode(m_MObjectParticles,&stat);
    MObject MObjectField;

    NIASSERT(stat == MS::kSuccess);

    // Get the input force
    MPlug Plug = dgNode.findPlug("inputForce");  

    NIASSERT( !Plug.isNull() );

    MPlug ForcePlug = Plug.elementByPhysicalIndex(iFieldNum, &stat);
    NIASSERT(stat == MS::kSuccess);


    MPlugArray ForcePlugConnections;

    ForcePlug.connectedTo(ForcePlugConnections, true, false, &stat);
    NIASSERT(stat == MS::kSuccess);

    if(ForcePlugConnections.length() == 1)
    {
        // FIND THE TEXTURE WE ARE CONNECTED TO 
        MPlug FieldPlug = ForcePlugConnections[0];
        MObjectField = FieldPlug.node(&stat);

        NIASSERT(stat == MS::kSuccess);
        return MObjectField;
    }


    return MObject::kNullObj;
}
//---------------------------------------------------------------------------
int kMDtParticles::GetNumColliders()
{

    MStatus stat = MS::kSuccess;


    MFnDependencyNode StartNode(m_MObjectParticles, &stat);
    NIASSERT(stat == MS::kSuccess);

    
    // Get the input force
    MPlug Plug = StartNode.findPlug("collisionGeometry");  
    if( Plug.isNull() )
    {
        return 0;
    }

    return Plug.numConnectedElements(&stat);

}
//---------------------------------------------------------------------------
void kMDtParticles::SetShader()
{
    MStatus kStatus;

    // Get the Particles Node
    MFnDependencyNode kParticlesNode(m_MObjectParticles, &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    // Find the Shaders plug
    MPlug kShaderPlug = kParticlesNode.findPlug("instObjGroups", &kStatus);
    NIASSERT(kStatus);

    // We should only be connected to one shader set because this is a 
    // particle system
    if(kShaderPlug.numElements(&kStatus) != 1)
        return;
    NIASSERT(kStatus == MS::kSuccess);

    // That plug should only be connected to one shader Group
    MPlugArray kConnectedPlugs;
    kShaderPlug.elementByPhysicalIndex(0).connectedTo(kConnectedPlugs, true, 
        true, &kStatus);

    NIASSERT(kStatus == MS::kSuccess);
    
    if(kConnectedPlugs.length() != 1)
        return;

    // Follow the Shader Group to the Surface Shader
    MObject kShaderGroupObj = kConnectedPlugs[0].node(&kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    MFnDependencyNode kShaderGroup(kShaderGroupObj, &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    MPlug kSurfaceShaderPlug = kShaderGroup.findPlug("surfaceShader", 
        &kStatus);

    NIASSERT(kStatus == MS::kSuccess);

    // Follow the surfaceShader Plug to the Shader
    kSurfaceShaderPlug.connectedTo(kConnectedPlugs, true, true, &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    if(kConnectedPlugs.length() != 1)
        return;

    m_MObjectShader = kConnectedPlugs[0].node(&kStatus);;
    NIASSERT(kStatus == MS::kSuccess);

    MFnDependencyNode kShaderNode(m_MObjectShader, &kStatus);
    NIASSERT(kStatus == MS::kSuccess);
   
}
//---------------------------------------------------------------------------
void kMDtParticles::SetMaterialID()
{
    DtExt_MtlGetIDByShader(m_MObjectShader, m_iMaterialID);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//   PARTICLE SYSTEM MANAGER
//
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
kMDtParticleSystemManager::kMDtParticleSystemManager()
{
    m_iNumEmitters = 0;
    m_ppEmitters = NULL;

    m_iNumParticles = 0;
    m_ppParticles = NULL;

}
//---------------------------------------------------------------------------
kMDtParticleSystemManager::~kMDtParticleSystemManager()
{
    Reset();
}
//---------------------------------------------------------------------------
void kMDtParticleSystemManager::ResetEmitters()
{
    if( !m_ppEmitters )
        return;
    
    unsigned int uiLoop;
    for (uiLoop = 0; uiLoop < (unsigned int)m_iNumEmitters; uiLoop++)
    {
        NiExternalDelete m_ppEmitters[uiLoop];
        m_ppEmitters[uiLoop] = NULL;
    }

    NiFree(m_ppEmitters);
    m_ppEmitters = NULL;

    m_iNumEmitters = 0;
}
//---------------------------------------------------------------------------
void kMDtParticleSystemManager::ResetParticles()
{
    if( !m_ppParticles )
        return;
    
    unsigned int uiLoop;
    for (uiLoop = 0; uiLoop < (unsigned int)m_iNumParticles; uiLoop++)
    {
        NiExternalDelete m_ppParticles[uiLoop];
        m_ppParticles[uiLoop] = NULL;
    }

    NiFree(m_ppParticles);
    m_ppParticles = NULL;

    m_iNumParticles = 0;
}
//---------------------------------------------------------------------------

void kMDtParticleSystemManager::Reset()
{
    ResetEmitters();
    ResetParticles();   
}
//---------------------------------------------------------------------------
int kMDtParticleSystemManager::GetNumEmitters()
{
    return m_iNumEmitters;
}
//---------------------------------------------------------------------------
int kMDtParticleSystemManager::AddEmitter( kMDtEmitter* pNewEmitter)
{
    // INCREASE THE SIZE OF THE ARRAY
    kMDtEmitter** ppExpandedArray = NiAlloc(kMDtEmitter*, m_iNumEmitters+1);

    // Copy over the old array;
    for(int iLoop = 0; iLoop < m_iNumEmitters; iLoop++)
    {
        ppExpandedArray[iLoop] = m_ppEmitters[iLoop];
    }

    // Add the new particle System
    ppExpandedArray[m_iNumEmitters] = pNewEmitter;

    // DELETE THE OLD ARRAY
    NiFree(m_ppEmitters);
    m_ppEmitters = NULL;

    // Assign the newly created array
    m_ppEmitters = ppExpandedArray;

    return m_iNumEmitters++;
}
//---------------------------------------------------------------------------
kMDtEmitter* kMDtParticleSystemManager::GetEmitter( int iIndex )
{
    NIASSERT(iIndex >= 0);
    NIASSERT(iIndex < m_iNumEmitters);

    return m_ppEmitters[iIndex];
}
//---------------------------------------------------------------------------
int kMDtParticleSystemManager::GetEmitter( MObject mobj )
{
    // Scan the Array of Particle Systems looking for the match
    for(int iLoop = 0; iLoop < m_iNumEmitters; iLoop++)
    {
        if(m_ppEmitters[iLoop]->m_MObjectEmitter == mobj)
            return iLoop;
    }

    return -1;
}
//---------------------------------------------------------------------------
int kMDtParticleSystemManager::GetNumParticles()
{
    return m_iNumParticles;
}
//---------------------------------------------------------------------------
int kMDtParticleSystemManager::AddParticles( kMDtParticles* pNewParticles)
{
    // INCREASE THE SIZE OF THE ARRAY
    kMDtParticles** ppExpandedArray = NiAlloc(kMDtParticles*, 
        m_iNumParticles + 1);

    // Copy over the old array;
    for(int iLoop = 0; iLoop < m_iNumParticles; iLoop++)
    {
        ppExpandedArray[iLoop] = m_ppParticles[iLoop];
    }

    // Add the new particle System
    ppExpandedArray[m_iNumParticles] = pNewParticles;

    // DELETE THE OLD ARRAY
    NiFree(m_ppParticles);

    // Assign the newly created array
    m_ppParticles = ppExpandedArray;

    return m_iNumParticles++;
}
//---------------------------------------------------------------------------
kMDtParticles* kMDtParticleSystemManager::GetParticles( int iIndex )
{
    NIASSERT(iIndex >= 0);
    NIASSERT(iIndex < m_iNumParticles);

    return m_ppParticles[iIndex];
}
//---------------------------------------------------------------------------
int kMDtParticleSystemManager::GetParticles( MObject mobj )
{
    // Scan the Array of Particle Systems looking for the match
    for(int iLoop = 0; iLoop < m_iNumParticles; iLoop++)
    {
        if(m_ppParticles[iLoop]->m_MObjectParticles == mobj)
            return iLoop;
    }

    return -1;
}
//---------------------------------------------------------------------------
void kMDtParticleSystemManager::SetParticleMaterials()
{
    // Set the Shader for each particles
    for(int iLoop = 0; iLoop < m_iNumParticles; iLoop++)
    {
        m_ppParticles[iLoop]->SetMaterialID();
    }
}

//---------------------------------------------------------------------------
//
//  BASIC ADDING FUNCTIONS
//
//---------------------------------------------------------------------------
int addTransformEmitter(MObject transformNode, MObject shapeNode)
{

    kMDtEmitter* pNewPSystem = NiExternalNew kMDtEmitter();

    if(!pNewPSystem->Load(shapeNode, transformNode))
    {
        NiExternalDelete pNewPSystem;
        return -1;
    }

    return gParticleSystemManager.AddEmitter(pNewPSystem);
}
//---------------------------------------------------------------------------
int addTransformParticles(MObject transformNode, MObject shapeNode, 
        MDagPath shapePath)
{

    kMDtParticles* pNewParticles = NiExternalNew kMDtParticles();

    if(!pNewParticles->Load(shapeNode, transformNode, shapePath))
    {
        NiExternalDelete pNewParticles;
        return -1;
    }

    return gParticleSystemManager.AddParticles(pNewParticles);
}
//---------------------------------------------------------------------------
void gParticleSystemNew()
{
    // INITIALIZE AT THE START OF THE EXPORTER

    gParticleSystemManager.Reset();
}
//---------------------------------------------------------------------------
int DtExt_EmitterGetShapeNode(int emitterID, MObject &obj )
{
    kMDtEmitter* pkEmitter = gParticleSystemManager.GetEmitter(emitterID);

    if(pkEmitter == NULL)
        return 0;

    obj = pkEmitter->m_MObjectEmitter;
    return 1;
}
//---------------------------------------------------------------------------
int DtExt_ParticlesGetTransform(int ParticlesID, MObject &obj )
{
    kMDtParticles* pkParticles = 
        gParticleSystemManager.GetParticles(ParticlesID);

    if(pkParticles == NULL)
        return 0;

    obj = pkParticles->m_MObjectParent;
    return 1;
}
//---------------------------------------------------------------------------
int DtExt_ParticlesGetShapeNode(int ParticlesID, MObject &obj )
{
    kMDtParticles* pkParticles = 
        gParticleSystemManager.GetParticles(ParticlesID);

    if(pkParticles == NULL)
        return 0;

    obj = pkParticles->m_MObjectParticles;
    return 1;
}
