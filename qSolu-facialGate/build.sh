#!/bin/sh 

set -e 

SHELL_FOLDER=$(cd "$(dirname "$0")";pwd)
cd $SHELL_FOLDER
CUR_DIR_NAME=`basename "$SHELL_FOLDER"`

##  Build:   ./build.sh
##  reBuild: ./build.sh all
##  Clear:   ./build.sh clear
## ========================================
set +e
if [ "$1" = "all" ]; then
	rm Makefile
	rm -rf Release
elif [ "$1" = "clear" ];then
	rm Makefile
	rm -rf Release
	exit 0
fi
set -e

if [ ! -d "Release" ]; then
	# this cmd include create dir operation(mkdir Release/)
	/opt/rv1126_rv1109_sdk/buildroot/output/rockchip_face_board/host/bin/qmake $CUR_DIR_NAME.pro
fi

make

##  custom shell
## ========================================
#adb push Release/$CUR_DIR_NAME /userdata/QSolu/$CUR_DIR_NAME

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
