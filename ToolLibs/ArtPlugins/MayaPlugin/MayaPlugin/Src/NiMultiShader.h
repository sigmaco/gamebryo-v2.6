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

#ifndef NIMULTISHADER_H
#define NIMULTISHADER_H

class NiMultiShader : public MPxNode
{
public:
                      NiMultiShader();
    virtual          ~NiMultiShader();

    virtual MStatus   compute( const MPlug&, MDataBlock& );
    virtual void      postConstructor();

    virtual bool    setInternalValue( const MPlug& kPlug, 
        const MDataHandle& kHandle);


    static  void *    creator();
    static  MStatus   initialize();
    static  MTypeId   id;

    static MObject CreateMaxAnisotropy();
    


//protected:
   static MObject  aResolution;  // Resolution 32,64,128,246
   static MObject  aBaseMapOnly;  // Boolean
   static MObject  aSelectedTexture;  // Enum


   static MObject  aColorR;   // Red component of surface color
   static MObject  aColorG;   // Green component of surface color
   static MObject  aColorB;   // Blue component of surface color
   static MObject  aColor;    // Surface color
   static MObject  aColorL;    // Surface color // OBSOLETE
   static MObject  aColorK;    // Surface color // OBSOLETE
   static MObject  aColorFilter;    // Surface Filter
   static MObject  aColorMaxAnisotropy; // max anisotropy
   static MObject  aColorTransparencyR;   
   static MObject  aColorTransparencyG;   
   static MObject  aColorTransparencyB;   
   static MObject  aColorTransparency;
   static MObject  aColorFlipBook;
   static MObject  aColorFlipBookStartFrame;
   static MObject  aColorFlipBookEndFrame;
   static MObject  aColorFlipBookFramesPerSecond;
   static MObject  aColorFlipBookStartTime;
   static MObject  aColorFlipBookCycleType;
   static MObject  aColorTempUVSet;
   



   static MObject  aDarkMapR;  // Red component of DarkMap
   static MObject  aDarkMapG;  // Green component of DarkMap
   static MObject  aDarkMapB;  // Blue component of DarkMap
   static MObject  aDarkMap;   // DarkMap
   static MObject  aDarkMapL;   // DarkMap      // OBSOLETE
   static MObject  aDarkMapK;   // DarkMap      // OBSOLETE
   static MObject  aDarkMapFilter;    // Surface Filter
   static MObject  aDarkMapMaxAnisotropy; // max anisotropy
   static MObject  aDarkMapFlipBook;
   static MObject  aDarkMapFlipBookStartFrame;
   static MObject  aDarkMapFlipBookEndFrame;
   static MObject  aDarkMapFlipBookFramesPerSecond;
   static MObject  aDarkMapFlipBookStartTime;
   static MObject  aDarkMapFlipBookCycleType;
   static MObject  aDarkMapTempUVSet;

   static MObject  aDetailMapR;  // Red component of DetailMap
   static MObject  aDetailMapG;  // Green component of DetailMap
   static MObject  aDetailMapB;  // Blue component of DetailMap
   static MObject  aDetailMap;   // DetailMap
   static MObject  aDetailMapL;   // DetailMap  // OBSOLETE
   static MObject  aDetailMapK;   // DetailMap  // OBSOLETE
   static MObject  aDetailMapFilter;    // Surface Filter
   static MObject  aDetailMapMaxAnisotropy; // max anisotropy
   static MObject  aDetailMapFlipBook;
   static MObject  aDetailMapFlipBookStartFrame;
   static MObject  aDetailMapFlipBookEndFrame;
   static MObject  aDetailMapFlipBookFramesPerSecond;
   static MObject  aDetailMapFlipBookStartTime;
   static MObject  aDetailMapFlipBookCycleType;
   static MObject  aDetailMapTempUVSet;

   static MObject  aDecal1MapR;  // Red component of Decal1Map
   static MObject  aDecal1MapG;  // Green component of Decal1Map
   static MObject  aDecal1MapB;  // Blue component of Decal1Map
   static MObject  aDecal1Map;   // Decal1Map
   static MObject  aDecal1MapL;   // Decal1Map  // OBSOLETE
   static MObject  aDecal1MapK;   // Decal1Map  // OBSOLETE
   static MObject  aDecal1MapFilter;    // Surface Filter
   static MObject  aDecal1MapMaxAnisotropy; // max anisotropy
   static MObject  aDecal1MapFlipBook;
   static MObject  aDecal1MapFlipBookStartFrame;
   static MObject  aDecal1MapFlipBookEndFrame;
   static MObject  aDecal1MapFlipBookFramesPerSecond;
   static MObject  aDecal1MapFlipBookStartTime;
   static MObject  aDecal1MapFlipBookCycleType;
   static MObject  aDecal1MapTempUVSet;

