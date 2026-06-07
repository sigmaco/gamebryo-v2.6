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

#include "NiSceneGraphPrinter.h"
#include <NiMesh.h>
#include <NiMain.h>
#include <NiFloat16.h>

bool NiSceneGraphPrinter::ms_bShowAppCulled = false;
bool NiSceneGraphPrinter::ms_bDropAppCulled = false;
bool NiSceneGraphPrinter::ms_bShowBoundingSpheres = false;
bool NiSceneGraphPrinter::ms_bShowMemoryAddress = false;
bool NiSceneGraphPrinter::ms_bShowProperties = false;
bool NiSceneGraphPrinter::ms_bShowTransforms = false;
bool NiSceneGraphPrinter::ms_bShowExtraData = false;
bool NiSceneGraphPrinter::ms_bShowMeshDataStreams = false;
bool NiSceneGraphPrinter::ms_bShowMeshDataStreamsVerbose = false;
bool NiSceneGraphPrinter::ms_bShowTimeControllers = true;
bool NiSceneGraphPrinter::ms_bShowModifiers = false;
bool NiSceneGraphPrinter::ms_bShowNodes = true;

unsigned short NiSceneGraphPrinter::ms_usTabStop = 2;
unsigned int NiSceneGraphPrinter::ms_uiObjectCount = 0;
unsigned int NiSceneGraphPrinter::ms_uiDepth = 0;

unsigned int NiSceneGraphPrinter::ms_uiTriShapeCount = 0;
unsigned int NiSceneGraphPrinter::ms_uiTriShape_TriCount = 0;
unsigned int NiSceneGraphPrinter::ms_uiTriShape_LowTriCount = 0;
unsigned int NiSceneGraphPrinter::ms_uiTriShape_HighTriCount = 0;
unsigned int NiSceneGraphPrinter::ms_uiTriShape_ZeroTriCount = 0;
unsigned int NiSceneGraphPrinter::ms_uiTriStripsCount = 0;
unsigned int NiSceneGraphPrinter::ms_uiTriStrips_TriCount = 0;
unsigned int NiSceneGraphPrinter::ms_uiTriStrips_LowTriCount = 0;
unsigned int NiSceneGraphPrinter::ms_uiTriStrips_HighTriCount = 0;
unsigned int NiSceneGraphPrinter::ms_uiTriStrips_ZeroTriCount = 0;

bool NiSceneGraphPrinter::ms_bFirstTriShape = true;
bool NiSceneGraphPrinter::ms_bFirstTriStrips = true;

std::ostream* NiSceneGraphPrinter::ms_pkOstr = &std::cout;

#define PRINT_INDENT(X) \
    { for (unsigned short i = 0; i < (X); i++) *ms_pkOstr << " "; }

#define PRINT_ELEMENT_VALUES(dataType, compCount) \
    { \
        NiTStridedRandomAccessIterator<dataType> kIter = \
            kElementLock.begin<dataType>(); \
        *ms_pkOstr << std::endl; \
        for (NiUInt32 ui = 0; ui < kElementLock.count(); ui++) \
        { \
            NiString kValue; \
            PRINT_INDENT((usIndent + 6) * ms_usTabStop); \
            kValue.Format("%d: ", ui); \
            if (compCount == 1) \
            { \
                kValue += NiString::From(kIter[ui]); \
                kValue += "\n"; \
            } \
            else \
            { \
                dataType* pkData = &kIter[ui]; \
                kValue += "("; \
                kValue += NiString::From(pkData[0]); \
                for (NiUInt32 uiComp = 1; uiComp < compCount; uiComp++) \
                { \
                    kValue += ", "; \
                    kValue += NiString::From(pkData[uiComp]); \
                } \
                kValue += ")\n"; \
            } \
            *ms_pkOstr << kValue; \
        } \
    }


