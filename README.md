# Deterministic testing tool for multithreaded programs based on Google ThreadSanitizer
https://github.com/google/sanitizers/wiki/ThreadSanitizerCppManual
## This repository forked from LLVM Project
https://github.com/llvm/llvm-project

## Getting Started with the LLVM System

Taken from https://llvm.org/docs/GettingStarted.html.

### Overview

This tool allows you to set the deterministic execution of threads using a test script.
Determinism is that each thread is executed sequentially. 
In other words, you can use this tool to set sequential execution of threads. 
This can be used to prove that there are mutual locks in the program.
Control between threads can be passed on the following events:
1. Create thread (create_thread)
2. Complete thread (complete_thread)
3. Mutex lock (mutex_lock)

Tool can work with posix threads.

### How use

1. Compile program with ThreadSanitizer flags.
https://github.com/google/sanitizers/wiki/ThreadSanitizerCppManual

2. Describe the thread execution scenario.

Example:

Source code 
```
#include <pthread.h>

pthread_mutex_t mutex1;
pthread_mutex_t mutex2;

void *thread1(void *arg) {
   pthread_mutex_lock(&mutex1);
   pthread_mutex_lock(&mutex2);
   pthread_mutex_unlock(&mutex1);
   pthread_mutex_unlock(&mutex2);
}

void *thread2(void *arg) {
   pthread_mutex_lock(&mutex2);
   pthread_mutex_lock(&mutex1);
   pthread_mutex_unlock(&mutex2);
   pthread_mutex_unlock(&mutex1);
}

int main() {
  pthread_mutex_init(&mutex1, NULL);
  pthread_mutex_init(&mutex2, NULL);
  pthread_t t[2];
  pthread_create(&t[0], NULL, thread1, NULL);
  pthread_create(&t[1], NULL, thread2, NULL);
  pthread_join(t[0], NULL);
  pthread_join(t[1], NULL);
  
  return 0;
}
```
Test scenario
```
main:
    create_thread(thread1)
    create_thread(thread2)
thread1:
    mutex_lock
thread2:
    mutex_lock
```
Run with this flag
```
TSAN_OPTIONS="ts_path=pathToTestScenario" executionFile
```

