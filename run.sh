#!/bin/bash

# 执行出错就停止运行
set -e

source env.sh

# 创建输出文件夹
mkdir -p ${build}
rm -rf ${build}*
mkdir -p ${build}/${iso_boot_grub}
# 拷贝.bin文件
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