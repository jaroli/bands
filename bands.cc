/*
 * karol jarolimek
  program bands
  -plot band structure
  -input: OUTCAR, KPOINTS, EIGENVAL

  -use scale=1.0 in your POSCAR to avoid confusion
 i changed this
*/


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <iomanip>

using namespace std;

int nband;		//number of bands
int nkpoint;		//number of all kpoints
int nkline;		//number of k-points on a line (e.g. G-X)
int nseg;		//number of segments
double b00, b01, b02;	//reciprocal basis vectors
double b10, b11, b12;
double b20, b21, b22;
double efermi;		//Fermi energy
double** E;		//band structure E[k-point][band]
double* klox;		//first kpoint of a segment
double* kloy;		
double* kloz;		
double* khix;		//last kpoint of a segment
double* khiy;		
double* khiz;		
double* len;		//segment length in rec angstrom
double* lencum;		//cumulative segment length in rec angstrom
string* slo;		//first high symmetry point of a segment	
string* shi;		//last high symmetry point of a segment


void fmt(fstream& file)
{
  file.setf(ios::fixed, ios::floatfield);
  file.setf(ios::right);
  file.precision(6);
  file.width(14);
}

void parse_outcar()
{
  fstream infile;
  string word;
  string line;
  bool found;
  int i, j;
  int dummy;

  infile.open("OUTCAR", ios::in);

  //number of k-points
  found = false;
  while ( infile.good() && (! found) )
  {
    infile >> word;
    if (word == "NKPTS") found = true;
  }
  infile >> word;
  infile >> nkpoint;

  //number of bands
  found = false;
  while ( infile.good() && (! found) )
  {
    infile >> word;
    if (word == "NBANDS=") found = true;
  }
  infile >> nband;

  //reciprocal lattice vectors
  found = false;
  while ( infile.good() && (! found) )
  {
    infile >> word;
    if (word == "energy-cutoff") found = true;
  }
  getline(infile, line);
  getline(infile, line);
  getline(infile, line);

  infile >> word; infile >> word; infile >> word;
  infile >> b00; infile >> b01; infile >> b02;

  infile >> word; infile >> word; infile >> word;
  infile >> b10; infile >> b11; infile >> b12;

  infile >> word; infile >> word; infile >> word;
  infile >> b20; infile >> b21; infile >> b22;

  //Fermi energy
  found = false;
  while ( infile.good() && (! found) )
  {
    infile >> word;
    if (word == "E-fermi") found = true;
  }
  infile >> word; 
  infile >> efermi;

  infile.close();

  cout << "number of all kpoints\t" << nkpoint << endl;
  cout << "number of bands\t\t" << nband << endl;
  cout << "Fermi energy\t\t" << efermi << endl;
}

void parse_kpoints()
{
  int i;
  fstream file;
  string line, word;


  file.open("KPOINTS", ios::in);

  //read header
  getline(file, line); //comment line
  file >> nkline; getline(file, line); //number of kpoints along each line
  cout << "kpoints per line\t" << nkline << endl;
  getline(file, line); //line mode
  getline(file, line); //reciprocal mode

  //calculate number of segments
  nseg = nkpoint / nkline;
  cout << "number of segments\t" << nseg << endl;

  //allocate arrays
  klox = new double [nseg];
  kloy = new double [nseg];
  kloz = new double [nseg];
  khix = new double [nseg];
  khiy = new double [nseg];
  khiz = new double [nseg];
  slo = new string [nseg];
  shi = new string [nseg];

  for (i = 0; i < nseg; i++)
  {
    file >> klox[i];   
    file >> kloy[i];   
    file >> kloz[i];   
    file >> word; //skip exclamation mark
    file >> slo[i]; 
    getline(file, line);

    file >> khix[i];   
    file >> khiy[i];   
    file >> khiz[i];   
    file >> word; //skip exclamation mark
    file >> shi[i]; 
    getline(file, line);

    getline(file, line);
  }

  file.close();
}




void parse_eigenval()
{
  fstream file;
  string line;
  int i, j;
  int dummy;

  file.open("EIGENVAL", ios::in);

  //skip header
  for (i = 0; i < 6; i++)
    getline(file, line);

  //allocate bandstructure array
  E = new double* [nkpoint];
  for (i = 0; i < nkpoint; i++)
    E[i] = new double [nband];

  for (i = 0; i < nkpoint; i++)
  {
    getline(file, line); //skip blank line
    getline(file, line); //skip kpoint coordinates
    for (j = 0; j < nband; j++)
    {
      file >> dummy; //skip band number
      file >> E[i][j];
      getline(file, line);
    }
  }
  file.close();
}


