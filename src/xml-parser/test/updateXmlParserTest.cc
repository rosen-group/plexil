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

#include "TestSupport.hh"
#include "test/TrivialNodeConnector.hh"
#include "UpdateImpl.hh"
#include "updateXmlParser.hh"

#include "pugixml.hpp"

using namespace PLEXIL;

using pugi::xml_attribute;
using pugi::xml_document;
using pugi::xml_node;
using pugi::node_pcdata;

static bool testUpdateParserBasics()
{
  TrivialNodeConnector conn;
  xml_document doc;

  // Empty
  xml_node emptyUpdateXml = doc.append_child("Update");
  {
    try {
      checkUpdateBody("empty", emptyUpdateXml);
    }
    catch (ParserException const &exc) {
      assertTrueMsg(ALWAYS_FAIL, "Unexpected parser exception: " << exc.what());
    }
    UpdateImpl *emptyUpdate = constructUpdate(&conn, emptyUpdateXml);
    try {
      finalizeUpdate(emptyUpdate, &conn, emptyUpdateXml);
      emptyUpdate->fixValues();
      assertTrue_1(emptyUpdate->getPairs().empty());
    }
    catch (ParserException const &exc) {
      assertTrueMsg(ALWAYS_FAIL, "Unexpected parser exception: " << exc.what());
    }
    delete emptyUpdate;
  }

  // Simple
  xml_node simpleXml = doc.append_child("Update");
  xml_node simplePair = simpleXml.append_child("Pair");
  simplePair.append_child("Name").append_child(node_pcdata).set_value("foo");
  simplePair.append_child("IntegerValue").append_child(node_pcdata).set_value("0");

  {
    try {
      checkUpdateBody("simple", simpleXml);
    }
    catch (ParserException const &exc) {
      assertTrueMsg(ALWAYS_FAIL, "Unexpected parser exception: " << exc.what());
    }

    UpdateImpl *simple = constructUpdate(&conn, simpleXml);
    try {
      finalizeUpdate(simple, &conn, simpleXml);
      simple->fixValues();
      Update::PairValueMap const &simplePairs = simple->getPairs();
      assertTrue_1(simplePairs.size() == 1);
      assertTrue_1(simplePairs.begin() != simplePairs.end());
      assertTrue_1(simplePairs.begin()->first == "foo");
      assertTrue_1(simplePairs.begin()->second == Value((int32_t) 0));
    }
    catch (ParserException const &exc) {
      assertTrueMsg(ALWAYS_FAIL, "Unexpected parser exception: " << exc.what());
    }
    delete simple;
  }

  return true;
}

// Should all be caught in checkUpdateBody
static bool testUpdateParserErrorHandling()
{
  xml_document doc;

  // Empty name
  xml_node mtNameXml = doc.append_child("Update");
  xml_node mtNamePair = mtNameXml.append_child("Pair");
  mtNamePair.append_child("Name");
  mtNamePair.append_child("IntegerValue").append_child(node_pcdata).set_value("0");
  try {
    checkUpdateBody("mtName", mtNameXml);
    assertTrue_2(ALWAYS_FAIL, "Failed to detect empty Name element");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  // Missing Name
  xml_node missingNameXml = doc.append_child("Update");
  xml_node missingNamePair = missingNameXml.append_child("Pair");
  missingNamePair.append_child("IntegerValue").append_child(node_pcdata).set_value("0");
  try {
    checkUpdateBody("missingName", missingNameXml);
    assertTrue_2(ALWAYS_FAIL, "Failed to detect missing value expression");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  // Missing value
  xml_node missingValueXml = doc.append_child("Update");
  xml_node missingValuePair = missingValueXml.append_child("Pair");
  missingValuePair.append_child("Name").append_child(node_pcdata).set_value("foo");
  try {
    checkUpdateBody("missingValue", missingValueXml);
    assertTrue_2(ALWAYS_FAIL, "Failed to detect missing value expression");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  // Duplicate pair
  xml_node duplicateXml = doc.append_child("Update");
  xml_node duplicatePair = duplicateXml.append_child("Pair");
  duplicatePair.append_child("Name").append_child(node_pcdata).set_value("foo");
  duplicatePair.append_child("IntegerValue").append_child(node_pcdata).set_value("0");
  duplicateXml.append_copy(duplicatePair);
  try {
    checkUpdateBody("duplicatePair", duplicateXml);
    assertTrue_2(ALWAYS_FAIL, "Failed to detect duplicate pair name");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  return true;
}

bool updateXmlParserTest()
{
  runTest(testUpdateParserBasics);
  runTest(testUpdateParserErrorHandling);

  return true;
}
