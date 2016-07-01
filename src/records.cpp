#include "records.h"

records * records::singleton = NULL;

records::records()
{
}

bool records::sort_records(records_entry pre, records_entry next)
{
  return pre.points < next.points;
  //return pre.points > next.points;
}

int records::loadFile(string path)
{
  char * name;
  char * points;
  char str [128];
  int cont = -1;
  records_vector.clear();
 
  if (path.length()==0) path = string(DEFAULT_RECORDS_FILE);
  this->path=path;
  FILE * fd = fopen (path.c_str(),"r");
  if (fd != NULL)
  {
    cont = 0; 
    while (fgets(str,sizeof(str)-1,fd)!=NULL)
    {
      name=strtok(str,SEPARATOR);
      points=strtok(NULL,SEPARATOR);
      add_record(string(name),atoi(points));
      cont ++;
    }
    fclose (fd);
  }
  return cont;
}

int records::saveFile(string * save_path)
{
  unsigned int cont = 0;
  FILE * fd = fopen ((save_path!=NULL)?save_path->c_str():this->path.c_str(),"w");
  size_t tope;
  if (records_vector.size() <= 6) tope = records_vector.size();
  else  tope = 6;
  //for (cont = 0; cont < records_vector.size(); cont ++) 
  for (cont = 0; cont < tope; cont ++) 
    fprintf(fd,"%s|%d\n",records_vector[cont].name.c_str() ,records_vector[cont].points);
  
  fclose (fd);
 
  return 0;
}

const vector <records_entry> records::getList (int amount )
{
  return records_vector;
}

int records::add_record (string name, int points)
{
  char tmp [64];
  sprintf(tmp,"%d:%d",points/60,points%60);
  if (!name.size()) name = "AAAAA";
  records_entry entry (name,points,string(tmp));
  records_vector.push_back (entry);
  std::sort(records_vector.begin(),records_vector.end(),records::sort_records);
  return 0;
}

void records::print()
{
  unsigned int cont = 0;
  cout << "ELEMENTS " << records_vector.size()<<endl;
  for (cont = 0; cont < records_vector.size(); cont ++) 
    cout << "Element " << cont << " Name: " << records_vector[cont].name << ", Points: " << records_vector[cont].points << endl;
  cout << "END LIST" << endl;
}

int records::getNext (string &name, int &points, string &timestamp, bool begin)
{
  static unsigned int cont = 0;
  if (begin) cont=0;
  if (cont >= records_vector.size()) return -1;
  else 
  {
    name=this->records_vector[cont].name;
    points=this->records_vector[cont].points;
    timestamp=this->records_vector[cont].timestamp;
    cont++;
    return 0;
  } 
}

records * records::getInstance()
{
  if (singleton == NULL)
  {
    singleton = new records();
  }
  return singleton;
}

void records::getBest (string & name,int & points)
{
  if (records_vector.size()>0)
  {
    name=this->records_vector[0].name;
    points=this->records_vector[0].points;
  }
  else
  {
    name ="";
    points =0;
  }
}

