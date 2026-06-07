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

#include "NiMaterialConfigurator.h"
#include "NiCodeBlock.h"

#if defined(WIN32)
#include <direct.h>
#endif //#if defined(WIN32) 

//---------------------------------------------------------------------------
NiMaterialConfigurator::NiMaterialConfigurator(const NiFixedString& kPlatform,
        const NiFixedString& kPlatformSpecificCodeID, 
        const NiTObjectPtrSet<NiFixedString>* kValidCompileTargets) : 
    m_kNodes(10,10), m_kBindings(10, 10), m_eDataInputType(INPUT_STRUCT)
{
    m_kPlatform = kPlatform;
    m_kPlatformSpecificCodeID = kPlatformSpecificCodeID;
    m_pkValidCompileTargets = kValidCompileTargets;

}
//---------------------------------------------------------------------------
unsigned int NiMaterialConfigurator::GetNodeCount()
{
    return m_kNodes.GetSize();
}
//---------------------------------------------------------------------------
void NiMaterialConfigurator::AddNode(NiMaterialNode* pkFrag)
{
    NIASSERT(pkFrag);
    m_kNodes.Add(pkFrag);
}
//---------------------------------------------------------------------------
NiMaterialNode* NiMaterialConfigurator::GetNode(unsigned int uiFrag)
{
    NIASSERT(uiFrag < GetNodeCount());
    return m_kNodes.GetAt(uiFrag);
}
//---------------------------------------------------------------------------
NiMaterialConfigurator::ProgramDataInputType NiMaterialConfigurator::
    GetDataInputType()
{
    return m_eDataInputType;
}
//---------------------------------------------------------------------------
void NiMaterialConfigurator::SetProgramDataInputType(
    ProgramDataInputType eDataInputType)
{
    m_eDataInputType = eDataInputType;
}
//---------------------------------------------------------------------------
unsigned int NiMaterialConfigurator::GetBindingCount()
{
    return m_kBindings.GetSize();
}
//---------------------------------------------------------------------------
NiMaterialResourceBinding* NiMaterialConfigurator::GetBinding(
    unsigned int uiBind)
{
    NIASSERT(uiBind < GetBindingCount());
    return m_kBindings.GetAt(uiBind);
}
//---------------------------------------------------------------------------
bool NiMaterialConfigurator::AddBinding(NiMaterialResourceBinding* pkBinding)
{
    if (!CanBind(pkBinding))
    {
        NIASSERT(!"Invalid binding");
        return false;
    }

    m_kBindings.Add(pkBinding);
    return true;
}
//---------------------------------------------------------------------------
bool NiMaterialConfigurator::CanBind(NiMaterialResourceBinding* pkBinding)
{
    if (!pkBinding)
        return false;

    NiMaterialResource* pkSrcRes = pkBinding->GetSourceResource();
    NiMaterialResource* pkDestRes = pkBinding->GetDestinationResource();
    
    if (!pkDestRes || !pkSrcRes)
        return false;

    const NiFixedString& kSrcType = pkSrcRes->GetType();
    const NiFixedString& kDestType = pkDestRes->GetType();

    return CanBindTypes(kSrcType, kDestType);
}
//---------------------------------------------------------------------------
bool NiMaterialConfigurator::CanBindTypes(const NiFixedString& kSrcType, 
    const NiFixedString& kDestType)
{
    if (kSrcType != kDestType)
    {
        if (kSrcType == "float4")
        {
            if (kDestType == "float3")
                return true;
            else if (kDestType == "float2")
                return true;
            else if (kDestType == "float")
                return true;
        }
        else if (kSrcType == "float3")
        {
            if (kDestType == "float2")
                return true;
            else if (kDestType == "float")
                return true;
        }
        else if (kSrcType == "float2")
        {
            if (kDestType == "float")
                return true;
        }
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiMaterialNode* NiMaterialConfigurator::GetNodeContainingResource(
    NiMaterialResource* pkRes)
{
    for (unsigned int ui = 0; ui < GetNodeCount(); ui++)
    {
        NiMaterialNode* pkNode = GetNode(ui);
        if (!pkNode)
            continue;
        if (pkNode->ContainsResource(pkRes))
            return pkNode;
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiMaterialNode* NiMaterialConfigurator::GetNodeByName(
    const NiFixedString& kName, unsigned int uiOccurance)
{
    unsigned int uiCount = 0;
    for (unsigned int ui = 0; ui < GetNodeCount(); ui++)
    {
        NiMaterialNode* pkNode = GetNode(ui);
        if (!pkNode)
            continue;

        if (pkNode->GetName() == kName)
        {
            if (uiCount != uiOccurance)
                uiCount++;
            else
                return pkNode;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
bool NiMaterialConfigurator::AddBinding(NiMaterialResource* pkOutputRes,
    NiMaterialResource* pkInputRes)
{
    if (pkInputRes == NULL || pkOutputRes == NULL)
    {
        NIASSERT(!"Invalid binding");
        return false;
    }

    NiMaterialNode* pkOutputNode = GetNodeContainingResource(pkOutputRes);
    NiMaterialNode* pkInputNode = GetNodeContainingResource(pkInputRes);
    
    if (pkInputNode == NULL || pkOutputNode == NULL)
    {
        NIASSERT(!"Invalid binding");
        return false;
    }

    NiMaterialResourceBinding* pkBinding = NiNew NiMaterialResourceBinding(
        pkOutputRes, pkOutputNode, pkInputRes, pkInputNode);
    return AddBinding(pkBinding);
}
//---------------------------------------------------------------------------
bool NiMaterialConfigurator::AddBinding(const NiFixedString& kOutputVar, 
    NiMaterialNode* pkOutputNode, const NiFixedString& kInputVar,
    NiMaterialNode* pkInputNode)
{
    NiMaterialResource* pkOutRes = 
        pkOutputNode->GetOutputResourceByVariableName(kOutputVar);
    NiMaterialResource* pkInRes =
        pkInputNode->GetInputResourceByVariableName(kInputVar);

    if (pkInRes == NULL || pkOutRes == NULL)
    {
        NIASSERT(!"Invalid binding");
        return false;
    }

    NiMaterialResourceBinding* pkBinding = NiNew NiMaterialResourceBinding(
        pkOutRes, pkOutputNode, pkInRes, pkInputNode);
    return AddBinding(pkBinding);
}
//---------------------------------------------------------------------------
bool NiMaterialConfigurator::AddBinding(NiMaterialResource* pkOutRes,
    const NiFixedString& kInputVar, NiMaterialNode* pkInputNode)
{
    NiMaterialResource* pkInRes =
        pkInputNode->GetInputResourceByVariableName(kInputVar);

    if (pkInRes == NULL || pkOutRes == NULL)
    {
        NIASSERT(!"Invalid binding");
        return false;
    }

    NiMaterialNode* pkOutputNode = GetNodeContainingResource(pkOutRes);

    if (pkInputNode == NULL || pkOutputNode == NULL)
    {
        NIASSERT(!"Invalid binding");
        return false;
    }

    NiMaterialResourceBinding* pkBinding = NiNew NiMaterialResourceBinding(
        pkOutRes, pkOutputNode, pkInRes, pkInputNode);
    return AddBinding(pkBinding);
}
//---------------------------------------------------------------------------
bool NiMaterialConfigurator::AddBinding(const NiFixedString& kOutputVar, 
    NiMaterialNode* pkOutputNode, NiMaterialResource* pkInRes)
{
    NiMaterialResource* pkOutRes = 
        pkOutputNode->GetOutputResourceByVariableName(kOutputVar);
    
    if (pkInRes == NULL || pkOutRes == NULL)
    {
        NIASSERT(!"Invalid binding");
        return false;
    }


    NiMaterialNode* pkInputNode = GetNodeContainingResource(pkInRes);
    NiMaterialResourceBinding* pkBinding = NiNew NiMaterialResourceBinding(
        pkOutRes, pkOutputNode, pkInRes, pkInputNode);
    return AddBinding(pkBinding);
}
//---------------------------------------------------------------------------
NiMaterialResourceBinding* NiMaterialConfigurator::GetOutputBindingForInput(
    NiMaterialResource* pkRes, unsigned int uiWhichOne)
{
    unsigned int uiFoundCount = 0;
    for (unsigned int ui = 0; ui < GetBindingCount(); ui++)
    {
        NiMaterialResourceBinding* pkBind = GetBinding(ui);
        NIASSERT(pkBind);

        if (pkBind->GetDestinationResource() == pkRes)
        {
            if (uiFoundCount == uiWhichOne)
                return pkBind;
            else
                uiFoundCount++;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiMaterialResourceBinding* NiMaterialConfigurator::GetInputBindingForOutput(
    NiMaterialResource* pkRes, unsigned int uiWhichOne)
{
    unsigned int uiFoundCount = 0;
    for (unsigned int ui = 0; ui < GetBindingCount(); ui++)
    {
        NiMaterialResourceBinding* pkBind = GetBinding(ui);
        NIASSERT(pkBind);

        if (pkBind->GetSourceResource() == pkRes)
        {
            if (uiFoundCount == uiWhichOne)
                return pkBind;
            else
                uiFoundCount++;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
void NiMaterialConfigurator::SetVariableNameForAllOutputBindings(
    NiMaterialResource* pkRes, const NiFixedString& kVariable, 
    bool bOverwriteExisting)
{
    unsigned int uiWhichOne = 0;
    NiMaterialResourceBinding* pkBinding = GetOutputBindingForInput(pkRes, 
        uiWhichOne);
    while (pkBinding)
    {
        if (bOverwriteExisting)
            pkBinding->SetVariable(kVariable);
        else if (pkBinding->GetVariable().GetLength() < 1 )
            pkBinding->SetVariable(kVariable);

        uiWhichOne++;
        pkBinding = GetOutputBindingForInput(pkRes, uiWhichOne);
    }
}
//---------------------------------------------------------------------------
void NiMaterialConfigurator::SetVariableNameForAllInputBindings(
    NiMaterialResource* pkRes, const NiFixedString& kVariable, 
    bool bOverwriteExisting)
{
    unsigned int uiWhichOne = 0;
    NiMaterialResourceBinding* pkBinding = GetInputBindingForOutput(pkRes, 
        uiWhichOne);
    while (pkBinding)
    {
        if (bOverwriteExisting)
            pkBinding->SetVariable(kVariable);
        else if (pkBinding->GetVariable().GetLength() < 1 )
            pkBinding->SetVariable(kVariable);

        uiWhichOne++;
        pkBinding = GetInputBindingForOutput(pkRes, uiWhichOne);
    }
}
//---------------------------------------------------------------------------
void NiMaterialConfigurator::Reset()
{
    m_kNodes.RemoveAll();
    m_kBindings.RemoveAll();
}
//---------------------------------------------------------------------------
const NiFixedString& NiMaterialConfigurator::GetPlatformString() const
{
    return m_kPlatform;
}
//---------------------------------------------------------------------------
NiString NiMaterialConfigurator::Evaluate(const char*,
    NiGPUProgram::ProgramType, 
    NiTObjectPtrSet<NiMaterialResourcePtr>& kUniforms)
{
    if (m_kBindings.GetSize() == 0)
        return NiString(0);

    // Iterate through all fragments, find any unresolved resources
    // add to Input/Output bindings
    NiString strInStruct(1024);
    NiString strOutStruct(1024);

    NiString strFunctions(1024);
    NiString strConstants(1024);

    NiString strMain(1024);

    strConstants +=
        "/*\n"
        "Shader description:\n";

    strConstants += m_kDescription;

    strConstants += "\n*/\n\n";

    strConstants += 
        "//------------------------------------------------------------------"
        "---------\n"
        "// Types:\n"
        "#if defined(DIRECT3D)\n"
        "#define BONEMATRIX_TYPE float4x3\n"
        "#else\n"
        "#define BONEMATRIX_TYPE float3x4\n"
        "#endif\n"
        "//------------------------------------------------------------------"
        "---------\n\n";

    strConstants += 
        "//------------------------------------------------------------------"
        "---------\n"
        "// Constant variables:\n"
        "//------------------------------------------------------------------"
        "---------\n\n";

    strFunctions += 
        "//------------------------------------------------------------------"
        "---------\n"
        "// Functions:\n"
        "//------------------------------------------------------------------"
        "---------\n\n";

    strInStruct += 
        "//------------------------------------------------------------------"
        "---------\n"
        "// Input:\n"
        "//------------------------------------------------------------------"
        "---------\n\n";
    strInStruct +=
        "struct Input\n"
        "{\n";

    strOutStruct += 
        "//------------------------------------------------------------------"
        "---------\n"
        "// Output:\n"
        "//------------------------------------------------------------------"
        "---------\n\n";
    strOutStruct +=
        "struct Output\n"
        "{\n";

    strMain += 
        "//------------------------------------------------------------------"
        "---------\n"
        "// Main():\n"
        "//------------------------------------------------------------------"
        "---------\n\n";

    if (m_eDataInputType == INPUT_XENON_INDEX)
    {
        strMain += "Output Main(int iIndex : INDEX)\n"
            "{\n"
            "    Input In;\n"
            "    Output Out;\n";
    }
    else
    {
        strMain += "Output Main(Input In)\n"
            "{\n"
            "    Output Out;\n";
    }

    NiTObjectPtrSet<NiMaterialResourcePtr> kInputs;
    NiTObjectPtrSet<NiMaterialResourcePtr> kOutputs;
    NiTObjectPtrSet<NiMaterialResourcePtr> kPixelOutputs;
    kUniforms.RemoveAll();

    // Add shader inputs/outputs/functions
    NiTObjectPtrSet<NiMaterialNodePtr> kTraversal;
    NiTObjectPtrSet<NiMaterialNodePtr> kFreeNodes;
    NiTObjectPtrSet<NiCodeBlockPtr> kWrittenFunctions;

    for (unsigned int uiNode = 0; uiNode < GetNodeCount(); uiNode++)
    {
        NiMaterialNode* pkNode = GetNode(uiNode);
        kFreeNodes.Add(pkNode);
    }

    while (kFreeNodes.GetSize() != 0)
    {
        for (unsigned int ui = 0; ui < kFreeNodes.GetSize(); ui++)
        {
            bool bHasAllInputsResolved = true;
            NiMaterialNode* pkNode = kFreeNodes.GetAt(ui);
            NIASSERT(pkNode);

            for (unsigned int uiRes = 0; 
                uiRes < pkNode->GetInputResourceCount();
                uiRes++)
            {
                NiMaterialResourceBinding* pkBinding = 
                    GetOutputBindingForInput(pkNode->GetInputResource(uiRes));

                if (pkBinding && kTraversal.Find(
                    pkBinding->GetSourceNode()) == -1)
                {
                    bHasAllInputsResolved = false;
                    break;
                }
            }

            if (bHasAllInputsResolved)
            {
                kTraversal.Add(pkNode);
                kFreeNodes.OrderedRemoveAt(ui);
                continue;
            }
        }      
    }

    if (m_eDataInputType == INPUT_XENON_INDEX)
    {
        // An input data type of 'INPUT_XENON_INDEX' implies that we are 
        // generating an instancing shader for xenon. Add the shader constant
        // 'XenonInstanceFrequency' to the shader constant map so the shader 
        // can determine the data frequency at runtime.

        NiMaterialResource* pkRes = NiNew NiMaterialResource();
        pkRes->SetType("float");
        pkRes->SetVariable("g_XenonInstanceFrequency");
        pkRes->SetLabel("");
        pkRes->SetSemantic("XenonInstanceFrequency");
        pkRes->SetCount(1);
        pkRes->SetSource(NiMaterialResource::SOURCE_PREDEFINED);
        pkRes->SetExtraData(0);
        pkRes->SetObjectType(NiShaderAttributeDesc::OT_UNDEFINED);
        kUniforms.Add(pkRes);
    }


    for (unsigned int ui = 0; ui < kTraversal.GetSize(); ui++)
    {
        NiMaterialNode* pkNode = kTraversal.GetAt(ui);
        if (NiIsKindOf(NiMaterialResourceConsumerNode, pkNode))
        {
            NiMaterialResourceConsumerNode* pkDerNode =
                (NiMaterialResourceConsumerNode*) pkNode;

            for (unsigned int uiRes = 0; uiRes < 
                pkDerNode->GetInputResourceCount(); uiRes++)
            {
                NiString strBindVarName;
                NiMaterialResource* pkRes = pkDerNode->GetInputResource(uiRes);
                kOutputs.Add(pkRes);
                strBindVarName += "Out.";

                strBindVarName += pkRes->GetVariable();
                SetVariableNameForAllOutputBindings(pkRes, 
                    (const char*)strBindVarName);

                SetVariableNameForAllInputBindings(pkRes, 
                    (const char*)strBindVarName, false);
            }
        }
        if (NiIsKindOf(NiMaterialResourceProducerNode, pkNode))
        {
            NiMaterialResourceProducerNode* pkDerNode =
                (NiMaterialResourceProducerNode*) pkNode;

            for (unsigned int uiRes = 0; 
                uiRes < pkDerNode->GetOutputResourceCount();
                uiRes++)
            {
                NiString strBindVarName;
                NiMaterialResource* pkRes = 
                    pkDerNode->GetOutputResource(uiRes);

                if (pkDerNode->GetProducerType() == "Uniform")
                {
                    kUniforms.Add(pkRes);
                }
                else if (pkDerNode->GetProducerType() == "Static")
                {
                }
                else
                {
                    kInputs.Add(pkRes);
                    strBindVarName += "In."; 
                }

                strBindVarName += pkRes->GetVariable();
                SetVariableNameForAllInputBindings(pkRes, 
                    (const char*)strBindVarName);
            }
        }


        if (NiIsKindOf(NiMaterialFragmentNode, pkNode))
        {
            NiMaterialFragmentNode* pkFragNode = 
                (NiMaterialFragmentNode*) pkNode;

            if (-1 == kWrittenFunctions.Find(pkFragNode->GetCodeBlock(
                m_kPlatformSpecificCodeID, m_kPlatform, 
                m_pkValidCompileTargets)))
            {
                strFunctions += GenerateFunctionString(
                    (NiMaterialFragmentNode*)pkFragNode);
                kWrittenFunctions.Add(pkFragNode->GetCodeBlock(
                    m_kPlatformSpecificCodeID, 
                    m_kPlatform,
                    m_pkValidCompileTargets));
            }
        }
    }

    for (unsigned int ui = 0; ui < kUniforms.GetSize(); ui++)
    {
        NiMaterialResource* pkRes = kUniforms.GetAt(ui);
        strConstants += FormatType(pkRes->GetType());
        strConstants += " ";
        strConstants += pkRes->GetVariable();
        if (pkRes->GetCount() != 1)
        {
            char acString[32];
            NiSprintf(acString, 32, "%d", pkRes->GetCount());
            strConstants += "[";
            strConstants += acString;
            strConstants += "]";
        }
        
        strConstants += ";\n";
    }
    
    if (m_eDataInputType == INPUT_XENON_INDEX)
    {   
        strMain += GenerateXenonVFetches(kInputs);
    }

    strInStruct += GenerateInOutStruct(kInputs);
    strInStruct += "\n};\n\n";

    strOutStruct += GenerateInOutStruct(kOutputs);
    strOutStruct += "\n};\n\n";

    unsigned int uiFragsCalled = 0;
    for (unsigned int ui = 0; ui < kTraversal.GetSize(); ui++)
    {
        NiMaterialNode* pkNode = kTraversal.GetAt(ui);
        if (NiIsKindOf(NiMaterialFragmentNode, pkNode))
        { 
            strMain += GenerateFunctionCall(
                (NiMaterialFragmentNode*) pkNode, uiFragsCalled);
            strMain += "\n";
            uiFragsCalled++;
        }
        else if (NiIsKindOf(NiMaterialResourceConsumerNode, pkNode))
        {
            NiMaterialResourceConsumerNode* pkTransNode = 
                (NiMaterialResourceConsumerNode*)pkNode;

            strMain += ResolvePassthroughVariables(pkTransNode);
        }
    }
    strMain += "    return Out;\n";
    strMain += "}\n\n";

    // Construct file name
    char acString[NI_MAX_PATH];
    acString[0] = '\0';

    NiString strPreProc(1024);
    
    int iLength = 
        strPreProc.Length() + 
        strInStruct.Length() +
        strOutStruct.Length() + 
        strFunctions.Length() +
        strConstants.Length() +
        strMain.Length();

    NiString strFile(iLength);
    strFile += strPreProc;
    strFile += strConstants;
    strFile += strFunctions;

    strFile += strInStruct;
    strFile += strOutStruct;
    strFile += strMain;
   
    return strFile;
}
//---------------------------------------------------------------------------
NiString NiMaterialConfigurator::FormatVariableDeclaration(
    NiMaterialResource* pkResource,
    bool bAddSemanticAndLabel)
{
    NiString strOutput(1024);
    strOutput += FormatType(pkResource->GetType());
    strOutput += " ";
    strOutput += pkResource->GetVariable();
    if (pkResource->GetCount() != 1)
    {
        char acString[32];
        NiSprintf(acString, 32, "%d", pkResource->GetCount());
        strOutput += "[";
        strOutput += acString;
        strOutput += "]";
    }

    if (bAddSemanticAndLabel)
    {
        NiFixedString kSemantic = pkResource->GetSemantic();
        if (kSemantic.GetLength() > 0)
        {
            strOutput += " : ";
            strOutput += kSemantic;

            NiFixedString kLabel = pkResource->GetLabel();
            if (kLabel.GetLength() > 0)
            {
                strOutput += "_";
                strOutput += kLabel;
            }
        }
    }

    return strOutput;
}
//---------------------------------------------------------------------------
NiString NiMaterialConfigurator::FormatType(const NiFixedString& kStr)
{
    return (NiString)kStr;
}
//---------------------------------------------------------------------------
NiString NiMaterialConfigurator::GenerateFunctionString(
    NiMaterialFragmentNode* pkFragNode)
{
    NiString strFunctions(1024);
    if (pkFragNode->GetDescription())
    {
        strFunctions += "/*\n";
        strFunctions += pkFragNode->GetDescription();
        strFunctions += "\n*/\n";
    }
    strFunctions += "\nvoid ";
    strFunctions += pkFragNode->GetName();
    strFunctions += "(";
    unsigned int uiNumArgs = 0;
    for (unsigned int uiInRes = 0; 
        uiInRes < pkFragNode->GetInputResourceCount(); uiInRes++)
    {
        NiMaterialResource* pkRes = 
            pkFragNode->GetInputResource(uiInRes);

        if (uiNumArgs > 0)
            strFunctions += ",\n    ";

        strFunctions += FormatVariableDeclaration(pkRes, false);
        uiNumArgs++;
    }

    for (unsigned int uiOutRes = 0; 
        uiOutRes < pkFragNode->GetOutputResourceCount(); uiOutRes++)
    {
        NiMaterialResource* pkRes = 
            pkFragNode->GetOutputResource(uiOutRes);

        if (uiNumArgs > 0)
            strFunctions += ",\n    ";

        strFunctions += "out ";
        strFunctions += FormatVariableDeclaration(pkRes, false);
        uiNumArgs++;
    }

    strFunctions += ")\n";
    strFunctions += "{\n";

    const NiCodeBlock* pkBlock = pkFragNode->GetCodeBlock(
        m_kPlatformSpecificCodeID, m_kPlatform, 
        m_pkValidCompileTargets);
    NIASSERT(pkBlock);

    strFunctions += pkBlock->GetText();
    strFunctions += "\n}\n";
    strFunctions += "//--------------------------------------"
        "-------------------------------------\n";
    return strFunctions;
}
//---------------------------------------------------------------------------
NiString NiMaterialConfigurator::GenerateInOutStruct(
    const NiTObjectPtrSet<NiMaterialResourcePtr>& kInputs)
{
    const unsigned int uiMaxSemantics = 16;
    NIASSERT(uiMaxSemantics >= GetPlatformSpecificSemanticCount());

    NiTPointerList<NiMaterialResource*> kOrderedList;
    NiTListIterator akIterators[uiMaxSemantics + 1];
    memset(akIterators, 0, sizeof(akIterators));

    for (unsigned int i = 0; i < kInputs.GetSize(); i++)
    {
        NiMaterialResource* pkRes = kInputs.GetAt(i);
        NIASSERT(pkRes);
        unsigned int uiOrdering = 
            GetPlatformSpecificInOutSemanticOrder(
            pkRes->GetSemantic());
        NiTListIterator kIter = akIterators[uiOrdering];
        int iTemp = (int)uiOrdering - 1;
        while (kIter == 0 && iTemp >= 0)
        {
            // If this semantic not encountered yet, find the position of the
            // last semantic thats in front of this one
            kIter = akIterators[iTemp--];
        }

        if (kIter == 0)
        {
            // If none entered at all before this one, put this one at the 
            // front
            kOrderedList.AddHead(pkRes);
            kIter = kOrderedList.GetHeadPos();
        }
        else
        {
            // Else put this one after the previous one
            kIter = kOrderedList.InsertAfter(kIter, pkRes);
        }
        akIterators[uiOrdering] = kIter;
    }

    NiString strStruct(1024);
    unsigned int uiElements = 0;

    NiTListIterator kIter = kOrderedList.GetHeadPos();
    NiString kCurrentSemantic = 0;
    unsigned int uiCurrentSemanticCount = 0;
    while (kIter)
    {
        NiMaterialResource* pkRes = kOrderedList.GetNext(kIter);
        NIASSERT(pkRes);
        NiString kNewSemantic = GetPlatformSpecificInOutSemantic(
            pkRes->GetSemantic());
        if (kNewSemantic.Compare(kCurrentSemantic) == 0)
        {
            uiCurrentSemanticCount++;
        }
        else
        {
            kCurrentSemantic = kNewSemantic;
            uiCurrentSemanticCount = 0;
        }

        strStruct += "    ";
        strStruct += FormatType(pkRes->GetType());
        strStruct += " ";
        strStruct += pkRes->GetVariable();
        strStruct += " : ";
        strStruct += kCurrentSemantic;
        NIASSERT(pkRes->GetCount() == 1);
        strStruct += NiString::FromInt(uiCurrentSemanticCount);
        strStruct += ";\n";
        uiElements++;
    }
    
    return strStruct;
}
//---------------------------------------------------------------------------
NiString NiMaterialConfigurator::GenerateFunctionCall(
    NiMaterialFragmentNode* pkFragNode,
    unsigned int uiUniqueIdentifier)
{
    NiString strCall(1024);
    NiString strFunction(1024);
    NiString strTemps(1024);
    unsigned int uiTempCount = 0;

    unsigned int uiNumArgs = 0;

    strFunction += "    ";
    strFunction += pkFragNode->GetName();
    strFunction += "(";
    unsigned int uiLineLength = strFunction.Length();

    // Generate input variables
    for (unsigned int ui = 0; ui < pkFragNode->GetInputResourceCount(); ui++)
    {
        NiMaterialResource* pkRes = pkFragNode->GetInputResource(ui);
        if (uiNumArgs != 0)
        {
            strFunction += ", ";
            uiLineLength += 2;
        }

        NiString strVar = GetVariableArgumentNameForCall(pkRes, pkFragNode, 
            true, true);

        // Handle default values if none were specified
        if (strVar.IsEmpty() && pkRes->GetDefaultValue().GetLength() > 0)
        {
            strVar = FormatType(pkRes->GetType());
            strVar += (const char*) pkRes->GetDefaultValue();
        }

        NIASSERT(!strVar.IsEmpty());

        if (uiLineLength + strVar.Length() > 79)
        {
            strFunction += "\n        ";
            uiLineLength = 8;
            uiLineLength += strVar.Length();
        }
        else
        {
            uiLineLength += strVar.Length();
        }
        strFunction += strVar;

        uiNumArgs++;
    }

    // Generate output variables
    for (unsigned int ui = 0; ui < pkFragNode->GetOutputResourceCount(); ui++)
    {
        NiMaterialResource* pkRes = pkFragNode->GetOutputResource(ui);
        if (uiNumArgs != 0)
        {
            strFunction += ", ";
            uiLineLength += 2;
        }

        NiString strVar = GetVariableArgumentNameForCall(pkRes, pkFragNode,
            false, false);
        if (strVar.IsEmpty())
        {
            char acName[256];
            NiSprintf(acName, 256, "%s_CallOut%d", 
                (const char*)pkRes->GetVariable(), uiUniqueIdentifier);
            strVar = acName;
            strTemps += "    ";
            strTemps += FormatType(pkRes->GetType());
            strTemps += " ";
            strTemps += strVar;
            strTemps += ";\n";
            uiTempCount++;

            SetVariableNameForAllInputBindings(pkRes, (const char*)strVar);
        }

        if (uiLineLength + strVar.Length() > 79)
        {
            strFunction += "\n        ";
            uiLineLength = 8;
            uiLineLength += strVar.Length();
        }
        else
        {
            uiLineLength += strVar.Length();
        }

        strFunction += strVar;
        uiNumArgs++;
    }
    strFunction += ");";

    char acDigit[48];
    NiSprintf(acDigit, 48, "\t// Function call #%d\n", uiUniqueIdentifier);
    strCall += acDigit;
    strCall += strTemps;
    strCall += strFunction;
    strCall += "\n";
    return strCall;
}
//---------------------------------------------------------------------------
NiString NiMaterialConfigurator::GetVariableArgumentNameForCall(
    NiMaterialResource* pkRes, NiMaterialNode*, bool bIsInput,
    bool bGoToStart)
{
    NiMaterialResourceBinding* pkBinding = NULL;

    unsigned int uiWhichBinding = 0;
    if (bIsInput)
        pkBinding = GetOutputBindingForInput(pkRes, uiWhichBinding);
    else        
        pkBinding = GetInputBindingForOutput(pkRes, uiWhichBinding);

    while (pkBinding != NULL)
    {
        const NiFixedString& kValue = pkBinding->GetVariable();
        if (kValue.GetLength() > 0)
            return kValue;


        uiWhichBinding++;
        if (bIsInput)
        {
            // There can only be one binding per input.
            // Check to see if binding is a constant
            NiMaterialResource* pkSrcRes = pkBinding->GetSourceResource();

            if (pkSrcRes->GetDefaultValue().GetLength() > 0)
            {
                NiString strValue = FormatType(pkSrcRes->GetType());
                strValue += pkSrcRes->GetDefaultValue();
                return strValue;
            }

            // Look at outputs
            return GetVariableArgumentNameForCall(pkSrcRes,
                pkBinding->GetSourceNode(), false, bGoToStart);
        }
        else        
        {
            pkBinding = GetInputBindingForOutput(pkRes, uiWhichBinding);
        }

    }

    return 0;
}
//---------------------------------------------------------------------------
NiString NiMaterialConfigurator::ResolvePassthroughVariables(
    NiMaterialResourceConsumerNode* pkNode)
{
    NiString strPassthroughs(1024);
    for (unsigned int ui = 0; ui < pkNode->GetInputResourceCount(); ui++)
    {
        NiMaterialResource* pkRes = pkNode->GetInputResource(ui);
        unsigned int uiWhichOne = 0;
        NiMaterialResourceBinding* pkBinding = GetOutputBindingForInput(pkRes,
            uiWhichOne);
        while (pkBinding)
        {
            NiMaterialNode* pkSrcNode = pkBinding->GetSourceNode();

            if (NiIsKindOf(NiMaterialResourceProducerNode, pkSrcNode))
            {
                strPassthroughs += "    Out.";
                strPassthroughs += pkRes->GetVariable();
                strPassthroughs += " = ";
                NiMaterialResourceProducerNode* pkMRPNode =
                    (NiMaterialResourceProducerNode*)pkSrcNode;
                NiMaterialResource* pkMaterialRes =
                    pkBinding->GetSourceResource();
                if (pkMRPNode->GetProducerType() == "Static")
                {
                    strPassthroughs += FormatType(pkMaterialRes->GetType());
                    strPassthroughs += (const char*)pkMaterialRes->GetDefaultValue();
                }
                else
                {
                    if (pkMRPNode->GetProducerType() != "Uniform")
                    {
                        strPassthroughs += "In.";
                    }
                    strPassthroughs += pkMaterialRes->GetVariable();
                }
                strPassthroughs += ";\n";
            }

            uiWhichOne++;
            pkBinding = GetOutputBindingForInput(pkRes, uiWhichOne);

        }
    }

    return strPassthroughs;
}
//---------------------------------------------------------------------------
void NiMaterialConfigurator::SetDescription(const char* pcDescription)
{
    m_kDescription = pcDescription;
}
//---------------------------------------------------------------------------
NiString NiMaterialConfigurator::GenerateXenonVFetches(
    NiTObjectPtrSet<NiMaterialResourcePtr>& kInputs)
{
    NiString strVFetchCode =  "    int iDivIndex = (iIndex + 0.5) / "
        "g_XenonInstanceFrequency;\n";
    strVFetchCode += "    int iModIndex = iIndex - "
        "(iDivIndex * g_XenonInstanceFrequency);\n";

    // Generate needed vfetch commands for Xenon.
    for (unsigned int uiInputs = 0; uiInputs < kInputs.GetSize(); 
        uiInputs++)
    {
        NiMaterialResourcePtr spMatRes = 
            (NiMaterialResourcePtr)kInputs.GetAt(uiInputs);

        unsigned int uiCurrentSemanticCount = 0;
        unsigned int uiTotalSemanticCount = 0;

        for (unsigned int ui = 0; ui < kInputs.GetSize(); ui++)
        {
            if (spMatRes->GetSemantic() == 
                ((NiMaterialResourcePtr)kInputs.GetAt(ui))->GetSemantic())
            {
                uiTotalSemanticCount++;
            }
        }

        for (unsigned int ui = 0; ui < uiInputs; ui++)
        {
            if (spMatRes->GetSemantic() == 
                ((NiMaterialResourcePtr)kInputs.GetAt(ui))->GetSemantic())
            {
                uiCurrentSemanticCount++;
            }
        }

        NiString kPlatformSemantic = 
            GetPlatformSpecificInOutSemantic(
            spMatRes->GetSemantic());

        kPlatformSemantic += NiString::FromInt(uiCurrentSemanticCount);
        kPlatformSemantic.ToLower();

        NiString kComputedIndex = "iModIndex";

        if (spMatRes->GetSemantic() == "Position" &&
            uiCurrentSemanticCount > 0)
        {
            kComputedIndex = "iDivIndex";
        }

        NiString kTempVarName = "fTemp";
        kTempVarName += spMatRes->GetVariable();
        strVFetchCode += "    float4 "; 
        strVFetchCode += kTempVarName;
        strVFetchCode += ";\n";

        strVFetchCode += "    asm\n    {\n";
        strVFetchCode += "    vfetch ";
        strVFetchCode += kTempVarName;
        strVFetchCode += ", ";
        strVFetchCode += kComputedIndex;
        strVFetchCode += ", ";
        strVFetchCode += kPlatformSemantic;
        strVFetchCode += ";\n";
        strVFetchCode += "    };\n";

        strVFetchCode += "    In.";
        strVFetchCode += spMatRes->GetVariable();
        strVFetchCode += " = ";
        strVFetchCode += kTempVarName;

        if (spMatRes->GetType() == "float3")
            strVFetchCode += ".xyz";
        else if (spMatRes->GetType() == "float2")
            strVFetchCode += ".xy";
        else if (spMatRes->GetType() == "float")
            strVFetchCode += ".x";

        strVFetchCode += ";\n\n";
    }

    return strVFetchCode;
}
//---------------------------------------------------------------------------
NiString NiMaterialConfigurator::GetPlatformSpecificInOutSemantic(
    const NiFixedString& kAgnosticSemantic)
{
    if (kAgnosticSemantic == "Position")
        return "POSITION";
    if (kAgnosticSemantic == "BlendWeight")
        return "BLENDWEIGHT";
    if (kAgnosticSemantic == "BlendIndices")
        return "BLENDINDICES";
    if (kAgnosticSemantic == "Normal")
        return "NORMAL";
    if (kAgnosticSemantic == "PSize")
        return "PSIZE";
    if (kAgnosticSemantic == "TexCoord")
        return "TEXCOORD";
    if (kAgnosticSemantic == "Tangent")
        return "TANGENT";
    if (kAgnosticSemantic == "Binormal")
        return "BINORMAL";
    if (kAgnosticSemantic == "TessFactor")
        return "TESSFACTOR";
    if (kAgnosticSemantic == "PositionT")
        return "POSITIONT";
    if (kAgnosticSemantic == "Color")
        return "COLOR";
    if (kAgnosticSemantic == "Fog")
        return "FOG";
    if (kAgnosticSemantic == "Depth")
        return "DEPTH";
    if (kAgnosticSemantic == "Sample")
        return "SAMPLE";
    NIASSERT(!"Could not find matching semantic");

    return "TEXCOORD";
}
//---------------------------------------------------------------------------
unsigned int NiMaterialConfigurator::GetPlatformSpecificInOutSemanticOrder(
    const NiFixedString& kAgnosticSemantic)
{
    if (kAgnosticSemantic == "Position")
        return 0;
    if (kAgnosticSemantic == "BlendWeight")
        return 1;
    if (kAgnosticSemantic == "BlendIndices")
        return 2;
    if (kAgnosticSemantic == "Normal")
        return 3;
    if (kAgnosticSemantic == "PSize")
        return 4;
    if (kAgnosticSemantic == "TexCoord")
        return 5;
    if (kAgnosticSemantic == "Tangent")
        return 6;
    if (kAgnosticSemantic == "Binormal")
        return 7;
    if (kAgnosticSemantic == "TessFactor")
        return 8;
    if (kAgnosticSemantic == "PositionT")
        return 9;
    if (kAgnosticSemantic == "Color")
        return 10;
    if (kAgnosticSemantic == "Fog")
        return 11;
    if (kAgnosticSemantic == "Depth")
        return 12;
    if (kAgnosticSemantic == "Sample")
        return 13;
    NIASSERT(!"Could not find matching semantic");

    return 6;
}
//---------------------------------------------------------------------------
unsigned int NiMaterialConfigurator::GetPlatformSpecificSemanticCount()
{
    return 13;
}
//---------------------------------------------------------------------------
