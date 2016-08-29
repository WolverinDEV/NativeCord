# NativeCord
NativeCord. A very fast C++ proxy for minecraft.

### Currently searching for some sponsors for a demo server and some buisness partners. Im also happy about every help.

Supported Systems:
- Linux x64
- Linux x32 (not tested with the newest but i think its working)

Compiler relequipments:
- CMake 3.6+
- UUID lib (sudo apt-get install uuid-dev)
- zlib lib (sudo apt-get install zlib1g-dev)
- yaml-cpp lib (https://github.com/jbeder/yaml-cpp)


TODO List:
- Implement ip_uuid_forward
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
