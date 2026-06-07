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

// Precompiled Header
#include "NiMainPCH.h"

#include "NiShaderConstantMapEntry.h"

NiShaderAttributeDesc::AttributeType 
    NiShaderConstantMapEntry::ms_aeAttribType[
    NiShaderAttributeDesc::ATTRIB_TYPE_COUNT];
unsigned char NiShaderConstantMapEntry::ms_aucAttribFlags[
    NiShaderAttributeDesc::ATTRIB_TYPE_COUNT];
bool NiShaderConstantMapEntry::ms_bAttribTableInitialized = false;

NiShaderAttributeDesc::ObjectType
    NiShaderConstantMapEntry::ms_aeObjectType[
    NiShaderAttributeDesc::OT_COUNT];
unsigned short NiShaderConstantMapEntry::ms_ausObjectFlags[
    NiShaderAttributeDesc::OT_COUNT];
bool NiShaderConstantMapEntry::ms_bObjectTableInitialized = false;

//---------------------------------------------------------------------------
void NiShaderConstantMapEntry::_SDMInit()
{
    NiShaderConstantMapEntry::InitializeAttribTable();
    NiShaderConstantMapEntry::InitializeObjectTable();
}
//---------------------------------------------------------------------------
void NiShaderConstantMapEntry::InitializeAttribTable()
{
    // Keep this updated as new attribute types are added!
    ms_aeAttribType[0x00] = NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
    ms_aeAttribType[0x01] = NiShaderAttributeDesc::ATTRIB_TYPE_BOOL;
    ms_aeAttribType[0x02] = NiShaderAttributeDesc::ATTRIB_TYPE_STRING;
    ms_aeAttribType[0x03] = NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
    ms_aeAttribType[0x04] = NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
    ms_aeAttribType[0x05] = NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
    ms_aeAttribType[0x06] = NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
    ms_aeAttribType[0x07] = NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
    ms_aeAttribType[0x08] = NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
    ms_aeAttribType[0x09] = NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
    ms_aeAttribType[0x0A] = NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
    ms_aeAttribType[0x0B] = NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE;
    ms_aeAttribType[0x0C] = NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT8;
    ms_aeAttribType[0x0D] = NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT12;
    ms_aeAttribType[0x0E] = NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY;

    ms_aucAttribFlags[NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED]   = 0x00;
    ms_aucAttribFlags[NiShaderAttributeDesc::ATTRIB_TYPE_BOOL]        = 0x01;
    ms_aucAttribFlags[NiShaderAttributeDesc::ATTRIB_TYPE_STRING]      = 0x02;
    ms_aucAttribFlags[NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT] = 0x03;
    ms_aucAttribFlags[NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT]       = 0x04;
    ms_aucAttribFlags[NiShaderAttributeDesc::ATTRIB_TYPE_POINT2]      = 0x05;
    ms_aucAttribFlags[NiShaderAttributeDesc::ATTRIB_TYPE_POINT3]      = 0x06;
    ms_aucAttribFlags[NiShaderAttributeDesc::ATTRIB_TYPE_POINT4]      = 0x07;
    ms_aucAttribFlags[NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3]     = 0x08;
    ms_aucAttribFlags[NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4]     = 0x09;
    ms_aucAttribFlags[NiShaderAttributeDesc::ATTRIB_TYPE_COLOR]       = 0x0A;
    ms_aucAttribFlags[NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE]     = 0x0B;
    ms_aucAttribFlags[NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT8]      = 0x0C;
    ms_aucAttribFlags[NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT12]     = 0x0D;
    ms_aucAttribFlags[NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY]       = 0x0E;

    ms_bAttribTableInitialized = true; 
}
//---------------------------------------------------------------------------
void NiShaderConstantMapEntry::InitializeObjectTable()
{
    // Keep this updated as new object types are added!
    ms_aeObjectType[0x00] = NiShaderAttributeDesc::OT_UNDEFINED;
    ms_aeObjectType[0x01] = NiShaderAttributeDesc::OT_EFFECT_GENERALLIGHT;
    ms_aeObjectType[0x02] = NiShaderAttributeDesc::OT_EFFECT_POINTLIGHT;
    ms_aeObjectType[0x03] = NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT;
    ms_aeObjectType[0x04] = NiShaderAttributeDesc::OT_EFFECT_SPOTLIGHT;
    ms_aeObjectType[0x05] = NiShaderAttributeDesc::OT_EFFECT_SHADOWPOINTLIGHT;
    ms_aeObjectType[0x06] = 
        NiShaderAttributeDesc::OT_EFFECT_SHADOWDIRECTIONALLIGHT;
    ms_aeObjectType[0x07] = NiShaderAttributeDesc::OT_EFFECT_SHADOWSPOTLIGHT;
    ms_aeObjectType[0x08] = NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP;
    ms_aeObjectType[0x09] =
        NiShaderAttributeDesc::OT_EFFECT_PROJECTEDSHADOWMAP;
    ms_aeObjectType[0x0A] =
        NiShaderAttributeDesc::OT_EFFECT_PROJECTEDLIGHTMAP;
    ms_aeObjectType[0x0B] = NiShaderAttributeDesc::OT_EFFECT_FOGMAP;
    ms_aeObjectType[0x0C] = NiShaderAttributeDesc::OT_EFFECT_POINTSHADOWMAP;
    ms_aeObjectType[0x0D] = NiShaderAttributeDesc::OT_EFFECT_DIRSHADOWMAP;
    ms_aeObjectType[0x0E] = NiShaderAttributeDesc::OT_EFFECT_SPOTSHADOWMAP;

    ms_ausObjectFlags[NiShaderAttributeDesc::OT_UNDEFINED] = 0x0000;
    ms_ausObjectFlags[NiShaderAttributeDesc::OT_EFFECT_GENERALLIGHT] = 0x0100;
    ms_ausObjectFlags[NiShaderAttributeDesc::OT_EFFECT_POINTLIGHT] = 0x0200;
    ms_ausObjectFlags[NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT] =
        0x0300;
    ms_ausObjectFlags[NiShaderAttributeDesc::OT_EFFECT_SPOTLIGHT] = 
        0x0400;
    ms_ausObjectFlags[NiShaderAttributeDesc::OT_EFFECT_SHADOWPOINTLIGHT] = 
        0x0500;
    ms_ausObjectFlags[NiShaderAttributeDesc::OT_EFFECT_SHADOWDIRECTIONALLIGHT]
    = 0x0600;
    ms_ausObjectFlags[NiShaderAttributeDesc::OT_EFFECT_SHADOWSPOTLIGHT] = 
        0x0700;
    ms_ausObjectFlags[NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP] =
        0x0800;
    ms_ausObjectFlags[NiShaderAttributeDesc::OT_EFFECT_PROJECTEDSHADOWMAP] =
        0x0900;
    ms_ausObjectFlags[NiShaderAttributeDesc::OT_EFFECT_PROJECTEDLIGHTMAP] =
        0x0A00;
    ms_ausObjectFlags[NiShaderAttributeDesc::OT_EFFECT_FOGMAP] = 0x0B00;
    ms_ausObjectFlags[NiShaderAttributeDesc::OT_EFFECT_POINTSHADOWMAP] = 
        0x0C00;
    ms_ausObjectFlags[NiShaderAttributeDesc::OT_EFFECT_DIRSHADOWMAP] = 0x0D00;
    ms_ausObjectFlags[NiShaderAttributeDesc::OT_EFFECT_SPOTSHADOWMAP] = 0x0E00;

    ms_bObjectTableInitialized = true;
}
//---------------------------------------------------------------------------
