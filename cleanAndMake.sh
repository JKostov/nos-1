cd ~/Projects/nos-1/
sudo rm linux-5.4.9-*

sudo apt-get --purge remove linux-image-5.4.9-states
sudo apt-get --purge remove linux-headers-5.4.9-states

git pull origin master

cd linux-5.4.9
make clean
make deb-pkg
