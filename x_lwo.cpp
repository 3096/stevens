#include "xentax.h"
#include "x_lwo.h"

template<class T>
inline uint32 GetVXBytes(T index)
{
 return ((index < 0xFF00) ? 2 : 4);
}

void WriteVX(std::ofstream& ofile, uint32 index)
{
 if(index < 0xFF00)
    BE_write_uint16(ofile, (uint16)index);
 else {
    reverse_byte_order(&index);
    index |= 0x000000FF;
    reverse_byte_order(&index);
    BE_write_uint32(ofile, index);
   }
}

lwFileSaver::lwFileSaver()
{
 tags.chunksize = 0;
 tags.currindex = 0;
}

lwFileSaver::~lwFileSaver()
{
}

bool lwFileSaver::hasTxuv(const char* name)const
{
 for(std::map<uint16, lwLAYR>::const_iterator liter = layrs.begin(); liter != layrs.end(); liter++) {
     const std::map<uint16, lwPNTS>& pntss = liter->second.pntss;
     for(std::map<uint16, lwPNTS>::const_iterator piter = pntss.begin(); piter != pntss.end(); piter++) {
         const std::map<std::string, lwTXUV>& uvmap = piter->second.txuv;
         if(piter->second.txuv.find(name) != uvmap.end()) return true;
        }
    }
 return false;
}

bool lwFileSaver::insertClip(uint32 clipindex, const char* filename)
{
 if(!filename) return false;
 if(!strlen(filename)) return false;

 // create CLIP
 lwCLIP item;
 item.chunksize = 0;
 item.filename = filename;

 // compute chunksize
 item.chunksize += 0x04; // clip index
 item.chunksize += 0x04; // STIL
 item.chunksize += 0x02; // STIL chunksize
 item.chunksize += align02(item.filename.length() + 1); // string
 item.chunksize += 0x04; // FLAG
 item.chunksize += 0x02; // FLAG chunksize
 item.chunksize += 0x02; // FLAG data
 item.chunksize += 0x02; // FLAG data

 // add clip
 clips.insert(std::map<uint32, lwCLIP>::value_type(clipindex, item));
 return true;
}

bool lwFileSaver::insertLayr(uint16 layer, const char* name)
{
 // validate
 using namespace std;
 if(!(layer < LWO_MAX_LAYERS)) return false;

 // create LAYR
 lwLAYR item;
 item.chunksize = 0;
 item.flags = 0;
 item.pivot_x = 0;
 item.pivot_y = 0;
 item.pivot_z = 0; 
 item.name = (name ? name : "");

 // compute LAYR chunksize
 item.chunksize += 0x02; // number
 item.chunksize += 0x02; // flags
 item.chunksize += 0x0C; // pivot
 item.chunksize += align02(item.name.length() + 1); // name

 // add LAYR
 layrs.insert(map<uint32, lwLAYR>::value_type(layer, item));
 return true;
}

bool lwFileSaver::insertPnts(uint16 layer, uint16 pointset, const boost::shared_array<lwVertex3D>& data, uint32 elem)
{
 // validate
 using namespace std;
 if(!(layer < LWO_MAX_LAYERS)) return error("lwFileSaver: Invalid layer number.");
 if(!data.get()) return error("lwFileSaver: Invalid pointer argument.");
 if(!elem) return error("lwFileSaver: Invalid number of point elements.");

 // validate
 map<uint16, lwLAYR>::iterator parent = layrs.find(layer);
 if(layrs.find(layer) == layrs.end()) return error("lwFileSaver: Layer not found.");

 // create PNTS
 lwPNTS item;
 item.chunksize = 0;
 item.data = data;
 item.elem = elem;

 // compute PNTS chunksize
 item.chunksize += elem*0x04; // x
 item.chunksize += elem*0x04; // y
 item.chunksize += elem*0x04; // z

 // pointset must not exist
 map<uint16, lwPNTS>& pmap = parent->second.pntss;
 if(pmap.find(pointset) != pmap.end()) return error("lwFileSaver: Point set already exists.");

 // add PNTS
 typedef map<uint16, lwPNTS>::value_type value_type;
 pmap.insert(value_type(pointset, item));
 return true;
}

bool lwFileSaver::insertTxuv(uint16 layer, uint16 pointset, const char* name, const boost::shared_array<lwRefVertex2D>& data, uint32 elem)
{
 // validate
 using namespace std;
 if(!(layer < LWO_MAX_LAYERS)) return error("lwFileSave: Invalid layer number.");
 if(!data.get()) return error("lwFileSaver: Invalid pointer argument.");
 if(!elem) return error("lwFileSaver: Invalid number of point elements.");
 if(!strlen(name)) return error("lwFileSaver: UV maps must have a valid name.");

 // validate
 map<uint16, lwLAYR>::iterator parent = layrs.find(layer);
 if(layrs.find(layer) == layrs.end()) return error("lwFileSaver: Layer not found.");

 // pointset must exist
 map<uint16, lwPNTS>::iterator pset = parent->second.pntss.find(pointset);
 if(pset == parent->second.pntss.end()) return error("lwFileSaver: A point set must exist to add a UV map.");

 // validate references
 for(uint32 i = 0; i < elem; i++)
     if(!(data[i].reference < pset->second.elem))
        return error("lwFileSaver: UV index out of range.");

 // no TXUV data yet
 auto TXUViter = pset->second.txuv.find(name);
 if(TXUViter == pset->second.txuv.end())
   {
    // create TXUV
    lwTXUV item;
    item.chunksize = 0;
    item.elem = elem;
    for(uint32 i = 0; i < elem; i++) item.data.push_back(data[i]);

    // compute TXUV chunksize
    item.chunksize += 0x04; // TXUV
    item.chunksize += 0x02; // dimension
    item.chunksize += align02(strlen(name) + 1); // name
    for(uint32 i = 0; i < elem; i++) item.chunksize += GetVXBytes(data[i].reference); // VX
    item.chunksize += elem*0x04; // u
    item.chunksize += elem*0x04; // v

    // insert TXUV
    typedef map<string, lwTXUV>::value_type value_type;
    pset->second.txuv.insert(value_type(name, item));
   }
 // modify TXUV
 else
   {
    TXUViter->second.elem += elem;
    for(uint32 i = 0; i < elem; i++) {
        TXUViter->second.chunksize += GetVXBytes(data[i].reference); // VX
        TXUViter->second.data.push_back(data[i]);
       }
    TXUViter->second.chunksize += elem*0x04; // u
    TXUViter->second.chunksize += elem*0x04; // v
   }

 return true;
}

bool lwFileSaver::insertWght(uint16 layer, uint16 pointset, const char* name, const boost::shared_array<lwRefVertex1D>& data, uint32 elem)
{
 // validate
 using namespace std;
 if(!(layer < LWO_MAX_LAYERS)) return error("lwFileSave: Invalid layer number.");
 if(!data.get()) return error("lwFileSaver: Invalid pointer argument.");
 if(!elem) return error("lwFileSaver: Invalid number of point elements.");
 if(!strlen(name)) return error("lwFileSaver: Weight maps must have a valid name.");

 // validate
 map<uint16, lwLAYR>::iterator parent = layrs.find(layer);
 if(layrs.find(layer) == layrs.end()) return error("lwFileSaver: Layer not found.");

 // pointset must exist
 map<uint16, lwPNTS>::iterator pset = parent->second.pntss.find(pointset);
 if(pset == parent->second.pntss.end()) return error("lwFileSaver: A point set must exist to add a weight map.");

 // validate references
 for(uint32 i = 0; i < elem; i++)
     if(!(data[i].reference < pset->second.elem))
        return error("lwFileSaver: Weight index out of range.");

 // no WGHT data yet
 auto WGHTiter = pset->second.wght.find(name);
 if(WGHTiter == pset->second.wght.end())
   {
    // create WGHT
    lwWGHT item;
    item.chunksize = 0;
    item.elem = elem;
    for(uint32 i = 0; i < elem; i++) item.data.push_back(data[i]);

    // compute WGHT chunksize
    item.chunksize += 0x04; // WGHT
    item.chunksize += 0x02; // dimension
    item.chunksize += align02(strlen(name) + 1); // name
    for(uint32 i = 0; i < elem; i++) item.chunksize += GetVXBytes(data[i].reference); // VX
    item.chunksize += elem*0x04; // weight

    // insert WGHT
    typedef map<string, lwWGHT>::value_type value_type;
    pset->second.wght.insert(value_type(name, item));
   }
 // modify WGHT
 else
   {
    WGHTiter->second.elem += elem;
    for(uint32 i = 0; i < elem; i++) {
        WGHTiter->second.chunksize += GetVXBytes(data[i].reference); // VX
        WGHTiter->second.data.push_back(data[i]);
       }
    WGHTiter->second.chunksize += elem*0x04; // weight
   }

 return true;
}

bool lwFileSaver::insertMorf(uint16 layer, uint16 pointset, const char* name, const boost::shared_array<lwRefVertex3D>& data, uint32 elem)
{
 // validate
 using namespace std;
 if(!(layer < LWO_MAX_LAYERS)) return error("lwFileSaver: Invalid layer number.");
 if(!data.get()) return error("lwFileSaver: Invalid pointer argument.");
 if(!elem) return error("lwFileSaver: Invalid number of point elements.");
 if(!strlen(name)) return error("lwFileSaver: Morph maps must have a valid name.");

 // validate
 map<uint16, lwLAYR>::iterator parent = layrs.find(layer);
 if(layrs.find(layer) == layrs.end()) return error("lwFileSaver: Layer not found.");

 // create MORF
 lwMORF item;
 item.chunksize = 0;
 item.data = data;
 item.elem = elem;

 // compute MORF chunksize
 item.chunksize += 0x04; // MORF
 item.chunksize += 0x02; // dimension
 item.chunksize += align02(strlen(name) + 1); // name
 for(uint32 i = 0; i < elem; i++) item.chunksize += GetVXBytes(data[i].reference); // VX
 item.chunksize += elem*0x04; // x
 item.chunksize += elem*0x04; // y
 item.chunksize += elem*0x04; // z

 // pointset must exist
 map<uint16, lwPNTS>::iterator pset = parent->second.pntss.find(pointset);
 if(pset == parent->second.pntss.end()) return error("lwFileSaver: A point set must exist to add a morph map.");

 // validate references
 for(uint32 i = 0; i < elem; i++)
     if(!(data[i].reference < pset->second.elem))
        return error("lwFileSaver: Morph index out of range.");

 // add MORF
 typedef map<string, lwMORF>::value_type value_type;
 pset->second.morf.insert(value_type(name, item));
 return true;
}

bool lwFileSaver::insertNorm(uint16 layer, uint16 pointset, const char* name, const boost::shared_array<lwRefVertex3D>& data, uint32 elem)
{
 // validate
 using namespace std;
 if(!(layer < LWO_MAX_LAYERS)) return error("lwFileSave: Invalid layer number.");
 if(!data.get()) return error("lwFileSaver: Invalid pointer argument.");
 if(!elem) return error("lwFileSaver: Invalid number of point elements.");
 if(!strlen(name)) return error("lwFileSaver: Norm maps must have a valid name.");

 // validate
 map<uint16, lwLAYR>::iterator parent = layrs.find(layer);
 if(layrs.find(layer) == layrs.end()) return error("lwFileSaver: Layer not found.");

 // pointset must exist
 map<uint16, lwPNTS>::iterator pset = parent->second.pntss.find(pointset);
 if(pset == parent->second.pntss.end()) return error("lwFileSaver: A point set must exist to add a norm map.");

 // validate references
 for(uint32 i = 0; i < elem; i++)
     if(!(data[i].reference < pset->second.elem))
        return error("lwFileSaver: Norm index out of range.");

 // no NORM data yet
 auto NORMiter = pset->second.norm.find(name);
 if(NORMiter == pset->second.norm.end())
   {
    // create NORM
    lwNORM item;
    item.chunksize = 0;
    item.elem = elem;
    for(uint32 i = 0; i < elem; i++) item.data.push_back(data[i]);

    // compute NORM chunksize
    item.chunksize += 0x04; // NORM
    item.chunksize += 0x02; // dimension
    item.chunksize += align02(strlen(name) + 1); // name
    for(uint32 i = 0; i < elem; i++) item.chunksize += GetVXBytes(data[i].reference); // VX
    item.chunksize += elem*0x04; // x
    item.chunksize += elem*0x04; // y
    item.chunksize += elem*0x04; // z

    // insert NORM
    typedef map<string, lwNORM>::value_type value_type;
    pset->second.norm.insert(value_type(name, item));
   }
 // modify NORM
 else
   {
    NORMiter->second.elem += elem;
    for(uint32 i = 0; i < elem; i++) {
        NORMiter->second.chunksize += GetVXBytes(data[i].reference); // VX
        NORMiter->second.data.push_back(data[i]);
       }
    NORMiter->second.chunksize += elem*0x04; // x
    NORMiter->second.chunksize += elem*0x04; // y
    NORMiter->second.chunksize += elem*0x04; // z
   }

 return true;
}

bool lwFileSaver::insertPols(uint16 layer, uint16 pointset, const char* name, const boost::shared_array<lwTriangle>& data, uint32 elem)
{
 // validate
 using namespace std;
 if(!(layer < LWO_MAX_LAYERS)) return error("lwFileSave: Invalid layer number.");
 if(!data.get()) return error("lwFileSaver: Invalid pointer argument.");
 if(!elem) return error("lwFileSaver: Invalid number of triangle elements.");

 // layer must exist
 map<uint16, lwLAYR>::iterator parent = layrs.find(layer);
 if(layrs.find(layer) == layrs.end()) return error("lwFileSaver: Layer not found.");

 // pointset must exist
 map<uint16, lwPNTS>::iterator pset = parent->second.pntss.find(pointset);
 if(pset == parent->second.pntss.end()) return error("lwFileSaver: A point set must exist to add polygons.");

 // create POLS
 lwPOLS item;
 item.chunksize = 0;
 item.name = (((name == nullptr) || !strlen(name)) ? "Default" : name);
 item.data = data;
 item.elem = elem;

 // compute POLS chunksize
 item.chunksize += 0x04; // FACE
 for(uint32 i = 0; i < elem; i++) {
     item.chunksize += 0x02; // number of vertices
     item.chunksize += GetVXBytes(data[i].a); // VX
     item.chunksize += GetVXBytes(data[i].b); // VX
     item.chunksize += GetVXBytes(data[i].c); // VX
    }

 // validate references
 for(uint32 i = 0; i < elem; i++) {
     if(!(data[i].a < pset->second.elem)) {
        stringstream ss;
        ss << "lwFileSaver: Face index out of range " << data[i].a << " is greater than " << pset->second.elem << ".";
        return error(ss.str().c_str());
       }
     if(!(data[i].b < pset->second.elem)) {
        stringstream ss;
        ss << "lwFileSaver: Face index out of range " << data[i].b << " is greater than " << pset->second.elem << ".";
        return error(ss.str().c_str());
       }
     if(!(data[i].c < pset->second.elem)) {
        stringstream ss;
        ss << "lwFileSaver: Face index out of range " << data[i].c << " is greater than " << pset->second.elem << ".";
        return error(ss.str().c_str());
       }
    }

 // create surface
 if(!insertSurf(item.name.c_str()))
    return false;

 // find surface TAGS index
 typedef boost::bimap<uint16, std::string> bmap_t;
 bmap_t::right_iterator tagiter = tags.surftags.right.find(item.name);
 if(tagiter == tags.surftags.right.end()) return error("lwFileSaver: Could not find surface tag index.");

 // create PTAG
 item.surf.chunksize = 0;    // PTAG chunksize
 item.surf.type = LWO_SURF;  // PTAG type

 // compute PTAG chunksize
 item.surf.chunksize += 0x04; // SURF
 if(item.elem < 0xFF00) item.surf.chunksize += 0x02 * item.elem; // VX data
 else item.surf.chunksize += (0x02 * 0xFF00) + (0x04 * (item.elem - 0xFF00)); // VX data
 item.surf.chunksize += 0x02 * item.elem; // tag index data

 // compute PTAG data
 for(uint32 i = 0; i < item.elem; i++) {
     typedef map<uint32, uint16>::value_type value_type;
     item.surf.tags.insert(value_type(i, tagiter->second));
    }

 // set POLS
 pset->second.face.push_back(item);
 return true;
}

bool lwFileSaver::insertJnts(uint16 layer, uint16 jointset, const boost::shared_array<lwVertex3D>& data, uint32 elem)
{
 // validate
 using namespace std;
 if(!(layer < LWO_MAX_LAYERS)) return error("lwFileSaver: Invalid layer number.");
 if(!data.get()) return error("lwFileSaver: Invalid pointer argument.");
 if(!elem) return error("lwFileSaver: Invalid number of joints.");

 // find layer
 map<uint16, lwLAYR>::iterator parent = layrs.find(layer);
 if(layrs.find(layer) == layrs.end()) return error("Layer not found.");

 // create JNTS
 lwJNTS item;
 item.chunksize = 0;
 item.data = data;
 item.elem = elem;

 // compute JNTS chunksize
 item.chunksize += elem*0x04; // x
 item.chunksize += elem*0x04; // y
 item.chunksize += elem*0x04; // z

 // jointset must not exist
 map<uint16, lwJNTS>& jmap = parent->second.jntss;
 if(jmap.find(jointset) != jmap.end()) return error("Joint set already exists.");

 // add JNTS
 typedef map<uint16, lwJNTS>::value_type value_type;
 jmap.insert(value_type(jointset, item));
 return true;
}

bool lwFileSaver::insertSkel(uint16 layer, uint16 jointset, const char* name, const boost::shared_array<lwSkelegon>& data, uint32 elem)
{
 // validate
 using namespace std;
 if(!(layer < LWO_MAX_LAYERS)) return error("Invalid layer number.");
 if(!name || !strlen(name)) return error("Invalid skeleton name.");
 if(!data.get()) return error("Invalid pointer argument.");
 if(!elem) return error("Invalid number of skelegon elements.");

 // layer must exist
 map<uint16, lwLAYR>::iterator parent = layrs.find(layer);
 if(layrs.find(layer) == layrs.end()) return error("Layer not found.");

 // jointset must exist
 map<uint16, lwJNTS>::iterator jset = parent->second.jntss.find(jointset);
 if(jset == parent->second.jntss.end()) return error("Jointset does not exist.");

 // create SKEL
 lwSKEL item;
 item.chunksize = 0;
 item.name = name;
 item.data = data;
 item.elem = elem;
 item.stag.chunksize = 0;
 item.btag.chunksize = 0;
 item.ptag.chunksize = 0;
 item.utag.chunksize = 0;

 // compute SKEL chunksize
 item.chunksize += 0x04; // BONE
 for(uint32 i = 0; i < elem; i++) {
     item.chunksize += 0x02; // number of vertices
     item.chunksize += GetVXBytes(data[i].a); // VX
     item.chunksize += GetVXBytes(data[i].b); // VX
    }

 // validate joint references
 for(uint32 i = 0; i < elem; i++) {
     if(!(data[i].a < jset->second.elem)) return error("Joint index out of range.");
     if(!(data[i].b < jset->second.elem)) return error("Joint index out of range.");
    }

 // create surface
 if(!insertSurf(name))
    return false;

 // find surface TAGS index
 typedef boost::bimap<uint16, std::string> bmap_t;
 bmap_t::right_iterator tagiter = tags.surftags.right.find(item.name);
 if(tagiter == tags.surftags.right.end()) return error("Could not find surface tag index.");

 // create PTAG:SURF
 item.stag.chunksize = 0;   // PTAG chunksize
 item.stag.type = LWO_SURF; // PTAG type

 // compute PTAG:SURF chunksize
 item.stag.chunksize += 0x04; // SURF
 if(item.elem < 0xFF00) item.stag.chunksize += 0x02 * item.elem; // VX data
 else item.stag.chunksize += (0x02 * 0xFF00) + (0x04 * (item.elem - 0xFF00)); // VX data
 item.stag.chunksize += 0x02 * item.elem; // tag index data

 // compute PTAG:SURF data
 for(uint32 i = 0; i < item.elem; i++) {
     typedef map<uint32, uint16>::value_type value_type;
     item.stag.tags.insert(value_type(i, tagiter->second));
    }

 // create bones and parts
 for(uint32 i = 0; i < item.elem; i++) {
     if(!insertBone(data[i].name.c_str())) return false;
     if(!insertPart(data[i].name.c_str())) return false;
    }

 // create PTAG:BONE
 item.btag.chunksize = 0;
 item.btag.type = LWO_BONE;

 // compute PTAG:BONE chunksize
 item.btag.chunksize += 0x04; // PART
 if(item.elem < 0xFF00) item.btag.chunksize += 0x02 * item.elem; // VX data
 else item.btag.chunksize += (0x02 * 0xFF00) + (0x04 * (item.elem - 0xFF00)); // VX data
 item.btag.chunksize += 0x02 * item.elem; // tag index data

 // compute PTAG:BONE data
 for(uint32 i = 0; i < item.elem; i++) {
     bmap_t::right_iterator iter = tags.bonetags.right.find(data[i].name);
     if(iter == tags.bonetags.right.end()) return error("Could not find bone tag index.");
     typedef map<uint32, uint16>::value_type value_type;
     item.btag.tags.insert(value_type(i, iter->second));
    }

 // create PTAG:PART
 item.ptag.chunksize = 0;
 item.ptag.type = LWO_PART;

 // compute PTAG:PART chunksize
 item.ptag.chunksize += 0x04; // PART
 if(item.elem < 0xFF00) item.ptag.chunksize += 0x02 * item.elem; // VX data
 else item.ptag.chunksize += (0x02 * 0xFF00) + (0x04 * (item.elem - 0xFF00)); // VX data
 item.ptag.chunksize += 0x02 * item.elem; // tag index data

 // compute PTAG:PART data
 for(uint32 i = 0; i < item.elem; i++) {
     bmap_t::right_iterator iter = tags.parttags.right.find(data[i].name);
     if(iter == tags.parttags.right.end()) return error("Could not find part tag index.");
     typedef map<uint32, uint16>::value_type value_type;
     item.ptag.tags.insert(value_type(i, iter->second));
    }

 // set SKEL
 jset->second.skel.push_back(item);
 return true;
}

