#include "common/hdfs_client.h"
#include "common/application_tools.h"

using namespace std;
using namespace dstream;
using namespace dstream::hdfs_client;
using namespace dstream::application_tools;

int main()
{
    logger::initialize("hpc_cmd");
    std::string cmd = "ps ux | awk '{if(NF>1)print $2,$3,$6}'";
    char* buffer = new char[kMaxReturnBufferLen];
    HdfsClient::HPCmdExec(cmd, 1, buffer);
    std::vector<std::string> lines;
    SplitStringEx(buffer, '\n', &lines);
    printf("lines %lu\n", lines.size());
    for (unsigned int i = 1; i < lines.size(); ++i) {
        printf("%s ", lines[i].c_str());
        std::vector<std::string> words;
        SplitStringEx(lines[i], ' ', &words);
        printf("%lu\n", words.size());
    }
    delete []buffer;
    return 0;
}
