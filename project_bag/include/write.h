#include "select.h"

inline void write(const MessageData& md, std::ifstream& ifs, std::ofstream& ofs){
    ifs.seekg(md.pos + 4);
    writev(md.header_len, 4);
    for(int i = 0; i < md.header_len; i++){
        char c;
        readv(c, 1);
        writev(c, 1);
    }
    writev(md.data_len, 4);
    ifs.seekg((long long) ifs.tellg() + 4);
    for(int i = 0; i < md.data_len; i++){
        char c;
        readv(c, 1);
        writev(c, 1);
    }
}

void write(Connection& c, Select& s, std::ifstream& ifs, std::ofstream& ofs);

void write(Chunk& c, Select& s, std::ifstream& ifs, std::ofstream& ofs);

void write(IndexData& id, Select& s, std::ifstream& ifs, std::ofstream& ofs);

void write_all(std::ifstream& ifs, std::ofstream& ofs);

void write_messages(Connection& c, Select& s, std::ifstream& ifs, std::ofstream& ofs);
