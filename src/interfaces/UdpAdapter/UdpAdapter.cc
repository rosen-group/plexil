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

#include "plexil-stdint.h" // includes plexil-config.h

#include "AdapterConfiguration.hh"
#include "AdapterExecInterface.hh"
#include "AdapterFactory.hh"    // REGISTER_ADAPTER() macro
#include "ArrayImpl.hh"
#include "Command.hh"
#include "Configuration.hh"
#include "Debug.hh"
#include "Error.hh"
#include "InterfaceAdapter.hh"
#include "InterfaceError.hh"
#include "MessageQueueMap.hh"
#include "StateCacheEntry.hh"
#include "udp-utils.hh"
#include "UdpEventLoop.hh"

#include "pugixml.hpp"

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h> // close()
#endif

#include <memory> // std::unique_ptr, std::make_unique
#include <mutex>
#include <thread>

#include <cerrno>
#include <cfloat>
#include <cstring>

namespace PLEXIL
{
  //
  // Local constants
  //

  // Command names
  static constexpr char const SEND_MESSAGE_COMMAND[] = "SendMessage";
  static constexpr char const RECEIVE_COMMAND_COMMAND[] = "ReceiveCommand";
  static constexpr char const GET_PARAMETER_COMMAND[] = "GetParameter";
  static constexpr char const SEND_RETURN_VALUE_COMMAND[] = "SendReturnValue";

  // Message formatting
  static constexpr char const COMMAND_PREFIX[] = "__COMMAND__";
  static constexpr char const PARAM_PREFIX[] = "__PARAMETER__";

  //
  // Local helper function
  //

  static std::string formatMessageName(const std::string &name,
                                       const char *command,
                                       int id = 0)
  {
    std::ostringstream ss;
    if (command == RECEIVE_COMMAND_COMMAND)
      ss << COMMAND_PREFIX;
    else if (command == GET_PARAMETER_COMMAND)
      ss << PARAM_PREFIX;
    ss << name << '_' << id;
    debugMsg("UdpAdapter:formatMessageName", " returning " << ss.str());
    return ss.str();
  }

  struct Parameter final
  {
    std::string desc;           // optional parameter description
    std::string type;           // int|float|bool|string|int-array|float-array|string-array|bool-array
    unsigned int len;           // number of bytes for type (or array element)
    unsigned int elements;      // number of elements in the array (non-array types are 0 or 1?)
  };

  struct UdpMessage final
  {
    std::string name;                // the Plexil Command name
    std::string peer;                // peer to which to send
    std::vector<Parameter> parameters; // message value parameters
    unsigned int len;                         // the length of the message in bytes
    unsigned int local_port;                  // local port on which to receive
    unsigned int peer_port;                   // port to which to send
    UdpMessage()
      : name(),
        peer(),
        parameters(),
        len(0),
        local_port(0),
        peer_port(0)
    {}
    UdpMessage(std::string nam)
      : name(nam),
        peer(),
        parameters(),
        len(0),
        local_port(0),
        peer_port(0)
    {}
    UdpMessage(UdpMessage const &) = default;
    UdpMessage(UdpMessage &&) = default;
    UdpMessage &operator=(UdpMessage const &) = default;
    UdpMessage &operator=(UdpMessage &&) = default;
    ~UdpMessage() = default;
  };

  class UdpAdapter : public InterfaceAdapter
  {
  private:
    //
    // Local types
    //
    using MessageMap = std::map<std::string, UdpMessage>;
    using SocketMap = std::map<std::string, int>;

  public:

    //! Constructor
    UdpAdapter(AdapterExecInterface &execInterface, AdapterConf *conf)
      : InterfaceAdapter(execInterface, conf),
        m_eventLoop(makeUdpEventLoop()),
        m_default_peer("localhost"),
        m_messageQueues(execInterface),
        m_default_local_port(0),
        m_default_peer_port(0),
        m_debug(false)
    {
      debugMsg("UdpAdapter", " constructor");
    }

    //! Virtual destructor
    virtual ~UdpAdapter()
    {
      debugMsg("UdpAdapter", " destructor");
    }

    // Initialize
    virtual bool initialize(AdapterConfiguration *config) override
    {
      debugMsg("UdpAdapter:initialize", " called");

      // Register the basic command handlers
      config->registerCommandHandlerFunction(SEND_MESSAGE_COMMAND,
                                             [this](Command *cmd, AdapterExecInterface *intf) -> void
                                             { this->executeSendMessageCommand(cmd, intf); });
      config->registerCommandHandlerFunction(RECEIVE_COMMAND_COMMAND,
                                             [this](Command *cmd, AdapterExecInterface *intf) -> void
                                             { this->executeReceiveCommandCommand(cmd, intf); },
                                             [this](Command *cmd, AdapterExecInterface *intf) -> void
                                             { this->abortReceiveCommandCommand(cmd, intf); });
      config->registerCommandHandlerFunction(GET_PARAMETER_COMMAND,
                                             [this](Command *cmd, AdapterExecInterface *intf) -> void
                                             { this->executeGetParameterCommand(cmd, intf); });
      config->registerCommandHandlerFunction(SEND_RETURN_VALUE_COMMAND,
                                             [this](Command *cmd, AdapterExecInterface *intf) -> void
                                             { this->executeSendReturnValueCommand(cmd, intf); });

      pugi::xml_node const xml = getXml();

      // Enable debug output if requested
      m_debug = xml.attribute("debug").as_bool();

      // Parsing the UDP configuration
      m_default_local_port = xml.attribute("default_local_port").as_uint(m_default_local_port);
      // TODO: range check
      m_default_peer_port = xml.attribute("default_peer_port").as_uint(m_default_peer_port);
      // TODO: range check
      m_default_peer = xml.attribute("default_peer").as_string(m_default_peer.c_str());
      if (m_default_peer.empty()) {
        warn("UdpAdapter: empty default_peer value supplied");
      }
      
      // parse the message definitions and register the commands
      if (!parseMessageDefinitions(config)) {
        debugMsg("UdpAdapter:initialize", " message definition parsing failed");
        return false;
      }
      if (m_debug)
        printMessageDefinitions();

      debugMsg("UdpAdapter:initialize", " succeeded");
      return true;
    }