//---------------------------------------------------------------------------
void NiSceneGraphPrinter::PrintID(NiObject* pkObject, unsigned short usIndent)
{
    PRINT_INDENT(usIndent * ms_usTabStop);

    //
    // Print level
    //
    *ms_pkOstr << usIndent << " - ";

    //
    // Print type
    //
    if (pkObject == NULL)
    {
        *ms_pkOstr << "WARNING: NULL Object found!!" << std::endl;

        if (usIndent > ms_uiDepth)
            ms_uiDepth = usIndent;

        return;
    }

    *ms_pkOstr << pkObject->GetRTTI()->GetName();

    NiObjectNET *pNET = NiDynamicCast(NiObjectNET, pkObject);
    if (pNET != NULL)
    {
        //
        // Print name
        //
        *ms_pkOstr << ":<";
        
        if (pNET->GetName())
            *ms_pkOstr << pNET->GetName();
        
        *ms_pkOstr << ">";
    }

    if (ms_bShowMemoryAddress)
        *ms_pkOstr << " <0x" << pkObject << ">";

    *ms_pkOstr << std::endl;

    ms_uiObjectCount++;
    if (usIndent > ms_uiDepth)
        ms_uiDepth = usIndent;
}
//---------------------------------------------------------------------------
void NiSceneGraphPrinter::PrintAppCulled(NiObject* pkObject,
    unsigned short usIndent)
{
    if (! NiIsKindOf(NiAVObject, pkObject))
        return;

    if (((NiAVObject*)pkObject)->GetAppCulled())
    {
        PRINT_INDENT((usIndent + 2) * ms_usTabStop);
        *ms_pkOstr << "CULLED (including sub-tree) by Application"
            << std::endl;
    }
}
//---------------------------------------------------------------------------
void NiSceneGraphPrinter::PrintTransforms(NiObject* pkObject, 
    unsigned short usIndent)
{
    if (! NiIsKindOf(NiAVObject, pkObject))
        return;

    NiAVObject* pAV = (NiAVObject*)pkObject;
    NiPoint3 row;
    unsigned int i;

    for (i = 0; i < 3; i ++)
    {
        PRINT_INDENT((usIndent + 2) * ms_usTabStop);
        pAV->GetRotate().GetRow(i, row);
        *ms_pkOstr << "Rotate: <" << row.x << "," << row.y 
            << "," << row.z << ">" << std::endl;
    }
    
    PRINT_INDENT((usIndent + 2) * ms_usTabStop);
    *ms_pkOstr << "Translate: <" << pAV->GetTranslate().x
        << "," << pAV->GetTranslate().y << "," << pAV->GetTranslate().z
        << ">" << std::endl;

    PRINT_INDENT((usIndent + 2) * ms_usTabStop);
    *ms_pkOstr << "Scale: " << pAV->GetScale() << std::endl;
}
//---------------------------------------------------------------------------
void NiSceneGraphPrinter::PrintBoundingSpheres(NiObject* pkObject, 
    unsigned short usIndent)
{
    if (! NiIsKindOf(NiAVObject, pkObject))
        return;

    NiBound bound = ((NiAVObject*)pkObject)->GetWorldBound();
    PRINT_INDENT((usIndent + 2) * ms_usTabStop);
    *ms_pkOstr << "World Bound: C <" << bound.GetCenter().x << ","
        << bound.GetCenter().y << "," << bound.GetCenter().z 
        << ">, R " << bound.GetRadius() << std::endl;
}
//---------------------------------------------------------------------------
void NiSceneGraphPrinter::PrintProperties(NiObject* pkObject, 
    unsigned short usIndent)
{
    if (! NiIsKindOf(NiAVObject, pkObject))
        return;

    NiTListIterator kPos = ((NiAVObject*)pkObject)->GetPropertyList().
        GetHeadPos();

    while (kPos)
    {
        NiProperty* pkProp = ((NiAVObject*)pkObject)->GetPropertyList().
            GetNext(kPos);

        if (pkProp)
        {
            PRINT_INDENT((usIndent + 2) * ms_usTabStop);
            *ms_pkOstr << pkProp->GetRTTI()->GetName() << std::endl;
        }

    }
}
//---------------------------------------------------------------------------
void NiSceneGraphPrinter::PrintExtraData(NiObject* pkObject,
    unsigned short usIndent)
{
    if (! NiIsKindOf(NiObjectNET, pkObject))
        return;

    NiObjectNET *pkObjectNET = (NiObjectNET *) pkObject;

    for (unsigned int i=0; i < pkObjectNET->GetExtraDataSize(); i++)
    {
        NiExtraData* pkData = pkObjectNET->GetExtraDataAt((unsigned short)i);

        PRINT_INDENT((usIndent + 2) * ms_usTabStop);
        *ms_pkOstr << pkData->GetRTTI()->GetName() << ": ";

        *ms_pkOstr << "Name: \"" << pkData->GetName() << "\" Value: ";
  
        if (NiIsKindOf(NiBooleanExtraData, pkData))
        {
            *ms_pkOstr << "<" <<
                (((NiBooleanExtraData*)pkData)->GetValue() ? "true" : "false")
                << ">";
        }
        else if (NiIsKindOf(NiColorExtraData, pkData))
        {
            *ms_pkOstr << "<"
                << ((NiColorExtraData*)pkData)->GetRed() << " "
                << ((NiColorExtraData*)pkData)->GetGreen() << " "
                << ((NiColorExtraData*)pkData)->GetBlue() << " "
                << ((NiColorExtraData*)pkData)->GetAlpha() << ">";
        }
        else if (NiIsKindOf(NiFloatExtraData, pkData))
        {
            *ms_pkOstr
                << "<" << ((NiFloatExtraData*)pkData)->GetValue() << ">";
        }
        else if (NiIsKindOf(NiFloatsExtraData, pkData))
        {
            unsigned int uiSize;
            float* afVals;
            ((NiFloatsExtraData*)pkData)->GetArray(uiSize, afVals);
            *ms_pkOstr << "<";
            for (unsigned int ui = 0; ui < uiSize; ui++)
            {
                if (ui != 0)
                    *ms_pkOstr << " ";
                *ms_pkOstr << afVals[ui];
            }
            *ms_pkOstr << ">";
        }
        else if (NiIsKindOf(NiIntegerExtraData, pkData))
        {
            *ms_pkOstr << "<"
                << ((NiIntegerExtraData*)pkData)->GetValue() << ">";
        }
        else if (NiIsKindOf(NiIntegersExtraData, pkData))
        {
            unsigned int uiSize;
            int* aiVals;
            ((NiIntegersExtraData*)pkData)->GetArray(uiSize, aiVals);
            *ms_pkOstr << "<";
            for (unsigned int ui = 0; ui < uiSize; ui++)
            {
                if (ui != 0)
                    *ms_pkOstr << " ";
                *ms_pkOstr << aiVals[ui];
            }
            *ms_pkOstr << ">";
        }
        else if (NiIsKindOf(NiStringExtraData, pkData))
        {
            *ms_pkOstr << "<\""
                << ((NiStringExtraData*)pkData)->GetValue() << "\">";
        }
        else if (NiIsKindOf(NiStringsExtraData, pkData))
        {
            unsigned int uiSize;
            char** apcVals;
            ((NiStringsExtraData*)pkData)->GetArray(uiSize, apcVals);
            *ms_pkOstr << "<";
            for (unsigned int ui = 0; ui < uiSize; ui++)
            {
                if (ui != 0)
                    *ms_pkOstr << " \"";
                else
                    *ms_pkOstr << "\"";
                *ms_pkOstr << apcVals[ui];
                *ms_pkOstr << "\"";
            }
            *ms_pkOstr << ">";
        }
        else if (NiIsKindOf(NiVectorExtraData, pkData))
        {
            *ms_pkOstr << "<"
                << ((NiVectorExtraData*)pkData)->GetX() << " "
                << ((NiVectorExtraData*)pkData)->GetY() << " "
                << ((NiVectorExtraData*)pkData)->GetZ() << " "
                << ((NiVectorExtraData*)pkData)->GetW() << ">";
        }
        *ms_pkOstr << std::endl;
    }
}
//---------------------------------------------------------------------------
void NiSceneGraphPrinter::PrintMeshDataStreams(NiObject* pkObject, 
    unsigned short usIndent)
{
    if (! NiIsKindOf(NiMesh, pkObject))
        return;

    NiMesh* pkMesh = (NiMesh*)pkObject;

    PRINT_INDENT((usIndent + 2) * ms_usTabStop);
    *ms_pkOstr << pkMesh->GetPrimitiveTypeString();
    if (ms_bShowMemoryAddress)
        *ms_pkOstr << " <0x" << pkMesh << "> ";
    *ms_pkOstr << std::endl;

    if (pkMesh->RequiresMaterialOption(
        NiRenderObjectMaterialOption::TRANSFORM_SKINNED()))
    {
        PRINT_INDENT((usIndent + 2) * ms_usTabStop);
        *ms_pkOstr << "Requires Material Option TRANSFORM_SKINNED";
        *ms_pkOstr << std::endl;
    }

    PRINT_INDENT((usIndent + 2) * ms_usTabStop);
    *ms_pkOstr << "Num Submeshes: " << pkMesh->GetSubmeshCount() << std::endl;
    
    NiUInt32 uiPrimCount = pkMesh->GetTotalPrimitiveCount();
    PRINT_INDENT((usIndent + 2) * ms_usTabStop);
    *ms_pkOstr << "Total Primitive Count: " << uiPrimCount << std::endl;

    if (pkMesh->GetPrimitiveType() == NiPrimitiveType::PRIMITIVE_TRIANGLES)
    {
        ms_uiTriShapeCount++;
        ms_uiTriShape_TriCount = pkMesh->GetTotalPrimitiveCount();
        
        if (ms_bFirstTriShape)
        {
            ms_uiTriShape_LowTriCount = uiPrimCount;
            ms_bFirstTriShape = false;
        }

        if (ms_uiTriShape_LowTriCount > uiPrimCount)
            ms_uiTriShape_LowTriCount = uiPrimCount;

        if (ms_uiTriShape_HighTriCount < uiPrimCount)
            ms_uiTriShape_HighTriCount = uiPrimCount;

        if (uiPrimCount == 0)
            ms_uiTriShape_ZeroTriCount++;
    }
    else if (pkMesh->GetPrimitiveType() == 
        NiPrimitiveType::PRIMITIVE_TRISTRIPS)
    {
        
        ms_uiTriStripsCount++;
        ms_uiTriStrips_TriCount = pkMesh->GetTotalPrimitiveCount();
        
        if (ms_bFirstTriStrips)
        {
            ms_uiTriStrips_LowTriCount = uiPrimCount;
            ms_bFirstTriStrips = false;
        }

        if (ms_uiTriStrips_LowTriCount > uiPrimCount)
            ms_uiTriStrips_LowTriCount = uiPrimCount;

        if (ms_uiTriStrips_HighTriCount < uiPrimCount)
            ms_uiTriStrips_HighTriCount = uiPrimCount;

        if (uiPrimCount == 0)
            ms_uiTriStrips_ZeroTriCount++;
    }

    NiUInt32 uiVertCount = pkMesh->GetVertexCount();
    PRINT_INDENT((usIndent + 2) * ms_usTabStop);
    *ms_pkOstr << "Total Vertex Count: " << uiVertCount << std::endl;

    PRINT_INDENT((usIndent + 2) * ms_usTabStop);
    *ms_pkOstr << "NiDataStreamRef's: " << pkMesh->GetStreamRefCount() << 
        std::endl;

    for (NiUInt32 ui = 0; ui < pkMesh->GetStreamRefCount(); ui++)
    {
        NiDataStreamRef* pkRef = pkMesh->GetStreamRefAt(ui);

        if (pkRef)
        {
            PRINT_INDENT((usIndent + 2) * ms_usTabStop);
            *ms_pkOstr << "[" << ui << "] Elements: " << "{ ";

            for (NiUInt32 uj = 0; uj < pkRef->GetElementDescCount(); uj++)
            {
                if (uj != 0)
                    *ms_pkOstr << ", ";

                *ms_pkOstr << (const char*) pkRef->GetSemanticNameAt(uj) << 
                    " " << pkRef->GetSemanticIndexAt(uj);
            }

            *ms_pkOstr << " } Size: " << pkRef->GetSize() << " Stride: " <<
                pkRef->GetStride() << std::endl; 

            // Show Values
            if (ms_bShowMeshDataStreamsVerbose)
            {
                for (NiUInt32 uj = 0; uj < pkRef->GetElementDescCount(); uj++)
                {
                    PRINT_INDENT((usIndent + 4) * ms_usTabStop);
                    *ms_pkOstr << (const char*) pkRef->GetSemanticNameAt(uj) <<
                        " " << pkRef->GetSemanticIndexAt(uj) << " (" <<
                        pkRef->GetElementDescAt(uj).GetFormatString() << ") "
                        << ": ";

                    const NiDataStreamElement& kDataStreamElement =
                        pkRef->FindElementDesc(
                        (const char*)pkRef->GetSemanticNameAt(uj), 
                        pkRef->GetSemanticIndexAt(uj));

                    NiDataStreamElementLock kElementLock(
                        pkMesh,
                        (const char*)pkRef->GetSemanticNameAt(uj),
                        pkRef->GetSemanticIndexAt(uj),
                        NiDataStreamElement::F_UNKNOWN,
                        NiDataStream::LOCK_TOOL_READ);

                    if (!kElementLock.IsLocked())
                        continue;

                    switch(kDataStreamElement.GetType())
                    {
                    case NiDataStreamElement::T_FLOAT16:
                        PRINT_ELEMENT_VALUES(NiFloat16, 
                            kDataStreamElement.GetComponentCount());
                        break;
                    case NiDataStreamElement::T_FLOAT32:
                        PRINT_ELEMENT_VALUES(float, 
                            kDataStreamElement.GetComponentCount());
                        break;
                    case NiDataStreamElement::T_INT16:
                        PRINT_ELEMENT_VALUES(NiInt16,
                            kDataStreamElement.GetComponentCount());
                        break;
                    case NiDataStreamElement::T_INT32:
                        PRINT_ELEMENT_VALUES(NiInt32,
                            kDataStreamElement.GetComponentCount());
                        break;
                    case NiDataStreamElement::T_INT8:
                        PRINT_ELEMENT_VALUES(NiInt8,
                            kDataStreamElement.GetComponentCount());
                        break;
                    case NiDataStreamElement::T_UINT16:
                        PRINT_ELEMENT_VALUES(NiUInt16,
                            kDataStreamElement.GetComponentCount());
                        break;
                    case NiDataStreamElement::T_UINT32:
                        PRINT_ELEMENT_VALUES(NiUInt32,
                            kDataStreamElement.GetComponentCount());
                        break;
                    case NiDataStreamElement::T_UINT8:
                        PRINT_ELEMENT_VALUES(NiUInt8,
                            kDataStreamElement.GetComponentCount());
                        break;
                    default:
                        NIASSERT(0 && "Unknown format type.");
                        continue;
                    }
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiSceneGraphPrinter::RecursivePrint(NiObject* pkObject,
    unsigned short usIndent)
{
    if (ms_bShowNodes == true || NiIsKindOf(NiNode, pkObject) == false)
    {
        //
        // Print current node
        // 
        PrintID(pkObject, usIndent);

        if (ms_bShowAppCulled)
            PrintAppCulled(pkObject, usIndent);

        if (ms_bShowTransforms)
            PrintTransforms(pkObject, usIndent);

        if (ms_bShowBoundingSpheres)
            PrintBoundingSpheres(pkObject, usIndent);

        if (ms_bShowProperties)
            PrintProperties(pkObject, usIndent);

        if (ms_bShowExtraData)
            PrintExtraData(pkObject, usIndent);

        if (ms_bShowModifiers)
        {
            if (NiIsKindOf(NiMesh, pkObject))
            {
                NiMesh* pkMesh = (NiMesh*)pkObject;
                NiUInt32 uiModCnt = pkMesh->GetModifierCount();
                for(NiUInt32 uiMod=0; uiMod<uiModCnt; uiMod++)
                {
                    NiMeshModifier* pkMod = pkMesh->GetModifierAt(uiMod);
                    char acBuf[512];
                    if (!pkMod->GetStreamableRTTIName(acBuf, 512))
                        continue;

                    PRINT_INDENT((usIndent + 2) * ms_usTabStop);
                    *ms_pkOstr << acBuf << std::endl;
                }
            }
        }

        if (ms_bShowMeshDataStreams || ms_bShowMeshDataStreamsVerbose)
            PrintMeshDataStreams(pkObject, usIndent);

        if (ms_bShowTimeControllers)
        {
            if (NiIsKindOf(NiObjectNET, pkObject))
            {
                NiTimeController* pControl = 
                    ((NiObjectNET *) pkObject)->GetControllers();

                for (/**/; pControl; pControl = pControl->GetNext())
                    RecursivePrint(pControl, usIndent+1);
            }
        }

        //  Don't print AppCulled subtree if ms_bDropAppCulled is true
        if (ms_bDropAppCulled && NiIsKindOf(NiAVObject, pkObject) &&
                ((NiAVObject*)pkObject)->GetAppCulled())
        {
            return;
        }
    }

    if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pNode = (NiNode*)pkObject;

        //
        // Iterate over all children
        //
        for (unsigned int i = 0; i < pNode->GetArrayCount(); i++)
        {
            NiAVObject* pChild = pNode->GetAt(i);

            if (pChild)
            {
                if (ms_bShowNodes == false)
                    RecursivePrint(pChild, usIndent);
                else
                    RecursivePrint(pChild, usIndent + 1);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiSceneGraphPrinter::SummaryPrint()
{
    *ms_pkOstr << "Total Object Count: " << ms_uiObjectCount 
        << ", Tree Depth: " << (ms_uiDepth + 1) << "\n\n";

    if (ms_bShowMeshDataStreams || ms_bShowMeshDataStreamsVerbose)
        PrintTriSummary();

    InitStats();
}
//---------------------------------------------------------------------------
void NiSceneGraphPrinter::PrintTriSummary()
{
    *ms_pkOstr << "TriShape:" << std::endl;
    *ms_pkOstr << "    Data objects = " << ms_uiTriShapeCount << std::endl;
    *ms_pkOstr << "    TriCount     = " << ms_uiTriShape_TriCount << 
        std::endl;
    *ms_pkOstr << "    Avg Ratio    = ";
    *ms_pkOstr << (((float)(ms_uiTriShape_TriCount) /
        (float)ms_uiObjectCount)) << std::endl;
    *ms_pkOstr << "    Low TriCount = " << ms_uiTriShape_LowTriCount << 
        std::endl;
    *ms_pkOstr << "    High TriCount= " << ms_uiTriShape_HighTriCount << 
        std::endl;
    *ms_pkOstr << "    Zero         = " << ms_uiTriShape_ZeroTriCount << 
        std::endl;
    *ms_pkOstr << "TriStrips:" << std::endl;
    *ms_pkOstr << "    Data objects = " << ms_uiTriStripsCount << std::endl;
    *ms_pkOstr << "    TriCount     = " << ms_uiTriStrips_TriCount << 
        std::endl;
    *ms_pkOstr << "    Avg Ratio    = ";
    *ms_pkOstr << (((float)(ms_uiTriStrips_TriCount) /
        (float)ms_uiObjectCount)) << std::endl;
    *ms_pkOstr << "    Low TriCount = " << ms_uiTriStrips_LowTriCount << 
        std::endl;
    *ms_pkOstr << "    High TriCount= " << ms_uiTriStrips_HighTriCount << 
        std::endl;
    *ms_pkOstr << "    Zero         = " << ms_uiTriStrips_ZeroTriCount << 
        std::endl;

}
//---------------------------------------------------------------------------
void NiSceneGraphPrinter::InitStats()
{
    // reset summary statistics
    ms_uiObjectCount = ms_uiDepth = ms_uiTriShapeCount = 
        ms_uiTriShape_TriCount = ms_uiTriShape_HighTriCount = 
        ms_uiTriShape_ZeroTriCount = ms_uiTriStripsCount = 
        ms_uiTriStrips_TriCount = ms_uiTriStrips_HighTriCount =
        ms_uiTriStrips_ZeroTriCount = 0;

    ms_uiTriShape_LowTriCount = ms_uiTriStrips_LowTriCount = 0;

    ms_bFirstTriShape = ms_bFirstTriStrips = true;
}

//---------------------------------------------------------------------------
void NiSceneGraphPrinter::OpenLog()
{
    if (ms_pkOstr == &std::cout)
    {
#if defined(_XENON)
        ms_pkOstr = new std::ofstream("D:\\NiSceneGraph.txt", std::ios::app);
#else   //#if defined(_XENON)
        ms_pkOstr = new std::ofstream("NiSceneGraph.txt", std::ios::app);
#endif  //#if defined(_XENON)
    }

    InitStats();
}
//---------------------------------------------------------------------------
void NiSceneGraphPrinter::OpenNewLog(const char* pcLogFile)
{
    if (ms_pkOstr == &std::cout)
    {
#if defined(_XENON)
        NiString strNewLog = "D:\\";
        strNewLog.Concatenate(pcLogFile);
        ms_pkOstr = new std::ofstream(strNewLog, std::ios::out);
#else   //#if defined(_XENON)
        ms_pkOstr = new std::ofstream(pcLogFile, std::ios::out);
#endif  //#if defined(_XENON)
    }

    InitStats();
}
//---------------------------------------------------------------------------
void NiSceneGraphPrinter::CloseLog()
{
    if (ms_pkOstr != &std::cout)
        delete ms_pkOstr;
    ms_pkOstr = &std::cout;
}
//---------------------------------------------------------------------------

