cmake_minimum_required(VERSION 3.2.2)
project(NativeCord)

message("cpr include path: ${CPR_INCLUDE_DIRS}")
message("cpr libs: ${CPR_LIBRARIES}")
message("curl libs: ${CURL_LIBRARIES}")
#message("${CURL_FOUND}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-pointer-arith")

set(SOURCE_FILES protocoll/Buffers/DataBuffer.cpp connection/PlayerConnection.cpp connection/PlayerConnection.h connection/Socket.cpp connection/Socket.h protocoll/Buffers/StreamedDataBuffer.cpp protocoll/Buffers/StreamedDataBuffer.h protocoll/packet/ClientPacketHandler.cpp protocoll/packet/ClientPacketHandler.h protocoll/packet/Packets.h chat/ChatMessage.cpp chat/ChatMessage.h chat/ActionEvent.h connection/ServerConnection.cpp connection/ServerConnection.h protocoll/packet/ServerPacketHandler.cpp protocoll/packet/ServerPacketHandler.h protocoll/packet/PacketHandler.h connection/ConnectionState.h connection/Connection.h utils/TimeUtils.h utils/UUIDUtils.h utils/SocketUtil.h utils/StringUtil.h utils/EntityRewrite.h server/TabManager.cpp server/TabManager.h chat/ChatColor.h server/ScoreboardManager.cpp server/ScoreboardManager.h config/Configuration.cpp config/Configuration.h NativeCord.cpp server/ServerInfo.cpp server/ServerInfo.h encription/Cipper.cpp encription/Cipper.h encription/RSAUtil.h utils/Base64Utils.cpp utils/Base64Utils.h utils/HTTPUtil.h utils/HTTPUtil.h connection/PlayerConnectionProperties.h utils/HexUtils.h utils/UtilImplement.cpp)
set(SOURCE_FILES ${SOURCE_FILES} numbers/BigIntegerAlgorithms.cc numbers/BigIntegerUtils.cc numbers/BigUnsignedInABase.hh numbers/BigIntegerAlgorithms.hh numbers/BigIntegerUtils.hh numbers/NumberlikeArray.hh numbers/BigInteger.cc numbers/BigUnsigned.cc numbers/BigInteger.hh numbers/BigUnsigned.hh numbers/BigIntegerLibrary.hh numbers/BigUnsignedInABase.cc)
#set(SOURCE_FILES ${SOURCE_FILES} test/BufferTest.cpp)

link_directories(/home/wolverindev/cgit/cpr/lib/)
link_libraries(pthread uuid z yaml-cpp crypto libcpr.a libcurl.so) #z = zlib   cpr from  https://github.com/msuvajac/cpr (With runtime ssl verification)

add_subdirectory(/home/wolverindev/cgit/cpr/cpr/ cpr)
include_directories(${CPR_INCLUDE_DIRS})


add_executable(NativeCord ${SOURCE_FILES})