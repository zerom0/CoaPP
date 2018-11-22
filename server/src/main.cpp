/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "CoAP.h"

#include <list>
#include <map>
#include <string>

int main() {
  auto messaging = CoAP::newMessaging();

  auto name = std::string("coap_server");
  auto dynamic = std::map<int, std::string>();
  auto dynamic_index = 0;
  int counter = 0;
  std::list<std::weak_ptr<CoAP::Notifications>> notificationObservers;
  auto getObservable = [&counter](const Path&){
    return CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload(std::to_string(counter));
  };

  messaging->requestHandler()
      .onUri("/name")
          .onGet([&name](const Path&){
            return CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload(name);
          })
          .onPut([](const Path&, const std::string&){
            return CoAP::RestResponse().withCode(CoAP::Code::Changed);
          })
      .onUri("/dynamic")
          .onPost([&dynamic, &dynamic_index](const Path&, const std::string& payload){
            dynamic.insert(std::make_pair(++dynamic_index, payload));
            return CoAP::RestResponse().withCode(CoAP::Code::Created).withPayload(std::to_string(dynamic_index));
          })
      .onUri("/dynamic/?")
          .onGet([&dynamic](const Path& path){
            auto payload = std::string("???");
            if (path.size() == 2) {
              auto index = stoi(path.getPart(1));
              auto it = dynamic.find(index);
              if (it != dynamic.end()) return CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload(it->second);
            }
            return CoAP::RestResponse().withCode(CoAP::Code::NotFound);
          })
          .onDelete([&dynamic](const Path& path) {
            if (path.size() == 2) {
              auto index = stoi(path.getPart(1));
              auto it = dynamic.find(index);
              if (it != dynamic.end()) {
                dynamic.erase(it);
                return CoAP::RestResponse().withCode(CoAP::Code::Deleted);
              }
            }
            return CoAP::RestResponse().withCode(CoAP::Code::NotFound);
          })
      .onUri("/observable")
           .onGet(getObservable)
           .onObserve([&notificationObservers](const Path&, std::weak_ptr<CoAP::Notifications> observer){
             notificationObservers.emplace_back(observer);
             return CoAP::RestResponse().withCode(CoAP::Code::Content);
           });

  int delay = 0;
  for(;;) {
    messaging->loopOnce();
    // currently loopOnce times out after 100ms thus sending notifications
    // every 10th time results in one notification per second.
    if (++delay % 10 == 0) {
      for(auto& notifications : notificationObservers) {
        auto sp = notifications.lock();
        if (sp) sp->onNext(getObservable(Path("")));
      }
      ++counter;
    }
  }
}
