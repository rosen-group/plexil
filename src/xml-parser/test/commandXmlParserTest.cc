/* Copyright (c) 2006-2021, Universities Space Research Association (USRA).
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

#include "CommandImpl.hh"
#include "commandXmlParser.hh"
#include "test/FactoryTestNodeConnector.hh"
#include "TestSupport.hh"
#include "test/TrivialNodeConnector.hh"
#include "UserVariable.hh"

#include "pugixml.hpp"

using namespace PLEXIL;

using pugi::xml_document;
using pugi::xml_node;
using pugi::node_pcdata;

static bool testCommandParserBasics()
{
  FactoryTestNodeConnector conn;
  Expression *flagVar = new BooleanVariable("flag");
  conn.storeVariable("flag", flagVar);

  xml_document doc;

  // Minimum case
  {
    xml_node simpleXml = doc.append_child("Command");
    simpleXml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("foo");
  
    try {
      checkCommandBody("simple", simpleXml);
    }
    catch (ParserException const & exc) {
      assertTrueMsg(ALWAYS_FAIL, "Unexpected parser exception: " << exc.what());
    }

    CommandImpl *simple = new CommandImpl("simple");
    finalizeCommand(simple, &conn, simpleXml);
    assertTrue_1(!simple->getDest());
    simple->activate();
    simple->fixValues();
    assertTrue_1(simple->getName() == "foo");
    assertTrue_1(simple->getArgValues().empty());
    assertTrue_1(simple->getCommand() == State("foo"));
    assertTrue_1(simple->getResourceValues().empty());
    delete simple;
  }

  // Empty arglist
  {
    xml_node emptyXml = doc.append_child("Command");
    emptyXml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("empty");
    emptyXml.append_child("Arguments");
  
    try {
      checkCommandBody("empty", emptyXml);
    }
    catch (ParserException const & exc) {
      assertTrueMsg(ALWAYS_FAIL, "Unexpected parser exception: " << exc.what());
    }
    
    CommandImpl *empty = new CommandImpl("empty");
    finalizeCommand(empty, &conn, emptyXml);
    assertTrue_1(!empty->getDest());
    empty->activate();
    empty->fixValues();
    assertTrue_1(empty->getName() == "empty");
    assertTrue_1(empty->getArgValues().empty());
    assertTrue_1(empty->getCommand() == State("empty"));
    assertTrue_1(empty->getResourceValues().empty());
    delete empty;
  }

  // Arguments
  {
    xml_node arghXml = doc.append_child("Command");
    arghXml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("argh");
    arghXml.append_child("Arguments").append_child("IntegerValue").append_child(node_pcdata).set_value("0");
  
    try {
      checkCommandBody("argh", arghXml);
    }
    catch (ParserException const & exc) {
      assertTrueMsg(ALWAYS_FAIL, "Unexpected parser exception: " << exc.what());
    }

    CommandImpl *argh = new CommandImpl("argh");
    finalizeCommand(argh, &conn, arghXml);
    assertTrue_1(!argh->getDest());
    argh->activate();
    argh->fixValues();
    assertTrue_1(argh->getName() == "argh");
    assertTrue_1(argh->getArgValues().size() == 1);
    State arghState = State("argh", 1);
    arghState.setParameter(0, Value((int32_t) 0));
    assertTrue_1(argh->getCommand() == arghState);
    assertTrue_1(argh->getResourceValues().empty());
    delete argh;
  }

  // Return value
  {
    xml_node resultantXml = doc.append_child("Command");
    resultantXml.append_child("BooleanVariable").append_child(node_pcdata).set_value("flag");
    resultantXml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("resultant");
  
    try {
      checkCommandBody("resultant", resultantXml);
    }
    catch (ParserException const & exc) {
      assertTrueMsg(ALWAYS_FAIL, "Unexpected parser exception: " << exc.what());
    }

    CommandImpl *resultant = new CommandImpl("resultant");
    finalizeCommand(resultant, &conn, resultantXml);
    assertTrue_1(resultant->getDest() == flagVar);
    resultant->activate();
    resultant->fixValues();
    assertTrue_1(resultant->getName() == "resultant");
    assertTrue_1(resultant->getArgValues().empty());
    assertTrue_1(resultant->getCommand() == State("resultant"));
    assertTrue_1(resultant->getResourceValues().empty());
    delete resultant;
  }

  // Empty resource
  {
    xml_node resourcelessXml = doc.append_child("Command");
    resourcelessXml.append_child("ResourceList");
    resourcelessXml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("resourceless");
  
    try {
      checkCommandBody("resourceless", resourcelessXml);
    }
    catch (ParserException const & exc) {
      assertTrueMsg(ALWAYS_FAIL, "Unexpected parser exception: " << exc.what());
    }

    CommandImpl *resourceless = new CommandImpl("resourceless");
    finalizeCommand(resourceless, &conn, resourcelessXml);
    assertTrue_1(!resourceless->getDest());
    resourceless->activate();
    resourceless->fixValues();
    assertTrue_1(resourceless->getName() == "resourceless");
    assertTrue_1(resourceless->getArgValues().empty());
    assertTrue_1(resourceless->getCommand() == State("resourceless"));
    assertTrue_1(resourceless->getResourceValues().empty());
    delete resourceless;
  }

  // Non-empty resource
  {
    xml_node resourcefulXml = doc.append_child("Command");
    xml_node resourcefulList = resourcefulXml.append_child("ResourceList");
    xml_node resource = resourcefulList.append_child("Resource");
    resource.append_child("ResourceName").append_child("StringValue").append_child(node_pcdata).set_value("a");
    resource.append_child("ResourcePriority").append_child("IntegerValue").append_child(node_pcdata).set_value("0");
    resourcefulXml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("resourceful");
  
    try {
      checkCommandBody("resourceful", resourcefulXml);
    }
    catch (ParserException const & exc) {
      assertTrueMsg(ALWAYS_FAIL, "Unexpected parser exception: " << exc.what());
    }

    CommandImpl *resourceful = new CommandImpl("resourceful");
    finalizeCommand(resourceful, &conn, resourcefulXml);
    assertTrue_1(!resourceful->getDest());
    resourceful->activate();
    resourceful->fixValues();
    assertTrue_1(resourceful->getName() == "resourceful");
    assertTrue_1(resourceful->getArgValues().empty());
    assertTrue_1(resourceful->getCommand() == State("resourceful"));
    ResourceValueList const &rlist = resourceful->getResourceValues();
    assertTrue_1(rlist.size() == 1);
    ResourceValue const &rmap = rlist.front();
    assertTrue_1(rmap.name == "a");
    assertTrue_1(rmap.priority == 0);
    delete resourceful;
  }

  // Non-empty resource with return value
  {
    xml_node remorsefulXml = doc.append_child("Command");
    xml_node remorsefulList = remorsefulXml.append_child("ResourceList");
    xml_node remorse = remorsefulList.append_child("Resource");
    remorse.append_child("ResourceName").append_child("StringValue").append_child(node_pcdata).set_value("a");
    remorse.append_child("ResourcePriority").append_child("IntegerValue").append_child(node_pcdata).set_value("1");
    remorsefulXml.append_child("BooleanVariable").append_child(node_pcdata).set_value("flag");
    remorsefulXml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("remorseful");
  
    try {
      checkCommandBody("remorseful", remorsefulXml);
    }
    catch (ParserException const & exc) {
      assertTrueMsg(ALWAYS_FAIL, "Unexpected parser exception: " << exc.what());
    }

    CommandImpl *remorseful = new CommandImpl("remorseful");
    finalizeCommand(remorseful, &conn, remorsefulXml);
    assertTrue_1(remorseful->getDest() == flagVar);
    remorseful->activate();
    remorseful->fixValues();
    assertTrue_1(remorseful->getName() == "remorseful");
    assertTrue_1(remorseful->getArgValues().empty());
    assertTrue_1(remorseful->getCommand() == State("remorseful"));
    ResourceValueList const &slist = remorseful->getResourceValues();
    assertTrue_1(slist.size() == 1);
    ResourceValue const &smap = slist.front();
    assertTrue_1(smap.name == "a");
    assertTrue_1(smap.priority == 1);
    delete remorseful;
  }

  // Kitchen sink
  {
    xml_node regretfulXml = doc.append_child("Command");
    xml_node regretfulList = regretfulXml.append_child("ResourceList");
    xml_node regret = regretfulList.append_child("Resource");
    regret.append_child("ResourceName").append_child("StringValue").append_child(node_pcdata).set_value("b");
    regret.append_child("ResourcePriority").append_child("IntegerValue").append_child(node_pcdata).set_value("2");
    regretfulXml.append_child("BooleanVariable").append_child(node_pcdata).set_value("flag");
    regretfulXml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("regretful");
    regretfulXml.append_child("Arguments").append_child("BooleanValue").append_child(node_pcdata).set_value("true");
  
    try {
      checkCommandBody("regretful", regretfulXml);
    }
    catch (ParserException const & exc) {
      assertTrueMsg(ALWAYS_FAIL, "Unexpected parser exception: " << exc.what());
    }

    CommandImpl *regretful = new CommandImpl("regretful");
    finalizeCommand(regretful, &conn, regretfulXml);
    assertTrue_1(regretful->getDest() == flagVar);
    regretful->activate();
    regretful->fixValues();
    assertTrue_1(regretful->getName() == "regretful");
    assertTrue_1(regretful->getArgValues().size() == 1);
    assertTrue_1(regretful->getArgValues().front() == Value(true));
    State regretCmd = State("regretful", 1);
    regretCmd.setParameter(0, Value(true));
    assertTrue_1(regretful->getCommand() == regretCmd);
    ResourceValueList const &tlist = regretful->getResourceValues();
    assertTrue_1(tlist.size() == 1);
    ResourceValue const &tmap = tlist.front();
    assertTrue_1(tmap.name == "b");
    assertTrue_1(tmap.priority == 2);
    delete regretful;
  }

  return true;
}

static bool testCommandParserErrorHandling()
{
  TrivialNodeConnector conn;

  xml_document doc;

  xml_node mtCmd = doc.append_child("Command");
  try {
    checkCommandBody("mt", mtCmd);
    assertTrue_2(ALWAYS_FAIL, "Failed to detect empty Command element");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  // Empty name
  xml_node mtName = doc.append_child("Command");
  mtName.append_child("Name");
  try {
    checkCommandBody("mtName", mtName);
    assertTrue_2(ALWAYS_FAIL, "Failed to detect empty Name element");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  // Name is not a string
  xml_node wrongTypeName = doc.append_child("Command");
  wrongTypeName.append_child("Name").append_child("RealValue").append_child(node_pcdata).set_value("3.14");
  {
    CommandImpl *wrongTypeNameCmd = nullptr;
    try {
      checkCommandBody("wrongTypeName", wrongTypeName);
      wrongTypeNameCmd = new CommandImpl("wrongTypeName");
      finalizeCommand(wrongTypeNameCmd, &conn, wrongTypeName);
      assertTrue_2(ALWAYS_FAIL, "Failed to detect non-string Name value");
    }
    catch (ParserException const & /* exc */) {
      std::cout << "Caught expected exception" << std::endl;
    }
    delete wrongTypeNameCmd;
  }

  // Invalid return expression
  xml_node invalidReturn = doc.append_child("Command");
  invalidReturn.append_child("StringValue").append_child(node_pcdata).set_value("illegal");
  invalidReturn.append_child("Name").append_child(node_pcdata).set_value("legal");
  // Do it this way because we may not detect error in first pass
  {
    CommandImpl *invalidReturnCmd = nullptr;
    try {
      checkCommandBody("invalidReturn", invalidReturn);
      invalidReturnCmd = new CommandImpl("invalidReturn");
      finalizeCommand(invalidReturnCmd, &conn, invalidReturn);
      assertTrue_2(ALWAYS_FAIL, "Failed to detect invalid return expression");
    }
    catch (ParserException const & /* exc */) {
      std::cout << "Caught expected exception" << std::endl;
    }
    delete invalidReturnCmd;
  }

  return true;
}

bool commandXmlParserTest()
{
  runTest(testCommandParserBasics);
  runTest(testCommandParserErrorHandling);

  return true;
}
