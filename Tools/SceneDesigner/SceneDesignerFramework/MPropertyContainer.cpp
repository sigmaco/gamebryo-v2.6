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

#include "SceneDesignerFrameworkPCH.h"

#include "MPropertyContainer.h"
#include "PropertyType.h"
#include "ServiceProvider.h"
#include "MUtility.h"
#include "MMakePropertyUniqueCommand.h"
#include "MEntityPropertyDescriptor.h"
#include "MExpandablePropertyConverter.h"
#include "MPoint2.h"
#include "MPoint3.h"
#include "MQuaternion.h"
#include "MMatrix3.h"
#include "MEntityFactory.h"
#include "MChangePropertyCommand.h"
#include "MSetElementCountCommand.h"
#include "MLayer.h"

using namespace System::Collections;
using namespace System::Drawing;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;

//---------------------------------------------------------------------------
MPropertyContainer::MPropertyContainer() :
    m_bDirtyBit(false),
    m_bWritable(true)
{
}
//---------------------------------------------------------------------------
void MPropertyContainer::Do_Dispose(bool)
{
}
//---------------------------------------------------------------------------
String* MPropertyContainer::get_Name()
{
    MVerifyValidInstance;

    return PropertyInterface->GetName();
}
//---------------------------------------------------------------------------
bool MPropertyContainer::get_Dirty()
{
    MVerifyValidInstance;

    return m_bDirtyBit;
}
//---------------------------------------------------------------------------
void MPropertyContainer::set_Dirty(bool bDirty)
{
    MVerifyValidInstance;

    MAssert(!bDirty || this->Writable, "Object is read-only and cannot be "
        "modified!");

    m_bDirtyBit = bDirty;
    // if we are being marked dirty tell our owning layer
    if (Layer && bDirty)
    {
        Layer->Dirty = bDirty;
    }
}
//---------------------------------------------------------------------------
bool MPropertyContainer::HasProperty(String* strPropertyName)
{
    MVerifyValidInstance;

    NiFixedString kDisplayName;
    const char* pcPropertyName = MStringToCharPointer(strPropertyName);
    bool bSuccess = NIBOOL_IS_TRUE(PropertyInterface->GetDisplayName(
        pcPropertyName, kDisplayName));
    MFreeCharPointer(pcPropertyName);

    return bSuccess;
}
//---------------------------------------------------------------------------
bool MPropertyContainer::IsCollection(String* strPropertyName)
{
    MVerifyValidInstance;

    bool bIsCollection;
    const char* pcPropertyName = MStringToCharPointer(strPropertyName);
    bool bSuccess = NIBOOL_IS_TRUE(PropertyInterface->IsCollection(
        pcPropertyName, bIsCollection));
    MFreeCharPointer(pcPropertyName);
    MAssert(bSuccess, "Property Error: Property name not found!");

    return bIsCollection;
}
//---------------------------------------------------------------------------
unsigned int MPropertyContainer::GetElementCount(String* strPropertyName)
{
    MVerifyValidInstance;

    unsigned int uiCount;
    const char* pcPropertyName = MStringToCharPointer(strPropertyName);
    bool bSuccess = NIBOOL_IS_TRUE(
        PropertyInterface->GetElementCount(pcPropertyName, uiCount));
    MFreeCharPointer(pcPropertyName);
    MAssert(bSuccess, "Property Error: Property name not found!");

    return uiCount;
}
//---------------------------------------------------------------------------
void MPropertyContainer::SetElementCount(String* strPropertyName,
    unsigned int uiCount)
{
    MVerifyValidInstance;

    this->Dirty = true;

    const char* pcPropertyName = MStringToCharPointer(strPropertyName);
    CommandService->ExecuteCommand(new MSetElementCountCommand(
        NiNew NiSetElementCountCommand(PropertyInterface, pcPropertyName,
        uiCount), this), true);
    MFreeCharPointer(pcPropertyName);
}
//---------------------------------------------------------------------------
bool MPropertyContainer::IsPropertyReadOnly(String* strPropertyName)
{
    MVerifyValidInstance;

    bool bIsReadOnly;
    const char* pcPropertyName = MStringToCharPointer(strPropertyName);
    bool bSuccess = NIBOOL_IS_TRUE(PropertyInterface->IsPropertyReadOnly(
        pcPropertyName, bIsReadOnly));
    MFreeCharPointer(pcPropertyName);
    MAssert(bSuccess, "Property Error: Property name not found!");

    return bIsReadOnly;
}
//---------------------------------------------------------------------------
PropertyType* MPropertyContainer::GetPropertyType(String* strPropertyName)
{
    MVerifyValidInstance;

    NiFixedString kSemanticType;
    const char* pcPropertyName = MStringToCharPointer(strPropertyName);
    bool bSuccess = NIBOOL_IS_TRUE(
        PropertyInterface->GetSemanticType(pcPropertyName, kSemanticType));
    MFreeCharPointer(pcPropertyName);
    MAssert(bSuccess, "Property Error: Property name not found!");

    return PropertyTypeService->LookupType(kSemanticType);
}
//---------------------------------------------------------------------------
bool MPropertyContainer::IsPropertyUnique(String* strPropertyName)
{
    MVerifyValidInstance;

    bool bIsUnique;
    const char* pcPropertyName = MStringToCharPointer(strPropertyName);
    bool bSuccess = NIBOOL_IS_TRUE(PropertyInterface->IsPropertyUnique(
        pcPropertyName, bIsUnique));
    MFreeCharPointer(pcPropertyName);
    MAssert(bSuccess, "Property Error: Property name not found!");

    return bIsUnique;
}
//---------------------------------------------------------------------------
bool MPropertyContainer::IsPropertyInheritable(String* strPropertyName)
{
    MVerifyValidInstance;

    bool bIsInheritable;
    const char* pcPropertyName = MStringToCharPointer(strPropertyName);
    bool bSuccess = NIBOOL_IS_TRUE(PropertyInterface->IsPropertyInheritable(
        pcPropertyName, bIsInheritable));
    MFreeCharPointer(pcPropertyName);
    MAssert(bSuccess, "Property Error: Property name not found!");

    return bIsInheritable;
}
//---------------------------------------------------------------------------
String* MPropertyContainer::GetPropertyDisplayName(String* strPropertyName)
{
    MVerifyValidInstance;

    NiFixedString kDisplayName;
    const char* pcPropertyName = MStringToCharPointer(strPropertyName);
    bool bSuccess = NIBOOL_IS_TRUE(PropertyInterface->GetDisplayName(
        pcPropertyName, kDisplayName));
    MFreeCharPointer(pcPropertyName);
    MAssert(bSuccess, "Property Error: Property name not found!");

    return kDisplayName;
}
//---------------------------------------------------------------------------
bool MPropertyContainer::IsExternalAssetPath(String* strPropertyName,
    unsigned int uiIndex)
{
    MVerifyValidInstance;

    bool bIsExternalAssetPath;
    const char* pcPropertyName = MStringToCharPointer(strPropertyName);
    bool bSuccess = NIBOOL_IS_TRUE(PropertyInterface->IsExternalAssetPath(
        pcPropertyName, uiIndex, bIsExternalAssetPath));
    MFreeCharPointer(pcPropertyName);
    MAssert(bSuccess, "Property Error: IsExternalAssetPath failed!");

    return bIsExternalAssetPath;
}
//---------------------------------------------------------------------------
Object* MPropertyContainer::GetPropertyData(String* strPropertyName)
{
    MVerifyValidInstance;

    return GetPropertyData(strPropertyName, 0);
}
//---------------------------------------------------------------------------
Object* MPropertyContainer::GetPropertyData(String* strPropertyName,
    unsigned int uiIndex)
{
    MVerifyValidInstance;

    const char* pcPropertyName = MStringToCharPointer(strPropertyName);
    NiFixedString kPropertyName = pcPropertyName;
    MFreeCharPointer(pcPropertyName);
    NiFixedString kPrimitiveType;
    bool bSuccess = NIBOOL_IS_TRUE(PropertyInterface->GetPrimitiveType(
        kPropertyName, kPrimitiveType));
    MAssert(bSuccess, "MEntity Error: Property name not found!");

    Object* pmData = NULL;
    if (kPrimitiveType == NiEntityPropertyInterface::PT_FLOAT)
    {
        float fData;
        bSuccess = NIBOOL_IS_TRUE(PropertyInterface->GetPropertyData(
            kPropertyName, fData, uiIndex));
        MAssert(bSuccess, "MEntity Error: Property name not found!");
        pmData = __box(fData);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_BOOL)
    {
        bool bData;
        bSuccess = NIBOOL_IS_TRUE(PropertyInterface->GetPropertyData(
            kPropertyName, bData, uiIndex));
        MAssert(bSuccess, "MEntity Error: Property name not found!");
        pmData = __box(bData);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_INT)
    {
        int iData;
        bSuccess = NIBOOL_IS_TRUE(PropertyInterface->GetPropertyData(
            kPropertyName, iData, uiIndex));
        MAssert(bSuccess, "MEntity Error: Property name not found!");
        pmData = __box(iData);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_UINT)
    {
        unsigned int uiData;
        bSuccess = NIBOOL_IS_TRUE(PropertyInterface->GetPropertyData(
            kPropertyName, uiData, uiIndex));
        MAssert(bSuccess, "MEntity Error: Property name not found!");
        pmData = __box(uiData);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_SHORT)
    {
        short sData;
        bSuccess = NIBOOL_IS_TRUE(PropertyInterface->GetPropertyData(
            kPropertyName, sData, uiIndex));
        MAssert(bSuccess, "MEntity Error: Property name not found!");
        pmData = __box(sData);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_USHORT)
    {
        unsigned short usData;
        bSuccess = NIBOOL_IS_TRUE(PropertyInterface->GetPropertyData(
            kPropertyName, usData, uiIndex));
        MAssert(bSuccess, "MEntity Error: Property name not found!");
        pmData = __box(usData);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_STRING)
    {
        NiFixedString kData;
        bSuccess = NIBOOL_IS_TRUE(PropertyInterface->GetPropertyData(
            kPropertyName, kData, uiIndex));
        MAssert(bSuccess, "MEntity Error: Property name not found!");
        pmData = new String(kData);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_POINT2)
    {
        NiPoint2 kData;
        bSuccess = NIBOOL_IS_TRUE(PropertyInterface->GetPropertyData(
            kPropertyName, kData, uiIndex));
        MAssert(bSuccess, "MEntity Error: Property name not found!");
        pmData = new MPoint2(kData);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_POINT3)
    {
        NiPoint3 kData;
        bSuccess = NIBOOL_IS_TRUE(PropertyInterface->GetPropertyData(
            kPropertyName, kData, uiIndex));
        MAssert(bSuccess, "MEntity Error: Property name not found!");
        pmData = new MPoint3(kData);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_QUATERNION)
    {
        NiQuaternion kData;
        bSuccess = NIBOOL_IS_TRUE(PropertyInterface->GetPropertyData(
            kPropertyName, kData, uiIndex));
        MAssert(bSuccess, "MEntity Error: Property name not found!");
        pmData = new MQuaternion(kData);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_MATRIX3)
    {
        NiMatrix3 kData;
        bSuccess = NIBOOL_IS_TRUE(PropertyInterface->GetPropertyData(
            kPropertyName, kData, uiIndex));
        MAssert(bSuccess, "MEntity Error: Property name not found!");
        pmData = new MMatrix3(kData);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_COLOR)
    {
        NiColor kData;
        bSuccess = NIBOOL_IS_TRUE(PropertyInterface->GetPropertyData(
            kPropertyName, kData, uiIndex));
        MAssert(bSuccess, "MEntity Error: Property name not found!");
        pmData = __box(Color::FromArgb(255, MUtility::FloatToRGB(kData.r),
            MUtility::FloatToRGB(kData.g), MUtility::FloatToRGB(kData.b)));
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_COLORA)
    {
        NiColorA kData;
        bSuccess = NIBOOL_IS_TRUE(PropertyInterface->GetPropertyData(
            kPropertyName, kData, uiIndex));
        MAssert(bSuccess, "MEntity Error: Property name not found!");
        pmData = __box(Color::FromArgb(MUtility::FloatToRGB(kData.a),
            MUtility::FloatToRGB(kData.r), MUtility::FloatToRGB(kData.g),
            MUtility::FloatToRGB(kData.b)));
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_ENTITYPOINTER)
    {
        NiEntityInterface* pkData;
        bSuccess = NIBOOL_IS_TRUE(PropertyInterface->GetPropertyData(
            kPropertyName, pkData, uiIndex));
        MAssert(bSuccess, "MEntity Error: Property name not found!");
        pmData = MEntityFactory::Instance->Get(pkData);
    }
    else
    {
        MessageService->AddMessage(MessageChannelType::Errors, new Message(
            String::Format("The \"{0}\" property on the \"{1}\" object has "
            "an unknown primitive type of \"{2}\".", strPropertyName,
            this->Name, new String(kPrimitiveType)), "The property data will "
            "not be displayed and cannot be edited.", NULL));
    }

    return pmData;
}
//---------------------------------------------------------------------------
void MPropertyContainer::SetPropertyData(String* strPropertyName, 
    Object* pmData, bool bUndoable)
{
    MVerifyValidInstance;

    SetPropertyData(strPropertyName, pmData, 0, bUndoable);
}
//---------------------------------------------------------------------------
void MPropertyContainer::SetPropertyData(String* strPropertyName,
    Object* pmData, unsigned int uiIndex, bool bUndoable)
{
    MVerifyValidInstance;

    this->Dirty = true;

    const char* pcPropertyName = MStringToCharPointer(strPropertyName);
    NiFixedString kPropertyName = pcPropertyName;
    MFreeCharPointer(pcPropertyName);
    NiFixedString kPrimitiveType;
    bool bSuccess = NIBOOL_IS_TRUE(PropertyInterface->GetPrimitiveType(
        kPropertyName, kPrimitiveType));
    MAssert(bSuccess, "MEntity Error: Property name not found!");

    if (kPrimitiveType == NiEntityPropertyInterface::PT_FLOAT)
    {
        __box float* pfData = dynamic_cast<__box float*>(pmData);
        if (pfData == NULL)
        {
            if (pmData == NULL)
            {
                pfData = __box((float)0);
            }
        }
        MAssert(pfData != NULL, "Object type does not match primitive type; "
            "cannot set value.");

        CommandService->ExecuteCommand(new MChangePropertyCommand(
            NiNew NiChangeFloatDataCommand(PropertyInterface, kPropertyName,
            *pfData, uiIndex), this), bUndoable);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_BOOL)
    {
        __box bool* pbData = dynamic_cast<__box bool*>(pmData);
        if (pbData == NULL)
        {
            if (pmData == NULL)
            {
                pbData = __box(false);
            }
        }
        MAssert(pbData != NULL, "Object type does not match primitive type; "
            "cannot set value.");

        CommandService->ExecuteCommand(new MChangePropertyCommand(
            NiNew NiChangeBoolDataCommand(PropertyInterface, kPropertyName,
            *pbData, uiIndex), this), bUndoable);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_INT)
    {
        __box int* piData = dynamic_cast<__box int*>(pmData);
        if (piData == NULL)
        {
            if (pmData == NULL)
            {
                piData = __box(0);
            }
        }
        MAssert(piData != NULL, "Object type does not match primitive type; "
            "cannot set value.");

        CommandService->ExecuteCommand(new MChangePropertyCommand(
            NiNew NiChangeIntDataCommand(PropertyInterface, kPropertyName,
            *piData, uiIndex), this), bUndoable);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_UINT)
    {
        __box unsigned int* puiData = dynamic_cast<__box unsigned int*>(
            pmData);
        if (puiData == NULL)
        {
            if (pmData == NULL)
            {
                puiData = __box((unsigned int)0);
            }
        }
        MAssert(puiData != NULL, "Object type does not match primitive type; "
            "cannot set value.");

        CommandService->ExecuteCommand(new MChangePropertyCommand(
            NiNew NiChangeUIntDataCommand(PropertyInterface, kPropertyName,
            *puiData, uiIndex), this), bUndoable);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_SHORT)
    {
        __box short* psData = dynamic_cast<__box short*>(pmData);
        if (psData == NULL)
        {
            if (pmData == NULL)
            {
                psData = __box((short)0);
            }
        }
        MAssert(psData != NULL, "Object type does not match primitive type; "
            "cannot set value.");

        CommandService->ExecuteCommand(new MChangePropertyCommand(
            NiNew NiChangeShortDataCommand(PropertyInterface, kPropertyName,
            *psData, uiIndex), this), bUndoable);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_USHORT)
    {
        __box unsigned short* pusData = dynamic_cast<__box unsigned short*>(
            pmData);
        if (pusData == NULL)
        {
            if (pmData == NULL)
            {
                pusData = __box((unsigned short)0);
            }
        }
        MAssert(pusData != NULL, "Object type does not match primitive type; "
            "cannot set value.");

        CommandService->ExecuteCommand(new MChangePropertyCommand(
            NiNew NiChangeUShortDataCommand(PropertyInterface, kPropertyName,
            *pusData, uiIndex), this), bUndoable);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_STRING)
    {
        String* strData = dynamic_cast<String*>(pmData);
        if (strData == NULL)
        {
            if (pmData == NULL)
            {
                strData = String::Empty;
            }
        }
        MAssert(strData != NULL, "Object type does not match primitive type; "
            "cannot set value.");
        const char* pcData = MStringToCharPointer(strData);
        NiFixedString kData = pcData;
        MFreeCharPointer(pcData);

        CommandService->ExecuteCommand(new MChangePropertyCommand(
            NiNew NiChangeStringDataCommand(PropertyInterface, kPropertyName,
            kData, uiIndex), this), bUndoable);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_POINT2)
    {
        MPoint2* pmPoint2 = dynamic_cast<MPoint2*>(pmData);
        if (pmPoint2 == NULL)
        {
            if (pmData == NULL)
            {
                pmPoint2 = new MPoint2();
            }
        }
        MAssert(pmPoint2 != NULL, "Object type does not match primitive "
            "type; cannot set value.");
        NiPoint2 kData;
        pmPoint2->ToNiPoint2(kData);

        CommandService->ExecuteCommand(new MChangePropertyCommand(
            NiNew NiChangePoint2DataCommand(PropertyInterface, kPropertyName,
            kData, uiIndex), this), bUndoable);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_POINT3)
    {
        MPoint3* pmPoint3 = dynamic_cast<MPoint3*>(pmData);
        if (pmPoint3 == NULL)
        {
            if (pmData == NULL)
            {
                pmPoint3 = new MPoint3();
            }
        }
        MAssert(pmPoint3 != NULL, "Object type does not match primitive "
            "type; cannot set value.");
        NiPoint3 kData;
        pmPoint3->ToNiPoint3(kData);

        CommandService->ExecuteCommand(new MChangePropertyCommand(
            NiNew NiChangePoint3DataCommand(PropertyInterface, kPropertyName,
            kData, uiIndex), this), bUndoable);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_QUATERNION)
    {
        MQuaternion* pmQuaternion = dynamic_cast<MQuaternion*>(pmData);
        if (pmQuaternion == NULL)
        {
            if (pmData == NULL)
            {
                pmQuaternion = new MQuaternion();
            }
        }
        MAssert(pmQuaternion != NULL, "Object type does not match primitive "
            "type; cannot set value.");
        NiQuaternion kData;
        pmQuaternion->ToNiQuaternion(kData);

        CommandService->ExecuteCommand(new MChangePropertyCommand(
            NiNew NiChangeQuaternionDataCommand(PropertyInterface,
            kPropertyName, kData, uiIndex), this), bUndoable);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_MATRIX3)
    {
        MMatrix3* pmMatrix3 = dynamic_cast<MMatrix3*>(pmData);
        if (pmMatrix3 == NULL)
        {
            if (pmData == NULL)
            {
                pmMatrix3 = new MMatrix3();
            }
        }
        MAssert(pmMatrix3 != NULL, "Object type does not match primitive "
            "type; cannot set value.");
        NiMatrix3 kData;
        pmMatrix3->ToNiMatrix3(kData);

        CommandService->ExecuteCommand(new MChangePropertyCommand(
            NiNew NiChangeMatrix3DataCommand(PropertyInterface, kPropertyName,
            kData, uiIndex), this), bUndoable);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_COLOR)
    {
        __box Color* pmColor = dynamic_cast<__box Color*>(pmData);
        if (pmColor == NULL)
        {
            if (pmData == NULL)
            {
                Color color;
                pmColor = __box(color);
            }
        }
        MAssert(pmColor != NULL, "Object type does not match primitive "
            "type; cannot set value.");
        Color mColor = *pmColor;
        NiColor kData(MUtility::RGBToFloat(mColor.R),
            MUtility::RGBToFloat(mColor.G), MUtility::RGBToFloat(mColor.B));

        CommandService->ExecuteCommand(new MChangePropertyCommand(
            NiNew NiChangeColorDataCommand(PropertyInterface, kPropertyName,
            kData, uiIndex), this), bUndoable);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_COLORA)
    {
        __box Color* pmColor = dynamic_cast<__box Color*>(pmData);
        if (pmColor == NULL)
        {
            if (pmData == NULL)
            {
                Color color;
                pmColor = __box(color);
            }
        }
        MAssert(pmColor != NULL, "Object type does not match primitive "
            "type; cannot set value.");
        Color mColor = *pmColor;
        NiColorA kData(MUtility::RGBToFloat(mColor.R),
            MUtility::RGBToFloat(mColor.G), MUtility::RGBToFloat(mColor.B),
            MUtility::RGBToFloat(mColor.A));

        CommandService->ExecuteCommand(new MChangePropertyCommand(
            NiNew NiChangeColorADataCommand(PropertyInterface, kPropertyName,
            kData, uiIndex), this), bUndoable);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_ENTITYPOINTER)
    {
        MAssert(pmData == NULL || pmData->GetType() == __typeof(MEntity),
            "Object type does not match primitive type; cannot set value.");
        MEntity* pmEntity = dynamic_cast<MEntity*>(pmData);
        NiEntityInterface* pkData = NULL;
        if (pmEntity != NULL)
        {
            pkData = pmEntity->GetNiEntityInterface();
        }

        CommandService->ExecuteCommand(new MChangePropertyCommand(
            NiNew NiChangeEntityPointerDataCommand(PropertyInterface,
            kPropertyName, pkData, uiIndex), this), bUndoable);
    }
    else
    {
        MAssert(false, "Unknown property type; cannot set value.");
    }
}
//---------------------------------------------------------------------------
bool MPropertyContainer::CanResetProperty(String* strPropertyName)
{
    MVerifyValidInstance;

    bool bCanReset;
    const char* pcPropertyName = MStringToCharPointer(strPropertyName);
    bool bSuccess = NIBOOL_IS_TRUE(PropertyInterface->CanResetProperty(
        pcPropertyName, bCanReset));
    MFreeCharPointer(pcPropertyName);
    MAssert(bSuccess, "MEntity Error: Property name not found!");

    return bCanReset;
}
//---------------------------------------------------------------------------
void MPropertyContainer::ResetProperty(String* strPropertyName)
{
    MVerifyValidInstance;

    this->Dirty = true;

    const char* pcPropertyName = MStringToCharPointer(strPropertyName);
    NiChangePropertyCommand* pkResetPropertyDataCommand =
        NiResetPropertyDataCommandFactory::GetResetDataCommand(
        PropertyInterface, pcPropertyName);
    MFreeCharPointer(pcPropertyName);

    MAssert(pkResetPropertyDataCommand != NULL, "Reset command could not be "
        "created!");

    CommandService->BeginUndoFrame(String::Format("Reset data for \"{0}\" "
        "property on \"{1}\" entity/component", strPropertyName, this->Name));

    CommandService->ExecuteCommand(new MChangePropertyCommand(
        pkResetPropertyDataCommand, this), true);

    CommandService->EndUndoFrame(true);
}
//---------------------------------------------------------------------------
void MPropertyContainer::MakePropertyUnique(String* strPropertyName)
{
    MVerifyValidInstance;

    this->Dirty = true;

    const char* pcPropertyName = MStringToCharPointer(strPropertyName);
    CommandService->ExecuteCommand(new MMakePropertyUniqueCommand(
        NiNew NiMakePropertyUniqueCommand(PropertyInterface, pcPropertyName),
        this), true);
    MFreeCharPointer(pcPropertyName);
}
//---------------------------------------------------------------------------
bool MPropertyContainer::get_Writable()
{
    MVerifyValidInstance;

    if (m_pmLayer != NULL)
    {
        return m_pmLayer->Writable;
    }

    return m_bWritable;
}
//---------------------------------------------------------------------------
void MPropertyContainer::set_Writable(bool bWritable)
{
    MVerifyValidInstance;

    m_bWritable = bWritable;
}
//---------------------------------------------------------------------------
Guid MPropertyContainer::get_ID()
{
    MVerifyValidInstance;

    return MUtility::IDToGuid(this->PropertyInterface->GetID());
}
//---------------------------------------------------------------------------
String* MPropertyContainer::get_SourceFilename()
{
    MVerifyValidInstance;

    return this->PropertyInterface->GetSourceFilename();
}
//---------------------------------------------------------------------------
MLayer* MPropertyContainer::get_Layer()
{
    return m_pmLayer;
}
//---------------------------------------------------------------------------
void MPropertyContainer::set_Layer(MLayer* pmLayer)
{
    m_pmLayer = pmLayer;
}
//---------------------------------------------------------------------------
IPropertyTypeService* MPropertyContainer::get_PropertyTypeService()
{
    if (ms_pmPropertyTypeService == NULL)
    {
        ms_pmPropertyTypeService = MGetService(IPropertyTypeService);
        MAssert(ms_pmPropertyTypeService != NULL, "Property type service "
            "not found!");
    }
    return ms_pmPropertyTypeService;
}
//---------------------------------------------------------------------------
ICommandService* MPropertyContainer::get_CommandService()
{
    if (ms_pmCommandService == NULL)
    {
        ms_pmCommandService = MGetService(ICommandService);
        MAssert(ms_pmCommandService != NULL, "Command service not found!");
    }
    return ms_pmCommandService;
}
//---------------------------------------------------------------------------
IMessageService* MPropertyContainer::get_MessageService()
{
    if (ms_pmMessageService == NULL)
    {
        ms_pmMessageService = MGetService(IMessageService);
        MAssert(ms_pmMessageService != NULL, "Message service not found!");
    }
    return ms_pmMessageService;
}
//---------------------------------------------------------------------------
// ICustomTypeDescriptor overrides
//---------------------------------------------------------------------------
AttributeCollection* MPropertyContainer::GetAttributes()
{
    MVerifyValidInstance;

    return TypeDescriptor::GetAttributes(this, true);
}
//---------------------------------------------------------------------------
String* MPropertyContainer::GetClassName()
{
    MVerifyValidInstance;

    return TypeDescriptor::GetClassName(this, true);
}
//---------------------------------------------------------------------------
String* MPropertyContainer::GetComponentName()
{
    MVerifyValidInstance;

    return TypeDescriptor::GetComponentName(this, true);
}
//---------------------------------------------------------------------------
TypeConverter* MPropertyContainer::GetConverter()
{
    MVerifyValidInstance;

    return TypeDescriptor::GetConverter(this, true);
}
//---------------------------------------------------------------------------
EventDescriptor* MPropertyContainer::GetDefaultEvent()
{
    MVerifyValidInstance;

    return TypeDescriptor::GetDefaultEvent(this, true);
}
//---------------------------------------------------------------------------
PropertyDescriptor* MPropertyContainer::GetDefaultProperty()
{
    MVerifyValidInstance;

    return NULL;
}
//---------------------------------------------------------------------------
Object* MPropertyContainer::GetEditor(Type* pmEditorBaseType)
{
    MVerifyValidInstance;

    return TypeDescriptor::GetEditor(this, pmEditorBaseType, true);
}
//---------------------------------------------------------------------------
EventDescriptorCollection* MPropertyContainer::GetEvents()
{
    MVerifyValidInstance;

    return TypeDescriptor::GetEvents(this, true);
}
//---------------------------------------------------------------------------
EventDescriptorCollection* MPropertyContainer::GetEvents(
    Attribute* amAttributes[])
{
    MVerifyValidInstance;

    return TypeDescriptor::GetEvents(this, amAttributes, true);
}
//---------------------------------------------------------------------------
PropertyDescriptorCollection* MPropertyContainer::GetProperties()
{
    MVerifyValidInstance;

    return GetProperties(new Attribute*[0]);
}
//---------------------------------------------------------------------------
PropertyDescriptorCollection* MPropertyContainer::GetProperties(
    Attribute*[])
{
    MVerifyValidInstance;

    NiEntityPropertyInterface* pkPropertyInterface = 
        PropertyInterface;

    ArrayList* pmPropertyDescs = new ArrayList();

    NiTObjectSet<NiFixedString> kPropertyNames;
    pkPropertyInterface->GetPropertyNames(kPropertyNames);
    for (unsigned int ui = 0; ui < kPropertyNames.GetSize(); ui++)
    {
        const NiFixedString& kPropertyName = kPropertyNames.GetAt(ui);

        NiFixedString kDisplayName;
        bool bSuccess = NIBOOL_IS_TRUE(pkPropertyInterface->GetDisplayName(
            kPropertyName, kDisplayName));
        MAssert(bSuccess, "PropertyInterface Error: Property name not "
            "found!");
        if (!kDisplayName.Exists())
        {
            continue;
        }

        ArrayList* pmPropAttributes = new ArrayList();
        NiDefaultErrorHandlerPtr spErrors;

        NiFixedString kCategory;
        bSuccess = NIBOOL_IS_TRUE(
            pkPropertyInterface->GetCategory(kPropertyName, kCategory));
        if (bSuccess && kCategory.Exists())
        {
            pmPropAttributes->Add(new CategoryAttribute(kCategory));
        }
        else
        {
            if (!spErrors)
            {
                spErrors = NiNew NiDefaultErrorHandler(3);
            }
            spErrors->ReportError("No category specified for property.",
                "The property will be added to a general category.",
                pkPropertyInterface->GetName(), kPropertyName);
        }

        NiFixedString kDescription;
        bSuccess = NIBOOL_IS_TRUE(
            pkPropertyInterface->GetDescription(kPropertyName, kDescription));
        if (bSuccess && kDescription.Exists())
        {
            pmPropAttributes->Add(new DescriptionAttribute(kDescription));
        }
        else
        {
            if (!spErrors)
            {
                spErrors = NiNew NiDefaultErrorHandler(2);
            }
            spErrors->ReportError("No description specified for property.",
                "No description will be displayed for this property.",
                pkPropertyInterface->GetName(), kPropertyName);
        }

        NiFixedString kSemanticType;
        bSuccess = NIBOOL_IS_TRUE(pkPropertyInterface->GetSemanticType(
            kPropertyName, kSemanticType));
        PropertyType* pmPropertyType = NULL;
        if (bSuccess && kSemanticType.Exists())
        {
            pmPropertyType = PropertyTypeService->LookupType(kSemanticType);
            if (pmPropertyType == NULL)
            {
                if (!spErrors)
                {
                    spErrors = NiNew NiDefaultErrorHandler(1);
                }
                const char* pcErrorDescription = MStringToCharPointer(
                    String::Format("The \"{0}\" semantic type could not be "
                    "found; the property will not be displayed.",
                    new String(kSemanticType)));
                spErrors->ReportError("Semantic type not found for property.",
                    pcErrorDescription, pkPropertyInterface->GetName(),
                    kPropertyName);
                MFreeCharPointer(pcErrorDescription);
            }
        }
        else
        {
            if (!spErrors)
            {
                spErrors = NiNew NiDefaultErrorHandler(1);
            }
            spErrors->ReportError("No semantic type specified for property.",
                "The property will not be displayed.", 
                pkPropertyInterface->GetName(), kPropertyName);
        }

        if (spErrors)
        {
            MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors,
                spErrors);
        }

        if (pmPropertyType != NULL)
        {
            bool bIsCollection;
            bSuccess = NIBOOL_IS_TRUE(
                pkPropertyInterface->IsCollection(kPropertyName, 
                bIsCollection));
            MAssert(bSuccess, "MEntity Error: Property name not found!");

            unsigned int uiIndex;
            Type* pmEditor;
            Type* pmTypeConverter;
            if (bIsCollection)
            {
                uiIndex = 
                    MEntityPropertyDescriptor::INVALID_PROPERTY_INDEX;
                pmEditor = pmPropertyType->CollectionEditor;
                pmTypeConverter = __typeof(MExpandablePropertyConverter);
            }
            else
            {
                uiIndex = 0;
                pmEditor = pmPropertyType->Editor;
                pmTypeConverter = pmPropertyType->TypeConverter;
            }

            pmPropertyDescs->Add(new MEntityPropertyDescriptor(this,
                kPropertyName, uiIndex, pmEditor, pmTypeConverter,
                kDisplayName, dynamic_cast<Attribute*[]>(
                pmPropAttributes->ToArray(__typeof(Attribute)))));
        }
    }

    PropertyDescriptor* amPropertyDescArray[] =
        dynamic_cast<PropertyDescriptor*[]>(pmPropertyDescs->ToArray(
        __typeof(PropertyDescriptor)));
    return new PropertyDescriptorCollection(amPropertyDescArray);
}
//---------------------------------------------------------------------------
Object* MPropertyContainer::GetPropertyOwner(
    PropertyDescriptor*)
{
    MVerifyValidInstance;

    return this;
}
//---------------------------------------------------------------------------
