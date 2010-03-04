create database imager;

use imager;

CREATE TABLE systems (
	name varchar (40) PRIMARY KEY ,
	ipadress varchar (20),
	currentstate enum ('dead', 'active', 'blockconnected', 'mounted'),
	requestedstate enum ('dead', 'active', 'blockconnected', 'mounted'),
	seenat DATETIME 
);

