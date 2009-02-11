////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// MIT X11 license, Copyright (c) 2005-2006 by:                               //
//                                                                            //
// Authors:                                                                   //
//      Michael Dominic K. <michaldominik@gmail.com>                          //
//                                                                            //
// Permission is hereby granted, free of charge, to any person obtaining a    //
// copy of this software and associated documentation files (the "Software"), //
// to deal in the Software without restriction, including without limitation  //
// the rights to use, copy, modify, merge, publish, distribute, sublicense,   //
// and/or sell copies of the Software, and to permit persons to whom the      //
// Software is furnished to do so, subject to the following conditions:       //
//                                                                            //
// The above copyright notice and this permission notice shall be included    //
// in all copies or substantial portions of the Software.                     //
//                                                                            //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS    //
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF                 //
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN  //
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   //
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR      //
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  //
// USE OR OTHER DEALINGS IN THE SOFTWARE.                                     //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

namespace Diva.Commands {

        using System;
        using Core;
        using System.Collections.Generic;
        using Basics;

        public class MultiCommand : ICommand, IUndoableCommand, IPreparableCommand,
                                    IBoilable, IMessagingCommand {

                // Fields //////////////////////////////////////////////////////

                List <IUndoableCommand> commands; // The inner commands we're wrapping
                string msg;                       // The message FIXME: It's not translatable over SAVE!!!
                string instantMsg;                // The instant message. FIXME: It's not translatable over SAVE!!!
                bool reversed;                    // We reverse the LIST instead of copying it all over

                // Properties //////////////////////////////////////////////////
                
                public string Message {
                        get { return msg; }
                }

                public string InstantMessage {
                        get { return instantMsg; }
                }
                
                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public MultiCommand (string msg, string instantMsg)
                {
                        commands = new List <IUndoableCommand> ();
                        this.msg = msg;
                        this.instantMsg = instantMsg;
                        reversed = false;
                }

                /* CONSTRUCTOR */
                public MultiCommand (ObjectContainer container, IBoilProvider provider)
                {
                        commands = new List <IUndoableCommand> ();

                        foreach (RefParameter reff in container.FindAllByName ("command"))
                                commands.Add ((IUndoableCommand) reff.ToObject (provider));

                        reversed = container.FindBool ("reversed").Value;
                        msg = container.FindString ("message").Value;                        
                }

                /* Add a command to the set */
                public void Add (IUndoableCommand cmd)
                {
                        commands.Add (cmd);
                }

                public void Prepare (Project project)
                {
                        foreach (ICommand cmd in commands)
                                if (cmd is IPreparableCommand)
                                        (cmd as IPreparableCommand).Prepare (project);
                }

                public void DoAction (Project project)
                {
                        if (reversed) {
                                commands.Reverse ();
                                reversed = false;
                        }
                        
                        foreach (ICommand cmd in commands)
                                cmd.DoAction (project);
                }

                public void UndoAction (Project project)
                {
                        if (! reversed) {
                                commands.Reverse ();
                                reversed = true;
                        }
                        
                        foreach (IUndoableCommand cmd in commands)
                                cmd.UndoAction (project);
                }
                
                public List <object> GetDepObjects ()
                {
                        List <object> lst = new List <object> ();
                        
                        foreach (object o in commands)
                                lst.Add (o);
                        
                        return lst;
                }

                public void Boil (ObjectContainer container, IBoilProvider provider)
                {
                        foreach (object o in commands)
                                container.Add (new RefParameter ("command", o, provider));

                        container.Add (new BoolParameter ("reversed", reversed));
                        container.Add (new StringParameter ("message", msg));                        
                }
                
        }

}

