#include <stdio.h>
#include <stdlib.h>

/*
 * 
 * REQUEST_METHOD   - тип запроса, GET или POST
 * QUERY_STRING     - данные запроса GET после ?
 * CONTENT_LENGTH   - размер данных метода POST, сами данные в stdin
 * 
 * все символы не принадлежащие к латинскому алфавиту и числам кодируются в 
 * виде %HH, где HH - шестнадцатеричное значение кода символа. Также кодируются все 
 * символы , которые нельзя использовать, т.е. !#%^&()=+ и пробел. Символ "&" 
 * используется, как мы уже видели для разделения пар "имя=значение", "=" 
 * используется в парах "имя=значение", "%" для кодирования символов, "пробел" 
 * кодируется символом "+"(плюс), сам же плюс кодируется через "%",
 * 
 */

int main(int argc, char **argv) 

{
    enum envid {
                    GATEWAY_INTERFACE,
                    REQUEST_METHOD,
                    QUERY_STRING,
                    CONTENT_LENGTH,
                    HTTP_USER_AGENT,
                    CONTENT_TYPE,
                    HTTP_COOKIE,
                    HTTP_ACCEPT,
                    HTTP_ACCEPT_CHARSET,
                    HTTP_ACCEPT_ENCODING,
                    HTTP_ACCEPT_LANGUAGE,
                    HTTP_HOST,
                    PATH_INFO,
                    REMOTE_ADDR,
                    REMOTE_PORT,
                    REMOTE_HOST,
                    SCRIPT_NAME,
                    SERVER_NAME,
                    SERVER_PROTOCOL,
                    SERVER_SOFTWARE,
                    NCHOME
        
    };

    char* vars[] = {
                    "GATEWAY_INTERFACE",
                    "REQUEST_METHOD",
                    "QUERY_STRING",
                    "CONTENT_LENGTH",
                    "HTTP_USER_AGENT",
                    "CONTENT_TYPE",
                    "HTTP_COOKIE",
                    "HTTP_ACCEPT",
                    "HTTP_ACCEPT_CHARSET",
                    "HTTP_ACCEPT_ENCODING",
                    "HTTP_ACCEPT_LANGUAGE",
                    "HTTP_HOST",
                    "PATH_INFO",
                    "REMOTE_ADDR",
                    "REMOTE_PORT",
                    "REMOTE_HOST",
                    "SCRIPT_NAME",
                    "SERVER_NAME",
                    "SERVER_PROTOCOL",
                    "SERVER_SOFTWARE",
                    "NCHOME"
    };
    
    char* val;
    int valn = sizeof(vars) / sizeof(&vars);
    
    printf("Content-type: text/plain\n\n");
    
    for (int i = 0; i < valn; i++) {

        val = getenv( vars[i] );
        printf("%s = %s\n", vars[i], val);
      
    }
    
    return 0;
}

