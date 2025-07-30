#pragma once

class Vbo {
public:
    unsigned int m_id = 0;

public:
    Vbo();
    ~Vbo();

    void addData(const void* data, unsigned int size);
    void bind();
    void unbind();
};