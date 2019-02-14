#include<stdio.h>


typedef unsigned char u_int8_t;
typedef unsigned int u_int32_t;


u_int8_t chr2Base(char c)
{
    if ( c >= 'A' && c <= 'Z' )
        return ( u_int8_t )( c - 'A' );
    else if ( c >= 'a' && c <= 'z' )
        return ( u_int8_t )( c - 'a' + 26 );
    else if ( c >= '0' && c <= '9' )
        return ( u_int8_t )( c - '0' + 52 );
    else if ( c == '+' )
        return 62;
    else if ( c == '/' )
        return 63;
    else
        return 64;  //  Invalid bse64 code
}

int base64Decode(const char *aSrc,u_int32_t length)
{
    u_int8_t   c, t;

	int counter = 0;
	char result;
	u_int32_t i;
    for (i = 0; i < length; i++ )
    {
        if ( aSrc[i] == '=' || aSrc[i] == '\0')
            break;
		c = chr2Base( aSrc[i] );
        if( c == 64 ) //  Invalid base64, omit it.
			continue;

        switch ( counter % 4 )
        {
        case 0 :
            t = c << 2;
            break;
        case 1 :
            result = ( u_int8_t )( t | ( c >> 4 ) );
            t = ( u_int8_t )( c << 4 );
			printf("-------decode :%c----\n",result);
            break;
        case 2 :
            result = ( u_int8_t )( t | ( c >> 2 ) );
            t = ( u_int8_t )( c << 6 );
			printf("-------decode :%c----\n",result);
            break;
        case 3 :
            result = ( u_int8_t )( t | c );
			printf("-------decode :%c----\n",result);
            break;
        }
		counter++;

    }
    return 1;
}


int main()
{
	char pstr[]="UEsDBBQABgAIAAAAIQDnIQddcAEAANcFAAATAAgCW0NvbnRlbnRfVHlwZXNdLnhtbCCiBAIooAAC";
	//printf("--------sizeof:%u------\n",sizeof(pstr));
	base64Decode(pstr,sizeof(pstr)-1);
}

