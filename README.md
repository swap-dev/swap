# Swap

Swap (previously known as Freehaven) is based on [Monero](README_original.md)  

## Resources

- Discord: [discord.gg/SmcFCPu](https://discord.gg/SmcFCPu)
- Explorer: [swap.coinscope.cc](https://swap.coinscope.cc/)
- Pool List: [miningpoolstats.stream/swap](https://miningpoolstats.stream/swap)


## Build on linux

install deps:

`sudo apt-get install build-essential cmake pkg-config libboost-all-dev libssl-dev libzmq3-dev libunbound-dev libsodium-dev libreadline6-dev libpgm-dev`

clone repo:

`git clone --recursive https://github.com/swap-dev/swap`

build daemon and wallet:

`cd swap && mkdir build && cd build && cmake .. && make -j4 daemon simplewallet`

