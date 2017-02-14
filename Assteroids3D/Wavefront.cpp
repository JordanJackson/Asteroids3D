#include "Wavefront.h"

#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

glsh::Mesh* LoadWavefrontOBJ(const std::string& path)
{
	std::cout << "Loading '" << path << "'" << std::endl;

	// open the file for reading
	std::ifstream file(path.c_str());

	// make sure the file opened correctly
	if (!file) {
		std::cerr << "ERROR: Failed to open " << path << std::endl;
		return NULL;
	}

	std::string line;                   // storage for a line of text
	std::vector<std::string> lineTok;   // storage for the tokens that make up a line
	std::vector<std::string> vertexTok; // storage for the components of a vertex definition (v/vt/vn)
	int lineno = 0;                     // tracks the line number that we're on (for desciptive error messages, mostly)

	// declare storage vectors
	std::vector<glm::vec3> positions = std::vector<glm::vec3>();
	std::vector<glm::vec2> texcoords = std::vector<glm::vec2>();
	std::vector<glm::vec3> normals = std::vector<glm::vec3>();
	// will contain final array of vertices
	std::vector<glsh::VertexPositionNormalTexture> vpnt = std::vector<glsh::VertexPositionNormalTexture>();
	std::vector<glsh::VertexPositionNormal> vpn = std::vector<glsh::VertexPositionNormal>();
	
	// go through the file one line at a time until the end
	for (;;) {

		// storage for faces
		std::vector<std::vector<glm::vec2>> vpnSub = std::vector<std::vector<glm::vec2>>();
		std::vector<std::vector<glm::vec3>> vpntSub = std::vector<std::vector<glm::vec3>>();

		// read a line of text and check status
		// - if the read failed due to end of file, then we're done reading and break out of the loop
		// - otherwise, there was an error, so bail out
		if (!std::getline(file, line)) {
			if (file.eof()) {
				break;  // done reading file, proceed with vertex buffer creation
			}
			else {
				std::cerr << "ERROR: Failed to read from " << path << " at line " << lineno << std::endl;
				return NULL;
			}
		}

		// increment the line counter
		++lineno;

		// break the line up into whitespace-separated tokens
		lineTok = glsh::Tokenize(line);

		// if it's an empty line, move on to the next line
		if (lineTok.empty()) {
			continue;
		}

		// if it's a comment, move on to the next line
		char firstChar = lineTok[0][0];
		if (firstChar == '#') {
			continue;
		}

		if (lineTok[0] == "v") {
			// it's a vertex position

			// make sure we have three coordinates following the "v"
			if (lineTok.size() != 4) {
				std::cerr << "ERROR: Incorrect number of vertex position coordinates on line " << lineno << std::endl;
				return NULL;
			}

			// convert the coordinates to floats
			float x = glsh::FromString<float>(lineTok[1]);
			float y = glsh::FromString<float>(lineTok[2]);
			float z = glsh::FromString<float>(lineTok[3]);

			//std::cout << "Position: " << x << ", " << y << ", " << z << std::endl;

			positions.push_back(glm::vec3(x, y, z));

		}
		else if (lineTok[0] == "vn") {
			// it's a vertex normal

			// make sure we have three coordinates following the "vn"
			if (lineTok.size() != 4) {
				std::cerr << "ERROR: Incorrect number of normal coordinates on line " << lineno << std::endl;
				return NULL;
			}

			// convert the coordinates to floats
			float nx = glsh::FromString<float>(lineTok[1]);
			float ny = glsh::FromString<float>(lineTok[2]);
			float nz = glsh::FromString<float>(lineTok[3]);

			//std::cout << "Normal: " << nx << ", " << ny << ", " << nz << std::endl;
			glm::vec3 normal = glm::vec3(nx, ny, nz);
			if (glm::length(normal) != 0)
			{
				normals.push_back(glm::vec3(nx, ny, nz));
			}
			else
			{
				normals.push_back(glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f)));
				std::cerr << "WARNING: 0 length normal vector found." << std::endl;
			}

		}
		else if (lineTok[0] == "vt") {
			// it's a texture coordinate

			// make sure we have at least two coordinates following the "vt"
			if (lineTok.size() < 3) {
				std::cerr << "ERROR: Incorrect number of uv coordinates on line " << lineno << std::endl;
				return NULL;
			}

			// convert the coordinates to floats
			float u = glsh::FromString<float>(lineTok[1]);
			float v = glsh::FromString<float>(lineTok[2]);

			//std::cout << "Texcoord: " << u << ", " << v << std::endl;

			texcoords.push_back(glm::vec2(u, v));

		}
		else if (lineTok[0] == "f") {
			// it's a face

			// need at least 3 vertices per face
			if (lineTok.size() < 4) {
				std::cerr << "ERROR: Insufficient number of face elements on line " << lineno << std::endl;
				return NULL;
			}
			
			//std::cout << "Face with " << lineTok.size() - 1 << " vertices" << std::endl;

			std::vector<glm::vec2> v2 = std::vector<glm::vec2>();
			std::vector<glm::vec3> v3 = std::vector<glm::vec3>();

			// process each vertex definition for this face
			for (unsigned i = 1; i < lineTok.size(); i++) {

				// split into three parts (v/vt/vn)
				vertexTok = glsh::Split(lineTok[i], '/');

				// make sure there are three parts
				if (vertexTok.size() != 3) {
					std::cerr << "ERROR: Incorrect number of vertex tokens for vertex " << i << " on line " << lineno << std::endl;
					return NULL;
				}

				// convert the index strings to ints
				int positionIndex = 0;
				int normalIndex = 0;
				int texcoordIndex = 0;

				// get position index (required)
				if (!vertexTok[0].empty()) {
					positionIndex = glsh::FromString<int>(vertexTok[0]);
				}
				else {
					std::cerr << "ERROR: Vertex position index not given for vertex " << i << " on line " << lineno << std::endl;
					return NULL;
				}

				// get normal index (required)
				if (!vertexTok[2].empty()) {
					normalIndex = glsh::FromString<int>(vertexTok[2]);
				}
				else {
					std::cerr << "ERROR: Vertex normal index not given for vertex " << i << " on line " << lineno << std::endl;
					return NULL;
				}

				// get texcoord index (optional)
				if (!vertexTok[1].empty()) {
					texcoordIndex = glsh::FromString<int>(vertexTok[1]);
				}
				else {
					// use a default texcoord of (0, 0)
					// ...or create a mesh without texcoords
				}

				//std::cout << "  Vertex pos at index " << positionIndex << ", normal at index " << normalIndex << ", texcoord at index " << texcoordIndex << std::endl;
				if (texcoordIndex == 0)
				{
					v2.push_back(glm::vec2(positionIndex, normalIndex));
				}
				else
				{
					v3.push_back(glm::vec3(positionIndex, normalIndex, texcoordIndex));
				}
			}

			if (!v2.empty())
			{
				vpnSub.push_back(v2);
			}
			else if (!v3.empty())
			{
				vpntSub.push_back(v3);
			}

			if (!vpnSub.empty())
			{
				for (auto & v : vpnSub)
				{
					for (int i = 0; i < v.size() - 2; i++)
					{
						for (int j = i; j < i + 3; j++)
						{
							int pIdx = 0;
							int nIdx = 0;
							if (j == i)
							{
								pIdx = v[0].x - 1;
								nIdx = v[0].y - 1;
							}
							else
							{
								pIdx = v[j].x - 1;
								nIdx = v[j].y - 1;
							}

							vpn.push_back(glsh::VertexPositionNormal(
								positions[pIdx].x,
								positions[pIdx].y,
								positions[pIdx].z,
								normals[nIdx].x,
								normals[nIdx].y,
								normals[nIdx].z
							));
						}
					}
				}
			}
			else
			{
				for (auto & v : vpntSub)
				{
					for (int i = 0; i < v.size() - 2; i++)
					{
						for (int j = i; j < i + 3; j++)
						{
							int pIdx = 0;
							int nIdx = 0;
							int tIdx = 0;
							if (j == i)
							{
								pIdx = v[0].x - 1;
								nIdx = v[0].y - 1;
								tIdx = v[0].z - 1;
							}
							else
							{
								pIdx = v[j].x - 1;
								nIdx = v[j].y - 1;
								tIdx = v[j].z - 1;
							}

							vpnt.push_back(glsh::VertexPositionNormalTexture(
								positions[pIdx].x,
								positions[pIdx].y,
								positions[pIdx].z,
								normals[nIdx].x,
								normals[nIdx].y,
								normals[nIdx].z,
								texcoords[tIdx].x,
								texcoords[tIdx].y
							));
						}
					}
				}
			}

		}
	}

	if (!vpn.empty())
	{
		std::vector<glsh::VertexPositionNormal> finalVPN = std::vector<glsh::VertexPositionNormal>();
		std::vector<unsigned int> indices = std::vector<unsigned int>();
		// for each vertex in vpn
		for (auto & v : vpn)
		{
			int pos = std::find(finalVPN.begin(), finalVPN.end(), v) - finalVPN.begin();

			// does not exist
			if (pos >= finalVPN.size())
			{
				finalVPN.push_back(v);
				indices.push_back(finalVPN.size() - 1);
			}
			// does exist
			else
			{
				indices.push_back(pos);
			}
		}

		glsh::IndexedMesh* mesh = glsh::CreateMesh(GL_TRIANGLES, finalVPN, indices);
		return mesh;
	}
	else if (!vpnt.empty())
	{
		std::vector<glsh::VertexPositionNormalTexture> finalVPNT = std::vector<glsh::VertexPositionNormalTexture>();
		std::vector<unsigned int> indices = std::vector<unsigned int>();
		// for each vertex in vpn
		for (auto & v : vpnt)
		{
			int pos = std::find(finalVPNT.begin(), finalVPNT.end(), v) - finalVPNT.begin();

			// does not exist
			if (pos >= finalVPNT.size())
			{
				finalVPNT.push_back(v);
				indices.push_back(finalVPNT.size() - 1);
			}
			// does exist
			else
			{
				indices.push_back(pos);
			}
		}

		glsh::IndexedMesh* mesh = glsh::CreateMesh(GL_TRIANGLES, finalVPNT, indices);

		return mesh;
	}

	return nullptr;
}
