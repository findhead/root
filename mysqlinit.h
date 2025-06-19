void mysqinit() {
    MYSQL * con;
    MYSQL_RES *res;
    
    con = mysql_init(NULL);
    if(con == NULL) {
        cout << "mysqlinit errno" << endl;
        exit(1);
    }
    
    if(!mysql_real_connect(con, "localhost", "root", "123456", "web", 0, NULL, 0)) {
        cout << "mysql connect errno" << endl;
        exit(1);
    }
    
    const char * query = "select * from message;";
    mysql_query(con, query);
    res = mysql_store_result(con);
    
    int num_fields = mysql_num_fields(res);
    MYSQL_FIELD * field = mysql_fetch_field(res);
    
    for(int i = 0; i < num_fields; ++i) {
        printf("%s\r", field[i].name);
    }
    cout << "\n";
    
    MYSQL_ROW row;
    while((row = mysql_fetch_row(res))) {
        for(int i = 0; i < num_fields; ++i) {
            printf("%s\r", row[i] ? row[i] : "NULL");
        }
        cout << endl;
    }
    
    mysql_free_result(res);
    mysql_close(con);
}
