# Contributing to LAI

Thanks for wanting to help, LAI. Fixes and improvements of any size are welcome, from a typo in a translation to a change in the installer itself.

## Ways to contribute

- **Report a bug.** The script failed, or did something you didn't expect.
- **Suggest an improvement.** A missing check, a clearer message, a better default.
- **Improve the docs or translations.** See [Documentation and translations](#documentation-and-translations).
- **Send a code change.** See [Making a change](#making-a-change).

**Security problems are different.** Don't open a public issue for a vulnerability. Follow [SECURITY.md](SECURITY.md) instead.

## Reporting a bug

Before opening an issue, search the existing ones, then read the Troubleshooting section of the README. If it's still unsolved, open an issue with:

- Your distribution and version (`cat /etc/os-release`) and architecture (`uname -m`).
- The exact command you ran, including any options or environment variables.
- The full output of the script, from the start to the failure. Copy it as text, not a screenshot.
- What you expected to happen, and what happened instead.
- Whether it's the first run or a re-run, and whether you already had a Neovim config.

## Suggesting a change

Open an issue before you start on anything big, so we can agree on the approach first, small fixes can go straight to a pull request.

### What fits the project

- **One script, minimal dependencies.** The installer should keep working on a fresh Debian or Ubuntu system with nothing but `apt` and `sudo`.
- **Conservative defaults.** The default run should not do anything surprising. For example, a full `apt-get upgrade` is opt-in (`--upgrade`), and so are the extras (`--with-extras`). New optional behaviour should go behind a flag.
- **Never destroy user data.** Existing Neovim folders are backed up under a name that doesn't exist yet, and the script asks first.
- **x86_64 and aarch64 stay in step.** A change must work on both architectures.

## Making a change

1. Fork the repository and create a branch from the default branch.
2. Make your change. Keep each pull request to one topic.
3. Open a pull request.

### Script guidelines

- Write Bash, keep `set -Eeuo pipefail`, and quote your variables.
- Check the result of a download or install before moving on. Optional steps, such as the tree-sitter CLI, should warn and continue rather than abort.
- Use `apt-get` (not `apt`) in scripts.
- Messages go through `info`, `warn` and `die`. Errors and warnings go to stderr.
- New external downloads must use HTTPS and come from an official source. Verify a checksum when the project publishes one.

### Pull request description

Explain **what** changed, **why**, and **how you tested it**. Link the issue it fixes, if any.

### Commit messages

Use a short, imperative subject line of about 72 characters or fewer, such as "Verify the Neovim checksum before installing". Add a body when the reason isn't obvious.

## Documentation and translations

The README exists in four languages:

| Language | File |
| --- | --- |
| English | `README.md` |
| Español | `docs/readme/README_es.md` |
| Português (Brasil) | `docs/readme/README_pt-BR.md` |
| Italiano | `docs/readme/README_it.md` |

If your change affects behaviour, options or requirements, update `README.md` and, as far as you can, the translations. If you can't translate, say so in the pull request, and the translations will be updated afterwards. Corrections from native speakers are always welcome.

When translating:

- Leave commands, options, environment variables and the project name untranslated. The script's own messages are in English, so any message quoted in the docs stays in English.
- Keep the structure the same across languages, so the files stay easy to compare.
- The links in the table of contents point at heading anchors, which GitHub builds from the heading text. If you translate a heading, update the matching link too.

### Adding a new language

1. Copy `README.md` to `docs/readme/README_<code>.md`, for example `README_fr.md`, and translate it.
2. Add the new language to the language switcher at the top of **every** README. The root `README.md` links to `docs/readme/README_xx.md`, and the translated files link to `README_xx.md` and `../../README.md`.
3. Mark the current language in bold in its own file's switcher.