bool lwFileSaver::insertBone(const char* name)
{
 // validate
 using namespace std;
 if(!name) return error("Invalid part name.");
 if(!strlen(name)) return error("Invalid part name.");

 // BONE already exists
 if(tags.bonetags.right.find(name) != tags.bonetags.right.end())
    return error("Bone names must be unique.");

 // add BONE to TAGS
 typedef boost::bimap<uint16, string>::value_type value_type;
 tags.bonetags.insert(value_type(tags.currindex, name));
 tags.chunksize += align02(strlen(name) + 1);
 tags.currindex++;

 return true;
}

bool lwFileSaver::insertPart(const char* name)
{
 // validate
 using namespace std;
 if(!name) return error("Invalid part name.");
 if(!strlen(name)) return error("Invalid part name.");

 // PART already exists (OK, not an error)
 if(tags.parttags.right.find(name) != tags.parttags.right.end())
    return true;

 // add PART to TAGS
 typedef boost::bimap<uint16, string>::value_type value_type;
 tags.parttags.insert(value_type(tags.currindex, name));
 tags.chunksize += align02(strlen(name) + 1);
 tags.currindex++;

 return true;
}

bool lwFileSaver::insertSurf(const char* name)
{
 // validate
 using namespace std;
 if(!name) return error("lwFileSaver: Invalid surface name.");
 if(!strlen(name)) return error("lwFileSaver: Invalid surface name.");

 // surface already exists (OK, not an error)
 if(surfs.find(name) != surfs.end())
    return true;

 // create SURF
 lwSURF item;
 item.chunksize = 0;
 item.flags = LWO_SURF_FLAGS_COLR | LWO_SURF_FLAGS_DIFF | LWO_SURF_FLAGS_SPEC | LWO_SURF_FLAGS_TRAN;
 item.colr[0] = 0.7843137f;
 item.colr[1] = 0.7843137f;
 item.colr[2] = 0.7843137f;
 item.diff = 1.0f;
 item.spec = 0.0f;
 item.tran = 0.0f;
 item.colr_map.chunksize = 0;
 item.diff_map.chunksize = 0;
 item.spec_map.chunksize = 0;
 item.tran_map.chunksize = 0;

 // compute SURF chunksize
 item.chunksize += align02(strlen(name) + 1); // name
 item.chunksize += 0x02; // source
 item.chunksize += 0x04; // COLR
 item.chunksize += 0x02; // COLOR sub-chunksize
 item.chunksize += 0x04; // color
 item.chunksize += 0x04; // color
 item.chunksize += 0x04; // color
 item.chunksize += 0x02; // envelope
 item.chunksize += 0x04; // DIFF
 item.chunksize += 0x02; // DIFF sub-chunksize
 item.chunksize += 0x04; // intensity
 item.chunksize += 0x02; // envelope
 item.chunksize += 0x04; // SPEC
 item.chunksize += 0x02; // SPEC sub-chunksize
 item.chunksize += 0x04; // intensity
 item.chunksize += 0x02; // envelope
 item.chunksize += 0x04; // TRAN
 item.chunksize += 0x02; // TRAN sub-chunksize
 item.chunksize += 0x04; // intensity
 item.chunksize += 0x02; // envelope

 // add SURF to TAGS
 typedef boost::bimap<uint16, string>::value_type tags_item;
 tags.surftags.insert(tags_item(tags.currindex, name));
 tags.chunksize += align02(strlen(name) + 1);
 tags.currindex++;

 // add SURF
 typedef map<string, lwSURF>::value_type value_type;
 surfs.insert(value_type(name, item));

 return true;
}

bool lwFileSaver::setSurfNorm(const char* name, const char* mapname)
{
 // validate
 using namespace std;
 if(!name) return error("setSurfNorm: Invalid surface name.");
 if(!strlen(name)) return error("setSurfNorm: Invalid surface name.");
 if(!strlen(mapname)) return error("setSurfNorm: Invalid vertex normal map name.");

 // find surface
 map<string, lwSURF>::iterator iter = surfs.find(name);
 if(iter == surfs.end()) return error("setSurfNorm: Surface name does not exist.");

 // previously set
 if(iter->second.flags & LWO_SURF_FLAGS_NORM_VMAP) {
    iter->second.chunksize -= iter->second.norm_map.chunksize;
    iter->second.norm_map.name = mapname;
    iter->second.norm_map.chunksize = align02(iter->second.norm_map.name.length() + 1);
    iter->second.chunksize += iter->second.norm_map.chunksize;
   }
 // not previously set
 else {
    iter->second.flags |= LWO_SURF_FLAGS_NORM_VMAP;
    iter->second.norm_map.name = mapname;
    iter->second.norm_map.chunksize = align02(iter->second.norm_map.name.length() + 1);
    iter->second.chunksize += 0x04; // NORM
    iter->second.chunksize += 0x02; // subchunksize
    iter->second.chunksize += iter->second.norm_map.chunksize;
   }

 return true;
}

bool lwFileSaver::setSurfColr(const char* name, real32 r, real32 g, real32 b)
{
 // validate
 using namespace std;
 if(!name) return error("setSurfColr: Invalid surface name.");
 if(!strlen(name)) return error("setSurfColr: Invalid surface name.");

 // find surface
 map<string, lwSURF>::iterator iter = surfs.find(name);
 if(iter == surfs.end()) return error("setSurfColr: Surface name does not exist.");

 // set color
 iter->second.flags |= LWO_SURF_FLAGS_COLR;
 iter->second.colr[0] = r;
 iter->second.colr[1] = g;
 iter->second.colr[2] = b;

 return true;
}

bool lwFileSaver::setSurfDiff(const char* name, real32 intensity)
{
 // validate
 using namespace std;
 if(!name) return error("Invalid surface name.");
 if(!strlen(name)) return error("Invalid surface name.");

 // find surface
 map<string, lwSURF>::iterator iter = surfs.find(name);
 if(iter == surfs.end()) return error("Surface name does not exist.");

 // set color
 iter->second.flags |= LWO_SURF_FLAGS_DIFF;
 iter->second.diff = intensity;

 return true;
}

bool lwFileSaver::setSurfSpec(const char* name, real32 intensity)
{
 // validate
 using namespace std;
 if(!name) return error("Invalid surface name.");
 if(!strlen(name)) return error("Invalid surface name.");

 // find surface
 map<string, lwSURF>::iterator iter = surfs.find(name);
 if(iter == surfs.end()) return error("Surface name does not exist.");

 // set color
 iter->second.flags |= LWO_SURF_FLAGS_SPEC;
 iter->second.spec = intensity;

 return true;
}

bool lwFileSaver::setSurfTran(const char* name, real32 intensity)
{
 // validate
 using namespace std;
 if(!name) return error("Invalid surface name.");
 if(!strlen(name)) return error("Invalid surface name.");

 // find surface
 map<string, lwSURF>::iterator iter = surfs.find(name);
 if(iter == surfs.end()) return error("Surface name does not exist.");

 // set color
 iter->second.flags |= LWO_SURF_FLAGS_TRAN;
 iter->second.tran = intensity;

 return true;
}

bool lwFileSaver::enableSurfColrImag(const char* name)
{
 // validate
 using namespace std;
 if(!name) return error("Invalid surface name.");
 if(!strlen(name)) return error("Invalid surface name.");

 // find surface
 map<string, lwSURF>::iterator iter = surfs.find(name);
 if(iter == surfs.end()) return error("Surface name does not exist.");

 // texture channel already enabled
 if(iter->second.flags & LWO_SURF_FLAGS_COLR_BLOK) return true;

 // set default PROJ
 iter->second.colr_map.proj.chunksize = 0x02;
 iter->second.colr_map.proj.mode = 0x05;

 // set default AXIS
 iter->second.colr_map.axis.chunksize = 0x02;
 iter->second.colr_map.axis.axis = 0x02;

 // set default IMAG
 iter->second.colr_map.imag.chunksize = 0x00;
 iter->second.colr_map.imag.clipindex = 0;

 // set default WRAP
 iter->second.colr_map.wrap.chunksize = 0x04;
 iter->second.colr_map.wrap.wrap_w = 1; // REPEAT
 iter->second.colr_map.wrap.wrap_h = 1; // REPEAT

 // set default VMAP
 iter->second.colr_map.vmap.chunksize = 0x00;
 iter->second.colr_map.vmap.name = "";

 // set default AAST
 iter->second.colr_map.aast.chunksize = 0x06;
 iter->second.colr_map.aast.flags = 0x01;
 iter->second.colr_map.aast.strength = 1.0f;

 // set default PIXB
 iter->second.colr_map.pixb.chunksize = 0x02;
 iter->second.colr_map.pixb.flags = 0x01;

 // set default IMAP
 iter->second.colr_map.imap.chunksize = 0x0;
 iter->second.colr_map.imap.chunksize += 0x02; // ordinal
 iter->second.colr_map.imap.chunksize += 0x0A; // CHAN chunk
 iter->second.colr_map.imap.chunksize += 0x08; // ENAB chunk
 iter->second.colr_map.imap.chunksize += 0x08; // NEGA chunk
 iter->second.colr_map.imap.chunksize += 0x0E; // OPAC chunk
 iter->second.colr_map.imap.chunksize += 0x08; // AXIS chunk

 // set default IMAP:CHAN
 iter->second.colr_map.imap.chan.chunksize = 0x04;
 iter->second.colr_map.imap.chan.channel = LWO_COLR;

 // set default IMAP:ENAB
 iter->second.colr_map.imap.enab.chunksize = 0x02;
 iter->second.colr_map.imap.enab.enable = 0x01;

 // set default IMAP:NEGA
 iter->second.colr_map.imap.nega.chunksize = 0x02;
 iter->second.colr_map.imap.nega.enable = 0x00;

 // set default IMAP:OPAC
 iter->second.colr_map.imap.opac.chunksize = 0x08;
 iter->second.colr_map.imap.opac.type = 0x00; // NORMAL
 iter->second.colr_map.imap.opac.opacity = 1.0f;

 // set default IMAP:AXIS
 iter->second.colr_map.imap.axis.chunksize = 0x02;
 iter->second.colr_map.imap.axis.axis = 0x01; 

 // set defalut TMAP
 iter->second.colr_map.tmap.chunksize = 0x0;
 iter->second.colr_map.tmap.chunksize += 0x14; // CNTR chunk
 iter->second.colr_map.tmap.chunksize += 0x14; // SIZE chunk
 iter->second.colr_map.tmap.chunksize += 0x14; // ROTA chunk
 iter->second.colr_map.tmap.chunksize += 0x16; // FALL chunk
 iter->second.colr_map.tmap.chunksize += 0x08; // CSYS chunk

 // set default TMAP:CNTR
 iter->second.colr_map.tmap.cntr.chunksize = 0x0E;
 iter->second.colr_map.tmap.cntr.x = 0.0f;
 iter->second.colr_map.tmap.cntr.y = 0.0f;
 iter->second.colr_map.tmap.cntr.z = 0.0f;

 // set default TMAP:SIZE
 iter->second.colr_map.tmap.size.chunksize = 0x0E;
 iter->second.colr_map.tmap.size.x = 0.0f;
 iter->second.colr_map.tmap.size.y = 0.0f;
 iter->second.colr_map.tmap.size.z = 0.0f;

 // set default TMAP:ROTA
 iter->second.colr_map.tmap.rota.chunksize = 0x0E;
 iter->second.colr_map.tmap.rota.x = 0.0f;
 iter->second.colr_map.tmap.rota.y = 0.0f;
 iter->second.colr_map.tmap.rota.z = 0.0f;

 // set default TMAP:FALL
 iter->second.colr_map.tmap.fall.chunksize = 0x10;
 iter->second.colr_map.tmap.fall.type = 0;
 iter->second.colr_map.tmap.fall.x = 0.0f;
 iter->second.colr_map.tmap.fall.y = 0.0f;
 iter->second.colr_map.tmap.fall.z = 0.0f;

 // set default TMAP:CSYS
 iter->second.colr_map.tmap.csys.chunksize = 0x02;
 iter->second.colr_map.tmap.csys.type = 0;

 // compute BLOK chunksize
 iter->second.colr_map.chunksize += 0x06 + iter->second.colr_map.imap.chunksize; // IMAP
 iter->second.colr_map.chunksize += 0x06 + iter->second.colr_map.tmap.chunksize; // TMAP
 iter->second.colr_map.chunksize += 0x06 + iter->second.colr_map.proj.chunksize; // PROJ
 iter->second.colr_map.chunksize += 0x06 + iter->second.colr_map.axis.chunksize; // AXIS
 iter->second.colr_map.chunksize += 0x06 + iter->second.colr_map.wrap.chunksize; // WRAP
 iter->second.colr_map.chunksize += 0x06 + iter->second.colr_map.aast.chunksize; // AAST
 iter->second.colr_map.chunksize += 0x06 + iter->second.colr_map.pixb.chunksize; // PIXB

 // enable texture channel
 iter->second.chunksize += 0x06 + iter->second.colr_map.chunksize; // BLOK
 iter->second.flags |= LWO_SURF_FLAGS_COLR_BLOK;

 return true;
}

bool lwFileSaver::enableSurfDiffImag(const char* name)
{
 // validate
 using namespace std;
 if(!name) return error("Invalid surface name.");
 if(!strlen(name)) return error("Invalid surface name.");

 // find surface
 map<string, lwSURF>::iterator iter = surfs.find(name);
 if(iter == surfs.end()) return error("Surface name does not exist.");

 // texture channel already enabled
 if(iter->second.flags & LWO_SURF_FLAGS_DIFF_BLOK) return true;

 // set default PROJ
 iter->second.diff_map.proj.chunksize = 0x02;
 iter->second.diff_map.proj.mode = 0x05;

 // set default AXIS
 iter->second.diff_map.axis.chunksize = 0x02;
 iter->second.diff_map.axis.axis = 0x02;

 // set default IMAG
 iter->second.diff_map.imag.chunksize = 0x00;
 iter->second.diff_map.imag.clipindex = 0;

 // set default WRAP
 iter->second.diff_map.wrap.chunksize = 0x04;
 iter->second.diff_map.wrap.wrap_w = 1; // REPEAT
 iter->second.diff_map.wrap.wrap_h = 1; // REPEAT

 // set default VMAP
 iter->second.diff_map.vmap.chunksize = 0x00;
 iter->second.diff_map.vmap.name = "";

 // set default AAST
 iter->second.diff_map.aast.chunksize = 0x06;
 iter->second.diff_map.aast.flags = 0x01;
 iter->second.diff_map.aast.strength = 1.0f;

 // set default PIXB
 iter->second.diff_map.pixb.chunksize = 0x02;
 iter->second.diff_map.pixb.flags = 0x01;

 // set default IMAP
 iter->second.diff_map.imap.chunksize = 0x0;
 iter->second.diff_map.imap.chunksize += 0x02; // ordinal
 iter->second.diff_map.imap.chunksize += 0x0A; // CHAN chunk
 iter->second.diff_map.imap.chunksize += 0x08; // ENAB chunk
 iter->second.diff_map.imap.chunksize += 0x08; // NEGA chunk
 iter->second.diff_map.imap.chunksize += 0x0E; // OPAC chunk
 iter->second.diff_map.imap.chunksize += 0x08; // AXIS chunk

 // set default IMAP:CHAN
 iter->second.diff_map.imap.chan.chunksize = 0x04;
 iter->second.diff_map.imap.chan.channel = LWO_DIFF;

 // set default IMAP:ENAB
 iter->second.diff_map.imap.enab.chunksize = 0x02;
 iter->second.diff_map.imap.enab.enable = 0x01;

 // set default IMAP:NEGA
 iter->second.diff_map.imap.nega.chunksize = 0x02;
 iter->second.diff_map.imap.nega.enable = 0x00;

 // set default IMAP:OPAC
 iter->second.diff_map.imap.opac.chunksize = 0x08;
 iter->second.diff_map.imap.opac.type = 0x07;
 iter->second.diff_map.imap.opac.opacity = 1.0f;

 // set default IMAP:AXIS
 iter->second.diff_map.imap.axis.chunksize = 0x02;
 iter->second.diff_map.imap.axis.axis = 0x01; 

 // set defalut TMAP
 iter->second.diff_map.tmap.chunksize = 0x0;
 iter->second.diff_map.tmap.chunksize += 0x14; // CNTR chunk
 iter->second.diff_map.tmap.chunksize += 0x14; // SIZE chunk
 iter->second.diff_map.tmap.chunksize += 0x14; // ROTA chunk
 iter->second.diff_map.tmap.chunksize += 0x16; // FALL chunk
 iter->second.diff_map.tmap.chunksize += 0x08; // CSYS chunk

 // set default TMAP:CNTR
 iter->second.diff_map.tmap.cntr.chunksize = 0x0E;
 iter->second.diff_map.tmap.cntr.x = 0.0f;
 iter->second.diff_map.tmap.cntr.y = 0.0f;
 iter->second.diff_map.tmap.cntr.z = 0.0f;

 // set default TMAP:SIZE
 iter->second.diff_map.tmap.size.chunksize = 0x0E;
 iter->second.diff_map.tmap.size.x = 0.0f;
 iter->second.diff_map.tmap.size.y = 0.0f;
 iter->second.diff_map.tmap.size.z = 0.0f;

 // set default TMAP:ROTA
 iter->second.diff_map.tmap.rota.chunksize = 0x0E;
 iter->second.diff_map.tmap.rota.x = 0.0f;
 iter->second.diff_map.tmap.rota.y = 0.0f;
 iter->second.diff_map.tmap.rota.z = 0.0f;

 // set default TMAP:FALL
 iter->second.diff_map.tmap.fall.chunksize = 0x10;
 iter->second.diff_map.tmap.fall.type = 0;
 iter->second.diff_map.tmap.fall.x = 0.0f;
 iter->second.diff_map.tmap.fall.y = 0.0f;
 iter->second.diff_map.tmap.fall.z = 0.0f;

 // set default TMAP:CSYS
 iter->second.diff_map.tmap.csys.chunksize = 0x02;
 iter->second.diff_map.tmap.csys.type = 0;

 // compute BLOK chunksize
 iter->second.diff_map.chunksize += 0x06 + iter->second.diff_map.imap.chunksize; // IMAP
 iter->second.diff_map.chunksize += 0x06 + iter->second.diff_map.tmap.chunksize; // TMAP
 iter->second.diff_map.chunksize += 0x06 + iter->second.diff_map.proj.chunksize; // PROJ
 iter->second.diff_map.chunksize += 0x06 + iter->second.diff_map.axis.chunksize; // AXIS
 iter->second.diff_map.chunksize += 0x06 + iter->second.diff_map.wrap.chunksize; // WRAP
 iter->second.diff_map.chunksize += 0x06 + iter->second.diff_map.aast.chunksize; // AAST
 iter->second.diff_map.chunksize += 0x06 + iter->second.diff_map.pixb.chunksize; // PIXB

 // enable texture channel
 iter->second.chunksize += 0x06 + iter->second.diff_map.chunksize; // BLOK
 iter->second.flags |= LWO_SURF_FLAGS_DIFF_BLOK;

 return true;
}

