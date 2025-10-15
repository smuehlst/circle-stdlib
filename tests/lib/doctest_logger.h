#ifndef DOCTEST_LOGGER_H
#define DOCTEST_LOGGER_H

#include "doctest/doctest.h"

class CLogger;

struct DoctestLogger : doctest::IReporter {
    const doctest::ContextOptions& opt;

    // constructor has to accept the ContextOptions by ref as a single argument
    DoctestLogger(const doctest::ContextOptions& in);

    void report_query(const doctest::QueryData& in) override;

    void test_run_start() override;

    void test_run_end(const doctest::TestRunStats& in) override;

    void test_case_start(const doctest::TestCaseData& in) override;

    // called when a test case is reentered because of unfinished subcases
    void test_case_reenter(const doctest::TestCaseData& in) override;

    void test_case_end(const doctest::CurrentTestCaseStats& in) override;

    void test_case_exception(const doctest::TestCaseException& in) override;

    void subcase_start(const doctest::SubcaseSignature& in) override;

    void subcase_end() override;

    void log_assert(const doctest::AssertData& in) override;

    void log_message(const doctest::MessageData& in ) override;

    void test_case_skipped(const doctest::TestCaseData& in) override;

    static void set_circle_logger(CLogger* logger);
 };

#endif