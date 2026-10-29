sudo apt update && sudo apt upgrade -y
sudo apt install -y git curl wget unzip xz-utils build-essential cmake ninja-build gettext ripgrep fd-find fzf fontconfig

NVIM_VERSION="v0.11.2"
curl -LO "https://github.com/neovim/neovim/releases/download/${NVIM_VERSION}/nvim-linux-arm64.tar.gz"

sudo rm -rf /opt/nvim
sudo tar -C /opt -xzf nvim-linux-arm64.tar.gz
sudo mv /opt/nvim-linux-arm64 /opt/nvim
sudo ln -sf /opt/nvim/bin/nvim /usr/local/bin/nvim
rm nvim-linux-arm64.tar.gz

mkdir -p ~/.local/bin
ln -sf "$(command -v fdfind)" ~/.local/bin/fd
grep -q 'HOME/.local/bin' ~/.bashrc || echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc

LAZYGIT_VERSION=$(curl -s https://api.github.com/repos/jesseduffield/lazygit/releases/latest | grep -Po '"tag_name": "v\K[^"]*')
curl -Lo lazygit.tar.gz "https://github.com/jesseduffield/lazygit/releases/latest/download/lazygit_${LAZYGIT_VERSION}_Linux_arm64.tar.gz"
tar xf lazygit.tar.gz lazygit
sudo install lazygit /usr/local/bin
rm lazygit lazygit.tar.gz

mv ~/.config/nvim{,.bak} 2>/dev/null || true
mv ~/.local/share/nvim{,.bak} 2>/dev/null || true
mv ~/.local/state/nvim{,.bak} 2>/dev/null || true
mv ~/.cache/nvim{,.bak} 2>/dev/null || true

git clone https://github.com/LazyVim/starter ~/.config/nvim
rm -rf ~/.config/nvim/.git

nvim