    // Start method
    virtual bool start() override
    {
      debugMsg("UdpAdapter:start()", " called");
      // Start the UDP listener thread
      m_eventLoop->start();
      return true;
    }

    // Stop method
    virtual void stop() override
    {
      debugMsg("UdpAdapter:stop", " called");
      // Stop the UDP listener thread
      m_eventLoop->stop();
    }

  private:

    //
    // Command handlers
    //

    // Generic command
    void executeDefaultCommand(Command *cmd, AdapterExecInterface *intf)
    {
      std::vector<Value> const &args = cmd->getArgValues();
      if (args.size() == 0) {
        warn("UdpAdapter:executeDefaultCommand: command requires at least one argument");
        intf->handleCommandAck(cmd, COMMAND_FAILED);
        intf->notifyOfExternalEvent();
        return;
      }
      
      if (args[0].valueType() != STRING_TYPE) {
        warn("UdpAdapter:executeDefaultCommand: message name must be a string");
        intf->handleCommandAck(cmd, COMMAND_FAILED);
        intf->notifyOfExternalEvent();
        return;
      }
      
      std::string const &msgName = cmd->getName();
      debugMsg("UdpAdapter:executeDefaultCommand",
               " called for \"" << msgName << "\" with " << args.size() << " args");
      std::lock_guard<std::mutex> guard(m_cmdMutex);
      MessageMap::const_iterator msg = m_messages.find(msgName);
      // Check for an obviously bogus port
      if (msg->second.peer_port == 0) {
        warn("executeDefaultCommand: bad peer port (0) given for " << msgName << " message");
        intf->handleCommandAck(cmd, COMMAND_FAILED);
        intf->notifyOfExternalEvent();
        return;
      }
      
      // Set up the outgoing UDP buffer to be sent
      int length = msg->second.len;
      unsigned char* udp_buffer = new unsigned char[length]; // fixed length to start with
      memset((char*)udp_buffer, 0, length); // zero out the buffer
      // Walk the parameters and encode them in the buffer to be sent out
      if (0 > buildUdpBuffer(udp_buffer, msg->second, args, false, m_debug)) {
        warn("executeDefaultCommand: error formatting buffer");
        delete[] udp_buffer;
        intf->handleCommandAck(cmd, COMMAND_FAILED);
        intf->notifyOfExternalEvent();
        return;
      }
      
      // Send the buffer to the given host:port
      int status = sendUdpMessage(udp_buffer, msg->second, m_debug);
      debugMsg("UdpAdapter:executeDefaultCommand",
               " sendUdpMessage returned " << status << " (bytes sent)");
      // Clean up some (one hopes)
      delete[] udp_buffer;
      // Do the internal Plexil Boiler Plate (as per example in IpcAdapter.cc)
      intf->handleCommandAck(cmd, COMMAND_SUCCESS);
      intf->notifyOfExternalEvent();
    }

    // RECEIVE_COMMAND_COMMAND
    void executeReceiveCommandCommand(Command *cmd, AdapterExecInterface *intf)
    {
      std::vector<Value> const &args = cmd->getArgValues();
      if (args.size() != 1) {
        warn("UdpAdapter: The " << RECEIVE_COMMAND_COMMAND
             << " command requires exactly one argument");
        intf->handleCommandAck(cmd, COMMAND_FAILED);
        intf->notifyOfExternalEvent();
        return;
      }

      if (args.front().valueType() != STRING_TYPE) {
        warn("UdpAdapter: The argument to the " << RECEIVE_COMMAND_COMMAND
             << " command, " << args.front() << ", is not a string");
        intf->handleCommandAck(cmd, COMMAND_FAILED);
        intf->notifyOfExternalEvent();
        return;
      }

      std::string msgName;
      if (!args.front().getValue(msgName)) {
        warn("UdpAdapter: The command name parameter to the "
             << RECEIVE_COMMAND_COMMAND << " is unknown");
        intf->handleCommandAck(cmd, COMMAND_FAILED);
        intf->notifyOfExternalEvent();
        return;
      }

      debugMsg("UdpAdapter:executeReceiveCommandCommand", " called for " << msgName);
      std::string command = formatMessageName(msgName, RECEIVE_COMMAND_COMMAND);
      m_messageQueues.addRecipient(command, cmd);
      // Set up the thread on which the message may/will eventually be received
      int status = startUdpMessageReceiver(msgName, cmd);
      if (status) {
        warn("executeReceiveCommandCommand: startUdpMessageReceiver failed");
        intf->handleCommandAck(cmd, COMMAND_FAILED);
      }
      else {
        debugMsg("UdpAdapter:executeReceiveCommandCommand",
                 " message handler for \"" << command << "\" registered");
        intf->handleCommandAck(cmd, COMMAND_SENT_TO_SYSTEM);
      }
      intf->notifyOfExternalEvent();
    }

    //   // RECEIVE_UDP_MESSAGE_COMMAND
    //   void executeReceiveUdpCommand(Command *cmd)
    //   {
    //     // Called when node _starts_ executing, so, record the message and args so that they can be filled in
    //     // if and when a UDP message comes in the fulfill this expectation.
    //     // First arg is message name (which better match one of the defined messages...)
    //     std::string command(args.front());
    //     debugMsg("UdpAdapter:executeReceiveUdpCommand", " " << command << ", dest==" << dest
    //              << ", ack==" << ack << ", args.size()==" << args.size());
    //     getInterface().handleCommandAck(cmd, COMMAND_SENT_TO_SYSTEM);
    //     getInterface().notifyOfExternalEvent();
    //     debugMsg("UdpAdapter:executeReceiveUdpCommand", " handler for \"" << command << "\" registered");
    //   }

