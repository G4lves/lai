#!/usr/bin/env bash
#
# LAI: LazyVim Automated Installation
# Debian/Ubuntu (apt) on x86_64/amd64 or aarch64/arm64.
#
# Usage: bash scripts/install-lazyvim.sh [options]
#
#   -y, --yes         Don't ask for confirmation (needed when not run from a terminal)
#       --upgrade     Also run "apt-get upgrade" (off by default)
#       --with-extras Also install Node.js, npm, Python 3 (venv, pip), xclip, wl-clipboard
#       --no-launch   Don't open Neovim at the end
#   -h, --help        Show this help
#
# Environment overrides:
#   NVIM_VERSION      Neovim release tag, e.g. v0.11.2 (default: latest stable)
#   LAZYGIT_VERSION   LazyGit version without the leading "v" (default: latest)

set -Eeuo pipefail

NVIM_VERSION="${NVIM_VERSION:-latest}"
LAZYGIT_VERSION="${LAZYGIT_VERSION:-}"

ASSUME_YES=0
UPGRADE=0
WITH_EXTRAS=0
LAUNCH=1

info() { printf '\033[1;34m==>\033[0m %s\n' "$*"; }
warn() { printf '\033[1;33mwarning:\033[0m %s\n' "$*" >&2; }
die()  { printf '\033[1;31merror:\033[0m %s\n' "$*" >&2; exit 1; }

usage() { awk 'NR > 1 && /^set /{exit} NR > 1 {sub(/^# ?/, ""); print}' "${BASH_SOURCE[0]}"; }

for arg in "$@"; do
  case "$arg" in
    -y|--yes)       ASSUME_YES=1 ;;
    --upgrade)      UPGRADE=1 ;;
    --with-extras)  WITH_EXTRAS=1 ;;
    --no-launch)    LAUNCH=0 ;;
    -h|--help)      usage; exit 0 ;;
    *)              die "Unknown option: $arg (see --help)" ;;
  esac
done

[[ "$(uname -s)" == "Linux" ]] || die "This script only supports Linux."
[[ $EUID -ne 0 ]] || die "Run this as your normal user, not root. It calls sudo itself."
command -v apt-get >/dev/null || die "apt-get not found. Only Debian/Ubuntu-based systems are supported."
command -v sudo >/dev/null || die "sudo is required."

case "$(uname -m)" in
  x86_64|amd64)  NVIM_ARCH="x86_64"; LAZYGIT_ARCH="x86_64"; TS_ARCH="x64" ;;
  aarch64|arm64) NVIM_ARCH="arm64";  LAZYGIT_ARCH="arm64";  TS_ARCH="arm64" ;;
  *) die "Unsupported architecture: $(uname -m)" ;;
esac

CONFIG_HOME="${XDG_CONFIG_HOME:-$HOME/.config}"
DATA_HOME="${XDG_DATA_HOME:-$HOME/.local/share}"
STATE_HOME="${XDG_STATE_HOME:-$HOME/.local/state}"
CACHE_HOME="${XDG_CACHE_HOME:-$HOME/.cache}"
NVIM_DIRS=("$CONFIG_HOME/nvim" "$DATA_HOME/nvim" "$STATE_HOME/nvim" "$CACHE_HOME/nvim")
STAMP="$(date +%Y%m%d-%H%M%S)"

TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT
trap 'die "Failed at line $LINENO."' ERR

confirm() {
  if (( ASSUME_YES )); then return 0; fi
  [[ -t 0 ]] || die "Not running in a terminal. Re-run with --yes to continue."
  local reply
  read -r -p "$1 [y/N] " reply
  [[ "$reply" =~ ^[Yy]$ ]]
}

existing=()
for d in "${NVIM_DIRS[@]}"; do
  if [[ -e "$d" || -L "$d" ]]; then existing+=("$d"); fi
