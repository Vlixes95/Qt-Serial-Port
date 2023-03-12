#ifndef CONSOLEAPPLICATION_SERIALPORT_MSGPACK_H
#define CONSOLEAPPLICATION_SERIALPORT_MSGPACK_H

#include <iostream>

// TODO: enum and swich statement
const std::string READ_COMMAND = "r";
const std::string WRITE_COMMAND = "w";
const std::string UPDATE_COMMAND = "u";
const std::string DELETE_COMMAND = "d";
const std::string PRINT_COMMAND = "p";
const std::string ERROR = "e";

class MSGPack {
private:
    std::string command;
    std::string fileName;
    std::string content;

public:
    MSGPack ( ) = default;

    MSGPack ( const MSGPack &msgPack ) :
            command( msgPack.command ), fileName( msgPack.fileName ), content( msgPack.content ) {}

    MSGPack ( std::string &command, std::string &fileName, std::string &content ) :
            command( command ), fileName( fileName ), content( content ) {}

    static void Pack ( MSGPack &packMSG, const std::string &msg );

    static void PackToString ( const MSGPack &packMSG, std::string &msg );

    std::string getCommand ( ) { return command; }

    void setCommand ( std::string &com ) { command = com; }

    std::string getFileName ( ) { return fileName; }

    void setFileName ( std::string &file ) { fileName = file; }

    std::string getContent ( ) { return content; }

    void setContent ( std::string &cnt ) { content = cnt; }

    static void PrintContent ( const std::string &msg );

};

#endif //CONSOLEAPPLICATION_SERIALPORT_MSGPACK_H
