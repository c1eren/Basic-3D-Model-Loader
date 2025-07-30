#pragma once

class Ebo {
public:
    unsigned int m_id = 0;

public:
    Ebo();
    ~Ebo();

    void addData(const void* data, unsigned int size);
    void bind();
    void unbind();
};