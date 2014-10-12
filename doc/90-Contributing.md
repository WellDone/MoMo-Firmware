# Contributing

Development can be done locally (see [Development Dependencies](#contributing-development-dependencies)) or using [Vagrant](http://www.vagrantup.com) by simply running `vagrant up && vagrant ssh`

## Development Dependencies
### Install Required Libraries

The MoMo build system and associated tools are written in python.  They have been tested with Python 2.7 and may or may not work with either older or new versions.  It also depends on the following external python packages:
* cmdln
* ZODB3
* colorama
* pyparsing
* intelhex
* BeautifulSoup4
* Cheetah
* scons
* pyserial
* pytest

These are almost all installable via easy_install, so the following command would work:
```shell
easy_install cmdln ZODB3 colorama pyparsing intelhex BeautifulSoup4 Cheetah pyserial pytest
```

*Note* scons cannot be successfully installed using easy_install currently.  You can download it from the [scons website](http://www.scons.org/download.php)

Note that on Mac OS X you will have to use sudo to install the packages as root.  On linux you probably will as well depending on your python installation. If you want to work with the circuit board layouts, you must install [CADSoft Eagle](https://www.cadsoftusa.com/download-eagle) version at least 6.5.0.