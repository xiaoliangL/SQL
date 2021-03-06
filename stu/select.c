#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include "cgic.h"

char * headname = "head.html";
char * footname = "footer.html";


int cgiMain()
{
	FILE * fd;

/*	fprintf(cgiOut, "<head><meta charset=\"utf-8\"/><title>查询结果</title>\
			<style>table {width:400px; margin: 50px auto; border: 1px solid gray; border-collapse: collapse; border-spacing: none; text-align:center;}\
			tr,td,th{border: 1px solid gray;}\
			</style>\
			</head>");*/


	char sno[12] = "\0";
  char school_id[10] = "\0";
  char cno[10] = "\0";
	int status = 0;

	char ch;
	fprintf(cgiOut, "Content-type:text/html;charset=utf-8\n\n");
	if(!(fd = fopen(headname, "r"))){
		fprintf(cgiOut, "Cannot open file, %s\n", headname);
		return -1;
	}
	ch = fgetc(fd);

	while(ch != EOF){
		fprintf(cgiOut, "%c", ch);
		ch = fgetc(fd);
	}
fclose(fd);


	status = cgiFormString("sno",  sno, 12);
	if (status != cgiFormSuccess)
	{
		fprintf(cgiOut, "get sno error!\n");
		return 1;
	}
  status = cgiFormString("school_id",  school_id, 10);
  if (status != cgiFormSuccess)
  {
    fprintf(cgiOut, "get school_id error!\n");
    return 1;
  }
  status = cgiFormString("cno",  cno, 10);
  if (status != cgiFormSuccess)
  {
    fprintf(cgiOut, "get cno error!\n");
    return 1;
  }

	int ret;
	MYSQL *db;
	char sql[128] = "\0";
  sprintf(sql, "select information.sno,school.school_id,course.cno,score from information,school,course,score where information.sno=score.sno and school.school_id=score.school_id and course.cno=score.cno and score.sno= %d and score.school_id=%d and score.cno=%d",atoi(sno),atoi(school_id),atoi(cno));

	/*if (sno[0] == '*')
	{
		sprintf(sql, "select * from information");
	}
	else
	{
		sprintf(sql, "select * from information where sno = %d ", atoi(sno));
	}*/


	//初始化
	db = mysql_init(NULL);
	mysql_options(db, MYSQL_SET_CHARSET_NAME, "utf8");
	if (db == NULL)
	{
		fprintf(cgiOut,"mysql_init fail:%s\n", mysql_error(db));
		return -1;
	}

	//连接数据库
	db = mysql_real_connect(db, "127.0.0.1", "root", "123456", "stu",  3306, NULL, 0);
	if (db == NULL)
	{
		fprintf(cgiOut,"mysql_real_connect fail:%s\n", mysql_error(db));
		mysql_close(db);
		return -1;
	}


	if ((ret = mysql_real_query(db, sql, strlen(sql) + 1)) != 0)
	{
		fprintf(cgiOut,"mysql_real_query fail:%s\n", mysql_error(db));
		mysql_close(db);
		return -1;
	}
	MYSQL_RES *res;
	res = mysql_store_result(db);
	if (res == NULL)
	{
		fprintf(cgiOut,"mysql_store_result fail:%s\n", mysql_error(db));
		return -1;
	}

	fprintf(cgiOut, "<div class=\"container\"> <h1 class=\"text-center\">查询结果</h1>");

	fprintf(cgiOut,"<table class=\"table table-striped table-bordered\"><tr>");
	int i = 0;


	unsigned int fields;
	fields = mysql_num_fields(res);

	MYSQL_FIELD *mysql_filed;
	mysql_filed = mysql_fetch_fields(res);
	for (i = 0; i < fields ; i++)
	{
		fprintf(cgiOut, "<th>%s</th>", mysql_filed[i].name);
	}
	fprintf(cgiOut,"</tr>");

	//访问每一条记录的值
	MYSQL_ROW  row;
	unsigned long  *len;

	while ((row = mysql_fetch_row(res)) != NULL)
	{
		fprintf(cgiOut,"<tr>");
		len = mysql_fetch_lengths(res);
		for (i = 0; i < fields ; i++)
		{
			fprintf(cgiOut,"<td>%.*s</td>", (int)len[i], row[i]);
		}
		fprintf(cgiOut,"</tr>");
	}
	fprintf(cgiOut,"</table></div>");



	mysql_close(db);
	return 0;
}
