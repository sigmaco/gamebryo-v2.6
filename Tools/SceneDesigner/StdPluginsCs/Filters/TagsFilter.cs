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
    class TagsFilter : IFilter
    {
        private List<string> m_tags = new List<string>();
        public List<string> Tags
        {
            get
            {
                return m_tags;
            }
        }

        public MEntity[] FilterEntities(MEntity[] inputEntities)
        {
            if (Tags.Count == 0)
            {
                return inputEntities;
            }

            List<MEntity> outputEntities = new List<MEntity>(
                inputEntities.Length);

            foreach (MEntity entity in inputEntities)
            {
                foreach (string tag in Tags)
                {
                    MEntity master = entity;
                    bool bFoundTag = false;
                    while (master != null)
                    {
                        if (master.ContainsTag(tag))
                        {
                            outputEntities.Add(entity);
                            bFoundTag = true;
                            break;
                        }
                        master = master.MasterEntity;
                    }
                    if (bFoundTag)
                    {
                        break;
                    }
                }
            }

            return outputEntities.ToArray();
        }
    }
}
