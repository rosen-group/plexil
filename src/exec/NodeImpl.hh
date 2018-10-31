/* Copyright (c) 2006-2018, Universities Space Research Association (USRA).
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

#ifndef NODE_IMPL_HH
#define NODE_IMPL_HH

#include "Node.hh"
#include "NodeVariables.hh"
#include "Notifier.hh"

// for int32_t type
#ifdef HAVE_STDINT_H
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#elif defined(__VXWORKS__)
#include <vxWorks.h>
#endif

namespace PLEXIL
{

  /**
   * @class NodeImpl
   * @brief The innards shared between node implementation classes,
   *        the XML parser, and external interfaces; also the 
   *        implementation class for empty nodes.
   */

  class NodeImpl :
    public Node,
    public Notifier
  {
  public:

    static const int32_t WORST_PRIORITY;
    static char const * const ALL_CONDITIONS[];

    // N.B.: These need to match the order of ALL_CONDITIONS
    enum ConditionIndex {
                         // Conditions on parent
                         // N.B. Ancestor end/exit/invariant MUST come before
                         // end/exit/invariant, respectively, because the former depend
                         // on the latter and must be cleaned up first.
                         ancestorExitIdx = 0,
                         ancestorInvariantIdx,
                         ancestorEndIdx,
                         // User specified conditions
                         skipIdx,
                         startIdx,
                         preIdx,
                         exitIdx,
                         invariantIdx,
                         endIdx,
                         postIdx,
                         repeatIdx,
                         // For all but Empty nodes
                         actionCompleteIdx,
                         // For all but Empty and Update nodes
                         abortCompleteIdx,

                         conditionIndexMax
    };

    /**
     * @brief The constructor.
     * @param nodeId The name of this node.
     * @param parent The parent of this node (used for the ancestor conditions and variable lookup).
     */
    NodeImpl(char const *nodeId, NodeImpl *parent = NULL);

    /**
     * @brief Alternate constructor.  Used only by Exec test module.
     */
    NodeImpl(const std::string& type,
             const std::string& name,
             NodeState state,
             NodeImpl *parent = NULL);

    virtual ~NodeImpl();

    //
    // Listenable API
    //
    
    virtual bool isPropagationSource() const
    {
      return true;
    }

    // Override Notifier method
    virtual bool isActive() const
    {
      return true;
    }

    virtual void activate()
    {
    }

    virtual void deactivate()
    {
    }

    Node *next() const
    {
      return (Node *) m_next;
    }

    Node **nextPtr()
    {
      return (Node **) &m_next;
    }

    uint8_t getQueueStatus() const
    {
      return m_queueStatus;
    }

    void setQueueStatus(uint8_t newval)
    {
      m_queueStatus = newval;
    }

    std::string toString(const unsigned int indent = 0);
    void print(std::ostream& stream, const unsigned int indent = 0) const;

    // Make the node active.
    virtual void activateNode();

    std::string const &getNodeId() const { return m_nodeId; }

    /**
     * @brief Accessor for the Node's parent.
     * @return This node's parent.
     */
    NodeImpl *getParentNode() {return m_parent; }
    NodeImpl const *getParentNode() const {return m_parent; }

    /**
     * @brief Gets the destination state of this node, were it to transition,
     *        based on the values of various conditions.
     * @return True if the new destination state is different from the last check, false otherwise.
     * @note Sets m_nextState, m_nextOutcome, m_nextFailureType as a side effect.
     */
    bool getDestState();

    /**
     * @brief Gets the previously calculated destination state of this node.
     * @return The destination state.

     */
    NodeState getNextState() const 
    {
      return (NodeState) m_nextState;
    }

    /**
     * @brief Commit a pending state transition based on the statuses of various conditions.
     * @param time The time of the transition.
     */
    void transition(double time = 0.0); // FIXME - need a better representation

    /**
     * @brief Accessor for the priority of a node.  The priority is used to resolve resource conflicts.
     * @return the priority of this node.
     * @note Default method; only assignment nodes care about priority.
     */
    virtual int32_t getPriority() const {return WORST_PRIORITY;}

    // ExpressionListener API
    virtual void notifyChanged();

    /**
     * @brief Gets the current state of this node.
     * @return the current node state as a NodeState (enum) value.
     */
    NodeState getState() const;

    /**
     * @brief Gets the outcome of this node.
     * @return the current outcome as an enum value.
     */
    NodeOutcome getOutcome() const;

    /**
     * @brief Gets the failure type of this node.
     * @return the current failure type as an enum value.
     */
    FailureType getFailureType() const;

    /**
     * @brief Sets the state variable to the new state.
     * @param newValue The new node state.
     * @note Virtual so it can be overridden by ListNode wrapper method.
     * @note Only used by node implementation classes and unit tests.
     */
    virtual void setState(NodeState newValue, double tym); // FIXME

    // Used by unit tests
    void setNodeFailureType(FailureType f);

    /**
     * @brief Gets the time at which this node entered its current state.
     * @return Time value as a double.
     * @note Used by GanttListener and PlanDebugListener.
     */
    double getCurrentStateStartTime() const;

    /**
     * @brief Looks up a variable by name.
     * @param name Name of the variable.
     * @return The variable, or NULL if not found.
     * @note Used only by XML parser.
     */
    Expression *findVariable(char const *name);

    /**
     * @brief Find the named variable in this node, ignoring its ancestors.
     * @param name Name of the variable.
     * @return The variable, or NULL if not found.
     * @note Used only by XML parser.
     */
    Expression *findLocalVariable(char const *name);

    /**
     * @brief Accessor for an assignment node's assigned variable.
     * @note Default method, overridden by AssignmentNode.
     */
    virtual Expression *getAssignmentVariable() const 
    {
      return NULL;
    }

    /**
     * @brief Gets the type of this node.
     * @return The type of this node.
     * @note Empty node method.
     */
    virtual PlexilNodeType getType() const
    {
      return NodeType_Empty;
    }

    /**
     * @brief Gets the parent of this node.
     */
    Node const *getParent() const
    {
      return dynamic_cast<Node const *>(m_parent);
    }
  
    virtual std::vector<NodeImpl *>& getChildren();
    virtual const std::vector<NodeImpl *>& getChildren() const;

    virtual NodeImpl const *findChild(char const *childName) const;
    virtual NodeImpl *findChild(char const *childName);

    //
    // Utilities for plan parser and analyzer
    //

    // Pre-allocate local variable vector, variable map.
    void allocateVariables(size_t n);

    /**
     * @brief Add a named "variable" to the node, to be deleted with the node.
     * @param name The name
     * @param var The expression to associate with the name.
     *            It will be deleted when the node is deleted.
     * @return true if successful, false if name is a duplicate
     */
    bool addLocalVariable(char const *name, Expression *var);

    // May return NULL.
    // Used by GanttListener.
    NodeVariableMap const *getVariableMap() const { return m_variablesByName; }

    /**
     * @brief Add a condition expression to the node.
     * @param cname The name of the condition.
     * @param cond The expression.
     * @param isGarbage True if the expression should be deleted with the node.
     */
    void addUserCondition(char const *cname, Expression *cond, bool isGarbage);

    /**
     * @brief Construct any internal conditions now that the node is complete.
     */
    void finalizeConditions();
    
    // Public only for plan analyzer
    static char const *getConditionName(size_t idx);

    /**
     * @brief Gets the state variable representing the state of this node.
     * @return the state variable.
     */
    Expression *getStateVariable() { return &m_stateVariable; }
    Expression *getOutcomeVariable() { return &m_outcomeVariable; }
    Expression *getFailureTypeVariable() { return &m_failureTypeVariable; }

    // For use of plan parser.
    Expression *ensureTimepoint(NodeState st, bool isEnd);

    // May return NULL.
    // Used by plan analyzer and plan parser module test only.
    const std::vector<Expression *> *getLocalVariables() const { return m_localVariables; }

    // Condition accessors
    // These are public only to appease the module test

    // These conditions belong to the parent node.
    Expression *getAncestorEndCondition()               { return getCondition(ancestorEndIdx); }
    Expression *getAncestorExitCondition()              { return getCondition(ancestorExitIdx); }
    Expression *getAncestorInvariantCondition()         { return getCondition(ancestorInvariantIdx); }

    // User conditions
    Expression *getSkipCondition()                      { return m_conditions[skipIdx]; }
    Expression *getStartCondition()                     { return m_conditions[startIdx]; }
    Expression *getEndCondition()                       { return m_conditions[endIdx]; }
    Expression *getExitCondition()                      { return m_conditions[exitIdx]; }
    Expression *getInvariantCondition()                 { return m_conditions[invariantIdx]; }
    Expression *getPreCondition()                       { return m_conditions[preIdx]; }
    Expression *getPostCondition()                      { return m_conditions[postIdx]; }
    Expression *getRepeatCondition()                    { return m_conditions[repeatIdx]; }
    // These are for specialized node types
    Expression *getActionCompleteCondition()            { return m_conditions[actionCompleteIdx]; }
    Expression *getAbortCompleteCondition()             { return m_conditions[abortCompleteIdx]; }

    // Abstracts out the issue of where the condition comes from.
    // Used internally, also by LuvListener. Non-const variant is protected.
    Expression const *getCondition(size_t idx) const;

  protected:

    friend class ListNode;

    Expression *getCondition(size_t idx);

    // Only used by Node, ListNode, LibraryCallNode.
    virtual NodeVariableMap const *getChildVariableMap() const;

    // *** Seems to be called only from NodeImpl constructor?
    void commonInit();

    // Called from the transition handler
    void execute();
    void reset();
    virtual void abort();
    void deactivateExecutable();

    // Variables
    void activateLocalVariables();
    void deactivateLocalVariables();

    // Activate conditions

    // These are special because parent owns the condition expression
    void activateAncestorEndCondition();
    void activateAncestorExitInvariantConditions();

    // User conditions
    void activatePreSkipStartConditions();
    void activateEndCondition();
    void activateExitCondition();
    void activateInvariantCondition();
    void activatePostCondition();
    void activateRepeatCondition();
    // These are for specialized node types
    void activateActionCompleteCondition();
    void activateAbortCompleteCondition();

    // Deactivate a condition

    // These are special because parent owns the condition expression
    void deactivateAncestorEndCondition();
    void deactivateAncestorExitInvariantConditions();

    // User conditions
    void deactivatePreSkipStartConditions();
    void deactivateEndCondition();
    void deactivateExitCondition();
    void deactivateInvariantCondition();
    void deactivatePostCondition();
    void deactivateRepeatCondition();
    // These are for specialized node types
    void deactivateActionCompleteCondition();
    void deactivateAbortCompleteCondition();

    // Specific behaviors for derived classes
    virtual void specializedCreateConditionWrappers();
    virtual void specializedActivate();
    virtual void specializedHandleExecution();
    virtual void specializedDeactivateExecutable();

    //
    // State transition implementation methods
    //
    // Non-virtual member functions are common to all node types.
    // Virtual members are specialized by node type.
    //

    // getDestStateFrom...
    // Return true if the new destination state is different from the last check, false otherwise.
    // Set m_nextState, m_nextOutcome, m_nextFailureType as a side effect.
    bool getDestStateFromInactive();
    bool getDestStateFromWaiting();
    virtual bool getDestStateFromExecuting();
    virtual bool getDestStateFromFinishing();
    bool getDestStateFromFinished();
    virtual bool getDestStateFromFailing();
    bool getDestStateFromIterationEnded();

    //
    // Transition out of the named current state.
    void transitionFromInactive();
    void transitionFromWaiting();
    virtual void transitionFromExecuting();
    virtual void transitionFromFinishing();
    void transitionFromFinished();
    virtual void transitionFromFailing();
    void transitionFromIterationEnded();

    void transitionToInactive();
    void transitionToWaiting();
    virtual void transitionToExecuting();
    virtual void transitionToFinishing();
    void transitionToFinished();
    virtual void transitionToFailing();
    void transitionToIterationEnded(); 

    // Phases of destructor
    // Not useful if called from base class destructor!
    virtual void cleanUpConditions();
    void cleanUpVars();
    virtual void cleanUpNodeBody();

    // Printing utility
    virtual void printCommandHandle(std::ostream& stream, const unsigned int indent) const;

    //
    // Common state
    //

    Node   *m_next;                /*!< For LinkedQueue<Node> */
    uint8_t m_queueStatus;         /*!< Which exec queue the node is in, if any. */
    uint8_t m_state;               /*!< The current state of the node. */
    uint8_t m_outcome;             /*!< The current outcome. */
    uint8_t m_failureType;         /*!< The current failure. */

    bool m_pad; // to ensure 8 byte alignment
    uint8_t m_nextState;           /*!< The state returned by getDestState() the last time checkConditions() was called. */
    uint8_t m_nextOutcome;         /*!< The pending outcome. */
    uint8_t m_nextFailureType;     /*!< The pending failure. */

    NodeImpl *m_parent;                          /*!< The parent of this node.*/
    Expression *m_conditions[conditionIndexMax]; /*!< The condition expressions. */
 
    std::vector<Expression *> *m_localVariables; /*!< Variables created in this node. */
    StateVariable m_stateVariable;
    OutcomeVariable m_outcomeVariable;
    FailureVariable m_failureTypeVariable;
    NodeVariableMap *m_variablesByName; /*!< Locally declared variables or references to variables gotten through an interface. */
    std::string m_nodeId;  /*!< the NodeId from the xml.*/

  private:
    
    // Node transition history trace
    double m_currentStateStartTime;
    NodeTimepointValue *m_timepoints;

  protected:

    // Housekeeping details
    bool m_garbageConditions[conditionIndexMax]; /*!< Flags for conditions to delete. */
    bool m_cleanedConditions, m_cleanedVars, m_cleanedBody;

  private:

    void createConditionWrappers();

    // These should only be called from transition().
    void setNodeOutcome(NodeOutcome o);
    void transitionFrom();
    void transitionTo(double tym); // FIXME
    void logTransition(double time, NodeState newState);

    //
    // Internal versions
    //

    void printVariables(std::ostream& stream, const unsigned int indent = 0) const;

  };

}

#endif // NODE_IMPL_HH
