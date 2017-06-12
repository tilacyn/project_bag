#include "records.h"

class Select{
public:
    std::map <int, int> new_offset;
    void make_map(long long time_start, long long time_end, std::string topic, bool topic_regex, std::string msg_regex, Chunk& c,
                  std::map<int, Connection>& map_conn, std::ifstream& ifs);
    void sift_index_data(long long chunk_data_start, long long time_start, long long time_end, std::string msg_regex,
                         IndexData& id, std::ifstream& ifs);
};

class BagFile{
public:
    std::vector<Chunk> chunks;
    //std::vector<Connection> connections;
    std::vector<Select> selects;
    BagHeader bh;
    std::vector<ChunkInfo> chunkinfo;
    std::map <int, Connection> map_conn;
};


// Чтобы сделать правильную chunk_new_data_len
void sift_conn_in_chunk(Chunk& c, std::ifstream& ifs);
void make_map(long long time_start, long long time_end, std::string topic, bool topic_regex,
              std::string msg_regex, BagFile& bf, std::ifstream& ifs);

