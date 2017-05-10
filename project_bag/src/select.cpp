#include "select.h"

void Select::sift_index_data(long long time_start, long long time_end, Connection& c, std::ifstream& ifs){
    ifs.seekg(c.id.data_start());
    std::cout << c.id.count << "\n";
    for(int i = 0; i < c.id.count; i++){
        int offset;
        readv(offset, 4);
        long long time;
        readv(time, 8);
        //std::cout << time << "\n";
        if(time <= time_end && time >= time_start){
            std::cout << "WOW\n";
            valid[offset] = true;
            MessageData md;
            std::streampos current_pos = ifs.tellg();
            ifs.seekg((long long) offset + md.data_start()); // go to the poin
            c.new_size += md.record_len(ifs);
            ifs.seekg(current_pos);
            c.id.new_count++;
        }
    }
    assert((long long) ifs.tellg() == c.id.data_end());
}


void Select::make_map(long long time_start, long long time_end, std::string topic, Chunk& c, std::ifstream& ifs){
    for(unsigned int i = 0; i < c.conns.size(); i++){
        if(topic != c.connections[c.conns[i]].topic && topic != "") continue;
        std::cout << "This index data is gonna be sifted\n";
        sift_index_data(time_start, time_end, c.connections[c.conns[i]], ifs);
    }
}

void Select::make_map(long long time_start, long long time_end, std::string topic, std::vector<Chunk>& chunks, std::ifstream& ifs){
    for(unsigned int i = 0; i < chunks.size(); i++){
        std::cout << "Another Chunk in *MAKE_MAP*\n";
        make_map(time_start, time_end, topic, chunks[i], ifs);
    }
}

