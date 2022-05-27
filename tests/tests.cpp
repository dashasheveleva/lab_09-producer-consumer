// Copyright 2022 Shevelyova Darya photodoshfy@gmail.com

#include <stdexcept>
#include <thread>
#include <gtest/gtest.h>

TEST(DISABLED_Snapshot, Speen) {
  for (;;) std::this_thread::yield();
}
