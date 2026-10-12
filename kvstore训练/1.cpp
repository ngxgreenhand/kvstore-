//#include<iostream>
//#include<fstream>
//#include<unordered_map>
//using namespace std;
//
//int main()
//{
//	unordered_map<string, string>kvstore;
//	kvstore.insert(pair<string, string>("name", "张三"));
//	cout << "name: " << kvstore["name"] << endl;
//	ofstream ofs;
//	ofs.open("data.bin", ios::out | ios::binary);
//	if (!ofs.is_open()) {
//		cout << "文件打开失败！" << endl;
//		return -1;
//	}
//	for (auto& pair : kvstore)
//	{
//		ofs << pair.first<<" "<< pair.second << endl;
//	}
//	ofs.close();
//	// 3. 从磁盘读取
//	ifstream ifs("data.bin", ios::in | ios::binary);
//	string k, v;
//	if (ifs >> k >> v) {
//		cout << "从硬盘恢复数据: " << k << " = " << v << endl;
//	}
//	ifs.close();
//}