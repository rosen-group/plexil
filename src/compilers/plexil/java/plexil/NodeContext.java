/* Copyright (c) 2006-2010, Universities Space Research Association (USRA).
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

package plexil;

import java.util.Vector;
import java.util.Map;
import java.util.TreeMap;

import net.n3.nanoxml.IXMLElement;

//
// A class to represent the context of one PLEXIL node in the translator.
//

public class NodeContext
{

    protected NodeContext m_parentContext;
    protected Vector<VariableName> m_variables = new Vector<VariableName>();
    protected Vector<NodeContext> m_children = new Vector<NodeContext>();
	protected Map<String, PlexilTreeNode> m_childIds = new TreeMap<String, PlexilTreeNode>();
    protected String m_nodeName = null;
    protected PlexilTreeNode m_resourcePriorityAST = null;
    protected IXMLElement m_resourcePriorityXML = null;
    protected Vector<PlexilTreeNode> m_resources = new Vector<PlexilTreeNode>();

    public NodeContext(NodeContext previous, String name)
    {
        m_parentContext = previous;
		m_nodeName = name;
        if (previous != null) {
			previous.addChildNode(this);
		}
    }

    public boolean isGlobalContext()
    {
        return false;
    }

    public NodeContext getParentContext()
    {
        return m_parentContext;
    }

    public void addChildNode(NodeContext child)
    {
        m_children.add(child);
    }

	public void setNodeName(String name)
	{
		m_nodeName = name;
	}

    public String getNodeName()
    {
        return m_nodeName;
    }

    // get the root of this context tree
    protected NodeContext getRootContext()
        throws Exception
    {
        if (m_parentContext == null)
            // is global context -- error
            throw new Exception("getRootContext() called on global context");
        else if (m_parentContext.isGlobalContext())
            return this;
        return m_parentContext.getRootContext();
    }

    protected boolean isRootContext()
    {
        return (m_parentContext != null) && m_parentContext.isGlobalContext();
    }

    public boolean isLibraryNode()
    {
        return (m_nodeName != null) && isRootContext();
    }

	public boolean isChildNodeId(String name)
	{
        if (name == null)
            return false;
		if (m_childIds.containsKey(name))
			return true;
		return false;
	}

	public PlexilTreeNode getChildNodeId(String name)
	{
        if (name == null)
            return null;
		return m_childIds.get(name);
	}

	public void addChildNodeId(PlexilTreeNode nameNode)
	{
        if (nameNode == null)
            return;
		String name = nameNode.getText();
		m_childIds.put(name, nameNode);
	}

    // *** this won't find library nodes!
    // Only finds nodes in the current tree.
    public PlexilTreeNode findNode(String name)
    {
        if (name == null)
            return null;
        PlexilTreeNode result = null;
        try {
			result = getRootContext().findNodeInternal(name);
		}
        catch (Exception e) {
		}
        return result;
    }

    protected PlexilTreeNode findNodeInternal(String name)
    {
        // check self
		// FIXME: implement (?)

		// check children
		if (m_childIds.containsKey(name))
			return m_childIds.get(name);

        // recurse down child contexts
        for (NodeContext child : m_children) {
			PlexilTreeNode result = 
				child.findNodeInternal(name);
			if (result != null)
				return result;
		}
        return null;
    }

    // Creates a locally unique node name based on this node's name
    public String generateChildNodeName()
    {
        int childCount = m_children.size() + 1;
        return ((m_nodeName == null) ? "__ANONYMOUS_NODE" : m_nodeName)
            + "__CHILD__" + childCount;
    }

    //
    // Resources and resource priority
    //

    public Vector<PlexilTreeNode> getResources()
    {
        return m_resources;
    }

    public void addResource(PlexilTreeNode resourceAST)
    {
        m_resources.add(resourceAST);
    }

    public PlexilTreeNode getResourcePriorityAST()
    {
        return m_resourcePriorityAST;
    }

    public void setResourcePriorityAST(PlexilTreeNode priority)
    {
        m_resourcePriorityAST = priority;
    }

    public IXMLElement getResourcePriorityXML()
    {
        return m_resourcePriorityXML;
    }

    public void setResourcePriorityXML(IXMLElement priority)
    {
        m_resourcePriorityXML = priority;
    }

    public boolean declareInterfaceVariable(PlexilTreeNode declaration,
											PlexilTreeNode nameNode, 
											boolean isInOut, 
											PlexilDataType typ)
    {
        // For library nodes, just add the declaration
        if (isLibraryNode()) {
			return addInterfaceVariable(declaration, nameNode, isInOut, typ, null, true);
		}
        else {
			boolean success = true;
			// Not a library node -- find original definition, if any
			// N.B. getInheritedVariable can issue a diagnostic if 
			// the variable is previously declared In and now is redeclared InOut
			VariableName ext = findInheritedVariable(nameNode.getText());
			if (ext == null) {
				CompilerState.getCompilerState().addDiagnostic(nameNode, 
															   "Interface variable \""
															   + nameNode.getText()
															   + "\" was not found",
															   Severity.ERROR);
				success = false;
			}
			else {
				// If type supplied, check it for consistency
				if ((typ != null) && (ext.getVariableType() != typ)) {
					CompilerState.getCompilerState().addDiagnostic(nameNode,
																   "Interface variable \"" + nameNode.getText() +
																   "\" declared as type " +
																   typ.typeName() +
																   ", but is actually of type " +
																   ext.getVariableType(),
																   Severity.ERROR);
					success = false;
				}
				success = addInterfaceVariable(declaration, nameNode, isInOut, typ, ext, true);
			}
			return success;
		}
    }

    protected boolean addInterfaceVariable(PlexilTreeNode declaration,
										   PlexilTreeNode nameNode,
										   boolean isInOut,
										   PlexilDataType typ,
										   VariableName original,
										   boolean isDeclared)
    {
        InterfaceVariableName var = null;
        if (isLibraryNode()) {
			var = new InterfaceVariableName(declaration, nameNode.getText(), isInOut, typ);
		}
        else if (original == null) {
			// no such variable
			CompilerState.getCompilerState().addDiagnostic(declaration,
														   "Interface variable \""
														   + nameNode.getText()
														   + "\" is not a known variable",
														   Severity.ERROR);
			return false;
		}
        else {
			var = 
				new InterfaceVariableName(declaration, nameNode.getText(), isInOut, original, isDeclared);
		}
		if (var != null)
			m_variables.add(var);
        return var != null;
    }

    // Caller is responsible for creating the 3 vectors
    public void getNodeVariables(Vector<VariableName> localVarsResult,
                                 Vector<InterfaceVariableName> inVarsResult,
                                 Vector<InterfaceVariableName> inOutVarsResult)
    {
        localVarsResult.removeAllElements();
        inVarsResult.removeAllElements();
        inOutVarsResult.removeAllElements();
        for (VariableName var : m_variables) {
			if (var.isLocal())
				localVarsResult.add(var);
			else if (var.isAssignable())
				inOutVarsResult.add((InterfaceVariableName) var);
			else
				inVarsResult.add((InterfaceVariableName) var);
		}
    }

    public VariableName declareVariable(PlexilTreeNode declaration,
										PlexilTreeNode nameNode,
										PlexilDataType varType,
										ExpressionNode initialValueExpr)
    {
        if (checkVariableName(nameNode)) {
			VariableName result = new VariableName(declaration, nameNode.getText(), varType, initialValueExpr);
			m_variables.add(result);
			return result;
		}
		else 
			return null;
    }

    //
    // Array variables
    //

    public VariableName declareArrayVariable(PlexilTreeNode declaration,
											 PlexilTreeNode nameNode, 
											 PlexilDataType arrayType,
											 String maxSize,
											 ExpressionNode initialValueExpr)
    {
		if (checkVariableName(nameNode)) {
			VariableName result = 
				new VariableName(declaration, nameNode.getText(), arrayType, maxSize, initialValueExpr);
			m_variables.add(result);
			return result;
		}
		else 
			return null;
    }

	// Returns true if name is not in direct conflict with other names in this context.
	// Adds diagnostics to myState if required
    public boolean checkVariableName(PlexilTreeNode nameNode)
    {
		boolean success = true;
		String name = nameNode.getText();
        VariableName existing = findLocalVariable(name);
        if (existing != null) {
			// error - duplicate variable name in node
			CompilerState.getCompilerState().addDiagnostic(nameNode,
														   "Variable name \"" + name
														   + "\" is already declared in this context",
														   Severity.ERROR);
			CompilerState.getCompilerState().addDiagnostic(existing.getDeclaration(),
														   "Variable \"" + name
														   + "\" previously declared here",
														   Severity.NOTE);
			success = false;
		}
        if (m_parentContext != null) {
			VariableName shadowedVar =
				m_parentContext.findInheritedVariable(name);
			if (shadowedVar != null)
				// warn of conflict
				CompilerState.getCompilerState().addDiagnostic(nameNode,
															   "Local variable \"" + name
															   + "\" shadows an inherited variable",
															   Severity.WARNING);
		}
		return success;
    }

    protected VariableName findLocalVariable(String name)
    {
        for (VariableName candidate : m_variables)
            if (candidate.getName().equals(name))
                return candidate;
        return null; 
    }

    // Look up an inherited variable with the given name.  
    // Returns the first instance found up the tree.
    // If none exists, return null.

    protected VariableName findInheritedVariable(String name)
    {
        if (m_parentContext == null)
            return null;
        VariableName vn = m_parentContext.findLocalVariable(name);
        if (vn != null)
            return vn;
        else
            return m_parentContext.findInheritedVariable(name);
    }

    public VariableName findVariable(String name)
    {
        VariableName result = findLocalVariable(name);
        if (result != null)
            return result;
        return findInheritedVariable(name);
    }

    //
    // Simple queries w/o side effects
    //

    public boolean isVariableName(String name)
    {
        VariableName vn = findVariable(name);
        return (vn != null) ;
    }

}
