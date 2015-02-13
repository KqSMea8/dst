#include "gtest/gtest.h"
#include "processnode/process_builder.h"
#include "processnode/process.h"
#include "common/logger.h"
#include "common/utils.h"

using namespace std;
using namespace dstream;
using namespace dstream::processnode;

class TestProcessBuilder : public ::testing::Test
{
protected:
    static ProcessBuilder* pb;
    void SetUp() {
        pb = new ProcessBuilder();
        DSTREAM_WARN("%s", "PB create");
    }
    void TearDown() {
        delete pb;
        DSTREAM_WARN("%s", "PB ~");
    }
};
ProcessBuilder* TestProcessBuilder::pb = NULL;
const int SIZE = 1000;
char buf[SIZE];
class OnGrepStartCallback: public OnStartCallback {
public:
    OnGrepStartCallback(std::string input, std::string expect): input_str_(input), expect_(expect) {
    }
    void Call(ProcessManager::ProcessPtr process) {
        FILE* in    =   fdopen(process->GetInputStream(), "w");
        FILE* out   =   fdopen(process->GetOutputStream(), "r");
        FILE* error =   fdopen(process->GetErrorStream(), "r");
        ASSERT_NE(static_cast<FILE*>(NULL), in)   << "get input stream failed..";
        ASSERT_NE(static_cast<FILE*>(NULL), out)  << "get output stream failed..";
        ASSERT_NE(static_cast<FILE*>(NULL), error) << "get error stream failed..";

        fprintf(in, "%s", input_str_.c_str());
        fclose(in);

        fgets(buf, SIZE, out);
        ASSERT_EQ(expect_, buf);
        ASSERT_EQ((char*)NULL, fgets(buf, SIZE, out));
        ASSERT_EQ((char*)NULL, fgets(buf, SIZE, error));
        fclose(out);
        fclose(error);
    }
protected:
    std::string input_str_;
    std::string expect_;
};

class CheckNoOutputNoErrorWithInputStartCallback: public OnStartCallback {
public:
    CheckNoOutputNoErrorWithInputStartCallback(std::string input_str): input_str_(input_str) {
    }
    void Call(ProcessManager::ProcessPtr process) {
        FILE* in    =  fdopen( process->GetInputStream(), "w");
        FILE* out   =   fdopen(process->GetOutputStream(), "r");
        FILE* error =   fdopen(process->GetErrorStream(), "r");
        ASSERT_NE(static_cast<FILE*>(NULL), in)   << "get input stream failed..";
        ASSERT_NE(static_cast<FILE*>(NULL), out)  << "get output stream failed..";
        ASSERT_NE(static_cast<FILE*>(NULL), error) << "get error stream failed..";

        fprintf(in, "%s", input_str_.c_str());
        fclose(in);

        ASSERT_EQ((char*)NULL, fgets(buf, SIZE, out));
        ASSERT_EQ((char*)NULL, fgets(buf, SIZE, error));

        fclose(out);
        fclose(error);
    }
protected:
    std::string input_str_;
};

class OnGrepExitCallback: public OnExitCallback {
public:
    void Call(int /*pid*/, int /*status*/, int /*exit_code*/, bool exited) {
        ASSERT_TRUE(exited);
    }
};

TEST_F(TestProcessBuilder, test_process_builder_redirect_all)
{
    pb->redirect_input_stream(true);
    pb->redirect_output_stream(true);
    pb->redirect_error_stream(true);
    pb->Start("grep abcd", new OnGrepExitCallback(), new OnGrepStartCallback("abcdefg", "abcdefg\n"));
    pb->Start("grep abcd", new OnGrepExitCallback(), new OnGrepStartCallback("ssss\nabcdaa\nsssss",
              "abcdaa\n"));
    pb->Start("grep abcd", new OnGrepExitCallback(),
              new CheckNoOutputNoErrorWithInputStartCallback("sssss"));
    usleep(100000);
}

