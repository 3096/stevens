#include "xentax.h"
#include "x_smc.h"
#include "x_dae.h"

/*
bool GeometryToDAE(const char* path, const char* name, const GEOMETRY& data, const MATERIAL_LIST& materials, const TEXTURE_LIST& textures, const SKELETON_LIST& sl)
{
 if(!path || !strlen(path)) return error("DAE EXPORT: Expecting pathname.");
 if(!name || !strlen(name)) return error("DAE EXPORT: Expecting filename.");
 if(!data.meshlist.size()) return error("DAE EXPORT: Expecting geometry.");

 // create DAE file
 stringstream ss;
 ss << path << name << ".dae";
 ofstream ofile(ss.str().c_str());
 if(!ofile) return error("DAE EXPORT: Error creating output file."); 

 // save <collada>
 ofile << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << endl;
 ofile << "<COLLADA xmlns=\"http://www.collada.org/2005/11/COLLADASchema\" version=\"1.4.1\">" << endl;
 ofile << endl;

 // save <asset>
 ofile << " <!-- ASSET -->" << endl;
 ofile << " <asset>" << endl;
 ofile << "  <created>2012-01-01T21:00:00Z</created>" << endl;
 ofile << "  <modified>2012-01-01T21:00:00Z</modified>" << endl;
 ofile << "  <up_axis>Z_UP</up_axis>" << endl;
 ofile << " </asset>" << endl;
 ofile << endl;

 // save <library_images>
 if(textures.size())
   {
    ofile << " <!-- LIBRARY_IMAGES -->" << endl;
    ofile << " <library_images>" << endl;
    for(size_t i = 0; i < textures.size(); i++) {
        ofile << "  <image id=\"" << textures[i].id << "\">" << endl;
        ofile << "   <init_from>" << textures[i].filename << "</init_from>" << endl;
        ofile << "  </image>" << endl;
       }
    ofile << " </library_images>" << endl;
    ofile << endl;
   }

 // save <library_effects>
 ofile << " <!-- LIBRARY_EFFECTS -->" << endl;
 ofile << " <library_effects>" << endl;
 for(size_t i = 0; i < materials.size(); i++)
    {
     // important texture information
     bool has_basemap = materials[i].basemap != 0xFFFFFFFF;
     bool has_specmap = materials[i].specmap != 0xFFFFFFFF;
     bool has_normmap = materials[i].normmap != 0xFFFFFFFF;
     bool has_bumpmap = materials[i].bumpmap != 0xFFFFFFFF;
     bool has_map = (has_basemap || has_specmap || has_normmap || has_bumpmap);

     stringstream e_id; // effect
     e_id << "effects_" << setfill('0') << setw(3) << i;

     ofile << "  <effect id=\"" << e_id.str() << "\">" << endl;
     ofile << "   <profile_COMMON>" << endl;
     ofile << "    <technique sid=\"common\">" << endl;

     // base map parameters
     stringstream basemap_id1;
     stringstream basemap_id2;
     basemap_id1 << "basemap_" << setfill('0') << setw(3) << i;
     basemap_id2 << "basemap_options_" << setfill('0') << setw(3) << i;

     if(has_basemap)
       {
        // parameter #1
        ofile << "     <newparam sid=\"" << basemap_id1.str() << "\">" << endl;
        ofile << "      <surface type=\"2D\">" << endl;
        ofile << "       <init_from>" << textures[materials[i].basemap].id << "</init_from>" << endl;
        ofile << "       <format>A8R8G8B8</format>" << endl;
        ofile << "      </surface>" << endl;
        ofile << "     </newparam>" << endl;
        // parameter #2
        ofile << "     <newparam sid=\"" << basemap_id2.str() << "\">" << endl;
        ofile << "      <sampler2D>" << endl;
        ofile << "       <source>" << basemap_id1.str() << "</source>" << endl;
        ofile << "       <minfilter>LINEAR_MIPMAP_LINEAR</minfilter>" << endl;
        ofile << "       <magfilter>LINEAR</magfilter>" << endl;
        ofile << "      </sampler2D>" << endl;
        ofile << "     </newparam>" << endl;
       }

     // spec map parameters
     stringstream specmap_id1;
     stringstream specmap_id2;
     specmap_id1 << "specmap_" << setfill('0') << setw(3) << i;
     specmap_id2 << "specmap_options_" << setfill('0') << setw(3) << i;

     if(has_specmap)
       {
        // parameter #1
        ofile << "     <newparam sid=\"" << specmap_id1.str() << "\">" << endl;
        ofile << "      <surface type=\"2D\">" << endl;
        ofile << "       <init_from>" << textures[materials[i].specmap].id << "</init_from>" << endl;
        ofile << "       <format>A8R8G8B8</format>" << endl;
        ofile << "      </surface>" << endl;
        ofile << "     </newparam>" << endl;
        // parameter #2
        ofile << "     <newparam sid=\"" << specmap_id2.str() << "\">" << endl;
        ofile << "      <sampler2D>" << endl;
        ofile << "       <source>" << specmap_id1.str() << "</source>" << endl;
        ofile << "       <minfilter>LINEAR_MIPMAP_LINEAR</minfilter>" << endl;
        ofile << "       <magfilter>LINEAR</magfilter>" << endl;
        ofile << "      </sampler2D>" << endl;
        ofile << "     </newparam>" << endl;
       }

     // norm map parameters
     stringstream normmap_id1;
     stringstream normmap_id2;
     normmap_id1 << "normmap_" << setfill('0') << setw(3) << i;
     normmap_id2 << "normmap_options_" << setfill('0') << setw(3) << i;

     if(has_normmap)
       {
        // parameter #1
        ofile << "     <newparam sid=\"" << normmap_id1.str() << "\">" << endl;
        ofile << "      <surface type=\"2D\">" << endl;
        ofile << "       <init_from>" << textures[materials[i].normmap].id << "</init_from>" << endl;
        ofile << "       <format>A8R8G8B8</format>" << endl;
        ofile << "      </surface>" << endl;
        ofile << "     </newparam>" << endl;
        // parameter #2
        ofile << "     <newparam sid=\"" << normmap_id2.str() << "\">" << endl;
        ofile << "      <sampler2D>" << endl;
        ofile << "       <source>" << normmap_id1.str() << "</source>" << endl;
        ofile << "       <minfilter>LINEAR_MIPMAP_LINEAR</minfilter>" << endl;
        ofile << "       <magfilter>LINEAR</magfilter>" << endl;
        ofile << "      </sampler2D>" << endl;
        ofile << "     </newparam>" << endl;
       }

     ofile << "     <lambert>" << endl;
     if(has_basemap) {
        ofile << "      <diffuse>" << endl;
        ofile << "       <texture texture=\"" << basemap_id2.str() << "\" texcoord=\"TEX0\" />" << endl;
        ofile << "      </diffuse>" << endl;
       }
     if(has_specmap) {
        ofile << "      <specular>" << endl;
        ofile << "       <texture texture=\"" << specmap_id2.str() << "\" texcoord=\"TEX0\" />" << endl;
        ofile << "      </specular>" << endl;
       }
     ofile << "     </lambert>" << endl;

     if(has_normmap) {
        ofile << "      <extra>" << endl;
        ofile << "       <technique profile=\"OpenCOLLADA3dsMax\">" << endl;
        ofile << "        <bump bumptype=\"NORMALMAP\">" << endl;
        ofile << "         <texture texture=\"" << normmap_id2.str() << "\" texcoord=\"TEX0\" />" << endl;
        ofile << "        </bump>" << endl;
        ofile << "       </technique>" << endl;
        ofile << "      </extra>" << endl;
       }

     ofile << "    </technique>" << endl;
     ofile << "   </profile_COMMON>" << endl;
     ofile << "  </effect>" << endl;
    }
 ofile << " </library_effects>" << endl;
 ofile << endl;

 // save <library_materials>
 ofile << " <!-- LIBRARY_MATERIALS -->" << endl;
 ofile << " <library_materials>" << endl;
 for(size_t i = 0; i < materials.size(); i++) {
     ofile << "  <material id=\"" << materials[i].id << "\">" << endl;
     ofile << "   <instance_effect url=\"#effects_" << setfill('0') << setw(3) << i << "\" />" << endl;
     ofile << "  </material>" << endl;
    }
 ofile << " </library_materials>" << endl;
 ofile << endl;

 // save <library_geometries>
 ofile << " <!-- LIBRARY_GEOMETRIES -->" << endl;
 ofile << " <library_geometries>" << endl;
 ofile << "  <geometry id=\"" << data.id << "\" name=\"" << data.name << "\">" << endl;
 ofile << "   <mesh>" << endl;

 // for each mesh
 for(size_t mesh_index = 0; mesh_index < data.meshlist.size(); mesh_index++)
    {
     // get vertices
     VTX_BUFFER& vertices = data.meshlist[mesh_index]->vertices;
     size_t n_vertices = vertices.elem;

     // get surfaces
     std::deque<IDX_BUFFER>& surfaces = data.meshlist[mesh_index]->surfaces;
     size_t n_surfaces = surfaces.size();

     // get properties
     bool has_vertex = (vertices.flags & VERTEX_POSITION) != 0;
     bool has_normal = (vertices.flags & VERTEX_NORMAL) != 0;
     bool has_texmap = (vertices.flags & VERTEX_UV) != 0;

     // vertex data identifiers
     stringstream pid; pid << data.id << "_position";
     stringstream nid; nid << data.id << "_normal";
     stringstream tid; tid << data.id << "_uv";
     stringstream vid; vid << data.id << "_vertices";
     
     // save <source> (positions)
     if(has_vertex) {
        size_t n_values = n_vertices*3;
        ofile << "    <source id=\"" << pid.str() << "\" name=\"" << pid.str() << "\">" << endl;
        ofile << "     <float_array id=\"" << pid.str() << "_array\" name=\"" << pid.str() << "_array\" count=\"" << n_values << "\">" << endl;
        for(size_t i = 0; i < n_vertices; i++) ofile << "      " << vertices.data[i].vx << " " << vertices.data[i].vy << " " << vertices.data[i].vz << endl;
        ofile << "     </float_array>" << endl;
        ofile << "     <technique_common>" << endl;
        ofile << "      <accessor source=\"#" << pid.str() << "_array\" count=\"" << n_vertices << "\" stride=\"3\">" << endl;
        ofile << "       <param name=\"X\" type=\"float\" />" << endl;
        ofile << "       <param name=\"Y\" type=\"float\" />" << endl;
        ofile << "       <param name=\"Z\" type=\"float\" />" << endl;
        ofile << "      </accessor>" << endl;
        ofile << "     </technique_common>" << endl;
        ofile << "    </source>" << endl;
       }

     // save <source> (normals)
     if(has_normal) {
        size_t n_values = n_vertices*3;
        ofile << "    <source id=\"" << nid.str() << "\" name=\"" << nid.str() << "\">" << endl;
        ofile << "     <float_array id=\"" << nid.str() << "_array\" name=\"" << nid.str() << "_array\" count=\"" << n_values << "\">" << endl;
        for(size_t i = 0; i < n_vertices; i++) ofile << "      " << vertices.data[i].nx << " " << vertices.data[i].ny << " " << vertices.data[i].nz << endl;
        ofile << "     </float_array>" << endl;
        ofile << "     <technique_common>" << endl;
        ofile << "      <accessor source=\"#" << nid.str() << "_array\" count=\"" << n_vertices << "\" stride=\"3\">" << endl;
        ofile << "       <param name=\"X\" type=\"float\" />" << endl;
        ofile << "       <param name=\"Y\" type=\"float\" />" << endl;
        ofile << "       <param name=\"Z\" type=\"float\" />" << endl;
        ofile << "      </accessor>" << endl;
        ofile << "     </technique_common>" << endl;
        ofile << "    </source>" << endl;
       }

     // save <source> (UV)
     if(has_texmap) {
        size_t n_values = n_vertices*2;
        ofile << "    <source id=\"" << tid.str() << "\" name=\"" << tid.str() << "\">" << endl;
        ofile << "     <float_array id=\"" << tid.str() << "_array\" name=\"" << tid.str() << "_array\" count=\"" << n_values << "\">" << endl;
        for(size_t i = 0; i < n_vertices; i++) ofile << "      " << vertices.data[i].tu << " " << vertices.data[i].tv << endl;
        ofile << "     </float_array>" << endl;
        ofile << "     <technique_common>" << endl;
        ofile << "      <accessor source=\"#" << tid.str() << "_array\" count=\"" << n_vertices << "\" stride=\"2\">" << endl;
        ofile << "       <param name=\"S\" type=\"float\" />" << endl;
        ofile << "       <param name=\"T\" type=\"float\" />" << endl;
        ofile << "      </accessor>" << endl;
        ofile << "     </technique_common>" << endl;
        ofile << "    </source>" << endl;
       }

     // save <vertices>
     if(has_vertex) {
        ofile << "    <vertices id=\"" << vid.str() << "\">" << endl;
        ofile << "     <input semantic=\"POSITION\" source=\"#" << pid.str() << "\" />" << endl;
        ofile << "    </vertices>" << endl;
       }

     // save <polygons>
     for(size_t i = 0; i < surfaces.size(); i++)
        {
         unsigned char type = surfaces[i].type;
         unsigned char format = surfaces[i].format;

         // material index
         uint32 material_index = surfaces[i].material;
         if(!(material_index < materials.size())) return error("DAE EXPORT: Material index out of range.");
         string material_id = materials[material_index].id;

         if(type == FACE_TYPE_TRIANGLES)
           {
            // begin
            size_t n_triangles = surfaces[i].elem/3;
            ofile << "    <triangles count=\"" << n_triangles << "\" material=\"" << material_id << "\">" << endl;

            // position + normals + texture
            if(has_vertex && has_normal && has_texmap) {
               ofile << "     <input semantic=\"VERTEX\" source=\"#" << vid.str() << "\" offset=\"0\" />" << endl;
               ofile << "     <input semantic=\"NORMAL\" source=\"#" << nid.str() << "\" offset=\"1\" />" << endl;
               ofile << "     <input semantic=\"TEXCOORD\" source=\"#" << tid.str() << "\" offset=\"2\" />" << endl;
               ofile << "     <p>" << endl;

               if(format == FACE_FORMAT_UINT_08) {
                  uint08* data = reinterpret_cast<uint08*>(surfaces[i].data.get());
                  size_t curr = 0;
                  for(size_t j = 0; j < n_triangles; j++) {
                      size_t a = data[curr++];
                      size_t b = data[curr++];
                      size_t c = data[curr++];
                      ofile << "     " << a << " " << a << " " <<  a << " " << b << " " << b << " " << b << " " << c << " " << c << " " << c << endl;
                     }
                 }
               else if(format == FACE_FORMAT_UINT_16) {
                  uint16* data = reinterpret_cast<uint16*>(surfaces[i].data.get());
                  size_t curr = 0;
                  for(size_t j = 0; j < n_triangles; j++) {
                      size_t a = data[curr++];
                      size_t b = data[curr++];
                      size_t c = data[curr++];
                      ofile << "     " << a << " " << a << " " << a << " " << b << " " << b << " " << b << " " << c << " " << c << " " << c << endl;
                     }
                 }
               else if(format == FACE_FORMAT_UINT_32) {
                  uint32* data = reinterpret_cast<uint32*>(surfaces[i].data.get());
                  size_t curr = 0;
                  for(size_t j = 0; j < n_triangles; j++) {
                      size_t a = data[curr++];
                      size_t b = data[curr++];
                      size_t c = data[curr++];
                      ofile << "     " << a << " " << a << " " << a << " " << b << " " << b << " " << b << " " << c << " " << c << " " << c << endl;
                     }
                 }

               ofile << "</p>" << endl;
              }
            // position
            else if(has_vertex && !has_normal && !has_texmap) {
               ofile << "     <input semantic=\"VERTEX\" source=\"#" << vid.str() << "\" offset=\"0\" />" << endl;
              }
            // position + texture
            else if(!has_normal && has_texmap)
              {
               ofile << "     <input semantic=\"VERTEX\" source=\"#" << vid.str() << "\" offset=\"0\" />" << endl;
               ofile << "     <input semantic=\"TEXCOORD\" source=\"#" << tid.str() << "\" offset=\"1\" />" << endl;
               ofile << "     <p>" << endl;

               if(format == FACE_FORMAT_UINT_08) {
                  uint08* data = reinterpret_cast<uint08*>(surfaces[i].data.get());
                  size_t curr = 0;
                  for(size_t j = 0; j < n_triangles; j++) {
                      size_t a = data[curr++];
                      size_t b = data[curr++];
                      size_t c = data[curr++];
                      ofile << "     " << a << " " << a << " " << b << " " << b << " " << c << " " << c << endl;
                     }
                 }
               else if(format == FACE_FORMAT_UINT_16) {
                  uint16* data = reinterpret_cast<uint16*>(surfaces[i].data.get());
                  size_t curr = 0;
                  for(size_t j = 0; j < n_triangles; j++) {
                      size_t a = data[curr++];
                      size_t b = data[curr++];
                      size_t c = data[curr++];
                      ofile << "     " << a << " " << a << " " << b << " " << b << " " << c << " " << c << endl;
                     }
                 }
               else if(format == FACE_FORMAT_UINT_32) {
                  uint32* data = reinterpret_cast<uint32*>(surfaces[i].data.get());
                  size_t curr = 0;
                  for(size_t j = 0; j < n_triangles; j++) {
                      size_t a = data[curr++];
                      size_t b = data[curr++];
                      size_t c = data[curr++];
                      ofile << "     " << a << " " << a << " " << b << " " << b << " " << c << " " << c << endl;
                     }
                 }

               ofile << "     </p>" << endl;
              }
            // position + normals
            else if(has_normal && !has_texmap)
              {
               ofile << "     <input semantic=\"VERTEX\" source=\"#" << vid.str() << "\" offset=\"0\" />" << endl;
               ofile << "     <input semantic=\"NORMAL\" source=\"#" << nid.str() << "\" offset=\"1\" />" << endl;
               ofile << "     <p>" << endl;

               if(format == FACE_FORMAT_UINT_08) {
                  uint08* data = reinterpret_cast<uint08*>(surfaces[i].data.get());
                  size_t curr = 0;
                  for(size_t j = 0; j < n_triangles; j++) {
                      size_t a = data[curr++];
                      size_t b = data[curr++];
                      size_t c = data[curr++];
                      ofile << "     " << a << " " << a << " " << b << " " << b << " " << c << " " << c << endl;
                     }
                 }
               else if(format == FACE_FORMAT_UINT_16) {
                  uint16* data = reinterpret_cast<uint16*>(surfaces[i].data.get());
                  size_t curr = 0;
                  for(size_t j = 0; j < n_triangles; j++) {
                      size_t a = data[curr++];
                      size_t b = data[curr++];
                      size_t c = data[curr++];
                      ofile << "     " << a << " " << a << " " << b << " " << b << " " << c << " " << c << endl;
                     }
                 }
               else if(format == FACE_FORMAT_UINT_32) {
                  uint32* data = reinterpret_cast<uint32*>(surfaces[i].data.get());
                  size_t curr = 0;
                  for(size_t j = 0; j < n_triangles; j++) {
                      size_t a = data[curr++];
                      size_t b = data[curr++];
                      size_t c = data[curr++];
                      ofile << "     " << a << " " << a << " " << b << " " << b << " " << c << " " << c << endl;
                     }
                 }

               ofile << "     </p>" << endl;
              }
            else return error("DAE EXPORT: Invalid input semantics.");

            // end
            ofile << "    </triangles>" << endl;
           }
         else if(type == FACE_TYPE_TRISTRIP)
           {
           }
         else if(type == FACE_TYPE_TRISTRIPCUT)
           {
           }
         else
            return error("DAE EXPORT: Invalid face type.");
        }
    }

 // terminate <library_geometries>
 ofile << "   </mesh>" << endl;
 ofile << "  </geometry>" << endl;
 ofile << " </library_geometries>" << endl;
 ofile << endl;

 // skeleton and weights
 bool has_skeletons = (sl.size() > 0);
 if(has_skeletons)
   {
    // save <library_nodes>
    ofile << " <!-- LIBRARY_NODES -->" << endl;
    ofile << " <library_nodes>" << endl;
    for(size_t i = 0; i < sl.size(); i++) {
        stringstream ss;
        sl[i].tree.PrintColladaNodeHeirarchy(sl[i].id, ss);
        ofile << ss.str();
       }
    ofile << " </library_nodes>" << endl;
    ofile << endl;

    // save <library_controllers>
    ofile << " <!-- LIBRARY_CONTROLLERS -->" << endl;
    ofile << " <library_controllers>" << endl;
    for(size_t i = 0; i < data.meshlist.size(); i++)
       {
        ofile << "  <controller id=\"" << data.id << "_controller\">" << endl;
        ofile << "   <skin source=\"#" << data.id << "\">" << endl;
        for(size_t j = 0; j < sl.size(); j++) {
            stringstream ss;
            sl[j].tree.PrintColladaJoints(sl[j].id, ss);
            ofile << ss.str();
           }
        for(size_t j = 0; j < sl.size(); j++) {
            stringstream ss;
            sl[j].tree.PrintColladaBindMatrices(sl[j].id, ss);
            ofile << ss.str();
           }
        // vertex weights
        if(data.meshlist[i]->vertices.flags & VERTEX_WEIGHTS)
          {
           size_t n_weights = 4*data.meshlist[i]->vertices.elem;
           ofile << "    <source id=\"" << data.id << "_weights\">" << endl;
           ofile << "     <float_array count=\"" << n_weights << "\">" << endl;
           for(size_t j = 0; j < data.meshlist[i]->vertices.elem; j++) {
               float w1 = data.meshlist[i]->vertices.data[j].w1;
               float w2 = data.meshlist[i]->vertices.data[j].w2;
               float w3 = data.meshlist[i]->vertices.data[j].w3;
               float w4 = data.meshlist[i]->vertices.data[j].w4;
               ofile << "      " << w1 << " " << w2 << " " << w3 << " " << w4 << endl;
              }
           ofile << "     </float_array>" << endl;
           ofile << "     <technique_common>" << endl;
           ofile << "      <acessor source=\"#" << data.id << "_weights\" count=\"" << n_weights << "\" stride=\"1\">" << endl;
           ofile << "       <param name=\"WEIGHT\" type=\"float\" />" << endl;
           ofile << "      </acessor>" << endl;
           ofile << "     </technique_common>" << endl;
           ofile << "    </source>" << endl;
          }
        // joints
        for(size_t j = 0; j < sl.size(); j++) {
            ofile << "    <joints>" << endl;
            ofile << "     <input semantic=\"JOINT\" source=\"#" << sl[j].id << "_joints\" />" << endl;
            ofile << "     <input semantic=\"INV_BIND_MATRIX\" source=\"#" << sl[j].id << "_matrices\" />" << endl;
            ofile << "    </joints>" << endl;
           }
        // vertex weights
        for(size_t j = 0; j < sl.size(); j++) {
            ofile << "    <vertex_weights count=\"" << data.meshlist[i]->vertices.elem << "\">" << endl;
            ofile << "     <input semantic=\"JOINT\" source=\"#" << sl[j].id << "_joints\"/>" << endl;
            ofile << "     <input semantic=\"WEIGHT\" source=\"#" << data.id << "_weights\"/>" << endl;
            ofile << "     <vcount>";
            for(size_t k = 0; k < data.meshlist[i]->vertices.elem - 1; k++) ofile << "4 ";
            ofile << 4 << "</vcount>" << endl;
            ofile << "     <v>" << endl;
            for(size_t k = 0; k < data.meshlist[i]->vertices.elem; k++) {
                size_t weight_offset = 4*k;
                ofile << "      " <<
                         data.meshlist[i]->vertices.data[k].b1 << " " << weight_offset + 0 << " " <<
                         data.meshlist[i]->vertices.data[k].b2 << " " << weight_offset + 1 << " " <<
                         data.meshlist[i]->vertices.data[k].b3 << " " << weight_offset + 2 << " " <<
                         data.meshlist[i]->vertices.data[k].b4 << " " << weight_offset + 3 << endl;
               }
            ofile << "     </v>" << endl;
            ofile << "    </vertex_weights>" << endl;
           }
        ofile << "   </skin>" << endl;
        ofile << "  </controller>" << endl;
       }
    ofile << " </library_controllers>" << endl;
    ofile << endl;
   }

 // save <scene>
 ofile << " <!-- SCENE -->" << endl;
 ofile << " <library_visual_scenes>" << endl;
 ofile << "  <visual_scene id=\"DefaultScene\">" << endl;
 ofile << "   <node id=\"DefaultNode\" name=\"DefaultNode\">" << endl;
 ofile << "    <rotate>1 0 0 90</rotate>" << endl;
 ofile << "    <instance_geometry url=\"#" << data.id << "\">" << endl;
 for(size_t i = 0; i < materials.size(); i++)
    {
     ofile << "     <bind_material>" << endl;
	 ofile << "      <technique_common>" << endl;
     ofile << "       <instance_material symbol=\"" << materials[i].id << "\" target=\"#" << materials[i].id << "\">" << endl;
     ofile << "        <bind_vertex_input semantic=\"TEX0\" input_semantic=\"TEXCOORD\" input_set=\"0\" />" << endl;
     ofile << "       </instance_material>" << endl;
	 ofile << "      </technique_common>" << endl;
     ofile << "     </bind_material>" << endl;
    }
 ofile << "    </instance_geometry>" << endl;
 ofile << "   </node>" << endl;

 // create skeleton instances
 for(size_t i = 0; i < sl.size(); i++)
    {
     // create skeleton instance (refers to a list of node names)
     ofile << "   <!-- SKELETON INSTANCE -->" << endl;
     ofile << "   <node id=\"" << sl[i].id << "_instance\">" << endl;
     ofile << "    <rotate>1 0 0 90</rotate>" << endl;
     ofile << "    <instance_node url=\"#" << sl[i].id << "\" />" << endl;
     ofile << "   </node>" << endl;
     // create skeleton controller (must define which skeleton and which controller to use)
     ofile << "   <!-- SKELETON CONTROLLER INSTANCE -->" << endl;
     ofile << "   <node>" << endl;
     ofile << "    <instance_controller url=\"#" << data.id << "_controller\">" << endl;
     ofile << "     <skeleton>#" << sl[i].id << "_instance</skeleton>" << endl;
     ofile << "    </instance_controller>" << endl;
     ofile << "   </node>" << endl;
    }
 ofile << "  </visual_scene>" << endl;
 ofile << " </library_visual_scenes>" << endl;
 ofile << endl;

 // save <scene>
 ofile << " <!-- SCENE -->" << endl;
 ofile << " <scene>" << endl;
 ofile << "  <instance_visual_scene url=\"#DefaultScene\"></instance_visual_scene>" << endl;
 ofile << " </scene>" << endl;
 ofile << endl;

 // finish <collada>
 ofile << "</COLLADA>";
 return true;
}

*/