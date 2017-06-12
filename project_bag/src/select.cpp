#include "select.h"

void Select::sift_index_data(long long chunk_data_start, long long time_start,
                             long long time_end, std::string msg_regex, IndexData& id, std::ifstream& ifs){
    id.new_size = 0;
    ifs.seekg(id.data_start());
    for(int i = 0; i < id.count; i++){
        long long time;
        readv(time, 8);
        int offset;
        readv(offset, 4);
        if(/*time <= time_end && time >= time_start*/ true){
            std::streampos current_pos = ifs.tellg();
            ifs.seekg((long long) offset + chunk_data_start); // go to the point
            MessageData md;

            md.skip_header(ifs);
            readv(md.data_len, 4);
            char* tmp = new char[md.data_len];
            for(int j = 0; j < md.data_len; j++) readv(tmp[j], 1);
            //std::cout << "msg_regex: " << msg_regex << " md.data_len: " << md.data_len << "\n";
            std::string msg_data_str = make_str(tmp);
            std::regex rgx(msg_regex);
            if(std::regex_search(msg_data_str.begin(), msg_data_str.end(), rgx)){
                new_offset[offset] = 1;
                id.new_size += md.data_len + md.header_len + 8;
                id.new_count++;
            }
            ifs.seekg(current_pos);
        }
    }
}


void Select::make_map(long long time_start, long long time_end, std::string topic, bool topic_regex,
                      std::string msg_regex, Chunk& c,
                      std::map<int, Connection>& map_conn, std::ifstream& ifs){

    c.new_index_data_size = c.ci.new_count = c.new_size = c.new_data_len = 0;
    for(std::map<int, IndexData>::iterator i = c.indexdata.begin(); i != c.indexdata.end(); i++){
        IndexData& id = i->second;
        id.new_size = id.new_count = 0;
        std::cout << "IndexData topic: " << map_conn[id.conn].topic << " Our topic: " << topic << "\n";

        // Делаем фильтрацию по топику

        if(!topic_regex){
            if(topic != map_conn[id.conn].topic && topic != "") continue;
        } else{
            std::regex rgx(topic);
            if(!std::regex_search(map_conn[id.conn].topic.begin(), map_conn[id.conn].topic.end(), rgx)) continue;
        }

        // Прошли фильтрацию, продолжаем фильтровать но уже по другим параметрам (sift_index_data)

        sift_index_data(c.data_start(), time_start, time_end, msg_regex, id, ifs);
        id.new_data_len = id.new_count * 12;
        std::cout << "MAKE MAP: " << id.conn << " " << id.new_count << " " << id.new_data_len << " " << id.data_len << "\n";
        if(id.new_count != 0){
            c.ci.new_count++;
            map_conn[id.conn].has_any_suitable_messages = true;
        }
        c.new_index_data_size += id.new_data_len + 8 + id.header_len;
        c.new_data_len += id.new_size;
    }
}

void sift_conn_in_chunk(Chunk& c, std::ifstream& ifs){
    ifs.seekg(c.data_start());
    while(ifs.tellg() < c.data_end()){
        char op = read_op(ifs);
        if(op == 0x07){
            Connection con;
            ifs >> con;
            if(c.indexdata[con.conn].new_count != 0){
                c.new_data_len += con.data_len + con.header_len + 8;
            }
        } else{
            Record r;
            r.skip_header(ifs);
            r.skip_data(ifs);
        }
    }
}


void make_map(long long time_start, long long time_end, std::string topic, bool topic_regex,
              std::string msg_regex, BagFile& bf, std::ifstream& ifs){
    bf.bh.new_index_pos = bf.bh.data_end();
    for(unsigned int i = 0; i < bf.chunks.size(); i++){
        bf.chunks[i].ci.new_chunk_pos = bf.bh.new_index_pos;
        Select s;
        s.make_map(time_start, time_end, topic, topic_regex, msg_regex, bf.chunks[i], bf.map_conn, ifs);
        bf.selects.push_back(s);
        sift_conn_in_chunk(bf.chunks[i], ifs);
        if(bf.chunks[i].ci.new_count != 0){
            bf.bh.new_index_pos += bf.chunks[i].new_data_len + bf.chunks[i].header_len + 8 + bf.chunks[i].new_index_data_size;
            bf.bh.new_chunk_count++;
            std::cout << "CHUNK New data len: " << bf.chunks[i].new_data_len << " Chunk old data_len: " << bf.chunks[i].data_len << "\n";
        }

    }
}

