VAGRANTFILE_API_VERSION = "2"

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
  config.vm.box = "hashicorp/precise64"
  config.vm.box_url = "https://vagrantcloud.com/hashicorp/precise64/version/2/provider/virtualbox.box"

  config.vm.provider :virtualbox do |vb|
    vb.customize ['modifyvm', :id, '--usb', 'on']
    vb.customize ['usbfilter', 'add', '0', '--target', :id, '--name', 'MoMoFSU', '--vendorid', '0x0403', '--productid', '0x6015']
  end

  config.vm.network "forwarded_port", guest: 80, host: 1111

  config.vm.provision "shell", path: "tools/vagrant/provision.sh"
end