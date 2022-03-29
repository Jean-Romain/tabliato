## Tabliato 1.0.1

- Bug: quand une ligne de code était précédée d'une ligne de commentaires, la ligne de code était considérée comme un commentaire et les notes n'étaient pas rendues
```ly
% Les notes de la ligne suivante n'étaient pas rendues
p5 6 7 t5 6 7
```
- Bug: réparation du mode de rendu Cogeron qui ne fonctionnait plus
