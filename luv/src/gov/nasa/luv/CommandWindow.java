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
import javax.swing.JTable;
import javax.swing.JFrame;

import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.Color;

import java.util.ArrayList;
import java.util.Vector;

import static gov.nasa.luv.Constants.*;

public class CommandWindow extends JPanel 
{
    
    Model model;
    static String status = UNKNOWN;
    static JFrame frame;
    static CommandWindow commandPane;
    int rows = 1000;
    int columns = 2;
    String info[][];
    JTable table;
    ArrayList elements; 
    String commandArray[];
    
    Vector<Model.ChangeListener> modelListeners = new Vector<Model.ChangeListener>();
    
    public CommandWindow(Model model, String save) 
    {       
        super(new GridLayout(1,0));
        
        this.model = model;
        
        status = save;

        String[] columnNames = {"Value",
                                "Command"};
        
        int row = 0;
        int col = 0;
        info = new String[rows][columns];

        String command = Luv.getLuv().getCommandHandler().nodeCommandHolder.get(model.getProperty(NODE_ID));
        
        if (command.length() > 100)
        {
            if (command.contains(","))
                commandArray = command.split(", ");
        }
        else
        {
            commandArray = new String[1];
            commandArray[0] = command;
        }
        
        info[row][col] = UNKNOWN;
        col++;
        
        for (row = 0; row < commandArray.length; row++)
        {
            if (row == 0)
            {
                info[row][col] = commandArray[row]; 
                if (row + 1 < commandArray.length)
                    info[row][col] += ",";
            }
            else
            {                
                info[row][col] = "                     ";
                for (int i = 0; i < commandArray[0].indexOf("("); i++)
                    info[row][col] += " ";
      
                info[row][col] += commandArray[row];
                
                if (row + 1 < commandArray.length)
                    info[row][col] += ",";
            }
        }
        
        // add model listener

        this.model.addChangeListener(new Model.ChangeAdapter()
           {
                 @Override 
                 public void propertyChange(Model model, String property)
                 {
                    if (property.contains("command"))
                    {
                        if (model.getProperty("command").equals("0"))
                            status = FALSE;
                        else if (model.getProperty("command").equals("1"))
                            status = TRUE;

                        info[0][0] = status;
                        table.setValueAt(status, 0, 0);
                        table.repaint();
                     }
                 }
           });
        
        table = new JTable(info, columnNames);
        
        // Disable auto resizing
        
        table.setAutoResizeMode(JTable.AUTO_RESIZE_OFF);
    
        // Set the first visible column to 100 pixels wide

        table.getColumnModel().getColumn(0).setPreferredWidth(100);
        table.getColumnModel().getColumn(1).setPreferredWidth(600);
        
        table.setPreferredScrollableViewportSize(new Dimension(700, 200));

        table.setShowGrid(false);

        table.setGridColor(Color.GRAY);

        //Create the scroll pane and add the table to it.
        JScrollPane scrollPane = new JScrollPane(table);

        //Add the scroll pane to this panel.
        add(scrollPane);
    }
    
    public static CommandWindow getCurrentWindow()
    {
        return commandPane;
    }
    
    public static boolean isCommandWindowOpen()
    {
        if (frame != null)
            return frame.isVisible();
        else 
            return false;
    }
    
    public static void closeCommandWindow()
    {
        frame.setVisible(false);
    }

    /**
     * Create the GUI and show it.  For thread safety,
     * this method should be invoked from the
     * event-dispatching thread.
     */
    public static void createAndShowGUI(Model model, String nodeName) 
    {       
        //Create and set up the window.
        if (frame != null)
            frame.setVisible(false);
        frame = new JFrame(nodeName);

        //Create and set up the content pane.
        commandPane = new CommandWindow(model, status);
        commandPane.setOpaque(true); //content panes must be opaque
        frame.setContentPane(commandPane);
        frame.setBounds(20, 20, 1200, 500);

        //Display the window.
        frame.pack();
        frame.setVisible(true);
    }
    
    public static void resetGUI(Model model, String nodeName)
    {
        frame.setTitle(nodeName);
        
        commandPane = new CommandWindow(model, status);
        commandPane.setOpaque(true); //content panes must be opaque
        frame.setContentPane(commandPane);

        //Display the window.
        frame.pack();
        frame.setVisible(true);
    }
}
