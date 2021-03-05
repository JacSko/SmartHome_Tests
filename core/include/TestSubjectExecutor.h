#ifndef TEST_SUBJECT_EXECUTOR_H_
#define TEST_SUBJECT_EXECUTOR_H_

/* ============================= */
/**
 * @file TestCore.h
 *
 * @brief TODO: update comment
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
