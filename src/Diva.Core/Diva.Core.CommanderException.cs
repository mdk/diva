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

namespace Diva.Core {

        using System;

        public class CommanderException : Exception {

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public CommanderException (string str) : base (str)
                {
                }

                /* CONSTRUCTOR */
                public CommanderException (string str, Exception excp) : base (str, excp)
                {
                }

                public static CommanderException PrepareStage (object o, Exception excp)
                {
                        return new CommanderException (String.Format ("Prepare stage failed for {0}", o), excp);
                }

                public static CommanderException CommandExecution (object o, Exception excp)
                {
                        return new CommanderException (String.Format ("Command {0} execution failed", o), excp);
                }

                public static CommanderException DoActionStage (object o, Exception excp)
                {
                        return new CommanderException (String.Format ("DoAction stage failed for {0}", o), excp);
                }

                public static CommanderException UndoActionStage (object o, Exception excp)
                {
                        return new CommanderException (String.Format ("UndoAction stage failed for {0}", o), excp);
                }

                public static CommanderException QueryStage (object o, Exception excp)
                {
                        return new CommanderException (String.Format ("Query stage failed for {0}", o), excp);
                }

                public static CommanderException NotUndoableStage (object o, Exception excp)
                {
                        return new CommanderException (String.Format ("NotUndoable stage failed for {0}", o), excp);
                }

                public static CommanderException PostUndoableStage (object o, Exception excp)
                {
                        return new CommanderException (String.Format ("PostUndoable stage failed for {0}", o), excp);
                }
                        

        }

}
                   
