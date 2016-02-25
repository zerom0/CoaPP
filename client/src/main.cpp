/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "CoAP.h"
#include "URI.h"
#include "Arguments.h"

#include <iostream>
#include <Logging.h>
#include <arpa/inet.h>

SETLOGLEVEL(LLWARNING)

/* Sample arguments:
 *
 * GET -n coap://*:5683/.well-known/core
 * GET -n coap://localhost:5683/.well-known/core
 * PUT -n coap://localhost:5683/actions/shutdown now
 */

void usage() {
  std::cout << "Usage: client <request> [-n] <uri> [<payload>]\n";
  std::cout << "       request : get/put/post/delete\n";
  std::cout << "       -n      : nonconfirmable message\n";
  std::cout << "       uri     : coap://localhost:5683/.well-known/core\n";
  std::cout << "                 Use * instead of the servername for multicast\n";
  std::cout << "       payload : string with payload\n";
  exit(1);
}

int main(int argc, const char* argv[]) {
  const auto arguments = Arguments::fromArgv(argc, argv);
  if (!arguments) usage();
  
  auto messaging = CoAP::newMessaging(9999);
  messaging->loopStart();

  const auto uri = arguments.value().getUri();
  const auto requestType = arguments.value().getRequest();

  if (uri.getServer() == "*") {
    // Multicast requests
    //

    if (requestType != "get") {
      std::cout << "Invalid request type " << requestType << " for multicast requests\n";
      usage();
    }

    auto client = messaging->getMulticastClient();

    auto responses = client.GET(uri.getPath());

    while (responses.empty());

    auto response = responses.get();
    in_addr addr = {response.fromIp()};
    std::cout << "IP: " << inet_ntoa(addr) << " Port: " << response.fromPort() << '\n';
    std::cout << response.code();
    if (response.hasContentFormat()) {
      std::cout << " - ContentFormat: " << response.contentFormat();
    }
    std::cout << '\n';
    std::cout << response.payload() << '\n';
  }
  else {
    // Unicast requests
    //
    auto client = messaging->getClientFor(uri.getServer().c_str());
    const auto payload = arguments.value().getPayload();

    CoAP::RestResponse response;

    if (requestType == "get") {
      response = client.GET(uri.getPath(), arguments.value().isConfirmable()).get();
    }
    else if (requestType == "put") {
      response = client.PUT(uri.getPath(), payload, arguments.value().isConfirmable()).get();
    }
    else if (requestType == "post") {
      response = client.POST(uri.getPath(), payload, arguments.value().isConfirmable()).get();
    }
    else if (requestType == "delete") {
      response = client.DELETE(uri.getPath(), arguments.value().isConfirmable()).get();
    } else {
      std::cerr << "Unknown request type: " << requestType << '\n';
    }

    std::cout << response.code();
    if (response.hasContentFormat()) {
      std::cout << " - ContentFormat: " << response.contentFormat();
    }
    std::cout << '\n';
    std::cout << response.payload() << '\n';
  }

  messaging->loopStop();
}
