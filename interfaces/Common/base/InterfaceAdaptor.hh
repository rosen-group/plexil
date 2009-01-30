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

#ifndef INTERFACE_ADAPTOR_H
#define INTERFACE_ADAPTOR_H

#include "Id.hh"
#include "ExecDefs.hh"
#include <list>
#include <map>
#include <set>
#include <vector>

// forward references without namespace
class TiXmlElement;

namespace PLEXIL
{
  // forward references
  class InterfaceAdaptor;
  typedef Id<InterfaceAdaptor> InterfaceAdaptorId;

  class Expression;
  typedef Id<Expression> ExpressionId;

  class LabelStr;

  class AdaptorExecInterface;

  /**
   * @brief An abstract base class for interfacing the PLEXIL Universal Exec
            to other systems. See also classes ExternalInterface and AdaptorExecInterface.
  */
  class InterfaceAdaptor
  {

  public:
    /**
     * @brief Default constructor.
     * @note Deprecated; a future version will require a reference to the
     *       parent AdaptorExecInterface instance.
     */
    InterfaceAdaptor();

    /**
     * @brief Constructor.
     * @param execInterface A reference to the AdaptorExecInterface which owns this adaptor.
     */
    InterfaceAdaptor(AdaptorExecInterface& execInterface);

    /**
     * @brief Constructor from configuration XML.
     * @param xml A const pointer to the TiXmlElement describing this adaptor
     * @note The instance maintains a shared pointer to the TiXmlElement.
     */
    InterfaceAdaptor(AdaptorExecInterface& execInterface, 
		     const TiXmlElement* xml);

    /**
     * @brief Destructor.
     */
    virtual ~InterfaceAdaptor();

    /**
     * @brief Register one LookupOnChange.
     * @param uniqueId The unique ID of this lookup.
     * @param stateKey The state key for this lookup.
     * @param tolerances A vector of tolerances for the LookupOnChange.
     * @note Derived classes may implement this method.  The default method causes an assertion to fail.
     */

    virtual void registerChangeLookup(const LookupKey& uniqueId,
				      const StateKey& stateKey,
				      const std::vector<double>& tolerances);

    /**
     * @brief Terminate one LookupOnChange.
     * @param uniqueId The unique ID of the lookup to be terminated.
     * @note Derived classes may implement this method.  The default method causes an assertion to fail.
     */

    virtual void unregisterChangeLookup(const LookupKey& uniqueId);

    /**
     * @brief Register one LookupWithFrequency.
     * @param uniqueId The unique ID of this lookup.
     * @param stateKey The state key for this lookup.
     * @param lowFrequency The maximum interval in seconds between lookups.
     * @param highFrequency The minimum interval in seconds between lookups.
     * @note Derived classes may implement this method.  The default method causes an assertion to fail.
     */

    virtual void registerFrequencyLookup(const LookupKey& uniqueId,
					 const StateKey& stateKey,
					 double lowFrequency, 
					 double highFrequency);

    /**
     * @brief Terminate one LookupWithFrequency.
     * @param uniqueId The unique ID of the lookup to be terminated.
     * @note Derived classes may implement this method.  The default method causes an assertion to fail.
     */

    virtual void unregisterFrequencyLookup(const LookupKey& uniqueId);

    /**
     * @brief Perform an immediate lookup of the requested state.
     * @param stateKey The state key for this lookup.
     * @param dest A (reference to a) vector of doubles where the result is to be stored.
     * @note Derived classes may implement this method.  The default method causes an assertion to fail.
     */

    virtual void lookupNow(const StateKey& stateKey,
			   std::vector<double>& dest);

    /**
     * @brief Send the name of the supplied node, and the supplied value pairs, to the planner.
     * @param node The Node requesting the update.
     * @param valuePairs A map of <LabelStr key, value> pairs.
     * @param ack The expression in which to store an acknowledgement of completion.
     * @note Derived classes may implement this method.  The default method causes an assertion to fail.
     */

