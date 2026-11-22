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
