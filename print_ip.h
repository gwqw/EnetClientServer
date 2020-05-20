/**
 * @file print_ip.h
 * @author gw
 * @brief print ip from int-value
*/
#pragma once
#ifndef ENET_TEST_PRINT_IP_H
#define ENET_TEST_PRINT_IP_H

#include <iostream>

/** @brief prints ip as integer number like "192.168.1.1" (split in bytes)
 */
void print_ip(std::ostream& out, int ip_address);

#endif //ENET_TEST_PRINT_IP_H
