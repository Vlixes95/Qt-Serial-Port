#include "msgpack.h"
#include <set>

void MSGPack::Pack ( MSGPack &packMSG, const std::string &msg ) {

    unsigned int i = 0;
    bool init = false;
    int size = 0;
    unsigned int elements = 0;

    char *characters = const_cast<char *>(msg.c_str( ));

    while ( true ) {

        if ( characters[i] == '}' ) {
            break;
        }

        if ( characters[i] == '{' && !init )
            init = true;

        if ( init ) {
            if ( characters[i] == 'L' ) {
                ++i;
                char tmpSize[10] = "\0";

                for ( int j = 0; j < 10; ++j ) {
                    if ( characters[i] == 'C' ) {
                        break;
                    }
                    tmpSize[j] = characters[i];
                    ++i;
                    //TODO: control if i>10
                }
                // TODO: replace strtol
                size = atoi( tmpSize );
            }

            if ( characters[i] == 'C' ) {
                ++i;
                //TODO: try new operator
                char *buff = ( char * ) malloc( size * sizeof( char ) + 1 ); // +1 for '\0'
                for ( int j = 0; j < size; ++j ) {
                    buff[j] = characters[i];
                    if (( j + 1 ) >= size ) {
                        buff[j + 1] = '\0';
                        break;
                    }
                    ++i;
                }

                if ( elements == 0 ) {
                    std::string s( buff );
                    packMSG.setCommand( s );
                } else if ( elements == 1 ) {
                    std::string s( buff );
                    packMSG.setFileName( s );
                } else if ( elements == 2 ) {
                    std::string s( buff );
                    packMSG.setContent( s );
                }
                ++elements;

                free( buff );
            }
        }
        ++i;
    }

}

void MSGPack::PackToString ( const MSGPack &packMSG, std::string &msg ) {

    msg.append( "{" );

    msg.append( "L" );
    msg.append( std::to_string( packMSG.command.length( )));
    msg.append( "C" );
    msg.append( packMSG.command );

    msg.append( "L" );
    msg.append( std::to_string( packMSG.fileName.length( )));
    msg.append( "C" );
    msg.append( packMSG.fileName );

    msg.append( "L" );
    msg.append( std::to_string( packMSG.content.length( )));
    msg.append( "C" );
    msg.append( packMSG.content );

    msg.append( "}" );

}

void MSGPack::PrintContent ( const std::string &msg) {

    std::set < std::string > driveContent;

    int element = 0;

    // TODO: smart pointer
    // TODO: recursive function with find
    int j = 0;
    int size = 0;
    for ( int i = 0; i < msg.length( ); ++i ) {
        if ( msg[i] == ';' ) {
            size = i - j;
            char *s = ( char * ) malloc( size * sizeof( char ) + 1 ); // +1 for the null terminator
            msg.copy( s, size, j );
            s[size *
              sizeof( char )] = '\0'; // string.copy does not copy the null terminator, you need to add it in order to create the string;
            if(driveContent.emplace( s ).second)
                std::cout << "\t" << s << "\n";
            free( s );
            ++i;
            j = i;
            ++element;
        }
    }

}
