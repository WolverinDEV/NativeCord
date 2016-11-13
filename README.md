# NativeCord

[![Join the chat at https://gitter.im/WolverinDEV/NativeCord](https://badges.gitter.im/WolverinDEV/NativeCord.svg)](https://gitter.im/WolverinDEV/NativeCord?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
NativeCord. A very fast C++ proxy for minecraft.

### Currently searching for some sponsors for a demo server and some buisness partners. Im also happy about every help.

Supported Systems:
- Linux x64
- Linux x32 (Not working... (Need some changes in jvm incluse and -luuid and -lyaml-cpp MAY than its working....))

Compiler relequipments:
- CMake 3.2.2+
- UUID lib (sudo apt-get install uuid-dev)
- zlib lib (sudo apt-get install zlib1g-dev)
- ssl lib (sudo apt-get install libssl-dev)

Compile/Install
  1. git clone https://github.com/WolverinDEV/NativeCord.git<br>
  2. cd NativeCord<br>
  3. git submodule update --init --recursive<br>
  4. go int lib and install cpr & yaml<br>
  5. cmake .<br>
  6. make<br>
  7. cd .. && mkdir nbungee<br>
  8. cp NativeCord/NativeCord nbungee/<br>
  9. Redy to launch NativeCord. Run it with ./NativeCord<br>

Install libs:<br>
  cpr: <br>
    1. cmake . <br>
    2. make<br>
    3. make install<br>
  yaml:<br>
    1. make<br>
    2. make install<br>

TODO List:
- Implement ip_uuid_forward [DONE]
- Implement online mode [DONE]
- Fix some exploits/crashes
  - Add size check for packet length
  - Handle exception on wrong compression
  - Handle exception on missing/wrong packet data
- Improve entity rewrite. Currently not for all packets.
- Implement configurable messages
- Add a bootloader in java to automaticly choose the right binary file
- Add a wikki
- Add c++ plugin api
- Add java plugin api
- Add mac-os and windows support
- May be add Minecraft 1.7 support

Buglist:
- Currently no known bugs. Most bugs are on the todo list as a future.
- Cant change network host (dont bind on that)
