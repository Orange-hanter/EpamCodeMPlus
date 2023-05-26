//
// Created by daniil on 5/22/23.
//

#ifndef EPAMMIDDLE_ITRANSPORTLAYER_HPP
#define EPAMMIDDLE_ITRANSPORTLAYER_HPP
namespace Clone::TransportLayer {


class iTransportLayer {
 public:
  virtual void write() { /*TODO make pure virtual*/ };
  virtual void read() { /*TODO make pure virtual*/  };
  virtual void start() {};
  virtual void stop() {};
  virtual void restart() {/*TODO no idea is that necessary*/};
  virtual ~iTransportLayer() = default;
};

}  // namespace Clone::TransportLayer
#endif  // EPAMMIDDLE_ITRANSPORTLAYER_HPP
