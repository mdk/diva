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

        using System;
        
        public static class StringFu {
                
                // Public methods /////////////////////////////////////////////
                
                /* Make the string HTML markup compatible (ie. for usage in
                 * markup-aware gtk widgets */
                public static string Markupize (string text)
                {
                        string res = text.Replace ("&", "&amp;");
                        res = res.Replace ("<" , "&lt;");
                        res = res.Replace (">" , "&gt;");
                        return res;
                }
                
                /* Produce a nice "report like" string will left/right aligned text. Ie:
                 * Here goes a..............................................Here goes b */
                public static string ReportLine (string a, string b, char fillCharacter, int width)
                {
                        int fillers = (width - a.Length - b.Length - 2);
                        fillers = (fillers <= 0) ? 1 : fillers;
                        
                        string created = a + " ";
                        for (int i = 0; i < fillers; i++)
                                created += fillCharacter;

                        created = created + " " + b;
                        return created;
                }
                
                /* Produced a centered string surrounded by fillers. Ie: 
                 * ====================== TEXT ===================================
                 * We're adding a spaces padding by default */
                public static string CenteredLine (string text, char fillCharacter, int width)
                {
                        string created = String.Empty;
                        for (int i = 0; i < (width - text.Length - 2) / 2; i++)
                                created += fillCharacter;
                        
                        created = created + " " + text + " ";
                        
                        for (int i = created.Length; i < width; i++)
                                created += fillCharacter;
                        
                        return created;
                }
                
                /* Word wrap the given string. The string is assumed to have no 
                 * line breaks. First line line-break can be specified */
                public static string Wrap (string text, int first, int column)
                {
                        int pos = first;
                        
                        while (pos < text.Length) {
                                text = text.Insert (pos, "\n");
                                
                                pos += column;
                        }
                        
                        return text;
                }
                
                /* Check if a given string contains whitespace characters */
                public static bool HasWhiteSpace (string str)
                {
                        foreach (char c in str)
                                if (Char.IsWhiteSpace (c))
                                        return true;
                        
                        return false;
                }
                
                /* Check if the given string contains only digits */
                public static bool IsDigit (string str)
                {
                        foreach (char c in str)
                                if (! Char.IsDigit (c))
                                        return false;
                        
                        return true;
                }
                
                /* Check if the given string contains only digit or numbers */
                public static bool IsLetterOrDigit (string str)
                {
                        foreach (char c in str)
                                if (! Char.IsLetterOrDigit (c))
                                        return false;
                        
                        return true;
                }
                
                /* Check if a given string contains only digit, numbers or spaces */
                public static bool IsLetterOrDigitOrWhiteSpace (string str)
                {
                        foreach (char c in str)
                                if (! (Char.IsLetterOrDigit (c) || Char.IsWhiteSpace (c)))
                                        return false;
                        
                        return true;
                }
                
                /* Make this string "sane" by replacing the tabs with spaces and
                 * trimming some extra whitespace */
                public static string MakeSane (string str)
                {
                        string ret = str.Replace ('\t', ' ');
                        return ret.Trim ();
                }
                
        }
        
}
 