    virtual void sendPlannerUpdate(const NodeId& node,
				   const std::map<double, double>& valuePairs,
				   ExpressionId ack);

    /**
     * @brief Execute a command with the requested arguments.
     * @param name The LabelString representing the command name.
     * @param args The command arguments expressed as doubles.
     * @param dest The expression in which to store any value returned from the command.
     * @param ack The expression in which to store an acknowledgement of command transmission.
     * @note Derived classes may implement this method.  The default method causes an assertion to fail.
     */

    virtual void executeCommand(const LabelStr& name,
				const std::list<double>& args,
				ExpressionId dest,
				ExpressionId ack);

    /**
     * @brief Execute a function with the requested arguments.
     * @param name The LabelString representing the command name.
     * @param args The command arguments expressed as doubles.
     * @param dest The expression in which to store any value returned from the function.
     * @param ack The expression in which to store an acknowledgement of function transmission.
     * @note Derived classes may implement this method.  The default method causes an assertion to fail.
     */

    virtual void executeFunctionCall(const LabelStr& name,
				     const std::list<double>& args,
				     ExpressionId dest,
				     ExpressionId ack);

    /**
     * @brief Abort the pending command with the supplied name and arguments.
     * @param name The LabelString representing the command name.
     * @param args The command arguments expressed as doubles.
     * @param ack The expression in which to store an acknowledgement of command abort.
     * @note Derived classes may implement this method.  The default method causes an assertion to fail.
     */

    virtual void invokeAbort(const LabelStr& name, 
			     const std::list<double>& args, 
			     ExpressionId ack);

    //
    // Methods provided by the base class to facilitate implementations
    //

    /**
     * @brief Register one asynchronous lookup, so that other lookups using the same state will share data.
     * @param uniqueId The unique ID of this lookup.
     * @param stateKey The state key for this lookup.
     */
    void registerAsynchLookup(const LookupKey& uniqueId,
			      const StateKey& stateKey);
    
    /**
     * @brief Unregister one asynchronous lookup.
     * @param uniqueId The unique ID of this lookup.
     */
    void unregisterAsynchLookup(const LookupKey& uniqueId);

    /**
     * @brief Get the ID of this instance.
     */
    InterfaceAdaptorId getId()
    {
      return m_id;
    }

    /**
     * @brief Get the configuration XML for this instance.
     */
    const TiXmlElement* getXml()
    {
      return m_xml;
    }

    /**
     * @brief Get the AdaptorExecInterface for this instance.
     */
    AdaptorExecInterface& getExecInterface()
    {
      return m_execInterface;
    }

  protected:

    //
    // Methods provided by the base class to facilitate implementations
    //

    /**
     * @brief Get an iterator that points to the start of the asynchronous lookup map.
     */
    std::map<StateKey, std::set<LookupKey> >::const_iterator getAsynchLookupsBegin();

    /**
     * @brief Get an iterator that points to the end of the asynchronous lookup map.
     */
    std::map<StateKey, std::set<LookupKey> >::const_iterator getAsynchLookupsEnd();

    /**
     * @brief Given a state key, fetch the corresponding state.
     * @return True if the state was found, false otherwise.
     */
    bool getState(const StateKey& key, State& state);

    /**
     * @brief Given a state, fetch the corresponding state key.
     * @return True if the key was found, false otherwise.
     */
    bool getStateKey(const State& state, StateKey& stateKey);

    AdaptorExecInterface& m_execInterface;

  private:

    // Deliberately unimplemented
    InterfaceAdaptor(const InterfaceAdaptor &);
    InterfaceAdaptor & operator=(const InterfaceAdaptor &);

    const TiXmlElement* m_xml;

    InterfaceAdaptorId m_id;

    std::map<StateKey, std::set<LookupKey> > m_asynchLookups;

  };

  typedef Id<InterfaceAdaptor> InterfaceAdaptorId;
}

#endif // INTERFACE_ADAPTOR_H
