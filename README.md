# flashingled-driver

*Driver d'une LED 3 couleurs clignotante pour buildroot (linux embarqué)*

## Objetifs

Ce répertoire contient un driver pour Linux embarqué dont la cible est une carte **Olimex-olinuxino-maxi** (ARM) dont l'objectif est de commander une LED tricolore à 4 PINs et à cathode commune.

Ce driver fait clignoter successivement la LED sur ses 3 couleurs possibles.

Celui-ci met à disposition un fichier virtuel disponible par l'intermédiaire du chemin suivant `/dev/` et accessible en écriture seule.

## Utilisation

Pour activer la LED clignotante, utilisez la commande suivante :

```sh
dd if=/dev/flashled of=INTEGER bs=1 count=1
```

Où INTEGER est un entier. La fréquence de clignotement de la LED est inversement proportionnelle à cet entier. Autrement dit, plus cet entier est grand, moins la LED clignotera vite.

Si cet entier vaut 0, la LED s'éteint!,.

## Particularité du driver

Ce driver pratique l'utilisation d'un *ktimer* (timer du noyau Linux).

La période de ce timer est coefficientée par la valeur (entière) renseignée par l'utilisateur. 

Initialement elle est fixée  *100ms*. Si l'utilisateur renseigne **0**, le LED s'éteint (le timer est *off*).

## GPIOs utlisées

Les GPIOs utilisées sur l'Olimex sont les suivantes (N° de PIN physiques) :

    - GND
    - PIN 13
    - PIN 14
    - PIN 15
    - PIN 16
    - PIN 17
    - PIN 24
    - PIN 26
    - PIN 28
    - PIN 28
    - PIN 30

## Spécification du circuit électrique utilisé

Matériel principal : LED tricolore à 4 PINs à cathode commune.

*Modèle de spécification :*
```
[LED tricolore] Broche utilisée --> éventuels composants --> GPIO correspondante
```

#### PINOUT

*Circuit :*
```
 - Cathode commune --> Résistance 100 Ohms --> 3.3V
 - Anode Couleur 1 --> PIN 13
 - Anode Couleur 2 --> PIN 14
 - Anode Couleur 3 --> PIN 15
```