done
if (( ${#existing[@]} )); then
  echo "These existing Neovim folders will be moved to <name>.bak.$STAMP:"
  printf '  %s\n' "${existing[@]}"
  confirm "Continue?" || die "Aborted. Nothing was changed."
fi

info "Installing packages"
sudo apt-get update
if (( UPGRADE )); then sudo apt-get upgrade -y; fi

PACKAGES=(git curl wget unzip xz-utils build-essential ripgrep fd-find fzf fontconfig)
if (( WITH_EXTRAS )); then
  PACKAGES+=(nodejs npm python3 python3-venv python3-pip xclip wl-clipboard)
fi
sudo apt-get install -y "${PACKAGES[@]}"

verify_checksum() {
  local base="$1" asset="$2" expected actual
  if ! curl -fsSL --retry 3 -o "$TMP/shasum.txt" "$base/shasum.txt"; then
    warn "No checksum file published for this release; skipping verification."
    return 0
  fi
  expected="$(awk -v f="$asset" '$2 == f || $2 == "*" f {print $1}' "$TMP/shasum.txt")"
  if [[ -z "$expected" ]]; then
    warn "No checksum entry for $asset; skipping verification."
    return 0
  fi
  actual="$(sha256sum "$TMP/$asset" | awk '{print $1}')"
  [[ "$expected" == "$actual" ]] || die "Checksum mismatch for $asset. Download is corrupt or tampered with."
}

install_neovim() {
  local asset="nvim-linux-${NVIM_ARCH}.tar.gz" base
  if [[ "$NVIM_VERSION" == "latest" ]]; then
    base="https://github.com/neovim/neovim/releases/latest/download"
  else
    base="https://github.com/neovim/neovim/releases/download/${NVIM_VERSION}"
  fi

  info "Downloading Neovim ($NVIM_VERSION)"
  curl -fL --retry 3 -o "$TMP/$asset" "$base/$asset"
  verify_checksum "$base" "$asset"

  mkdir -p "$TMP/nvim"
  tar -xzf "$TMP/$asset" -C "$TMP/nvim" --strip-components=1

  "$TMP/nvim/bin/nvim" --version >/dev/null \
    || die "The downloaded Neovim doesn't run on this system (glibc too old?)."

  sudo rm -rf /opt/nvim
  sudo mv "$TMP/nvim" /opt/nvim
  sudo chown -R root:root /opt/nvim
  sudo ln -sf /opt/nvim/bin/nvim /usr/local/bin/nvim
}
install_neovim

mkdir -p "$HOME/.local/bin"
if command -v fdfind >/dev/null; then
  ln -sf "$(command -v fdfind)" "$HOME/.local/bin/fd"
fi

PATH_LINE='export PATH="$HOME/.local/bin:$PATH"'
for rc in "$HOME/.bashrc" "$HOME/.zshrc"; do
  if [[ "$rc" == "$HOME/.zshrc" && ! -f "$rc" ]]; then continue; fi
  if ! grep -qs '\.local/bin' "$rc"; then
    printf '\n%s\n' "$PATH_LINE" >> "$rc"
  fi
done
export PATH="$HOME/.local/bin:$PATH"

install_lazygit() {
  local version="${LAZYGIT_VERSION#v}" url asset
  if [[ -z "$version" ]]; then
    # Follow the /releases/latest redirect instead of the rate-limited GitHub API.
    url="$(curl -fsSLI -o /dev/null -w '%{url_effective}' https://github.com/jesseduffield/lazygit/releases/latest)"
    version="${url##*/v}"
  fi
  [[ "$version" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]] \
    || die "Could not determine the LazyGit version (got '$version')."

  asset="lazygit_${version}_Linux_${LAZYGIT_ARCH}.tar.gz"
  info "Downloading LazyGit v$version"
  curl -fL --retry 3 -o "$TMP/$asset" \
    "https://github.com/jesseduffield/lazygit/releases/download/v${version}/${asset}"
  tar -xzf "$TMP/$asset" -C "$TMP" lazygit
  sudo install -m 0755 "$TMP/lazygit" /usr/local/bin/lazygit
}
install_lazygit

install_tree_sitter() {
  local asset="tree-sitter-linux-${TS_ARCH}"
  curl -fL --retry 3 -o "$TMP/$asset.gz" \
    "https://github.com/tree-sitter/tree-sitter/releases/latest/download/$asset.gz" || return 1
  gunzip -f "$TMP/$asset.gz" || return 1
  chmod +x "$TMP/$asset"
  "$TMP/$asset" --version >/dev/null || return 1
  sudo install -m 0755 "$TMP/$asset" /usr/local/bin/tree-sitter
}

if command -v tree-sitter >/dev/null; then
  info "tree-sitter already installed: $(tree-sitter --version)"
elif (trap - ERR; install_tree_sitter); then
  info "Installed tree-sitter: $(tree-sitter --version)"
else
  warn "Could not install tree-sitter-cli automatically."
  warn "Install it manually (see README), otherwise syntax highlighting parsers won't build."
fi

info "Installing the LazyVim starter"
backup_name() {
  local base="$1.bak.$STAMP" candidate i=1
  candidate="$base"
  while [[ -e "$candidate" || -L "$candidate" ]]; do
    candidate="$base.$i"
    i=$((i + 1))
  done
  printf '%s' "$candidate"
}

for d in "${NVIM_DIRS[@]}"; do
  if [[ -e "$d" || -L "$d" ]]; then
    dest="$(backup_name "$d")"
    mv -T -- "$d" "$dest"
    info "Backed up $d -> $dest"
  fi
done

mkdir -p "$CONFIG_HOME"
git clone --depth 1 https://github.com/LazyVim/starter "$CONFIG_HOME/nvim"
rm -rf "$CONFIG_HOME/nvim/.git"

info "Installed: $(nvim --version | head -n1)"
echo "Open a new terminal (or run: source ~/.bashrc) so 'fd' and PATH changes apply."
echo "Remember to install a Nerd Font and select it in your terminal settings."

if (( LAUNCH )) && [[ -t 1 ]]; then
  info "Launching Neovim. Wait for plugins to finish installing, then quit with :qa"
  exec nvim
else
  info "Run 'nvim' to finish the plugin installation."
fi
