/****************************************************************************
**
** Copyright (C) 2017
** Indian Institute of Technology, Roorkee (www.iitr.ac.in)
** The University of Sheffield (www.sheffield.ac.uk)
**
** This file is part of Entropy based Termination Criterion for 
** Multi-objective Evolutionary Algorithms
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General
** Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/
#include <vector>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

using namespace std;

/* population global variables */
int nobj;
int popsize;

/* population functions */
int get_popsize(string filename);
vector <double> get_population(string filename);

/* entropy global variables */
int nbins;
vector< vector< double > > bins_realvar;

/* entropy functions */
vector <double> evaluate_rel_ent(vector <vector<double> > pop_child, vector <vector<double> > pop_par);
void which_cluster(int obj, vector <vector<double> > pop, vector<double> obj_max, vector<double> obj_min, vector<int> &cluster_i);
bool is_belong_vec(vector< vector<int> > clusters, vector<int> cluster_i, int &pos);

int main(int argc, char* argv[]) {

    int k;
    vector <double> matrix;

    /*cout << "argc = " << argc << endl;*/

    if(argc != 5) {
    	cerr << "Number of arguments incorrect!" << endl;
    	cerr << "Usage: " << argv[0] << " [number_objectives] [number_bins] [parent_file] [child_file]" << endl;
    	cerr << "Example: " << argv[0] << " 2 100 file1.out file2.out" << endl;
    	return -1;
    }

    nobj=atoi(argv[1]);
    nbins=atoi(argv[2]);
    string file1 = argv[3];
    string file2 = argv[4];

    cout << "Number of objectives: " << nobj << endl;
    cout << "Number of bins: " << nbins << endl;


    /************************/
    /* 1) built bins matrix */
    /************************/
    for(int i=0;i<nobj;i++) {
        vector<double> q;
        for(int j=0;j<=nbins;j++) {
            q.push_back((double)j/(double)nbins);
        }
        bins_realvar.push_back(q);
    }


    /**************************/
    /* 2) get population size */
    /**************************/
    popsize=get_popsize(file1);
    cout << "Population size: " << popsize << endl;


    /****************************/
    /* 3) get parent population */
    /****************************/
    vector <vector<double> > parent;
    matrix=get_population(file1);
    k=0;
    for(int i=0; i<popsize; i++) {
        vector <double> row(nobj,0.0);
        for(int j=0; j<nobj; j++) {
            row[j]=matrix[k];
            k++;
        }
        parent.push_back(row);
    }


    /****************************/
    /* 4) get child population */
    /****************************/
    vector <vector<double> > child;
    matrix=get_population(file2);
    k=0;
    for(int i=0; i<popsize; i++) {
        vector <double> row(nobj,0.0);
        for(int j=0; j<nobj; j++) {
            row[j]=matrix[k];
            k++;
        }
        child.push_back(row);
    }


    /***************************************************/
    /* 5) Calculate the entropy based distance measure */
    /***************************************************/
    vector <double> v;
    v = evaluate_rel_ent(child, parent);
    cout << scientific << "I: " << v[0] << endl; 		/* Intersection */
    cout << scientific << "O: " << v[1] << endl; 		/* Non-Intersection */
    cout << scientific << "D: " << v[0]+v[1] << endl;	/* Distance Measure */


    return 0;
}

vector <double> get_population(string filename) {

    vector<double> data;
    double temp = 0.0;

    ifstream my_file;
    my_file.open(filename.c_str(),ios::in);
    if(!my_file) {
        cout << "Unable to open file!"<< endl;
        exit(-1);
    }

    while(my_file >> temp) {
        data.push_back(temp);
    }
    my_file.close();

    return data;
}

int get_popsize(string filename) {

    fstream my_file;
    my_file.open(filename.c_str(),ios::in);
    if (!my_file) {
        cout << "Unable to open file!"<< endl;
        exit(-1);
    }

    string my_line;
    int rows;

    rows=0;
    while (getline(my_file,my_line)) {
        stringstream my_s(my_line);
        string my_token;
        while (getline(my_s,my_token,'\n'))
            ;
        rows++;
    }
    my_file.close();

    return rows;
}

