#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <string>
#include <map>
#include <vector>
#include <regex>

#define readv(c, n) ifs.read((char*)&c, n)
#define writev(c, n) ofs.write((char*)&c, n)

class Record{
public:
    long long pos;
    int header_len;
    int data_len;
    int new_data_len;
    void skip_data(std::ifstream& ifs);
    void skip_header(std::ifstream& ifs);
    long long data_end();
    long long data_start();
    long long header_end();
    long long record_len(std::ifstream& ifs);
};

char read_op(std::ifstream& ifs);

int read_conn(std::ifstream& ifs);

std::string make_str(char* arr);

std::string read_name(std::ifstream& ifs);

void write_name(std::string str, std::ofstream& ofs);

std::string read_str(std::ifstream& ifs);

class BagHeader : public Record{
public:
    long long index_pos;
    long long new_index_pos;
    int conn_count;
    int chunk_count;
    int new_conn_count;
    int new_chunk_count;
    friend std::ifstream& operator>>(std::ifstream& ifs, BagHeader& bh){
        bh.pos = ifs.tellg();
        readv(bh.header_len, 4);
        std::streampos start = ifs.tellg();
        while(bh.header_len + start > ifs.tellg()){
            int field_len;
            readv(field_len, 4);
            std::string field_name = read_name(ifs);
            if(field_name == "index_pos"){
                readv(bh.index_pos, 8);
            } else if(field_name == "conn_count"){
                readv(bh.conn_count, 4);
            } else if(field_name == "chunk_count"){
                readv(bh.chunk_count, 4);
            } else ifs.seekg((long long) ifs.tellg() + field_len - field_name.length() - 1);
        }
        bh.skip_data(ifs);
        return ifs;
    }
};


class MessageData : public Record{
public:
    int conn;
    long long time;
    friend std::ifstream& operator>>(std::ifstream& ifs, MessageData& md){
        md.pos = ifs.tellg();
        readv(md.header_len, 4);
        std::streampos start = ifs.tellg();
        while(md.header_len + start > ifs.tellg()){
            int field_len;
            readv(field_len, 4);
            std::string field_name = read_name(ifs);
            if(field_name == "time"){
                readv(md.time, 8);
            } else if(field_name == "conn"){
                readv(md.conn, 4);
            } else ifs.seekg((long long) ifs.tellg() + field_len - field_name.length() - 1);
        }
        md.skip_data(ifs);
        return ifs;
    }
};

class IndexData : public Record{
public:
    int ver;
    int conn;
    int count;
    int new_count;
    long long new_size;
    friend std::ifstream& operator>>(std::ifstream& ifs, IndexData& id){
        id.pos = ifs.tellg();
        readv(id.header_len, 4);
        std::streampos start = ifs.tellg();
        while(id.header_len + start > ifs.tellg()){
            int field_len;
            readv(field_len, 4);
            std::string field_name = read_name(ifs);
            if(field_name == "ver"){
                readv(id.ver, 4);
            } else if(field_name == "conn"){
                readv(id.conn, 4);
            } else if(field_name == "count"){
                readv(id.count, 4);
            } else ifs.seekg((long long) ifs.tellg() + field_len - field_name.length() - 1);
        }
        id.skip_data(ifs);
        return ifs;
    }
    bool operator<(const IndexData& other){
        return conn < other.conn;
    }
};


class Connection : public Record{
public:
    //IndexData id;
    std::string topic;
    int conn;
    bool has_any_suitable_messages;
    friend std::ifstream& operator>>(std::ifstream& ifs, Connection& c){
        c.pos = ifs.tellg();
        readv(c.header_len, 4);
        std::streampos start = ifs.tellg();
        while(c.header_len + start > ifs.tellg()){
            int field_len;
            readv(field_len, 4);
            std::string field_name = read_name(ifs);
            if(field_name == "topic"){
                c.topic = read_str(ifs);
            } else if(field_name == "conn"){
                readv(c.conn, 4);
            } else ifs.seekg((long long) ifs.tellg() + field_len - field_name.length() - 1);
        }
        ifs.seekg((long long) start + c.header_len);
        c.skip_data(ifs);
        int kek;
        readv(kek, 4);
        ifs.seekg((long long)start + 4 + c.header_len + c.data_len);
        return ifs;
    }
};


class ChunkInfo : public Record{
public:
    int ver;
    long long chunk_pos;
    long long start_time;
    long long end_time;
    long long new_chunk_pos;
    long long new_start_time;
    long long new_end_time;
    int count;
    int new_count;
    friend std::ifstream& operator>>(std::ifstream& ifs, ChunkInfo& ci){
        ci.pos = ifs.tellg();
        readv(ci.header_len, 4);
        std::streampos start = ifs.tellg();
        while(ci.header_len + start > ifs.tellg()){
            int field_len;
            readv(field_len, 4);
            std::string field_name = read_name(ifs);
            if(field_name == "ver"){
                readv(ci.ver, 4);
            } else if(field_name == "count"){
                readv(ci.count, 4);
            } else if(field_name == "chunk_pos"){
                readv(ci.chunk_pos, 8);
            } else if(field_name == "start_time"){
                readv(ci.start_time, 8);
            } else if(field_name == "end_time"){
                readv(ci.end_time, 8);
            } else ifs.seekg((long long) ifs.tellg() + field_len - field_name.length() - 1);
        }
        ci.skip_data(ifs);
        return ifs;
    }
};





class Chunk : public Record{
public:
    ChunkInfo ci;
    std::string compression;
    int size;
    int new_size;
    bool has_any_suitable_messages;
    std::map <int, IndexData> indexdata;
    long long new_index_data_size;

    friend std::ifstream& operator>>(std::ifstream& ifs, Chunk& c){
        c.pos = (long long) ifs.tellg();
        readv(c.header_len, 4);
        std::streampos start = ifs.tellg();
        while(c.header_len + start > ifs.tellg()){
            int field_len;
            readv(field_len, 4);
            std::string field_name = read_name(ifs);
            if(field_name == "compression"){
                char first;
                readv(first, 1);
                if(first == 'b'){
                    ifs.seekg((long long) ifs.tellg() + 2);
                    c.compression = "bz2";
                } else if(first == 'n'){
                    ifs.seekg((long long) ifs.tellg() + 3);
                    c.compression = "none";
                }
            } else if(field_name == "size"){
                readv(c.size, 4);
            } else ifs.seekg((long long) ifs.tellg() + field_len - field_name.length() - 1);
        }
        c.skip_data(ifs);
        return ifs;
    }
    //void seq_id_to_conn(std::ifstream& ifs);
};

bool compare_chunks(Chunk& c, long long offset);

void seq_chunk_to_info(std::vector<Chunk>& chunks, std::vector<ChunkInfo>& chunkinfo);