class CheckIfNormalExitedExitCallback : public OnExitCallback {
public:
    CheckIfNormalExitedExitCallback(bool ex): expect(ex) {}
    void Call(int /*pid*/, int status, int /*exit_code*/, bool normal_exited) {
        bool normal = WIFEXITED(status);
        ASSERT_EQ(expect, normal_exited);
        ASSERT_EQ(expect, normal);
    }
private:
    bool expect;
};

TEST_F(TestProcessBuilder, test_process_builder_exceptions)
{
    pb->Start("ls", new CheckIfNormalExitedExitCallback(true));

    ProcessManager::WkProcessPtr wk_child = pb->Start("sleep 10",
                                                      new CheckIfNormalExitedExitCallback(false));
    ProcessManager::ProcessPtr child = wk_child.lock();
    ASSERT_TRUE(child);
    child->Destroy();

    child.reset();
    wk_child.reset();
    wk_child = pb->Start("sleep 10", new CheckIfNormalExitedExitCallback(false));
    child = wk_child.lock();
    ASSERT_TRUE(child);
    int pid = child->pid();
    kill(pid, 11);

    usleep(100000);

    std::string core_file = "./core." + NumberToString(pid);
    if (access(core_file.c_str(), F_OK) != 0) {
        // process coresave
        std::string coresave_file = "/home/coresave/core.*." + NumberToString(pid) + ".*";
        system(("cp " + coresave_file + " " + core_file).c_str());
    }
    if (access(core_file.c_str(), F_OK) != 0) {
        // process coresave
        std::string coresave_file = "/home/coresave/core.*." + NumberToString(pid);
        system(("cp " + coresave_file + " " + core_file).c_str());
    }
    ASSERT_EQ(0, access(core_file.c_str(),
                        F_OK)) << "core file:" << core_file << " not found" << std::endl;
    system(("rm " + core_file).c_str());
}

class CheckRetExitCallback : public OnExitCallback {
public:
    CheckRetExitCallback(int ret): return_val_(ret) {}
    void Call(int , int, int exit_code, bool normal) {
        ASSERT_TRUE(normal);
        ASSERT_EQ(return_val_, exit_code);
    }
private:
    int return_val_;
};

TEST_F(TestProcessBuilder, check_ret)
{
    pb->Start("cat /proc/version", new CheckRetExitCallback(0));
    pb->Start("cat /sss", new CheckRetExitCallback(1));
    pb->Start("bash -c ssss", new CheckRetExitCallback(127));
    usleep(100000);
}
class CheckSignaledExpected : public OnExitCallback {
public:
    CheckSignaledExpected(int expect): expect_(expect) {}
    void Call(int, int status, int /*exit_code*/, bool normal) {
        ASSERT_FALSE(normal);
        bool signaled = WIFSIGNALED(status);
        ASSERT_TRUE(signaled);
        int term_sig = WTERMSIG(status);
        ASSERT_EQ(expect_, term_sig);
    }
private:
    int expect_;
};



TEST_F(TestProcessBuilder, test_env)
{
    map<std::string, std::string>& mp = pb->Environment();
    mp["abc"] = "myresult";
    mp["sss"] = "./";
    pb->redirect_output_stream(true);
    pb->redirect_input_stream(true);
    pb->redirect_error_stream(true);
    pb->Start("echo $abc", NULL, new OnGrepStartCallback("", "myresult\n"));
    pb->Start("echo $sss", NULL, new OnGrepStartCallback("", "./\n"));
    //  char s[1000];
    //  strcpy(s,getenv("LD_LIBRARY_PATH"));
    //  pb->Start("echo $LD_LIBRARY_PATH",NULL,new OnGrepStartCallback("",strcat(s,"\n"))); // LD_LIBRARY_PATH will be the same as his father
    usleep(100000);
}

int main(int argc, char** argv) {
    ::dstream::logger::initialize("TestProcessBuilder");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
