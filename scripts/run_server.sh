#!/usr/bin/env bash
PORT=${1:-3333}
LOG=${2:-server.log}
./server "${PORT}" "${LOG}"
