all:
	mkdir -p tabliato
	mkdir -p tabliato/DEBIAN
	mkdir -p tabliato/usr/bin/
	mkdir -p tabliato/usr/share/icons/
	mkdir -p tabliato/usr/share/applications/
	mkdir -p tabliato/usr/share/app-install/icons/
	cp control tabliato/DEBIAN/
	cp tabliato.desktop tabliato/usr/share/applications/
	cp ../tabliato tabliato/usr/bin/
	cp ../ressources/icons/tabliato.svg tabliato/usr/share/app-install/icons/
	cp ../ressources/icons/tabliato.svg tabliato/usr/share/icons/
	rm -rf tabliato/usr/share/tabliato
	dpkg-deb --build tabliato

clean:
	rm -rf tabliato/

install:
	apt-get install --reinstall ./tabliato.deb

