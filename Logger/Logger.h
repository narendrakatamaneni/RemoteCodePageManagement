#ifndef LOGGERA_H
#define LOGGERA_H
/////////////////////////////////////////////////////////////////////
// Logger.h - log text messages to std::ostream                    //
// ver 1.0                                                         //
// Author:     Narendra Katamaneni, Syracuse University            //
/////////////////////////////////////////////////////////////////////

/*
* Package Operations:
* -------------------
* This package supports logging for multiple concurrent clients to a
* single std::ostream.  It does this be enqueuing messages in a
* blocking queue and dequeuing with a single thread that writes to
* the std::ostream.
*
* Build Process:
* --------------
* Required Files: Cpp11-BlockingQueue.h
*
* Build Command: devenv logger.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.0 : 11 Feb 2019
* - added multiple stream capability
*
* Planned Additions and Changes:
* ------------------------------
* - none yet
*/

#include <iostream>
#include <string>
#include <thread>
#include "Cpp11-BlockingQueue.h"

namespace Logging {

  class Logger
  {
  public:
    Logger() {
	}
    bool attach(std::ostream* pOut);
    bool detach(std::ostream* pOut);
    void start();
    bool running();
    void pause(bool doPause);
    bool paused();
    void flush();
    void stop(const std::string& msg = "");
    void wait();
    void write(const std::string& msg);
    void title(const std::string& msg, char underline = '-');
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
  private:
    std::thread* _pThr;
    std::vector<std::ostream*> _streams;
    BlockingQueue<std::string> _queue;
    bool _ThreadRunning = false;
    bool _Paused = false;
  };

  template<int i>
  class StaticLogger
  {
  public:
    static bool attach(std::ostream* pOut) { return _logger.attach(pOut); }
    static bool detach(std::ostream* pOut) { return _logger.detach(pOut); }
    static void start() { _logger.start(); }
    static bool running() { return _logger.running(); }
    static void flush() { _logger.flush(); }
    static void stop(const std::string& msg = "") { _logger.stop(msg); }
    static void write(const std::string& msg) { _logger.write(msg); }
    static void title(const std::string& msg, char underline = '-') { _logger.title(msg, underline); }
    static Logger& instance() { return _logger; }
    StaticLogger(const StaticLogger&) = delete;
    StaticLogger& operator=(const StaticLogger&) = delete;
  private:
    static Logger _logger;
  };

  template<int i>
  Logger StaticLogger<i>::_logger;

  template<int i>
  inline static StaticLogger<i> operator <<(StaticLogger<i>, const std::string& str) {
    StaticLogger<i>::write(str);
    return StaticLogger<i>;
  }

  using LoggerDebug = StaticLogger<1>;
  using LoggerDemo = StaticLogger<2>;
}

#endif
