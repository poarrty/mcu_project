# robot_nav_mcu_main
- ### 安装加密库
```bash
pip3 install pycryptodome pycryptodomex ecdsa pyelftools numpy pycrypto
```
- ### 编译microros固件库并编译代码（不可加-j8等多线程选项）
```bash
make microros
```
- ### 编译并打包带版本号的固件
```bash
make images
```
- ### 编译出用于OTA升级的IAP固件
```bash
make iap
```
- ### 编译并烧录固件
```bash
make flash
```

# 安保底盘
#设置型号
MODEL_NAME=SECURITY_CHASSIS
#选择机器人型号
ROBOT=ROBOT_SECURITY
CFLAGS += -D ${ROBOT}
#选择电机型号
MOTOR_TYPE=USE_MOTOR_SELF
CFLAGS += -D ${MOTOR_TYPE}

# 清洁底盘
#设置型号
MODEL_NAME=CLEAN_CHASSIS
#选择机器人型号
ROBOT=BINGO
CFLAGS += -D ${ROBOT}
#选择电机型号
MOTOR_TYPE=USE_MOTOR_FENGDEKONG
CFLAGS += -D ${MOTOR_TYPE}

或

#设置型号
MODEL_NAME=CLEAN_CHASSIS
#选择机器人型号
ROBOT=BINGO
CFLAGS += -D ${ROBOT}
#选择电机型号
MOTOR_TYPE=USE_MOTOR_SYNTRON
CFLAGS += -D ${MOTOR_TYPE}