bool lwFileSaver::enableSurfSpecImag(const char* name)
{
 // validate
 using namespace std;
 if(!name) return error("Invalid surface name.");
 if(!strlen(name)) return error("Invalid surface name.");

 // find surface
 map<string, lwSURF>::iterator iter = surfs.find(name);
 if(iter == surfs.end()) return error("Surface name does not exist.");

 // texture channel already enabled
 if(iter->second.flags & LWO_SURF_FLAGS_SPEC_BLOK) return true;

 // set default PROJ
 iter->second.spec_map.proj.chunksize = 0x02;
 iter->second.spec_map.proj.mode = 0x05;

 // set default AXIS
 iter->second.spec_map.axis.chunksize = 0x02;
 iter->second.spec_map.axis.axis = 0x02;

 // set default IMAG
 iter->second.spec_map.imag.chunksize = 0x00;
 iter->second.spec_map.imag.clipindex = 0;

 // set default WRAP
 iter->second.spec_map.wrap.chunksize = 0x04;
 iter->second.spec_map.wrap.wrap_w = 1; // REPEAT
 iter->second.spec_map.wrap.wrap_h = 1; // REPEAT

 // set default VMAP
 iter->second.spec_map.vmap.chunksize = 0x00;
 iter->second.spec_map.vmap.name = "";

 // set default AAST
 iter->second.spec_map.aast.chunksize = 0x06;
 iter->second.spec_map.aast.flags = 0x01;
 iter->second.spec_map.aast.strength = 1.0f;

 // set default PIXB
 iter->second.spec_map.pixb.chunksize = 0x02;
 iter->second.spec_map.pixb.flags = 0x01;

 // set default IMAP
 iter->second.spec_map.imap.chunksize = 0x0;
 iter->second.spec_map.imap.chunksize += 0x02; // ordinal
 iter->second.spec_map.imap.chunksize += 0x0A; // CHAN chunk
 iter->second.spec_map.imap.chunksize += 0x08; // ENAB chunk
 iter->second.spec_map.imap.chunksize += 0x08; // NEGA chunk
 iter->second.spec_map.imap.chunksize += 0x0E; // OPAC chunk
 iter->second.spec_map.imap.chunksize += 0x08; // AXIS chunk

 // set default IMAP:CHAN
 iter->second.spec_map.imap.chan.chunksize = 0x04;
 iter->second.spec_map.imap.chan.channel = LWO_SPEC;

 // set default IMAP:ENAB
 iter->second.spec_map.imap.enab.chunksize = 0x02;
 iter->second.spec_map.imap.enab.enable = 0x01;

 // set default IMAP:NEGA
 iter->second.spec_map.imap.nega.chunksize = 0x02;
 iter->second.spec_map.imap.nega.enable = 0x00;

 // set default IMAP:OPAC
 iter->second.spec_map.imap.opac.chunksize = 0x08;
 iter->second.spec_map.imap.opac.type = 0x07;
 iter->second.spec_map.imap.opac.opacity = 1.0f;

 // set default IMAP:AXIS
 iter->second.spec_map.imap.axis.chunksize = 0x02;
 iter->second.spec_map.imap.axis.axis = 0x01; 

 // set defalut TMAP
 iter->second.spec_map.tmap.chunksize = 0x0;
 iter->second.spec_map.tmap.chunksize += 0x14; // CNTR chunk
 iter->second.spec_map.tmap.chunksize += 0x14; // SIZE chunk
 iter->second.spec_map.tmap.chunksize += 0x14; // ROTA chunk
 iter->second.spec_map.tmap.chunksize += 0x16; // FALL chunk
 iter->second.spec_map.tmap.chunksize += 0x08; // CSYS chunk

 // set default TMAP:CNTR
 iter->second.spec_map.tmap.cntr.chunksize = 0x0E;
 iter->second.spec_map.tmap.cntr.x = 0.0f;
 iter->second.spec_map.tmap.cntr.y = 0.0f;
 iter->second.spec_map.tmap.cntr.z = 0.0f;

 // set default TMAP:SIZE
 iter->second.spec_map.tmap.size.chunksize = 0x0E;
 iter->second.spec_map.tmap.size.x = 0.0f;
 iter->second.spec_map.tmap.size.y = 0.0f;
 iter->second.spec_map.tmap.size.z = 0.0f;

 // set default TMAP:ROTA
 iter->second.spec_map.tmap.rota.chunksize = 0x0E;
 iter->second.spec_map.tmap.rota.x = 0.0f;
 iter->second.spec_map.tmap.rota.y = 0.0f;
 iter->second.spec_map.tmap.rota.z = 0.0f;

 // set default TMAP:FALL
 iter->second.spec_map.tmap.fall.chunksize = 0x10;
 iter->second.spec_map.tmap.fall.type = 0;
 iter->second.spec_map.tmap.fall.x = 0.0f;
 iter->second.spec_map.tmap.fall.y = 0.0f;
 iter->second.spec_map.tmap.fall.z = 0.0f;

 // set default TMAP:CSYS
 iter->second.spec_map.tmap.csys.chunksize = 0x02;
 iter->second.spec_map.tmap.csys.type = 0;

 // compute BLOK chunksize
 iter->second.spec_map.chunksize += 0x06 + iter->second.spec_map.imap.chunksize; // IMAP
 iter->second.spec_map.chunksize += 0x06 + iter->second.spec_map.tmap.chunksize; // TMAP
 iter->second.spec_map.chunksize += 0x06 + iter->second.spec_map.proj.chunksize; // PROJ
 iter->second.spec_map.chunksize += 0x06 + iter->second.spec_map.axis.chunksize; // AXIS
 iter->second.spec_map.chunksize += 0x06 + iter->second.spec_map.wrap.chunksize; // WRAP
 iter->second.spec_map.chunksize += 0x06 + iter->second.spec_map.aast.chunksize; // AAST
 iter->second.spec_map.chunksize += 0x06 + iter->second.spec_map.pixb.chunksize; // PIXB

 // enable texture channel
 iter->second.chunksize += 0x06 + iter->second.spec_map.chunksize; // BLOK
 iter->second.flags |= LWO_SURF_FLAGS_SPEC_BLOK;

 return true;
}

bool lwFileSaver::enableSurfTranImag(const char* name)
{
 // validate
 using namespace std;
 if(!name) return error("Invalid surface name.");
 if(!strlen(name)) return error("Invalid surface name.");

 // find surface
 map<string, lwSURF>::iterator iter = surfs.find(name);
 if(iter == surfs.end()) return error("Surface name does not exist.");

 // texture channel already enabled
 if(iter->second.flags & LWO_SURF_FLAGS_TRAN_BLOK) return true;

 // set default PROJ
 iter->second.tran_map.proj.chunksize = 0x02;
 iter->second.tran_map.proj.mode = 0x05;

 // set default AXIS
 iter->second.tran_map.axis.chunksize = 0x02;
 iter->second.tran_map.axis.axis = 0x02;

 // set default IMAG
 iter->second.tran_map.imag.chunksize = 0x00;
 iter->second.tran_map.imag.clipindex = 0;

 // set default WRAP
 iter->second.tran_map.wrap.chunksize = 0x04;
 iter->second.tran_map.wrap.wrap_w = 1; // REPEAT
 iter->second.tran_map.wrap.wrap_h = 1; // REPEAT

 // set default VMAP
 iter->second.tran_map.vmap.chunksize = 0x00;
 iter->second.tran_map.vmap.name = "";

 // set default AAST
 iter->second.tran_map.aast.chunksize = 0x06;
 iter->second.tran_map.aast.flags = 0x01;
 iter->second.tran_map.aast.strength = 1.0f;

 // set default PIXB
 iter->second.tran_map.pixb.chunksize = 0x02;
 iter->second.tran_map.pixb.flags = 0x01;

 // set default IMAP
 iter->second.tran_map.imap.chunksize = 0x0;
 iter->second.tran_map.imap.chunksize += 0x02; // ordinal
 iter->second.tran_map.imap.chunksize += 0x0A; // CHAN chunk
 iter->second.tran_map.imap.chunksize += 0x08; // ENAB chunk
 iter->second.tran_map.imap.chunksize += 0x08; // NEGA chunk
 iter->second.tran_map.imap.chunksize += 0x0E; // OPAC chunk
 iter->second.tran_map.imap.chunksize += 0x08; // AXIS chunk

 // set default IMAP:CHAN
 iter->second.tran_map.imap.chan.chunksize = 0x04;
 iter->second.tran_map.imap.chan.channel = LWO_TRAN;

 // set default IMAP:ENAB
 iter->second.tran_map.imap.enab.chunksize = 0x02;
 iter->second.tran_map.imap.enab.enable = 0x01;

 // set default IMAP:NEGA
 iter->second.tran_map.imap.nega.chunksize = 0x02;
 iter->second.tran_map.imap.nega.enable = 0x00;

 // set default IMAP:OPAC
 iter->second.tran_map.imap.opac.chunksize = 0x08;
 iter->second.tran_map.imap.opac.type = 0x07;
 iter->second.tran_map.imap.opac.opacity = 1.0f;

 // set default IMAP:AXIS
 iter->second.tran_map.imap.axis.chunksize = 0x02;
 iter->second.tran_map.imap.axis.axis = 0x01; 

 // set defalut TMAP
 iter->second.tran_map.tmap.chunksize = 0x0;
 iter->second.tran_map.tmap.chunksize += 0x14; // CNTR chunk
 iter->second.tran_map.tmap.chunksize += 0x14; // SIZE chunk
 iter->second.tran_map.tmap.chunksize += 0x14; // ROTA chunk
 iter->second.tran_map.tmap.chunksize += 0x16; // FALL chunk
 iter->second.tran_map.tmap.chunksize += 0x08; // CSYS chunk

 // set default TMAP:CNTR
 iter->second.tran_map.tmap.cntr.chunksize = 0x0E;
 iter->second.tran_map.tmap.cntr.x = 0.0f;
 iter->second.tran_map.tmap.cntr.y = 0.0f;
 iter->second.tran_map.tmap.cntr.z = 0.0f;

 // set default TMAP:SIZE
 iter->second.tran_map.tmap.size.chunksize = 0x0E;
 iter->second.tran_map.tmap.size.x = 0.0f;
 iter->second.tran_map.tmap.size.y = 0.0f;
 iter->second.tran_map.tmap.size.z = 0.0f;

 // set default TMAP:ROTA
 iter->second.tran_map.tmap.rota.chunksize = 0x0E;
 iter->second.tran_map.tmap.rota.x = 0.0f;
 iter->second.tran_map.tmap.rota.y = 0.0f;
 iter->second.tran_map.tmap.rota.z = 0.0f;

 // set default TMAP:FALL
 iter->second.tran_map.tmap.fall.chunksize = 0x10;
 iter->second.tran_map.tmap.fall.type = 0;
 iter->second.tran_map.tmap.fall.x = 0.0f;
 iter->second.tran_map.tmap.fall.y = 0.0f;
 iter->second.tran_map.tmap.fall.z = 0.0f;

 // set default TMAP:CSYS
 iter->second.tran_map.tmap.csys.chunksize = 0x02;
 iter->second.tran_map.tmap.csys.type = 0;

 // compute BLOK chunksize
 iter->second.tran_map.chunksize += 0x06 + iter->second.tran_map.imap.chunksize; // IMAP
 iter->second.tran_map.chunksize += 0x06 + iter->second.tran_map.tmap.chunksize; // TMAP
 iter->second.tran_map.chunksize += 0x06 + iter->second.tran_map.proj.chunksize; // PROJ
 iter->second.tran_map.chunksize += 0x06 + iter->second.tran_map.axis.chunksize; // AXIS
 iter->second.tran_map.chunksize += 0x06 + iter->second.tran_map.wrap.chunksize; // WRAP
 iter->second.tran_map.chunksize += 0x06 + iter->second.tran_map.aast.chunksize; // AAST
 iter->second.tran_map.chunksize += 0x06 + iter->second.tran_map.pixb.chunksize; // PIXB

 // enable texture channel
 iter->second.chunksize += 0x06 + iter->second.tran_map.chunksize; // BLOK
 iter->second.flags |= LWO_SURF_FLAGS_TRAN_BLOK;

 return true;
}

bool lwFileSaver::setSurfVmap(const char* surface, const char* mapname, uint32 type)
{
 // validate
 using namespace std;
 if(!surface) return error("Invalid surface name.");
 if(!strlen(surface)) return error("Invalid surface name.");

 // find surface
 map<string, lwSURF>::iterator iter = surfs.find(surface);
 if(iter == surfs.end()) return error("Surface name does not exist.");

 // texture channel not enabled
 lwSURF* surf = &(iter->second);
 if(!(surf->flags & LWO_SURF_FLAGS_COLR_BLOK)) return error("Texture channel not enabled.");

 // find UV map name
 if(!hasTxuv(mapname)) return error("UV map name not found.");

 // set block
 lwBLOK* blok = nullptr;
 if(type & LWO_SURF_FLAGS_COLR_BLOK) blok = &(iter->second.colr_map);
 else if(type & LWO_SURF_FLAGS_DIFF_BLOK) blok = &(iter->second.diff_map);
 else if(type & LWO_SURF_FLAGS_SPEC_BLOK) blok = &(iter->second.spec_map);
 else if(type & LWO_SURF_FLAGS_TRAN_BLOK) blok = &(iter->second.tran_map);

 // adjust chunksizes
 uint32 prev_vmap_chunksize = blok->vmap.chunksize;
 if(prev_vmap_chunksize) {
    blok->chunksize -= prev_vmap_chunksize;
    surf->chunksize -= prev_vmap_chunksize;
   }
 else {
    blok->chunksize += 0x06;
    surf->chunksize += 0x06;
   }

 // set VMAP data
 blok->vmap.name = mapname;
 blok->vmap.chunksize = align02(blok->vmap.name.length() + 1); // BUG FIXED! WAS +=

 // adjust chunksizes
 blok->chunksize += blok->vmap.chunksize;
 surf->chunksize += blok->vmap.chunksize;

 return true;
}

bool lwFileSaver::setSurfColrVmap(const char* surface, const char* mapname)
{
 return setSurfVmap(surface, mapname, LWO_SURF_FLAGS_COLR_BLOK);
}

bool lwFileSaver::setSurfDiffVmap(const char* surface, const char* mapname)
{
 return setSurfVmap(surface, mapname, LWO_SURF_FLAGS_DIFF_BLOK);
}

bool lwFileSaver::setSurfSpecVmap(const char* surface, const char* mapname)
{
 return setSurfVmap(surface, mapname, LWO_SURF_FLAGS_SPEC_BLOK);
}

bool lwFileSaver::setSurfTranVmap(const char* surface, const char* mapname)
{
 return setSurfVmap(surface, mapname, LWO_SURF_FLAGS_TRAN_BLOK);
}

bool lwFileSaver::setSurfImag(const char* surface, uint32 clipindex, uint32 type)
{
 // validate
 using namespace std;
 if(!surface) return error("Invalid surface name.");
 if(!strlen(surface)) return error("Invalid surface name.");
 if(!clipindex) return error("Invalid clip index.");

 // find surface
 map<string, lwSURF>::iterator iter = surfs.find(surface);
 if(iter == surfs.end()) return error("Surface name does not exist.");

 // texture channel not enabled
 lwSURF* surf = &(iter->second);
 if(!(surf->flags & type)) return error("Texture channel not enabled.");

 // find clip index
 map<uint32, lwCLIP>::iterator clipiter = clips.find(clipindex);
 if(clipiter == clips.end()) {
    stringstream ss;
    ss << "Clip index 0x" << hex << clipindex << dec << " not found.";
    return error(ss.str().c_str());
   }

 // set block
 lwBLOK* blok = nullptr;
 if(type & LWO_SURF_FLAGS_COLR_BLOK) blok = &(iter->second.colr_map);
 else if(type & LWO_SURF_FLAGS_DIFF_BLOK) blok = &(iter->second.diff_map);
 else if(type & LWO_SURF_FLAGS_SPEC_BLOK) blok = &(iter->second.spec_map);
 else if(type & LWO_SURF_FLAGS_TRAN_BLOK) blok = &(iter->second.tran_map);

 // adjust chunksizes
 uint32 prev_imag_chunksize = blok->imag.chunksize;
 if(prev_imag_chunksize) {
    blok->chunksize -= prev_imag_chunksize;
    surf->chunksize -= prev_imag_chunksize;
   }
 else {
    blok->chunksize += 0x06;
    surf->chunksize += 0x06;
   }

 // set IMAG data
 blok->imag.clipindex = clipindex;
 blok->imag.chunksize = GetVXBytes(clipindex); // FIXED! WAS +=

 // adjust chunksizes
 blok->chunksize += blok->imag.chunksize;
 surf->chunksize += blok->imag.chunksize;

 return true;
}

bool lwFileSaver::setSurfColrImag(const char* surface, uint32 clipindex)
{
 return setSurfImag(surface, clipindex, LWO_SURF_FLAGS_COLR_BLOK);
}

bool lwFileSaver::setSurfDiffImag(const char* surface, uint32 clipindex)
{
 return setSurfImag(surface, clipindex, LWO_SURF_FLAGS_DIFF_BLOK);
}

bool lwFileSaver::setSurfSpecImag(const char* surface, uint32 clipindex)
{
 return setSurfImag(surface, clipindex, LWO_SURF_FLAGS_SPEC_BLOK);
}

bool lwFileSaver::setSurfTranImag(const char* surface, uint32 clipindex)
{
 return setSurfImag(surface, clipindex, LWO_SURF_FLAGS_TRAN_BLOK);
}

bool lwFileSaver::setSurfNega(const char* surface, uint16 state, uint32 type)
{
 // validate
 using namespace std;
 if(!surface) return error("Invalid surface name.");
 if(!strlen(surface)) return error("Invalid surface name.");

 // find surface
 map<string, lwSURF>::iterator iter = surfs.find(surface);
 if(iter == surfs.end()) return error("Surface name does not exist.");

 // texture channel not enabled
 lwSURF* surf = &(iter->second);
 if(!(surf->flags & type)) return error("Texture channel not enabled.");

 // set block
 lwBLOK* blok = nullptr;
 if(type & LWO_SURF_FLAGS_COLR_BLOK) blok = &(iter->second.colr_map);
 else if(type & LWO_SURF_FLAGS_DIFF_BLOK) blok = &(iter->second.diff_map);
 else if(type & LWO_SURF_FLAGS_SPEC_BLOK) blok = &(iter->second.spec_map);
 else if(type & LWO_SURF_FLAGS_TRAN_BLOK) blok = &(iter->second.tran_map);

 // set NEGA data
 blok->imap.nega.enable = state;
 return true;
}

bool lwFileSaver::setSurfColrNega(const char* surface, uint16 state)
{
 return setSurfNega(surface, state, LWO_SURF_FLAGS_COLR_BLOK);
}

bool lwFileSaver::setSurfDiffNega(const char* surface, uint16 state)
{
 return setSurfNega(surface, state, LWO_SURF_FLAGS_DIFF_BLOK);
}

bool lwFileSaver::setSurfSpecNega(const char* surface, uint16 state)
{
 return setSurfNega(surface, state, LWO_SURF_FLAGS_SPEC_BLOK);
}

bool lwFileSaver::setSurfTranNega(const char* surface, uint16 state)
{
 return setSurfNega(surface, state, LWO_SURF_FLAGS_TRAN_BLOK);
}