    //  // SEND_UDP_MESSAGE_COMMAND
    //   void executeSendUdpMessageCommand(Command *cmd)
    //   {
    //     // First arg is message name (which better match one of the defined messages...)
    //     // Lookup the appropriate message in the message definitions in m_messages
    //     std::string msgName(args.front());
    //     debugMsg("UdpAdapter:executeSendUdpMessageCommand", " called for " << msgName);
    //     //printMessageContent(msgName, args);
    //     MessageMap::iterator msg;
    //     msg=m_messages.find(msgName);
    //     // Set up the outgoing UDP buffer to be sent
    //     int length = msg->second.len;
    //     unsigned char* udp_buffer = new unsigned char[length]; // fixed length to start with
    //     memset((char*)udp_buffer, 0, length); // zero out the buffer
    //     // Walk the parameters and encode them in the buffer to be sent out
    //     buildUdpBuffer(udp_buffer, msg->second, args, true, m_debug);
    //     // Send the buffer to the given host:port
    //     int status = -1;
    //     status = sendUdpMessage(udp_buffer, msg->second, m_debug);
    //     debugMsg("UdpAdapter:executeSendUdpMessageCommand", " sendUdpMessage returned " << status << " (bytes sent)");
    //     // Do the internal Plexil Boiler Plate (as per example in IpcAdapter.cc)
    //     getInterface().handleCommandAck(cmd, COMMAND_SUCCESS);
    //     getInterface().notifyOfExternalEvent();
    //     // Clean up some (one hopes)
    //     delete udp_buffer;
    //  }

    // GET_PARAMETER_COMMAND
    void executeGetParameterCommand(Command *cmd, AdapterExecInterface *intf)
    {
      std::vector<Value> const &args = cmd->getArgValues();
      if (args.size() < 1 || args.size() > 2) {
        warn("UdpAdapter: The " << GET_PARAMETER_COMMAND
             << " command requires either one or two arguments");
        intf->handleCommandAck(cmd, COMMAND_FAILED);
        intf->notifyOfExternalEvent();
        return;
      }

      if (args.front().valueType() != STRING_TYPE) {
        warn("UdpAdapter: The oparameter name argument to the " << GET_PARAMETER_COMMAND
             << " command, " << args.front() << ", is not a string");
        intf->handleCommandAck(cmd, COMMAND_FAILED);
        intf->notifyOfExternalEvent();
        return;
      }

      // Extract the message name and try to verify the number of parameters defined vs the number of args used in the plan
      std::string msgName;
      if (!args.front().getValue(msgName)) {
        warn("UdpAdapter:executeGetParameterCommand: message name is unknown");
        intf->handleCommandAck(cmd, COMMAND_FAILED);
        intf->notifyOfExternalEvent();
        return;
      }
      
      debugMsg("UdpAdapter:executeGetParameterCommand", " " << msgName);
      size_t pos;
      pos = msgName.find(":");
      std::string const baseName = msgName.substr(0, pos);
      MessageMap::const_iterator msg = m_messages.find(baseName);
      if (msg == m_messages.end()) {
        warn("UdpAdapter:executeGetParameterCommand: no message definition found for "
             << baseName);
        intf->handleCommandAck(cmd, COMMAND_FAILED);
        intf->notifyOfExternalEvent();
        return;
      }
      
      int params = msg->second.parameters.size();
      debugMsg("UdpAdapter:executeGetParameterCommand",
               " msgName==" << msgName << ", params==" << params);
      std::vector<Value>::const_iterator it = ++args.begin();
      int32_t id = 0;
      if (it != args.end()) {
        if (it->valueType() != INTEGER_TYPE) {
          warn("UdpAdapter: The second argument to the " << GET_PARAMETER_COMMAND
               << " command, " << *it << ", is not an integer");
          intf->handleCommandAck(cmd, COMMAND_FAILED);
          intf->notifyOfExternalEvent();
          return;
        }
        
        if (!it->getValue(id)) {
          warn("UdpAdapter: The second argument to the " << GET_PARAMETER_COMMAND
               << " command is unknown");
          intf->handleCommandAck(cmd, COMMAND_FAILED);
          intf->notifyOfExternalEvent();
          return;
        }

        if (id < 0) {
          warn("UdpAdapter: The second argument to the " << GET_PARAMETER_COMMAND
               << " command, " << *it << ", is not a valid index");
          warn("UdpAdapter: The second argument to the " << GET_PARAMETER_COMMAND
               << " command is unknown");
          intf->handleCommandAck(cmd, COMMAND_FAILED);
          intf->notifyOfExternalEvent();
          return;
        }

        // Brute strength error check for the plan using a message/command with to many arguments.
        // The intent is that this might be discovered during development.
        if (id >= params) {
          warn("UdpAdapter: the message \"" << msgName << "\" is defined to have " << params
               << " parameters in the XML configuration file, but is being used in the plan with "
               << id+1 << " arguments");
          intf->handleCommandAck(cmd, COMMAND_FAILED);
          intf->notifyOfExternalEvent();
          return;
        }
      }
      std::string command = formatMessageName(msgName, GET_PARAMETER_COMMAND, id);
      m_messageQueues.addRecipient(command, cmd);
      debugMsg("UdpAdapter:executeGetParameterCommand", " message handler for \"" << cmd->getName() << "\" registered");
      intf->handleCommandAck(cmd, COMMAND_SENT_TO_SYSTEM);
      intf->notifyOfExternalEvent();
    }

    // SEND_RETURN_VALUE_COMMAND
    // Required by OnCommand XML macro. No-op for UDP.
    void executeSendReturnValueCommand(Command *cmd, AdapterExecInterface *intf)
    {
      intf->handleCommandAck(cmd, COMMAND_SUCCESS);
      intf->notifyOfExternalEvent();
    }

    // SEND_MESSAGE_COMMAND
    void executeSendMessageCommand(Command *cmd, AdapterExecInterface *intf)
    {
      std::vector<Value> const &args = cmd->getArgValues();
      // Check for one argument, the message
      if (args.size() != 1) {
        warn("UdpAdapter: The SendMessage command requires exactly one argument");
        intf->handleCommandAck(cmd, COMMAND_FAILED);
        intf->notifyOfExternalEvent();
        return;
      }
      if (args.front().valueType() != STRING_TYPE) {
        warn("UdpAdapter: The message name argument to the SendMessage command, "
             << args.front() << ", is not a string");
        intf->handleCommandAck(cmd, COMMAND_FAILED);
        intf->notifyOfExternalEvent();
        return;
      }

      std::string theMessage;
      if (!args.front().getValue(theMessage)) {
        warn("UdpAdapter: The message name argument to the SendMessage command is unknown");
        intf->handleCommandAck(cmd, COMMAND_FAILED);
        intf->notifyOfExternalEvent();
        return;
      }
      
      debugMsg("UdpAdapter:executeSendMessageCommand", " SendMessage(\"" << theMessage << "\")");
      debugMsg("UdpAdapter:executeSendMessageCommand", " message \"" << theMessage << "\" sent.");
      // store ack
      intf->handleCommandAck(cmd, COMMAND_SUCCESS);
      intf->notifyOfExternalEvent();
    }

