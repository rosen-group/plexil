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

import javax.swing.JPanel;
import javax.swing.JScrollPane;

import java.awt.Container;
import java.awt.BorderLayout;

import java.util.Map;
import static gov.nasa.luv.Constants.*;
import static java.awt.BorderLayout.*;


public class ViewHandler 
{
    
      private View currentView = null;                                   // current view

      private JPanel viewPanel = new JPanel();                           // Panel in which different views are placed.

      public void clearCurrentView()
      {
          currentView = null;
      }

      public View getCurrentView()
      {
          return currentView;
      }

      public JPanel getViewPanel()
      {
          return viewPanel;
      }

      public void setViewProperties(Properties properties)
      {
          currentView.setViewProperties(properties);
      }

      /** Refresh the current view */

      public void refreshView()
      {
         ((Container)currentView).repaint();
      }

      /**
       * Sets the current view. 
       *
       * @param view view to display
       */

      public void setView(Container view)
      {
         // handle view properties

         if (currentView != null)
            setViewProperties(Luv.getLuv().getProperties());
         currentView = ((View)view);
         currentView.getViewProperties(Luv.getLuv().getProperties());

         // clear out the view panel and put the new view in there

         viewPanel.removeAll();
         viewPanel.setLayout(new BorderLayout());
         JScrollPane sp = new JScrollPane(view);
         sp.setBackground(Luv.getLuv().getProperties().getColor(PROP_WIN_BCLR));
         viewPanel.add(sp, CENTER);

         // insert the view menu items

         Luv.getLuv().getViewMenu().removeAll(); 
         for(LuvAction action: currentView.getViewActions())
            Luv.getLuv().getViewMenu().add(action);
         Luv.getLuv().getViewMenu().add(Luv.getLuv().showHidePrlNodes);


         // enable that menu if we actually have menu items
         
         Luv.getLuv().getViewMenu().setEnabled(Luv.getLuv().getViewMenu().getMenuComponentCount() > 0);
         Luv.getLuv().setLocation(Luv.getLuv().getLocation());

         // size everything

         Luv.getLuv().setPreferredSize(Luv.getLuv().getSize());

         // set the frame title
         
         Luv.getLuv().setTitle();

         // show the new view

         Luv.getLuv().pack();
         Luv.getLuv().repaint();
      }
      
      /** Reset the current view to refect the changes in the world. */

      public void resetView()
      {
         // create a new instance of the view
          
          if (Model.getRoot().getChildren().size() < 1)
              ; //do not setView
          else
              setView(new TreeTableView("", Model.getRoot()));

         // map all the breakpoints into the new model

         Luv.getLuv().getLuvBreakPointHandler().getUnfoundBreakPoints().clear();
         
         for (Map.Entry<BreakPoint, ModelPath> pair: Luv.getLuv().getLuvBreakPointHandler().getBreakPointMap().entrySet())
         {
            BreakPoint breakPoint = pair.getKey();
            ModelPath path = pair.getValue();

            Model target = path.find(Model.getRoot());
            if (target != null)
            {
               breakPoint.setModel(target);
            }
            else
               Luv.getLuv().getLuvBreakPointHandler().getUnfoundBreakPoints().add(breakPoint);
         }
      }

}
