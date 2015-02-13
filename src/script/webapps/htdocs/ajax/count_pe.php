<?php
/***************************************************************************
 * 
 * Copyright (c) 2012 baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file count_pe.php
 * @author Yuncong Zhang(zhangyuncong@baidu.com)
 * @date 2012/07/27 11:28:01
 * @version 1.0 
 * @brief 
 *  
 **/
set_time_limit(10);
chdir("..");
$no_db=true;
include ("include/common.php");
echo count(get_pe_on_pn_by_pn_debugger("$_GET[addr]"));

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
?>
