// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#include <iostream>
#include <unordered_map>
#include "table.h"
#include "TableEntry.h"
#include <cstring>
#include <cstdlib>
using namespace std;

int main(int argc, char* argv[]){
    ios_base::sync_with_stdio(false); // you should already have this
    cin >> std::boolalpha;  // add these two lines
    cout << std::boolalpha; // add these two lines
    
    unordered_map <string, table> database;
    string cmd;
    bool quiet = false;
    for(int i=0;i<argc;i++){
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 ){
            cout << "helpful message" << endl;
            return 0;
        }
        if (strcmp(argv[i], "-q") == 0 || strcmp(argv[i], "--quiet") == 0){
            quiet = true;
        }
    }
    
  
    //int times = 0;
    do{
        cout << "% ";
        
        cin >> cmd;
        if (cmd == "CREATE"){
            cin >> cmd;
            if (database.find(cmd) != database.end()){
                cout << "Error during CREATE: Cannot create already existing table " << cmd << endl; 
                getline (cin, cmd);
            }
            else{
                table test;
                test.CREATE(cmd);
                database.insert({cmd, test});
            }    

        }
        
        else if (cmd == "REMOVE"){
            cin >> cmd;
            if (database.find(cmd) == database.end()){      // Didn't find the table 
                cout << "Error during REMOVE: " << cmd << " does not name a table in the database" << endl;
            }
            else{
                database.erase(cmd);
                cout << "Table " << cmd << " deleted"<<endl;
            }
        }

        else if (cmd[0] == '#'){
            getline(cin, cmd);
            //cout << cmd << endl;
        }

        else if (cmd == "INSERT"){
            cin >> cmd;
            if (cmd != "INTO") continue;
            else{
                cin >> cmd;
                string table_name = cmd;
                if (database.find(cmd) == database.end()) { // Didn't find the table
                    cout << "Error during INSERT: " << cmd << " does not name a table in the database" << endl;
                    getline(cin, cmd);
                }
                else{
                    cin >> cmd;
                    int N = stoi(cmd);
                    cin >> cmd; // skip "ROWS"
                    database[table_name].INSERT(N);    
                }
            }
        }
        

        else if (cmd == "PRINT"){
            cin >> cmd; // FROM
            string table_name;
            cin >> table_name;
            if (database.find(table_name) == database.end()){
                cout << "Error during PRINT: " << table_name << " does not name a table in the database" << endl;
                getline(cin, cmd);
            }
            else {
                int N;
                cin >> cmd;
                N = stoi(cmd);
                database[table_name].PRINT(N, quiet);
            }
        }
            
        else if (cmd == "DELETE"){
            cin >> cmd; // FROM
            string table_name;
            cin >> table_name;
            cin >> cmd; // WHERE
            if (database.find(table_name) == database.end()){
                cout << "Error during DELETE: " << cmd << " does not name a table in the database" << endl;
                getline(cin, cmd);
            }
            else{
                database[table_name].DELETE();
            }
        }
            
        else if (cmd == "JOIN"){
            string tablename1;
            string tablename2;
            cin >> tablename1;
            cin >> cmd; // AND
            cin >> tablename2;
            cin >> cmd; // WHERE
            if (database.find(tablename1) == database.end()){
                cout << "Error during JOIN: " << tablename1 << " does not name a table in the database\n";
                getline(cin, cmd);
                continue;
            }
            if (database.find(tablename2) == database.end()){
                cout << "Error during JOIN: " << tablename2 << " does not name a table in the database\n";
                getline(cin, cmd);
                continue;
            }

            string colname1;
            string colname2;
            int N;
            cin >> colname1 >> cmd >> colname2 >> cmd >> cmd >> N;
            if (find(database[tablename1].column_names.begin(), database[tablename1].column_names.end(), colname1) == 
            database[tablename1].column_names.end()){
                cout << "Error during JOIN: " << colname1 << " does not name a column in "<< tablename1 <<"\n";
                getline(cin, cmd);
                continue;
            }
            if (find(database[tablename2].column_names.begin(), database[tablename2].column_names.end(), colname2) == 
            database[tablename2].column_names.end()){
                cout << "Error during JOIN: " << colname2 << " does not name a column in "<< tablename2 << "\n";
                getline(cin, cmd);
                continue;
            }       



            // Get all the columns that need to be printed
            vector <int> table1_col;
            vector <int> table2_col;
            vector <int> one_or_two;  // record which table shou

            
            for (int i=0;i<N;i++){
                string temp_colname;
                int which_table;
                cin >> temp_colname >> which_table;
                if (!quiet) cout << temp_colname << " ";
                int this_index;             // The index correspond to the colname
                one_or_two.push_back(which_table);
                
                if (which_table == 1){
                    this_index = int(find(database[tablename1].column_names.begin(), database[tablename1].column_names.end(), temp_colname) - 
                    database[tablename1].column_names.begin());
                    table1_col.push_back(this_index);
                }

                else if (which_table == 2){
                    this_index = int(find(database[tablename2].column_names.begin(), database[tablename2].column_names.end(), temp_colname) - 
                    database[tablename2].column_names.begin());
                    table2_col.push_back(this_index);                 
                }
            } // read all the col that needs to be printed

            if(!quiet) cout << "\n";

            // find the two cols that need to be compared
            int check_col1 = int(find(database[tablename1].column_names.begin(), database[tablename1].column_names.end(), colname1) - database[tablename1].column_names.begin());
            int check_col2 = int(find(database[tablename2].column_names.begin(), database[tablename2].column_names.end(), colname2) - database[tablename2].column_names.begin());
            int times = 0;
            



            if (database[tablename2].have_index == 2 && database[tablename2].the_index == check_col2) { // hash
                for (int i = 0; i < int(database[tablename1].values.size()); i++){ // iterate through lines in table1, i    
                    auto ittt = database[tablename2].hash.find(database[tablename1].values[i][check_col1]) ;
                    if (ittt != database[tablename2].hash.end()){
                        // if  there are matching rows in table2        
                        if (!quiet) {              
                            for (int j = 0; j < int(ittt->second.size()); j++){  // j is the line in table2
                                // for all the rows that have the same corresponding val in table2
                                int t1 = 0;
                                int t2 = 0;
                                
                                for (auto itr = one_or_two.begin(); itr != one_or_two.end(); itr++){    // print the corresponding col
                                    if (*itr == 1){
                                        cout << database[tablename1].values[i][table1_col[t1]] << " ";
                                        t1 ++;
                                    }
                                    else {
                                        cout << database[tablename2].values[ittt->second[j]] [table2_col[t2]] << " ";
                                        t2 ++;
                                    }
                                } // finish a line
                                cout << "\n";
                                times ++; 
                            } // finish a key
                        }
                        else times += int(ittt->second.size());
                    } // if matching
                } // finish a line in table 1
            }

            else {              // If there is neither bst nor hash
                unordered_map <TableEntry, vector<int>> table2; // store all the rows in table2 into this hash_map
                vector <int> index;
                for (int i=0;i< int(database[tablename2].values.size()); i++){

                    table2[database[tablename2].values[i][check_col2]].reserve(table2[database[tablename2].values[i][check_col2]].size()+1);
                    table2[database[tablename2].values[i][check_col2]].emplace_back(i);
                }



                for (int i = 0; i < int(database[tablename1].values.size()); i++){ // iterate through lines in table1, i
                    auto ittt = table2.find(database[tablename1].values[i][check_col1]);
                    if (ittt != table2.end()){ 
                        // if we there are matching rows in table2        
                        if (!quiet){
                            int j_size = int(ittt->second.size());
                            for (int j = 0; j < j_size; j++){  // j is the line in table2
                                // for all the rows that have the same corresponding val in table2
                                int t1 = 0;
                                int t2 = 0; 
                                for (auto itr = one_or_two.begin(); itr != one_or_two.end(); itr++){    // print the corresponding col
                                    if (*itr == 1){
                                        cout << database[tablename1].values[i][table1_col[t1]] << " ";
                                        t1 ++;
                                    }
                                    else {
                                        cout << database[tablename2].values[ittt->second[j] ] [table2_col[t2]] << " ";
                                        t2 ++;
                                    }
                                } // finish a line
                                cout << "\n";
                                times ++;
                            } // finish a key
                        }
                        else times += int(ittt->second.size());
                    } // if matching
                } // finish a line in table 1
            }
            
            cout << "Printed " << times <<" rows from joining " << tablename1 << " to " << tablename2 << endl;
        }



        else if (cmd == "GENERATE"){
            cin >> cmd; // FOR
            string tablename;
            cin >> tablename;
            string indextype;
            cin >> indextype;
            cin >> cmd; // INDEX
            cin >> cmd; // ON
            string colname;
            cin >> colname;
            if (database.find(tablename) == database.end()){
                cout << "Error during INSERT: " << cmd << " does not name a table in the database" << endl;
                getline(cin, cmd);
            }
            else{
                if (indextype == "hash") database[tablename].GEN_hash(colname);
                else if (indextype == "bst") database[tablename].GEN_bst(colname);
            }

            cout << "Created " << indextype << " index for table " << tablename << " on column " << colname << endl;
        }

        else if (cmd == "QUIT"){
            break;
        }

        else {
            cout << "Error: unrecognized command" << endl;
            getline(cin, cmd);
        }


    }while (cmd != "QUIT");
    cout << "Thanks for being silly!" << endl;
    
    return 0;
}