#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#define loop for (;;)

using namespace std;

enum Vote
{
    Yea,
    Nay,
    Unknown
};
enum Party
{
    Republican,
    Democrat
};

struct Record
{
    Party party;
    vector<Vote> data;
};

class Classifier
{
public:
    void read_file()
    {
        ifstream file("house-votes-84.data");

        if (!file)
        {
            cout << "Error opening file" << endl;
        }

        loop
        {
            string input;
            file >> input;

            if(file.eof())
            {
                break;
            }

            if (!file)
            {
                cout << "Error reading file" << endl;
            }

            Record rec;
            stringstream line(input);

            string word;

            getline(line, word, ',');

            if(word == "republican")
            {
                rec.party = Republican;
            }
            else if (word == "democrat")
            {
                rec.party = Democrat;
            }
            
            while(getline(line, word, ','))
            {
                Vote v;
                if(word == "y")
                {
                    v = Yea;
                }
                else if(word == "n")
                {
                    v = Nay;
                }
                else if(word == "?")
                {
                    v = Unknown;
                }
                else
                {
                    cout << "Error reading file" << endl;
                }
                
                rec.data.push_back(v);
            }
            data.push_back(rec);
        }
    }

private:
    vector<Record> data;
    vector<VoteStatistic> likelihoods;
    int dem_count;
    int rep_count;

    pair<int, int> count(int col)
    {
        int a = 0, b = 0;
        for (auto rec : data)
        {
            if (rec.data[col] == 0)
            {
                a++;
            }
            else if (rec.data[col] == 1)
            {
                b++;
            }
        }
        return make_pair(a, b);
    }

    pair<int, int> count_class()
    {
        int a = 0, b = 0;
        for (auto rec : data)
        {
            if (rec.party == 0)
            {
                a++;
            }
            else if (rec.party == 1)
            {
                b++;
            }
        }
        return make_pair(a, b);
    }

    pair<int, int> count_votes_in_party(int col, int party)
    {
        int a = 0, b = 0;

        for (auto rec : data)
        {
            if (rec.party == party)
            {
                if (rec.data[col] == 0)
                {
                    a++;
                }
                else if (rec.data[col] == 1)
                {
                    b++;
                }
            }
        }
        return make_pair(a, b);
    }
};

int main()
{
    Classifier cl;
    cl.read_file();
    return 0;
}