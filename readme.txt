Rozwiązanie charakteryzuje się:
- nie może być 2 wolnych bloków obok siebie
- blok wolny zajmuje co najmniej 40 bajtów (czyli rozmiar struktury bloku)
- blok zajęty zajmuje co najmniej 24 bajty (16 bajtów oszczędzamy)
- struktura bloku znajduje się bezpośrednio przed zwróconym adresem 
- brak dziur (bloki są przenoszone, a poprzednik powiększany w miarę możliwości)
- jedyna dziura może powstać na początku areny (jeśli nie można zmieścić bloku w to miejsce, dziura ta jest eliminowana w momencie zwalniania tego bloku)
- arena jest zwalniania gdy są co najmniej 2 areny puste
- identyfikacja błędnych adresów przekazywanych do free (program po prostu nic nie robi, jak wykryje błędny adres)

