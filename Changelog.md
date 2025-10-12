## Tabliato 1.3.5

- Bug: #52 ajout d'une option pour les machines très lentes.
- Bug: #56 les doubles répétition ne font plus planter le logiciel.
- Amélioration: tabliato n'est plus bloqué quand on lance la compilation. La fenêtre reste réactive.

## Tabliato 1.3.4

- New: Affichage d’une infobulle au survol de certains mots-clés du code pour aider l’utilisateur.
- New: Ajout du clavier "Castagnari Roma"
- Fix: Correction de l’affichage des doigtés qui ne fonctionnait plus.
- Fix: Imposition du thème clair par défaut, indépendamment du thème du système d’exploitation.
- Fix: Le mode Corgeron souligne désormais correctement les notes.
- Fix: Coloration syntaxique des chaines de charactères multiples améliorée (non-greedy regex)
- Change: Révision des exemples fournis avec Tabliato.

## Tabliato 1.3.3

- New: on peut maintenant ouvrir un fichier tabliato en cliquant sur un fichier `.dtb`.
- New: `tabliato` retient la taille des caratères de l'éditeur entre deux sessions.
- Change: utilisation de la police Consola sous windows pour le code source + taille 11 par defaut.
- Change: sous Windows `tabliato` test si `lilypond` est installé avant de compiler afin de renvoyer une erreur informative le cas contraire.
- Bug: les chaines de caractères sont colorées et parsées correctement. Par ex. dans `\mbox "Partie 1 - valse"` le 1 n'est plus interprété comme le bouton 1. La chaine est traitée correctement.
- Bug: amélioration de coloration syntaxique des informations de durée.
- Bug: sous Windows certain motifs rythmiques disparaissaient de la liste déroulante pour certain tempo [#39](https://github.com/Jean-Romain/tabliato/issues/39)

## Tabliato 1.3.2

- Change: quand une nouvelle partition est créée, le champ `Tagline` est prérempli avec la commande `tabliato` afin que les partitions créées avec `tabliato` aient le texte suivant en bas de page: *Éditié avec Tabliato et Lilypond*. Les utilisateurs peuvent supprimer cette marque mais elle apparait par default.
- Bug: quand une nouvelle partition est créée, les paroles de l'ancienne n'étaient pas supprimées.
- New: nouvelle commande `\mbox`. Voir la [documentation](https://jean-romain.github.io/tabliato/doc.html) pour plus de détails.
- New: simplification de l'interface graphique. Tous les éléments de formulaire sont maintenant au même endroit.

## Tabliato 1.3.1

- Change: les fichiers de configuration clavier sont maintenant dans un dossier utilisateur ce qui permet de créer ses propres claviers.
- New: ajout d'un bouton Fichier > Configuation pour trouver les dossiers qui contiennent les fichiers de configuration (clavier et soundfonts).

## Tabliato 1.3.0

- New: support d'ajout d'indications de doigté
- Doc: ajout d'une section sur les indications doigté dans la documentation.

## Tabliato 1.2.0

- New: le mode d'écriture par note plutôt que par bouton à été repensé, réparé et documenté. Plutôt que d'écire les boutons `p7' t8 ...` etc. il est aussi possible d'écrire les notes de musiques. Les correspondances muliples sont affichées sur la tabature. Quand une note correspond à plusieurs bouttons possibles, c'est à dire quand l'utilisateur donne une note sans indication de rangée ou de direction, toutes les possibilités sont affichées sur la tablature en rouge au lieu d'un message d'erreur.
- New: la page de documentation a été modifiée pour ajouter une partie sur ce mode d'écriture.
- Bug: tracking de la musique sur le rendu même si il n'y pas d'accompagnement main gauche.

## Tabliato 1.1.0

- New: support du "point and click" bidirectionnel. Il possible de cliquer sur la tablature pour trouver la ligne de code correspondante et inversement se mettre sur une ligne de code met en evidence la correspondance dans la tablature en surlignant en jaune les éléments.
- New: tracking de la musique. Lorsque la pièce est jouée un curseur suit la musique en directe sur le rendu graphique
- New: Changement de l'interface pour zoomer et changer de page sur le rendu graphique.
- Change: Les soundfonts sont maintenant téléchargées depuis leur origine sur http://jmi.ovh/DiatonicTab/SoundFonts/
- Bug: nombreux bugs corrigés

## Tabliato 1.0.5

- Bug: inversion du sens des notes après avoir écrit des accords [#26](https://github.com/Jean-Romain/tabliato/issues/26)
- Bug: barre de tenue des notes qui ne s'arrête pas après un silence [#25](https://github.com/Jean-Romain/tabliato/issues/25)
- Bug: décalage des boutons à deux chiffres avec barre de tenue [#25](https://github.com/Jean-Romain/tabliato/issues/25)
- Bug: crash du logiciel lorsque un silence apparait sans `:` dans un motif p.ex. `\motif [B:4 a:4 r4]`


## Tabliato 1.0.4

- New: nouvelle syntaxe simplifiée pour la main gauche. Quand une mesure comporte plusieurs basses et accords différents on peut écrire uniquement les basses et ne pas écrire les accords et laisser tabliato interpréter. Par example en 6/8 avec un motif rythmique "B aB a":
  ```ly
  A >> A aA a
  [C] >> C cC c
  [A a C c] >> A a C c
  [A C] >> A a C c % Nouvelle syntaxe simplifiée
  ```
- Bug: tabliato 1.0.3 ne fonctionnait plus sous Windows à cause de fichiers de configuration modifiés mais non inclus dans la version Windows. Les fichiers de configurations sont maintenant correctement inclus dans la version Windows et les tablatures rendues correctement.

## Tabliato 1.0.3

- New: les accords main gauche affichent une ligne de tenu si nécessaire [#14](https://github.com/Jean-Romain/tabliato/issues/14)
- New: capacité à détecter et informer avec un message les symboles inattendus dans un accord. Par exemple `p<7 8 G>` affiche un message: *Ligne 1 - Symbole inattendu détecté à l'intérieur d'un accord: G*.
- New: capacité à détecter et informer avec un message les symboles inattendus dans un accompagnement. Par exemple `[A 2 a]` affiche un message: *Ligne 1 - Symbole inattendu détecté à l'intérieur d'un accompagnement: 2*.
- New: la coloration syntaxique reconnaît les commandes `\set Timing.*`
- Bug: lorsqu'une erreur est détectée dans le code, le numéro de la ligne est le bon même si il y a des commentaires avant. Les commentaires trompaient le compte des lignes.
- Bug: amélioration de l'interprétation de anacrouses. Dans `\partial 4` le 4 était interprété comme le bouton 4. Il fallait écrire `\partial:4` ou  `\partial :4` ou `\partial 2*8` pour protéger le 4. Le mot clé `\partial` est maintenant reconnu et `\partial 4` fonctionne. La coloration syntaxe a été mise à jour.

## Tabliato 1.0.2

- New: tabliato test toujours si une nouvelle version est disponible lors de l'ouverture. Si oui il propose de télécharger la nouvelle version auquel cas l'application renvoie vers la page de téléchargemnt et se ferme. L'utilisateur peut aussi dire non ou remettre à plus tard c'est à dire la prochaine fois que le programme va s'ouvrir.
- Fix: tabliato démarre maintenant toujours sur la tablature et sur l'onglet "Information"" au lieu de "paroles" et "Information complémentaires"
- Fix: le dock "lecture de la musique" a une taille fixe. Sa taille variait d'une ouverture à l'autre sans raison apparente. Il a maintenant la taille minimal fixe.


## Tabliato 1.0.1

- New: [windows] tabliato dispose maintenant d'un vrai installateur et est installé par default dans `C:\Program File\tabliato` comme un vrai logiciel
- Change: [windows] les soudfonts et les rendus temporaires sont maintenant sauvegardés dans `C:\Users\<USER>\AppData\Local\tabliato`. Une conséquence de ce changement est  que les futures ré-instalations du programme ne supprimeront plus les soundfonts téléchargées par l'utilisateur.
- Bug: quand une ligne de code était précédée d'une ligne de commentaires, la ligne de code était considérée comme un commentaire et les notes n'étaient pas rendues
  ```ly
  % Les notes de la ligne suivante n'étaient pas rendues
  p5 6 7 t5 6 7
  ```
- Bug: réparation du mode de rendu Cogeron qui ne fonctionnait plus
- Bug: encodage UTF-8 de l'erreur en cas d'échec de compilation par `lilypond`
- Bug: [windows] le rendu audio est maintenant rechargé correctement après la compilation.
