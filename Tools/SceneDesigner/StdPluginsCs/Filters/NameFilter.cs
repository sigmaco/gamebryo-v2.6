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
using System.Collections.Generic;
using System.Text;
using Emergent.Gamebryo.SceneDesigner.Framework;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Filters
{
    class NameFilter : IFilter
    {
        public enum Type
        {
            Entity,
            Template,
            Component
        }

        private Type m_filterType = Type.Entity;
        public Type FilterType
        {
            get
            {
                return m_filterType;
            }
            set
            {
                m_filterType = value;
            }
        }

        private string m_matchString;
        public string MatchString
        {
            get
            {
                return m_matchString;
            }
            set
            {
                m_matchString = value;
            }
        }

        public MEntity[] FilterEntities(MEntity[] inputEntities)
        {
            if (string.IsNullOrEmpty(MatchString))
            {
                return inputEntities;
            }

            List<MEntity> outputEntities = new List<MEntity>(
                inputEntities.Length);

            foreach (MEntity entity in inputEntities)
            {
                switch (FilterType)
                {
                    case Type.Entity:
                        if (entity.Name.ToUpper().Contains(
                            MatchString.ToUpper()))
                        {
                            outputEntities.Add(entity);
                        }
                        break;
                    case Type.Template:
                        MEntity master = entity.MasterEntity;
                        while (master != null)
                        {
                            if (master.Name.ToUpper().Contains(
                                MatchString.ToUpper()))
                            {
                                outputEntities.Add(entity);
                                break;
                            }
                            master = master.MasterEntity;
                        }
                        break;
                    case Type.Component:
                        MComponent[] components = entity.GetComponents();
                        foreach (MComponent component in components)
                        {
                            if (component.Name.ToUpper().Contains(
                                MatchString.ToUpper()))
                            {
                                outputEntities.Add(entity);
                                break;
                            }
                        }
                        break;
                }
            }

            return outputEntities.ToArray();
        }
    }
}
