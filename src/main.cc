/*
 * Copyright (c) 2012-2016, Nic McDonald
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * - Neither the name of prim nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior
 * written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <des/SpinLock.h>
#include <prim/prim.h>
#include <tclap/CmdLine.h>

#include <cmath>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <ratio>
#include <thread>
#include <vector>

bool doLock = true;
des::SpinLock lock_;

void bankTransaction(volatile s64* _balance, s64 _delta) {
  if (doLock) {
    lock_.lock();
  }
  *_balance += _delta;
  if (doLock) {
    lock_.unlock();
  }
}

volatile bool go = false;

void bankTeller(u32 _id, volatile s64* _account, u64 _transactions,
                f64* _delay) {
  s64 amount = (s64)_id + 100;
  printf("teller %u starting\n", _id);
  while (!go) {}  // attempt to sync threads
  std::chrono::steady_clock::time_point start =
      std::chrono::steady_clock::now();
  for (u64 c = 0; c < _transactions; c++) {
    bankTransaction(_account, c % 2 == 0 ? amount : -amount);
  }
  std::chrono::steady_clock::time_point end =
      std::chrono::steady_clock::now();
  double delay = std::chrono::duration_cast<
    std::chrono::duration<double> >(end - start).count();
  printf("teller %u ended (%f seconds)\n", _id, delay);
  *_delay = delay;
}

int main(s32 _argc, char** _argv) {
  u32 numThreads = U32_MAX;
  u64 numRounds = U64_MAX;

  try {
    TCLAP::CmdLine cmd("Command description message", ' ', "0.0.1");
    TCLAP::ValueArg<u32> threadsArg("t", "threads", "Number of threads", false,
                                    2, "u32", cmd);
    TCLAP::ValueArg<u64> roundsArg("r", "rounds", "Number of rounds", false,
                                   1000000, "u64", cmd);
    TCLAP::SwitchArg nolockArg("n", "nolock", "Turn off spinlocks", cmd, false);

    cmd.parse(_argc, _argv);

    numThreads = threadsArg.getValue();
    numRounds = roundsArg.getValue();
    doLock = !nolockArg.getValue();
  } catch (TCLAP::ArgException &e) {
    fprintf(stderr, "error: %s for arg %s\n",
            e.error().c_str(), e.argId().c_str());
    exit(-1);
  }

  if (numThreads > std::thread::hardware_concurrency()) {
    fprintf(stderr, "WARNING: 'threads' is greater than the amount of hardware "
            "concurrency. Prepare for terrible performance!\n");
  }

  if (!doLock) {
    fprintf(stderr, "Disabling spinlock\n");
  }

  if (numRounds % 2 != 0) {
    fprintf(stderr, "'rounds' must be an even number\n");
    exit(-1);
  }

  std::vector<f64> delay(numThreads);
  std::vector<std::thread> threads;
  const s64 EXP = 12345;
  volatile s64 balance = EXP;
  for (u64 t = 0; t < numThreads; t++) {
    threads.emplace_back(bankTeller, t, &balance, numRounds, &delay[t]);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  go = true;
  for (auto& t : threads) {
    t.join();
  }
  printf("account balance is %li, expected %li\n", balance, EXP);
  if (balance != EXP) {
    fprintf(stderr, "ERROR!!!!!!!\n");
  }

  f64 maxDelay = *std::max_element(delay.cbegin(), delay.cend());
  printf("max delay is %f\n", maxDelay);
  printf("thread lock rate: %f\n", numRounds / maxDelay);
  printf("total lock rate: %f\n", (numRounds * numThreads) / maxDelay);

  return balance == EXP;
}
