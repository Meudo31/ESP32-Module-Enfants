# Étape 1 : boutons et lumières

Au repos, l'anneau NeoPixel tourne en arc-en-ciel. Quand l'enfant appuie sur un bouton, l'anneau se remplit de sa couleur en partant de son quart, reste allumé tant que le bouton est tenu, puis s'efface doucement.

Le code compile avec PlatformIO (ESP32 DevKit, framework Arduino) mais n'a pas encore été testé sur une vraie carte.

## Câblage

| De | Vers |
| --- | --- |
| Bouton rouge | GPIO32 et GND |
| Bouton vert | GPIO33 et GND |
| Bouton bleu | GPIO18 et GND |
| Bouton jaune | GPIO19 et GND |
| Anneau, DIN | GPIO13, via une résistance de 330 Ω |
| Anneau, 5V | VIN de l'ESP32 |
| Anneau, GND | GND |
| Condensateur 1000 µF | entre 5V et GND de l'anneau, le + côté 5V |

Les boutons d'arcade ont souvent 4 cosses : deux pour le contact, deux pour la LED intégrée. Repère les cosses du contact au multimètre (bip quand on appuie). Les LED intégrées ne sont pas utilisées.

L'ordre des LED de l'anneau est découpé en 4 quarts de 3 LED : LED 0 à 2 pour le rouge, 3 à 5 pour le vert, et ainsi de suite. Place les boutons autour de l'anneau dans le même ordre pour que la lumière parte du bon côté.

## Mise en route

1. Branche la carte en USB, puis lance `pio run -t upload` et `pio device monitor` depuis ce dossier. Si le téléversement reste bloqué sur « Connecting... », maintiens le bouton BOOT de la carte.
2. Au démarrage, le moniteur affiche « Module enfants - étape 1 prête » et l'anneau tourne en arc-en-ciel.
3. Chaque appui affiche « Bouton 1 » à « Bouton 4 » et remplit l'anneau.

## Points à vérifier ensemble

- **L'anneau reste éteint ou clignote au hasard** : vérifie la flèche de sens sur l'anneau (DIN, pas DOUT) et la masse commune. Le signal 3,3 V de l'ESP32 suffit presque toujours pour un anneau alimenté en 5 V ; sinon, ajoute un convertisseur de niveau 74AHCT125.
- **Couleurs inversées (rouge et vert échangés)** : remplace `NEO_GRB` par `NEO_RGB` dans `main.cpp`.
- **Un bouton déclenche tout seul** : fil trop long ou mal serré ; l'anti-rebond est réglé à 30 ms dans `main.cpp`.
