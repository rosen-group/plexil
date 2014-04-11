/* Copyright (c) 2006-2014, Universities Space Research Association (USRA).
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

#ifndef VARIABLE_HH
#define VARIABLE_HH

#include "Expression.hh"
#include "PlexilPlan.hh"
#include "lifecycle-utils.h"

// Macro for creating named static "constant" variables
#define DECLARE_STATIC_CLASS_EXPRESSION_ID_CONSTANT(TYPE, FN_NAME, VALUE, PRINT_NAME) \
private: \
  static TYPE* ensure__ ## FN_NAME() { \
    static TYPE *sl_ptr; \
    static bool sl_inited; \
    if (!sl_inited) { \
      sl_ptr = new TYPE(VALUE, true); \
      sl_ptr->setName(PRINT_NAME); \
      addFinalizer(&FN_NAME ## __destroy); \
      sl_inited = true; \
    } \
    return sl_ptr; \
  } \
  static void FN_NAME ## __destroy() { \
    delete ensure__ ## FN_NAME(); \
  } \
public: \
  static const ExpressionId& FN_NAME() { \
    static ExpressionId sl_id = (ensure__ ## FN_NAME())->getId(); \
    if (!sl_id->isActive()) \
      sl_id->activate(); \
    return sl_id; \
  }

namespace PLEXIL
{

  /**
   * An abstract base class representing anything that can be on the left side
   * of an assignment, including but not limited to actual variables, 
   * variable aliases, array elements, etc.
   */

  class Variable :
    public virtual Expression
  {
  public:
    /**
     * @brief Default constructor.
     */
    Variable();

    /**
     * @brief Destructor.
     */
    virtual ~Variable();
    
    /**
     * @brief Check to make sure a value is appropriate for this expression.
     * @param value The new value for this variable.
     */
    virtual bool checkValue(const Value& value) const = 0;

    /**
     * @brief Set the value of this expression back to the initial value with which it was
     *        created.
     */
    virtual void reset() = 0;

    /**
     * @brief Sets the value of this variable.  Will throw an error if the variable was
     *        constructed with isConst == true.
     * @param value The new value for this variable.
     */
    virtual void setValue(const Value& value) = 0;

    /**
     * @brief Temporarily stores the previous value of this variable.
     * @note Used to implement recovery from failed Assignment nodes.
     */
    virtual void saveCurrentValue() = 0;

    /**
     * @brief Restore the value set aside by saveCurrentValue().
     * @note Used to implement recovery from failed Assignment nodes.
     * @note The default method should be appropriate for most derived classes.
     */
    virtual void restoreSavedValue();
     
    /**
     * @brief Commit the assignment by erasing the saved previous value.
     * @note Used to implement recovery from failed Assignment nodes.
     */
    virtual void commitAssignment() = 0;

    /**
     * @brief Get the saved value.
     * @return The saved value.
     */
    virtual const Value& getSavedValue() const = 0;

    /**
     * @brief Get the name of this variable, as declared in the node that owns it.
     */
    virtual const std::string& getName() const = 0;

    /**
     * @brief Get the LabelStr key of this variable's name, as declared in the node that owns it.
     */
    virtual double getNameKey() const = 0;

    /**
     * @brief Gets the const-ness of this variable.
     * @return True if this variable is const, false otherwise.
     */
    virtual bool isConst() const = 0;

    /**
     * @brief Get the node that owns this expression.
     * @return The NodeId of the parent node; may be noId.
     * @note Used by LuvFormat::formatAssignment().  
     * @note Default method.
     */
    virtual const NodeId& getNode() const
    {
      return NodeId::noId(); 
    }

    const VariableId& getId() const {return m_evid;}

    /**
     * @brief Get the real variable for which this may be a proxy.
     * @return The VariableId of the base variable
     * @note Used by the assignment node conflict resolution logic.
     */
    virtual const VariableId& getBaseVariable() const = 0;

  protected:

  private:
    VariableId m_evid;
  };

  /**
   *   A class for notifying derived variables (e.g. array variables,
   *   variable aliases, etc.) of changes in sub-expressions.
   */
  class DerivedVariableListener :
    public ExpressionListener 
  {
  public:

    /**
     * @brief Constructor.
     * @param exp The expression to be notified of any changes.
     */
    DerivedVariableListener(const ExpressionId& exp)
      : ExpressionListener(),
        m_exp(exp)
    {}

    /**
     * @brief Notifies the destination expression of a value change.
     * @param exp The expression which has changed.
     */
    void notifyValueChanged(const ExpressionId& exp)
    {
      // prevent infinite loop
      // FIXME: how is infinite loop possible? any other way to break it?
      if (exp != m_exp)
        m_exp->handleChange(exp);
    }

  private:

    // deliberately unimplemented
    DerivedVariableListener();
    DerivedVariableListener(const DerivedVariableListener&);
    DerivedVariableListener& operator=(const DerivedVariableListener&);

    ExpressionId m_exp; /*<! The destination expression for notifications. */
  };

  /**
   * An abstract base class representing a variable with a single value.
   * Derived classes are specialized by value type.
   */
  class VariableImpl : public virtual Variable
  {
  public:

    /**
     * @brief Constructor.  Creates a variable that is initially UNKNOWN.
     * @param isConst True if this variable should have a constant value, false otherwise.
     */
    VariableImpl(const bool isConst = false);

    /**
     * @brief Constructor.  Creates a variable with a given value.
     * @param value The initial value of the variable.
     * @param isConst True if this variable should have a constant value, false otherwise.
     */
    VariableImpl(const Value& value, const bool isConst = false);

    /**
     * @brief Constructor.  Creates a variable from XML.
     * @param expr The PlexilExprId for this variable.
     * @param node A connection back to the node that created this variable.
     * @param isConst True if this variable should have a constant value, false otherwise.
     */
    VariableImpl(const PlexilExprId& expr, const NodeConnectorId& node,
                 const bool isConst = false);

    virtual ~VariableImpl();

    /**
     * @brief Get a string representation of this Expression.
     * @return The string representation.
     */
    void print(std::ostream& s) const;

    /**
     * @brief Print the variable's value to the given stream.
     * @param s The output stream.
     */
    virtual void printValue(std::ostream& s) const;

    /**
     * @brief Get a string representation of the value of this Variable.
     * @return The string representation.
     * @note This method always uses the stored value whether or not the variable is active,
     *       unlike the base class method.
     */
    virtual std::string valueString() const;

    /**
     * @brief Set the value of this expression back to the initial value with which it was
     *        created.
     */
    virtual void reset();

    /**
     * @brief Sets the value of this variable.  Will throw an error if the variable was
     *        constructed with isConst == true.
     * @param value The new value for this variable.
     */
    virtual void setValue(const Value& value);

    /**
     * @brief Temporarily stores the previous value of this variable.
     * @note Used to implement recovery from failed Assignment nodes.
     */
    virtual void saveCurrentValue();
     
    /**
     * @brief Commit the assignment by erasing the saved previous value.
     * @note Used to implement recovery from failed Assignment nodes.
     */
    virtual void commitAssignment();

    /**
     * @brief Get the saved value.
     * @return The saved value.
     * @note Intended for debug display only.
     */
    const Value& getSavedValue() const
    {
      return m_savedValue;
    }

    /**
     * @brief Gets the const-ness of this variable.
     * @return True if this variable is const, false otherwise.
     */
    bool isConst() const {return m_isConst;}

    /**
     * @brief Make this variable const.
     * @note Required by various derived constructors.
     */
    void makeConst();

    /**
     * @brief Get the node that owns this expression.
     * @return The NodeId of the parent node; may be noId.
     * @note Used by LuvFormat::formatAssignment().  
     */
    const NodeId& getNode() const { return m_node; }

    /**
     * @brief Gets the initial value of this variable.
     * @return The initial value of this variable.
     */
    const Value& initialValue() const {return m_initialValue;}

    /**
     * @brief Set the name of this variable.
     */
    void setName(const std::string& name)
    {
      m_name = name;
    }

    /**
     * @brief Get the name of this variable, as declared in the node that owns it.
     */
    const std::string& getName() const
    {
      return m_name.toString();
    }

    /**
     * @brief Get the LabelStr key of this variable's name, as declared in the node that owns it.
     */
    double getNameKey() const
    {
      return m_name.getKey(); 
    }

    /**
     * @brief Add a listener for changes to this Expression's value.
     * @param id The Id of the listener to notify.
     * @note Overrides method on Expression base class.
     */
    virtual void addListener(ExpressionListenerId id);

    /**
     * @brief Remove a listener from this Expression.
     * @param id The Id of the listener to remove.
     * @note Overrides method on Expression base class.
     */
    virtual void removeListener(ExpressionListenerId id);

    /**
     * @brief Get the real variable for which this may be a proxy.
     * @return The VariableId of the base variable
     * @note Used by the assignment node conflict resolution logic.
     */
    virtual const VariableId& getBaseVariable() const
    {
      return Variable::getId(); 
    }

  protected:

    /**
     * @brief Ensure that, if a variable is constant, it is never really deactivated
     */
    virtual void handleDeactivate(const bool changed);

    const ExecListenerHubId& getExecListenerHub(); /*<! Listener for publishing assignment notifications. */

    //
    // Shared member variables
    //

    const NodeId m_node; /*<! The node that owns this variable */
    Value m_initialValue; /*<! The initial value of the expression */
    Value m_savedValue;   /*<! The value saved during an Assignment node. */
    LabelStr m_name; /*<! The name under which this variable was declared */
    bool m_isConst; /*<! Flag indicating the const-ness of this variable */
  };

  class AliasVariable : public virtual Variable
  {
  public:
    /**
     * @brief Constructor. Creates a variable-like object that wraps another expression.
     * @param name The name of this variable in the node that constructed the alias.
     * @param node The node which owns this alias.
     * @param original The original expression for this alias.
     * @param isConst True if assignments to the alias are forbidden.
     */
    AliasVariable(const std::string& name, 
                  const NodeConnectorId& nodeConnector,
                  const ExpressionId& exp,
                  bool expIsGarbage,
                  bool isConst);

    virtual ~AliasVariable();

    /**
     * @brief Get a string representation of this Expression.
     * @return The string representation.
     */
    void print(std::ostream& s) const;

    /**
     * @brief Set the value of this expression back to the initial value with which it was
     *        created.
     */
    virtual void reset();

    /**
     * @brief Retrieve the value type of this Expression.
     * @return The value type of this Expression.
     */
    virtual PlexilType getValueType() const;
    
    /**
     * @brief Check to make sure a value is appropriate for this expression.
     */
    virtual bool checkValue(const Value& val) const;

    /**
     * @brief Sets the value of this variable.  Will throw an error if the variable was
     *        constructed with isConst == true.
     * @param value The new value for this variable.
     */
    virtual void setValue(const Value& value);

    /**
     * @brief Temporarily stores the previous value of this variable.
     * @note Used to implement recovery from failed Assignment nodes.
     */
    virtual void saveCurrentValue();

    /**
     * @brief Restore the value set aside by saveCurrentValue().
     * @note Used to implement recovery from failed Assignment nodes.
     */
    virtual void restoreSavedValue();
     
    /**
     * @brief Commit the assignment by erasing the saved previous value.
     * @note Used to implement recovery from failed Assignment nodes.
     */
    virtual void commitAssignment();

    /**
     * @brief Get the saved value.
     * @return The saved value.
     * @note Intended for debug display only.
     */
    virtual const Value& getSavedValue() const;

    /**
     * @brief Notify this expression that a subexpression's value has changed.
     * @param exp The changed subexpression.
     */
    virtual void handleChange(const ExpressionId& exp);

    /**
     * @brief Gets the const-ness of this variable.
     * @return True if this variable is const, false otherwise.
     */
    bool isConst() const {return m_isConst;}

    /**
     * @brief Get the node that owns this expression.
     * @return The NodeId of the parent node; may be noId.
     * @note Used by LuvFormat::formatAssignment().  
     */
    const NodeId& getNode() const { return m_node; }

    /**
     * @brief Get the name of this alias, as declared in the node that owns it.
     */
    const std::string& getName() const
    {
      return m_name.toString(); 
    }

    /**
     * @brief Get the LabelStr key of this alias's name, as declared in the node that owns it.
     */
    double getNameKey() const
    {
      return m_name.getKey(); 
    }

    /**
     * @brief Get the real variable for which this may be a proxy.
     * @return The VariableId of the base variable
     * @note Used by the assignment node conflict resolution logic.
     */
    virtual const VariableId& getBaseVariable() const;

  protected:

    /**
     * @brief Handle the activation of the expression.
     * @param changed True if the call to activate actually caused a change from inactive to
     *                active.
     */
    virtual void handleActivate(const bool changed);

    /**
     * @brief Handle the deactivation of the expression
     * @param changed True if the call to deactivate actually caused a change from active to
     *                inactive.
     */
    virtual void handleDeactivate(const bool changed);

  private:
    
    // Deliberately unimplemented
    AliasVariable();
    AliasVariable(const AliasVariable&);
    AliasVariable& operator=(const AliasVariable&);

    // Private member variables
    VariableId m_originalExpression;
    DerivedVariableListener m_listener;
    const NodeId m_node;
    const LabelStr m_name;
    bool m_isGarbage, m_isConst;
  };

}

#endif // VARIABLE_HH
