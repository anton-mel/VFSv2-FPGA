petalinux-config --get-hw-description ../../../hw_vfs_v2/mult/design_1_wrapper.xsa
petalinux-build
petalinux-package boot --u-boot --force
