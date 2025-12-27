set(CMAKE_CXX_STANDARD 23)

add_executable(proxy-server
        proxy/CacheManager.h
        proxy/NetworkClient.h
        proxy/ParsedUrl.h
        proxy/ProxyHandler.h
        proxy/ProxyServer.h
        proxy/UrlParser.h
        utils/StatisticsManager.h
        proxy_server.cpp
)

set_target_properties(proxy-server PROPERTIES LINKER_LANGUAGE CXX)
