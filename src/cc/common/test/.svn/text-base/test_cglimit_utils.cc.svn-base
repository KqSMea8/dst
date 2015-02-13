/***************************************************************************
 *
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

#include <common/cglimit_utils.h>
#include <common/logger.h>
#include <gtest/gtest.h>
#include <sys/wait.h>

using namespace dstream;

int main(int argc, char** argv)
{
    ::dstream::logger::initialize("test_cglimit_utils");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/**
 * @brief
**/
class test_CGLimitUtils_suite : public ::testing::Test {
protected:
    test_CGLimitUtils_suite() {};
    virtual ~test_CGLimitUtils_suite() {};
    virtual void SetUp() {
        if (check_cglimit()) {
            int pid = fork();
            if (pid == 0) {
                const char* argv[] = {
                    "/bin/cglimit",
                    "-C 50",
                    "-M 100M",
                    "sh -c 'top -b -n 20 &>/dev/null'",
                    NULL
                };
                execv(argv[0], (char * const*)argv);
                exit(0);
            } else if (pid > 0) {
                printf("child pid %d\n", pid);
                pid_ = pid;
                sleep(2); // wait child start
                m_cgutils = new CGLimitUtils(pid);
            } else {
                printf("fork error");
                return;
            }
        }
    };
    virtual void TearDown() {
        if (check_cglimit()) {
            kill(pid_, SIGKILL);
            waitpid(-1, NULL, 0);
            delete m_cgutils;
        }
    };

    bool check_cglimit()
    {
        // check bin
        if (access("/bin/cglimit", X_OK) != 0) {
            return false;
        }

        // check mem sub-system
        if (access("/cgroup/memory", R_OK | X_OK) != 0) {
            return false;
        }

        // check cpu sub-ssystem
        if (access("/cgroup/cpuacct", R_OK | X_OK) != 0) {
            return false;
        }
        return true;
    }

    int pid_;
    CGLimitUtils *m_cgutils;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_CGLimitUtils_suite, test_get_cur_cpu)
{
    if (check_cglimit()) {
        CpuStat ustat = m_cgutils->GetCurCpuUsage();
        ASSERT_TRUE(ustat.system != 0);
        ASSERT_TRUE(ustat.group != 0);
        printf("system: %lu, group: %lu\n", ustat.system, ustat.group);

        sleep(2);
        CpuStat ustat2 = m_cgutils->GetCurCpuUsage();
        ASSERT_TRUE(ustat2.system != 0);
        ASSERT_TRUE(ustat2.group != 0);
        printf("system: %lu, group: %lu\n", ustat2.system, ustat2.group);
        printf("cpu usage: %lf%%\n", (100 * m_cgutils->GetCpuUsage(ustat, ustat2)));
    }
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_CGLimitUtils_suite, test_get_cur_mem)
{
    if (check_cglimit()) {
        MemStat mstat = m_cgutils->GetMemUsage();
        ASSERT_TRUE(mstat.group);
        printf("mem usage: %luK\n", mstat.group / 1024);
    }
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