vector <double> evaluate_rel_ent(vector <vector<double> > pop_child, vector <vector<double> > pop_par) {

    int i,j;

    vector<double> v(2,0.0);
    vector<double> obj_max(nobj), obj_min(nobj);

    for(j=0;j<nobj;j++) {
        obj_max[j]=pop_par[0][j];
        obj_min[j]=pop_par[0][j];
    }

    for(i=1;i<popsize;i++) {
        for(j=0;j<nobj;j++) {

            if(pop_par[i][j]>obj_max[j])
                obj_max[j] = pop_par[i][j];

            if(pop_par[i][j]<obj_min[j])
                obj_min[j] = pop_par[i][j];

            if(pop_child[i][j]>obj_max[j])
                obj_max[j] = pop_child[i][j];

            if(pop_child[i][j]<obj_min[j])
                obj_min[j] = pop_child[i][j];
        }
    }

    double p_par, p_child;
    int pos, total_par = 0, total_child = 0;
    vector< vector<int> > clusters;
    vector< vector<int> > clusters_only_child;
    vector<int> par_num;
    vector<int> child_num;
    vector<int> only_child_num;
    int num_par = popsize, num_child = popsize;
    vector<int> belong_to(nobj,0);

    for(i=0;i<popsize;i++) {
        pos = 0;
        for(j=0;j<nobj;j++)
            belong_to[j]=0;
        which_cluster(i, pop_par, obj_max, obj_min, belong_to);
        if(is_belong_vec(clusters, belong_to, pos))
            par_num[pos] += 1;
        else {
            clusters.push_back(belong_to);
            par_num.push_back(1);
            child_num.push_back(0);
        }
    }
    for(i=0;i<popsize;i++) {
        pos = 0;
        for(j=0;j<nobj;j++)
            belong_to[j]=0;
        which_cluster(i, pop_child, obj_max, obj_min, belong_to);
        if(is_belong_vec(clusters, belong_to, pos))
            child_num[pos] += 1;
        else {
            if(is_belong_vec(clusters_only_child, belong_to, pos))
                only_child_num[pos] += 1;
            else {
                clusters_only_child.push_back(belong_to);
                only_child_num.push_back(1);
            }
        }
    }
    total_par = num_par;
    total_child = num_child;
    for(i=0;i<(int)clusters.size();i++) {
        p_par = (double)par_num[i]/(double)total_par;
        p_child = (double)child_num[i]/(double)total_child;
        if(p_child>0)
            v[0] += (-p_par*log(p_child/p_par) - p_child*log(p_par/p_child))/2.0;
        if(p_child==0)
            v[1] += -p_par*log(p_par);
    }
    for(i=0;i<(int)clusters_only_child.size();i++) {
        p_child = (double)only_child_num[i]/(double)total_child;
        v[1] += -p_child*log(p_child);
    }
    clusters.clear();
    clusters_only_child.clear();
    par_num.clear();
    child_num.clear();
    only_child_num.clear();

    return v;
}

void which_cluster(int obj, vector <vector<double> > pop, vector<double> obj_max, vector<double> obj_min, vector<int> &cluster_i) {

    int i,k;
    double obj_val= 0.0;
    for(k=0;k<nobj;k++) {
        obj_val = (pop[obj][k] - obj_min[k])/(obj_max[k] - obj_min[k]);
        for(i=0;i<nbins;i++) {
            if( (obj_val > bins_realvar[k][i]) && (obj_val <= bins_realvar[k][i+1]) )
                cluster_i[k] = i;
        }
    }
    return;
}

bool is_belong_vec(vector< vector<int> > clusters, vector<int> cluster_i, int &pos) {

    int i,j;
    bool b = false;
    pos = -1;
    for(i=0;i<(int)clusters.size();i++) {
        b = true;
        for(j=0;j<nobj;j++)
            if(clusters[i][j]!=cluster_i[j])
                b = false;
        if(b) {
            pos = i;
            break;
        }
    }
    return b;
}
