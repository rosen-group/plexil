// Copyright (c) 2006-2022, Universities Space Research Association (USRA).
//  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//    * Neither the name of the Universities Space Research Association nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
// OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

package plexil;

import java.util.List;

import org.antlr.runtime.*;
import org.antlr.runtime.tree.*;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

public class CommandNode
    extends NodeTreeNode
    implements Expression
{
    private GlobalDeclaration m_commandDeclaration = null;
    private ArgumentListNode m_parameters = null;
    private PlexilDataType m_dataType = PlexilDataType.VOID_TYPE;

    public CommandNode(Token t)
    {
        super(t);
    }

    public CommandNode(CommandNode n)
    {
        super(n);
		m_commandDeclaration = n.m_commandDeclaration;
		m_parameters = n.m_parameters;
    }

    @Override
	public Tree dupNode()
	{
		return new CommandNode(this);
	}

    // Expression API
    @Override
    public PlexilDataType getDataType()
    {
        return m_dataType;
    }

    // Overrides NodeTreeNode method
    @Override
    public boolean hasNodeId()
    {
        // Check ancestry
        PlexilTreeNode parent = this.getParent();
        if (parent == null)
            return false; // shouldn't happen

        // If directly wrapped in an action, check the action.
        if (parent instanceof ActionNode)
            return ((ActionNode) parent).hasNodeId();

        // If directly wrapped in an assignment, ask the assignment.
        if (parent instanceof AssignmentNode)
            return ((AssignmentNode) parent).hasNodeId();

        // If directly wrapped in block, and we're its only body child,
        // ask the block.
        if (parent instanceof BlockNode
            && ((BlockNode) parent).isCollapsible())
            return ((BlockNode) parent).hasNodeId();

        return false;
    }

    // Ensure that there is always a valid NodeId.
    // Overrides NodeTreeNode method.
    protected void initializeNodeId()
    {
        // Check ancestry
        PlexilTreeNode parent = this.getParent();
        if (parent != null) {
            // If directly wrapped in an Action node, get its ID.
            if (parent instanceof ActionNode
                && ((ActionNode) parent).hasNodeId()) {
                m_nodeId = ((ActionNode) parent).getNodeId();
                return;
            }

            // If wrapped in an Assignment node, check it.
            if (parent instanceof AssignmentNode
                && ((AssignmentNode) parent).hasNodeId()) {
                m_nodeId = ((AssignmentNode) parent).getNodeId();
                return;
            }

            // If directly wrapped by a block, and we're its only body child,
            // check the block.
            if (parent instanceof BlockNode
                && ((BlockNode) parent).hasNodeId()
                && ((BlockNode) parent).isCollapsible()) { // i.e. we're an only child
                m_nodeId = ((BlockNode) parent).getNodeId();
                return;
            }
        }

        // Create one
        this.createNodeId(this.getToken().getText());
    }

    // Overrides NodeTreeNode method.
    @Override
    public boolean inheritsParentContext()
    {
        PlexilTreeNode parent = this.getParent();
        if (parent == null)
            return false; // no parent - shouldn't happen
        if (parent instanceof AssignmentNode)
            return true;  // command w/ assignment, get its context
        if (parent instanceof BlockNode
            && ((BlockNode) parent).isCollapsible())
            return true; // only child of a block, inherit its context
        return false;
    }

    // N.B. Only valid after earlyCheckSelf()
    public GlobalDeclaration getCommand()
    {
        return m_commandDeclaration;
    }

    // AST is:
    // (COMMAND ((COMMAND_NAME NCNAME) | expression) (ARGUMENT_LIST expression*)?)

    @Override
    protected void earlyCheckSelf(NodeContext parentContext, CompilerState state)
    {
        initializeContext(parentContext);

        PlexilTreeNode nameAST = this.getChild(0);
        if (this.getChildCount() > 1)
            m_parameters = (ArgumentListNode) this.getChild(1);

        if (nameAST.getType() == PlexilLexer.COMMAND_NAME) {
            // Literal command name - 
            // Check that name is defined as a command
            PlexilTreeNode nameNode = nameAST.getChild(0);
            String name = nameNode.getText();
            // Set source locators
            this.getToken().setLine(nameNode.getLine());
            this.getToken().setCharPositionInLine(nameNode.getCharPositionInLine());

            if (!state.getGlobalContext().isCommandName(name)) {
                state.addDiagnostic(nameNode,
                                    "Command \"" + name + "\" is not defined",
                                    Severity.ERROR);
            }

            // get declaration for type info
            m_commandDeclaration = state.getGlobalContext().getCommandDeclaration(name);
            if (m_commandDeclaration == null) {
                // We can't make any assumptions about return type
                m_dataType = PlexilDataType.ANY_TYPE;
            }
            else {
                // We know this command
                // Set return type 
                PlexilDataType retnType = m_commandDeclaration.getReturnType();
                if (PlexilDataType.isValid(retnType))
                    m_dataType = retnType;

                // We have a valid command declaration
                // Check parameter list
                List<VariableName> parmSpecs = m_commandDeclaration.getParameterVariables();
                if (parmSpecs == null) {
                    // No parameters expected
                    if (m_parameters != null) {
                        state.addDiagnostic(m_parameters,
                                            "Command \"" + name + "\" expects 0 parameters, but "
                                            + String.valueOf(m_parameters.getChildCount() + " were supplied"),
                                            Severity.ERROR);
                    }
                }
                else {
                    // No parameters given
                    if (m_parameters == null) {
                        if (parmSpecs.size() == 1 &&
                           parmSpecs.get(0) instanceof WildcardVariableName){
                            // No parameters required, do nothing

                        }

                        else {
                            // Node supplied
                            state.addDiagnostic(nameNode,
                                                "Command \"" + name + "\" expects "
                                                + String.valueOf(parmSpecs.size())
                                                + " parameters, but none were supplied",
                                                Severity.ERROR);
                        }
                    }
                    else {
                        m_parameters.earlyCheckArgumentList(m_context,
                                                            state,
                                                            "Command",
                                                            name,
                                                            parmSpecs);
                    }
                    // Parameter type checking done in checkTypeConsistency() below
                }
                // Resource list is handled by BlockNode
            }
        }
        else {
            // Set source locators to expression
            this.getToken().setLine(getChild(0).getLine());
            this.getToken().setCharPositionInLine(getChild(0).getCharPositionInLine());
        }
    }

    @Override
    protected void earlyCheckChildren(NodeContext parentContext, CompilerState state)
    {
        for (PlexilTreeNode child: this.getChildren())
            child.earlyCheck(m_context, state);
    }
    
    @Override
    protected void checkChildren(NodeContext context, CompilerState state)
    {
        for (PlexilTreeNode child: this.getChildren())
            child.check(m_context, state);

        PlexilTreeNode nameAST = this.getChild(0);
        if (nameAST.getType() != PlexilLexer.COMMAND_NAME) {
            // if name is not literal, 
            // check that name expression returns a string
            ExpressionNode nameExp = (ExpressionNode) nameAST;
            if (!nameExp.assumeType(PlexilDataType.STRING_TYPE, state)) {
                state.addDiagnostic(nameExp,
                                    "Command name expression is not a string expression",
                                    Severity.ERROR);
            }
        }

        if (m_commandDeclaration != null) {
            // Check parameter list against declaration
            String cmdName = m_commandDeclaration.getName();
            List<VariableName> parmSpecs = m_commandDeclaration.getParameterVariables();
            if (parmSpecs != null && m_parameters != null)
                m_parameters.checkArgumentList(m_context, state, "command", cmdName, parmSpecs);
        }
    }        

    @Override
    protected void constructXML(Document root)
    {
        // construct Node XML
        super.constructXMLBase(root);
        m_xml.setAttribute("NodeType", "Command");

        // construct node body
        Element nodeBody = root.createElement("NodeBody");
        m_xml.appendChild(nodeBody);

        Element commandBody = root.createElement("Command");
        // set source location to the loc'n of the command name (expression)
        commandBody.setAttribute("LineNo", String.valueOf(this.getChild(0).getLine()));
        commandBody.setAttribute("ColNo", String.valueOf(this.getChild(0).getCharPositionInLine()));

        nodeBody.appendChild(commandBody);

        // BlockNode handles resource list

        // Add name (expression)
        PlexilTreeNode commandName = this.getChild(0);
        Element nameXML = root.createElement("Name");
        commandBody.appendChild(nameXML);
        if (commandName.getType() == PlexilLexer.COMMAND_NAME) {
            // Literal command name
            Element stringVal = root.createElement("StringValue");
            stringVal.appendChild(root.createTextNode(commandName.getChild(0).getText()));
            nameXML.appendChild(stringVal);
        }
        else {
            // Command name expression
            nameXML.appendChild(commandName.getXML(root));
        }

        // Handle parameters
        if (m_parameters != null) {
            commandBody.appendChild(m_parameters.getXML(root));
        }
    }

    @Override
    protected String getXMLElementName() {return "Node"; }

    // TODO: extend to return true for constant string expressions
    private boolean isCommandNameLiteral()
    {
        return this.getChild(0).getType() == PlexilLexer.COMMAND_NAME;
    }

}
