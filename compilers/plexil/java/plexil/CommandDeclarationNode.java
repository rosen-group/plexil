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

import java.util.ArrayList;
import java.util.List;

import org.antlr.runtime.*;
import org.antlr.runtime.tree.*;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

public class CommandDeclarationNode extends PlexilTreeNode
{
    public CommandDeclarationNode(Token t)
    {
        super(t);
    }

    public CommandDeclarationNode(CommandDeclarationNode n)
    {
        super(n);
    }

	public Tree dupNode()
	{
		return new CommandDeclarationNode(this);
	}

    // structure is:
    // ^(COMMAND_KYWD NCNAME paramsSpec? returnsSpec? resourcesList?)

    public void earlyCheck(NodeContext context, CompilerState state)
    {
        // check that name is not already defined
        String cmdName = this.getChild(0).getText();
        if (state.getGlobalContext().isCommandName(cmdName)) {
            // Report duplicate definition
            state.addDiagnostic(this.getChild(0),
                                "Command \"" + cmdName + "\" is already defined",
                                Severity.ERROR);
        }

        // Parse parameter list, if supplied
        List<VariableName> parmSpecs = null;
        ParameterSpecNode parmAST = getParameters();
        if (parmAST != null) {
            parmAST.earlyCheck(context, state); // for effect
            parmSpecs = parmAST.getParameterList();
            if (parmSpecs != null) {
                for (VariableName vn : parmSpecs) {
                    if (vn instanceof InterfaceVariableName) {
                        state.addDiagnostic(vn.getDeclaration(),
                                            (vn.isAssignable() ? "InOut" : "In")
                                            + " declaration is illegal in command parameter declarations",
                                            Severity.ERROR);
                    }
                }
            }
        }

        // Parse return type, if supplied
        VariableName returnSpec = null;
        ReturnSpecNode returnAST = (ReturnSpecNode) getReturn();
        if (returnAST != null) {
            returnAST.earlyCheck(context, state); // for effect
            returnSpec = returnAST.getReturnSpec();
        } 

        // TODO: Handle resource list

        // Define in global environment
        state.getGlobalContext().addCommandName(this, cmdName, parmSpecs, returnSpec);
    }

    @Override
    protected void constructXML(Document root)
    {
        this.constructXMLBase(root);

        // add name
        PlexilTreeNode nameTree = this.getChild(0);
        Element nameXML = root.createElement("Name");
        nameXML.appendChild(root.createTextNode(nameTree.getText()));
        m_xml.appendChild(nameXML);

        if (this.getChildCount() > 1) {
            // Add return spec(s) if provided
            ReturnSpecNode returnSpec = getReturn();
            if (returnSpec != null) {
                returnSpec.constructReturnXML(root, m_xml);
            }

            // Add parameter spec(s) if provided
            ParameterSpecNode parametersSpec = getParameters();
            if (parametersSpec != null)
                parametersSpec.constructParameterXML(root, m_xml);

            // TODO: add resource list if provided
            PlexilTreeNode resourceList = getResourceList();
            if (resourceList != null) {

            }
        }
    }

    public String getXMLElementName() { return "CommandDeclaration"; }

    protected ParameterSpecNode getParameters()
    {
        if (this.getChildCount() < 2)
            return null;
        if (this.getChild(1).getType() != PlexilLexer.PARAMETERS)
            return null;
        return (ParameterSpecNode) this.getChild(1);
    }

    protected ReturnSpecNode getReturn()
    {
        if (this.getChildCount() < 2)
            return null;
        for (int i = 1; i < this.getChildCount(); i++) {
            if (this.getChild(i).getType() == PlexilLexer.RETURNS_KYWD)
                return (ReturnSpecNode) this.getChild(i);
        }
        return null; // not found
    }

    protected PlexilTreeNode getResourceList()
    {
        // *** TODO: implement!
        return null;
    }

}
