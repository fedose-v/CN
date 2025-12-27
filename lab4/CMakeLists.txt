set(CMAKE_CXX_STANDARD 23)

add_executable(dns-resolver
        dns_resolver.cpp
        utils/Logger.h
        dns/DnsHeader.h
        dns/DnsPacketBuilder.h
        dns/DnsRecord.h
        dns/DnsResolver.h
        dns/DnsResponseParser.h
        dns/SocketHandler.h
)

set_target_properties(dns-resolver PROPERTIES LINKER_LANGUAGE CXX)
