cmake_minimum_required(VERSION 3.6)
project(CBungee)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES protocoll/Buffers/DataBuffer.cpp connection/PlayerConnection.cpp connection/PlayerConnection.h connection/Socket.cpp connection/Socket.h protocoll/Buffers/StreamedDataBuffer.cpp protocoll/Buffers/StreamedDataBuffer.h protocoll/packet/ClientPacketHandler.cpp protocoll/packet/ClientPacketHandler.h protocoll/packet/Packets.h chat/ChatMessage.cpp chat/ChatMessage.h chat/HoverEvent.h connection/ServerConnection.cpp connection/ServerConnection.h protocoll/packet/ServerPacketHandler.cpp protocoll/packet/ServerPacketHandler.h protocoll/packet/PacketHandler.h connection/ConnectionState.h connection/Connection.h utils/TimeUtils.h utils/UUIDUtils.h utils/SocketUtil.h)
set(SOURCE_FILES ${SOURCE_FILES} test/BufferTest.cpp chat/ChatColor.h)

link_libraries(uuid z) #z = zlib

add_executable(CBungee ${SOURCE_FILES})
target_link_libraries(CBungee pthread)