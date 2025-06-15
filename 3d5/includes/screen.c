#include "screen.h"

int indexing(int max_w, int x, int y)
{
    return x + max_w*y;
}

Color add_lb(Color A, Color B)
{
    Color C = {A.r+B.r, A.g+B.g, A.b+B.b};
    return C;
}

Color mul_lb(Color A, Color B)
{
    Color C = {A.r*B.r, A.g*B.g, A.b*B.b};
    return C;
}

Color div_lb(Color A, Color B)
{
    Color C = {A.r/B.r, A.g/B.g, A.b/B.b};
    return C;
}
