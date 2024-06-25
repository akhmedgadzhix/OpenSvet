# Пошаговое руководство для себя
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
