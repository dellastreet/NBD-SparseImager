#!/usr/bin/python
import os
import sys
import string
import time
import shlex, subprocess
import syslog

baseport=6000
baseexec_path="/usr/local/sparse-imager/bin/"
basemountpoint="/tmp/nbdmount/"

def mount_partition(device):
	global basemountpoint
	os.makedirs(basemountpoint+device)
	cmd = [ "/bin/mount" ,"-o","ro", "/dev/"+device, basemountpoint+device]
	print cmd
	p = subprocess.call(cmd)
	
def umount_partition(device):
	global basemountpoint
	cmd = [ "/bin/umount" , "/dev/"+device]
	print cmd
	p = subprocess.call(cmd)
	os.rmdir(basemountpoint+device)
	
def mount_partitions(devnr):
	partitions=open("/proc/partitions","r").read()
	for part in partitions.split('\n'):
		cols=part.split()
		if (len(cols)>3):
			if (string.find(cols[3],"nbd"+str(devnr)+"p")==0):
				mount_partition(cols[3])

def umount_partitions(devnr):
	partitions=open("/proc/partitions","r").read()
	for part in partitions.split('\n'):
		cols=part.split()
		if (len(cols)>3):
			if (string.find(cols[3],"nbd"+str(devnr)+"p")==0):
				umount_partition(cols[3])


def attach_blockdevice(devnr, ipaddress,port,imagename):
        global baseexec_path,baseport
	cmd = [ baseexec_path+"xnbd-server" ,"--daemonize", "--proxy", "--readonly", "--pidfile", "/tmp/nbd"+str(devnr) +".pid" , ipaddress ,str(port),"--lport",str(baseport+devnr) , imagename + ".img" , imagename + ".bit"]
	print cmd
	p = subprocess.call(cmd)

	cmd = [ "/sbin/nbd-client" ,"127.0.0.1", str(baseport+devnr), "/dev/nbd"+str(devnr)]
	print cmd
	p = subprocess.call(cmd)

	mount_partitions(devnr)

	
	
def dettach_blockdevice(devnr):
	try:
		umount_partitions(devnr)
	except (OSError) as e:
		syslog.syslog("dettach_blockdevice unmount (%d)    %s (%d)" % (devnr,e.strerror, e.errno))

	cmd = [ "/sbin/nbd-client" ,"-d",  "/dev/nbd"+str(devnr)]
	print cmd
	p = subprocess.call(cmd)

        try :
		pidfile = open("/tmp/nbd"+str(devnr) +".pid", 'r')
		pidstr= pidfile.read()
		pidfile.close()
	        os.kill(int(pidstr),15) #TERM
		os.unlink("/tmp/nbd"+str(devnr) +".pid")
	except (OSError ,IOError) as  e:
		syslog.syslog("dettach_blockdevice(%d)    %s (%d)" % (devnr,e.strerror, e.errno))

	
#	cmd = [ baseexec_path+"xnbd-server" ,"--daemonize", "--proxy", "--readonly", "--pidfile", "/tmp/nbd"+str(devnr) +".pid" , ipaddress ,str(port),"--lport",str(baseport+devnr) , imagename + ".img" , imagename + ".bit"]
#	print cmd
#	p = subprocess.call(cmd)
#	print "unimplemented"
	


if __name__=='__main__':
	umount_partitions(0)
#	attach_blockdevice(0,"192.168.0.122",7000,"/tmp/192.168.0.122")
	#time.sleep(10)

#	dettach_blockdevice(0)
