set -e
source $HOME/.profile
#cd $MOMOROOT/momo_modules/gsm_module && momo build
#cd $MOMOROOT/momo_modules/pic12_executive && momo build
cd $MOMOROOT/momo_modules/multi_sensor_module && momo build -c && momo build
cd $MOMOROOT/momo_modules/shared/pic24 && momo build -c && momo build
cd $MOMOROOT/momo_modules/mainboard && momo build -c && momo build 
cd $MOMOROOT/momo_modules/field_service_unit && momo build -c && momo build
cd $MOMOROOT/momo_modules/mainboard_bootloader && momo build -c && momo build

#Test pymomo
cd $MOMOROOT/tools && nosetests -w pymomo/test