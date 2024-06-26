#!/bin/bash
# Creating an array with "source:destination_item" pairs
mkdir -p .config/{sxhkd,bspwm,polybar,picom}
declare -A files=(
    ["bspwmrc"]=".config/bspwm/"
    ["sxhkdrc"]=".config/sxhkd/"
    ["picom.conf"]=".config/picom/"
    ["alacritty.yml"]=".config/alacritty/"
    ["barconfig"]=".config/polybar/config.ini"
)

# Searches all array elements and performs copy operations
for src in "${!files[@]}"; do
    dest=${files[$src]}
    cp "~/OpenSvet/userconf/$src" "$dest"
    # If it is a bspwmrc file, make it executable
    if [[ $src == "bspwmrc" ]]; then
        chmod +x "${dest}${src}"
    fi
done

# Execute the migration command for alacritty after copying
alacritty migrate