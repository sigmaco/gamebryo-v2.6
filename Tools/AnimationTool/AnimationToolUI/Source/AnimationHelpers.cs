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
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.IO;
using System.Runtime.InteropServices;
using System.Reflection;
using System.Diagnostics;
using NiManagedToolInterface;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for AnimationHelpers.
    /// </summary>
    public class AnimationHelpers
    {
        #region KFM Helpers

        /// <summary>
        /// UserSaveOrCancel returns false if user cancels
        /// or true if otherwise
        /// </summary>
        static public bool UserSaveOrCancel()
        {
            if (MFramework.Instance.Animation.CanSave && 
                (MFramework.Instance.Animation.NeedToSave ||
                ProjectData.Instance.NeedToSave ||
                MFramework.Instance.Physics.NeedToSave))
            {
                
                string[] strVariables = new string[1];
                bool bSaveAs = true;
                if (MFramework.Instance.Animation.KFMFilename == null)
                {
                    strVariables[0] = "Untitled";
                }
                else
                {
                    strVariables[0] = MessageBoxManager.ConvertFilename(
                        AppUtils.ExtractFileFromPath(MFramework.Instance
                        .Animation.KFMFilename));
                }

                DialogResult kRes = MessageBoxManager.DoMessageBox(
                    "FileHasChanged.rtf", "Save Changed File",
                    MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question,
                    strVariables);

                if (kRes == DialogResult.Cancel)
                {
                    return false;
                }

                if (kRes == DialogResult.Yes)
                {
                    if (SaveKFMDlg(null, bSaveAs) == false) 
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        static public void NewKFMFile()
        {
            if (UserSaveOrCancel() == false)
                return;

            MFramework.Instance.Animation.NewKFM();

            MFramework.Instance.Input.SubmitCommand(
                MUICommand.CommandType.ZOOM_EXTENTS);

            MFramework.Instance.Renderer.CreateFrame();
        }

        static public void OpenKFMFile(string strFilename)
        {
            // Ensure that the file path is a long path instead of a short
            // 8.3 path.
            string strLongPath = strFilename;
            strFilename = AppUtils.ToLongPathName(strFilename);

            if (strFilename == "")
            {
                string [] strVariables = new string[2];
                strVariables[0] = MessageBoxManager.ConvertFilename(
                    strLongPath);
                strVariables[1] =  "Could not locate File.";

                MessageBoxManager.DoMessageBox("LoadKFMError.rtf",
                    "Error Loading KFM File", MessageBoxButtons.OK,
                    MessageBoxIcon.Error, strVariables);

                MRUManager.RemoveFile(strLongPath);
                return;
            }

            if (UserSaveOrCancel() == false)
                return;

            MFramework.Instance.Animation.NewKFM();

            if (!MFramework.Instance.Animation.LoadKFM(strFilename))
            {               
                string [] strVariables = new string[2];
                strVariables[0] = MessageBoxManager.ConvertFilename(
                    strFilename);
                strVariables[1] = MessageBoxManager.ConvertFilename(
                    MFramework.Instance.Animation.GetLastErrorString());

                MessageBoxManager.DoMessageBox("LoadKFMError.rtf",
                    "Error Loading KFM File", MessageBoxButtons.OK,
                    MessageBoxIcon.Error, strVariables);

                MRUManager.RemoveFile(strLongPath);

                return;
            }

            string strOriginalName = strFilename;

            int iDotIdx = strFilename.LastIndexOf('.');
            if (iDotIdx != -1)
            {
                strFilename = strFilename.Substring(0, iDotIdx);
                string strSettingsFilename = strFilename + ".settings";
                if (!ProjectData.Load(strSettingsFilename))
                {
                    string strLogMsg = "WARNING: Unable to load file: \"" + 
                        strSettingsFilename + "\"";
                    MLogger.LogWarning(strLogMsg);
                
                    // Try loading the old AnimationTool v1.0 files
                    string strSetFilename = strFilename + ".set";
                    string strMapFilename = strFilename + ".map";
                    if (!ProjectData.LoadOldSetFile(strSetFilename,
                        strMapFilename))
                    {
                        strLogMsg = "WARNING: Unable to load legacy " +
                            "AnimationTool files: \"" + strSetFilename +
                            "\" and \"" + strMapFilename + "\"";
                        MLogger.LogWarning(strLogMsg);
                    }
                }
            }

            MFramework.Instance.Input.SubmitCommand(
                MUICommand.CommandType.RESET_BACKGROUND_HEIGHT);
            MFramework.Instance.Input.SubmitCommand(
                MUICommand.CommandType.ZOOM_EXTENTS);

            if (MFramework.Instance.Animation.UnresolvedSequenceInfo.Count 
                != 0)
            {
                Hashtable hashFilenames = new Hashtable();
                ArrayList aMissingFilenames = new ArrayList();
                ArrayList aMissingSequenceNames = new ArrayList();
                foreach (MSequence.MInvalidSequenceInfo kInfo in
                    MFramework.Instance.Animation.UnresolvedSequenceInfo)
                {
                    if (!hashFilenames.Contains(kInfo.Filename))
                    {
                        string strConvertedFilename = MessageBoxManager
                            .ConvertFilename(kInfo.Filename);
                        aMissingFilenames.Add(strConvertedFilename);
                        hashFilenames.Add(kInfo.Filename, null);
                    }

                    string strMissing;
                    if (kInfo.SequenceName == null)
                        strMissing = "AnimIndex = " + kInfo.AnimIndex;
                    else
                        strMissing = "SequenceName = " + kInfo.SequenceName;

                    aMissingSequenceNames.Add(strMissing);
                }

                string[] astrVariables = new string[2];
                astrVariables[0] = MessageBoxManager.CreateNonBulletedList(
                    aMissingFilenames);
                astrVariables[1] = MessageBoxManager.CreateNonBulletedList(
                    aMissingSequenceNames);
                MessageBoxManager.DoMessageBox("CannotFindKFError.rtf",
                    "Cannot Find Project KF Files", MessageBoxButtons.OK,
                    MessageBoxIcon.Error, astrVariables);
            }

            string strErrors = "";
            ArrayList kSequences = MFramework.Instance.Animation.Sequences;
            for (int i = 0; i < kSequences.Count; i++)
            {
                if (kSequences[i] == null)
                    continue;
                MSequence kSeq = (MSequence) kSequences[i];
                CheckForModelRootConflicts(kSeq, ref strErrors);
            }

            CorrectMissingTextAndMorphTags(ref strErrors);

            if (strErrors.Length != 0)
            {
                DoKFLoadError("(Multiple Files)",  
                    "\\pard\\par\n" + strErrors,
                    "Error(s) Loading Multiple KF Files");
            }

            try
            {
                // add successfully opened file to MRU list
                MRUManager.AddFile(strOriginalName);
            }
            catch (Exception ex)
            {
                MLogger.LogWarning(ex.Message);
                // remove file from MRU list
                MRUManager.RemoveFile(strOriginalName);
            }
            
            ProjectData.Instance.SortTree();

            ArrayList aClampMorphs = GetMorphsHavingClamps();
            
            if (aClampMorphs.Count != 0)
            {
                string [] strMorphs = new string[aClampMorphs.Count];
                int j = 0;
                foreach (MTransition tran in aClampMorphs)
                {
                    strMorphs[j] = "\"" + tran.Source + "\" to \"" + 
                        tran.Destination + "\"";
                    j++;
                }

                // This kfm has morphs having sequences with a cycle type
                // of CLAMP. Most likely, this is not intentional.
                string [] strVariables = new string[1];
                strVariables[0] = 
                    MessageBoxManager.CreateBulletedList(strMorphs);

                MessageBoxManager.DoMessageBox(
                    "MorphsHavingClampTypeWarning.rtf",
                    "Morphs Having CLAMP cycle type found", 
                    MessageBoxButtons.OK, MessageBoxIcon.Warning, 
                    strVariables);
            }
            //TestFrequencies();
            MFramework.Instance.Renderer.CreateFrame();
        }
    
        static public void OpenKFMDlg(System.Windows.Forms.Form kForm)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = "Animation Tool Files (*.kfm)|*.kfm";
            if (dlg.ShowDialog(kForm) == DialogResult.OK)
            {
                string strFilename = dlg.FileName;                

                string strLog = 
                    UserLogs.GetLogString(UserLogs.LogType.KFM_DLG_OK);
                MLogger.LogGeneral(strLog + strFilename);

                AnimationHelpers.OpenKFMFile(strFilename);
            }
            else
            {
                string strLog = 
                    UserLogs.GetLogString(UserLogs.LogType.KFM_DLG_CANCEL);
                MLogger.LogGeneral(strLog);
            }
        }

        static public bool SaveKFMDlg(System.Windows.Forms.Form kForm,
            bool bSaveAs)
        {
            if (!MFramework.Instance.Animation.CanSave)
            {
                MessageBoxManager.DoErrorBox(
                    "CannotSaveIncompleteProject.rtf",
                    "Cannot Save Incomplete Project", null);
                return false;
            }

            string strFilename = MFramework.Instance.Animation.KFMFilename;

            if (strFilename == null || strFilename.Length == 0)
                bSaveAs = true;

            if (bSaveAs)
            {
                SaveFileDialog dlg = new SaveFileDialog();
                if (strFilename != null && strFilename.Length != 0)
                    dlg.FileName = strFilename;
                dlg.Filter = "Animation Tool Files (*.kfm)|*.kfm";
                bool bSave = dlg.ShowDialog(kForm) == DialogResult.OK;
                if (bSave)
                {
                    strFilename = dlg.FileName;
                    string strLog = 
                        UserLogs.GetLogString(UserLogs.LogType.KFM_DLG_OK);
                    MLogger.LogGeneral(strLog + strFilename);
                }
                else
                {
                    string strLog = 
                        UserLogs.GetLogString(UserLogs.LogType.KFM_DLG_CANCEL);
                    MLogger.LogGeneral(strLog);
                    return false;
                }
            }

            string strSettingsFilename = null;
            string strHeaderFilename = null;
            bool bSettingsReadOnly = false;
            bool bHeaderReadOnly = false;
            bool bKFMReadOnly = false;

            int iDotIdx = strFilename.LastIndexOf('.');
            
            if (iDotIdx != -1)
            {
                string strFilenameBase = strFilename.Substring(0, iDotIdx);
                strSettingsFilename = strFilenameBase + ".settings";
                strHeaderFilename = strFilenameBase + "_Anim.h";

                if (File.Exists(strSettingsFilename) && 
                    (File.GetAttributes(strSettingsFilename) & 
                     FileAttributes.ReadOnly) == FileAttributes.ReadOnly)
                {
                    bSettingsReadOnly = true;
                }

                if (File.Exists(strHeaderFilename) && 
                    (File.GetAttributes(strHeaderFilename) & 
                     FileAttributes.ReadOnly) == FileAttributes.ReadOnly)
                {
                    bHeaderReadOnly = true;
                }
            }

            if (File.Exists(strFilename) && 
                (File.GetAttributes(strFilename) & FileAttributes.ReadOnly) ==
                FileAttributes.ReadOnly)
            {
                bKFMReadOnly = true;
            }
            
            if (bKFMReadOnly || bSettingsReadOnly || bHeaderReadOnly)
            {
                string strFile = null;
                ArrayList kFileArray = new ArrayList();
                if (bKFMReadOnly)
                    kFileArray.Add(AppUtils.ExtractFileFromPath(strFilename));
                if (bSettingsReadOnly)
                    kFileArray.Add(
                        AppUtils.ExtractFileFromPath(strSettingsFilename));
                if (bHeaderReadOnly)
                    kFileArray.Add(
                        AppUtils.ExtractFileFromPath(strHeaderFilename));

                strFile = MessageBoxManager.CreateBulletedList(kFileArray);
                string strAppTitle = "Animation Tool";

                FileOverwriteForm kOverwriteForm = new FileOverwriteForm(
                    strFile, strAppTitle);
                DialogResult kRes = kOverwriteForm.ShowDialog(kForm);
                if (kRes == DialogResult.Yes)
                {
                    // Save As
                    return SaveKFMDlg(kForm, true); 
                }
                else if (kRes == DialogResult.No)
                {
                    // Overwrite
                    try 
                    {
                        if (bKFMReadOnly)
                            File.SetAttributes(
                                strFilename, FileAttributes.Normal);
                        if (bSettingsReadOnly)
                            File.SetAttributes(
                                strSettingsFilename, FileAttributes.Normal);
                        if (bHeaderReadOnly)
                            File.SetAttributes(
                                strHeaderFilename,FileAttributes.Normal);
                        
                    }
                    catch(Exception e)
                    {
                        Debug.Assert(false, e.Message);
                        return false;
                    }
                }
                else
                {
                    // Cancel
                    return false;
                }
            }

            if (File.Exists(strFilename) &&
                (File.GetAttributes(strFilename) & FileAttributes.ReadOnly) ==
                System.IO.FileAttributes.ReadOnly)
            {
                Debug.Assert(false, "File.SetAttributes failed!");
                ShowSaveError(strFilename, "File is read-only.");
                return false;
            }

            if (File.Exists(strSettingsFilename) &&
                (File.GetAttributes(strSettingsFilename) & 
                 FileAttributes.ReadOnly) ==System.IO.FileAttributes.ReadOnly)
            {
                Debug.Assert(false, "File.SetAttributes failed!");
                ShowSaveError(strSettingsFilename, "File is read-only.");
                return false;
            }

            if (File.Exists(strHeaderFilename) &&
                (File.GetAttributes(strHeaderFilename) & 
                 FileAttributes.ReadOnly) == System.IO.FileAttributes.ReadOnly)
            {
                Debug.Assert(false, "File.SetAttributes failed!");
                ShowSaveError(strHeaderFilename, "File is read-only.");
                return false;
            }

            if (!MFramework.Instance.Animation.SaveKFM(strFilename))
            {
                ShowSaveError(strFilename, 
                    MFramework.Instance.Animation.GetLastErrorString());
                return false;
            }

            if (strSettingsFilename != null)
            {
                bool bWriteSettings = ProjectData.Save(strSettingsFilename);
                Debug.Assert(bWriteSettings, 
                    "Failed to write settings", "Failed to write settings");
            }

            if (strHeaderFilename != null)
            {
                bool bWriteHeader = ProjectData.Instance.
                    WriteHeaderFile(strHeaderFilename);
                Debug.Assert(bWriteHeader, 
                    "Failed to write header", "Failed to write header");
            }
            
            try
            {
                // add successfully opened file to MRU list
                MRUManager.AddFile(strFilename);
            }
            catch (Exception ex)
            {
                MLogger.LogWarning(ex.Message);
                // remove file from MRU list
                MRUManager.RemoveFile(strFilename);
            }
            
            return true;
        }

        private static void ShowSaveError(string strFilename, string strError)
        {
            string [] strVariables = new string[2];
            strVariables[0] = MessageBoxManager.ConvertFilename(strFilename);
            strVariables[1] = strError + "\\par\n";
                    
            MessageBoxManager.DoMessageBox(
                "SaveKFMError.rtf", 
                "Error Saving KFM",
                MessageBoxButtons.OK,
                MessageBoxIcon.Error, strVariables);
        }

        #endregion

        #region KF Helpers

        private static void LoadKF(string strFilename,
            ref string strErrors)
        {
            ArrayList kSequencesAdded = MFramework.Instance.Animation
                .AddSequencesFromKF(strFilename, ref strErrors);

            if (kSequencesAdded != null)
            {
                foreach (MSequence kSeq in kSequencesAdded)
                {
                    CheckForModelRootConflicts(kSeq, ref strErrors);
                }
            }
        }

        private static bool VerifyValidFile()
        {
            if (!MFramework.Instance.Animation.CanSave)
            {
                MessageBoxManager.DoErrorBox(
                    "CannotCompleteOperationDueToIncompleteProj.rtf",
                    "Cannot Complete Operation", null);
                return false;
            }
            return true;
        }

        private static void DoKFLoadError(string strFilename,
            string strError, string strTitle)
        {
            string [] strVariables = new string[2];
            strVariables[0] = MessageBoxManager.ConvertFilename(strFilename);
            strVariables[1] = strError + "\\par\n";
                    
            MessageBoxManager.DoMessageBox(
                "LoadKFError.rtf", 
                strTitle,
                MessageBoxButtons.OK,
                MessageBoxIcon.Error, strVariables);
        }

        public static void OpenMultipleKFFiles(
            string [] strKFFilenames)
        {
            MAnimation kAnim = MFramework.Instance.Animation;
            string strErrors = "";

            foreach (string Filename in strKFFilenames)
            {
                string strCurrentErrors = "";
                LoadKF(Filename, ref strCurrentErrors);

                if (strCurrentErrors.Length != 0)
                {
                    strErrors += "Filename: " +
                        MessageBoxManager.ConvertFilename(Filename) +
                        "\\par\n\\par\nErrors:\\par\n" + strCurrentErrors
                        + "\\par\n\\par\n";
                }
            }

            if (strErrors.Length != 0)
            {
                DoKFLoadError("(Multiple Files)",  
                    "\\pard\\par\n" + strErrors,
                    "Error Loading Multiple KF Files");
            }
        }

        public static void OpenMultipleKFsDialog(
            System.Windows.Forms.Form kForm)
        {
            if (!VerifyValidFile())
                return;

            OpenFileDialog OpenDialog_KF = new OpenFileDialog();
            OpenDialog_KF.DefaultExt = "kf";
            OpenDialog_KF.Filter = "KF files (*.kf)|*.kf";
            OpenDialog_KF.Title = "Open KF File(s)";
            OpenDialog_KF.Multiselect = true;

            if(OpenDialog_KF.ShowDialog(kForm) == DialogResult.OK)
            {
                foreach (string str in OpenDialog_KF.FileNames)
                {
                    MLogger.LogGeneral("   " + str);                   
                }

                OpenMultipleKFFiles(OpenDialog_KF.FileNames);
            }
            else
            {
                MLogger.LogGeneral("   Cancel");                   
            }
        }

        public static bool DoMissingKFDialog(
            System.Windows.Forms.Form kForm, bool bUseFilenamePath, 
            ref string strFilename, ref bool bCancel)
        {
            OpenFileDialog OpenDialog_KF = new OpenFileDialog();
            if (bUseFilenamePath)
                OpenDialog_KF.FileName = strFilename;
            else
                OpenDialog_KF.FileName = Path.GetFileName(strFilename);

            OpenDialog_KF.DefaultExt = "kf";
            OpenDialog_KF.Filter = "KF files (*.kf)|*.kf";
            OpenDialog_KF.Title = "Find Missing KF File";
            OpenDialog_KF.Multiselect = false;
            OpenDialog_KF.CheckFileExists = true;
            OpenDialog_KF.CheckPathExists = true;

            DialogResult dr = OpenDialog_KF.ShowDialog(kForm);

            if (dr == DialogResult.OK)
            {
                strFilename = OpenDialog_KF.FileName;
                return true;
            }
            else if (dr == DialogResult.Cancel)
            {
                bCancel = true;
            }
            return false;
        }

        public static bool DoMissingSequenceDialog(
            System.Windows.Forms.Form kForm, string missingName,
            string strFileName, ref string strSequenceName, ref bool bCancel)
        {
            ChooseSequenceForm kChooser = new ChooseSequenceForm();
            kChooser.KFFile = strFileName;
            kChooser.MissingComponent = missingName;

            DialogResult dr = kChooser.ShowDialog(kForm);
            if (dr == DialogResult.OK)
            {
                strSequenceName = kChooser.SequenceName;
                return true;
            }
            else if (dr == DialogResult.Cancel)
            {
                bCancel = true;
            }

            return false;
        }

        private static bool CheckForModelRootConflicts(MSequence kSeq,
            ref string strErrors)
        {
            ArrayList kConflicts = MFramework.Instance.Animation
                .GetModelRootConflictsForSequence(kSeq);
            
            if (kConflicts.Count != 0)
            {
                strErrors += "The following objects were not found when " +
                    "attempting to attach \"" + kSeq.Name + "\" to the " +
                    "current scene:\\par";
                strErrors += MessageBoxManager.CreateBulletedList(kConflicts)
                    + "\\par\n\\par\n";;
                return true;
            }

            return false;
        }
        #endregion

        #region Transition Helpers
        public static int CountTransitionStoredType(ArrayList aTrans, 
            MTransition.TransitionType eType)
        {
            int iCount = 0;
            foreach(MTransitionData td in aTrans)
            {
                if (td.Transition == null)
                    continue;

                if (td.Transition.StoredType == eType)
                    iCount++;
            }

            return iCount;
        }

        public static ArrayList GetTransStoredTypeSubset(ArrayList aTrans,
            MTransition.TransitionType eType)
        {
            int iCount = CountTransitionStoredType(aTrans, eType);

            ArrayList al = new ArrayList(iCount);
            foreach(MTransitionData td in aTrans)
            {
                if (td.Transition == null)
                    continue;

                if (td.Transition.StoredType == eType)
                    al.Add(td.Transition);
            }
            
            return al;
        }


        public static int CountTransitionType(ArrayList aTrans, 
            MTransition.TransitionType eType)
        {
            int iCount = 0;
            foreach(MTransitionData td in aTrans)
            {
                if (td.Transition == null)
                    continue;

                if (td.Transition.Type == eType)
                    iCount++;
            }

            return iCount;
        }

        public static ArrayList GetTransTypeSubset(ArrayList aTrans,
            MTransition.TransitionType eType)
        {
            int iCount = CountTransitionType(aTrans, eType);

            ArrayList al = new ArrayList(iCount);
            foreach(MTransitionData td in aTrans)
            {
                if (td.Transition == null)
                    continue;

                if (td.Transition.Type == eType)
                    al.Add(td.Transition);
            }
            
            return al;
        }

        public static bool ConvertTransition(ref ArrayList Transitions, 
            MTransition.TransitionType eConvertToType,
            ArrayList ChainSequences)
        {
            bool bSomeChange = false;

            DialogResult kYesNoCancelResult =
                MessageBoxManager.DoMessageBox("ChangeTransitionWarning.rtf", 
                "Change Transition Warning",
                MessageBoxButtons.YesNo,
                MessageBoxIcon.Question, null);

            if (kYesNoCancelResult != DialogResult.Yes)
                return false;

            for (int i = 0; i < Transitions.Count; i++)
            {
                MTransitionData data = ((MTransitionData)Transitions[i]);
                if (data.Source == data.Destination)
                    continue; // Not legal

                MTransition Tran = data.Transition;
                if (Tran == null && eConvertToType ==
                    MTransition.TransitionType.Trans_None)
                    continue; // No work to be done

                if (Tran == null)
                {
                    CreateTransition(Tran, data, eConvertToType,
                        ChainSequences);
                    bSomeChange = true;
                    continue;
                }

                bSomeChange = ConvertTransition(
                    Tran, data, eConvertToType, ChainSequences);
            }

            MLogger.Update();

            return bSomeChange;
        }

        private static bool ConvertTransition(
            MTransition Tran, MTransitionData data,
            MTransition.TransitionType eConvertToType,
            ArrayList ChainSequences)
        {
            Debug.Assert(Tran != null);               

            // If converting to a Morph and there are sequences with
            // CLAMP cycle types, allow the user to abort.
            if (AbortDueToClampMorphs(Tran, eConvertToType))
                return false;  

            // If converting affects chains, allow the user to abort.
            if (AbortDueToChainDependencies(Tran, eConvertToType))
                return false; 

            MSequence Src = data.Source;
            MSequence Dest = data.Destination;

            string strOldTran = 
                ProjectData.GetTypeNameForTransitionType(Tran.StoredType);
            string strNewTran = 
                ProjectData.GetTypeNameForTransitionType(eConvertToType);    

            switch(eConvertToType)
            {
                case MTransition.TransitionType.Trans_None:
                    MFramework.Instance.Animation.RemoveTransition(
                        Src.SequenceID, Dest.SequenceID);
                    Tran = null;
                    break;
                case MTransition.TransitionType.Trans_Chain:
                    ConvertChainTransition(Tran, ChainSequences);
                    break;
                default:
                    Tran.StoredType = eConvertToType;
                    break;
            }

            data.Transition = Tran;      

            MLogger.LogGeneral("Changed transition (\"" + Src + "\" to \"" + 
                Dest + "\") from \"" + strOldTran + "\" to \"" + 
                strNewTran + "\"", false);               

            return true;
        }

        private static bool AbortDueToClampMorphs(MTransition Tran,
            MTransition.TransitionType eConvertToType)
        {
            if (eConvertToType != MTransition.TransitionType.Trans_Morph)
                return false; // Not a morph, so user doesn't get option

            if (Tran.Source.Loop == true && Tran.Destination.Loop == true)
                return false; // No CLAMPs, so user doesn't get option

            // Clamps and Morphs are not well supported...
            string [] strVariables = new string[2];
            strVariables[0] = Tran.Source.Name;
            strVariables[1] = Tran.Destination.Name;

            DialogResult kYesNoCancelResult = MessageBoxManager.DoMessageBox(
                "MorphSequenceIsClamp.rtf",
                "Source or Destination Sequence is CLAMP", 
                MessageBoxButtons.YesNo, MessageBoxIcon.Warning, 
                strVariables);

            if (kYesNoCancelResult != DialogResult.Yes)
                return true; // User wishes to abort

            return false;
        }

        private static bool AbortDueToChainDependencies(MTransition Tran,
            MTransition.TransitionType eConvertToType)
        {
            ArrayList kList = GetChainDependencies(Tran);

            if (kList.Count == 0)
                return false; // Do not abort

            string [] strTrans = new string[kList.Count];
            int j = 0;
            foreach (MTransition tran in kList)
            {
                strTrans[j] = "\"" + tran.Source + "\" to \"" + 
                    tran.Destination + "\"";
                j++;
            }

            string [] strVariables = new string[3];
            strVariables[0] = Tran.Source.Name +
                '"' + " to " + '"' + Tran.Destination.Name;
            strVariables[1] = "transition";
            strVariables[2] = MessageBoxManager.CreateBulletedList(strTrans);

            string strEmbeddedMsg;
            switch(eConvertToType)
            {
                case MTransition.TransitionType.Trans_None:
                    strEmbeddedMsg = "DependentChainsFoundDeleteDelete.rtf";
                    break;
                case MTransition.TransitionType.Trans_Chain:
                    strEmbeddedMsg = "DependentChainsFoundModifyDelete.rtf";
                    break;
                default:
                    strEmbeddedMsg = "DependentChainsFoundModifyModify.rtf";
                    break;
            }

            if (MessageBoxManager.DoMessageBox(strEmbeddedMsg,
                "Chain Transition Dependencies Found", 
                MessageBoxButtons.YesNo, MessageBoxIcon.Warning, 
                strVariables) != DialogResult.Yes)
                return true; // User abort due to chain dependency

            return false; // Do not abort
        }

        private static void CreateTransition(
            MTransition Tran, MTransitionData data,
            MTransition.TransitionType eConvertToType,
            ArrayList ChainSequences)
        {
            Debug.Assert(Tran == null);
            Debug.Assert(eConvertToType != 
                MTransition.TransitionType.Trans_None);

            MSequence Src = data.Source;
            MSequence Dest = data.Destination;

            if (eConvertToType ==
                MTransition.TransitionType.Trans_Chain)
            {
                Tran = MFramework.Instance.Animation.AddTransition(
                    Src.SequenceID, Dest.SequenceID, 
                    MTransition.TransitionType.Trans_ImmediateBlend,
                    0.5f);
                ConvertChainTransition(Tran, ChainSequences);
            }
            else
            {
                Tran = MFramework.Instance.Animation.AddTransition(
                    Src.SequenceID, Dest.SequenceID, eConvertToType,
                    0.5f);
            }

            string strOldTran = ProjectData.GetTypeNameForTransitionType(
                MTransition.TransitionType.Trans_None);
            string strNewTran = ProjectData.GetTypeNameForTransitionType(
                eConvertToType);
        
            data.Transition = Tran;

            MLogger.LogGeneral("Changed transition (\"" + Src + "\" to \"" + 
                Dest + "\") from \"" + strOldTran + "\" to \"" + 
                strNewTran + "\"");
        }

        public static void ConvertChainTransition(MTransition tran,
            ArrayList aChainSequences)
        {
            Debug.Assert(aChainSequences != null);

            if (tran.Type != MTransition.TransitionType.Trans_Chain)
            {
                float[] afDurations = new float[aChainSequences.Count - 2];
                MSequence[] aSequences = new MSequence[aChainSequences.Count -
                    2];

                for (int j = 1; j < aChainSequences.Count - 1; j++)
                {
                    afDurations[j - 1] =
                        MTransition.MChainInfo.MAX_DURATION;
                    aSequences[j - 1] = (MSequence) aChainSequences[j];
                }

                tran.ConvertToChain(aSequences, afDurations);
                return;
            }

            ReplaceChainTransition(tran, aChainSequences);
        }

        public static void ReplaceChainTransition(MTransition tran,
            ArrayList aChainSequences)
        {
            Debug.Assert(aChainSequences != null);

            float[] afDurations = new float[aChainSequences.Count - 2];
            MSequence[] aSequences = new MSequence[aChainSequences.Count -
                2];

            for (int j = 0; j < afDurations.Length; j++)
                afDurations[j] = MTransition.MChainInfo.MAX_DURATION;

            // Save Duration settings...
            int i;
            for (i = 1; i < aChainSequences.Count - 1; i++)
            {
                MSequence seq = (MSequence) aChainSequences[i];

                int iOccurance = 0;
                for (int iPrev = i-1; iPrev >=0; iPrev--)
                {
                    if (seq.SequenceID == 
                        ((MSequence)aChainSequences[iPrev]).SequenceID)
                        iOccurance++;
                }

                int iFoundIdx = 
                    tran.GetChainInfoIndex(seq.SequenceID, iOccurance);
                
                if (iFoundIdx != -1)
                {
                    afDurations[i-1] = tran.ChainInfo[iFoundIdx].Duration;
                }
            }

            i=0;
            for (int iInsertIdx = 1; iInsertIdx < aChainSequences.Count-1; 
                 iInsertIdx++)
            {
                MSequence seq = (MSequence) aChainSequences[iInsertIdx];
                aSequences[i] = seq;
                i++;
            }

            tran.ReplaceChain(aSequences, afDurations);
        }


        //        public static void TestFrequencies()
        //        {
        //            float fFreq = 0.3f;
        //
        //            // TestFunction
        //            foreach (MSequence seq in MFramework.Instance.Animation
        //                .Sequences)
        //            {               
        //                seq.Frequency = fFreq;
        //                fFreq += 1.0f;
        //
        //                if (fFreq > 6.0f)
        //                    fFreq = 0.3f;
        //
        //
        //                // Ensure that our defaults are initialize correctly
        //                if (seq.Transitions != null)
        //                {
        //                    foreach (MTransition t in seq.Transitions)
        //                    {
        //                        t.DefaultTransitionTime = seq.Length;
        //                    }
        //                }
        //            }
        //        }

        public static ArrayList GetSequenceGroupDependencies(MSequence seq)
        {
            ArrayList aDependents = new ArrayList();

            foreach (MSequenceGroup sgPossibleDep in 
                MFramework.Instance.Animation.SequenceGroups)
            {
                MSequenceGroup.MSequenceInfo[] aSeqInfo = 
                    sgPossibleDep.SequenceInfo;

                if (aSeqInfo == null)
                    continue;

                foreach (MSequenceGroup.MSequenceInfo seqInfo in aSeqInfo)
                {
                    if (seqInfo == null)
                        continue;

                    if (seq.SequenceID == seqInfo.SequenceID)
                    {
                        // A possible dependency... we can break;
                        aDependents.Add(sgPossibleDep);
                        break;
                    }                   
                }
            }

            return aDependents;
        }

        public static ArrayList GetMorphsHavingClamps()
        {
            ArrayList aMorphs = new ArrayList();

            foreach (MSequence seqPossibleDep in
                MFramework.Instance.Animation.Sequences)
            {
                if (seqPossibleDep == null || 
                    seqPossibleDep.Transitions == null)
                    continue;

                foreach (MTransition kT in seqPossibleDep.Transitions)
                {
                    if (kT == null)
                        continue;

                    if (kT.Type != MTransition.TransitionType.Trans_Morph)
                        continue;

                    if (kT.Source.Loop == true && 
                        kT.Destination.Loop == true)
                        continue;

                    aMorphs.Add(kT);
                }
            }

            return aMorphs;
        }

        public static ArrayList GetChainDependencies(MTransition tran)
        {
            MSequence srcSeq = tran.Source;
            MSequence destSeq = tran.Destination;

            ArrayList aDependents = new ArrayList();
            foreach (MSequence seqPossibleDep in
                MFramework.Instance.Animation.Sequences)
            {
                if (seqPossibleDep == null ||
                    seqPossibleDep.Transitions == null)
                    continue;

                foreach (MTransition kT in seqPossibleDep.Transitions)
                {
                    if (kT == null)
                        continue;

                    if (kT == tran)
                        continue;

                    if (kT.Type != MTransition.TransitionType.Trans_Chain)
                        continue;

                    MTransition.MChainInfo[] akInfo = kT.ChainInfo;

                    // First check source to initial chain...
                    // and second to last to final...
                    if (kT.SrcID == srcSeq.SequenceID)
                    {
                        if (akInfo[0].SequenceID == destSeq.SequenceID)
                        {
                            aDependents.Add(kT);
                            continue;
                        }
                    }

                    if (akInfo[akInfo.Length-1].SequenceID == 
                        srcSeq.SequenceID)
                    {
                        if (kT.DesID == destSeq.SequenceID)
                        {
                            aDependents.Add(kT);
                            continue;
                        }
                    }

                    // interior sections....
                    for (int i=1; i<akInfo.Length-1; i++)
                    {
                        MTransition.MChainInfo info = akInfo[i];

                        // If we find the same pair, report this chain
                        if (info.SequenceID == srcSeq.SequenceID)
                        {                        
                            if (akInfo[i+1].SequenceID == destSeq.SequenceID)
                            {
                                aDependents.Add(kT);
                                break;
                            }
                        }
                    }

                }
            }

            return aDependents;
        }

        public static ArrayList GetChainDependencies(MSequence seq)
        {
            ArrayList aDependents = new ArrayList();
            foreach (MSequence seqPossibleDep in
                MFramework.Instance.Animation.Sequences)
            {
                if (seq.SequenceID == seqPossibleDep.SequenceID)
                    continue;
                
                if (seqPossibleDep.Transitions != null)
                {
                    foreach (MTransition tran in seqPossibleDep.Transitions)
                    {
                        if (tran.Type ==
                            MTransition.TransitionType.Trans_Chain)
                        {
                            foreach (MTransition.MChainInfo info in
                                tran.ChainInfo)
                            {
                                if (info.SequenceID == seq.SequenceID)
                                {
                                    aDependents.Add(tran);
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            return aDependents;
        }

        static public MTransition.TransitionType GetCommonStoredType(
            ArrayList kList)
        {
            MTransition.TransitionType eType = MTransition.TransitionType
                .Trans_None;

            MTransition kTran = ((MTransitionData)kList[0]).Transition;
            if (kTran != null)
                eType = kTran.StoredType;

            for (int i = 1; i < kList.Count; i++)
            {
                MTransitionData data = (MTransitionData) kList[i];

                if (data.Source == data.Destination)
                    continue;

                if (data.Transition != null && 
                    data.Transition.StoredType != eType)
                {
                    return MTransition.TransitionType.Trans_MultipleSelected;
                }
                else if (data.Transition != null && 
                    data.Transition.StoredType ==
                    MTransition.TransitionType.Trans_Chain)
                {
                    return MTransition.TransitionType.Trans_MultipleSelected;
                }
                else if (data.Transition == null && eType != 
                    MTransition.TransitionType.Trans_None)
                {
                    return MTransition.TransitionType.Trans_MultipleSelected;
                }
            }

            return eType;
        }

        static public ArrayList GetPossibleTransitionTypes(
            ArrayList TransitionData)
        {
            bool bMultiSelect = false;

            ArrayList PossibleTransitionList = null;
            foreach (MTransitionData data in TransitionData)
            {
                ArrayList kValidTypes =
                    MTransition.GetValidTypesForTransition(data.Source,
                    data.Destination);
                if (data.Source == data.Destination)
                    continue;

                if (PossibleTransitionList == null)
                {
                    PossibleTransitionList = kValidTypes;
                }
                else
                {
                    bMultiSelect = true;

                    ArrayList kNewList = new ArrayList(0);
                    foreach (MTransition.TransitionType type in
                        PossibleTransitionList)
                    {
                        if (type == MTransition.TransitionType.Trans_Chain)
                            continue;

                        if (kValidTypes.Contains(type))
                            kNewList.Add(type);
                    }
                    PossibleTransitionList = kNewList;
                }
            }

            // If a mixture of chains and non-chains, we allow for 
            // chain conversion.
            if (bMultiSelect)
            {
                bool bAllChains = true;

                foreach (MTransitionData data in TransitionData)
                {
                    if (data.Transition == null ||
                        (data.Transition.Type != 
                        MTransition.TransitionType.Trans_Chain))
                    {
                        bAllChains = false;
                        break;
                    }
                }

                if (bAllChains == false)
                {
                    PossibleTransitionList.Add(
                        MTransition.TransitionType.Trans_Chain);
                }
            }


            return PossibleTransitionList;
        }

        public static ArrayList Convert_aTran_To_aaDur(ArrayList aTran)
        {
            // Converts an array of MTransitions to and array of
            // arrays of durations (for corresponding sequences).
            ArrayList aaDur = new ArrayList(aTran.Count);

            foreach (MTransition tran in aTran)
            {
                int iLength = tran.ChainInfo.Length;
                ArrayList aDur = new ArrayList(iLength + 2);

                aDur.Add(tran.Duration);
                foreach(MTransition.MChainInfo ci in tran.ChainInfo)
                {
                    aDur.Add(ci.Duration);
                }
                aDur.Add(-1);
                aaDur.Add(aDur);
            }
            return aaDur;
        }

        public static ArrayList Convert_aTran_To_aaSeq(ArrayList aTran)
        {
            // Converts an array of MTransitions to and array of
            // arrays of sequences.
            ArrayList aaSeq = new ArrayList(aTran.Count);

            foreach (MTransition tran in aTran)
            {
                int iLength = tran.ChainInfo.Length;
                ArrayList aSeq = new ArrayList(iLength + 2);

                aSeq.Add(tran.Source);
                foreach(MTransition.MChainInfo ci in tran.ChainInfo)
                {
                    MSequence seq = 
                        MFramework.Instance.Animation.GetSequence(
                        ci.SequenceID);

                    aSeq.Add(seq);
                }
                aSeq.Add(tran.Destination);
                aaSeq.Add(aSeq);
            }
            return aaSeq;
        }

        public static ArrayList GetCommonSrcTextKeys(ArrayList aTran)
        {
            return GetCommonTextKeys(aTran,true);
        }
        
        public static ArrayList GetCommonDesTextKeys(ArrayList aTran)
        {
            return GetCommonTextKeys(aTran,false);
        }

        public static ArrayList GetCommonTextKeys(
            ArrayList aTran, bool bFromSrc)
        {
            MTransition baseTran = (MTransition)aTran[0]; 
            MSequence.MTextKey[] aBaseTextKeys;
            
            if (bFromSrc)
                aBaseTextKeys = baseTran.Source.TextKeys;
            else
                aBaseTextKeys = baseTran.Destination.TextKeys;

            int iBaseTextKeyCount = 0;          
            if (aBaseTextKeys != null)
                iBaseTextKeyCount = aBaseTextKeys.Length;

            if (iBaseTextKeyCount < 1)
                return null;

            // ensure that each of the pairs from the base exist...
            ArrayList al = new ArrayList(iBaseTextKeyCount);

            foreach (MSequence.MTextKey tk in aBaseTextKeys)
            { // for each of our base textkeys

                bool bFound = true;
                for (int i = 1; i < aTran.Count; i++)
                {   // for each blend pair transition
                    bFound = false;

                    MTransition currTran = (MTransition)aTran[i];
                    int iKeys = 0;
                
                    MSequence.MTextKey[] currTextKeys = null;

                    if (bFromSrc)
                        currTextKeys = currTran.Source.TextKeys;
                    else
                        currTextKeys = currTran.Destination.TextKeys;

                    if (currTextKeys != null)
                        iKeys = currTextKeys.Length;

                    for (int j=0; j<iKeys; j++)
                    {   // For each pair in the transition
                        if (tk.Text == currTextKeys[j].Text)
                        {
                            // This pair is good for this transition
                            bFound = true;
                            break;
                        }
                    }

                    if (bFound)
                    {
                        // We found it in this one, so continue
                        continue;
                    }
                    
                    // wasn't found in this transition, so it isn't 
                    // common to all
                    break;
                }

                if (bFound == true)
                {
                    // was found in every transition, so add it
                    al.Add(tk);
                }
            }

            return al;
        }

        public static MTransition.MBlendPair[] GetCommonBlendPairs(
            ArrayList aTran)
        {
            // [1] Ensure that every sequence list is the same size
            MTransition baseTran = (MTransition)aTran[0]; 

            if (baseTran.Type != MTransition.TransitionType.Trans_DelayedBlend)
                return null;

            MTransition.MBlendPair[] aBaseBP = baseTran.BlendPairs;
            int iBlendPairCount = 0;
            
            if (aBaseBP != null)
                iBlendPairCount = aBaseBP.Length;

            if (iBlendPairCount < 1)
                return null;

            // ensure that each of the pairs from the base exist...
            //ArrayList al = new ArrayList(iBlendPairCount);
            MTransition.MBlendPair[] aBP = 
                new MTransition.MBlendPair[iBlendPairCount];
            uint uiCnt = 0;

            foreach(MTransition.MBlendPair bp in aBaseBP)
            { // For each blend pair

                bool bFound = true;

                for (int i = 1; i < aTran.Count; i++)
                {   // for each blend pair transition
                    bFound = false;

                    MTransition currTran = (MTransition)aTran[i];
                    int iPairs = 0;
                
                    if (currTran.Type != 
                        MTransition.TransitionType.Trans_DelayedBlend)
                        break;

                    if (currTran.BlendPairs != null)
                        iPairs = currTran.BlendPairs.Length;

                    for (int j=0; j<iPairs; j++)
                    {   // For each pair in the transition
                        if (bp.StartKey == currTran.BlendPairs[j].StartKey &&
                            bp.TargetKey == currTran.BlendPairs[j].TargetKey)
                        {
                            // This pair is good for this transition
                            bFound = true;
                            break;
                        }
                    }

                    if (bFound)
                    {
                        // We found it in this one, so continue
                        continue;
                    }
                    
                    // wasn't found in this transition, so it isn't 
                    // common to all
                    break;
                }

                if (bFound == true)
                {
                    // was found in every transition, so add it
                    //al.Add(bp);
                    aBP[uiCnt] = bp;
                    uiCnt++;
                }
            }

            return aBP;
        }

        public static bool DoChainsHaveExactCommonSequences(ArrayList aaSeq)
        {
            // [1] Ensure that every sequence list is the same size
            ArrayList aSeqBase = (ArrayList)aaSeq[0]; 
            int iChainLength = aSeqBase.Count;

            if (iChainLength <= 2)
            {
                return false;
            }
               
            for (int i = 1; i < aaSeq.Count; i++)
            {
                ArrayList aSeq = (ArrayList)aaSeq[i]; 
                int iLength = aSeq.Count;

                if (iChainLength != iLength)
                {
                    return false;
                }
            }

            // Each chain has the same amount of intermediates
            // Now we must compare to see that thare are all equal
            // Basically, we do for each index, check every sequence
            // to ensure sameness.
            for (int i=1; i<iChainLength-1; i++)
            {
                for (int j=1; j<aaSeq.Count; j++)
                {
                    ArrayList aSeq = (ArrayList)aaSeq[j]; 

                    if (((MSequence)aSeq[i]).SequenceID ==
                        ((MSequence)aSeqBase[i]).SequenceID)
                        continue;

                    return false;
                }                       
            }

            return true;
        }

        public static bool AreAllChainsCurrentlyEmpty(ArrayList aaSeq)
        {
            // [1] Ensure that every sequence list is at least size 2
            ArrayList aSeqBase = (ArrayList)aaSeq[0]; 
            int iChainLength = aSeqBase.Count;

            if (iChainLength > 2)
            {
                return false;
            }
            
            for (int i = 1; i < aaSeq.Count; i++)
            {
                ArrayList aSeq = (ArrayList)aaSeq[i]; 
                int iLength = aSeq.Count;

                if (iLength > 2)
                {
                    return false;
                }
            }

            return true;
        }

        public static bool DoChainsHaveAnyCommonSequences(ArrayList aaSeq,
            ref int iIndexFromEnd)
        {
            iIndexFromEnd = -1;

            // [1] Ensure that every sequence list is at least size 2
            ArrayList aSeqBase = (ArrayList)aaSeq[0]; 
            int iChainLength = aSeqBase.Count;

            if (iChainLength <= 2)
            {
                return false;
            }
            
            for (int i = 1; i < aaSeq.Count; i++)
            {
                ArrayList aSeq = (ArrayList)aaSeq[i]; 
                int iLength = aSeq.Count;

                if (iLength <= 2)
                {
                    return false;
                }
            }

            // We know that each chain has at least 1 intermediate
            // Now we must compare to see that the last intermediates
            // are the same, if they are - then we some common ground
            for (int j=1; j<aaSeq.Count; j++)
            {
                ArrayList aSeq = (ArrayList)aaSeq[j];             

                if (((MSequence)aSeq[aSeq.Count-2]).SequenceID ==
                    ((MSequence)aSeqBase[iChainLength-2]).SequenceID)
                    continue;

                return false;
            }                       

            // We know we have common ground, find first.
            //uiStartIndex = iChainLength-2;
            iIndexFromEnd = 2;
            int iTestIndexFromEnd = 2;

            //for (int i=iChainLength-2; i>1; i--)
            for (int i=iChainLength-2; i>0; i--)
            {
                // From our based downward, find as much common as possible
                for (int j=1; j<aaSeq.Count; j++)
                {
                    ArrayList aSeq = (ArrayList)aaSeq[j];             

                    if (iTestIndexFromEnd > aSeq.Count - 1)
                        return true; // finished...

                    //if (iTestIndexFromEnd > aSeq.Count - 2)
                    //    return true; // finished...

                    if (((MSequence)aSeq[aSeq.Count - 
                                         iTestIndexFromEnd]).SequenceID ==
                        ((MSequence)aSeqBase[iChainLength - 
                                             iTestIndexFromEnd]).SequenceID)
                    {
                        if (j == aaSeq.Count-1)
                        {
                            iIndexFromEnd = iTestIndexFromEnd;
                            iTestIndexFromEnd++;
                        }

                        continue;
                    }

                    //Not in common anymore. Decrement to remove last candidate
                    return true;
                }                       
            }

            return true;
        }

        public static string GetListBoxEntryString(
            string strName, float fDuration)
        {
            string strEntry = strName+ " (";

            if (fDuration == MTransition.MChainInfo.MAX_DURATION)
            {
                strEntry += "End";
            }
            else if (fDuration == -2.0f)
            {
                strEntry += "DURATIONS DIFFER";
            }
            else
            {
                strEntry += fDuration.ToString("f3");
            }
            strEntry += ")";

            return strEntry;
        }

        public static ArrayList GetDurationsOfCommonSequences(
            ArrayList aTran, int iIndexFromEnd)
        {
            // will returns strings of durations values

            // Should need aaSeq...
            ArrayList aaSeq = Convert_aTran_To_aaSeq(aTran);
            ArrayList aaDur = Convert_aTran_To_aaDur(aTran);

            ArrayList aSeqBase = (ArrayList)aaSeq[0]; 
            ArrayList aDurBase = (ArrayList)aaDur[0];
            int iChainLengthBase = aSeqBase.Count;
        
            Debug.Assert(iChainLengthBase > iIndexFromEnd);

            ArrayList aDurs = new ArrayList(iIndexFromEnd);

            for (int i=iIndexFromEnd; i>1; i--)
            {
                int iStartIndexBase = iChainLengthBase /*- 1*/ - i;
                float fBaseDur = (float)aDurBase[iStartIndexBase];

                aDurs.Add(fBaseDur);

                // For every other transition at this index.
                for(int j=1; j<aaDur.Count; j++)
                {
                    ArrayList aDurCurr = (ArrayList)(aaDur[j]);
                    int iStartIndexCurr = aDurCurr.Count /*- 1*/ - i;

                    float fCurrDur = (float)(aDurCurr[iStartIndexCurr]);

                    if (fBaseDur != fCurrDur)
                    {
                        // -2.0 indicates  sequences are different
                        aDurs[iIndexFromEnd - i] = -2.0f;
                        break;
                    }
                }
            }

            return aDurs;
        }

        public static ArrayList GetCommonChainSequencesWithDurations(
            ArrayList aTran)
        {
            ArrayList aaSeq = Convert_aTran_To_aaSeq(aTran);

            Debug.Assert(aaSeq.Count > 1, "Used for multiple selection only.");
            
            // Check for commonality, make a string list.
            bool bHasExactCommonSeqs = 
                AnimationHelpers.DoChainsHaveExactCommonSequences(aaSeq);

            ArrayList aSeqBase = (ArrayList)aaSeq[0]; 
            int iChainLength = aSeqBase.Count;

            if (bHasExactCommonSeqs == true)
            {
                // from first index...
                ArrayList aDurs = GetDurationsOfCommonSequences(aTran, 
                    iChainLength - 1); 

                // validation was completed... build string
                ArrayList al = new ArrayList(iChainLength);

                // exact same, so we can just perform as if single.
                MTransition tran = (MTransition)aTran[0];
                int iTran = 0;
                foreach (MTransition.MChainInfo chain in tran.ChainInfo)
                {
                    MSequence kSequence = MFramework.Instance.Animation
                        .GetSequence(chain.SequenceID);

                    al.Add(GetListBoxEntryString(kSequence.Name,
                        (float)(aDurs[iTran++])));
                }

                return al;
            }

            int iIndexFromEnd = 0;
            bool bHasAnyCommonSeqs = 
                AnimationHelpers.DoChainsHaveAnyCommonSequences(aaSeq, 
                ref iIndexFromEnd);

            if (bHasAnyCommonSeqs == true)
            {
                ArrayList aDurs = GetDurationsOfCommonSequences(aTran, 
                    iIndexFromEnd); 

                int iStartIndex = iChainLength - iIndexFromEnd;

                // Need to find all the final all the final
                // intermediates that are in common.

                // validation was completed... build string
                ArrayList al = new ArrayList(iIndexFromEnd + 1);

                al.Add("<INTERMEDIATES DIFFER>");

                int iTran=0;
                for( int i=iStartIndex; i < aSeqBase.Count -1; i++)
                {
                    al.Add(GetListBoxEntryString(((MSequence)aSeqBase[i]).Name,
                        (float)(aDurs[iTran++])));
                }

                return al;
            }

            // If nothing in common was found, return nothing in common...
            ArrayList aNoSeq = new ArrayList(1);

            if (AreAllChainsCurrentlyEmpty(aaSeq))
                aNoSeq.Add("");
            else
                aNoSeq.Add("<INTERMEDIATES DIFFER>");

            return aNoSeq;
        }

        public static ArrayList GetCommonChainSequences(ArrayList aaSeq)
        {
            Debug.Assert(aaSeq.Count > 1, "Used for multiple selection only.");
            
            // Check for commonality, make a string list.
            bool bHasExactCommonSeqs = 
                AnimationHelpers.DoChainsHaveExactCommonSequences(aaSeq);

            ArrayList aSeqBase = (ArrayList)aaSeq[0]; 
            int iChainLength = aSeqBase.Count;

            if (bHasExactCommonSeqs == true)
            {
                // validation was completed... build string
                ArrayList al = new ArrayList(iChainLength);

                for( int i=1; i < aSeqBase.Count -1; i++)
                {
                    al.Add(((MSequence)aSeqBase[i]).Name);
                }

                return al;
            }

            int iIndexFromEnd = 0;
            bool bHasAnyCommonSeqs = 
                AnimationHelpers.DoChainsHaveAnyCommonSequences(aaSeq, 
                ref iIndexFromEnd);

            if (bHasAnyCommonSeqs == true)
            {
                int iStartIndex = iChainLength - iIndexFromEnd;

                // Need to find all the final all the final
                // intermediates that are in common.

                // validation was completed... build string
                ArrayList al = new ArrayList(iIndexFromEnd + 1);

                al.Add("<INTERMEDIATES DIFFER>");

                for( int i=iStartIndex; i < aSeqBase.Count -1; i++)
                {
                    al.Add(((MSequence)aSeqBase[i]).Name);
                }

                return al;
            }

            // If nothing in common was found, return nothing in common...
            ArrayList aNoSeq = new ArrayList(1);

            if (AreAllChainsCurrentlyEmpty(aaSeq))
                aNoSeq.Add("");
            else
                aNoSeq.Add("<INTERMEDIATES DIFFER>");

            return aNoSeq;
        }

        public static bool DoTransitionsExist()
        {
            foreach (MSequence seq in 
                MFramework.Instance.Animation.Sequences)
            {
                MTransition[] at = seq.Transitions;

                if (at == null)
                    continue;

                if (at.Length > 0)
                    return true;
            }

            return false;
        }

        #endregion

        #region Sequence Helpers
        
        public static bool DoSequencesExist()
        {
            ArrayList alSeq = MFramework.Instance.Animation.Sequences;

            if (alSeq == null || alSeq.Count < 1)
                return false;

            return true;
        }


        public static void CorrectMissingTextAndMorphTags(ref string strErrors)
        {
            MSharedData.Instance.Lock();
            ArrayList kTransitionsToRemove = new ArrayList();
            MAnimation kAnim = MFramework.Instance.Animation;

            // Remove all invalid Morphs, Delayed Blends, and Immediate
            // Blends due to missing text keys
            if (kAnim.Sequences != null)
            {
                foreach (MSequence kSequence in kAnim.Sequences)
                {
                    if (kSequence.Transitions == null)
                        continue;

                    foreach (MTransition kTrans in kSequence.Transitions)
                    {
                        if (kTrans.Type == 
                            MTransition.TransitionType.Trans_DelayedBlend)
                        {
                            // Go through all blend pairs
                            // If one uses a text key that does not exist,
                            // add the transition to the removal list.
                            bool bRemove = false;
                            if (kTrans.BlendPairs == null)
                            {
                                kTransitionsToRemove.Add(kTrans);
                                continue;
                            }

                            foreach (MTransition.MBlendPair kPair 
                                in kTrans.BlendPairs)
                            {
                                if (!DoesTextKeyExist(kTrans.Source,
                                                      kPair.StartKey) ||
                                    !DoesTextKeyExist(kTrans.Destination,
                                                      kPair.TargetKey))
                                {
                                    bRemove = true;
                                    break;
                                }
                            }

                            if (bRemove)
                            {
                                kTransitionsToRemove.Add(kTrans);
                            }
                        }
                        else if (kTrans.Type == 
                                 MTransition.TransitionType.Trans_Morph)
                        {
                            if (false ==
                                kTrans.Source.CanSyncTo(kTrans.Destination))
                            {
                                kTransitionsToRemove.Add(kTrans);
                            }
                        }
                        else if (kTrans.Type == 
                                 MTransition.TransitionType.
                                     Trans_ImmediateBlend)
                        {
                            if (kTrans.ImmediateOffsetTextKey != null && 
                                !DoesTextKeyExist(kTrans.Destination, 
                                kTrans.ImmediateOffsetTextKey))
                            {
                                kTransitionsToRemove.Add(kTrans);
                            }
                        }
                    }
                }
            }

            if (kTransitionsToRemove.Count != 0)
            {
                ArrayList kDepChainsAsStrings = new ArrayList();
                // Find any dependent chains so that we can warn
                // the user that we will be deleting them as well
                foreach (MTransition kTrans in kTransitionsToRemove)
                {
                    ArrayList kDepChainsList = GetChainDependencies(kTrans);
                    if (kDepChainsList.Count != 0)
                    {
                        foreach (MTransition kDepChain in kDepChainsList)
                        {
                            kDepChainsAsStrings.Add(
                                "Chain \"" + kDepChain.Source + "\" to \"" + 
                                kDepChain.Destination + 
                                "\" is dependent on the transition \"" +
                                kTrans.Source + "\" to \"" + 
                                kTrans.Destination + "\"");
                        }
                    }
                }

                strErrors += 
                    "Required text or morph keys were not found for the " +
                    "following transitions:\\par(These transitions will "
                    + "be removed)\\par";

                ArrayList kTransitionsAsStrings = new ArrayList();
                foreach (MTransition kTrans in kTransitionsToRemove)
                {
                    kTransitionsAsStrings.Add( "\"" + kTrans.Source + 
                        "\" to \"" + kTrans.Destination + "\"");
                    kAnim.RemoveTransition(kTrans.SrcID, kTrans.DesID);
                }

                strErrors += MessageBoxManager.CreateBulletedList(
                    kTransitionsAsStrings)+ "\\par\n\\par\n";    

                kTransitionsToRemove.Clear();

                if (kDepChainsAsStrings.Count != 0)
                {
                    strErrors += "The following chain transitions were "+
                        "dependent on deleted transitions:\\par(These "+
                        "chain transitions will be removed)\\par";
                    strErrors += 
                        MessageBoxManager.CreateBulletedList(
                        kDepChainsAsStrings) + "\\par\n\\par\n"; 
                }
            }


            // Find all SequenceGroups that have synchronized sequences
            // If they can no longer sync, reset their sync to none.
            
            ArrayList kResetSeqInGroup = new ArrayList();
            if (kAnim.SequenceGroups != null)
            {
                foreach (MSequenceGroup kGroup in kAnim.SequenceGroups)
                {
                    if (kGroup.SequenceInfo == null)
                        continue;

                    foreach (MSequenceGroup.MSequenceInfo kSeqInfo
                        in kGroup.SequenceInfo)
                    {
                        if (kSeqInfo.Synchronized == true )
                        {
                            MSequence kSyncSeq = kAnim.GetSequence(
                                    kSeqInfo.SynchronizeToSequenceID);
                            if (kSyncSeq == null || 
                                !kSeqInfo.Sequence.CanSyncTo(kSyncSeq))
                            {                
                                kResetSeqInGroup.Add("Group: \"" + 
                                    kGroup.Name + "\"  Sequence: \""
                                    + kSeqInfo.Sequence.Name + "\"");
                                kSeqInfo.ResetSyncSequence();
                            }
                        }
                    }
                }
            }


            if (kResetSeqInGroup.Count != 0)
            {
                strErrors += "Required text or morph keys " +
                    "were not found for the " +
                    "following synchronized sequences in sequence " + 
                    "groups:\\par(These sequences will be no longer " + 
                    "be synchronized)\\par";

                strErrors += MessageBoxManager.CreateBulletedList(
                    kResetSeqInGroup) + "\\par\n\\par\n";    

                kResetSeqInGroup.Clear();
            }

            MSharedData.Instance.Unlock();
        }

        private static bool DoesTextKeyExist(
            MSequence kSequence, string strKey)
        {
            Debug.Assert(strKey != null);
            foreach (MSequence.MTextKey kKey in kSequence.TextKeys)
            {
                if (kKey.Text == strKey)
                    return true;
            }

            return false;
        }

        #endregion

        #region Model Helpers

        public static bool OpenModelFile(string strFilename,
            System.Windows.Forms.Form kForm)
        {
            string strDefaultDir = AppUtils.ExtractDirFromPath(
                strFilename);
            if (MFramework.Instance.Animation.BaseKFMPath == String.Empty)
            {
                MFramework.Instance.Animation.BaseKFMPath = strDefaultDir;
            }

            if (!MFramework.Instance.Animation.SetModelPathAndRoot(
                strFilename, null))
            {
                string[] astrVariables = new string[]
                        {
                            MessageBoxManager.ConvertFilename(
                            strFilename),
                            MFramework.Instance.Animation.GetLastErrorString()
                        };
                MessageBoxManager.DoMessageBox("LoadKFError.rtf",
                    "NIF File Load Error", MessageBoxButtons.OK,
                    MessageBoxIcon.Error, astrVariables);
                return false;
            }
            MFramework.Instance.Renderer.CreateFrame();
            return DoModelRootDialog(kForm);
        }

        public static bool OpenModelDialog(
            System.Windows.Forms.Form kForm, bool bLoadFileOnOK,
            ref string strFilename, ref bool bCancel)
        {
            System.Windows.Forms.OpenFileDialog ofdNIFFile =
                new System.Windows.Forms.OpenFileDialog();
            ofdNIFFile.DefaultExt = "NIF";
            ofdNIFFile.Filter = "Gamebryo NIF files|*.NIF";
            ofdNIFFile.Title = "Open Character NIF";
            ofdNIFFile.FileName = strFilename;
            
            DialogResult dr = ofdNIFFile.ShowDialog(kForm);
            if (dr == DialogResult.OK)
            {
                strFilename = ofdNIFFile.FileName;
                    
                if (bLoadFileOnOK)
                {
                    return OpenModelFile(ofdNIFFile.FileName, kForm);
                }
                else
                {
                    return true;
                }
            }
            else if (dr == DialogResult.Cancel)
            {
                bCancel = true;
                return false;
            }

            return false;
        }

        public static bool OpenModelDialog(
            System.Windows.Forms.Form kForm)
        {
            string strFilename = null;
            bool bCancel = false;
            return OpenModelDialog(kForm, true,
                ref strFilename, ref bCancel);
        }

        public static bool GetModelAndRootNames(
            System.Windows.Forms.Form kParentForm,
            ref string strModelPath, ref string strModelRoot,
            ref bool bCancel)
        {
            if (!OpenModelDialog(
                     kParentForm, false, ref strModelPath, ref bCancel))
                return false;

            uint uiIndex = MSceneGraph.Instance.LoadAndAddNIF(strModelPath);
            if (uiIndex == MSceneGraph.INVALID_INDEX)
                return false;

            MSceneObject kScene = MSceneGraph.Instance.GetScene(uiIndex);
            bool bSuccess = DoModelRootDialog(kParentForm,
                kScene, strModelPath, ref strModelRoot, ref bCancel);

            MSceneGraph.Instance.RemoveScene(uiIndex);
            return bSuccess;
        }

        public static bool DoModelRootDialog(
            System.Windows.Forms.Form kParentForm,
            MSceneObject kScene, string strFullModelPath,
            ref string strModelRoot, ref bool bCancel)
        {
            CharacterEditForm kForm = new CharacterEditForm(strFullModelPath, 
                kScene, strModelRoot);
            DialogResult dr = kForm.ShowDialog(kParentForm);
            if (dr == DialogResult.OK)
            {
                strModelRoot = kForm.ModelRoot;
                return true;
            }
            else if (dr == DialogResult.Cancel)
            {
                bCancel = true;
                return false;
            }

            return false;
        }

        public static bool DoModelRootDialog(
            System.Windows.Forms.Form kParentForm)
        {
            string strFullPath = null;
            // Update model path text box contents.
            if (MFramework.Instance.Animation.FullModelPath == null ||
                MFramework.Instance.Animation.FullModelPath == string.Empty)
            {
                strFullPath = "<UNDEFINED>";
            }
            else
            {
                strFullPath = MFramework.Instance.Animation.FullModelPath;
            }

            MSceneObject kScene = MSceneGraph.Instance.GetScene(
                MSceneGraph.Instance.CharacterIndex);
            CharacterEditForm kForm = new CharacterEditForm(strFullPath, 
                kScene, MFramework.Instance.Animation.ModelRoot);
            DialogResult dr = kForm.ShowDialog(kParentForm);
            
            if (dr == DialogResult.OK)
            {
                if (MFramework.Instance.Animation.ModelRoot == kForm.ModelRoot)
                {
                    return true;
                }
                else 
                {
                    string OldModelRoot = 
                        MFramework.Instance.Animation.ModelRoot;
                    MFramework.Instance.Animation.SetModelPathAndRoot(
                        MFramework.Instance.Animation.ModelPath,
                        kForm.ModelRoot);
                    ArrayList kConflicts = 
                        MFramework.Instance.Animation.GetModelRootConflicts();
                    if (kConflicts.Count != 0)
                    {
                        string [] strVariables = new string[2];
                        strVariables[0] = kForm.ModelRoot;
                        strVariables[1] = 
                            MessageBoxManager.CreateBulletedList(kConflicts);
                    
                        DialogResult kYesNoResult = MessageBoxManager
                            .DoMessageBox("RootInterpolatorMismatches.rtf", 
                            "Change Model Root Warning",
                            MessageBoxButtons.YesNo,
                            MessageBoxIcon.Question, strVariables);

                        if (kYesNoResult == DialogResult.No)
                        {
                            MFramework.Instance.Animation.SetModelPathAndRoot(
                                MFramework.Instance.Animation.ModelPath,
                                OldModelRoot);
                        }
                    }
                }
            }
            return true;
        }

        #endregion

    }
}
