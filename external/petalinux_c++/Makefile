include objdir.gmk
include generic.paths.gmk

build:
	@mkdir -p ${LIBDIR}
	cp "$$PETALINUX"/tools/linux-i386/arm-xilinx-linux-gnueabi/arm-xilinx-linux-gnueabi/libc/usr/lib/libstdc++.so.6 ${LIBDIR}/

install:
	cp ${LIBDIR}/libstdc++.so.6 ${DESTDIR}/${PREFIX}/lib/

pkg_name=petalinux_c++
include package.gmk

