#pragma once

#include <vector>

class Vbo {
public:
    Vbo();
    ~Vbo() {}

    unsigned int m_id = 0;

public:
    void addData(const void* data, unsigned int size);
    void bind();
    void unbind();
};