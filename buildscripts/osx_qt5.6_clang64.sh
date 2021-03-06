#!/bin/bash
PROJECT=deathloop
BUILDDIR=build_osx_qt5.6_clang64
APPNAME="DeathLoop"
DMGNAME="${PROJECT}_qt5.6_clang64"

QTDIR="/opt/Qt/5.6/clang_64"
CMD_QMAKE="${QTDIR}/bin/qmake"
CMD_DEPLOY="${QTDIR}/bin/macdeployqt"

cd "$(dirname $0)"/..
rm -rf "${BUILDDIR}"
mkdir -p "${BUILDDIR}"
cd "${BUILDDIR}"
${CMD_QMAKE} CONFIG+="release" QMAKE_MACOSX_DEPLOYMENT_TARGET=10.7 "../${PROJECT}.pro"
make
RES_PATH="${APPNAME}.app/Contents/Resources"
rm -f "${RES_PATH}/empty.lproj"
mkdir -p "${RES_PATH}/en.lproj" "${RES_PATH}/ru.lproj"
${CMD_DEPLOY} "${APPNAME}.app" -dmg -verbose=2

hdiutil convert -format UDRW -o "${APPNAME}_rw.dmg" "${APPNAME}.dmg"
mkdir "${APPNAME}_rw_mount"
hdiutil attach -mountpoint "${APPNAME}_rw_mount" -noautoopen "${APPNAME}_rw.dmg"
cd "${APPNAME}_rw_mount"
ln -s /Applications ./Applications
cd ..
hdiutil detach "${APPNAME}_rw_mount"
hdiutil convert -format UDRO -o "${APPNAME}_ro.dmg" "${APPNAME}_rw.dmg"
cp "${APPNAME}_ro.dmg" ../"${DMGNAME}.dmg"

