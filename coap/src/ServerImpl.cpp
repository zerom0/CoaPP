/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ServerImpl.h"

#include "RequestHandlerDispatcher.h"
#include "RequestHandler.h"
#include "IConnection.h"
#include "Logging.h"
#include "Message.h"
#include "Messaging.h"

#include <thread>

SETLOGLEVEL(LLWARNING)

namespace CoAP {

void ServerImpl::onMessage(const Message& request, in_addr_t fromIP, uint16_t fromPort) {
  ILOG << "Received request with msgID=" << request.messageId()
      << " and token=" << request.token()
      << " with code=" << request.code()
      << " and " << request.payload().length() << " bytes payload.\n";

  if (request.code() == Code::Empty && request.type() == Type::Confirmable) {
    // Ping request gets ping response
    reply(fromIP, fromPort, Type::Reset, request.messageId(), request.token(), RestResponse().withCode(Code::Empty));
  }
  else if (request.type() == Type::Reset) {
    // TODO: Timeout for confirmable notification messages also cancels the observation
    if (observations_.erase(std::make_tuple(fromIP, fromPort, request.token()))) {
      ILOG << "Observation cancelled, " << observations_.size() << " active observations\n";
    }
  }
  else {
    reply(fromIP, fromPort, request.type(), request.messageId(), request.token(), onRequest(request, fromIP, fromPort));
  }
}

RestResponse ServerImpl::onRequest(const Message& request, in_addr_t fromIP, uint16_t fromPort) {
  const auto path = Path(request.path());

  switch (request.code()) {
    case Code::Empty:
      return RestResponse().withCode(Code::Empty);

    case Code::GET:
      if (request.hasObserveValue()) {
        if (requestHandler_.isObserveDelayed(path)) {
          // Send acknowledgement for delayed responses
          reply(fromIP, fromPort, CoAP::Type::Acknowledgement, request.messageId(), 0, RestResponse());
        }

        if (request.observeValue() == 0) {
          createObservation(fromIP, fromPort, request.type(), request.token(), path);
        } else if (request.observeValue() == 1) {
          deleteObservation(fromIP, fromPort, request.token());
        }
        else {
          ELOG << "Received observe request with unsupported observe value "
               << request.observeValue() << '\n';
        }
      }
      else {
        if (requestHandler_.isGetDelayed(path)) {
          // Send acknowledgement for delayed responses
          reply(fromIP, fromPort, CoAP::Type::Acknowledgement, request.messageId(), 0, RestResponse());
        }
      }
      return requestHandler_.GET(path);

    case Code::PUT:
      return requestHandler_.PUT(path, request.payload());

    case Code::POST:
      return requestHandler_.POST(path, request.payload());

    case Code::DELETE:
      return requestHandler_.DELETE(path);

    default:
      // We reply with bad request if we receive an unknown request code
      return RestResponse().withCode(Code::BadRequest);
  }
}

// TODO: Test that server responds with correct messageId
void ServerImpl::reply(in_addr_t ip,
                       uint16_t port,
                       Type type,
                       MessageId messageId,
                       uint64_t token,
                       const RestResponse& response) {
  auto message = CoAP::Message(type, messageId, response.code(), token, "", response.payload());
  if (response.hasContentFormat()) message.withContentFormat(response.contentFormat());
  messaging_.sendMessage(ip, port, message);
}

void ServerImpl::createObservation(in_addr_t fromIP,
                                   uint16_t fromPort,
                                   Type requestType,
                                   uint64_t token,
                                   const Path& path) {
  // TODO: Keep sending notifications as long as the client is interested.
  //       The client indicates its disinterest in further notifications by replying with a reset messages.
  auto observation = std::make_shared<Notifications>();
  observations_.emplace(std::make_tuple(fromIP, fromPort, token), observation);
  ILOG << observations_.size() << " active observations\n";
  observation->subscribe([this, fromIP, fromPort, requestType, token](const CoAP::RestResponse& response){
    // TODO: reply with unique messageIDs??
    reply(fromIP, fromPort, requestType, 0, token, response);
  });
  requestHandler_.OBSERVE(path, observation);
}

void ServerImpl::deleteObservation(in_addr_t fromIP, uint16_t fromPort, uint64_t token) {
  if (!observations_.erase(std::make_tuple(fromIP, fromPort, token))) {
    ELOG << "Received remove observation request for not observed ressource with token "
         << token << '\n';
  }
}

}  // namespace CoAP

