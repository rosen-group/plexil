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

import org.antlr.runtime.*;
import org.antlr.runtime.tree.*;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

//* @class NodeTreeNode
//* NodeTreeNode is an implementation class representing any parse subtree
//* which could represent a PLEXIL Node.  It is a superclass of several
//* instantiable tree node classes, but is not meant to be instantiated
//* by itself.  It implements common behavior required by all Node types.

public class NodeTreeNode
    extends PlexilTreeNode
    implements PlexilNode
{
    protected NodeContext m_context = null;
    protected String m_nodeId = null;

    // Gensym counter
    private static int s_generatedIdCount = 0;

    protected NodeTreeNode(Token t)
    {
        super(t);
    }

    protected NodeTreeNode(NodeTreeNode n)
    {
        super(n);
        m_context = n.m_context;
    }

    //
    // PlexilNode API
    //

    @Override
    public NodeContext getLocalContext()
    {
        return m_context;
    }

    @Override
    public boolean hasNodeId()
    {
        // Return true if we've already cached it
        if (m_nodeId != null)
            return true;

        // Check ancestry
        PlexilTreeNode parent = this.getParent();
        if (parent == null)
            return false; // shouldn't happen

        // If directly wrapped in an action, check the action.
        if (parent.getType() == PlexilLexer.ACTION)
            return parent.hasNodeId();

        // If directly wrapped by a block, and we're its only body child,
        // check the block.
        if (parent instanceof BlockNode
            && ((BlockNode) parent).isCollapsible())
            return parent.hasNodeId();

        return false;
    }

    @Override
    public boolean inheritsParentContext()
    {
        PlexilTreeNode parent = this.getParent();
        if (parent == null)
            return false; // no parent - shouldn't happen

        // Should be the only case where a Node inherits its parent's context.
        if (parent instanceof BlockNode
            && ((BlockNode) parent).isCollapsible())
            return true;

        return false;
    }

    // Ensure that there is always a valid NodeId.
    protected void initializeNodeId()
    {
        // Check ancestry
        PlexilTreeNode parent = this.getParent();
        if (parent != null &&
            parent.getType() == PlexilLexer.ACTION
            && ((ActionNode) parent).hasNodeId()) {
            m_nodeId = parent.getChild(0).getText();
            return;
        }

        // If directly wrapped by a block, and we're its only body child,
        // check the block.
        if (parent instanceof BlockNode) {
            BlockNode oldBlock = (BlockNode) parent;
            if (oldBlock.isCollapsible() // i.e. we're an only child
                && oldBlock.hasNodeId()) {
                m_nodeId = oldBlock.m_nodeId;
                return;
            }
        }

        // Else gensym one
        String prefix = this.getToken().getText();
        if (prefix.equals("{"))
            prefix = "BLOCK";
        m_nodeId = prefix + "__" + s_generatedIdCount++;
    }

    public void initializeContext(NodeContext parentContext)
    {
        initializeNodeId();
        if (this.inheritsParentContext()) {
            m_context = parentContext;
        }
        else {
            m_context = new NodeContext(parentContext, m_nodeId);
        }
    }

    @Override
    protected void earlyCheckSelf(NodeContext parentContext, CompilerState state)
    {
        initializeContext(parentContext);
    }

    @Override
    protected void earlyCheckChildren(NodeContext parentContext, CompilerState state)
    {
        for (PlexilTreeNode child : this.getChildren())
            child.earlyCheck(m_context, state);
    }

    @Override
    protected void checkChildren(NodeContext parentContext, CompilerState state)
    {
        for (PlexilTreeNode child : this.getChildren())
            child.check(m_context, state);
    }

    protected Element createNodeIdElement(Document root)
    {
        Element result = root.createElement("NodeId");
        result.appendChild(root.createTextNode(m_nodeId));
        return result;
    }

    // Add the NodeId to the base XML element.
    @Override
    protected void constructXMLBase(Document root)
    {
        super.constructXMLBase(root); // PlexilTreeNode method
        m_xml.appendChild(createNodeIdElement(root));
    }

}
