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

  if (request.type() == Type::Confirmable && request.code() == Code::Empty)
    reply(fromIP, fromPort, Type::Reset, request.messageId(), request.token(), RestResponse().withCode(Code::Empty));
  else
    reply(fromIP, fromPort, request.type(), request.messageId(), request.token(), onRequest(request, fromIP, fromPort));
}

RestResponse ServerImpl::onRequest(const Message& request, in_addr_t fromIP, uint16_t fromPort) {
  switch (request.code()) {
    case Code::Empty:
      return RestResponse().withCode(Code::Empty);

    case Code::GET:
      if (requestHandler_.isGetDelayed(Path(request.path()))) {
        reply(fromIP, fromPort, CoAP::Type::Acknowledgement, request.messageId(), 0, RestResponse());
      }
      return requestHandler_.GET(Path(request.path()));

    case Code::PUT:
      return requestHandler_.PUT(Path(request.path()), request.payload());

    case Code::POST:
      return requestHandler_.POST(Path(request.path()), request.payload());

    case Code::DELETE:
      return requestHandler_.DELETE(Path(request.path()));

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
  if (response.hasContentFormat()) message.setContentFormat(response.contentFormat());
  messaging_.sendMessage(ip, port, message);
}

}  // namespace CoAP

