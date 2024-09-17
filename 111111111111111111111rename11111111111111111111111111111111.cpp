#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
#include <io.h>
#include <windows.h>
#include <vector>
using namespace std;
string* tmpfilelist;
intptr_t fn;


int renameAll(_finddata_t& fileinfo,string path,string rep,string torep,intptr_t& countNum)
{
    string fname = fileinfo.name;
    string::size_type st;
    st = fname.find(torep);
    if (st == string::npos)
    {
        return 1;
    }
    string new_path_name, new_name;//改名后，带path
    new_path_name = path;
    new_name = fname.substr(0, st);
    new_name += rep;
    new_name += fname.substr(st + torep.size());
    new_path_name += new_name;
    bool FileHasBeenRenamed = false;
    for (intptr_t i1 = 0; i1 < fn + 1; i1++)//读取已经重命名的文件列表
    {
        if (tmpfilelist[i1] == fileinfo.name)
        {
            FileHasBeenRenamed = true;
            break;
        }
    }
    if (FileHasBeenRenamed)
        return 1;
    cout << "rename: \t" << fileinfo.name << "\t >>> \t" << new_name << endl;
    int RenameErr = rename((path + fileinfo.name).c_str(), new_path_name.c_str());
    if (RenameErr != 0)
    {
        cout << "!!!!!!请注意，这个文件重命名失败!!!!!!\n" << "错误码：" << RenameErr << endl;
        return 1;
    }
    if (countNum < fn && countNum >= 0)
    {
        tmpfilelist[countNum] = new_name;
        countNum += 1;
    }

}
intptr_t getFileNum(string path)
{
    struct _finddata_t fileinfo;
    if (path.back() != '\\' && path.back() != '/')
    {
        path += "\\";
    }
    std::string current = path + "*.*";
    intptr_t handle = _findfirst(current.c_str(), &fileinfo);
    intptr_t fileNum = 0;
    if (handle == -1)
        return 0;
    // 是否是文件夹，并且名称不为或".." 

    while (!_findnext(handle, &fileinfo))
    {
        if (0 == strcmp(fileinfo.name, "..") || 0 == strcmp(fileinfo.name, ".") || (fileinfo.attrib & _A_SUBDIR)) {

            continue;
        }
        else
        {
            fileNum++;
        }
    }
    _findclose(handle);
    return fileNum;
}
intptr_t renameFiles(string path,string torep,string rep)
{   //需要用到<io.h>头文件
    if (tmpfilelist != NULL)
    {
        delete[]tmpfilelist;
    }
    fn = getFileNum(path);
    tmpfilelist = new string[fn+1];//储存已经重命名的文件名字
    struct _finddata_t fileinfo;
    if (path.back() != '\\' && path.back() != '/')
    {
        path += "\\";
    }
    std::string current = path + "*.*";
    intptr_t handle = _findfirst(current.c_str(), &fileinfo);
    intptr_t fileNum = 0;
    intptr_t folderNum = 0;
    if (handle == -1)
        return 0;
    // 是否是文件夹，并且名称不为或".." 
    intptr_t countNum = 0;

    while (!_findnext(handle, &fileinfo))
    {
        if (0 == strcmp(fileinfo.name, "..") || 0 == strcmp(fileinfo.name, ".")) 
        {
            continue;
        }
        else if (fileinfo.attrib & _A_SUBDIR)
        {
            ++folderNum;
            continue;
        }
        else
        {
            ++fileNum;
            if (renameAll(fileinfo, path, rep, torep, countNum))
                continue;

        }
    }
    _findclose(handle);
    return fileNum;
}

int main()
{
    string lastPath;
    bool HadUsed = false;
    while (true)
    {
        string path;  // 文件夹路径
        cout << "一键替换文件夹内文件或文件夹名字的部分字符(串)，但只会替换文件名开头找到的第一个对应文字" << endl
            << "请输入文件路径，末尾可以有/或者\\，Ctrl+C退出：";
        if (HadUsed)
        {
            cout << "(直接回车可以使用上次的地址!)";
        }
        //cin >> path;
        getline(cin, path);
        cin.clear();
        while (path == "" && lastPath=="")
        {
            cout << "文件夹路径不能为空！:(" << endl << "请再次输入:";
            getline(cin, path);
            //cin >> torep;
            cin.clear();
        }
        if(path != "")
            lastPath = path;
        else {
            path = lastPath;
        }
        cout << "请输入需要被替换掉的文字(不能为空！):";
        string torep;
        getline(cin, torep);
        //cin >> torep;
        cin.clear();
        while (torep == "")
        {
            cout << "需要被替换掉的文字不能为空！:(" << endl << "请再次输入:";
            getline(cin, torep);
            //cin >> torep;
            cin.clear();
        } 
        cout << "替换成:";
        string rep;
        getline(cin, rep);
        //cin >> rep;
        cin.clear();
        renameFiles(path, torep, rep);
        HadUsed = true;
        system("pause");
    }
    return 0;
}