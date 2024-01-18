#!/bin/sh 

set -e 

alias_file=~/.bash_aliases
if [ -e "$alias_file" ]; then
        . $alias_file
fi

SHELL_FOLDER=$(cd "$(dirname "$0")";pwd)
cd $SHELL_FOLDER
CUR_DIR_NAME=`basename "$SHELL_FOLDER"`

APP_DIR=${SYSROOT}/userdata/apps/facialGate/
APP_NAME=${CUR_DIR_NAME}

##  Build:   ./build.sh
##  reBuild: ./build.sh all
##  Clear:   ./build.sh clear
## ========================================
set +e
if [ "$1" = "all" ]; then
	rm -f Makefile
	rm -rf Release
elif [ "$1" = "clear" ];then
	rm -f Makefile
	rm -rf Release
	exit 0
fi
set -e

QCORE_FILE=${SYSROOT}/usr/lib/libQt5Core.so.5.15.2
if [ ! -d "Release" ]; then
	# this cmd include create dir operation(mkdir Release/)
	if [ -e "$QCORE_FILE" ]; then
		${SYSROOT}/usr/bin/qmake $APP_NAME.pro
	else
		qmake $APP_NAME.pro
	fi
fi

make

##  custom shell
## ========================================
#adb push Release/$APP_NAME ${APP_DIR}/$APP_NAME
mkdir -p ${APP_DIR} && cp Release/$APP_NAME ${APP_DIR}

#rm -rf rootfs
#mkdir rootfs
#tar xf release-qt.tar -C rootfs
#cd -
#cp $CUR_DIR_NAME ../Release/rootfs/userdata/QFacePass1109_release/
#mv ../Release/rootfs/userdata/QFacePass1109_release/$CUR_DIR_NAME ../Release/rootfs/userdata/QFacePass1109_release/$CUR_DIR_NAME_1.0.001
#cd ../Release/rootfs/
#tar cf ../release-qt.tar *
#cd -
#ls -lh ../Release/release-qt.tar

exit 0