    //
    // Abort command methods
    //

    // Abort a Plexil Command
    // Also handy utility for the below
    void abortCommand(Command *cmd, AdapterExecInterface *intf, bool status = true)
    {
      debugMsg("UdpAdapter:abortCommand",
               " for " << cmd->getName() << ", status = "
               << (status ? "true" : "false"));
      intf->handleCommandAbortAck(cmd, status);
      intf->notifyOfExternalEvent();
    }

    // Abort a ReceiveCommand command
    void abortReceiveCommandCommand(Command *cmd, AdapterExecInterface *intf)
    {
      std::vector<Value> const &cmdArgs = cmd->getArgValues();
      // Shouldn't be possible if the original command worked
      if (cmdArgs.size() < 1) {
        warn("UdpAdapter:abortCommand: Malformed ReceiveCommand command;\n"
             << " no command name supplied");
        abortCommand(cmd, intf, false);
        return;
      }

      // Shouldn't be possible if the original command worked
      if (cmdArgs.front().valueType() != STRING_TYPE) {
        warn("UdpAdapter:abortCommand: Malformed ReceiveCommand command;\n"
             << "\n command name parameter value " << cmdArgs.front() << ", is not a String");
        abortCommand(cmd, intf, false);
        return;
      }

      std::string msgName;
      // Shouldn't be possible if the original command worked
      if (!cmdArgs.front().getValue(msgName)) { // The defined message name, needed for looking up the thread and socket
        warn("UdpAdapter:abortCommand: ReceiveCommand command name argument is unknown");
        abortCommand(cmd, intf, false);
        return;
      }
      
      debugMsg("UdpAdapter:abortCommand", " ReceiveCommand(\"" << msgName << "\")");

      // Tell event loop we're done with this port
      UdpMessage &msg = m_messages[msgName];
      if (!msg.local_port) {
        warn("UdpAdapter:abortCommand: No local port found for ReceiveCommand(\\"
             << msgName << "\")");
        abortCommand(cmd, intf, false);
        return;
      }
      m_eventLoop->closeListener(msg.local_port);

      debugMsg("UdpAdapter:abortCommand", " ReceiveCommand(\"" << msgName << "\") complete");

      // Let the exec know that we believe things are cleaned up
      abortCommand(cmd, intf, true);
    }

    //
    // Configuration parsing
    //

    // Parse the message definitions; assign a command handler to each of them.
    bool parseMessageDefinitions(AdapterConfiguration *config)
    {
      if (!getXml().child("Message"))
        return true;  // no messages, hence nothing to do

      CommandHandlerPtr handler;
      for (pugi::xml_node const msgXml : getXml().children("Message")) {
        if (!parseMessageDefinition(msgXml))
          return false;
        config->registerCommandHandlerFunction(std::string(msgXml.attribute("name").value()),
                                               [this](Command *cmd, AdapterExecInterface *intf) -> void
                                               { this->executeDefaultCommand(cmd, intf); },
                                               [this](Command *cmd, AdapterExecInterface *intf) -> void
                                               { this->abortCommand(cmd, intf, true); });
      }
      return true;
    }

