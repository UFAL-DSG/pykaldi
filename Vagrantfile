# -*- mode: ruby -*- # vi: set ft=ruby :
Vagrant.configure("2") do |config|
  config.ssh.forward_x11 = true # useful since some audio testing programs use x11
  config.vm.box = "ubuntu/trusty64"
  config.vm.synced_folder ".", "/vagrant"
  config.vm.provision :shell, :inline => $BOOTSTRAP_SCRIPT # see below

  # enable audio drivers on VM settings
  config.vm.provider :virtualbox do |vb|
    vb.memory = 4096 
    vb.cpus = 4
    vb.customize ["modifyvm", :id, '--audio', 'coreaudio', '--audiocontroller', 'ac97']  # choices: hda sb16 ac97
  end

end

$BOOTSTRAP_SCRIPT = <<EOF
  set -e # Stop on any error

  # --------------- SETTINGS ----------------
  # Other settings
  export DEBIAN_FRONTEND=noninteractive

  sudo usermod -a -G audio vagrant

  sudo apt-get update
  sudo apt-get install -y sox git python-dev python-pip python-setuptools libatlas-base-dev build-essential
  sudo pip install cython pyyaml pystache

  # install pyaudio
  # sudo apt-get install -y alsa alsa-tools portaudio19-dev
  # sudo pip install flask
  # pushd /tmp
  # wget http://people.csail.mit.edu/hubert/pyaudio/packages/python-pyaudio_0.2.8-1_amd64.deb
  # sudo dpkg -i python-pyaudio_0.2.8-1_amd64.deb
  # popd # /tmp


  if [ ! -f ~/runonce ]
  then
    echo 'echo export LANG=en_US.UTF-8' >> ~/.bashrc
    echo 'echo export LC_ALL="$LANG"' >> ~/.bashrc
    echo 'export LANGUAGE="$LANG"' >> ~/.bashrc

    # have to reboot for alsa drivers to kick in, but only the first time of course
    sudo reboot
    touch ~/runonce
    ln -s /vagrant pykaldi
  fi
EOF
