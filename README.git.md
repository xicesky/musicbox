
Diese README beschreibt wie git mit diesem repository benutzt werden soll.

Git Konfiguration
=================

Wenn das Repo das erste mal heruntergeladen wird (git clone ...), insbesondere unter
Windows, dann bitte kofigurierre mit folgendem Befehl git so, dass standardmäßig
"Unix Line Endings" (also LF statt CRLF) für Textdateien verwendet werden:
    
    git config core.eol lf

Am besten ist es, das ganze gleich global zu machen, so dass es für alle Repositories
auf dem Computer gilt:

    git config --global core.eol lf

Normale Vorgänge
================

Normaler Commit
---------------

1. Die zu commitenden Dateien werden "gestaged", in anderen Worten:
    es wird ausgewählt, was genau commited werden soll.

    git status          # Nachschauen was gerade los ist
    git add -A          # Ich möchte alles commiten
    git add ...         # Ich möchte eine bestimmte Datei/Unterverz. commiten
    git reset HEAD ...  # Ich möchte eine bestimmte Datei/Unterverz. doch nicht commiten
    git status          # Nochmal nachschauen (grün = wird commited)

2. Commit durchführen

    git commit -m "My message"      # Jetzt tatsächlich ausführen (mit Message)
    # - oder -
    git commit                      # Jetzt tatsächlich ausführen (Message kann in VIM editiert werden)

3. Mit dem zentralen Repo synchronisieren

    git pull                        # Hole neueste Änderungen vom Server
    git push                        # Schiebe meine neuesten Änderungen (commits) hoch

Anderes Zeug (TODO)
-------------------

////MERGEING
1) ist alle commitet? -> git status
2) git checkout master //der von dem ichs haben will
3) git pull //neuste änderungen vom master
4) git checkout freddy //zuruck zu meinem
5) git merge master //merged master mit freddy
	--> dabei kanns Konflikte geben!
		--> TODO

////CONFIG wiederherstellen ////ODER ANDERE FILES WIEDERHERSTELLEN
1) schau nach deinem letzten commit im dev.georeach.com wo alles ok war
2) hol dir die commit-hash
3) git checkout xxxxx-nr filepath
4) git commit //config in meinem branch nutzen
5) git pull
6) git push

/// Sachen "deployen"
1) alles commited?
2) git pull	muss klappen
3) git push	klappt dann auch
4) git checkout master
5) git pull
6) git log 	merk dir den hash ganz oben als <HASH>
7) git merge freddy_deployable			-- "Hol von deployable"
	-- Config zerfetzt? Kein problem:
8) 	git checkout <HASH> frontend/.mebb
und 	git checkout <HASH> frontend/app/georeach/.mebb
9) git commit -m "Reverted config for server deployment."
10) git push


// HOTFIX and BRANCHES
1) alles muss clean sein git st
2) git checkout master //gehe auf den branch von dem ich abzweigen will
3) git checkout -b fix_spelling_error_on_notifications //neuen erstellen
4) ganz normale commits machen
5) git checkout master // zurück wo es rein soll (den von oben)
6) git pull
7) git merge fix_spelling_error_on_notifications
8) git branch -d fix_spelling_error_on_notifications // löschen weil ich den fertig habe
9) git push


