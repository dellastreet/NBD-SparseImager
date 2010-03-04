all:
	make -C xnbd/trunk

clean:
	make -C xnbd/trunk clean

install: all
	install -d ${DESTDIR}/usr/local/sparse-imager/bin/
	install -d ${DESTDIR}/usr/share/doc/sparse-imager/
	
	install -D -m 500 database.py ${DESTDIR}/usr/local/sparse-imager/bin/
	install -D -m 500 broadcast_listener ${DESTDIR}/usr/local/sparse-imager/bin/
	install -D -m 500 xnbd/trunk/xnbd-bgctl ${DESTDIR}/usr/local/sparse-imager/bin/
	install -D -m 500 xnbd/trunk/xnbd-server ${DESTDIR}/usr/local/sparse-imager/bin/
	
	install -D -m 500 dbschema.sql ${DESTDIR}/usr/share/doc/sparse-imager/
