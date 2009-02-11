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

namespace Diva.Util {

        using Gdk;
        using Gtk;
        using System;

        public static class GtkFu {
                
                // Public methods //////////////////////////////////////////////
                
                /* Now, I have no *fcking clue why Gtk.Window.ParseGeometry doesn't 
                 * work like expected. Franky, I don't care */
                public static void ParseGeometry (Gtk.Window win, string str)
                {
                        int x = -1;
                        int y = -1;
                        int width = -1;
                        int height = -1;
                        
                        try { 
                                string[] matches = str.Split ('x', '+');
                                
                        if (matches.Length == 0)
                                        throw new Exception ();
                                
                                if (matches.Length == 1)
                                        throw new Exception ();
                                
                                width = Convert.ToInt32 (matches[0]);
                                height = Convert.ToInt32 (matches[1]);
                                
                                if (matches.Length > 2)
                                        x = Convert.ToInt32 (matches [2]);
                                
                                if (matches.Length > 3)
                                        y = Convert.ToInt32 (matches [3]);
                        } catch {
                        } finally {
                                if (width != -1 && height != -1) 
                                        win.Resize (width, height);
                                if (x != -1 && y != -1)
                                        win.Move (x, y);
                        }
                }
                
                /* Convert x, y, widdth, height into a X-geometry like string */
                public static string GetGeometry (Gtk.Window win)
                {
                        int width, height, x, y;
                        win.GetSize (out width, out height);
                        win.GetPosition (out x, out y);
                        
                        return String.Format ("{0}x{1}+{2}+{3}", width, height, x, y);
                }
                

                /* Find the iter that represents the given object in a model */
                public static TreeIter TreeModelIterByObject (TreeModel model, object searched, int clmn)
                {
                        TreeIter iter;
                        model.GetIterFirst (out iter);
                        
                        do {
                                object currentObject = model.GetValue (iter, clmn);
                                if (currentObject == searched)
                                        // Success
                                        return iter;
                        } while (model.IterNext (ref iter));
                        
                        // Failure
                        return TreeIter.Zero;
                }
                
                /* Find the iter that represents the given int in a model */
                public static TreeIter TreeModelIterByInt (TreeModel model, int searched, int clmn)
                {
                        TreeIter iter;
                        model.GetIterFirst (out iter);
                        
                        do {
                                int current = (int) model.GetValue (iter, clmn);
                                if (current == searched)
                                        // Success
                                        return iter;
                        } while (model.IterNext (ref iter));
                        
                        // Failure
                        return TreeIter.Zero;
                }

                /* If a given span intersects with another one */
                public static bool LinearIntersectsWith (int s1, int e1, int s2, int e2)
                {
                        if (s1 == e1 || s2 == e2)
                                return false;
                        
                        // equals
                        if (s1 == s2 &&
                            e1 == e2)
                                return true;
        
                        // contained in
                        if (s1 < s2 &&
                            e1 > e2)
                                return true;

                        if (s2 < s1 &&
                            e2 > e1)
                                return true;
        
                        // start or end in
                        if (s2 >= s1 &&
                            s2 <  e1)
                                return true;
        
                        if (e2 > s1 &&
                            e2 < e1)
                                return true;
        
                        if (s1 >= s2 &&
                            s1 <  e2)
                                return true;
                        
                        if (e1 > s2 &&
                            e1 < e2)
                                return true;
                        
                        return false;
                }
                
                /* Get a linear intersection of two regions */
                public static void LinearGetIntersection (int s1, int e1, int s2, int e2, out int s3, out int e3)
                {
                        // equals
                        if (s1 == s2 &&
                            e1 == e2) {
                                s3 = s1;
                                e3 = e1;
                                return;
                        }

                        // contained in
                        if (s1 < s2 &&
                            e1 > e2) {
                                s3 = s2;
                                e3 = e2;
                                return;
                        }
        
                        if (s2 < s1 &&
                            e2 > e1) {
                                s3 = s1;
                                e3 = e1;
                                return;
                        }
        
                        // start or end in
                        if (s2 >= s1 &&
                            s2 <  e1) {
                                s3 = s2;
                                e3 = e1;
                                return;
                        }
        
                        if (e2 > s1 &&
                            e2 < e1) {
                                s3 = s1;
                                e3 = e2;
                                return;
                        }
        
                        if (s1 >= s2 &&
                            s1 <  e2) {
                                s3 = s1;
                                e3 = e2;
                                return;
                        }
                        
                        if (e1 > s2 &&
                            e1 < e2) {
                                s3 = s2;
                                e3 = e1;
                                return;
                        }
                        
                        s3 = 0;
                        e3 = 0;
                }
                
                /* Check if two Rectangles intersect */
                public static bool RectanglesIntersect (Rectangle r1, Rectangle r2)
                {
                        if (LinearIntersectsWith (r1.Left, r1.Right, r2.Left, r2.Right) &&
                            LinearIntersectsWith (r1.Top, r1.Bottom, r2.Top, r2.Bottom))
                                return true;
                        else
                                return false;
                }
                
                /* Get an intersection between two rectangles */
                public static Rectangle GetIntersection (Rectangle r1, Rectangle r2)
                {
                        if (! RectanglesIntersect (r1, r2))
                                return Rectangle.Zero;
                        
                        int hs, he;
                        LinearGetIntersection (r1.Left, r1.Right, r2.Left, r2.Right, out hs, out he);
                        int vs, ve;
                        LinearGetIntersection (r1.Top, r1.Bottom, r2.Top, r2.Bottom, out vs, out ve);
                        
                        return new Rectangle (hs, vs, he - hs, ve - vs);
                }
                
                public static Gtk.Window GetParentForWidget (Widget widget)
                {
                        Gtk.Window parent = null;
                        
                        try {
                                if (widget is Gtk.Window)
                                        return widget as Gtk.Window;
                                
                                if (widget != null && widget.Toplevel != null)
                                        parent = (widget.Toplevel as Gtk.Window);
                                // FIXME: Check for toplevel widget flags
                        } catch (Exception excp) {
                                parent = null;
                        }
                        
                        return parent;
                }
                
        }
        
}
