#
# This file is the datagenerator recipe.
#

SUMMARY = "Simple generator application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"


SRC_URI = "file://config \
	   file://main \
	   file://script_input \
		  "

S = "${WORKDIR}"

#RDEPENDS:${PN} += "bash glibc libm"


do_install() {
	     install -d ${D}/usr/bin/
	     install -m 0755 ${WORKDIR}/config ${D}/usr/bin/
    	     install -m 0755 ${WORKDIR}/main ${D}/usr/bin/
             install -m 0755 ${WORKDIR}/script_input ${D}/usr/bin/
}

DEPENDS += "bash"
RDEPENDS:${PN} += "bash"
