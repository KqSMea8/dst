#########################################
##   liyuanjian@baidu.com 2013-8-8     ##
#########################################
import sys

#index dict for each index:module:log_format
index_dict={
    "trace_pe_create_event":{
        "pm"    :["Call create pe \[$pe_id\] on pn"],
        "pn"    :["create PE ($pe_id) success"],
        "pe"    :["conn to outflow\[$pe_id\]@pn"]
    },
    "trace_app_submit_event":{
        "pm"    :["ask for app id \[$app_id\]","Call create pe \[$pe_id\] on pn"]
    },
    "trace_pe_restart_event":{
        "pn"    :["get pe:\[$pe_id\] exit signal","create PE ($pe_id) success"],
        "pm"    :["report fail pe \[$pe_id\]","Call create pe \[$pe_id\] on pn"],
        "pe"    :["conn to outflow\[$pe_id\]@pn"]
    }
    
}
#table_dict for each table_name:module:row_name
table_dict={
    "trace_pe_create_event":{
        "pm"    :["pm_start_create"],
        "pn"    :["pn_complete_create"],
        "pe"    :["pe_recv_tuples"]
    },
    "trace_app_submit_event":{
        "pm"    :["get_appid","call_pn_create"]
    },
    "trace_pe_restart_event":{
        "pn"    :["recv_exit_sig","pn_complete_create"],
        "pm"    :["pm_recv_report","pm_start_create"],
        "pe"    :["pe_recv_tuples"]
    }
}
dict_type=sys.argv[1]
item=sys.argv[2];
module=sys.argv[3];
index=sys.argv[4];
if dict_type == "target":
    if index == "num":
        if index_dict[item].has_key(module):
            print len(index_dict[item][module]);
        else:
            print 0;
    else:
        print index_dict[item][module][int(index)];
else:
    if index == "num":
        if index_dict[item].has_key(module):
            print len(table_dict[item][module]);
        else:
            print 0;
    else:
        print table_dict[item][module][int(index)];
