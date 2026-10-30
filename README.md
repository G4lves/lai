<div align="center">

**English** · [Español](docs/readme/README_es.md) · [Português (Brasil)](docs/readme/README_pt-BR.md) · [Italiano](docs/readme/README_it.md) 
  
# LAI: LazyVim Automated Installation
  
### One script that installs LazyVim and everything it needs

[Quick start](#quick-start) · [Options](#options) · [What the script does](#what-the-script-does) · [Not included](#not-included) · [Troubleshooting](#troubleshooting) · [Uninstall](#uninstall)

</div>
<br />

> [!IMPORTANT]
> **Support systms:** Debian, Ubuntu and derivatives (anything that uses 'apt') on
> aarch64 (arm64) and x86_64 (amd64). The script check both and stops with a clear message on anything else.
> 
> Run it as your normal user, **not** as root or with 'sudo' itself when it needs to.

<br />

---

Just installed Linux and want LazyVim, with all of its requirements, from a single script? Now you can.

## Quick start
1. Clone this repository and run:
```bash
bash install-lazyvim.sh 
```
2. Enter your sudo password when asked

3. Neovim opens at the end. LazyVim installs its plugins on first launch, so wait until it finishes, then quit with:
```code
:q 
```
4. To open LazyVim run:
```bash
nvim
```

## Options
```bash
bash install-lazyvim.sh -y --upgrade 
bash scripts/install-lazyvim.sh --yes --with-extras --no-launch
```

| Option | Effect |
| --- | --- |
| `-y`, `--yes` | Don't ask for confirmation (required when not running in a terminal) |
| `--upgrade` | Also run `apt-get upgrade` (off by default) |
| `--with-extras` | Also install Node.js, npm, Python 3 (venv, pip), xclip and wl-clipboard |
| `--no-launch` | Don't open Neovim at the end |
| `-h`, `--help` | Show the help text |

Environment variables:
 
| Variable | Default | Effect |
| --- | --- | --- |
| `NVIM_VERSION` | `latest` | Pin a Neovim release, e.g. `NVIM_VERSION=v0.11.2 bash scripts/install-lazyvim.sh` |
| `LAZYGIT_VERSION` | latest | Pin a LazyGit version (without the leading `v`) |
 
LazyVim needs Neovim 0.11.2 or newer, so don't pin anything older than that.

## What the script does
 
- Installs the base packages: git, curl, wget, unzip, xz-utils, build tools,
  ripgrep, fd-find, fzf and fontconfig (plus the extras if you asked for them).
- Downloads Neovim, verifies its checksum when the release publishes one, checks that
  the binary runs on your system, and only then installs it in `/opt/nvim` with a link
  in `/usr/local/bin`.
- Links `fdfind` as `fd` in `~/.local/bin` and adds that folder to your PATH
  (`~/.bashrc`, and `~/.zshrc` if you have one).
- Installs the latest LazyGit release.
- Installs the tree-sitter CLI, which LazyVim requires. If this step fails, the script
  warns you and carries on; see [Not included](#not-included) for the manual route.
- Asks before touching anything, then moves any existing Neovim config, data, state and
  cache folders to `<name>.bak.<timestamp>` (existing backups are never overwritten).
- Clones the LazyVim starter into `~/.config/nvim` and opens Neovim.

Re-running the script is safe: your current config is backed up first, which means a
re-run **replaces your customised config with a fresh starter**. Restore the `.bak`
folder if you didn't mean to.

## Not included
 
The script does not install the following. LazyVim works best with them:
 
| Item | Why it matters |
| --- | --- |
| **Nerd Font** | Needed for icons. Install one (e.g. JetBrainsMono Nerd Font) and select it in your terminal's settings. If you connect over SSH, install it on the machine you are sitting at. |
| **True-color terminal** | Most modern terminals support it (Kitty, WezTerm, Alacritty, GNOME Terminal). |
| **Node.js, npm, Python 3 (with venv and pip)** | Mason uses these to install many language servers and formatters. Installed by `--with-extras`. |
| **xclip or wl-clipboard** | Lets Neovim copy to the system clipboard (X11 or Wayland). Installed by `--with-extras`. |

### Installing the tree-sitter CLI manually
 
Only needed if the script warned that it couldn't install it. LazyVim needs a recent
version, so distro packages are often too old. One way to get it:
 
```bash
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
source ~/.cargo/env
cargo install --locked tree-sitter-cli
```
 
## Troubleshooting
 
| Problem | Fix |
| --- | --- |
| Icons show as empty boxes | Install a Nerd Font and select it in your terminal settings. |
| `fd: command not found` | Open a new terminal, or run `source ~/.bashrc`. |
| Errors about parsers or tree-sitter | Check `tree-sitter --version`; if it's missing, see above. |
| Something else looks off | Run `:checkhealth lazyvim` inside Neovim. |
| "Downloaded Neovim doesn't run" | Your distro's glibc is probably too old for the prebuilt binary. Use a newer release of your distro. |
 
## Uninstall
 
```bash
sudo rm -rf /opt/nvim
sudo rm -f /usr/local/bin/nvim /usr/local/bin/lazygit /usr/local/bin/tree-sitter
rm -rf ~/.config/nvim ~/.local/share/nvim ~/.local/state/nvim ~/.cache/nvim
```
 
To restore your previous setup, rename the matching `*.bak.<timestamp>` folders back
afterwards. The `PATH` line the script added to `~/.bashrc` is harmless and can stay.
