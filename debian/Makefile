all:
	mkdir -p tabliato
	mkdir -p tabliato/DEBIAN
	mkdir -p tabliato/usr/bin/
	mkdir -p tabliato/usr/share/applications/
	mkdir -p tabliato/usr/share/app-install/icons/
	mkdir -p tabliato/usr/share/icons/
	cp control tabliato/DEBIAN/
	cp tabliato.desktop tabliato/usr/share/applications/
	cp ../tabliato tabliato/usr/bin/
	cp -r ../share/ tabliato/usr/share/
	cp ../share/icon/tabliato.svg tabliato/usr/share/app-install/icons/
	cp ../share/icon/tabliato.svg tabliato/usr/share/icons/
	rm -rf tabliato/usr/share/tabliato
	mv tabliato/usr/share/share tabliato/usr/share/tabliato
	dpkg-deb --build tabliato

clean:
	rm -rf tabliato/
	rm tabliato.deb

install:
	apt-get install --reinstall ./tabliato.deb

