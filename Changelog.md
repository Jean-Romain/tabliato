## Tabliato 1.0.1

- New: tabliato dispose maintenant d'un vrai installateur sous Windows et est installé par default dans `C:`\Program File` comme un vrai logiciel
- Change: les soudfonts et les rendus temporaires sont maintenant sauvegardés dans `C:/Users/<USER>/AppData/Local/tabliato`. Une conséquence de ce changement est  que les futures ré-instalations du programme ne supprimeront plus les soundfonts téléchargées par l'utilisateur.
- Bug: quand une ligne de code était précédée d'une ligne de commentaires, la ligne de code était considérée comme un commentaire et les notes n'étaient pas rendues
  ```ly
  % Les notes de la ligne suivante n'étaient pas rendues
  p5 6 7 t5 6 7
  ```
- Bug: réparation du mode de rendu Cogeron qui ne fonctionnait plus
- Bug: encodage UTF-8 de l'erreur en cas d'échec de compilation par `lilypond`
- Bug: le rendu audio est maintenant rechargé correctement sous Windows après la compilation.
