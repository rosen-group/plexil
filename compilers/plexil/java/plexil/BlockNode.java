// Copyright (c) 2006-2020, Universities Space Research Association (USRA).
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

import java.util.Vector;
import java.util.TreeSet;

import org.antlr.runtime.*;
import org.antlr.runtime.tree.*;

import net.n3.nanoxml.*;

public class BlockNode extends PlexilTreeNode
{
    // Name binding context
    private NodeContext m_context = null;

    // Components of the block
    private PlexilTreeNode m_comment = null;
    private PlexilTreeNode m_priority = null;
    private Vector<PlexilTreeNode> m_conditions = null;
    private Vector<PlexilTreeNode> m_resources = null;
    private Vector<PlexilTreeNode> m_body = null;
    private Vector<VariableName> m_localVars = null;
    private Vector<InterfaceVariableName> m_inVars = null;
    private Vector<InterfaceVariableName> m_inOutVars = null;

    public BlockNode(Token t)
    {
        super(t);
    }

    public BlockNode(BlockNode n)
    {
        super(n);
		m_comment = n.m_comment;
		m_priority = n.m_priority;
		m_conditions = n.m_conditions;
		m_resources = n.m_resources;
		m_body = n.m_body;
        m_localVars = n.m_localVars;
        m_inVars = n.m_inVars;
        m_inOutVars = n.m_inOutVars;
    }

	public Tree dupNode()
	{
		return new BlockNode(this);
	}

    /**
     * @brief Get the containing name binding context for this branch of the parse tree.
     * @return A NodeContext instance, or the global context.
     */
    public NodeContext getContext()
    {
        return m_context;
    }

    /**
     * @brief Prepare for the semantic check.
     */
    public void earlyCheck(NodeContext parentContext, CompilerState state)
    {
        earlyCheckSelf(parentContext, state);
        for (int i = 0; i < this.getChildCount(); i++)
            this.getChild(i).earlyCheck(m_context, state);
    }

    public void earlyCheckSelf(NodeContext parentContext, CompilerState state)
    {
        // See if we have a node ID
        String nodeId = null;
        PlexilTreeNode parent = this.getParent();
        if (parent != null && parent instanceof ActionNode) {
            nodeId = ((ActionNode) parent).getNodeId();
        }
        else {
            // should never happen
            state.addDiagnostic(this,
                                "Internal error: BlockNode instance has no parent ActionNode",
                                Severity.FATAL);
        }
        m_context = new NodeContext(parentContext, nodeId);

        // Divide up the children by category
        partitionChildren(state);
    }

    /**
     * @brief Perform a recursive semantic check.
     * @return true if check is successful, false otherwise.
     * @note Uses new binding context for children.
     */
    public void check(NodeContext parentContext, CompilerState state)
    {
        checkChildren(m_context, state);
        checkSelf(parentContext, state);
        // Partition variable declarations from the context
        m_context.getNodeVariables(m_localVars, m_inVars, m_inOutVars);
    }

    // N.B. Interface, variable, mutex decl's, mutex refs, and conditions check themselves.
    public void checkSelf(NodeContext context, CompilerState state)
    {
        // Check for duplicate conditions
        TreeSet<Integer> conditionsSeen = new TreeSet<Integer>();
        for (PlexilTreeNode c : m_conditions) {
            Integer condType = Integer.valueOf(c.getType());
            if (conditionsSeen.contains(condType)) {
                state.addDiagnostic(c,
                                    "In node " + m_context.getNodeName()
                                    + ": Multiple \"" + c.getToken().getText()
                                    + "\" conditions specified",
                                    Severity.ERROR);
            }
            else {
                conditionsSeen.add(condType);
            }
        }

        if (isCommandNode()) {
            // TODO: Resources legal, look for collisions
        }
        else {
            // Resources are NOT legal, flag them as errors
            for (PlexilTreeNode r : m_resources) {
                state.addDiagnostic(r, 
                                    "In node " + m_context.getNodeName()
                                    + "The \"" + r.getToken().getText()
                                    + "\" keyword is only valid for Command actions",
                                    Severity.ERROR);
            }
        }
        // Priority is self-checking
    }

