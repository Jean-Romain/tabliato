## Tabliato 1.0.3

- New: les accords main gauche affichent une ligne de tenu si néccessaire [#14](https://github.com/Jean-Romain/tabliato/issues/14)
- New: capacité à détecté et informer avec un message les symboles inattendus dans un accord. Par exemple `p<7 8 G>` affiche un message: *Ligne 1 - Symbole innatentu détecté à l'intérieur d'un accord: G*.
- New: capacité à détecté et informer avec un message les symboles innatedus dans un accompagment. Par exemple `[A 2 a]` affiche un message: *Ligne 1 - Symbole innatentu détecté à l'intérieur d'un accompagnement: 2*.
- New: la coloration syntaxique reconnait les commandes `\set Timing.*`
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
