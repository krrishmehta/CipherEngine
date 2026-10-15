#include <iostream>
#include <string>
#include <fstream>
#include <cctype>
#include <tuple>

#define KEY_LENGTH 12

using namespace std;

class CipherEngine{
    private:
        string key;
        
        string transform(string input, size_t offset, unsigned char prevChar, bool decrypting){
            if(key.empty()){
                cout<<"KEY IS EMPTY"<<endl;
                return "KEY IS EMPTY";
            }

            string output = input;
            unsigned char feedback = prevChar;
            
            for(int i=0; i<input.size(); i++){
                unsigned char inputByte = static_cast<unsigned char>(input[i]);
                unsigned char keyByte = static_cast<unsigned char>(key[(offset + i)%key.size()]);
                unsigned char cipherByte = inputByte ^ keyByte ^ feedback;
                output[i] = static_cast<char>(cipherByte);
                feedback = decrypting ? cipherByte : inputByte;
            }
            
            return output;
        }

    public:
        CipherEngine(string key){
            this->key = key;
        }

        //Encrypt file data;
        string operator <<(const tuple<string, size_t, unsigned char> &data){
            return transform(get<0>(data), get<1>(data), get<2>(data), false);
        }
        
        //Decrypt file data;
        string operator >>(const tuple<string, size_t, unsigned char> &data){  
            return transform(get<0>(data), get<1>(data), get<2>(data), true);
        }
};


bool keyChecker(string key){
    bool isDigit = false;
    bool isChar = false;
    bool isSymbol = false;

    if(key.length() < KEY_LENGTH){
        cout<<"KEY: "<<key<<" length is less than "<<KEY_LENGTH<<endl;
        return false;
    }

    for(int i=0; i<key.length(); i++){
        if(isdigit(key.at(i))){
            isDigit = true;
        } else if(isalpha(key.at(i))){
            isChar = true;
        } else if (ispunct(key.at(i))){
            isSymbol = true;
        }
    }

    if(!isDigit){
        cout<<"KEY "<<"[ "<<key<<" ]"<<" : "<<" numbers are not available"<<endl;
        return false;
    }

    if(!isChar){
        cout<<"KEY "<<"[ "<<key<<" ]"<<" : "<<" chars are not available"<<endl;
        return false;
    }

    if(!isSymbol){
        cout<<"KEY "<<"[ "<<key<<" ]"<<" : "<<" symbols are not available"<<endl;
        return false;
    }

    return true;
}

void encrypt(string filename, string key, string outputfilename){
    ifstream inFile(filename, ios::binary);
    ofstream outFile(outputfilename, ios::binary | ios::trunc);
    CipherEngine CipherEngine(key);

    if(!inFile){
        cout<<"Error opening file for reading!"<<endl;
        return;
    }

    if(!outFile){
        cout<<"Error opening file for writing!"<<endl;
        return;
    }

    char buffer[32];
    unsigned char prevCipher = 0;
    size_t offset = 0;

    while(inFile.read(buffer, sizeof(buffer)) || inFile.gcount() > 0){
        size_t bytesRead = inFile.gcount();
        string input(buffer, bytesRead);

        string output = CipherEngine << make_tuple(input, offset, prevCipher);
        outFile.write(output.data(), static_cast<streamsize>(output.size()));

        offset = (offset + bytesRead) % key.size();

        if(!output.empty()){
            prevCipher = static_cast<unsigned char>(output.back());
        }
    }

    inFile.clear();
    inFile.close();
    outFile.close();
}

void decrypt(string filename, string key, string outputfilename){
    ifstream inFile(filename, ios::binary);
    ofstream outFile(outputfilename, ios::binary | ios::trunc);
    CipherEngine CipherEngine(key);

    if(!inFile){
        cout<<"Error opening file for reading!"<<endl;
        return;
    }

    if(!outFile){
        cout<<"Error opening file for writing!"<<endl;
        return;
    }

    char buffer[32];
    unsigned char prevPlain = 0;
    size_t offset = 0;

    while(inFile.read(buffer, sizeof(buffer)) || inFile.gcount() > 0){
        size_t bytesRead = inFile.gcount();
        string input(buffer, bytesRead);

        string output = CipherEngine >> make_tuple(input, offset, prevPlain);
        outFile.write(output.data(), static_cast<streamsize>(output.size()));

        offset = (offset + bytesRead) % key.size();

        if(!input.empty()){
            prevPlain = static_cast<unsigned char>(input.back());
        }
    }

    inFile.close();
    outFile.close();
}

int main(){
    int choice;
    string key;
    string filename;
    string output_file_name;

    cout<<"[1]. Encrypt\n[2]. Decrypt\nEnter your choice [1,2]: ";
    cin>>choice;

    cout<<"Enter a filename: ";
    cin>>filename;

    cout<<"Enter a KEY: ";
    cin>>key;

    cout<<"Enter a output filename: ";
    cin>>output_file_name;

    if(!keyChecker(key)){
        return 0;
    }

    switch (choice)
    {
    case 1:
        encrypt(filename, key, output_file_name);
        cout<<"Data Encrypted Successfully!"<<endl;
        break;

    case 2:
        decrypt(filename, key, output_file_name);
        cout<<"Data Decrypted Successfully!"<<endl;
        break;
    
    default:
    cout<<"Invalid Input"<<endl;
        break;
    }
    
    return 0;
}