    private void partitionChildren(CompilerState state)
    {
        m_conditions = new Vector<PlexilTreeNode>();
        m_resources = new Vector<PlexilTreeNode>();
        m_body = new Vector<PlexilTreeNode>();
        m_localVars = new Vector<VariableName>();
        m_inVars = new Vector<InterfaceVariableName>();
        m_inOutVars = new Vector<InterfaceVariableName>();

        if (this.getChildCount() > 0) {
            int i = 0;
            if (this.getChild(i).getType() == PlexilLexer.COMMENT_KYWD) {
                m_comment = this.getChild(i);
                i++;
            }
            for ( ; i < this.getChildCount(); i++) {
                PlexilTreeNode child = this.getChild(i);
                switch (child.getType()) {
                case PlexilLexer.IN_KYWD:
                case PlexilLexer.IN_OUT_KYWD:
                case PlexilLexer.VARIABLE_DECLARATIONS:
                case PlexilLexer.MUTEX_KYWD:
                case PlexilLexer.USING_KYWD:
                    // declarations add selves to current context
                    break;

                case PlexilLexer.END_CONDITION_KYWD:
                case PlexilLexer.EXIT_CONDITION_KYWD:
                case PlexilLexer.INVARIANT_CONDITION_KYWD:
                case PlexilLexer.POST_CONDITION_KYWD:
                case PlexilLexer.PRE_CONDITION_KYWD:
                case PlexilLexer.REPEAT_CONDITION_KYWD:
                case PlexilLexer.SKIP_CONDITION_KYWD:
                case PlexilLexer.START_CONDITION_KYWD:
                    m_conditions.add(child);
                    break;

                case PlexilLexer.RESOURCE_KYWD:
                    m_resources.add(child);
                    break;

                case PlexilLexer.PRIORITY_KYWD:
                    if (m_priority != null) {
                        state.addDiagnostic(child, 
                                            "In node " + m_context.getNodeName()
                                            + " Multiple Priority attributes",
                                            Severity.ERROR);
                    } else {
                        m_priority = child;
                    }
                    break;

                case PlexilLexer.ACTION:
                    m_body.add(child);
                    break;

                default:
                    // should never happen (?)
                    state.addDiagnostic(this,
                                        "Internal error: Unexpected child "
                                        + child.getToken().getText()
                                        + " in block",
                                        Severity.FATAL);
                    break;
                }
            }
        }
    }

