#include <condition_variable>
#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <thread>
#include <vector>

using namespace std;
/*
 * Class holding a number of worker threads. Runs given tasks in parallel.
 */
class Workers {
public:
  bool wait = true;
  bool run = true;
  int sleepingThreads = 0;
  mutex sleepingThreads_mutex;
  int threadcount;
  vector<thread> worker_threads;
  list<function<void()>> tasks;
  mutex wait_mutex;
  condition_variable cv;

  Workers(int threadnum) : threadcount(threadnum) {
  }

  void start() {
    cout << "Initializing worker threads" << endl;
    for (int i = 0; i < threadcount; i++) {
      worker_threads.emplace_back([this] {
        while (true) {
          function<void()> task;
          {
            unique_lock<mutex> lock(wait_mutex);
            while (wait)
              cv.wait(lock);

            if (!tasks.empty()) {
              task = *tasks.begin();
              tasks.pop_front();
            }
          }
          if (task) {
            task();
          }
          if (tasks.empty() && !run) {
            break;
          }
        }

        {
          unique_lock<mutex> lock(wait_mutex);
          wait = false;
        }
        cv.notify_one();
      });
    }
  }
  void join() {
    stop();
    for (auto &thread : worker_threads) {
      thread.join();
    }
  }

  void post(function<void()> func) {
    unique_lock<mutex> lock(wait_mutex);
    tasks.emplace_back(func);
    wait = false;
    cv.notify_one();
  }

  void post_timeout(function<void()> func, int timeInMilliseconds) {
    sleepingThreads_mutex.lock();
    sleepingThreads++;
    sleepingThreads_mutex.unlock();
    thread t([this, timeInMilliseconds, func] {
      this_thread::sleep_for(chrono::milliseconds(timeInMilliseconds));
      post(func);
      sleepingThreads_mutex.lock();
      sleepingThreads--;
      sleepingThreads_mutex.unlock();
    });
    t.detach();
  }

  void stop() {
    while (true) {
      if (sleepingThreads == 0) {
        run = false;
        break;
      }
    }
  }
};

int main() {
  Workers workers(4);
  workers.start();

  workers.post([] {
    for (int i = 0; i < 1; i++) {
      cout << "task A worked by thread " << this_thread::get_id() << endl;
      this_thread::sleep_for(1s);
    }
    cout << "Task A finished" << endl;
  });
  workers.post_timeout([] {
    for (int i = 0; i < 10; i++) {
      cout << "task B worked by thread " << this_thread::get_id() << endl;
      this_thread::sleep_for(1s);
    }
    cout << "Task B finished" << endl;
  },
                       1000);

  workers.post([] {
    for (int i = 0; i < 5; i++) {
      cout << "task C worked by thread " << this_thread::get_id() << endl;
      this_thread::sleep_for(1s);
    }
    cout << "Task C finished" << endl;
  });

  workers.post([] {
    for (int i = 0; i < 3; i++) {
      cout << "task D worked by thread " << this_thread::get_id() << endl;
      this_thread::sleep_for(1s);
    }
    cout << "Task D finished" << endl;
  });

  workers.join();
  cout << "Workers returned" << endl;
}
