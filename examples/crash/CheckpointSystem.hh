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
#include "Value.hh"
#include <iostream>

#ifndef _H_CheckpointSystem
#define _H_CheckpointSystem

using std::string;
using std::vector;
using std::tuple;
using std::map;

// This is a class that stores, operates on, and provides information
// about crashes and checkpoints

class CheckpointSstem
{
public:

  //TODO: These are from PLEXIL-2 branch of sample-app, but why?
  CheckpointSystem (const CheckpointSystem&) = delete;
  CheckpointSystem& operator= (const CheckpointSystem&) = delete;
  ~CheckpointSystem();

  static CheckpointSystem *getInstance () {
    if (!m_system) {
      m_system = new CheckpointSystem;
    }
    return m_system;
  }
  
  //Lookups
  bool didCrash();
  int32_t numActiveCrashes();
  int32_t numTotalCrashes();
  Value getCheckpointState(const string checkpoint_name,int32_t boot_num);
  Value getCheckpointTime(const string checkpoint_name, int32_t boot_num);
  Value getTimeOfBoot(int32_t boot_num);
  Value getTimeOfCrash(int32_t boot_num);
  
  //Commands
  Value setCheckpoint(const string& checkpoint_name, bool value);
  Value setSafeReboot(bool b);
  Value deleteCrash(int32_t boot_num);

private:
  CheckpointSystem();

  static CheckpointSystem *m_system

  // Current boot information
  bool safe_to_reboot;
  bool did_crash;
  bool num_active_crashes;
  bool num_total_crashes;

  // Data structure that tracks boot-specific metadata and checkpoints
  vector<tuple<int, /*time of boot*/
	       int, /*time of crash*/
	       map< /*map of checkpoint info*/
		 const string&, /*checkpoint name*/
		 tuple<bool, /*state of checkpoint*/
		       int>> data_vector;/*time of checkpoint activation*/


  // Helper functions
  void load_crashes(const string& directory);
  bool valid_boot(int32_t boot_num);
  bool valid_checkpoint(const string checkpoint_name,int32_t boot_num);
};
#endif
