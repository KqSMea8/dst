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

std::string excutive_name;

class WordCount : public Task
{
public:
    // types we may encounter.
    static const char* in_tag;
    static const char* out_tag;
    typedef ProtoTuple< WordCountRequest > InTuple;
    typedef DefaultTupleTypeInfo< InTuple > InTupleInfo;
    typedef ProtoTuple< WordCountResponse> OutTuple;
    typedef DefaultTupleTypeInfo< OutTuple > OutTupleInfo;

    map<string, int> wordcounts;

    WordCount()
    {
        registerTupleTypeInfo(WordCount::in_tag, new InTupleInfo());
        registerTupleTypeInfo(WordCount::out_tag, new OutTupleInfo());
    }

    virtual void onTuples(const Tuples& tuples)
    {
        DSTREAM_INFO(" ====> %s, [TEST]onTuples", excutive_name.c_str());

        //count the number of words
        string word;
        for (size_t i = 0; i < tuples.size(); i++)
        {
            const InTuple* in_tuple = dynamic_cast<const InTuple*>(tuples[i]);
            wordcounts[in_tuple->word()] += in_tuple->counts();
            if (in_tuple->word() == "EOF") {
                sleep(2);
            }
        }
        //emit data to out_tuples
        map<string, int>::iterator it;
        for (it = wordcounts.begin(); it != wordcounts.end(); it++)
        {
            /**out_tag*/
            OutTuple* out_tuple = dynamic_cast<OutTuple*>(emit(out_tag));
            out_tuple->set_word(it->first);
            out_tuple->set_counts(it->second);
            out_tuple->set_hash_code(it->first.length());
        }
    }
}; // class WordCount

const char* WordCount::in_tag = "in";
const char* WordCount::out_tag = "out";

int main(int /*argc*/, char** argv)
{
    excutive_name = argv[0];

    logger::initialize("pe_task");
    setenv(Task::kEnvKeyLogLevel, "WARNING", 1);
    setenv(Task::kEnvKeyProcessElementName, "wordcount", 1);

    // make a task instance and run.
    WordCount task;
    // get env of loopback & importer

    bool loopback = false;
    if (getenv("loopback") != 0)
    {
        loopback = true;
    }
    task.run(loopback);
}
