#include "write.h"



void write(Chunk& c, Select& s, std::ifstream& ifs, std::ofstream& ofs){
    ifs.seekg(c.pos + 4);
    writev(c.header_len, 4);
    while(ifs.tellg() < c.header_end()){
        long long start = ifs.tellg();
        int field_len;
        readv(field_len, 4);
        writev(field_len, 4);
        std::string field_name = read_name(ifs);
        write_name(field_name, ofs);
        if(field_name == "size"){
            writev(c.new_size, 4);
        } else{
            while((long long)ifs.tellg() < start + field_len){
                char c;
                readv(c, 1);
                writev(c, 1);
            }
        }
    }
    for(unsigned int i = 0; i < c.conns.size(); i++){
        if(c.connections[c.conns[i]].id.new_count != 0){
            write(c.connections[c.conns[i]], s, ifs, ofs);
        }
    }
}


void write(Connection& c, Select& s, std::ifstream& ifs, std::ofstream& ofs){
    ifs.seekg(c.pos);
    while(ifs.tellg() < c.data_end()){
        char sym;
        readv(sym, 1);
        writev(sym, 1);
    }
}

void write_all(std::ifstream& ifs, std::ofstream& ofs){
    int header_len;
    readv(header_len, 4);
    long long start = ifs.tellg();
    while(ifs.tellg() < header_len + start){
        char c;
        readv(c, 1);
        writev(c, 1);
    }
    int data_len;
    readv(data_len, 4);
    long long data_start = ifs.tellg();
    while(ifs.tellg() < data_len + data_start){
        char c;
        readv(c, 1);
        writev(c, 1);
    }
}

void write_messages(Connection& c, Select& s, std::ifstream& ifs, std::ofstream& ofs){
    ifs.seekg(c.id.data_start());
    for(int i = 0; i < c.id.count; i++){
        int offset;
        readv(offset, 4);
        long long current_pos = ifs.tellg();
        if(s.valid.find(offset) != s.valid.end()){
            ifs.seekg(offset); //Chunk pos Requested
            write_all(ifs, ofs);
        }
        ifs.seekg(current_pos + 8);
    }
}

void write(IndexData& id, Select& s, std::ifstream& ifs, std::ofstream& ofs){

}



