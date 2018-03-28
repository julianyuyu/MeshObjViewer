
#include "pch.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader\tiny_obj_loader.h"

#include "ObjLoader.h"
#include "teximage.h"

using namespace easymath;

class timerutil {
public:
	typedef DWORD time_t;

	timerutil() { ::timeBeginPeriod(1); }
	~timerutil() { ::timeEndPeriod(1); }

	void start() { t_[0] = ::timeGetTime(); }
	void end() { t_[1] = ::timeGetTime(); }

	time_t sec() { return (time_t)((t_[1] - t_[0]) / 1000); }
	time_t msec() { return (time_t)((t_[1] - t_[0])); }
	time_t usec() { return (time_t)((t_[1] - t_[0]) * 1000); }
	time_t current() { return ::timeGetTime(); }

private:
	DWORD t_[2];
};


static std::string getBaseDir(const std::string& filepath)
{
	std::string out="";
	auto pos = filepath.find_last_of("/\\");
	if (pos != std::string::npos)
	{
		out = filepath.substr(0, pos) + "\\";
	}

	if (out.empty())
	{
		out = ".\\";
	}
	return out;
}

static bool FileExists(const std::string& abs_filename) {
	bool ret;
	FILE* fp = fopen(abs_filename.c_str(), "rb");
	if (fp) {
		ret = true;
		fclose(fp);
	}
	else {
		ret = false;
	}

	return ret;
}

void computeSmoothingNormals(const tinyobj::attrib_t& attrib, const tinyobj::mesh_t &mesh,
	std::map<int, easymath::vector3>& smoothVertexNormals
) {
	// Check if `mesh_t` contains smoothing group id.
	bool contained = false;
	for (size_t i = 0; i < mesh.smoothing_group_ids.size(); i++)
	{
		if (mesh.smoothing_group_ids[i] > 0)
		{
			contained = true;
		}
	}
	if (!contained)
	{
		std::cout << "smoothingNormal not found" << std::endl;
		return;
	}

	smoothVertexNormals.clear();

	size_t curr_vtx_num = 0;
	for each (auto vtx_num in mesh.num_face_vertices)
	{
		assert(vtx_num == 3); // assume all the faces are triangle.

		tinyobj::index_t idx0 = mesh.indices[curr_vtx_num + 0];
		tinyobj::index_t idx1 = mesh.indices[curr_vtx_num + 1];
		tinyobj::index_t idx2 = mesh.indices[curr_vtx_num + 2];
		int idx[3] = { idx0.vertex_index, idx1.vertex_index, idx2.vertex_index };
		assert(min3way<int>(idx[0], idx[1], idx[2]) >= 0);

		vector3 vtx0, vtx1, vtx2;
		vtx0 = &(attrib.vertices[3 * idx[0]]);
		vtx1 = &(attrib.vertices[3 * idx[1]]);
		vtx2 = &(attrib.vertices[3 * idx[2]]);

		// Compute the normal of the face
		vector3 normal;
		calcNormal(normal, vtx0, vtx1, vtx2);

		// Add the normal to the three vertexes
		for (size_t i = 0; i < 3; ++i)
		{
			auto it = smoothVertexNormals.find(idx[i]);
			if (it != smoothVertexNormals.end())
			{
				it->second += normal;
			}
			else
			{
				smoothVertexNormals.insert(std::map<int, easymath::vector3>::value_type(idx[i], normal));
			}
		}

		curr_vtx_num += vtx_num;
	}

	// normalize
	for each (auto n in smoothVertexNormals)
	{
		normalize(n.second);
	}
}

void ObjLoader::buildMaterialTexMap(const std::string& texname, const std::string& base_dir)
{
	if (!texname.empty())
	{
		// Only load the texture if it is not already loaded
		if (texDatas.find(texname) == texDatas.end())
		{
			TexImage image;
			if (image.Load(texname, base_dir))
			{
				texDatas.insert(std::make_pair(texname, image));
			}
		}
	}
}

void ObjLoader::Load(const std::wstring& objname)
{
	char ansiName[MAX_PATH];
	size_t size = 0;
	wcstombs_s(&size, ansiName, MAX_PATH, objname.c_str(), objname.length());
	Load(ansiName);
}

