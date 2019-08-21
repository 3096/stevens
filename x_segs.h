#ifndef __XENTAX_SEGS_H
#define __XENTAX_SEGS_H

bool extractSEGS(LPCTSTR ifname, LPCTSTR ofname);
bool extractSEGS(std::ifstream& ifile, std::ofstream& ofile);
bool extractSEGS(std::ifstream& ifile, uint32 offset, std::ofstream& ofile);

#endif
