<div align="center">

[English](../../README.md) · [Español](README_es.md) · [Português (Brasil)](README_pt-BR.md) · **Italiano**

# LAI: LazyVim Automated Installation

### Un unico script che installa LazyVim e tutto ciò di cui ha bisogno

[Avvio rapido](#avvio-rapido) · [Opzioni](#opzioni) · [Cosa fa lo script](#cosa-fa-lo-script) · [Non incluso](#non-incluso) · [Risoluzione dei problemi](#risoluzione-dei-problemi) · [Disinstallazione](#disinstallazione)

</div>
<br />

> [!IMPORTANT]
> **Sistemi supportati:** Debian, Ubuntu e derivate (qualsiasi sistema che usa `apt`) su
> aarch64 (arm64) e x86_64 (amd64). Lo script controlla entrambe le architetture e si ferma
> con un messaggio chiaro in tutti gli altri casi.
>
> Eseguilo come utente normale, **non** come root né con `sudo`. Sarà lo script stesso a
> chiamare `sudo` quando serve.

<br />

---

Hai appena installato Linux e vuoi LazyVim, con tutti i suoi requisiti, da un unico script? Ora puoi.

## Avvio rapido

1. Clona questo repository, entra nella cartella ed esegui:

   ```bash
   bash scripts/install-lazyvim.sh
   ```

2. Inserisci la password di sudo quando richiesto.

3. Neovim si apre alla fine. LazyVim installa i suoi plugin al primo avvio, quindi aspetta che finisca, poi esci con:

   ```vim
   :q
   ```

4. Per aprire LazyVim esegui:

   ```bash
   nvim
   ```

## Opzioni

```bash
bash scripts/install-lazyvim.sh -y --upgrade
bash scripts/install-lazyvim.sh --yes --with-extras --no-launch
```

| Opzione | Effetto |
| --- | --- |
| `-y`, `--yes` | Non chiede conferma (necessario quando non viene eseguito in un terminale) |
| `--upgrade` | Esegue anche `apt-get upgrade` (disattivato di default) |
| `--with-extras` | Installa anche Node.js, npm, Python 3 (venv, pip), xclip e wl-clipboard |
| `--no-launch` | Non apre Neovim alla fine |
| `-h`, `--help` | Mostra il testo di aiuto |

Variabili d'ambiente:

| Variabile | Predefinito | Effetto |
| --- | --- | --- |
| `NVIM_VERSION` | `latest` | Fissa una versione di Neovim, ad es. `NVIM_VERSION=v0.11.2 bash scripts/install-lazyvim.sh` |
| `LAZYGIT_VERSION` | l'ultima | Fissa una versione di LazyGit (senza la `v` iniziale) |

LazyVim richiede Neovim 0.11.2 o successivo, quindi non fissare una versione precedente.

## Cosa fa lo script

- Installa i pacchetti di base: git, curl, wget, unzip, xz-utils, strumenti di compilazione,
  ripgrep, fd-find, fzf e fontconfig (più gli extra, se li hai richiesti).
- Scarica Neovim, ne verifica il checksum quando la release ne pubblica uno, controlla che
  il binario funzioni sul tuo sistema e solo dopo lo installa in `/opt/nvim` con un link
  in `/usr/local/bin`.
- Collega `fdfind` come `fd` in `~/.local/bin` e aggiunge quella cartella al tuo PATH
  (`~/.bashrc`, e `~/.zshrc` se ne hai uno).
- Installa l'ultima release di LazyGit.
- Installa la CLI di tree-sitter, richiesta da LazyVim. Se questo passaggio fallisce, lo
  script ti avvisa e continua; vedi [Non incluso](#non-incluso) per la procedura manuale.
- Chiede conferma prima di toccare qualsiasi cosa, poi sposta le cartelle di configurazione,
  dati, stato e cache di Neovim già esistenti in `<nome>.bak.<timestamp>` (i backup esistenti
  non vengono mai sovrascritti).
- Clona lo starter di LazyVim in `~/.config/nvim` e apre Neovim.

Rieseguire lo script è sicuro: la configurazione attuale viene prima salvata in backup, il che
significa che una nuova esecuzione **sostituisce la tua configurazione personalizzata con uno
starter nuovo**. Ripristina la cartella `.bak` se non era quello che volevi.

## Non incluso

Lo script non installa quanto segue. LazyVim funziona meglio con questi elementi:

| Elemento | Perché è importante |
| --- | --- |
| **Nerd Font** | Necessario per le icone. Installane uno (ad es. JetBrainsMono Nerd Font) e selezionalo nelle impostazioni del terminale. Se ti colleghi via SSH, installalo sul computer a cui sei seduto. |
| **Terminale true-color** | La maggior parte dei terminali moderni lo supporta (Kitty, WezTerm, Alacritty, GNOME Terminal). |
| **Node.js, npm, Python 3 (con venv e pip)** | Mason li usa per installare molti language server e formatter. Installati da `--with-extras`. |
| **xclip o wl-clipboard** | Permettono a Neovim di copiare negli appunti di sistema (X11 o Wayland). Installati da `--with-extras`. |

### Installare la CLI di tree-sitter manualmente

Serve solo se lo script ha avvisato di non essere riuscito a installarla. LazyVim richiede una
versione recente, quindi i pacchetti della distribuzione sono spesso troppo vecchi. Un modo per ottenerla:

```bash
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
source ~/.cargo/env
cargo install --locked tree-sitter-cli
```

## Risoluzione dei problemi

| Problema | Soluzione |
| --- | --- |
| Le icone appaiono come riquadri vuoti | Installa un Nerd Font e selezionalo nelle impostazioni del terminale. |
| `fd: command not found` | Apri un nuovo terminale oppure esegui `source ~/.bashrc`. |
| Errori su parser o tree-sitter | Controlla `tree-sitter --version`; se manca, vedi la sezione sopra. |
| Qualcos'altro non va | Esegui `:checkhealth lazyvim` dentro Neovim. |
| "Downloaded Neovim doesn't run" | Probabilmente la glibc della tua distribuzione è troppo vecchia per il binario precompilato. Usa una versione più recente della tua distribuzione. |

## Disinstallazione

```bash
sudo rm -rf /opt/nvim
sudo rm -f /usr/local/bin/nvim /usr/local/bin/lazygit /usr/local/bin/tree-sitter
rm -rf ~/.config/nvim ~/.local/share/nvim ~/.local/state/nvim ~/.cache/nvim
```

Per ripristinare la configurazione precedente, rinomina di nuovo le cartelle `*.bak.<timestamp>`
corrispondenti. La riga `PATH` che lo script ha aggiunto a `~/.bashrc` è innocua e può restare.
