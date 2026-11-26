#!/bin/bash

SERVER_IP="172.21.38.10"
NETWORK="z38_network"
CLIENT_IMAGE="z38_client_1c"

MSG_CLIENT1=("test" "Hello world!" "trzecia wiadomość")
MSG_CLIENT2=("Programowanie sieciowe" "Zespół 38" "Klient numer dwa")
MSG_CLIENT3=($(printf '%2500s' | tr ' ' 'A') "Trzy" "Lab")

MAX_DISPLAY_LEN=30

echo "Rozpoczęto test współbieżności"
echo "Uruchamianie 3 równoległych klientów..."
echo

run_client() {
    local id=$1
    shift

    echo "[Klient $id] Start..."
    docker run --rm --network "$NETWORK" --name "z38_client_$id" "$CLIENT_IMAGE" "$SERVER_IP" "$@" | sed "s/^/ [Klient $id] /"
}

run_client 1 "${MSG_CLIENT1[@]}" &
PID1=$!

run_client 2 "${MSG_CLIENT2[@]}" &
PID2=$!

run_client 3 "${MSG_CLIENT3[@]}" &
PID3=$!

wait $PID1 $PID2 $PID3

echo
echo "Weryfikacja poprawności"

for msg in "${MSG_CLIENT1[@]}" "${MSG_CLIENT2[@]}" "${MSG_CLIENT3[@]}"; do
    hash=$(echo -n "$msg" | md5sum | head -c 32)

    if [ ${#msg} -gt $MAX_DISPLAY_LEN ]; then
        disp="'${msg:0:$MAX_DISPLAY_LEN}...'"
    else
        disp="'$msg'"
    fi

    printf "Tekst: %s |-> Oczekiwany MD5: %s\n" "$disp" "$hash"
done | column -t -s '|'
