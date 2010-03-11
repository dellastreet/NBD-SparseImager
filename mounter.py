#!/usr/bin/python
import os
import sys
import string
import time
import shlex, subprocess

baseport=6000
baseexec_path="/usr/local/sparse-imager/bin/"

def attach_blockdevice(devnr, ipaddress,port,imagename):
        global baseexec_path,baseport
	cmd = [ baseexec_path+"xnbd-server" ,"--daemonize", "--proxy", "--readonly", "--pidfile", "/tmp/nbd"+str(devnr) +".pid" , ipaddress ,str(port),"--lport",str(baseport+devnr) , imagename + ".img" , imagename + ".bit"]
	print cmd
	p = subprocess.call(cmd)

	cmd = [ "/sbin/nbd-client" ,"127.0.0.1", str(baseport+devnr), "/dev/nbd"+str(devnr)]
	print cmd
	p = subprocess.call(cmd)

	
def dettach_blockdevice(devnr):
	cmd = [ "/sbin/nbd-client" ,"-d",  "/dev/nbd"+str(devnr)]
	print cmd
	p = subprocess.call(cmd)

        pidfile = open("/tmp/nbd"+str(devnr) +".pid", 'r')
	pidstr= pidfile.read()
	pidfile.close()
        os.kill(int(pidstr),15) #TERM
	os.unlink("/tmp/nbd"+str(devnr) +".pid")
	
#	cmd = [ baseexec_path+"xnbd-server" ,"--daemonize", "--proxy", "--readonly", "--pidfile", "/tmp/nbd"+str(devnr) +".pid" , ipaddress ,str(port),"--lport",str(baseport+devnr) , imagename + ".img" , imagename + ".bit"]
#	print cmd
#	p = subprocess.call(cmd)
#	print "unimplemented"
	


if __name__=='__main__':
	attach_blockdevice(0,"192.168.0.122",6666,"/tmp/192.168.0.122")

	time.sleep(10)

	dettach_blockdevice(0)
