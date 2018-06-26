# Booklist

Belegarbeit WS12/13 für Progammierung I

[![Build Status](https://semaphoreci.com/api/v1/eri451/booklist/branches/master/badge.svg)](https://semaphoreci.com/eri451/booklist)

## Beschreibung
Eine Liste von Medien die an Freunde verliehen wurden.
* Suche nach Title und Freunden
* sortieren nach Title, Author, to whom und Nr.
* Medium hinzu fügen (automatisches Datum)
* Medium löschen

## Abhängigkeiten
Dieses Programm hängt ab von
* libgtk2.0-dev (> 2.16)
* pkg-config

## Starten
```bash
curl https://nodeload.github.com/eri451/booklist/tar.gz/master | tar xvz
cd booklist-master
make
./booklist [file]
```
oder

```bash
git clone https://github.com/eri451/booklist.git
cd booklist
make
./booklist [file]
```

## Danksagungen
Ich möchte mich herzlich bei meinen Freunden vom c3d2, meinem Vater und
Prof. Beck für alle Anreize und Hilfestellungen bedanken.

## Lizenz
GNU Public License
```bash
cat booklist/LICENCE
```