bool lwFileSaver::save(const char* filename)const
{
 // create file
 using namespace std;
 ofstream ofile(filename, ios::binary);
 if(!ofile) return error("Failed to create lwo file.");

 //
 // COMPUTE FORM CHUNKSIZE
 //

 // keep track of FORM chunksize
 uint32 FORM_chunksize = 0;
 FORM_chunksize += 0x04; // LWO2

 // TAGS contribution
 if(tags.chunksize) {
     FORM_chunksize += 0x08;
     FORM_chunksize += tags.chunksize;
   }

 // CLIP contribution
 for(map<uint32, lwCLIP>::const_iterator i = clips.begin(); i != clips.end(); i++) {
     FORM_chunksize += 0x08;
     FORM_chunksize += i->second.chunksize;
    }

 // LAYR contribution
 for(map<uint16, lwLAYR>::const_iterator i = layrs.begin(); i != layrs.end(); i++)
    {
     FORM_chunksize += 0x08;
     FORM_chunksize += i->second.chunksize;

     // PNTS contribution
     for(map<uint16, lwPNTS>::const_iterator j = i->second.pntss.begin(); j != i->second.pntss.end(); j++)
        {
         FORM_chunksize += 0x08;
         FORM_chunksize += j->second.chunksize;
         // TXUV contribution
         for(map<string, lwTXUV>::const_iterator k = j->second.txuv.begin(); k != j->second.txuv.end(); k++) {
             FORM_chunksize += 0x08;
             FORM_chunksize += k->second.chunksize;
            }
         // WGHT contribution
         for(map<string, lwWGHT>::const_iterator k = j->second.wght.begin(); k != j->second.wght.end(); k++) {
             FORM_chunksize += 0x08;
             FORM_chunksize += k->second.chunksize;
            }
         // MORF contribution
         for(map<string, lwMORF>::const_iterator k = j->second.morf.begin(); k != j->second.morf.end(); k++) {
             FORM_chunksize += 0x08;
             FORM_chunksize += k->second.chunksize;
            }
         // NORM contribution
         for(map<string, lwNORM>::const_iterator k = j->second.norm.begin(); k != j->second.norm.end(); k++) {
             FORM_chunksize += 0x08;
             FORM_chunksize += k->second.chunksize;
            }
         // POLS contribution
         for(size_t faceindex = 0; faceindex < j->second.face.size(); faceindex++)
            {
             // POLS[faceindex]:FACE contribution
             if(j->second.face[faceindex].chunksize) {
                FORM_chunksize += 0x08;
                FORM_chunksize += j->second.face[faceindex].chunksize;
               }
             // PTAG[faceindex]:SURF contribution
             if(j->second.face[faceindex].surf.chunksize) {
                FORM_chunksize += 0x08;
                FORM_chunksize += j->second.face[faceindex].surf.chunksize;
               }
            }
        }

     // JNTS contribution
     for(map<uint16, lwJNTS>::const_iterator j = i->second.jntss.begin(); j != i->second.jntss.end(); j++)
        {
         // JNTS contribution
         FORM_chunksize += 0x08;
         FORM_chunksize += j->second.chunksize;

         // SKEL contribution
         for(size_t skelindex = 0; skelindex < j->second.skel.size(); skelindex++)
            {
             // POLS[skelindex]:BONE contribution
             if(j->second.skel[skelindex].chunksize) {
                FORM_chunksize += 0x08;
                FORM_chunksize += j->second.skel[skelindex].chunksize;
               }
             // PTAG[skelindex]:SURF contribution
             if(j->second.skel[skelindex].stag.chunksize) {
                FORM_chunksize += 0x08;
                FORM_chunksize += j->second.skel[skelindex].stag.chunksize;
               }
             // PTAG[skelindex]:BONE contribution
             if(j->second.skel[skelindex].btag.chunksize) {
                FORM_chunksize += 0x08;
                FORM_chunksize += j->second.skel[skelindex].btag.chunksize;
               }
             // PTAG[skelindex]:PART contribution
             if(j->second.skel[skelindex].ptag.chunksize) {
                FORM_chunksize += 0x08;
                FORM_chunksize += j->second.skel[skelindex].ptag.chunksize;
               }
             // PTAG[skelindex]:BNUP contribution
             if(j->second.skel[skelindex].utag.chunksize) {
                FORM_chunksize += 0x08;
                FORM_chunksize += j->second.skel[skelindex].utag.chunksize;
               }
            }
        }
    }

 // SURF contribution
 for(map<string, lwSURF>::const_iterator i = surfs.begin(); i != surfs.end(); i++) {
     FORM_chunksize += 0x08;
     FORM_chunksize += i->second.chunksize;
    }

 //
 // SAVE DATA
 //

 // save FORM + FORM_chunksize + LWO2
 BE_write_uint32(ofile, LWO_FORM);
 BE_write_uint32(ofile, FORM_chunksize);
 BE_write_uint32(ofile, LWO_LWO2);

 // save TAGS data
 uint32 n_tags = tags.currindex;
 if(tags.chunksize && n_tags)
   {
    // construct TAGS data
    typedef boost::bimap<uint16, string>::const_iterator bmiterator;
    vector<string> tagdata(n_tags);
    for(bmiterator i = tags.surftags.begin(); i != tags.surftags.end(); i++) tagdata[i->left] = i->right;
    for(bmiterator i = tags.bonetags.begin(); i != tags.bonetags.end(); i++) tagdata[i->left] = i->right;
    for(bmiterator i = tags.parttags.begin(); i != tags.parttags.end(); i++) tagdata[i->left] = i->right;
    for(bmiterator i = tags.bnuptags.begin(); i != tags.bnuptags.end(); i++) tagdata[i->left] = i->right;
   
    // save TAGS data
    BE_write_uint32(ofile, LWO_TAGS);
    BE_write_uint32(ofile, tags.chunksize);
    for(uint32 i = 0; i < n_tags; i++) write_aligned_string_02(ofile, tagdata[i].c_str());
   }

 // save CLIP data
 for(map<uint32, lwCLIP>::const_iterator i = clips.begin(); i != clips.end(); i++)
    {
     // CLIP
     BE_write_uint32(ofile, LWO_CLIP);
     BE_write_uint32(ofile, i->second.chunksize);
     BE_write_uint32(ofile, i->first);

     // STIL
     BE_write_uint32(ofile, LWO_STIL);
     BE_write_uint16(ofile, (uint16)align02(i->second.filename.length() + 1));
     write_aligned_string_02(ofile, i->second.filename.c_str());

     // FLAGS subchunk
     BE_write_uint32(ofile, LWO_FLAG);
     BE_write_uint16(ofile, 0x0004);
     BE_write_uint16(ofile, 0x0800);
     BE_write_uint16(ofile, 0x0080);
    }

 // save LAYR data
 for(map<uint16, lwLAYR>::const_iterator i = layrs.begin(); i != layrs.end(); i++)
    {
     // save LAYR chunk
     BE_write_uint32(ofile, LWO_LAYR); // LAYR
     BE_write_uint32(ofile, i->second.chunksize); // chunksize
     BE_write_uint16(ofile, i->first); // number
     BE_write_uint16(ofile, i->second.flags); // flags
     BE_write_real32(ofile, i->second.pivot_x); // pivot
     BE_write_real32(ofile, i->second.pivot_y); // pivot
     BE_write_real32(ofile, i->second.pivot_z); // pivot
     if(i->second.name.length()) write_aligned_string_02(ofile, i->second.name.c_str()); // name
     else BE_write_uint16(ofile, 0); // name

     // save PNTS
     const map<uint16, lwPNTS>& pmap = i->second.pntss;
     for(map<uint16, lwPNTS>::const_iterator j = pmap.begin(); j != pmap.end(); j++)
        {
         // save PNTS chunk
         BE_write_uint32(ofile, LWO_PNTS); // PNTS
         BE_write_uint32(ofile, j->second.chunksize); // chunksize
         for(uint32 index = 0; index < j->second.elem; index++) { // data
             BE_write_real32(ofile, j->second.data[index].x); // x
             BE_write_real32(ofile, j->second.data[index].y); // y
             BE_write_real32(ofile, j->second.data[index].z); // z
            }

         // save TXUV data
         for(map<string, lwTXUV>::const_iterator k = j->second.txuv.begin(); k != j->second.txuv.end(); k++)
            {
             // save VMAP chunk
             BE_write_uint32(ofile, LWO_VMAP); // VMAP
             BE_write_uint32(ofile, k->second.chunksize); // chunksize
             BE_write_uint32(ofile, LWO_TXUV); // TXUV
             BE_write_uint16(ofile, 2); // dimension
             write_aligned_string_02(ofile, k->first.c_str()); // name
             for(uint32 index = 0; index < k->second.elem; index++) {
                 WriteVX(ofile, k->second.data[index].reference); // reference
                 BE_write_real32(ofile, k->second.data[index].x); // x
                 BE_write_real32(ofile, 1.0f - k->second.data[index].y); // y
                }
            }

         // save WGHT data
         for(map<string, lwWGHT>::const_iterator k = j->second.wght.begin(); k != j->second.wght.end(); k++)
            {
             // save VMAP chunk
             BE_write_uint32(ofile, LWO_VMAP); // VMAP
             BE_write_uint32(ofile, k->second.chunksize); // chunksize
             BE_write_uint32(ofile, LWO_WGHT); // WGHT
             BE_write_uint16(ofile, 1); // dimension
             write_aligned_string_02(ofile, k->first.c_str()); // name
             for(uint32 index = 0; index < k->second.elem; index++) {
                 WriteVX(ofile, k->second.data[index].reference); // reference
                 BE_write_real32(ofile, k->second.data[index].x); // x
                }
            }

         // save MORF data
         for(map<string, lwMORF>::const_iterator k = j->second.morf.begin(); k != j->second.morf.end(); k++)
            {
             // save VMAP chunk
             BE_write_uint32(ofile, LWO_VMAP); // VMAP
             BE_write_uint32(ofile, k->second.chunksize); // chunksize
             BE_write_uint32(ofile, LWO_MORF); // MORF
             BE_write_uint16(ofile, 3); // dimension
             write_aligned_string_02(ofile, k->first.c_str()); // name
             for(uint32 index = 0; index < k->second.elem; index++) {
                 WriteVX(ofile, k->second.data[index].reference); // reference
                 BE_write_real32(ofile, k->second.data[index].x); // x
                 BE_write_real32(ofile, k->second.data[index].y); // y
                 BE_write_real32(ofile, k->second.data[index].z); // z
                }
            }

         // save NORM data
         for(map<string, lwNORM>::const_iterator k = j->second.norm.begin(); k != j->second.norm.end(); k++)
            {
             // save VMAP chunk
             BE_write_uint32(ofile, LWO_VMAP); // VMAP
             BE_write_uint32(ofile, k->second.chunksize); // chunksize
             BE_write_uint32(ofile, LWO_NORM); // NORM
             BE_write_uint16(ofile, 3); // dimension
             write_aligned_string_02(ofile, k->first.c_str()); // name
             for(uint32 index = 0; index < k->second.elem; index++) {
                 WriteVX(ofile, k->second.data[index].reference); // reference
                 BE_write_real32(ofile, k->second.data[index].x); // x
                 BE_write_real32(ofile, k->second.data[index].y); // y
                 BE_write_real32(ofile, k->second.data[index].z); // z
                }
            }

         // save POLS data
         for(size_t faceindex = 0; faceindex < j->second.face.size(); faceindex++)
            {
             // save POLS data
             const lwPOLS& pols = j->second.face[faceindex];
             if(pols.chunksize) {
                BE_write_uint32(ofile, LWO_POLS);
                BE_write_uint32(ofile, pols.chunksize);
                BE_write_uint32(ofile, LWO_FACE);
                for(uint32 index = 0; index < pols.elem; index++) {
                    BE_write_uint16(ofile, 3);
                    WriteVX(ofile, pols.data[index].a);
                    WriteVX(ofile, pols.data[index].b);
                    WriteVX(ofile, pols.data[index].c);
                   }
               }

             // save PTAG:SURF data
             const lwPTAG& ptag = pols.surf;
             if(ptag.chunksize) {
                BE_write_uint32(ofile, LWO_PTAG);
                BE_write_uint32(ofile, ptag.chunksize);
                BE_write_uint32(ofile, ptag.type);
                for(map<uint32, uint16>::const_iterator k = ptag.tags.begin(); k != ptag.tags.end(); k++) {
                    WriteVX(ofile, k->first);
                    BE_write_uint16(ofile, k->second);
                   }
               }
            }
        }

     // save JNTS
     const map<uint16, lwJNTS>& jmap = i->second.jntss;
     for(map<uint16, lwJNTS>::const_iterator j = jmap.begin(); j != jmap.end(); j++)
        {
         // save PNTS chunk
         BE_write_uint32(ofile, LWO_PNTS); // PNTS
         BE_write_uint32(ofile, j->second.chunksize); // chunksize
         for(uint32 index = 0; index < j->second.elem; index++) { // data
             BE_write_real32(ofile, j->second.data[index].x); // x
             BE_write_real32(ofile, j->second.data[index].y); // y
             BE_write_real32(ofile, j->second.data[index].z); // z
            }

         // save SKEL data
         for(size_t skelindex = 0; skelindex < j->second.skel.size(); skelindex++)
            {
             // save SKEL data
             const lwSKEL& skel = j->second.skel[skelindex];
             if(skel.chunksize) {
                BE_write_uint32(ofile, LWO_POLS); // POLS
                BE_write_uint32(ofile, skel.chunksize); // chunksize
                BE_write_uint32(ofile, LWO_BONE); // BONE
                for(uint32 index = 0; index < skel.elem; index++) { // data
                    BE_write_uint16(ofile, 2); // vertices
                    WriteVX(ofile, skel.data[index].a); // a
                    WriteVX(ofile, skel.data[index].b); // b
                   }
               }

             // save PTAG:SURF data
             const lwPTAG& stag = skel.stag;
             if(stag.chunksize) {
                BE_write_uint32(ofile, LWO_PTAG);
                BE_write_uint32(ofile, stag.chunksize);
                BE_write_uint32(ofile, stag.type);
                for(map<uint32, uint16>::const_iterator k = stag.tags.begin(); k != stag.tags.end(); k++) {
                    WriteVX(ofile, k->first);
                    BE_write_uint16(ofile, k->second);
                   }
               }

             // save PTAG:BONE data
             const lwPTAG& btag = skel.btag;
             if(btag.chunksize) {
                BE_write_uint32(ofile, LWO_PTAG);
                BE_write_uint32(ofile, btag.chunksize);
                BE_write_uint32(ofile, btag.type);
                for(map<uint32, uint16>::const_iterator k = btag.tags.begin(); k != btag.tags.end(); k++) {
                    WriteVX(ofile, k->first);
                    BE_write_uint16(ofile, k->second);
                   }
               }

             // save PTAG:PART data
             const lwPTAG& ptag = skel.ptag;
             if(ptag.chunksize) {
                BE_write_uint32(ofile, LWO_PTAG);
                BE_write_uint32(ofile, ptag.chunksize);
                BE_write_uint32(ofile, ptag.type);
                for(map<uint32, uint16>::const_iterator k = ptag.tags.begin(); k != ptag.tags.end(); k++) {
                    WriteVX(ofile, k->first);
                    BE_write_uint16(ofile, k->second);
                   }
               }

             // save PTAG:BNUP data
             const lwPTAG& utag = skel.utag;
             if(utag.chunksize)
               {
               }
            }
        }
    }

 // save SURF data
 for(map<string, lwSURF>::const_iterator i = surfs.begin(); i != surfs.end(); i++)
    {
     // part 1
     BE_write_uint32(ofile, LWO_SURF); // SURF
     BE_write_uint32(ofile, i->second.chunksize); // chunksize
     write_aligned_string_02(ofile, i->first.c_str()); // name
     BE_write_uint16(ofile, 0); // source

     // part 2
     if(i->second.flags & LWO_SURF_FLAGS_COLR) {
        BE_write_uint32(ofile, LWO_COLR);
        BE_write_uint16(ofile, 0x0E);
        BE_write_real32(ofile, i->second.colr[0]);
        BE_write_real32(ofile, i->second.colr[1]);
        BE_write_real32(ofile, i->second.colr[2]);
        BE_write_uint16(ofile, 0x00);
       }
     if(i->second.flags & LWO_SURF_FLAGS_DIFF) {
        BE_write_uint32(ofile, LWO_DIFF);
        BE_write_uint16(ofile, 0x06);
        BE_write_real32(ofile, i->second.diff);
        BE_write_uint16(ofile, 0x00);
       }
     if(i->second.flags & LWO_SURF_FLAGS_SPEC) {
        BE_write_uint32(ofile, LWO_SPEC);
        BE_write_uint16(ofile, 0x06);
        BE_write_real32(ofile, i->second.spec);
        BE_write_uint16(ofile, 0x00);
       }
     if(i->second.flags & LWO_SURF_FLAGS_TRAN) {
        BE_write_uint32(ofile, LWO_TRAN);
        BE_write_uint16(ofile, 0x06);
        BE_write_real32(ofile, i->second.tran);
        BE_write_uint16(ofile, 0x00);
       }

     // part 3
     if(i->second.flags & LWO_SURF_FLAGS_NORM_VMAP) {
        BE_write_uint32(ofile, LWO_NORM);
        BE_write_uint16(ofile, i->second.norm_map.chunksize);
        write_aligned_string_02(ofile, i->second.norm_map.name.c_str());
       }

     // part 4
     if(i->second.flags & LWO_SURF_FLAGS_COLR_BLOK)
       {
        // BLOK
        BE_write_uint32(ofile, LWO_BLOK);
        BE_write_uint16(ofile, i->second.colr_map.chunksize);

        // BLOK:IMAP
        BE_write_uint32(ofile, LWO_IMAP);
        BE_write_uint16(ofile, i->second.colr_map.imap.chunksize);
        BE_write_uint16(ofile, 0x8000); // constant ordinal

        // BLOK:IMAP:CHAN
        BE_write_uint32(ofile, LWO_CHAN);
        BE_write_uint16(ofile, i->second.colr_map.imap.chan.chunksize);
        BE_write_uint32(ofile, i->second.colr_map.imap.chan.channel);

        // BLOK:IMAP:ENAB
        BE_write_uint32(ofile, LWO_ENAB);
        BE_write_uint16(ofile, i->second.colr_map.imap.enab.chunksize);
        BE_write_uint16(ofile, i->second.colr_map.imap.enab.enable);

        // BLOK:IMAP:NEGA
        BE_write_uint32(ofile, LWO_NEGA);
        BE_write_uint16(ofile, i->second.colr_map.imap.nega.chunksize);
        BE_write_uint16(ofile, i->second.colr_map.imap.nega.enable);

        // BLOK:IMAP:OPAC
        BE_write_uint32(ofile, LWO_OPAC);
        BE_write_uint16(ofile, i->second.colr_map.imap.opac.chunksize);
        BE_write_uint16(ofile, i->second.colr_map.imap.opac.type);
        BE_write_real32(ofile, i->second.colr_map.imap.opac.opacity);
        BE_write_uint16(ofile, 0x00); // envelope

        // BLOK:IMAP:AXIS
        BE_write_uint32(ofile, LWO_AXIS);
        BE_write_uint16(ofile, i->second.colr_map.imap.axis.chunksize);
        BE_write_uint16(ofile, i->second.colr_map.imap.axis.axis);

        // BLOK:TMAP
        uint16 TMAP_chunksize = 0x5A;
        BE_write_uint32(ofile, LWO_TMAP);
        BE_write_uint16(ofile, i->second.colr_map.tmap.chunksize);

        // BLCK:TMAP:CNTR
        BE_write_uint32(ofile, LWO_CNTR);
        BE_write_uint16(ofile, i->second.colr_map.tmap.cntr.chunksize);
        BE_write_real32(ofile, i->second.colr_map.tmap.cntr.x);
        BE_write_real32(ofile, i->second.colr_map.tmap.cntr.y);
        BE_write_real32(ofile, i->second.colr_map.tmap.cntr.z);
        BE_write_uint16(ofile, 0x00); // envelope

        // BLCK:TMAP:SIZE
        BE_write_uint32(ofile, LWO_SIZE);
        BE_write_uint16(ofile, i->second.colr_map.tmap.size.chunksize);
        BE_write_real32(ofile, i->second.colr_map.tmap.size.x);
        BE_write_real32(ofile, i->second.colr_map.tmap.size.y);
        BE_write_real32(ofile, i->second.colr_map.tmap.size.z);
        BE_write_uint16(ofile, 0x00); // envelope

        // BLCK:TMAP:ROTA
        BE_write_uint32(ofile, LWO_ROTA);
        BE_write_uint16(ofile, i->second.colr_map.tmap.rota.chunksize);
        BE_write_real32(ofile, i->second.colr_map.tmap.rota.x);
        BE_write_real32(ofile, i->second.colr_map.tmap.rota.y);
        BE_write_real32(ofile, i->second.colr_map.tmap.rota.z);
        BE_write_uint16(ofile, 0x00); // envelope

        // BLCK:TMAP:FALL
        BE_write_uint32(ofile, LWO_FALL);
        BE_write_uint16(ofile, i->second.colr_map.tmap.fall.chunksize);
        BE_write_uint16(ofile, i->second.colr_map.tmap.fall.type);
        BE_write_real32(ofile, i->second.colr_map.tmap.fall.x);
        BE_write_real32(ofile, i->second.colr_map.tmap.fall.y);
        BE_write_real32(ofile, i->second.colr_map.tmap.fall.z);
        BE_write_uint16(ofile, 0x00); // envelope

        // BLCK:TMAP:CSYS
        BE_write_uint32(ofile, LWO_CSYS);
        BE_write_uint16(ofile, i->second.colr_map.tmap.csys.chunksize);
        BE_write_uint16(ofile, i->second.colr_map.tmap.csys.type);

        // BLCK:PROJ
        BE_write_uint32(ofile, LWO_PROJ);
        BE_write_uint16(ofile, i->second.colr_map.proj.chunksize);
        BE_write_uint16(ofile, i->second.colr_map.proj.mode);

        // BLCK:AXIS
        BE_write_uint32(ofile, LWO_AXIS);
        BE_write_uint16(ofile, i->second.colr_map.axis.chunksize);
        BE_write_uint16(ofile, i->second.colr_map.axis.axis);

        // BLCK:IMAG
        if(i->second.colr_map.imag.chunksize) {
           BE_write_uint32(ofile, LWO_IMAG);
           BE_write_uint16(ofile, i->second.colr_map.imag.chunksize);
           WriteVX(ofile, i->second.colr_map.imag.clipindex);
          }

        // BLCK:WRAP
        BE_write_uint32(ofile, LWO_WRAP);
        BE_write_uint16(ofile, i->second.colr_map.wrap.chunksize);
        BE_write_uint16(ofile, i->second.colr_map.wrap.wrap_w);
        BE_write_uint16(ofile, i->second.colr_map.wrap.wrap_h);

        // BLCK:VMAP
        if(i->second.colr_map.vmap.chunksize) {
           BE_write_uint32(ofile, LWO_VMAP);
           BE_write_uint16(ofile, i->second.colr_map.vmap.chunksize);
           write_aligned_string_02(ofile, i->second.colr_map.vmap.name.c_str());
          }

        // BLCK:AAST
        BE_write_uint32(ofile, LWO_AAST);
        BE_write_uint16(ofile, i->second.colr_map.aast.chunksize);
        BE_write_uint16(ofile, i->second.colr_map.aast.flags);
        BE_write_real32(ofile, i->second.colr_map.aast.strength);

        // BLCK:PIXB
        BE_write_uint32(ofile, LWO_PIXB);
        BE_write_uint16(ofile, i->second.colr_map.pixb.chunksize);
        BE_write_uint16(ofile, i->second.colr_map.pixb.flags);
       }
     if(i->second.flags & LWO_SURF_FLAGS_DIFF_BLOK)
       {
        // BLOK
        BE_write_uint32(ofile, LWO_BLOK);
        BE_write_uint16(ofile, i->second.diff_map.chunksize);

        // BLOK:IMAP
        BE_write_uint32(ofile, LWO_IMAP);
        BE_write_uint16(ofile, i->second.diff_map.imap.chunksize);
        BE_write_uint16(ofile, 0x8000); // constant ordinal

        // BLOK:IMAP:CHAN
        BE_write_uint32(ofile, LWO_CHAN);
        BE_write_uint16(ofile, i->second.diff_map.imap.chan.chunksize);
        BE_write_uint32(ofile, i->second.diff_map.imap.chan.channel);

        // BLOK:IMAP:ENAB
        BE_write_uint32(ofile, LWO_ENAB);
        BE_write_uint16(ofile, i->second.diff_map.imap.enab.chunksize);
        BE_write_uint16(ofile, i->second.diff_map.imap.enab.enable);

        // BLOK:IMAP:NEGA
        BE_write_uint32(ofile, LWO_NEGA);
        BE_write_uint16(ofile, i->second.diff_map.imap.nega.chunksize);
        BE_write_uint16(ofile, i->second.diff_map.imap.nega.enable);

        // BLOK:IMAP:OPAC
        BE_write_uint32(ofile, LWO_OPAC);
        BE_write_uint16(ofile, i->second.diff_map.imap.opac.chunksize);
        BE_write_uint16(ofile, i->second.diff_map.imap.opac.type);
        BE_write_real32(ofile, i->second.diff_map.imap.opac.opacity);
        BE_write_uint16(ofile, 0x00); // envelope

        // BLOK:IMAP:AXIS
        BE_write_uint32(ofile, LWO_AXIS);
        BE_write_uint16(ofile, i->second.diff_map.imap.axis.chunksize);
        BE_write_uint16(ofile, i->second.diff_map.imap.axis.axis);

        // BLOK:TMAP
        uint16 TMAP_chunksize = 0x5A;
        BE_write_uint32(ofile, LWO_TMAP);
        BE_write_uint16(ofile, i->second.diff_map.tmap.chunksize);

        // BLCK:TMAP:CNTR
        BE_write_uint32(ofile, LWO_CNTR);
        BE_write_uint16(ofile, i->second.diff_map.tmap.cntr.chunksize);
        BE_write_real32(ofile, i->second.diff_map.tmap.cntr.x);
        BE_write_real32(ofile, i->second.diff_map.tmap.cntr.y);
        BE_write_real32(ofile, i->second.diff_map.tmap.cntr.z);
        BE_write_uint16(ofile, 0x00); // envelope

        // BLCK:TMAP:SIZE
        BE_write_uint32(ofile, LWO_SIZE);
        BE_write_uint16(ofile, i->second.diff_map.tmap.size.chunksize);
        BE_write_real32(ofile, i->second.diff_map.tmap.size.x);
        BE_write_real32(ofile, i->second.diff_map.tmap.size.y);
        BE_write_real32(ofile, i->second.diff_map.tmap.size.z);
        BE_write_uint16(ofile, 0x00); // envelope

        // BLCK:TMAP:ROTA
        BE_write_uint32(ofile, LWO_ROTA);
        BE_write_uint16(ofile, i->second.diff_map.tmap.rota.chunksize);
        BE_write_real32(ofile, i->second.diff_map.tmap.rota.x);
        BE_write_real32(ofile, i->second.diff_map.tmap.rota.y);
        BE_write_real32(ofile, i->second.diff_map.tmap.rota.z);
        BE_write_uint16(ofile, 0x00); // envelope

        // BLCK:TMAP:FALL
        BE_write_uint32(ofile, LWO_FALL);
        BE_write_uint16(ofile, i->second.diff_map.tmap.fall.chunksize);
        BE_write_uint16(ofile, i->second.diff_map.tmap.fall.type);
        BE_write_real32(ofile, i->second.diff_map.tmap.fall.x);
        BE_write_real32(ofile, i->second.diff_map.tmap.fall.y);
        BE_write_real32(ofile, i->second.diff_map.tmap.fall.z);
        BE_write_uint16(ofile, 0x00); // envelope

        // BLCK:TMAP:CSYS
        BE_write_uint32(ofile, LWO_CSYS);
        BE_write_uint16(ofile, i->second.diff_map.tmap.csys.chunksize);
        BE_write_uint16(ofile, i->second.diff_map.tmap.csys.type);

        // BLCK:PROJ
        BE_write_uint32(ofile, LWO_PROJ);
        BE_write_uint16(ofile, i->second.diff_map.proj.chunksize);
        BE_write_uint16(ofile, i->second.diff_map.proj.mode);

        // BLCK:AXIS
        BE_write_uint32(ofile, LWO_AXIS);
        BE_write_uint16(ofile, i->second.diff_map.axis.chunksize);
        BE_write_uint16(ofile, i->second.diff_map.axis.axis);

        // BLCK:IMAG
        if(i->second.diff_map.imag.chunksize) {
           BE_write_uint32(ofile, LWO_IMAG);
           BE_write_uint16(ofile, i->second.diff_map.imag.chunksize);
           WriteVX(ofile, i->second.diff_map.imag.clipindex);
          }

        // BLCK:WRAP
        BE_write_uint32(ofile, LWO_WRAP);
        BE_write_uint16(ofile, i->second.diff_map.wrap.chunksize);
        BE_write_uint16(ofile, i->second.diff_map.wrap.wrap_w);
        BE_write_uint16(ofile, i->second.diff_map.wrap.wrap_h);

        // BLCK:VMAP
        if(i->second.diff_map.vmap.chunksize) {
           BE_write_uint32(ofile, LWO_VMAP);
           BE_write_uint16(ofile, i->second.diff_map.vmap.chunksize);
           write_aligned_string_02(ofile, i->second.diff_map.vmap.name.c_str());
          }

        // BLCK:AAST
        BE_write_uint32(ofile, LWO_AAST);
        BE_write_uint16(ofile, i->second.diff_map.aast.chunksize);
        BE_write_uint16(ofile, i->second.diff_map.aast.flags);
        BE_write_real32(ofile, i->second.diff_map.aast.strength);

        // BLCK:PIXB
        BE_write_uint32(ofile, LWO_PIXB);
        BE_write_uint16(ofile, i->second.diff_map.pixb.chunksize);
        BE_write_uint16(ofile, i->second.diff_map.pixb.flags);
       }
     if(i->second.flags & LWO_SURF_FLAGS_SPEC_BLOK)
       {
        // BLOK
        BE_write_uint32(ofile, LWO_BLOK);
        BE_write_uint16(ofile, i->second.spec_map.chunksize);

        // BLOK:IMAP
        BE_write_uint32(ofile, LWO_IMAP);
        BE_write_uint16(ofile, i->second.spec_map.imap.chunksize);
        BE_write_uint16(ofile, 0x8000); // constant ordinal

        // BLOK:IMAP:CHAN
        BE_write_uint32(ofile, LWO_CHAN);
        BE_write_uint16(ofile, i->second.spec_map.imap.chan.chunksize);
        BE_write_uint32(ofile, i->second.spec_map.imap.chan.channel);

        // BLOK:IMAP:ENAB
        BE_write_uint32(ofile, LWO_ENAB);
        BE_write_uint16(ofile, i->second.spec_map.imap.enab.chunksize);
        BE_write_uint16(ofile, i->second.spec_map.imap.enab.enable);

        // BLOK:IMAP:NEGA
        BE_write_uint32(ofile, LWO_NEGA);
        BE_write_uint16(ofile, i->second.spec_map.imap.nega.chunksize);
        BE_write_uint16(ofile, i->second.spec_map.imap.nega.enable);

        // BLOK:IMAP:OPAC
        BE_write_uint32(ofile, LWO_OPAC);
        BE_write_uint16(ofile, i->second.spec_map.imap.opac.chunksize);
        BE_write_uint16(ofile, i->second.spec_map.imap.opac.type);
        BE_write_real32(ofile, i->second.spec_map.imap.opac.opacity);
        BE_write_uint16(ofile, 0x00); // envelope

        // BLOK:IMAP:AXIS
        BE_write_uint32(ofile, LWO_AXIS);
        BE_write_uint16(ofile, i->second.spec_map.imap.axis.chunksize);
        BE_write_uint16(ofile, i->second.spec_map.imap.axis.axis);

        // BLOK:TMAP
        uint16 TMAP_chunksize = 0x5A;
        BE_write_uint32(ofile, LWO_TMAP);
        BE_write_uint16(ofile, i->second.spec_map.tmap.chunksize);

        // BLCK:TMAP:CNTR
        BE_write_uint32(ofile, LWO_CNTR);
        BE_write_uint16(ofile, i->second.spec_map.tmap.cntr.chunksize);
        BE_write_real32(ofile, i->second.spec_map.tmap.cntr.x);
        BE_write_real32(ofile, i->second.spec_map.tmap.cntr.y);
        BE_write_real32(ofile, i->second.spec_map.tmap.cntr.z);
        BE_write_uint16(ofile, 0x00); // envelope

        // BLCK:TMAP:SIZE
        BE_write_uint32(ofile, LWO_SIZE);
        BE_write_uint16(ofile, i->second.spec_map.tmap.size.chunksize);
        BE_write_real32(ofile, i->second.spec_map.tmap.size.x);
        BE_write_real32(ofile, i->second.spec_map.tmap.size.y);
        BE_write_real32(ofile, i->second.spec_map.tmap.size.z);
        BE_write_uint16(ofile, 0x00); // envelope

        // BLCK:TMAP:ROTA
        BE_write_uint32(ofile, LWO_ROTA);
        BE_write_uint16(ofile, i->second.spec_map.tmap.rota.chunksize);
        BE_write_real32(ofile, i->second.spec_map.tmap.rota.x);
        BE_write_real32(ofile, i->second.spec_map.tmap.rota.y);
        BE_write_real32(ofile, i->second.spec_map.tmap.rota.z);
        BE_write_uint16(ofile, 0x00); // envelope

        // BLCK:TMAP:FALL
        BE_write_uint32(ofile, LWO_FALL);
        BE_write_uint16(ofile, i->second.spec_map.tmap.fall.chunksize);
        BE_write_uint16(ofile, i->second.spec_map.tmap.fall.type);
        BE_write_real32(ofile, i->second.spec_map.tmap.fall.x);
        BE_write_real32(ofile, i->second.spec_map.tmap.fall.y);
        BE_write_real32(ofile, i->second.spec_map.tmap.fall.z);
        BE_write_uint16(ofile, 0x00); // envelope

        // BLCK:TMAP:CSYS
        BE_write_uint32(ofile, LWO_CSYS);
        BE_write_uint16(ofile, i->second.spec_map.tmap.csys.chunksize);
        BE_write_uint16(ofile, i->second.spec_map.tmap.csys.type);

        // BLCK:PROJ
        BE_write_uint32(ofile, LWO_PROJ);
        BE_write_uint16(ofile, i->second.spec_map.proj.chunksize);
        BE_write_uint16(ofile, i->second.spec_map.proj.mode);

        // BLCK:AXIS
        BE_write_uint32(ofile, LWO_AXIS);
        BE_write_uint16(ofile, i->second.spec_map.axis.chunksize);
        BE_write_uint16(ofile, i->second.spec_map.axis.axis);

        // BLCK:IMAG
        if(i->second.spec_map.imag.chunksize) {
           BE_write_uint32(ofile, LWO_IMAG);
           BE_write_uint16(ofile, i->second.spec_map.imag.chunksize);
           WriteVX(ofile, i->second.spec_map.imag.clipindex);
          }

        // BLCK:WRAP
        BE_write_uint32(ofile, LWO_WRAP);
        BE_write_uint16(ofile, i->second.spec_map.wrap.chunksize);
        BE_write_uint16(ofile, i->second.spec_map.wrap.wrap_w);
        BE_write_uint16(ofile, i->second.spec_map.wrap.wrap_h);

        // BLCK:VMAP
        if(i->second.spec_map.vmap.chunksize) {
           BE_write_uint32(ofile, LWO_VMAP);
           BE_write_uint16(ofile, i->second.spec_map.vmap.chunksize);
           write_aligned_string_02(ofile, i->second.spec_map.vmap.name.c_str());
          }

        // BLCK:AAST
        BE_write_uint32(ofile, LWO_AAST);
        BE_write_uint16(ofile, i->second.spec_map.aast.chunksize);
        BE_write_uint16(ofile, i->second.spec_map.aast.flags);
        BE_write_real32(ofile, i->second.spec_map.aast.strength);

        // BLCK:PIXB
        BE_write_uint32(ofile, LWO_PIXB);
        BE_write_uint16(ofile, i->second.spec_map.pixb.chunksize);
        BE_write_uint16(ofile, i->second.spec_map.pixb.flags);
       }
     if(i->second.flags & LWO_SURF_FLAGS_TRAN_BLOK)
       {
        // BLOK
        BE_write_uint32(ofile, LWO_BLOK);
        BE_write_uint16(ofile, i->second.tran_map.chunksize);

        // BLOK:IMAP
        BE_write_uint32(ofile, LWO_IMAP);
        BE_write_uint16(ofile, i->second.tran_map.imap.chunksize);
        BE_write_uint16(ofile, 0x8000); // constant ordinal

        // BLOK:IMAP:CHAN
        BE_write_uint32(ofile, LWO_CHAN);
        BE_write_uint16(ofile, i->second.tran_map.imap.chan.chunksize);
        BE_write_uint32(ofile, i->second.tran_map.imap.chan.channel);

        // BLOK:IMAP:ENAB
        BE_write_uint32(ofile, LWO_ENAB);
        BE_write_uint16(ofile, i->second.tran_map.imap.enab.chunksize);
        BE_write_uint16(ofile, i->second.tran_map.imap.enab.enable);

        // BLOK:IMAP:NEGA
        BE_write_uint32(ofile, LWO_NEGA);
        BE_write_uint16(ofile, i->second.tran_map.imap.nega.chunksize);
        BE_write_uint16(ofile, i->second.tran_map.imap.nega.enable);

        // BLOK:IMAP:OPAC
        BE_write_uint32(ofile, LWO_OPAC);
        BE_write_uint16(ofile, i->second.tran_map.imap.opac.chunksize);
        BE_write_uint16(ofile, i->second.tran_map.imap.opac.type);
        BE_write_real32(ofile, i->second.tran_map.imap.opac.opacity);
        BE_write_uint16(ofile, 0x00); // envelope

        // BLOK:IMAP:AXIS
        BE_write_uint32(ofile, LWO_AXIS);
        BE_write_uint16(ofile, i->second.tran_map.imap.axis.chunksize);
        BE_write_uint16(ofile, i->second.tran_map.imap.axis.axis);

        // BLOK:TMAP
        uint16 TMAP_chunksize = 0x5A;
        BE_write_uint32(ofile, LWO_TMAP);
        BE_write_uint16(ofile, i->second.tran_map.tmap.chunksize);

        // BLCK:TMAP:CNTR
        BE_write_uint32(ofile, LWO_CNTR);
        BE_write_uint16(ofile, i->second.tran_map.tmap.cntr.chunksize);
        BE_write_real32(ofile, i->second.tran_map.tmap.cntr.x);
        BE_write_real32(ofile, i->second.tran_map.tmap.cntr.y);
        BE_write_real32(ofile, i->second.tran_map.tmap.cntr.z);
        BE_write_uint16(ofile, 0x00); // envelope

        // BLCK:TMAP:SIZE
        BE_write_uint32(ofile, LWO_SIZE);
        BE_write_uint16(ofile, i->second.tran_map.tmap.size.chunksize);
        BE_write_real32(ofile, i->second.tran_map.tmap.size.x);
        BE_write_real32(ofile, i->second.tran_map.tmap.size.y);
        BE_write_real32(ofile, i->second.tran_map.tmap.size.z);
        BE_write_uint16(ofile, 0x00); // envelope

        // BLCK:TMAP:ROTA
        BE_write_uint32(ofile, LWO_ROTA);
        BE_write_uint16(ofile, i->second.tran_map.tmap.rota.chunksize);
        BE_write_real32(ofile, i->second.tran_map.tmap.rota.x);
        BE_write_real32(ofile, i->second.tran_map.tmap.rota.y);
        BE_write_real32(ofile, i->second.tran_map.tmap.rota.z);
        BE_write_uint16(ofile, 0x00); // envelope

        // BLCK:TMAP:FALL
        BE_write_uint32(ofile, LWO_FALL);
        BE_write_uint16(ofile, i->second.tran_map.tmap.fall.chunksize);
        BE_write_uint16(ofile, i->second.tran_map.tmap.fall.type);
        BE_write_real32(ofile, i->second.tran_map.tmap.fall.x);
        BE_write_real32(ofile, i->second.tran_map.tmap.fall.y);
        BE_write_real32(ofile, i->second.tran_map.tmap.fall.z);
        BE_write_uint16(ofile, 0x00); // envelope

        // BLCK:TMAP:CSYS
        BE_write_uint32(ofile, LWO_CSYS);
        BE_write_uint16(ofile, i->second.tran_map.tmap.csys.chunksize);
        BE_write_uint16(ofile, i->second.tran_map.tmap.csys.type);

        // BLCK:PROJ
        BE_write_uint32(ofile, LWO_PROJ);
        BE_write_uint16(ofile, i->second.tran_map.proj.chunksize);
        BE_write_uint16(ofile, i->second.tran_map.proj.mode);

        // BLCK:AXIS
        BE_write_uint32(ofile, LWO_AXIS);
        BE_write_uint16(ofile, i->second.tran_map.axis.chunksize);
        BE_write_uint16(ofile, i->second.tran_map.axis.axis);

        // BLCK:IMAG
        if(i->second.tran_map.imag.chunksize) {
           BE_write_uint32(ofile, LWO_IMAG);
           BE_write_uint16(ofile, i->second.tran_map.imag.chunksize);
           WriteVX(ofile, i->second.tran_map.imag.clipindex);
          }

        // BLCK:WRAP
        BE_write_uint32(ofile, LWO_WRAP);
        BE_write_uint16(ofile, i->second.tran_map.wrap.chunksize);
        BE_write_uint16(ofile, i->second.tran_map.wrap.wrap_w);
        BE_write_uint16(ofile, i->second.tran_map.wrap.wrap_h);

        // BLCK:VMAP
        if(i->second.tran_map.vmap.chunksize) {
           BE_write_uint32(ofile, LWO_VMAP);
           BE_write_uint16(ofile, i->second.tran_map.vmap.chunksize);
           write_aligned_string_02(ofile, i->second.tran_map.vmap.name.c_str());
          }

        // BLCK:AAST
        BE_write_uint32(ofile, LWO_AAST);
        BE_write_uint16(ofile, i->second.tran_map.aast.chunksize);
        BE_write_uint16(ofile, i->second.tran_map.aast.flags);
        BE_write_real32(ofile, i->second.tran_map.aast.strength);

        // BLCK:PIXB
        BE_write_uint32(ofile, LWO_PIXB);
        BE_write_uint16(ofile, i->second.tran_map.pixb.chunksize);
        BE_write_uint16(ofile, i->second.tran_map.pixb.flags);
       }
    }

 return true;
}

