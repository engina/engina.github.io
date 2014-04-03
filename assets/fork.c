#include <stdio.h>
#include <string.h>

#define BUF_LEN (1024)

void print_command( const char* cmd )
{
        char stdout_buf[ BUF_LEN ];
        size_t r;
        FILE* p = popen( cmd, "r" );
        if( p )
        {
                memset( stdout_buf, 0, BUF_LEN );
                while( ( r = fread( stdout_buf, 1, BUF_LEN, p ) ) )
                {
                        printf( "%s", stdout_buf );
                        memset( stdout_buf, 0, BUF_LEN );
                }

                if( !feof( p ) )
                {
                        fprintf( stderr, "Could not read whole output.\n" );
                        return;
                }
                pclose( p );
                return;
        }
        fprintf( stderr, "Could not open pipe.\n" );
}

int main( int argc, char** argv )
{
    printf( "Content-type: text/plain\r\n\n" );
    print_command( "/bin/uname -a" );
    print_command( "/usr/bin/uptime" );
    return 0;
}