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
	def seensystem(self,ip):
		print "update/insert"
		self.cursor.execute ("INSERT INTO systems (ipadress,currentstate,requestedstate,seenat) VALUES (\"" + ip + "\",'active','active',NOW()) ON DUPLICATE KEY UPDATE currentstate=if(currentstate='dead','active',currentstate),seenat=NOW()")
 
	def requeststate(self,ip,state):
		print "update/insert"
		self.cursor.execute ("UPDATE systems SET requestedstate='"+state+"' WHERE ipadress=\""+ip+"\";")

	def setcurrentstate(self,ip,state):
		print "update/insert"
		self.cursor.execute ("UPDATE systems SET currentstate='"+state+"' WHERE ipadress=\""+ip+"\";")

	def expirestate(self,sec):
		print "update/insert"
		self.cursor.execute ("UPDATE systems SET requestedstate='dead' WHERE (NOW()-seenat)>"+sec+";")

if __name__=='__main__':
	X=ImagerDb()
	X.listsystems()
	X.expirestate("2")
	X.listsystems()
	X.seensystem("AAA")
	X.listsystems()
	X.setcurrentstate("AAA","dead")
	X.listsystems()
	X.seensystem("AAA")
	X.listsystems()
	del X


