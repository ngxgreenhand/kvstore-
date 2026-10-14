#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
using namespace std;

class KVStore {
private:
    string filename;
    unordered_map<string, long> index;
    // 内存中的索引表：key -> value在文件中的字节偏移量

public:
    KVStore(string fname) : filename(fname) {
        // 启动时自动从文件恢复索引
        loadIndex();
    }

    void pian(const string& key)
    {
        cout << "偏移量表" << index[key] << endl;
        ifstream in(filename);
        in.seekg(index[key]); // 跳到记录的偏移量位置)
        string value;
        in >> value;
        cout << "偏移量表对应的值" << value << endl;

    }

    void printIndex() {
        cout << "Current index table:" << endl;
        for (auto& kv : index) //遍历内存中的索引表
        {
            cout << kv.first << " -> " << get(kv.first) << endl;
        }
    }


    void del(const string& key) {
        if (index.find(key) != index.end()) {
            // 1. 内存里删掉索引
            index.erase(key);

            // 2. 硬盘里追加一条墓碑记录 (append "DELETED")
            ofstream out(filename, ios::app);
            out << key << " DELETED" << endl;
            out.close();

            cout << "Deleted (Tombstone written)." << endl;
        }
        else {
            cout << "Key not found." << endl;
        }
    }

    // 核心逻辑：从磁盘读取文件，重建内存哈希表
    void loadIndex() {
        ifstream in(filename);
        if (!in.is_open()) return; // 如果文件不存在，直接返回

        string key;
        while (in >> key) {
            in.get(); // 跳过 key 后面的空格
            long pos = in.tellg(); // 记录当前文件指针位置（这就是 value 的起始偏移量！）


            string value;
            getline(in, value); // 读取剩下的整行（支持 value 里包含空格）
            if (value == "DELETED") {
                // 如果 value 是墓碑标记，说明这个 key 已经被删除了
                index.erase(key); // 从内存索引里删掉
                continue; // 跳过这条记录
            }
            else {
                index[key] = pos; // 否则，把偏移量记在内存里
            }
        }
        in.close();
    }

    // 写入数据，并记录位置
    void put(const string& key, const string& value) {
        ofstream out(filename, ios::app); // 追加模式打开
        if (!out.is_open()) return;

        out << key << " "; // 先写 key 和空格
        long pos = out.tellp(); // 记录当前指针位置（这就是 value 在文件里的偏移量）
        out << value << endl; // 写入 value 并换行

        index[key] = pos; // 把偏移量记在内存里
        out.close();
    }

    // 读取数据，从偏移量跳过去读
    string get(const string& key) {
        if (index.find(key) == index.end()) {
            return "(not found)";
        }

        ifstream in(filename);
        if (!in.is_open()) return "(file error)";

        in.seekg(index[key]); // 跳到记录的偏移量位置
        string value;
        in >> value;
        in.close();
        return value;
    }

    void count() {
        cout << "当前索引表中键值对数量: " << index.size() << endl;
    }
    void compact()
    {
        ofstream newfile("data_new.txt"); // 新文件，清空内容
        if (!newfile.is_open()) {
            cout << "Failed to open new file for compaction." << endl;
            return;
        }
        ifstream oldfile("data.txt");
        unordered_map<string, long> newIndex;
		for (auto& kv : index)
		{
			string key = kv.first;
			long pos = kv.second;
			oldfile.seekg(pos);
			string value;
			oldfile >> value;
			// 写入新文件
            newfile << key << " ";
			long newpos = newfile.tellp(); // 获取新文件中 value 的偏移量
			newfile << value << endl;
            newIndex[key] = newpos;// 更新新索引表的偏移量
		}
        newfile.close();
        oldfile.close();

        if (remove("data.txt") != 0) {
            cout << "Error deleting old data file." << endl;
            return;
        }

        if (rename("data_new.txt", "data.txt") != 0) {
            cout << "Error renaming compacted file." << endl;
            return;
        }

        index = newIndex; // Update the in-memory index to the new one
        cout << "Compaction completed successfully." << endl;
    }

};


int main() {
    KVStore store("data.txt");

    string cmd, key, value;
    cout << "KVStore Ready. Input commands: put/get/del/print/count/compact/pian/compact/exit" << endl;

    while (true) {
        cout << "> ";
        cin >> cmd;

        if (cmd == "put") {
            cin >> key >> value;
            store.put(key, value);
            cout << "OK" << endl;
        }
        else if (cmd == "get") {
            cin >> key;
            cout << store.get(key) << endl;
        }
        else if (cmd == "exit") {
            break;
        }
        else if (cmd == "del") {
            cin >> key;
            store.del(key);
        }
        else if (cmd == "print")
        {
            store.printIndex();
        }
        else if (cmd == "count") {
            store.count();
        }
        else if (cmd == "pian")
        {
            cin >> key;
            store.pian(key);
        }
        else if (cmd == "compact")
        {
            store.compact();
        }
        else {
            cout << "Unknown command" << endl;
        }
    }
    return 0;
}
