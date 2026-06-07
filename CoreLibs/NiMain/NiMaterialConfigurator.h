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

#ifndef NIMATERIALCONFIGURATOR_H
#define NIMATERIALCONFIGURATOR_H

#include "NiGPUProgram.h"
#include "NiString.h"
#include "NiFixedString.h"
#include "NiTSet.h"
#include "NiMaterialResourceProducerNode.h"
#include "NiMaterialResourceConsumerNode.h"
#include "NiMaterialResourceBinding.h"
#include "NiMaterialFragmentNode.h"
#include "NiGPUProgramCache.h"

class NIMAIN_ENTRY NiMaterialConfigurator : public NiRefObject
{
public:
    enum ProgramDataInputType
    {
        INPUT_STRUCT,
        INPUT_XENON_INDEX
    };

    NiMaterialConfigurator(const NiFixedString& kPlatform,
        const NiFixedString& kPlatformSpecificCodeID, 
        const NiTObjectPtrSet<NiFixedString>* kValidCompileTargets);
    inline virtual ~NiMaterialConfigurator(){};

    unsigned int GetNodeCount();
    NiMaterialNode* GetNode(unsigned int uiNode);
    void AddNode(NiMaterialNode* pkNode);

    ProgramDataInputType GetDataInputType();
    void SetProgramDataInputType(ProgramDataInputType eDataInputType);

    unsigned int GetBindingCount();
    NiMaterialResourceBinding* GetBinding(unsigned int uiBind);
    NiMaterialNode* GetNodeContainingResource(NiMaterialResource* pkRes);
    NiMaterialNode* GetNodeByName(const NiFixedString& kName, 
        unsigned int uiOccurance = 0);
    bool AddBinding(NiMaterialResourceBinding* pkBinding);
    bool AddBinding(NiMaterialResource* pkOutputRes,
        NiMaterialResource* pkInputRes);
    bool AddBinding(const NiFixedString& kOutputVar, 
        NiMaterialNode* pkOutputNode,
        const NiFixedString& kInputVar,
        NiMaterialNode* pkInputNode);
    bool AddBinding(NiMaterialResource* pkOutRes,
        const NiFixedString& kInputVar,
        NiMaterialNode* pkInputNode);
    bool AddBinding(const NiFixedString& kOutputVar, 
        NiMaterialNode* pkOutputNode,
        NiMaterialResource* pkInRes);

    virtual NiString Evaluate(const char* pcName, 
        NiGPUProgram::ProgramType eType, 
        NiTObjectPtrSet<NiMaterialResourcePtr>& kUniforms);

    const NiFixedString& GetPlatformString() const;

    virtual void Reset();

    virtual bool CanBindTypes(const NiFixedString& kSrcType, 
        const NiFixedString& kDestType);

    void SetDescription(const char* pcDescription);
protected:
    virtual bool CanBind(NiMaterialResourceBinding* pkBinding);

    NiMaterialResourceBinding* GetOutputBindingForInput(
        NiMaterialResource* pkRes, unsigned int uiWhichOne = 0);
    NiMaterialResourceBinding* GetInputBindingForOutput(
        NiMaterialResource* pkRes, unsigned int uiWhichOne = 0);
    void SetVariableNameForAllOutputBindings(NiMaterialResource* pkRes,
        const NiFixedString& kVariable, bool bOverwriteExisting = true);
    void SetVariableNameForAllInputBindings(NiMaterialResource* pkRes,
        const NiFixedString& kVariable, bool bOverwriteExisting = true);

    NiString GenerateXenonVFetches(
        NiTObjectPtrSet<NiMaterialResourcePtr>& kInputs);

    NiString FormatVariableDeclaration(NiMaterialResource* pkResource,
        bool bAddSemanticAndLabel = true);
    NiString FormatType(const NiFixedString& kStr);
    NiString GenerateFunctionString(NiMaterialFragmentNode* pkNode);
    NiString GenerateInOutStruct(
        const NiTObjectPtrSet<NiMaterialResourcePtr>& kInputs);
    NiString GenerateFunctionCall(NiMaterialFragmentNode* pkNode,
        unsigned int uiUniqueIdentifier);
    NiString GetVariableArgumentNameForCall(NiMaterialResource* pkRes, 
        NiMaterialNode* pkNode, bool bIsInput, bool bGoToStart);
    NiString ResolvePassthroughVariables(
        NiMaterialResourceConsumerNode* pkNode);

    virtual NiString GetPlatformSpecificInOutSemantic(
        const NiFixedString& kAgnosticSemantic);
    virtual unsigned int GetPlatformSpecificInOutSemanticOrder(
        const NiFixedString& kAgnosticSemantic);
    virtual unsigned int GetPlatformSpecificSemanticCount();

    NiTObjectArray<NiMaterialNodePtr> m_kNodes;
    NiTObjectArray<NiMaterialResourceBindingPtr> m_kBindings;

    ProgramDataInputType m_eDataInputType;
    NiString m_kDescription;
    NiFixedString m_kPlatform;
    NiFixedString m_kPlatformSpecificCodeID;
    const NiTObjectPtrSet<NiFixedString>* m_pkValidCompileTargets;

};

typedef NiPointer<NiMaterialConfigurator> NiMaterialConfiguratorPtr;

#endif  //#ifndef NIMATERIALCONFIGURATOR_H