void lwFileSaverExample(void)
{
 using namespace std;
 lwFileSaver saver;

 // image clips
 saver.insertClip(1, "image1.tga");
 saver.insertClip(2, "image2.tga");

 // layer #0
 saver.insertLayr(0, nullptr);
 boost::shared_array<lwVertex3D> data1(new lwVertex3D[3]);
 data1[0].x = data1[0].y = data1[0].z = 0.0f;
 data1[1].x = data1[1].y = data1[1].z = 1.0f;
 data1[2].x = data1[2].y = data1[2].z = 2.0f;
 saver.insertPnts(0, 0, data1, 3);

 boost::shared_array<lwRefVertex2D> uv1(new lwRefVertex2D[3]);
 uv1[0].reference = 0; uv1[0].x = 0.0f; uv1[0].y = 0.0f;
 uv1[1].reference = 1; uv1[1].x = 0.0f; uv1[1].y = 0.5f;
 uv1[2].reference = 2; uv1[2].x = 0.0f; uv1[2].y = 1.0f;
 saver.insertTxuv(0, 0, "uv1", uv1, 3);

 boost::shared_array<lwRefVertex1D> w1(new lwRefVertex1D[3]);
 w1[0].reference = 0; w1[0].x = 0.1f;
 w1[1].reference = 1; w1[1].x = 0.2f;
 w1[2].reference = 2; w1[2].x = 0.3f;
 saver.insertWght(0, 0, "w1", w1, 3);

 boost::shared_array<lwRefVertex3D> m1(new lwRefVertex3D[3]);
 m1[0].reference = 0; m1[0].x = m1[0].y = m1[0].z = -1.0f;
 m1[1].reference = 1; m1[1].x = m1[1].y = m1[1].z = -1.0f;
 m1[2].reference = 2; m1[2].x = m1[2].y = m1[2].z = -1.0f;
 saver.insertMorf(0, 0, "m1", m1, 3);

 boost::shared_array<lwTriangle> p1(new lwTriangle[1]);
 p1[0].a = 0;
 p1[0].b = 1;
 p1[0].c = 2;
 saver.insertPols(0, 0, "surface_000", p1, 1);

 // layer #1
 saver.insertLayr(1, nullptr);
 boost::shared_array<lwVertex3D> data2(new lwVertex3D[3]);
 data2[0].x = data2[0].y = data2[0].z = 3.0f;
 data2[1].x = data2[1].y = data2[1].z = 4.0f;
 data2[2].x = data2[2].y = data2[2].z = 5.0f;
 saver.insertPnts(1, 0, data2, 3);

 boost::shared_array<lwRefVertex2D> uv2(new lwRefVertex2D[3]);
 uv2[0].reference = 0; uv2[0].x = 1.0; uv2[0].y = 0.0f;
 uv2[1].reference = 1; uv2[1].x = 1.0; uv2[1].y = 0.5f;
 uv2[2].reference = 2; uv2[2].x = 1.0; uv2[2].y = 1.0f;
 saver.insertTxuv(1, 0, "uv2", uv2, 3);

 boost::shared_array<lwRefVertex1D> w2(new lwRefVertex1D[3]);
 w2[0].reference = 0; w2[0].x = 0.7f;
 w2[1].reference = 1; w2[1].x = 0.8f;
 w2[2].reference = 2; w2[2].x = 0.9f;
 saver.insertWght(1, 0, "w2", w2, 3);

 boost::shared_array<lwRefVertex3D> m2(new lwRefVertex3D[3]);
 m2[0].reference = 0; m2[0].x = m2[0].y = m2[0].z = 1.0f;
 m2[1].reference = 1; m2[1].x = m2[1].y = m2[1].z = 1.0f;
 m2[2].reference = 2; m2[2].x = m2[2].y = m2[2].z = 1.0f;
 saver.insertMorf(1, 0, "m2", m2, 3);

 boost::shared_array<lwTriangle> p2(new lwTriangle[1]);
 p2[0].a = 0;
 p2[0].b = 1;
 p2[0].c = 2;
 saver.insertPols(1, 0, "surface_001", p2, 1);

 saver.insertSurf("surface_000");
 saver.insertSurf("surface_001");
 saver.insertSurf("surface_002");
 saver.setSurfColr("surface_000", 1.0f, 0.0f, 0.0f);
 saver.setSurfColr("surface_001", 0.0f, 1.0f, 0.0f);
 saver.setSurfColr("surface_002", 0.0f, 0.0f, 1.0f);

 // enable surface UV map
 saver.enableSurfColrImag("surface_000");
 saver.enableSurfDiffImag("surface_000");
 saver.enableSurfSpecImag("surface_000");
 saver.enableSurfTranImag("surface_000");

 // set surface UV map 
 saver.setSurfColrVmap("surface_000", "uv1");
 saver.setSurfDiffVmap("surface_000", "uv1");
 saver.setSurfSpecVmap("surface_000", "uv1");
 saver.setSurfTranVmap("surface_000", "uv1");

 // set surface image
 saver.setSurfColrImag("surface_000", 1);
 saver.setSurfTranImag("surface_000", 1);
 saver.setSurfTranNega("surface_000", 1);

 // enable surface UV map
 saver.enableSurfColrImag("surface_001");
 saver.enableSurfDiffImag("surface_001");
 saver.enableSurfSpecImag("surface_001");
 saver.enableSurfTranImag("surface_001");

 // set surface UV map 
 saver.setSurfColrVmap("surface_001", "uv2");
 saver.setSurfDiffVmap("surface_001", "uv2");
 saver.setSurfSpecVmap("surface_001", "uv2");
 saver.setSurfTranVmap("surface_001", "uv2");

 // set surface image
 saver.setSurfColrImag("surface_001", 1);

 //
 // HOW TO: SKELETONS
 //

 // add joints
 uint32 n_joints = 4;
 boost::shared_array<lwVertex3D> joints(new lwVertex3D[n_joints]);
 joints[0].x = 1.0f; joints[0].y = 1.0f; joints[0].z = 0.0f;
 joints[1].x = 1.0f; joints[1].y = 2.0f; joints[1].z = 0.0f;
 joints[2].x = 2.0f; joints[2].y = 2.0f; joints[2].z = 0.0f;
 joints[3].x = 2.0f; joints[3].y = 1.0f; joints[3].z = 0.0f;
 saver.insertJnts(0, 0, joints, n_joints);

 // add bones
 uint32 n_bones = 3;
 boost::shared_array<lwSkelegon> bones(new lwSkelegon[n_bones]);
 bones[0].name = "root"; bones[0].a = 0; bones[0].b = 1;
 bones[1].name = "hand"; bones[1].a = 1; bones[1].b = 2;
 bones[2].name = "fing"; bones[2].a = 2; bones[2].b = 3;
 saver.insertSkel(0, 0, "skeleton", bones, n_bones);

 //
 // HOW TO: SAVE LWO
 //

 saver.save("test.lwo");
}

