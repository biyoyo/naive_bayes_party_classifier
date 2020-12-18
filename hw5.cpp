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
        : attributes_count(16)
    {
        read_file();
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
        file.close();
    }

    struct VoteStatistic
    {
        float ry;
        float rn;
        float dy;
        float dn;
    };

    void calculate_likelihoods(int subset_to_ignore)
    {
        for (int col = 0; col < attributes_count; col++)
        {
            pair<int, int> p0 = count_votes_in_party(col, 0, subset_to_ignore);
            pair<int, int> p1 = count_votes_in_party(col, 1, subset_to_ignore);

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

        for (int i = 0; i < attributes_count; i++)
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

        for (int i = 0; i < attributes_count; i++)
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
        subsets.resize(10);

        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> distro(0, data.size() - 1);
        uniform_int_distribution<> set_size(35, 45);
        vector<bool> visited(data.size(), false);

        for (int i = 0; i < 9; i++)
        {
            //generate one random set
            int subset_size = set_size(gen);
            for (int j = 0; j < subset_size; j++)
            {
                int index = distro(gen);
                while (visited[index] != false)
                {
                    index = distro(gen);
                }
                subsets[i].push_back(index);
                visited[index] = true;
            }
        }

        for (int i = 0; i < data.size(); i++)
        {
            if (visited[i] == false)
            {
                subsets[9].push_back(i);
            }
        }
    }

    void train_and_test()
    {
        generate_random_subsets();

        ofstream test_data("test_data.txt", ios::trunc);
        double average = 0;

        //one train cycle out of 10
        for (int subset = 0; subset < subsets.size(); subset++)
        {
            pair<int, int> parties_count = count_class(subset);
            rep_count = parties_count.first;
            dem_count = parties_count.second;

            calculate_likelihoods(subset);

            double success_rate = test_with_one_subset(subset);

            average += success_rate;

            if(!test_data)
            {
                cout << "Error opening output file" << endl;
            }

            test_data << success_rate << ", ";

            likelihoods.clear();
        }
        test_data << average / subsets.size();
        test_data.close();
    }

    double test_with_one_subset(int subset)
    {
        int test_subjects = subsets[subset].size();
        int successes = 0;
        for(int i = 0; i < test_subjects; i++)
        {
            int result = classify(subsets[subset][i]);
            successes += result == data[i].party ? 1 : 0;
        }

        return double(successes) / test_subjects;
    }

private:
    vector<Record> data;
    vector<VoteStatistic> likelihoods;
    int rep_count;
    int dem_count;
    int attributes_count;

    vector<vector<int>> subsets;

    pair<int, int> count_class(int subset_to_ignore)
    {
        int reps = 0, dems = 0;
        for (int i = 0; i < subsets.size(); i++)
        {
            if (i != subset_to_ignore)
            {
                for (int j : subsets[i])
                {
                    reps += data[j].party == Republican ? 1 : 0;
                    dems += data[j].party == Democrat ? 1 : 0;
                }
            }
        }
        return make_pair(reps, dems);
    }

    pair<int, int> count_votes_in_party(int col, int party, int subset_to_ignore)
    {
        int yeas = 0, nays = 0;

        for (int i = 0; i < subsets.size(); i++)
        {
            if (i != subset_to_ignore)
            {
                for (int j : subsets[i])
                {
                    if (data[j].party == party)
                    {
                        yeas += data[j].data[col] == Yea ? 1 : 0;
                        nays += data[j].data[col] == Nay ? 1 : 0;
                    }
                }
            }
        }
        return make_pair(yeas, nays);
    }
};

int main()
{
    Classifier cl;
    cl.train_and_test();
    return 0;
}