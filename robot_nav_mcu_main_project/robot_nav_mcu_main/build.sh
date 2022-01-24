#!/bin/sh
#此脚本将需要生成的固件都编译拷贝到编译目录（编译目录必须为工程目录下的 /build 目录）
#CI 会将工程目录下的 build/*.elf build/*.bin build/*.hex 加上构建号后推送到 artifactory

if [ 1 -eq $# ]
then
    CI_BUILD_NUM_=$1
else
    CI_BUILD_NUM_="0"
fi

echo "CI Build id $CI_BUILD_NUM_\r\n"

make clean

#ROBOT_="ROBOT_SECURITY"
#MODEL_NAME_="SECURITY_CHASSIS"
#MOTOR_TYPE_="USE_MOTOR_SELF"
#echo "\r\nBuild images[$CI_BUILD_NUM_]: $ROBOT_-$MODEL_NAME_-$MOTOR_TYPE_\r\n"
#make images ROBOT=$ROBOT_ MODEL_NAME=$MODEL_NAME_ MOTOR_TYPE=$MOTOR_TYPE_ CI_BUILD_NUM=$CI_BUILD_NUM_ -j32

ROBOT_="BINGO"
MODEL_NAME_="CLEAN_CHASSIS"
MOTOR_TYPE_="USE_MOTOR_SYNTRON"
echo "\r\nBuild images[$CI_BUILD_NUM_]: $ROBOT_-$MODEL_NAME_-$MOTOR_TYPE_\r\n"
make iap ROBOT=$ROBOT_ MODEL_NAME=$MODEL_NAME_ MOTOR_TYPE=$MOTOR_TYPE_ CI_BUILD_NUM=$CI_BUILD_NUM_

#ROBOT_="BINGO"
#MODEL_NAME_="CLEAN_CHASSIS"
#MOTOR_TYPE_="USE_MOTOR_FENGDEKONG"
#echo "\r\nBuild images[$CI_BUILD_NUM_]: $ROBOT_-$MODEL_NAME_-$MOTOR_TYPE_\r\n"
#make images ROBOT=$ROBOT_ MODEL_NAME=$MODEL_NAME_ MOTOR_TYPE=$MOTOR_TYPE_ CI_BUILD_NUM=$CI_BUILD_NUM_ -j32
