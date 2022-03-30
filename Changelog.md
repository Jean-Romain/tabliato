## Tabliato 1.0.2

- Fix: tabliato démarre maintenant toujours sur la tablature et sur l'onglet "Information"" au lieu de "paroles" et "Information complémentaires".
- New: tabliato teste toujour si une nouvelle version est disponible lors de l'ouverture. Si oui il propose de télécharger la nouvelle version auquel cas l'application renvoie vers la page de téléchargemnt et se ferme. L'utilisateur peut aussi dire non ou remettre à plus tard c'est à dire la prochaine fois que le programme va s'ouvrir.

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
