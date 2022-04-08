#include <vector>
#include <string>
//bool list_directory(const char *dirname);

enum class  filetype : short{file,folder,link,other};
struct DirCont{std::string FullPath;filetype type;};

struct ReadDir{
    bool is_good=true;
    std::string PathToDir;
    std::vector<DirCont> result;
    ReadDir(const std::string& dirname);
    ReadDir(std::string&& dirname);
    std::vector<const char*>& GetFilePath();
    std::vector<const char*>& GetFolderPath();

    private: 
    std::vector<const char*> _FilesPath;
    std::vector<const char*> _FoldersPath;
    void init();
};