#include "write.h"


void write(BagHeader& bh, std::ifstream& ifs, std::ofstream& ofs){
    writev(bh.header_len, 4);
    ifs.seekg(bh.pos + 4);
    long long curpos = ifs.tellg();
    while(ifs.tellg() < curpos + bh.header_end()){
        int field_len;
        readv(field_len, 4);
        long long cur_pos = ifs.tellg();
        std::string field_name = read_name(ifs);
        write_name(field_name, ofs);
        if(field_name == "index_pos"){
            writev(bh.new_index_pos, 8);
        } else if(field_name == "chunk_count"){
            writev(bh.new_chunk_count, 4);
        } else if(field_name == "conn_count"){
            writev(bh.new_conn_count, 4);
        } else {
            while(ifs.tellg() < field_len + cur_pos){
                char sym;
                readv(sym, 1);
                writev(sym, 1);
            }
        }
        ifs.seekg(cur_pos + field_len);
    }
    writev(bh.data_len, 4);
    ifs.seekg((long long)ifs.tellg() + 4);
    curpos = ifs.tellg();
    while(ifs.tellg() < curpos + bh.data_len){
        char c;
        readv(c, 1);
        writev(c, 1);
    }
}

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
            while((long long)ifs.tellg() < start + field_len + 4){
                //std::cout << "kek\n";
                char sym;
                readv(sym, 1);
                writev(sym, 1);
            }
        }
    }
    writev(c.data_len, 4);
    for(unsigned int i = 0; i < c.conns.size(); i++){
        if(c.connections[c.conns[i]].id.new_count != 0){
            write(c.data_start(), c.connections[c.conns[i]], s, ifs, ofs);
        }
    }
    for(unsigned int i = 0; i < c.conns.size(); i++){
        if(c.connections[c.conns[i]].id.new_count != 0){
            write(c.connections[c.conns[i]].id, s, ifs, ofs);
        }
    }
}


void write(long long chunk_data_start, Connection& c, Select& s, std::ifstream& ifs, std::ofstream& ofs){
    ifs.seekg(c.pos);
    while(ifs.tellg() < c.data_end()){
        char sym;
        readv(sym, 1);
        writev(sym, 1);
    }
    write_messages(chunk_data_start, c, s, ifs, ofs);
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

void write_messages(long long chunk_data_start, Connection& c, Select& s, std::ifstream& ifs, std::ofstream& ofs){
    ifs.seekg(c.id.data_start());
    for(int i = 0; i < c.id.count; i++){
        long long time;
        readv(time, 8);
        int offset;
        readv(offset, 4);
        if(s.new_offset.find(offset) != s.new_offset.end()){
            ifs.seekg(offset + chunk_data_start); //Chunk pos Requested
            write_all(ifs, ofs);
        }
    }
}

void write(IndexData& id, Select& s, std::ifstream& ifs, std::ofstream& ofs){
    ifs.seekg(id.pos + 4);
    writev(id.header_len, 4);
    while(ifs.tellg() < id.header_end()){
        int field_len;
        readv(field_len, 4);
        writev(field_len, 4);
        long long start = ifs.tellg();
        std::string field_name = read_name(ifs);
        write_name(field_name, ofs);
        if(field_name == "count"){
            writev(id.new_count, 4);
            ifs.seekg((long long)ifs.tellg() + 4);
        } else{
            while((long long)ifs.tellg() < start + field_len){
                char c;
                readv(c, 1);
                writev(c, 1);
            }
        }
    }
    ifs.seekg((long long)ifs.tellg() + 4);
    long long cur_pos = ifs.tellg();
    writev(id.new_data_len, 4);
    while((long long)ifs.tellg() < cur_pos + id.data_len){
        long long time;
        readv(time, 8);
        int offset;
        readv(offset, 4);
        if(s.new_offset.find(offset) != s.new_offset.end()){
            writev(time, 8);
            writev(s.new_offset[offset], 4);
        }
    }
}

void write(std::vector <Select>& selects, std::vector <Chunk>& chunks, std::ifstream& ifs, std::ofstream& ofs){
    for(unsigned int i = 0; i < chunks.size(); i++){
        write(chunks[i], selects[i], ifs, ofs);
    }
}

void write_chunk_info(Chunk& c, std::ifstream& ifs, std::ofstream& ofs){
    writev(c.ci.header_len, 4);
    ifs.seekg(c.ci.pos + 4);
    while(ifs.tellg() < c.ci.header_end()){
        int field_len;
        readv(field_len, 4);
        long long cur_pos = ifs.tellg();
        std::string field_name = read_name(ifs);
        write_name(field_name, ofs);
        if(field_name == "chunk_pos"){
            writev(c.ci.new_chunk_pos, 8);
        } else if(field_name == "count"){
            writev(c.ci.new_count, 4);
        } else {
            while(ifs.tellg() < field_len + cur_pos){
                char sym;
                readv(sym, 1);
                writev(sym, 1);
            }
        }
        ifs.seekg(cur_pos + field_len);
    }
    c.ci.new_data_len = c.ci.new_count * 8;
    writev(c.ci.new_data_len, 4);
    ifs.seekg((long long)ifs.tellg() + 4);
    long long curpos = ifs.tellg();
    while(ifs.tellg() < curpos + c.ci.data_len){
        int conn;
        readv(conn, 4);
        if(c.connections[conn].id.new_count != 0){
            writev(conn, 4);
            writev(c.connections[conn].id.new_count, 4);
        }
    }
}

void write_all_the_chunk_info(std::vector <Chunk>& chunks, std::ifstream& ifs, std::ofstream& ofs){
    for(unsigned int i = 0; i < chunks.size(); i++){
        write_chunk_info(chunks[i], ifs, ofs);
    }
}

