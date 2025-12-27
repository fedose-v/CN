set(CMAKE_CXX_STANDARD 23)

add_executable(udp-pinger
        udp/PingClient.h
        udp/UdpSocket.h
        utils/TimeUtils.h
        udp_pinger.cpp
)

add_executable(udp-server
        udp/PingServer.h
        udp/SocketAddress.h
        udp/UdpServerSocket.h
        utils/RandomGenerator.h
        udp_pinger.cpp
)

set_target_properties(udp-pinger PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(udp-server PROPERTIES LINKER_LANGUAGE CXX)