    // Parse one message definition
    bool parseMessageDefinition(pugi::xml_node const msgXml)
    {
      const char* name = msgXml.attribute("name").value();
      if (!name || !*name) {
        warn("UdpAdapter: Message definition missing required 'name' attribute");
        return false;
      }

      pugi::xml_attribute local_port_attr = msgXml.attribute("local_port");
      unsigned int local_port = 0;
      if (local_port_attr) {
        local_port = local_port_attr.as_uint();
        if (local_port == 0 || local_port > 65535) {
          warn("UdpAdapter: Message " << name
               << ": local_port value " << local_port_attr.value()
               << " is out of range or invalid");
          return false;
        }
      }
      else if (m_default_local_port) {
        local_port = m_default_local_port;
        warn("UdpAdapter: Message " << name
             << ": using default local port " << m_default_local_port);
      }
      else {
        warn("UdpAdapter: Message " << name
             << ": no local_port attribute and no default local port specified");
        return false;
      }

      pugi::xml_attribute peer_port_attr = msgXml.attribute("peer_port");
      unsigned int peer_port = 0;
      if (peer_port_attr) {
        peer_port = peer_port_attr.as_uint();
        if (peer_port == 0 || peer_port > 65535) {
          warn("UdpAdapter: Message " << name
               << ": peer_port value " << peer_port_attr.value()
               << " is out of range or invalid");
          return false;
        }
      }
      else if (m_default_peer_port) {
        peer_port = m_default_peer_port;
        warn("UdpAdapter: Message " << name
             << ": using default peer port " << m_default_peer_port);
      }
      else {
        warn("UdpAdapter: Message " << name
             << ": no peer_port attribute and no default peer port specified");
        return false;
      }

      // Construct the message
      UdpMessage msg(name);
      msg.local_port = local_port;
      msg.peer_port = peer_port;
      msg.peer = msgXml.attribute("peer").as_string(m_default_peer.c_str());

      // Walk the <Parameter/> elements of this <Message/>
      for (pugi::xml_node param : msgXml.children("Parameter")) {
        Parameter arg;

        // Get the (required) type
        const char *param_type = param.attribute("type").value();
        if (!param_type || !*param_type) {
          warn("UdpAdapter: Message " << name
               << ": Parameter 'type' attribute missing or empty");
          return false;
        }
        arg.type = param_type;

        // Get the required length in bytes for one value
        pugi::xml_attribute len = param.attribute("bytes");
        if (!len) {
          warn("UdpAdapter: Message " << name << ": Parameter missing required attribute 'bytes'");
          return false;
        }
        arg.len = len.as_uint();
        if (!arg.len) {
          warn("UdpAdapter: Message " << name
               << ": Parameter 'bytes' value " << len.value() << " invalid");
          return false;
        }

        // Check type, and oh BTW bytes value for the type
        if ((arg.type.compare("int") == 0) || (arg.type.compare("int-array") == 0)) {
          if (arg.len != 2 && arg.len != 4) {
            warn("UdpAdapter: Message " << name
                 << ": Invalid 'bytes' value " << arg.len
                 << " for " << arg.type << " parameter;\n valid values are 2 or 4");
            return false;
          }
        }
        else if ((arg.type.compare("float") == 0) || (arg.type.compare("float-array") == 0)) {
          // FIXME - shouldn't these be 4 or 8??
          if (arg.len != 2 && arg.len != 4) {
            warn("UdpAdapter: Message " << name
                 << ": Invalid 'bytes' value " << arg.len
                 << " for " << arg.type << " parameter;\n valid values are 2 or 4");
            return false;
          }
        }
        else if ((arg.type.compare("bool") == 0) || (arg.type.compare("bool-array") == 0)) {
          if (arg.len != 1 && arg.len != 2 && arg.len != 4) {
            warn("UdpAdapter: Message " << name
                 << ": Invalid 'bytes' value " << arg.len
                 << " for " << arg.type << " parameter;\n valid values are 1, 2, or 4");
            return false;
          }
        }
        // what about strings? -- fixed length to start with I suppose...
        else if ((arg.type.compare("string") == 0) || (arg.type.compare("string-array") == 0)) {
          if (arg.len < 1) {
            warn("UdpAdapter: Message " << name << ": " << arg.type
                 << " parameter 'bytes' value must be greater than 0");
            return false;
          }
        }
        else {
          warn("UdpAdapter: Message " << name << ": Invalid parameter type \""
               << arg.type << '"');
          return false;
        }

        // Get the number of elements for the array types
        pugi::xml_attribute param_elements = param.attribute("elements");
        if (arg.type.find("array") != std::string::npos) {
          if (!param_elements) {
            warn("UdpAdapter: Message " << name << ": " << arg.type
                 << " parameter missing required 'elements' attribute");
            return false;
          }
          arg.elements = param_elements.as_uint(0);
          if (!arg.elements) {
            warn("UdpAdapter: Message " << name << ": " << arg.type
                 << " parameter with zero or invalid 'elements' attribute");
            return false;
          }
        }
        else if (param_elements) {
          warn("UdpAdapter: Message " << name << ": " << arg.type <<
               "parameter may not have an 'elements' attribute");
          return false;
        }
        else {
          arg.elements = 1;
        }

        // Get the (optional) description
        pugi::xml_attribute param_desc = param.attribute("desc");
        if (param_desc)
          arg.desc = param_desc.value();

        // Success!
        msg.len += arg.len * arg.elements;
        msg.parameters.push_back(arg);
      }
      m_messages[name] = msg; // record the message with the name as the key
      return true;
    }

    void printMessageDefinitions()
    {
      // print all of the stuff in m_message for debugging
      std::string indent = "             ";
      for (std::pair<std::string const, UdpMessage> const &msg : m_messages) {
        std::cout << "UDP Message: " << msg.first;
        for (Parameter const &param : msg.second.parameters) {
          std::string temp = param.desc.empty() ? " (no description)" : " (" + param.desc + ")";
          if (param.elements == 1) {
            std::cout << "\n" << indent << param.len << " byte " << param.type << temp;
          }
          else {
            size_t pos = param.type.find("-");
            std::cout << "\n" << indent << param.elements << " element array of " << param.len << " byte "
                      << param.type.substr(0, pos) << "s" << temp;
          }
        }
        std::cout << std::endl << indent << "length: " << msg.second.len << " (bytes)";
        std::cout << ", peer: " << msg.second.peer << ", peer_port: " << msg.second.peer_port;
        std::cout << ", local_port: " << msg.second.local_port;
        std::cout << std::endl;
      }
    }

    // Start a UDP Message Handler for a node waiting on a UDP message
    int startUdpMessageReceiver(const std::string& name, Command * /* cmd */)
    {
      debugMsg("UdpAdapter:startUdpMessageReceiver",
               " for " << name);
      // Find the message definition to get the message port and size
      MessageMap::iterator msgIt = m_messages.find(name);
      if (msgIt == m_messages.end()) {
        warn("UdpAdapter:startUdpMessageReceiver: no message found for " << name);
        return -1;
      }

      UdpMessage &msg = msgIt->second;

      // Check for a bogus local port
      if (msg.local_port == 0) {
        warn("startUdpMessageReceiver: bad local port (0) given for " << name << " message");
        return -1;
      }

      // Hand off to the event loop
      if (!m_eventLoop->openListener(msg.local_port,
                                     msg.len,
                                     [this, &msg](in_port_t port,
                                                  const void *buffer,
                                                  size_t length,
                                                  const struct sockaddr *address,
                                                  socklen_t address_len) -> void
                                     { this->handleUdpMessage(msg,
                                                              reinterpret_cast<const unsigned char *>(buffer),
                                                              length); })) {
        warn("UdpAdapter:startUdpMessageReceiver: openListener() failed for " << name);
        return -1;
      }
      debugMsg("UdpAdapter:startUdpMessageReceiver",
                 " " << name << " listener started");
      return 0;
    }

