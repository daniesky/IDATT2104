#include <chrono>
#include <iostream>
#include <list>
#include <mutex>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace std;
using namespace std::chrono;
/*
 * Program that uses specified number of threads to find  prime numbers. 
 */
int main() {
  vector<thread> threads;
  int minnr, maxnr, threadcount = 0;
  mutex minnr_mutex;
  list<int> result;
  mutex result_mutex;

  cout << "Input the lower end of the interval:";
  cin >> minnr;
  cout << "Input the upper end of the interval: ";
  cin >> maxnr;
  cout << "Input the number of threads you wish to run: ";
  cin >> threadcount;
  vector<int> calccount(threadcount);

  cout << minnr << " - " << maxnr << " - " << threadcount << endl;

  auto start = high_resolution_clock::now();

  for (int i = 0; i < threadcount; i++) {
    threads.emplace_back([i, &minnr, &maxnr, &minnr_mutex, &result, &result_mutex, &calccount] { // i is copied to the thread, do not capture i as reference (&i) as it might be freed before all the threads finishes.
      int temp;
      while (true) {
        minnr_mutex.lock();
        temp = minnr;
        minnr++;
        minnr_mutex.unlock();
        int flag = 0;
        if (temp > maxnr) {
          break;
        }
        if (temp == 0 || temp == 1) {
          flag = 1;
        } else {
          for (int i = 2; i <= temp / 2; i++) {
            if (temp % i == 0) {
              flag = 1;
              break;
            }
          }
        }
        if (flag == 0) {
          result_mutex.lock();
          result.push_back(temp);
          result_mutex.unlock();
          calccount[i]++;
        }
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  auto stop = high_resolution_clock::now();
  /*
  for (int i = 0; i < threadcount; i++) {
    cout << "Thread " << i << " performed " << calccount[i] << " calculations." << endl;
  }
  */
  //result.sort();
  cout << result.size() << " prime numbers in the interval" << endl;

  for (int e : result) {
    cout << e << endl;
  }

  auto duration = duration_cast<seconds>(stop - start);
  cout << "Calculation took " << duration.count() << " s" << endl;
}
