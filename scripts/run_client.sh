#!/usr/bin/env bash
HOST=${1:-127.0.0.1}
PORT=${2:-3333}
LOG=${3:-client.log}
./client "${HOST}" "${PORT}" "${LOG}"
