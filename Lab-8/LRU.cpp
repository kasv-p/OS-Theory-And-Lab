#include<iostream>
#include<bits/stdc++.h>

using namespace std;
int NO_PAGES,NO_FRAMES,NO_DISK_BLOCKS;

void remove_From_Swap_Space(vector<int> &swap_space,int page_num)
{
    if(find(swap_space.begin(), swap_space.end(), page_num)!=swap_space.end())
    {
        cout<< "REMOVING PAGE "<<page_num<<"FROM SWAP SPACE"<<endl;
        vector<int>::iterator itr = find(swap_space.begin(), swap_space.end(), page_num);
        int swapped_out=distance(swap_space.begin(), itr);
        swap_space.erase(swap_space.begin()+swapped_out);
    }
}

void lru_replacement(vector<int>& data)
{
    vector<int> swap_space;
    vector<int> memory;
    int page_faults=0;
    int hits=0;
    for (int i=0;i<data.size();i++)
    {
        if (data[i]<1 && data[i]>NO_PAGES)
        {
            cout <<"INVLAID PAGE\n";
            continue;
        }
        vector<int>::iterator itr = find(memory.begin(), memory.end(), data[i]);
        if ( itr!= memory.cend() )
        {
            int pageIndex=distance(memory.begin(), itr);
            cout << "ACCESSED PAGE "<<data[i]<<" PAGE INDEX "<<pageIndex<<endl;
            memory.erase(memory.begin()+pageIndex);
            memory.push_back(data[i]);
            hits+=1;
            continue;
        }
        
        if (memory.size()<NO_FRAMES)
        {
            memory.push_back(data[i]);
            remove_From_Swap_Space(swap_space,data[i]);
            page_faults+=1;
            cout <<"PUSHED PAGE "<<data[i]<<endl;
            continue;
        }
        if (swap_space.size()>=NO_DISK_BLOCKS)
        {
            cout <<"CANNOT SWAP IN OR OUT\n";
            return;
        }
        int victim = memory.front();
        memory.erase(memory.begin());
        remove_From_Swap_Space(swap_space,data[i]);
        swap_space.push_back(victim);
        memory.push_back(data[i]);
        page_faults+=1;
        cout << "VICTIM IS "<<victim<<" PUSHED DATA INTO MEMORY "<<memory[memory.size()-1]<<endl;
    }
    cout << "HITS "<<hits<<" PAGE FAULTS "<<page_faults<<endl;
}

int main(int argc, char* argv[])
{
    NO_PAGES=atoi(argv[1]);
    NO_FRAMES=atoi(argv[2]);
    NO_DISK_BLOCKS=atoi(argv[3]);
    ifstream infile(argv[4]);

    vector<int> data;
    int d;
    while (infile >>  d) {
        data.push_back(d);
    }
    infile.close();
    
    cout<<"-----------LRU------------"<<endl;
    lru_replacement(data);
    return 0;
}
