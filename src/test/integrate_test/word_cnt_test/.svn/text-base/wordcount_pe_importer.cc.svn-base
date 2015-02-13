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

    // importer logic
    virtual void onSpout(ReadableByteArray* bytes)
    {
        DSTREAM_INFO("  ====> %s, onSpout", excutive_name.c_str());
        ByteSize size = 0;
        string line(reinterpret_cast<const char*>(bytes->Data(&size)), size);
        DSTREAM_INFO("  ====> %s, TESTdata : %s", excutive_name.c_str(), line.c_str());
        string separators(" \t.:\v\r\n\f");
        string word;
        int wordlen = 0, len = line.length();
        string::size_type startpos = 0, endpos = 0;
        endpos = line.find_first_of(separators, 0);

        if (endpos == string::npos) { //only one word/line
            wordlen = len - startpos;
        }
        else {
            wordlen = endpos -  startpos;
        }

        word.assign(line.begin() + startpos, line.begin() + startpos + wordlen );
        InTuple* in_tuple  = dynamic_cast<InTuple*>(emit(in_tag));
        in_tuple->set_word(word);
        in_tuple->set_counts(1);
        in_tuple->set_hash_code(word.length());
        word.clear();

        while ((startpos = line.find_first_not_of(separators, endpos)) != string::npos )
        {
            endpos = line.find_first_of(separators, startpos);
            if (endpos != startpos ) //wordlen >= 1, record word and count
            {
                if (endpos != string::npos) {
                    wordlen = endpos - startpos;
                }
                else {
                    wordlen = len - startpos;
                }
                word.assign(line.begin() + startpos, line.begin() + startpos + wordlen);
                in_tuple  = dynamic_cast<InTuple*>(emit(in_tag));
                in_tuple->set_word(word);
                in_tuple->set_counts(1);
                DSTREAM_DEBUG("insert word '%s'", word.c_str());
                in_tuple->set_hash_code(word.length());
                word.clear();
            }
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
    setenv(Task::kEnvKeyProcessElementName, "wordcount_importer", 1);

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
