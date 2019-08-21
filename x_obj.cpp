#include "xentax.h"
#include "x_smc.h"
#include "x_obj.h"

/*
//bool GeometryToOBJ(const char* path, const char* name, const SIMPLEMODELCONTAINER& data)
{
 // create OBJ file
 stringstream ss;
 ss << path << name << ".obj";
 ofstream objfile(ss.str().c_str());
 if(!objfile) return error("OBJ EXPORT: Error creating output file.");

 // OBJ file header
 objfile << "o " << name << ".obj" << endl;
 objfile << "mtllib " << name << ".mtl" << endl;

 struct OBJOFFSETINFO {
  uint32 vertex_base;
  uint32 normal_base;
  uint32 texmap_base;
 };

 // index to each vertex buffer
 OBJOFFSETINFO ooi = { 0, 0, 0 };
 std::deque<OBJOFFSETINFO> offset_list;
 for(size_t i = 0; i < vd.size(); i++) {
     offset_list.push_back(ooi);
     if(vd[i].flags & VERTEX_POSITION) ooi.vertex_base += vd[i].elem;
     if(vd[i].flags & VERTEX_NORMAL) ooi.normal_base += vd[i].elem;
     if(vd[i].flags & VERTEX_UV) ooi.texmap_base += vd[i].elem;
    }

 // output vertices
 for(size_t i = 0; i < vd.size(); i++) {
     if(vd[i].flags & VERTEX_POSITION) {
        objfile << "# POSITION BUFFER " << i << endl;
        for(size_t j = 0; j < vd[i].elem; j++)
            objfile << "v " << vd[i].data[j].vx << " " << vd[i].data[j].vy << " " << vd[i].data[j].vz << endl;
       }
    }
 for(size_t i = 0; i < vd.size(); i++) {
     if(vd[i].flags & VERTEX_NORMAL) {
        objfile << "# NORMAL BUFFER " << i << endl;
        for(size_t j = 0; j < vd[i].elem; j++)
            objfile << "vn " << vd[i].data[j].nx << " " << vd[i].data[j].ny << " " << vd[i].data[j].nz << endl;
       }
    }
 for(size_t i = 0; i < vd.size(); i++) {
     if(vd[i].flags & VERTEX_UV) {
        objfile << "# UV BUFFER " << i << endl;
        for(size_t j = 0; j < vd[i].elem; j++)
            objfile << "vt " << vd[i].data[j].tu << " " << vd[i].data[j].tv << endl;
       }
    }

 // output faces
 for(size_t i = 0; i < fd.size(); i++)
    {
     objfile << "# INDEX BUFFER " << i << endl;
     objfile << "g " << fd[i].name.c_str() << endl;
     objfile << "usemtl " << fd[i].name.c_str() << endl;

     // set vertex buffer reference index
     uint32 vertex_buffer_index = fd[i].reference;
     if(!(vertex_buffer_index < offset_list.size())) return error("OBJ EXPORT: Invalid vertex buffer reference.");

     // set vertex buffer properties
     bool has_vertex = (vd[vertex_buffer_index].flags & VERTEX_POSITION) != 0;
     bool has_normal = (vd[vertex_buffer_index].flags & VERTEX_NORMAL) != 0;
     bool has_texmap = (vd[vertex_buffer_index].flags & VERTEX_UV) != 0;

     // set base vertex
     uint32 base_vertex = offset_list[vertex_buffer_index].vertex_base;
     uint32 base_normal = offset_list[vertex_buffer_index].normal_base;
     uint32 base_texmap = offset_list[vertex_buffer_index].texmap_base;

     // tri-strips
     if(fd[i].type == FACE_TYPE_TRISTRIP)
       {
        // validate
        if(fd[i].elem < 3) return error("OBJ EXPORT: Tri-strips require at least three points.");

        if(fd[i].format == FACE_FORMAT_UINT_16)
          {
           // first triangle
           const uint16* data = reinterpret_cast<uint16*>(fd[i].data.get());
           uint32 va = base_vertex + data[0] + 1;
           uint32 vb = base_vertex + data[1] + 1;
           uint32 vc = base_vertex + data[2] + 1;
           uint32 na = base_normal + data[0] + 1;
           uint32 nb = base_normal + data[1] + 1;
           uint32 nc = base_normal + data[2] + 1;
           uint32 ta = base_texmap + data[0] + 1;
           uint32 tb = base_texmap + data[1] + 1;
           uint32 tc = base_texmap + data[2] + 1;

           // first triangle (draw only if not degenerate)
           if(!(va == vb || va == vc || vb == vc)) {
              if(!has_normal && !has_texmap) objfile << "f " << va << " " << vb << " " << vc << endl;
              else if(!has_normal && has_texmap) objfile << "f " << va << "/" << ta << " " << vb << "/" << tb << " " << vc << "/" << tc << endl;
              else if(!has_texmap && has_normal) objfile << "f " << va << "//" << na << " " << vb << "//" << nb << " " << vc << "//" << nc << endl;
              else objfile << "f " << va << "/" << ta << "/" << na << " " << vb << "/" << tb << "/" << nb << " " << vc << "/" << tc << "/" << nc << endl;
             }

           // other triangles
           for(size_t j = 3; j < fd[i].elem; j++)
              {
               // update position
               va = vb;
               vb = vc;
               vc = base_vertex + data[j] + 1;

               // update normal
               if(has_normal) {
                  na = nb;
                  nb = nc;
                  nc = base_normal + data[j] + 1;
                 }

               // update UV
               if(has_texmap) {
                  ta = tb;
                  tb = tc;
                  tc = base_texmap + data[j] + 1;
                 }

               // skip degenerate triangles
               if(va == vb || va == vc || vb == vc) continue;

               if(!has_normal && !has_texmap) {
                  if(j % 2) objfile << "f " << va << " " << vc << " " << vb << endl;
                  else objfile << "f " << va << " " << vb << " " << vc << endl;
                 }
               else if(!has_normal && has_texmap) {
                  if(j % 2) objfile << "f " << va << "/" << ta << " " << vc << "/" << tc << " " << vb << "/" << tb << endl;
                  else objfile << "f " << va << "/" << ta << " " << vb << "/" << tb << " " << vc << "/" << tc << endl;
                 }
               else if(has_normal && !has_texmap) {
                  if(j % 2) objfile << "f " << va << "//" << na << " " << vc << "//" << nc << " " << vb << "//" << nb << endl;
                  else objfile << "f " << va << "//" << na << " " << vb << "//" << nb << " " << vc << "//" << nc << endl;
                 }
               else {
                  if(j % 2) objfile << "f " << va << "/" << ta << "/" << na << " " << vc << "/" << tc << "/" << nc << " " << vb << "/" << tb << "/" << nb << endl;
                  else objfile << "f " << va << "/" << ta << "/" << na << " " << vb << "/" << tb << "/" << nb << " " << vc << "/" << tc << "/" << nc << endl;
                 }
              }
          }
        else if(fd[i].format == FACE_FORMAT_UINT_32)
          {
           // first triangle
           const uint32* data = reinterpret_cast<uint32*>(fd[i].data.get());
           uint32 va = base_vertex + data[0] + 1;
           uint32 vb = base_vertex + data[1] + 1;
           uint32 vc = base_vertex + data[2] + 1;
           uint32 na = base_normal + data[0] + 1;
           uint32 nb = base_normal + data[1] + 1;
           uint32 nc = base_normal + data[2] + 1;
           uint32 ta = base_texmap + data[0] + 1;
           uint32 tb = base_texmap + data[1] + 1;
           uint32 tc = base_texmap + data[2] + 1;
           if(!has_normal && !has_texmap) objfile << "f " << va << " " << vb << " " << vc << endl;
           else if(!has_normal && has_texmap) objfile << "f " << va << "/" << ta << " " << vb << "/" << tb << " " << vc << "/" << tc << endl;
           else if(!has_texmap && has_normal) objfile << "f " << va << "//" << na << " " << vb << "//" << nb << " " << vc << "//" << nc << endl;
           else objfile << "f " << va << "/" << ta << "/" << na << " " << vb << "/" << tb << "/" << nb << " " << vc << "/" << tc << "/" << nc << endl;

           // other triangles
           for(size_t j = 3; j < fd[i].elem; j++)
              {
               va = vb;
               vb = vc;
               vc = base_vertex + data[j] + 1;

               if(has_normal) {
                  na = nb;
                  nb = nc;
                  nc = base_normal + data[j] + 1;
                 }
               if(has_texmap) {
                  ta = tb;
                  tb = tc;
                  tc = base_texmap + data[j] + 1;
                 }

               if(!has_normal && !has_texmap) {
                  if(j % 2) objfile << "f " << va << " " << vc << " " << vb << endl;
                  else objfile << "f " << va << " " << vb << " " << vc << endl;
                 }
               else if(!has_normal && has_texmap) {
                  if(j % 2) objfile << "f " << va << "/" << ta << " " << vc << "/" << tc << " " << vb << "/" << tb << endl;
                  else objfile << "f " << va << "/" << ta << " " << vb << "/" << tb << " " << vc << "/" << tc << endl;
                 }
               else if(has_normal && !has_texmap) {
                  if(j % 2) objfile << "f " << va << "//" << na << " " << vc << "//" << nc << " " << vb << "//" << nb << endl;
                  else objfile << "f " << va << "//" << na << " " << vb << "//" << nb << " " << vc << "//" << nc << endl;
                 }
               else {
                  if(j % 2) objfile << "f " << va << "/" << ta << "/" << na << " " << vc << "/" << tc << "/" << nc << " " << vb << "/" << tb << "/" << nb << endl;
                  else objfile << "f " << va << "/" << ta << "/" << na << " " << vb << "/" << tb << "/" << nb << " " << vc << "/" << tc << "/" << nc << endl;
                 }
              }
          }
        else
           return error("OBJ EXPORT: Invalid index buffer data format.");
       }
     // triangles
     else if(fd[i].type == FACE_TYPE_TRIANGLES)
       {
        if(fd[i].format == FACE_FORMAT_UINT_16)
          {
           size_t n_triangles = fd[i].elem/3;
           for(size_t j = 0; j < n_triangles; j++)
              {
               // base OBJ indices
               const uint16* ib_data = reinterpret_cast<uint16*>(fd[i].data.get());
               uint32 va = base_vertex + ib_data[(3*j + 0)] + 1;
               uint32 vb = base_vertex + ib_data[(3*j + 1)] + 1;
               uint32 vc = base_vertex + ib_data[(3*j + 2)] + 1;
               uint32 na = base_normal + ib_data[(3*j + 0)] + 1;
               uint32 nb = base_normal + ib_data[(3*j + 1)] + 1;
               uint32 nc = base_normal + ib_data[(3*j + 2)] + 1;
               uint32 ta = base_texmap + ib_data[(3*j + 0)] + 1;
               uint32 tb = base_texmap + ib_data[(3*j + 1)] + 1;
               uint32 tc = base_texmap + ib_data[(3*j + 2)] + 1;

               // write triangle
               if(!has_normal && !has_texmap) objfile << "f " << va << " " << vb << " " << vc << endl;
               else if(!has_normal && has_texmap) objfile << "f " << va << "/" << ta << " " << vb << "/" << tb << " " << vc << "/" << tc << endl;
               else if(has_normal && !has_texmap) objfile << "f " << va << "//" << na << " " << vb << "//" << nb << " " << vc << "//" << nc << endl;
               else objfile << "f " << va << "/" << ta << "/" << na << " " << vb << "/" << tb << "/" << nb << " " << vc << "/" << tc << "/" << nc << endl;
              }
          }
        else if(fd[i].format == FACE_FORMAT_UINT_32)
          {
           size_t n_triangles = fd[i].elem/3;
           for(size_t j = 0; j < n_triangles; j++)
              {
               // base OBJ indices
               const uint32* ib_data = reinterpret_cast<uint32*>(fd[i].data.get());
               uint32 va = base_vertex + ib_data[(3*j + 0)] + 1;
               uint32 vb = base_vertex + ib_data[(3*j + 1)] + 1;
               uint32 vc = base_vertex + ib_data[(3*j + 2)] + 1;
               uint32 na = base_normal + ib_data[(3*j + 0)] + 1;
               uint32 nb = base_normal + ib_data[(3*j + 1)] + 1;
               uint32 nc = base_normal + ib_data[(3*j + 2)] + 1;
               uint32 ta = base_texmap + ib_data[(3*j + 0)] + 1;
               uint32 tb = base_texmap + ib_data[(3*j + 1)] + 1;
               uint32 tc = base_texmap + ib_data[(3*j + 2)] + 1;

               // write triangle
               if(!has_normal && !has_texmap) objfile << "f " << va << " " << vb << " " << vc << endl;
               else if(!has_normal && has_texmap) objfile << "f " << va << "/" << ta << " " << vb << "/" << tb << " " << vc << "/" << tc << endl;
               else if(has_normal && !has_texmap) objfile << "f " << va << "//" << na << " " << vb << "//" << nb << " " << vc << "//" << nc << endl;
               else objfile << "f " << va << "/" << ta << "/" << na << " " << vb << "/" << tb << "/" << nb << " " << vc << "/" << tc << "/" << nc << endl;
              }
          }
       }
    }

 // create MTL file
 stringstream mtlfilename;
 mtlfilename << path << name << ".mtl" << ends;
 ofstream mtlfile(mtlfilename.str().c_str());
 if(!mtlfile) return error("OBJ EXPORT: Error creating material file.");

 // save materials
 for(size_t i = 0; i < fd.size(); i++)
    {
     mtlfile << "newmtl " << fd[i].name.c_str() << endl;
     mtlfile << "Ka 0 0 0" << endl;
     mtlfile << "Kd 0.784314 0.784314 0.784314" << endl;
     mtlfile << "Ks 0 0 0" << endl;
     mtlfile << "Ni 1" << endl;
     mtlfile << "Ns 400" << endl;
     mtlfile << "Tf 1 1 1" << endl;
     mtlfile << "d 1" << endl;
     //if(fd[i].basemap.length()) mtlfile << "map_Ka " << fd[i].basemap << endl;
     //if(fd[i].basemap.length()) mtlfile << "map_Kd " << fd[i].basemap << endl;
     //if(fd[i].specmap.length()) mtlfile << "map_Ks " << fd[i].specmap << endl;
     //if(fd[i].basemap.length()) mtlfile << "map_d  " << fd[i].basemap << endl;
     mtlfile << endl;
    }

 return true;
}
*/