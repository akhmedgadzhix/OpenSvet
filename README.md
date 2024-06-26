# A step-by-step guide

**ATTENTION!**</br>
This guide is for a specific machine with specific components. The only reason the repository is not marked as private is for cloning convenience!
## 1. LiveISO
```bash
    # Partitions
    mkfs.fat -F 32 /dev/nvme0n1p1
    mkfs.ext4 /dev/nvme0n1p2
    mkfs.ext4 /dev/vg_raid0/raiden
    mkdir -p /mnt/{boot/efi,home}
    mount /dev/nvme0n1p2 /mnt
    mount /dev/nvme0n1p1 /mnt/boot/efi
    mount /dev/vg_raid0/raiden /mnt/home
```

```bash
    # SWAP-file
    cd /mnt/home && fallocate -l 24G .swapfile
    dd if=/dev/zero of=.swapfile bs=1G count=24 status=progress
    chmod 600 .swapfile
    mkswap .swapfile
    swapon .swapfile
```

```bash
    # Pacman Config
    sudo sed -i -e 's/#ParallelDownloads = [0-9]*/ParallelDownloads = 10/' \
        -e '/#\[multilib\]/s/^#//' -e '/#\[multilib\]/{n;s/^#//}' /etc/pacman.conf

```

```bash
    # Install packages
    pacstrap /mnt base linux linux-firmware linux-headers sudo dhcpcd lvm2 \
        vim nano btop htop fastfetch iwd samba openssh git base-devel zsh \
        && cp /etc/pacman.conf /mnt/etc/pacman.conf
```

```bash
    # Generate FSTAB & enter to system
    genfstab -U /mnt >> /mnt/etc/fstab
    arch-chroot /mnt
```

## Chroot
```bash
    # OpenSSH
    sudo sed -i 's/^#\s*PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config
```

```bash
    # Setup sudoers & add users
    sudo sed -i 's/^# %wheel ALL=(ALL:ALL) NOPASSWD: ALL/%wheel ALL=(ALL:ALL) NOPASSWD: ALL/' /etc/sudoers
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
    grub-install --efi-directory=/boot/efi
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

After logging in, enable time synchronization just in case
```bash
    # NTP sinc
    sudo timedatectl set-ntp 1
```

```bash
    # AMD GPU drivers
    sudo pacman -S amdvlk mesa vulkan-radeon libva-mesa-driver \
    lib32-vulkan-radeon lib32-amdvlk xf86-video-amdgpu
```

```bash
    # X11 install
    sudo pacman -S xorg xorg-server xorg-xinit xorg-xrandr
    echo -e "exec sxhkd &\nexec numlockx\nexec bspwm" > .xinitrc
```

All configuration files for the user will be located in the 'userconf' directory of this repository

```bash
    # Cloning this repository into the system
    git clone https://github.com/akhmedgadzhix/OpenSvet.git
```

```bash
    # BSPWM setup
    sudo pacman -S bspwm sxhkd polybar alacritty picom
    mkdir -p .config/{sxhkd,bspwm,polybar,picom} # !
    cp OpenSvet/userconf/bspwmrc .config/bspwm/ && chmod +x .config/bspwm/bspwmrc # !
    cp OpenSvet/userconf/sxhkdrc .config/sxhkd/ # !
    cp OpenSvet/userconf/picom.conf .config/picom/ # !
    cp OpenSvet/userconf/alacritty.yml .config/alacritty/ && alacritty migrate # !
    cp OpenSvet/userconf/barconfig .config/polybar/config.ini # !
    # All commands labeled '# !' will be replaced by the script:
    #chmod +x OpenSvet/userconf.sh && ./OpenSvet/userconf.sh
```