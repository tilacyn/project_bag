#include "write.h"

using namespace std;

BagFile bf;

char* first_symbols = new char[20];

void read_records(const char* in_file){
    std::ifstream ifs(in_file, std::ios::binary);
    readv(first_symbols, 13);
    while(ifs.peek() != std::ifstream::traits_type::eof()){
        char op = read_op(ifs);
        if(op == 0x02){
            MessageData md;
            ifs >> md;
        } else if(op == 0x04){
            IndexData id;
            ifs >> id;
            cout << "Index Data: " << id.conn << " " << id.header_len << "\n";
            assert(!bf.chunks.empty());
            bf.chunks[bf.chunks.size() - 1].indexdata[id.conn] = id;
        } else if(op == 0x06){
            ChunkInfo ci;
            ifs >> ci;
            cout << "Chunk Info: " << ci.data_end() << " " << ci.pos << "\n";
            bf.chunkinfo.push_back(ci);
        } else if(op == 0x03){
            ifs >> bf.bh;
            cout << "Bag Header: " << bf.bh.data_end() << "\n";
        } else if(op == 0x05){
            Chunk c;
            ifs >> c;
            cout << "Chunk: " << c.header_len << " " << c.data_len << " " << c.pos << "\n";
            bf.chunks.push_back(c);
        } else if(op == 0x07){
            Connection c;
            ifs >> c;
            cout << "Connection: " << c.conn << " " << c.topic << "\n";
            //bf.connections.push_back(c);
            bf.map_conn[c.conn] = c;
            bf.map_conn[c.conn].has_any_suitable_messages = false;
        }
    }
    for(std::map<int, IndexData>::iterator i = bf.chunks[0].indexdata.begin(); i != bf.chunks[0].indexdata.end(); i++){
        //cout << i->first << " " << i->second.count << "\n";
    }
    cout << "Reading finished\n";
    seq_chunk_to_info(bf.chunks, bf.chunkinfo);
    ifs.close();
}

void select_data(const char* in_file, long long time_start, long long time_end, string topic){
    ifstream ifs(in_file, ios::binary);
    make_map(time_start, time_end, topic, bf, ifs);
    cout << "Map Made\n";
    ifs.close();
}

void write_file(const char* in_file, const char* out_file){
    ifstream ifs(in_file, ios::binary);
    ofstream ofs(out_file, ios::binary);

    writev(first_symbols, 13);
    write(bf.bh, ifs, ofs);
    write(bf.selects, bf.chunks, ifs, ofs);
    write(bf.map_conn, ifs, ofs);
    write_all_the_chunk_info(bf.chunks, ifs, ofs);
    cout << "New file created\n";
}

string make_str(char* arr){
    string s = "";
    for(unsigned int i = 0; i < strlen(arr); i++){
        s += arr[i];
    }
    return s;
}


int main(int argc, char* argv[]){
    char* in_file = new char[100];
    char* out_file = new char[100];
    long long time_start = 0;
    long long time_end = 1e16;
    string topic;
    for(int i = 1; i < argc; i++){
        if(!strcmp(argv[i], "--in")){
            strcpy(in_file, argv[++i]);
        }else if(!strcmp(argv[i], "--out")){
            strcpy(out_file, argv[++i]);
        } else if(!strcmp(argv[i], "--time")){
            time_start = atoll(argv[++i]);
            time_end = atoll(argv[++i]);
        } else if(!strcmp(argv[i], "--topic")){
            topic = make_str(argv[++i]);
        }
    }


    read_records(in_file);
    select_data(in_file, time_start, time_end, "");
    write_file(in_file, out_file);
    delete[] in_file;
    delete[] out_file;
    return 0;
}
