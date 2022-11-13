// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include "TableEntry.h"
#include <iostream>
#include <sstream>
#include <map>

using namespace std;


class table{
    public:
    int row_num;
    int col_num;
    
    string table_name;
	struct TableEntryLess {
		bool operator() (const TableEntry& lhs, const TableEntry& rhs) const
		{
			return (lhs < rhs);
		}
	};
    
    int the_index;
    int have_index;
    map <TableEntry, vector<int>, TableEntryLess> bst;
    unordered_map<TableEntry, vector<int>> hash;



    TableEntry correct_type(string val2, string type){
        if (type == "string"){
            TableEntry result(val2);
            return result;
        }
        else if (type == "bool"){
            if (val2 == "true"){
                TableEntry result(true);
                return result;
            }
            else {
                TableEntry result(false);
                return result;
            }
        }
        else if (type == "double"){
            TableEntry result(stod(val2));
            return result;
        }
        else {
            TableEntry result(stoi(val2));
            return result;
        }
    }

    bool Comp(string OP, TableEntry val1, string val2, string type){
        if (OP == "="){
            return val1 == correct_type(val2, type);
        }
        else if (OP == ">"){
            return val1 > correct_type(val2, type);
        }
        else if (OP == "<"){
            return val1 < correct_type(val2, type);
        }
        else return false;
    }



    class Compare_eq{
    private:
        TableEntry value;
        int col;
    public:
        Compare_eq(TableEntry val, int col): value(val), col(col){}
        bool operator()(vector<TableEntry>& entry) const {
            return entry[col] == value;
        }
    };

    class Compare_gt{
    private:
        TableEntry value;
        int col;
    public:
        Compare_gt(TableEntry val, int col): value(val), col(col){}
        bool operator()(vector<TableEntry>& entry) const {
            return entry[col] > value;
        }
    };

    class Compare_lt{
    private:
        TableEntry value;
        int col;
    public:
        Compare_lt(TableEntry val, int col): value(val), col(col){}
        bool operator()(vector<TableEntry>& entry) const {
            return entry[col] < value;
        }
    };



    
    vector<vector<TableEntry>> values;
    vector <string> column_type;
    vector <string> column_names;
    table(){}; //default constructor
    
    //~table();

    void CREATE(string name){
        string word;
        table_name = name;
        cin >> word;
        row_num = 0;
        col_num = stoi(word); // read the col number

        for (int i=0;i<col_num;i++){
            cin >> word;
            column_type.push_back(word);
        }
        for (int i=0;i<col_num;i++){
            cin >> word;
            column_names.push_back(word);
        }

        have_index = 0;
        the_index = 0;
        cout << "New table " << table_name << " with column(s) ";
        for (int i=0;i<col_num;i++){
            cout << column_names[i] << " ";
        }
        cout << "created\n";
    }

    void INSERT(int N){
        string cmd;
        for (int i=0;i<N;i++){
            vector <TableEntry> one_row;
            for (int j=0;j<col_num;j++){
                cin >> cmd;
                if (column_type[j] == "string"){
                    one_row.emplace_back(cmd);
                }
                else if (column_type[j] == "int"){
                    one_row.emplace_back(stoi(cmd));
                }
                else if (column_type[j] == "double"){
                    one_row.emplace_back(stod(cmd));
                }
                else if (column_type[j] == "bool") {
                    if (cmd == "true"){
                        one_row.emplace_back(true);
                    }
                    else{
                        one_row.emplace_back(false);
                    }
                } // bool
            } // for cols
            values.emplace_back(one_row); // add one row to the table
            
        } //for rows
        cout << "Added " << N << " rows to " << table_name << " from position " << row_num << " to " << row_num + N - 1<< endl;
        row_num += N;
        if (have_index == 1){ //this is a bst
            this->GEN_bst(column_names[the_index]);
        }
        else if (have_index == 2){
            this->GEN_hash(column_names[the_index]);
        }
    } // INSERT




