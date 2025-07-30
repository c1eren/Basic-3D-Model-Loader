#pragma once

class Vao {
public:
	unsigned int m_id = 0;

public:
	Vao();
	~Vao() {}

	void setLayout(bool positions = 1, bool normal = 1, bool texture = 1, bool color = 0, bool block = 0);

	void bind();
	void unbind();
};