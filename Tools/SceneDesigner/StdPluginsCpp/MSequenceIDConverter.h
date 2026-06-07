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

#pragma once

using namespace System::ComponentModel;
using namespace System::Globalization;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace StdPluginsCpp
{
    public __gc class MSequenceIDConverter : public TypeConverter
    {
    public:
        virtual bool GetStandardValuesSupported(
            ITypeDescriptorContext* pmContext);
        virtual bool GetStandardValuesExclusive(
            ITypeDescriptorContext* pmContext);
        virtual StandardValuesCollection* GetStandardValues(
            ITypeDescriptorContext* pmContext);

        virtual bool CanConvertTo(ITypeDescriptorContext* pmContext,
            Type* pmDestinationType);
        virtual Object* ConvertTo(ITypeDescriptorContext* pmContext,
            CultureInfo* pmCulture, Object* pmValue, Type* pmDestinationType);
        virtual bool CanConvertFrom(ITypeDescriptorContext* pmContext,
            Type* pmType);
        virtual Object* ConvertFrom(ITypeDescriptorContext* pmContext,
            CultureInfo* pmInfo, Object* pmValue);

    private:
        static String* ms_strSequenceIDsName = "Sequence IDs";
        static String* ms_strSequenceNamesName = "Sequence Names";
    };
}}}}
