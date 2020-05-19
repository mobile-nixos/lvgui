#!/bin/sh

set -eu

_pc() {
	includedir=${PREFIX}/include
	libdir=${PREFIX}/lib

	cat <<EOF
Name: lvgui
Description: LVGL-based GUI library
Version: ${VERSION}
Requires: ${REQUIRES}
Conflicts:

Cflags: -I${includedir}
EOF
	if [ "$STATIC" = "true" ]; then
	cat <<EOF
Libs: -L${libdir} -llvgui
EOF
	else
	cat <<EOF
Libs: -L${libdir} -Wl,-rpath,${libdir} -llvgui
EOF
	fi
}

_pc > lvgui.pc
