#include "Logger.h"
#include "TestSubjectExecutor.h"
#include <string.h>
#include <thread>
#include "SocketDriver.h"
#include "TestCore.h"

int main()
{

   TestCore tc;

   tc.runTest("../../logs/log.txt");

   std::this_thread::sleep_for(std::chrono::seconds(10));

   tc.stopTest();

   std::this_thread::sleep_for(std::chrono::seconds(5));

   tc.runTest("../../logs/log2.txt");

   std::this_thread::sleep_for(std::chrono::seconds(10));

   tc.stopTest();


}
