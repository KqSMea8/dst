




##########################
# check_result assist functions
##########################
findcore_in_folder(){ # findcore dir
	corenum=0
	find $1 -name 'core.[1-9]*'| wc -l >tmp
	cat tmp
}

finderror_in_folder(){ # findcore dir
	error_log=0
	grep "\[ERROR\]" -r $1 | wc -l
	cat temp
}
##########################
# tools unit_test
##########################
# findcore_in_folder ./
#finderror_in_folder ../testenv


