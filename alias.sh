#!/bin/bash

# Путь к файлу .zshrc
ZSHRC="$HOME/.zshrc"

# Добавление пустой строки и "# Aliases" в конец файла
echo "" >> "$ZSHRC"
echo "# Aliases" >> "$ZSHRC"

# Добавление алиасов
echo "alias poweroff=\"sudo poweroff\"" >> "$ZSHRC"
echo "alias reboot=\"sudo reboot\"" >> "$ZSHRC"
echo "alias pacman=\"sudo pacman\"" >> "$ZSHRC"
echo "alias cp=\"copic\"" >> "$ZSHRC"