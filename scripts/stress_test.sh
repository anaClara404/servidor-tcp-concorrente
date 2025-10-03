#!/usr/bin/env bash
N=${1:-10}
MSGS=${2:-20}
DELAY=${3:-50}

for i in $(seq 1 ${N}); do
  ./sim_client "c${i}" "${MSGS}" "${DELAY}" &
done

echo "Iniciados ${N} clientes simulados (PID list):"
jobs -p
