#include <iostream>
//#include "SqlData.h"
#include <string>
//#pragma comment(lib, "ZDataDll.lib")

#include "src/Data.h"
#include "src/Element.h"
#include <time.h>

#include "Socket.h"

using LL::Data;
using LL::Element;
std::string _db = "user=root;password=123456;db=mytest;compress=true;auto-reconnect=true;port=3306;host=localhost";

int main()
{
	//Data* data = new Data(_db);
	//--------------test db--------------//
//  	Data* data = new Data();
//  	data->connect(_db);
//  	Element e(1,2,3,4,"111", "222", "333", "444", "555", "666"); 
//  	data->commit(e);

	//---------------test socket--------------//
	CSocket s(stat_service, 9000);
	s.Start();
	

// 	time_t c_start,t_start, c_end,t_end;
// 	c_start = clock();
// 	t_start = time(NULL) ;
// 	system("pause") ;
// 	c_end = clock();
// 	t_end = time(NULL) ;

// 	CSqlData *sd = new CSqlData("");
// 	sd->WriteData("");

	while (true)
	{
		Poco::Thread::sleep(200);
	}

	system("pause");
	return 0;
}