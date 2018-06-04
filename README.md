# Czas

Repozytorium zawiera interpreter języka Czas zdefiniowanego poniżej.

## Składnia języka
Składnię języka **Czas** opisuje gramatyka, której symbolami końcowymi są 
  - `identyfikator`, zapisany jak w języku C, 
  - `liczba`, reprezentująca liczbę całkowitą w zapisie dziesiętnym, poprzedzoną opcjonalnym znakiem `+` lub `-`, 
  - znaki ujęte w apostrofy.

W kodzie źródłowym programu, przed lub po, ale nie wewnątrz, tekstowych reprezentacji symboli końcowych gramatyki języka, mogą wystąpić, w dowolnej liczbie, spacje, tabulacje, końce wiersza i znaki `|`. Nazywane są separatorami i nie mają wpływu na interpretację programu.

Symbolem początkowym gramatyki języka Czas jest `Program`.
```
Program → ε | Element Program

Element → Definicja | Instrukcja

Definicja → ':' Etykieta

Instrukcja → Odejmowanie | Skok | Wywołanie | Powrót | Czytanie | Pisanie

Odejmowanie → Adres Adres

Skok → Adres Etykieta

Wywołanie → Etykieta

Powrót → ';'

Czytanie → '^' Adres

Pisanie → Adres '^'

Etykieta → identyfikator

Adres → liczba
```
Etykiety reprezentują miejsca w kodzie. Każda etykieta w programie musi wystąpić w `Definicji`. O instrukcji bezpośrednio za definicją etykiety powiemy, że jest tą etykietą oznaczona. Program, w którym są dwie definicje etykiety o tym samym identyfikatorze, jest błędny.

Adresy reprezentują komórki pamięci.

## Pamięć maszyny
Pamięć maszyny jest nieograniczona. Adresami i wartościami jej komórek mogą być dowolne liczby całkowite ze znakiem. Początkowa wartość komórki o adresie `a` jest równa `-1 - a`. W opisie semantyki instrukcji wartość komórki o adresie `adres` oznaczamy przez `*adres`.

Maszyna ma też stos powrotu, który przechowuje informacje o miejscach w kodzie. Udostępnia on operacje włożenia na stos nowej informacji oraz zdjęcia ze stosu ostatniej położonej informacji, która nie została jeszcze zdjęta.

## Wykonanie programu
Pusty program nie ma żadnego efektu.

Wykonanie programu niepustego zaczyna się od pierwszej instrukcji. Kończy się albo po przejściu za ostatnią instrukcję albo po wykonaniu instrukcji kończącej program.

Po instrukcji aktualnej wykonywana jest albo instrukcja wskazana przez instrukcję aktualną albo instrukcja następna po niej w kodzie.

## Instrukcje maszyny
Maszyna Czas ma sześć instrukcji.

  - Instrukcja odejmowania `adres1 adres2` zapisuje w komórce o adresie `*adres1` wynik odejmowania `**adres2` od `**adres1`.

Jeśli np. przed wykonaniem instrukcji `3 1` było `*1 == 10`, `*3 == 30`, `*10 == 100` i `*30 == 300` to po jej wykonaniu będzie `*30 == 200`.

  - Instrukcja skoku `adres etykieta` sprawdza, czy `**adres` jest większe od zera. Jeśli tak, to następną wykonaną instrukcją będzie instrukcja oznaczona `etykietą`.

  - Instrukcja `wywołania` etykieta wskazuje, że następną wykonaną instrukcją ma być instrukcja oznaczona `etykietą`. Jednocześnie na stos powrotu wkładane jest miejsce w kodzie bezpośrednio za instrukcją aktualną.

  - Instrukcja powrotu `;` wskazuje, że następną wykonaną instrukcją ma być instrukcja w miejscu, które zdejmujemy ze stosu powrotu. Jeśli stos jest pusty, instrukcja powrotu kończy program.

  - Instrukcja czytania `^ adres` zapisuje w komórce o adresie `*adres` kod znaku wczytanego za pomocą funkcji `getchar()` języka C lub `-1`, jeśli wynikiem `getchar()` było `EOF`.

  - Instrukcja pisania `adres ^` pisze, za pomocą funkcji `putchar()` języka C, znak o kodzie `**adres`.
  
