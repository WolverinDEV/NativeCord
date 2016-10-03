cmake_minimum_required(VERSION 3.2.2)
project(NativeCord)

message("cpr include path: ${CPR_INCLUDE_DIRS}")
message("cpr libs: ${CPR_LIBRARIES}")
message("curl libs: ${CURL_LIBRARIES}")
#message("${CURL_FOUND}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-pointer-arith -Wno-deprecated -Wno-return-local-addr")

set(SOURCE_FILES src/protocoll/Buffers/DataBuffer.cpp src/connection/PlayerConnection.cpp include/connection/PlayerConnection.h src/connection/Socket.cpp include/connection/Socket.h src/protocoll/Buffers/StreamedDataBuffer.cpp include/protocoll/StreamedDataBuffer.h src/protocoll/packet/ClientPacketHandler.cpp include/protocoll/ClientPacketHandler.h include/protocoll/Packets.h src/chat/ChatMessage.cpp include/chat/ChatMessage.h include/chat/ActionEvent.h src/connection/ServerConnection.cpp include/connection/ServerConnection.h src/protocoll/packet/ServerPacketHandler.cpp include/protocoll/ServerPacketHandler.h include/protocoll/PacketHandler.h include/connection/ConnectionState.h include/connection/Connection.h include/utils/TimeUtils.h include/utils/UUIDUtils.h include/utils/SocketUtil.h include/utils/StringUtil.h include/utils/EntityRewrite.h src/server/TabManager.cpp include/server/TabManager.h include/chat/ChatColor.h src/server/ScoreboardManager.cpp include/server/ScoreboardManager.h src/config/Configuration.cpp include/config/Configuration.h NativeCord.cpp src/server/ServerInfo.cpp include/server/ServerInfo.h src/encription/Cipper.cpp include/cryption/Cipper.h include/cryption/RSAUtil.h src/utils/Base64Utils.cpp include/utils/Base64Utils.h include/utils/HTTPUtil.h include/utils/HTTPUtil.h include/utils/HexUtils.h src/utils/UtilImplement.cpp)
set(SOURCE_FILES ${SOURCE_FILES} src/numbers/BigIntegerAlgorithms.cc src/numbers/BigIntegerUtils.cc include/numbers/BigUnsignedInABase.hh include/numbers/BigIntegerAlgorithms.hh include/numbers/BigIntegerUtils.hh include/numbers/NumberlikeArray.hh src/numbers/BigInteger.cc src/numbers/BigUnsigned.cc include/numbers/BigInteger.hh include/numbers/BigUnsigned.hh include/numbers/BigIntegerLibrary.hh src/numbers/BigUnsignedInABase.cc include/connection/GameProfile.h include/plugin/cxx/CXXPlugin.h include/plugin/Plugin.h src/plugin/handle/JavaPluginManagerImpl.cpp src/plugin/handle/JavaPluginManagerImpl.h include/plugin/java/JavaPlugin.h src/plugin/PluginManager.cpp include/plugin/PluginManager.h src/plugin/handle/CXXPluginManagerImpl.cpp src/plugin/handle/CXXPluginManagerImpl.h)
#set(SOURCE_FILES ${SOURCE_FILES} test/BufferTest.cpp)

link_directories(/home/wolverindev/cgit/cpr/lib/)
link_libraries(pthread uuid z yaml-cpp crypto libcpr.a libcurl.so) #z = zlib   cpr from  https://github.com/msuvajac/cpr (With runtime ssl verification)

add_subdirectory(/home/wolverindev/cgit/cpr/cpr/ cpr)
include_directories(${CPR_INCLUDE_DIRS})


########################### Java ####################################
include_directories(/usr/lib/jvm/java-8-oracle/include/)
include_directories(/usr/lib/jvm/java-8-oracle/include/linux)

link_libraries(/usr/lib/jvm/java-8-oracle/jre/lib/amd64/server/libjvm.so)
#####################################################################

########################## 32 Bit [DONT WORK :/] ###################################
#add_definitions(-DCURL_32_BIT)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
#####################################################################

add_executable(NativeCord ${SOURCE_FILES})