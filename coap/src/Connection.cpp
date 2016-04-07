/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Connection.h"
#include "NetUtils.h"

#include <iostream>
#include <stdexcept>
#include <cstring>

namespace CoAP {

// Multicast "All CoAP Nodes" Address
// for IPv4: 224.0.1.187
// for IPv6: link-local scoped address ff02::fd and the site-local scoped address ff05::fd
void Connection::open(uint16_t port) {
  if (socket_ != 0) throw std::logic_error("Cannot open already open connection.");

  socket_ = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  struct sockaddr_in sa;
  if (socket_ == 0) {
    perror("creating socket failed");
    exit(EXIT_FAILURE);
  }

  const int32_t timeoutInMS = 100;
  timeval timeout{timeoutInMS / 1000, (timeoutInMS % 1000) * 1000};
  if (-1 == setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))) {
    perror("setting receive timeout on socket failed");
    close(socket_);
    exit(EXIT_FAILURE);
  }

  auto const WITH_MULTICAST = true;
  if (WITH_MULTICAST) {
    // enable multicast messages to be sent back to the local host
    u_char loop = 1;
    if (-1 == setsockopt(socket_, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop))) {
      perror("setting multicast loopback on socket failed");
      close(socket_);
      exit(EXIT_FAILURE);
    }

    // joining a multicast group
    ip_mreq mreq;
    mreq.imr_interface.s_addr = INADDR_ANY;
    mreq.imr_multiaddr.s_addr = inet_addr("224.0.1.187");
    if (-1 == setsockopt(socket_, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq))) {
      perror("setting multicast membership on socket failed");
      close(socket_);
      exit(EXIT_FAILURE);
    }
  }

  memset(&sa, 0, sizeof sa);
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = htonl(INADDR_ANY);
  sa.sin_port = htons(port);

  // TODO: Throw exception if bind fails
  if (-1 == bind(socket_, (struct sockaddr*) &sa, sizeof(sa))) {
    throw(std::runtime_error("bind failed"));
  }
}

void Connection::send(Telegram&& telegram) {
  if (socket_ == 0) throw std::logic_error("Cannot send on connection if it is not open.");

  struct sockaddr_in sa;

  memset(&sa, 0, sizeof sa);
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = telegram.getIP();
  sa.sin_port = htons(telegram.getPort());

  // TODO: How can we replace this temporary copy, e.g. by a move or reference
  auto msg = telegram.getMessage();
  auto bytes_sent = sendto(socket_, msg.data(), msg.size(), 0, (struct sockaddr*) &sa, sizeof(sa));
  if (bytes_sent < 0) {
    // TODO: Throw a better exception if send fails
    printf("Error sending packet: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
}

Optional<Telegram> Connection::get() {
  if (socket_ == 0) throw std::logic_error("Cannot send on connection if it is not open.");

  struct sockaddr_in sa;
  socklen_t fromlen = sizeof(sa);
  memset(&sa, 0, sizeof(sa));
  ssize_t bytesReceived = recvfrom(socket_, buffer_, bufferSize_, 0, (struct sockaddr*) &sa, &fromlen);
  // TODO: Throw exception if receive failed
  if (bytesReceived < 0) {
    if (errno == EAGAIN) return Optional<Telegram>();
    fprintf(stderr, "%s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  return Optional<Telegram>(sa.sin_addr.s_addr, ntohs(sa.sin_port), std::vector<uint8_t>(buffer_, buffer_+bytesReceived));
}

int Connection::socket(int domain, int type, int protocol) const {
  return ::socket(domain, type, protocol);
}

int Connection::setsockopt(int socket, int level, int option_name,
                           const void* option_value, socklen_t option_len) const {
  return ::setsockopt(socket, level, option_name, option_value, option_len);
}

int Connection::bind(int socket, const struct sockaddr* address, socklen_t address_len) const {
  return ::bind(socket, address, address_len);
}
}  // namespace CoAP
