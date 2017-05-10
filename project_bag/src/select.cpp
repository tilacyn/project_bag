#include "select.h"

void Select::sift_index_data(long long chunk_data_start, long long time_start, long long time_end, Connection& c, std::ifstream& ifs){
    c.new_size = 0;
    ifs.seekg(c.id.data_start() - 4);
    int kek;
    readv(kek, 4);
    std::cout << c.id.count << "\n";
    for(int i = 0; i < c.id.count; i++){
        long long time;
        readv(time, 8);
        int offset;
        readv(offset, 4);
        if(time <= time_end && time >= time_start){
            //std::cout << ifs.tellg() << "\n";
            new_offset[offset] = c.next_message_offset + c.new_size;
            std::streampos current_pos = ifs.tellg();
            ifs.seekg((long long) offset + chunk_data_start); // go to the poin
            MessageData md;
            c.new_size += md.record_len(ifs);
            ifs.seekg(current_pos);
            c.id.new_count++;
        }
    }
    assert(ifs.tellg() == c.id.data_end());
}


void Select::make_map(long long time_start, long long time_end, std::string topic, Chunk& c, std::ifstream& ifs){
    long long offset = 0;
    c.new_index_data_size = 0;
    c.ci.new_count = 0;
    for(unsigned int i = 0; i < c.conns.size(); i++){
        if(topic != c.connections[c.conns[i]].topic && topic != "") continue;
        std::cout << "This index data is gonna be sifted\n";
        // Посчитаем оффсет этого коннекшена
        offset += c.connections[c.conns[i]].data_end() - c.connections[c.conns[i]].pos;
        c.connections[c.conns[i]].next_message_offset = offset;
        sift_index_data(c.data_start(), time_start, time_end, c.connections[c.conns[i]], ifs);
        // Посчитаем новый размер id.data
        c.connections[c.conns[i]].id.new_data_len = c.connections[c.conns[i]].id.new_count * 12;
        // Посчитаем ci.count
        if(c.connections[c.conns[i]].id.new_count != 0){
            c.ci.new_count++;
        }
        std::cout << "Another connection id.new_count: " << c.connections[c.conns[i]].id.new_count << "\n";
        c.new_index_data_size += c.connections[c.conns[i]].id.new_data_len + 8 + c.connections[c.conns[i]].id.header_len;
        offset += c.connections[c.conns[i]].new_size;
    }
    c.new_data_len = offset;
    std::cout << "New Data len: " << c.new_data_len << "\n";
    std::cout << "New ci count: " << c.ci.new_count << "\n";
}

void make_map(long long time_start, long long time_end, std::string topic, std::vector<Chunk>& chunks, std::vector<Select>& selects,
               BagHeader& bh, std::ifstream& ifs){
    bh.new_index_pos = bh.data_end();
    for(unsigned int i = 0; i < chunks.size(); i++){
        chunks[i].ci.new_chunk_pos = bh.new_index_pos;
        std::cout << "Another Chunk in *MAKE_MAP*\n";
        Select s;
        s.make_map(time_start, time_end, topic, chunks[i], ifs);
        selects.push_back(s);
        if(chunks[i].ci.new_count != 0){
            bh.new_index_pos += chunks[i].new_size + chunks[i].header_len + 8 + chunks[i].new_index_data_size;
            bh.new_chunk_count++;
            bh.new_conn_count += chunks[i].ci.new_count;
        }
    }
}

