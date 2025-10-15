#include "doctest_logger.h"
#include "doctest/doctest.h"
#include <circle/logger.h>
#include <string>
#include <sstream>
#include <cmath>
#include <iomanip>

namespace
{
    CLogger *circle_logger = nullptr;
}

DoctestLogger::DoctestLogger(const doctest::ContextOptions &in)
    : opt(in)
{
}

void DoctestLogger::report_query(const doctest::QueryData & /*in*/) {}

void DoctestLogger::test_run_start() {}

void DoctestLogger::test_run_end(const doctest::TestRunStats &in)
{
    if (!circle_logger)
    {
        return;
    }

    const TLogSeverity severity = (in.numTestCasesFailed > 0 || in.numAssertsFailed > 0) ? LogError : LogNotice;

    circle_logger->Write("doctest", severity, "===============================================================================");

    std::ostringstream s;
    s << std::dec;

    auto const totwidth = int(std::ceil(log10(static_cast<double>(std::max(in.numTestCasesPassingFilters, static_cast<unsigned>(in.numAsserts))) + 1)));
    auto const passwidth = int(std::ceil(log10(static_cast<double>(std::max(in.numTestCasesPassingFilters - in.numTestCasesFailed, static_cast<unsigned>(in.numAsserts - in.numAssertsFailed))) + 1)));
    auto const failwidth = int(std::ceil(log10(static_cast<double>(std::max(in.numTestCasesFailed, static_cast<unsigned>(in.numAssertsFailed))) + 1)));
    s << "test cases: " << std::setw(totwidth)
      << in.numTestCasesPassingFilters << " | "
      << std::setw(passwidth) << in.numTestCasesPassingFilters - in.numTestCasesFailed << " passed"
      << " | "
      << std::setw(failwidth) << in.numTestCasesFailed << " failed" << " |";
    if (opt.no_skipped_summary == false)
    {
        const int numSkipped = in.numTestCases - in.numTestCasesPassingFilters;
        s << " " << numSkipped
          << " skipped";
    }
    circle_logger->Write("doctest", severity, "%s", s.str().c_str());
    s.str("");

    s << "assertions: " << std::setw(totwidth)
      << in.numAsserts << " | "
      << std::setw(passwidth) << (in.numAsserts - in.numAssertsFailed) << " passed"
      << " | " << std::setw(failwidth)
      << in.numAssertsFailed << " failed" << " |";
    circle_logger->Write("doctest", severity, "%s", s.str().c_str());
    s.str("");

    circle_logger->Write("doctest", severity, (in.numTestCasesFailed > 0) ? "FAILURE!" : "SUCCESS!");
}

void DoctestLogger::test_case_start(const doctest::TestCaseData &in)
{
    if (!circle_logger)
    {
        return;
    }

    circle_logger->Write("doctest", LogNotice,
                         "TEST START: %s (%s:%u)", in.m_name,
                         in.m_file.c_str(), in.m_line);
}

void DoctestLogger::test_case_reenter(const doctest::TestCaseData & /*in*/) {}

void DoctestLogger::test_case_end(const doctest::CurrentTestCaseStats & /*in*/) {}

void DoctestLogger::test_case_exception(const doctest::TestCaseException & /*in*/) {}

void DoctestLogger::subcase_start(const doctest::SubcaseSignature & /*in*/) {}

void DoctestLogger::subcase_end() {}

void DoctestLogger::log_assert(const doctest::AssertData &in)
{
    if ((!in.m_failed && !opt.success) || !circle_logger)
    {
        return;
    }

    std::string const msg = std::string("ERROR: ") + in.m_expr + " at " +
                            in.m_file + ":" + std::to_string(in.m_line);

    circle_logger->Write("doctest", LogError, "%s", msg.c_str());
}

void DoctestLogger::log_message(const doctest::MessageData &in)
{
    if (!circle_logger)
    {
        return;
    }

    std::string const msg = std::string("INFO: ") + in.m_string.c_str() + " at " +
                            in.m_file + ":" + std::to_string(in.m_line);

    circle_logger->Write("doctest", LogNotice, "%s", msg.c_str());
}

void DoctestLogger::test_case_skipped(const doctest::TestCaseData & /*in*/) {}

void DoctestLogger::set_circle_logger(CLogger *logger)
{
    circle_logger = logger;
}