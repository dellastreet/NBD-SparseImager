create database imager;

use imager;

CREATE TABLE systems (
	name varchar (40) PRIMARY KEY ,
	ipadress varchar (20),
	currentstate enum ('dead', 'active', 'blockconnected', 'mounted'),
	requestedstate enum ('dead', 'active', 'blockconnected', 'mounted'),
	seenat DATETIME 
);

CREATE TABLE defaulttasks (
        id int PRIMARY KEY ,
        depends_on int,
	priority int, 
	material enum ('disk', 'partition', 'filesystem'),
        command varchar (256),
	defaultrun bool
);

-- priority: lower is better

INSERT into defaulttasks values (1,0,1,partition, "fls -r $PART",true);
INSERT into defaulttasks values (2,0,1,filesystem, "find $ROOT -iname \*.log -exec cat '{}' \;",true);
INSERT into defaulttasks values (1,0,1,partition, "find $ROOT -iname \*.evt -exec cat '{}' \;",true);

CREATE TABLE tasks (
        id int PRIMARY KEY ,
	systemname varchar (40),
        defaulttask_id int,
	state enum { 'idle', 'waiting', 'running' , 'finished'),
	finished DATETIME
);


