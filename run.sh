#!/bin/bash

# ִ�г����ֹͣ����
set -e

source env.sh

# ��������ļ���
mkdir -p ${build}
rm -rf ${build}*
mkdir -p ${build}/${iso_boot_grub}
# ����.bin�ļ�
cp -f ${cmake_out_build}/${bin} ${build}/${iso_boot}

echo 'set timeout=12' > ${build}/${iso_boot_grub}/grub.cfg
echo 'set default=0' >> ${build}/${iso_boot_grub}/grub.cfg
echo '' >> ${build}/${iso_boot_grub}/grub.cfg
echo 'menuentry "my os" {' >> ${build}/${iso_boot_grub}/grub.cfg
echo '  multiboot /boot/kernel.bin' >> ${build}/${iso_boot_grub}/grub.cfg
echo '  boot' >> ${build}/${iso_boot_grub}/grub.cfg
echo '}' >> ${build}/${iso_boot_grub}/grub.cfg


cd ${build}
grub-mkrescue --output=${iso} iso