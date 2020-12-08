cd ~/Projects/nos-1/
sudo rm linux-5.4.9-state*
sudo rm linux-image-5.4.9-state*
sudo rm linux-headers-5.4.9-state*
sudo rm linux-libc-dev_5.4.9-state*

sudo apt-get --purge remove linux-image-5.4.9-state
sudo apt-get --purge remove linux-headers-5.4.9-state

git pull

cd linux-5.4.9
make clean
# make deb-pkg
