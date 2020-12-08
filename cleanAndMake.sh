cd ~/Projects/nos-1/
sudo rm linux-5.4.9*
sudo rm linux-image-5.4.9*
sudo rm linux-headers-5.4.9*
sudo rm linux-libc-dev_5.4.9*

sudo apt-get --purge remove linux-image-5.4.9
sudo apt-get --purge remove linux-headers-5.4.9

git pull

cd linux-5.4.9
make clean
# make deb-pkg