   static MObject  aDecal2MapR;  // Red component of Decal2Map
   static MObject  aDecal2MapG;  // Green component of Decal2Map
   static MObject  aDecal2MapB;  // Blue component of Decal2Map
   static MObject  aDecal2Map;   // Decal2Map
   static MObject  aDecal2MapL;   // Decal2Map  // OBSOLETE
   static MObject  aDecal2MapK;   // Decal2Map  // OBSOLETE
   static MObject  aDecal2MapFilter;    // Surface Filter
   static MObject  aDecal2MapMaxAnisotropy; // max anisotropy
   static MObject  aDecal2MapFlipBook;
   static MObject  aDecal2MapFlipBookStartFrame;
   static MObject  aDecal2MapFlipBookEndFrame;
   static MObject  aDecal2MapFlipBookFramesPerSecond;
   static MObject  aDecal2MapFlipBookStartTime;
   static MObject  aDecal2MapFlipBookCycleType;
   static MObject  aDecal2MapTempUVSet;

   static MObject  aGlowMapR;  // Red component of GlowMap
   static MObject  aGlowMapG;  // Green component of GlowMap
   static MObject  aGlowMapB;  // Blue component of GlowMap
   static MObject  aGlowMap;   // GlowMap
   static MObject  aGlowMapL;   // GlowMap  // OBSOLETE
   static MObject  aGlowMapK;   // GlowMap  // OBSOLETE
   static MObject  aGlowMapFilter;    // Surface Filter
   static MObject  aGlowMapMaxAnisotropy; // max anisotropy
   static MObject  aGlowMapFlipBook;
   static MObject  aGlowMapFlipBookStartFrame;
   static MObject  aGlowMapFlipBookEndFrame;
   static MObject  aGlowMapFlipBookFramesPerSecond;
   static MObject  aGlowMapFlipBookStartTime;
   static MObject  aGlowMapFlipBookCycleType;
   static MObject  aGlowMapTempUVSet;

   static MObject  aGlossMapR;  // Red component of GlossMap
   static MObject  aGlossMapG;  // Green component of GlossMap
   static MObject  aGlossMapB;  // Blue component of GlossMap
   static MObject  aGlossMap;   // GlossMap
   static MObject  aGlossMapL;   // GlossMap    // OBSOLETE
   static MObject  aGlossMapK;   // GlossMap    // OBSOLETE
   static MObject  aGlossMapFilter;    // Surface Filter
   static MObject  aGlossMapMaxAnisotropy; // max anisotropy
   static MObject  aGlossMapFlipBook;
   static MObject  aGlossMapFlipBookStartFrame;
   static MObject  aGlossMapFlipBookEndFrame;
   static MObject  aGlossMapFlipBookFramesPerSecond;
   static MObject  aGlossMapFlipBookStartTime;
   static MObject  aGlossMapFlipBookCycleType;
   static MObject  aGlossMapTempUVSet;

   static MObject  aBumpMapR;  // Red component of BumpMap
   static MObject  aBumpMapG;  // Green component of BumpMap
   static MObject  aBumpMapB;  // Blue component of BumpMap
   static MObject  aBumpMap;   // BumpMap
   static MObject  aBumpMapL;   // BumpMap  // OBSOLETE
   static MObject  aBumpMapK;   // BumpMap  // OBSOLETE
   static MObject  aBumpMapFilter;    // Surface Filter
   static MObject  aBumpMapMaxAnisotropy; // max anisotropy
   static MObject  aBumpMapLuminaScale;
   static MObject  aBumpMapLuminaOffset;
   static MObject  aBumpMapMat00;
   static MObject  aBumpMapMat01;
   static MObject  aBumpMapMat10;
   static MObject  aBumpMapMat11;
   static MObject  aBumpMapFlipBook;
   static MObject  aBumpMapFlipBookStartFrame;
   static MObject  aBumpMapFlipBookEndFrame;
   static MObject  aBumpMapFlipBookFramesPerSecond;
   static MObject  aBumpMapFlipBookStartTime;
   static MObject  aBumpMapFlipBookCycleType;
   static MObject  aBumpMapTempUVSet;

