#include "select.h"

void write(BagHeader& bh, std::ifstream& ifs, std::ofstream& ofs);

void write(std::vector <Select>& selects, std::vector <Chunk>& chunks, std::ifstream& ifs, std::ofstream& ofs);

void write(long long chunk_data_start, Connection& c, Select& s, std::ifstream& ifs, std::ofstream& ofs);

void write(Chunk& c, Select& s, std::ifstream& ifs, std::ofstream& ofs);

void write(IndexData& id, Select& s, std::ifstream& ifs, std::ofstream& ofs);

long long write_all(std::ifstream& ifs, std::ofstream& ofs);

void write_messages(long long chunk_data_start, Connection& c, Select& s, std::ifstream& ifs, std::ofstream& ofs);

void write_chunk_info(Chunk& c, std::ifstream& ifs, std::ofstream& ofs);

void write_all_the_chunk_info(std::vector <Chunk>& chunks, std::ifstream& ifs, std::ofstream& ofs);

void write(std::map<int, Connection>& map_conn, std::ifstream& ifs, std::ofstream& ofs);
