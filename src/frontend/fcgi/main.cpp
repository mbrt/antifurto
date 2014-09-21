#include <fcgi_stdio.h>

int main(int, char*[])
{
    while (FCGI_Accept() >= 0) {

    }
    return 0;
}
