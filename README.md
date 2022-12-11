# Tabliato

[Tabliato](https://jean-romain.github.io/tabliato/) est un logiciel pour GNU/Linux et Windows permettant d'écrire des tablatures d'accordéon diatonique de grande qualité aux formats CADB ou Corgeron. [Télécharger Tabliato](https://jean-romain.github.io/tabliato/download.html).

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

Ce dépot git ne contient pas les `.dll` nécessaires au fonctionnment du logiciel. De plus le logiciel `timidity++` est inclu dans le `.exe` téléchargeable sur le site mais ne se trouve pas dans le dépôt git. Si vous souhaitez vraiment compiler pour Windows le mieux est d'ouvrir une [issue](https://github.com/Jean-Romain/tabliato/issues).