    // N.B. Could add more cases to inherit XML from child.
    protected void constructXML()
    {
        // Place children in canonical order
        int insertIdx = 0;

        // Build basic node
        if (m_body.isEmpty()) {
            // Empty node
            m_xml = new XMLElement("Node");
            m_xml.setAttribute("NodeType", "Empty");
            this.addSourceLocatorAttributes();
        }
        else if (isSimpleNode()) {
            // Get base XML from child
            PlexilTreeNode action = m_body.firstElement();
            if (firstChildHasNodeId()) {
                m_xml = action.getXML();  // get it with NodeId
                insertIdx++;              // insert everything after NodeId
            }
            else
                m_xml = action.getChild(0).getXML(); // get bare node
        }
        else {
            // 1 or more items in body
            if (this.getType() == PlexilLexer.LBRACE)
                m_xml = new XMLElement("Sequence");
            else
                m_xml = new XMLElement(this.getToken().getText());
            this.addSourceLocatorAttributes();
            // Add children from body
            for (PlexilTreeNode n : m_body)
                m_xml.addChild(n.getXML());
        }

        // Add comment
        if (m_comment != null) {
            IXMLElement comment = m_comment.getChild(0).getXML();
            comment.setName("Comment");
            m_xml.insertChild(comment, insertIdx++);
        }

        if (!m_inVars.isEmpty() || !m_inOutVars.isEmpty()) {
            IXMLElement intfc = new XMLElement("Interface");
            m_xml.insertChild(intfc, insertIdx++);
            if (!m_inVars.isEmpty()) {
                IXMLElement inXML = new XMLElement("In");
                intfc.addChild(inXML);
                for (InterfaceVariableName iv : m_inVars)
                    inXML.addChild(iv.makeDeclarationXML());
            }
            if (!m_inOutVars.isEmpty()) {
                IXMLElement inOutXML = new XMLElement("InOut");
                intfc.addChild(inOutXML);
                for (InterfaceVariableName iov : m_inOutVars)
                    inOutXML.addChild(iov.makeDeclarationXML());
            }
        }

        if (!m_localVars.isEmpty() || !m_context.getMutexes().isEmpty()) {
            IXMLElement decls = new XMLElement("VariableDeclarations");
            m_xml.insertChild(decls, insertIdx++);
            for (VariableName v : m_localVars)
                decls.addChild(v.makeDeclarationXML());
            for (MutexName m : m_context.getMutexes())
                decls.addChild(m.makeDeclarationXML());
        }

        // Add conditions
        for (PlexilTreeNode n : m_conditions)
            m_xml.insertChild(n.getXML(), insertIdx++);

        // Add mutex references
        if (!m_context.getMutexRefs().isEmpty()) {
            IXMLElement mtx = new XMLElement("UsingMutex");
            m_xml.insertChild(mtx, insertIdx++);
            for (MutexName m : m_context.getMutexRefs()) {
                IXMLElement ref = m.asReference();
                // TODO: Add source locators
                mtx.addChild(ref);
            }
        }

        // Add priority
        if (m_priority != null)
            m_xml.insertChild(m_priority.getXML(), insertIdx++);

        if (!m_resources.isEmpty()) {
            if (isCommandNode()) {
                // Add command resources, if required
                // Has to be done here because CommandNode represents the naked command
                // and resources are in the surrounding braces
                IXMLElement rlist = new XMLElement("ResourceList");
                for (PlexilTreeNode n : m_resources)
                    rlist.addChild(n.getXML());
                XMLElement commandXml =
                    (XMLElement) m_xml.getFirstChildNamed("NodeBody").getFirstChildNamed("Command");
                commandXml.insertChild(rlist, 0);
            } else {
                // TODO: generate warning
            }
        }
    }

    private boolean firstChildHasNodeId()
    {
        if (m_body.isEmpty())
            return false;
        return 1 < m_body.firstElement().getChildCount();
    }

    private boolean hasOuterNodeId()
    {
        PlexilTreeNode parent = this.getParent();
        return parent != null
            && parent instanceof ActionNode
            && parent.getChildCount() > 1;
    }

    private boolean isSimpleNode()
    {
        // Don't collapse these block types because outcome semantics change
        if (this.getType() == PlexilLexer.UNCHECKED_SEQUENCE_KYWD
            || this.getType() == PlexilLexer.CONCURRENCE_KYWD)
            return false;

        if (m_body.size() != 1)
            return false;

        PlexilTreeNode action = m_body.firstElement(); // (Action NCNAME? *)
        int childCt = action.getChildCount();
        if (hasOuterNodeId() && childCt > 1)
            return false; // both parent & child have node IDs

        PlexilTreeNode subaction = action.getChild(childCt - 1);
        switch (subaction.getType()) {
            // Primitives
        case PlexilLexer.ASSIGNMENT:
        case PlexilLexer.COMMAND:
        case PlexilLexer.UPDATE_KYWD:
        case PlexilLexer.LIBRARY_CALL_KYWD:
            return true;

            // Compound
        case PlexilLexer.IF_KYWD:
        case PlexilLexer.FOR_KYWD:
        case PlexilLexer.WHILE_KYWD:
        case PlexilLexer.ON_COMMAND_KYWD:
        case PlexilLexer.ON_MESSAGE_KYWD:
        case PlexilLexer.SYNCHRONOUS_COMMAND_KYWD:
            return true;

        default:
            return false;
        }
    }

    private boolean isCommandNode()
    {
        if (m_body.size() != 1) {
            return false;
        }
        PlexilTreeNode action = m_body.firstElement();
        // this should NEVER fail!
        if (action.getType() != PlexilLexer.ACTION) {
            return false;
        }
        return action.getChild(action.getChildCount() - 1).getType() == PlexilLexer.COMMAND;
    }

}
