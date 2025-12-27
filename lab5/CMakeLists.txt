set(CMAKE_CXX_STANDARD 23)

add_executable(rdtp
        rdtp.cpp
        rdtp/Ack.h
        rdtp/FileReader.h
        rdtp/Packet.h
        rdtp/RdtReceiver.h
        rdtp/RdtSender.h
        rdtp/UdpSocket.h
        utils/Logger.h
)

set_target_properties(rdtp PROPERTIES LINKER_LANGUAGE CXX)
