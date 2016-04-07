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
  std::cout << "       request : get/put/post/delete/observe\n";
  std::cout << "       -n      : nonconfirmable message\n";
  std::cout << "       uri     : coap://localhost:5683/.well-known/core\n";
  std::cout << "                 Use * instead of the servername for multicast\n";
  std::cout << "       payload : string with payload\n";
  exit(1);
}

void printResponse(const CoAP::RestResponse& response) {
  std::cout << response.code();
  if (response.hasContentFormat()) {
        std::cout << " - ContentFormat: " << response.contentFormat();
      }
  std::cout << '\n';
  std::cout << response.payload() << '\n';
}

int main(int argc, const char* argv[]) {
  const auto arguments = Arguments::fromArgv(argc, argv);
  if (!arguments) usage();

  std::unique_ptr<CoAP::IMessaging> messaging;
  uint16_t port = 9999;
  while (!messaging && port > 9900) {
    try {
      messaging = CoAP::newMessaging(port);
    } catch (std::exception& e) {
      std::cout << "Port " << port << " already in use, trying port " << --port << '\n';
    }
  }
  messaging->loopStart();
  bool exit(true);

  const auto uri = arguments.value().getUri();
  const auto requestType = arguments.value().getRequest();
  std::shared_ptr<Observable<CoAP::RestResponse>> notifications;

  if (uri.getServer() == "*") {
    // Multicast requests
    //

    if (requestType != "get") {
      std::cout << "Invalid request type " << requestType << " for multicast requests\n";
      usage();
    }

    auto client = messaging->getMulticastClient();

    notifications = client.GET(uri.getPath());

    std::vector<CoAP::RestResponse> responses;
    notifications->subscribe([&responses](const CoAP::RestResponse& response){
      responses.push_back(response);
    });
    while (responses.empty());

    auto response = responses.front();
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

    Optional<CoAP::RestResponse> response;

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
    }
    else if (requestType == "observe") {
      exit = false;
      notifications = client.OBSERVE(uri.getPath(), arguments.value().isConfirmable());
      notifications->subscribe([&exit](const CoAP::RestResponse& response) {
        printResponse(response);
        //exit = true;
      });
    }
    else {
      std::cerr << "Unknown request type: " << requestType << '\n';
    }

    if (response) printResponse(response.value());
  }

  while (!exit) {
    timespec t;
    t.tv_sec = 0;
    t.tv_nsec = 100*1000*1000;
    nanosleep(&t, &t);
  }

  messaging->loopStop();
}
