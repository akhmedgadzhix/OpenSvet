# A step-by-step guide

**ATTENTION!**</br>
This guide is for a specific machine with specific components. The only reason the repository is not marked as private is for cloning convenience!
## 1. LiveISO
If the partitions have been deleted, execute the following commands (otherwise, ignore)
```bash
    echo -e "g\nn\n\n\n+1G\nt\n1\nn\n\n\n\nw" | fdisk /dev/nvme0n1
    echo -e "g\nn\n\n\n\nw" | fdisk /dev/sda
    echo -e "g\nn\n\n\n\nw" | fdisk /dev/sdb
    pvcreate /dev/sda1 /dev/sdb1
    vgcreate vg_raid0 /dev/sda1 /dev/sdb1
    lvcreate -i 2 -I 64 -l 100%FREE -n raiden vg_raid0
```

```bash
    # Partitions
    mkfs.fat -F 32 /dev/nvme0n1p1
    mkfs.ext4 /dev/nvme0n1p2
    mkfs.ext4 /dev/vg_raid0/raiden
    mount /dev/nvme0n1p2 /mnt
    mkdir -p /mnt/{boot/efi,home}
    mount /dev/nvme0n1p1 /mnt/boot/efi
    mount /dev/vg_raid0/raiden /mnt/home
```

```bash
    # SWAP-file
    cd /mnt/home && fallocate -l 24G .swapfile
    dd if=/dev/zero of=/mnt/home/.swapfile bs=1G count=24 status=progress
    chmod 600 /mnt/home/.swapfile
    mkswap /mnt/home/.swapfile
    swapon /mnt/home/.swapfile && cd
```

```bash
    # Pacman Config
    sed -i -e 's/^#ParallelDownloads = 5/ParallelDownloads = 10/' -e '/^#\[multilib\]/{N;s/#\[multilib\]\n#/[multilib]\n/}' /etc/pacman.conf
```

```bash
    # Install packages
    pacstrap /mnt base linux linux-firmware linux-headers sudo dhcpcd lvm2 \
    vim nano btop htop fastfetch iwd samba openssh git base-devel zsh
    cp /etc/pacman.conf /mnt/etc/pacman.conf
```

```bash
    # Generate FSTAB & enter to system
    genfstab -U /mnt >> /mnt/etc/fstab
    arch-chroot /mnt
```

## Chroot
```bash
    # OpenSSH
    sed -i 's/^#\s*PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config
```

```bash
    # Setup sudoers & add users
    sed -i 's/^# %wheel ALL=(ALL:ALL) NOPASSWD: ALL/%wheel ALL=(ALL:ALL) NOPASSWD: ALL/' /etc/sudoers
    useradd -mG wheel -g users -s /bin/zsh statix
    echo 'statix:1234' | sudo chpasswd
    echo 'root:1234' | sudo chpasswd
```

```bash
    # Setup Autostart Services (systemctl)
    systemctl enable sshd dhcpcd iwd
```

```bash
    # Clock
    ln -sf /usr/share/zoneinfo/Europe/Moscow /etc/localtime
    hwclock --systohc
```

```bash
    # Install GRUB
    pacman -Syy grub efibootmgr
    grub-install --target=x86_64-efi --efi-directory=/boot/efi
    grub-mkconfig -o /boot/grub/grub.cfg
```

```bash
    # End
    exit
```

## Reboot
```bash
    # Unmount /mnt & reboot
    umount -R /mnt
    reboot
```
## First Boot

```bash
    # AMD GPU drivers
    sudo pacman -S amdvlk mesa vulkan-radeon libva-mesa-driver \
    lib32-vulkan-radeon lib32-amdvlk xf86-video-amdgpu
```
```bash
    # NVidia GPUs
    sudo pacman -Syu nvidia nvidia-utils vulkan-icd-loader lib32-nvidia-utils lib32-vulkan-icd-loader opencl-nvidia lib32-opencl-nvidia
```
```bash
    # X11 install
    sudo pacman -S xorg xorg-server xorg-xinit xorg-xrandr
    echo -e "exec sxhkd &\nexec bspwm" > .xinitrc
```

All configuration files for the user will be located in the 'userconf' directory of this repository

```bash
    # Cloning this repository into the system
    git clone https://github.com/akhmedgadzhix/OpenSvet.git
```

```bash
    # BSPWM setup
    sudo pacman -S bspwm sxhkd polybar alacritty picom
    mkdir -p .config/{sxhkd,bspwm,polybar,picom,alacritty} # !
    cp OpenSvet/userconf/bspwmrc .config/bspwm/ && chmod +x .config/bspwm/bspwmrc # !
    cp OpenSvet/userconf/sxhkdrc .config/sxhkd/ # !
    cp OpenSvet/userconf/picom.conf .config/picom/ # !
    cp OpenSvet/userconf/alacritty.yml .config/alacritty/ && alacritty migrate # !
    cp OpenSvet/userconf/barconfig .config/polybar/config.ini # !
    # All commands labeled '# !' will be replaced by the script:
    #chmod +x OpenSvet/userconf.sh && ./OpenSvet/userconf.sh
```

```bash
    # Font Installation
    sudo pacman -Syy ttf-ibm-plex
```

```bash
    # Root Configs
    sudo cp OpenSvet/rootconf/smb.conf /etc/samba
    sudo cp OpenSvet/rootconf/makepkg.conf /etc/ && sudo pacman -Sy ccache rust
```

```bash
    # Paru Installation
    git clone https://aur.archlinux.org/paru && cd paru
    makepkg -si && cd .. && rm -dfR paru
```

```bash
    # ZSH Configuration
    sh -c "$(curl -fsSL https://raw.github.com/ohmyzsh/ohmyzsh/master/tools/install.sh)"
    git clone https://github.com/zsh-users/zsh-autosuggestions ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-autosuggestions
    git clone https://github.com/zsh-users/zsh-syntax-highlighting.git ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-syntax-highlighting
    sed -i 's/plugins=(git)/plugins=(git zsh-autosuggestions zsh-syntax-highlighting)/' ~/.zshrc
    git clone --depth 1 https://github.com/junegunn/fzf.git ~/.fzf
    ~/.fzf/install
    sed -i 's/ZSH_THEME="robbyrussell"/ZSH_THEME="af-magic"/' ~/.zshrc 
    echo "setopt CORRECT" >> ~/.zshrc && exec zsh
```

```bash
    # Aliases
    chmod +x ~/OpenSvet/alias.sh && ~/OpenSvet/alias.sh
    exec zsh
```