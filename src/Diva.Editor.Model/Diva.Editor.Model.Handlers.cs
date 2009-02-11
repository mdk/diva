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

namespace Diva.Editor.Model {
        
        // Delegates ///////////////////////////////////////////////////////////
        
        public delegate void StuffHandler (object o, StuffArgs args);
        
        public delegate void TagHandler (object o, TagArgs args);
        
        public delegate void ExceptionHandler (object o, ExceptionArgs args);
        
        public delegate void RequisitionHandler (object o, RequisitionArgs args);
        
        public delegate void OrganizeByMethodHandler (object o, OrganizeByMethodArgs args);
        
        public delegate void GtkModelHandler (object o, GtkModelArgs args);
        
        public delegate void TimeHandler (object o, TimeArgs args);
        
        public delegate void TimeSpanHandler (object o, TimeSpanArgs args);
        
        public delegate void ToolSetToolHandler (object o, ToolSetToolArgs args);
        
        public delegate void ProgressHandler (object o, ProgressArgs args);
        
        public delegate void UndoRedoHandler (object o, UndoRedoArgs args);
        
        public delegate void NodeHandler (object o, NodeArgs args);
        
        public delegate void QuitHandler (object o, QuitArgs args);
        
        public delegate void PipelineStateHandler (object o, PipelineStateArgs args);
        
        public delegate void PipelineTickerHandler (object o, PipelineTickerArgs args);

        public delegate void PipelineErrorHandler (object o, PipelineErrorArgs args);
        
        public delegate void RectangleHandler (object o, RectangleArgs args);
        
        public delegate void CursorHandler (object o, CursorArgs args);

        public delegate void MessageHandler (object o, MessageArgs args);

        public delegate void SaveNagActionHandler (object o, SaveNagActionArgs args);
        
        public delegate void PipelineVolumeHandler (object o, PipelineVolumeArgs args);

}
       
