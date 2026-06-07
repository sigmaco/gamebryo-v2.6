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
#include "GetPackingProfiles.h"
#include <NiMorphMeshModifier.h>
#include <NiSkinningMeshModifier.h>
#include <NiToolDataStream.h>
#include <NiMeshProfileProcessor.h>
#include <NiMeshTools.h>

#include <errno.h>

#if (REQUIRE_IOSTREAM == 1)
#include <iostream>
using namespace std;
#endif

const char* MyiTriMesh::UV_SET_PREFIX = "gb_uv_set";

// All the flags here must get changed to their correct versions
static const NiUInt8 ACCESS_FLAGS = NiDataStream::ACCESS_CPU_WRITE_STATIC | 
    NiDataStream::ACCESS_GPU_READ;


//---------------------------------------------------------------------------
MyiTriMesh::MyiTriMesh( int iShapeNum, int iGroupID,
    MyiMaterialManager* pkMaterialManager,
    MyiTextureManager* pkTextureManager)
{
    char szName[NI_MAX_MAYA_OBJECT_NAME_LENGTH] = "";

    m_bHasAlpha = false;
    m_bHasVertexColors = false;

    // Various Texture Groups - Ensure we get object in it's entirety.
    // Grouping breaks apart a shape by differences in texturing,
    // materials, color, etc.

    // NOTE:    NEED TO LOOK AT HOW GROUPING WORKS FROM WITHIN MAYA
    //          THERE ARE ASSUMPTIONS BEING MADE - NAMELY THAT EACH
    //          SHAPE HAS AT LEAST ONE GROUP WHICH ISN'T APPARENTLY
    //          ALWAYS TRUE.

    // UPDATE: If the shape is a pure transformation/joint node then
    // the number of groups will be Zero (for instance, no shader assignment
    // This usually only occurs when the hierarchy is set to FULL, or all of
    // the transformations are flattened out, and geometry is associated with 
    // all of them.  STILL NEED TO LOOK INTO THIS TO ENSURE GROUP != 0

    // Currently - we offer support only for 1 group
    int iGroupCnt = DtGroupGetCount(iShapeNum);

    if (iGroupID < 0)
    {
        const char *szShapeName;
        DtShapeGetShapeName(iShapeNum, &szShapeName);
        DtExt_Err("ERROR: Object [%s] has abnormal Geometry.\n",
            szShapeName);

        SetStatus(MYIOBJ_SUCCESS);
        return;
    }

    // GET THE NAME OF THE SHAPE
    const char *szShapeName;
    DtShapeGetShapeName(iShapeNum, &szShapeName);

    NIASSERT(m_spMesh == NULL);

    int iInstance = DtExt_ShapeIsInstanced(iShapeNum);

    if (iInstance >= 0)
    {
        NIASSERT(iInstance < iShapeNum);
        if (iInstance < iShapeNum)
        {
            // This Shape is a clone of another.
            // Should have already been processed, but assert anyway.
            // Let's take a look at the shape to which this is a clone of
            // and actually "clone" it.
            int iComponentNumForInstance = gMDtObjectFindComponentID(
                ETypeShape, iInstance);

            NiNode* pMasterNode = 
                gUserData.GetNode(iComponentNumForInstance);

            NIASSERT(pMasterNode);

            // Get the children 
            // Possibly more than one if mesh had multiple "groups" of
            // textures,materials, etc.
            int iNumChildren = pMasterNode->GetArrayCount();

            NiAVObject* pAVObject = NULL;

// Get appropriate child
// This is proving difficult because an object with multiple material groups
// may not match one-to-one with the children... an object may have a child
// naturally in hierarchy.  My assumption here (keep an eye on this) is that
// we create the separate mesh for each group last - so they will be
// the last children and they will correspond to each group.
// If this is the case, then we just take the number of children, subtract
// of the number of "groups", and then add the current group number.

            // Find the matching name...
            char szSearchString[NI_MAX_MAYA_OBJECT_NAME_LENGTH] = "";
            NiStrcpy(szSearchString, NI_MAX_MAYA_OBJECT_NAME_LENGTH,
                szShapeName);

            if (iGroupCnt>1)
            {
// If this object has multiple groups - identify each uniquely
                char szBuffer1[16];
                char szBuffer2[16];
#if defined(_MSC_VER) && _MSC_VER >= 1400
                _itoa_s(iGroupID,szBuffer1,16,10);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
                _itoa(iGroupID,szBuffer1,10);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
                NiSprintf(szBuffer2, 16, ":%s", szBuffer1 );
                NiStrcat( szSearchString, NI_MAX_MAYA_OBJECT_NAME_LENGTH,
                    szBuffer2 );
            }

            for(int i = iNumChildren - 1; i >= 0; i--)
            {
                pAVObject = pMasterNode->GetAt(i);

                if (pAVObject && 
                    strstr(pAVObject->GetName(),szSearchString)!=NULL)
                    break;

                pAVObject = NULL;
            }

            NIASSERT(pAVObject);
            NIASSERT(NiIsKindOf(NiMesh, pAVObject));

            NiMesh *pkMesh = (NiMesh*)pAVObject;

            NiObject* pObj = pkMesh->Clone();
            NIASSERT(NiIsKindOf(NiMesh, pObj));
            m_spMesh = (NiMesh *)pObj;

            // Remove Shader any shader that might be on this object
            m_spMesh->SetShader(NULL);

            // convert scale for the instance
            m_fScale[0] = m_fScale[1] = m_fScale[2] = 1.0f;
            if (!DtShapeGetFinalNonUniformNonAnimatedScale(iShapeNum, 
                &m_fScale[0], &m_fScale[1], &m_fScale[2]))
            {
                // Error From DtShapeGetFinalNonUniformNonAnimatedScale
                m_fScale[0] = m_fScale[1] = m_fScale[2] = 1.0f;
            }

            float fEpsilon = m_fScale[0] * 0.001f;

            if( !NiOptimize::CloseTo(m_fScale[0], m_fScale[1], 
                    fEpsilon) || 
                !NiOptimize::CloseTo(m_fScale[0], m_fScale[2], 
                    fEpsilon) )
            {
                DtExt_Err("WARNING: Instanced Object [%s] " \
                    "Can not have non-uniform scale.\n",
                    szShapeName);
            }

            m_spMesh->SetScale (m_fScale[0]);

            // if we have cloned the base object, none of the rest
            // of the conversion process is necessary
            SetStatus(MYIOBJ_SUCCESS);
            return;
        }
    }

    NiTexturingProperty *pTexturingProperty = NULL;
    
    unsigned int* puiNewToOld = 0;

    // Check that an instance wasn't made...
    if (m_spMesh == NULL)
    {
        // Create the mesh if an instance was not made.
        m_spMesh = NiNew NiMesh();
        m_spMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRIANGLES);

        // Collect info from MDtAPI and store relavant data
        // in our Ni member variables.
        if (!GetShapeAttributesForNi(iShapeNum, iGroupID))
        {
            const char *szShapeName;
            DtShapeGetShapeName(iShapeNum, &szShapeName);
            DtExt_Err("ERROR: Object [%s] has no Shape Attributes and can " \
                "not be exported.\n", szShapeName);
            SetStatus(MYIOBJ_ERROR);
            return;
        }

        // Create the Texturing Property
        CreateMultiTexture( iShapeNum, iGroupID, pkTextureManager, 
            pTexturingProperty, m_bHasAlpha);

#ifdef SHOW_OBJECT_TYPE_IN_NAME
        NiSprintf(szName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, 
            "Mesh %d: ", iShapeNum);
#endif


        // Add User Defined Properties for the Shape
        if(m_spMesh != NULL)
        {
            // Get the Object and dgNode
            MObject mObj = MObject::kNullObj;
            DtExt_ShapeGetShapeNode(iShapeNum, mObj);
            
            MFnDagNode dgNode;
            dgNode.setObject(mObj);
            
            // Attach All of the Attributes to the Mesh
            AttachUserDefinedAttributes(dgNode, m_spMesh);

            // Attach any No Stripify Options
            AddNoStripifyAttributes(dgNode);
            
            // Attach physics if necessary
            MyiPhysX::ProcessMesh(m_spMesh, iShapeNum, iGroupID);
        }

    }

        // ERROR CHECK
    if (m_spMesh == NULL)
    {
        DtExt_Err("ERROR: Failed to create Mesh: %s\n", szShapeName);
        SetStatus(MYIOBJ_ERROR);
        return;
    }

    // ADD ON THE SHAPE NAME
    NiStrcat(szName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, szShapeName);

    if (iGroupCnt > 1)
    {
        // If this object has multiple groups - identify each uniquely
        char szBuffer1[16];
        char szBuffer2[16];
#if defined(_MSC_VER) && _MSC_VER >= 1400
        _itoa_s(iGroupID,szBuffer1,16,10);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
        _itoa(iGroupID,szBuffer1,10);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
        NiSprintf(szBuffer2, 16, ":%s", szBuffer1);
        NiStrcat( szName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, szBuffer2 );
    }

    // SAVE THE SHAPE NAME
    m_spMesh->SetName(szName);


    // Create the Material Properties
    if (!GetMaterialProperties(iShapeNum, iGroupID, -1, pkMaterialManager, 
            pTexturingProperty, m_bHasAlpha, m_bHasVertexColors, 
            m_spMesh))
    {
        const char *szShapeName;
        DtShapeGetShapeName(iShapeNum, &szShapeName);
        DtExt_Err("ERROR: Object [%s] had Material Properties error.\n",
            szShapeName);
        SetStatus(MYIOBJ_ERROR);
        return;
    }

    // Assign the Texture Property
    if (pTexturingProperty != NULL)
    {
        m_spMesh->AttachProperty(pTexturingProperty);
    }
    else
    {
        pTexturingProperty = (NiTexturingProperty*)
            m_spMesh->GetProperty(NiTexturingProperty::GetType());
    }

    // Attach any PixelShaders and Attributes
    int iMaterialID;
    DtMtlGetID(iShapeNum, iGroupID, &iMaterialID);
    pkMaterialManager->AddPixelShaderAndAttributes(iMaterialID, m_spMesh);

    NiShaderRequirementDesc::NBTFlags eNBTMethod = 
        pkMaterialManager->GetNBTMethod(iMaterialID);
    unsigned int uiNBTUVSource = 
        pkMaterialManager->GetNBTUVSource(iMaterialID);

    if (eNBTMethod == NiShaderRequirementDesc::NBT_METHOD_NONE &&
        pTexturingProperty)
    {
        NiTexturingProperty::Map* pkNormalMap = 
            pTexturingProperty->GetNormalMap();
        if (pkNormalMap)
        {
            if (NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT == uiNBTUVSource)
            {
                uiNBTUVSource = pkNormalMap->GetTextureIndex();
            }
            eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_ATI;
        }

        NiTexturingProperty::Map* pkParallaxMap = 
            pTexturingProperty->GetParallaxMap();
        if (pkParallaxMap)
        {
            if (pkNormalMap && 
                pkParallaxMap->GetTextureIndex() != 
                pkNormalMap->GetTextureIndex())
            {
                char acString[1024];
                const char *szShapeName;
                DtShapeGetShapeName( iShapeNum, &szShapeName );
                NiSprintf(acString, 1024,"WARNING: Object [%s]. " \
                    "Parallax Map and Normal Map use" \
                    " different UV sets, only normal map UV set will have"
                    " its NBT frame generated.\n", szShapeName);
                DtExt_Err(acString);
            }
            else
            {
                if (NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT == 
                    uiNBTUVSource)
                {
                    uiNBTUVSource = pkParallaxMap->GetTextureIndex();
                }
                eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_ATI;
            }
        }
    }

    // On the Wii, check to see if a bump map or sphere map is applied 
    // since having one of these maps requires NBT streams.
    if (NiSystemDesc::GetSystemDesc().GetToolModeRendererID() == 
        NiSystemDesc::RENDERER_WII)
    {
        // Check for a bump map
        NiTexturingProperty::Map* pkBumpMap = NULL;
        if (pTexturingProperty)
        {
            pkBumpMap = pTexturingProperty->GetBumpMap();
        }

        if (pkBumpMap)
        {
            eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_ATI;

            if (uiNBTUVSource == NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT)
            {
                if (pkBumpMap)
                {
                    uiNBTUVSource = pkBumpMap->GetTextureIndex();
                }
            }
        }
    }

    if (NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT == uiNBTUVSource)
    {
        // This would only happen if we are creating BTs when nothing 
        // requires it.
        uiNBTUVSource = 0;
    }

    // if the Maya NBT method has been chosen, we must extract the data here
    if (eNBTMethod == NiShaderRequirementDesc::NBT_METHOD_DCC)
    {
        if (!GetBinormalTangents(iShapeNum, iGroupID, uiNBTUVSource))
        {
            const char *szShapeName;
            DtShapeGetShapeName( iShapeNum, &szShapeName );
            DtExt_Err("WARNING: Object [%s]. " \
                "The NBT frame was not created!\n",szShapeName);
        }
    }
    else if (eNBTMethod != NiShaderRequirementDesc::NBT_METHOD_NONE)
    {
        // finally, call into the NBT generation ToolLib
        // this won't do anything if the NBT method is set to NONE
        if (!NiNBTGenerator::SetupBinormalTangentData(m_spMesh, uiNBTUVSource,
            eNBTMethod))
        {
            const char *szShapeName;
            DtShapeGetShapeName( iShapeNum, &szShapeName );
            DtExt_Err("WARNING: Object [%s]. " \
                "The NBT frame was not created!\n",szShapeName);
        }
    }

    if (DtExt_GetShapeHasSkin(iShapeNum))
        ConvertSkinModifier(iShapeNum, iGroupID, m_spMesh);

    // weld our verts together
    MObject kDestObj;
    if (GetBlendShape(iShapeNum, iGroupID, kDestObj))
    {
        // We expect to morph this shape, so do not remove
        // degenerate triangles and do get a mapof changed vertex
        // indexes.
        puiNewToOld = NiAlloc(NiUInt32, m_uiNiVertices);
        NiOptimize::RemoveDegenerateVertices(m_spMesh, puiNewToOld);
    }
    else
    {
        NiOptimize::RemoveDegenerateTriangles(m_spMesh);
        NiOptimize::RemoveDegenerateVertices(m_spMesh);
    }
    NiMeshTools::RemoveUnusedVertices(m_spMesh);

    m_spMesh->RecomputeBounds();

    // Attach morph controllers and morph data
    AttachMorpherControllers(iShapeNum, iGroupID, puiNewToOld);


    MObject kTransformObject;
    DtExt_ShapeGetTransform(iShapeNum, kTransformObject);

    const char* pcProfileName = gExport.m_DefaultProfileName;
    if(pcProfileName == NULL)
    {
        DtExt_Err("Could not resolve scene default mesh packing profile. Using"
            " \"Default\".\n");
        pcProfileName = "Default";
    }

    NiStringExtraData* pkMeshProfileExtraData = NiNew NiStringExtraData(
        pcProfileName);

    pkMeshProfileExtraData->SetName("MeshProfileName");
    //This must happen after the creation of the new mesh
    m_spMesh->AddExtraData(pkMeshProfileExtraData);

    AttachPerMeshProfileData(kTransformObject);
    

    if (puiNewToOld)
        NiFree(puiNewToOld);

    SetStatus(MYIOBJ_SUCCESS);
}
//---------------------------------------------------------------------------
MyiTriMesh::~MyiTriMesh( void )
{
    m_spMesh = 0;
}
//---------------------------------------------------------------------------
bool MyiTriMesh::GetMaterialProperties(int iShapeID, int iGroupID, 
    int iMaterialID,
    MyiMaterialManager* pkMaterialManager, 
    NiTexturingProperty* pTexturingProperty,
    bool bNeedsAlphaProperty, bool bHasVertexColors,
    NiAVObject* pkAVObject)
{
    int iMaterials;

    iMaterials = DtGroupGetCount( iShapeID );

    // For now, assign the global material (the first one in the array
    // returned by ?) to the entire model.

    if ((iMaterials > 0) || (iMaterialID >= 0))
    {
        NiMaterialProperty *pMaterial;
        NiSpecularProperty *pSpecular;
        NiVertexColorProperty *pVertexColor;
        NiAlphaProperty *pAlpha;
        
        // Get all Materials;
        // Pass them into pMaterialManger so that they will get translated
        // into ni format.

        // Create pMaterial based on iShapeNum, iMaterials=0
        if (iMaterialID == -1)
        {
            DtMtlGetID( iShapeID, iGroupID, &iMaterialID );
        }

        // Maybe pass in the m_spp instead.
        pkMaterialManager->GetMaterial(iMaterialID, pMaterial, 
                                      pSpecular, pVertexColor,pAlpha);

        NiProperty *pProp = pkAVObject->GetProperty(pMaterial->Type());
        
        if (pProp)
        {
            pkAVObject->DetachProperty(pProp);
        }

        pkAVObject->AttachProperty(pMaterial);


        // ********************************************************
        // Apply Transparency
        //*********************************************************

        // Create an Alpha property if one wasn't created already
        bool bTransparencyOverride = 
            DtTextureGetMaterialTransparencyOverride(iMaterialID);

        if((pAlpha == NULL) && 
            ((bNeedsAlphaProperty) || ( bTransparencyOverride)) )
        {
            pAlpha = NiNew NiAlphaProperty;
            pAlpha->SetSrcBlendMode( NiAlphaProperty::ALPHA_SRCALPHA);
            pAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
            pAlpha->SetAlphaBlending(true);
        }

        if(bTransparencyOverride)
        {
            // Alpha Blending
            pAlpha->SetAlphaBlending(
                DtTextureGetMaterialTransparencyUseAlphaBlending(
                    iMaterialID));
            
            char cSrcBlendMode;
            DtTextureGetMaterialTransparencySource(iMaterialID, 
                cSrcBlendMode);

            pAlpha->SetSrcBlendMode(
                (NiAlphaProperty::AlphaFunction)cSrcBlendMode);
            
            char cDestBlendMode;
            DtTextureGetMaterialTransparencyDestination(iMaterialID, 
                cDestBlendMode);
            pAlpha->SetDestBlendMode(
                (NiAlphaProperty::AlphaFunction)cDestBlendMode);
            
            // Alpha Testing UseAlphaTesting
            pAlpha->SetAlphaTesting(
                DtTextureGetMaterialTransparencyUseAlphaTesting(iMaterialID));
            
            pAlpha->SetNoSorter(
                DtTextureGetMaterialTransparencyNoTestingSorter(iMaterialID));
            
            short sTestReference;
            DtTextureGetMaterialTransparencyTestReference(iMaterialID, 
                sTestReference);
            pAlpha->SetTestRef((unsigned char)sTestReference);

            char cTestingTestMode;
            DtTextureGetMaterialTransparencyTestingTestMode(iMaterialID, 
                cTestingTestMode);

            pAlpha->SetTestMode(
                (NiAlphaProperty::TestFunction)cTestingTestMode);
        }

        if (pAlpha != NULL)
        {
            pkAVObject->AttachProperty(pAlpha);
        }


        // ********************************************************
        // Apply Specular Highlights
        //*********************************************************
        NIASSERT(pSpecular != NULL);
        pProp = pkAVObject->GetProperty(NiProperty::SPECULAR);
        if (pProp)
        {
            pkAVObject->DetachProperty(pProp);
        }
        pkAVObject->AttachProperty(pSpecular);


        // ********************************************************
        // Apply Vertex Colors
        //*********************************************************

        NiVertexColorPropertyPtr pVertexProperty = NiNew NiVertexColorProperty;
        
        if (pVertexProperty == NULL)
            return false;

        // Create a VertexColor Property from what we know from the shape...
        if (!bHasVertexColors && NiIsKindOf(NiMesh, pkAVObject))
        {
            NiMesh* pkMesh = (NiMesh*)pkAVObject;

            pVertexProperty->SetSourceMode(
                NiVertexColorProperty::SOURCE_IGNORE);
            pVertexProperty->SetLightingMode(
                NiVertexColorProperty::LIGHTING_E_A_D);

            // Remove Vertex Colors 

            NiDataStreamRef* pkStreamRef;
            NiDataStreamElement kElement;
            if (pkMesh->FindStreamRefAndElementBySemantic(
                NiCommonSemantics::COLOR(), 0, 
                NiDataStreamElement::F_FLOAT32_4, pkStreamRef,
                kElement))
            {
                pkMesh->RemoveStreamRef(pkStreamRef);
            }
        }
        else
        {
            // Default Values
            pVertexProperty->SetSourceMode(
                NiVertexColorProperty::SOURCE_AMB_DIFF);
            pVertexProperty->SetLightingMode(
                NiVertexColorProperty::LIGHTING_E_A_D);
            
            if(DtTextureGetMaterialVertexColorOverride(iMaterialID))
            {
                char cSrcMode;
                DtTextureGetMaterialVertexColorSrcMode(iMaterialID, cSrcMode);
                pVertexProperty->SetSourceMode(
                    (NiVertexColorProperty::SourceVertexMode)cSrcMode);
                
                char cLightMode;
                DtTextureGetMaterialVertexColorLightMode(iMaterialID, 
                    cLightMode);

                pVertexProperty->SetLightingMode(
                    (NiVertexColorProperty::LightingMode)cLightMode);
            }
        }

        pProp = pkAVObject->GetProperty(pVertexProperty->Type());
        if (pProp)
        {
            pkAVObject->DetachProperty(pProp);
        }        
        pkAVObject->AttachProperty(pVertexProperty);
        


        // ********************************************************
        // Apply Shading
        //*********************************************************

        char cShading;
        if(DtTextureGetMaterialShading(iMaterialID, cShading))
        {
            NiShadePropertyPtr spShadeProperty = NiNew NiShadeProperty;
            spShadeProperty->SetSmooth(cShading == 1);
            pkAVObject->AttachProperty(spShadeProperty);
        }
        

        // ********************************************************
        // Apply Texturing Properties
        //*********************************************************

        char cTextureApplyMode;
        if((pTexturingProperty != NULL) &&
            DtTextureGetMaterialTextureApplyMode(iMaterialID, 
                cTextureApplyMode))
        {
            pTexturingProperty->SetApplyMode(
                (NiTexturingProperty::ApplyMode)cTextureApplyMode);
        }

        // ********************************************************
        // Apply NBT Method
        //*********************************************************

        char cNBTMethod;
        if( NiIsKindOf(NiMesh, pkAVObject) &&
            DtTextureGetMaterialNBTMethod(iMaterialID, cNBTMethod))
        {
            NiShaderRequirementDesc::NBTFlags eNBTMethod = 
                NiShaderRequirementDesc::NBT_METHOD_NONE;
            switch(cNBTMethod)
            {
            case 0:
                eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_NONE;
                break;
            case 1:
                eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_NDL;
                break;
            case 2:
                eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_ATI;
                break;
            case 3:
                eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_DCC;
                break;
            }

            pkMaterialManager->SetNBTMethod(iMaterialID, eNBTMethod);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool MyiTriMesh::GetShapeAttributesForNi(int iShapeID, int iGroupID )
{
    if (!DtShapeGetShapeName( iShapeID, &m_pName ))
    {
        // Error From DtShapeGetShapeName
        return false;
    }

    m_fScale[0] = m_fScale[1] = m_fScale[2] = 1.0f;
    if (!DtShapeGetFinalNonUniformNonAnimatedScale( iShapeID, &m_fScale[0], 
        &m_fScale[1], &m_fScale[2]))
    {
        // Error From DtShapeGetFinalNonUniformNonAnimatedScale
        return false;
    }

    if (!GetVertsForNi(iShapeID, iGroupID))
    {
        // Error From GetVertsForNi
        return false;
    }

    if (!GetTriIndexListForNi(iShapeID, iGroupID))
    {
        // Error From GetTriIndexListForNi
        return false;
    }

    if (!GetUVsForNi(iShapeID, iGroupID))
    {
        return false;
    }


// Calculation of Exploded Normals doesn't function properly
// So, we'll rely on the Mesh's CalculateNormals function.
    if (!GetNormsForNi(iShapeID, iGroupID))
    {
        // Error From GetNormsForNi
        return false;
    }

    if (!GetVertexColorsForNi(iShapeID, iGroupID))
    {
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------
bool MyiTriMesh::GetVertexColorsForNi(int iShapeID, int iGroupID)
{
    int iColorCnt;
    DtRGBA *DtFltRGBAVertexColors;
    DtShapeGetVerticesFaceColors( iShapeID, &iColorCnt,
        &DtFltRGBAVertexColors);

    // Check if the object has vertex colors
    if (DtFltRGBAVertexColors == NULL)
        return true;

    int iVertex = 0;
    int iCount;
    long *plIndices;

    DtFaceGetColorIndexByShape( iShapeID, iGroupID, &iCount, &plIndices );

    m_bHasVertexColors = false;

    NiColorA* pkColor = NULL;
    pkColor = NiNew NiColorA[iCount];
    if(!pkColor)
    {
        NIASSERT(0);
        return false;
    }
    
    for( int i = 0; i < iCount; i++ )
    {
        if( plIndices[i] != DtEND_OF_FACE )
        {
            // SET THE VERTEX COLOR SKIPPING THE END OF FACE
            pkColor[iVertex].r = DtFltRGBAVertexColors[ plIndices[i] ].r;

            pkColor[iVertex].g = DtFltRGBAVertexColors[ plIndices[i] ].g;

            pkColor[iVertex].b = DtFltRGBAVertexColors[ plIndices[i] ].b;

            pkColor[iVertex].a = DtFltRGBAVertexColors[ plIndices[i] ].a;

            // CHECK FOR A VERTEX COLOR
            if( !m_bHasVertexColors && ( pkColor[iVertex].r || 
                pkColor[iVertex].g || 
                pkColor[iVertex].b ||
                pkColor[iVertex].a ) )
            {
                m_bHasVertexColors = true;
            }

            // CHECK FOR A VERTEX COLOR ALPHA
            if(m_bHasVertexColors && !m_bHasAlpha &&
                (pkColor[iVertex].a < 1.0f))
                m_bHasAlpha = true;

            iVertex++;

        }
    }

    if(m_bHasVertexColors)
    {
        NiDataStreamRef* pkSR = m_spMesh->AddStream(NiCommonSemantics::COLOR(),
            0, NiDataStreamElement::F_FLOAT32_4, iCount, 
            ACCESS_FLAGS, NiDataStream::USAGE_VERTEX, pkColor);
        NI_UNUSED_ARG(pkSR);
    }


    NIASSERT(iVertex == (int)m_uiNiVertices);

    if(pkColor)
    {
        NiDelete[] pkColor;
    }
    else
    {
        NIASSERT(0);
    }

    return true;
}
//---------------------------------------------------------------------------
bool MyiTriMesh::GetNormsForNi(int iShapeID, int iGroupID )
{
    // Translate the Normals
    int iNumNormals;
    DtVec3f *pDtNormal;
    
    if (!DtGroupGetExplodedNormals( iShapeID, iGroupID,&iNumNormals, 
        &pDtNormal))
    {
        const char *szShapeName;
        DtShapeGetShapeName( iShapeID, &szShapeName );
        DtExt_Err("ERROR: Object [%s]. " \
            "DtGroupGetExplodedNormals Failed!\n",szShapeName);

        return false;
    }

    NIASSERT(iNumNormals == (int)m_uiNiVertices);

    NiPoint3* pkNormal = NULL;
    pkNormal = NiNew NiPoint3[m_uiNiVertices];
    if(!pkNormal)
    {
        NIASSERT(0);
        return false;
    }


    for (int i=0; i < (int)m_uiNiVertices; i++)
    {
        pkNormal[i].x = pDtNormal[i].vec[0];
        pkNormal[i].y = pDtNormal[i].vec[1];
        pkNormal[i].z = pDtNormal[i].vec[2];

        // Unitize the normal
        NiPoint3::UnitizeVector(pkNormal[i]);
    }

    NiDataStreamRef* pkSR=m_spMesh->AddStream(NiCommonSemantics::NORMAL(), 0,
        NiDataStreamElement::F_FLOAT32_3, m_uiNiVertices, 
        ACCESS_FLAGS, 
        NiDataStream::USAGE_VERTEX, pkNormal);
    NI_UNUSED_ARG(pkSR);

    // Deallocate memory used for pDtNormal
    if (pDtNormal)
        NiFree(pDtNormal);
    pDtNormal = NULL;

    if(pkNormal)
    {
        NiDelete[] pkNormal;
    }
    else
    {
        NIASSERT(0);
    }

    return true;
}
//---------------------------------------------------------------------------
bool MyiTriMesh::GetUVsForNi(int iShapeID, int iGroupID )
{
    MStatus kStat = MStatus::kSuccess;

    MObject kMeshObject;
    DtExt_ShapeGetOriginal(iShapeID, iGroupID, kMeshObject);

    MFnMesh kMeshFn(kMeshObject, &kStat);
    CHECK_AND_ASSERT(kStat);

    int iUVSetCount = kMeshFn.numUVSets(&kStat);
    CHECK_AND_ASSERT(kStat);

    MStringArray kUVSetNames;
    kStat = kMeshFn.getUVSetNames(kUVSetNames);
    CHECK_AND_ASSERT(kStat);

    MIntArray kUVSetIds(kUVSetNames.length(), -1);
    kStat = ConvertUVSetNameToId(kUVSetNames, kUVSetIds);
    CHECK_AND_ASSERT(kStat);

    NIASSERT(kUVSetIds.length() == kUVSetNames.length());

    for (int iUVSetIndex = 0; iUVSetIndex < iUVSetCount; iUVSetIndex++)
    {
        MString kUVSetName = kUVSetNames[iUVSetIndex];
        const char* pcUVSetName = kUVSetName.asChar();
        
        int iUVVertCount = 0;
        DtVec2f * pkUVs = 0;
        if (!DtGroupGetExplodedTextureVerticesByIndex(iShapeID, iGroupID, 
            iUVSetIndex, &iUVVertCount, &pkUVs))
        {
            return false;
        }

        NIASSERT(iUVVertCount == (int)m_uiNiVertices);

        int iUVSetId = kUVSetIds[iUVSetIndex];

        NiPoint2* pkUVsForStream = NiNew NiPoint2[iUVVertCount];

        for (unsigned int iVertexIndex = 0; iVertexIndex < m_uiNiVertices; 
            iVertexIndex++)
        {
            NIASSERT(iUVSetId >= 0);

            pkUVsForStream[iVertexIndex].x = pkUVs[iVertexIndex].vec[0];
            pkUVsForStream[iVertexIndex].y = pkUVs[iVertexIndex].vec[1];
        }

        NiDataStreamRef* pkSR=m_spMesh->AddStream(
            NiCommonSemantics::TEXCOORD(), iUVSetId,
            NiDataStreamElement::F_FLOAT32_2, m_uiNiVertices, 
            ACCESS_FLAGS, 
            NiDataStream::USAGE_VERTEX, pkUVsForStream);
        NI_UNUSED_ARG(pkSR);

        //cache the values for later
        m_UVNameToIdMap.SetAt(pcUVSetName, iUVSetId);

        // Deallocate memory used for pkUVs
        if (pkUVs)
        {
            NiFree(pkUVs);
        }
        pkUVs = NULL;

        if(pkUVsForStream)
        {
            NiDelete[] pkUVsForStream;
        }
        else
        {
            NIASSERT(0);
        }
    }
    return true;
}
//---------------------------------------------------------------------------
bool MyiTriMesh::GetBinormalTangents(int iShapeID, int iGroupID, int iUVSet)
{
    MStatus kReturnStatus;
    MObject kMeshObject;
    DtExt_ShapeGetOriginal(iShapeID, iGroupID, kMeshObject);

    MFnMesh kMeshFn(kMeshObject, &kReturnStatus);
    int iUVSetCount = kMeshFn.numUVSets(&kReturnStatus);

    MStringArray kUVSetNames;
    kReturnStatus = kMeshFn.getUVSetNames(kUVSetNames);

    MIntArray kUVSetIds(kUVSetNames.length(), -1);
    kReturnStatus = ConvertUVSetNameToId(kUVSetNames, kUVSetIds);

    MString kUVSetName;
    // find the Maya UV set that cooresponds to this Gamebryo index
    for (int iUVSetIndex = 0; iUVSetIndex < iUVSetCount; iUVSetIndex++)
    {
        if (kUVSetIds[iUVSetIndex] == iUVSet)
            kUVSetName = kUVSetNames[iUVSetIndex];
    }

    // Create binormal and tangent data streams
    // Look for an existing binormal stream that matches the UV index
    NiDataStreamRef* pkRef;
    NiDataStreamElement kElement;
    bool bFound;
    bFound = m_spMesh->FindStreamRefAndElementBySemantic(
        NiCommonSemantics::BINORMAL(), iUVSet, 
        NiDataStreamElement::F_UNKNOWN, pkRef, kElement);
    if (!bFound)
    {
        // if we didn't find a matching stream, we need to create it
        m_spMesh->AddStream(NiCommonSemantics::BINORMAL(), iUVSet,
            NiDataStreamElement::F_FLOAT32_3, m_uiNiVertices, ACCESS_FLAGS,
            NiDataStream::USAGE_VERTEX, NULL, true, true);
    }

    // Look for an existing tangent stream that matches the UV index
    bFound = m_spMesh->FindStreamRefAndElementBySemantic(
        NiCommonSemantics::TANGENT(), iUVSet, 
        NiDataStreamElement::F_UNKNOWN, pkRef, kElement);
    if (!bFound)
    {
        // if we didn't find a matching stream, we need to create it
        m_spMesh->AddStream(NiCommonSemantics::TANGENT(), iUVSet,
            NiDataStreamElement::F_FLOAT32_3, m_uiNiVertices, ACCESS_FLAGS,
            NiDataStream::USAGE_VERTEX, NULL, true, true);
    }

    // Get the iterators

    // Get the Binormal iterator
    NiDataStreamElementLock kLockBiNormals(m_spMesh,
        NiCommonSemantics::BINORMAL(), iUVSet, 
        NiDataStreamElement::F_FLOAT32_3, 
        NiDataStream::LOCK_TOOL_WRITE | NiDataStream::LOCK_TOOL_READ);

    if (!kLockBiNormals.IsLocked())
    {
        NIASSERT(!"Failed to lock BiNormal data stream!");
        return false;
    }

    NiTStridedRandomAccessIterator<NiPoint3> kBinormalIter = 
        kLockBiNormals.begin<NiPoint3>();

    // Get the Tangent iterator
    NiDataStreamElementLock kLockTangents(m_spMesh,
        NiCommonSemantics::TANGENT(), iUVSet, 
        NiDataStreamElement::F_FLOAT32_3, 
        NiDataStream::LOCK_TOOL_WRITE | NiDataStream::LOCK_TOOL_READ);

    if (!kLockTangents.IsLocked())
    {
        NIASSERT(!"Failed to lock Tangent data stream");
        return false;
    }

    NiTStridedRandomAccessIterator<NiPoint3> kTangentIter = 
        kLockTangents.begin<NiPoint3>();
    // how far into the buffers we have we written data
    unsigned int uiCurrentGBIndex = 0;

    // Map data to Gamebryo

    // get DAG path of object
    MDagPath kDagPath;
    kMeshFn.getPath(kDagPath);
    // get component of object
    MObjectArray kComponents;
    MObjectArray kSets;
    kMeshFn.getConnectedSetsAndMembers(kDagPath.instanceNumber(), 
        kSets, kComponents, true);
    // for now only  deal with the first material?
    MObject kComponent = MObject::kNullObj;
    if (kComponents.length() > (unsigned int)iGroupID)
    {
        kComponent = kComponents[iGroupID];
    }
    else
    {
        NIASSERT(!"Failed to retrieve mesh component when converting NBTs");
        return false;
    }

    // Maya face iterator
    MItMeshPolygon kMayaFaceIter(kDagPath, kComponent, &kReturnStatus);

    for (; !kMayaFaceIter.isDone(); kMayaFaceIter.next())
    {
        // the number of triangles on this face
        int iNumTriangles;
        kMayaFaceIter.numTriangles(iNumTriangles);

        for (int iTriangle = 0; iTriangle < iNumTriangles; iTriangle++)
        {
            MIntArray kFaceIndices;
            kMayaFaceIter.getVertices(kFaceIndices);
            for (int iVertex = 0; iVertex < 3; iVertex++)
            {
                // this tells us what vertex on the face we are considering
                int iFaceVertexIndex = 0;
                MIntArray kTriangleIndices;
                MPointArray kTrianglePoints;
                kMayaFaceIter.getTriangle(iTriangle, kTrianglePoints, 
                    kTriangleIndices);
                for (unsigned int ui = 0; ui < kFaceIndices.length(); ui++)
                {
                    if (kFaceIndices[ui] == kTriangleIndices[iVertex])
                    {
                        iFaceVertexIndex = ui;
                        break;
                    }
                }

                // get Maya's tangent and binormal vector
                MVector kMayaTangent;
                kMeshFn.getFaceVertexTangent(
                    kMayaFaceIter.index(&kReturnStatus),
                    kFaceIndices[iFaceVertexIndex],
                    kMayaTangent, MSpace::kObject, &kUVSetName);
                MVector kMayaBinorm;
                kMeshFn.getFaceVertexBinormal(
                    kMayaFaceIter.index(&kReturnStatus),
                    kFaceIndices[iFaceVertexIndex], 
                    kMayaBinorm, MSpace::kObject, &kUVSetName);

                if (uiCurrentGBIndex < m_uiNiVertices)
                {
                    kTangentIter[uiCurrentGBIndex] = NiPoint3(
                        (float)kMayaTangent.x, (float)kMayaTangent.y,
                        (float)kMayaTangent.z);
                    kBinormalIter[uiCurrentGBIndex] = NiPoint3(
                        (float)kMayaBinorm.x, (float)kMayaBinorm.y,
                        (float)kMayaBinorm.z);

                    uiCurrentGBIndex++;
                }
            } // end vertex iterator
        } // end triangle iterator
    } // end face iterator

    return true;
}
//---------------------------------------------------------------------------
MStatus MyiTriMesh::ConvertUVSetNameToId(const MStringArray& kUVSetNames, 
    MIntArray& kUVSetIds)
{
    MStatus kStat = MStatus::kSuccess;
    //cache old errno
    int iOldErrno = errno;
    
    NIASSERT(kUVSetNames.length() == kUVSetIds.length());

    //cache the count
    unsigned int iUVSetCount = kUVSetNames.length();

    //setup an array that says which ids are still available for use
    int* pAvailableIds = NULL;
    pAvailableIds = new int[iUVSetCount];
    if(pAvailableIds == NULL)
    {
        NIASSERT(0);
        return MStatus::kFailure;
    }
    memset(pAvailableIds, -1, sizeof(int) * iUVSetCount);
    
    //I need to split the UVSet names into two groups
    //I do this because I can't guarantee the order that
    //the sets are going to be in
    //It might be possible that someone has the setup
    //uvSet0 and map1, in which case I want uvSet0 to goto id 0
    //and map1 to goto 1 as opposed to the id dictated by the order

    //First time through looking for uv sets named uvSet*

    //Variables used in the loop
    unsigned int nextUVSetId = 0;
    MString kUVSetName;
    bool bAlreadyUsedId = false;
    unsigned int iNamingConventionLength = (unsigned int)strlen(UV_SET_PREFIX);
    
    for(unsigned int iUVSetNameIndex = 0; iUVSetNameIndex < iUVSetCount; 
        iUVSetNameIndex++)
    {
        //reset
        bAlreadyUsedId = false;
        kUVSetName = kUVSetNames[iUVSetNameIndex];
        const char* pcUVSetName = kUVSetName.asChar();

        if(strncmp(pcUVSetName, UV_SET_PREFIX, iNamingConventionLength) == 0)
        {
            //iNamingConventionLength gets us past the
            //prefix
            pcUVSetName += iNamingConventionLength;

            //I use strtol instead of Atoi because it reports an error
            errno = 0;
            nextUVSetId = strtol(pcUVSetName, NULL, 10);

            //check to see if the id has not already been assigned
            //if it is not use the next available one
            for(unsigned int iUsedIdIndex = 0; iUsedIdIndex < iUVSetCount; 
                iUsedIdIndex++)
            {
                if((int)nextUVSetId == pAvailableIds[iUsedIdIndex])
                {
                    bAlreadyUsedId = true;
                    break;
                }
            }

            //have to pick a new id
            if(bAlreadyUsedId || errno != 0)
            {
                nextUVSetId = 
                    FindNextAvailableUVId(iUVSetCount, pAvailableIds);
            }
     
            kUVSetIds[iUVSetNameIndex] = nextUVSetId;
            pAvailableIds[iUVSetNameIndex] = nextUVSetId;
        }
    }

    //Second pass for uv sets that do not follow the naming convention
    for(unsigned int iUVSetNameIndex = 0; iUVSetNameIndex < iUVSetCount; 
        iUVSetNameIndex++)
    {
        kUVSetName = kUVSetNames[iUVSetNameIndex];
        const char* pcUVSetName = kUVSetName.asChar();

        if(strncmp(pcUVSetName, UV_SET_PREFIX, iNamingConventionLength) != 0)
        {
            nextUVSetId = FindNextAvailableUVId(iUVSetCount, pAvailableIds);
            kUVSetIds[iUVSetNameIndex] = nextUVSetId;
            pAvailableIds[iUVSetNameIndex] = nextUVSetId;
        }
    }
       
    delete[] pAvailableIds;
    errno = iOldErrno;

    return kStat;
}
//---------------------------------------------------------------------------
unsigned int MyiTriMesh::FindNextAvailableUVId(unsigned int iUVSetCount, 
    int* pAvailableIds)
{
    unsigned int nextUVSetId = 0;
    bool bAlreadyUsedId = false;
    for(unsigned int iPotentialId = 0; iPotentialId < iUVSetCount; 
        iPotentialId++)
    {
        bAlreadyUsedId = false;
        for(unsigned int iUsedIdIndex = 0; iUsedIdIndex < iUVSetCount; 
            iUsedIdIndex++)
        {
            if((int)iPotentialId == pAvailableIds[iUsedIdIndex])
            {
                bAlreadyUsedId = true;
                break;
            }
        }

        if(!bAlreadyUsedId)
        {
            nextUVSetId = iPotentialId;
            break;
        }
    }
    
    NIASSERT(nextUVSetId < iUVSetCount);

    return nextUVSetId;
}

//---------------------------------------------------------------------------
bool MyiTriMesh::GetTriIndexListForNi(int iShapeID, int iGroupID)
{
    int iNumOfGrps = DtGroupGetCount( iShapeID );
    
    if (iNumOfGrps < 1)
    {
        // Failed Assumption...
        const char *szShapeName;
        DtShapeGetShapeName( iShapeID, &szShapeName );
        DtExt_Err("ERROR: Object [%s]. " \
            "Failed Assumption - DtGroupGetCount return 0.\n", szShapeName);

        return false;
    }

    unsigned int uiNumIndices = 0;
    int *piShpIndexList;
    int iIndexCnt = 0;

    // Note that all this function is "really" doing in most cases is
    // generating a sequential list of numbers like 0,1,2,3,4,... etc.
    // because, remember, we have created 1 index for *every* vertex,
    // so our indexs naturally will just count upward.  The function
    // itself is more complicated - but generates this sequence.  If
    // *multiple* groups are ever supported, this function might need
    // to be tweaked.
    dtPolygonGetExplodedFaceList( iShapeID, iGroupID, &piShpIndexList, 
        &iIndexCnt );

    unsigned int* puiTriList = NiAlloc(unsigned int, iIndexCnt);

    for (int i = 0; i<(int)iIndexCnt; i++)
    {
        if (piShpIndexList[i] >= 0)
        {
            puiTriList[uiNumIndices++] = piShpIndexList[i];
        }
    }

    NiDataStreamRef* pkSR=m_spMesh->AddStream(NiCommonSemantics::INDEX(), 0, 
        NiDataStreamElement::F_UINT32_1, uiNumIndices, 
        ACCESS_FLAGS, NiDataStream::USAGE_VERTEX_INDEX, puiTriList);
    NI_UNUSED_ARG(pkSR);

    if (piShpIndexList)
        NiFree(piShpIndexList);
    piShpIndexList = NULL;

    if (puiTriList)
        NiFree(puiTriList);
    puiTriList = NULL;

    return true;
}
//---------------------------------------------------------------------------
bool MyiTriMesh::GetVertsForNi(int iShapeID, int iGroupID)
{
    NiPoint3* pkPosition = 0;
    
    if (!GetVertsForNiAsArray(iShapeID, iGroupID, pkPosition, m_uiNiVertices))
    {
        NiDelete pkPosition;
        return false;
    }

    NiDataStreamRef* pkSR=m_spMesh->AddStream(NiCommonSemantics::POSITION(), 0,
        NiDataStreamElement::F_FLOAT32_3, m_uiNiVertices, 
        ACCESS_FLAGS, NiDataStream::USAGE_VERTEX, pkPosition);
    NI_UNUSED_ARG(pkSR);

    if(pkPosition)
    {
        NiDelete[] pkPosition;
    }
    else
    {
        NIASSERT(0);
    }
    
    return true;
}
//---------------------------------------------------------------------------
bool MyiTriMesh::ConvertSkinModifier(NiInt32 iShapeID, NiInt32 iGroupID,
                                     NiMesh* pkMesh)
{
    // this function will create a skinning mesh modifier, give it the data
    // it needs, attach it to the mesh, and create any necessary data streams
    // for skinning to work correctly. It has the following restrictions:
    //  * only 4 bones can influence each vertex
    //  * the result is not partitioned (we expect that to be later)

    // Start by converting the per-vertex weight and bone index data
    NiInt32 iNumJoints = 0;
    NiInt32* piJoints = NULL;
    NiUInt32 uiBonesPerVertex = 4;

    DtShapeGetAllJoints(iShapeID, &iNumJoints, &piJoints);

    NiTMap<NiInt32, NiInt32> kGlobalJointIdToLocal;

    {
        for(NiInt32 iJointIndex = 0; iJointIndex < iNumJoints; iJointIndex++)
        {
            NiInt32 iGlobalJointId = piJoints[iJointIndex];
            NiInt32 iLocalJointId = iJointIndex;
            kGlobalJointIdToLocal.SetAt(iGlobalJointId, iLocalJointId);
        }
    }

    if (iNumJoints == 0)
        return false;

    // If the model was not set up correctly, there may be invalid joints, ie
    // having "-1" as an index. Below, we validate that all Joints have valid
    // indices and if not, we fail.
    for (NiInt32 iJ = 0; iJ < iNumJoints; iJ++)
    {
        if (piJoints[iJ] == -1)
        {
            DtExt_Err("Error:: Invalid joints were found in a skin. Skinning "
                "will be disabled");
            NiFree(piJoints);
            return false;
        }
    }

    // get per-vertex weight data from the MDt layer
    NiInt32 iNumWeights;
    kMDtVertexWeights *pVertexWeights;
    DtShapeGetExplodedWeights(iShapeID, iGroupID, &iNumWeights, 
        &pVertexWeights);
    NiUInt32 uiNumWeights = (NiUInt32)iNumWeights;
    NIASSERT(pVertexWeights != NULL);

    NiDataStreamElementLock kBoneLock;
    NiDataStreamElementLock kWeightLock;
    // create data streams for the weights and indices
    if (!CreateSkinDataStreams(pkMesh, uiNumWeights, iNumJoints, kBoneLock, 
        kWeightLock))
    {
        DtExt_Err("Error:: Failed to create skinning data streams. Skinning "
            "will be disabled.");
        NiFree(piJoints);
        return false;
    }
    NiTIndexIter kBoneIndices = kBoneLock.begin<NiTSimpleArray<NiInt16,4> >();
    NiTWeightIter kBoneWeights = kWeightLock.begin<NiTSimpleArray<float,4> >();

    // zero out the Gamebryo index and weight data
    for (NiUInt32 uiVert = 0; uiVert < uiNumWeights; uiVert++)
    {
        for (NiUInt32 uiBone = 0; uiBone < uiBonesPerVertex; uiBone++)
        {
            kBoneIndices[uiVert][uiBone] = 0;
            kBoneWeights[uiVert][uiBone] = 0.0f;
        }
    }

    // iterate over all of the verts and fill in the weight and index data
    for (NiUInt32 uiVert = 0; uiVert < uiNumWeights; uiVert++)
    {
        // for each vertex, we need to determine the top 4 weights
        // we don't mind n^2 behavior because we expect the list to be short
        for (NiUInt32 uiGBBone = 0; uiGBBone < uiBonesPerVertex; uiGBBone++)
        {
            float fLargestWeight = 0.0f;
            NiUInt32 uiLargestIndex = UINT_MAX;
            //go through all the bones that affect the vertices
            //if the bone has the largest influence and has not 
            //already been added, add it
            for (NiUInt32 uiMDtBone = 0; 
                uiMDtBone < (NiUInt32)pVertexWeights[uiVert].m_iSize; 
                uiMDtBone++)
            {
                // is this bone weight larger than the ones before it?
                if (pVertexWeights[uiVert].m_pfWeights[uiMDtBone] > 
                    fLargestWeight)
                {
                    // make sure this one isn't already added
                    bool bAlreadyAdded = false;
                    for (NiUInt32 uiGBBone2 = 0; uiGBBone2 < uiBonesPerVertex;
                        uiGBBone2++)
                    {
                        if ((kBoneIndices[uiVert][uiGBBone2] == (int)uiMDtBone)
                            && (kBoneWeights[uiVert][uiGBBone2] > 0.0f))
                        {
                            bAlreadyAdded = true;
                        }
                    }
                    if (!bAlreadyAdded)
                    {
                        fLargestWeight = 
                            pVertexWeights[uiVert].m_pfWeights[uiMDtBone];
                        NiInt32 iGlobalId = 
                            pVertexWeights[uiVert].m_piJointIndices[uiMDtBone];
                        NiInt32 iLocalId = 0;
                        kGlobalJointIdToLocal.GetAt(iGlobalId, iLocalId);
                        uiLargestIndex = iLocalId;
                    }
                }
            }
            // now we know the largest weight and index if any were found
            if (uiLargestIndex != UINT_MAX)
            {
                // we found a bone influence to add
                kBoneIndices[uiVert][uiGBBone] = (NiUInt16)uiLargestIndex;
                kBoneWeights[uiVert][uiGBBone] = fLargestWeight;
            }
            // if we didn't find enough bones, leave the index and weight of
            // zero indicating no influence
        }
    }

    // Now the bone weights and bone indices data streams are filled in
    // Gather the data that the skin mesh modifier needs
    NiSkinningMeshModifier* pkModifier = NiNew NiSkinningMeshModifier(
        iNumJoints);
    NIASSERT(pkModifier);

    // Get the root to skin transform
    float afMatrix[4][4];
    NiTransform kRootToSkin;
    DtShapeGetInverseMatrix(iShapeID, afMatrix);
    MatrixToTransform(afMatrix, kRootToSkin);
    pkModifier->SetRootBoneParentToSkinTransform(kRootToSkin);

    // recurse up the scenegraph until we find the ancestor immediately below
    // the scene root and assign it as the root bone parent
    NiInt32 iComponentID = gMDtObjectFindComponentID(ETypeJoint, piJoints[0]);
    NiNode* pkParent = gUserData.GetNode(iComponentID);
    while (pkParent->GetParent())
    {
        pkParent = pkParent->GetParent();
    }
    pkModifier->SetRootBoneParent(pkParent);

    // allocate the bone pointer list and the bone bind pose transform list
    NiAVObject** ppkBones = pkModifier->GetBones();
    NiTransform* pkSkinToBoneXForms = pkModifier->GetSkinToBoneTransforms();

    for (NiInt32 i = 0; i < iNumJoints; i++)
    {
        // get the bone pointer
        NiInt32 iComponentID = gMDtObjectFindComponentID(ETypeJoint, 
            piJoints[i]);
        NiNode* pkBone = gUserData.GetNode(iComponentID);
        NIASSERT(pkBone != NULL);
        ppkBones[i] = pkBone;

        // get the skin to bone transform
        float afMatrix[4][4];
        DtShapeGetSkinToBoneMat(iShapeID, piJoints[i], afMatrix);
        MatrixToTransform(afMatrix, pkSkinToBoneXForms[i]);
    }

    // by default we are software skinned
    pkModifier->SetSoftwareSkinned(true);

    // attach the modifier
    pkMesh->AddModifier(pkModifier);

    // this function will set the bone bounds on the modifier for us
    NiOptimize::CalculateBoneBounds(pkMesh);

    NiOptimize::RemoveLowInfluenceBones(pkMesh, 
        gExport.m_fMinimumBoneInfluence);

    NiFree(piJoints);
    return true;
}
//---------------------------------------------------------------------------
void MyiTriMesh::MatrixToTransform(float afMat[4][4], 
    NiTransform& kTransform)
{
    kTransform.m_Rotate.SetCol(0, afMat[0][0], afMat[0][1], afMat[0][2]);
    kTransform.m_Rotate.SetCol(1, afMat[1][0], afMat[1][1], afMat[1][2]);
    kTransform.m_Rotate.SetCol(2, afMat[2][0], afMat[2][1], afMat[2][2]);

    kTransform.m_Translate.x = afMat[3][0] * gExport.m_fLinearUnitMultiplier;
    kTransform.m_Translate.y = afMat[3][1] * gExport.m_fLinearUnitMultiplier;
    kTransform.m_Translate.z = afMat[3][2] * gExport.m_fLinearUnitMultiplier;

    kTransform.m_fScale = 1.0f;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//    Code for checking Shading Model...
//
//  TAKE INTO ACCOUNT MAYA's SHADING MODEL
//    char *pszShadingModel;
//    DtMtlGetName( iShapeNum, 0, &pszShadingModel ); 
// Shape, group to get shading model name
//    
//    MObject MObj;
//    DtExt_ShapeGetShader( iShapeNum, 0, MObj);
//    const char *pszName = MObj.apiTypeStr();
//    switch (MObj.apiType())
//    {
//        case MFn::kLambert:
//        {
//            break;
//        }
//        case MFn::kBlinn:
//        {
//            break;            
//        }
//    }
//---------------------------------------------------------------------------

bool MyiTriMesh::CreateMultiTexture( int iShapeID, int iGroupID, 
    MyiTextureManager* pkTextureManager,
    NiTexturingProperty *&pTexturingProperty,
    bool &bNeedsAlphaProperty)
{

    int iTextures;  // iTextures is the number of textures for the shape.
    if (!DtExt_ShapeGetTexCnt( iShapeID, &iTextures ))
    {
        NIASSERT(0);
        return false;
    }

    // If no textures exist, we set the texture property to NULL and exit.
    if (iTextures < 1)
    {
        pTexturingProperty = NULL;
        return true;
    }

    m_uiMultiIndex = 0;  // Start after base
    
    int iMaterialID;
    DtMtlGetID( iShapeID, iGroupID, &iMaterialID);
    int iNumMaps = DtExt_MtlGetNumTextureMapsByID(iMaterialID); 

    for(int iLoop = 0; iLoop < iNumMaps; iLoop++)
    {
        const char* pcMapName = 
            DtExt_MtlGetTextureMapNameByID(iMaterialID, iLoop);

        bool bNeedsAlpha = false;

        CreateMultiTextureForType(iShapeID, iGroupID, pkTextureManager, 
            pTexturingProperty, (char*)pcMapName, bNeedsAlpha);

        // Only check for alpha on the base map
        if (iLoop == 0)
        {
            bNeedsAlphaProperty |= bNeedsAlpha;
        }

    }
    BakeUVs(pTexturingProperty);

    return true;
}
//---------------------------------------------------------------------------

bool MyiTriMesh::CreateMultiTexture( int iMaterialID, 
    MyiTextureManager* pkTextureManager,
    NiTexturingProperty *&pTexturingProperty,
    bool &bNeedsAlphaProperty)
{
    int iNumMaps = DtExt_MtlGetNumTextureMapsByID(iMaterialID); 

    for(int iLoop = 0; iLoop < iNumMaps; iLoop++)
    {
        const char* pcMapName = DtExt_MtlGetTextureMapNameByID(iMaterialID, 
            iLoop);

        bool bNeedsAlpha = false;

        CreateMultiTextureForType(iMaterialID, pkTextureManager, 
            pTexturingProperty, (char*)pcMapName, bNeedsAlpha);

        // Only check for alpha on the base map
        if (iLoop == 0)
        {
            bNeedsAlphaProperty |= bNeedsAlpha;
        }

    }


    return true;
}
//---------------------------------------------------------------------------
static MStatus CreateDefaultTextureTransform( int mtlID, 
    const char* pcTextureType, NiTextureTransform*& pkTransform)
{
    if (!pkTransform)
    {
        NiPoint2 kRepeatUV;
        NiPoint2 kOffsetUV;
        float fRotate;
        bool bSuccess;

        bSuccess = DtTextureGetRepeatOffsetRotateUVs(mtlID, pcTextureType,
            kRepeatUV.x, kRepeatUV.y, kOffsetUV.x, kOffsetUV.y, fRotate);

        NIASSERT(bSuccess);
       
        pkTransform = NiNew NiTextureTransform(NiPoint2(0.0f, 0.0f), 0.0f,
            NiPoint2(1.0f, 1.0f), NiPoint2(0.5f, 0.5f),
            NiTextureTransform::MAYA_TRANSFORM);

        pkTransform->SetScale(kRepeatUV);
        pkTransform->SetTranslate(kOffsetUV);
        pkTransform->SetRotate(fRotate);
    }

    return MS::kSuccess;
}
//---------------------------------------------------------------------------
static MStatus GetTextureTransform( int mtlID, 
    const char* pcTextureType, char* pcAttribute, 
    NiTextureTransform*& pkTransform, 
    NiTextureTransformController*& pkUVController,
    NiTextureTransformController::TransformMember eMember, bool bInvert)
{

    // find shader from material ID
    MObject kMtlNode;
    int iStatus = DtExt_MtlGetShader( mtlID, kMtlNode );
    if (iStatus != 1)
        return MS::kFailure;
    
    MStatus kStatus;
    // get place2dTexture object for this texture
    MFnDependencyNode kPlace2d = DtExt_GetPlace2dTexture(kMtlNode,
        pcTextureType, &kStatus);

    // examine animatable parameters to see if they're animated
    MPlug kPlug = kPlace2d.findPlug(pcAttribute, &kStatus);

    // Go ahead and apply a texture transform per uv coordinate set.
    // We will optimize this out later...

    // Create the TextureTransform
    CreateDefaultTextureTransform(mtlID, pcTextureType, pkTransform);

    if (PlugIsConnected(kPlug)) 
    {
        float fScale = 1.0f;
        if (bInvert)
            fScale = -1.0f;

        unsigned int uiNumKeys;
        NiFloatKey* pkKeys;
        NiAnimationKey::KeyType eType;
        if (kPlug.isConnected() && 
            ConvertFloatAnim(kPlug, uiNumKeys, pkKeys, eType, fScale))
        {
            // Create the Animation Data
            NiFloatData* pkData = NiNew NiFloatData;
            pkData->ReplaceAnim(pkKeys, uiNumKeys, eType);
            
            // Create the Interpolator
            NiFloatInterpolator* pkInterp = NiNew NiFloatInterpolator(pkData);
            pkInterp->Collapse();

            // create NiTextureTransformController
            pkUVController = NiNew NiTextureTransformController;
            pkUVController->SetAffectedMember(eMember);
            pkUVController->SetInterpolator(pkInterp);
            pkUVController->ResetTimeExtrema();

            SetAnimations(pkUVController);
        }
    }

    return MS::kSuccess;
}   

//---------------------------------------------------------------------------
NiTexturingProperty::ClampMode MyiTriMesh::GetClampMode(char* pTextureName)
{
    
    int iHorzRepeat, iVertRepeat;
    DtTextureGetWrap( pTextureName, &iHorzRepeat, &iVertRepeat );
    
    NiTexturingProperty::ClampMode eClampMode;
    
    if (iHorzRepeat == DT_REPEAT)
    {
        if (iVertRepeat == DT_REPEAT)
            eClampMode = NiTexturingProperty::WRAP_S_WRAP_T;
        else
            eClampMode = NiTexturingProperty::WRAP_S_CLAMP_T;
    }
    else
    {
        if (iVertRepeat == DT_REPEAT)
            eClampMode = NiTexturingProperty::CLAMP_S_WRAP_T;
        else
            eClampMode = NiTexturingProperty::CLAMP_S_CLAMP_T;
    }
    
    return eClampMode;
}
//---------------------------------------------------------------------------
bool MyiTriMesh::CreateMultiTextureForType(
    int iShapeID, 
    int iGroupID,
    MyiTextureManager* pkTextureManager,
    NiTexturingProperty *&pTexturingProperty,
    char *szType,
    bool &bNeedsAlphaProperty)
{
    int iTextureID;
    if (!DtTextureGetIDMulti( iShapeID, iGroupID, szType, &iTextureID ))
    {
        NIASSERT(0);
        return false;
    }

    if (iTextureID < 0)
    {
        //? pMultiTextureProperty = NULL;
        return true;
    }

    char *pTextureName;
    if (!DtTextureGetNameID(iTextureID,&pTextureName))
    {
        NIASSERT(0);
        return false;
    }


    // Assign the Texture to our NiTexturingProperty;
    NiTexturingProperty::Map *pMap = NULL;
    bool bShaderMap = false;
    
    bool bBumpMap = strcmp(szType, "BumpMap") == 0;
    bool bParallaxMap = strcmp(szType, "ParallaxMap") == 0;

    if (!bBumpMap && !bParallaxMap)
    {
        if(strlen(szType) < strlen("Ni_PixelShaderAttribute_"))
            pMap = NiNew NiTexturingProperty::Map();
        else
        {
            pMap = (NiTexturingProperty::Map*)NiNew 
                NiTexturingProperty::ShaderMap();

            bShaderMap = true;
        }
    }
    else if (bBumpMap)
    {
        pMap = (NiTexturingProperty::Map*)(
            NiNew NiTexturingProperty::BumpMap());
    }
    else if (bParallaxMap)
    {
        pMap = (NiTexturingProperty::Map*)(
            NiNew NiTexturingProperty::ParallaxMap());
    }

    // Cube Maps are only valid in the shader slot
    if (NiIsKindOf(NiSourceCubeMap, 
        pkTextureManager->GetTexture(iTextureID)) && !bShaderMap)
    {
        DtExt_Err(
            "ERROR: Texture %s: Is a Cube Map Texture in the %s. Cube "
            "Maps are only valid in as Dynamic Effects or Shader Maps.\n", 
            pTextureName, szType);
        NiDelete pMap;
        return false;
    }

    pMap->SetTexture( pkTextureManager->GetTexture( iTextureID ) );

    // If any of the textures have Alpha then we need and Alpha property
    bNeedsAlphaProperty |= pkTextureManager->NeedsAlphaProperty(iTextureID);

    if (pTexturingProperty == NULL)
        pTexturingProperty = NiNew NiTexturingProperty;

    int iUVIndex = AssignUVs( iShapeID, iGroupID, pTextureName, iTextureID);

    // If we optimized the UV then use that UV set else use the one we just
    // created.
    if (iUVIndex >= 0)
    {
        pMap->SetTextureIndex((unsigned int)iUVIndex);
    }
    else
    {
        return false;
    }

    // handle animated UVs
    int iMtlID;
    DtMtlGetID( iShapeID, iGroupID, &iMtlID);

    NiTextureTransform* pkTexTransform = NULL;
    NiTextureTransformController* pkTexControllerTU = NULL;
    NiTextureTransformController* pkTexControllerTV = NULL;
    NiTextureTransformController* pkTexControllerSU = NULL;
    NiTextureTransformController* pkTexControllerSV = NULL;
    NiTextureTransformController* pkTexControllerR = NULL;

    GetTextureTransform(iMtlID, szType, "repeatU", pkTexTransform,
        pkTexControllerSU, NiTextureTransformController::TT_SCALE_U, false);

    GetTextureTransform(iMtlID, szType, "repeatV", pkTexTransform,
        pkTexControllerSV, NiTextureTransformController::TT_SCALE_V,
        false);

    GetTextureTransform(iMtlID, szType, "offsetU", pkTexTransform,
        pkTexControllerTU, NiTextureTransformController::TT_TRANSLATE_U, 
        false);

    GetTextureTransform(iMtlID, szType, "offsetV", pkTexTransform,
        pkTexControllerTV, NiTextureTransformController::TT_TRANSLATE_V,
        false);

    GetTextureTransform(iMtlID, szType, "rotateUV", pkTexTransform,
        pkTexControllerR, NiTextureTransformController::TT_ROTATE, false);

    if (!pkTexTransform)
    {
        CreateDefaultTextureTransform(iMtlID, szType, pkTexTransform);
    }

    // Get the Repeat values (Does texture repeat or clamp?)
    // Check against DT_REPEAT, DT_CLAMP
    NiTexturingProperty::ClampMode eClampMode = GetClampMode(pTextureName);

    if (eClampMode != NiTexturingProperty::WRAP_S_WRAP_T)
    {
        pMap->SetClampMode(eClampMode);
    }

    // APPLY THE FILTER MODE AND L AND K VALUES FROM THE MATERIAL
    char cFilterMode;
    char cMap;
    unsigned short usMaxAnisotropy = 1;

    // Assign the Filter Mode based on the global settings
    pMap->SetFilterMode( (NiTexturingProperty::FilterMode)
        gExport.m_iFilterType );

    // Check for Material Properties
    if (DtTextureGetMaterialMapProperties(
            iShapeID, 
            iGroupID, 
            szType, 
            cFilterMode, 
            cMap, 
            usMaxAnisotropy))
    {
        // Only set the Filter mode if it's not the default value
        if(cFilterMode != -1)
        {
            pMap->SetFilterMode(
                (NiTexturingProperty::FilterMode)cFilterMode);
            pMap->SetMaxAnisotropy(usMaxAnisotropy);
        }
    }


    // APPLY THE Map AND check for L and K extra attributes
    char* pcMaterialName;
    DtMtlGetName(iShapeID, iGroupID, &pcMaterialName);
    
    if (strcmp(szType,"color")==0)
    {
        pTexturingProperty->SetBaseMap(pMap);
    }
    else if (strcmp(szType,"DarkMap")==0)
    {
        pTexturingProperty->SetDarkMap(pMap);
    }
    else if (strcmp(szType,"DetailMap")==0)
    {
        pTexturingProperty->SetDetailMap(pMap);
    }
    else if (strcmp(szType,"Decal1Map")==0)
    {
        pTexturingProperty->SetDecalMap(0, pMap);
    }
    else if (strcmp(szType,"Decal2Map")==0)
    {
        pTexturingProperty->SetDecalMap(1, pMap);
    }
    else if (strcmp(szType,"GlowMap")==0)
    {
        pTexturingProperty->SetGlowMap(pMap);
    }
    else if (strcmp(szType,"GlossMap")==0)
    {
        pTexturingProperty->SetGlossMap(pMap);
    }
    else if (strcmp(szType,"BumpMap")==0)
    {
        float fBumpMapLuminaScale = 0.0f;
        float fBumpMapLuminaOffset = 0.0f;
        float fBumpMapMat00 = 0.0f;
        float fBumpMapMat01 = 0.0f;
        float fBumpMapMat10 = 0.0f;
        float fBumpMapMat11 = 0.0f;

        DtTextureGetMaterialBumpMapProperties( iShapeID, iGroupID,
            fBumpMapLuminaScale, fBumpMapLuminaOffset, fBumpMapMat00, 
            fBumpMapMat01, fBumpMapMat10, fBumpMapMat11);

        ((NiTexturingProperty::BumpMap*)pMap)->SetLumaScale(
            fBumpMapLuminaScale);

        ((NiTexturingProperty::BumpMap*)pMap)->SetLumaOffset(
            fBumpMapLuminaOffset);

        ((NiTexturingProperty::BumpMap*)pMap)->SetBumpMat00(fBumpMapMat00);
        ((NiTexturingProperty::BumpMap*)pMap)->SetBumpMat01(fBumpMapMat01);
        ((NiTexturingProperty::BumpMap*)pMap)->SetBumpMat10(fBumpMapMat10);
        ((NiTexturingProperty::BumpMap*)pMap)->SetBumpMat11(fBumpMapMat11);

        pTexturingProperty->SetBumpMap((NiTexturingProperty::BumpMap*)pMap);
    }
    else if (strcmp(szType,"NormalMap")==0)
    {
        pTexturingProperty->SetNormalMap(pMap);
    }
    else if (strcmp(szType,"ParallaxMap")==0)
    {
        float fOffset;
        DtTextureGetMaterialParallaxMapProperties( iShapeID, iGroupID, 
            fOffset);
        ((NiTexturingProperty::ParallaxMap*)pMap)->SetOffset(fOffset);
        pTexturingProperty->SetParallaxMap((NiTexturingProperty::ParallaxMap*)
            pMap);
    }
    else
    {
        pTexturingProperty->SetShaderMap(cMap, 
            (NiTexturingProperty::ShaderMap*)pMap);
    }


    // Setup the Texture Transforms and Animations if necessary
    if (pkTexTransform)
    {
        pMap->SetTextureTransform(pkTexTransform);
        
        if (pkTexControllerTU)
        {
            pkTexControllerTU->SetMap(pMap, pTexturingProperty);
        }
        
        if (pkTexControllerTV)
        {
            pkTexControllerTV->SetMap(pMap, pTexturingProperty);
        }
        
        if (pkTexControllerSU)
        {
            pkTexControllerSU->SetMap(pMap, pTexturingProperty);
        }
        
        if (pkTexControllerSV)
        {
            pkTexControllerSV->SetMap(pMap, pTexturingProperty);
        }
        
        if (pkTexControllerR)
        {
            pkTexControllerR->SetMap(pMap, pTexturingProperty);
        }
    }

        // Create the Flip Book Properties for this texture
    CreateFlipBookProperty(iShapeID, iGroupID, iTextureID, 
            szType, pkTextureManager, pMap, pTexturingProperty);

    return true;
}
//---------------------------------------------------------------------------
bool MyiTriMesh::CreateMultiTextureForType(
    int iMaterialID,
    MyiTextureManager* pkTextureManager,
    NiTexturingProperty *&pTexturingProperty,
    char *szType,
    bool &bNeedsAlphaProperty)
{
    int iTextureID;
    if (!DtTextureGetIDMulti( iMaterialID, szType, &iTextureID ))
    {
        NIASSERT(0);
        return false;
    }

    if (iTextureID < 0)
    {
        //? pMultiTextureProperty = NULL;
        return true;
    }

    char *pTextureName;
    if (!DtTextureGetNameID(iTextureID,&pTextureName))
    {
        NIASSERT(0);
        return false;
    }


    // Assign the Texture to our NiTexturingProperty;
    NiTexturingProperty::Map *pMap = NULL;
    bool bShaderMap = false;
    
    bool bBumpMap = strcmp(szType, "BumpMap") == 0;
    bool bParallaxMap = strcmp(szType, "ParallaxMap") == 0;

    if (!bBumpMap && !bParallaxMap)
    {
        if(strlen(szType) < strlen("Ni_PixelShaderAttribute_"))
            pMap = NiNew NiTexturingProperty::Map();
        else
        {
            pMap = (NiTexturingProperty::Map*)NiNew 
                NiTexturingProperty::ShaderMap();

            bShaderMap = true;
        }
    }
    else if (bBumpMap)
    {
        pMap = (NiTexturingProperty::Map*)(
            NiNew NiTexturingProperty::BumpMap());
    }
    else if (bParallaxMap)
    {
        pMap = (NiTexturingProperty::Map*)(
            NiNew NiTexturingProperty::ParallaxMap());
    }


    // Cube Maps are only valid in the shader slot
    if (NiIsKindOf(NiSourceCubeMap, 
        pkTextureManager->GetTexture(iTextureID)) && !bShaderMap)
    {
        DtExt_Err(
            "ERROR: Texture %s: Is a Cube Map Texture in the %s. Cube "
            "Maps are only valid in as Dynamic Effects or Shader Maps.\n", 
            pTextureName, szType);
        NiDelete pMap;
        return false;
    }


    pMap->SetTexture( pkTextureManager->GetTexture( iTextureID ) );

    // Always use the base texture UVs
    pMap->SetTextureIndex( 0 );

    // If we do not have a shape id this means a particle which is maped 0-1.
    NiPoint2 kCenterUV(0.5f, 0.5f);

    // If any of the textures have Alpha then we need and Alpha property
    bNeedsAlphaProperty |= pkTextureManager->NeedsAlphaProperty(iTextureID);

    if (pTexturingProperty == NULL)
        pTexturingProperty = NiNew NiTexturingProperty;

    // handle animated UVs
    NiTextureTransform* pkTexTransform = NULL;
    NiTextureTransformController* pkTexControllerTU = NULL;
    NiTextureTransformController* pkTexControllerTV = NULL;
    NiTextureTransformController* pkTexControllerSU = NULL;
    NiTextureTransformController* pkTexControllerSV = NULL;
    NiTextureTransformController* pkTexControllerR = NULL;

    GetTextureTransform(iMaterialID, szType, "repeatU", pkTexTransform,
        pkTexControllerSU, NiTextureTransformController::TT_SCALE_U, false);

    GetTextureTransform(iMaterialID, szType, "repeatV", pkTexTransform,
        pkTexControllerSV, NiTextureTransformController::TT_SCALE_V, false);

    GetTextureTransform(iMaterialID, szType, "offsetU", pkTexTransform,
        pkTexControllerTU, NiTextureTransformController::TT_TRANSLATE_U,
        false);

    GetTextureTransform(iMaterialID, szType, "offsetV", pkTexTransform,
        pkTexControllerTV, NiTextureTransformController::TT_TRANSLATE_V,
        false);

    GetTextureTransform(iMaterialID, szType, "rotateUV", pkTexTransform,
        pkTexControllerR, NiTextureTransformController::TT_ROTATE, false);

    if (!pkTexTransform)
    {
        CreateDefaultTextureTransform(iMaterialID, szType, pkTexTransform);
    }

    // Get the Repeat values (Does texture repeat or clamp?)
    // Check against DT_REPEAT, DT_CLAMP
    NiTexturingProperty::ClampMode eClampMode = GetClampMode(pTextureName);

    if (eClampMode != NiTexturingProperty::WRAP_S_WRAP_T)
    {
        pMap->SetClampMode(eClampMode);
    }



    // APPLY THE FILTER MODE FROM THE MATERIAL
    char cFilterMode;
    char cMap;
    unsigned short usMaxAnisotropy = 1;


    // Assign the Filter Mode based on the global settings
    pMap->SetFilterMode( 
        (NiTexturingProperty::FilterMode)gExport.m_iFilterType );

    // Check for Material Properties
    if (DtTextureGetMaterialMapProperties(
            iMaterialID, 
            szType, 
            cFilterMode, 
            cMap,
            usMaxAnisotropy))
    {
        // Only set the Filter mode if it's not the default value
        if (cFilterMode != -1)
        {
            pMap->SetFilterMode((NiTexturingProperty::FilterMode)cFilterMode);
            pMap->SetMaxAnisotropy(usMaxAnisotropy);
        }
    }


    // APPLY THE Map 
    char* pcMaterialName;
    DtMtlGetNameByID(iMaterialID, &pcMaterialName);
    
    if (strcmp(szType,"color")==0)
    {
        pTexturingProperty->SetBaseMap(pMap);
    }
    else if (strcmp(szType,"DarkMap")==0)
    {
        pTexturingProperty->SetDarkMap(pMap);
    }
    else if (strcmp(szType,"DetailMap")==0)
    {
        pTexturingProperty->SetDetailMap(pMap);
    }
    else if (strcmp(szType,"Decal1Map")==0)
    {
        pTexturingProperty->SetDecalMap(0, pMap);
    }
    else if (strcmp(szType,"Decal2Map")==0)
    {
        pTexturingProperty->SetDecalMap(1, pMap);
    }
    else if (strcmp(szType,"GlowMap")==0)
    {
        pTexturingProperty->SetGlowMap(pMap);
    }
    else if (strcmp(szType,"GlossMap")==0)
    {
        pTexturingProperty->SetGlossMap(pMap);
    }
    else if (strcmp(szType,"BumpMap")==0)
    {
        float fBumpMapLuminaScale = 0.0f;
        float fBumpMapLuminaOffset = 0.0f;
        float fBumpMapMat00 = 0.0f;
        float fBumpMapMat01 = 0.0f;
        float fBumpMapMat10 = 0.0f;
        float fBumpMapMat11 = 0.0f;

        DtTextureGetMaterialBumpMapProperties( iMaterialID,
            fBumpMapLuminaScale, fBumpMapLuminaOffset, fBumpMapMat00, 
            fBumpMapMat01, fBumpMapMat10, fBumpMapMat11);

        ((NiTexturingProperty::BumpMap*)pMap)->SetLumaScale(
            fBumpMapLuminaScale);

        ((NiTexturingProperty::BumpMap*)pMap)->SetLumaOffset(
            fBumpMapLuminaOffset);

        ((NiTexturingProperty::BumpMap*)pMap)->SetBumpMat00(fBumpMapMat00);
        ((NiTexturingProperty::BumpMap*)pMap)->SetBumpMat01(fBumpMapMat01);
        ((NiTexturingProperty::BumpMap*)pMap)->SetBumpMat10(fBumpMapMat10);
        ((NiTexturingProperty::BumpMap*)pMap)->SetBumpMat11(fBumpMapMat11);

        pTexturingProperty->SetBumpMap((NiTexturingProperty::BumpMap*)pMap);
    }
    else if (strcmp(szType,"NormalMap")==0)
    {
        pTexturingProperty->SetNormalMap(pMap);
    }
    else if (strcmp(szType,"ParallaxMap")==0)
    {
        float fOffset;
        DtTextureGetMaterialParallaxMapProperties( iMaterialID, 
            fOffset);
        ((NiTexturingProperty::ParallaxMap*)pMap)->SetOffset(fOffset);
        pTexturingProperty->SetParallaxMap((NiTexturingProperty::ParallaxMap*)
            pMap);
    }
    else
    {
        pTexturingProperty->SetShaderMap(cMap, 
            (NiTexturingProperty::ShaderMap*)pMap);
    }


    // Setup the Texture Transforms and Animations if necessary
    if (pkTexTransform)
    {
        pMap->SetTextureTransform(pkTexTransform);
        
        if (pkTexControllerTU)
        {
            pkTexControllerTU->SetMap(pMap, pTexturingProperty);
        }
        
        if (pkTexControllerTV)
        {
            pkTexControllerTV->SetMap(pMap, pTexturingProperty);
        }
        
        if (pkTexControllerSU)
        {
            pkTexControllerSU->SetMap(pMap, pTexturingProperty);
        }
        
        if (pkTexControllerSV)
        {
            pkTexControllerSV->SetMap(pMap, pTexturingProperty);
        }
        
        if (pkTexControllerR)
        {
            pkTexControllerR->SetMap(pMap, pTexturingProperty);
        }
    }

        // Create the Flip Book Properties for this texture
    CreateFlipBookProperty(iMaterialID, iTextureID, 
            szType, pkTextureManager, pMap, pTexturingProperty);

    return true;
}
//---------------------------------------------------------------------------
int MyiTriMesh::AssignUVs(int iShapeID, int iGroupID, char *pTextureName,
    int iTextureID)
{
    NI_UNUSED_ARG(pTextureName);
    NI_UNUSED_ARG(iGroupID);
    // Find the correct, previously exported UV set
    MObject kTextureNode;
    DtTextureGetTextureNodeID( iTextureID, kTextureNode);

    char pcUVSetName[256] = "map1";
    DtShapeGetUVSetNameForTexture( iShapeID, kTextureNode, pcUVSetName,
        256);

    int iSetIndex = 0;
    NiBool isValid = m_UVNameToIdMap.GetAt(pcUVSetName, iSetIndex);
    
    //There is really nothing we can do if this fails but error or warn
    NIASSERT(isValid);

    return iSetIndex;
}
//---------------------------------------------------------------------------
bool MyiTriMesh::HasAnimatedTextureTransform(NiTexturingProperty* pkProperty,
    NiTexturingProperty::Map* pkMap)
{
    if (!pkProperty || !pkMap)
        return false;

    NiTimeController* pkController = pkProperty->GetControllers();

    while (pkController)
    {
        if (NiIsKindOf(NiTextureTransformController, pkController))
        {
            NiTextureTransformController* pkTTCont = 
                (NiTextureTransformController*) pkController;

            NIASSERT(pkTTCont->GetMap() != NULL);

            if (pkTTCont->GetMap() == pkMap)
                return true;
        }
        pkController = pkController->GetNext();
    }

    return false;
}
//---------------------------------------------------------------------------
void MyiTriMesh::BakeUVs(NiTexturingProperty* pkTexProp)
{

    if (!pkTexProp || m_uiNiVertices == 0)
        return;

    // Build an array containing all maps in this property.
    NiTPrimitiveArray<NiTexturingProperty::Map*> kMapArray;
    const NiTexturingProperty::NiMapArray& kTempMaps = pkTexProp->GetMaps();
    unsigned int uiTempMapCount = kTempMaps.GetSize();
    for (unsigned int ui = 0; ui < uiTempMapCount; ui++)
    {
        if (kTempMaps.GetAt(ui))
        {
            kMapArray.Add(kTempMaps.GetAt(ui));
        }
    }

    for (unsigned int uj = 0; uj < pkTexProp->GetShaderArrayCount(); uj++)
    {
        if (pkTexProp->GetShaderMap(uj))
            kMapArray.Add(pkTexProp->GetShaderMap(uj));
    }

    // For each map, check to see if it uniquely points to a UV coordinate set
    // or if the UV transform is the same for ALL of them.
    // If so and the texture transform is NOT animated, get its texture 
    // transform matrix and apply it to all UV's in that set and then remove 
    // the texture transform. Otherwise, we will need to keep the texture
    // transforms around.


    unsigned int uiMapSize = kMapArray.GetSize();
    for (unsigned int uiIdx = 0; uiIdx < uiMapSize; uiIdx++)
    {
        NiTexturingProperty::Map* pkMap = kMapArray.GetAt(uiIdx);
        if (pkMap)
        {
            unsigned int uiMapUVSet = pkMap->GetTextureIndex();
            NiTextureTransform* pkTexTransform = pkMap->GetTextureTransform();
            bool bAnimated = HasAnimatedTextureTransform(pkTexProp, pkMap);
            if (bAnimated)
                continue;

            bool bUnique = true;

            for (unsigned int uiCheckIdx = 0; uiCheckIdx < uiMapSize; 
                uiCheckIdx++)
            {
                // Don't compare against yourself
                if (uiCheckIdx != uiIdx)
                {
                    NiTexturingProperty::Map* pkOtherMap = 
                        kMapArray.GetAt(uiCheckIdx);
                    
                    if (!pkOtherMap)
                        continue;

                    NIASSERT(pkOtherMap != pkMap);
                    NiTextureTransform* pkOtherTexTransform = 
                        pkOtherMap->GetTextureTransform();
                    unsigned int uiOtherUVSet = pkOtherMap->GetTextureIndex();

                    if (uiMapUVSet == uiOtherUVSet)
                    {
                        if (HasAnimatedTextureTransform(pkTexProp, pkOtherMap))
                        {
                            bUnique = false;
                            break;
                        }
                        else if ((pkOtherTexTransform != NULL && 
                            pkTexTransform == NULL) ||
                            (pkOtherTexTransform == NULL && 
                            pkTexTransform != NULL))
                        {
                            bUnique = false;
                            break;
                        }
                        else if (pkOtherTexTransform != NULL && 
                          pkTexTransform != NULL && 
                          *(pkOtherTexTransform) != *(pkTexTransform))
                        {
                            bUnique = false;
                            break;
                        }
                    }
                }
            }

            if (!bUnique)
                continue;

            // We know that we have a unique mapping, bake the UV's
            if (pkTexTransform != NULL)
            {
                NiMatrix3 kMatrix = *(pkTexTransform->GetMatrix());

                    NiDataStreamElementLock kLock(m_spMesh, 
                        NiCommonSemantics::TEXCOORD(), uiMapUVSet,    
                        NiDataStreamElement::F_FLOAT32_2, 
                        NiDataStream::LOCK_TOOL_READ |  
                         NiDataStream::LOCK_TOOL_WRITE); 

                    // If the lock is not valid, then vertex positions 
                    // were not present on the mesh.
                    NiBool bIsLocked = kLock.IsLocked();
                    NIASSERT(bIsLocked); 

                    if(!bIsLocked)
                    {
                        //probably the uiMapUVset is bad
                        //just try the next one
                        continue;
                    }

                    NiTStridedRandomAccessIterator<NiPoint2> kBegin = 
                        kLock.begin<NiPoint2>();            
                    NiTStridedRandomAccessIterator<NiPoint2> kEnd = 
                        kLock.end<NiPoint2>();

                    for (NiTStridedRandomAccessIterator<NiPoint2> kIter = 
                        kBegin; kIter != kEnd; ++kIter)
                    {
                        NiPoint2 kUV = *kIter;

                        NiPoint3 kPt(kUV.x,
                            kUV.y, 1.0f);
                        kPt = kMatrix * kPt;
                        kUV.x = kPt.x;
                        kUV.y = kPt.y;

                        *kIter = kUV;
                    }

                // Because the TextureTransform may have been the
                // same on multiple maps, we need to set ALL back to NULL
                for (unsigned int uk = 0; uk < uiMapSize; uk++)
                {
                    NiTexturingProperty::Map* pkTestMap = kMapArray.GetAt(uk);
                    if (pkTestMap && pkTestMap->GetTextureIndex() == 
                        uiMapUVSet)
                    {
                        NiTextureTransform* pkOldTT = 
                            pkTestMap->GetTextureTransform();
                        pkTestMap->SetTextureTransform(NULL);
                        NiDelete pkOldTT;
                    }
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
NiPoint3* MyiTriMesh::GetUndeformedBaseVertices(
    const MFnBlendShapeDeformer& kBlendShape, 
    const unsigned int* puiNewToOld, const int* piFaceList)
{
    // get undeformed base mesh object from blendshape deformer
    MFnMesh kMesh = GetBaseMesh(kBlendShape);

    // get vertices from mesh object
    MPointArray kPointArray;
    kMesh.getPoints(kPointArray);

    // convert to exploded, collapsed, undeformed verts
    return GetCollapsedVertices(kPointArray, puiNewToOld, piFaceList);

}
//---------------------------------------------------------------------------
NiPoint3* MyiTriMesh::GetCollapsedVertices(
    const MPointArray& kPointArray, 
    const unsigned int* puiNewToOld, const int* piFaceList)
{
    // get exploded, collapsed, undeformed verts 
   
    unsigned int uiVertices = m_spMesh->GetVertexCount();
    NiPoint3* pkVertices = NiNew NiPoint3[uiVertices];

    int iPointArrayLen = (int)kPointArray.length();

    for (unsigned int ui = 0; ui < uiVertices; ui++)
    {
        unsigned int uiExploded = puiNewToOld[ui];
        // faces are separated by -1.  assume 3 verts per face.
        uiExploded = uiExploded + (uiExploded / 3);
        int iOriginalIndex = piFaceList[uiExploded];
        NIASSERT(iOriginalIndex != DtEND_OF_FACE);
        NIASSERT(iOriginalIndex < (int)kPointArray.length());

        // Check for overflowing the array
        if (iOriginalIndex >= iPointArrayLen)
        {
            // An error has been detected. Return NULL.
            NiDelete[] pkVertices;
            return NULL;
        }

        MPoint kPoint = kPointArray[iOriginalIndex];


        pkVertices[ui].x = (float)kPoint.x;
        pkVertices[ui].y = (float)kPoint.y;
        pkVertices[ui].z = (float)kPoint.z;

        if(gExport.m_bUseCurrentWorkingUnits && 
            (gExport.m_fLinearUnitMultiplier != 1.0f) )
        {
            pkVertices[ui].x *= gExport.m_fLinearUnitMultiplier;
            pkVertices[ui].y *= gExport.m_fLinearUnitMultiplier;
            pkVertices[ui].z *= gExport.m_fLinearUnitMultiplier;
        }
        
    }


    return pkVertices;
}
//---------------------------------------------------------------------------
bool MyiTriMesh::GetVertsForNiAsArray(int iShapeID, int iGroupID,
    NiPoint3*& pkPosition, unsigned int& uiNumVertices)
{
    DtVec3f *dtVertexList;

    // Get the Vertices from the MDtAPI... actually,
    // DtGroupGetExplodedVertices is a custom function that
    // Explodes the shape so that each index references a 
    // unique index.
    //
    // The reason this was needed is because the NiMesh
    // constructor requires that the number of UV coords equal
    // the number of model coords.  In our case, the UV coords
    // were greater because the model coords were shared.
    int iverts;
    if (!DtGroupGetExplodedVertices( iShapeID, iGroupID, &iverts, 
        &dtVertexList ))
    {
        NIASSERT(0);
        const char *szShapeName;
        DtShapeGetShapeName( iShapeID, &szShapeName );
        DtExt_Err("ERROR: Unable to explode vertices from object [%s].\n",
            szShapeName);
        return false;
    }

    uiNumVertices = iverts;

    // Now we need to Translate the dtVertexList to our Ni format.
    NIASSERT(uiNumVertices > 0);

    float fScaleX = m_fScale[0];
    float fScaleY = m_fScale[1];
    float fScaleZ = m_fScale[2];

    // Check for the current working units
    if(gExport.m_bUseCurrentWorkingUnits && (
        gExport.m_fLinearUnitMultiplier != 1.0f) )
    {
        fScaleX *= gExport.m_fLinearUnitMultiplier;
        fScaleY *= gExport.m_fLinearUnitMultiplier;
        fScaleZ *= gExport.m_fLinearUnitMultiplier;
    }

    pkPosition = NiNew NiPoint3[uiNumVertices];

    // Modify by the scale and linear unit multiplier
    for (unsigned int i=0; i < uiNumVertices; i++)
    {
        pkPosition[i].x = dtVertexList[i].vec[0] * fScaleX;
        pkPosition[i].y = dtVertexList[i].vec[1] * fScaleY;
        pkPosition[i].z = dtVertexList[i].vec[2] * fScaleZ;
    }

    // Deallocate memory for dtVertexList
    NiFree(dtVertexList); 
    dtVertexList = NULL;
    
    return true;
}
//---------------------------------------------------------------------------
MFnMesh MyiTriMesh::GetBaseMesh(const MFnBlendShapeDeformer& kBlendShape)
{
    MPlug kInput = kBlendShape.findPlug("input");

    // currently only support 1 group 
    MPlug kInputMesh = kInput[0].child(0);

    MPlugArray kPlugArr;
    MStatus kStatus;
    kInputMesh.connectedTo(kPlugArr, true, false, &kStatus);

    NIASSERT(kPlugArr.length() == 1);

    MObject kDestObj = kPlugArr[0].node();

    // find mesh object
    MItDependencyGraph::Direction direction = MItDependencyGraph::kUpstream;
    MItDependencyGraph::Traversal traversalType = 
        MItDependencyGraph::kBreadthFirst;

    MItDependencyGraph::Level level = MItDependencyGraph::kNodeLevel;
    MFn::Type filter = MFn::kMesh;

    MItDependencyGraph dgIter( kDestObj, filter, direction,
                            traversalType, level, &kStatus);
                            
    // return first kMesh object found 
    if (!dgIter.isDone())
    {        
        MObject kMeshObj = dgIter.thisNode( &kStatus );
        MFnMesh kMesh(kMeshObj);
        NIASSERT(kMesh.isIntermediateObject());
        return kMesh;
    }
    else
    {
        MFnMesh kNone;
        return kNone;
    }
}
//---------------------------------------------------------------------------
bool MyiTriMesh::GetBlendShape(int iShapeID, int iGroupID, 
    MObject& BlendShape)
{
    NI_UNUSED_ARG(iGroupID);
    MStatus kStatus;

    // Get Shape Node
    MObject kShapeNode;
    if (!DtExt_ShapeGetShapeNode(iShapeID, kShapeNode))
    {
        return false;
    }
    
    // Look for an MFnBlendShapeDeformer attached to the inMesh plug
    // of the shape

    MFnDependencyNode kShapeDepend(kShapeNode, &kStatus);
    MPlug kMapPlug = kShapeDepend.findPlug("inMesh");

    if (!kMapPlug.isConnected())
    {
        return false;
    }

    MPlugArray kPlugArr;
    kMapPlug.connectedTo(kPlugArr, true, false, &kStatus);

    // I think the apiType of kMesh guarantees
    // that there will only be one connection, but
    // I'm not sure.  EH.
    if (kPlugArr.length() != 1)
        return false;

    bool bMeshError = false;
    bool bFirstModifier = true;

    // Get the type of this node
    MFn::Type kType = kPlugArr[0].node().apiType();

    // Skip over any polyModifiers, Skin Clusters, or group parts
    while ((kType == MFn::kPolyTweakUV) ||
           (kType == MFn::kPolyTriangulate) ||
           (kType == MFn::kPolyCollapseEdge) ||
           (kType == MFn::kPolyCollapseF) ||
           (kType == MFn::kPolyDelVertex) ||
           (kType == MFn::kPolySplitEdge) ||
           (kType == MFn::kPolyCloseBorder) ||
           (kType == MFn::kPolyDelFacet) ||
           (kType == MFn::kPolyMapCut) ||
           (kType == MFn::kPolyMapDel) ||
           (kType == MFn::kPolyMapSew) ||
           (kType == MFn::kPolySmooth) ||
           (kType == MFn::kPolyDelEdge) ||
           (kType == MFn::kPolyFlipEdge) ||
           (kType == MFn::kPolyAppendVertex) ||
           (kType == MFn::kPolyMergeFacet) ||
           (kType == MFn::kPolyNormal) ||
           (kType == MFn::kPolySubdFacet) ||
           (kType == MFn::kPolyTransfer) ||
           (kType == MFn::kPolyTweak) ||
#ifndef MAYA50
           (kType == MFn::kPolySmoothProxy) ||
#endif
           (kType == MFn::kPolySmoothFacet) ||
           (kType == MFn::kPolyReduce) ||
           (kType == MFn::kPolyAppend) ||
           (kType == MFn::kPolyMergeEdge) ||
           (kType == MFn::kPolyColorPerVertex) ||
           (kType == MFn::kPolyMoveUV) ||
           (kType == MFn::kPolyNormalPerVertex) ||
           (kType == MFn::kPolySplit) ||
           (kType == MFn::kJointCluster) ||
           (kType == MFn::kSkinClusterFilter) ||
           (kType == MFn::kGroupParts) ||
           (kType == MFn::kDeleteComponent) ||
           (kType == MFn::kPolySoftEdge))

    {
        char* pcPlugName = NULL;

        // determine the input plug name
        switch (kType)
        {
        case MFn::kPolyTweakUV:
        case MFn::kPolyMapCut:
        case MFn::kPolyMapDel:
        case MFn::kPolyMapSew:
        case MFn::kPolySmooth:
        case MFn::kPolyNormal:
        case MFn::kPolyTweak:
#ifndef MAYA50
        case MFn::kPolySmoothProxy:
#endif
        case MFn::kPolySmoothFacet:
        case MFn::kPolyMergeEdge:
        case MFn::kPolyColorPerVertex:
        case MFn::kPolyMoveUV:
        case MFn::kPolyNormalPerVertex:
            pcPlugName = "inputPolymesh";
            break;
        case MFn::kJointCluster:
        case MFn::kSkinClusterFilter:
        case MFn::kGroupParts:
            pcPlugName = "inputGeometry";
            break;
        case MFn::kPolyTriangulate:
        case MFn::kPolyCollapseEdge:
        case MFn::kPolyCollapseF:
        case MFn::kPolyDelVertex:
        case MFn::kPolySplitEdge:
        case MFn::kPolyCloseBorder:
        case MFn::kPolyDelFacet:
        case MFn::kPolyDelEdge:
        case MFn::kPolyFlipEdge:
        case MFn::kPolyAppendVertex:
        case MFn::kPolyMergeFacet:
        case MFn::kPolySubdFacet:
        case MFn::kPolyTransfer:
        case MFn::kPolyReduce:
        case MFn::kPolyAppend:
        case MFn::kPolySplit:
        case MFn::kPolySoftEdge:
            pcPlugName = "inputPolymesh";

            // Skip over the first modifier if it is a poly tranguate
            // because it was created during export.
            if (!bFirstModifier || 
                (kType != MFn::kPolyTriangulate))
                bMeshError = true;
            break;
        case MFn::kDeleteComponent:
            pcPlugName = "inputGeometry";
            bMeshError = true;
            break;
        default:
            return false;
        }


        MFnDependencyNode kModifier(kPlugArr[0].node());

        kMapPlug = kModifier.findPlug(pcPlugName);

        kMapPlug.connectedTo(kPlugArr, true, false, &kStatus);

        if (kPlugArr.length() != 1)
            return false;

        // Get the type again.
        kType = kPlugArr[0].node().apiType();
        bFirstModifier = false;
    }



    // Make sure we are looking at a blend shape.
    if (kPlugArr[0].node().apiType() == MFn::kBlendShape)
    {
        BlendShape = kPlugArr[0].node();
        MFnDependencyNode BlendShapeNode(BlendShape, &kStatus);

        if (bMeshError)
        {
            const char *szShapeName;
            DtShapeGetShapeName( iShapeID, &szShapeName);
            DtExt_Err("ERROR: Object %s: The object's history contains " \
                "changes to the number of vertices or faces after you " \
                "created the blend shape. Creating the blend shape has been "\
                "aborted.\n", szShapeName);

        }

        return (kStatus == MS::kSuccess && !bMeshError);
    }

    return false;
}
//---------------------------------------------------------------------------
void MyiTriMesh::AttachMorpherControllers(int iShapeID, int iGroupID,
        const unsigned int* puiNewToOld, MStatus* pkStatus)
{

    MObject kDestObj;

    if(!GetBlendShape(iShapeID, iGroupID, kDestObj)) 
    {
        if (pkStatus)
            *pkStatus = MS::kSuccess;
        return;
    }

    const char *szShapeName;
    DtShapeGetShapeName( iShapeID, &szShapeName);

    MStatus kStatus;
    MFnBlendShapeDeformer kBlendShape(kDestObj, &kStatus);

    if (!kStatus)
    {
        // Shape not connected to a MFnBlendShapeDeformer
        *pkStatus = MS::kSuccess;
        return;
    }

    MObjectArray kBaseArray;
    kBlendShape.getBaseObjects(kBaseArray);
    unsigned int uiNumBaseObjects = kBaseArray.length();

    if (uiNumBaseObjects == 0)
    {
        // Error From GetVertsForNi
        DtExt_Err("ERROR:: Object %s: Associated blend shape has no base "
            "objects\n.", szShapeName);

        if (pkStatus)
        {
            // No base objects on MFnBlendShapeDeformer
                *pkStatus = MS::kSuccess;
        }
        return;
    }

    // count targets
    unsigned int uiNumNiTargets = 0;
    MObjectArray kTargetArray;
    unsigned int uiNumWeights = kBlendShape.numWeights();

    MPlug kInputTargetArray = kBlendShape.findPlug("inputTarget");
    unsigned int uiNumInputTargets = kInputTargetArray.numElements();

    if (!uiNumInputTargets)
    {
        // Error From GetVertsForNi
        DtExt_Err("ERROR:: Object %s: Associated blend shape has no target "
            "objects\n.", szShapeName);
        return;
    }
    
    unsigned int uiInputTargetChildren = kInputTargetArray[0].numChildren();
    // not sure when this would be anything else
    NIASSERT(uiInputTargetChildren == 1);

    MPlug kInputTargetGroupArray = kInputTargetArray[0].child(0);

    unsigned int uiNumInputTargetGroupElts = 
        kInputTargetGroupArray.numElements();
    NIASSERT(uiNumInputTargetGroupElts == uiNumWeights);

    unsigned int i, j;
    for (i = 0; i < uiNumInputTargetGroupElts; i++)
    {
        MPlug kInputTargetGroup = kInputTargetGroupArray[i];
        unsigned int uiNumTargetGroups = kInputTargetGroup.numChildren();

        for (j = 0; j < uiNumTargetGroups; j++)
        {
            MPlug kInputTargetItemArray = kInputTargetGroup.child(j);

            // If there are any "in-between" targets, this will be > 1.
            unsigned int uiNumInputTargetItems = 
                kInputTargetItemArray.numElements();

            uiNumNiTargets += uiNumInputTargetItems;
        }
    }
    // done counting targets

    if (uiNumNiTargets == 0)
    {
        DtExt_Err("ERROR:: Object %s: Has No Blend Shape Targets.\n", 
            szShapeName);

        if (pkStatus)
        {
            // No targets on MFnBlendShapeDeformer
                *pkStatus = MS::kSuccess;
        }
        return;
    }

    // ************ BUILD THE TARGETS  *************

    // Increment the number of targets to include the base
    uiNumNiTargets++;

    unsigned int uiVertices = m_spMesh->GetVertexCount();

    // Create the Controller. We need it to set names for interpolators.
    NiMorphWeightsController* pkMorph =
        NiNew NiMorphWeightsController(uiNumNiTargets);
    pkMorph->SetAnimType(NiTimeController::APP_TIME);
    pkMorph->SetCycleType(NiTimeController::LOOP);

    // get exploded vertex map
    int* piFaceList;
    int iCount;
    dtPolygonGetFaceList (iShapeID, iGroupID,&piFaceList, &iCount);

    NiPoint3* pkBaseVertices = GetUndeformedBaseVertices(kBlendShape,
        puiNewToOld, piFaceList);

    if (pkBaseVertices == NULL)
    {
        // For one reason or another an error occurred.
        MString kBlendShapeName = kBlendShape.name();
        const char* pcBlendShapeName = kBlendShapeName.asChar();
        const char* pcShapeName;
        DtShapeGetShapeName( iShapeID, &pcShapeName);
        DtExt_Err("ERROR: Object %s: and it's BlendShape target %s have " \
            "different numbers of vertices.\n", pcShapeName, pcBlendShapeName);
        
        NiDelete pkMorph;
        
        return;
    }

    const char *szBaseName;
    DtShapeGetShapeName( iShapeID, &szBaseName );
    NiFixedString kBaseName(szBaseName);
    pkMorph->SetTargetName(0, kBaseName);

    // If at any point we bail out, there will be extra, unused streams
    // on the mesh. There is no easy way to deal with this, so leave it as is.

    // Check for skinning
    NiSkinningMeshModifier* pkSkinningModifier =
        NiGetModifier(NiSkinningMeshModifier, m_spMesh);
    bool bHasSkinModifier = (pkSkinningModifier != 0);

    NiFixedString kMorphDestSemantic = bHasSkinModifier ? 
        NiCommonSemantics::POSITION_BP() : NiCommonSemantics::POSITION();
    NiFixedString kMorphTargetSemantic = 
        NiMorphMeshModifier::MorphElementSemantic(kMorphDestSemantic);

    // Add stream for implied base target
    NiDataStreamRef* pkSR = m_spMesh->AddStream(kMorphTargetSemantic, 0, 
        NiDataStreamElement::F_FLOAT32_3, uiVertices, 
        NiDataStream::ACCESS_CPU_WRITE_STATIC | 
        NiDataStream::ACCESS_CPU_READ, NiDataStream::USAGE_VERTEX, 
        pkBaseVertices, true);

    unsigned int uiBaseNumExplodedVertices = m_uiNiVertices;

    // convert targets
    uiNumNiTargets = 1;
    for (i = 0; i < uiNumInputTargetGroupElts; i++)
    {
        MPlug kInputTargetGroup = kInputTargetGroupArray[i];

        MPlug kInputTargetItemArray = kInputTargetGroup.child(0);

        // If there are any "in-between" targets, this will be > 1.
        unsigned int uiNumInputTargetItems =
            kInputTargetItemArray.numElements();

        for (unsigned int j = 0; j < uiNumInputTargetItems; 
            j++, uiNumNiTargets++)
        {
            MPlug kInputTargetItem = kInputTargetItemArray[j];
            NIASSERT(kInputTargetItem.numChildren() == 3);

            // convert mesh
            NiPoint3* pkTargetVertices = 0;
            if (kInputTargetItem.child(0).isConnected())
            {
                MPlugArray kPlugArr;

                kInputTargetItem.child(0).connectedTo(
                    kPlugArr, true, false, pkStatus);

                if (pkStatus && *pkStatus != MS::kSuccess)
                {
                    // Error From GetVertsForNi
                    DtExt_Err("ERROR:: Object %s: Failed to extract Target"
                        " vert data\n", szShapeName);
        
                    NiDelete pkMorph;
        
                    return;
                }

                // I think the apiType of kMesh guarantees
                // that there will only be one connection, but
                // I'm not sure.  EH.
                NIASSERT(kPlugArr.length() == 1);

                int iTargetID = DtExt_FindShapeWithNode(kPlugArr[0].node());

                dtPolygonCopyFaceList(iTargetID, iShapeID, iGroupID);

                unsigned int uiNumVerts = 0;
                NiPoint3* pkPosition = 0;
                if (!GetVertsForNiAsArray(iTargetID, iGroupID, pkPosition,
                    uiNumVerts))
                {
                    const char *szTargetName;
                    DtShapeGetShapeName( iTargetID, &szTargetName );

                    // Error From GetVertsForNi
                    DtExt_Err("ERROR:: Object %s: Failed to extract vert data"
                        " for Target %s\n", szShapeName, szTargetName);

                    NiDelete[] pkPosition;
                    NiDelete pkMorph;
        
                    if (pkStatus)
                        *pkStatus = MS::kFailure;
                    return;
                }

                // Insure we have the same number of vertices
                if (uiNumVerts != uiBaseNumExplodedVertices)
                {
                    const char *szTargetName;
                    DtShapeGetShapeName( iTargetID, &szTargetName );
                    DtExt_Err("ERROR: Object %s: and it's morph target %s " \
                        "have different numbers of vertices. (%d / %d)\n", 
                        szShapeName, szTargetName, uiVertices, uiNumVerts);

                    NiDelete[] pkPosition;
                    NiDelete pkMorph;

                    if (pkStatus)
                        *pkStatus = MS::kFailure;
                    return;
                }


                // collapse verts using mapping from base mesh
                pkTargetVertices = NiNew NiPoint3[uiNumVerts];

                for (unsigned int uiVertexIndex = 0; 
                    uiVertexIndex < uiVertices; 
                    ++uiVertexIndex)
                {
                    pkTargetVertices[uiVertexIndex] = 
                        pkPosition[puiNewToOld[uiVertexIndex]] -
                        pkBaseVertices[uiVertexIndex];
                }
                
                // Create a Unique Target Name
                const char *szTargetName;
                DtShapeGetShapeName( iTargetID, &szTargetName );
                AddUniqueName(pkMorph, szTargetName, uiNumNiTargets); 

                NiDelete[] pkPosition;
            }
            else
            {
                // get point data array
                MObject kTemp;

                kInputTargetItem.child(1).getValue(kTemp);
                MFnPointArrayData kPointArrayData(kTemp);
                MPointArray kPointArray = kPointArrayData.array();

                pkTargetVertices = 
                    GetCollapsedVertices(kPointArray, puiNewToOld, piFaceList);

                if (pkTargetVertices == NULL)
                {
                    // An error occurred.
                    MString kInputTargetName = kInputTargetItem.name();
                    const char* pcInputTargetName = kInputTargetName.asChar();

                    const char* pcShapeName;
                    DtShapeGetShapeName( iShapeID, &pcShapeName);
                    DtExt_Err("ERROR: Object [%s] failed to extract deleted "
                        "BlendShape at input target [%s].\n", pcShapeName, 
                        pcInputTargetName);

                    NiDelete pkMorph;

                    if (pkStatus)
                        *pkStatus = MS::kFailure;
                    return;
                }

                AddUniqueName(pkMorph, "NotConnected", uiNumNiTargets); 
            }

            // Add stream for target
            NiDataStreamRef* pkSR=m_spMesh->AddStream(kMorphTargetSemantic, 
                uiNumNiTargets, NiDataStreamElement::F_FLOAT32_3, uiVertices, 
                NiDataStream::ACCESS_CPU_WRITE_STATIC | 
                NiDataStream::ACCESS_CPU_READ, NiDataStream::USAGE_VERTEX, 
                pkTargetVertices, true);
            NI_UNUSED_ARG(pkSR);

            NiDelete[] pkTargetVertices;
        }
    }

    // Add the weights stream
    float* pkWeights = NiAlloc(float, uiNumNiTargets);
    pkWeights[0] = 1.0f;
    for (unsigned int ui = 1; ui < uiNumNiTargets; ui++)
        pkWeights[ui] = 0.0f;

    pkSR = m_spMesh->AddStream(NiCommonSemantics::MORPHWEIGHTS(),
        0, NiDataStreamElement::F_FLOAT32_1, uiNumNiTargets, 
        NiDataStream::ACCESS_CPU_READ | NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
        NiDataStream::USAGE_USER, pkWeights, true);

    NiFree(pkWeights);

    NiDelete[] pkBaseVertices;

    bool bCalcNormals = GetMorphUpdateNormals(iShapeID);
    const NiFixedString& kNormSemantic = bHasSkinModifier ? 
        NiCommonSemantics::NORMAL_BP() : NiCommonSemantics::NORMAL();
    if (bCalcNormals)
    {
        NiMeshTools::GenerateNormalSharing(m_spMesh,
            kMorphDestSemantic, 0, kNormSemantic, 0);
    }

    // Set the access mask on the output data. It needs to be mutable so
    // that it can be morphed but not necessarily on every frame.        
    NiDataStreamRef* pkRef = m_spMesh->FindStreamRef(kMorphDestSemantic);
    
    NIASSERT(pkRef);
    NiDataStream* pkDS = pkRef->GetDataStream();

    NIASSERT(NiIsKindOf(NiToolDataStream, pkDS));
    NiToolDataStream* pkTDS = (NiToolDataStream*)pkDS;
    pkTDS->MakeWritable();
    pkTDS->SetCloningBehavior(NiObject::CLONE_BLANK_COPY);

    if (bCalcNormals)
    {
        pkRef = m_spMesh->FindStreamRef(NiCommonSemantics::INDEX());
        pkDS = pkRef->GetDataStream();
        NIASSERT(NiIsKindOf(NiToolDataStream, pkDS));
        NiToolDataStream* pkTDS = (NiToolDataStream*)pkDS;
        pkTDS->MakeReadable();

        pkRef = m_spMesh->FindStreamRef(kNormSemantic);
        pkDS = pkRef->GetDataStream();
        NIASSERT(NiIsKindOf(NiToolDataStream, pkDS));
        pkTDS = (NiToolDataStream*)pkDS;
        pkTDS->MakeReadable();
        pkTDS->MakeWritable();
        pkTDS->SetCloningBehavior(NiObject::CLONE_BLANK_COPY);
    }

    // Create the mesh modifier
    NiMorphMeshModifier* pkModifier =
        NiNew NiMorphMeshModifier(
        (NiUInt16)uiNumNiTargets);
    
    pkModifier->SetRelativeTargets(true);
    pkModifier->SetAlwaysUpdate(false);
    pkModifier->SetUpdateNormals(bCalcNormals);
    pkModifier->SetNeedsUpdate();
    pkModifier->SetSkinned(bHasSkinModifier);
    if(bHasSkinModifier)
    {
         pkModifier->SetSWSkinned(pkSkinningModifier->GetSoftwareSkinned());
    }
    else
    {
        pkModifier->SetSWSkinned(false);
    }
   

    // Specify which semantics it is morphing
    pkModifier->AddMorphedElement(kMorphDestSemantic, 0);
    
    // Add it to the mesh as the first modifier to guarantee it runs before 
    // the NiSkinningMeshModifier.
    m_spMesh->AddModifierAt(0, pkModifier, false);
    pkModifier->CalculateMorphBound(m_spMesh);

    // Have to set the target after the modifier has been added
    pkMorph->SetTarget(m_spMesh);

    // ***** BUILD THE WEIGHTS *****

    // weight keys for the implicit base
    NiLinFloatKey* pkKey = NiNew NiLinFloatKey[1];
    pkKey->SetTime(0.0f);
    pkKey->SetValue(1.0f);

    // Create the Data and Interpolator
    NiFloatData* pkFloatData = NiNew NiFloatData();
    pkFloatData->ReplaceAnim((NiFloatKey*)pkKey, 1, NiAnimationKey::LINKEY);

    NiFloatInterpolator* pkInterp = NiNew NiFloatInterpolator(pkFloatData);
    pkInterp->Collapse();

    // Add into the base morph slot
    pkMorph->SetInterpolator(pkInterp, 0);

    // get weight array plug
    MPlug kWeightPlug = kBlendShape.findPlug("weight");

    // convert targets
    uiNumNiTargets = 1;
    for (i = 0; i < uiNumInputTargetGroupElts; i++)
    {
        MPlug kInputTargetGroup = kInputTargetGroupArray[i];

        MPlug kInputTargetItemArray = kInputTargetGroup.child(0);

        // If there are any "in-between" targets, this will be > 1.
        unsigned int uiNumInputTargetItems =
            kInputTargetItemArray.numElements();

        // Handle The Inbetweens
        float fInBetweenWeightPrev = 0.0f;
        float fInBetweenWeight;
        float fInBetweenWeightNext = 0.0f;

        for (unsigned int j = 0; j < uiNumInputTargetItems; 
            j++)
        {
            MPlug kInputTargetItem = kInputTargetItemArray[j];
            NIASSERT(kInputTargetItem.numChildren() == 3);


            // Get the TargetPlug
            MPlug kTargetPlug = kWeightPlug[i];


            // Define our Key Arrays
            unsigned int uiNumKeys;
            NiFloatKey* pkKeys;
            NiAnimationKey::KeyType eType;


            // IF There is just one target then convert the animation directly
            if (uiNumInputTargetItems == 1)
            {
                // Make sure the target plug is a weight
                NIASSERT(kTargetPlug.attribute().apiType() == 
                    MFn::kNumericAttribute);

                if (ConvertFloatAnim(kTargetPlug, uiNumKeys, pkKeys, eType) != 
                    MS::kSuccess)
                {
                    double dValue = 1.0;

                    kTargetPlug.getValue(dValue);

                    // Just Create a Key
                    NiLinFloatKey* pkLinKey = NiNew NiLinFloatKey[1];
                    pkLinKey->SetTime(0.0f);
                    pkLinKey->SetValue((float)dValue);

                    uiNumKeys = 1;
                    pkKeys = (NiFloatKey*)pkLinKey;
                    eType = NiAnimationKey::LINKEY;
                }

            }
            else 
            {
                // Multiple Targets for this weight so we need to check 
                //inbetween values

                //
                // DETERMINE THE INBETWEEN WEIGHTS
                //
                // convert weight
                // index = wt * 1000 + 5000
                // wt = (index - 5000) / 1000

                unsigned char ucTargetType = 0;
                ucTargetType |= (j > 0);
                if (ucTargetType & 1)
                    fInBetweenWeightPrev = 
                    ((float)((int)kInputTargetItemArray[j-1].logicalIndex()
                    - 5000)) / 1000.0f;

                fInBetweenWeight = 
                    ((float)((int)kInputTargetItemArray[j].logicalIndex()
                    - 5000)) / 1000.0f;

                ucTargetType |= (j < uiNumInputTargetItems-1) * 2;
                if (ucTargetType & 2)
                {
                    fInBetweenWeightNext = 
                        ((float)((int)kInputTargetItemArray[j+1].logicalIndex()
                        - 5000)) / 1000.0f;
                }

                // use implied base target instead if it is closer
                if (fInBetweenWeight > 0.0f &&
                    (((ucTargetType & 1) && fInBetweenWeightPrev < 0.0f) ||
                    !(ucTargetType & 1)))
                {
                    fInBetweenWeightPrev = 0.0f;
                    ucTargetType |= 1;
                }
                else if (fInBetweenWeight < 0.0f &&
                    (((ucTargetType & 2) && fInBetweenWeightNext > 0.0f) ||
                    !(ucTargetType & 2)))
                {
                    fInBetweenWeightNext = 0.0f;
                    ucTargetType |= 2;
                }


                // Find the Min and Max times
                bool bAnimated;
                MFnAnimCurve kWeightAnimCurve;
                if (ConvertPlugToAnimCurve(kTargetPlug, bAnimated, 
                    kWeightAnimCurve) != MS::kSuccess)
                    return;

                if (kWeightAnimCurve.numKeyframes() == 0)
                    return;

                // Determine our sample info
                MTime kMinTime = kWeightAnimCurve.time(0);
                MTime kMaxTime = kWeightAnimCurve.time(
                    kWeightAnimCurve.numKeyframes() - 1);

                unsigned int uiNumSamples = 
                    (unsigned int)(kMaxTime.value() - kMinTime.value()) + 1;

                MTime kSampleDuration = (kMaxTime - kMinTime) / 
                    (double)uiNumSamples;

                // we sample this anim-curve dependent formula to
                // determine the weight keys for this target.

                NiLinFloatKey* pkLinKeys = NiNew NiLinFloatKey[uiNumSamples];


                for (unsigned int uiSample = 0;
                    uiSample < uiNumSamples; uiSample++)
                {

                    MTime kSampleTime = kMinTime + (kSampleDuration * 
                        (double)uiSample);

                    float fTargetWeight = 0.0f;

                    float fWeight = 
                        (float) kWeightAnimCurve.evaluate(kSampleTime);

                    switch (ucTargetType)
                    {
                    case 0:     // no next or prev target
                        // this will never happen because there's always
                        // the implied base target
                        NIASSERT(0);
                        break;
                    case 1:     // prev but no next target
                        {
                            if (fWeight <= fInBetweenWeightPrev)
                            {
                                fTargetWeight = 0.0f;
                            }
                            else if (fWeight >= fInBetweenWeight)
                            {
                                fTargetWeight = fWeight - fInBetweenWeightPrev;
                            }
                            else
                            {
                                NIASSERT(fWeight < fInBetweenWeight);

                                float fDelta = 
                                    fInBetweenWeight - fInBetweenWeightPrev;

                                fTargetWeight = 
                                    (fDelta + (fWeight - fInBetweenWeight)) / 
                                    fDelta;
                            }

                        }
                        break;
                    case 2:     // next but no prev target
                        {
                            if (fWeight >= fInBetweenWeightNext)
                            {
                                fTargetWeight = 0.0f;
                            }
                            else if (fWeight >= fInBetweenWeight)
                            {
                                float fDelta = 
                                    fInBetweenWeightNext - fInBetweenWeight;

                                fTargetWeight = 
                                    (fDelta - (fWeight - fInBetweenWeight)) / 
                                    fDelta;
                            }
                            else
                            {
                                NIASSERT(fWeight < fInBetweenWeight);
                                fTargetWeight = fInBetweenWeightNext - fWeight;
                            }
                        }
                        break;
                    default:    // next and prev targets
                        NIASSERT(ucTargetType == 3);
                        {
                            if (fWeight >= fInBetweenWeightNext ||
                                fWeight <= fInBetweenWeightPrev)
                            {
                                fTargetWeight = 0.0f;
                            }
                            else if (fWeight >= fInBetweenWeight)
                            {
                                float fDelta = 
                                    fInBetweenWeightNext - fInBetweenWeight;

                                fTargetWeight = 
                                    (fDelta - (fWeight - fInBetweenWeight)) / 
                                    fDelta;
                            }
                            else
                            {
                                NIASSERT(fWeight < fInBetweenWeight);

                                float fDelta = 
                                    fInBetweenWeight - fInBetweenWeightPrev;

                                fTargetWeight = 
                                    (fDelta + (fWeight - fInBetweenWeight)) / 
                                    fDelta;
                            }

                        }
                        break;
                    }

                    pkLinKeys[uiSample].SetTime((float)kSampleTime.as(
                        MTime::kSeconds));

                    pkLinKeys[uiSample].SetValue(fTargetWeight);
                }


                // Copy over our key values so we can build the interpolator
                uiNumKeys = uiNumSamples;
                pkKeys = (NiFloatKey*)pkLinKeys;
                eType = NiAnimationKey::LINKEY;


            }

            // Create the Float Data
            pkFloatData = NiNew NiFloatData();
            pkFloatData->ReplaceAnim((NiFloatKey*)pkKeys, uiNumKeys, eType);

            // Create the Interpolator
            pkInterp = NiNew NiFloatInterpolator(pkFloatData);
            pkInterp->Collapse();

            // Add the Interpolator to the GeoMorphController
            pkMorph->SetInterpolator(pkInterp, (unsigned short)(uiNumNiTargets++));

            NiTimeController::CycleType kCycleType = 
                GetAnimationPostCycleType(kTargetPlug, kStatus);

            pkMorph->SetCycleType(kCycleType);
        }
    }
   
    pkMorph->ResetTimeExtrema();
    return;
}
//---------------------------------------------------------------------------
void MyiTriMesh::AddUniqueName(NiMorphWeightsController* pkMorph,
    const char *szTargetName, int iTarget)
{
    char szNameBuffer[512];
    bool bDuplicate = true;
    int iIndex = 0;

    // Keep looking until we make a unique name
    while(bDuplicate)
    {
        bDuplicate = false;

        if (iIndex > 0)
            NiSprintf(szNameBuffer, 512, "%s%d", szTargetName, iIndex);
        else
            NiStrcpy(szNameBuffer, 512, szTargetName);

        iIndex++;

        // Ensure we have a unique name
        int iLoop;
        for (iLoop = 0; iLoop < iTarget; iLoop++)
        {
            NiFixedString kName;
            pkMorph->GetTargetName((NiUInt16)iLoop, kName);
            if (kName.Exists() &&
                strcmp(szNameBuffer, (const char*)kName) == 0)
            {
                bDuplicate = true;
                break;
            }
        }
    }

    NiFixedString kName(szNameBuffer);
    pkMorph->SetTargetName((NiUInt16)iTarget, kName);
}
//---------------------------------------------------------------------------
bool MyiTriMesh::GetMorphUpdateNormals(int iShapeID)
{
    // Create the Dag Node for Our Object
    MFnDagNode dgNode;
    MObject mObj;
    DtExt_ShapeGetTransform(iShapeID, mObj);
    dgNode.setObject( mObj );

    // By default don't update the normals
    bool bUpdateNormals = false;

    // Check for the Extra Attribute
    GetExtraAttribute(dgNode,"Ni_MorphUpdateNormals", true, bUpdateNormals);

    return bUpdateNormals;
}
//---------------------------------------------------------------------------
void MyiTriMesh::AttachProperties(int iShapeID, int iGroupID, 
    NiTexturingProperty* pTexturingProperty)
{
    
    m_spMesh->AttachProperty(pTexturingProperty);
    
    // Setup the Alpha Property
    if(m_spMesh->GetProperty(NiProperty::ALPHA) == NULL) 
    {
        NiAlphaProperty* pAlpha = NiNew NiAlphaProperty;
        pAlpha->SetSrcBlendMode( NiAlphaProperty::ALPHA_SRCALPHA);
        pAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
        pAlpha->SetAlphaBlending(true);
        
        m_spMesh->AttachProperty( pAlpha );
    }
    
    
    // Setup the Vertex Color Properties
    if(DtTextureGetMaterialVertexColorOverride(iShapeID, iGroupID))
    {
        NiVertexColorPropertyPtr spVertexColor = NiNew NiVertexColorProperty;
        
        
        char cSrcMode;
        if(DtTextureGetMaterialVertexColorSrcMode(iShapeID, iGroupID, 
            cSrcMode))
        {
            spVertexColor->SetSourceMode(
                (NiVertexColorProperty::SourceVertexMode)cSrcMode);
        }
        else
        {
            NIASSERT(false);  // SHould never get here
        }
        
        
        char cLightMode;
        if(DtTextureGetMaterialVertexColorLightMode(iShapeID, iGroupID, 
            cLightMode))
        {
            spVertexColor->SetLightingMode(
                (NiVertexColorProperty::LightingMode)cLightMode);
        }
        else
        {
            NIASSERT(false);  // SHould never get here
        }
        
        m_spMesh->AttachProperty(spVertexColor);
    }
}
//---------------------------------------------------------------------------
void MyiTriMesh::CreateFlipBookProperty(int iShapeID, int iGroupID, 
    int iTextureID, char* szType, MyiTextureManager* pkTextureManager, 
    NiTexturingProperty::Map *pMap, 
    NiTexturingProperty *pTexturingProperty)
{

    int iMaterialID;
    DtMtlGetID( iShapeID, iGroupID, &iMaterialID );

    CreateFlipBookProperty(iMaterialID, iTextureID, szType, pkTextureManager,
        pMap, pTexturingProperty);
}
//---------------------------------------------------------------------------
void MyiTriMesh::CreateFlipBookProperty(int iMaterialID, 
    int iTextureID, char* szType, MyiTextureManager* pkTextureManager, 
    NiTexturingProperty::Map *pMap, 
    NiTexturingProperty *pTexturingProperty)
{
    bool bFlipBook = false;
    short sStartFrame = 0;
    short sEndFrame = 0;
    float fFramesPerSecond = 0.0f;
    float fStartTime = 0.0f;
    char cCycleType = 0;


        // Check for a Flip Book Texture
    if(DtTextureGetMaterialMapFlipBookProperties( iMaterialID, szType, 
        bFlipBook, sStartFrame, sEndFrame, fFramesPerSecond, fStartTime, 
        cCycleType))
    {
        NiFlipControllerPtr spFlip = NiNew NiFlipController;
        
        spFlip->SetTarget(pTexturingProperty);
        spFlip->SetAffectedMap(pMap);
        spFlip->SetCycleType((NiTimeController::CycleType)cCycleType);
        
        
        // Attach in each of the Source Textures
        char *szFullPath = NULL;
        DtTextureGetFileNameID(iTextureID, &szFullPath);
        
        
        for(int iFrame = sStartFrame; iFrame <= sEndFrame; iFrame++)
        {
            char szFramePath[MAX_PATH];
            
            MyiTexture::CreateSequentialTexturePath(szFullPath, szFramePath, 
                MAX_PATH, iFrame, '/');
            
            spFlip->AttachTexture( pkTextureManager->GetTexture(szFramePath), 
                (iFrame-sStartFrame) );
        }

        // Determine when the first key starts
        float fFirstKeyTime = fStartTime / 
            (float)gAnimControlGetFramesPerSecond();

        // Create the Keys
        NiLinFloatKey* pkKeys = NiNew NiLinFloatKey[ sEndFrame - sStartFrame + 
            1 ];

        for (int iLoop = 0; iLoop <=  sEndFrame - sStartFrame; iLoop++)
        {
            pkKeys[iLoop].SetValue((float)iLoop);
            pkKeys[iLoop].SetTime(fFirstKeyTime + ((float)iLoop * (1.0f / 
                fFramesPerSecond)));
        }
        
        // Create the Data
        NiFloatData* pkData = NiNew NiFloatData();
        pkData->ReplaceAnim(pkKeys, sEndFrame - sStartFrame + 1, 
            NiFloatKey::LINKEY);

        // Create the Interpolator
        NiFloatInterpolator* pkInterp = NiNew NiFloatInterpolator(pkData);
        pkInterp->Collapse();

        // Assign the Interpolator
        spFlip->SetInterpolator(pkInterp);
        spFlip->ResetTimeExtrema();
    }
}

void MyiTriMesh::AddNoStripifyAttributes(MFnDagNode& dgNode)
{
    if(CheckForExtraAttribute(dgNode, "Ni_No_Stripify", true))
    {
        bool bValue;
        if(GetExtraAttribute(dgNode, "Ni_No_Stripify", true, bValue))
        {
            if(bValue)
            {
                NiStringExtraData* pkSData = NiNew NiStringExtraData(
                    "NiNoStripify");
                m_spMesh->AddExtraData("NiNoStripify", (NiExtraData*)pkSData);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool MyiTriMesh::CreateSkinDataStreams(NiMesh* pkMesh, 
    NiUInt32 uiVertCount, NiUInt32 uiBoneCount, 
    NiDataStreamElementLock& kBoneLock, NiDataStreamElementLock& kWeightLock)
{
    NI_UNUSED_ARG(uiBoneCount);
    // This function will add the bone index and bone weight data streams if
    // they don't already exist. It will return locks to both sets of data
    // It will also duplicate the position data and make a unique stream out
    // of it for bind pose data

    // check for morphing to see if we need to make data mutable
    NiMorphMeshModifier* pkMorphModifier =
        NiGetModifier(NiMorphMeshModifier, m_spMesh);
    bool bHasMorphModifier = (pkMorphModifier != 0);

    // copy position data into the position bind pose
    if (!CreateBindPoseStream(pkMesh, uiVertCount, 
        NiCommonSemantics::POSITION(), NiCommonSemantics::POSITION_BP(), 
        bHasMorphModifier))
    {
        DtExt_Err("Failed to create position bind pose. Skinning will be "
            "disabled.");
        return false;
    }

    // create normal, binormal, and tangent bind pose data if it exists
    // if it fails, we can continue so we ignore the return value
    CreateBindPoseStream(pkMesh, uiVertCount, NiCommonSemantics::NORMAL(), 
        NiCommonSemantics::NORMAL_BP(), bHasMorphModifier);
    CreateBindPoseStream(pkMesh, uiVertCount, NiCommonSemantics::BINORMAL(),
        NiCommonSemantics::BINORMAL_BP(), bHasMorphModifier);
    CreateBindPoseStream(pkMesh, uiVertCount, NiCommonSemantics::TANGENT(),
        NiCommonSemantics::TANGENT_BP(), bHasMorphModifier);

    kBoneLock = NiDataStreamElementLock(pkMesh, 
        NiCommonSemantics::BLENDINDICES(), 0, NiDataStreamElement::F_INT16_4,
        NiDataStream::LOCK_TOOL_READ | NiDataStream::LOCK_TOOL_WRITE);
    if (!kBoneLock.DataStreamExists())
    {
        // if we couldn't find the semantic and index, create it
        kBoneLock = pkMesh->AddStreamGetLock(
            NiCommonSemantics::BLENDINDICES(), 0,
            NiDataStreamElement::F_INT16_4, uiVertCount, 
            NiDataStream::ACCESS_CPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_VERTEX, true);
    }

    kWeightLock = NiDataStreamElementLock(pkMesh, 
        NiCommonSemantics::BLENDWEIGHT(), 0, NiDataStreamElement::F_FLOAT32_4,
        NiDataStream::LOCK_TOOL_READ | NiDataStream::LOCK_TOOL_WRITE);
    if (!kWeightLock.DataStreamExists())
    {
        // if we couldn't find the semantic and index, create it
        kWeightLock = pkMesh->AddStreamGetLock(
            NiCommonSemantics::BLENDWEIGHT(), 0, 
            NiDataStreamElement::F_FLOAT32_4, uiVertCount, 
            NiDataStream::ACCESS_CPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_VERTEX, true);
    }

    if (!kBoneLock.IsLocked() || !kWeightLock.IsLocked())
    {
        // we can't lock the data steams, we fail
        DtExt_Err("Error:: Failed to lock skinning data. Skinning will be "
            "disabled.h");
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool MyiTriMesh::CreateBindPoseStream(NiMesh*pkMesh, NiUInt32 uiVertCount,
        const NiFixedString& kSourceSemantic, 
        const NiFixedString& kDestSemantic, bool bHasMorph)
{
    // find the first index the semantic occurs at
    NiDataStreamElementLock kLock;
    NiInt32 iMaxIndex = pkMesh->GetMaxIndexBySemantic(kSourceSemantic);
    // a return value of -1 means the source semantic doesn't exist
    if (iMaxIndex == -1)
        return false;

    for (NiUInt32 ui = 0; ui <= (NiUInt32)iMaxIndex; ui++)
    {
        kLock = NiDataStreamElementLock(pkMesh,
            kSourceSemantic, ui, NiDataStreamElement::F_FLOAT32_3,
            NiDataStream::LOCK_TOOL_READ);
        if (kLock.DataStreamExists() && kLock.IsLocked())
        {
            NiTStridedRandomAccessIterator<NiPoint3> kDataIter = 
                kLock.begin<NiPoint3>();

            // set the access to make sure it works with skinning
            NiToolDataStream* pkToolDataStream =
                NiDynamicCast(NiToolDataStream, kLock.GetDataStream());
            pkToolDataStream->MakeWritable();
            pkToolDataStream->SetCloningBehavior(NiObject::CLONE_BLANK_COPY);

            NiDataStreamElementLock kBindPoseLock = 
                NiDataStreamElementLock(pkMesh,
                kDestSemantic, ui, NiDataStreamElement::F_FLOAT32_3,
                NiDataStream::LOCK_TOOL_READ | NiDataStream::LOCK_TOOL_WRITE);
            if (!kBindPoseLock.DataStreamExists())
            {
                NiUInt8 ucAccessMask = (bHasMorph) ? 
                    (NiDataStream::ACCESS_CPU_READ | 
                    NiDataStream::ACCESS_CPU_WRITE_MUTABLE) :
                    (NiDataStream::ACCESS_CPU_READ | 
                    NiDataStream::ACCESS_CPU_WRITE_STATIC);

                kBindPoseLock = pkMesh->AddStreamGetLock(
                    kDestSemantic, ui, NiDataStreamElement::F_FLOAT32_3, 
                    uiVertCount, ucAccessMask, NiDataStream::USAGE_VERTEX,
                    true);
            }

            if (kBindPoseLock.IsLocked())
            {
                // if we have successfully grabbed the bind pose stream, fill
                // it in with position data
                NiTStridedRandomAccessIterator<NiPoint3> kBindPose = 
                    kBindPoseLock.begin<NiPoint3>();
                for (NiUInt32 ui = 0; ui < uiVertCount; ui++)
                {
                    kBindPose[ui] = kDataIter[ui];
                }
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool MyiTriMesh::AttachPerMeshProfileData(MObject kTransformObject)
{

    MStatus kStatus = MStatus::kSuccess;

    //Get the attribute value
    MFnDependencyNode kFnDepNode(kTransformObject, &kStatus);
    if(kStatus != MStatus::kSuccess)
    {
        //This is a problem
        NIASSERT("Invalid Shape Node passed in"); 
        DtExt_Err("Invalid Shape Node. Could not access mesh profile" 
            ", if there is one. Using Scene default");
        return false;
    }

    MString kAttributeName("Gb_PackingProfile");

    MObject kAttributeRef = kFnDepNode.attribute(kAttributeName, &kStatus);

    MPlug kPlug = kFnDepNode.findPlug(kAttributeRef, true, &kStatus);
    if(kStatus != MStatus::kSuccess)
    {
        //this just means that the extra attribute was not added
        //this is not a problem
        
        return false;
    }
    
    MString kProfileName;
    kStatus = kPlug.getValue(kProfileName);
    if(kStatus != MStatus::kSuccess)
    {   
        NIASSERT("Could not get Profile Name"); 
        DtExt_Err("Could not get profile name from Mesh Packing Profile "
            "attribute. Using Default.");
        return false;
    }

    kProfileName = GetPackingProfiles::GetProfileName(kProfileName);
    const char* pcProfileName = kProfileName.asChar();

    NiExtraData* pExtraData = NULL;
    pExtraData = m_spMesh->GetExtraData("MeshProfileName");

    NiStringExtraData* pkMeshProfileExtraData = NiDynamicCast(
        NiStringExtraData, pExtraData);

    if(!pkMeshProfileExtraData)
    {
        //Something probably went wrong when the data was added to the 
        //object
        NIASSERT("Could not retrieve \"MeshProfileName\" from the mesh");
        DtExt_Err("Invalid Shape Node. Could not find mesh profile" 
            " extra data.");
        return false;
    }

    pkMeshProfileExtraData->SetValue(pcProfileName);

    return true;
}
//---------------------------------------------------------------------------
