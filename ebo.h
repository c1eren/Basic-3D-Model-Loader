#pragma once

class Ebo {
public:
    Ebo();
    ~Ebo();

    unsigned int m_id = 0;

public:
    void addData(const void* data, unsigned int size);
    void bind();
    void unbind();
};