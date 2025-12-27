set(CMAKE_CXX_STANDARD 23)

add_executable(smtp-client
        smtp_client.cpp
        smtp/TcpSocket.h
        smtp/SmtpClient.h
        smtp/InputValidator.h
)

set_target_properties(smtp-client PROPERTIES LINKER_LANGUAGE CXX)
