/* Copyright (c) 2006-2020, Universities Space Research Association (USRA).
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

#include "NodeFactory.hh"

#include "AssignmentNode.hh"
#include "CommandNode.hh"
#include "Debug.hh"
#include "Error.hh"
#include "LibraryCallNode.hh"
#include "ListNode.hh"
#include "UpdateNode.hh"
#include "lifecycle-utils.h"

namespace PLEXIL
{
  // NodeFactory implementation class

  //! \class ConcreteNodeFactory
  //! \brief Templated concrete implementation of the NodeFactory API.
  template<class NODE_TYPE>
  class ConcreteNodeFactory final : public NodeFactory
  {
  public:

    //! \brief Default constructor.
    ConcreteNodeFactory() = default;

    //! \brief Virtual destructor.
    virtual ~ConcreteNodeFactory() = default;

  private:

    //! \brief Primary factory method.
    //! \param name The NodeId of the node to construct.
    //! \param parent The parent node of the node to be constructed.
    //! \return Pointer to the new NodeImpl.
    NodeImpl *create(char const *name, NodeImpl *parent) const
    {
      return static_cast<NodeImpl *>(new NODE_TYPE(name, parent));
    }

    //! \brief Alternate factory method.  Used only by Exec test module.
    //! \param type The name of the node type to construct.
    //! \param name The NodeId of the node to construct.
    //! \param state The initial NodeState.
    //! \param parent The parent node of the node to be constructed.
    //! \return Pointer to the new NodeImpl.
    NodeImpl *create(const std::string& type,
                     const std::string& name, 
                     NodeState state,
                     NodeImpl *parent) const
    {
      return static_cast<NodeImpl *>(new NODE_TYPE(type, name, state, parent));
    }

  };

  ///@{
  //! \internal
  //! \ingroup Exec-Core

  //! \brief The array of node factories, indexed by PlexilNodeType.
  static NodeFactory* s_nodeFactories[NodeType_error];

  //! \brief Delete all node factories.  Cleanup function to be run at application exit.
  static void purgeNodeFactories()
  {
    NodeFactory *tmp;
    for (size_t i = 0; i < NodeType_error; ++i)
      if ((tmp = s_nodeFactories[i]))
        delete tmp;
  }

  //! \brief Populate the array of node factories, and set up the cleanup function.
  static void initializeNodeFactories()
  {
    plexilAddFinalizer(&purgeNodeFactories);
    // Ensure entire map is correctly initialized
    s_nodeFactories[NodeType_uninitialized] = nullptr;
    s_nodeFactories[NodeType_NodeList] = new ConcreteNodeFactory<ListNode>();
    s_nodeFactories[NodeType_Command] = new ConcreteNodeFactory<CommandNode>();
    s_nodeFactories[NodeType_Assignment] = new ConcreteNodeFactory<AssignmentNode>();
    s_nodeFactories[NodeType_Update] = new ConcreteNodeFactory<UpdateNode>();
    s_nodeFactories[NodeType_Empty] = new ConcreteNodeFactory<NodeImpl>();
    s_nodeFactories[NodeType_LibraryNodeCall] = new ConcreteNodeFactory<LibraryCallNode>();
    debugMsg("NodeFactory", " initialized");
  }

  //! \brief Get the node factory for the given node type.
  //! \param nodeType The type of the node to be constructed.
  //! \return Const pointer to the NodeFactory instance.
  static NodeFactory const *getNodeFactory(PlexilNodeType nodeType)
  {
    assertTrue_2((nodeType > NodeType_uninitialized)
		 && (nodeType < NodeType_error),
		 "getNodeFactory: Invalid node type value");

    static bool s_inited = false;
    if (!s_inited) {
      initializeNodeFactories();
      s_inited = true;
    }
    assertTrueMsg(s_nodeFactories[nodeType],
                  "Internal error: no node factory for valid node type" << nodeType);
    return s_nodeFactories[nodeType];
  }

  ///@}

  NodeImpl *NodeFactory::createNode(char const *name, 
                                    PlexilNodeType nodeType,
                                    NodeImpl *parent)
  {
    NodeImpl *result = getNodeFactory(nodeType)->create(name, parent);
    debugMsg("NodeFactory", " created node " << name);
    // common post process here
    return result;
  }

  NodeImpl *NodeFactory::createNode(const std::string& type, 
                                    const std::string& name, 
                                    NodeState state,
                                    NodeImpl *parent)
  {
    PlexilNodeType nodeType = parseNodeType(type.c_str());
    checkError(nodeType < NodeType_error,
               "Invalid node type string " << type);
    NodeImpl *result = getNodeFactory(nodeType)->create(type, name, state, parent);
    debugMsg("NodeFactory", " created node " << name);
    // common post process here
    return result;
  }

}
