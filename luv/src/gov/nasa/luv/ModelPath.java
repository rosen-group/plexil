/* Copyright (c) 2006-2008, Universities Space Research Association (USRA).
*  All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Universities Space Research Association nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

package gov.nasa.luv;

import java.util.Vector;
import java.util.Enumeration;

import static gov.nasa.luv.Constants.*;


/** A path from the root node of a model tree to a given model emement
 * in that tree. */

public class ModelPath
{
      /** Place to store the model path. */

      private Vector<String> path = new Vector<String>();
      
      /** The model propert to form this path from */
      
      String property;

      /** Construct a model path from a given model element
       *
       * @param model the model to create the path to.
       */
      
      public ModelPath(Model model)
      {
         this(model, MODEL_NAME);
      }
      
      /** Construct a model path from a given model element
       *
       * @param model the model to create the path to.
       */
      
      public ModelPath(Model model, String property)
      {
         this.property = property;
         establishPath(model);
      }

      /** Establish the path to the model element. */
      
      private void establishPath(Model model)
      {
         if (model.isRoot())
            path.add(model.getProperty(property));
         else
         {
            establishPath(model.getParent());
            path.add(model.getProperty(property));
         }
      }

      /** { @inheritDoc } */
      
      @Override public String toString()
      {
         StringBuffer buf = new StringBuffer();
         for (String element: path)
         {
            buf.append(element);
            buf.append(path.lastElement() == element ? "" : ":");
         }

         return buf.toString();
      }

      /** Find the model indicated by the path in the tree. 
       *
       * @param root root node of model tree
       * @return the indicated model in the tree, or null if it does not
       * exist
       */

      public Model find(Model root)
      {
         // attach dummy as root of root

         Model oldParent = root.getParent();
         Model dummy = new Model("dummy");
         dummy.addChild(root);
         Model current = dummy;
         
         // work down the path

         for (String element: path)
         {
            current = current.findChild(property, element);

            // if no matching node was found, the search failed

            if (current == null)
               break;
         }

         // remove dummy as root of root
         
         dummy.removeChild(root);
         root.setParent(oldParent);

         // return the matching node or null if not found
         
         return current;
      }
}
