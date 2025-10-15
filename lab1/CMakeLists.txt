set(CMAKE_CXX_STANDARD 23)

set(COMMON_FILES
        socket/Client.cpp
        socket/Client.h
        socket/MessageData.h
        socket/Server.cpp
        socket/Server.h
        socket/SocketProvider.cpp
        socket/SocketProvider.h
)

add_executable(socket-client
        ${COMMON_FILES}
        client.cpp
)

add_executable(socket-server
        ${COMMON_FILES}
        server.cpp
)

set_target_properties(socket-client PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(socket-server PROPERTIES LINKER_LANGUAGE CXX)