    void PRINT(int N, bool quiet){
        string cmd;
        vector <int> index;
        // Read all the columns needed
        for (int i=0;i<N;i++){
            cin >> cmd;
            auto find_result = find(column_names.begin(), column_names.end(), cmd);
            if (find_result == column_names.end()){
                cout << "Error during PRINT: " << cmd << "does not name a column in "<< table_name << endl;
                getline(cin, cmd);
                return;
            }
            // record all the columns needed
            index.push_back( int(find_result - column_names.begin()));
        }     
        int matching_row = 0;
        string line;
        getline(cin, line);
        stringstream ss(line);
        ss >> cmd;

        // ALl or no requirement           PRINT ALL
        if ( (line.size() <= 1) || (cmd == "ALL")) {
            if (!quiet){
                for (auto j=index.begin(); j!=index.end();j++){
                    cout << column_names[*j] << " ";
                }
                cout << "\n";
            }
            
            for (int i=0;i<row_num;i++){
                if (!quiet){
                    for (auto j=index.begin();j != index.end();j++){
                        cout << values[i][*j] << " ";
                    }
                }
                if(!quiet) cout << "\n";
                matching_row ++;
            }
        }
        else {          // PRINT WHERE
            string temp_col_nampe;
            ss >> temp_col_nampe;
            string OP;
            ss >> OP;
            int check_col;
            vector <int> correct_rows;
            string check_val;
            ss >> check_val;
                // find the col
            if (find(column_names.begin(), column_names.end(), temp_col_nampe) == column_names.end()){
                cout << "Error during PRINT: " << temp_col_nampe << " does not name a column in "<< table_name << endl;
                return;
            }
            else {
                check_col = int(find(column_names.begin(), column_names.end(), temp_col_nampe) - column_names.begin());
            }

            // Check BST:
            if ((have_index == 1) && (column_names[the_index] == temp_col_nampe)){
                if (!quiet){
                    for (auto j=index.begin();j!=index.end();j++){
                        cout << column_names[*j] << " ";
                    }
                    cout << "\n"; // print column name
                }
                for (auto itr=bst.begin();itr!=bst.end();itr++){    // iterate through all bst
                    if (Comp(OP, itr->first, check_val, column_type[check_col])){   // If the value match
                        if (!quiet){
                            for (auto tt=itr->second.begin(); tt != itr->second.end(); tt++){ // all the rows match
                                for (auto tt2=index.begin(); tt2!=index.end(); tt2++){
                                    cout << values[*tt][*tt2] << " ";
                                } // cout for
                                cout << "\n";
                                matching_row ++;
                            }   // for all rows
                        }   
                        else matching_row += int(itr->second.size());
                    } // if
                } // for all bst
            } // if bst == name 
            
            else {
                // find the matching rows
                for (int i=0;i<row_num;i++){
                    if(Comp(OP, values[i][check_col], check_val, column_type[check_col])){
                        correct_rows.push_back(i);
                    }
                }
                
                    // print the rows
                if (!quiet){
                    for (auto j=index.begin();j!=index.end();j++){
                        cout << column_names[*j] << " ";
                    }
                    cout << "\n";
                    for (auto i=correct_rows.begin();i != correct_rows.end();i++){
                        for (auto j=index.begin();j != index.end(); j++){
                            cout << values[*i][*j] << " ";
                        }
                    cout << "\n";
                    matching_row ++;
                    }
                }
                else matching_row += int(correct_rows.size());

                 // is quiet
            } // end of non-bst
        } // end of PRINT WHERE
        cout << "Printed " << matching_row << " matching rows from " << table_name << endl;
    }

    void DELETE(){
        string temp_col_name;
        cin >> temp_col_name;
        string OP;
        cin >> OP;
        string val;
        cin >> val;
        int check_col;
        int pre_rows =  row_num;
        // Didn't find
        if (find(column_names.begin(), column_names.end(), temp_col_name) == column_names.end()){
            cout << "Error during DELETE: " << temp_col_name << " does not name a column in "<< table_name << endl;
            return;
        }
        else{
            auto itr_remove = values.end();
            check_col = int(find(column_names.begin(), column_names.end(), temp_col_name) - column_names.begin());
            TableEntry val1(correct_type(val, column_type[check_col]));
            if (OP == "="){
                itr_remove = remove_if(values.begin(), values.end(), Compare_eq(val1, check_col));
            }
            else if (OP == "<"){
                itr_remove = remove_if(values.begin(), values.end(), Compare_lt(val1, check_col));
            }
            else if (OP == ">"){
                itr_remove = remove_if(values.begin(), values.end(), Compare_gt(val1, check_col));
            }
            int nDel = (int)(values.size() - (itr_remove - values.begin()));
            if (nDel > 0){
                for (int i=0; i< nDel;i++){
                    values.pop_back();
                }
            }
        }
        

        row_num = int (values.size());
        cout << "Deleted " << pre_rows - row_num << " rows from " << table_name << endl;
        if (have_index == 1){
            this->GEN_bst(column_names[the_index]);
        }
        else if(have_index == 2) {
            this->GEN_bst(column_names[the_index]);
        }
    }


    void GEN_hash(string colname){
        if (! hash.empty()) hash.clear(); 
        if (! bst.empty()) bst.clear();
        if (find(column_names.begin(), column_names.end(), colname) == column_names.end()){
            cout << "Error during GENERATE: " << colname << " does not name a column in "<< table_name << endl;
            return;
        }
        
        int chosen_col = int(find(column_names.begin(), column_names.end(), colname) - column_names.begin());
        the_index = chosen_col;
        have_index = 2;
        for (int i=0;i < int(values.size()); i++){

            hash[values[i][chosen_col]].reserve(hash[values[i][chosen_col]].size()+1);
            hash[values[i][chosen_col]].emplace_back(i);
        }

    }

    void GEN_bst(string colname){
        if (! hash.empty()) hash.clear(); 
        if (! bst.empty()) bst.clear();
        if (find(column_names.begin(), column_names.end(), colname) == column_names.end()){
            cout << "Error during GENERATE: " << colname << " does not name a column in "<< table_name << endl;
            return;
        }

        int chosen_col = int(find(column_names.begin(), column_names.end(), colname) - column_names.begin());
        the_index = chosen_col;
        have_index = 1;
        for (int i=0;i < int(values.size()); i++){
            bst[values[i][chosen_col]].reserve(bst[values[i][chosen_col]].size()+1);
            bst[values[i][chosen_col]].emplace_back(i);
        }
    }

};