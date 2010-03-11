# server_version.py - retrieve and display database server version

import MySQLdb

#CREATE TABLE systems (
#	ipadress varchar (20),
#	currentstate enum ('dead', 'active', 'blockconnected', 'mounted'),
#	requestedstate enum ('dead', 'active', 'blockconnected', 'mounted'),
#	seenat DATETIME
#);


class ImagerDb:
	def __init__(self):
		self.conn = MySQLdb.connect (host = "localhost",
                        user = "root",
                        passwd = "",
                        db = "imager")
		self.cursor = self.conn.cursor ()

	def __del__(self):
		self.cursor.close ()
		self.conn.close ()
		print "deleted"

	def listsystems(self):
		print "execute"
		self.cursor.execute ("SELECT *,(NOW()-seenat) as age from systems")
		row = self.cursor.fetchone ()
		while (not row is None):
			print row
			row = self.cursor.fetchone ()
	def seensystem(self,name,ip):
		self.cursor.execute ("INSERT INTO systems (name, ipadress,currentstate,requestedstate,seenat) VALUES (\"" + name + "\",\"" + ip + "\",'active','active',NOW()) ON DUPLICATE KEY UPDATE requestedstate=if(currentstate='dead' and requestedstate='dead','active',requestedstate),seenat=NOW()")
 
	def requeststate(self,name,state):
		print "update/insert"
		self.cursor.execute ("UPDATE systems SET requestedstate='"+state+"' WHERE name=\""+name+"\";")

	def setcurrentstate(self,name,state):
		print "update/insert"
		self.cursor.execute ("UPDATE systems SET currentstate='"+state+"' WHERE name=\""+name+"\";")

	def expirestate(self,sec):
		print "update/insert"
		self.cursor.execute ("UPDATE systems SET requestedstate='dead' WHERE (NOW()-seenat)>"+str(sec)+";")

	def getchange_request(self):
		self.cursor.execute ("SELECT name,ipadress,currentstate,requestedstate FROM systems where currentstate <> requestedstate LIMIT 1");
		row = self.cursor.fetchone ()
		return row

if __name__=='__main__':
	X=ImagerDb()
	X.listsystems()
	X.expirestate(2)
	X.listsystems()
	X.seensystem("1234_1.2.3.4","1.2.3.4")
	X.listsystems()
	X.setcurrentstate("1234_1.2.3.4","dead")
	X.listsystems()
	X.seensystem("1234_1.2.3.4","1.2.3.4")
	X.listsystems()
	del X


