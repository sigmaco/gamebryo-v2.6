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
using namespace System::Windows::Forms::Design;
using namespace System::IO;
using namespace System::Drawing::Design;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MFileLocationConverter : public TypeConverter
    {
    public:
        virtual bool CanConvertFrom(ITypeDescriptorContext* pmContext, 
            Type* pmSourceType);
        virtual bool CanConvertTo(ITypeDescriptorContext* pmContext, 
            Type* pmDestinationType);

        virtual Object* ConvertFrom(ITypeDescriptorContext* pmContext,
            CultureInfo* pmCulture, Object* pmValue);
        virtual Object* ConvertTo(ITypeDescriptorContext* pmContext, 
            CultureInfo* pmCulture, Object* pmValue, Type* pmDestinationType);
    };

    public __gc class MFileLocationNameEditor : public FileNameEditor
    {
    public:
        virtual Object* EditValue(ITypeDescriptorContext* pmContext,
            System::IServiceProvider* pmProvider, Object* pmValue);
    };

    [Serializable]
    [Editor(__typeof(MFileLocationNameEditor), __typeof(UITypeEditor))]
    [TypeConverter(__typeof(MFileLocationConverter))]
    public __gc class MFileLocation
    {
    public:
        MFileLocation(String* strPath);
        MFileLocation(String* strPath, String* strDefaultBrowsePath);

        __property String* get_Path();
        __property void set_Path(String* strPath);

        __property String* get_DefaultBrowsePath();
        __property void set_DefaultBrowsePath(String* strDefaultBrowsePath);

        virtual String* ToString();

    private:
        String* m_strPath;
        String* m_strDefaultBrowsePath;

        static String* ms_strSeparatorString =
            Path::DirectorySeparatorChar.ToString();
        static String* ms_strAltSeparatorString = 
            Path::AltDirectorySeparatorChar.ToString();
    };
}}}}
