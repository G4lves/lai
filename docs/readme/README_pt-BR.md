<div align="center">

[English](../../README.md) · [Español](README_es.md) · **Português (Brasil)** · [Italiano](README_it.md)

# LAI: LazyVim Automated Installation

### Um único script que instala o LazyVim e tudo o que ele precisa

[Início rápido](#início-rápido) · [Opções](#opções) · [O que o script faz](#o-que-o-script-faz) · [Não incluído](#não-incluído) · [Solução de problemas](#solução-de-problemas) · [Desinstalação](#desinstalação)

</div>
<br />

> [!IMPORTANT]
> **Sistemas compatíveis:** Debian, Ubuntu e derivados (qualquer sistema que use `apt`) em
> aarch64 (arm64) e x86_64 (amd64). O script verifica as duas arquiteturas e para com uma
> mensagem clara em qualquer outro caso.
>
> Execute-o como seu usuário normal, **não** como root nem com `sudo`. O próprio script
> chama o `sudo` quando precisa.

<br />

---

Acabou de instalar o Linux e quer o LazyVim, com todos os requisitos, a partir de um único script? Agora você pode.

## Início rápido

1. Clone este repositório, entre na pasta e execute:

   ```bash
   bash scripts/install-lazyvim.sh
   ```

2. Digite sua senha de sudo quando solicitado.

3. O Neovim abre no final. O LazyVim instala seus plugins na primeira inicialização, então aguarde até terminar e depois saia com:

   ```vim
   :q
   ```

4. Para abrir o LazyVim, execute:

   ```bash
   nvim
   ```

## Opções

```bash
bash scripts/install-lazyvim.sh -y --upgrade
bash scripts/install-lazyvim.sh --yes --with-extras --no-launch
```

| Opção | Efeito |
| --- | --- |
| `-y`, `--yes` | Não pedir confirmação (necessário quando não está sendo executado em um terminal) |
| `--upgrade` | Também executa `apt-get upgrade` (desativado por padrão) |
| `--with-extras` | Também instala Node.js, npm, Python 3 (venv, pip), xclip e wl-clipboard |
| `--no-launch` | Não abrir o Neovim no final |
| `-h`, `--help` | Mostra o texto de ajuda |

Variáveis de ambiente:

| Variável | Padrão | Efeito |
| --- | --- | --- |
| `NVIM_VERSION` | `latest` | Fixa uma versão do Neovim, p. ex. `NVIM_VERSION=v0.11.2 bash scripts/install-lazyvim.sh` |
| `LAZYGIT_VERSION` | a mais recente | Fixa uma versão do LazyGit (sem o `v` inicial) |

O LazyVim precisa do Neovim 0.11.2 ou superior, então não fixe uma versão anterior.

## O que o script faz

- Instala os pacotes básicos: git, curl, wget, unzip, xz-utils, ferramentas de compilação,
  ripgrep, fd-find, fzf e fontconfig (mais os extras, se você os pediu).
- Baixa o Neovim, verifica o checksum quando a versão publica um, confere se o binário
  roda no seu sistema e só então o instala em `/opt/nvim`, com um link em `/usr/local/bin`.
- Cria o link de `fdfind` como `fd` em `~/.local/bin` e adiciona essa pasta ao seu PATH
  (`~/.bashrc` e `~/.zshrc`, se você tiver um).
- Instala a versão mais recente do LazyGit.
- Instala a CLI do tree-sitter, exigida pelo LazyVim. Se esta etapa falhar, o script avisa
  e continua; veja [Não incluído](#não-incluído) para o método manual.
- Pergunta antes de mexer em qualquer coisa e depois move as pastas de configuração, dados,
  estado e cache do Neovim que já existirem para `<nome>.bak.<timestamp>` (backups
  existentes nunca são sobrescritos).
- Clona o starter do LazyVim em `~/.config/nvim` e abre o Neovim.

Executar o script de novo é seguro: sua configuração atual passa por backup antes, o que
significa que uma nova execução **substitui sua configuração personalizada por um starter
novo**. Restaure a pasta `.bak` se não era isso que você queria.

## Não incluído

O script não instala o que segue. O LazyVim funciona melhor com isso:

| Item | Por que importa |
| --- | --- |
| **Nerd Font** | Necessária para os ícones. Instale uma (p. ex. JetBrainsMono Nerd Font) e selecione-a nas configurações do seu terminal. Se você se conecta por SSH, instale-a na máquina em que você está sentado. |
| **Terminal true-color** | A maioria dos terminais modernos suporta (Kitty, WezTerm, Alacritty, GNOME Terminal). |
| **Node.js, npm, Python 3 (com venv e pip)** | O Mason os usa para instalar muitos language servers e formatadores. Instalados com `--with-extras`. |
| **xclip ou wl-clipboard** | Permitem ao Neovim copiar para a área de transferência do sistema (X11 ou Wayland). Instalados com `--with-extras`. |

### Instalando a CLI do tree-sitter manualmente

Só é necessário se o script avisou que não conseguiu instalá-la. O LazyVim precisa de uma
versão recente, então os pacotes da distribuição costumam ser antigos demais. Uma forma de obtê-la:

```bash
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
source ~/.cargo/env
cargo install --locked tree-sitter-cli
```

## Solução de problemas

| Problema | Solução |
| --- | --- |
| Os ícones aparecem como quadrados vazios | Instale uma Nerd Font e selecione-a nas configurações do seu terminal. |
| `fd: command not found` | Abra um novo terminal ou execute `source ~/.bashrc`. |
| Erros sobre parsers ou tree-sitter | Verifique `tree-sitter --version`; se não estiver instalado, veja a seção acima. |
| Algo mais parece errado | Execute `:checkhealth lazyvim` dentro do Neovim. |
| "Downloaded Neovim doesn't run" | A glibc da sua distribuição provavelmente é antiga demais para o binário pré-compilado. Use uma versão mais recente da sua distribuição. |

## Desinstalação

```bash
sudo rm -rf /opt/nvim
sudo rm -f /usr/local/bin/nvim /usr/local/bin/lazygit /usr/local/bin/tree-sitter
rm -rf ~/.config/nvim ~/.local/share/nvim ~/.local/state/nvim ~/.cache/nvim
```

Para restaurar sua configuração anterior, renomeie de volta as pastas `*.bak.<timestamp>`
correspondentes depois. A linha de `PATH` que o script adicionou ao `~/.bashrc` é inofensiva
e pode ficar.
