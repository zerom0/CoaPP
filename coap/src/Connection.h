/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __Connection_h
#define __Connection_h

#include "Telegram.h"
#include "IConnection.h"

#include <vector>

struct hostent;

namespace CoAP {

class Connection : public IConnection {
 public:
  Connection() = default;

  virtual ~Connection() = default;

  /**
   * Opens a connection on the given port.
   *
   * @param port  Number of the port to open for incoming and outgoing telegrams
   *
   * @throws  std::logic_error    when open was already called before
   * @throws  std::runtime_error  when the connection could not be opened
   */
  void open(uint16_t port);

  /**
   * Closes the socket and resets the internal state of this object.
   */
  void close();

  void send(Telegram&& telegram) override;

  Optional<Telegram> get(std::chrono::milliseconds timeout) override;

 protected:
  // sets the receive timeout on the socket
  virtual void setReceiveTimeout(std::chrono::milliseconds timeout);

  // trampoline for unit test to override system call socket
  virtual int socket(int domain, int type, int protocol) const;

  // trampoline for unit test to override system call bind
  virtual int bind(int socket, const struct sockaddr* address, socklen_t address_len) const;

  // trampoline for unit test to override system call setsockopt
  virtual int setsockopt(int socket, int level, int option_name, const void* option_value, socklen_t option_len) const;

 private:
  int socket_{0};
  static constexpr int bufferSize_{2048};
  uint8_t buffer_[bufferSize_];
};

}  // namespace CoAP


#endif  // __Connection_h
