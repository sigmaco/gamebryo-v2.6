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
    class LayersFilter : IFilter
    {
        private List<MLayer> m_layers = new List<MLayer>();
        public List<MLayer> Layers
        {
            get
            {
                return m_layers;
            }
        }

        public MEntity[] FilterEntities(MEntity[] inputEntities)
        {
            if (Layers.Count == 0)
            {
                return inputEntities;
            }

            List<MEntity> outputEntities = new List<MEntity>(
                inputEntities.Length);

            foreach (MEntity entity in inputEntities)
            {
                foreach (MLayer layer in Layers)
                {
                    if (entity.Layer == layer)
                    {
                        outputEntities.Add(entity);
                        break;
                    }
                }
            }

            return outputEntities.ToArray();
        }
    }
}
