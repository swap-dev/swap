# Swap

Copyright (c) 2014-2019 The Monero Project.   
Swap (previously known as Freehaven) is based on [Monero](README_original.md)

![Build-Linux](https://github.com/swap-dev/swap/workflows/Build-Linux/badge.svg)

## Production & Development

Active Branches:
- Stable: master(latest/release)
- Unstable: swap-v3.1dev(latest)
- Testing: N/A

To contribute to the Swap Project, please make all pull requests to the _swap-v3.1dev_ branch.<br/>
For production, please use the _latest or tagged release_ of the _master_ branch.

## Resources
- It is HIGHLY recommended that you join the #monero-dev IRC channel if you are developing software that uses Monero. Due to the nature of this open source software project, joining this channel and idling is the best way to stay updated on best practices and new developments in the Monero ecosystem. All you need to do is join the IRC channel and idle to stay updated with the latest in Monero development. If you do not, you risk wasting resources on developing integrations that are not compatible with the Monero network. The Monero core team and community continuously make efforts to communicate updates, developments, and documentation via other platforms – but for the best information, you need to talk to other Monero developers, and they are on IRC. #monero-dev is about Monero development, not getting help about using Monero, or help about development of other software, including yours, unless it also pertains to Monero code itself. For these cases, checkout #monero. 

- Webpage: [xwp.one](https://xwp.one)
- Explorer: [explorer.xwp.one](https://explorer.xwp.one)
- Pool List: [miningpoolstats.stream/swap](https://miningpoolstats.stream/swap)
- GitHub: [github.com/swap-dev/swap](https://github.com/swap-dev/swap)

## Social/Contact

- Bitcointalk [bitcointalk.org/index.php?topic=5083455](https://bitcointalk.org/index.php?topic=5083455)
- Discord: [discord.gg/SmcFCPu](https://discord.gg/SmcFCPu)
- Reddit: [r/SwapProject/](https://www.reddit.com/r/SwapProject/)
- Twitter: [@Swap_Community](https://twitter.com/Swap_Community)
- Email: xwpdev@gmail.com

## Specifications & Emission

- Coin ticker: XWP
- Total supply: 18,400,000 coins before tail emission
- Tail emission: ~158,000 coins each year (starting at year 8)
- Decimal places: 12
- PoW hash algorithm: Cuckaroo29s
- Block time: 15 seconds
- Difficulty Adjustment Algorithm: Monero DAA
- Genesis block: 2018-11-16 (November 16, 2018) at 09:06:03 (UTC)
- Premine: No
- Developer fee: No
- Founders reward: No
- Mainnet default P2P port: 19949
- Mainnet default RPC port: 19950

## Donation Address
fh2jc6PbQYd4a5PY3ooPMZiPVniMy4MGcjSRBnoBVc1xLmdCHJ6hc98Ess2hpN2mDgPnCAXtDUUbmjWYutRvdoSr2Nps2o5wc

## Build on linux

install deps:

`sudo apt-get install build-essential cmake pkg-config libboost-all-dev libssl-dev libzmq3-dev libunbound-dev libsodium-dev libreadline6-dev libpgm-dev libnorm-dev`

clone repo:

`git clone --recursive https://github.com/swap-dev/swap`

build daemon and wallet:

`cd swap && mkdir build && cd build && cmake .. && make daemon simplewallet`

or build everything:

`cd swap && mkdir build && cd build && cmake .. && make`

## Build on Windows (using MinGW)

install deps:

`pacman -S mingw-w64-x86_64-toolchain make mingw-w64-x86_64-cmake mingw-w64-x86_64-boost mingw-w64-x86_64-openssl mingw-w64-x86_64-zeromq mingw-w64-x86_64-libsodium mingw-w64-x86_64-hidapi`

clone repo:

`git clone --recursive https://github.com/swap-dev/swap`

build daemon and wallet:

`cd swap && make release-static-win64`

