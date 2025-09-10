#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <cassert>

namespace MiniEvent {

class Buffer {
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize)
        : buffer_(kCheapPrepend + initialSize),
          readIndex_(kCheapPrepend),
          writeIndex_(kCheapPrepend) {}

    // 可读、可写、预留空间大小
    size_t readableBytes() const { return writeIndex_ - readIndex_; }
    size_t writableBytes() const { return buffer_.size() - writeIndex_; }
    size_t prependableBytes() const { return readIndex_; }

    // 返回可读数据的起始位置
    const char* peek() const { return begin() + readIndex_; }

    // 返回可写缓冲区的起始位置
    char* beginWrite() { return begin() + writeIndex_; }
    const char* beginWrite() const { return begin() + writeIndex_; }

    // 从缓冲区中“取出”数据，实际上只是移动指针
    void retrieve(size_t len) {
        assert(len <= readableBytes());
        if (len < readableBytes()) {
            readIndex_ += len;
        } else {
            retrieveAll();
        }
    }

    void retrieveAll() {
        readIndex_ = kCheapPrepend;
        writeIndex_ = kCheapPrepend;
    }

    std::string retrieveAllAsString() {
        return retrieveAsString(readableBytes());
    }

    std::string retrieveAsString(size_t len) {
        assert(len <= readableBytes());
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    // 向缓冲区追加数据
    void append(const char* data, size_t len) {
        ensureWritableBytes(len);
        std::copy(data, data + len, beginWrite());
        hasWritten(len);
    }

    // 确保有足够的可写空间
    void ensureWritableBytes(size_t len) {
        if (writableBytes() < len) {
            makeSpace(len);
        }
        assert(writableBytes() >= len);
    }

    // 更新写指针
    void hasWritten(size_t len) {
        assert(len <= writableBytes());
        writeIndex_ += len;
    }

    // 从文件描述符读取数据
    ssize_t readFd(int fd, int* savedErrno);

private:
    char* begin() { return &*buffer_.begin(); }
    const char* begin() const { return &*buffer_.begin(); }

    // 为写入腾出空间
    void makeSpace(size_t len) {
        // 当 [可写空间] + [已读过的空间] < 需要的空间时，必须扩容
        if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
            buffer_.resize(writeIndex_ + len);
        } else {
            // 否则，只需将可读数据前移，复用 prependable 空间
            size_t readable = readableBytes();
            std::copy(begin() + readIndex_,
                      begin() + writeIndex_,
                      begin() + kCheapPrepend);
            readIndex_ = kCheapPrepend;
            writeIndex_ = readIndex_ + readable;
            assert(readable == readableBytes());
        }
    }

private:
    std::vector<char> buffer_;
    size_t readIndex_;
    size_t writeIndex_;
};

} // namespace MiniEvent
