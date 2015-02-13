CREATE DATABASE IF NOT EXISTS `dstream`;
use `dstream`;

DROP TABLE if EXISTS `counter`;
CREATE TABLE `counter` (
    `cluster_name` VARCHAR(64) DEFAULT '',
    `machine_name` VARCHAR(64) DEFAULT '',
    `c_time`  TIMESTAMP DEFAULT NOW(),
    `c_name` VARCHAR(64) DEFAULT '',
    `c_attr1` BIGINT UNSIGNED DEFAULT 0,
    `c_attr2` BIGINT UNSIGNED DEFAULT 0,
    `c_attr3` BIGINT UNSIGNED DEFAULT 0,
    `c_type` TINYINT DEFAULT 0,
    `c_value` BIGINT DEFAULT 0,
    PRIMARY KEY (c_time, c_name, machine_name, cluster_name, c_attr1)
) ENGINE=Innodb DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS QWatcher;
DROP VIEW IF EXISTS QWatcher;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `QWatcher` AS select unix_timestamp(`counter`.`c_time`) AS `time`,`counter`.`c_attr2` AS `APPID`,`counter`.`machine_name` AS `ip`,if((`counter`.`c_name` = 'PnRecvQueue.TupleNum'),'RQUEUE','SQUEUE') AS `type`,`counter`.`c_attr1` AS `PEID`,`counter`.`c_value` AS `queue_size` from `counter` where ((`counter`.`c_name` = 'PnRecvQueue.TupleNum') or (`counter`.`c_name` = 'PnSendQueue.TupleNum')) AND (`counter`.`c_time` >= (now() - interval 2 minute)) order by `counter`.`c_time` desc;

DROP TABLE IF EXISTS QPS;
DROP VIEW IF EXISTS QPS;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `QPS` AS select unix_timestamp(`counter`.`c_time`) AS `time`,(`counter`.`c_attr2`) AS `APPID`,`counter`.`machine_name` AS `ip`, CASE `counter`.`c_name`  WHEN 'pe.SendTuples' then 'SEND' WHEN 'pe.RecvTuples' then 'RECV' END AS `type`,`counter`.`c_attr1` AS `PEID`,`counter`.`c_value` AS `value` from `counter` where (`counter`.`c_name` = 'pe.SendTuples' OR `counter`.`c_name` = 'pe.RecvTuples') and (`counter`.`c_time` >= (now() - interval 2 minute)) order by `counter`.`c_time` desc;

DROP VIEW IF EXISTS MYSQL_USAGE;
CREATE VIEW MYSQL_USAGE as select concat(truncate(sum(data_length)/1024/1024,2),'MB') as data_size,concat(truncate(sum(data_free)/1024/1024,2),'MB') as data_free, concat(truncate(sum(index_length)/1024/1024,2),'MB') as index_size,concat(truncate(sum(data_length+index_length+data_free)/1024/1024,2),'MB') as total from information_schema.tables where TABLE_SCHEMA = 'dstream';   

SET GLOBAL event_scheduler = ON;
DROP EVENT IF EXISTS `Delete_Old_Data_EveryDay` ;
delimiter //
CREATE EVENT `Delete_Old_Data_EveryDay` ON SCHEDULE EVERY 1 DAY STARTS '2012-05-15 00:00:00' ON COMPLETION NOT PRESERVE ENABLE 
DO 
BEGIN
	PURGE MASTER LOGS BEFORE DATE_SUB( NOW(), INTERVAL 3 DAY);
END
//
delimiter ;

SET GLOBAL event_scheduler = ON;
DROP EVENT IF EXISTS `Delete_Old_Counter_EveryMinute` ;
delimiter //
CREATE EVENT `Delete_Old_Counter_EveryMinute` ON SCHEDULE EVERY 1 MINUTE STARTS '2012-05-15 00:00:00' ON COMPLETION NOT PRESERVE ENABLE 
DO 
BEGIN
    delete from counter where c_time < date_add(current_timestamp,interval -3 day);
END
//
delimiter ;

DROP VIEW IF EXISTS ERRORS;
CREATE TABLE IF NOT EXISTS `ERRORS` (
  `id` int(20) NOT NULL auto_increment,
  `time` timestamp NOT NULL ,
  `logline` varchar(400) collate utf8_bin NOT NULL,
  `senderip` varchar(30) collate utf8_bin NOT NULL,
  PRIMARY KEY  (`id`)
)  DEFAULT CHARSET=utf8 COLLATE=utf8_bin AUTO_INCREMENT=1 ;




DROP VIEW IF EXISTS FetchLogDropTuple;
CREATE TABLE IF NOT EXISTS `FetchLogDropTuple`(
	`time` TIMESTAMP NOT NULL ,
	`senderip` varchar(30) not null,
	`peid` bigint(25) not null,
	`dropnum` bigint(25) 
);



DROP VIEW IF EXISTS NoDownstreamDropTuple;
CREATE TABLE IF NOT EXISTS NoDownstreamDropTuple(
	`time` timestamp not null,
	`senderip` varchar(30) not null,
	`tag` varchar(40) ,
	`hashcode` bigint(25)
)DEFAULT CHARSET = utf8 COLLATE = utf8_bin;


DROP VIEW IF EXISTS SendQueueDropTuple;
CREATE TABLE IF NOT EXISTS SendQueueDropTuple(
	`time` timestamp not null,
	`senderip` varchar(30) not null,
	`dropnum` bigint(25)
);


DROP VIEW IF EXISTS CreatePEHisotry;
CREATE TABLE IF NOT EXISTS CreatePEHistory(
	`time` timestamp not null,
	`senderip` varchar(30) not null,
	`peid` bigint(25) not null,
	`pnid` varchar(100) not null,
	`result` varchar(20) not null

);

drop view if exists mailbox;
create table if not exists mailbox(
	`type` varchar(40),
	`cnt` int(20),
	primary key (`type`)
);

SET GLOBAL event_scheduler = ON;
DROP EVENT IF EXISTS `LogMonitorDeleteOldDatas` ;
delimiter //
CREATE EVENT `LogMonitorDeleteOldDatas` ON SCHEDULE EVERY 1 DAY STARTS '2012-05-15 00:00:00' ON COMPLETION NOT PRESERVE ENABLE 
DO 
BEGIN
	delete from ERRORS where time < date_add(current_timestamp,interval -3 day);
	delete from CreatePEHistory where time < date_add(current_timestamp,interval -30 day);
	delete from FetchLogDropTuple where time < date_add(current_timestamp,interval -3 day);
	delete from SendQueueDropTuple where time < date_add(current_timestamp,interval -3 day);
	delete from NoDownstreamDropTuple where time < date_add(current_timestamp,interval -3 day);
END
//
delimiter ;