bool ObjLoader::Load(const std::string& filename)
{
	if (m_bLoaded)
		Close();

	timerutil tm;

	tm.start();

	std::string base_dir = getBaseDir(filename);
	std::string err;
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str(), base_dir.c_str());
	if (!err.empty())
	{
		std::cerr << err << std::endl;
	}

	tm.end();

	if (!ret)
	{
		std::cerr << "Failed to load " << filename << std::endl;
		return false;
	}

	printf("Parsing time: %d [ms]\n", (int)tm.msec());

	printf("# of vertices  = %d\n", (int)(attrib.vertices.size()) / 3);
	printf("# of normals   = %d\n", (int)(attrib.normals.size()) / 3);
	printf("# of texcoords = %d\n", (int)(attrib.texcoords.size()) / 2);
	printf("# of materials = %d\n", (int)materials.size());
	printf("# of shapes    = %d\n", (int)shapes.size());

	// Append `default` material
	materials.push_back(tinyobj::material_t());

	// Load diffuse textures
	for (size_t m = 0; m < materials.size(); m++)
	{
		tinyobj::material_t& mp = materials[m];
		buildMaterialTexMap(mp.diffuse_texname, base_dir);
		printf("material[%d].diffuse_texname = %s\n", int(m), mp.diffuse_texname.c_str());
	}

	bmin.x = bmin.y = bmin.z = std::numeric_limits<float>::max();
	bmax.x = bmax.y = bmax.z = -std::numeric_limits<float>::max();

	for (size_t s = 0; s < shapes.size(); s++)
	{
		DrawObject o;
		std::vector<float> &buffer = o.buffer; // pos(3float), normal(3float), color(3float)
		tinyobj::mesh_t &mesh = shapes[s].mesh;

		// Check for smoothing group and compute smoothing normals
		std::map<int, easymath::vector3> smoothVertexNormals;
		computeSmoothingNormals(attrib, mesh, smoothVertexNormals);

		size_t total_vtx_num = mesh.indices.size();
		size_t curr_vtx_num = 0;
		int face_idx = 0;

		for each (auto vtx_num in mesh.num_face_vertices)
		/*num_face_vertices is a vector, each item is vtx count in that face*/
		{
			assert(vtx_num == 3); // assume all the poylgons are triangle.

			tinyobj::index_t idx0 = mesh.indices[curr_vtx_num + 0];
			tinyobj::index_t idx1 = mesh.indices[curr_vtx_num + 1];
			tinyobj::index_t idx2 = mesh.indices[curr_vtx_num + 2];

			int current_material_id = mesh.material_ids[face_idx];

			if ((current_material_id < 0) ||
				(current_material_id >= static_cast<int>(materials.size())))
			{
				// Invaid material index. Use default material.
				current_material_id = materials.size() - 1;  // Default material is added to the last item in `materials`.
			}

			vector3 diffuse_clr = materials[current_material_id].diffuse;

			vector2 uv[3] = {};
			if (attrib.texcoords.size() > 0)
			{
				if ((idx0.texcoord_index >= 0) &&
					(idx1.texcoord_index >= 0) &&
					(idx2.texcoord_index >= 0))
				{
					assert(attrib.texcoords.size() > size_t(2 * idx0.texcoord_index + 1));
					assert(attrib.texcoords.size() > size_t(2 * idx1.texcoord_index + 1));
					assert(attrib.texcoords.size() > size_t(2 * idx2.texcoord_index + 1));

					uv[0] = &(attrib.texcoords[2 * idx0.texcoord_index]);
					uv[1] = &(attrib.texcoords[2 * idx1.texcoord_index]);
					uv[2] = &(attrib.texcoords[2 * idx2.texcoord_index]);
					// flip Y coord
					uv[0].y = 1.f - uv[0].y;
					uv[1].y = 1.f - uv[1].y;
					uv[2].y = 1.f - uv[2].y;
				}
			}

			vector3 pos[3] = {};
			{
				int f0 = idx0.vertex_index;
				int f1 = idx1.vertex_index;
				int f2 = idx2.vertex_index;
				assert(f0 >= 0);
				assert(f1 >= 0);
				assert(f2 >= 0);

				pos[0] = &(attrib.vertices[3 * f0]);
				pos[1] = &(attrib.vertices[3 * f1]);
				pos[2] = &(attrib.vertices[3 * f2]);
				calcBoundBox(bmin, bmax, pos[0], pos[1], pos[2]);
			}

			vector3 nv[3] = {};
			{
				bool invalid_normal_index = attrib.normals.empty();
				if (!invalid_normal_index)
				{
					int nf0 = idx0.normal_index;
					int nf1 = idx1.normal_index;
					int nf2 = idx2.normal_index;

					if ((nf0 < 0) || (nf1 < 0) || (nf2 < 0))
					{
						// normal index is missing from this face.
						invalid_normal_index = true;
					}
					else
					{
						assert((size_t)max3way(3 * nf0 + 2, 3 * nf1 + 2, 3 * nf2 + 2) < attrib.normals.size());
						nv[0] = &(attrib.normals[3 * nf0]);
						nv[1] = &(attrib.normals[3 * nf1]);
						nv[2] = &(attrib.normals[3 * nf2]);
					}
				}

				if (invalid_normal_index && !smoothVertexNormals.empty())
				{
					// Use smoothing normals
					int f0 = idx0.vertex_index;
					int f1 = idx1.vertex_index;
					int f2 = idx2.vertex_index;

					if (f0 >= 0 && f1 >= 0 && f2 >= 0)
					{
						nv[0] = smoothVertexNormals.find(f0)->second;
						nv[1] = smoothVertexNormals.find(f1)->second;
						nv[2] = smoothVertexNormals.find(f2)->second;
						invalid_normal_index = false;
					}
				}

				if (invalid_normal_index)
				{
					easymath::calcNormal(nv[0], pos[0], pos[1], pos[2]);
					nv[2] = nv[0];
					nv[1] = nv[0];
				}
			}

			// save vertices
			for (int k = 0; k < 3; k++)
			{
				buffer.push_back(pos[k].x);
				buffer.push_back(pos[k].y);
				buffer.push_back(pos[k].z);
				buffer.push_back(nv[k].x);
				buffer.push_back(nv[k].y);
				buffer.push_back(nv[k].z);
				// Combine normal and diffuse to get color.
				float normal_factor = 0.2;
				float diffuse_factor = 1 - normal_factor;
				vector3 cc = nv[k] * normal_factor + diffuse_clr * diffuse_factor;
				easymath::normalize(cc);

				buffer.push_back(cc.x * 0.5 + 0.5);
				buffer.push_back(cc.y * 0.5 + 0.5);
				buffer.push_back(cc.z * 0.5 + 0.5);
				buffer.push_back(uv[k].x);
				buffer.push_back(uv[k].y);
			}

			curr_vtx_num += vtx_num;
			face_idx++;
		}
		assert (curr_vtx_num == total_vtx_num);

		size_t material_id = -1;
		// OpenGL viewer does not support texturing with per-face material.
		if (mesh.material_ids.size() > 0 &&
			mesh.material_ids.size() > s)
		{
			material_id = mesh.material_ids[0]; // use the material ID of the first face.
			if (material_id >= materials.size() || material_id < 0)
				material_id = materials.size() - 1; // use default material.
		}
		else
		{
			material_id = materials.size() - 1;  // use default material.
		}

		if (material_id >= materials.size() || material_id < 0)
		{
			o.diffuse_tex_name = "";
		}
		else
		{
			o.diffuse_tex_name = materials.at(material_id).diffuse_texname;
		}

		printf("shape[%d] material_id %d\n", int(s), int(material_id));

		o.numTriangles = 0;
		if (buffer.size() > 0)
		{
			o.numTriangles = buffer.size() / (3 + 3 + 3 + 2) / 3;  // 3:vtx, 3:normal, 3:col, 2:texcoord
			printf("shape[%d] # of triangles = %d\n", static_cast<int>(s), o.numTriangles);
		}

		drawObjects.push_back(o);
	}

	m_bLoaded = true;
	return true;
}
