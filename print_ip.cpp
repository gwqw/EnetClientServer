#include "print_ip.h"

void print_ip(std::ostream& out, int ip_address) {
    auto p = reinterpret_cast<unsigned char*>(&ip_address);
    auto i = p + sizeof(int);
    bool is_first = true;
    while (i != p) {
        --i;
        if (is_first) {
            is_first = false;
        } else {
            out << '.';
        }
        out << int(*i);
    }
}