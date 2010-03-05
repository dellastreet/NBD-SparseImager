all:
	make -C xnbd/trunk

clean:
	make -C xnbd/trunk clean

install: all
	install -d ${DESTDIR}/usr/local/sparse-imager/bin/
	install -d ${DESTDIR}/usr/share/doc/sparse-imager/
	install -d ${DESTDIR}/var/www/spim/
	install -d ${DESTDIR}/var/www/spim/img/
	
	install -D -m 540 imagerdb.py ${DESTDIR}/usr/local/sparse-imager/bin/
	install -D -m 540 broadcast_listener ${DESTDIR}/usr/local/sparse-imager/bin/
	install -D -m 540 xnbd/trunk/xnbd-bgctl ${DESTDIR}/usr/local/sparse-imager/bin/
	install -D -m 540 xnbd/trunk/xnbd-server ${DESTDIR}/usr/local/sparse-imager/bin/
	
	install -D -m 500 dbschema.sql ${DESTDIR}/usr/share/doc/sparse-imager/
	
	
	install -D -o 33 -m 400 www/status.php ${DESTDIR}/var/www/spim/ 
	install -D -o 33 -m 400 www/db.php ${DESTDIR}/var/www/spim/ 
	install -D -o 33 -m 400 www/images/DiskTransparantSmall.png ${DESTDIR}/var/www/spim/img/
