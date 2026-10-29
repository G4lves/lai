#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

case "$(uname -m)" in
  x86_64)
    ARCH="x86_64"
    ;;
  aarch64|arm64)
    ARCH="aarch64"
    ;;
  *)
    echo "Unsupported architecture: $(uname -m)" >&2
    exit 1
    ;;
esac

SCRIPT="$SCRIPT_DIR/setup-lazyvim-${ARCH}.sh"

if [[ ! -f "$SCRIPT" ]]; then
  echo "Missing installer: $SCRIPT" >&2
  exit 1
fi

echo "Running $SCRIPT"
bash "$SCRIPT"
