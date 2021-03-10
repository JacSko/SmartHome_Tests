#ifndef TEST_SUBJECT_EXECUTOR_H_
#define TEST_SUBJECT_EXECUTOR_H_

/* ============================= */
/**
 * @file TestCore.h
 *
 * @brief
 *    Class is responsible for executing binary given by constructor parameter.
 *
 * @details
 *    Given process is executed in background.
 *    To start process, call start_test_subject() and save the PID.
 *    To stop execution, call stop_test_subject(_pid_).
 *
 * @details
 *
 *
 * @author Jacek Skowronek
 * @date 05/03/2021
 */
/* ============================= */

/* =============================
 *  Includes of common headers
 * =============================*/
#include <string>
/* =============================
 *  Includes of project headers
 * =============================*/
/* =============================
 *          Defines
 * =============================*/
/* =============================
 *       Data structures
 * =============================*/

class TestSubjectExecutor
{
public:
   TestSubjectExecutor(const std::string& process_path);
   pid_t start_test_subject();
   void stop_test_subject(pid_t pid);
private:
   std::string m_test_subject_path;
};


#endif
