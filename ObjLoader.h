#pragma once

#include "pch.h"
#include "easymath.h"
#include "teximage.h"

typedef struct {

	int numTriangles;
	std::string diffuse_tex_name;
	std::vector<float> buffer;
} DrawObject;

class ObjLoader
{
public:
	ObjLoader() : m_bLoaded(false) {}
	virtual ~ObjLoader() { Close(); }
	virtual void Close()
	{
		for each (auto o in drawObjects)
		{
			o.diffuse_tex_name.clear();
			o.buffer.clear();
		}
		drawObjects.clear();
		texDatas.clear();
		m_bLoaded = false;
	}

	virtual bool Load(const std::string& objname);
	virtual void Load(const std::wstring& objname);
//protected:
	std::vector<DrawObject> drawObjects;
	std::map<std::string, TexImage> texDatas;

	easymath::vector3 bmin;
	easymath::vector3 bmax;

	bool m_bLoaded;
protected:
	void buildMaterialTexMap(const std::string& texname, const std::string& base_dir);
};