void calculate_path()
{
  int i;
  double x, y, z;
  double dx, dy, dz;
  double mem;

  //convert to cartesian coordinates
  for (i = 0; i < nseg; i++)
  {
    x = b00*klox[i] + b10*kloy[i] + b20*kloz[i];
    y = b01*klox[i] + b11*kloy[i] + b21*kloz[i];
    z = b02*klox[i] + b12*kloy[i] + b22*kloz[i];

    klox[i] = x;
    kloy[i] = y;
    kloz[i] = z;

    x = b00*khix[i] + b10*khiy[i] + b20*khiz[i];
    y = b01*khix[i] + b11*khiy[i] + b21*khiz[i];
    z = b02*khix[i] + b12*khiy[i] + b22*khiz[i];

    khix[i] = x;
    khiy[i] = y;
    khiz[i] = z;
  }

  //calculate the length of each segment
  len = new double [nseg];
  for (i = 0; i < nseg; i++)
    len[i] = sqrt(pow(khix[i]-klox[i],2) + pow(khiy[i]-kloy[i],2) + pow(khiz[i]-kloz[i],2));
  
 
  //cumulative length of segments
  //this is the same as the end point of a segment on the kpath
  lencum = new double [nseg];
  lencum[0] = len[0];
  for (i = 1; i < nseg; i++)
    lencum[i] = lencum[i-1] + len[i]; 


}


void print_segments()
{
  int i, j, s;
  double dk;
  stringstream ss;
  string fname;
  fstream file;

  for (s = 0; s < nseg; s++)
  {
    dk = len[s] / (nkline - 1);
    
    ss.str(""); //clear the stream
    ss << s + 1;
    fname = "segment" + ss.str();

    file.open(fname.c_str(), ios::out);
    for (i = 0; i < nband; i++)
    {
      file << "#band\t" << i + 1 << endl;
      for (j = 0; j < nkline; j++)
      {
        fmt(file); file << lencum[s] - len[s] + j*dk;
        fmt(file); file << E[s*nkline+j][i];
        file << endl;
      }
      file << endl;
      file << endl;
    }
    file.close();
  }
}

void print_gnuplot_file()
{
  int i;
  fstream file;


  file.open("bands.plt", ios::out);

  file << "set term postscript enhanced color eps" << endl;
  file << "set output \"bands.eps\"" << endl;
  file << "set ylabel \"Energy (eV)\"" << endl;
  file << endl;

  file << "Elo = -4" << endl;
  file << "Ehi = 4" << endl;
  file << "Efermi = " << efermi << endl;
  file << "set xrange [0.0:" << lencum[nseg-1] << "]" << endl;
  file << "set yrange [Elo:Ehi]" << endl;
  file << endl;

  //print x-axis labels
  for (i = 0; i < nseg; i++)
  {
    if (slo[i] == "\\Gamma") slo[i] = "{/Symbol G}";
    if (shi[i] == "\\Gamma") shi[i] = "{/Symbol G}";
  }
  file << "set xtics (\"" << slo[0] << "\" 0.0,\\" << endl; //first label
  for (i = 0; i < nseg - 1; i++)  //for each segment print the last label 
    if (shi[i] == slo[i+1])
    {
      file << "\"" << shi[i] << "\" " << lencum[i] << ",\\" << endl;
    }
    else
    {
      file << "\"" << shi[i] << "|" << slo[i+1] << "\" " << lencum[i] << ",\\" << endl;
    }
  file << "\"" << shi[nseg-1] << "\" " << lencum[nseg-1] << ")" << endl;  //last label
  file << endl;


  //print vertical lines
  //leave our the first and last symmetry point
  for (i = 0; i < nseg - 1; i++)
  {
    file << "set arrow from";
    fmt(file); file << lencum[i];
    file << ", Elo to";
    fmt(file); file << lencum[i];
    file << ", Ehi nohead" << endl;
  }
  file << endl;

  file << "set style line 1 lt 1 lw 1 pt 7 ps 0.3 lc rgb \"black\"" << endl;
  file << "unset key" << endl;
  file << endl;

  file << "plot ";
  for (i = 0; i < nseg - 1; i++)
  {
    file << "for[i=0:"; file << nband - 1; file << "] ";
    file << "\"segment" << i + 1 << "\" index i ";
    file << "u 1:($2-Efermi) w lines ls 1,\\" << endl;
  }
  file << "for[i=0:"; file << nband - 1; file << "] ";
  file << "\"segment" << nseg << "\" index i ";
  file << "u 1:($2-Efermi) w lines ls 1" << endl;


  file.close();
}


int main()
{
  int i, j;
  string word, line;
  fstream file;

  parse_outcar(); //get nkpoint, nbands
  parse_kpoints(); //get nkline, calculate nseg
  parse_eigenval(); //get E(k,n)

  calculate_path();

  print_segments();
  print_gnuplot_file();

  return 0;
}