   static MObject  aNormalMapR;  // Red component of NormalMap
   static MObject  aNormalMapG;  // Green component of NormalMap
   static MObject  aNormalMapB;  // Blue component of NormalMap
   static MObject  aNormalMap;   // NormalMap
   static MObject  aNormalMapFilter;    // Surface Filter
   static MObject  aNormalMapMaxAnisotropy; // max anisotropy
   static MObject  aNormalMapFlipBook;
   static MObject  aNormalMapFlipBookStartFrame;
   static MObject  aNormalMapFlipBookEndFrame;
   static MObject  aNormalMapFlipBookFramesPerSecond;
   static MObject  aNormalMapFlipBookStartTime;
   static MObject  aNormalMapFlipBookCycleType;
   static MObject  aNormalMapTempUVSet;

   static MObject  aParallaxMapR;  // Red component of ParallaxMap
   static MObject  aParallaxMapG;  // Green component of ParallaxMap
   static MObject  aParallaxMapB;  // Blue component of ParallaxMap
   static MObject  aParallaxMap;   // ParallaxMap
   static MObject  aParallaxMapFilter;    // Surface Filter
   static MObject  aParallaxMapMaxAnisotropy; // max anisotropy
   static MObject  aParallaxMapFlipBook;
   static MObject  aParallaxMapFlipBookStartFrame;
   static MObject  aParallaxMapFlipBookEndFrame;
   static MObject  aParallaxMapFlipBookFramesPerSecond;
   static MObject  aParallaxMapFlipBookStartTime;
   static MObject  aParallaxMapFlipBookCycleType;
   static MObject  aParallaxMapTempUVSet;
   static MObject  aParallaxMapOffset;  // Parallax offset

   static MObject  aOutColorR;   // Red component of output color
   static MObject  aOutColorG;   // Green component of output color
   static MObject  aOutColorB;   // Blue component of output color
   static MObject  aOutColor;    // Output color

   static MObject  aInTransR;
   static MObject  aInTransG;
   static MObject  aInTransB;
   static MObject  aInTransparency;

   static MObject  aOutTransparencyR;   
   static MObject  aOutTransparencyG;   
   static MObject  aOutTransparencyB;   
   static MObject  aOutTransparency;

   static MObject  aMaterialDiffuseR;   
   static MObject  aMaterialDiffuseG;   
   static MObject  aMaterialDiffuseB;   
   static MObject  aMaterialDiffuse;    


   static MObject  aMaterialSpecularR;  
   static MObject  aMaterialSpecularG;  
   static MObject  aMaterialSpecularB;  
   static MObject  aMaterialSpecular;   

   static MObject  aMaterialAmbientR;   
   static MObject  aMaterialAmbientG;   
   static MObject  aMaterialAmbientB;   
   static MObject  aMaterialAmbient;    // Material Diffuse color

   static MObject  aMaterialEmittanceR;   
   static MObject  aMaterialEmittanceG;   
   static MObject  aMaterialEmittanceB;   
   static MObject  aMaterialEmittance;    // Material Emittance color

   static MObject  aMaterialShineness;  // Shineness

   static MObject  aShading;  // Flat or Gouraud

   static MObject  aVertexColorOverride;
   static MObject  aVertexColorSrcMode;  
   static MObject  aVertexColorLightMode;  

   static MObject  aTextureApplyMode;  

   static MObject  aTransparencyOverride;  
   static MObject  aTransparencyUseAlphaBlending;
   static MObject  aTransparencySource;  
   static MObject  aTransparencyDestination;  
   static MObject  aTransparencyUseAlphaTesting;
   static MObject  aTransparencyNoTestingSorter;
   static MObject  aTransparencyTestReference;
   static MObject  aTransparencyTestingTestMode;

   static MObject  aPixelShaderName;
   static MObject  aPixelShaderID;
   static MObject  aPixelShaderImplementation;
   static MObject  aPixelShaderExportImplementation;

   static MObject  aNBTMethod;
};


#endif // NIMULTISHADER_H