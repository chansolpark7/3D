#include "geometry.h"
#include "screen.h"

int indexing(int max_w, int x, int y)
{
    return x + max_w*y;
}

Brightness add_lb(Brightness A, Brightness B)
{
    Brightness C = {A.r+B.r, A.g+B.g, A.b+B.b};
    return C;
}

Brightness mul_lb(Brightness A, Brightness B)
{
    Brightness C = {A.r*B.r, A.g*B.g, A.b*B.b};
    return C;
}

Brightness div_lb(Brightness A, Brightness B)
{
    Brightness C = {A.r/B.r, A.g/B.g, A.b/B.b};
    return C;
}
