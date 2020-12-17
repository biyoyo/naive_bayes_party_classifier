#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <random>

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
    Classifier()
    {
        read_file();
        pair<int, int> count = count_class();
        rep_count = count.first;
        dem_count = count.second;
        calculate_likelihoods();
    }

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

            if (file.eof())
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

            if (word == "republican")
            {
                rec.party = Republican;
            }
            else if (word == "democrat")
            {
                rec.party = Democrat;
            }

            while (getline(line, word, ','))
            {
                Vote v;
                if (word == "y")
                {
                    v = Yea;
                }
                else if (word == "n")
                {
                    v = Nay;
                }
                else if (word == "?")
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

    struct VoteStatistic
    {
        float ry;
        float rn;
        float dy;
        float dn;
    };

    void calculate_likelihoods()
    {
        pair<int, int> cl = count_class();
        int reps = cl.first;
        int dems = cl.second;

        for (int col = 0; col < 16; col++) //todo
        {
            pair<int, int> p0 = count_votes_in_party(col, 0);
            pair<int, int> p1 = count_votes_in_party(col, 1);

            VoteStatistic vs;
            vs.ry = double(p0.first) / rep_count;
            vs.rn = double(p0.second) / rep_count;
            vs.dy = double(p1.first) / dem_count;
            vs.dn = double(p1.second) / dem_count;

            likelihoods.push_back(vs);
        }
    }

    int classify(int record_index)
    {
        double rep_prob = 0;

        for (int i = 0; i < 16; i++)
        {
            double likelihood;
            if (data[record_index].data[i] == 0)
            {
                likelihood = likelihoods[i].ry;
            }
            else if (data[record_index].data[i] == 1)
            {
                likelihood = likelihoods[i].rn;
            }
            rep_prob += log(likelihood);
        }

        rep_prob += log(double(rep_count) / (rep_count + dem_count));

        double dem_prob = 0;

        for (int i = 0; i < 16; i++)
        {
            double likelihood;
            if (data[record_index].data[i] == 0)
            {
                likelihood = likelihoods[i].dy;
            }
            else if (data[record_index].data[i] == 1)
            {
                likelihood = likelihoods[i].dn;
            }
            dem_prob += log(likelihood);
        }

        dem_prob += log(double(dem_count) / (rep_count + dem_count));

        return rep_prob > dem_prob ? 0 : 1;
    }

    void generate_random_subsets()
    {
        vector<vector<int>> subsets(10);

        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> distro(0, data.size() - 1);
        uniform_int_distribution<> set_size(35, 45);
        vector<bool> visited(data.size(), false);

        for(int i = 0; i < 9; i++)
        {
            //generate one random set
            int subset_size = set_size(gen);
            for(int j = 0; j < subset_size; j++)
            {
                int index = distro(gen);
                while(visited[index] != false)
                {
                    index = distro(gen);
                }
                subsets[i].push_back(index);
                visited[index] = true;
            }
        }

        for(int i = 0; i < data.size(); i++)
        {
            if(visited[i] == false)
            {
                subsets[9].push_back(i);
            }
        }
    }

private:
    vector<Record> data;
    vector<VoteStatistic> likelihoods;
    int dem_count;
    int rep_count;

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
    int index;
    //cin >> index;
    //cout << cl.classify(index) << endl;
    cl.generate_random_subsets();
    return 0;
}