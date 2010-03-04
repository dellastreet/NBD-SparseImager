# server_version.py - retrieve and display database server version

import MySQLdb

conn = MySQLdb.connect (host = "localhost",
                        user = "root",
                        passwd = "",
                        db = "imager")
cursor = conn.cursor ()
cursor.execute ("SELECT VERSION()")
row = cursor.fetchone ()
print "server version:", row[0]
cursor.close ()
conn.close ()

