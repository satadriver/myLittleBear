#pragma once

#include <windows.h>

#include <iostream>


std::string base64_decode(const std::string& encoded_string);

std::string base64_encode(const char* bytes_to_encode, unsigned int in_len);
