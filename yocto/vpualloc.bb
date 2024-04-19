OSS_CLASS = "${OSS_FOSSOLOGY}"
SUMMARY = "vpualloc"
DESCRIPTION = "VPU allocation test"
HOMEPAGE = "https://github.com/cspiel1/vpualloc"
LICENSE = "BSD"
PR = "r1"
PV = "git-${SRCPV}"
LIC_FILES_CHKSUM = "file://LICENSE;md5=38b5a22bb376712b7c88b52d9429eb06"

DEPENDS = "imx-vpuwrap"

SRCREV = "${AUTOREV}"
SRC_URI = "git://github.com/cspiel1/vpualloc.git;protocol=https;branch=main \
	"
inherit pkgconfig cmake

S = "${WORKDIR}/git"

BUILDHISTORY_CHECKVERBACKWARDS = "0"

EXTRA_OEMAKE += " \
	-C ${S} \
	"
