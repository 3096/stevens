//
// VERTEX FORMATS
// PART 1
//

bool namco_fvf_16YY(AMC_VTXBUFFER& vb, binary_stream& bs, uint32 offset)
{
 // optional flags
 vb.flags |= AMC_VERTEX_WEIGHTS;

 // seek vertex data
 bs.seek(offset);
 if(bs.fail()) return error("Stream seek failure.", __LINE__);

 // read vertices
 for(uint32 j = 0; j < vb.elem; j++)
    {
     vb.data[j].vx = bs.BE_read_real32(); // position
     vb.data[j].vy = bs.BE_read_real32(); // position
     vb.data[j].vz = bs.BE_read_real32(); // position
     vb.data[j].nx = bs.BE_read_real16(); // normal
     vb.data[j].ny = bs.BE_read_real16(); // normal
     vb.data[j].nz = bs.BE_read_real16(); // normal
     vb.data[j].nw = bs.BE_read_real16(); // normal
     vb.data[j].wi[0] = bs.BE_read_uint32(); // blendindice
     vb.data[j].wi[1] = bs.BE_read_uint32(); // blendindice
     vb.data[j].wi[2] = bs.BE_read_uint32(); // blendindice
     vb.data[j].wi[3] = bs.BE_read_uint32(); // blendindice
     vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[j].wv[0] = bs.BE_read_real32(); // blendweight
     vb.data[j].wv[1] = bs.BE_read_real32(); // blendweight
     vb.data[j].wv[2] = bs.BE_read_real32(); // blendweight
     vb.data[j].wv[3] = bs.BE_read_real32(); // blendweight
     vb.data[j].wv[4] = 0.0f;
     vb.data[j].wv[5] = 0.0f;
     vb.data[j].wv[6] = 0.0f;
     vb.data[j].wv[7] = 0.0f;
    }

 return true;
}

bool namco_fvf_46YY(AMC_VTXBUFFER& vb, binary_stream& bs, uint32 offset)
{
 // optional flags
 vb.flags |= AMC_VERTEX_WEIGHTS;

 // seek vertex data
 bs.seek(offset);
 if(bs.fail()) return error("Stream seek failure.", __LINE__);

 // read vertices
 for(uint32 j = 0; j < vb.elem; j++)
    {
     vb.data[j].vx = bs.BE_read_real32(); // position
     vb.data[j].vy = bs.BE_read_real32(); // position
     vb.data[j].vz = bs.BE_read_real32(); // position
     vb.data[j].nx = bs.BE_read_real16(); // normal
     vb.data[j].ny = bs.BE_read_real16(); // normal
     vb.data[j].nz = bs.BE_read_real16(); // normal
     vb.data[j].nw = bs.BE_read_real16(); // normal
     vb.data[j].wi[0] = bs.BE_read_uint08(); // blendindice
     vb.data[j].wi[1] = bs.BE_read_uint08(); // blendindice
     vb.data[j].wi[2] = bs.BE_read_uint08(); // blendindice
     vb.data[j].wi[3] = bs.BE_read_uint08(); // blendindice
     vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[j].wv[0] = bs.BE_read_uint08()/255.0f; // blendweight
     vb.data[j].wv[1] = bs.BE_read_uint08()/255.0f; // blendweight
     vb.data[j].wv[2] = bs.BE_read_uint08()/255.0f; // blendweight
     vb.data[j].wv[3] = bs.BE_read_uint08()/255.0f; // blendweight
     vb.data[j].wv[4] = 0.0f;
     vb.data[j].wv[5] = 0.0f;
     vb.data[j].wv[6] = 0.0f;
     vb.data[j].wv[7] = 0.0f; 
    }

 return true;
}

