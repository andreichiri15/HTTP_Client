# Client HTTP - Andrei Chirimeaca 322CA

## OVERVIEW
Pentru aceasta tema, am avut de implementat un client care face cereri HTTP catre un server, clientul avand la dispozitie mai multe comenzi in linia de comanda, si anume: register, login, enter_library, add_book, get_book, get_books, logout si exit.

## REGISTER
Pentru a se inregistra, utilizatorul trebuie sa dea un username si parola valida (fara spatii), dupa care se creeza un message de tip POST request catre server, continand username-ul si parola.
Serverul intoarce un raspuns, care poate contine eroare (de obicei daca username-ul a fost utilizat de altcineva) sau nu, caz in care afisam faptul ca un nou cont a fost creat cu informatiile date.

## LOGIN
Utilizatorul furnizeaza username-ul si parola contului la care vrea sa se logheze, fiind verificat daca datele sunt valide(nu contin spatii). Daca sunt valide, este trimis un mesaj catre server care contine aceste date. Server-ul raspunde cu un raspuns care poate contine: eroare, daca datele nu corespund vreunui cont creat, JWT si cookies pentru utilizator in cazul in care acesta s-a logat cu datele corect, acesta devenind logat.

## ENTER_LIBRARY
Pentru a putea accesa cartile, utilizatorul trebuie sa fie "intrat in biblioteca" (adica sa fi folosit aceasta comanda inainte de a folosi comenzi ce au legatura cu obtinerea, adaugarea de carti). Utilizatorul trebuie sa fie logat pentru a executa comanda. Daca este, trimite un mesaj catre server, care ii raspunde cu jwt updatat (sau eroare, daca e cazul).

## ADD_BOOK
Aceasta comanda presupune adaugarea unei noi carti in biblioteca utilizatorului. Utilizatorului ii sunt cerute datele despre cartea pe care vrea sa o adauge, dupa care acestea sunt introduse intr-un json, care este incorporat in mesajul trimis catre server. Server-ul raspunde cu un mesaj care informeaza utilizatorul daca operatia s-a realizat cu succes.

## GET_BOOK / GET_BOOKS
Prin aceasta comanda, utilizatorul face o cerere la server pentru a primi fie toate cartile (get_books), fie cartea cu id-ul cautat(get_book <id>). Server-ul intoarce un mesaj cu un json care contine cartea cu id-ul cautat / lista de carti aflate in biblioteca utilizatorului.

## DELETE_BOOK
Utilizatorul doreste sa elimine din biblioteca sa cartea cu id-ul dat de la tastatura. Face o cerere la server, la url-ul care contine id-ul cartii, iar server-ul raspunde daca a reusit sa elimine cartea din biblioteca sa cu succes, fiind afisat in consola acest lucru.

## LOGOUT
Prin aceasta comanda, utilizatorul doreste sa se delogheze, lucru pe care il face trimitand un mesaj la server, la url-ul specific destinat pentru delogari. Server-ul raspunde cu un mesaj care specifica daca delogarea a avut loc cu succes sau nu.

