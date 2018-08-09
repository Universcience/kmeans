# `kmeans`

Illustration du partitionnement de jeux de données aléatoires par la méthode des *k*-moyennes, développé dans le cadre de l'exposé **L'Intelligence Artificielle** du Palais de la Découverte.

## Description

Ce programme permet d'illustrer le fonctionnement d'un algorithme d'apprentissage automatique non-supervisé. Il génère plusieurs jeux de données sous la formes de nuages de points. Il choisit ensuite une série de points au hasard et tente de reconstruire les catégories d'origine par la méthode des [*k*-moyennes(en)](https://en.wikipedia.org/wiki/K-means_clustering).

Afin d'illustrer le déroulement de l'algorithme, les points et leur partitionnement supposé sont affichés à l'écran ; des contrôles claviers permettent de faire progresser l'algorithme et de comparer ses résultats avec la "vraie" réponse.

# Déploiement

Le programme fourni est conçu pour un système GNU/Linux ([Debian(fr)](https://www.debian.org/index.fr.html)), mais peut être aisément porté sur d'autres systèmes.

Le fichier `Makefile` fourni permet de compiler le programme C.

## Paquetages nécessaires (Debian Stretch)
- `gcc`
- `make`
- `libsdl2-dev`

## Usage
```
git clone https://github.com/universcience/kmeans
cd kmeans
make
./kmeans
```

Le logiciel génère (par défaut) 3 groupes de dix points distribués aléatoirement autour de 3 centroïdes placés au hasard. Les touches `c` et `h` permettent de visualiser les centroïdes et l'appartenance réelle des points à chaque groupe. Ces informations sont inconnus de l'algorithme, qui tente de les déduire à partir du placement des points.

Le logiciel choisit immédiatement trois points au hasard dans le lot, comme point de départ pour déterminer l'emplacement de chaque catégorie. Tous les points les plus proches de chacun des "centroïdes" choisis sont réputés appartenir à la même "catégorie". Cette supposition initiale (matérialisée par les couleur visibles des points et des traits) est souvent très erronée, mais s'améliore progressivement au fil du déroulement de l'algorithme.

La touche `n` permet d'avancer d'un pas : l'algorithme recherche alors le point médian de chaque "catégorie" et le désigne comme le nouveau "centroïde", avant de recalculer les "catégories" correspondantes. Après quelques pas de calcul, l'algorithme parvient à une réponse définitive (les catégories restent les mêmes) : le résultat obtenu est généralement assez proche des catégories d'origines (vérifiable avec la touche `h`), sauf dans certains cas difficiles (notamment si deux catégories sont très proches, voire mélangées).

Il est possible de relancer l'algorithme de zéro sur le même ensemble de points en appuyant sur `r`. Autrement, un nouveau jeu de données est généré à chaque démarrage du programme.

# Licence

Ceci est un [logiciel libre(fr)](https://www.gnu.org/philosophy/free-sw.fr.html), distribué sous les termes de la licence GNU GPL Affero. Les termes exacts de cette licence sont disponibles dans le fichier `LICENSE`.

En résumé, vous êtes libres de télécharger ce logiciel, de l'utiliser et de le modifier sans restrictions, ainsi que de le redistribuer sous les mêmes termes, avec ou sans modifications. 
