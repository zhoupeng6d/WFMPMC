/*
 * @Author: Dash Zhou
 * @Date: 2021-01-01 17:26:01
 * @LastEditors: Dash Zhou
 * @LastEditTime: 2021-01-01 19:11:06
 */

#include <chrono>
#include <thread>

#include "WFMPMC.h"
#include "gtest/gtest.h"

TEST(test_queue, base) {
  WFMPMC<int, 8> q;

  // write integer 123 into queue
  auto idx = q.getWriteIdx();
  int* data;
  while ((data = q.getWritable(idx)) == nullptr)
    ;
  *data = 123;
  q.commitWrite(idx);

  // read an integer from queue
  auto idxr = q.getReadIdx();
  int* read;
  while ((read = q.getReadable(idxr)) == nullptr)
    ;
  ASSERT_EQ(*read, 123);
  q.commitRead(idxr);
}

#define TEST_COUNT 100000

WFMPMC<int, 8> q2;

void producer() {
  int count = 0;
  while (count++ <= TEST_COUNT) {
    auto idx = q2.getWriteIdx();
    int* data;
    while ((data = q2.getWritable(idx)) == nullptr) {
      // std::cout << "waiting..." << std::endl;
    }

    *data = count;

    q2.commitWrite(idx);
  }
}

// Fast production slow consumption
void consumer() {
  int count = 0;
  while (count++ <= TEST_COUNT) {
    // read an integer from queue
    auto idxr = q2.getReadIdx();
    int* read;
    while ((read = q2.getReadable(idxr)) == nullptr) {
    }
    std::this_thread::sleep_for(std::chrono::microseconds(1));

    ASSERT_EQ(*read, count);
    if (*read != count) return;

    q2.commitRead(idxr);
  }
}

TEST(test_queue, fast_production_slow_consumption) {
  std::thread pro(producer);

  std::thread con(consumer);

  pro.join();
  con.join();
}

void producer2() {
  int count = 0;
  while (count++ <= TEST_COUNT) {
    auto idx = q2.getWriteIdx();
    int* data;
    while ((data = q2.getWritable(idx)) == nullptr) {
    }
    std::this_thread::sleep_for(std::chrono::microseconds(1));

    *data = count;

    q2.commitWrite(idx);
  }
}

// slow production fast consumption
void consumer2() {
  int count = 0;
  while (count++ <= TEST_COUNT) {
    // read an integer from queue
    auto idxr = q2.getReadIdx();
    int* read;
    if ((read = q2.getReadableWaiting(idxr, 10)) == nullptr) {
      std::cout << "timeout..." << std::endl;
    }
    ASSERT_NE(read, nullptr);
    if (read == nullptr) return;

    ASSERT_EQ(*read, count);
    q2.commitRead(idxr);
  }
}

TEST(test_queue, slow_production_fast_consumption) {
  std::thread pro(producer2);

  std::thread con(consumer2);

  pro.join();
  con.join();
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int ret = RUN_ALL_TESTS();
  return ret;
}
