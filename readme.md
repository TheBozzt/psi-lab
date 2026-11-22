# PSI – Zadanie 1C: Współbieżny serwer TCP (MD5)

**Bartosz Pietrzak, Wojciech Szozda, Piotr Winiarczyk (Zespół 38)**

Przy testach lokalnych poza serwerem `bigubu` należy jednorazowo utworzyć sieć:

```bash
docker network create --subnet=172.21.38.0/24 z38_network
```

## Uruchomienie i testowanie

1. **Zbudowanie obrazów oraz uruchomienie serwera w tle:**

    ```bash
    docker compose up -d --build
    ```

2. **Uruchomienie testów równoległych klientów:**

    ```bash
    chmod +x test.sh
    ./test.sh
    ```

3. **Aby wpisywać wiadomości ręcznie (zakończenie komendą `exit`):**

    ```bash
    docker attach z38_client
    ```

4. **Zatrzymanie serwera i usunięcie kontenerów po zakończeniu testów:**

    ```bash
    docker compose down
    ```

## Użycie klienta (bezpośrednio)

```bash
./client [adres_IP] [wiadomość_1] [wiadomość_2] ...
```

- Przy braku argumentów klient łączy się z domyślnym adresem `127.0.0.1` i uruchamia tryb interaktywny.
- Przy podaniu samego adresu IP (np. `./client 172.21.38.0`) klient łączy się ze wskazanym serwerem i uruchamia tryb interaktywny.
- Przy większej liczbie argumentów (np. `./client 172.21.38.0 "tekst 1" "tekst 2"`) klient wysyła podane ciągi znaków jeden po drugim.
