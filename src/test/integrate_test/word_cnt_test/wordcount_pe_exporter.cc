#include <string>
//#include <com_log.h>
#include "common/logger.h"
#include "processelement/task.h"
#include <map>
#include <iostream>
#include <fstream>

#include "../test/integrate_test/word_cnt_test/wordcount.pb.h"
using namespace std;
using namespace dstream;
using namespace dstream::processelement;

std::string executive_name;

class WordCount : public Task {
public:
    // types we may encounter.
    static const char* in_tag;
    static const char* out_tag;
    typedef ProtoTuple< WordCountRequest > InTuple;
    typedef DefaultTupleTypeInfo< InTuple > InTupleInfo;
    typedef ProtoTuple< WordCountResponse> OutTuple;
    typedef DefaultTupleTypeInfo< OutTuple > OutTupleInfo;

    map<string, int> wordcounts;
    FILE* fp;
    static const int kCount = 32;
    int count;
    WordCount() {
        registerTupleTypeInfo(WordCount::in_tag, new InTupleInfo());
        registerTupleTypeInfo(WordCount::out_tag, new OutTupleInfo());
        fp = fopen("exporter.txt", "a");
        assert(fp);
        count = kCount;
    }
    ~WordCount() {
        fclose(fp);
    }
    void emitResult() {
        fprintf(fp, "%s", "--------------------\n");
        for (map<string, int>::const_iterator it = wordcounts.begin();
             it != wordcounts.end(); ++it) {
            fprintf(stderr, "%10s %d\n", it->first.c_str(), it->second);
            fprintf(fp, "%10s %d\n", it->first.c_str(), it->second);
        }
        fflush(fp);
    }
    virtual void onTuples(const Tuples& tuples) {
        //DSTREAM_WARN("\n[TEST]onTuples");
        DSTREAM_INFO(" ====> %s, onTuples", executive_name.c_str());

        //count the number of words
        bool EOF_END = false;
        for (size_t i = 0; i < tuples.size(); i++) {
            const OutTuple* in_tuple = dynamic_cast<const OutTuple*>(tuples[i]);
            wordcounts[in_tuple->word()] += in_tuple->counts();
            if (in_tuple->word() == "EOF") {
                EOF_END = true;
            }
        }
        if (EOF_END) {
            ofstream out_file("../word_cnt.txt");
            map<string, int>::iterator it;
            for (it = wordcounts.begin(); it != wordcounts.end(); it++) {
                out_file << it->first.c_str() << ":" << it->second << endl;
            }
        }
        count--;
        if (count == 0) {
            emitResult();
            count = kCount;
        }
    }
}; // class WordCount

const char* WordCount::in_tag = "in";
const char* WordCount::out_tag = "out";

int main(int /*argc*/, char** argv) {
    executive_name = argv[0];

    logger::initialize("pe_task");
    setenv(Task::kEnvKeyLogLevel, "WARNING", 1);
    setenv(Task::kEnvKeyProcessElementName, "wordcount_exporter", 1);

    // make a task instance and run.
    WordCount task;
    // get env of loopback & importer

    bool loopback = false;
    if (getenv("loopback") != 0) {
        loopback = true;
    }
    task.run(loopback);
}
