//
// Created by daniil on 6/2/23.
//

#ifndef EPAMMIDDLE_DLMANAGER_HPP
#define EPAMMIDDLE_DLMANAGER_HPP

#include <set>

#include "Downloader.hpp"

namespace Clone::TransportLayer {

class DLManager {
 public:

  void registerSession(Downloader_ptr session){
    m_DLManager.insert(session);
  }

  void stopSession(Downloader_ptr session)
  {
    m_DLManager.erase(session);
  }

 private:
  std::set<Downloader_ptr> m_DLManager;
};

};      // namespace Clone::TransportLayer
#endif  // EPAMMIDDLE_DLMANAGER_HPP
