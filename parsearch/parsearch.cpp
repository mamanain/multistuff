#include <iostream>
#include <thread>
#include <string>
#include <future>
#include <vector>
#include <chrono>
#include <string>

#include "parallel_queue.h"
#include "matcher.hpp"
#include "crawler.hpp"

int main() {

  int MAX_WAIT_TIME = 1000;
  ParallelQueue<std::string> q(MAX_WAIT_TIME);
  FileMatcher f;

  std::promise<void> death_signal;
  std::shared_future<void>  death_future(death_signal.get_future());

  std::cout << "Creating threads!\n";

  std::vector <std::thread> matchers;
  for (int i = 0; i < 4; ++i) {
    std::thread thr(f, std::ref(q), std::string("#!/bin/sh"), death_future);
    matchers.push_back(std::move(thr));
  }

  std::cout << "Waiting for directories!\n";
  FolderCrawler crl;
  std::thread crawler_thread(crl, std::ref(q), std::string("/home/max/Documents"));
  crawler_thread.join();

  std::cout << "Terminating!\n";
  death_signal.set_value();

  for( size_t i = 0; i < matchers.size(); ++i )
    matchers[i].join();

  return 0;
}