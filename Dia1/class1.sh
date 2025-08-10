#!/bin/bash
#Este script esat encargado de instalar los emuladores para este edtudio de systemas embdebidos
#Se instalara QEMU GCC y Make
#lo cual nos permitira instalar harware

echo "¿Estamos listos para empezar? (y/n)"
read choice

if [ "$choice" == "y" ]; then
    echo "Instalando QEMU, GCC y Make..."
    sudo apt update
    sudo apt install -y qemu qemu-system qemu-user build-essential
    echo "Instalación completada. Aquí empieza todo."
else
    echo "Me vale, aquí empezamos igual..."
    sudo apt update
    sudo apt install -y qemu qemu-system qemu-user build-essential
    echo "Instalación completada a la fuerza. Aquí empieza todo."
fi

