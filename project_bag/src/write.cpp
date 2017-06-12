#include "write.h"


void write(BagHeader& bh, std::ifstream& ifs, std::ofstream& ofs){
    writev(bh.header_len, 4);
    ifs.seekg(bh.pos + 4);
    while(ifs.tellg() < bh.header_end()){
        int field_len;
        readv(field_len, 4);
        writev(field_len, 4);
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
    while(ifs.tellg() < bh.data_end()){
        char c;
        readv(c, 1);
        writev(c, 1);
    }
}


long long write_all(std::ifstream& ifs, std::ofstream& ofs){
    int header_len;
    readv(header_len, 4);
    writev(header_len, 4);
    long long start = ifs.tellg();
    while(ifs.tellg() < header_len + start){
        char c;
        readv(c, 1);
        writev(c, 1);
    }
    int data_len;
    readv(data_len, 4);
    writev(data_len, 4);
    long long data_start = ifs.tellg();
    while(ifs.tellg() < data_len + data_start){
        char c;
        readv(c, 1);
        writev(c, 1);
    }
    return data_len + header_len + 8;
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
        } else{
            while((long long)ifs.tellg() < start + field_len){
                char c;
                readv(c, 1);
                writev(c, 1);
            }
        }
        ifs.seekg(start + field_len);
    }
    ifs.seekg((long long)ifs.tellg() + 4);
    writev(id.new_data_len, 4);
    while((long long)ifs.tellg() < id.data_end()){
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


void write(Chunk& c, Select& s, std::ifstream& ifs, std::ofstream& ofs){
    ifs.seekg(c.ci.chunk_pos + 4);
    writev(c.header_len, 4);

    while(ifs.tellg() < c.header_end()){
        long long start = ifs.tellg();
        int field_len;
        readv(field_len, 4);
        writev(field_len, 4);
        std::string field_name = read_name(ifs);
        write_name(field_name, ofs);
        if(field_name == "size"){
            std::cout << "WOOOOOWOWOWOW\n";
            writev(c.new_data_len, 4);
        } else{
            while((long long)ifs.tellg() < start + field_len + 4){
                //std::cout << "kek\n";
                char sym;
                readv(sym, 1);
                writev(sym, 1);
            }
        }
        ifs.seekg(start + field_len + 4);
    }
    writev(c.new_data_len, 4);
    ifs.seekg(c.data_start());
    long long curpos = 0;
    while(ifs.tellg() < c.data_end()){
        char op = read_op(ifs);
        if(op == 0x07){
            int conn = read_conn(ifs);
            if(c.indexdata[conn].new_count == 0){
                Record r;
                r.skip_data(ifs), r.skip_header(ifs);
                continue;
            }
            curpos += write_all(ifs, ofs);
        } else{
            if(s.new_offset.find(ifs.tellg() - c.data_start()) != s.new_offset.end()){
                s.new_offset[ifs.tellg() - c.data_start()] = curpos;
                curpos += write_all(ifs, ofs);
            } else{
                Record r;
                r.skip_data(ifs), r.skip_header(ifs);
                continue;
            }
        }
    }

    for(std::map<int, IndexData>::iterator i = c.indexdata.begin(); i != c.indexdata.end(); i++){
        IndexData& id = i->second;
        if(id.new_count == 0) continue;
        write(id, s, ifs, ofs);
    }
}


void write(std::map<int, Connection>& map_conn, std::ifstream& ifs, std::ofstream& ofs){
    for(std::map<int, Connection>::iterator i = map_conn.begin(); i != map_conn.end(); i++){
        if(i->second.has_any_suitable_messages){
            ifs.seekg(i->second.pos);
            write_all(ifs, ofs);
        }
    }
}


void write(std::vector <Select>& selects, std::vector <Chunk>& chunks, std::ifstream& ifs, std::ofstream& ofs){
    for(unsigned int i = 0; i < chunks.size(); i++){
        if(chunks[i].ci.new_count != 0)
            write(chunks[i], selects[i], ifs, ofs);
    }
}

void write_chunk_info(Chunk& c, std::ifstream& ifs, std::ofstream& ofs){
    writev(c.ci.header_len, 4);
    ifs.seekg(c.ci.pos + 4);
    while(ifs.tellg() < c.ci.header_end()){
        int field_len;
        readv(field_len, 4);
        writev(field_len, 4);
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
    while(ifs.tellg() != c.ci.data_end()){
        int conn, old_count;
        readv(conn, 4);
        readv(old_count, 4);
        if(c.indexdata[conn].new_count != 0){
            writev(conn, 4);
            writev(c.indexdata[conn].new_count, 4);
        }
    }
}

void write_all_the_chunk_info(std::vector <Chunk>& chunks, std::ifstream& ifs, std::ofstream& ofs){
    for(unsigned int i = 0; i < chunks.size(); i++){
        if(chunks[i].ci.new_count != 0)
            write_chunk_info(chunks[i], ifs, ofs);
    }
}

