#!/bin/bash
PROJECT=deathloop
BUILDDIR=build_osx_qt5.6_clang64_libstdc++
APPNAME="DeathLoop"
DMGNAME="${PROJECT}_qt5.6_clang64_libstdc++"

QTDIR="/opt/Qt/5.6.2/clang_64_libstdc++_sdk10.11"
CMD_QMAKE="${QTDIR}/bin/qmake"
CMD_DEPLOY="${QTDIR}/bin/macdeployqt"

cd "$(dirname $0)"/..
rm -rf "${BUILDDIR}"
mkdir -p "${BUILDDIR}"
cd "${BUILDDIR}"
${CMD_QMAKE} CONFIG+="release" "../${PROJECT}.pro"
make -j3
sed -e 's/10.7.0/10.6.0/' -i "" "${APPNAME}.app/Contents/Info.plist"
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

