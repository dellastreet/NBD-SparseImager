#
# Regular cron jobs for the sparse-imager package
#
0 4	* * *	root	[ -x /usr/bin/sparse-imager_maintenance ] && /usr/bin/sparse-imager_maintenance
