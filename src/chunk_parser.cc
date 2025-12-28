#include "chunk_parser.h"
#include <algorithm>

ChunkParser::ChunkParser() : totalChunks_(0) {
}

void ChunkParser::setTotalChunks(size_t totalChunks) {
    totalChunks_ = totalChunks;
    chunks_.resize(totalChunks_);
    received_.assign(totalChunks_, false);
}

void ChunkParser::addChunk(size_t chunkIndex, size_t offset, const std::string& data) {
    if (totalChunks_ == 0 || chunkIndex >= totalChunks_) return;
    chunks_[chunkIndex] = data;
    received_[chunkIndex] = true;
}

bool ChunkParser::hasChunk(size_t chunkIndex) const {
    if (totalChunks_ == 0 || chunkIndex >= totalChunks_) return false;
    return received_[chunkIndex];
}

std::vector<size_t> ChunkParser::getReceivedChunks() const {
    std::vector<size_t> res;
    for (size_t i = 0; i < received_.size(); ++i) {
        if (received_[i]) res.push_back(i);
    }
    return res;
}

std::string ChunkParser::mergeChunks() const {
    std::string result;
    for (const auto& chunk : chunks_) {
        result += chunk;
    }
    return result;
}

size_t ChunkParser::getTotalChunks() const {
    return totalChunks_;
}

bool ChunkParser::isComplete() const {
    return std::all_of(received_.begin(), received_.end(), [](bool v) { return v; });
}

void ChunkParser::reset() {
    totalChunks_ = 0;
    chunks_.clear();
    received_.clear();
}