    int handleUdpMessage(const UdpMessage &msgDef,
                         const unsigned char* buffer,
                         size_t length)
    {
      // Handle a UDP message once it has indeed arrived.
      // msgDef is passed in, therefore, we will assume it is good.
      debugMsg("UdpAdapter:handleUdpMessage", " called for " << msgDef.name);
      if (m_debug) {
        std::cout << "  handleUdpMessage: buffer: ";
        print_buffer(buffer, msgDef.len);
      }
      // (1) addMessage for expected message
      static int counter = 1;     // gensym counter
      std::ostringstream unique_id;
      unique_id << msgDef.name << ":msg_parameter:" << counter++;
      std::string msg_label(unique_id.str());
      debugMsg("UdpAdapter:handleUdpMessage", " adding \"" << msgDef.name << "\" to the command queue");
      const std::string msg_name = formatMessageName(msgDef.name, RECEIVE_COMMAND_COMMAND);
      m_messageQueues.addMessage(msg_name, msg_label);
      // (2) walk the parameters, and for each, call addMessage(label, <value-or-key>), which
      //     (somehow) arranges for executeCommand(GetParameter) to be called, and which in turn
      //     calls addRecipient and updateQueue
      int i = 0;
      int offset = 0;
      for (std::vector<Parameter>::const_iterator param = msgDef.parameters.begin();
           param != msgDef.parameters.end();
           param++, i++) {
        const std::string param_label = formatMessageName(msg_label, GET_PARAMETER_COMMAND, i);
        int len = param->len;   // number of bytes to read
        int size = param->elements; // size of the array, or 1 for scalars
        std::string type = param->type; // type to decode
        if (m_debug) {
          if (size == 1) {
            std::cout << "  handleUdpMessage: decoding " << len << " byte " << type
                      << " starting at buffer[" << offset << "]: ";
          }
          else {
            size_t pos = type.find("-"); // remove the "-array" from the type
            std::cout << "  handleUdpMessage: decoding " << size << " element array of " << len
                      << " byte " << type.substr(0, pos) << "s starting at buffer[" << offset
                      << "]: ";
          }
        }
        if (type.compare("int") == 0) {
          if (len != 2 && len != 4){
            warn("handleUdpMessage: Integers must be 2 or 4 bytes, not " << len);
            return -1;
          }
          int num;
          num = (len == 2) ? decode_short_int(buffer, offset) : decode_int32_t(buffer, offset);
          if (m_debug)
            std::cout << num << std::endl;
          debugMsg("UdpAdapter:handleUdpMessage", " queueing numeric (integer) parameter " << num);
          m_messageQueues.addMessage(param_label, Value(num));
          offset += len;
        }
        else if (type.compare("int-array") == 0) {
          if (len != 2 && len != 4) {
            warn("handleUdpMessage: Integers must be 2 or 4 bytes, not " << len);
            return -1;
          }
          IntegerArray array(size);
          for (int i = 0 ; i < size ; i++) {
            array.setElement(i, (int32_t) ((len == 2) ? decode_short_int(buffer, offset) : decode_int32_t(buffer, offset)));
            offset += len;
          }
          if (m_debug)
            std::cout << array.toString() << std::endl;
          debugMsg("UdpAdapter:handleUdpMessage", " queueing numeric (integer) array " << array.toString());
          m_messageQueues.addMessage(param_label, array);
        }
        else if (type.compare("float") == 0) {
          if (len != 4) {
            warn("handleUdpMessage: Reals must be 4 bytes, not " << len);
            return -1;
          }
          float num = decode_float(buffer, offset);
          if (m_debug)
            std::cout << num << std::endl;
          debugMsg("UdpAdapter:handleUdpMessage", " queueing numeric (real) parameter " << num);
          m_messageQueues.addMessage(param_label, Value((double) num));
          offset += len;
        }
        else if (type.compare("float-array") == 0) {
          if (len != 4) {
            warn("handleUdpMessage: Reals must be 4 bytes, not " << len);
            return -1;
          }
          RealArray array(size);
          for (int i = 0 ; i < size ; i++) {
            array.setElement(i, (double) decode_float(buffer, offset));
            offset += len;
          }
          if (m_debug)
            std::cout << array.toString() << std::endl;
          debugMsg("UdpAdapter:handleUdpMessage", " queueing numeric (real) array " << array.toString());
          m_messageQueues.addMessage(param_label, Value(array));
        }
        else if (type.compare("bool") == 0) {
          int num;
          switch (len) {
          case 1:
            num = buffer[offset];
            break;

          case 2:
            num = decode_short_int(buffer, offset); break;

          case 4:
            num = decode_int32_t(buffer, offset); break;

          default:
            warn("handleUdpMessage: Booleans must be 1, 2 or 4 bytes, not " << len);
            return -1;
          }
          if (m_debug)
            std::cout << num << std::endl;
          debugMsg("UdpAdapter:handleUdpMessage", " queueing numeric (boolean) parameter " << num);
          m_messageQueues.addMessage(param_label, Value(num != 0));
          offset += len;
        }
        else if (type.compare("bool-array") == 0) {
          BooleanArray array(size);
          for (int i = 0 ; i < size ; i++) {
            switch (len) {
            case 1: 
              array.setElement(i, 0 != buffer[offset]); break;
            case 2:
              array.setElement(i, 0 != decode_short_int(buffer, offset)); break;
            case 4:
              array.setElement(i, 0 != decode_int32_t(buffer, offset)); break;
            default:
              warn("handleUdpMessage: Booleans must be 1, 2 or 4 bytes, not " << len);
              return -1;
            }
            offset += len;
          }
          if (m_debug)
            std::cout << array.toString() << std::endl;
          debugMsg("UdpAdapter:handleUdpMessage", " queueing boolean array " << array.toString());
          m_messageQueues.addMessage(param_label, Value(array));
        }
        else if (type.compare("string-array") == 0) {
          // XXXX For unknown reasons, OnCommand(... String arg); is unable to receive this (inlike int and float arrays)
          StringArray array(size);
          for (int i = 0 ; i < size ; i++) {
            array.setElement(i, decode_string(buffer, offset, len));
            offset += len;
          }
          if (m_debug)
            std::cout << array.toString() << std::endl;
          debugMsg("UdpAdapter:handleUdpMessage", " queuing string array " << array.toString());
          m_messageQueues.addMessage(param_label, Value(array));
        }
        else { // string or die
          if (type.compare("string")) {
            warn("handleUdpMessage: unknown parameter type " << type);
            return -1;
          }
          std::string str = decode_string(buffer, offset, len);
          if (m_debug)
            std::cout << str << std::endl;
          debugMsg("UdpAdapter:handleUdpMessage", " queuing string parameter \"" << str << "\"");
          m_messageQueues.addMessage(param_label, Value(str));
          offset += len;
        }
      }
      debugMsg("UdpAdapter:handleUdpMessage", " for " << msgDef.name << " complete");
      return 0;
    }

    int sendUdpMessage(const unsigned char* buffer, const UdpMessage& msg, bool debug)
    {
      int status = 0; // return status
      debugMsg("UdpAdapter:sendUdpMessage", " sending " << msg.len << " bytes to " << msg.peer << ":" << msg.peer_port);
      status = send_message_connect(msg.peer.c_str(), msg.peer_port, (const char*) buffer, msg.len, debug);
      return status;
    }

