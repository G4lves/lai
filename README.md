<div align="center">

**English**  
  
# LAI: LazyVim Automated Installation
  
### One script for installing all the requirements for working with LazyVim

[Quick start](#quick-start) . [What the script does](#what-the-script-does) . [Not included](#not-included)

</div>
<br />

> [!IMPORTANT]
> The only CPU architecture suported is aarch64/amd64 and x86_64,
>  the script it self runs a check for the architecture compatability

<br />

---

Have you just install linux and want to just run a script and have LazyVim installed, with all of the requirements, now is possible.

## Quick start
1. After downloading the repo from this repository run:
```bash
bash install-lazyvim.sh 
```
2. Enter your sudo password when asked

3. LazyVim installs its plugins the first time Neovim opens. Waint until it finishes, then quit with:
```code
:q 
```

## What the script does

- Updates the system and installs the base packages: git, curl, wget, unzip, build tools, ripgrep,fd-find,fzf and fontconfig.

- Downloads Neovim (pinned in NVIM_VERSION) and installs it in /opt/nvim, with a link in /usr/local/bin.

- Links fdfind as fd in ~/.local/bin and adds that folder to your PATH.

- Installs the latest LazyGit release.

- Moves any existing Neovim config, data, state and cache folders to .bak copies.

- Clones the LazyVim starter into ~/.config/nvim and opens Neovim.

## Not included
 
The script does not install the following. LazyVim works best with them:
 
| Item | Why it matters |
| --- | --- |
| **Nerd Font** | Needed for icons. Install one (e.g. JetBrainsMono Nerd Font) and select it in your terminal's settings. If you connect over SSH, install it on the machine you are sitting at. |
| **tree-sitter-cli** | Required by LazyVim for syntax parsing. One way to install it: `curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs \| sh`, then `source ~/.cargo/env`, then `cargo install --locked tree-sitter-cli`. |
| **Node.js, npm, Python 3 (with venv and pip)** | Mason uses these to install many language servers and formatters. |
| **xclip or wl-clipboard** | Lets Neovim copy to the system clipboard (X11 or Wayland). |
| **True-color terminal** | Most modern terminals support it (Kitty, WezTerm, Alacritty, GNOME Terminal). |