bool namco_fvf_47YY(AMC_VTXBUFFER& vb, binary_stream& bs, uint32 offset)
{
 // optional flags
 vb.flags |= AMC_VERTEX_WEIGHTS;

 // seek vertex data
 bs.seek(offset);
 if(bs.fail()) return error("Stream seek failure.", __LINE__);

 // read vertices
 for(uint32 j = 0; j < vb.elem; j++)
    {
     vb.data[j].vx = bs.BE_read_real32(); // position
     vb.data[j].vy = bs.BE_read_real32(); // position
     vb.data[j].vz = bs.BE_read_real32(); // position
     vb.data[j].nx = bs.BE_read_real16(); // normal
     vb.data[j].ny = bs.BE_read_real16(); // normal
     vb.data[j].nz = bs.BE_read_real16(); // normal
     vb.data[j].nw = bs.BE_read_real16(); // normal
     bs.BE_read_real16(); // unknown #1
     bs.BE_read_real16(); // unknown #1 
     bs.BE_read_real16(); // unknown #1 
     bs.BE_read_real16(); // unknown #1 
     bs.BE_read_real16(); // unknown #2
     bs.BE_read_real16(); // unknown #2 
     bs.BE_read_real16(); // unknown #2 
     bs.BE_read_real16(); // unknown #2
     vb.data[j].wi[0] = bs.BE_read_uint08(); // blendindice
     vb.data[j].wi[1] = bs.BE_read_uint08(); // blendindice
     vb.data[j].wi[2] = bs.BE_read_uint08(); // blendindice
     vb.data[j].wi[3] = bs.BE_read_uint08(); // blendindice
     vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[j].wv[0] = bs.BE_read_uint08()/255.0f; // blendweight
     vb.data[j].wv[1] = bs.BE_read_uint08()/255.0f; // blendweight
     vb.data[j].wv[2] = bs.BE_read_uint08()/255.0f; // blendweight
     vb.data[j].wv[3] = bs.BE_read_uint08()/255.0f; // blendweight
     vb.data[j].wv[4] = 0.0f;
     vb.data[j].wv[5] = 0.0f;
     vb.data[j].wv[6] = 0.0f;
     vb.data[j].wv[7] = 0.0f; 
    }

 return true;
}

//
// VERTEX FORMATS
// PART 2
//

bool namco_fvf_XX10(AMC_VTXBUFFER& vb, binary_stream& bs, uint32 offset)
{
 // number of channels
 vb.uvchan = 1;
 vb.colors = 0;
 vb.uvtype[0] = AMC_DIFFUSE_MAP;

 // seek vertex data
 bs.seek(offset);
 if(bs.fail()) return error("Stream seek failure.", __LINE__);

 // read UV map
 for(uint32 j = 0; j < vb.elem; j++) {
     vb.data[j].uv[0][0] = bs.BE_read_real16();
     vb.data[j].uv[0][1] = bs.BE_read_real16();
    }

 return true;
}

bool namco_fvf_XX12(AMC_VTXBUFFER& vb, binary_stream& bs, uint32 offset)
{
 // number of channels
 vb.uvchan = 1;
 vb.colors = 1;
 vb.uvtype[0] = AMC_DIFFUSE_MAP;

 // seek vertex data
 bs.seek(offset);
 if(bs.fail()) return error("Stream seek failure.", __LINE__);

 // read color + UV maps
 for(uint32 j = 0; j < vb.elem; j++) {
     vb.data[j].color[0][0] = (bs.BE_read_uint08()/255.0f);
     vb.data[j].color[0][1] = (bs.BE_read_uint08()/255.0f);
     vb.data[j].color[0][2] = (bs.BE_read_uint08()/255.0f);
     vb.data[j].color[0][3] = (bs.BE_read_uint08()/255.0f);
     vb.data[j].uv[0][0] = bs.BE_read_real16();
     vb.data[j].uv[0][1] = bs.BE_read_real16();
    }

 return true;
}

bool namco_fvf_XX22(AMC_VTXBUFFER& vb, binary_stream& bs, uint32 offset)
{
 // number of channels
 vb.uvchan = 2;
 vb.colors = 1;
 vb.uvtype[0] = AMC_DIFFUSE_MAP;
 vb.uvtype[1] = AMC_DIFFUSE_MAP;

 // seek vertex data
 bs.seek(offset);
 if(bs.fail()) return error("Stream seek failure.", __LINE__);

 // read color + UV maps
 for(uint32 j = 0; j < vb.elem; j++) {
     vb.data[j].color[0][0] = (bs.BE_read_uint08()/255.0f);
     vb.data[j].color[0][1] = (bs.BE_read_uint08()/255.0f);
     vb.data[j].color[0][2] = (bs.BE_read_uint08()/255.0f);
     vb.data[j].color[0][3] = (bs.BE_read_uint08()/255.0f);
     vb.data[j].uv[0][0] = bs.BE_read_real16();
     vb.data[j].uv[0][1] = bs.BE_read_real16();
     vb.data[j].uv[1][0] = bs.BE_read_real16();
     vb.data[j].uv[1][1] = bs.BE_read_real16();
    }

 return true;
}

bool namco_fvf_XX32(AMC_VTXBUFFER& vb, binary_stream& bs, uint32 offset)
{
 // number of channels
 vb.uvchan = 3;
 vb.colors = 1;
 vb.uvtype[0] = AMC_DIFFUSE_MAP;
 vb.uvtype[1] = AMC_DIFFUSE_MAP;
 vb.uvtype[2] = AMC_DIFFUSE_MAP;

 // seek vertex data
 bs.seek(offset);
 if(bs.fail()) return error("Stream seek failure.", __LINE__);

 // read color + UV maps
 for(uint32 j = 0; j < vb.elem; j++) {
     vb.data[j].color[0][0] = (bs.BE_read_uint08()/255.0f);
     vb.data[j].color[0][1] = (bs.BE_read_uint08()/255.0f);
     vb.data[j].color[0][2] = (bs.BE_read_uint08()/255.0f);
     vb.data[j].color[0][3] = (bs.BE_read_uint08()/255.0f);
     vb.data[j].uv[0][0] = bs.BE_read_real16();
     vb.data[j].uv[0][1] = bs.BE_read_real16();
     vb.data[j].uv[1][0] = bs.BE_read_real16();
     vb.data[j].uv[1][1] = bs.BE_read_real16();
     vb.data[j].uv[2][0] = bs.BE_read_real16();
     vb.data[j].uv[2][1] = bs.BE_read_real16();
    }

 return true;
}

