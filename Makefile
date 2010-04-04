all:
	make -C xnbd/trunk

clean:
	make -C xnbd/trunk clean

install: all
	install -d ${DESTDIR}/usr/local/sparse-imager/bin/
	install -d ${DESTDIR}/usr/share/doc/sparse-imager/
	install -d ${DESTDIR}/var/www/spim/
	install -d ${DESTDIR}/var/www/spim/img/
	install -d ${DESTDIR}/var/www/spim/windows/
	install -d ${DESTDIR}/etc/init.d/
	
	install -D -m 555 imagerdb.py ${DESTDIR}/usr/local/sparse-imager/bin/
	install -D -m 555 daemon.py ${DESTDIR}/usr/local/sparse-imager/bin/
	install -D -m 555 mounter.py ${DESTDIR}/usr/local/sparse-imager/bin/
	install -D -m 555 mount_daemon ${DESTDIR}/usr/local/sparse-imager/bin/
	install -D -m 555 run_daemon ${DESTDIR}/usr/local/sparse-imager/bin/
	install -D -m 555 broadcast_listener ${DESTDIR}/usr/local/sparse-imager/bin/
	install -D -m 555 xnbd/trunk/xnbd-bgctl ${DESTDIR}/usr/local/sparse-imager/bin/
	install -D -m 555 xnbd/trunk/xnbd-server ${DESTDIR}/usr/local/sparse-imager/bin/
	
	install -D -m 500 dbschema.sql ${DESTDIR}/usr/share/doc/sparse-imager/
	
	
	install -D -o 33 -m 400 www/status.php ${DESTDIR}/var/www/spim/ 
	install -D -o 33 -m 400 www/remove.php ${DESTDIR}/var/www/spim/ 
	install -D -o 33 -m 400 www/index.php ${DESTDIR}/var/www/spim/ 
	install -D -o 33 -m 400 www/database.php ${DESTDIR}/var/www/spim/
	install -D -o 33 -m 400 www/config.php ${DESTDIR}/var/www/spim/
	install -D -o 33 -m 400 www/navigation.php ${DESTDIR}/var/www/spim/
	install -D -o 33 -m 400 www/help.php ${DESTDIR}/var/www/spim/ 
	install -D -o 33 -m 400 www/style.css ${DESTDIR}/var/www/spim/
	install -D -o 33 -m 400 www/images/DiskTransparantSmall.png ${DESTDIR}/var/www/spim/img/
	install -D -o 33 -m 400 windows/nbd-forensic-imager.exe ${DESTDIR}/var/www/spim/windows/
	
	install -D -m 555 startup/broadcast_listener ${DESTDIR}/etc/init.d/
	install -D -m 555 startup/mount_daemon ${DESTDIR}/etc/init.d/
	install -D -m 555 startup/run_daemon ${DESTDIR}/etc/init.d/
