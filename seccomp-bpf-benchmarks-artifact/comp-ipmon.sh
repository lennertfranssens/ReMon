cd ../IP-MON
ruby ./generate_headers.rb

#cp "$__home_dir/benchmarks/patches/IP-MON/"* "$__home_dir/../IP-MON/"

./comp.sh
mv ./libipmon.so ./libipmon-default.so

#patch -p 2 -d ./ < ../eurosys2022-artifact/benchmarks/patches/ipmon-nginx.patch
#./comp.sh
#mv ./libipmon.so ./libipmon-nginx.so
#patch -R -p 2 -d ./ < ../eurosys2022-artifact/benchmarks/patches/ipmon-nginx.patch

#patch -p 2 -d ./ < ../eurosys2022-artifact/benchmarks/patches/ipmon-apache.patch
#./comp.sh
#mv ./libipmon.so ./libipmon-apache.so
#patch -R -p 2 -d ./ < ../eurosys2022-artifact/benchmarks/patches/ipmon-apache.patch

#patch -p 2 -d ./ < ../eurosys2022-artifact/benchmarks/patches/ipmon-mplayer.patch
#./comp.sh
#mv ./libipmon.so ./libipmon-mplayer.so
#patch -R -p 2 -d ./ < ../eurosys2022-artifact/benchmarks/patches/ipmon-mplayer.patch

ln -fs ./libipmon-default.so ./libipmon.so
## ReMon Setup ###############################
