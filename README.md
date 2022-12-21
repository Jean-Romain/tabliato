# Tabliato

[Tabliato](https://jean-romain.github.io/tabliato/) est un logiciel pour GNU/Linux et Windows permettant d'écrire des tablatures d'accordéon diatonique de grande qualité aux formats CADB ou Corgeron. [Télécharger Tabliato](https://jean-romain.github.io/tabliato/download.html) et lire le [tutoriel](https://jean-romain.github.io/tabliato/doc.html).

[![](docs/img/slideview.png)](https://jean-romain.github.io/tabliato/)

## Compilation

Les informations suivantes ne sont intéressantes que pour les développeurs logiciel. Si vous souhaitez simplement utiliser le logiciel rendez-vous sur le [site](https://jean-romain.github.io/tabliato/).

### Linux

```
sudo apt-get install qtbase5-dev lilypond timidity timidity-interfaces-extra freepats libpoppler-qt5-dev qtmultimedia5-dev
qmake
make
```

### Windows

Ce dépot git ne contient pas les `.dll` nécessaires au fonctionnment du logiciel. De plus le logiciel `timidity++` est inclu dans le `.exe` téléchargeable sur le site mais ne se trouve pas dans le dépôt git. Si vous souhaitez vraiment compiler pour Windows le mieux est d'ouvrir une [issue](https://github.com/Jean-Romain/tabliato/issues) pour obtenir de l'aide.
- Installer `Qt5` avec `OpenSSL` ([help](https://stackoverflow.com/questions/71636426/qnetworkaccessmanager-does-not-work-on-windows-with-https)) en **32 bits** en utilisant `Qt maintenance tool`.
- Télécharger [Timidity++ pour Windows](https://sourceforge.net/projects/timidity/files/TiMidity%2B%2B/TiMidity%2B%2B-git-hourly/w32/)
- Utiliser [windeployqt](https://doc.qt.io/qt-6/windows-deployment.html) pour trouver les DLL
- Copier les DLL de [poppler](https://sourceforge.net/projects/poppler-qt5-mingw32/files/Poppler%200.32.0/)
- Compiler le project avec `mingw32`. Le projet doit être compilé en 32 bits car impossible de trouver des dll poppler en 64 bits.
