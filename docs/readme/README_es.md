<div align="center">

[English](../../README.md) · **Español** · [Português (Brasil)](README_pt-BR.md) · [Italiano](README_it.md)

# LAI: LazyVim Automated Installation

### Un solo script que instala LazyVim y todo lo que necesita

[Inicio rápido](#inicio-rápido) · [Opciones](#opciones) · [Qué hace el script](#qué-hace-el-script) · [No incluido](#no-incluido) · [Solución de problemas](#solución-de-problemas) · [Desinstalación](#desinstalación)

</div>
<br />

> [!IMPORTANT]
> **Sistemas compatibles:** Debian, Ubuntu y derivados (cualquier sistema que use `apt`) en
> aarch64 (arm64) y x86_64 (amd64). El script comprueba ambas arquitecturas y se detiene con
> un mensaje claro en cualquier otro caso.
>
> Ejecútalo como tu usuario normal, **no** como root ni con `sudo`. El propio script llama
> a `sudo` cuando lo necesita.

<br />

---

¿Acabas de instalar Linux y quieres LazyVim, con todos sus requisitos, a partir de un único script? Ahora puedes.

## Inicio rápido

1. Clona este repositorio, entra en la carpeta y ejecuta:

   ```bash
   bash scripts/install-lazyvim.sh
   ```

2. Introduce tu contraseña de sudo cuando se te pida.

3. Neovim se abre al final. LazyVim instala sus plugins en el primer arranque, así que espera a que termine y luego sal con:

   ```vim
   :q
   ```

4. Para abrir LazyVim ejecuta:

   ```bash
   nvim
   ```

## Opciones

```bash
bash scripts/install-lazyvim.sh -y --upgrade
bash scripts/install-lazyvim.sh --yes --with-extras --no-launch
```

| Opción | Efecto |
| --- | --- |
| `-y`, `--yes` | No pedir confirmación (necesario cuando no se ejecuta en una terminal) |
| `--upgrade` | Ejecuta además `apt-get upgrade` (desactivado por defecto) |
| `--with-extras` | Instala además Node.js, npm, Python 3 (venv, pip), xclip y wl-clipboard |
| `--no-launch` | No abrir Neovim al final |
| `-h`, `--help` | Muestra el texto de ayuda |

Variables de entorno:

| Variable | Por defecto | Efecto |
| --- | --- | --- |
| `NVIM_VERSION` | `latest` | Fija una versión de Neovim, p. ej. `NVIM_VERSION=v0.11.2 bash scripts/install-lazyvim.sh` |
| `LAZYGIT_VERSION` | la última | Fija una versión de LazyGit (sin la `v` inicial) |

LazyVim necesita Neovim 0.11.2 o superior, así que no fijes una versión anterior.

## Qué hace el script

- Instala los paquetes base: git, curl, wget, unzip, xz-utils, herramientas de compilación,
  ripgrep, fd-find, fzf y fontconfig (más los extras si los has pedido).
- Descarga Neovim, verifica su checksum cuando la versión publica uno, comprueba que el
  binario funciona en tu sistema y solo entonces lo instala en `/opt/nvim` con un enlace
  en `/usr/local/bin`.
- Enlaza `fdfind` como `fd` en `~/.local/bin` y añade esa carpeta a tu PATH
  (`~/.bashrc`, y `~/.zshrc` si tienes uno).
- Instala la última versión de LazyGit.
- Instala la CLI de tree-sitter, que LazyVim requiere. Si este paso falla, el script te
  avisa y continúa; consulta [No incluido](#no-incluido) para la instalación manual.
- Pregunta antes de tocar nada y luego mueve las carpetas de configuración, datos, estado y
  caché de Neovim que ya existan a `<nombre>.bak.<marca-de-tiempo>` (las copias de seguridad
  existentes nunca se sobrescriben).
- Clona el starter de LazyVim en `~/.config/nvim` y abre Neovim.

Volver a ejecutar el script es seguro: primero se hace una copia de seguridad de tu
configuración actual, lo que significa que una nueva ejecución **reemplaza tu configuración
personalizada por un starter limpio**. Restaura la carpeta `.bak` si no era lo que querías.

## No incluido

El script no instala lo siguiente. LazyVim funciona mejor con ello:

| Elemento | Por qué es importante |
| --- | --- |
| **Nerd Font** | Necesaria para los iconos. Instala una (p. ej. JetBrainsMono Nerd Font) y selecciónala en los ajustes de tu terminal. Si te conectas por SSH, instálala en la máquina en la que estás sentado. |
| **Terminal true-color** | La mayoría de las terminales modernas lo soportan (Kitty, WezTerm, Alacritty, GNOME Terminal). |
| **Node.js, npm, Python 3 (con venv y pip)** | Mason los usa para instalar muchos servidores de lenguaje y formateadores. Los instala `--with-extras`. |
| **xclip o wl-clipboard** | Permiten a Neovim copiar al portapapeles del sistema (X11 o Wayland). Los instala `--with-extras`. |

### Instalar la CLI de tree-sitter manualmente

Solo es necesario si el script avisó de que no pudo instalarla. LazyVim necesita una versión
reciente, así que los paquetes de la distribución suelen ser demasiado antiguos. Una forma de obtenerla:

```bash
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
source ~/.cargo/env
cargo install --locked tree-sitter-cli
```

## Solución de problemas

| Problema | Solución |
| --- | --- |
| Los iconos aparecen como cuadros vacíos | Instala una Nerd Font y selecciónala en los ajustes de tu terminal. |
| `fd: command not found` | Abre una terminal nueva o ejecuta `source ~/.bashrc`. |
| Errores sobre parsers o tree-sitter | Comprueba `tree-sitter --version`; si no está instalado, consulta la sección anterior. |
| Algo más parece ir mal | Ejecuta `:checkhealth lazyvim` dentro de Neovim. |
| "Downloaded Neovim doesn't run" | Probablemente la glibc de tu distribución es demasiado antigua para el binario precompilado. Usa una versión más reciente de tu distribución. |

## Desinstalación

```bash
sudo rm -rf /opt/nvim
sudo rm -f /usr/local/bin/nvim /usr/local/bin/lazygit /usr/local/bin/tree-sitter
rm -rf ~/.config/nvim ~/.local/share/nvim ~/.local/state/nvim ~/.cache/nvim
```

Para restaurar tu configuración anterior, vuelve a renombrar después las carpetas
`*.bak.<marca-de-tiempo>` correspondientes. La línea de `PATH` que el script añadió a
`~/.bashrc` es inofensiva y puede quedarse.
