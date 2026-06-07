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
using System.Reflection;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs
{
    /// <summary>
    /// Summary description for UICommandUtility.
    /// </summary>
    internal sealed class UICommandUtility
    {
        //cached services
        private static IUICommandService m_commandService;
        private static IMessageService m_messageService;
        private static readonly string m_bindErrorFormatString = @"Could not bind:
Command: {0}
Method: {1}.{2}
Check the method's signature";


        private UICommandUtility()
        {
        }

        private static IUICommandService CommandService
        {
            get
            {
                if (m_commandService == null)
                {
                    m_commandService = 
                        ServiceProvider.Instance.GetService(
                        typeof(IUICommandService))
                        as IUICommandService;
                }
                return m_commandService;
            }
        }

        private static IMessageService MessageService
        {
            get
            {
                if (m_messageService == null)
                {
                    m_messageService =
                        ServiceProvider.Instance.GetService(
                        typeof(IMessageService))
                        as IMessageService;
                }
                return m_messageService;
            }
        }

        public static void Init()
        {
        }

        public static void AttachMethodToCommand(
            string commandName, MethodInfo method, object target)
        {
            UICommand command = 
                CommandService.GetCommand(commandName);
            try
            {
                Delegate del = null;
                if (target == null)
                {
                    del = Delegate.CreateDelegate(typeof(EventHandler),
                        method);
                }
                else
                {
                    del = Delegate.CreateDelegate(typeof(EventHandler),
                        target, method.Name);
                }
                command.Click += (EventHandler) del;
                if (!command.CheckParameters)
                {
                    //Check for Parameters and set UICommand flag
                    object[] attributeInfo = method.GetCustomAttributes(
                        typeof(RequiresParameterAttribute), false);
                    if (attributeInfo.Length > 0)
                    {
                        command.CheckParameters = true;
                    }
                }
                
            }
            catch (ArgumentException)
            {
                Message message = new Message();
                message.m_strText = "Could not bind command";
                //build details string
                message.m_strDetails = 
                    string.Format(m_bindErrorFormatString, 
                    commandName, method.DeclaringType, method.Name);

                MessageService.AddMessage(MessageChannelType.Errors,
                    message);
            }
        }

        public static void AttachValidatorToCommand(
            string commandName, MethodInfo method, object target)
        {
            UICommand command = 
                CommandService.GetCommand(commandName);
            try
            {
                Delegate del = null;
                if (target == null)
                {
                    del = Delegate.CreateDelegate(typeof(
                        UICommand.ValidateCommandHandler), method);
                }
                else
                {
                    del = Delegate.CreateDelegate(typeof(
                        UICommand.ValidateCommandHandler), target,
                        method.Name);
                }
                command.Validate += (UICommand.ValidateCommandHandler) del;
            }
            catch (ArgumentException)
            {
                Message message = new Message();
                message.m_strText = "Could not bind command validator";
                //build details string
                message.m_strDetails = 
                    string.Format(m_bindErrorFormatString, 
                    commandName, method.DeclaringType, method.Name);

                MessageService.AddMessage(MessageChannelType.Errors,
                    message);
            }

        }


        public static void BindMethodsOnType(Type type, 
            BindingFlags bindingFlags, object target)
        {
            MethodInfo[] methods = type.GetMethods(
                bindingFlags);
            foreach(MethodInfo method in methods)
            {
                //Attach Handlers
                object[] methodAttributes = 
                    method.GetCustomAttributes(
                    typeof(UICommandHandlerAttribute), false);
                if (methodAttributes.Length > 0)
                {
                    UICommandHandlerAttribute ha = 
                        methodAttributes[0] 
                        as UICommandHandlerAttribute;
                    AttachMethodToCommand(ha.Name, method, target);
                }
                //Attach Validators
                methodAttributes =
                    method.GetCustomAttributes(
                    typeof(UICommandValidatorAttribute), false);
                foreach(UICommandValidatorAttribute va 
                            in methodAttributes)
                {
                    AttachValidatorToCommand(va.Name, method, target);
                }
            }
        }

    }
}
