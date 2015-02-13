#include "common/application_tools.h"

using namespace dstream;
using namespace dstream::application_tools;

int main()
{
    ::dstream::logger::initialize("app_id");
    Application app;
    ConstructFakeApp(&app, 3, 1);
    typedef std::list<ProcessorElement> PE_LIST;
    PE_LIST pe_list;
    ClusterConfig config;
    CreateAppPEInstance(app, config, 0, &pe_list);
    for (PE_LIST::iterator it = pe_list.begin(); it != pe_list.end(); ++it) {
        printf("%lu %lu\n", it->pe_id().id(), ConstructProcessorID(it->pe_id()).id());
    }
    ProcessorID pid;
    for (int i = 0; i < 3; ++i) {
        pid.set_id(i);
        printf("%lu\n", ConstructProcessorID(app.id(), pid).id());
    }
    return 0;
}
