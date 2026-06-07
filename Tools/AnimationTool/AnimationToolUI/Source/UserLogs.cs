// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
// 
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
// 
// Copyright (c) 1996-2008 Emergent Game Technologies.
// All Rights Reserved.
// 
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

using System;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for UserLogs.
    /// </summary>
    public class UserLogs
    {
        public enum LogType : int
        {
            FILE_NEW = 0,
            FILE_OPEN,
            FILE_RECENT,
            FILE_SAVE,
            FILE_SAVEAS,

            EDIT_SAVE_DOCK,
            EDIT_RESET_DOCK,
            EDIT_RESET_MESSAGE_BOXES,
            EDIT_SCENE_OPTIONS,
            EDIT_TRANSITION_TABLE_SETTINGS,

            VIEW_PROJECT_VIEW,
            VIEW_EDIT_VIEW,
            VIEW_RENDER_VIEW,
            VIEW_TRANSITIONS_VIEW,
            VIEW_OUTPUT_VIEW,
            VIEW_PHYSICS_VIEW,
            VIEW_INTERACTIVE,

            HELP_CONTENTS,
            HELP_INDEX,
            HELP_SEARCH,
            HELP_ABOUT,

            ICON_NEW,
            ICON_OPEN,
            ICON_SAVE,
            ICON_ADDSEQ,
            ICON_PROJECT_VIEW,
            ICON_EDIT_VIEW,
            ICON_RENDER_VIEW,
            ICON_TRANSITIONS_VIEW,
            ICON_OUTPUT_VIEW,
            ICON_PHYSICS_VIEW,
            ICON_INTERACTIVE,

            KFM_DLG_OK,
            KFM_DLG_CANCEL,

            PROJECTVIEW_CHARACTER,
            PROJECTVIEW_SEQUENCE,
            PROJECTVIEW_SEQUENCEGRP,
            PROJECTVIEW_SEQUENCE_FOLDER,
            PROJECTVIEW_SEQUENCEGRP_FOLDER,
            PROJECTVIEW_SEQID,
            PROJECTVIEW_SEQGRPID,
            PROJECTVIEW_INTERACTIVE,
            PROJECTVIEW_TRANSITIONS,

            PROJECTVIEW_DELETE_FOLDER,
            PROJECTVIEW_DELETE_SEQUENCE,
            PROJECTVIEW_DELETE_SEQUENCE_GROUP,
        };

        public static string[] m_astrLogs = 
        {
            "File->New",
            "File->Open",
            "File->Recent Files->",
            "File->Save",
            "File->Save As",

            "Edit->Save Dock Configuration",
            "Edit->Reset Dock Configuration",
            "Edit->Reset Message Boxes",
            "Edit->Scene Options...",
            "Edit->Transition Table Settings...",

            "View->Project View",
            "View->Edit View",
            "View->Render View",
            "View->Transitions View",
            "View->Output View",
            "View->Physics View",
            "View->Interactive Preview",

            "Help->Contents",
            "Help->Index",
            "Help->Search",
            "Help->About Animation Tool",

            "Icon: New KFM",
            "Icon: Open KFM",
            "Icon: Save KFM",
            "Icon: Add One or More Sequences",
            "Icon: Toggle Project View",
            "Icon: Toggle Edit View",
            "Icon: Toggle Render View",
            "Icon: Toggle Transitions View",
            "Icon: Toggle Output View",
            "Icon: Toggle Physics View",
            "Icon: Interactive Preview",
    
            "   KFM = ",
            "   Cancel",

            "ProjView: Character Model",
            "ProjView: Sequence = ",
            "ProjView: Sequence Group = ",
            "ProjView: Seq Folder = ",
            "ProjView: SeqGrp Folder = ",
            "ProjView: Sequence ID Overview",
            "ProjView: Sequence Group ID Overview",
            "ProjView: Interactive Preview",
            "ProjView: Transitions Preview",

            "ProjView: Delete Folder = ",
            "ProjView: Delete Sequence = ",
            "ProjView: Delete Sequence Group = "
        };

        public static string GetLogString(LogType eLogType)
        {
            return m_astrLogs[(int)eLogType];
        }
    }
}
