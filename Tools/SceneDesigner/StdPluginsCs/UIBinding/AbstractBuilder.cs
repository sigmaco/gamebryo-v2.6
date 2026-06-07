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

using System;
using System.IO;
using System.Xml;
using System.Xml.Schema;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.UIBinding
{
    /// <summary>
    /// Summary description for IBuilder.
    /// </summary>
    internal abstract class AbstractBuilder
    {
        #region PrivateData
        protected XmlSchema m_schema;
        #endregion

        public XmlSchema Schema
        {
            get
            { return m_schema; }
            set
            { m_schema = value; }
        }
        public abstract void Build(Stream xmlStream);

        protected XmlDocument ReadXML(Stream stream)
        {
            // Set the validation settings if we have a schema.
            XmlDocument doc = new XmlDocument();
            if (m_schema != null)
            {
                XmlReaderSettings settings = new XmlReaderSettings();
                settings.ValidationType = ValidationType.Schema;
                settings.ValidationFlags |=
                    XmlSchemaValidationFlags.ProcessInlineSchema;
                settings.ValidationFlags |=
                    XmlSchemaValidationFlags.ReportValidationWarnings;
                settings.ValidationEventHandler +=
                    new ValidationEventHandler(ValidationCallBack);
                settings.Schemas.Add(m_schema);
                XmlReader reader = XmlReader.Create(stream, settings);
                doc.Load(reader);
            }
            else
            {
                XmlReader reader = XmlReader.Create(stream);
                doc.Load(reader);
            }

            return doc;
        }

        // Display any warnings or errors.
        private static void ValidationCallBack(object sender,
            ValidationEventArgs args)
        {
            if (args.Severity == XmlSeverityType.Warning)
                Console.WriteLine(
                    "\tWarning: Matching schema not found. " +
                    "No validation occurred." + args.Message);
            else
                Console.WriteLine("\tValidation error: " + args.Message);
        }
    }


}
