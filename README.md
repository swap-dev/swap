# swoop

swoop (previously known as Freehaven) is based on [Monero](README_original.md)  

## Resources

- Discord: [discord.gg/SmcFCPu](https://discord.gg/SmcFCPu)
- Explorer: [swoop.coinscope.cc](https://swoop.coinscope.cc/)
- Pool List: [miningpoolstats.stream/swoop](https://miningpoolstats.stream/swoop)
- Bitcointalk [bitcointalk.org/index.php?topic=5083455](https://bitcointalk.org/index.php?topic=5083455)


## Build on linux

install deps:

`sudo apt-get install build-essential cmake pkg-config libboost-all-dev libssl-dev libzmq3-dev libunbound-dev libsodium-dev libreadline6-dev libpgm-dev`

clone repo:

`git clone --recursive https://github.com/swoop-dev/swoop`

build daemon and wallet:

`cd swoop && mkdir build && cd build && cmake .. && make -j4 daemon simplewallet`

