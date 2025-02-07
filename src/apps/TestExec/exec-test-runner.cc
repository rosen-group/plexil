/* Copyright (c) 2006-2022, Universities Space Research Association (USRA).
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

#include "plexil-config.h"
#include "Debug.hh"
#include "Error.hh"
#include "ExecListenerHub.hh"
#include "lifecycle-utils.h"
#include "Logging.hh"
#include "NodeImpl.hh"
#include "parseNode.hh"
#include "parsePlan.hh"
#include "parser-utils.hh"
#include "planLibrary.hh"
#include "PlexilExec.hh"
#include "PlexilSchema.hh"
#include "ResourceArbiterInterface.hh"
#include "TestExternalInterface.hh"

#ifdef HAVE_DEBUG_LISTENER
#include "PlanDebugListener.hh"
#endif

#ifdef HAVE_LUV_LISTENER
#include "LuvListener.hh"
#endif

#include <fstream>
#include <string>

#include <cstring>

using std::endl;
using std::set;
using std::string;
using std::vector;

using namespace PLEXIL;

static int run(int argc, char** argv);

int main(int argc, char** argv)
{
  int result = run(argc, argv);
  plexilRunFinalizers();
  return result;
}

int run(int argc, char** argv) 
{
  string scriptName("error");
  string planName("error");
  string debugConfig("Debug.cfg");
  string resourceFile("resource.data");
  vector<string> libraryNames;
  vector<string> libraryPaths;
  string
    usage("Usage: exec-test-runner -s <script> -p <plan>\n\
                        [-l <library-file>]*     (no default)\n\
                        [-L <library-dir>]*      (default .)\n\
                        [-d <debug_config_file>] (default ./Debug.cfg)\n\
                        [+d]                     (disable debug messages)\n\
                        [-r <resource_file>]     (default ./resource.data)\n\
                        [+r]                     (don't read resource data)\n");

#ifdef HAVE_LUV_LISTENER
  string luvHost = LUV_DEFAULT_HOSTNAME;
  int luvPort = LUV_DEFAULT_PORT;
  bool luvRequest = false;
  bool luvBlock = false;
  usage += "                        [-v [-h <viewer-hostname>] [-n <viewer-portnumber>] [-b] ]\n";
#endif

  bool debugConfigSupplied = false;
  bool useDebugConfig = true;
  bool resourceFileSupplied = false;
  bool useResourceFile = true;

  // if not enough parameters, print usage

  if (argc < 5) {
    if (argc >= 2 && strcmp(argv[1], "-h") == 0) {
      // print usage and exit
      std::cout << usage << std::endl;
      return 0;
    }
    warn("Not enough arguments.\n At least the -p and -s arguments must be provided.\n" << usage);
    return 2;
  }
  // parse out parameters

  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-p") == 0) {
      if (argc == (++i)) {
        warn("Missing argument to the " << argv[i-1] << " option.\n"
             << usage);
        return 2;
      }
      planName = argv[i];
    }
    else if (strcmp(argv[i], "-s") == 0) {
      if (argc == (++i)) {
        warn("Missing argument to the " << argv[i-1] << " option.\n"
             << usage);
        return 2;
      }
      scriptName = argv[i];
    }
    else if (strcmp(argv[i], "-l") == 0) {
      if (argc == (++i)) {
        warn("Missing argument to the " << argv[i-1] << " option.\n"
             << usage);
        return 2;
      }
      libraryNames.push_back(argv[i]);
    }
    else if (strcmp(argv[i], "-L") == 0) {
      if (argc == (++i)) {
        warn("Missing argument to the " << argv[i-1] << " option.\n"
             << usage);
        return 2;
      }
      libraryPaths.push_back(argv[i]);
    }
    else if (strcmp(argv[i], "-d") == 0) {
      if (!useDebugConfig) {
        warn("Both -d and +d options specified.\n"
             << usage);
        return 2;
      }
      else if (debugConfigSupplied) {
        warn("Multiple -d options specified.\n"
             << usage);
        return 2;
      }
      else if (argc == (++i)) {
        warn("Missing argument to the " << argv[i-1] << " option.\n"
             << usage);
        return 2;
      }
      debugConfig = string(argv[i]);
      useDebugConfig = true;
      debugConfigSupplied = true;
    }
    else if (strcmp(argv[i], "+d") == 0) {
      if (debugConfigSupplied) {
        warn("Both -d and +d options specified.\n"
             << usage);
        return 2;
      }
      debugConfig.clear();
      useDebugConfig = false;
    }
    else if (strcmp(argv[i], "-r") == 0) {
      if (!useResourceFile) {
        warn("Both -r and +r options specified.\n"
             << usage);
        return 2;
      }
      else if (resourceFileSupplied) {
        warn("Multiple -r options specified.\n"
             << usage);
        return 2;
      }
      else if (argc == (++i)) {
        warn("Missing argument to the " << argv[i-1] << " option.\n"
             << usage);
        return 2;
      }
      resourceFile = string(argv[i]);
      useResourceFile = true;
      resourceFileSupplied = true;
    }
    else if (strcmp(argv[i], "+r") == 0) {
      if (resourceFileSupplied) {
        warn("Both -r and +r options specified.\n"
             << usage);
        return 2;
      }
      resourceFile.clear();
      useResourceFile = false;
    }
#ifdef HAVE_LUV_LISTENER
    else if (strcmp(argv[i], "-v") == 0)
      luvRequest = true;
    else if (strcmp(argv[i], "-b") == 0)
      luvBlock = true;
    else if (strcmp(argv[i], "-h") == 0) {
      if (argc == (++i)) {
        warn("Missing argument to the " << argv[i-1] << " option.\n"
             << usage);
        return 2;
      }
      luvHost = argv[i];
    }
    else if (strcmp(argv[i], "-n") == 0) {
      if (argc == (++i)) {
        warn("Missing argument to the " << argv[i-1] << " option.\n"
             << usage);
        return 2;
      }
      std::istringstream buffer(argv[i]);
      buffer >> luvPort;
      SHOW(luvPort);
    } 
#endif
    else if (strcmp(argv[i], "-log") == 0) {
      if (argc == (++i)) {
        warn("Missing argument to the " << argv[i-1] << " option.\n"
             << usage);
        return 2;
      }
      Logging::ENABLE_LOGGING = 1;
      Logging::set_log_file_name(argv[i]);
    }
    else if (strcmp(argv[i], "-eprompt") == 0)
      Logging::ENABLE_E_PROMPT = 1;
    else if (strcmp(argv[i], "-wprompt") == 0)
      Logging::ENABLE_W_PROMPT = 1;
    else {
      warn("Unknown option '" << argv[i] << "'.  " << usage);
      return 2;
    }
  }

  // if no plan or script supplied, error out
  if (scriptName == "error") {
    warn("No -s option found.\n" << usage);
    return 2;
  }
  if (planName == "error") {
    warn("No -p option found.\n" << usage);
    return 2;
  }

  if (Logging::ENABLE_LOGGING) {

#ifdef __linux__
    Logging::print_to_log(argv, argc);
#endif
#ifdef __APPLE__
    string cmd = "user command: ";
    for (int i = 1; i < argc; ++i)
      cmd = cmd + argv[i] + " ";

    Logging::print_to_log(cmd.c_str());
#endif
  }

  // basic initialization

  // initialize debug messaging first
  if (useDebugConfig) {
    std::ifstream config(debugConfig.c_str());
    if (config.good())
      readDebugConfigStream(config);
  }

  setLibraryPaths(libraryPaths);

  // create external interface

  TestExternalInterface intf;
  g_dispatcher = &intf;

  // create the exec

  g_exec = makePlexilExec();
  g_exec->setDispatcher(g_dispatcher);
  ExecListenerHub hub;
  g_exec->setExecListener(&hub);
  if (useResourceFile) {
    g_exec->getArbiter()->readResourceHierarchyFile(resourceFile);
  }


#ifdef HAVE_DEBUG_LISTENER
  // add the debug listener
  hub.addListener(makePlanDebugListener());
#endif

#ifdef HAVE_LUV_LISTENER
  // if a Plexil Viewer is to be attached
  if (luvRequest) {
    // create and add luv listener
    LuvListener* ll = makeLuvListener(luvHost.c_str(), luvPort, luvBlock);
    if (ll->start()) {
      hub.addListener(ll);
    }
    else {
      warn("WARNING: Unable to connect to Plexil Viewer at "
           << luvHost << ":" << luvPort
           << "\nExecution will continue without the viewer.");
      delete ll;
    }
  }
#endif

  // if specified on command line, load libraries
  for (vector<string>::const_iterator libraryName = libraryNames.begin(); 
       libraryName != libraryNames.end();
       ++libraryName) {
    std::string fname = *libraryName;
    if (fname.rfind(".plx") == std::string::npos)
      fname += ".plx";
    
    Library const *l;
    try {
      l = loadLibraryNode(fname.c_str());
      if (!l) {
        warn("Unable to find file for library " << *libraryName);
        
        // Clean up
        delete g_exec;
        g_exec = nullptr;
        g_dispatcher = nullptr;

        return 1;
      }
    }
    catch (ParserException const &e) {
      warn("Error while reading library " << *libraryName << ": \n" << e.what());

      // Clean up
      delete g_exec;
      g_exec = nullptr;
      g_dispatcher = nullptr;

      return 1;
    }
  }

  // Load the plan
  {
    pugi::xml_document *planDoc;
    try {
      planDoc = loadXmlFile(planName);
      if (!planDoc)
        warn("Error: plan file " << planName << " not found or not readable");
    }
    catch (ParserException const &e) {
      warn("Error loading plan file '" << planName
           << "):\n" << e.what());
    }

    if (!planDoc) {
      // Clean up
      delete g_exec;
      g_exec = nullptr;
      g_dispatcher = nullptr;

      return 1;
    }

    NodeImpl *root = nullptr;
    try {
      root = parsePlan(planDoc->document_element());
      hub.notifyOfAddPlan(planDoc->document_element());
      delete planDoc;
    }
    catch (ParserException& e) {
      warn("Error parsing plan '" << planName << "':\n" << e.what());
      delete planDoc;
      
      // Clean up
      delete g_exec;
      g_exec = nullptr;
      g_dispatcher = nullptr;

      return 1;
    }

    if (!g_exec->addPlan(root)) {
      warn("Adding plan " << planName << " failed");
      delete root;

      // Clean up
      delete g_exec;
      g_exec = nullptr;
      g_dispatcher = nullptr;

      return 1;
    }
  }

  // load script
  {
    pugi::xml_document *scriptDoc;
    try {
      scriptDoc = loadXmlFile(scriptName);
      if (!scriptDoc)
        warn("Error: script file " << scriptName << " not found or not readable");
    }
    catch (ParserException const &e) {
      warn("Error parsing script " << scriptName << ":\n"
           << e.what());
    }
    if (!scriptDoc) {
      // Clean up
      delete g_exec;
      g_exec = nullptr;
      g_dispatcher = nullptr;
      
      return 1;
    }

    // execute plan
    clock_t time = clock();
    pugi::xml_node scriptElement = scriptDoc->document_element();
    if (scriptElement.empty()
        || !testTag("PLEXILScript", scriptElement)) {
      warn("File " << scriptName << " is not a valid PLEXIL simulator script");
      delete scriptDoc;

      // Clean up
      delete g_exec;
      g_exec = nullptr;
      g_dispatcher = nullptr;

      return 1;
    }
    intf.run(scriptElement);
    debugMsg("Time", "Time spent in execution: " << clock() - time);
    delete scriptDoc;
  }

  // clean up

  delete g_exec;
  g_exec = nullptr;
  g_dispatcher = nullptr;

  return 0;
}

#if defined(__VXWORKS__)
extern "C"
int test_exec_for_vxworks(char* plan, char* script, char* debug_cfg)
{
  char *argv[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int argc = 0;
  argv[argc++] = "TestExec";
  if (plan) {
    argv[argc++] = "-p";
    argv[argc++] = plan;
  }
  if (script) {
    argv[argc++] = "-s";
    argv[argc++] = script;
  }
  if (debug_cfg) {
    argv[argc++] = "-d";
    argv[argc++] = debug_cfg;
  }
  return main(argc, argv);
}
#endif // __VXWORKS__
