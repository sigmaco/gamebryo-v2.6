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
using System.Diagnostics;
using System.Collections;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Panels;
using Message = Emergent.Gamebryo.SceneDesigner.PluginAPI.Message;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Utility
{
    /// <summary>
    /// Summary description for PaletteUtilities.
    /// </summary>
    public class PaletteUtilities
    {
        private PaletteUtilities()//utlity/static class no constructor
        {
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="palette"></param>
        /// <param name="category"></param>
        /// <returns>An array of the full paths of all immediate decendents
        /// of the passed in path</returns>
        internal static string[] BuildSubcategories(MPalette palette, string category)
        {
            //contains the category entries for all entities in palette
            ArrayList masterCategoryList = new ArrayList();
            MEntity[] entities = palette.GetEntities();
            foreach (MEntity entity in entities)
            {
                string entityCategory = palette.GetEntityCategory(entity);
                if (//entityCategory != null && 
                    !entityCategory.Equals(string.Empty))
                {
                    masterCategoryList.Add(entityCategory);
                }
            }

            ArrayList categoryList = new ArrayList();

            foreach (string entityCategory in masterCategoryList)
            {
                string[] simpleCategories = entityCategory.Split('.');
                StringBuilder builder = new StringBuilder();
                for (int i = 0; i < simpleCategories.Length; i++)
                {
                    builder.Append(simpleCategories[i]);
                    string localCategory = builder.ToString();
                    string parentCategory = ParentCategory(localCategory);
                    if (parentCategory.Equals(category))
                    {
                        if (!categoryList.Contains(localCategory))
                        {
                            categoryList.Add(localCategory);
                        }
                        break; //no need to continue searching
                    }
                    builder.Append("/");
                }
            }
            return categoryList.ToArray(typeof(string)) as string[];
        }

        internal static string ParentCategory(string fullName)
        {
            string fullNameWithoutPalette = MPalette.StripPaletteName(fullName);
            string simpleCategory = SimpleName(fullName);
            if (fullNameWithoutPalette.Equals(simpleCategory))
            {
                return string.Empty;
            }
            return fullNameWithoutPalette.Substring(0, 
                fullNameWithoutPalette.LastIndexOf(simpleCategory)-1);
        }

        internal static string SimpleName(string fullName)
        {
            string namewithoutPalette = MPalette.StripPaletteName(fullName);
            Regex regex = new Regex(@"[.]");
            string[] categories = regex.Split(namewithoutPalette);
            return categories[categories.Length - 1];
        }


        internal static string FullCategoryName(string[] categories)
        {
            if (categories.Length > 0)
            {
                StringBuilder sb = new StringBuilder();
                sb.Append(categories[0]);
                for(int i = 1; i < categories.Length; i++)
                {
                    sb.AppendFormat("/{0}", categories[i]);
                }
                return sb.ToString();
            }
            return string.Empty;                     
        }

        internal static void ImportPalettes(string[] filenames)
        {

            MFramework fw = MFramework.Instance;
            string destinationFolderPath = fw.PaletteManager.PaletteFolder;
            foreach(string filename in filenames)
            {
                FileInfo fileInfo = new FileInfo(filename);
                string paletteName = //GetUniqueName(
                    fileInfo.Name.Replace(".pal", "");

                string newPath = destinationFolderPath + paletteName + ".pal";
                //fileInfo.CopyTo(newPath);
                MPalette destinationPalette = 
                    fw.PaletteManager.GetPaletteByName(paletteName);
                if (destinationPalette == null)
                {
                    destinationPalette = new MPalette(paletteName, 0);                    
                    fw.PaletteManager.AddPalette(destinationPalette);
                }

                MPalette tempPalette = fw.PaletteManager.LoadPalette(filename);                
                
                if (CheckForDuplicateTemplates(tempPalette, 
                    fw.PaletteManager.GetPalettes()))
                {
                    tempPalette.Name = paletteName;
                    tempPalette.ResetEntityNames();
                    FileInfo newFileInfo = new FileInfo(newPath);

                    if (newFileInfo.Exists)
                    {
                        newFileInfo.Attributes = newFileInfo.Attributes & 
                            (~FileAttributes.ReadOnly);

                        newFileInfo.Delete();
                    }

                    MEntity[] newTemplates = tempPalette.GetEntities();

                    foreach(MEntity template in newTemplates)
                    {
                        destinationPalette.AddEntity(template, string.Empty, 
                            true);
                    }

                    IUICommandService uiCommandService = 
                        ServiceProvider.Instance.GetService(
                        typeof(IUICommandService)) as IUICommandService;
                    UICommand reCheckScene = 
                        uiCommandService.GetCommand(
                        "ReCheckSceneAfterPaletteImport");
                    //reCheckScene.Data = palette;
                    reCheckScene.DoClick(reCheckScene, null);

                    fw.PaletteManager.SavePalette(destinationPalette, newPath,
                        "GSA");
                }

            }
        }


        private static string GetUniqueName(string paletteName)
        {
            MFramework fw = MFramework.Instance;
            if (fw.PaletteManager.GetPaletteByName(paletteName) == null)
            {
                return paletteName;
            }
            MPalette[] existingPalettes = fw.PaletteManager.GetPalettes();
            ArrayList existingPaletteNames = new ArrayList();
            foreach (MPalette palette in existingPalettes)
            {
                existingPaletteNames.Add(palette.Name.ToUpper());
            }
            int index = 1;
            while (true)
            {
                string possibleName = string.Format("{0}_{1:00#}",
                    paletteName, index);
                if (!existingPaletteNames.Contains(possibleName.ToUpper()))
                {
                    return possibleName;
                }
                index++;
            }
        }

        private class RemovalRecord
        {
            public MPalette palette;
            public MEntity entity;
        }

        internal static bool CheckForDuplicateTemplates(MPalette palette, 
            MPalette[] existingPalettes)
        {
            TemplateConflictDlg dlg = new TemplateConflictDlg();

            if (palette == null)
                return false;

            ServiceProvider sp = ServiceProvider.Instance;
            IEntityPathService pathService = sp.GetService(
                typeof(IEntityPathService)) as IEntityPathService;
            ArrayList removalList = new ArrayList();
            ArrayList additionList = new ArrayList();
            MEntity[] paletteEntities = palette.Scene.GetEntities();
            foreach (MEntity paletteEntity in paletteEntities)
            {
                foreach (MPalette existingPalette in existingPalettes)
                {
                    MEntity[] existingEntities = 
                        existingPalette.Scene.GetEntities();
                    foreach(MEntity existingEntity in existingEntities)
                    {
                        //Then a conflict was found
                        if (paletteEntity.TemplateID == 
                            existingEntity.TemplateID)
                        {
                            if (existingPalette.DontSave)
                            {//Then the conflict is with a temporary palette
                                //pulll the existing entity from the 
                                //temporary palette and place it in the 
                                //imported one
                                RemovalRecord existingrecord = 
                                    new RemovalRecord();
                                existingrecord.palette = existingPalette;
                                existingrecord.entity = existingEntity;
                                removalList.Add(existingrecord);
                                RemovalRecord record = new RemovalRecord();
                                record.palette = palette;
                                record.entity = paletteEntity;
                                removalList.Add(record);
                                additionList.Add(existingEntity);
                                continue;
                            }
                            string msg = string.Format("Template '{0}'" +
                                " conflicts with existing template '{1}'",
                                pathService.GetFullPath(
                                palette.Scene, paletteEntity),
                                pathService.GetFullPath(
                                existingPalette.Scene, existingEntity));

                            dlg.Message = msg;
                            if (!dlg.ApplyToAll)
                            {
                                dlg.ShowDialog();
                            }
                            switch (dlg.Action)
                            {
                                case TemplateConflictDlg.ConflictAction.Skip: 
                                {
                                    RemovalRecord record = new RemovalRecord();
                                    record.palette = palette;
                                    record.entity = paletteEntity;
                                    removalList.Add(record);
                                    break;
                                }
                                case TemplateConflictDlg.ConflictAction.Clone: 
                                {
                                    RemovalRecord record = new RemovalRecord();
                                    record.palette = palette;
                                    record.entity = paletteEntity;
                                    removalList.Add(record);
                                    // Template entities cannot be prefabs so
                                    // we only get one entity when we clone it.
                                    MEntity[] amClones = paletteEntity.Clone(
                                        paletteEntity.Name, false);
                                    Debug.Assert(amClones.Length == 1);
                                    MEntity clone = amClones[0];
                                    clone.TemplateID = Guid.NewGuid();
                                    additionList.Add(clone);
                                    break;
                                }
                                case TemplateConflictDlg.ConflictAction.Replace:
                                {
                                    RemovalRecord record = new RemovalRecord();
                                    record.palette = existingPalette;
                                    record.entity = existingEntity;
                                    removalList.Add(record);
                                    additionList.Add(paletteEntity);
                                    break;
                                }
                                case TemplateConflictDlg.ConflictAction.Cancel: 
                                {
                                    return false;
                                }
                            }
                        }
                    }
                }
            }
            
            foreach(RemovalRecord record in removalList)
            {
                record.palette.Scene.RemoveEntity(record.entity, false);
            }
            foreach(MEntity entityToAdd in additionList)
            {
                palette.Scene.AddEntity(entityToAdd, false);
            }
            return true;
        }


    }
}
