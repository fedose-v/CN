set(CMAKE_CXX_STANDARD 23)

add_executable(web-server
        server/HttpRequest.h
        webserver.cpp
        server/HttpResponse.h
        server/FileHandler.h
        server/Socket.h
        server/HttpClient.h
        server/HttpServer.h
)

set_target_properties(web-server PROPERTIES LINKER_LANGUAGE CXX)
