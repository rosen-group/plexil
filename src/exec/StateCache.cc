/* Copyright (c) 2006-2013, Universities Space Research Association (USRA).
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

#include "StateCache.hh"

#include "Debug.hh"
#include "Expression.hh"
#include "Expressions.hh"
#include "ExternalInterface.hh"

#include <cmath> // for fabs()
#include <iomanip> // for setprecision()
#include <limits>

namespace PLEXIL
{

  //
  // Helper function
  //

  /**
   * @brief Compute the magnitude of the difference between x and y.
   * @param x First value.
   * @param y Second value.
   * @return The magnitude of the difference.  If both x and y
   *         are UNKNOWN, the magnitude is 0.  If exactly one of
   *         them is UNKNOWN, the magnitude is inf.  Otherwise,
   *         it's abs(x - y).
   */
  static double differenceMagnitude(double x, double y)
  {
    if (x == Value::UNKNOWN_VALUE()) {
      if (y == Value::UNKNOWN_VALUE())
        return 0;
      else
        return std::numeric_limits<double>::max();
    }
    else if (y == Value::UNKNOWN_VALUE())
      return std::numeric_limits<double>::max();
    else
      return fabs(x - y);
  }

  //
  // Helper classes
  //

  class LookupDesc 
  {
  public:
    LookupDesc(const CacheEntryId& _entry,
               const ExpressionId& _expr)
      : entry(_entry),
        dest(_expr),
        m_id(this),
        previousValue(),
        tolerance(0.0),
        changeLookup(false)
    {
    }
    LookupDesc(const CacheEntryId& _entry,
               const ExpressionId& _expr, 
               const double& _tolerance)
      : entry(_entry),
        dest(_expr),
        m_id(this),
        previousValue(),
        tolerance(_tolerance),
        changeLookup(true)
    {
    }
    
    virtual ~LookupDesc()
    {
      m_id.remove();
    }

    const LookupDescId& getId() const
    {
      return m_id;
    }

    inline const bool& isChangeLookup() const
    {
      return changeLookup;
    }

    inline const double& getTolerance() const
    {
      return tolerance;
    }

    inline const Value& getPreviousValue() const
    {
      return previousValue;
    }

    inline void setPreviousValue(const Value& value)
    {
      previousValue = value;
    }

    void update(const Value& value)
    {
      if (previousValue.isUnknown()
          && !value.isUnknown()) {
          debugMsg("StateCache:updateState", "Updating because the previous value is UNKNOWN.");
      }
      else if (tolerance == 0.0) {
        debugMsg("StateCache:updateState",
                 "Updating because the value has changed and zero tolerance specified");
      }
      // FIXME!: Assumes all values are numeric
      else if (tolerance > differenceMagnitude(previousValue.getRawValue(),
                                               value.getRawValue())) {
        debugMsg("StateCache:updateState", 
                 "Not updating. All changes are within the tolerance.");
        return;
      }
      else {
        debugMsg("StateCache:updateState",
                 "Updating because the change in value exceeds tolerance " << 
                 tolerance);
      }
      check_error(dest.isValid());
      debugMsg("StateCache:updateState",
               "Updating " << (changeLookup ? "change " : "") << "lookup " <<
               dest->toString() << " from " << previousValue << " to " << value);
      dest->setValue(value);
      previousValue = value;
    }

    CacheEntryId entry; //* Backpointer to CacheEntry instance
    ExpressionId dest;  //* The Lookup expression

  private:
    // Not implemented
    LookupDesc();
    LookupDesc(const LookupDesc&);
    LookupDesc& operator=(const LookupDesc&);

    LookupDescId m_id;
    Value previousValue;
    double tolerance;
    bool changeLookup;  //* True if a change lookup, false otherwise
  };

  class CacheEntry
  {
  public:
    CacheEntry(const State& s)
    : state(s),
      value(),
      highThreshold(Value::UNKNOWN_VALUE()),
      lowThreshold(Value::UNKNOWN_VALUE()),
      lastQuiescence(-1),
      m_id(this)
    {
    }

    ~CacheEntry()
    {
      m_id.remove();
    }

    const CacheEntryId & getId() const
    {
      return m_id; 
    }


    /**
     * @brief Returns true if any change lookups are active on this state.
     */
    bool activeChangeLookups() const
    {
      for (std::set<LookupDescId>::const_iterator it = lookups.begin();
           it != lookups.end();
           ++it) {
        if ((*it)->isChangeLookup())
          return true;
      }
      return false;
    }

    /**
     * @brief Update all lookups with the new value.
     * @param newValue The latest value for this state.
     * @param quiescenceCount The current quiescence count.
     * @return True if the thresholds have changed, false otherwise.
     */
    bool update(const Value& newValue, int quiescenceCount)
    {
      value = newValue;
      lastQuiescence = quiescenceCount;

      for (std::set<LookupDescId>::const_iterator lit = lookups.begin();
           lit != lookups.end();
           ++lit) {
        // check_error(lit->isValid()); // *** only if paranoid
        (*lit)->update(value);
      }

      return calculateThresholds();
    }

    bool calculateThresholds()
    {
      debugMsg("CacheEntry:calculateThresholds",
               " for " << state.first.toString() << ", " << lookups.size() << " active lookups");
      double newHi = Value::UNKNOWN_VALUE();
      double newLo = Value::UNKNOWN_VALUE();

      for (std::set<LookupDescId>::const_iterator lit = lookups.begin();
           lit != lookups.end();
           ++lit) {
        LookupDescId lookup = *lit;
        // check_error(lookup.isValid()); // *** only if paranoid
        condDebugMsg(!lookup->isChangeLookup(),
                     "CacheEntry:calculateThresholds",
                     " entry is not a change lookup");
        if (lookup->isChangeLookup()) {
          if (lookup->getTolerance() == 0.0) {
            debugMsg("CacheEntry:calculateThresholds",
                     " returning false; at least one change lookup w/ no tolerance");
            highThreshold = lowThreshold = Value::UNKNOWN_VALUE();
            return false;
          }
          // Update thresholds based on lookup's last value
          double hi = lookup->getPreviousValue().getDoubleValue() + lookup->getTolerance();
          if (newHi == Value::UNKNOWN_VALUE() || hi < newHi) {
            debugMsg("CacheEntry:calculateThresholds",
                     " updating high threshold to " << Value::valueToString(hi));
            newHi = hi;
          }
          double lo = lookup->getPreviousValue().getDoubleValue() - lookup->getTolerance();
          if (newLo == Value::UNKNOWN_VALUE()
              || lo > newLo) {
            debugMsg("CacheEntry:calculateThresholds",
                     " updating low threshold to " << Value::valueToString(lo));
            newLo = lo;
          }
        }
      }

      bool changed = false;
      if (newHi != Value::UNKNOWN_VALUE()
          && newHi != highThreshold) {
        highThreshold = newHi;
        changed = true;
      }
      if (newLo != Value::UNKNOWN_VALUE()
          && newLo != lowThreshold) {
        lowThreshold = newLo;
        changed = true;
      }
      debugMsg("CacheEntry:calculateThresholds",
               " returning " << (changed ? "true" : "false"));
      return changed;
    }

    State state;
    std::set<LookupDescId> lookups;
    Value value;
    double highThreshold;
    double lowThreshold;
    int lastQuiescence;

  private:
    // Not implemented
    CacheEntry();
    CacheEntry(const CacheEntry&);
    CacheEntry& operator=(const CacheEntry&);

    CacheEntryId m_id;
  };

  StateCache::StateCache() 
    : m_id(this),
      m_inQuiescence(false), 
      m_quiescenceCount(0)
  {
    // Initialize the time state
    m_timeState.first = "time";
    m_timeEntry = ensureCacheEntry(m_timeState);
    m_timeEntry->value = 0.0;
  }

  // FIXME: should this unsubscribe from interface?
  StateCache::~StateCache()
  {
    // Delete states
    m_timeEntry = CacheEntryId::noId();
    for (StateCacheMap::const_iterator it = m_states.begin();
         it != m_states.end();
         ++it) {
      delete (CacheEntry*) it->second;
    }
    m_states.clear();

    // Delete lookups
    for (ExpressionToLookupMap::iterator it = 
           m_lookupsByExpression.begin(); 
         it != m_lookupsByExpression.end();
         ++it) {
      delete (LookupDesc*) it->second;
    }
    m_lookupsByExpression.clear();

    m_id.remove();
  }
   
  void StateCache::registerLookupNow(const ExpressionId& expr,
                                     const State& state)
  {
    check_error(m_inQuiescence, "Lookup outside of quiescence!");
    debugMsg("StateCache:lookupNow",
             "Looking up value for state " << toString(state)
             << " because of " << expr->toString());

    // Register the lookup for updates as long as it's active
    CacheEntryId entry = ensureCacheEntry(state);
    LookupDescId lookup = (new LookupDesc(entry, expr))->getId();
    m_lookupsByExpression[expr] = lookup;
    entry->lookups.insert(lookup);

    if (entry->lastQuiescence < m_quiescenceCount) {
      debugMsg("StateCache:registerLookupNow",
               (entry->lastQuiescence == -1 ? "New" : "Stale")
               << " state, so performing external lookup.");
      // Perform the lookup and propagate to anyone else listening to this state
      // Tell interface if change thresholds moved in the process
      if (internalStateUpdate(entry, m_interface->lookupNow(entry->state)))
        m_interface->setThresholds(state, entry->highThreshold, entry->lowThreshold);
    }
    // state is known and the cached value is current - return it
    else {
      debugMsg("StateCache:lookupNow", 
               "Already have up-to-date value for state, so using that ("
               << entry->value << ")");

      expr->setValue(entry->value);
    }
  }

  void StateCache::registerChangeLookup(const ExpressionId& expr,
                                        const State& state,
                                        double tolerance)
  {
    check_error(m_inQuiescence, "Lookup outside of quiescence!");
    debugMsg("StateCache:registerChangeLookup", "Registering change lookup " <<
             expr->toString() << " for state " << toString(state) <<
             " with tolerance (" << tolerance << ")");

    if (state == m_timeState) {
      assertTrueMsg(tolerance > 0, "LookupOnChange(time) requires a positive tolerance");
    }

    CacheEntryId entry = ensureCacheEntry(state);
    bool wasSubscribed = entry->activeChangeLookups();
    LookupDescId lookup = (new LookupDesc(entry, expr, tolerance))->getId();
    m_lookupsByExpression[expr] = lookup;
    entry->lookups.insert(lookup);
    if (!wasSubscribed) {
      // Tell the external interface to listen for changes on this state
      // FIXME: allow for exception here?
      m_interface->subscribe(entry->state);
    }

    bool thresholdUpdate = false;
    // Update the current value of the expression if needed
    if (entry->lastQuiescence < m_quiescenceCount) {
      debugMsg("StateCache:registerChangeLookup",
               (entry->lastQuiescence == -1 ? "New" : "Stale")
               << " state, so performing external lookup.");
      thresholdUpdate = 
        internalStateUpdate(entry, m_interface->lookupNow(entry->state));
    }
    else {
      debugMsg("StateCache:registerChangeLookup",
               "Already have up-to-date value for state, so using that (" <<
               entry->value << ")");
      lookup->setPreviousValue(entry->value);
      expr->setValue(entry->value);
      thresholdUpdate = entry->calculateThresholds();
    }
    // Update thresholds if required
    if (thresholdUpdate) {
      m_interface->setThresholds(state, entry->highThreshold, entry->lowThreshold);
    }
    else {
      debugMsg("StateCache:registerChangeLookup", " not updating thresholds");
    }
  }

  // FIXME: potential for race conditions on CacheEntry lookups?

  CacheEntryId StateCache::internalUnregisterLookup(const ExpressionId& source)
  {
    check_error(m_inQuiescence, "Lookup outside of quiescence!");
    check_error(source.isValid());
    ExpressionToLookupMap::iterator eit = m_lookupsByExpression.find(source);
    checkError(eit != m_lookupsByExpression.end(),
               "No stored lookup for " << source->toString());
    LookupDescId lookup = eit->second;
    CacheEntryId entry = lookup->entry;
    checkError(entry.isId(), "No state entry for " << source->toString());
    entry->lookups.erase(lookup);
    m_lookupsByExpression.erase(eit);
    delete (LookupDesc*) lookup;
    return entry;
  }

  void StateCache::unregisterChangeLookup(const ExpressionId& source)
  {
    debugMsg("StateCache:unregisterChangeLookup",
             " Removing change lookup " << source->toString());
    CacheEntryId entry = internalUnregisterLookup(source);
    check_error(entry.isId());
    if (!entry->activeChangeLookups()) {
      m_interface->unsubscribe(entry->state);
      entry->highThreshold = entry->lowThreshold = Value::UNKNOWN_VALUE();
    }
  }

  void StateCache::unregisterLookupNow(const ExpressionId& source)
  {
    internalUnregisterLookup(source);
  }

  void StateCache::updateState(const State& state, const Value& value)
  {
    check_error(!m_inQuiescence);
    CacheEntryId entry;
    StateCacheMap::const_iterator it = m_states.find(state);
    if (it == m_states.end()) {
      debugMsg("StateCache:updateState",
               " for previously unknown state " << toString(state) << ", creating entry");
      entry = ensureCacheEntry(state);
    }
    else {
      entry = it->second;
    }
    if (internalStateUpdate(entry, value))
      m_interface->setThresholds(state, entry->highThreshold, entry->lowThreshold);
  }

  bool StateCache::internalStateUpdate(const CacheEntryId& entry,
                                       const Value& value)
  {
    debugMsg("StateCache:updateState",
             "Updating state " << toString(entry->state) <<
             " with value " << value);
    return entry->update(value, m_quiescenceCount);
  }


  /**
   * @brief Get the last known value of the state.
   * @param state The state.
   * @return The value.
   */
  const Value& StateCache::getLastValue(const State& state)
  {
    check_error(m_inQuiescence);
    StateCacheMap::const_iterator it = m_states.find(state);
    if (it == m_states.end())
      return UNKNOWN();
    else
      return it->second->value;
  }

  /**
   * @brief Find or create the cache entry for this state.
   * @param state The state being looked up.
   * @return Pointer to the CacheEntryId for the state.
   */

  CacheEntryId StateCache::ensureCacheEntry(const State& state)
  {
    StateCacheMap::const_iterator it = m_states.find(state);
    if (it != m_states.end()) {
      debugMsg("StateCache:ensureCacheEntry", " Found existing state " << toString(state));
      return it->second;
    }

    // Create new entry
    debugMsg("StateCache:ensureCacheEntry",
             " Allocating cache entry for state " << toString(state));
    CacheEntryId entry = m_states[state] = (new CacheEntry(state))->getId();
    return entry;
  }

  void StateCache::handleQuiescenceStarted()
  {
    check_error(!m_inQuiescence);
    m_inQuiescence = true;
    if (m_timeEntry->lastQuiescence < m_quiescenceCount)
      updateTimeState();
  }

  void StateCache::handleQuiescenceEnded()
  {
    check_error(m_inQuiescence);
    ++m_quiescenceCount;
    checkError(m_quiescenceCount > 0, "Quiescence counter wrapped around!");
    m_inQuiescence = false;
  }

  const Value& StateCache::currentTime()
  {
    if (!m_inQuiescence) 
      updateTimeState();
    return m_timeEntry->value;
  }

  void StateCache::updateTimeState()
  {
    double time = m_interface->lookupNow(m_timeState).getDoubleValue();
    checkError(m_timeEntry->value.getDoubleValue() <= time,
               "Time has regressed from "
               << std::setprecision(15) << m_timeEntry->value.getDoubleValue() << " to " << time);
    if (internalStateUpdate(m_timeEntry, time))
      m_interface->setThresholds(m_timeState, m_timeEntry->highThreshold, m_timeEntry->lowThreshold);
  }

  const State& StateCache::getTimeState() const
  {
    return m_timeState;
  }

  std::string StateCache::toString(const State& state)
  {
    std::ostringstream str;
    str << state.first.toString() << "(";
    for (size_t i = 0; i < state.second.size(); ++i) {
      if (i != 0)
        str << ", ";
      str << state.second[i];
    }
    str << ")";
    return str.str();
  }

}
