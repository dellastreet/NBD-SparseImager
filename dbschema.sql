create database imager;

use imager;

CREATE TABLE systems (
	ipadress varchar (20) PRIMARY KEY,
	currentstate enum ('dead', 'active', 'blockconnected', 'mounted'),
	requestedstate enum ('dead', 'active', 'blockconnected', 'mounted'),
	seenat DATETIME
);