//
// VERTEX FORMATS
// COMBINED
//

bool namco_fvf_1612(AMC_VTXBUFFER& vb, binary_stream& vs1, binary_stream& vs2, uint32 vo1, uint32 vo2)
{
 // TYPE 0x1612
 // EXAMPLE: [WiiU] Super Smash Bros\content\output\fighter\szerosuit\model\whip\c00\model.nud
 if(!namco_fvf_XX12(vb, vs1, vo1)) return false;
 if(!namco_fvf_16YY(vb, vs2, vo2)) return false;
 return true;
}

bool namco_fvf_4610(AMC_VTXBUFFER& vb, binary_stream& vs1, binary_stream& vs2, uint32 vo1, uint32 vo2)
{
 // TYPE 0x4610
 // EXAMPLE: [WiiU] Super Smash Bros\content\output\fighter\diddy\model\dkbarrel\c00\model.nud
 if(!namco_fvf_XX10(vb, vs1, vo1)) return false;
 if(!namco_fvf_46YY(vb, vs2, vo2)) return false;
 return true;
}

bool namco_fvf_4612(AMC_VTXBUFFER& vb, binary_stream& vs1, binary_stream& vs2, uint32 vo1, uint32 vo2)
{
 // TYPE 0x4612
 // EXAMPLE: [WiiU] Super Smash Bros\content\output\ui\model\diffi\dyr_background_set\model.nud
 if(!namco_fvf_XX12(vb, vs1, vo1)) return false;
 if(!namco_fvf_46YY(vb, vs2, vo2)) return false;
 return true;
}

bool namco_fvf_4622(AMC_VTXBUFFER& vb, binary_stream& vs1, binary_stream& vs2, uint32 vo1, uint32 vo2)
{
 // TYPE 0x4622
 // EXAMPLE: [WiiU] Super Smash Bros\content\output\fighter\duckhunt\model\body\c00\model.nud
 if(!namco_fvf_XX22(vb, vs1, vo1)) return false;
 if(!namco_fvf_46YY(vb, vs2, vo2)) return false;
 return true;
}

bool namco_fvf_4632(AMC_VTXBUFFER& vb, binary_stream& vs1, binary_stream& vs2, uint32 vo1, uint32 vo2)
{
 // TYPE 0x4632
 // EXAMPLE: [WiiU] Super Smash Bros\content\output\stage\end\XNorfair\model\dys_magma_set\model.nud
 if(!namco_fvf_XX32(vb, vs1, vo1)) return false;
 if(!namco_fvf_46YY(vb, vs2, vo2)) return false;
 return true;
}

bool namco_fvf_4710(AMC_VTXBUFFER& vb, binary_stream& vs1, binary_stream& vs2, uint32 vo1, uint32 vo2)
{
 // TYPE 0x4710
 // EXAMPLE: [WiiU] Super Smash Bros\content\output\fighter\diddy\model\peanuts\c00\model.nud
 if(!namco_fvf_XX10(vb, vs1, vo1)) return false;
 if(!namco_fvf_47YY(vb, vs2, vo2)) return false;
 return true;
}

bool namco_fvf_4712(AMC_VTXBUFFER& vb, binary_stream& vs1, binary_stream& vs2, uint32 vo1, uint32 vo2)
{
 // TYPE 0x4712
 // EXAMPLE: [WiiU] Super Smash Bros\content\output\ui\model\chara\chara_hand1\model.nud
 if(!namco_fvf_XX12(vb, vs1, vo1)) return false;
 if(!namco_fvf_47YY(vb, vs2, vo2)) return false;
 return true;
}

bool namco_fvf_4722(AMC_VTXBUFFER& vb, binary_stream& vs1, binary_stream& vs2, uint32 vo1, uint32 vo2)
{
 // TYPE 0x4722
 // EXAMPLE: [WiiU] Super Smash Bros\content\output\fighter\purin\model\body\c00\model.nud
 if(!namco_fvf_XX22(vb, vs1, vo1)) return false;
 if(!namco_fvf_47YY(vb, vs2, vo2)) return false;
 return true;
}