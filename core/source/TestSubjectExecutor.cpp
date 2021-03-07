/* =============================
 *   Includes of project headers
 * =============================*/
#include "TestSubjectExecutor.h"
/* =============================
 *   Includes of common headers
 * =============================*/
#include <unistd.h>
#include <signal.h>
#include <thread>
#include <string.h>

TestSubjectExecutor::TestSubjectExecutor(const std::string& process_path):
m_test_subject_path(process_path)
{

}

pid_t TestSubjectExecutor::start_test_subject()
{
   pid_t pid = fork();
   if (pid == 0)
   {
      int res = execl(m_test_subject_path.c_str(), NULL);
      if (res < 0)
      {
         printf("returned %d, %s\n", res, strerror(errno));
      }
      exit(1);
   }
   return pid;
}

void TestSubjectExecutor::stop_test_subject(pid_t pid)
{
   kill(pid, SIGINT);
}