    int buildUdpBuffer(unsigned char* buffer,
                       const UdpMessage& msg,
                       const std::vector<Value>& args,
                       bool skip_arg,
                       bool debug)
    {
      std::vector<Value>::const_iterator it;
      std::vector<Parameter>::const_iterator param;
      int start_index = 0; // where in the buffer to write

      // Do what error checking we can, since we absolutely know that planners foul this up.
      debugMsg("UdpAdapter:buildUdpBuffer",
               " args.size()==" << args.size()
               << ", parameters.size()==" << msg.parameters.size());
      size_t param_count = msg.parameters.size();
      if (skip_arg)
        param_count++;

      if (args.size() != param_count) {
        warn("the " << param_count
             << " parameters defined in the XML configuration file do not match the "
             << args.size() << " parameters used in the plan for <Message name=\""
             << msg.name << "\"/>");
        return -1;
      }

      // Iterate over the given args (it) and the message definition (param) in lock step to encode the outgoing buffer.
      for (param = msg.parameters.begin(), it=args.begin(); param != msg.parameters.end(); param++, it++) {
        if (skip_arg) { // only skip the first arg
          it++; 
          skip_arg = false; 
        }
        unsigned int len = param->len;
        std::string type = param->type;
        Value const &plexil_val = *it;
        if (!plexil_val.isKnown()) {
          warn("buildUdpBuffer: Value to be sent is unknown");
          return -1;
        }

        ValueType valType = plexil_val.valueType();
      
        if (debug)
          std::cout << "  buildUdpBuffer: encoding ";
        if (type.compare("bool") == 0) {
          if (valType != BOOLEAN_TYPE) {
            warn("buildUdpBuffer: Format requires Boolean, but supplied value is not");
            return -1;
          }
          bool temp;
          plexil_val.getValue(temp);
          if (debug)
            std::cout << len << " byte bool starting at buffer[" << start_index << "]: " << temp;
          switch (len) {
          case 1: 
            buffer[start_index] = (unsigned char) temp;
            break;
          case 2:
            encode_short_int(temp, buffer, start_index);
            break;
          case 4:
            encode_int32_t(temp, buffer, start_index);
            break;
          default:
            warn("buildUdpBuffer: Booleans must be 1, 2 or 4 bytes, not " << len);
            return -1;
          }
          start_index += len;
        }
        else if (type.compare("int") == 0) {
          if (valType != INTEGER_TYPE) {
            warn("buildUdpBuffer: Format requires Integer, but supplied value is not");
            return -1;
          }
          if (len != 2 && len != 4) {
            warn("buildUdpBuffer: Integers must be 2 or 4 bytes, not " << len);
            return -1;
          }
          if (debug)
            std::cout << len << " byte int starting at buffer[" << start_index << "]: " << plexil_val;
          Integer temp;
          plexil_val.getValue(temp);
          if (len == 2) {
            if (INT16_MIN > temp || temp > INT16_MAX) {
              warn("buildUdpBuffer: 2 byte integers must be between "
                   << INT16_MIN << " and " << INT16_MAX
                   << ", " << temp << " is not");
              return -1;
            }
            encode_short_int(temp, buffer, start_index);
          }
          else
            encode_int32_t(temp, buffer, start_index);
          start_index += len;
        }
        else if (type.compare("float") == 0) {
          if (valType != REAL_TYPE) {
            warn("buildUdpBuffer: Format requires Real, but supplied value is not");
            return -1;
          }
          if (len != 4) {
            warn("buildUdpBuffer: Reals must be 4 bytes, not " << len);
            return -1;
          }
          double temp;
          plexil_val.getValue(temp);
          // Catch really big floats
          if ((-FLT_MAX) > temp || temp > FLT_MAX) {
            warn("buildUdpBuffer: Reals (floats) must be between "
                 << (-FLT_MAX) << " and " << FLT_MAX <<
                 ", not " << plexil_val);
            return -1;
          }
          if (debug)
            std::cout << len << " byte float starting at buffer[" << start_index << "]: " << temp;
          encode_float((float) temp, buffer, start_index);
          start_index += len;
        }
        else if (type.compare("string") == 0) {
          if (valType != STRING_TYPE) {
            warn("buildUdpBuffer: Format requires String, but supplied value is not");
            return -1;
          }
          std::string const *str = nullptr;
          plexil_val.getValuePointer(str);
          if (str->length() > len) {
            warn("buildUdpBuffer: declared string length (" << len <<
                 ") and actual length (" << str->length() << ", " << *str <<
                 ") used in the plan are not compatible");
            return -1;
          }
          if (debug) 
            std::cout << len << " byte string starting at buffer[" << start_index << "]: " << str;
          encode_string(*str, buffer, start_index);
          start_index += len;
        }
        else if (type.compare("bool-array") == 0) {
          if (valType != BOOLEAN_ARRAY_TYPE) {
            warn("buildUdpBuffer: Format requires BooleanArray, but supplied value is not");
            return -1;
          }
          if (len != 1 && len != 2 && len != 4) {
            warn("buildUdpBuffer: Booleans must be 1, 2 or 4 bytes, not " << len);
            return -1;
          }
          unsigned int size = param->elements;
          BooleanArray const *array = nullptr;
          plexil_val.getValuePointer(array);
          if (debug)
            std::cout << size << " element array of "
                      << len << " byte booleans starting at buffer["
                      << start_index << "]: " << array->toString();
          if (size != array->size()) {
            warn("buildUdpBuffer: declared and actual (boolean) array sizes differ: "
                 << size << " was declared, but "
                 << array->size() << " is being used in the plan");
            return -1;
          }
          for (unsigned int i = 0 ; i < size ; i++) {
            bool temp;
            if (!array->getElement(i, temp)) {
              warn("buildUdpBuffer: Array element at index " << i << " is unknown");
              return -1;
            }
            switch (len) {
            case 1:
              buffer[start_index] = (unsigned char) temp;
              break;
            case 2:
              encode_short_int(temp, buffer, start_index);
              break;
            default:
              encode_int32_t(temp, buffer, start_index);
              break;
            }
            start_index += len;
          }
        }
        else if (type.compare("int-array") == 0) {
          if (plexil_val.valueType() != INTEGER_ARRAY_TYPE) {
            warn("buildUdpBuffer: Format requires IntegerArray, supplied value is a "
                 << valueTypeName(plexil_val.valueType()));
            return -1;
          }
          if (len != 2 && len != 4) {
            warn("buildUdpBuffer: Integers must be 2 or 4 bytes, not " << len);
            return -1;
          }
          unsigned int size = param->elements;
          IntegerArray const *array = nullptr;
          plexil_val.getValuePointer(array);
          if (debug)
            std::cout << size << " element array of " << len << " byte ints starting at ["
                      << start_index << "]: " << array->toString();
          if (size != array->size()) {
            warn("buildUdpBuffer: declared and actual array sizes differ: "
                 << size << " was declared, but "
                 << array->size() << " is being used in the plan");
            return -1;
          }
          for (unsigned int i = 0 ; i < size ; i++) {
            Integer temp;
            if (!array->getElement(i, temp)) {
              warn("buildUdpBuffer: Array element at index " << i << " is unknown");
              return -1;
            }
            if (len == 2) {
              if (INT16_MIN > temp || temp > INT16_MAX) {
                warn("buildUdpBuffer: 2 bytes integers must be between "
                     << INT16_MIN << " and " << INT16_MAX
                     << ", " << temp << " is not");
                return -1;
              }
              encode_short_int(temp, buffer, start_index);
            }
            else
              encode_int32_t(temp, buffer, start_index);
            start_index += len;
          }
        }
        else if (type.compare("float-array") == 0) {
          if (plexil_val.valueType() != REAL_ARRAY_TYPE) {
            warn("buildUdpBuffer: Format requires RealArray, supplied value is a "
                 << valueTypeName(plexil_val.valueType()));
            return -1;
          }
          if (len != 4) {
            warn("buildUdpBuffer: Reals must be 4 bytes, not " << len);
            return -1;
          }
          unsigned int size = param->elements;
          if (size < 1) {
            warn("buildUdpBuffer: all scalars and arrays must be of at least size 1, not " << size);
            return -1;
          }
          RealArray const *array = nullptr;
          plexil_val.getValuePointer(array);
          if (debug)
            std::cout << size << " element array of " << len << " byte floats starting at buffer["
                      << start_index << "]: " << array->toString();
          if (size != array->size()) {
            warn("buildUdpBuffer: declared and actual (float) array sizes differ: "
                 << size << " was declared, but "
                 << array->size() << " is being used in the plan");
            return -1;
          }
          for (unsigned int i = 0 ; i < size ; i++) {
            double temp;
            if (!array->getElement(i, temp)) {
              warn("buildUdpBuffer: Array element at index " << i << " is unknown");
              return -1;
            }
            if ((-FLT_MAX) > temp || temp > FLT_MAX) {
              warn("buildUdpBuffer: Reals (floats) must be between "
                   << (-FLT_MAX) << " and " << FLT_MAX <<
                   ", " << temp << " is not");
              return -1;
            }
            encode_float((float) temp, buffer, start_index);
            start_index += len;
          }
        }
        else if (type.compare("string-array") == 0) {
          if (plexil_val.valueType() != STRING_ARRAY_TYPE) {
            warn("buildUdpBuffer: Format requires StringArray, supplied value is a "
                 << valueTypeName(plexil_val.valueType()));
            return -1;
          }
          unsigned int size = param->elements;
          StringArray const *array = nullptr;
          plexil_val.getValuePointer(array);
          if (debug)
            std::cout << size << " element array of " << len << " byte strings starting at buffer["
                      << start_index << "]: " << array->toString();
          if (size != array->size()) {
            warn("buildUdpBuffer: declared and actual (string) array sizes differ: "
                 << size << " was declared, but " << array->size() << " is being used in the plan");
            return -1;
          }
          for (unsigned int i = 0 ; i < size ; i++) {
            std::string const *temp = nullptr;
            if (!array->getElementPointer(i, temp)) {
              warn("buildUdpBuffer: Array element at index " << i << " is unknown");
              return -1;
            }
            if (temp->length() > len) {
              warn("buildUdpBuffer: declared string length (" << len <<
                   ") and actual length (" << temp->length() <<
                   ") used in the plan are not compatible");
              return -1;
            }
            encode_string(*temp, buffer, start_index);
            start_index += len;
          }
        }
        else {
          warn("buildUdpBuffer: unknown parameter type " << type);
          return -1;
        }

        if (debug)
          std::cout << std::endl;
      }
      if (debug) {
        std::cout << "  buildUdpBuffer: buffer: ";
        print_buffer(buffer, msg.len);
      }
      return start_index;
    }

