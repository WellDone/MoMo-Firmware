set -e
source $HOME/.profile
if [ "$1" == "pro" ]; then
	cd $MOMOPATH/momo_modules/pic12_executive && momo build test && momo build
	cd $MOMOPATH/momo_modules/gsm_module && momo build test
else
	echo "Skipping modules that require XC8 PRO"
fi
cd $MOMOPATH/momo_modules/gsm_module && momo build -c && momo build
cd $MOMOPATH/momo_modules/multi_sensor_module && momo build -c && momo build test && momo build

cd $MOMOPATH/momo_modules/shared/pic24 && momo build -c && momo build
cd $MOMOPATH/momo_modules/mainboard && momo build -c && momo build 
cd $MOMOPATH/momo_modules/field_service_unit && momo build -c && momo build
cd $MOMOPATH/momo_modules/mainboard_bootloader && momo build -c && momo build