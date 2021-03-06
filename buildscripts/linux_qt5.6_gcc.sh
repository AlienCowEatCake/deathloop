#!/bin/bash
PROJECT="deathloop"
BUILDDIR="build_linux_qt5.6_gcc"
SUFFIX="_qt5.6_$(gcc -dumpmachine)"

QTDIR="/opt/qt-5.6.2-static"
CMD_QMAKE="${QTDIR}/bin/qmake"

cd "$(dirname $0)"/..
rm -rf "${BUILDDIR}"
mkdir -p "${BUILDDIR}"
cd "${BUILDDIR}"
${CMD_QMAKE} CONFIG+="release" "../${PROJECT}.pro"
make
strip --strip-all "${PROJECT}"
cp -a "${PROJECT}" ../"${PROJECT}${SUFFIX}.elf"

