#include "objimport.h"

/* Call this function to load a model, only loads triangulated meshes */
ObjMesh LoadObjMesh(std::string filename){
    ObjMesh myMesh;

    std::vector<Vector3f>           positions;
    std::vector<Vector2f>           texcoords;
    std::vector<Vector3f>           normals;
    std::vector<_ObjMeshFaceIndex>  faces;
    /**
     * Load file, parse it
     * Lines beginning with:
     * '#'  are comments can be ignored
     * 'v'  are vertices positions (3 floats that can be positive or negative)
     * 'vt' are vertices texcoords (2 floats that can be positive or negative)
     * 'vn' are vertices normals   (3 floats that can be positive or negative)
     * 'f'  are faces, 3 values that contain 3 values which are separated by / and <space>
     */

    std::ifstream filestream;
    filestream.open(filename.c_str());

	std::string line_stream;	// No longer depending on char arrays thanks to: Dale Weiler
	while(std::getline(filestream, line_stream)){
		std::stringstream str_stream(line_stream);
		std::string type_str;
        str_stream >> type_str;
        if(type_str == TOKEN_VERTEX_POS){
            Vector3f pos;
            str_stream >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
            //printf("vertex: %f %f %f\n", pos.x, pos.y, pos.z);
        }
        /*else if(type_str == TOKEN_VERTEX_TEX){
            Vector2f tex;
            str_stream >> tex.x >> tex.y;
            texcoords.push_back(tex);
        }
        else if(type_str == TOKEN_VERTEX_NOR){
            Vector3f nor;
            str_stream >> nor.x >> nor.y >> nor.z;
            normals.push_back(nor);
        }*/
        else if(type_str == TOKEN_FACE){
            _ObjMeshFaceIndex face_index;
            //char interupt;
            for(int i = 0; i < 3; ++i){
                str_stream >> face_index.pos_index[i]/* >> interupt
                           >> face_index.tex_index[i]  >> interupt
                           >> face_index.nor_index[i]*/;
            }
            //printf("face: %d %d %d\n", face_index.pos_index[0], face_index.pos_index[1], face_index.pos_index[2]);
            faces.push_back(face_index);
        }
    }
	// Explicit closing of the file
    filestream.close();

    for(size_t i = 0; i < faces.size(); ++i){
        ObjMeshFace face;
        for(size_t j = 0; j < 3; ++j){
            face.vertices[j].pos        = positions[faces[i].pos_index[j] - 1];
            //face.vertices[j].texcoord   = texcoords[faces[i].tex_index[j] - 1];
            //face.vertices[j].normal     = normals[faces[i].nor_index[j] - 1];
        }
        myMesh.faces.push_back(face);
    }

    return myMesh;
}