    void printMessageContent(const std::string& name, const std::vector<Value>& args)
    {
      // Print the content of a message
      std::vector<Value>::const_iterator it;
      std::cout << "Message: " << name << ", Params:";
      for (it=args.begin(); it != args.end(); it++) {
        // Real, Integer, Boolean, String (and Array, maybe...)
        // Integers and Booleans are represented as Real (oops...)
        std::cout << " ";
        if (it->valueType() == STRING_TYPE) { 
          // Extract strings
          std::string const *temp = nullptr;
          if (it->getValuePointer(temp))
            std::cout << "\"" << *temp << "\"";
          else
            std::cout << "UNKNOWN";
        }
        else { // Extract numbers (bool, float, int)
          std::cout << *it;
        }
      }
      std::cout << std::endl;
    }
    
    //
    // Member variables
    //

    std::mutex m_cmdMutex;
    std::unique_ptr<UdpEventLoop> m_eventLoop;
    
    // Somewhere to hang the messages, default ports and peers, threads and sockets
    std::string m_default_peer;
    MessageMap m_messages;
    MessageQueueMap m_messageQueues;
    unsigned int m_default_local_port;
    unsigned int m_default_peer_port;
    bool m_debug; // Show debugging output

  }; // class UdpAdapter

} // namespace PLEXIL

// Register the UdpAdapter
extern "C"
void initUdpAdapter()
{
  REGISTER_ADAPTER(PLEXIL::UdpAdapter, "UdpAdapter");
}

