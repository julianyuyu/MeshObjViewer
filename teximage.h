#pragma once


class TexImage
{
public:
	TexImage() : m_data(nullptr), m_w(0), m_h(0), m_bpp(0) {}
	~TexImage() { Release(); }
	virtual void Init() {}
	virtual bool Load(const std::string& filename, const std::string& base_dir);
	virtual void Release();
//protected:
	void *m_data;
	int m_w, m_h;
	int m_bpp;
};
