# Security Policy

## Supported Versions

LAI has no versioned releases. Only the latest commit on the default branch is supported. Fixes are not backported, so if you are on an older copy, update first and check whether the problem still exists.

## Reporting a Vulnerability

**Do not open a public issue, pull request or discussion for a security problem.**

Use **Report a vulnerability** in the repository's **Security** tab as the preferred way to submit the report privately. Do not open a public issue to request a private contact channel.

Please include:
 
- What the problem is and what an attacker could do with it.
- The steps to reproduce it, or a proof of concept.
- Your distribution, version and CPU architecture (x86_64 or aarch64).
- The commit you tested, or the date you downloaded the script.
- Any fix or mitigation you have in mind (optional, but welcome).

The maintainer will acknowledge the report, assess its scope, and coordinate
remediation and disclosure timing with the reporter. Please allow time for
provider-dependent behavior to be reproduced safely.

## Security considerations for users
 
What the script does with your system, so you can decide whether to trust it:
 
- **It uses `sudo`.** It installs packages with `apt-get`, writes to `/opt/nvim`, and installs binaries into `/usr/local/bin`. Everything else happens as your normal user. It refuses to run as root.
- **It downloads from GitHub over HTTPS.** Neovim, LazyGit and the tree-sitter CLI come from their official GitHub release pages. The LazyVim starter is cloned from GitHub without being pinned to a specific commit.
- **Checksum checking is limited.** Neovim's checksum is verified when one is available. It comes from the same release as the download, so it protects against corrupt downloads, not against a compromised release or account. LazyGit and tree-sitter are not checksum-verified.
- **It edits your shell startup files.** It may add one `PATH` line to `~/.bashrc` (and `~/.zshrc` if you have one).
- **LazyVim runs third-party code.** On first launch it installs plugins, and Mason can download language servers and tools. These come from upstream projects, not from this repository.
Good habits:
 
- Read the script before you run it, especially anything that runs with `sudo`.
- Run it as a normal user, never as root.
- Pin versions with `NVIM_VERSION` and `LAZYGIT_VERSION` if you need reproducible installs.
- Download the script only from this repository.
## Security improvements that aren't vulnerabilities
 
If you have a hardening idea but no actual vulnerability, such as pinning versions with checksums or adding signature checks, open a normal issue or pull request. Those are welcome.