/*
inline uint32 lwConvertToVX(uint32 index)
{
 if(index < 0xFF00) return index;
 reverse_byte_order(&index);
 index |= 0x000000FF;
 reverse_byte_order(&index);
 return index;
}

class lwJointPolygonTagsChunkSizeCalc : public JOINTVISITOR {
 private :
  std::ofstream& ofile;
  uint32 chunksize;
 public :
  lwJointPolygonTagsChunkSizeCalc(std::ofstream& out) : ofile(out), chunksize(0)
  {
  }
  uint32 getChunkSize(void)const
  {
   return chunksize;
  }
  return_type operator ()(argument1 joint, argument2 index)
  {
   chunksize += align02(joint.name.length() + 1);
   return true;
  }
};

class lwJointPolygonTagsSaver : public JOINTVISITOR {
 private :
  std::ofstream& ofile;
 public :
  lwJointPolygonTagsSaver(std::ofstream& out) : ofile(out)
  {
  }
  return_type operator ()(argument1 joint, argument2 index)
  {
   write_aligned_string_02(ofile, joint.name.c_str());
   return true;
  }
};

class lwJointPointSaver : public JOINTVISITOR {
 private :
  std::ofstream& ofile;
 public :
  lwJointPointSaver(std::ofstream& out) : ofile(out)
  {
  }
  return_type operator ()(argument1 joint, argument2 index)
  {
   BE_write_real32(ofile, -joint.matrix[12]);
   BE_write_real32(ofile, -joint.matrix[13]);
   BE_write_real32(ofile, -joint.matrix[14]);
   return true;
  }
};

class lwJointPolygonChunkSizeCalc : public JOINTVISITOR {
 private :
  std::ofstream& ofile;
  uint32 chunksize;
 public :
  lwJointPolygonChunkSizeCalc(std::ofstream& out) : ofile(out), chunksize(0)
  {
  }
  uint32 getChunkSize(void)const
  {
   return chunksize;
  }
  return_type operator ()(argument1 joint, argument2 index)
  {
   // skip root joint
   if(joint.parent == INVALID_JOINT) return true;
   chunksize += 0x02; // for number of vertices + flags
   chunksize += 0x04; // for indices
   return true;
  }
};

class lwJointPolygonSaver : public JOINTVISITOR {
 private :
  std::ofstream& ofile;
  std::map<uint32, uint32> assoc;
  uint32 point_id;
 public :
  lwJointPolygonSaver(std::ofstream& out) : ofile(out), point_id(0)
  {
   // TODO:
   // We need to set a different initial point_id when we have
   // multiple skeletons.
  }
  return_type operator ()(argument1 joint, argument2 joint_id)
  {
   // add (joint_id, point_id) association data to map
   assoc.insert(std::map<uint32, uint32>::value_type(joint_id, point_id));

   // polygon for each parent-child bone
   if(joint.parent != INVALID_JOINT)
     {
      // get parent point_id
      std::map<uint32, uint32>::iterator iter = assoc.find(joint.parent);
      if(iter == assoc.end()) return false; // every child must have a parent

      // polygon data
      BE_write_uint16(ofile, 0x0002);
      BE_write_uint16(ofile, (uint16)iter->second);
      BE_write_uint16(ofile, (uint16)point_id);
     }

   // update current point index
   point_id++;
   return true;
  }
};

class lwJointPolyTagChunkSizeCalc : public JOINTVISITOR {
 private :
  std::ofstream& ofile;
  uint32 chunksize;
 public :
  lwJointPolyTagChunkSizeCalc(std::ofstream& out) : ofile(out), chunksize(0)
  {
  }
  uint32 getChunkSize(void)const
  {
   return chunksize;
  }
  return_type operator ()(argument1 joint, argument2 index)
  {
   // skip root joint
   if(joint.parent == INVALID_JOINT) return true;
   chunksize += 0x02; // for VX
   chunksize += 0x02; // for tag
   return true;
  }
};

class lwJointPolyTagSaver : public JOINTVISITOR {
 private :
  std::ofstream& ofile;
  uint32 polygon_index;
  uint32 joint_index;
 public :
  lwJointPolyTagSaver(std::ofstream& out) : ofile(out), polygon_index(0), joint_index(0)
  {
   // TODO:
   // We need to set a different initial point_id when we have
   // multiple skeletons.
  }
  return_type operator ()(argument1 joint, argument2 joint_id)
  {
   // polygon for each parent-child bone
   if(joint.parent != INVALID_JOINT)
     {
      // polygon data
      BE_write_uint16(ofile, (uint16)(polygon_index));
      BE_write_uint16(ofile, (uint16)(joint_index + 1));

      // update number of polygons seen
      polygon_index++;
     }

   joint_index++;
   return true;
  }
};

class lwJointPartTagChunkSizeCalc : public JOINTVISITOR {
 private :
  std::ofstream& ofile;
  uint32 chunksize;
 public :
  lwJointPartTagChunkSizeCalc(std::ofstream& out) : ofile(out), chunksize(0)
  {
  }
  uint32 getChunkSize(void)const
  {
   return chunksize;
  }
  return_type operator ()(argument1 joint, argument2 index)
  {
   // skip root joint
   if(joint.parent == INVALID_JOINT) return true;
   chunksize += 0x02; // for VX
   chunksize += 0x02; // for tag
   return true;
  }
};

class lwJointPartTagSaver : public JOINTVISITOR {
 private :
  std::ofstream& ofile;
  uint32 polygon_index;
  uint32 joint_index;
 public :
  lwJointPartTagSaver(std::ofstream& out) : ofile(out), polygon_index(0), joint_index(0)
  {
   // TODO:
   // We need to set a different initial point_id when we have
   // multiple skeletons.
  }
  return_type operator ()(argument1 joint, argument2 joint_id)
  {
   // polygon for each parent-child bone
   if(joint.parent != INVALID_JOINT)
     {
      // polygon data
      BE_write_uint16(ofile, (uint16)(polygon_index));
      BE_write_uint16(ofile, (uint16)(joint_index + 1));

      // update number of polygons seen
      polygon_index++;
     }

   joint_index++;
   return true;
  }
};

struct lwWeightMapPair {
 uint32 index;
 real32 value;
 lwWeightMapPair(uint32 p1 = 0, real32 p2 = 0.0f) : index(p1), value(p2) {}
};

struct lwWeightMap {
 std::string name;
 std::deque<lwWeightMapPair> data;
};

class lwConverter {
 private :
  const SIMPLEMODELCONTAINER& data;
  std::ofstream ofile;
 private :
  uint32 n_wmaps;
  std::vector<lwWeightMap> wmapdata;
 private :
  uint32 FORM_chunksize;
  uint32 LAYR_chunksize;
  uint32 TAGS_chunksize;
  uint32 PNTS_chunksize;
  uint32 POLS_chunksize;
  uint32 PTAG_chunksize;
  boost::shared_array<uint32> TXUV_chunksize;
  boost::shared_array<uint32> WGHT_chunksize;
 private :
  uint32 BONELAYR_chunksize;
  uint32 BONETAGS_chunksize;
  uint32 BONEPNTS_chunksize;
  uint32 BONEPOLS_chunksize;
  uint32 BONEPTAG_chunksize;
  uint32 BONEPART_chunksize;
 private :
  bool sanityChecker(void)const;
  bool error(const char* message)const;
  bool constructWeightMaps(void);
 private :
  void computeFORMChunkSize(void);  // FORM
  void computeLAYRChunkSize(void);  // LAYR
  void computeTAGSChunkSize(void);  // TAGS
  void computePNTSChunkSize(void);  // PNTS
  void computePOLSChunkSize(void);  // POLS
  void computePTAGChunkSize(void);  // SURF
  void computeTXUVChunkSize(void);  // VMAP:TXUV
  void computeWGHTChunkSize(void);  // VMAP:WGHT
  bool saveFORM(void);
  bool saveLAYR(void);
  bool saveTAGS(void);
  bool savePNTS(void);
  bool savePOLS(void);
  bool savePTAG(void);
  bool saveTXUV(void);
  bool saveWGHT(void);
 private :
  void computeBONELAYRChunkSize(void);  // LAYR
  void computeBONETAGSChunkSize(void);  // TAGS
  void computeBONEPNTSChunkSize(void);  // PNTS
  void computeBONEPOLSChunkSize(void);  // POLS
  void computeBONEPTAGChunkSize(void);  // PTAG - BONE
  void computeBONEPARTChunkSize(void);  // PTAG - PART
  bool saveBONELAYR(void);
  bool saveBONETAGS(void);
  bool saveBONEPNTS(void);
  bool saveBONEPOLS(void);
  bool saveBONEPTAG(void);
  bool saveBONEPART(void);
 public :
  bool convert(const char* path, const char* name);
 public :
  lwConverter(const SIMPLEMODELCONTAINER& smc);
 ~lwConverter();
};

lwConverter::lwConverter(const SIMPLEMODELCONTAINER& smc) : data(smc), n_wmaps(0)
{
 FORM_chunksize = 0;
 LAYR_chunksize = 0;
 TAGS_chunksize = 0;
 PNTS_chunksize = 0;
 POLS_chunksize = 0;
 PTAG_chunksize = 0;

 BONELAYR_chunksize = 0;
 BONETAGS_chunksize = 0;
 BONEPNTS_chunksize = 0;
 BONEPOLS_chunksize = 0;
 BONEPTAG_chunksize = 0;
 BONEPART_chunksize = 0;
}

lwConverter::~lwConverter()
{
}

bool lwConverter::sanityChecker(void)const
{
 // must have model
 if(!data.model) return error("No model present in container.");

 // if index buffers are present
 uint32 n_ibuffers = data.model->ibuffer.size();
 for(size_t i = 0; i < n_ibuffers; i++)
    {
     // index buffer must be a triangle list
     IDX_BUFFER& ib = data.model->ibuffer[i];
     if(ib.elem == 0) return error("One or more index buffers are empty.");
     if(ib.elem <= 2) return error("One or more index buffers do not contain at least one triangle.");

     // index buffer must have a valid format
     if(data.model->ibuffer[i].format == FACE_FORMAT_UINT_32) ;
     else if(data.model->ibuffer[i].format == FACE_FORMAT_UINT_16) ;
     else if(data.model->ibuffer[i].format == FACE_FORMAT_UINT_08) ;
     else return error("Unknown face format.");

     // index buffer must have a valid triangle type
     if(data.model->ibuffer[i].type == FACE_TYPE_TRIANGLES) ;
     else if(data.model->ibuffer[i].type == FACE_TYPE_TRISTRIP) ;
     else if(data.model->ibuffer[i].type == FACE_TYPE_TRISTRIPCUT) ;
     else return error("Unknown triangle type.");
    }

 // if skeletons are present
 uint32 n_skeletons = data.skeletons.size();
 if(n_skeletons) {
    for(uint32 i = 0; i < n_skeletons; i++) {
        const SKELETON& skel = data.skeletons[i];
        if(skel.joints() < 2) return error("Skeletons must have at least two joints.");
        if(strlen(skel.get_identifier()) < 1) return error("Skeletons must have a valid string identifier.");
       }
   }

 // TODO:
 // if weight maps are present

 return true;
}

bool lwConverter::error(const char* message)const
{
 std::cout << "LWO Export Error: " << message << std::endl;
 return false;
}

bool lwConverter::constructWeightMaps(void)
{
 // nothing to compute
 n_wmaps = 0;
 if(!(data.model->vbuffer.flags & VERTEX_WEIGHTS)) return true;

 // compute number of weight maps
 if(data.model->vbuffer.flags & VERTEX_WMAPIDX)
   {
    // create weight maps
    uint16 max_index = 0;
    for(uint32 i = 0; i < data.model->vbuffer.elem; i++) {
        const VERTEX& v = data.model->vbuffer.data[i];
        if(v.m1 != INVALID_VERTEX_WMAP_INDEX && !(v.m1 < max_index)) max_index = v.m1;
        if(v.m2 != INVALID_VERTEX_WMAP_INDEX && !(v.m2 < max_index)) max_index = v.m2;
        if(v.m3 != INVALID_VERTEX_WMAP_INDEX && !(v.m3 < max_index)) max_index = v.m3;
        if(v.m4 != INVALID_VERTEX_WMAP_INDEX && !(v.m4 < max_index)) max_index = v.m4;
        if(v.m5 != INVALID_VERTEX_WMAP_INDEX && !(v.m5 < max_index)) max_index = v.m5;
        if(v.m6 != INVALID_VERTEX_WMAP_INDEX && !(v.m6 < max_index)) max_index = v.m6;
        if(v.m7 != INVALID_VERTEX_WMAP_INDEX && !(v.m7 < max_index)) max_index = v.m7;
        if(v.m8 != INVALID_VERTEX_WMAP_INDEX && !(v.m8 < max_index)) max_index = v.m8;
       }
    n_wmaps = max_index + 1;
    wmapdata.resize(n_wmaps);

    // assign names
    for(uint32 i = 0; i < wmapdata.size(); i++) {
        std::stringstream ss;
        ss << "wmap_" << std::setfill('0') << std::setw(3) << i;
        wmapdata[i].name = ss.str();
       }

    // assign weights
    for(uint32 i = 0; i < data.model->vbuffer.elem; i++) {
        const VERTEX& v = data.model->vbuffer.data[i];
        if(v.m1 != INVALID_VERTEX_WMAP_INDEX) wmapdata[v.m1].data.push_back(lwWeightMapPair(i, v.w1/65535.0f));
        if(v.m2 != INVALID_VERTEX_WMAP_INDEX) wmapdata[v.m2].data.push_back(lwWeightMapPair(i, v.w2/65535.0f));
        if(v.m3 != INVALID_VERTEX_WMAP_INDEX) wmapdata[v.m3].data.push_back(lwWeightMapPair(i, v.w3/65535.0f));
        if(v.m4 != INVALID_VERTEX_WMAP_INDEX) wmapdata[v.m4].data.push_back(lwWeightMapPair(i, v.w4/65535.0f));
        if(v.m5 != INVALID_VERTEX_WMAP_INDEX) wmapdata[v.m5].data.push_back(lwWeightMapPair(i, v.w5/65535.0f));
        if(v.m6 != INVALID_VERTEX_WMAP_INDEX) wmapdata[v.m6].data.push_back(lwWeightMapPair(i, v.w6/65535.0f));
        if(v.m7 != INVALID_VERTEX_WMAP_INDEX) wmapdata[v.m7].data.push_back(lwWeightMapPair(i, v.w7/65535.0f));
        if(v.m8 != INVALID_VERTEX_WMAP_INDEX) wmapdata[v.m8].data.push_back(lwWeightMapPair(i, v.w8/65535.0f));
       }
   }
 else if(data.model->vbuffer.flags & VERTEX_BONEIDX)
   {
    // create weight maps
    uint16 max_index = 0;
    for(uint32 i = 0; i < data.model->vbuffer.elem; i++) {
        const VERTEX& v = data.model->vbuffer.data[i];
        if(v.b1 != INVALID_VERTEX_BONE_INDEX && !(v.b1 < max_index)) max_index = v.b1;
        if(v.b2 != INVALID_VERTEX_BONE_INDEX && !(v.b2 < max_index)) max_index = v.b2;
        if(v.b3 != INVALID_VERTEX_BONE_INDEX && !(v.b3 < max_index)) max_index = v.b3;
        if(v.b4 != INVALID_VERTEX_BONE_INDEX && !(v.b4 < max_index)) max_index = v.b4;
        if(v.b5 != INVALID_VERTEX_BONE_INDEX && !(v.b5 < max_index)) max_index = v.b5;
        if(v.b6 != INVALID_VERTEX_BONE_INDEX && !(v.b6 < max_index)) max_index = v.b6;
        if(v.b7 != INVALID_VERTEX_BONE_INDEX && !(v.b7 < max_index)) max_index = v.b7;
        if(v.b8 != INVALID_VERTEX_BONE_INDEX && !(v.b8 < max_index)) max_index = v.b8;
       }
    n_wmaps = max_index + 1;
    wmapdata.resize(n_wmaps);
    std::cout << " n_wmaps = " << n_wmaps << std::endl;

    // TODO
   }
 else
   {
    // assume there is only one weight map
    n_wmaps = 1;
    wmapdata.resize(n_wmaps);
    std::cout << " n_wmaps = " << n_wmaps << std::endl;

    // TODO
   }

 return true;
}

bool lwConverter::convert(const char* path, const char* name)
{
 // preprocessing
 if(!sanityChecker()) return false;
 if(!constructWeightMaps()) return false;

 // create file
 std::stringstream ss;
 ss << path << name << ".lwo";
 ofile.open(ss.str().c_str(), std::ios::binary);
 if(!ofile) return error("Error creating LWO file.");

 // compute 1st layer chunksizes
 computeLAYRChunkSize();
 computeTAGSChunkSize();
 computePNTSChunkSize();
 computePOLSChunkSize();
 computePTAGChunkSize();
 computeTXUVChunkSize();
 computeWGHTChunkSize();

 // compute 2nd layer chunksizes
 computeBONELAYRChunkSize();
 computeBONETAGSChunkSize();
 computeBONEPNTSChunkSize();
 computeBONEPOLSChunkSize();
 computeBONEPTAGChunkSize();
 computeBONEPARTChunkSize();

 // compute form chunksize
 computeFORMChunkSize();

 // save 1st layer chunks
 if(!saveFORM()) return false;
 if(!saveLAYR()) return false;
 if(!saveTAGS()) return false;
 if(!savePNTS()) return false;
 if(!savePOLS()) return false;
 if(!savePTAG()) return false;
 if(!saveTXUV()) return false;
 if(!saveWGHT()) return false;

 // save 2nd layer chunks
 if(!saveBONELAYR()) return false;
 if(!saveBONETAGS()) return false;
 if(!saveBONEPNTS()) return false;
 if(!saveBONEPOLS()) return false;
 if(!saveBONEPTAG()) return false;
 if(!saveBONEPART()) return false;

 return true;
}

//
// FORM
//

void lwConverter::computeFORMChunkSize(void)
{
 FORM_chunksize = 0;
 FORM_chunksize += 0x04; // LWO2

 // TAGS contribution
 if(TAGS_chunksize) {
    FORM_chunksize += 0x08;
    FORM_chunksize += TAGS_chunksize;
   }

 // LAYR contribution
 FORM_chunksize += 0x08;
 FORM_chunksize += LAYR_chunksize;

 // PNTS contribution
 if(PNTS_chunksize) {
    FORM_chunksize += 0x08;
    FORM_chunksize += PNTS_chunksize;
   }

 // POLS contribution
 if(POLS_chunksize) {
    FORM_chunksize += 0x08;
    FORM_chunksize += POLS_chunksize;
   }

 // PTAG contribution
 if(PTAG_chunksize) {
    FORM_chunksize += 0x08;
    FORM_chunksize += PTAG_chunksize;
   }

 // TXUV contribution
 if(TXUV_chunksize) {
    uint32 n_surface = (uint32)data.model->ibuffer.size();
    for(uint32 i = 0; i < n_surface; i++) {
        FORM_chunksize += 0x08;
        FORM_chunksize += TXUV_chunksize[i];
       }
   }

 // WGHT contribution
 if(WGHT_chunksize) {
    for(uint32 i = 0; i < n_wmaps; i++) {
        if(WGHT_chunksize[i]) {
           FORM_chunksize += 0x08;
           FORM_chunksize += WGHT_chunksize[i];
          }
       }
   }

 // LAYR (from bones) contribution
 if(BONELAYR_chunksize) {
    FORM_chunksize += 0x08;
    FORM_chunksize += BONELAYR_chunksize;
   }

 // TAGS (from bones) contribution
 if(BONETAGS_chunksize) {
    FORM_chunksize += 0x08;
    FORM_chunksize += BONETAGS_chunksize;
   }

 // PNTS (from bones) contribution
 if(BONEPNTS_chunksize) {
    FORM_chunksize += 0x08;
    FORM_chunksize += BONEPNTS_chunksize;
   }

 // POLS (from bones) contribution
 if(BONEPOLS_chunksize) {
    FORM_chunksize += 0x08;
    FORM_chunksize += BONEPOLS_chunksize;
   }

 // PTAG (from bones) contribution
 if(BONEPTAG_chunksize) {
    FORM_chunksize += 0x08;
    FORM_chunksize += BONEPTAG_chunksize;
   }

 // PTAG (from bones) contribution
 if(BONEPART_chunksize) {
    FORM_chunksize += 0x08;
    FORM_chunksize += BONEPART_chunksize;
   }
}

bool lwConverter::saveFORM(void)
{
 // FORM + FORM_chunksize + LWO2
 BE_write_uint32(ofile, LWO_FORM);
 BE_write_uint32(ofile, FORM_chunksize);
 BE_write_uint32(ofile, LWO_LWO2);
 return true;
}

void lwConverter::computeLAYRChunkSize(void)
{
 LAYR_chunksize = 0x12;
}

bool lwConverter::saveLAYR(void)
{
 // save LAYR + LAYR_chunksize
 BE_write_uint32(ofile, LWO_LAYR);
 BE_write_uint32(ofile, LAYR_chunksize);

 // save LAYR data
 BE_write_uint16(ofile, 0); // number
 BE_write_uint16(ofile, 0); // flags
 BE_write_real32(ofile, 0); // pivot
 BE_write_real32(ofile, 0); // ...
 BE_write_real32(ofile, 0); // ...
 BE_write_uint08(ofile, 0); // name
 BE_write_uint08(ofile, 0); // ...

 return true;
}

void lwConverter::computeTAGSChunkSize(void)
{
 // compute chunksize
 TAGS_chunksize = 0;
 for(size_t i = 0; i < data.model->ibuffer.size(); i++) {
     IDX_BUFFER& ib = data.model->ibuffer[i];
     TAGS_chunksize += align02((ib.name.length() + 1));
    }
}

bool lwConverter::saveTAGS(void)
{
 // don't save chunk if there are no TAGS
 if(!TAGS_chunksize) return true;

 // save TAGS + TAGS_chunksize
 BE_write_uint32(ofile, LWO_TAGS);
 BE_write_uint32(ofile, TAGS_chunksize);

 // save surface names
 for(size_t i = 0; i < data.model->ibuffer.size(); i++) {
     IDX_BUFFER& ib = data.model->ibuffer[i];
     write_aligned_string_02(ofile, ib.name.c_str());
    }

 return true;
}

void lwConverter::computePNTSChunkSize(void)
{
 // compute chunksize
 uint32 n_points = data.model->vbuffer.elem;
 PNTS_chunksize = 0x0C*n_points;
}

bool lwConverter::savePNTS(void)
{
 // don't save chunk if there are no PNTS
 if(!PNTS_chunksize) return true;

 // save PNTS + PNTS_chunksize
 BE_write_uint32(ofile, LWO_PNTS);
 BE_write_uint32(ofile, PNTS_chunksize);

 // save points
 for(uint32 i = 0; i < data.model->vbuffer.elem; i++) {
     float x = data.model->vbuffer.data[i].vx;
     float y = data.model->vbuffer.data[i].vy;
     float z = data.model->vbuffer.data[i].vz;
     BE_write_real32(ofile, x);
     BE_write_real32(ofile, y);
     BE_write_real32(ofile, z);
    }

 return true;
}

//
// POLS
//

template<class T>
uint32 computeHelperPOLS(const IDX_BUFFER& ib)
{
 uint32 datasize = 0;

 if(ib.type == FACE_TYPE_TRIANGLES)
   {
    // for number of vertices and flags
    uint32 n_triangles = ib.elem/3;
    datasize += 2*n_triangles;

    // for indices
    const T limit = (T)0xFF00;
    const T* buffer = (const T*)(ib.data.get());
    for(uint32 j = 0; j < ib.elem; j++) {
        if(buffer[j] < limit) datasize += 2;
        else datasize += 4;
       }
   }
 else if(ib.type == FACE_TYPE_TRISTRIP)
   {
   }
 else if(ib.type == FACE_TYPE_TRISTRIPCUT)
   {
   }

 return datasize;
}

template<class T>
bool saveHelperPOLS(const IDX_BUFFER& ib, std::ofstream& ofile)
{
 if(ib.type == FACE_TYPE_TRIANGLES)
   {
    uint32 index = 0;
    uint32 n_triangles = ib.elem/3;
    const T* buffer = reinterpret_cast<const T*>(ib.data.get());
    for(uint32 j = 0; j < n_triangles; j++) {
        BE_write_uint16(ofile, 0x0003);     // number of vertices + flags
        uint32 a = (uint32)buffer[index++]; // vertex
        uint32 b = (uint32)buffer[index++]; // vertex
        uint32 c = (uint32)buffer[index++]; // vertex
        if(a < 0xFF00) BE_write_uint16(ofile, (uint16)a); else BE_write_uint32(ofile, lwConvertToVX(a));
        if(b < 0xFF00) BE_write_uint16(ofile, (uint16)b); else BE_write_uint32(ofile, lwConvertToVX(b));
        if(c < 0xFF00) BE_write_uint16(ofile, (uint16)c); else BE_write_uint32(ofile, lwConvertToVX(c));
       }
   }
 else if(ib.type == FACE_TYPE_TRISTRIP)
   {
   }
 else if(ib.type == FACE_TYPE_TRISTRIPCUT)
   {
   }

 return true;
}

void lwConverter::computePOLSChunkSize(void)
{
 // compute chunksize
 POLS_chunksize = 0;
 if(data.model->ibuffer.size()) {
    POLS_chunksize += 0x04; // FACE
    for(uint32 i = 0; i < data.model->ibuffer.size(); i++) {
        // for indices
        IDX_BUFFER& ib = data.model->ibuffer[i];
        if(ib.format == FACE_FORMAT_UINT_32) POLS_chunksize += computeHelperPOLS<const uint32>(ib);
        else if(ib.format == FACE_FORMAT_UINT_16) POLS_chunksize += computeHelperPOLS<const uint16>(ib);
        else if(ib.format == FACE_FORMAT_UINT_08) POLS_chunksize += computeHelperPOLS<const uint08>(ib);
       }
   }
}

bool lwConverter::savePOLS(void)
{
 // don't save chunk if there are no POLS
 if(!POLS_chunksize) return true;

 // save PNTS + PNTS_chunksize + FACE
 BE_write_uint32(ofile, LWO_POLS);
 BE_write_uint32(ofile, POLS_chunksize);
 BE_write_uint32(ofile, LWO_FACE);

 // save polygons
 for(uint32 i = 0; i < data.model->ibuffer.size(); i++) {
     IDX_BUFFER& ib = data.model->ibuffer[i];
     if(ib.format == FACE_FORMAT_UINT_32) saveHelperPOLS<const uint32>(ib, ofile);
     if(ib.format == FACE_FORMAT_UINT_16) saveHelperPOLS<const uint16>(ib, ofile);
     if(ib.format == FACE_FORMAT_UINT_08) saveHelperPOLS<const uint08>(ib, ofile);
    }

 return true;
}

//
// PTAG:SURF
//

void lwConverter::computePTAGChunkSize(void)
{
 // compute chunksize
 PTAG_chunksize = 0;
 if(data.model->ibuffer.size())
   {
    PTAG_chunksize += 0x04; // SURF
    for(uint32 i = 0; i < (uint32)data.model->ibuffer.size(); i++)
       {
        // compute size of VX + tag
        IDX_BUFFER& ib = data.model->ibuffer[i];
        if(ib.tris < 0xFF00) {
           PTAG_chunksize += ib.tris*2; // VX
           PTAG_chunksize += ib.tris*2; // tag
          }
        else {
           PTAG_chunksize += 0xFF00*2; // VX
           PTAG_chunksize += (ib.tris - 0xFF00)*4; // VX
           PTAG_chunksize += ib.tris*2; // tag
          }
       }
   }
}

bool lwConverter::savePTAG(void)
{
 // don't save chunk if there are no TAGS
 if(!PTAG_chunksize) return true;

 // save PTAG + PTAG_chunksize + SURF
 BE_write_uint32(ofile, LWO_PTAG);
 BE_write_uint32(ofile, PTAG_chunksize);
 BE_write_uint32(ofile, LWO_SURF);

 // save VX + tags
 uint32 total_triangles = 0;
 for(uint32 i = 0; i < (uint32)data.model->ibuffer.size(); i++)
    {
     // get index buffer
     IDX_BUFFER& ib = data.model->ibuffer[i];
     if(!ib.elem) continue;

     // for each triangle
     for(uint32 j = 0; j < ib.tris; j++)
        {
         // save VX
         uint32 v1 = total_triangles + j;
         if(v1 < 0xFF00) BE_write_uint16(ofile, (uint16)v1);
         else BE_write_uint32(ofile, lwConvertToVX(v1));
    
         // save tag
         uint16 v2 = (uint16)i;
         BE_write_uint16(ofile, v2);
        }

     // update triangles seen
     total_triangles += ib.tris;
    }

 return true;
}

//
// VMAP:TXUV
//

void lwConverter::computeTXUVChunkSize(void)
{
 // must have surfaces
 uint32 n_surface = (uint32)data.model->ibuffer.size();
 if(!n_surface) return;

 // create chunksize array
 TXUV_chunksize.reset(new uint32[n_surface]);
 for(uint32 i = 0; i < n_surface; i++) TXUV_chunksize[i] = 0;

 // for each surface
 for(uint32 i = 0; i < n_surface; i++)
    {
     // get index buffer
     IDX_BUFFER& ib = data.model->ibuffer[i];
     if(!ib.elem) continue;

     // build a set of vertex indices used in surface
     std::set<uint32> iset;
     if(ib.format == FACE_FORMAT_UINT_32) {
        const uint32* ptr = reinterpret_cast<const uint32*>(ib.data.get());
        for(uint32 j = 0; j < ib.elem; j++) iset.insert((uint32)(ptr[j]));
       }
     else if(ib.format == FACE_FORMAT_UINT_16) {
        const uint16* ptr = reinterpret_cast<const uint16*>(ib.data.get());
        for(uint32 j = 0; j < ib.elem; j++) iset.insert((uint32)(ptr[j]));
       }
     else if(ib.format == FACE_FORMAT_UINT_08) {
        const uint08* ptr = reinterpret_cast<const uint08*>(ib.data.get());
        for(uint32 j = 0; j < ib.elem; j++) iset.insert((uint32)(ptr[j]));
       }

     // TXUV + dimension + name
     TXUV_chunksize[i] += 0x04;
     TXUV_chunksize[i] += 0x02;
     TXUV_chunksize[i] += align02(ib.name.length() + 1);

     // VX + data
     for(std::set<uint32>::iterator ptr = iset.begin(); ptr != iset.end(); ptr++) {
         if((*ptr) < 0xFF00) TXUV_chunksize[i] += 0x02;
         else TXUV_chunksize[i] += 0x04;
         TXUV_chunksize[i] += 0x08;
        }
    }
}

bool lwConverter::saveTXUV(void)
{
 // don't save chunk if there are no TXUV
 if(!TXUV_chunksize) return true;

 // must have vertex buffer
 const VTX_BUFFER& vb = data.model->vbuffer;
 if(!vb.elem) return true;

 // save UV maps
 uint32 n_surface = (uint32)data.model->ibuffer.size();
 for(size_t i = 0; i < n_surface; i++)
    {
     // get index buffer
     IDX_BUFFER& ib = data.model->ibuffer[i];
     if(TXUV_chunksize[i] == 0) continue;

     // build a set of vertex indices used in surface
     std::set<uint32> iset;
     if(ib.format == FACE_FORMAT_UINT_32) {
        const uint32* ptr = reinterpret_cast<const uint32*>(ib.data.get());
        for(uint32 j = 0; j < ib.elem; j++) iset.insert((uint32)(ptr[j]));
       }
     else if(ib.format == FACE_FORMAT_UINT_16) {
        const uint16* ptr = reinterpret_cast<const uint16*>(ib.data.get());
        for(uint32 j = 0; j < ib.elem; j++) iset.insert((uint32)(ptr[j]));
       }
     else if(ib.format == FACE_FORMAT_UINT_08) {
        const uint08* ptr = reinterpret_cast<const uint08*>(ib.data.get());
        for(uint32 j = 0; j < ib.elem; j++) iset.insert((uint32)(ptr[j]));
       }

     // save chunkname, chunksize
     BE_write_uint32(ofile, LWO_VMAP);
     BE_write_uint32(ofile, TXUV_chunksize[i]);

     // save type and dimension
     BE_write_uint32(ofile, LWO_TXUV);
     BE_write_uint16(ofile, 2);

     // save name
     write_aligned_string_02(ofile, ib.name.c_str());

     // save UV map
     for(std::set<uint32>::iterator ptr = iset.begin(); ptr != iset.end(); ptr++) {
         uint32 index = *ptr;
         if(index < 0xFF00) BE_write_uint16(ofile, (uint16)index);
         else BE_write_uint32(ofile, lwConvertToVX(index));
         BE_write_real32(ofile, vb.data[index].tu);
         BE_write_real32(ofile, vb.data[index].tv);
        }
    }

 return true;
}

//
// VMAP:WGHT
//

void lwConverter::computeWGHTChunkSize(void)
{
 // nothing to compute
 if(!n_wmaps) return;

 // create chunksize array
 WGHT_chunksize.reset(new uint32[n_wmaps]);
 for(uint32 i = 0; i < n_wmaps; i++) WGHT_chunksize[i] = 0;

 // for each weight map
 for(uint32 i = 0; i < n_wmaps; i++)
    {
     // skip, nothing in weight map
     const lwWeightMap& wm = wmapdata[i];
     if(!wm.data.size()) continue;

     // WGHT + dimension + name
     WGHT_chunksize[i] += 0x04;
     WGHT_chunksize[i] += 0x02;
     WGHT_chunksize[i] += align02(wm.name.length() + 1);

     // VX + data
     for(uint32 j = 0; j < wm.data.size(); j++) {
         if(wm.data[j].index < 0xFF00) WGHT_chunksize[i] += 0x02;
         else WGHT_chunksize[i] += 0x04;
         WGHT_chunksize[i] += 0x04;
        }
    }
}

bool lwConverter::saveWGHT(void)
{
 // don't save chunk if there are no WGHT
 if(!WGHT_chunksize) return true;

 // save WGHT maps
 for(size_t i = 0; i < n_wmaps; i++)
    {
     // skip, nothing in weight map
     const lwWeightMap& wm = wmapdata[i];
     if(!wm.data.size()) continue;

     // save chunkname, chunksize
     BE_write_uint32(ofile, LWO_VMAP);
     BE_write_uint32(ofile, WGHT_chunksize[i]);

     // save type and dimension
     BE_write_uint32(ofile, LWO_WGHT);
     BE_write_uint16(ofile, 1);

     // save name
     write_aligned_string_02(ofile, wm.name.c_str());

     // save VX + data
     for(uint32 j = 0; j < wm.data.size(); j++) {
         uint32 index = wm.data[j].index;
         real32 value = wm.data[j].value;
         if(index < 0xFF00) BE_write_uint16(ofile, (uint16)index);
         else BE_write_uint32(ofile, lwConvertToVX(index));
         BE_write_real32(ofile, value);
        }
    }

 return true;
}

//
// LAYR (BONES)
//

void lwConverter::computeBONELAYRChunkSize(void)
{
 // compute chunksize
 BONELAYR_chunksize = 0;
 if(data.skeletons.size()) BONELAYR_chunksize = 0x12;
}

bool lwConverter::saveBONELAYR(void)
{
 // don't save chunk if there is no skeleton
 if(!BONELAYR_chunksize) return true;

 // save LAYR + LAYR_chunksize
 BE_write_uint32(ofile, LWO_LAYR);
 BE_write_uint32(ofile, BONELAYR_chunksize);

 // save LAYR data
 BE_write_uint16(ofile, 1); // number
 BE_write_uint16(ofile, 0); // flags
 BE_write_real32(ofile, 0); // pivot
 BE_write_real32(ofile, 0); // ...
 BE_write_real32(ofile, 0); // ...
 BE_write_uint08(ofile, 0); // name
 BE_write_uint08(ofile, 0); // ...

 return true;
}

//
// TAGS (BONES)
//

void lwConverter::computeBONETAGSChunkSize(void)
{
 // compute chunksize
 BONETAGS_chunksize = 0;
 for(size_t i = 0; i < data.skeletons.size(); i++) {
     const SKELETON& skel = data.skeletons[i];
     lwJointPolygonTagsChunkSizeCalc lwinfo(ofile);
     skel.traverse(lwinfo);
     BONETAGS_chunksize += align02(strlen(skel.get_identifier()) + 1); // surface name
     BONETAGS_chunksize += lwinfo.getChunkSize(); // bone names
    }
}

bool lwConverter::saveBONETAGS(void)
{
 // don't save chunk if there are no TAGS
 if(!BONETAGS_chunksize) return true;

 // save TAGS + TAGS_chunksize
 BE_write_uint32(ofile, LWO_TAGS);
 BE_write_uint32(ofile, BONETAGS_chunksize);

 // save surface and bone names
 for(size_t i = 0; i < data.skeletons.size(); i++) {
     const SKELETON& skel = data.skeletons[i];
     write_aligned_string_02(ofile, skel.get_identifier()); // surface name
     lwJointPolygonTagsSaver lwinfo(ofile); // bone names
     skel.traverse(lwinfo);
    }

 return true;
}

//
// PNTS (BONES)
//

void lwConverter::computeBONEPNTSChunkSize(void)
{
 // compute chunksize
 BONEPNTS_chunksize = 0;
 uint32 n_skeletons = data.skeletons.size();
 for(uint32 i = 0; i < n_skeletons; i++) BONEPNTS_chunksize += 0x0C*data.skeletons[i].joints();
}

bool lwConverter::saveBONEPNTS(void)
{
 // don't save chunk if there is no skeleton
 if(!BONEPNTS_chunksize) return true;

 // save PNTS + PNTS_chunksize
 BE_write_uint32(ofile, LWO_PNTS);
 BE_write_uint32(ofile, BONEPNTS_chunksize);

 // save joint positions
 uint32 n_skeletons = data.skeletons.size();
 for(uint32 i = 0; i < n_skeletons; i++) {
     const SKELETON& skel = data.skeletons[i];
     lwJointPointSaver lwjs(ofile);
     skel.traverse(lwjs);
    }

 return true;
}

//
// POLS (BONES)
//

void lwConverter::computeBONEPOLSChunkSize(void)
{
 // compute chunksize
 BONEPOLS_chunksize = 0;
 for(uint32 i = 0; i < data.skeletons.size(); i++) {
     BONEPOLS_chunksize += 0x04; // BONE
     const SKELETON& skel = data.skeletons[i];
     lwJointPolygonChunkSizeCalc lwinfo(ofile);
     skel.traverse(lwinfo);
     BONEPOLS_chunksize += lwinfo.getChunkSize();
    }
}

bool lwConverter::saveBONEPOLS(void)
{
 // don't save chunk if there is no skeleton
 if(!BONEPOLS_chunksize) return true;

 // save POLS + POLS_chunksize
 BE_write_uint32(ofile, LWO_POLS);
 BE_write_uint32(ofile, BONEPOLS_chunksize);
 BE_write_uint32(ofile, LWO_BONE);

 // save joint polygons
 uint32 n_skeletons = data.skeletons.size();
 for(uint32 i = 0; i < n_skeletons; i++) {
     const SKELETON& skel = data.skeletons[i];
     lwJointPolygonSaver lwinfo(ofile);
     skel.traverse(lwinfo);
    }

 return true;
}

//
// PTAG:BONE (BONES)
//

void lwConverter::computeBONEPTAGChunkSize(void)
{
 // compute chunksize
 BONEPTAG_chunksize = 0;
 if(!data.skeletons.size()) return;

 // BONE
 uint32 n_skeletons = data.skeletons.size();
 BONEPTAG_chunksize += 0x04;

 // VX + tags
 for(uint32 i = 0; i < n_skeletons; i++) {
     const SKELETON& skel = data.skeletons[i];
     lwJointPolyTagChunkSizeCalc lwinfo(ofile);
     skel.traverse(lwinfo);
     BONEPTAG_chunksize += lwinfo.getChunkSize();
    }
}

bool lwConverter::saveBONEPTAG(void)
{
 // don't save chunk if there is no skeleton
 if(!BONEPTAG_chunksize) return true;

 // save PTAG + PTAG_chunksize
 BE_write_uint32(ofile, LWO_PTAG);
 BE_write_uint32(ofile, BONEPTAG_chunksize);
 BE_write_uint32(ofile, LWO_BONE);

 // save joint polygon tags
 uint32 n_skeletons = data.skeletons.size();
 for(uint32 i = 0; i < n_skeletons; i++) {
     const SKELETON& skel = data.skeletons[i];
     lwJointPolyTagSaver lwinfo(ofile);
     skel.traverse(lwinfo);
    }

 return true;
}

//
// PTAG:PART (BONES)
//

void lwConverter::computeBONEPARTChunkSize(void)
{
 // compute chunksize
 BONEPART_chunksize = 0;
 if(!data.skeletons.size()) return;

 // BONE
 uint32 n_skeletons = data.skeletons.size();
 BONEPART_chunksize += 0x04;

 // VX + tags
 for(uint32 i = 0; i < n_skeletons; i++) {
     const SKELETON& skel = data.skeletons[i];
     lwJointPartTagChunkSizeCalc lwinfo(ofile);
     skel.traverse(lwinfo);
     BONEPART_chunksize += lwinfo.getChunkSize();
    }
}

bool lwConverter::saveBONEPART(void)
{
 // don't save chunk if there is no skeleton
 if(!BONEPART_chunksize) return true;

 // save PTAG + PTAG_chunksize
 BE_write_uint32(ofile, LWO_PTAG);
 BE_write_uint32(ofile, BONEPART_chunksize);
 BE_write_uint32(ofile, LWO_PART);

 // save joint polygon tags
 uint32 n_skeletons = data.skeletons.size();
 for(uint32 i = 0; i < n_skeletons; i++) {
     const SKELETON& skel = data.skeletons[i];
     lwJointPartTagSaver lwinfo(ofile);
     skel.traverse(lwinfo);
    }

 return true;
}







uint32 lwGetCLIPContribution(const boost::shared_array<uint32>& chunksize, uint32 n);
boost::shared_array<uint32> lwGetCLIPChunkSize(const SIMPLEMODELCONTAINER& data);
bool lwSaveCLIP(std::ofstream& ofile, const SIMPLEMODELCONTAINER& data, const boost::shared_array<uint32>& chunksize);

boost::shared_array<uint32> lwGetSURFChunkSize(const SIMPLEMODELCONTAINER& data);
bool lwSaveSURF(std::ofstream& ofile, const SIMPLEMODELCONTAINER& data, const boost::shared_array<uint32>& chunksize);

bool lwSaveVmapTxuv(std::ofstream& ofile, const SIMPLEMODELCONTAINER& data, const boost::shared_array<uint32>& TXUV_chunksize);

bool GeometryToLWO(const char* path, const char* name, const SIMPLEMODELCONTAINER& data)
{
 lwConverter lwc(data);
 return lwc.convert(path, name);
}

//
// LWO_CLIP
//

uint32 lwGetCLIPContribution(const boost::shared_array<uint32>& chunksize, uint32 n)
{
 uint32 retval = 0;
 for(uint32 i = 0; i < n; i++) {
     retval += 0x04;         // CLIP
     retval += 0x04;         // chunksize
     retval += chunksize[i]; // chunkdata
    }
 return retval;
}

boost::shared_array<uint32> lwGetCLIPChunkSize(const SIMPLEMODELCONTAINER& data)
{
 // must have textures
 boost::shared_array<uint32> retval;
 if(data.textures.size() == 0) return retval;

 // compute chunksizes
 retval.reset(new uint32[data.textures.size()]);
 for(uint32 i = 0; i < data.textures.size(); i++)
    {
     // get texture
     const TEXTURE& tex = data.textures[i];
     retval[i] = 0;

     // index
     retval[i] += 0x04;

     // STIL
     retval[i] += 0x04;

     // STIL chunksize
     retval[i] += 0x02;

     // filename
     uint32 namelen = tex.filename.length() + 1;
     retval[i] += align02(namelen);

     // FLAG, chunksize, flags
     retval[i] += 0x0A;
    }

 return retval;
}

bool lwSaveCLIP(std::ofstream& ofile, const SIMPLEMODELCONTAINER& data, const boost::shared_array<uint32>& chunksize)
{
 // for each texture
 for(uint32 i = 0; i < (uint32)data.textures.size(); i++)
    {
     // get texture
     const TEXTURE& tex = data.textures[i];

     // save chunkname, chunksize
     BE_write_uint32(ofile, LWO_CLIP);
     BE_write_uint32(ofile, chunksize[i]);

     // save index
     uint32 clipindex = (i + 1);
     BE_write_uint32(ofile, clipindex);

     // save STIL
     BE_write_uint32(ofile, LWO_STIL);

     // save STIL chunksize
     uint16 namelen = (uint16)(tex.filename.length() + 1);
     namelen = align02(namelen);
     BE_write_uint16(ofile, namelen);

     // save filename
     write_aligned_string_02(ofile, tex.filename.c_str());

     // save FLAG
     BE_write_uint32(ofile, LWO_FLAG);
     BE_write_uint16(ofile, 0x0004);
     BE_write_uint16(ofile, 0x0800);
     BE_write_uint16(ofile, 0x0080);
    }

 return true;
}

//
// LWO_SURF
//

boost::shared_array<uint32> lwGetSURFChunkSize(const SIMPLEMODELCONTAINER& data)
{
 // must have index buffer
 boost::shared_array<uint32> retval;
 if(data.model->ibuffer.size() == 0) return retval;

 uint32 n_surface = (uint32)data.model->ibuffer.size();
 retval.reset(new uint32[n_surface]);

 // for each surface
 for(uint32 i = 0; i < n_surface; i++)
    {
     // get index buffer
     IDX_BUFFER& ib = data.model->ibuffer[i];
     if(!ib.elem) continue;

     // initialize chunksize
     retval[i] = 0;

     // name
     uint32 namelen = ib.name.length() + 1;
     retval[i] += align02(namelen);

     // source
     retval[i] += 2;

     // color, diffuse, specular
     retval[i] += 0x14;
     retval[i] += 0x0C;
     retval[i] += 0x0C;

     // materials
     if((ib.material != 0xFFFFFFFF) && (ib.material < data.materials.size()))
       {
        // base color map
        const MATERIAL& mat = data.materials[ib.material];
        if((mat.basemap != 0xFFFFFFFF) && (mat.basemap < data.textures.size()))
          {
           retval[i] += 0x06; // BLOK, chunksize
           retval[i] += 0x08; // IMAP, chunksize
           retval[i] += 0x0A; // CHAN, chunksize, channel
           retval[i] += 0x0E; // OPAC, chunksize, type, opacity, envelope
           retval[i] += 0x08; // ENAB, chunksize, bool
           retval[i] += 0x08; // NEGA, chunksize, bool
           retval[i] += 0x06; // TMAP, chunksize
           retval[i] += 0x14; // CNTR, chunksize, x, y, z, envelope
           retval[i] += 0x14; // SIZE, chunksize, x, y, z, envelope
           retval[i] += 0x14; // ROTA, chunksize, x, y, z, envelope
           retval[i] += 0x16; // FALL, chunksize, type, x, y, z, envelope
           retval[i] += 0x08; // PROJ, chunksize, mode
           retval[i] += 0x08; // OREF, chunksize, name
           retval[i] += 0x08; // CSYS, chunksize, type
           retval[i] += 0x08; // AXIS, chunksize, axis
           retval[i] += 0x08; // IMAG, chunksize, clip
           retval[i] += 0x0A; // WRAP, chunksize, wrap, wrap
           retval[i] += 0x0C; // WRPW, chunksize, cycles, envelope
           retval[i] += 0x0C; // WRPW, chunksize, cycles, envelope
           retval[i] += 0x06; // VMAP, chunksize, string
           retval[i] += align02(ib.name.length() + 1);
           retval[i] += 0x0C; // AAST, chunksize, flags, strength
           retval[i] += 0x08; // PIXB, chunksize, flags
          }

        // specular map
        if((mat.specmap != 0xFFFFFFFF) && (mat.specmap < data.textures.size()))
          {
           retval[i] += 0x06; // BLOK, chunksize
           retval[i] += 0x08; // IMAP, chunksize
           retval[i] += 0x0A; // CHAN, chunksize, channel
           retval[i] += 0x0E; // OPAC, chunksize, type, opacity, envelope
           retval[i] += 0x08; // ENAB, chunksize, bool
           retval[i] += 0x08; // NEGA, chunksize, bool
           retval[i] += 0x06; // TMAP, chunksize
           retval[i] += 0x14; // CNTR, chunksize, x, y, z, envelope
           retval[i] += 0x14; // SIZE, chunksize, x, y, z, envelope
           retval[i] += 0x14; // ROTA, chunksize, x, y, z, envelope
           retval[i] += 0x16; // FALL, chunksize, type, x, y, z, envelope
           retval[i] += 0x08; // OREF, chunksize, name
           retval[i] += 0x08; // CSYS, chunksize, type
           retval[i] += 0x08; // PROJ, chunksize, mode
           retval[i] += 0x08; // AXIS, chunksize, axis
           retval[i] += 0x08; // IMAG, chunksize, clip
           retval[i] += 0x0A; // WRAP, chunksize, wrap, wrap
           retval[i] += 0x0C; // WRPW, chunksize, cycles, envelope
           retval[i] += 0x0C; // WRPW, chunksize, cycles, envelope
           retval[i] += 0x06; // VMAP, chunksize, string
           retval[i] += align02(ib.name.length() + 1);
           retval[i] += 0x0C; // AAST, chunksize, flags, strength
           retval[i] += 0x08; // PIXB, chunksize, flags
          }

        // TODO
        // normal map
        if((mat.normmap != 0xFFFFFFFF) && (mat.normmap < data.textures.size()))
          {
          }
       }
    }

 return retval;
}

bool lwSaveSURF(std::ofstream& ofile, const SIMPLEMODELCONTAINER& data, const boost::shared_array<uint32>& chunksize)
{
 // must have index buffer
 if(data.model->ibuffer.size() == 0)
    return true;

 // surface names
 for(size_t i = 0; i < data.model->ibuffer.size(); i++)
    {
     // get index buffer
     IDX_BUFFER& ib = data.model->ibuffer[i];
     if(!ib.elem) continue;

     // save chunkname, chunksize
     BE_write_uint32(ofile, LWO_SURF);
     BE_write_uint32(ofile, chunksize[i]);

     // save name
     write_aligned_string_02(ofile, ib.name.c_str());

     // save source
     BE_write_uint08(ofile, 0);
     BE_write_uint08(ofile, 0);

     // save color
     BE_write_uint32(ofile, LWO_COLR);
     BE_write_uint16(ofile, 0x0E);
     BE_write_uint32(ofile, 0x3F48C8C9);
     BE_write_uint32(ofile, 0x3F48C8C9);
     BE_write_uint32(ofile, 0x3F48C8C9);
     BE_write_uint16(ofile, 0x00);

     // save diffuse
     BE_write_uint32(ofile, LWO_DIFF);
     BE_write_uint16(ofile, 0x06);
     BE_write_uint32(ofile, 0x3F800000);
     BE_write_uint16(ofile, 0x00);

     // save specular
     BE_write_uint32(ofile, LWO_SPEC);
     BE_write_uint16(ofile, 0x06);
     BE_write_uint32(ofile, 0x00000000);
     BE_write_uint16(ofile, 0x00);

     // materials
     if((ib.material != 0xFFFFFFFF) && (ib.material < data.materials.size()))
       {
        // base color map
        const MATERIAL& mat = data.materials[ib.material];
        if((mat.basemap != 0xFFFFFFFF) && (mat.basemap < data.textures.size()))
          {
           // BLCK
           uint16 BLOK_chunksize = 0xEA + (uint16)align02(ib.name.length() + 1);
           BE_write_uint32(ofile, LWO_BLOK);
           BE_write_uint16(ofile, BLOK_chunksize);

           // IMAP
           uint16 IMAP_chunksize = 0x2A;
           BE_write_uint32(ofile, LWO_IMAP);
           BE_write_uint16(ofile, IMAP_chunksize);
           BE_write_uint16(ofile, 0x8000); // ordinal

           // CHAN
           uint16 CHAN_chunksize = 0x04;
           BE_write_uint32(ofile, LWO_CHAN);
           BE_write_uint16(ofile, CHAN_chunksize);
           BE_write_uint32(ofile, LWO_COLR);

           // OPAC
           uint16 OPAC_chunksize = 0x08;
           BE_write_uint32(ofile, LWO_OPAC);
           BE_write_uint16(ofile, OPAC_chunksize);
           BE_write_uint16(ofile, 0x00);
           BE_write_uint32(ofile, 0x3F800000);
           BE_write_uint16(ofile, 0x00);

           // ENAB
           uint16 ENAB_chunksize = 0x02;
           BE_write_uint32(ofile, LWO_ENAB);
           BE_write_uint16(ofile, ENAB_chunksize);
           BE_write_uint16(ofile, 0x01);

           // NEGA
           uint16 NEGA_chunksize = 0x02;
           BE_write_uint32(ofile, LWO_NEGA);
           BE_write_uint16(ofile, NEGA_chunksize);
           BE_write_uint16(ofile, 0x00);

           // TMAP
           uint16 TMAP_chunksize = 0x5A;
           BE_write_uint32(ofile, LWO_TMAP);
           BE_write_uint16(ofile, TMAP_chunksize);

           // CNTR
           uint16 CNTR_chunksize = 0x0E;
           BE_write_uint32(ofile, LWO_CNTR);
           BE_write_uint16(ofile, CNTR_chunksize);
           BE_write_uint32(ofile, 0x00000000);
           BE_write_uint32(ofile, 0x00000000);
           BE_write_uint32(ofile, 0x00000000);
           BE_write_uint16(ofile, 0x00);

           // SIZE
           uint16 SIZE_chunksize = 0x0E;
           BE_write_uint32(ofile, LWO_SIZE);
           BE_write_uint16(ofile, SIZE_chunksize);
           BE_write_uint32(ofile, 0x3F800000);
           BE_write_uint32(ofile, 0x3F800000);
           BE_write_uint32(ofile, 0x3F800000);
           BE_write_uint16(ofile, 0x00);

           // ROTA
           uint16 ROTA_chunksize = 0x0E;
           BE_write_uint32(ofile, LWO_ROTA);
           BE_write_uint16(ofile, ROTA_chunksize);
           BE_write_uint32(ofile, 0x00000000);
           BE_write_uint32(ofile, 0x00000000);
           BE_write_uint32(ofile, 0x00000000);
           BE_write_uint16(ofile, 0x00);

           // FALL
           uint16 FALL_chunksize = 0x10;
           BE_write_uint32(ofile, LWO_FALL);
           BE_write_uint16(ofile, FALL_chunksize);
           BE_write_uint16(ofile, 0x00);
           BE_write_uint32(ofile, 0x00000000);
           BE_write_uint32(ofile, 0x00000000);
           BE_write_uint32(ofile, 0x00000000);
           BE_write_uint16(ofile, 0x00);

           // OMAP
           uint16 OMAP_chunksize = 0x02;
           BE_write_uint32(ofile, LWO_OMAP);
           BE_write_uint16(ofile, OMAP_chunksize);
           BE_write_uint16(ofile, 0x0000);

           // CSYS
           uint16 CSYS_chunksize = 0x02;
           BE_write_uint32(ofile, LWO_CSYS);
           BE_write_uint16(ofile, CSYS_chunksize);
           BE_write_uint16(ofile, 0x0000);

           // PROJ
           uint16 PROJ_chunksize = 0x02;
           BE_write_uint32(ofile, LWO_PROJ);
           BE_write_uint16(ofile, PROJ_chunksize);
           BE_write_uint16(ofile, 0x0005);

           // AXIS
           uint16 AXIS_chunksize = 0x02;
           BE_write_uint32(ofile, LWO_AXIS);
           BE_write_uint16(ofile, AXIS_chunksize);
           BE_write_uint16(ofile, 0x0002);

           // IMAG
           uint16 IMAG_chunksize = 0x02;
           BE_write_uint32(ofile, LWO_IMAG);
           BE_write_uint16(ofile, IMAG_chunksize);
           BE_write_uint16(ofile, (mat.basemap + 1));

           // WRAP
           uint16 WRAP_chunksize = 0x04;
           BE_write_uint32(ofile, LWO_WRAP);
           BE_write_uint16(ofile, WRAP_chunksize);
           BE_write_uint16(ofile, 0x0001);
           BE_write_uint16(ofile, 0x0001);

           // WRPW
           uint16 WRPW_chunksize = 0x06;
           BE_write_uint32(ofile, LWO_WRPW);
           BE_write_uint16(ofile, WRPW_chunksize);
           BE_write_uint32(ofile, 0x3F800000);
           BE_write_uint16(ofile, 0x0000);

           // WRPH
           uint16 WRPH_chunksize = 0x06;
           BE_write_uint32(ofile, LWO_WRPH);
           BE_write_uint16(ofile, WRPH_chunksize);
           BE_write_uint32(ofile, 0x3F800000);
           BE_write_uint16(ofile, 0x0000);

           // VMAP
           uint16 VMAP_chunksize = align02(ib.name.length() + 1);
           BE_write_uint32(ofile, LWO_VMAP);
           BE_write_uint16(ofile, VMAP_chunksize);
           write_aligned_string_02(ofile, ib.name.c_str());

           // AAST
           uint16 AAST_chunksize = 0x06;
           BE_write_uint32(ofile, LWO_AAST);
           BE_write_uint16(ofile, AAST_chunksize);
           BE_write_uint16(ofile, 0x0001);
           BE_write_uint32(ofile, 0x3F800000);

           // PIXB
           uint16 PIXB_chunksize = 0x02;
           BE_write_uint32(ofile, LWO_PIXB);
           BE_write_uint16(ofile, PIXB_chunksize);
           BE_write_uint16(ofile, 0x0001);
          }
        // specular map
        if((mat.specmap != 0xFFFFFFFF) && (mat.specmap < data.textures.size()))
          {
           // BLCK
           uint16 BLOK_chunksize = 0xEA + (uint16)align02(ib.name.length() + 1);
           BE_write_uint32(ofile, LWO_BLOK);
           BE_write_uint16(ofile, BLOK_chunksize);

           // IMAP
           uint16 IMAP_chunksize = 0x2A;
           BE_write_uint32(ofile, LWO_IMAP);
           BE_write_uint16(ofile, IMAP_chunksize);
           BE_write_uint16(ofile, 0x8000); // ordinal

           // CHAN
           uint16 CHAN_chunksize = 0x04;
           BE_write_uint32(ofile, LWO_CHAN);
           BE_write_uint16(ofile, CHAN_chunksize);
           BE_write_uint32(ofile, LWO_SPEC);

           // OPAC
           uint16 OPAC_chunksize = 0x08;
           BE_write_uint32(ofile, LWO_OPAC);
           BE_write_uint16(ofile, OPAC_chunksize);
           BE_write_uint16(ofile, 0x00);
           BE_write_uint32(ofile, 0x3F800000);
           BE_write_uint16(ofile, 0x00);

           // ENAB
           uint16 ENAB_chunksize = 0x02;
           BE_write_uint32(ofile, LWO_ENAB);
           BE_write_uint16(ofile, ENAB_chunksize);
           BE_write_uint16(ofile, 0x01);

           // NEGA
           uint16 NEGA_chunksize = 0x02;
           BE_write_uint32(ofile, LWO_NEGA);
           BE_write_uint16(ofile, NEGA_chunksize);
           BE_write_uint16(ofile, 0x00);

           // TMAP
           uint16 TMAP_chunksize = 0x5A;
           BE_write_uint32(ofile, LWO_TMAP);
           BE_write_uint16(ofile, TMAP_chunksize);

           // CNTR
           uint16 CNTR_chunksize = 0x0E;
           BE_write_uint32(ofile, LWO_CNTR);
           BE_write_uint16(ofile, CNTR_chunksize);
           BE_write_uint32(ofile, 0x00000000);
           BE_write_uint32(ofile, 0x00000000);
           BE_write_uint32(ofile, 0x00000000);
           BE_write_uint16(ofile, 0x00);

           // SIZE
           uint16 SIZE_chunksize = 0x0E;
           BE_write_uint32(ofile, LWO_SIZE);
           BE_write_uint16(ofile, SIZE_chunksize);
           BE_write_uint32(ofile, 0x3F800000);
           BE_write_uint32(ofile, 0x3F800000);
           BE_write_uint32(ofile, 0x3F800000);
           BE_write_uint16(ofile, 0x00);

           // ROTA
           uint16 ROTA_chunksize = 0x0E;
           BE_write_uint32(ofile, LWO_ROTA);
           BE_write_uint16(ofile, ROTA_chunksize);
           BE_write_uint32(ofile, 0x00000000);
           BE_write_uint32(ofile, 0x00000000);
           BE_write_uint32(ofile, 0x00000000);
           BE_write_uint16(ofile, 0x00);

           // FALL
           uint16 FALL_chunksize = 0x10;
           BE_write_uint32(ofile, LWO_FALL);
           BE_write_uint16(ofile, FALL_chunksize);
           BE_write_uint16(ofile, 0x00);
           BE_write_uint32(ofile, 0x00000000);
           BE_write_uint32(ofile, 0x00000000);
           BE_write_uint32(ofile, 0x00000000);
           BE_write_uint16(ofile, 0x00);

           // OMAP
           uint16 OMAP_chunksize = 0x02;
           BE_write_uint32(ofile, LWO_OMAP);
           BE_write_uint16(ofile, OMAP_chunksize);
           BE_write_uint16(ofile, 0x0000);

           // CSYS
           uint16 CSYS_chunksize = 0x02;
           BE_write_uint32(ofile, LWO_CSYS);
           BE_write_uint16(ofile, CSYS_chunksize);
           BE_write_uint16(ofile, 0x0000);

           // PROJ
           uint16 PROJ_chunksize = 0x02;
           BE_write_uint32(ofile, LWO_PROJ);
           BE_write_uint16(ofile, PROJ_chunksize);
           BE_write_uint16(ofile, 0x0005);

           // AXIS
           uint16 AXIS_chunksize = 0x02;
           BE_write_uint32(ofile, LWO_AXIS);
           BE_write_uint16(ofile, AXIS_chunksize);
           BE_write_uint16(ofile, 0x0002);

           // IMAG
           uint16 IMAG_chunksize = 0x02;
           BE_write_uint32(ofile, LWO_IMAG);
           BE_write_uint16(ofile, IMAG_chunksize);
           BE_write_uint16(ofile, (mat.specmap + 1));

           // WRAP
           uint16 WRAP_chunksize = 0x04;
           BE_write_uint32(ofile, LWO_WRAP);
           BE_write_uint16(ofile, WRAP_chunksize);
           BE_write_uint16(ofile, 0x0001);
           BE_write_uint16(ofile, 0x0001);

           // WRPW
           uint16 WRPW_chunksize = 0x06;
           BE_write_uint32(ofile, LWO_WRPW);
           BE_write_uint16(ofile, WRPW_chunksize);
           BE_write_uint32(ofile, 0x3F800000);
           BE_write_uint16(ofile, 0x0000);

           // WRPH
           uint16 WRPH_chunksize = 0x06;
           BE_write_uint32(ofile, LWO_WRPH);
           BE_write_uint16(ofile, WRPH_chunksize);
           BE_write_uint32(ofile, 0x3F800000);
           BE_write_uint16(ofile, 0x0000);

           // VMAP
           uint16 VMAP_chunksize = align02(ib.name.length() + 1);
           BE_write_uint32(ofile, LWO_VMAP);
           BE_write_uint16(ofile, VMAP_chunksize);
           write_aligned_string_02(ofile, ib.name.c_str());

           // AAST
           uint16 AAST_chunksize = 0x06;
           BE_write_uint32(ofile, LWO_AAST);
           BE_write_uint16(ofile, AAST_chunksize);
           BE_write_uint16(ofile, 0x0001);
           BE_write_uint32(ofile, 0x3F800000);

           // PIXB
           uint16 PIXB_chunksize = 0x02;
           BE_write_uint32(ofile, LWO_PIXB);
           BE_write_uint16(ofile, PIXB_chunksize);
           BE_write_uint16(ofile, 0x0001);
          }
        
        // TODO
        // normal map
        if((mat.normmap != 0xFFFFFFFF) && (mat.normmap < data.textures.size()))
          {
          }
       }
    }

 return true;
}
*/
