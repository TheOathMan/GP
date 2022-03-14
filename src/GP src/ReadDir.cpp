
#include "ReadDir.h"
#include "../NMC/dirent/dirent.h"

void ReadDir::init(){
	DIR *dir = opendir(PathToDir.c_str());
	if (!dir) {
		fprintf(stderr,"Cannot open %s (%s)\n", PathToDir.c_str(), strerror(errno));
        is_good = false;
        return;
	}
	dirent *ent;
	while ((ent = readdir(dir)) != NULL) {
        std::string temps =  PathToDir + "/" + ent->d_name;
		switch (ent->d_type) {
		case DT_REG:
             result.push_back({temps ,filetype::file});
			break;
		case DT_DIR:
            PathToDir.append("/");
            result.push_back({temps,filetype::folder});
			break;

		case DT_LNK:
            PathToDir.append("@");
            result.push_back({temps,filetype::link});
			break;

		default:
            PathToDir.append("*");
            result.push_back({temps,filetype::other});
		}
	}

	closedir(dir);  

}

ReadDir::ReadDir(const std::string& dirname):PathToDir(dirname){init();}
ReadDir::ReadDir(std::string&& dirname):PathToDir(std::move(dirname)){ printf("moved"); init();}

std::vector<const char*>& ReadDir::GetFilePath(){
    if(_FilesPath.empty()){
        for(auto&& i : result)
            if(i.type == filetype::file) _FilesPath.push_back(i.FullPath.c_str());
    }
    return _FilesPath;
}
std::vector<const char*>& ReadDir::GetFolderPath(){
    if(_FoldersPath.empty()){
        for(auto&& i : result)
        if(i.type == filetype::folder) _FoldersPath.push_back(i.FullPath.c_str());
    }
    return _FoldersPath;
}
