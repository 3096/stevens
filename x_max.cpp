#include "xentax.h"
#include "x_smc.h"
#include "x_max.h"

/*
template<class T>
bool MAXOutputTriangleFaceMaterials(const SIMPLEMODELCONTAINER& data, const IDX_BUFFER& ib, std::ofstream& ofile)
{
 // pointer to data
 const T* pointer = reinterpret_cast<const T*>(ib.data.get());
 if(!pointer) return error("MAX EXPORT: Invalid data pointer.");

 // output triangles
 ofile << "  -- materials" << std::endl;
 for(size_t j = 0; j < ib.elem/3; j++) {
     if(ib.material != 0xFFFFFFFF) ofile << "  append mbuffer mm.materialIDList[" << (ib.material + 1) << "]" << std::endl;
     else ofile << "  append mbuffer 2" << std::endl;
    }

 return true;
}

template<class T>
bool MAXOutputTriangles(const IDX_BUFFER& ib, std::ofstream& ofile)
{
 // pointer to data
 const T* pointer = reinterpret_cast<const T*>(ib.data.get());
 if(!pointer) return error("MAX EXPORT: Invalid data pointer.");

 // output triangles
 ofile << "  -- triangles" << std::endl;
 for(size_t j = 0; j < ib.elem/3; j++) {
     T a = pointer[3*j + 0] + 1;
     T b = pointer[3*j + 1] + 1;
     T c = pointer[3*j + 2] + 1;
     ofile << "  append ibuffer [" << a << "," << b << "," << c << "]" << std::endl;
    }

 return true;
}

template<class T>
bool MAXOutputMapVert(const IDX_BUFFER& ib, uint32 channel, std::ofstream& ofile)
{
 // pointer to data
 const T* pointer = reinterpret_cast<const T*>(ib.data.get());
 if(!pointer) return error("MAX EXPORT: Invalid data pointer.");

 // output texture coordinates
 ofile << "  -- set texture coordinates # " << channel << std::endl;
 ofile << "  meshop.setNumMapVerts model " << channel << " " << ib.elem << std::endl;
 for(size_t j = 0; j < ib.elem; j++) {
     uint32 vertex_index = pointer[j] + 1;
     ofile << "  meshop.setMapVert model " << channel << " " << (j + 1) << " tbuffer[" << vertex_index << "]" << std::endl;
    }

 return true;
}

bool GeometryToMAX(const char* path, const char* name, const SIMPLEMODELCONTAINER& data)
{
 // create maxscript file
 std::stringstream ss;
 ss << path << name << ".ms";
 std::ofstream ofile(ss.str().c_str());
 if(!ofile) return error("MAXSCRIPT EXPORT: Error creating output file.");

 ofile << "--" << std::endl;
 ofile << "-- INITIALIZATION " << std::endl;
 ofile << "--" << std::endl;
 ofile << std::endl;

 ofile << "  -- initialize buffers" << std::endl;
 ofile << "  clearListner()" << std::endl;
 if(data.model->vbuffer.flags & VERTEX_POSITION) ofile << "  vbuffer = #()" << std::endl;
 if(data.model->vbuffer.flags & VERTEX_NORMAL) ofile << "  nbuffer = #()" << std::endl;
 if(data.model->vbuffer.flags & VERTEX_UV) ofile << "  tbuffer = #()" << std::endl;
 if(data.model->ibuffer.size()) ofile << "  ibuffer" << " = #()" << std::endl;
 if(data.materials.size()) ofile << "  mbuffer" << " = #()" << std::endl;
 ofile << std::endl;

 if(data.materials.size())
   {
    ofile << "--" << std::endl;
    ofile << "-- MATERIALS " << std::endl;
    ofile << "--" << std::endl;
    ofile << std::endl;
   
    ofile << "  texturePathName = getSavePath caption:\"Select Texture Folder\"" << std::endl;
    ofile << std::endl;
   
    ofile << "  mm = multimaterial()" << std::endl;
    ofile << "  mm.numsubs = " << data.materials.size() << std::endl;

    for(size_t i = 0; i < data.materials.size(); i++)
       {
        ofile << "  -- create material" << std::endl;
        ofile << "  mat = standardMaterial()" << std::endl;
        ofile << "  mat.shaderByName = " << "\"" << "Blinn" << "\"" << std::endl;
        ofile << "  mat.name = " << "\"" << data.materials[i].id << "\"" << std::endl;
        ofile << "  mat.twoSided = false" << std::endl;
        ofile << "  mat.showInViewport = true" << std::endl;
        ofile << "  mat.faceMap = false" << std::endl;
        ofile << std::endl;
   
        uint32 basemap = data.materials[i].basemap;
        if(data.materials[i].basemap != 0xFFFFFFFF) {
           ofile << "  -- assign base texture to material" << std::endl;
           ofile << "  mat.diffuseMap = BitmapTexture()" << std::endl;        
           ofile << "  mat.diffuseMap.filename = texturePathName + " << "\"/" << data.textures[basemap].filename << "\"" << std::endl;
           ofile << std::endl;
          }
   
        uint32 specmap = data.materials[i].specmap;
        if(data.materials[i].specmap != 0xFFFFFFFF) {
           ofile << "  -- assign specular texture to material" << std::endl;
           ofile << "  mat.specularMap = BitmapTexture()" << std::endl;        
           ofile << "  mat.specularMap.filename = texturePathName + " << "\"/" << data.textures[specmap].filename << "\"" << std::endl;
           ofile << std::endl;
          }

        uint32 normmap = data.materials[i].normmap;
        if(data.materials[i].normmap != 0xFFFFFFFF) {
           ofile << "  -- assign normal texture to material" << std::endl;
           ofile << "  bt = BitmapTexture()" << std::endl;        
           ofile << "  bt.filename = texturePathName + " << "\"/" << data.textures[normmap].filename << "\"" << std::endl;
           ofile << "  mat.bumpMapEnable = true" << std::endl;
           ofile << "  mat.bumpMapAmount = 100" << std::endl;
           ofile << "  mat.bumpMap = Normal_Bump()" << std::endl;
           ofile << "  mat.bumpMap.normal_map = bt" << std::endl;
           ofile << std::endl;
          }

        ofile << "  -- insert material" << std::endl;
        ofile << "  mm.mapEnabled[" << (i + 1) << "] = true" << std::endl;
        ofile << "  mm[" << (i + 1) << "] = mat" << std::endl;
        ofile << std::endl;
       }

    ofile << "--" << std::endl;
    ofile << "-- FACE MATERIALS " << std::endl;
    ofile << "--" << std::endl;
    ofile << std::endl;
   
    // process faces
    for(size_t i = 0; i < data.model->ibuffer.size(); i++)
       {
        const IDX_BUFFER& ib = data.model->ibuffer[i];
        if(ib.type == FACE_TYPE_TRIANGLES) {
           if(ib.elem < 3) return error("MAX EXPORT: Invalid index buffer.");
           if(ib.elem % 3) return error("MAX EXPORT: Invalid index buffer.");
           if(ib.format == FACE_FORMAT_UINT_08) { if(!MAXOutputTriangleFaceMaterials<uint08>(data, ib, ofile)) return false; }
           if(ib.format == FACE_FORMAT_UINT_16) { if(!MAXOutputTriangleFaceMaterials<uint16>(data, ib, ofile)) return false; }
           if(ib.format == FACE_FORMAT_UINT_32) { if(!MAXOutputTriangleFaceMaterials<uint32>(data, ib, ofile)) return false; }
           ofile << std::endl;
          }
       }
   }

 ofile << "--" << std::endl;
 ofile << "-- VERTEX BUFFER " << std::endl;
 ofile << "--" << std::endl;
 ofile << std::endl;

 if(data.model->vbuffer.flags & VERTEX_POSITION)
   {
    for(size_t i = 0; i < data.model->vbuffer.elem; i++) {
        float x = data.model->vbuffer.data[i].vx;
        float y = data.model->vbuffer.data[i].vy;
        float z = data.model->vbuffer.data[i].vz;
        ofile << "  append vbuffer [" << x << "," << y << "," << z << "]" << std::endl;
       }
    ofile << std::endl;
   }
 ofile << std::endl;

 if(data.model->vbuffer.flags & VERTEX_NORMAL)
   {
    for(size_t i = 0; i < data.model->vbuffer.elem; i++) {
        float x = data.model->vbuffer.data[i].nx;
        float y = data.model->vbuffer.data[i].ny;
        float z = data.model->vbuffer.data[i].nz;
        ofile << "  append nbuffer [" << x << "," << y << "," << z << "]" << std::endl;
       }
    ofile << std::endl;
   }

 if(data.model->vbuffer.flags & VERTEX_UV)
   {
    for(size_t i = 0; i < data.model->vbuffer.elem; i++) {
        float x = data.model->vbuffer.data[i].tu;
        float y = data.model->vbuffer.data[i].tv;
        float z = 0.0f;
        ofile << "  append tbuffer [" << x << "," << y << "," << z << "]" << std::endl;
       }
    ofile << std::endl;
   }

 ofile << "--" << std::endl;
 ofile << "-- INDEX BUFFERS " << std::endl;
 ofile << "--" << std::endl;
 ofile << std::endl;

 // process index buffers
 for(size_t i = 0; i < data.model->ibuffer.size(); i++)
    {
     const IDX_BUFFER& ib = data.model->ibuffer[i];
     if(ib.type == FACE_TYPE_TRIANGLES) {
        if(ib.elem < 3) return error("MAX EXPORT: Invalid index buffer.");
        if(ib.elem % 3) return error("MAX EXPORT: Invalid index buffer.");
        if(ib.format == FACE_FORMAT_UINT_08) { if(!MAXOutputTriangles<uint08>(ib, ofile)) return false; }
        if(ib.format == FACE_FORMAT_UINT_16) { if(!MAXOutputTriangles<uint16>(ib, ofile)) return false; }
        if(ib.format == FACE_FORMAT_UINT_32) { if(!MAXOutputTriangles<uint32>(ib, ofile)) return false; }
        ofile << std::endl;
       }
    }

 ofile << "  -- create model" << std::endl;
 if(data.materials.size()) ofile << "  model = mesh vertices:vbuffer faces:ibuffer materialIDs:mbuffer" << std::endl;
 else ofile << "  model = mesh vertices:vbuffer faces:ibuffer" << std::endl;
 ofile << "  model.name = " << "\"" << data.model->id << "\"" << std::endl;
 ofile << "  update model" << std::endl;
 ofile << std::endl;

 ofile << "--" << std::endl;
 ofile << "-- TEXTURE COORDINATES " << std::endl;
 ofile << "--" << std::endl;
 ofile << std::endl;

 ofile << "  -- enable texture map channels" << std::endl;
 ofile << "  meshop.setNumMaps model 2" << std::endl;
 ofile << "  update model" << std::endl;
 ofile << std::endl;

 // for each index buffer
 ofile << "  meshop.setNumTVerts model " << data.model->vbuffer.elem << std::endl;
 for(size_t i = 0; i < data.model->vbuffer.elem; i++)
     ofile << "  setTVert model " << (i + 1) << " tbuffer[" << (i + 1) << "]" << std::endl;
 ofile << std::endl;

 ofile << "  -- update model" << std::endl;
 ofile << "  model.material = mm" << std::endl;
 ofile << "  update model" << std::endl;

 return true;
}
*/